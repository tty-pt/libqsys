## 1.2.1

- **Portable dynamic-library loading**: `qsys_dlopen` / `qsys_dlsym` / `qsys_dlclose` / `qsys_dlerror` — thin POSIX `dlopen`/`dlsym`/`dlclose`/`dlerror` wrappers on Unix, `LoadLibraryA`/`GetProcAddress`/`FreeLibrary`/`GetLastError`+`FormatMessageA` on Windows. Always resolves eagerly and locally (`RTLD_NOW|RTLD_LOCAL`); `QSYS_RTLD_NODELETE` flag (no-op on Windows). No suffix-appending or path canonicalization.
- **Portable POSIX-shim helpers**: `qsys_mkstemps`, `qsys_fsync`, `qsys_setenv`/`qsys_unsetenv`, `qsys_strlcpy`, `qsys_gmtime_r` — native thin wrappers on Unix, emulated on Windows.
- New `qsys_dlopen_test` binary (`make test`) covering the loader API.

## [0.1.0] - 2025-10-19
- Windows compatibility
- Change release strategy
- Headers in ttypt folder

## [0.0.11] - 2025-10-02
- System Logging
- Debug Macros
- Error Logging
