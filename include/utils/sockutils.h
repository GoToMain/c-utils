/*
 * Copyright (c) 2021-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _UTILS_SOCKUTILS_H_
#define _UTILS_SOCKUTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

// Provide macros to old function names so as to not break existing
// users. TODO: remove it in next release.
#define unix_socket_listen sock_unix_listen
#define unix_socket_connect sock_unix_connect

int sock_unix_listen(const char *path, int max_clients);
int sock_unix_connect(const char *path);

int sock_stream_connect(const char *host, int port);
int sock_stream_listen(int port, int nr_clients);
int sock_wait(int listening_socket_fd);
int sock_shutdown(int listening_socket_fd);

#ifdef __cplusplus
}
#endif

#endif /* _UTILS_SOCKUTILS_H_ */