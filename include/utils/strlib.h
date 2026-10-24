/*
 * Copyright (c) 2020-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _UTIL_STRLIB_H_
#define _UTIL_STRLIB_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define STRING_ALLOCATED      0x00000001

typedef struct {
	uint32_t flags;
	char *buf;
	size_t len;
	size_t max_len;
} string_t;

/**
 * Description:
 * 	Defines a instance of string_t of specified length.
 *
 * Example:
 * 	GLOBAL_STRING_DEF(my_string, 100);
 */
#define STRING_DEF(str, length)	        \
	char str ## _buf[length+1]={0};	\
	string_t str = {		\
		.buf = str ## _buf,	\
		.len = 0,		\
		.max_len = length	\
	}

/**
 * String Ops Modes:
 *
 *  - "a" (append): bytes are appended to the existing contents of
 *    the string. If there is not enough space in the string, an
 *    error is returned.
 *
 *  - "af" (append, fill): same as "a" but fills as many bytes as
 *     possible into the string.
 *
 *  - "c" (create/copy): bytes are copied to the beginning of the
 *    string. If there is not enough space in the string, an
 *    error is returned.
 *
 *  - "cf" (copy, fill): same as "c" but fills as many bytes as
 *    possible into the string.
 */

/**
 * Description:
 * 	Do formated print operations (as with printf) on string_t.
 *
 * Note:
 * 	Use `mode` as defined above in "String Ops Modes" section.
 *
 * Example:
 * 	string_printf(&my_string, "c", "Age: %d\n", 28);
 * 	// now, my_string == "Age: 28\n"
 */
int string_printf(string_t *s, const char *mode, const char *fmt, ...);

/**
 * Description:
 * 	Copy contents of `buf` of length `len` into string `s`.
 *
 * Note:
 * 	Use `mode` as defined above in "String Ops Modes" section.
 *
 * Example:
 * 	string_copy(&my_string, "c", "Hello", 5);
 * 	// now, my_string == "Hello"
 * 	string_copy(&my_string, "a", " World", 6);
 * 	// now, my_string == "Hello World"
 */
int string_copy(string_t *s, const char *mode, const char *str, size_t len);

void string_create(string_t *s, const char *buf, size_t len);
void string_clone(string_t *dest, const string_t *src);
void string_destroy(string_t *s);
int string_resize(string_t *s, size_t new_len);
int string_merge(string_t *primary, string_t *secondary);

/* macro helpers */

#define str_flush(s)			\
	do {				\
		(s)->buf[0] = 0;	\
		(s)->len = 0;		\
	} while(0)
#define str_const_copy(s, str)		string_copy(s, "c", str, strlen(s))
#define str_const_append(s, str)	string_copy(s, "a", str, strlen(str))

#ifdef __cplusplus
}
#endif

#endif /* _UTIL_STRLIB_H_ */
