#!/usr/bin/env python3

# On importe les outils pour echapper le HTML.
import html

# On importe le module pour lire et decoder le JSON.
import json

# On importe les fonctions pour travailler avec les chemins de fichiers.
import os


# Cette fonction cherche le fichier contacts.json a plusieurs endroits possibles.
def find_contacts_file():
    # On recupere le dossier du script courant.
    script_dir = os.path.dirname(os.path.abspath(__file__))

    # On prepare plusieurs chemins possibles pour retrouver le fichier JSON.
    candidates = [
        os.path.join(script_dir, "contacts.json"),
        os.path.join(script_dir, "..", "contacts.json"),
        os.path.join(script_dir, "..", "..", "contacts.json"),
        os.path.join(os.getcwd(), "contacts.json"),
    ]

    # On garde une trace des chemins deja testes.
    seen = set()

    # On parcourt chaque chemin candidat.
    for path in candidates:
        # On transforme le chemin en chemin absolu.
        full_path = os.path.abspath(path)

        # Si on a deja teste ce chemin, on passe au suivant.
        if full_path in seen:
            continue

        # On memorize le chemin deja teste.
        seen.add(full_path)

        # Si le fichier existe, on retourne ce chemin.
        if os.path.isfile(full_path):
            return full_path

    # Si rien n'a ete trouve, on renvoie None.
    return None


# Cette fonction charge les contacts depuis le fichier JSON.
def load_contacts():
    # On cherche le fichier contacts.json.
    contacts_file = find_contacts_file()

    # Si aucun fichier n'a ete trouve, on renvoie une liste vide.
    if contacts_file is None:
        return []

    # On essaie d'ouvrir et de lire le fichier.
    try:
        # On ouvre le fichier en lecture.
        with open(contacts_file, "r") as file:
            # On lit le contenu et on retire les espaces inutiles.
            content = file.read().strip()

            # Si le fichier est vide, on renvoie une liste vide.
            if not content:
                return []

            # On decode le JSON.
            contacts = json.loads(content)

            # Si le contenu est bien une liste, on la renvoie.
            if isinstance(contacts, list):
                return contacts

    # On ignore les erreurs de lecture ou de format JSON.
    except (IOError, OSError, ValueError, TypeError):
        pass

    # Si une erreur arrive, on renvoie une liste vide.
    return []


# Cette fonction ecrit le debut de la page HTML.
def print_page_start(title):
    # On declare le type de document.
    print("<!doctype html>")

    # On ouvre la balise html.
    print('<html lang="en">')

    # On ouvre la balise head.
    print("<head>")

    # On declare l'encodage de la page.
    print('\t<meta charset="utf-8">')

    # On affiche le titre de la page.
    print(f"\t<title>{title}</title>")

    # On relie la feuille de style CSS.
    print('\t<link rel="stylesheet" href="style.css">')

    # On ferme la balise head.
    print("</head>")

    # On ouvre la balise body.
    print("<body>")


# Cette fonction ferme la page HTML.
def print_page_end():
    # On ferme le body.
    print("</body>")

    # On ferme le html.
    print("</html>")


# Cette fonction affiche la page quand il n'y a aucun contact.
def print_empty_state():
    # On affiche le debut de la page.
    print_page_start("View Contact")

    # On ouvre le conteneur principal.
    print('\t<div class="menu">')

    # On affiche le message principal.
    print("\t\t<h1>No contact yet</h1>")

    # On affiche un petit texte explicatif.
    print("\t\t<p>There are no contacts saved at the moment.</p>")

    # On affiche le bouton de retour vers l'index.
    print('\t\t\t<a href="index.html"><button class="links">Back</button></a>')

    # On ferme le conteneur principal.
    print("\t\t</div>")

    # On ferme la page HTML.
    print_page_end()


# Cette fonction affiche tous les contacts trouves dans le JSON.
def print_contacts(contacts):
    # On affiche le debut de la page.
    print_page_start("View Contacts")

    # On ouvre le conteneur principal.
    print('\t<div class="menu contact-page">')

    # On affiche un titre general.
    print("\t\t<h1>Contacts</h1>")

    # On parcourt chaque contact de la liste.
    for contact in contacts:
        # On recupere le nom et on le securise pour le HTML.
        name = html.escape(str(contact.get("name", "")))

        # On recupere le numero de telephone et on le securise pour le HTML.
        phone = html.escape(str(contact.get("phone", "")))

        # On ouvre une carte pour un contact.
        print("\t\t<div class=\"contact-card\">")

        # On affiche le nom.
        print(f"\t\t\t<p class=\"contact-name\">{name}</p>")

        # On affiche le telephone.
        print(f"\t\t\t<p class=\"contact-phone\">{phone}</p>")

        # On ferme la carte du contact.
        print("\t\t</div>")

    # On ajoute la zone des actions.
    print('\t\t<div class="contact-actions" style="margin-top: 50px;">')

    # On affiche le bouton de retour.
    print('\t\t\t<a href="/index.html"><button class="links">Back</button></a>')

    # On ferme la zone des actions.
    print("\t\t</div>")

    # On ferme le conteneur principal.
    print("\t</div>")

    # On ferme la page HTML.
    print_page_end()


# Le CGI doit annoncer au serveur que la reponse est du HTML.
print("Content-Type: text/html")

# Ligne vide obligatoire entre les en-tetes HTTP et le contenu.
print()

# On charge les contacts.
contacts = load_contacts()

# Si la liste est vide, on affiche la page vide.
if len(contacts) == 0:
    print_empty_state()

# Sinon, on affiche les contacts.
else:
    print_contacts(contacts)