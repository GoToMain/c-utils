/*
 * Copyright (c) 2019-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <utils/utils.h>
#include <utils/serial.h>
#include <utils/channel.h>
#include <utils/fdutils.h>

struct channel_msgbuf {
	long mtype;		/* message type, must be > 0 */
	uint8_t mtext[512];	/* message data */
};

struct channel_msgq_s {
	int send_id;
	int send_msgid;
	int is_server;
	int recv_id;
	int recv_msgid;
};

struct channel_msgbuf send_buf, recv_buf;

int channel_msgq_send(void *data, uint8_t *buf, int len)
{
	int ret;
	struct channel_msgq_s *ctx = data;

	send_buf.mtype = ctx->send_id;
	memcpy(&send_buf.mtext, buf, len);
	ret = msgsnd(ctx->send_msgid, &send_buf, len, 0);
	if (ret < 0 && errno == EIDRM) {
		printf("Error: msgq was removed externally. Exiting..\n");
		exit(-1);
	}

	return len;
}

int channel_msgq_recv(void *data, uint8_t *buf, int max_len)
{
	int ret;
	struct channel_msgq_s *ctx = data;

	ret = msgrcv(ctx->recv_msgid, &recv_buf, max_len,
		     ctx->recv_id, MSG_NOERROR | IPC_NOWAIT);
	if (ret == 0 || (ret < 0 && errno == EAGAIN))
		return 0;
	if (ret < 0 && errno == EIDRM) {
		printf("Error: msgq was removed externally. Exiting..\n");
		exit(-1);
	}

	if (ret > 0)
		memcpy(buf, recv_buf.mtext, ret);

	return ret;
}

void channel_msgq_flush(void *data)
{
	int ret;
	uint8_t buf[128];

	while (1) {
		ret = channel_msgq_recv(data, buf, 128);
		if (ret <= 0)
			break;
	}
}

int channel_msgq_setup(void **data, struct channel *c)
{
	key_t key;
	struct channel_msgq_s *ctx;

	ctx = malloc(sizeof(struct channel_msgq_s));
	if (ctx == NULL) {
		printf("Failed at alloc for msgq channel\n");
		return -1;
	}
	ctx->is_server = c->is_server;
	ctx->send_id = ctx->is_server ? 13 : 17;
	key = ftok(c->device, ctx->send_id);
	ctx->send_msgid = msgget(key, 0666 | IPC_CREAT);
	if (ctx->send_msgid < 0) {
		printf("Error: failed to create send message queue %s. ",
		       c->device);
		perror("");
		free(ctx);
		return -1;
	}

	ctx->recv_id = ctx->is_server ? 17 : 13;
	key = ftok(c->device, ctx->recv_id);
	ctx->recv_msgid = msgget(key, 0666 | IPC_CREAT);
	if (ctx->recv_msgid < 0) {
		printf("Error: failed to create recv message queue %s. ",
		       c->device);
		perror("");
		free(ctx);
		return -1;
	}
	*data = (void *)ctx;

	return 0;
}

void channel_msgq_teardown(void *data)
{
	struct channel_msgq_s *ctx = data;

	if (data == NULL)
		return;
	if (ctx->is_server) {
		msgctl(ctx->send_msgid, IPC_RMID, NULL);
		msgctl(ctx->recv_msgid, IPC_RMID, NULL);
	}
	free(ctx);
}

int channel_uart_send(void *data, uint8_t *buf, int len)
{
	return serial_write(data, (unsigned char *)buf, len);
}

int channel_uart_recv(void *data, uint8_t *buf, int maxLen)
{
	return serial_read(data, (unsigned char *)buf, maxLen);
}

void channel_uart_flush(void *data)
{
	serial_flush(data);
}

int channel_uart_setup(void **data, struct channel *c)
{
	struct serial *ctx;

	ctx = serial_open(c->device, c->speed, "8N1");
	if (ctx == NULL) {
		printf("Error: failed to open device %s\n", c->device);
		return -1;
	}
	*data = (void *)ctx;
	return 0;
}

void channel_uart_teardown(void *data)
{
	serial_close(data);
}

struct channel_fifo {
	char *fifo0;
	char *fifo1;
	bool is_server;
	int rfd;
	int wfd;
};

int channel_fifo_send(void *data, uint8_t *buf, int len)
{
	struct channel_fifo *ctx = data;

	return (int)write_loop(ctx->wfd, buf, len);
}

int channel_fifo_recv(void *data, uint8_t *buf, int max_len)
{
	struct channel_fifo *ctx = data;

	return (int)read_loop(ctx->rfd, buf, max_len);
}

void channel_fifo_flush(void *data)
{
	struct channel_fifo *ctx = data;

	flush_fd(ctx->rfd);
}

