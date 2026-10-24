/*
 * Copyright (c) 2020-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include <utils/strlib.h>
#include <utils/memory.h>

void string_create(string_t *s, const char *buf, size_t len)
{
	s->buf = safe_malloc(sizeof(char) * (len + 1));
	s->len = 0;
	s->max_len = len;
	s->flags = STRING_ALLOCATED;
	if (buf != NULL) {
		memcpy(s->buf, buf, len);
		s->len = len;
	}
	s->buf[len] = '\0'; /* Make sure there is always a terminating null */
}

void string_clone(string_t *dest, const string_t *src)
{
	string_create(dest, src->buf, src->len);
}

int string_resize(string_t *s, size_t new_len)
{
	if (!(s->flags & STRING_ALLOCATED))
		return -1;

	s->buf = safe_realloc(s->buf, new_len + 1);
	s->max_len = new_len;
	if (s->len > new_len) {
		s->len = new_len;
	}
	s->buf[new_len] = '\0';
	return 0;
}

void string_destroy(string_t *s)
{
	if (s->flags & STRING_ALLOCATED)
		safe_free(s->buf);
	s->buf = NULL;
	s->flags = 0;
	s->len = 0;
	s->max_len = 0;
}

int string_printf(string_t *s, const char *mode, const char *fmt, ...)
{
	int ret, start;
	va_list (args);

	start = (mode[0] == 'a') ? s->len : 0;
	va_start(args, fmt);
	ret = vsnprintf(s->buf + start, s->max_len - start, fmt, args);
	va_end(args);

	if ((start + ret) >= (int)s->max_len) {
		return -1;
	}

	s->len = start + ret;
	s->buf[s->len] = 0;
	return ret;
}

int string_copy(string_t *s, const char *mode, const char *str, size_t len)
{
	int start;

	start = (mode[0] == 'a') ? s->len : 0;
	if ((start + len) > s->max_len) {
		if (mode[1] != 'f') {
			return -1;
		}
		len = s->max_len - start;
	}
	memcpy(s->buf + start, str, len);
	s->len = start + len;
	s->buf[s->len] = '\0';
	return len;
}

int string_merge(string_t *primary, string_t *secondary)
{
	size_t target;

	target = primary->len + secondary->len;
	if (target > primary->max_len) {
		if (string_resize(primary, target) != 0) {
			return -1;
		}
	}
	string_copy(primary, "a", secondary->buf, secondary->len);
	string_destroy(secondary);
	return 0;
}
