# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

`webserv` is a C++98 HTTP/1.1 web server built around the Reactor pattern with non-blocking I/O via Linux epoll. It is a 42 school project.

## Build & Run

```bash
make          # build → produces ./webserv
make re       # clean rebuild
make clean    # remove .build/ object directory
make fclean   # clean + remove ./webserv binary
make run      # build and run with src/Configuration.conf

./webserv Configuration.conf   # run with a specific config file
```

Object files land in `.build/` mirroring the `src/` tree. All source globs are picked up automatically by the Makefile — no manual registration needed when adding files.

Compiler flags: `-Wall -Wextra -Werror -g -std=c++98`. The project **must** compile as C++98; do not use C++11 or later features.

There is no test runner; `src/http_protocol/main_test.cpp` is a standalone test harness that gets compiled in with the rest of the sources.

## Architecture

The server follows the **Reactor pattern**: a single-threaded event loop multiplexes all I/O through one epoll instance.

### Startup flow

```
main()
  → ConfigLexer::Tokenize()      # lex the .conf file into tokens
  → ConfigParser::Parse()        # produce std::vector<Config>
  → Reactor(configs)             # create listening sockets
  → Reactor::Run()               # event loop (blocks forever)
```

### Core classes

| Class | File | Role |
|---|---|---|
| `Reactor` | `src/reactor/reactor.{hpp,cpp}` | Owns epoll, all `EventHandler*`, and the main `Run()` loop. Calls `Dispatch()`, `CheckTimeouts()`, `CloseHandlers()` each iteration. |
| `EventHandler` | `src/reactor/event_handler.hpp` | Abstract base. Subclasses implement `HandleEvent(events)` and optionally `CheckTimeout(now)`, returning `kKeep` or `kClose`. |
| `Listen` | `src/reactor/listen.{hpp,cpp}` | Wraps a listening socket; on `EPOLLIN` accepts a new connection and pushes a `Connection*` into the Reactor. |
| `Connection` | `src/reactor/connection.{hpp,cpp}` | Wraps a client fd. State machine: `kReading → kWriting → kClose`. Owns `HttpParser`, `HttpRequest`, `Router`, `HttpResponse`. 60 s idle timeout. |
| `CgiHandler` | `src/reactor/cgi_handler.{hpp,cpp}` | Wraps the stdout pipe of a CGI child process. 10 s timeout. Calls `Connection::OnCgiDone()` when finished, or kills the child on timeout. |
| `Epoll` | `src/reactor/epoll.{hpp,cpp}` | Thin RAII wrapper around `epoll_create`/`epoll_ctl`/`epoll_wait`. MAX_EVENTS = 128. |
| `Config` | `src/config/config.{hpp,cpp}` | Holds parsed server block: `vector<ListenInfo>`, root path, `map<string, LocationConfig>`. |
| `ConfigLexer` | `src/config/config_lexer.{hpp,cpp}` | Reads a `.conf` file and produces `vector<Token>` (WORD, OPEN_BRACKET, CLOSE_BRACKET, SEMICOLON). |
| `ConfigParser` | `src/config/config_parser.{hpp,cpp}` | Recursive-descent parser; turns tokens into `vector<Config>` (one per `server {}` block). |
| `HttpParser` | `src/http_protocol/http_parser.{hpp,cpp}` | Accumulates raw bytes; signals completion when headers + body are fully received. |
| `HttpRequest` | `src/http_protocol/http_request.{hpp,cpp}` | Parsed HTTP request: method, path, version, headers, body. |
| `HttpResponse` | `src/http_protocol/http_response.{hpp,cpp}` | Builds the HTTP response string/byte vector. |
| `Router` | `src/http_protocol/router.{hpp,cpp}` | Maps a parsed request to a response: GET (file serving), POST, DELETE. Uses `Config` for root/location resolution. |

### Request lifecycle (per connection)

```
recv() bytes → HttpParser::Add()
  → complete? → HttpParser::ParseRequest(HttpRequest&)
    → Router::HandleRequest(HttpRequest&) → HttpResponse
      → HttpResponse::ToCharVector() → write_buf_
        → send() bytes until write_buf_ drained → kClose
```

CGI path: `Router` forks + execs the CGI binary, `CgiHandler` is registered with the Reactor to read its stdout asynchronously, then calls back `Connection::OnCgiDone()`.

### Config file syntax (nginx-like)

```
server {
    listen 8080;
    root ./www;

    location /images {
        root ./assets;
    }
}
```

Multiple `server {}` blocks are supported; each `listen` directive inside a block creates a separate `Listen` handler.

