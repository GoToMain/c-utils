/*
 * Copyright (c) 2020-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _UTILS_MEMORY_H_
#define _UTILS_MEMORY_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Check p to be not NULL before calling safe_free()
 *
 * @param p pointer to be free-ed
 */
void safe_free(void *p);

/**
 * @brief safe_* variants of the standard alloc methods do a check on the
 * returned pointer and will call exit() if any of the returned NULL. The
 * safe_free() method will check if pointer is NULL before calling safe_free().
 */
void  safe_free(void *p);
void *safe_malloc(size_t size);
void *safe_calloc(size_t count, size_t size);
void *safe_realloc(void *data, size_t size);
void *safe_strdup(const char *s);
void *safe_realloc_zero(void *data, size_t old_size, size_t new_size);

#ifdef __cplusplus
}
#endif

#endif /* _UTILS_MEMORY_H_ */
