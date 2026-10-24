/*
 * Copyright (c) 2021-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <utils/utils.h>
#include <utils/disjoint_set.h>

int disjoint_set_make(struct disjoint_set *set, int max_nodes)
{
	int i;

	if (max_nodes >= DISJOINT_SET_MAX)
		return -1;

	set->max_nodes = max_nodes;
	for (i = 0; i < max_nodes; i++) {
		set->parent[i] = i;
		set->rank[i] = 0;
	}

	return 0;
}

int disjoint_set_find(struct disjoint_set *set, int a)
{
	int tmp, root = a;

	/* find root */
	while (set->parent[root] != root)
		root = set->parent[root];

	/* path compression */
	while (set->parent[a] != root) {
		tmp = set->parent[a];
		set->parent[a] = root;
		a = tmp;
	}

	return root;
}

void disjoint_set_union(struct disjoint_set *set, int a, int b)
{
	a = disjoint_set_find(set, a);
	b = disjoint_set_find(set, b);

	if (a == b)
		return;

	if (set->rank[a] < set->rank[b]) {
		int tmp;
		tmp = a;
		a = b;
		b = tmp;
	}

	set->parent[b] = a;
	if (set->rank[a] == set->rank[b])
		set->rank[a] += 1;
}

int disjoint_set_num_roots(struct disjoint_set *set)
{
	int i, roots = 0;

	for (i = 0; i < set->max_nodes; i++) {
		if (set->parent[i] == i)
			roots++;
	}

	return roots;
}
