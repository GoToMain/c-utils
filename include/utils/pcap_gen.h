/*
 * Copyright (c) 2024-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _UTILS_PCAP_GEN_H_
#define _UTILS_PCAP_GEN_H_

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	FILE *file;
	size_t offset;
	void *cache;
	size_t num_packets;
} pcap_t;

pcap_t *pcap_start(char *path, uint32_t max_packet_size, uint32_t link_type);
int pcap_add(pcap_t *cap, uint8_t *capture_data, uint32_t length);
int pcap_stop(pcap_t *cap);

#ifdef __cplusplus
}
#endif

#endif /* _UTILS_PCAP_GEN_H_ */
