# Webserv — Roadmap de fin de projet

Projet à 3 personnes, presque terminé. État des lieux confronté au sujet PDF.
Les 3 bugs bloquants du Tier 0 ont été **vérifiés à la main dans le code source**.

Niveau visé : **mandatory qui passe + robustesse** (keep-alive, EAGAIN, timeouts, cleanup).

---

## TIER 0 — Bugs bloquants (prioritaires, sinon note = 0 ou features KO)

1. **CGI POST body jamais envoyé** — `src/reactor/cgi_in_handler.cpp:25-26`
   ```cpp
   if (events & (EPOLLERR | EPOLLHUP)) {}   // corps vide
       return kClose;                        // s'exécute TOUJOURS
   ```
   Le `return kClose` est hors du `if` → au premier EPOLLOUT on ferme stdin sans
   rien écrire. Tout CGI attendant un body (POST) hang → timeout 504.
   → Fix : mettre le `return kClose;` **dans** le bloc `if`. La logique d'écriture
   EPOLLOUT existante (l.28-37) est correcte.

2. **`get_header()` renvoie toujours vide** — `src/http_protocol/http_request.cpp:77`
   ```cpp
   std::string value;
   try { std::string value = header_.at(key); }  // shadowing : variable locale
   ...
   return value;                                  // renvoie le vide externe
   ```
   → Fix : `value = header_.at(key);` (sans redéclarer). Puis vérifier tous les
   appelants (Content-Type / boundary multipart, Content-Length).

3. **Crash sur signal (EINTR)** — `src/reactor/reactor.cpp:48`
   ```cpp
   if (errno == EINTR)
       throw std::runtime_error("epoll_wait() failed");  // logique inversée
   ```
   Un SIGCHLD (fin d'enfant CGI) peut crasher la boucle → note 0.
   → Fix : sur EINTR, `return;` (continuer) ; throw seulement pour les autres erreurs.

---

## TIER 1 — Features mandatory manquantes (config + consommation)

Pattern : **1) parser la directive, 2) la stocker dans `LocationConfig`,
3) la consommer dans le routeur/handler.**

Fichiers : `src/config/config.hpp` (`LocationConfig`, `Config`),
`src/config/config_parser.cpp` (`ParseLocation`),
consommation `src/http_protocol/route_resolve.cpp` + `static_handler.cpp` + `router.cpp`.

| Feature (sujet)             | Directive           | État | À faire |
|-----------------------------|---------------------|------|---------|
| Méthodes autorisées / route | `methods`/`allow`   | ❌   | parser → champ → check, sinon **405** |
| Redirection HTTP            | `return 301 <url>`  | ❌   | parser → réponse 301/302 + `Location:` |
| Directory listing          | `autoindex on/off`  | ❌   | parser → générer listing HTML si dossier sans index |
| Index par défaut           | `index`             | ⚠️   | hardcodé `index.html` (`static_handler.cpp:81`) → configurable |
| Pages d'erreur custom      | `error_page <c> <p>`| ❌   | map code→path dans `Config` → utilisée par `Router::ErrorResponse` |
| Dossier d'upload           | `upload` / storage  | ⚠️   | chemin séparé du `root`, à parser + utiliser dans le POST |

- **Autoindex** : brancher là où `is_directory` est détecté ; aujourd'hui on append
  aveuglément `index.html` → 404 si absent.
- **error_page** : `Router::ErrorResponse()` sert des pages en dur ; tenter d'abord
  la page config, fallback générique (fallback requis par le sujet, à garder).
- **Multipart cassé** : `static_handler.cpp` — la variable `boundary` n'est jamais
  affectée → parsing multipart (upload) échoue en silence. À corriger avec le bug #2.

---

## TIER 2 — Robustesse (le serveur doit rester dispo en permanence)

1. **`send()` ne gère pas EAGAIN** — `src/reactor/conn_handler.cpp` (~l.70-83).
   Tout retour ≤ 0 → `kClose`. Sur socket non-bloquant, `-1` = "buffer plein,
   réessaie". Fermer = réponses tronquées sous charge.
   → Si l'écriture n'est pas finie, ne pas fermer : rester en EPOLLOUT, attendre le
   prochain event. ⚠️ Le sujet **interdit de tester errno** après read/write → ne
   pas inspecter errno, juste ne pas fermer et laisser epoll re-signaler. Idem
   `CgiInHandler`.

