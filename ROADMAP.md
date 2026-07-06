# Webserv — Roadmap de fin de projet

Projet à 3 personnes, presque terminé. État des lieux confronté au sujet PDF.
Niveau visé : **mandatory qui passe + robustesse** (keep-alive, EAGAIN, timeouts, cleanup).

**Légende** : ✅ fait · 🔴 bloquant · 🟠 mandatory manquant · 🟡 robustesse · ⚪ cleanup

Comment lire une tâche : **Quoi / Pourquoi** (l'objectif), **Où** (fichier:ligne),
**Comment** (les étapes concrètes), **Test** (comment vérifier que c'est bon).

---

## TIER 0 — Bugs bloquants ✅ TOUS CORRIGÉS

Vérifiés dans le code + `make re` compile sans warning.

- [x] **CGI POST body jamais envoyé** — `cgi_in_handler.cpp:26-27`
  Le `return kClose` est maintenant **dans** le `if (EPOLLERR|EPOLLHUP)`. Le body
  est bien écrit en EPOLLOUT. → Les CGI avec body (POST) ne timeout plus.
- [x] **`get_header()` renvoyait toujours vide** — `http_request.cpp:78`
  `value = header_.at(key);` sans redéclaration. → La lecture des headers
  (Content-Type, boundary multipart, Content-Length) fonctionne.
- [x] **Crash sur signal (EINTR)** — `reactor.cpp:47-49`
  Sur EINTR `return;`, throw seulement pour les autres erreurs. → Un SIGCHLD
  (fin d'enfant CGI) ne crashe plus la boucle.

> ⚠️ Reste à re-tester **end-to-end** que le fix #1 débloque bien un vrai POST vers
> un CGI Python (voir section Vérification, point 3) — la correction est en place
> mais le comportement complet n'a pas encore été observé.

---

## TIER 1 — Features mandatory manquantes 🟠

Le sujet (IV.3) impose des directives de config qui ne sont **ni parsées ni
appliquées**. Chaque feature suit le même chemin en 3 étapes :

1. **Parser** la directive → `src/config/config_parser.cpp` (`ParseLocation` / `ParseServer`)
2. **Stocker** dans la struct → `src/config/config.hpp` (`LocationConfig` l.18-22, `Config` l.28-59)
3. **Consommer** → `src/http_protocol/route_resolve.cpp`, `router.cpp`, `static_handler.cpp`

### 1.1 🟠 Méthodes autorisées par route (→ 405)
- **Quoi / Pourquoi** : chaque `location` doit pouvoir restreindre les méthodes
  acceptées. Aujourd'hui GET/POST/DELETE sont acceptées partout, sans contrôle.
- **Où** : ajouter `std::vector<std::string> methods;` à `LocationConfig`
  (`config.hpp`). Parsing dans `ParseLocation` (`config_parser.cpp`). Contrôle
  dans le routeur (`router.cpp`, avant de résoudre le fichier).
- **Comment** : directive config ex. `methods GET POST;`. Si la méthode de la
  requête n'est pas dans la liste → renvoyer **405 Method Not Allowed** (avec
  header `Allow:` listant les méthodes permises).
- **Test** : `location` avec `methods GET;` puis `curl -X DELETE` → doit renvoyer 405.

### 1.2 🟠 Redirection HTTP (301/302)
- **Quoi / Pourquoi** : une route doit pouvoir rediriger vers une autre URL.
- **Où** : champ `std::string redirect; int redirect_code;` dans `LocationConfig`.
  Parsing `ParseLocation`. Consommation **tôt** dans `router.cpp` (avant résolution
  fichier).
- **Comment** : directive `return 301 /nouvelle-url;`. Si présente → réponse avec
  le code (301/302) + header `Location: <url>`, corps vide ou minimal.
- **Test** : `curl -v` sur la route → statut 301 + `Location:` correct ; le
  navigateur suit la redirection.

### 1.3 🟠 Directory listing / autoindex
- **Quoi / Pourquoi** : si une requête pointe sur un dossier sans fichier index,
  soit on liste le contenu (autoindex on), soit on renvoie 403 (off). Aujourd'hui
  on append aveuglément `index.html` → 404 si absent.
- **Où** : champ `bool autoindex;` dans `LocationConfig`. Logique dans
  `static_handler.cpp` là où `is_directory` est détecté (~l.81).
- **Comment** : directive `autoindex on;`. Si dossier + pas d'index + autoindex on
  → générer une page HTML listant les entrées (utiliser `opendir`/`readdir`, tous
  deux autorisés par le sujet). Si off → **403 Forbidden**.
- **Test** : dossier sans `index.html` → `autoindex on` liste les fichiers,
  `autoindex off` renvoie 403.

### 1.4 🟠 Fichier index configurable
- **Quoi / Pourquoi** : le fichier servi par défaut sur un dossier est **hardcodé**
  `index.html`. Le sujet veut qu'il soit configurable.
- **Où** : champ `std::string index;` dans `LocationConfig`. Remplacer le
  `append("index.html")` de `static_handler.cpp:81`.
- **Comment** : directive `index index.html;`. Fallback sur `index.html` si non
  précisé.
- **Test** : `index accueil.html;` → une requête sur le dossier sert `accueil.html`.

### 1.5 🟠 Pages d'erreur personnalisées
- **Quoi / Pourquoi** : le sujet demande de pouvoir configurer des pages d'erreur.
  Actuellement `Router::ErrorResponse()` sert uniquement des pages génériques en dur.
- **Où** : `std::map<int, std::string> error_pages;` dans `Config` (niveau serveur).
  Parsing dans `ParseServer`. Consommation dans `Router::ErrorResponse` (`router.cpp`).
- **Comment** : directive `error_page 404 /errors/404.html;`. Dans `ErrorResponse` :
  chercher d'abord la page configurée pour ce code, la servir si le fichier existe ;
  sinon fallback sur la page générique actuelle (**le fallback reste requis par le sujet, ne pas le supprimer**).
- **Test** : `error_page 404 /404.html;` → une 404 sert cette page ; sans directive,
  la page générique s'affiche toujours.

### 1.6 🟠 Upload : parsing multipart cassé + dossier de stockage
- **Quoi / Pourquoi** : le sujet impose l'upload de fichiers. Deux problèmes :
  (a) le parsing multipart est cassé, (b) le dossier de stockage n'est pas
  configurable séparément du `root`.
- **Où** : `static_handler.cpp` (parsing multipart + écriture fichier) ; champ
  `std::string upload_dir;` dans `LocationConfig`.
- **Comment** :
  - **Bug multipart** : la variable `boundary` n'est jamais affectée avant d'être
    utilisée → le découpage des parts échoue en silence. La récupérer depuis
    `Content-Type: multipart/form-data; boundary=...` via `get_header` (maintenant
    que le bug Tier 0 #2 est réglé, la valeur est correctement lue). Vérifier tout
    le flux : extraction boundary → découpage des parts → nom de fichier → écriture.
  - **Dossier upload** : directive `upload ./uploads;`. Écrire le fichier reçu dans
    ce dossier (pas dans `root`).
- **Test** : `test_file_upload.html` → le fichier apparaît dans le dossier d'upload,
  intact (comparer taille/contenu avec l'original).

---

## TIER 2 — Robustesse 🟡

Le sujet insiste : *« Resilience is key. Your server must remain operational at all times. »*

### 2.1 🟡 `send()` ne gère pas EAGAIN → connexions coupées sous charge
- **Quoi / Pourquoi** : sur socket non-bloquant, `send` peut renvoyer `-1` quand le
  buffer noyau est plein — c'est normal, il faut réessayer plus tard. Aujourd'hui
  tout retour ≤ 0 → `kClose`, donc réponses tronquées / connexions coupées en
  stress test.
- **Où** : `conn_handler.cpp` (~l.70-83, boucle d'envoi) ; même schéma dans
  `cgi_in_handler.cpp`.
- **Comment** : si l'écriture n'est **pas terminée** (write_off_ < taille), ne pas
  fermer : rester en EPOLLOUT et attendre le prochain event — epoll re-signalera
  quand le socket sera de nouveau écrivable.
  ⚠️ **Le sujet interdit de tester `errno` après read/write.** Donc on ne
  distingue PAS EAGAIN d'une vraie erreur via errno : la règle simple et conforme
  est *« si l'écriture n'est pas finie, ne pas fermer, réessayer au prochain
  EPOLLOUT »*. Un vrai socket cassé sera de toute façon signalé par EPOLLERR/EPOLLHUP.
- **Test** : stress test (`siege -b` ou `ab -n 10000 -c 100`) sur une grosse
  ressource → aucune réponse tronquée, le serveur reste dispo.

### 2.2 🟡 Keep-alive HTTP/1.1
- **Quoi / Pourquoi** : après une réponse complète, la connexion est fermée
  (`kClose`). En HTTP/1.1 le défaut est de garder la connexion ouverte → sinon
  comportement non conforme et perfs dégradées.
- **Où** : `conn_handler.cpp:77` (le commentaire `// if HTTP 1.1 need to keep alive`
  marque déjà l'endroit) ; headers dans `http_response.cpp`.
- **Comment** : après envoi complet, si la requête n'a pas `Connection: close` (et
  version 1.1) → repasser l'état de la connexion en `kReading` + EPOLLIN et
  réinitialiser les buffers pour la requête suivante. Ajouter les headers
  `Connection: keep-alive` (ou `close`) et `Date:` dans les réponses.
- **Test** : `curl -v http://localhost:6770/ http://localhost:6770/style.css`
  (2 requêtes, une seule connexion) → les deux réponses passent sans réouvrir le
  socket ; comparer les headers avec NGINX.

### 2.3 🟡 Timeout côté écriture CGI
- **Quoi / Pourquoi** : `CgiHandler` (lecture stdout) a un timeout de 10s, mais
  `CgiInHandler` (écriture stdin) n'en a pas. Un CGI qui ne lit jamais son stdin
  peut bloquer l'écriture indéfiniment.
- **Où** : `cgi_in_handler.cpp` (ajouter un `CheckTimeout(time_t now)` sur le modèle
  de `cgi_handler.cpp:56-64`), branché dans la boucle du reactor.
- **Comment** : mémoriser l'instant de départ, si dépassement → fermer stdin et
  renvoyer une erreur 504.
- **Test** : un CGI qui `sleep` sans lire stdin → le serveur coupe au bout du
  timeout au lieu de rester bloqué.

### 2.4 🟡 (mineur) Timeout de connexion trop permissif
- **Quoi / Pourquoi** : le timeout 60s existe (`conn_handler.cpp`) mais
  `last_activity_` est remis à zéro à **chaque** event → un client envoyant 1 octet
  régulièrement ne timeout jamais (attaque type slowloris).
- **Comment** : envisager un timeout global sur la durée totale de la requête, pas
  seulement sur l'inactivité. Acceptable de le laisser tel quel pour le mandatory —
  à mentionner en soutenance.

---

## TIER 3 — Cleanup avant soutenance ⚪

- [ ] **Retirer `main_test.cpp` du build** : le fichier est **compilé et linké**
  (présent dans le Makefile), même s'il est entièrement commenté. Le sortir de la
  liste des sources du `Makefile` puis supprimer le fichier.
- [ ] **Prints de debug à enlever** : `router.cpp:16`, `route_resolve.cpp:97`
  (`std::cout << "===..."`), `http_request.cpp:81` (`std::cerr << "Value for ..."`),
  et autres traces. `grep -rn "cout\|cerr" src/` pour tout lister.
- [ ] **Statuts HTTP corrects** :
  - `kPayloadTooLarge` vaut **406** → doit être **413** (`http_response.hpp`).
  - `kNoContent` : phrase actuelle "No content found" → "No Content".
  - DELETE en échec : renvoie 405 (`static_handler.cpp:120`) → doit être **500**.
  - Extension inconnue : renvoie 405 → doit être 500 ou 415, pas 405.
- [ ] **`RouteResult::operator=`** (`route_result.cpp:30-38`) : la copie des membres
  est commentée → assignation partielle. Compléter la copie ou, si l'opérateur
  n'est pas utilisé, le passer en privé.
- [ ] **`ServerConfig` / `server_name_`** : struct et champ jamais utilisés (les
  virtual hosts sont hors-scope) → supprimer pour alléger.
- [ ] **Fichiers de conf / assets** : `Configuration.conf` référence `./database`
  (dossier absent) ; `www/assets/main.cpp"` a un `"` dans son nom → renommer/nettoyer.
- [ ] **Artefacts hors repo** : `vgcore.2511858` (10 Mo), le binaire `webserv`,
  `__pycache__/` → vérifier qu'ils sont bien dans `.gitignore` et non commités.
- [ ] **README** (sujet chap. V, obligatoire) : à réécrire. Doit contenir :
  - 1re ligne en italique : *"This project has been created as part of the 42
    curriculum by <login1>, <login2>, <login3>."*
  - sections **Description**, **Instructions** (compilation/exécution), **Resources**
    (références + description de l'usage de l'IA).
  - En anglais. Le README actuel ne parle que des conventions de nommage.

---

## Répartition suggérée (3 personnes)

| Personne | Tâches |
|----------|--------|
| **Reactor/CGI (toi)** | 2.1 (EAGAIN), 2.3 (timeout CGI stdin), part reactor du keep-alive (2.2), re-test CGI POST |
| **HTTP/router** | 1.1 méthodes, 1.2 redirect, 1.3 autoindex, 1.4 index, 1.6 multipart, statuts HTTP (T3), headers keep-alive |
| **Config** | parsing des directives (1.1→1.6 étape « Parser/Stocker »), 1.5 error_page, cleanup conf, README |

---

## Vérification (tests end-to-end)

1. **Build** : `make re` compile avec `-Wall -Wextra -Werror -std=c++98`, sans relink inutile. ✅
2. **Static** : `./webserv Configuration.conf` → navigateur `localhost:6770` :
   `index.html`, CSS, images servis, bons `Content-Type` / `Content-Length`.
3. **CGI POST** (valide le fix Tier 0 #1) : soumettre `add.html` → le script Python
   reçoit le body sur stdin et écrit dans le JSON. `curl -v` pour comparer avec NGINX.
4. **DELETE** : supprimer un contact → entrée effacée, bon code retour.
5. **Upload** (valide 1.6) : `test_file_upload.html` → fichier écrit dans le dossier
   d'upload, contenu intact.
6. **Codes d'erreur** : 404 (absent), 405 (méthode interdite — valide 1.1),
   413 (body > `client_max_body_size` — valide statut T3), 301 (redirect — valide 1.2),
   autoindex on/off sur dossier sans index (valide 1.3).
7. **Résilience** : `siege` / `ab` en stress test → pas de crash, reste dispo
   (valide 2.1). `kill -INT` sur un enfant CGI ne casse pas la boucle (valide Tier 0 #3).
8. **Non-bloquant** : tout read/write passe par epoll, aucun fd socket/pipe bloquant.
