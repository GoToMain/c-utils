/*
 * Copyright (c) 2021-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _UTILS_BUS_SERVER_H_
#define _UTILS_BUS_SERVER_H_

#include <pthread.h>
#include <utils/workqueue.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int fd;
	int max_clients;
	char *path;
	workqueue_t wq;
	pthread_t thread;
	work_t *work;
} bus_server_t;

int bus_server_start(bus_server_t *s, int max_clients, const char *path);
void bus_server_stop(bus_server_t *s);

#ifdef __cplusplus
}
#endif

#endif /* _UTILS_BUS_SERVER_H_ */