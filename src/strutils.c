/*
 * Copyright (c) 2020-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <ctype.h>

#include <utils/strutils.h>
#include <utils/memory.h>

static inline int hex2int(char ch)
{
	if (ch >= '0' && ch <= '9')
		return ch - '0';
	ch &= ~0x20; // to upper case.
	if (ch >= 'A' && ch <= 'F')
		return ch - 'A' + 10;
	return -1;
}

static inline char int2hex(int v)
{
	v &= 0x0f; // consider only the lower nibble
	return (v >= 0 && v <= 9) ? '0' + v : 'A' + (v - 10);
}

int atohstr(char *hstr, const uint8_t *arr, const int arr_len)
{
	int i;

	for (i = 0; i < arr_len; i++) {
		hstr[(i * 2) + 0] = int2hex(arr[i] >> 4);
		hstr[(i * 2) + 1] = int2hex(arr[i]);
	}
	hstr[i * 2] = '\0';

	return 0;
}

int hstrtoa(uint8_t *arr, const char *hstr)
{
	int i, len, nib1, nib2;

	len = strlen(hstr);
	if ((len & 1) == 1 || len == 0)
		return -1; // Must have even number of chars
	len = len / 2;
	for (i = 0; i < len; i++) {
		nib1 = hex2int(hstr[(i * 2) + 0]);
		nib2 = hex2int(hstr[(i * 2) + 1]);
		if (nib1 == -1 || nib2 == -1)
			return -1;
		arr[i] = nib1 << 4 | nib2;
	}

	return len;
}

int safe_atoi(const char *a, int *i)
{
	int val;

	if (a == NULL)
		return -1;
	val = atoi(a);
	if (val == 0 && a[0] != '0')
		return -1;
	*i = val;
	return 0;
}

char *safe_strncpy(char* dest, const char* src, size_t size)
{
    strncpy(dest, src, size - 1);
    dest[size - 1] = '\0';
    return dest;
}

int trim_suffix(char *str, const char *suffix)
{
	int i, j;

	if (!str || !suffix)
		return -1;

	i = strlen(str);
	j = strlen(suffix);

	if (j > i)
		return -1;

	while (j > 0 && str[i-1] == suffix[j-1]) {
		i--; j--;
	}

	return str[i] = '\0';
}

int rstrip(char *str)
{
	int i;

	i = strlen(str);
	while (i > 0 && str[i - 1] == ' ') {
		str[i - 1] = '\0';
		i -= 1;
	}
	return i;
}

/* Note: can return -ve */
int lstrip(char *str)
{
	int i = 0, j = 0;

	while (str[i] && str[i] == ' ')
		i++;
	if (i == 0)
		return -1;
	while (str[i]) {
		str[j] = str[i];
		i += 1; j += 1;
	}
	str[j] = '\0';
	return (j > 0) ? j - 1 : 0;
}

int strip(char *str)
{
	int rs_len, ls_len;

	rs_len = rstrip(str);
	ls_len = lstrip(str); /* can be -ve */
	return (ls_len > 0) ? ls_len : rs_len;
}

size_t chomp(char *str)
{
	size_t i;

	i = strlen(str);
	while (i > 0 && (str[i - 1] == '\n' || str[i - 1] == '\r')) {
		str[i - 1] = '\0';
		i -= 1;
	}
	return i;
}

void remove_all(char *str, char c)
{
	int i, j = 0;

	for (i = 0; str[i]; i++) {
		if (str[i] != c)
			str[j++] = str[i];
	}
	str[j] = '\0';
}

int split_string(char *buf, char *sep, char ***tokens)
{
	char *tok, *rest, **toks = NULL;
	size_t length = 0, size = 0;
	int chunk = 16;

	tok = strtok_r(buf, sep, &rest);
	while (tok != NULL) {
		if (length >= size) {
			toks = safe_realloc(toks, sizeof(char *) * (size + chunk));
			size += chunk;
			chunk <<= 2;
		}
		toks[length] = safe_strdup(tok);
		length += 1;
		tok = strtok_r(NULL, sep, &rest);
	}
	if (toks == NULL) {
		return -1;
	}
	toks = safe_realloc(toks, sizeof(char *) * (length + 1));
	toks[length] = NULL;
	*tokens = toks;
	return 0;
}

int strcntchr(char *s, char c)
{
	int i = 0, count = 0;

	while (s[i]) {
		if (s[i] == c)
			count += 1;
		i += 1;
	}
	return count;
}

int strisempty(char *s)
{
	while (s && *s != '\0' && isspace((unsigned char)*s))
		s++;

	return s == NULL || *s == '\0';
}

uint32_t hash32_djb2(const char *str, int len)
{
	int c;
	uint32_t hash = 5381;

	while ((c = *str++) && len-- != 0) {
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}
	return hash;
}

uint32_t hash32_fnv(const char *str, int len)
{
	uint32_t hval = 0;
	unsigned char *s = (unsigned char *)str; /* unsigned string */

	/* FNV-1 hash */
	while (*s && len-- != 0) {
		/* multiply by the 32 bit FNV magic prime mod 2^32 */
		hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
		hval ^= (uint32_t)*s++;
	}
	return hval;
}

uint64_t poly_hash(const char *str, int len)
{
	int c;
	const int p = 31;
	const int m = 1e9 + 9;
	uint64_t hash = 0;
	uint64_t p_pow = 1;

	while ((c = *str++) && len-- != 0) {
		hash = (hash + (c - 'a' + 1) * p_pow) % m;
		p_pow = (p_pow * p) % m;
	}
	return hash;
}

char *str_sep(char **str, const char *sep)
{
	char *start, *end;

	if (*str == NULL || *str[0] == '\0')
		return *str;

	start = *str;
	while (*start && strchr(sep, *start))
		start++;
	end = start;
	while (*end && !strchr(sep, *end))
		end++;
	if (*end != '\0')
		*end++ = '\0';
	*str = end;
	return start;
}

size_t str_sep_count(const char *str, const char *sep)
{
	size_t count = 0;
	const char *p;

	if (str == NULL || str[0] == '\0')
		return 0;
	if (sep == NULL)
		return 1;

	p = str;
	while (*p) {
		while (*p && strchr(sep, *p))
			p++;
		if (*p == '\0')
			break;
		count++;
		while(*p && !strchr(sep, *p))
			p++;
	}
	return count;
}

void to_upper(char *s)
{
	while (*s) {
		if (isalpha((unsigned char)*s))
			*s &= ~0x20;
		s++;
	}
}

void to_lower(char *s)
{
	while (*s) {
		if (isalpha((unsigned char)*s))
			*s |= 0x20;
		s++;
	}
}
