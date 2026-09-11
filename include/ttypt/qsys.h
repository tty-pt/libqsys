#ifndef QSYS_H
#define QSYS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#ifdef __APPLE__
#define WEAK __attribute__((weak_import))
#else
#define WEAK __attribute__((weak))
#endif

#define UNUSED __attribute__((unused))

#define LOG(TYPE, ...) { \
	qsyslog(QLOG_##TYPE, "%s: ", __func__); \
	qsyslog(QLOG_##TYPE, __VA_ARGS__); \
}

#define WARN(...) LOG(WARNING, __VA_ARGS__)
#define ERR(...) LOG(ERR, __VA_ARGS__)

#define CBUG(cond, ...) \
	if (cond) { \
		ERR(__VA_ARGS__); \
		abort(); \
	}

enum {
    QLOG_ERR,
    QLOG_WARNING,
    QLOG_NOTICE,
    QLOG_INFO,
    QLOG_DEBUG
};

typedef void (*qsyslog_t)(int type, const char *fmt, ...);
void qsyslog_set(qsyslog_t logger);
extern qsyslog_t qsyslog;

/* Cross-platform syslog-like interface */
void qsys_openlog(const char *ident);
void qsys_syslog(int priority, const char *fmt, ...);
void qsys_closelog(void);

/* Portable dynamic-library loading. Thin POSIX dlopen/dlsym/dlclose/dlerror
 * wrappers on Unix; LoadLibraryA/GetProcAddress/FreeLibrary/GetLastError+
 * FormatMessageA on Windows. No suffix-appending or path canonicalization —
 * callers pass the exact path.
 *
 * qsys_dlopen always resolves eagerly + locally (POSIX RTLD_NOW|RTLD_LOCAL;
 * no equivalent distinction on Windows). `flags` is a bitmask of the
 * QSYS_RTLD_* constants below, layered on top of that default; ignored on
 * Windows (LoadLibraryA has no matching concept, so passing QSYS_RTLD_NODELETE
 * there is a silent no-op — same as the historical behavior of code that used
 * to macro dlopen()'s flags arg away entirely on Windows). */
#define QSYS_RTLD_NODELETE 0x01   /* POSIX RTLD_NODELETE; no-op on Windows */

void       *qsys_dlopen(const char *path, int flags);   /* NULL + qsys_dlerror() on failure */
void       *qsys_dlsym(void *handle, const char *symbol);   /* NULL if absent */
int         qsys_dlclose(void *handle);      /* 0 ok, mirrors dlclose's convention */
const char *qsys_dlerror(void);              /* last error string, or NULL */

#ifdef __cplusplus
}
#endif

#endif
