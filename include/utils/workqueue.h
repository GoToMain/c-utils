/*
 * Copyright (c) 2021-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _UTILS_WORKQUEUE_H_
#define _UTILS_WORKQUEUE_H_

#include <pthread.h>
#include <utils/event.h>
#include <utils/queue.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*work_fn_t) (void *);
typedef void (*work_complete_fn_t) (void *work);

#define WORK_ERR   -1
#define WORK_DONE   0
#define WORK_YIELD  1

enum work_status {
	WQ_WORK_NEW,
	WQ_WORK_QUEUED,
	WQ_WORK_IN_PROGRESS,
	WQ_WORK_COMPLETE,
};

typedef struct {
	queue_node_t node;
	tick_t slice;
	enum work_status status;
	uint32_t requests;

	/* Filled by user */
	void *arg;
	work_fn_t work_fn;
	work_complete_fn_t complete_fn;
} work_t;

typedef struct {
	int id;
	int state;
	pthread_t thread;
	event_t event;
	void *wq;
} worker_t;

typedef struct {
	worker_t *workers;
	int num_workers;
	queue_t backlog;
	int backlog_count;
	pthread_mutex_t backlog_lock;
} workqueue_t;

/**
 * @brief Create new workqueue with at most N workers
 *
 * @param wq work queue context
 * @param num_workers Max number of workers
 *
 * @return 0 Success
 * @return -1 Failure
 */
int workqueue_create(workqueue_t *wq, int num_workers);

/**
 * @brief Add a work to work queue, next free worker thread will
 * pick it up
 *
 * @param wq work queue context
 * @param work Worker definition see work_t.
 *
 * @return 0 Success
 * @return -1 Failure
 */
int workqueue_add_work(workqueue_t *wq, work_t *work);

/**
 * @brief Get the current backlog of the work queue
 *
 * @param wq work queue context
 * @return Number of work items in the work queue.
 */
int workqueue_backlog_count(workqueue_t *wq);

/**
 * @brief Destroy work queue
 *
 * @param wq work queue context
 */
void workqueue_destroy(workqueue_t *wq);

/**
 * @brief Request to cancel a queued work
 *
 * @param wq work queue context
 * @param work previously added worker pointer
 */
void workqueue_cancel_work(workqueue_t *wq, work_t *work);

/**
 * @brief Check of a given work is completed or still running
 *
 * @param wq work queue context
 * @param work previously added worker pointer
 *
 * @return true if work is complete
 */
bool workqueue_work_is_complete(workqueue_t *wq, work_t *work);

#ifdef __cplusplus
}
#endif

#endif /* _UTILS_WORKQUEUE_H_ */
