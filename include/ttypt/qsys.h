#ifndef QSYS_H
#define QSYS_H

/**
 * @file qsys.h
 * @brief Cross-platform system shims: logging, dl, env, strings.
 *
 * Portable wrappers (POSIX mirrors / Win32 emulation) for the low-level
 * facilities the site runtime needs on both platforms.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <time.h>

#ifdef __APPLE__
/** @brief Mark a symbol as weak-linked (macOS weak import). */
#define WEAK __attribute__((weak_import))
#else
/** @brief Mark a symbol as weak (ELF weak attribute). */
#define WEAK __attribute__((weak))
#endif

/** @brief Mark intentionally-unused symbols or parameters. */
#define UNUSED __attribute__((unused))

/**
 * @brief Log a formatted message tagged with the calling function's name.
 *
 * @param TYPE Severity selector; expands to a QLOG_* level.
 * @param ...  printf-style format and arguments.
 */
#define LOG(TYPE, ...) { \
	qsyslog(QLOG_##TYPE, "%s: ", __func__); \
	qsyslog(QLOG_##TYPE, __VA_ARGS__); \
}

/** @brief Log a warning-level message with the calling function's name. */
#define WARN(...) LOG(WARNING, __VA_ARGS__)

/** @brief Log an error-level message with the calling function's name. */
#define ERR(...) LOG(ERR, __VA_ARGS__)

/**
 * @brief Abort the process when a condition holds, after logging.
 *
 * @param cond Expression; when truthy the message is logged and abort() runs.
 * @param ...  printf-style message to log before aborting.
 */
#define CBUG(cond, ...) \
	if (cond) { \
		ERR(__VA_ARGS__); \
		abort(); \
	}

/**
 * @brief Log severity levels, syslog-style.
 */
enum {
    /** Error conditions. */
    QLOG_ERR,
    /** Warning conditions. */
    QLOG_WARNING,
    /** Notable events. */
    QLOG_NOTICE,
    /** Informational events. */
    QLOG_INFO,
    /** Debug messages. */
    QLOG_DEBUG
};

/**
 * @brief Logger callback type.
 *
 * @param type QLOG_* severity level.
 * @param fmt  printf-style format string.
 */
typedef void (*qsyslog_t)(int type, const char *fmt, ...);

/**
 * @brief Install the global logger callback.
 *
 * @param logger Logger callback to install.
 */
void qsyslog_set(qsyslog_t logger);

/** @brief Global logger callback; defaults to stderr output. */
extern qsyslog_t qsyslog;

/* Cross-platform syslog-like interface */

/**
 * @brief Open the system log with the given identifier.
 *
 * @param ident Identifier string, e.g. the program name.
 */
void qsys_openlog(const char *ident);

/**
 * @brief Write a message to the system log.
 *
 * @param priority Severity level.
 * @param fmt      printf-style format string.
 */
void qsys_syslog(int priority, const char *fmt, ...);

/**
 * @brief Close the system log.
 */
void qsys_closelog(void);

/**
 * @brief Cross-platform dynamic-library loading.
 *
 * Thin POSIX dlopen/dlsym/dlclose/dlerror wrappers on Unix; LoadLibraryA/
 * GetProcAddress/FreeLibrary/GetLastError+FormatMessageA on Windows. No
 * suffix-appending or path canonicalization — callers pass the exact path.
 */

/**
 * @brief dlopen flag: keep the library mapped after dlclose.
 *
 * qsys_dlopen always resolves eagerly and locally (POSIX RTLD_NOW|RTLD_LOCAL).
 * flags is a bitmask of QSYS_RTLD_* constants layered on that default;
 * ignored on Windows (a silent no-op).
 */
#define QSYS_RTLD_NODELETE 0x01   /* POSIX RTLD_NODELETE; no-op on Windows */

/**
 * @brief Open a dynamic library.
 *
 * @param path  Exact library path.
 * @param flags Bitmask of QSYS_RTLD_* flags, or 0.
 * @return Library handle, or NULL with qsys_dlerror() set on failure.
 */
void       *qsys_dlopen(const char *path, int flags);

/**
 * @brief Resolve a symbol in an opened library.
 *
 * @param handle Library handle from qsys_dlopen().
 * @param symbol Symbol name to resolve.
 * @return Symbol pointer, or NULL if absent.
 */
void       *qsys_dlsym(void *handle, const char *symbol);

/**
 * @brief Close an opened dynamic library.
 *
 * @param handle Library handle from qsys_dlopen().
 * @return 0 on success, mirroring dlclose()'s convention.
 */
int         qsys_dlclose(void *handle);

/**
 * @brief Return the last dynamic-library error string.
 *
 * @return Error string, or NULL when no error is pending.
 */
const char *qsys_dlerror(void);

/* Portable POSIX-shim helpers. On Unix these are thin wrappers around the
 * native call; on Windows the closest Win32/_CRT equivalent (mkstemps has no
 * direct equivalent and is emulated with _mktemp_s + _open(_O_EXCL)). */

/**
 * @brief Like mkstemps(3): create a temp file from a XXXXXX template.
 *
 * @param tmpl      Template ending in "XXXXXX" plus a suffix of suffixlen bytes.
 * @param suffixlen Number of suffix bytes after the XXXXXX.
 * @return File descriptor, or -1 with errno set.
 */
int qsys_mkstemps(char *tmpl, int suffixlen);

/**
 * @brief Flush a file descriptor to stable storage (fsync/_commit).
 *
 * @param fd File descriptor to flush.
 * @return 0 on success, -1 on error.
 */
int qsys_fsync(int fd);

/**
 * @brief Set an environment variable (setenv/_putenv_s).
 *
 * @param name      Variable name.
 * @param value     Value to assign.
 * @param overwrite Non-zero to replace an existing variable.
 * @return 0 on success, -1 with errno set on error.
 */
int qsys_setenv(const char *name, const char *value, int overwrite);

/**
 * @brief Unset an environment variable (unsetenv/_putenv_s).
 *
 * @param name Variable name.
 * @return 0 on success, -1 with errno set on error.
 */
int qsys_unsetenv(const char *name);

/**
 * @brief Bounded string copy like strlcpy(3).
 *
 * @param dst  Destination buffer.
 * @param src  Source string.
 * @param size Capacity of @p dst.
 * @return Length of @p src.
 */
size_t qsys_strlcpy(char *dst, const char *src, size_t size);

/**
 * @brief Thread-safe gmtime (gmtime_r/gmtime_s).
 *
 * @param timer  Time to convert.
 * @param result Output buffer for the broken-down time.
 * @return @p result on success, NULL on failure.
 */
struct tm *qsys_gmtime_r(const time_t *timer, struct tm *result);

#ifdef __cplusplus
}
#endif

#endif
