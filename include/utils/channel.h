/*
 * Copyright (c) 2019-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <utils/hashmap.h>

#ifdef __cplusplus
extern "C" {
#endif

enum channel_errors {
	CHANNEL_ERR_NONE,
	CHANNEL_ERR_ALREADY_OPEN,
	CHANNEL_ERR_OPEN_FAILED,
	CHANNEL_ERR_UNKNOWN_TYPE,
	CHANNEL_ERR_NOT_OPEN,
	CHANNEL_ERR_OOM,
};

enum channel_type {
	CHANNEL_TYPE_ERR,
	CHANNEL_TYPE_UART,
	CHANNEL_TYPE_MSGQ,
	CHANNEL_TYPE_FIFO,
	CHANNEL_TYPE_UNIX_BUS,
	CHANNEL_TYPE_SENTINEL
};

/**
 * @brief pointer to function that copies received bytes into buffer
 * @param data for use by underlying layers. channel_s::data is passed
 * @param buf byte array copy incoming data
 * @param len sizeof `buf`. Can copy utmost `len` bytes into `buf`
 *
 * @retval +ve: number of bytes copied on to `bug`. Must be <= `len`
 * @retval -ve on errors
 */
typedef int (*channel_receive_fn_t)(void *data, uint8_t *buf, int maxlen);

/**
 * @brief pointer to function that sends byte array into some channel
 * @param data for use by underlying layers. channel_s::data is passed
 * @param buf byte array to be sent
 * @param len number of bytes in `buf`
 *
 * @retval +ve: number of bytes sent. must be <= `len`
 * @retval -ve on errors
 */
typedef int (*channel_send_fn_t)(void *data, uint8_t *buf, int len);

/**
 * @brief pointer to function that drops all bytes in TX/RX fifo
 * @param data for use by underlying layers. channel_s::data is passed
 */
typedef void (*channel_flush_fn_t)(void *data);

struct channel {
	int id;
	int speed;
	char *device;
	int is_server;
	enum channel_type type;
	void *data;
};

struct channel_manager {
	int open_channels;
	hash_map_t channels;
};

void channel_manager_init(struct channel_manager *ctx);

enum channel_type channel_guess_type(const char *desc);

int channel_open(struct channel_manager *ctx, enum channel_type type, char *device,
		 int speed, int is_server);

int channel_get(struct channel_manager *ctx, const char *device,
		int *id, void **data,
		channel_send_fn_t *send,
		channel_receive_fn_t *recv,
		channel_flush_fn_t *flush);

int channel_close(struct channel_manager *ctx, const char *device);

void channel_manager_teardown(struct channel_manager *ctx);

#ifdef __cplusplus
}
#endif

#endif /* _CHANNEL_H_ */
