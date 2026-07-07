# Webserv — Roadmap du restant (révision post-corrections)

## Contexte

Roadmap confrontée à l'état réel du code (dernière relecture + build le 2026-07-07).
Le **Tier 0** (3 bugs bloquants), le **Tier 2** (robustesse) et **4 des 6 features
Tier 1** sont **faits et vérifiés** ; `make re` compile proprement avec
`-Wall -Wextra -Werror -std=c++98`. Ce document ne liste que **ce qui reste**.

Répartition : moi = reactor/CGI/connexion ; binôme = HTTP/router + config.

---

## TIER 1 — Features mandatory de config

Pattern commun : **1) champ dans `LocationConfig`, 2) parser la directive
(`ParseLocation`), 3) consommer dans le routeur/handler.**

| Feature (sujet)             | Directive           | État | Détail |
|-----------------------------|---------------------|------|--------|
| Méthodes autorisées / route | `methods`           | ✅   | `MethodNotAllowed` dans `route_resolve.cpp`, 405 via la porte de sortie erreur |
| Redirection HTTP            | `return 301 <url>`  | ✅   | `redirect` parsé (taille validée), `RedirectResponse` + header `Location`, court-circuit dans `ProcessRequest` |
| Directory listing           | `autoindex on/off`  | ✅   | `BuildAutoindex` (`opendir`/`readdir`), défaut `off`, argument validé |
| Fichier index par défaut    | `index`             | ✅   | liste de candidats testés via `StatCheck`, fallback `index.html` |
| **Pages d'erreur custom**   | `error_page <c> <p>`| ⬜   | map code→path dans `Config` (niveau **serveur**) → utilisée par `Router::ErrorResponse` (`router.cpp:99`) avant le fallback générique (garder le fallback) |
| **Dossier d'upload**        | `upload`/storage    | ⬜   | l'upload marche via `root` ; ajouter un chemin de stockage séparé si l'eval l'exige (consommé dans `StaticHandler::BuildPost`) |

Points à vérifier au passage (pas des bugs bloquants) :
- **Jointure de chemin** dans `BuildGet`/`BuildAutoindex` : `path.append(candidate)`
  suppose que `info.file_path` finit par `/`. Confirmer via `BuildFilesystemPath`
  pour une URI de dossier ; ajouter le `/` si besoin.
- Harmoniser la branche « pas de directive `index` » avec l'autoindex (aujourd'hui
  gérée par le fallback `index.html`, cohérent mais à garder en tête).

---

## TIER 3 — Cleanup avant soutenance

- **Prints de debug à retirer** (confirmés actifs le 2026-07-07) :
  - `src/http_protocol/router.cpp:23` (`====...` + status)
  - `src/http_protocol/route_resolve.cpp` (`====...max_body_size`)
  - `src/http_protocol/http_request.cpp:81` (`std::cerr Value for ... not found`)
  - `src/http_protocol/static_handler.cpp` (`e.what()` dans `GetFilename`)
  - `src/handlers/conn_handler.cpp:89` (buffer requête, CYAN) et `:97` (réponse, YELLOW)
  - *(garder `main.cpp` : usage + erreur légitimes)*
- **Code mort** : supprimer `src/http_protocol/main_test.cpp` (déjà hors du Makefile).
- **`RouteResult::operator=`** (`route_result.cpp`) : `plan_` et `response_` sont
  **commentés** → copie partielle. Compléter (ou confirmer qu'il n'est pas utilisé).
- **`CgiResponse` fragile** (`router.cpp`, ~l.38-66) — *ma partie CGI* :
  - le `substr` de fin : résultat **jeté**, les `\n` de tête restent dans le body.
  - parsing d'en-têtes qui suppose exactement `": "` et `erase` manuel → fiabiliser
    (couper sur `\n\n` / `\r\n\r\n`, séparer clé/valeur au premier `:`).
- **DELETE en échec** renvoie **405** (`static_handler.cpp`, `BuildDelete`) → devrait
  être 404 (absent) ou 500 (échec `remove`), pas 405.
- **Config d'éval** : `Configuration.conf` référence `./database` (dossier absent) ;
  fichier parasite `www/assets/main.cpp"` (guillemet dans le nom).
- **`.gitignore`** : exclure `vgcore.*`, le binaire `webserv`, `__pycache__/`.
- **README** (sujet chap. V) : 1re ligne en italique *"This project has been created
  as part of the 42 curriculum by <login1>, ..."* + sections Description /
  Instructions / Resources (+ usage IA). À réécrire.

---

## Vérification end-to-end (à faire une fois error_page + upload finis)

1. **Build** : `make re` clean (✅ au 2026-07-07).
2. **Static** : `localhost:6770` → index, CSS, images (bons Content-Type/Length).
3. **CGI GET/POST** : `view_contact.py` répond vite et se ferme ;
   `add_contact.py` reçoit le body sur stdin et écrit le JSON. Comparer `curl -v` à NGINX.
4. **Méthodes** : `methods GET;` sur une route → un POST renvoie **405**.
5. **Redirection** : `return 301 /new;` → 301 + header `Location`, le navigateur suit.
6. **Autoindex** : `autoindex on;` sur un dossier sans index → listing cliquable ;
   `off` → **403**.
7. **Index** : `index home.html;` → sert bien `home.html` sur le dossier.
8. **Upload** : formulaire multipart → fichier écrit dans le dossier cible.
9. **DELETE** : suppression → bon code retour (une fois le 405 corrigé).
10. **Erreurs** : 404, 413 (body > `client_max_body_size`), error_page custom.
11. **Résilience** : stress `siege`/`ab` → pas de crash ; `kill -INT` ne casse pas la boucle.
12. **Non-bloquant** : tout read/write passe par epoll, aucun fd bloquant.

---

## Historique — déjà réglé (ne plus toucher)

- **Tier 0** : `cgi_in_handler` (EPOLLERR/HUP) ✅ ; `get_header` (shadowing) ✅ ;
  `reactor` EINTR ✅.
- **Tier 2** : `send`/`write` ne ferment plus sur `n<0` (EAGAIN) ✅ ; fermeture
  après réponse (HTTP/1.0) ✅ ; timeout stdin CGI ✅.
- **Tier 1 (4/6)** : `methods`/405 ✅ ; `index` configurable ✅ ; `autoindex` ✅ ;
  `return`/redirection ✅.
- **Tier 3 (partiel)** : `kPayloadTooLarge` = 413 ✅ ; multipart/boundary ✅ ;
  extension inconnue → `application/octet-stream` (plus de 405 fantôme) ✅.
