#!/usr/bin/env python3

import json
import os
from urllib.parse import parse_qs


def contacts_file():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    return os.path.join(script_dir, "contacts.json")


def load_contacts():
    path = contacts_file()
    if not os.path.exists(path):
        return []

    try:
        with open(path, "r") as file:
            content = file.read().strip()
            if not content:
                return []
            contacts = json.loads(content)
            return contacts if isinstance(contacts, list) else []
    except (OSError, ValueError, TypeError):
        return []


def save_contacts(contacts):
    path = contacts_file()
    try:
        with open(path, "w") as file:
            json.dump(contacts, file, indent=2)
        return True
    except OSError:
        return False


query = parse_qs(os.environ.get("QUERY_STRING", ""))
contact_id = query.get("id", [None])[0]

print("Content-Type: application/json")
print()

if contact_id is None:
    print(json.dumps({"success": False, "error": "Missing id"}))
    raise SystemExit

try:
    index = int(contact_id)
except ValueError:
    print(json.dumps({"success": False, "error": "Invalid id"}))
    raise SystemExit

contacts = load_contacts()

if index < 0 or index >= len(contacts):
    print(json.dumps({"success": False, "error": "Contact not found"}))
    raise SystemExit

del contacts[index]

if not save_contacts(contacts):
    print(json.dumps({"success": False, "error": "Unable to save contacts"}))
    raise SystemExit

print(json.dumps({"success": True}))
