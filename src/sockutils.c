/*
 * Copyright (c) 2021-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <utils/sockutils.h>

int sock_unix_listen(const char *path, int max_clients)
{
	int rc, fd;
	socklen_t len;
	struct sockaddr_un sock_serv;

	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd < 0)
		return -1;

	sock_serv.sun_family = AF_UNIX;
	strcpy(sock_serv.sun_path, path);
	len = sizeof(sock_serv.sun_family) + strlen(path) + 1;
	unlink(path);

	rc = bind(fd, (const struct sockaddr *)&sock_serv, len);
	if (rc < 0)
		return -1;

	rc = listen(fd, max_clients);
	if (rc < 0)
		return -1;

	return fd;
}

int sock_unix_connect(const char *path)
{
	int rc, fd;
	socklen_t len;
	struct sockaddr_un serv_addr;

	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd < 0)
		return -1;

	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, path);
	len = sizeof(serv_addr.sun_family) + strlen(serv_addr.sun_path) + 1;

	rc = connect(fd, (struct sockaddr *)&serv_addr, len);
	if (rc != 0)
		return -1;

	return fd;
}

int sock_stream_connect(const char *host, int port)
{
	int fd;
	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = htons(port)
	};

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Socket creation failed!");
		return -1;
	}
	if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
		perror("Invalid address / Address not supported!");
		return -1;
	}
	if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("connect failed");
		return -1;
	}
	return fd;
}

int sock_stream_listen(int port, int nr_clients)
{
	int fd, opt = 1;
	struct sockaddr_in address = {
		.sin_family = AF_INET,
		.sin_addr = {
			.s_addr = INADDR_ANY,
		},
		.sin_port = htons(port),
	};

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
		return -1;
	}
	if (setsockopt(fd, SOL_SOCKET,
				SO_REUSEADDR /*| SO_REUSEPORT*/, &opt, sizeof(opt))) {
		perror("setsockopt failed");
		return -1;
	}
	if (bind(fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
		perror("bind failed");
		return -1;
	}
	if (listen(fd, nr_clients) < 0) {
		perror("listen failed");
		return -1;
	}
	return fd;
}


int sock_wait(int listening_socket_fd)
{
	int client_fd;
	struct sockaddr_in addr;
	size_t addr_len = sizeof(addr);

	client_fd = accept(listening_socket_fd, (struct sockaddr *)&addr,
			(socklen_t *)&addr_len);
	if (client_fd < 0) {
		perror("accept failed");
		return -1;
	}
	return client_fd;
}

int sock_shutdown(int listening_socket_fd)
{
	return shutdown(listening_socket_fd, SHUT_RDWR);
}