int channel_fifo_setup(void **data, struct channel *c)
{
	int rc, len;
	char path[128];
	struct channel_fifo *ctx;

	len = strlen(c->device);
	if (len > 120)
		return -1;

	ctx = calloc(1, sizeof(struct channel_fifo));
	if (ctx == NULL)
		return -1;
	ctx->is_server = c->is_server;

	snprintf(path, 128, "%s-0", c->device);
	if (ctx->is_server) {
		unlink(path);
		rc = mkfifo(path, 0666);
		if (rc < 0) {
			perror("Error: mkfifo(0)");
			goto error;
		}
	}
	ctx->fifo0 = strdup(path);

	snprintf(path, 128, "%s-1", c->device);
	if (ctx->is_server) {
		unlink(path);
		rc = mkfifo(path, 0666);
		if (rc < 0) {
			perror("Error: mkfifo(1)");
			goto error;
		}
	}
	ctx->fifo1 = strdup(path);

	if (ctx->is_server) {
		ctx->rfd = open(ctx->fifo0, O_RDWR);
		if (ctx->rfd < 0) {
			perror("Error: open_fifo(0, r)");
			goto error;
		}
		ctx->wfd = open(ctx->fifo1, O_RDWR);
		if (ctx->wfd < 0) {
			perror("Error: open_fifo(1, w)");
			goto error;
		}
	} else {
		ctx->rfd = open(ctx->fifo1, O_RDWR);
		if (ctx->rfd < 0) {
			perror("Error: open_fifo(1, r)");
			goto error;
		}
		ctx->wfd = open(ctx->fifo0, O_RDWR);
		if (ctx->wfd < 0) {
			perror("Error: open_fifo(0, w)");
			goto error;
		}
	}

	rc = fcntl_setfl(ctx->rfd, O_NONBLOCK);
	if (rc < 0)
		goto error;

	rc = fcntl_setfl(ctx->wfd, O_NONBLOCK);
	if (rc < 0)
		goto error;

	*data = (void *)ctx;

	return 0;
error:
	if (ctx->rfd > 0)
		close(ctx->rfd);
	if (ctx->wfd > 0)
		close(ctx->rfd);
	if (ctx->fifo0) {
		unlink(ctx->fifo0);
		free(ctx->fifo0);
	}
	if (ctx->fifo1) {
		unlink(ctx->fifo1);
		free(ctx->fifo1);
	}
	free(ctx);
	return -1;
}

void channel_fifo_teardown(void *data)
{
	struct channel_fifo *ctx = data;

	close(ctx->rfd);
	close(ctx->wfd);
	if (ctx->is_server) {
		unlink(ctx->fifo0);
		unlink(ctx->fifo1);
	}
	free(ctx->fifo0);
	free(ctx->fifo1);
	free(ctx);
}

#include <utils/bus_server.h>
#include <utils/sockutils.h>

struct channel_unix_bus {
	int fd;
	bus_server_t *bus_server;
};

int channel_unix_bus_send(void *data, uint8_t *buf, int len)
{
	struct channel_unix_bus *ctx = data;

	return (int)write_loop(ctx->fd, buf, len);
}

int channel_unix_bus_recv(void *data, uint8_t *buf, int max_len)
{
	struct channel_unix_bus *ctx = data;

	return (int)read_loop(ctx->fd, buf, max_len);
}

void channel_unix_bus_flush(void *data)
{
	struct channel_unix_bus *ctx = data;

	flush_fd(ctx->fd);
}

int channel_unix_bus_setup(void **data, struct channel *c)
{
	int rc, len;
	struct channel_unix_bus *ctx;

	len = strlen(c->device);
	if (len > 120)
		return -1;

	ctx = calloc(1, sizeof(struct channel_unix_bus));
	if (ctx == NULL)
		return -1;

	if (access(c->device, F_OK) != 0) {
		/* start bus server */
		ctx->bus_server = calloc(1, sizeof(bus_server_t));
		if (ctx->bus_server == NULL)
			goto error;
		rc = bus_server_start(ctx->bus_server, 5, c->device);
		if (rc < 0)
			goto error;
	}

	rc = sock_unix_connect(c->device);
	if (rc < 0)
		goto error;

	ctx->fd = rc;

	fcntl_setfl(ctx->fd, O_NONBLOCK);

	*data = ctx;
	return 0;
error:
	if (ctx) {
		if (ctx->bus_server) {
			bus_server_stop(ctx->bus_server);
			free(ctx->bus_server);
		}
		free(ctx);
	}
	return -1;
}

void channel_unix_bus_teardown(void *data)
{
	struct channel_unix_bus *ctx = data;

	close(ctx->fd);
	if (ctx->bus_server) {
		bus_server_stop(ctx->bus_server);
		free(ctx->bus_server);
	}
	free(ctx);
}

struct channel_ops {
	channel_send_fn_t send;
	channel_receive_fn_t receive;
	channel_flush_fn_t flush;
	int (*setup)(void **data, struct channel *c);
	void (*teardown)(void *data);
};

