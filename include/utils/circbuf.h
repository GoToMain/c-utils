/*
 * Copyright (c) 2020-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _UTIL_CIRCBUF_H_
#define _UTIL_CIRCBUF_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** --- Internal methods and structures. DON'T USE --------------------------- */
typedef struct {
	void * const buffer;
	int push_count;
	int pop_count;
	int const size;
	int const element_size;
} circbuf_t;

#define __CIRCBUF_VAR_DEF(type, buf, sz)		\
	type buf ## _circbuf_data[sz];			\
	circbuf_t buf = {				\
		.buffer = buf ## _circbuf_data,		\
		.push_count = 0,			\
		.pop_count = 0,				\
		.size = sz,				\
		.element_size = sizeof(type)		\
	};

int __circbuf_push(circbuf_t *circbuf, void *elem);
int __circbuf_pop (circbuf_t *circbuf, void *elem, int read_only);
int __circbuf_free_space(circbuf_t *circbuf);
/* -------------------------------------------------------------------------- */

/**
 * Description:
 *   Zero slots in circular buffer after a pop.
 */
// #define CRICBUF_CLEAN_ON_POP

/**
 * Description:
 *   Defines a global circular buffer `buf` of a given type and size. The type
 *   can be native data types or user-defined data types.
 *
 * Usage:
 *   CIRCBUF_DEF(uint8_t, byte_buf, 13);
 *   CIRCBUF_DEF(struct foo, foo_buf, 10);
 */
#define CIRCBUF_DEF(type, buf, size)			\
	__CIRCBUF_VAR_DEF(type, buf, size)		\
	int buf ## _push_refd(type *pt)			\
	{						\
		return __circbuf_push(&buf, pt);	\
	}						\
	int buf ## _pop_refd(type *pt)			\
	{						\
		return __circbuf_pop(&buf, pt, 0);	\
	}						\
	int buf ## _peek_refd(type *pt)			\
	{						\
		return __circbuf_pop(&buf, pt, 1);	\
	}

/**
 * Description:
 *   Resets the circular buffer offsets to zero. Does not clean the newly freed
 *   slots.
 */
#define CIRCBUF_FLUSH(buf)				\
	do {						\
		buf.push_count = 0;			\
		buf.pop_count = 0;			\
	} while(0)

/**
 * Description:
 *   Pushes element pointed to by `elem` at the head of circular buffer `buf`.
 *   This is read-write method, occupancy count increases by one.
 *
 * Returns (int):
 *   0 - Success
 *  -1 - Out of space
 */
#define CIRCBUF_PUSH(buf, elem)             buf ## _push_refd(elem)

/**
 * Description:
 *   Copies the element at tail of circular buffer `buf` into location pointed
 *   by `elem`. This method is read-only, does not alter occupancy status.
 *
 * Returns (int):
 *   0 - Success
 *  -1 - Empty
 */
#define CIRCBUF_PEEK(buf, elem)             buf ## _peek_refd(elem)

/**
 * Description:
 *   Removes the element at tail from circular buffer `buf` and makes it
 *   available at `elem`. This is read-write method, occupancy count reduces
 *   by one.
 *
 * Returns (int):
 *   0 - Success
 *  -1 - Empty
 */
#define CIRCBUF_POP(buf, elem)              buf ## _pop_refd(elem)

/**
 * Description:
 *   Returns the number of free slots in the circular buffer `buf`.
 *
 * Returns (int):
 *   0..N - number of slots available.
 */
#define CIRCBUF_FS(buf)                     __circbuf_free_space(&buf)

#ifdef __cplusplus
}
#endif

#endif /* _UTIL_CIRCBUF_H_ */
