/*
 * Copyright (c) 2020-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef _UTILS_SLAB_H_
#define _UTILS_SLAB_H_

#include <stddef.h>
#include <stdint.h>

#include <utils/utils.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	uint8_t *blob;
	size_t size;
	size_t count;
} slab_t;

/**
 * @brief Initializes a resource pool of slabs (out of `blob`) that are
 * at-least `slab_size` bytes long.
 *
 * @return -1 on errors
 * @return number of slabs issuable from `blob` on success
 */
int slab_init(slab_t *slab, size_t slab_size,
	      uint8_t *blob, size_t blob_size);

/**
 * @brief Allocates a slab of memory from the resource pool held at
 * slab_t. The allocated block is at least `size` bytes large is
 * guaranteed to be aligned to a power the nearest power of 2.
 *
 * @return -1 on errors
 * @return  0 on success
 */
int slab_alloc(slab_t *slab, void **block);

/**
 * @brief Releases a slab that was previously allocated by a call
 * to slab_alloc(). This method can fail if the pointer passed did
 * not belong to the slab pool of slab_t.
 *
 * @return -1 on errors
 * @return  0 on success
 */
int slab_free(slab_t *slab, void *block);

#ifdef __cplusplus
}
#endif

#endif /* _UTILS_SLAB_H_ */
