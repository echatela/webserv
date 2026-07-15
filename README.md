# *This project has been created as part of the 42 curriculum by althorel, echatela, agalleze.*

# Webserv

## Description

**Webserv** is a custom HTTP/1.1 web server written entirely in **C++98**, without relying on existing web server implementations.

The goal of this project is to understand how web servers work internally by implementing the complete request/response lifecycle: accepting TCP connections, parsing HTTP requests, serving static files, executing CGI scripts, handling multiple clients simultaneously, and generating appropriate HTTP responses.

The server behavior is configured through a configuration file inspired by **NGINX**, allowing multiple virtual servers, custom routes, error pages, upload directories, CGI execution, and request limitations.

---

## Features

* HTTP/1.1 compliant request parsing
* Multiple virtual servers
* Configurable host and ports
* Static file serving
* Directory listing (autoindex)
* Custom error pages
* GET, POST and DELETE methods
* File uploads
* CGI support
* Request body parsing
* Configurable client body size
* Event-driven architecture using `epoll`
* Multiple simultaneous client connections
* Fully compatible with C++98

---

## Instructions

### Compilation

```bash
make
```

This generates the executable:

```text
./webserv
```

### Execution

Run the server with a configuration file:

```bash
./webserv Configuration.conf
```

### Testing

Open your browser and visit:

```
http://localhost:8080
```

You can also test the server using:

```bash
curl http://localhost:8080
```

---

## Project Architecture

```
Client
   │
   ▼
Socket
   │
   ▼
Epoll
   │
   ▼
Connection Handler
   │
   ▼
HTTP Parser
   │
   ▼
Request
   │
   ├─────────────► Static File
   │
   ├─────────────► CGI
   │
   └─────────────► Error Response
                 │
                 ▼
             HTTP Response
                 │
                 ▼
              Client
```

---

## Technical Choices

* Language: **C++98**
* Event-driven architecture
* Non-blocking sockets
* `epoll` for I/O multiplexing
* Configuration parser inspired by NGINX
* CGI execution using child processes
* Object-oriented design with separated responsibilities

---

## Project Structure

```
.
├── src/
  ├── config/
  ├── handlers/
  ├── http_protocol/
  ├── reactor/
├── www/
  ├── cgi-bin/
  ├── errors/
  ├── folder/
  ├── uploads/
├── Configuration.conf
├── Makefile
└── README.md
```

---

## Resources

### Official documentation

* RFC 7230 – HTTP/1.1 Message Syntax and Routing
* RFC 7231 – HTTP/1.1 Semantics and Content
* NGINX Documentation
* Linux `epoll(7)` Manual
* Beej's Guide to Network Programming
* C++98 Reference

### Project documentation

* Internal project documentation (Notion):
  https://app.notion.com/p/Documentation-35faf02d8b30801db2e2fd19a5de99a4?source=copy_link

### AI Usage

Artificial Intelligence was used as a learning and productivity tool throughout the project.

It was primarily used to:

* clarify HTTP protocol concepts;
* understand Linux networking APIs and `epoll`;
* review design ideas and architecture;
* explain RFC specifications;
* assist with debugging and interpreting compiler or runtime errors;
* improve project documentation and code comments.
* debugging and learning aid for the Python language used to develop the project's CGI scripts.

All implementation decisions, architecture, development, testing, debugging, and validation were carried out by the project authors.

---

