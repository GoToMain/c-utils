/*
 * Copyright (c) 2020-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _UTILS_RANDOM_H_
#define _UTILS_RANDOM_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the secure random bytes from the systems
 *
 * @param out Buffer to fill random bytes
 * @param len Number of random bytes to be filled to `out`
 * @return true if successful, false otherwise.
 */
bool get_random_bytes(uint8_t *out, size_t len);

#ifdef __cplusplus
}
#endif

#endif