# *This project has been created as part of the 42 curriculum by althorel, echatela, agalleze.*

# Webserv

## Description

**Webserv** is a custom HTTP/1.1 web server written entirely in **C++98**, without
relying on any existing web server implementation.

The goal of the project is to understand how a web server works internally by
implementing the full request/response lifecycle: accepting TCP connections,
parsing HTTP requests, serving static files, running CGI scripts, handling many
clients at once through a single event loop, and producing accurate HTTP
responses.

All I/O — listening sockets, client connections and CGI pipes — is driven by a
**single `epoll` instance**. Sockets are non-blocking and the server never reads
or writes outside of an epoll readiness notification, so a slow or misbehaving
client can never block the others.

The server is configured through an **NGINX-inspired configuration file** that
supports multiple virtual servers, per-route rules, custom error pages, upload
directories, redirections, CGI execution and request-size limits.

To exercise every feature, the repository ships a small **phonebook
application** (`www/cgi-bin/`): a set of Python CGI scripts that add, list and
delete contacts, each contact carrying an uploaded image.

---

## Architecture

The server is built around a **reactor pattern** on top of `epoll`:

* `Reactor` owns the event loop and dispatches ready file descriptors to their
  handlers; any exception raised while handling one connection is caught and
  closes only that connection, never the server.
* `EventHandler` is the base class for everything registered in epoll:
  * `ListenHandler` — accepts new connections on a listening socket;
  * `ConnHandler` — reads a request, parses it, routes it, sends the response;
  * `CgiHandler` / `CgiInHandler` — stream a request body into a CGI process and
    read its output back, all non-blocking.

```
Client ─► Listen socket ─► epoll ─► ConnHandler ─► HTTP parser ─► Router
                                                                    │
                                        ┌───────────────┬───────────┤
                                        ▼               ▼           ▼
                                   Static file    CGI process   Error page
                                        └───────────────┴───────────┘
                                                        ▼
                                                  HTTP response ─► Client
```

---

## Configuration

The configuration file describes one or more `server` blocks. The demo
configuration (`Configuration.conf`) exposes two virtual servers on
`127.0.0.1:8080` and `127.0.0.1:8081`.

Supported directives:

| Directive               | Level    | Purpose                                        |
|-------------------------|----------|------------------------------------------------|
| `listen host:port`      | server   | Interface and port to bind                     |
| `root`                  | server / location | Filesystem root for the served files  |
| `client_max_body_size`  | server   | Maximum accepted request body (→ 413)          |
| `error_page code path`  | server   | Custom error page for a status code            |
| `location <prefix>`     | server   | Per-route rules                                |
| `methods`               | location | Allowed HTTP methods (→ 405 + `Allow`)         |
| `index`                 | location | Default file served for a directory            |
| `autoindex on\|off`     | location | Directory listing                              |
| `return code target`    | location | HTTP redirection                               |
| `cgi .ext interpreter`  | location | CGI interpreter for a file extension           |
| `upload on\|off`        | location | Enable multipart file upload                   |
| `upload_path`           | location | Storage directory for uploaded files           |

Example route:

```nginx
location /cgi {
    root ./cgi-bin;
    cgi .py /usr/bin/python3;
}
```

---

## Features

* HTTP/1.1 request parsing with accurate status codes
* Multiple virtual servers (`host:port` pairs)
* Static file serving with MIME-type detection
* Directory listing (autoindex)
* `GET`, `POST`, `DELETE` methods, with per-route allow-lists
* Configurable custom error pages, with sensible built-in fallbacks
* HTTP redirections (`return`)
* Multipart file uploads to a configurable directory
* CGI execution (Python) with chunked-body support and a 30 s timeout (→ 504)
* Configurable request-body size limit (→ 413)
* Single `epoll` event loop, non-blocking sockets, resilient to client failures
* Fully C++98, no external dependency

---

## Instructions

### Build

```bash
make
```

Produces the `./webserv` executable.

### Run

```bash
./webserv Configuration.conf
```

### Try it

Open a browser at `http://localhost:8080/`, or use `curl`:

```bash
curl http://localhost:8080/                 # home page
curl http://localhost:8080/folder/          # autoindex listing
curl -X DELETE http://localhost:8080/...     # delete a file
```

The phonebook demo lives under `/cgi/add_contact.py`, `/cgi/view_contact.py`
and `/cgi/delete_contact.py`.

---

## Project Structure

```
.
├── src/
│   ├── config/          # config lexer / parser / Config objects
│   ├── handlers/        # ListenHandler, ConnHandler, CgiHandler
│   ├── http_protocol/   # parser, request, response, router, static handler
│   └── reactor/         # Reactor + epoll wrapper
├── www/
│   ├── assets/          # images (uploaded by the CGI phonebook)
│   ├── cgi-bin/         # Python CGI scripts
│   ├── errors/          # custom error pages
│   ├── folder/          # autoindex demo
│   └── uploads/         # static multipart upload target
├── Configuration.conf
├── Makefile
└── README.md
```

---

## Resources

### Official documentation

* RFC 7230 – HTTP/1.1 Message Syntax and Routing
* RFC 7231 – HTTP/1.1 Semantics and Content
* RFC 3875 – The Common Gateway Interface (CGI/1.1)
* NGINX Documentation
* Linux `epoll(7)` manual
* Beej's Guide to Network Programming
* C++98 Reference

### Project documentation

* Internal project documentation (Notion):
  https://app.notion.com/p/Documentation-35faf02d8b30801db2e2fd19a5de99a4?source=copy_link

### AI Usage

Artificial Intelligence was used as a learning and productivity tool throughout
the project.

It was primarily used to:

* clarify HTTP protocol concepts;
* understand Linux networking APIs and `epoll`;
* review design ideas and architecture;
* explain RFC specifications;
* assist with debugging and interpreting compiler or runtime errors;
* improve project documentation and code comments;
* act as a debugging and learning aid for the Python CGI scripts.

All implementation decisions, architecture, development, testing, debugging and
validation were carried out by the project authors.
