#ifndef QSYS_H
#define QSYS_H

#include <stddef.h>

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

#endif