struct channel_ops g_channel_ops[CHANNEL_TYPE_SENTINEL] = {
	[CHANNEL_TYPE_UART] = {
		.send = channel_uart_send,
		.receive = channel_uart_recv,
		.flush = channel_uart_flush,
		.setup = channel_uart_setup,
		.teardown = channel_uart_teardown
	},
	[CHANNEL_TYPE_MSGQ] = {
		.send = channel_msgq_send,
		.receive = channel_msgq_recv,
		.flush = channel_msgq_flush,
		.setup = channel_msgq_setup,
		.teardown = channel_msgq_teardown
	},
	[CHANNEL_TYPE_FIFO] = {
		.send = channel_fifo_send,
		.receive = channel_fifo_recv,
		.flush = channel_fifo_flush,
		.setup = channel_fifo_setup,
		.teardown = channel_fifo_teardown
	},
	[CHANNEL_TYPE_UNIX_BUS] = {
		.send = channel_unix_bus_send,
		.receive = channel_unix_bus_recv,
		.flush = channel_unix_bus_flush,
		.setup = channel_unix_bus_setup,
		.teardown = channel_unix_bus_teardown
	},
};

void channel_manager_init(struct channel_manager *ctx)
{
	hash_map_init(&ctx->channels);
}

enum channel_type channel_guess_type(const char *desc)
{
	if (strcmp("uart", desc) == 0 ||
	    strcmp("usart", desc) == 0 ||
	    strcmp("serial", desc) == 0)
		return CHANNEL_TYPE_UART;

	if (strcmp("msgq", desc) == 0 ||
	    strcmp("message_queue", desc) == 0)
	    	return CHANNEL_TYPE_MSGQ;

	if (strcmp("fifo", desc) == 0 ||
	    strcmp("pipe", desc) == 0)
		return CHANNEL_TYPE_FIFO;

	if (strcmp("unix_bus", desc) == 0)
		return CHANNEL_TYPE_UNIX_BUS;

	return CHANNEL_TYPE_ERR;
}

int channel_open(struct channel_manager *ctx, enum channel_type type, char *device,
		 int speed, int is_server)
{
	struct channel *c;

	if (type <= CHANNEL_TYPE_ERR || type >= CHANNEL_TYPE_SENTINEL)
		return CHANNEL_ERR_UNKNOWN_TYPE;

	if (!device)
		return CHANNEL_ERR_OPEN_FAILED;

	if (hash_map_get(&ctx->channels, device, 0) != NULL)
		return CHANNEL_ERR_ALREADY_OPEN;

	c = calloc(1, sizeof(struct channel));
	if (c == NULL)
		return CHANNEL_ERR_OOM;

	c->type = type;
	c->speed = speed;
	c->device = strdup(device);
	c->is_server = is_server;
	if (g_channel_ops[type].setup(&c->data, c)) {
		free(c->device);
		free(c);
		return CHANNEL_ERR_OPEN_FAILED;
	}

	if (g_channel_ops[type].flush) {
		g_channel_ops[type].flush(c->data);
	}
	ctx->open_channels++;
	c->id = ctx->open_channels;
	hash_map_insert(&ctx->channels, c->device, c);

	return CHANNEL_ERR_NONE;
}

int channel_get(struct channel_manager *ctx, const char *device,
		int *id, void **data,
		channel_send_fn_t *send,
		channel_receive_fn_t *receive,
		channel_flush_fn_t *flush)
{
	struct channel *c;

	c = hash_map_get(&ctx->channels, device, 0);
	if (c == NULL)
		return CHANNEL_ERR_NOT_OPEN;

	if(id != NULL)
		*id = c->id;
	if (data != NULL)
		*data = c->data;
	if (send != NULL)
		*send = g_channel_ops[c->type].send;
	if (receive != NULL)
		*receive = g_channel_ops[c->type].receive;
	if (flush != NULL)
		*flush = g_channel_ops[c->type].flush;

	return CHANNEL_ERR_NONE;
}

int channel_close(struct channel_manager *ctx, const char *device)
{
	struct channel *c;

	c = hash_map_get(&ctx->channels, device, 0);
	if (c == NULL)
		return CHANNEL_ERR_NOT_OPEN;

	g_channel_ops[c->type].teardown(c->data);
	hash_map_delete(&ctx->channels, device, 0);
	ctx->open_channels--;
	free(c->device);
	free(c);
	return CHANNEL_ERR_NONE;
}

void channel_hash_map_callback(const char *key, void *val)
{
	struct channel *c = val;
	ARG_UNUSED(key);

	free(c->device);
	free(c);
}

void channel_manager_teardown(struct channel_manager *ctx)
{
	char *device;
	struct channel *c;

	HASH_MAP_FOREACH(&ctx->channels, &device, &c) {
		channel_close(ctx, device);
	}

	hash_map_free(&ctx->channels, channel_hash_map_callback);
}
