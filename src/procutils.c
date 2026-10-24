/*
 * Copyright (c) 2020-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <utils/procutils.h>
#include <utils/memory.h>
#include <utils/file.h>

#define MAX_PROC_CMDLINE_PATH_SZ			40
#define MAX_PROC_CMDLINE_ARG_SZ				200

int read_pid(const char *file, int *pid)
{
	int pid_val, ret = 0;
	FILE *fd;

	if (file == NULL)
		return -1;

	fd = fopen(file, "r");
	if (fd == NULL)
		return -1;
	if (fscanf(fd, "%d", &pid_val) != 1) {
		printf("Failed to read PID from file %s\n", file);
		ret = -1;
	}
	fclose(fd);

	if (pid)
		*pid = pid_val;
	return ret;
}

int write_pid(const char *file)
{
	FILE *fd;
	char pid_str[32];

	if (file == NULL)
		return 1;

	fd = fopen(file, "w");
	if (fd == NULL) {
		perror("Error opening pid_file to write");
		return -1;
	}
	snprintf(pid_str, 32, "%d\n", getpid());
	fputs(pid_str, fd);
	fclose(fd);
	return 0;
}

int o_redirect(int mode, const char *file)
{
	int fd = -1;
	int has_err = 0;

	if (file != NULL) {
		fd = open(file, O_RDWR | O_CREAT | O_APPEND, 0644);
		if (fd == -1) {
			perror("opening log_file");
			return -1;
		}
	}

	if (mode == 3 && file == NULL) {
		close(fileno(stdin));
		close(fileno(stdout));
		close(fileno(stderr));
		return 0;
	}

	if (fd != -1 && mode & 0x01) {
		if (dup2(fd, fileno(stdout)) == -1) {
			perror("cannot redirect stdout to log_file");
			has_err = -1;
		}
	}

	if (fd != -1 && mode & 0x02) {
		if (dup2(fd, fileno(stderr)) == -1) {
			perror("cannot redirect stderr to log_file");
			has_err = -1;
		}
	}

	return has_err;
}

char *parse_proc_cmdline(unsigned pid, int pos)
{
	char buf[MAX_PROC_CMDLINE_PATH_SZ + 1];
	sprintf(buf, "/proc/%u/cmdline", pid);

	int c = 0, ndx = 0;
	FILE *fd = fopen(buf, "r");
	if (fd == NULL)
		return NULL;
	while (--pos > 0) {
		for (c = fgetc(fd); (c != EOF) && (c != 0); c = fgetc(fd));
	}
	if (feof(fd)) {
		fclose(fd);
		return NULL;
	}
	char *arg = safe_malloc((MAX_PROC_CMDLINE_ARG_SZ + 1) * sizeof(char));
	while ((c = fgetc(fd)) != EOF) {
		arg[ndx++] = c;
		if ((c == '\0') || (ndx >= MAX_PROC_CMDLINE_ARG_SZ))
			break;
	}
	arg[ndx] = 0;
	fclose(fd);
	if (ndx == 1) {
		safe_free(arg);
		return NULL;
	}
	return arg;
}

static int pid_cmp_func(const void *a, const void *b)
{
   return ( *(int*)a - *(int*)b );
}

static int pid_bsearch_func(const void *pkey, const void *pelem)
{
   return ( *(int*)pkey - *(int*)pelem );
}

static bool cmp_arg0_basename(const char *arg0, const char *basename)
{
	char *ptr = NULL;
	path_extract(arg0, NULL, &ptr);
	if (ptr == NULL)
		return false;

	bool found = strncmp(basename, ptr, MAX_PROC_CMDLINE_ARG_SZ) == 0;
	safe_free(ptr);
	return found;
}

unsigned pid_of(const char* exe_name, unsigned *pomit_arr, size_t arr_len)
{
	char *ptr;
	struct dirent *ent;
	if (!pomit_arr && arr_len)
		return 0;

	DIR *dir = opendir("/proc");
	if (dir == NULL)
		return 0;
	if (pomit_arr)
		qsort(pomit_arr, arr_len, sizeof(*pomit_arr), pid_cmp_func);

	while ((ent = readdir(dir)) != NULL) {
		if (strlen(ent->d_name) == 0)
			continue;
		unsigned pid = (unsigned)strtoul(ent->d_name, &ptr, 10);
		if (*ptr != '\0')
			continue;
		if (pomit_arr &&
			bsearch(&pid, pomit_arr, arr_len, sizeof(*pomit_arr), pid_bsearch_func))
			continue;
		char *arg0 = parse_proc_cmdline(pid, 1);
		if (arg0 == NULL)
			continue;
		if (!strcmp(arg0, "/proc/self/exe")) {
			safe_free(arg0);
			continue;
		}
		if (!cmp_arg0_basename(arg0, exe_name)) {
			safe_free(arg0);
			continue;
		}
		safe_free(arg0);
		closedir(dir);
		return pid;
	}
	closedir(dir);
	return 0;
}

unsigned any_pid_of(const char* exe_name)
{
	return pid_of(exe_name, NULL, 0);
}