## Naming conventions

Strict Google C++ naming (names only — not language-feature guidance):

| Entity | Convention | Example |
|---|---|---|
| Class / struct / enum / typedef | `PascalCase` | `HttpRequest` |
| Function (member or free) | `PascalCase` | `ParseHeader()` |
| Accessor / mutator | like the variable | `port()` / `set_port()` |
| Local variable / parameter | `snake_case` | `client_fd` |
| Class data member | `snake_case_` (trailing `_`) | `socket_fd_` |
| Struct data member | `snake_case` (no trailing `_`) | `socket_fd` |
| Constant / static storage | `kPascalCase` | `kMaxBodySize` |
| Enumerator | `kPascalCase` | `kGet` |
| Namespace | `snake_case` | `webserv` |
| Macro | `ALL_CAPS` | `MAX_BUFFER_SIZE` |

File names: lowercase with underscores, `.cpp` / `.hpp` extensions. One public class per file pair.


Subject:

Mandatory part
Program Name webserv
Files to Submit Makefile, *.{h, hpp}, *.cpp, *.tpp, *.ipp,
configuration files
Makefile NAME, all, clean, fclean, re
Arguments [A configuration file]
External Function All functionality must be implemented in C++ 98.
execve, pipe, strerror, gai_strerror, errno, dup,
dup2, fork, socketpair, htons, htonl, ntohs, ntohl,
select, poll, epoll (epoll_create, epoll_ctl,
epoll_wait), kqueue (kqueue, kevent), socket,
accept, listen, send, recv, chdir, bind, connect,
getaddrinfo, freeaddrinfo, setsockopt, getsockname,
getprotobyname, fcntl, close, read, write, waitpid,
kill, signal, access, stat, open, opendir, readdir
and closedir.
Libft authorized n/a
Description An HTTP server in C++ 98
You must write an HTTP server in C++ 98.
Your executable should be executed as follows:
./webserv [configuration file]
Even though poll() is mentioned in the subject and evaluation sheet,
you can use any equivalent function such as select(), kqueue(), or
epoll().
6
Webserv This is when you finally understand why URLs start with HTTP
Please read the RFCs defining the HTTP protocol, and perform tests
with telnet and NGINX before starting this project.
Although you are not required to implement the entire RFCs, reading
it will help you develop the required features.
The HTTP 1.0 is suggested as a reference point, but not enforced.
7
Webserv This is when you finally understand why URLs start with HTTP
IV.1 Requirements
• Your program must use a configuration file, provided as an argument on the com-
mand line, or available in a default path.
• You cannot execve another web server.
• Your server must remain non-blocking at all times and properly handle client dis-
connections when necessary.
• It must be non-blocking and use only 1 poll() (or equivalent) for all the I/O
operations between the clients and the server (listen included).
• poll() (or equivalent) must monitor both reading and writing simultaneously.
• You must never do a read or a write operation without going through poll() (or
equivalent).
• Checking the value of errno to adjust the server behaviour is strictly forbidden
after performing a read or write operation.
• You are not required to use poll() (or an equivalent function) for regular disk files;
read() and write() on them do not require readiness notifications.
I/O that can wait for data (sockets, pipes/FIFOs, etc.) must be
non-blocking and driven by a single poll() (or equivalent). Calling
read/recv or write/send on these descriptors without prior readiness
will result in a grade of 0. Regular disk files are exempt.
• When using poll() or any equivalent call, you can use every associated macro or
helper function (e.g., FD_SET for select()).
• A request to your server should never hang indefinitely.
• Your server must be compatible with standard web browsers of your choice.
• NGINX may be used to compare headers and answer behaviours (pay attention to
differences between HTTP versions).
• Your HTTP response status codes must be accurate.
• Your server must have default error pages if none are provided.
• You can’t use fork for anything other than CGI (like PHP, or Python, and so forth).
• You must be able to serve a fully static website.
• Clients must be able to upload files.
• You need at least the GET, POST, and DELETE methods.
8
Webserv This is when you finally understand why URLs start with HTTP
• Stress test your server to ensure it remains available at all times.
• Your server must be able to listen to multiple ports to deliver different content (see
Configuration file).
We deliberately chose to offer only a subset of the HTTP RFC. In this
context, the virtual host feature is considered out of scope. But
you are allowed to implement it if you want.
9
Webserv This is when you finally understand why URLs start with HTTP
IV.2 For MacOS only
Since macOS handles write() differently from other Unix-based OSes,
you are allowed to use fcntl().
You must use file descriptors in non-blocking mode to achieve
behaviour similar to that of other Unix OSes.
However, you are allowed to use fcntl() only with the following
flags:
F_SETFL, O_NONBLOCK and, FD_CLOEXEC.
Any other flag is forbidden.
IV.3 Configuration file
You can take inspiration from the ’server’ section of the NGINX
configuration file.
In the configuration file, you should be able to:
• Define all the interface:port pairs on which your server will listen to (defining mul-
tiple websites served by your program).
• Set up default error pages.
• Set the maximum allowed size for client request bodies.
• Specify rules or configurations on a URL/route (no regex required here), for a
website, among the following:
◦ List of accepted HTTP methods for the route.
◦ HTTP redirection.
◦ Directory where the requested file should be located (e.g., if URL /kapouet
is rooted to /tmp/www, URL /kapouet/pouic/toto/pouet will search for
/tmp/www/pouic/toto/pouet).
◦ Enabling or disabling directory listing.
◦ Default file to serve when the requested resource is a directory.
◦ Uploading files from the clients to the server is authorized, and storage location
is provided.
10
Webserv This is when you finally understand why URLs start with HTTP
◦ Execution of CGI, based on file extension (for example .php). Here are some
specific remarks regarding CGIs:
∗ Do you wonder what a CGI is?
∗ Have a careful look at the environment variables involved in the web
server-CGI communication. The full request and arguments provided by
the client must be available to the CGI.
∗ Just remember that, for chunked requests, your server needs to un-chunk
them, the CGI will expect EOF as the end of the body.
∗ The same applies to the output of the CGI. If no content_length is
returned from the CGI, EOF will mark the end of the returned data.
∗ The CGI should be run in the correct directory for relative path file access.
∗ Your server should support at least one CGI (php-CGI, Python, and so
forth).
You must provide configuration files and default files to test and demonstrate that
every feature works during the evaluation.
You can have other rules or configuration information in your file (e.g., a server name
for a website if you plan to implement virtual hosts).
If you have a question about a specific behaviour, you can compare
your program’s behaviour with NGINX’s.
We have provided a small tester. Using it is not mandatory if
everything works fine with your browser and tests, but it can help
you find and fix bugs.
Resilience is key. Your server must remain operational at all times.
Do not test with only one program. Write your tests in a more
suitable language, such as Python or Golang, among others, even
in C or C++ if you prefer.
11
Chapter V
Readme Requirements
A README.md file must be provided at the root of your Git repository. Its purpose is
to allow anyone unfamiliar with the project (peers, staff, recruiters, etc.) to quickly
understand what the project is about, how to run it, and where to find more information
on the topic.
The README.md must include at least:
• The very first line must be italicized and read: This project has been created as part
of the 42 curriculum by <login1>[, <login2>[, <login3>[...]]].
• A “Description” section that clearly presents the project, including its goal and a
brief overview.
• An “Instructions” section containing any relevant information about compilation,
installation, and/or execution.
• A “Resources” section listing classic references related to the topic (documen-
tation, articles, tutorials, etc.), as well as a description of how AI was used —
specifying for which tasks and which parts of the project.
➠ Additional sections may be required depending on the project (e.g., usage
examples, feature list, technical choices, etc.).
Any required additions will be explicitly listed below.
Your README must be written in English.
12
Chapter VI
Bonus part
Here are some additional features you can implement:
• Support cookies and session management (provide simple examples).
• Handle multiple CGI types.
The bonus part will only be assessed if the mandatory part is fully
completed without issues. If you fail to meet all the mandatory
requirements, your bonus part will not be evaluated.
13
Chapter VII
Submission and peer-evaluation
Submit your assignment in your Git repository as usual. Only the content of your repos-
itory will be evaluated during the defense. Be sure to double-check the names of your
files to ensure they are correct.
During the evaluation, a brief modification of the project may occasionally be re-
quested. This could involve a minor behaviour change, a few lines of code to write or
rewrite, or an easy-to-add feature.
While this step may not be applicable to every project, you must be prepared for it
if it is mentioned in the evaluation guidelines.
This step is meant to verify your actual understanding of a specific part of the project.
The modification can be performed in any development environment you choose (e.g.,
your usual setup), and it should be feasible within a few minutes — unless a specific time
frame is defined as part of the evaluation.
You can, for example, be asked to make a small update to a function or script, modify a
display, or adjust a data structure to store new information, etc.
The details (scope, target, etc.) will be specified in the evaluation guidelines and may
vary from one evaluation to another for the same project
