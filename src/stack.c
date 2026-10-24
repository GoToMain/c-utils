/*
 * Copyright (c) 2020-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <utils/stack.h>

void stack_init(stack_t *stack)
{
	slist_init(&stack->list);
}

void stack_push(stack_t *stack, stack_node_t *node)
{
	slist_appendleft(&stack->list, node);
}

int stack_pop(stack_t *stack, stack_node_t **node)
{
	return slist_popleft(&stack->list, node);
}

int stack_get_top(stack_t *stack, stack_node_t **node)
{
	if (stack->list.head == NULL)
		return -1;

	*node = stack->list.head;
	return 0;
}
