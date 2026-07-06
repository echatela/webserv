# Webserv — Roadmap du restant (révision post-corrections)

## Contexte

Roadmap confrontée à l'état réel du code (relecture ligne à ligne le 2026-07-06).
Le **Tier 0** (3 bugs bloquants) et le **Tier 2** (robustesse : EAGAIN, fermeture
HTTP/1.0, timeout stdin CGI) sont **corrigés et vérifiés**. Le multipart lit
désormais le boundary correctement (upload OK) et le code 413 est juste. Ce
document ne liste donc **que ce qui reste** pour un mandatory propre.

Répartition : moi = reactor/CGI/connexion ; binôme = HTTP/router + config.

---

## TIER 1 — Features mandatory de config manquantes (priorité) — *binôme HTTP/config*

`LocationConfig` (`src/config/config.hpp:18-22`) ne contient encore que
`base_location`, `root`, `cgi`. Le pattern reste : **1) parser la directive,
2) champ dans `LocationConfig`, 3) consommer dans le routeur/handler.**

| Feature (sujet)             | Directive           | À faire |
|-----------------------------|---------------------|---------|
| Méthodes autorisées / route | `methods`/`allow`   | parser → champ → check dans `Router::ProcessRequest` (`router.cpp:19`), sinon **405** |
| Redirection HTTP            | `return 301 <url>`  | parser → champ → réponse 301 + header `Location:` avant résolution fichier ; ajouter `kMovedPermanently=301` à l'enum (`http_response.hpp:7`) |
| Directory listing           | `autoindex on/off`  | brancher dans `StaticHandler::BuildGet` (`static_handler.cpp:84`) : générer le listing si dossier + pas d'index |
| Fichier index par défaut    | `index`             | aujourd'hui **hardcodé** `index.html` (`static_handler.cpp:86`) → rendre configurable |
| Pages d'erreur custom       | `error_page <c> <p>`| map code→path dans `Config` → utilisée par `Router::ErrorResponse` (`router.cpp:74`) avant le fallback générique (fallback OK et à garder) |
| Dossier d'upload            | `upload`/storage    | l'upload marche via `root` ; ajouter un chemin de stockage séparé si le sujet/eval l'exige |

---

## TIER 3 — Cleanup avant soutenance

- **Prints de debug à retirer** (confirmés actifs) :
  - `src/http_protocol/router.cpp:22` (`====...` + status)
  - `src/http_protocol/route_resolve.cpp:99` (`====...max_body_size`)
  - `src/http_protocol/http_request.cpp:81` (`std::cerr Value for ... not found`)
  - `src/http_protocol/static_handler.cpp:237` (`e.what()`)
  - `src/handlers/conn_handler.cpp:89` (buffer requête, CYAN) et `:97` (réponse, YELLOW)
  - *(garder `main.cpp:12/26` : usage + erreur légitimes)*
- **Code mort** : supprimer `src/http_protocol/main_test.cpp` (déjà hors du Makefile).
- **`RouteResult::operator=`** (`route_result.cpp`) : `plan_` et `response_` sont
  **commentés** → copie partielle. Compléter (ou confirmer qu'il n'est pas utilisé).
- **`CgiResponse` fragile** (`router.cpp:41-64`) — *ma partie CGI* :
  - `cgi_result.substr(i, ...)` ligne 61 : résultat **jeté**, les `\n` de tête
    restent potentiellement dans le body.
  - parsing d'en-têtes qui suppose exactement `": "` et `erase` manuel → fiabiliser
    (couper proprement sur `\n\n` / `\r\n\r\n`, séparer clé/valeur au premier `:`).
- **DELETE en échec** renvoie **405** (`static_handler.cpp:127`) → devrait être
  404 (absent) ou 500 (échec `remove`), pas 405.
- **Config d'éval** : `Configuration.conf` référence `./database` (dossier absent) ;
  vérifier le fichier parasite `www/assets/main.cpp"` (guillemet dans le nom).
- **`.gitignore`** : exclure `vgcore.*`, le binaire `webserv`, `__pycache__/`.
- **README** (sujet chap. V) : 1re ligne en italique *"This project has been created
  as part of the 42 curriculum by <login1>, ..."* + sections Description /
  Instructions / Resources (+ usage IA). À réécrire.

---

## Vérification end-to-end

1. **Build** : `make re` clean avec `-Wall -Wextra -Werror -std=c++98`.
2. **Static** : `localhost:6770` → index, CSS, images (bons Content-Type/Length).
3. **CGI GET/POST** : `view_contact.py` répond vite et se ferme ;
   `add_contact.py` reçoit le body sur stdin et écrit le JSON. Comparer `curl -v` à NGINX.
4. **Upload** : formulaire multipart → fichier écrit dans le dossier cible.
5. **DELETE** : suppression → bon code retour (une fois le 405 corrigé).
6. **Erreurs config** : 404, **405** (méthode interdite par route), **413**
   (body > `client_max_body_size`), **301** (route de redirection), autoindex on/off.
7. **Résilience** : stress `siege`/`ab` → pas de crash ; `kill -INT` ne casse pas la boucle.
8. **Non-bloquant** : tout read/write passe par epoll, aucun fd bloquant.

---

## Historique — déjà réglé (ne plus toucher)

- **Tier 0** : `cgi_in_handler` fermait toujours (EPOLLERR/HUP mal placé) ✅ ;
  `get_header` renvoyait vide (shadowing) ✅ ; `reactor` crashait sur EINTR ✅.
- **Tier 2** : `send`/`write` ne ferment plus sur `n<0` (EAGAIN) ✅ ; connexion
  fermée après réponse complète (HTTP/1.0, `conn_handler.cpp:79`) ✅ ; timeout
  stdin CGI (`CgiInHandler::CheckTimeout`) ✅.
- **Tier 3** : `kPayloadTooLarge` = **413** ✅ ; parsing multipart/boundary OK ✅.
