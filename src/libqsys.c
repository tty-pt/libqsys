#define _GNU_SOURCE
#include "./../include/ttypt/qsys.h"

#include <stdio.h>
#include <stdarg.h>

#define UNUSED __attribute__((unused))

static void
qsyslog_stderr(int type UNUSED, const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	vfprintf(stderr, fmt, va);
	va_end(va);
}

qsyslog_t qsyslog = qsyslog_stderr;

void
qsyslog_set(qsyslog_t logger)
{
	qsyslog = logger;
}



#ifdef _WIN32
#include <windows.h>
#include <stdlib.h>

static HANDLE hEventLog = NULL;
static char ident_buf[128] = "Application";

void qsys_openlog(const char *ident) {
    if (ident)
        strncpy(ident_buf, ident, sizeof(ident_buf) - 1);
    hEventLog = RegisterEventSourceA(NULL, ident_buf);
}

void qsys_syslog(int priority, const char *fmt, ...) {
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    WORD type = EVENTLOG_INFORMATION_TYPE;
    if (priority <= 3)
        type = EVENTLOG_ERROR_TYPE;
    else if (priority == 4)
        type = EVENTLOG_WARNING_TYPE;

    const char *strings[1] = { buf };

    if (hEventLog) {
        ReportEventA(hEventLog, type, 0, 0, NULL, 1, 0, strings, NULL);
    } else {
        fprintf(stderr, "%s: %s\n", ident_buf, buf);
    }
}

void qsys_closelog(void) {
    if (hEventLog) {
        DeregisterEventSource(hEventLog);
        hEventLog = NULL;
    }
}

/* qsys_dlopen/dlsym/dlclose/dlerror — Windows */

void *
qsys_dlopen(const char *path, int flags UNUSED)
{
    return (void *) LoadLibraryA(path);
}

void *
qsys_dlsym(void *handle, const char *symbol)
{
    void *sym = NULL;
    FARPROC fp = GetProcAddress((HMODULE) handle, symbol);
    memcpy(&sym, &fp, sizeof(sym));
    return sym;
}

int
qsys_dlclose(void *handle)
{
    return !FreeLibrary((HMODULE) handle);
}

static char dlerror_buf[256];

const char *
qsys_dlerror(void)
{
    DWORD err_code = GetLastError();
    if (err_code == 0)
        return NULL;
    memset(dlerror_buf, 0, sizeof(dlerror_buf));
    FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, err_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        dlerror_buf, sizeof(dlerror_buf), NULL);
    return dlerror_buf;
}

#else /* POSIX ---------------------------------------------------------------- */

#include <syslog.h>
#include <dlfcn.h>
#include <string.h>

void qsys_openlog(const char *ident) {
    openlog(ident, LOG_PID | LOG_CONS, LOG_USER);
}

void qsys_syslog(int priority, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsyslog(priority, fmt, ap);
    va_end(ap);
}

void qsys_closelog(void) {
    closelog();
}

/* qsys_dlopen/dlsym/dlclose/dlerror — POSIX */

void *
qsys_dlopen(const char *path, int flags)
{
    int posix_flags = RTLD_NOW | RTLD_LOCAL;
    if (flags & QSYS_RTLD_NODELETE)
        posix_flags |= RTLD_NODELETE;
    return dlopen(path, posix_flags);
}

void *
qsys_dlsym(void *handle, const char *symbol)
{
    return dlsym(handle, symbol);
}

int
qsys_dlclose(void *handle)
{
    return dlclose(handle);
}

const char *
qsys_dlerror(void)
{
    return dlerror();
}

#endif /* _WIN32 */
