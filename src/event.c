/*
 * Copyright (c) 2021-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <errno.h>
#include <unistd.h>

#include <sys/fcntl.h>

#include <utils/fdutils.h>
#include <utils/event.h>

int event_init(event_t *e, bool active, bool blocking)
{
	int fds[2];
	int ret;

	if (pipe(fds) < 0)
		return -1;

	if (!blocking) {
		ret = fcntl_setfl(fds[0], O_NONBLOCK);
		if (ret < 0)
			goto error;
		ret = fcntl_setfl(fds[1], O_NONBLOCK);
		if (ret < 0)
			goto error;
	}

	e->rfd = fds[0];
	e->wfd = fds[1];
	e->initialized = true;

	if (active)
		event_set(e);

	return 0;
error:
	close(fds[0]);
	close(fds[1]);
	return -1;
}

void event_cleanup(event_t *e)
{
	if (!e->initialized)
		return;

	close(e->rfd);
	close(e->wfd);
	e->wfd = e->rfd = -1;
	e->initialized = false;
}

bool event_set(event_t *e)
{
	static const uint64_t value = 1;
	ssize_t ret;

	if (!e->initialized)
		return false;

	ret = write_loop(e->wfd, &value, sizeof(value));
	if (ret < 0)
		return false;

	return true;
}

bool event_is_set(event_t *e)
{
	if (!e->initialized)
		return 0;

	return flush_fd(e->rfd);
}
