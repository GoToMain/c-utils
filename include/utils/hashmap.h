/*
 * Copyright (c) 2020-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _HASHMAP_H_
#define _HASHMAP_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hash_map_item_s hash_map_item_t;
typedef uint32_t hash_t;

struct hash_map_item_s {
	hash_map_item_t *next;
	hash_t hash;
	char *key;
	void *val;
};

typedef struct {
	hash_map_item_t **pool;
	size_t capacity;
	size_t count;
} hash_map_t;

typedef void (*hash_map_callback_t)(const char *key, void *val);

void hash_map_init(hash_map_t *map);
void hash_map_free(hash_map_t *map, hash_map_callback_t cb);
hash_t hash_map_insert(hash_map_t *map, const char *key, void *val);
void *hash_map_get(hash_map_t *map, const char *key, hash_t key_hash);
void *hash_map_delete(hash_map_t *map, const char *key, hash_t key_hash);

/* --- iterators --- */

typedef struct {
	size_t pos;
	void *item;
	hash_map_t *map;
} hash_map_iterator_t;

void hash_map_it_init(hash_map_iterator_t *it, hash_map_t *map);
int hash_map_it_next(hash_map_iterator_t *it, char **key, void **val);

#define HASH_MAP_FOREACH(map, key_ref, val_ref)                               \
		hash_map_iterator_t it;                                       \
		hash_map_it_init(&it, map);                                   \
		while (hash_map_it_next(&it, key_ref, (void **)val_ref) == 0)

#ifdef __cplusplus
}
#endif

#endif /* _HASHMAP_H_ */
