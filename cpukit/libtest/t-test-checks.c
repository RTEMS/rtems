/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestFrameworkImpl
 *
 * @brief This source file contains the implementation of various RTEMS Test
 *   Framework check functions.
 */

/*
 * Copyright (C) 2018 embedded brains GmbH & Co. KG
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

#include <rtems/test.h>

#include <inttypes.h>

void
T_check_eq_ptr(const T_check_context_msg *t, uintptr_t a, uintptr_t e)
{
	T_check(&t->base, a == e, "%s", t->msg);
}

void
T_check_ne_ptr(const T_check_context_msg *t, uintptr_t a, uintptr_t e)
{
	T_check(&t->base, a != e, "%s", t->msg);
}

void
T_check_null(const T_check_context_msg *t, uintptr_t a)
{
	T_check(&t->base, a == 0, "%s == NULL", t->msg);
}

void
T_check_not_null(const T_check_context_msg *t, uintptr_t a)
{
	T_check(&t->base, a != 0, "%s != NULL", t->msg);
}

void
T_check_eq_mem(const T_check_context_msg *t, const void *a, const void *e,
    size_t n)
{
	T_check(&t->base, memcmp(a, e, n) == 0, "%s", t->msg);
}

void
T_check_ne_mem(const T_check_context_msg *t, const void *a, const void *e,
    size_t n)
{
	T_check(&t->base, memcmp(a, e, n) != 0, "%s", t->msg);
}

void
T_check_eq_str(const T_check_context *t, const char *a, const char *e)
{
	T_check(t, strcmp(a, e) == 0, "\"%s\" == \"%s\"", a, e);
}

void
T_check_ne_str(const T_check_context *t, const char *a, const char *e)
{
	T_check(t, strcmp(a, e) != 0, "\"%s\" != \"%s\"", a, e);
}

void
T_check_eq_nstr(const T_check_context *t, const char *a, const char *e, size_t n)
{
	T_check(t, strncmp(a, e, n) == 0, "\"%.*s\" == \"%.*s\"", (int)n, a,
	    (int)n, e);
}

void
T_check_ne_nstr(const T_check_context *t, const char *a, const char *e, size_t n)
{
	T_check(t, strncmp(a, e, n) != 0, "\"%.*s\" != \"%.*s\"", (int)n, a,
	    (int)n, e);
}

void
T_check_eq_char(const T_check_context *t, char a, char e)
{
	T_check(t, a == e, "'%c' == '%c'", a, e);
}

void
T_check_ne_char(const T_check_context *t, char a, char e)
{
	T_check(t, a != e, "'%c' != '%c'", a, e);
}

void
T_check_eq_int(const T_check_context *t, int a, int e)
{
	T_check(t, a == e, "%i == %i", a, e);
}

void
T_check_ne_int(const T_check_context *t, int a, int e)
{
	T_check(t, a != e, "%i != %i", a, e);
}

void
T_check_ge_int(const T_check_context *t, int a, int e)
{
	T_check(t, a >= e, "%i >= %i", a, e);
}

void
T_check_gt_int(const T_check_context *t, int a, int e)
{
	T_check(t, a > e, "%i > %i", a, e);
}

void
T_check_le_int(const T_check_context *t, int a, int e)
{
	T_check(t, a <= e, "%i <= %i", a, e);
}

void
T_check_lt_int(const T_check_context *t, int a, int e)
{
	T_check(t, a < e, "%i < %i", a, e);
}

void
T_check_eq_uint(const T_check_context *t, unsigned int a, unsigned int e)
{
	T_check(t, a == e, "%u == %u", a, e);
}

void
T_check_ne_uint(const T_check_context *t, unsigned int a, unsigned int e)
{
	T_check(t, a != e, "%u != %u", a, e);
}

void
T_check_ge_uint(const T_check_context *t, unsigned int a, unsigned int e)
{
	T_check(t, a >= e, "%u >= %u", a, e);
}

void
T_check_gt_uint(const T_check_context *t, unsigned int a, unsigned int e)
{
	T_check(t, a > e, "%u > %u", a, e);
}

void
T_check_le_uint(const T_check_context *t, unsigned int a, unsigned int e)
{
	T_check(t, a <= e, "%u <= %u", a, e);
}

void
T_check_lt_uint(const T_check_context *t, unsigned int a, unsigned int e)
{
	T_check(t, a < e, "%u < %u", a, e);
}

void
T_check_eq_long(const T_check_context *t, long a, long e)
{
	T_check(t, a == e, "%li == %li", a, e);
}

void
T_check_ne_long(const T_check_context *t, long a, long e)
{
	T_check(t, a != e, "%li != %li", a, e);
}

void
T_check_ge_long(const T_check_context *t, long a, long e)
{
	T_check(t, a >= e, "%li >= %li", a, e);
}

void
T_check_gt_long(const T_check_context *t, long a, long e)
{
	T_check(t, a > e, "%li > %li", a, e);
}

void
T_check_le_long(const T_check_context *t, long a, long e)
{
	T_check(t, a <= e, "%li <= %li", a, e);
}

void
T_check_lt_long(const T_check_context *t, long a, long e)
{
	T_check(t, a < e, "%li < %li", a, e);
}

void
T_check_eq_ulong(const T_check_context *t, unsigned long a, unsigned long e)
{
	T_check(t, a == e, "%lu == %lu", a, e);
}

void
T_check_ne_ulong(const T_check_context *t, unsigned long a, unsigned long e)
{
	T_check(t, a != e, "%lu != %lu", a, e);
}

void
T_check_ge_ulong(const T_check_context *t, unsigned long a, unsigned long e)
{
	T_check(t, a >= e, "%lu >= %lu", a, e);
}

void
T_check_gt_ulong(const T_check_context *t, unsigned long a, unsigned long e)
{
	T_check(t, a > e, "%lu > %lu", a, e);
}

void
T_check_le_ulong(const T_check_context *t, unsigned long a, unsigned long e)
{
	T_check(t, a <= e, "%lu <= %lu", a, e);
}

void
T_check_lt_ulong(const T_check_context *t, unsigned long a, unsigned long e)
{
	T_check(t, a < e, "%lu < %lu", a, e);
}

void
T_check_eq_ll(const T_check_context *t, long long a, long long e)
{
	T_check(t, a == e, "%lli == %lli", a, e);
}

void
T_check_ne_ll(const T_check_context *t, long long a, long long e)
{
	T_check(t, a != e, "%lli != %lli", a, e);
}

void
T_check_ge_ll(const T_check_context *t, long long a, long long e)
{
	T_check(t, a >= e, "%lli >= %lli", a, e);
}

void
T_check_gt_ll(const T_check_context *t, long long a, long long e)
{
	T_check(t, a > e, "%lli > %lli", a, e);
}

void
T_check_le_ll(const T_check_context *t, long long a, long long e)
{
	T_check(t, a <= e, "%lli <= %lli", a, e);
}

void
T_check_lt_ll(const T_check_context *t, long long a, long long e)
{
	T_check(t, a < e, "%lli < %lli", a, e);
}

void
T_check_eq_ull(const T_check_context *t, unsigned long long a,
    unsigned long long e)
{
	T_check(t, a == e, "%llu == %llu", a, e);
}

void
T_check_ne_ull(const T_check_context *t, unsigned long long a,
    unsigned long long e)
{
	T_check(t, a != e, "%llu != %llu", a, e);
}

void
T_check_ge_ull(const T_check_context *t, unsigned long long a,
    unsigned long long e)
{
	T_check(t, a >= e, "%llu >= %llu", a, e);
}

void
T_check_gt_ull(const T_check_context *t, unsigned long long a,
    unsigned long long e)
{
	T_check(t, a > e, "%llu > %llu", a, e);
}

void
T_check_le_ull(const T_check_context *t, unsigned long long a,
    unsigned long long e)
{
	T_check(t, a <= e, "%llu <= %llu", a, e);
}

void
T_check_lt_ull(const T_check_context *t, unsigned long long a,
    unsigned long long e)
{
	T_check(t, a < e, "%llu < %llu", a, e);
}
