#!/usr/bin/env python3

import html
import json
import os
import sys
from email import policy
from email.parser import BytesParser
from urllib.parse import parse_qs


def find_contacts_file():
	script_dir = os.path.dirname(os.path.abspath(__file__))
	return os.path.join(script_dir, "contacts.json")


# Assure de retrouver les images
def find_upload_dir():
	script_dir = os.path.dirname(os.path.abspath(__file__))
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


def save_contacts(contacts):
	contacts_file = find_contacts_file()
	with open(contacts_file, "w") as file:
		json.dump(contacts, file, indent=2)
		file.write("\n")


# Le corps CGI est lu depuis stdin
def read_request_body():
	length = int(os.environ.get("CONTENT_LENGTH", "0") or "0")
	if length <= 0:
		return b""
	return sys.stdin.buffer.read(length)


# Transforme la request pour etre parser en python
def multipart_parts(body, content_type):
	header = f"Content-Type: {content_type}\r\nMIME-Version: 1.0\r\n\r\n".encode("utf-8")
	message = BytesParser(policy=policy.default).parsebytes(header + body)
	for part in message.iter_parts():
		name = part.get_param("name", header="content-disposition") or ""
		filename = part.get_filename()
		payload = part.get_payload(decode=True) or b""
		yield name, filename, payload


# Recupere les infos du formulaire
def parse_post_data():
	# verif type de formulaire
	content_type = os.environ.get("CONTENT_TYPE", "")
	# lit les donnees du formulaire
	body = read_request_body()
	# si data, on parse le formulaire multipart/form-data
	if content_type.startswith("multipart/form-data") and body:
		fields = {"name": "", "phone": ""}
		image_field = None
		for name, filename, payload in multipart_parts(body, content_type):
			if name in ("name", "phone"):
				fields[name] = payload.decode("utf-8", "ignore").strip()
			elif name == "image" and filename:
				image_field = {"filename": filename, "content": payload}
		return {
			"name": fields["name"],
			"phone": fields["phone"],
			"image": image_field,
		}

	fields = parse_qs(body.decode("utf-8", "ignore"), keep_blank_values=True)
	return {
		"name": fields.get("name", [""])[0].strip(),
		"phone": fields.get("phone", [""])[0].strip(),
		"image": None,
	}

# Calcule le prochain ID de contact en fonction des contacts existants
def next_contact_id(contacts):
	if not contacts:
		return 0
	return max(int(contact.get("id", -1)) for contact in contacts) + 1

# Nettoie les infos
# Ne conserve que les caractères sûrs pour un nom de fichier.
def safe_filename(filename):
	base_name = os.path.basename(filename)
	allowed = []
	for character in base_name:
		if character.isalnum() or character in (".", "_", "-"):
			allowed.append(character)
	return "".join(allowed) or "contact-image"


# Enregistre l'image dans un chemin public prévisible et retourne son URL
def store_uploaded_image(image_field, contact_id):
	#si pas dimage, on retourne une chaine vide
	if not image_field or not image_field.get("filename"):
		return ""

	# recupere le chemin du dossier upload et cree le dossier s'il n'existe pas
	upload_dir = find_upload_dir()
	os.makedirs(upload_dir, exist_ok=True)
	file_name = safe_filename(image_field["filename"])
	stored_name = f"contact_{contact_id}_{file_name}"
	stored_path = os.path.join(upload_dir, stored_name)
	# ecrit le contenu de l'image dans le fichier
	with open(stored_path, "wb") as output_file:
		output_file.write(image_field["content"])
	return "/upload/" + stored_name

def print_page_start(title):
	print("<!doctype html>")
	print('<html lang="en">')
	print("<head>")
	print('\t<meta charset="utf-8">')
	print(f"\t<title>{html.escape(title)}</title>")
	print('\t<link rel="stylesheet" href="../style.css">')
	print("\t<link href='https://unpkg.com/boxicons@2.1.4/css/boxicons.min.css' rel='stylesheet'>")
	print("</head>")
	print("<body>")

def print_page_end():
	print("</body>")
	print("</html>")

def print_form(message="", message_color=None):
	# Affiche le même formulaire pour les réponses GET et POST.
	print_page_start("Add Contact")
	print('\t<div class="phonebook">')
	print("\t\t<h1>Add Contact</h1>")

	if message:
		if message_color:
			print(f'\t\t<p style="color: {html.escape(message_color)}">{html.escape(message)}</p>')
		else:
			print(f'\t\t<p>{html.escape(message)}</p>')

	print('\t\t<form action="/cgi/add_contact.py" method="post" enctype="multipart/form-data">')
	print('\t\t\t<label for="image-upload" class="upload-box">')
	print('\t\t\t\t<i class="bx bx-image-add"></i>')
	print('\t\t\t\t<p>Upload image</p>')
	print('\t\t\t</label>')
	print('\t\t\t<input type="file" id="image-upload" name="image" accept="image/*" hidden>')
	print('\t\t\t<input type="text" name="name" placeholder="Name"><br>')
	print('\t\t\t<input type="text" name="phone" placeholder="Phone"><br>')
	print('\t\t\t<button class="save" type="submit">Save</button>')
	print("\t\t</form>")
	print('\t\t<a href="/index.html"><button class="links">Back</button></a>')
	print("\t</div>")
	print_page_end()


# Valide l'entrée, enregistre le contact, puis réaffiche le formulaire avec un retour.
def handle_post():
	data = parse_post_data()
	name = data["name"]
	phone = data["phone"]
	image_field = data["image"]

	if not name or not phone:
		print_form("Name and phone are required.", message_color="red")
		return

	contacts = load_contacts()
	contact_id = next_contact_id(contacts)
	image_path = store_uploaded_image(image_field, contact_id)

	contact = {
		"id": contact_id,
		"name": name,
		"phone": phone,
	}
	if image_path:
		contact["image"] = image_path

	contacts.append(contact)
	save_contacts(contacts)

	message = "Contact saved"
	if image_path:
		message += " with image"
	print_form(message, message_color="green")


# Affichage de la réponse
# ______________________________________
# Garde une réponse CGI simple : renvoyer toujours du HTML.
print("Content-Type: text/html")
print()

if os.environ.get("REQUEST_METHOD", "GET").upper() == "POST":
	handle_post()
else:
	print_form()
