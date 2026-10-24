/*
 * Copyright (c) 2020-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _UTIL_PROCUTILS_H_
#define _UTIL_PROCUTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * read_pid() - Reads PID from `file`. The file's content must just be
 * be PID number, as written by a call to write_pid(). On success, copies
 * the PID read from file into `pid`.
 *
 * Returns:
 *  0 - Success
 * -1 - Failure
 */
int read_pid(const char *file, int *pid);

/*
 * write_pid() - Writes the calling process's PID to the file specified.
 *
 * Returns:
 *  0 - Success
 * -1 - Failure
 */
int write_pid(const char *file);

/*
 * o_redirect() - Redirects stdout/stderr to file specified.
 *
 * Params:
 *   mode - Bit mask.
 *           bit-0: STDOUT
 *           bit-1: STDERR
 *   file - Path to log file. if null, close() is called on the
 *          corresponding FDs (from mode above).
 *  Examples:
 *    o_redirect(1, "/tmp/my_log.txt"); // redirects stdout to "/tmp/my_log.txt"
 *    o_redirect(3, "/tmp/my_log.txt"); // Same as above; but redirects stdout and strerr
 *    o_redirect(3, NULL); // close both stdout and strerr. Same as `> /dev/null 1>&2`
 */
int o_redirect(int mode, const char *file);

/*
 * pid_of() - Return the pid of "exe_name"
 *
 * Params:
 *   exe_name  - Name of the executable of which pid should be returned.
 *   pomit_arr - Array of pid not to be considered.
 *   arr_len   - Size of pomit_arr
 * 
 *  * Returns:
 *  0   - Failed. No Process with exe_name found
 *  > 0 - PID of the process having name "exe_name"
 * 
 *  Examples:
 *    pid_of("test-utils", NULL, 0); // return the pid of the test
 * 
 *  Limitations:
 *    Currently, it doesn't work with scripts or kernel threads.
 */
unsigned pid_of(const char* exe_name, unsigned *pomit_arr, size_t arr_len);


/*
 * any_pid_of() - Return the pid of one of the process having "exe_name"
 *
 * Params:
 *   exe_name  - Name of the executable of which pid should be returned.
 * 
 *  * Returns:
 *  0   - Failed. No Process with exe_name found
 *  > 0 - PID of the process having name "exe_name"
 * 
 *  Examples:
 *    any_pid_of("test-utils"); // return the pid of the test
 * 
 *  Limitations:
 *    Currently, it doesn't work with scripts or kernel threads.
 */
unsigned any_pid_of(const char* exe_name);

/*
 * parse_proc_cmdline0() - Return the arg0 of the process having pid 'pid'
 *
 * Params:
 *   pid  - PID of the process for which arg0 will be returned
 */
char *parse_proc_cmdline(unsigned pid, int pos);


#ifdef __cplusplus
}
#endif

#endif /* _UTIL_PROCUTILS_H_ */
