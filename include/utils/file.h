/*
 * Copyright (c) 2020-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _UTIL_FILE_H_
#define _UTIL_FILE_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int read_binary_file(const char *path, uint8_t **buf, size_t *size);
int write_binary_file(const char *path, uint8_t *buf, size_t size);

int file_read_all(FILE *in, char **dataptr, size_t *sizeptr);
char *get_working_directory(void);
char *path_join(const char *p1, const char *p2);
bool file_exists(const char *path);
int is_regular_file(const char *path);
int path_extract(const char *path, char **dir_name, char **base_name);

char **fs_path_walk(const char *root_dir);
void fs_path_walk_free(char **files);
void fs_path_walk_free(char **files);
size_t file_size(FILE *fp);
bool dir_exists(const char *path);

#ifdef __cplusplus
}
#endif

#endif /* _UTIL_FILE_H_ */
