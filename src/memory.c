/*
 * Copyright (c) 2021-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static void die_oom(const char *msg, size_t count, size_t size)
{
	fprintf(stderr, "fatal: %s() out of memory during alloc for %zu*%zu\n",
		msg, count, size);
	exit(-1);
}

void safe_free(void *p)
{
	if (p != NULL) {
		free(p);
	}
}

void *safe_malloc(size_t size)

{
	void *p;

	p = malloc(size);

	if (p == NULL)
		die_oom("malloc", 1, size);

	return p;
}

void *safe_calloc(size_t count, size_t size)
{
	void *p;

	p = calloc(count, size);

	if (p == NULL)
		die_oom("calloc", count, size);

	return p;
}

void *safe_strdup(const char *s)
{
	char *p;

	p = strdup(s);

	if (p == NULL)
		die_oom("strdup", 1, strlen(s));

	return p;
}

void *safe_realloc(void *data, size_t size)
{
	void *p;

	p = realloc(data, size);
	if (p == NULL)
		die_oom("realloc", 1, size);

	return p;
}

void *safe_realloc_zero(void *data, size_t old_size, size_t new_size)
{
	void *p;

	assert(old_size != new_size);

	p = safe_realloc(data, new_size);
	if (new_size > old_size) {
		memset((unsigned char *)p + old_size, 0, new_size - old_size);
	}
	return p;
}

