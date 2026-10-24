/*
 * Copyright (c) 2020-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <string.h>
#include <stdbool.h>

#include <utils/slab.h>

struct slab_unit {
	uint32_t leased;
	uint32_t canary;
	uint8_t data[];
};

int slab_init(slab_t *slab, size_t slab_size,
	      uint8_t *blob, size_t blob_size)
{
	slab->size = ROUND_UP(slab_size, sizeof(void *)) +
		     sizeof(struct slab_unit);

	if (slab->size > blob_size)
		return -1;

	slab->count = blob_size / slab->size;
	memset(blob, 0, blob_size);
	slab->blob = blob;

	return (int)slab->count;
}

int slab_alloc(slab_t *slab, void **block)
{
	size_t i;
	struct slab_unit *p;

	for (i = 0; i < slab->count; i++) {
		p = (struct slab_unit *)(slab->blob + (i * slab->size));
		if (!p->leased) {
			p->leased = true;
			p->canary = 0xdeadbeaf;
			*block = p->data;
			return 0;
		}
	}

	return -1;
}

int slab_free(slab_t *slab, void *block)
{
	ARG_UNUSED(slab);
	struct slab_unit *p = CONTAINER_OF(block, struct slab_unit, data);

	if (p->canary != 0xdeadbeaf)
		return -1;

	p->leased = false;

	return 0;
}
