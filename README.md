# webserv

## Naming Conventions

This project follows the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html#Naming) for naming.
We adopt only its **naming** rules; language-feature recommendations that assume C++11 or later do not apply, since the project targets C++98.

The guiding principle: a name's casing should immediately tell the reader what kind of entity it is (type, variable, function, constant, macro) without having to look up its declaration.

| Category | Convention | Example |
|---|---|---|
| Class / struct / enum / typedef | `PascalCase` | `HttpRequest` |
| Function (member or free) | `PascalCase` | `ParseHeader()` |
| Accessor / mutator | like a variable | `port()` / `set_port()` |
| Local variable, function parameter | `snake_case` | `client_fd` |
| **Class** data member | `snake_case` + trailing `_` | `socket_fd_` |
| **Struct** data member | `snake_case` (no trailing `_`) | `socket_fd` |
| Constant (`const` / static storage) | `k` + `PascalCase` | `kMaxBodySize` |
| Enumerator | `k` + `PascalCase` | `kGet` |
| Namespace | `snake_case` | `webserv` |
| Macro (avoid unless necessary) | `ALL_CAPS` | `MAX_BUFFER_SIZE` |

### Notes

- **Class vs. struct members.** Class members carry a trailing underscore (`socket_fd_`); struct members do not (`socket_fd`). This makes the entity's kind visible at a glance.
- **Accessors/mutators** are the one function exception: they may be named like the variable they expose (`port()` and `set_port()`), rather than `PascalCase`.
- **File names.** Lowercase with underscores between words: `http_request.cpp`, `config_parser.hpp`.
  Deviation from Google (intentional): the subject requires the extensions `*.cpp`, `*.hpp`, `*.tpp`, `*.ipp`, so we use those instead of Google's `.cc` / `.h`.

### File Organization

- One `.hpp` / `.cpp` pair per class, named after the class in `snake_case` (`ConfigParser` → `config_parser.hpp` / `config_parser.cpp`).
- One public class per file.
- Template and inline definitions go in `.tpp` / `.ipp` files, `#include`d at the **end** of the corresponding `.hpp`.

### Reference

- Google C++ Style Guide — *Naming* section: <https://google.github.io/styleguide/cppguide.html#Naming>
