/*
 * Copyright (c) 2020-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <stdio.h>
#include <utils/utils.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LOGGER_NAME_MAXLEN 32

/**
 * @brief A printf() like method that will be used to write out log lines.
 *
 * @param fmt C printf() style format string. See man 3 printf
 *
 * @retval number of characters written to the log stream
 */
typedef int (*log_puts_fn_t)(const char *buf);

/**
 * @brief A callback function to be used with external loggers
 *
 * @param log_level A syslog style log level. See `enum log_levels`
 * @param file Relative path to file which produced the log message
 * @param line Line number in `file` which produced the log message
 * @param msg The log message
 */
typedef void (*log_callback_fn_t)(int log_level, const char *file,
				 unsigned long line, const char *msg);

typedef struct {
	char name[LOGGER_NAME_MAXLEN];
	const char *root_path;
	int log_level;
	uint32_t flags;
	FILE *file;
	log_puts_fn_t puts;
	log_callback_fn_t cb;
} logger_t;

enum log_levels {
	LOG_EMERG,
	LOG_ALERT,
	LOG_CRIT,
	LOG_ERR,
	LOG_WARNING,
	LOG_NOTICE,
	LOG_INFO,
	LOG_DEBUG,
	LOG_MAX_LEVEL
};

#define LOGGER_FLAG_NONE                   0x00000000
#define LOGGER_FLAG_NO_COLORS              0x00000001

/* Don't call this method directly. Use LOG_* macros below */
int __logger_log(logger_t *ctx, int log_level, const char *file,
		 unsigned long line, const char *fmt, ...);

int logger_init(logger_t *ctx, int log_level, const char *name,
		const char *root_path, log_puts_fn_t puts_fn, FILE *file,
		log_callback_fn_t cb, int flags);
void logger_get_default(logger_t *ctx);
void logger_set_default(logger_t *logger);
void logger_set_log_level(logger_t *ctx, int log_level);
void logger_set_put_fn(logger_t *ctx, log_puts_fn_t fn);
void logger_set_file(logger_t *ctx, FILE *f);
void logger_set_name(logger_t *ctx, const char *name);

#ifndef USE_CUSTOM_LOGGER
#define LOG_EM(...)    __logger_log(NULL, LOG_EMERG,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ALERT(...) __logger_log(NULL, LOG_ALERT,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_CRIT(...)  __logger_log(NULL, LOG_CRIT,   __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERR(...)   __logger_log(NULL, LOG_ERR,    __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INF(...)   __logger_log(NULL, LOG_INFO,   __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WRN(...)   __logger_log(NULL, LOG_WARNING,__FILE__, __LINE__, __VA_ARGS__)
#define LOG_NOT(...)   __logger_log(NULL, LOG_NOTICE, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DBG(...)   __logger_log(NULL, LOG_DEBUG,  __FILE__, __LINE__, __VA_ARGS__)
#endif

#define LOG_PRINT(...) __logger_log(NULL, LOG_INFO,   __FILE__, __LINE__, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* _LOGGER_H_ */
