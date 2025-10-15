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

#else /* POSIX ---------------------------------------------------------------- */

#include <syslog.h>

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

#endif /* _WIN32 */
