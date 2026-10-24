/*
 * Copyright (c) 2021-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>

#ifndef _UTILS_EVENTS_H_
#define _UTILS_EVENTS_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int rfd;
	int wfd;
	bool initialized;
} event_t;

int event_init(event_t *e, bool active, bool blocking);
void event_cleanup(event_t *e);

bool event_set(event_t *e);
bool event_is_set(event_t *e);

#ifdef __cplusplus
}
#endif

#endif /* _UTILS_EVENTS_H_ */
