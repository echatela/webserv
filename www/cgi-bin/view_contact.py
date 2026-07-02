#!/usr/bin/env python3

import html
import json
#biblio pour rechercher des fichiers
import os


# Recherche du json
def findContactsFile():
    # Chemin du script courant
    script_dir = os.path.dirname(os.path.abspath(__file__))
    # Chemin du fichier JSON.
    return os.path.join(script_dir, "contacts.json")


# Chercher les contacts
def loadContacts():
    # Recherche du json
    contactsFile = findContactsFile()

    # Si pas de fhcier, liste vide
    if contactsFile is None:
        return []

    # Sinon on essaie d'ouvrir et de lire le fichier
    try:
        # Ouverture du fichier en mode lecteur "r" = read
		#with ferme le fichier apres la lecture
        with open(contactsFile, "r") as file:
            # On lit le contenu et on retire les espaces inutiles.
            content = file.read().strip()

            # Si fichier vide, return liste vide
            if not content:
                return []

            # On decode le json (loads = car chaine de char)
            contacts = json.loads(content)

            # Sinon on renvoit la lst
            if isinstance(contacts, list):
                return contacts

    # On ignore les erreurs de lecture ou de format JSON.
    except (IOError, OSError, ValueError, TypeError):
        pass

    # Si une erreur arrive, on renvoie une liste vide.
    return []


# Head du HTML pour la response
def print_page_start(title):
    print("<!doctype html>")
    print('<html lang="en">')
    print("<head>")
    print('\t<meta charset="utf-8">')
    print(f"\t<title>{title}</title>")
    print('\t<link rel="stylesheet" href="../style.css">')
    print("\t<link href='https://unpkg.com/boxicons@2.1.4/css/boxicons.min.css' rel='stylesheet'>")
    # Suppression du contact
    print("\t<script>")
    print("\t\tfunction deleteContact(id) {")
    print("\t\t\tfetch('/cgi-bin/delete_contact.py?id=' + id, { method: 'DELETE' })")
    print("\t\t\t\t.then(response => response.json())")
    print("\t\t\t\t.then(() => location.reload())")
    print("\t\t}")
    print("\t</script>")
    print("</head>")
    print("<body>")


# Fin du HTML
def print_page_end():
    print("</body>")
    print("</html>")


# Response pour aucun contact.
def print_empty_state():
    print_page_start("View Contact")
    print('\t<div class="phonebook">')
    print("\t\t<h1>No contact yet</h1>")
    print("\t\t<p>There are no contacts saved at the moment.</p>")
    print('\t\t\t<a href="index.html"><button class="links">Back</button></a>')
    print("\t\t</div>")
    print_page_end()


# Cette fonction affiche tous les contacts trouves dans le JSON.
def print_contacts(contacts):
    # On affiche le debut de la page.
    print_page_start("View Contacts")

    # On ouvre le conteneur principal.
    print('\t<div class="phonebook contact-page">')

    # On affiche un titre general.
    print("\t\t<h1>Contacts</h1>")

    # On parcourt chaque contact de la liste.
    for i, contact in enumerate(contacts):
        # On recupere le nom et on le securise pour le HTML.
        name = html.escape(str(contact.get("name", "")))

        # On recupere le numero de telephone et on le securise pour le HTML.
        phone = html.escape(str(contact.get("phone", "")))

        # On ouvre une carte pour un contact.
        print('\t\t<div class="contact-card">')

        # On affiche la photo.
        print(f'\t\t\t<div class="contact-photo"><img src="images/default.png" alt="profile"></div>')

        # On affiche le nom.
        print(f'\t\t\t<div class="contact-info"><h2>{name}</h2>')

        # On affiche le telephone.
        print(f"\t\t\t<p>{phone}</p>")

        # On ferme le bloc d'informations avant les actions.
        print("\t\t\t</div>")

        # Bouton delete
        # print('\t\t <form action="#" method ="delete"> <button class="delete-btn"><i class="bx bx-trash"></i></button><br></form>')
        print(f'\t\t <button class="delete-btn" onclick="deleteContact({i})" type="button"><i class="bx bx-trash"></i></button>')
        # On ferme la carte du contact.
        print("\t\t</div>")

    # On affiche le bouton de retour.
    print('\t\t\t<a href="/index.html"><button class="links">Back</button></a>')

    # On ferme le conteneur principal.
    print("\t</div>")

    # On ferme la page HTML.
    print_page_end()
	

# Affichage de la response
# ______________________________________
print("Content-Type: text/html")

print()

contacts = loadContacts()

if len(contacts) == 0:
    print_empty_state()
else:
    print_contacts(contacts)