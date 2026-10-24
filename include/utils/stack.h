/*
 * Copyright (c) 2020-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef _UTILS_STACK_H_
#define _UTILS_STACK_H_

#include <utils/list.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	slist_t list;
} stack_t;

#define stack_node_t snode_t

void stack_init(stack_t *stack);
void stack_push(stack_t *stack, stack_node_t *node);
int stack_pop(stack_t *stack, stack_node_t **node);
int stack_get_top(stack_t *stack, stack_node_t **node);

#ifdef __cplusplus
}
#endif

#endif /* _UTILS_STACK_H_ */
