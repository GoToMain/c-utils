/*
 * Copyright (c) 2020-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef _SERIAL_H_
#define _SERIAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <termios.h>

struct serial {
	int fd;
	struct termios new_termios;
	struct termios old_termios;
};

struct serial *serial_open(const char *device, int baud, const char *mode);
void serial_close(struct serial *ctx);
int serial_read(struct serial *ctx, unsigned char *buf, int size);
int serial_write(struct serial *ctx, unsigned char *buf, int size);
void serial_flush_rx(struct serial *ctx);
void serial_flush_tx(struct serial *ctx);
void serial_flush(struct serial *ctx);

/* line control methods */
int serial_get_dcd(struct serial *ctx);
int serial_get_rng(struct serial *ctx);
int serial_get_cts(struct serial *ctx);
int serial_get_dsr(struct serial *ctx);
void serial_assert_dtr(struct serial *ctx, int state);
void serial_assert_rts(struct serial *ctx, int state);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _SERIAL_H_ */
