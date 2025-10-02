#include "./../include/qsys.h"

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

