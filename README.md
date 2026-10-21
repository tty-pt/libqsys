# libqsys

[![C99](https://img.shields.io/badge/C-C99-555?logo=c)](#)
[![BSD-2-Clause](https://img.shields.io/badge/License-BSD--2--Clause-blue)](#)
[![Sys utilities](https://img.shields.io/badge/sys-utilities-6B7280)](#)

> Minimal system utilities, pluggable logging sinks, compiler attribute macros, and cross-platform dynamic loading for plain C.

`libqsys` serves as the foundational substrate for the entire `tty-pt` stack, providing standardized logging, compiler compatibility abstractions, and POSIX/Windows dynamic module loading without heavy external runtime dependencies.

---

## Contents

- [Features](#features)
- [Quickstart: Logging & Custom Sinks](#quickstart-logging--custom-sinks)
- [API Reference](#api-reference)
- [Cross-Platform Substrate](#cross-platform-substrate)
- [Installation & Build](#installation--build)
- [License](#license)

---

## Features

- **Pluggable Logging Pipeline**: Standardized `LOG()`, `WARN()`, `ERR()`, and `CBUG()` macros routing through a thread-safe `qsyslog_t` callback sink.
- **Compiler Attribute Normalization**: Portable definitions for `WEAK`, `UNUSED`, `PACKED`, and format string checking across GCC, Clang, and MSVC.
- **Dynamic Module Loader**: Wrappers around `dlopen()` / `LoadLibrary()` with `QSYS_RTLD_NODELETE` semantics for hot-reloading safe plugin modules.
- **Zero Overhead**: Inlined macros and direct POSIX veneers compiling to minimal machine code.

---

## Quickstart: Logging & Custom Sinks

```c
#include <stdio.h>
#include <ttypt/qsys.h>

// Custom logging sink callback
static void custom_logger(int level, const char *msg, void *user) {
    (void)user;
    fprintf(stderr, "[CUSTOM-%d] %s\n", level, msg);
}

int main(void) {
    // Install custom log sink (or leave default stderr printer)
    qsyslog_set(custom_logger, NULL);

    LOG(QLOG_INFO, "System initializing...");
    WARN("Resource usage approaching limit: %d%%", 85);
    ERR("Failed to bind socket on port %d", 8080);

    return 0;
}
```

Compile and link:
```sh
cc app.c $(pkg-config --cflags --libs qsys)
```

---

## API Reference

### Logging Macros (`<ttypt/qsys.h>`)

| Macro / Function | Signature | Description |
| :--- | :--- | :--- |
| `LOG(LEVEL, ...)` | `LOG(int level, const char *fmt, ...)` | Log formatted message at specified severity |
| `WARN(...)` | `WARN(const char *fmt, ...)` | Emit formatted warning message to log sink |
| `ERR(...)` | `ERR(const char *fmt, ...)` | Emit formatted error message to log sink |
| `CBUG(...)` | `CBUG(cond, ...)` | Assert condition; emit diagnostic and abort on failure |
| `qsyslog_set` | `void qsyslog_set(qsyslog_t fn, void *ud)` | Install custom sink callback |

### Compiler Attribute Helpers

| Macro | GCC / Clang Equivalent | MSVC Equivalent | Purpose |
| :--- | :--- | :--- | :--- |
| `WEAK` | `__attribute__((weak))` | *noop* | Allow default symbol definitions overridden by modules |
| `UNUSED` | `__attribute__((unused))` | *noop* | Suppress unused variable/parameter warnings |
| `PACKED` | `__attribute__((packed))` | `#pragma pack` | Enforce byte-aligned struct memory layouts |

---

## Cross-Platform Substrate

`libqsys` abstracts platform differences across operating systems:
- **POSIX (Linux, macOS, OpenBSD)**: Uses `dlopen`, `dlsym`, `dlclose` with `RTLD_NOW | RTLD_LOCAL`, respecting `QSYS_RTLD_NODELETE` to keep symbols mapped during plugin teardowns.
- **Windows (MSVC / MinGW)**: Maps module operations to `LoadLibraryEx` and `GetProcAddress`.

---

## Installation & Build

### Prebuilt Packages
Prebuilt packages are distributed from tty.pt for Linux (APT / Alpine / Arch / Fedora-RHEL), macOS (Homebrew), Windows (winget / MSYS2), and OpenBSD. Follow the [installation instructions](https://github.com/tty-pt/ci/blob/main/docs/install.md) and use **libqsys** as the package name.

### Build from Source
```sh
make                  # builds lib/libqsys.so + bin/qsys_dlopen_test
make test             # run in-tree unit and dynamic loading tests
sudo make install     # install headers, library, and qsys.pc
```

## License

BSD 2-Clause License. Copyright (c) 2025, tty-pt. See `LICENSE`.
