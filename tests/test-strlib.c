/*
 * Copyright (c) 2020-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <utils/strlib.h>

#include "test.h"

STRING_DEF(test_str, 8);

void test_print_info_string(const char *msg, string_t *s)
{
	printf("\t[%s] s->len: %zu s->max_len: %zu s->buf: %s\n",
		msg, s->len, s->max_len, s->buf);
}

int test_check_str(string_t *s, const char *str)
{
	if (strcmp(s->buf, str) != 0)
		return -1;

	return 0;
}

int test_str_printf()
{
	int ret;

	ret = string_printf(&test_str, "c", "age: %d", 10);
	if (ret <= 0 || test_check_str(&test_str, "age: 10")) {
		printf("\tret: %d", ret);
		test_print_info_string("string_printf: create", &test_str);
		return -1;
	}

	ret = string_printf(&test_str, "a", " id: %d", 101);
	if (ret != -1) {
		printf("\tret: %d", ret);
		test_print_info_string("string_printf: create", &test_str);
		return -1;
	}

	return 0;
}

int test_str_copy()
{
	int ret;

	ret = string_copy(&test_str, "c", "1234567890", 10);
	if (ret != -1) {
		test_print_info_string("Create excess", &test_str);
		return -1;
	}

	ret = string_copy(&test_str, "cf", "1234567890", 10);
	if (ret != 8) {
		test_print_info_string("Create fill", &test_str);
		return -1;
	}

	ret = string_copy(&test_str, "c", "1234567", 7);
	if (ret != 7) {
		test_print_info_string("Create", &test_str);
		return -1;
	}

	ret = string_copy(&test_str, "a", "1", 1);
	if (ret != 1) {
		test_print_info_string("Append", &test_str);
		return -1;
	}

	ret = string_copy(&test_str, "a", "123", 3);
	if (ret != -1) {
		test_print_info_string("Append after full", &test_str);
		return -1;
	}

	return 0;
}

TEST_DEF(strlib)
{
	TEST_MOD_INIT();

	TEST_MOD_EXEC(test_str_copy());
	TEST_MOD_EXEC(test_str_printf());

	TEST_MOD_REPORT();
}
