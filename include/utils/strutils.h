/*
 * Copyright (c) 2020-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _UTIL_STRUTIL_H_
#define _UTIL_STRUTIL_H_

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * atohstr() - Converts an array of bytes to its hexadecimal string
 * representation.
 *
 * Usage:
 *   uint8_t arr[4] = { 0xca, 0xfe, 0xba, 0xbe };
 *   char hstr[16];
 *   if (atohstr(hstr, arr, 4) == 0) {
 *       // hstr = "cafebabe\0";
 *   }
 *
 * Note:
 * sizeof passed char *hstr, has to be atleast (2 * arr_len) + 1.
 */
int atohstr(char *hstr, const uint8_t *arr, const int arr_len);

/*
 * hstrtoa() - Converts a hexadecimal string to it's value as array of
 * bytes.
 *
 * Usage:
 *   uint8_t arr[4];
 *   const char *hstr = "cafebabe";
 *   if (hstrtoa(arr, hstr) == 0) {
 *       // arr[4] = { 0xca, 0xfe, 0xba, 0xbe };
 *   }
 *
 * Note:
 * sizeof uint8_t *arr has to be atleast half of strlen(hstr)
 */
int hstrtoa(uint8_t *arr, const char *hstr);

/*
 * safe_atoi() - A wrapper for atoi() that returns -ve on non number-ish
 * strings. This can be used to distinguish "0" from "A" as both would
 * return 0 by the bare atoi()'s contract.
 */
int safe_atoi(const char *a, int *i);

/*
 * safe_strncpy() - A wrapper for strnlen() that guarantees null
 * terminated copy to dest. If strlen(src) > size, then only size-1
 * chars are copied to dest and a terminating '\0' is added.
 */
char *safe_strncpy(char* dest, const char* src, size_t size);

/**
 * @brief rstrip(), lstrip(), strip() - String space trim methods, as defined
 * in python3 programming language.
 *
 * @return returns the length of the modified string. In the case of lstrip(),
 * if the string was not modified, -ve error is returned to indicate this. This
 * is done to avoid a needless O(N) strlen() call.
 */
int rstrip(char *str);
int lstrip(char *str);
int strip(char *str);

/**
 * @brief Removes any tailing \r or \n chars.
 *
 * @return Length of modified string.
 */
size_t chomp(char *str);

/**
 * @brief lstrip() without shifting chars but looses the passed pointer. This
 * can be used on local non-alloced refs.
 */
#define lstrip_soft(p) \
	do { if (p) while (*p != '\0' && *p == ' ') p++; } while (0)

/**
 * @brief Remove all occurrence of of c in str
 *
 * @param str source string
 * @param c char to be removed
 */
void remove_all(char *str, char c);

/**
 * @brief Splits `buf` by `sep` (as defined in strtok) into char **
 * and adds tailing NULL pointer into `tokens`
 *
 * @param buf source string
 * @param sep array of chars that mark separation in `buf` (see strtok(3)).
 * @param tokens allocated array of char ptrs. Must be freed by caller.
 *
 * @return -1 on errors
 * @return  0 on success
 */
int split_string(char *buf, char *sep, char ***tokens);

/**
 * @brief Return the number of times c occurs in null terminated string s.
 */
int strcntchr(char *s, char c);

/**
 * @brief Checks if string poited to by s is empty.
 *
 * This functions evaluates to true under the following conditions:
 *   - pointer s is NULL
 *   - s[0] == '\0'
 *   - s has only whitespace chars followed by a '\0'
 *
 * @returns 1 when s is empty (see conditions)
 * @returns 0 when string has other non-whitespace chars
 */
int strisempty(char *s);

/**
 * @brief Compute a 32/64 bit hash value for str upto len chars or upto the
 * null char. Trick: for computing hash for null terminated strings len can
 * be set to -ve instead of passing strlen(s) explicitly.
 *
 * @return 32-bit hash code
 */
uint32_t hash32_djb2(const char *str, int len);
uint32_t hash32_fnv(const char *str, int len);
uint64_t poly_hash(const char *str, int len);
#define hash32 hash32_djb2
#define hash64 poly_hash
/**
 * @brief A strsep() clone. See `man 3 strsep` for more details. str_sep() is
 * cloned here because strsep() is not guaranteed to be available everywhere.
 */
char *str_sep(char **str, const char *sep);

/**
 * @brief Return a count of tokens in `str` when split by chars in `sep`. A
 * token is as it is defined (and returned) by tokenizing method such as
 * strtok() or strsep().
 */
size_t str_sep_count(const char *str, const char *sep);

/**
 * @brief Transform the input null-terminated string to upper/lower case.
 * Only alphapbets are transformed other characters are left as-is.
 */
void to_upper(char *s);
void to_lower(char *s);

#ifdef __cplusplus
}
#endif

#endif /* _UTIL_STRUTIL_H_ */
