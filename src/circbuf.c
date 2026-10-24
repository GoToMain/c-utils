/*
 * Copyright (c) 2020-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include <utils/circbuf.h>

int __circbuf_pop(circbuf_t *circ_buf, void *elem, int read_only)
{
	int total;
	char *tail;

	total = circ_buf->push_count - circ_buf->pop_count;
	if (total < 0)
		total += (2 * circ_buf->size);

	if (total == 0)
		return -1; // Empty

	tail = (char *)circ_buf->buffer + ((circ_buf->pop_count % circ_buf->size)
			* circ_buf->element_size);

	if (elem)
		memcpy(elem, tail, circ_buf->element_size);

	if (!read_only) {
#ifdef CRICBUF_CLEAN_ON_POP
		memset(tail, 0, circ_buf->element_size);
#endif
		circ_buf->pop_count++;
		if (circ_buf->pop_count >= (2*circ_buf->size))
			circ_buf->pop_count = 0;
	}
	return 0;
}

int __circbuf_push(circbuf_t *circ_buf, void *elem)
{
	int total;
	char *head;

	total = circ_buf->push_count - circ_buf->pop_count;
	if (total < 0)
		total += (2 * circ_buf->size);

	if (total >=  circ_buf->size)
		return -1; // Full

	head = (char *)circ_buf->buffer + ( (circ_buf->push_count % circ_buf->size)
			* circ_buf->element_size );
	memcpy(head, elem, circ_buf->element_size);
	circ_buf->push_count++;
	if (circ_buf->push_count >= (2*circ_buf->size))
		circ_buf->push_count = 0;
	return 0;
}

int __circbuf_free_space(circbuf_t *circ_buf)
{
	int total;

	total = circ_buf->push_count - circ_buf->pop_count;
	if (total < 0)
		total += (2 * circ_buf->size);

	return circ_buf->size - total;
}
