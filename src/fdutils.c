/*
 * Copyright (c) 2021-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include <sys/fcntl.h>

#include <utils/fdutils.h>

int fcntl_setfl(int fd, int flag)
{
	int flags;

	flags = fcntl(fd, F_GETFL);
	if (flags == -1)
		return -1;

	if (fcntl(fd, F_SETFL, flags | flag) == -1)
		return -1;

	return 0;
}

ssize_t read_loop(int fd, void *buf, size_t max_len)
{
	ssize_t ret;

	do {
		ret = read(fd, buf, max_len);
	} while (ret == -1 && errno == EINTR);

	if (ret < 0) {
		if (errno != EAGAIN)
			return -1;
		ret = 0;
	}

	return ret;
}

ssize_t write_loop(int fd, const void *buf, size_t len)
{
	ssize_t ret;

	do {
		ret = write(fd, buf, len);
	} while (ret < 0 && errno == EINTR);

	if (ret < 0) {
		if (errno != EAGAIN)
			return -1;
		ret = 0;
	}

	return ret;
}

int flush_fd(int fd)
{
	int ret = 0;
	ssize_t len;
	char buf[64];

	do {
		len = read(fd, buf, sizeof(buf));
		ret |= (len > 0);
	} while ((len == -1 && errno == EINTR) || len == sizeof(buf));

	return ret;
}
