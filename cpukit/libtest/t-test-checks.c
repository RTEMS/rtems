/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2018 embedded brains GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <t.h>

#include <inttypes.h>

void
T_check_eq_ptr(const void *a, const T_check_context_msg *t, const void *e)
{
	T_check_true(a == e, &t->base, "%s", t->msg);
}

void
T_check_ne_ptr(const void *a, const T_check_context_msg *t, const void *e)
{
	T_check_true(a != e, &t->base, "%s", t->msg);
}

void
T_check_null(const void *a, const T_check_context_msg *t)
{
	T_check_true(a == NULL, &t->base, "%s == NULL", t->msg);
}

void
T_check_not_null(const void *a, const T_check_context_msg *t)
{
	T_check_true(a != NULL, &t->base, "%s != NULL", t->msg);
}

void
T_check_eq_mem(const void *a, const T_check_context_msg *t, const void *e,
    size_t n)
{
	T_check_true(memcmp(a, e, n) == 0, &t->base, "%s", t->msg);
}

void
T_check_ne_mem(const void *a, const T_check_context_msg *t, const void *e,
    size_t n)
{
	T_check_true(memcmp(a, e, n) != 0, &t->base, "%s", t->msg);
}

void
T_check_eq_str(const char *a, const T_check_context *t, const char *e)
{
	T_check_true(strcmp(a, e) == 0, t, "\"%s\" == \"%s\"", a, e);
}

void
T_check_ne_str(const char *a, const T_check_context *t, const char *e)
{
	T_check_true(strcmp(a, e) != 0, t, "\"%s\" != \"%s\"", a, e);
}

void
T_check_eq_nstr(const char *a, const T_check_context *t, const char *e, size_t n)
{
	T_check_true(strncmp(a, e, n) == 0, t, "\"%.*s\" == \"%.*s\"", (int)n, a,
	    (int)n, e);
}

void
T_check_ne_nstr(const char *a, const T_check_context *t, const char *e, size_t n)
{
	T_check_true(strncmp(a, e, n) != 0, t, "\"%.*s\" != \"%.*s\"", (int)n, a,
	    (int)n, e);
}

void
T_check_eq_char(char a, const T_check_context *t, char e)
{
	T_check_true(a == e, t, "'%c' == '%c'", a, e);
}

void
T_check_ne_char(char a, const T_check_context *t, char e)
{
	T_check_true(a != e, t, "'%c' != '%c'", a, e);
}

void
T_check_eq_int(int a, const T_check_context *t, int e)
{
	T_check_true(a == e, t, "%i == %i", a, e);
}

void
T_check_ne_int(int a, const T_check_context *t, int e)
{
	T_check_true(a != e, t, "%i != %i", a, e);
}

void
T_check_ge_int(int a, const T_check_context *t, int e)
{
	T_check_true(a >= e, t, "%i >= %i", a, e);
}

void
T_check_gt_int(int a, const T_check_context *t, int e)
{
	T_check_true(a > e, t, "%i > %i", a, e);
}

void
T_check_le_int(int a, const T_check_context *t, int e)
{
	T_check_true(a <= e, t, "%i <= %i", a, e);
}

void
T_check_lt_int(int a, const T_check_context *t, int e)
{
	T_check_true(a < e, t, "%i < %i", a, e);
}

void
T_check_eq_uint(unsigned int a, const T_check_context *t, unsigned int e)
{
	T_check_true(a == e, t, "%u == %u", a, e);
}

void
T_check_ne_uint(unsigned int a, const T_check_context *t, unsigned int e)
{
	T_check_true(a != e, t, "%u != %u", a, e);
}

void
T_check_ge_uint(unsigned int a, const T_check_context *t, unsigned int e)
{
	T_check_true(a >= e, t, "%u >= %u", a, e);
}

void
T_check_gt_uint(unsigned int a, const T_check_context *t, unsigned int e)
{
	T_check_true(a > e, t, "%u > %u", a, e);
}

void
T_check_le_uint(unsigned int a, const T_check_context *t, unsigned int e)
{
	T_check_true(a <= e, t, "%u <= %u", a, e);
}

