#!/usr/bin/env python3

import html
import json
import os

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

def print_form(message=""):
	print_page_start("Add Contact")
	print('\t<div class="phonebook">')
	print("\t\t<h1>Add Contact</h1>")

	if message:
		print(f'\t\t<p>{html.escape(message)}</p>')

	print('\t\t<label for="image-upload" class="upload-box">')
	print('\t\t\t<i class="bx bx-image-add"></i>')
	print('\t\t\t<p>Upload image</p>')
	print("\t\t</label>")
	print('\t\t<input type="file" id="image-upload" accept="image/*" hidden>')
	print('\t\t<form action="/cgi/add_contact.py" method="post">')
	print('\t\t\t<input type="text" name="name" placeholder="Name"><br>')
	print('\t\t\t<input type="text" name="phone" placeholder="Phone"><br>')
	print('\t\t\t<button class="save" type="submit">Save</button>')
	print("\t\t</form>")
	print('\t\t<a href="/index.html"><button class="links">Back</button></a>')
	print("\t</div>")
	print_page_end()


# Affichage de la response
# ______________________________________
print("Content-Type: text/html")
print()
print_form()