2. **Keep-alive HTTP/1.1** — `conn_handler.cpp:77` (commentaire déjà là).
   Après réponse complète on ferme. Pour HTTP/1.1 propre : repasser en `kReading`
   + EPOLLIN, réinitialiser l'état, sauf `Connection: close`. Ajouter headers
   `Connection:` + `Date` (`http_response.cpp`).

3. **Timeout écriture CGI** — `CgiInHandler` n'a pas de `CheckTimeout` (seul
   `CgiHandler` en a un, 10s). Un CGI qui ne lit pas son stdin peut bloquer.
   → Ajouter le même timeout côté écriture stdin.

4. **Timeout connexion** — OK (60s), mais `last_activity_` reset à chaque event :
   un client envoyant 1 octet régulièrement ne timeout jamais. Acceptable, à noter.

---

## TIER 3 — Cleanup avant soutenance

- **Supprimer** `src/http_protocol/main_test.cpp` (entièrement commenté, code mort).
- **Retirer prints de debug** : `router.cpp:16`, `route_resolve.cpp:97`,
  `http_request.cpp:74/80`, etc.
- **Statuts HTTP** : `kPayloadTooLarge` = 406 → doit être **413** (`http_response.hpp`) ;
  `kNoContent` → phrase "No Content" ; DELETE en échec → 500 (pas 405,
  `static_handler.cpp:120`) ; extension inconnue → pas 405.
- **`RouteResult::operator=`** (`route_result.cpp:30-38`) : copie des membres
  commentée → assignation partielle. Compléter ou supprimer selon usage.
- **`ServerConfig` / `server_name_`** : jamais utilisés (virtual hosts hors-scope) →
  supprimer.
- **Config d'éval** : `Configuration.conf` référence `./database` (absent) ;
  `www/assets/main.cpp"` a un `"` dans le nom → nettoyer.
- **Artefacts à ne pas commit** : `vgcore.2511858` (10 Mo), binaire `webserv`,
  `__pycache__/` → vérifier `.gitignore`.
- **README** (sujet chap. V) : 1re ligne italique
  *"This project has been created as part of the 42 curriculum by <login1>, ..."*,
  sections **Description / Instructions / Resources** (+ usage de l'IA). Le README
  actuel ne parle que du naming → à réécrire.

---

## Répartition suggérée (3 personnes)

- **Reactor/CGI** : Tier 0 #1 & #3, Tier 2 #1/#3, timeouts.
- **HTTP/router** : Tier 0 #2, multipart, méthodes/redirect/autoindex côté handler,
  statuts HTTP, keep-alive headers.
- **Config** : Tier 1 parsing (methods, return, autoindex, index, error_page,
  upload), cleanup config, README.

---

## Vérification (tests end-to-end)

1. **Build** : `make re` compile avec `-Wall -Wextra -Werror -std=c++98`, sans relink inutile.
2. **Static** : `./webserv Configuration.conf` → navigateur `localhost:6770`, bons
   Content-Type/Content-Length.
3. **CGI POST** (valide fix #1) : soumettre `add.html` → le script Python reçoit le
   body sur stdin et écrit le JSON. Comparer headers avec NGINX (`curl -v`).
4. **DELETE** : supprimer un contact → entrée effacée, bon code.
5. **Upload** : `test_file_upload.html` → fichier écrit dans le dossier d'upload.
6. **Erreurs** : 404, 405 (méthode interdite), 413 (body > `client_max_body_size`),
   301 (redirection), autoindex on/off sur dossier sans index.
7. **Résilience** : stress test (`siege`/`ab`) → pas de crash, reste dispo (valide
   fix #3 EINTR + EAGAIN). `kill -INT` ne casse pas la boucle.
8. **Non-bloquant** : tout read/write passe par epoll, aucun fd socket/pipe bloquant.