void
T_check_lt_uint(unsigned int a, const T_check_context *t, unsigned int e)
{
	T_check_true(a < e, t, "%u < %u", a, e);
}

void
T_check_eq_long(long a, const T_check_context *t, long e)
{
	T_check_true(a == e, t, "%li == %li", a, e);
}

void
T_check_ne_long(long a, const T_check_context *t, long e)
{
	T_check_true(a != e, t, "%li != %li", a, e);
}

void
T_check_ge_long(long a, const T_check_context *t, long e)
{
	T_check_true(a >= e, t, "%li >= %li", a, e);
}

void
T_check_gt_long(long a, const T_check_context *t, long e)
{
	T_check_true(a > e, t, "%li > %li", a, e);
}

void
T_check_le_long(long a, const T_check_context *t, long e)
{
	T_check_true(a <= e, t, "%li <= %li", a, e);
}

void
T_check_lt_long(long a, const T_check_context *t, long e)
{
	T_check_true(a < e, t, "%li < %li", a, e);
}

void
T_check_eq_ulong(unsigned long a, const T_check_context *t, unsigned long e)
{
	T_check_true(a == e, t, "%lu == %lu", a, e);
}

void
T_check_ne_ulong(unsigned long a, const T_check_context *t, unsigned long e)
{
	T_check_true(a != e, t, "%lu != %lu", a, e);
}

void
T_check_ge_ulong(unsigned long a, const T_check_context *t, unsigned long e)
{
	T_check_true(a >= e, t, "%lu >= %lu", a, e);
}

void
T_check_gt_ulong(unsigned long a, const T_check_context *t, unsigned long e)
{
	T_check_true(a > e, t, "%lu > %lu", a, e);
}

void
T_check_le_ulong(unsigned long a, const T_check_context *t, unsigned long e)
{
	T_check_true(a <= e, t, "%lu <= %lu", a, e);
}

void
T_check_lt_ulong(unsigned long a, const T_check_context *t, unsigned long e)
{
	T_check_true(a < e, t, "%lu < %lu", a, e);
}

void
T_check_eq_ll(long long a, const T_check_context *t, long long e)
{
	T_check_true(a == e, t, "%lli == %lli", a, e);
}

void
T_check_ne_ll(long long a, const T_check_context *t, long long e)
{
	T_check_true(a != e, t, "%lli != %lli", a, e);
}

void
T_check_ge_ll(long long a, const T_check_context *t, long long e)
{
	T_check_true(a >= e, t, "%lli >= %lli", a, e);
}

void
T_check_gt_ll(long long a, const T_check_context *t, long long e)
{
	T_check_true(a > e, t, "%lli > %lli", a, e);
}

void
T_check_le_ll(long long a, const T_check_context *t, long long e)
{
	T_check_true(a <= e, t, "%lli <= %lli", a, e);
}

void
T_check_lt_ll(long long a, const T_check_context *t, long long e)
{
	T_check_true(a < e, t, "%lli < %lli", a, e);
}

void
T_check_eq_ull(unsigned long long a, const T_check_context *t,
    unsigned long long e)
{
	T_check_true(a == e, t, "%llu == %llu", a, e);
}

void
T_check_ne_ull(unsigned long long a, const T_check_context *t,
    unsigned long long e)
{
	T_check_true(a != e, t, "%llu != %llu", a, e);
}

void
T_check_ge_ull(unsigned long long a, const T_check_context *t,
    unsigned long long e)
{
	T_check_true(a >= e, t, "%llu >= %llu", a, e);
}

void
T_check_gt_ull(unsigned long long a, const T_check_context *t,
    unsigned long long e)
{
	T_check_true(a > e, t, "%llu > %llu", a, e);
}

void
T_check_le_ull(unsigned long long a, const T_check_context *t,
    unsigned long long e)
{
	T_check_true(a <= e, t, "%llu <= %llu", a, e);
}

void
T_check_lt_ull(unsigned long long a, const T_check_context *t,
    unsigned long long e)
{
	T_check_true(a < e, t, "%llu < %llu", a, e);
}
