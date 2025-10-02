#ifndef QSYS_H
#define QSYS_H

#include <syslog.h>
#include <stddef.h>

#define UNUSED __attribute__((unused))

#define LOG(TYPE, ...) { \
	qsyslog(LOG_##TYPE, "%s: ", __func__); \
	qsyslog(LOG_##TYPE, __VA_ARGS__); \
}

#define WARN(...) LOG(WARNING, __VA_ARGS__)
#define ERR(...) LOG(ERR, __VA_ARGS__)

#define CBUG(cond, ...) \
	if (cond) { \
		ERR(__VA_ARGS__); \
		abort(); \
	}

typedef void (*qsyslog_t)(int type, const char *fmt, ...);
void qsyslog_set(qsyslog_t logger);
extern qsyslog_t qsyslog;

#endif
