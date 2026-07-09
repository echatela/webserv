#!/usr/bin/env python3

import json
import os
from urllib.parse import parse_qs  # Pour parser la query string de l'URL (ex: ?id=5)

def find_contacts_file():
	script_dir = os.path.dirname(os.path.abspath(__file__))
	return os.path.join(script_dir, "contacts.json")

def find_upload_dir():
	# Récupère le dossier du script
	script_dir = os.path.dirname(os.path.abspath(__file__))
	# Remonte d'un niveau (..) puis descend dans "assets"
	# Exemple: /www/cgi-bin -> /www/assets
	return os.path.abspath(os.path.join(script_dir, "..", "assets"))


def load_contacts():
	contacts_file = find_contacts_file()
	try:
		with open(contacts_file, "r") as file:
			content = file.read().strip()
			if not content:
				return []
			contacts = json.loads(content)
			if isinstance(contacts, list):
				return contacts
	except (IOError, OSError, ValueError, TypeError):
		pass
	return []


# Sauvegarder la liste mise à jour dans le fichier JSON
def save_contacts(contacts):
	contacts_file = find_contacts_file()
	with open(contacts_file, "w") as file:
		# Convertit la liste Python en JSON formaté avec indentation
		json.dump(contacts, file, indent=2)
		# Ajoute une newline à la fin du fichier (pour propretée)
		file.write("\n")


# Envoyer une réponse JSON au navigateur
def json_response(ok, message, status=None):
	# Envoie le header Content-Type pour que le navigateur sache que c'est du JSON
	print("Content-Type: application/json")
	# Ligne vide (séparation entre headers et corps)
	print()
	# Convertit le dictionnaire en JSON et l'envoie
	# Exemple: {"ok": true, "message": "Contact deleted."}
	print(json.dumps({"ok": ok, "message": message}))


# Extraire l'ID du contact depuis la query string
def parse_contact_id():
	# Récupère la chaîne de query de l'URL (ex: "id=5" depuis /cgi/delete_contact.py?id=5)
	query = os.environ.get("QUERY_STRING", "")
	# Parse la query string en dictionnaire (ex: {"id": ["5"]})
	fields = parse_qs(query, keep_blank_values=True)
	# Récupère la valeur du paramètre "id" (première valeur, ou "" par défaut)
	raw_id = fields.get("id", [""])[0].strip()
	# Si l'ID est vide, retourne une erreur
	if not raw_id:
		return None, "Missing contact id."
	try:
		# Essaie de convertir l'ID en nombre entier
		return int(raw_id), None
	except ValueError:
		# Si la conversion échoue, retourne une erreur
		return None, "Invalid contact id."


# Supprimer l'image du disque associée au contact
def remove_uploaded_image(image_path):
	# Si pas d'image, on ne fait rien et on sort
	if not image_path:
		return
	# Si le chemin ne commence pas par "/upload/", on refuse de le supprimer 
	if not image_path.startswith("/upload/"):
		return

	# Récupère seulement le nom de fichier (ex: "contact_5_photo.jpg")
	file_name = os.path.basename(image_path)
	# Construit le chemin complet sur le disque
	file_path = os.path.join(find_upload_dir(), file_name)
	try:
		# Essaie de supprimer le fichier
		os.remove(file_path)
		print("image deleted\n")
	except OSError:
		# Si la suppression échoue, on ignore l'erreur silencieusement
		pass


# Supprimer un contact spécifique par son ID
def delete_contact(contact_id):
	# Charge la liste complète des contacts
	contacts = load_contacts()
	# Liste pour stocker les contacts qui ne seront pas supprimés
	updated_contacts = []
	# Stocke le contact supprimé pour accéder à son image
	deleted_contact = None

	# Parcourt chaque contact
	for contact in contacts:
		# Si on n'a pas encore trouvé le contact à supprimer ET que c'est le bon ID
		if deleted_contact is None and int(contact.get("id", -1)) == contact_id:
			# Marque ce contact comme supprimé
			deleted_contact = contact
			# Continue sans l'ajouter à la liste mise à jour (donc il n'est pas inclus)
			continue
		# Sinon, ajoute le contact à la liste des contacts restants
		updated_contacts.append(contact)

	# Si aucun contact n'a été trouvé avec cet ID
	if deleted_contact is None:
		return False, "Contact not found."

	# Supprime l'image du disque si elle existe
	remove_uploaded_image(deleted_contact.get("image", ""))
	# Sauvegarde la liste mise à jour (sans le contact supprimé)
	save_contacts(updated_contacts)
	# Retourne succès
	return True, "Contact deleted."

# Récupère la méthode HTTP (POST ou DELETE), convertie en majuscules
method = os.environ.get("REQUEST_METHOD", "GET").upper()
# Vérifie que la méthode est bien DELETE ou POST
if method != "DELETE" and method != "POST":
	# Si ce n'est pas l'un de ces deux, envoie une erreur 405 (Méthode non autorisée)
	json_response(False, "Method not allowed.")
	# Arrête l'exécution du script
	raise SystemExit

# Extrait l'ID du contact depuis la URL
contact_id, error = parse_contact_id()
# Si une erreur s'est produite (ID manquant ou invalide)
if error:
	# Envoie une erreur 400 (Requête invalide)
	json_response(False, error)
	# Arrête l'exécution du script
	raise SystemExit

# Essaie de supprimer le contact avec cet ID
ok, message = delete_contact(contact_id)
# Si la suppression a échoué
if not ok:
	# Envoie une erreur 404 (Contact non trouvé)
	json_response(False, message)
	# Arrête l'exécution du script
	raise SystemExit

# Si tout s'est bien passé, envoie une réponse de succès en JSON
json_response(True, message)
