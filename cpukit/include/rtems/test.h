/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2017, 2021 embedded brains GmbH
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

#ifndef THE_T_TEST_FRAMEWORK_H
#define THE_T_TEST_FRAMEWORK_H

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifdef __rtems__
#include <rtems/score/cpu.h>
#include <rtems/rtems/status.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSTestFramework RTEMS Test Framework
 *
 * @ingroup RTEMSAPI
 *
 * @brief The RTEMS Test Framework helps you to write tests.
 *
 * @{
 */

typedef enum {
	T_QUIET,
	T_NORMAL,
	T_VERBOSE
} T_verbosity;

typedef struct T_fixture {
	void (*setup)(void *);
	void (*stop)(void *);
	void (*teardown)(void *);
	size_t (*scope)(void *, char *, size_t);
	void *initial_context;
} T_fixture;

typedef struct T_fixture_node {
	struct T_fixture_node *next;
	struct T_fixture_node *previous;
	const T_fixture *fixture;
	void *context;
	unsigned int next_planned_steps;
	unsigned int next_steps;
	unsigned int failures;
} T_fixture_node;

#define T_ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

/*
 * The __FILE__ includes the full file path from the command line.  Enable the
 * build system to give a sorter file name via a command line define.
 */
#ifndef T_FILE_NAME
#define T_FILE_NAME __FILE__
#endif

#if defined(__GNUC__) || __STDC_VERSION__ >= 199409L
#define T_ZERO_LENGTH_ARRAY
#define T_ZERO_LENGTH_ARRAY_EXTENSION(n) (n)
#else
#define T_ZERO_LENGTH_ARRAY 1
#define T_ZERO_LENGTH_ARRAY_EXTENSION(n) ((n) - 1)
#endif

/** @} */

/**
 * @defgroup RTEMSTestFrameworkImpl RTEMS Test Framework Implementation
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Implementation details.
 *
 * @{
 */

#ifdef __cplusplus
#define T_NO_RETURN [[ noreturn ]]
#else
#define T_NO_RETURN _Noreturn
#endif

typedef struct T_case_context {
	const char *name;
	void (*body)(void);
	const T_fixture *fixture;
	const struct T_case_context *next;
} T_case_context;

void T_case_register(T_case_context *);

#define T_CHECK_STOP 1U

#define T_CHECK_QUIET 2U

/* If you change this value, you have to adjust T_VA_ARGS_KIND() as well */
#define T_CHECK_FMT 4U

#define T_CHECK_STEP_FLAG 8U

#define T_CHECK_STEP_TO_FLAGS(step) ((unsigned int)(step) << 8)

#define T_CHECK_STEP_FROM_FLAGS(flags) ((flags) >> 8)

#define T_CHECK_STEP(step) (T_CHECK_STEP_TO_FLAGS(step) | T_CHECK_STEP_FLAG)

typedef struct {
	const char *file;
	int line;
	unsigned int flags;
} T_check_context;

typedef struct {
	T_check_context base;
	const char *msg;
} T_check_context_msg;

#define T_VA_ARGS_FIRST(...) T_VA_ARGS_FIRST_SELECT(__VA_ARGS__, throw_away)
#define T_VA_ARGS_FIRST_SELECT(first, ...) first

/*
 * The T_VA_ARGS_MORE() supports up to nine arguments.  It expands to nothing
 * if only one argument is given.  It expands to a comma and the second and
 * following arguments if at least two arguments are present.  The 4U shall
 * correspond to T_CHECK_FMT.
 */
#define T_VA_ARGS_MORE(...) \
    T_VA_ARGS_XEXPAND(T_VA_ARGS_KIND(__VA_ARGS__), __VA_ARGS__)
#define T_VA_ARGS_XEXPAND(kind, ...) T_VA_ARGS_EXPAND(kind, __VA_ARGS__)
#define T_VA_ARGS_EXPAND(kind, ...) T_VA_ARGS_EXPAND_##kind(__VA_ARGS__)
#define T_VA_ARGS_EXPAND_0U(first)
#define T_VA_ARGS_EXPAND_4U(first, ...) , __VA_ARGS__
#define T_VA_ARGS_KIND(...) \
    T_VA_ARGS_SELECT(__VA_ARGS__, \
    4U, 4U, 4U, 4U, 4U, 4U, 4U, 4U, 0U, throw_away)
#define T_VA_ARGS_SELECT(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, ...) a10

void T_check(const T_check_context *, bool, ...);

extern const T_check_context T_special;

#define T_flags_true(flags, ...)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, 					\
	    (flags) | T_VA_ARGS_KIND(__VA_ARGS__) };			\
	T_check(&T_check_instance,					\
	    T_VA_ARGS_FIRST(__VA_ARGS__) T_VA_ARGS_MORE(__VA_ARGS__));	\
}

#define T_flags_eq(flags, a, ...) \
    T_flags_true(flags, \
    (a) == (T_VA_ARGS_FIRST(__VA_ARGS__)) T_VA_ARGS_MORE(__VA_ARGS__))

#define T_flags_ne(flags, a, ...) \
    T_flags_true(flags, \
    (a) != (T_VA_ARGS_FIRST(__VA_ARGS__)) T_VA_ARGS_MORE(__VA_ARGS__))

void T_check_eq_ptr(const T_check_context_msg *, const void *, const void *);

#define T_flags_eq_ptr(a, e, flags, sa, se)				\
{									\
	static const T_check_context_msg T_check_instance = {		\
	    { T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT },		\
	    sa " == " se };						\
	T_check_eq_ptr(&T_check_instance, a, e);			\
}

void T_check_ne_ptr(const T_check_context_msg *, const void *, const void *);

#define T_flags_ne_ptr(a, e, flags, sa, se)				\
{									\
	static const T_check_context_msg T_check_instance = {		\
	    { T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT },		\
	    sa " != " se };						\
	T_check_ne_ptr(&T_check_instance, a, e);			\
}

void T_check_null(const T_check_context_msg *, const void *);

#define T_flags_null(a, flags, sa)					\
{									\
	static const T_check_context_msg T_check_instance = {		\
	    { T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT }, sa };	\
	T_check_null(&T_check_instance, a);				\
}

void T_check_not_null(const T_check_context_msg *, const void *);

#define T_flags_not_null(a, flags, sa)					\
{									\
	static const T_check_context_msg T_check_instance = {		\
	    { T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT }, sa };	\
	T_check_not_null(&T_check_instance, a);				\
}

void T_check_eq_mem(const T_check_context_msg *, const void *, const void *,
    size_t);

#define T_flags_eq_mem(a, e, n, flags, sa, se, sn)			\
{									\
	static const T_check_context_msg T_check_instance = {		\
	    { T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT },		\
	    "memcmp(" sa ", " se ", " sn ") == 0" };			\
	T_check_eq_mem(&T_check_instance, a, e, n);			\
}

void T_check_ne_mem(const T_check_context_msg *, const void *, const void *,
    size_t);

#define T_flags_ne_mem(a, e, n, flags, sa, se, sn)			\
{									\
	static const T_check_context_msg T_check_instance = {		\
	    { T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT },		\
	    "memcmp(" sa ", " se ", " sn ") != 0" };			\
	T_check_ne_mem(&T_check_instance, a, e, n);			\
}

void T_check_eq_str(const T_check_context *, const char *, const char *);

#define T_flags_eq_str(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_eq_str(&T_check_instance, a, e);			\
}

void T_check_ne_str(const T_check_context *, const char *, const char *);

#define T_flags_ne_str(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_ne_str(&T_check_instance, a, e);			\
}

void T_check_eq_nstr(const T_check_context *, const char *, const char *,
    size_t);

#define T_flags_eq_nstr(a, e, n, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_eq_nstr(&T_check_instance, a, e, n);			\
}

void T_check_ne_nstr(const T_check_context *, const char *, const char *,
    size_t);

#define T_flags_ne_nstr(a, e, n, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_ne_nstr(&T_check_instance, a, e, n);			\
}

void T_check_eq_char(const T_check_context *, char, char);

#define T_flags_eq_char(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_eq_char(&T_check_instance, a, e);			\
}

void T_check_ne_char(const T_check_context *, char, char);

#define T_flags_ne_char(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_ne_char(&T_check_instance, a, e);			\
}

void T_check_eq_int(const T_check_context *, int, int);

#define T_flags_eq_int(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_eq_int(&T_check_instance, a, e);			\
}

void T_check_ne_int(const T_check_context *, int, int);

#define T_flags_ne_int(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_ne_int(&T_check_instance, a, e);			\
}

void T_check_ge_int(const T_check_context *, int, int);

#define T_flags_ge_int(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_ge_int(&T_check_instance, a, e);			\
}

void T_check_gt_int(const T_check_context *, int, int);

#define T_flags_gt_int(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_gt_int(&T_check_instance, a, e);			\
}

void T_check_le_int(const T_check_context *, int, int);

#define T_flags_le_int(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_le_int(&T_check_instance, a, e);			\
}

void T_check_lt_int(const T_check_context *, int, int);

#define T_flags_lt_int(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_lt_int(&T_check_instance, a, e);			\
}

void T_check_eq_uint(const T_check_context *, unsigned int, unsigned int);

#define T_flags_eq_uint(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_eq_uint(&T_check_instance, a, e);			\
}

void T_check_ne_uint(const T_check_context *, unsigned int, unsigned int);

#define T_flags_ne_uint(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_ne_uint(&T_check_instance, a, e);			\
}

void T_check_ge_uint(const T_check_context *, unsigned int, unsigned int);

#define T_flags_ge_uint(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_ge_uint(&T_check_instance, a, e);			\
}

void T_check_gt_uint(const T_check_context *, unsigned int, unsigned int);

#define T_flags_gt_uint(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_gt_uint(&T_check_instance, a, e);			\
}

void T_check_le_uint(const T_check_context *, unsigned int, unsigned int);

#define T_flags_le_uint(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_le_uint(&T_check_instance, a, e);			\
}

void T_check_lt_uint(const T_check_context *, unsigned int, unsigned int);

#define T_flags_lt_uint(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_lt_uint(&T_check_instance, a, e);			\
}

void T_check_eq_long(const T_check_context *, long, long);

#define T_flags_eq_long(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_eq_long(&T_check_instance, a, e);			\
}

void T_check_ne_long(const T_check_context *, long, long);

#define T_flags_ne_long(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_ne_long(&T_check_instance, a, e);			\
}

void T_check_ge_long(const T_check_context *, long, long);

#define T_flags_ge_long(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_ge_long(&T_check_instance, a, e);			\
}

void T_check_gt_long(const T_check_context *, long, long);

#define T_flags_gt_long(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_gt_long(&T_check_instance, a, e);			\
}

void T_check_le_long(const T_check_context *, long, long);

#define T_flags_le_long(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_le_long(&T_check_instance, a, e);			\
}

void T_check_lt_long(const T_check_context *, long, long);

#define T_flags_lt_long(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_lt_long(&T_check_instance, a, e);			\
}

void T_check_eq_ulong(const T_check_context *, unsigned long, unsigned long);

#define T_flags_eq_ulong(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_eq_ulong(&T_check_instance, a, e);			\
}

void T_check_ne_ulong(const T_check_context *, unsigned long, unsigned long);

#define T_flags_ne_ulong(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_ne_ulong(&T_check_instance, a, e);			\
}

void T_check_ge_ulong(const T_check_context *, unsigned long, unsigned long);

#define T_flags_ge_ulong(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_ge_ulong(&T_check_instance, a, e);			\
}

void T_check_gt_ulong(const T_check_context *, unsigned long, unsigned long);

#define T_flags_gt_ulong(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_gt_ulong(&T_check_instance, a, e);			\
}

void T_check_le_ulong(const T_check_context *, unsigned long, unsigned long);

#define T_flags_le_ulong(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_le_ulong(&T_check_instance, a, e);			\
}

void T_check_lt_ulong(const T_check_context *, unsigned long, unsigned long);

#define T_flags_lt_ulong(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_lt_ulong(&T_check_instance, a, e);			\
}

void T_check_eq_ll(const T_check_context *, long long, long long);

#define T_flags_eq_ll(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_eq_ll(&T_check_instance, a, e);				\
}

void T_check_ne_ll(const T_check_context *, long long, long long);

#define T_flags_ne_ll(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_ne_ll(&T_check_instance, a, e);				\
}

void T_check_ge_ll(const T_check_context *, long long, long long);

#define T_flags_ge_ll(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_ge_ll(&T_check_instance, a, e);				\
}

void T_check_gt_ll(const T_check_context *, long long, long long);

#define T_flags_gt_ll(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_gt_ll(&T_check_instance, a, e);				\
}

void T_check_le_ll(const T_check_context *, long long, long long);

#define T_flags_le_ll(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_le_ll(&T_check_instance, a, e);				\
}

void T_check_lt_ll(const T_check_context *, long long, long long);

#define T_flags_lt_ll(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_lt_ll(&T_check_instance, a, e);				\
}

void T_check_eq_ull(const T_check_context *, unsigned long long,
    unsigned long long);

#define T_flags_eq_ull(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_eq_ull(&T_check_instance, a, e);			\
}

void T_check_ne_ull(const T_check_context *, unsigned long long,
    unsigned long long);

#define T_flags_ne_ull(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_ne_ull(&T_check_instance, a, e);			\
}

void T_check_ge_ull(const T_check_context *, unsigned long long,
    unsigned long long);

#define T_flags_ge_ull(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_ge_ull(&T_check_instance, a, e);			\
}

void T_check_gt_ull(const T_check_context *, unsigned long long,
    unsigned long long);

#define T_flags_gt_ull(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_gt_ull(&T_check_instance, a, e);			\
}

void T_check_le_ull(const T_check_context *, unsigned long long,
    unsigned long long);

#define T_flags_le_ull(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_le_ull(&T_check_instance, a, e);			\
}

void T_check_lt_ull(const T_check_context *, unsigned long long,
    unsigned long long);

#define T_flags_lt_ull(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_lt_ull(&T_check_instance, a, e);			\
}

void T_check_eno(const T_check_context *, int, int);

#define T_flags_eno(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_eno(&T_check_instance, a, e);				\
}

void T_check_eno_success(const T_check_context *, int);

#define T_flags_eno_success(a, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_eno_success(&T_check_instance, a);			\
}

void T_check_psx_error(const T_check_context *, int, int);

#define T_flags_psx_error(a, eno, flags)				\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_psx_error(&T_check_instance, a, eno);			\
}

void T_check_psx_success(const T_check_context *, int);

#define T_flags_psx_success(a, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_psx_success(&T_check_instance, a);			\
}

/** @} */

int T_printf(char const *, ...);

int T_vprintf(char const *, va_list);

int T_puts(const char *, size_t);

int T_snprintf(char *, size_t, const char *, ...);

void T_log(T_verbosity, char const *, ...);

const char *T_case_name(void);

T_verbosity T_set_verbosity(T_verbosity);

/**
 * @defgroup RTEMSTestFrameworkChecksBool Boolean Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for boolean expressions.
 *
 * @{
 */

#define T_true(...) T_flags_true(0, __VA_ARGS__)
#define T_assert_true(...) T_flags_true(T_CHECK_STOP, __VA_ARGS__)
#define T_quiet_true(...) T_flags_true(T_CHECK_QUIET, __VA_ARGS__)
#define T_step_true(s, ...) T_flags_true(T_CHECK_STEP(s), __VA_ARGS__)
#define T_step_assert_true(s, ...) \
    T_flags_true(T_CHECK_STEP(s) | T_CHECK_STOP, __VA_ARGS__)

#define T_false(...) \
    T_flags_true(0, \
    !(T_VA_ARGS_FIRST(__VA_ARGS__)) T_VA_ARGS_MORE(__VA_ARGS__))
#define T_assert_false(...) \
    T_flags_true(T_CHECK_STOP, \
    !(T_VA_ARGS_FIRST(__VA_ARGS__)) T_VA_ARGS_MORE(__VA_ARGS__))
#define T_quiet_false(...) \
    T_flags_true(T_CHECK_QUIET, \
    !(T_VA_ARGS_FIRST(__VA_ARGS__)) T_VA_ARGS_MORE(__VA_ARGS__))
#define T_step_false(s, ...) \
    T_flags_true(T_CHECK_STEP(s), \
    !(T_VA_ARGS_FIRST(__VA_ARGS__)) T_VA_ARGS_MORE(__VA_ARGS__))
#define T_step_assert_false(s, ...) \
    T_flags_true(T_CHECK_STEP(s) | T_CHECK_STOP, \
    !(T_VA_ARGS_FIRST(__VA_ARGS__)) T_VA_ARGS_MORE(__VA_ARGS__))

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksGeneric Generic Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for data types with an equality or inequality operator.
 *
 * @{
 */

#define T_eq(a, ...) T_flags_eq(0, a, __VA_ARGS__)
#define T_assert_eq(a, ...) T_flags_eq(T_CHECK_STOP, a, __VA_ARGS__)
#define T_quiet_eq(a, ...) T_flags_eq(T_CHECK_QUIET, a, __VA_ARGS__)
#define T_step_eq(s, a, ...) T_flags_eq(T_CHECK_STEP(s), a, __VA_ARGS__)
#define T_step_assert_eq(s, a, ...) \
    T_flags_eq(T_CHECK_STEP(s) | T_CHECK_STOP, a, __VA_ARGS__)

#define T_ne(a, ...) T_flags_ne(0, a, __VA_ARGS__)
#define T_assert_ne(a, ...) T_flags_ne(T_CHECK_STOP, a, __VA_ARGS__)
#define T_quiet_ne(a, ...) T_flags_ne(T_CHECK_QUIET, a, __VA_ARGS__)
#define T_step_ne(s, a, ...) T_flags_ne(T_CHECK_STEP(s), a, __VA_ARGS__)
#define T_step_assert_ne(s, a, ...) \
    T_flags_ne(T_CHECK_STEP(s) | T_CHECK_STOP, a, __VA_ARGS__)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksPointer Pointer Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for pointers.
 *
 * @{
 */

#define T_eq_ptr(a, e) T_flags_eq_ptr(a, e, 0, #a, #e)
#define T_assert_eq_ptr(a, e) T_flags_eq_ptr(a, e, T_CHECK_STOP, #a, #e)
#define T_quiet_eq_ptr(a, e) T_flags_eq_ptr(a, e, T_CHECK_QUIET, #a, #e)
#define T_step_eq_ptr(s, a, e) T_flags_eq_ptr(a, e, T_CHECK_STEP(s), #a, #e)
#define T_step_assert_eq_ptr(s, a, e) \
    T_flags_eq_ptr(a, e, T_CHECK_STEP(s) | T_CHECK_STOP, #a, #e)

#define T_ne_ptr(a, e) T_flags_ne_ptr(a, e, 0, #a, #e)
#define T_assert_ne_ptr(a, e) T_flags_ne_ptr(a, e, T_CHECK_STOP, #a, #e)
#define T_quiet_ne_ptr(a, e) T_flags_ne_ptr(a, e, T_CHECK_QUIET, #a, #e)
#define T_step_ne_ptr(s, a, e) T_flags_ne_ptr(a, e, T_CHECK_STEP(s), #a, #e)
#define T_step_assert_ne_ptr(s, a, e) \
    T_flags_ne_ptr(a, e, T_CHECK_STEP(s) | T_CHECK_STOP, #a, #e)

#define T_null(a) T_flags_null(a, 0, #a)
#define T_assert_null(a) T_flags_null(a, T_CHECK_STOP, #a)
#define T_quiet_null(a) T_flags_null(a, T_CHECK_QUIET, #a)
#define T_quiet_assert_null(a) \
    T_flags_null(a, T_CHECK_QUIET | T_CHECK_STOP, #a)
#define T_step_null(s, a) T_flags_null(a, T_CHECK_STEP(s), #a)
#define T_step_assert_null(s, a) \
    T_flags_null(a, T_CHECK_STEP(s) | T_CHECK_STOP, #a)

#define T_not_null(a) T_flags_not_null(a, 0, #a)
#define T_assert_not_null(a) T_flags_not_null(a, T_CHECK_STOP, #a)
#define T_quiet_not_null(a) T_flags_not_null(a, T_CHECK_QUIET, #a)
#define T_quiet_assert_not_null(a) \
    T_flags_not_null(a, T_CHECK_QUIET | T_CHECK_STOP, #a)
#define T_step_not_null(s, a) T_flags_not_null(a, T_CHECK_STEP(s), #a)
#define T_step_assert_not_null(s, a) \
    T_flags_not_null(a, T_CHECK_STEP(s) | T_CHECK_STOP, #a)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksMemStr Memory Area Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for memory areas.
 *
 * @{
 */

#define T_eq_mem(a, e, n) T_flags_eq_mem(a, e, n, 0, #a, #e, #n)
#define T_assert_eq_mem(a, e, n) \
    T_flags_eq_mem(a, e, n, T_CHECK_STOP, #a, #e, #n)
#define T_quiet_eq_mem(a, e, n) \
    T_flags_eq_mem(a, e, n, T_CHECK_QUIET, #a, #e, #n)
#define T_step_eq_mem(s, a, e, n) \
    T_flags_eq_mem(a, e, n, T_CHECK_STEP(s), #a, #e, #n)
#define T_step_assert_eq_mem(s, a, e, n) \
    T_flags_eq_mem(a, e, n, T_CHECK_STEP(s) | T_CHECK_STOP, #a, #e, #n)

#define T_ne_mem(a, e, n) T_flags_ne_mem(a, e, n, 0, #a, #e, #n)
#define T_assert_ne_mem(a, e, n) \
    T_flags_ne_mem(a, e, n, T_CHECK_STOP, #a, #e, #n)
#define T_quiet_ne_mem(a, e, n) \
    T_flags_ne_mem(a, e, n, T_CHECK_QUIET, #a, #e, #n)
#define T_step_ne_mem(s, a, e, n) \
    T_flags_ne_mem(a, e, n, T_CHECK_STEP(s), #a, #e, #n)
#define T_step_assert_ne_mem(s, a, e, n) \
    T_flags_ne_mem(a, e, n, T_CHECK_STEP(s) | T_CHECK_STOP, #a, #e, #n)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksStr String Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for strings.
 *
 * @{
 */


#define T_eq_str(a, e) T_flags_eq_str(a, e, 0)
#define T_assert_eq_str(a, e) T_flags_eq_str(a, e, T_CHECK_STOP)
#define T_quiet_eq_str(a, e) T_flags_eq_str(a, e, T_CHECK_QUIET)
#define T_step_eq_str(s, a, e) T_flags_eq_str(a, e, T_CHECK_STEP(s))
#define T_step_assert_eq_str(s, a, e) \
    T_flags_eq_str(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_str(a, e) T_flags_ne_str(a, e, 0)
#define T_assert_ne_str(a, e) T_flags_ne_str(a, e, T_CHECK_STOP)
#define T_quiet_ne_str(a, e) T_flags_ne_str(a, e, T_CHECK_QUIET)
#define T_step_ne_str(s, a, e) T_flags_ne_str(a, e, T_CHECK_STEP(s))
#define T_step_assert_ne_str(s, a, e) \
    T_flags_ne_str(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_eq_nstr(a, e, n) T_flags_eq_nstr(a, e, n, 0)
#define T_assert_eq_nstr(a, e, n) T_flags_eq_nstr(a, e, n, T_CHECK_STOP)
#define T_quiet_eq_nstr(a, e, n) T_flags_eq_nstr(a, e, n, T_CHECK_QUIET)
#define T_step_eq_nstr(s, a, e, n) T_flags_eq_nstr(a, e, n, T_CHECK_STEP(s))
#define T_step_assert_eq_nstr(s, a, e, n) \
    T_flags_eq_nstr(a, e, n, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_nstr(a, e, n) T_flags_ne_nstr(a, e, n, 0)
#define T_assert_ne_nstr(a, e, n) T_flags_ne_nstr(a, e, n, T_CHECK_STOP)
#define T_quiet_ne_nstr(a, e, n) T_flags_ne_nstr(a, e, n, T_CHECK_QUIET)
#define T_step_ne_nstr(s, a, e, n) T_flags_ne_nstr(a, e, n, T_CHECK_STEP(s))
#define T_step_assert_ne_nstr(s, a, e, n) \
    T_flags_ne_nstr(a, e, n, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksChar Character Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for characters (char).
 *
 * @{
 */

#define T_eq_char(a, e) T_flags_eq_char(a, e, 0)
#define T_assert_eq_char(a, e) T_flags_eq_char(a, e, T_CHECK_STOP)
#define T_quiet_eq_char(a, e) T_flags_eq_char(a, e, T_CHECK_QUIET)
#define T_step_eq_char(s, a, e) T_flags_eq_char(a, e, T_CHECK_STEP(s))
#define T_step_assert_eq_char(s, a, e) \
    T_flags_eq_char(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_char(a, e) T_flags_ne_char(a, e, 0)
#define T_assert_ne_char(a, e) T_flags_ne_char(a, e, T_CHECK_STOP)
#define T_quiet_ne_char(a, e) T_flags_ne_char(a, e, T_CHECK_QUIET)
#define T_step_ne_char(s, a, e) T_flags_ne_char(a, e, T_CHECK_STEP(s))
#define T_step_assert_ne_char(s, a, e) \
    T_flags_ne_char(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksSChar Signed Character Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for signed characters (signed char).
 *
 * @{
 */

#define T_eq_schar(a, e) T_flags_eq_int(a, e, 0)
#define T_assert_eq_schar(a, e) T_flags_eq_int(a, e, T_CHECK_STOP)
#define T_quiet_eq_schar(a, e) T_flags_eq_int(a, e, T_CHECK_QUIET)
#define T_step_eq_schar(s, a, e) T_flags_eq_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_eq_schar(s, a, e) \
    T_flags_eq_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_schar(a, e) T_flags_ne_int(a, e, 0)
#define T_assert_ne_schar(a, e) T_flags_ne_int(a, e, T_CHECK_STOP)
#define T_quiet_ne_schar(a, e) T_flags_ne_int(a, e, T_CHECK_QUIET)
#define T_step_ne_schar(s, a, e) T_flags_ne_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_ne_schar(s, a, e) \
    T_flags_ne_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ge_schar(a, e) T_flags_ge_int(a, e, 0)
#define T_assert_ge_schar(a, e) T_flags_ge_int(a, e, T_CHECK_STOP)
#define T_quiet_ge_schar(a, e) T_flags_ge_int(a, e, T_CHECK_QUIET)
#define T_step_ge_schar(s, a, e) T_flags_ge_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_ge_schar(s, a, e) \
    T_flags_ge_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_gt_schar(a, e) T_flags_gt_int(a, e, 0)
#define T_assert_gt_schar(a, e) T_flags_gt_int(a, e, T_CHECK_STOP)
#define T_quiet_gt_schar(a, e) T_flags_gt_int(a, e, T_CHECK_QUIET)
#define T_step_gt_schar(s, a, e) T_flags_gt_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_gt_schar(s, a, e) \
    T_flags_gt_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_le_schar(a, e) T_flags_le_int(a, e, 0)
#define T_assert_le_schar(a, e) T_flags_le_int(a, e, T_CHECK_STOP)
#define T_quiet_le_schar(a, e) T_flags_le_int(a, e, T_CHECK_QUIET)
#define T_step_le_schar(s, a, e) T_flags_le_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_le_schar(s, a, e) \
    T_flags_le_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_lt_schar(a, e) T_flags_lt_int(a, e, 0)
#define T_assert_lt_schar(a, e) T_flags_lt_int(a, e, T_CHECK_STOP)
#define T_quiet_lt_schar(a, e) T_flags_lt_int(a, e, T_CHECK_QUIET)
#define T_step_lt_schar(s, a, e) T_flags_lt_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_lt_schar(s, a, e) \
    T_flags_lt_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksUChar Unsigned Character Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for unsigned characters (unsigned char).
 *
 * @{
 */

#define T_eq_uchar(a, e) T_flags_eq_uint(a, e, 0)
#define T_assert_eq_uchar(a, e) T_flags_eq_uint(a, e, T_CHECK_STOP)
#define T_quiet_eq_uchar(a, e) T_flags_eq_uint(a, e, T_CHECK_QUIET)
#define T_step_eq_uchar(s, a, e) T_flags_eq_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_eq_uchar(s, a, e) \
    T_flags_eq_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_uchar(a, e) T_flags_ne_uint(a, e, 0)
#define T_assert_ne_uchar(a, e) T_flags_ne_uint(a, e, T_CHECK_STOP)
#define T_quiet_ne_uchar(a, e) T_flags_ne_uint(a, e, T_CHECK_QUIET)
#define T_step_ne_uchar(s, a, e) T_flags_ne_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_ne_uchar(s, a, e) \
    T_flags_ne_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ge_uchar(a, e) T_flags_ge_uint(a, e, 0)
#define T_assert_ge_uchar(a, e) T_flags_ge_uint(a, e, T_CHECK_STOP)
#define T_quiet_ge_uchar(a, e) T_flags_ge_uint(a, e, T_CHECK_QUIET)
#define T_step_ge_uchar(s, a, e) T_flags_ge_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_ge_uchar(s, a, e) \
    T_flags_ge_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_gt_uchar(a, e) T_flags_gt_uint(a, e, 0)
#define T_assert_gt_uchar(a, e) T_flags_gt_uint(a, e, T_CHECK_STOP)
#define T_quiet_gt_uchar(a, e) T_flags_gt_uint(a, e, T_CHECK_QUIET)
#define T_step_gt_uchar(s, a, e) T_flags_gt_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_gt_uchar(s, a, e) \
    T_flags_gt_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_le_uchar(a, e) T_flags_le_uint(a, e, 0)
#define T_assert_le_uchar(a, e) T_flags_le_uint(a, e, T_CHECK_STOP)
#define T_quiet_le_uchar(a, e) T_flags_le_uint(a, e, T_CHECK_QUIET)
#define T_step_le_uchar(s, a, e) T_flags_le_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_le_uchar(s, a, e) \
    T_flags_le_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_lt_uchar(a, e) T_flags_lt_uint(a, e, 0)
#define T_assert_lt_uchar(a, e) T_flags_lt_uint(a, e, T_CHECK_STOP)
#define T_quiet_lt_uchar(a, e) T_flags_lt_uint(a, e, T_CHECK_QUIET)
#define T_step_lt_uchar(s, a, e) T_flags_lt_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_lt_uchar(s, a, e) \
    T_flags_lt_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksShort Signed Short Integer Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for signed short integers (short).
 *
 * @{
 */

#define T_eq_short(a, e) T_flags_eq_int(a, e, 0)
#define T_assert_eq_short(a, e) T_flags_eq_int(a, e, T_CHECK_STOP)
#define T_quiet_eq_short(a, e) T_flags_eq_int(a, e, T_CHECK_QUIET)
#define T_step_eq_short(s, a, e) T_flags_eq_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_eq_short(s, a, e) \
    T_flags_eq_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_short(a, e) T_flags_ne_int(a, e, 0)
#define T_assert_ne_short(a, e) T_flags_ne_int(a, e, T_CHECK_STOP)
#define T_quiet_ne_short(a, e) T_flags_ne_int(a, e, T_CHECK_QUIET)
#define T_step_ne_short(s, a, e) T_flags_ne_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_ne_short(s, a, e) \
    T_flags_ne_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ge_short(a, e) T_flags_ge_int(a, e, 0)
#define T_assert_ge_short(a, e) T_flags_ge_int(a, e, T_CHECK_STOP)
#define T_quiet_ge_short(a, e) T_flags_ge_int(a, e, T_CHECK_QUIET)
#define T_step_ge_short(s, a, e) T_flags_ge_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_ge_short(s, a, e) \
    T_flags_ge_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_gt_short(a, e) T_flags_gt_int(a, e, 0)
#define T_assert_gt_short(a, e) T_flags_gt_int(a, e, T_CHECK_STOP)
#define T_quiet_gt_short(a, e) T_flags_gt_int(a, e, T_CHECK_QUIET)
#define T_step_gt_short(s, a, e) T_flags_gt_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_gt_short(s, a, e) \
    T_flags_gt_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_le_short(a, e) T_flags_le_int(a, e, 0)
#define T_assert_le_short(a, e) T_flags_le_int(a, e, T_CHECK_STOP)
#define T_quiet_le_short(a, e) T_flags_le_int(a, e, T_CHECK_QUIET)
#define T_step_le_short(s, a, e) T_flags_le_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_le_short(s, a, e) \
    T_flags_le_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_lt_short(a, e) T_flags_lt_int(a, e, 0)
#define T_assert_lt_short(a, e) T_flags_lt_int(a, e, T_CHECK_STOP)
#define T_quiet_lt_short(a, e) T_flags_lt_int(a, e, T_CHECK_QUIET)
#define T_step_lt_short(s, a, e) T_flags_lt_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_lt_short(s, a, e) \
    T_flags_lt_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksUShort Unsigned Short Integer Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for unsigned short integers (unsigned short).
 *
 * @{
 */

#define T_eq_ushort(a, e) T_flags_eq_uint(a, e, 0)
#define T_assert_eq_ushort(a, e) T_flags_eq_uint(a, e, T_CHECK_STOP)
#define T_quiet_eq_ushort(a, e) T_flags_eq_uint(a, e, T_CHECK_QUIET)
#define T_step_eq_ushort(s, a, e) T_flags_eq_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_eq_ushort(s, a, e) \
    T_flags_eq_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_ushort(a, e) T_flags_ne_uint(a, e, 0)
#define T_assert_ne_ushort(a, e) T_flags_ne_uint(a, e, T_CHECK_STOP)
#define T_quiet_ne_ushort(a, e) T_flags_ne_uint(a, e, T_CHECK_QUIET)
#define T_step_ne_ushort(s, a, e) T_flags_ne_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_ne_ushort(s, a, e) \
    T_flags_ne_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ge_ushort(a, e) T_flags_ge_uint(a, e, 0)
#define T_assert_ge_ushort(a, e) T_flags_ge_uint(a, e, T_CHECK_STOP)
#define T_quiet_ge_ushort(a, e) T_flags_ge_uint(a, e, T_CHECK_QUIET)
#define T_step_ge_ushort(s, a, e) T_flags_ge_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_ge_ushort(s, a, e) \
    T_flags_ge_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_gt_ushort(a, e) T_flags_gt_uint(a, e, 0)
#define T_assert_gt_ushort(a, e) T_flags_gt_uint(a, e, T_CHECK_STOP)
#define T_quiet_gt_ushort(a, e) T_flags_gt_uint(a, e, T_CHECK_QUIET)
#define T_step_gt_ushort(s, a, e) T_flags_gt_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_gt_ushort(s, a, e) \
    T_flags_gt_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_le_ushort(a, e) T_flags_le_uint(a, e, 0)
#define T_assert_le_ushort(a, e) T_flags_le_uint(a, e, T_CHECK_STOP)
#define T_quiet_le_ushort(a, e) T_flags_le_uint(a, e, T_CHECK_QUIET)
#define T_step_le_ushort(s, a, e) T_flags_le_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_le_ushort(s, a, e) \
    T_flags_le_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_lt_ushort(a, e) T_flags_lt_uint(a, e, 0)
#define T_assert_lt_ushort(a, e) T_flags_lt_uint(a, e, T_CHECK_STOP)
#define T_quiet_lt_ushort(a, e) T_flags_lt_uint(a, e, T_CHECK_QUIET)
#define T_step_lt_ushort(s, a, e) T_flags_lt_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_lt_ushort(s, a, e) \
    T_flags_lt_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksInt Signed Integer Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for signed integers (int).
 *
 * @{
 */

#define T_eq_int(a, e) T_flags_eq_int(a, e, 0)
#define T_assert_eq_int(a, e) T_flags_eq_int(a, e, T_CHECK_STOP)
#define T_quiet_eq_int(a, e) T_flags_eq_int(a, e, T_CHECK_QUIET)
#define T_step_eq_int(s, a, e) T_flags_eq_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_eq_int(s, a, e) \
    T_flags_eq_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_int(a, e) T_flags_ne_int(a, e, 0)
#define T_assert_ne_int(a, e) T_flags_ne_int(a, e, T_CHECK_STOP)
#define T_quiet_ne_int(a, e) T_flags_ne_int(a, e, T_CHECK_QUIET)
#define T_step_ne_int(s, a, e) T_flags_ne_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_ne_int(s, a, e) \
    T_flags_ne_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ge_int(a, e) T_flags_ge_int(a, e, 0)
#define T_assert_ge_int(a, e) T_flags_ge_int(a, e, T_CHECK_STOP)
#define T_quiet_ge_int(a, e) T_flags_ge_int(a, e, T_CHECK_QUIET)
#define T_step_ge_int(s, a, e) T_flags_ge_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_ge_int(s, a, e) \
    T_flags_ge_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_gt_int(a, e) T_flags_gt_int(a, e, 0)
#define T_assert_gt_int(a, e) T_flags_gt_int(a, e, T_CHECK_STOP)
#define T_quiet_gt_int(a, e) T_flags_gt_int(a, e, T_CHECK_QUIET)
#define T_step_gt_int(s, a, e) T_flags_gt_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_gt_int(s, a, e) \
    T_flags_gt_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_le_int(a, e) T_flags_le_int(a, e, 0)
#define T_assert_le_int(a, e) T_flags_le_int(a, e, T_CHECK_STOP)
#define T_quiet_le_int(a, e) T_flags_le_int(a, e, T_CHECK_QUIET)
#define T_step_le_int(s, a, e) T_flags_le_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_le_int(s, a, e) \
    T_flags_le_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_lt_int(a, e) T_flags_lt_int(a, e, 0)
#define T_assert_lt_int(a, e) T_flags_lt_int(a, e, T_CHECK_STOP)
#define T_quiet_lt_int(a, e) T_flags_lt_int(a, e, T_CHECK_QUIET)
#define T_step_lt_int(s, a, e) T_flags_lt_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_lt_int(s, a, e) \
    T_flags_lt_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksUInt Unsigned Integer Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for unsigned integers (unsigned int).
 *
 * @{
 */

#define T_eq_uint(a, e) T_flags_eq_uint(a, e, 0)
#define T_assert_eq_uint(a, e) T_flags_eq_uint(a, e, T_CHECK_STOP)
#define T_quiet_eq_uint(a, e) T_flags_eq_uint(a, e, T_CHECK_QUIET)
#define T_step_eq_uint(s, a, e) T_flags_eq_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_eq_uint(s, a, e) \
    T_flags_eq_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_uint(a, e) T_flags_ne_uint(a, e, 0)
#define T_assert_ne_uint(a, e) T_flags_ne_uint(a, e, T_CHECK_STOP)
#define T_quiet_ne_uint(a, e) T_flags_ne_uint(a, e, T_CHECK_QUIET)
#define T_step_ne_uint(s, a, e) T_flags_ne_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_ne_uint(s, a, e) \
    T_flags_ne_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ge_uint(a, e) T_flags_ge_uint(a, e, 0)
#define T_assert_ge_uint(a, e) T_flags_ge_uint(a, e, T_CHECK_STOP)
#define T_quiet_ge_uint(a, e) T_flags_ge_uint(a, e, T_CHECK_QUIET)
#define T_step_ge_uint(s, a, e) T_flags_ge_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_ge_uint(s, a, e) \
    T_flags_ge_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_gt_uint(a, e) T_flags_gt_uint(a, e, 0)
#define T_assert_gt_uint(a, e) T_flags_gt_uint(a, e, T_CHECK_STOP)
#define T_quiet_gt_uint(a, e) T_flags_gt_uint(a, e, T_CHECK_QUIET)
#define T_step_gt_uint(s, a, e) T_flags_gt_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_gt_uint(s, a, e) \
    T_flags_gt_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_le_uint(a, e) T_flags_le_uint(a, e, 0)
#define T_assert_le_uint(a, e) T_flags_le_uint(a, e, T_CHECK_STOP)
#define T_quiet_le_uint(a, e) T_flags_le_uint(a, e, T_CHECK_QUIET)
#define T_step_le_uint(s, a, e) T_flags_le_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_le_uint(s, a, e) \
    T_flags_le_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_lt_uint(a, e) T_flags_lt_uint(a, e, 0)
#define T_assert_lt_uint(a, e) T_flags_lt_uint(a, e, T_CHECK_STOP)
#define T_quiet_lt_uint(a, e) T_flags_lt_uint(a, e, T_CHECK_QUIET)
#define T_step_lt_uint(s, a, e) T_flags_lt_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_lt_uint(s, a, e) \
    T_flags_lt_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksLong Signed Long Integer Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for signed long integers (long).
 *
 * @{
 */

#define T_eq_long(a, e) T_flags_eq_long(a, e, 0)
#define T_assert_eq_long(a, e) T_flags_eq_long(a, e, T_CHECK_STOP)
#define T_quiet_eq_long(a, e) T_flags_eq_long(a, e, T_CHECK_QUIET)
#define T_step_eq_long(s, a, e) T_flags_eq_long(a, e, T_CHECK_STEP(s))
#define T_step_assert_eq_long(s, a, e) \
    T_flags_eq_long(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_long(a, e) T_flags_ne_long(a, e, 0)
#define T_assert_ne_long(a, e) T_flags_ne_long(a, e, T_CHECK_STOP)
#define T_quiet_ne_long(a, e) T_flags_ne_long(a, e, T_CHECK_QUIET)
#define T_step_ne_long(s, a, e) T_flags_ne_long(a, e, T_CHECK_STEP(s))
#define T_step_assert_ne_long(s, a, e) \
    T_flags_ne_long(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ge_long(a, e) T_flags_ge_long(a, e, 0)
#define T_assert_ge_long(a, e) T_flags_ge_long(a, e, T_CHECK_STOP)
#define T_quiet_ge_long(a, e) T_flags_ge_long(a, e, T_CHECK_QUIET)
#define T_step_ge_long(s, a, e) T_flags_ge_long(a, e, T_CHECK_STEP(s))
#define T_step_assert_ge_long(s, a, e) \
    T_flags_ge_long(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_gt_long(a, e) T_flags_gt_long(a, e, 0)
#define T_assert_gt_long(a, e) T_flags_gt_long(a, e, T_CHECK_STOP)
#define T_quiet_gt_long(a, e) T_flags_gt_long(a, e, T_CHECK_QUIET)
#define T_step_gt_long(s, a, e) T_flags_gt_long(a, e, T_CHECK_STEP(s))
#define T_step_assert_gt_long(s, a, e) \
    T_flags_gt_long(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_le_long(a, e) T_flags_le_long(a, e, 0)
#define T_assert_le_long(a, e) T_flags_le_long(a, e, T_CHECK_STOP)
#define T_quiet_le_long(a, e) T_flags_le_long(a, e, T_CHECK_QUIET)
#define T_step_le_long(s, a, e) T_flags_le_long(a, e, T_CHECK_STEP(s))
#define T_step_assert_le_long(s, a, e) \
    T_flags_le_long(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_lt_long(a, e) T_flags_lt_long(a, e, 0)
#define T_assert_lt_long(a, e) T_flags_lt_long(a, e, T_CHECK_STOP)
#define T_quiet_lt_long(a, e) T_flags_lt_long(a, e, T_CHECK_QUIET)
#define T_step_lt_long(s, a, e) T_flags_lt_long(a, e, T_CHECK_STEP(s))
#define T_step_assert_lt_long(s, a, e) \
    T_flags_lt_long(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksULong Unsigned Long Integer Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for unsigned long integers (unsigned long).
 *
 * @{
 */

#define T_eq_ulong(a, e) T_flags_eq_ulong(a, e, 0)
#define T_assert_eq_ulong(a, e) T_flags_eq_ulong(a, e, T_CHECK_STOP)
#define T_quiet_eq_ulong(a, e) T_flags_eq_ulong(a, e, T_CHECK_QUIET)
#define T_step_eq_ulong(s, a, e) T_flags_eq_ulong(a, e, T_CHECK_STEP(s))
#define T_step_assert_eq_ulong(s, a, e) \
    T_flags_eq_ulong(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_ulong(a, e) T_flags_ne_ulong(a, e, 0)
#define T_assert_ne_ulong(a, e) T_flags_ne_ulong(a, e, T_CHECK_STOP)
#define T_quiet_ne_ulong(a, e) T_flags_ne_ulong(a, e, T_CHECK_QUIET)
#define T_step_ne_ulong(s, a, e) T_flags_ne_ulong(a, e, T_CHECK_STEP(s))
#define T_step_assert_ne_ulong(s, a, e) \
    T_flags_ne_ulong(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ge_ulong(a, e) T_flags_ge_ulong(a, e, 0)
#define T_assert_ge_ulong(a, e) T_flags_ge_ulong(a, e, T_CHECK_STOP)
#define T_quiet_ge_ulong(a, e) T_flags_ge_ulong(a, e, T_CHECK_QUIET)
#define T_step_ge_ulong(s, a, e) T_flags_ge_ulong(a, e, T_CHECK_STEP(s))
#define T_step_assert_ge_ulong(s, a, e) \
    T_flags_ge_ulong(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_gt_ulong(a, e) T_flags_gt_ulong(a, e, 0)
#define T_assert_gt_ulong(a, e) T_flags_gt_ulong(a, e, T_CHECK_STOP)
#define T_quiet_gt_ulong(a, e) T_flags_gt_ulong(a, e, T_CHECK_QUIET)
#define T_step_gt_ulong(s, a, e) T_flags_gt_ulong(a, e, T_CHECK_STEP(s))
#define T_step_assert_gt_ulong(s, a, e) \
    T_flags_gt_ulong(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_le_ulong(a, e) T_flags_le_ulong(a, e, 0)
#define T_assert_le_ulong(a, e) T_flags_le_ulong(a, e, T_CHECK_STOP)
#define T_quiet_le_ulong(a, e) T_flags_le_ulong(a, e, T_CHECK_QUIET)
#define T_step_le_ulong(s, a, e) T_flags_le_ulong(a, e, T_CHECK_STEP(s))
#define T_step_assert_le_ulong(s, a, e) \
    T_flags_le_ulong(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_lt_ulong(a, e) T_flags_lt_ulong(a, e, 0)
#define T_assert_lt_ulong(a, e) T_flags_lt_ulong(a, e, T_CHECK_STOP)
#define T_quiet_lt_ulong(a, e) T_flags_lt_ulong(a, e, T_CHECK_QUIET)
#define T_step_lt_ulong(s, a, e) T_flags_lt_ulong(a, e, T_CHECK_STEP(s))
#define T_step_assert_lt_ulong(s, a, e) \
    T_flags_lt_ulong(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksLong Signed Long Long Integer Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for signed long long integers (long long).
 *
 * @{
 */

#define T_eq_ll(a, e) T_flags_eq_ll(a, e, 0)
#define T_assert_eq_ll(a, e) T_flags_eq_ll(a, e, T_CHECK_STOP)
#define T_quiet_eq_ll(a, e) T_flags_eq_ll(a, e, T_CHECK_QUIET)
#define T_step_eq_ll(s, a, e) T_flags_eq_ll(a, e, T_CHECK_STEP(s))
#define T_step_assert_eq_ll(s, a, e) \
    T_flags_eq_ll(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_ll(a, e) T_flags_ne_ll(a, e, 0)
#define T_assert_ne_ll(a, e) T_flags_ne_ll(a, e, T_CHECK_STOP)
#define T_quiet_ne_ll(a, e) T_flags_ne_ll(a, e, T_CHECK_QUIET)
#define T_step_ne_ll(s, a, e) T_flags_ne_ll(a, e, T_CHECK_STEP(s))
#define T_step_assert_ne_ll(s, a, e) \
    T_flags_ne_ll(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ge_ll(a, e) T_flags_ge_ll(a, e, 0)
#define T_assert_ge_ll(a, e) T_flags_ge_ll(a, e, T_CHECK_STOP)
#define T_quiet_ge_ll(a, e) T_flags_ge_ll(a, e, T_CHECK_QUIET)
#define T_step_ge_ll(s, a, e) T_flags_ge_ll(a, e, T_CHECK_STEP(s))
#define T_step_assert_ge_ll(s, a, e) \
    T_flags_ge_ll(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_gt_ll(a, e) T_flags_gt_ll(a, e, 0)
#define T_assert_gt_ll(a, e) T_flags_gt_ll(a, e, T_CHECK_STOP)
#define T_quiet_gt_ll(a, e) T_flags_gt_ll(a, e, T_CHECK_QUIET)
#define T_step_gt_ll(s, a, e) T_flags_gt_ll(a, e, T_CHECK_STEP(s))
#define T_step_assert_gt_ll(s, a, e) \
    T_flags_gt_ll(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_le_ll(a, e) T_flags_le_ll(a, e, 0)
#define T_assert_le_ll(a, e) T_flags_le_ll(a, e, T_CHECK_STOP)
#define T_quiet_le_ll(a, e) T_flags_le_ll(a, e, T_CHECK_QUIET)
#define T_step_le_ll(s, a, e) T_flags_le_ll(a, e, T_CHECK_STEP(s))
#define T_step_assert_le_ll(s, a, e) \
    T_flags_le_ll(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_lt_ll(a, e) T_flags_lt_ll(a, e, 0)
#define T_assert_lt_ll(a, e) T_flags_lt_ll(a, e, T_CHECK_STOP)
#define T_quiet_lt_ll(a, e) T_flags_lt_ll(a, e, T_CHECK_QUIET)
#define T_step_lt_ll(s, a, e) T_flags_lt_ll(a, e, T_CHECK_STEP(s))
#define T_step_assert_lt_ll(s, a, e) \
    T_flags_lt_ll(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksULongLong Unsigned Long Long Integer Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for unsigned long long integers (unsigned long long).
 *
 * @{
 */

#define T_eq_ull(a, e) T_flags_eq_ull(a, e, 0)
#define T_assert_eq_ull(a, e) T_flags_eq_ull(a, e, T_CHECK_STOP)
#define T_quiet_eq_ull(a, e) T_flags_eq_ull(a, e, T_CHECK_QUIET)
#define T_step_eq_ull(s, a, e) T_flags_eq_ull(a, e, T_CHECK_STEP(s))
#define T_step_assert_eq_ull(s, a, e) \
    T_flags_eq_ull(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_ull(a, e) T_flags_ne_ull(a, e, 0)
#define T_assert_ne_ull(a, e) T_flags_ne_ull(a, e, T_CHECK_STOP)
#define T_quiet_ne_ull(a, e) T_flags_ne_ull(a, e, T_CHECK_QUIET)
#define T_step_ne_ull(s, a, e) T_flags_ne_ull(a, e, T_CHECK_STEP(s))
#define T_step_assert_ne_ull(s, a, e) \
    T_flags_ne_ull(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ge_ull(a, e) T_flags_ge_ull(a, e, 0)
#define T_assert_ge_ull(a, e) T_flags_ge_ull(a, e, T_CHECK_STOP)
#define T_quiet_ge_ull(a, e) T_flags_ge_ull(a, e, T_CHECK_QUIET)
#define T_step_ge_ull(s, a, e) T_flags_ge_ull(a, e, T_CHECK_STEP(s))
#define T_step_assert_ge_ull(s, a, e) \
    T_flags_ge_ull(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_gt_ull(a, e) T_flags_gt_ull(a, e, 0)
#define T_assert_gt_ull(a, e) T_flags_gt_ull(a, e, T_CHECK_STOP)
#define T_quiet_gt_ull(a, e) T_flags_gt_ull(a, e, T_CHECK_QUIET)
#define T_step_gt_ull(s, a, e) T_flags_gt_ull(a, e, T_CHECK_STEP(s))
#define T_step_assert_gt_ull(s, a, e) \
    T_flags_gt_ull(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_le_ull(a, e) T_flags_le_ull(a, e, 0)
#define T_assert_le_ull(a, e) T_flags_le_ull(a, e, T_CHECK_STOP)
#define T_quiet_le_ull(a, e) T_flags_le_ull(a, e, T_CHECK_QUIET)
#define T_step_le_ull(s, a, e) T_flags_le_ull(a, e, T_CHECK_STEP(s))
#define T_step_assert_le_ull(s, a, e) \
    T_flags_le_ull(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_lt_ull(a, e) T_flags_lt_ull(a, e, 0)
#define T_assert_lt_ull(a, e) T_flags_lt_ull(a, e, T_CHECK_STOP)
#define T_quiet_lt_ull(a, e) T_flags_lt_ull(a, e, T_CHECK_QUIET)
#define T_step_lt_ull(s, a, e) T_flags_lt_ull(a, e, T_CHECK_STEP(s))
#define T_step_assert_lt_ull(s, a, e) \
    T_flags_lt_ull(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksInt8 Signed 8-Bit Integer Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for signed 8-bit integers (int8_t).
 *
 * @{
 */

#define T_eq_i8(a, e) T_flags_eq_int(a, e, 0)
#define T_assert_eq_i8(a, e) T_flags_eq_int(a, e, T_CHECK_STOP)
#define T_quiet_eq_i8(a, e) T_flags_eq_int(a, e, T_CHECK_QUIET)
#define T_step_eq_i8(s, a, e) T_flags_eq_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_eq_i8(s, a, e) \
    T_flags_eq_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_i8(a, e) T_flags_ne_int(a, e, 0)
#define T_assert_ne_i8(a, e) T_flags_ne_int(a, e, T_CHECK_STOP)
#define T_quiet_ne_i8(a, e) T_flags_ne_int(a, e, T_CHECK_QUIET)
#define T_step_ne_i8(s, a, e) T_flags_ne_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_ne_i8(s, a, e) \
    T_flags_ne_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ge_i8(a, e) T_flags_ge_int(a, e, 0)
#define T_assert_ge_i8(a, e) T_flags_ge_int(a, e, T_CHECK_STOP)
#define T_quiet_ge_i8(a, e) T_flags_ge_int(a, e, T_CHECK_QUIET)
#define T_step_ge_i8(s, a, e) T_flags_ge_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_ge_i8(s, a, e) \
    T_flags_ge_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_gt_i8(a, e) T_flags_gt_int(a, e, 0)
#define T_assert_gt_i8(a, e) T_flags_gt_int(a, e, T_CHECK_STOP)
#define T_quiet_gt_i8(a, e) T_flags_gt_int(a, e, T_CHECK_QUIET)
#define T_step_gt_i8(s, a, e) T_flags_gt_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_gt_i8(s, a, e) \
    T_flags_gt_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_le_i8(a, e) T_flags_le_int(a, e, 0)
#define T_assert_le_i8(a, e) T_flags_le_int(a, e, T_CHECK_STOP)
#define T_quiet_le_i8(a, e) T_flags_le_int(a, e, T_CHECK_QUIET)
#define T_step_le_i8(s, a, e) T_flags_le_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_le_i8(s, a, e) \
    T_flags_le_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_lt_i8(a, e) T_flags_lt_int(a, e, 0)
#define T_assert_lt_i8(a, e) T_flags_lt_int(a, e, T_CHECK_STOP)
#define T_quiet_lt_i8(a, e) T_flags_lt_int(a, e, T_CHECK_QUIET)
#define T_step_lt_i8(s, a, e) T_flags_lt_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_lt_i8(s, a, e) \
    T_flags_lt_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksUInt8 Unsigned 8-Bit Integer Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for unsigned 8-bit integers (uint8_t).
 *
 * @{
 */

#define T_eq_u8(a, e) T_flags_eq_uint(a, e, 0)
#define T_assert_eq_u8(a, e) T_flags_eq_uint(a, e, T_CHECK_STOP)
#define T_quiet_eq_u8(a, e) T_flags_eq_uint(a, e, T_CHECK_QUIET)
#define T_step_eq_u8(s, a, e) T_flags_eq_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_eq_u8(s, a, e) \
    T_flags_eq_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_u8(a, e) T_flags_ne_uint(a, e, 0)
#define T_assert_ne_u8(a, e) T_flags_ne_uint(a, e, T_CHECK_STOP)
#define T_quiet_ne_u8(a, e) T_flags_ne_uint(a, e, T_CHECK_QUIET)
#define T_step_ne_u8(s, a, e) T_flags_ne_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_ne_u8(s, a, e) \
    T_flags_ne_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ge_u8(a, e) T_flags_ge_uint(a, e, 0)
#define T_assert_ge_u8(a, e) T_flags_ge_uint(a, e, T_CHECK_STOP)
#define T_quiet_ge_u8(a, e) T_flags_ge_uint(a, e, T_CHECK_QUIET)
#define T_step_ge_u8(s, a, e) T_flags_ge_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_ge_u8(s, a, e) \
    T_flags_ge_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_gt_u8(a, e) T_flags_gt_uint(a, e, 0)
#define T_assert_gt_u8(a, e) T_flags_gt_uint(a, e, T_CHECK_STOP)
#define T_quiet_gt_u8(a, e) T_flags_gt_uint(a, e, T_CHECK_QUIET)
#define T_step_gt_u8(s, a, e) T_flags_gt_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_gt_u8(s, a, e) \
    T_flags_gt_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_le_u8(a, e) T_flags_le_uint(a, e, 0)
#define T_assert_le_u8(a, e) T_flags_le_uint(a, e, T_CHECK_STOP)
#define T_quiet_le_u8(a, e) T_flags_le_uint(a, e, T_CHECK_QUIET)
#define T_step_le_u8(s, a, e) T_flags_le_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_le_u8(s, a, e) \
    T_flags_le_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_lt_u8(a, e) T_flags_lt_uint(a, e, 0)
#define T_assert_lt_u8(a, e) T_flags_lt_uint(a, e, T_CHECK_STOP)
#define T_quiet_lt_u8(a, e) T_flags_lt_uint(a, e, T_CHECK_QUIET)
#define T_step_lt_u8(s, a, e) T_flags_lt_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_lt_u8(s, a, e) \
    T_flags_lt_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksInt16 Signed 16-Bit Integer Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for signed 16-bit integers (int16_t).
 *
 * @{
 */

#define T_eq_i16(a, e) T_flags_eq_int(a, e, 0)
#define T_assert_eq_i16(a, e) T_flags_eq_int(a, e, T_CHECK_STOP)
#define T_quiet_eq_i16(a, e) T_flags_eq_int(a, e, T_CHECK_QUIET)
#define T_step_eq_i16(s, a, e) T_flags_eq_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_eq_i16(s, a, e) \
    T_flags_eq_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_i16(a, e) T_flags_ne_int(a, e, 0)
#define T_assert_ne_i16(a, e) T_flags_ne_int(a, e, T_CHECK_STOP)
#define T_quiet_ne_i16(a, e) T_flags_ne_int(a, e, T_CHECK_QUIET)
#define T_step_ne_i16(s, a, e) T_flags_ne_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_ne_i16(s, a, e) \
    T_flags_ne_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ge_i16(a, e) T_flags_ge_int(a, e, 0)
#define T_assert_ge_i16(a, e) T_flags_ge_int(a, e, T_CHECK_STOP)
#define T_quiet_ge_i16(a, e) T_flags_ge_int(a, e, T_CHECK_QUIET)
#define T_step_ge_i16(s, a, e) T_flags_ge_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_ge_i16(s, a, e) \
    T_flags_ge_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_gt_i16(a, e) T_flags_gt_int(a, e, 0)
#define T_assert_gt_i16(a, e) T_flags_gt_int(a, e, T_CHECK_STOP)
#define T_quiet_gt_i16(a, e) T_flags_gt_int(a, e, T_CHECK_QUIET)
#define T_step_gt_i16(s, a, e) T_flags_gt_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_gt_i16(s, a, e) \
    T_flags_gt_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_le_i16(a, e) T_flags_le_int(a, e, 0)
#define T_assert_le_i16(a, e) T_flags_le_int(a, e, T_CHECK_STOP)
#define T_quiet_le_i16(a, e) T_flags_le_int(a, e, T_CHECK_QUIET)
#define T_step_le_i16(s, a, e) T_flags_le_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_le_i16(s, a, e) \
    T_flags_le_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_lt_i16(a, e) T_flags_lt_int(a, e, 0)
#define T_assert_lt_i16(a, e) T_flags_lt_int(a, e, T_CHECK_STOP)
#define T_quiet_lt_i16(a, e) T_flags_lt_int(a, e, T_CHECK_QUIET)
#define T_step_lt_i16(s, a, e) T_flags_lt_int(a, e, T_CHECK_STEP(s))
#define T_step_assert_lt_i16(s, a, e) \
    T_flags_lt_int(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksUInt16 Unsigned 16-Bit Integer Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for unsigned 16-bit integers (uint16_t).
 *
 * @{
 */

#define T_eq_u16(a, e) T_flags_eq_uint(a, e, 0)
#define T_assert_eq_u16(a, e) T_flags_eq_uint(a, e, T_CHECK_STOP)
#define T_quiet_eq_u16(a, e) T_flags_eq_uint(a, e, T_CHECK_QUIET)
#define T_step_eq_u16(s, a, e) T_flags_eq_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_eq_u16(s, a, e) \
    T_flags_eq_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_u16(a, e) T_flags_ne_uint(a, e, 0)
#define T_assert_ne_u16(a, e) T_flags_ne_uint(a, e, T_CHECK_STOP)
#define T_quiet_ne_u16(a, e) T_flags_ne_uint(a, e, T_CHECK_QUIET)
#define T_step_ne_u16(s, a, e) T_flags_ne_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_ne_u16(s, a, e) \
    T_flags_ne_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ge_u16(a, e) T_flags_ge_uint(a, e, 0)
#define T_assert_ge_u16(a, e) T_flags_ge_uint(a, e, T_CHECK_STOP)
#define T_quiet_ge_u16(a, e) T_flags_ge_uint(a, e, T_CHECK_QUIET)
#define T_step_ge_u16(s, a, e) T_flags_ge_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_ge_u16(s, a, e) \
    T_flags_ge_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_gt_u16(a, e) T_flags_gt_uint(a, e, 0)
#define T_assert_gt_u16(a, e) T_flags_gt_uint(a, e, T_CHECK_STOP)
#define T_quiet_gt_u16(a, e) T_flags_gt_uint(a, e, T_CHECK_QUIET)
#define T_step_gt_u16(s, a, e) T_flags_gt_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_gt_u16(s, a, e) \
    T_flags_gt_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_le_u16(a, e) T_flags_le_uint(a, e, 0)
#define T_assert_le_u16(a, e) T_flags_le_uint(a, e, T_CHECK_STOP)
#define T_quiet_le_u16(a, e) T_flags_le_uint(a, e, T_CHECK_QUIET)
#define T_step_le_u16(s, a, e) T_flags_le_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_le_u16(s, a, e) \
    T_flags_le_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_lt_u16(a, e) T_flags_lt_uint(a, e, 0)
#define T_assert_lt_u16(a, e) T_flags_lt_uint(a, e, T_CHECK_STOP)
#define T_quiet_lt_u16(a, e) T_flags_lt_uint(a, e, T_CHECK_QUIET)
#define T_step_lt_u16(s, a, e) T_flags_lt_uint(a, e, T_CHECK_STEP(s))
#define T_step_assert_lt_u16(s, a, e) \
    T_flags_lt_uint(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksInt32 Signed 32-Bit Integer Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for signed 32-bit integers (int32_t).
 *
 * @{
 */

#define T_eq_i32(a, e) T_flags_eq_long(a, e, 0)
#define T_assert_eq_i32(a, e) T_flags_eq_long(a, e, T_CHECK_STOP)
#define T_quiet_eq_i32(a, e) T_flags_eq_long(a, e, T_CHECK_QUIET)
#define T_step_eq_i32(s, a, e) T_flags_eq_long(a, e, T_CHECK_STEP(s))
#define T_step_assert_eq_i32(s, a, e) \
    T_flags_eq_long(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_i32(a, e) T_flags_ne_long(a, e, 0)
#define T_assert_ne_i32(a, e) T_flags_ne_long(a, e, T_CHECK_STOP)
#define T_quiet_ne_i32(a, e) T_flags_ne_long(a, e, T_CHECK_QUIET)
#define T_step_ne_i32(s, a, e) T_flags_ne_long(a, e, T_CHECK_STEP(s))
#define T_step_assert_ne_i32(s, a, e) \
    T_flags_ne_long(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ge_i32(a, e) T_flags_ge_long(a, e, 0)
#define T_assert_ge_i32(a, e) T_flags_ge_long(a, e, T_CHECK_STOP)
#define T_quiet_ge_i32(a, e) T_flags_ge_long(a, e, T_CHECK_QUIET)
#define T_step_ge_i32(s, a, e) T_flags_ge_long(a, e, T_CHECK_STEP(s))
#define T_step_assert_ge_i32(s, a, e) \
    T_flags_ge_long(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_gt_i32(a, e) T_flags_gt_long(a, e, 0)
#define T_assert_gt_i32(a, e) T_flags_gt_long(a, e, T_CHECK_STOP)
#define T_quiet_gt_i32(a, e) T_flags_gt_long(a, e, T_CHECK_QUIET)
#define T_step_gt_i32(s, a, e) T_flags_gt_long(a, e, T_CHECK_STEP(s))
#define T_step_assert_gt_i32(s, a, e) \
    T_flags_gt_long(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_le_i32(a, e) T_flags_le_long(a, e, 0)
#define T_assert_le_i32(a, e) T_flags_le_long(a, e, T_CHECK_STOP)
#define T_quiet_le_i32(a, e) T_flags_le_long(a, e, T_CHECK_QUIET)
#define T_step_le_i32(s, a, e) T_flags_le_long(a, e, T_CHECK_STEP(s))
#define T_step_assert_le_i32(s, a, e) \
    T_flags_le_long(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_lt_i32(a, e) T_flags_lt_long(a, e, 0)
#define T_assert_lt_i32(a, e) T_flags_lt_long(a, e, T_CHECK_STOP)
#define T_quiet_lt_i32(a, e) T_flags_lt_long(a, e, T_CHECK_QUIET)
#define T_step_lt_i32(s, a, e) T_flags_lt_long(a, e, T_CHECK_STEP(s))
#define T_step_assert_lt_i32(s, a, e) \
    T_flags_lt_long(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksUInt32 Unsigned 32-Bit Integer Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for unsigned 32-bit integers (uint32_t).
 *
 * @{
 */

#define T_eq_u32(a, e) T_flags_eq_ulong(a, e, 0)
#define T_assert_eq_u32(a, e) T_flags_eq_ulong(a, e, T_CHECK_STOP)
#define T_quiet_eq_u32(a, e) T_flags_eq_ulong(a, e, T_CHECK_QUIET)
#define T_step_eq_u32(s, a, e) T_flags_eq_ulong(a, e, T_CHECK_STEP(s))
#define T_step_assert_eq_u32(s, a, e) \
    T_flags_eq_ulong(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_u32(a, e) T_flags_ne_ulong(a, e, 0)
#define T_assert_ne_u32(a, e) T_flags_ne_ulong(a, e, T_CHECK_STOP)
#define T_quiet_ne_u32(a, e) T_flags_ne_ulong(a, e, T_CHECK_QUIET)
#define T_step_ne_u32(s, a, e) T_flags_ne_ulong(a, e, T_CHECK_STEP(s))
#define T_step_assert_ne_u32(s, a, e) \
    T_flags_ne_ulong(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ge_u32(a, e) T_flags_ge_ulong(a, e, 0)
#define T_assert_ge_u32(a, e) T_flags_ge_ulong(a, e, T_CHECK_STOP)
#define T_quiet_ge_u32(a, e) T_flags_ge_ulong(a, e, T_CHECK_QUIET)
#define T_step_ge_u32(s, a, e) T_flags_ge_ulong(a, e, T_CHECK_STEP(s))
#define T_step_assert_ge_u32(s, a, e) \
    T_flags_ge_ulong(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_gt_u32(a, e) T_flags_gt_ulong(a, e, 0)
#define T_assert_gt_u32(a, e) T_flags_gt_ulong(a, e, T_CHECK_STOP)
#define T_quiet_gt_u32(a, e) T_flags_gt_ulong(a, e, T_CHECK_QUIET)
#define T_step_gt_u32(s, a, e) T_flags_gt_ulong(a, e, T_CHECK_STEP(s))
#define T_step_assert_gt_u32(s, a, e) \
    T_flags_gt_ulong(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_le_u32(a, e) T_flags_le_ulong(a, e, 0)
#define T_assert_le_u32(a, e) T_flags_le_ulong(a, e, T_CHECK_STOP)
#define T_quiet_le_u32(a, e) T_flags_le_ulong(a, e, T_CHECK_QUIET)
#define T_step_le_u32(s, a, e) T_flags_le_ulong(a, e, T_CHECK_STEP(s))
#define T_step_assert_le_u32(s, a, e) \
    T_flags_le_ulong(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_lt_u32(a, e) T_flags_lt_ulong(a, e, 0)
#define T_assert_lt_u32(a, e) T_flags_lt_ulong(a, e, T_CHECK_STOP)
#define T_quiet_lt_u32(a, e) T_flags_lt_ulong(a, e, T_CHECK_QUIET)
#define T_step_lt_u32(s, a, e) T_flags_lt_ulong(a, e, T_CHECK_STEP(s))
#define T_step_assert_lt_u32(s, a, e) \
    T_flags_lt_ulong(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksInt64 Signed 64-Bit Integer Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for signed 64-bit integers (int64_t).
 *
 * @{
 */

#define T_eq_i64(a, e) T_flags_eq_ll(a, e, 0)
#define T_assert_eq_i64(a, e) T_flags_eq_ll(a, e, T_CHECK_STOP)
#define T_quiet_eq_i64(a, e) T_flags_eq_ll(a, e, T_CHECK_QUIET)
#define T_step_eq_i64(s, a, e) T_flags_eq_ll(a, e, T_CHECK_STEP(s))
#define T_step_assert_eq_i64(s, a, e) \
    T_flags_eq_ll(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_i64(a, e) T_flags_ne_ll(a, e, 0)
#define T_assert_ne_i64(a, e) T_flags_ne_ll(a, e, T_CHECK_STOP)
#define T_quiet_ne_i64(a, e) T_flags_ne_ll(a, e, T_CHECK_QUIET)
#define T_step_ne_i64(s, a, e) T_flags_ne_ll(a, e, T_CHECK_STEP(s))
#define T_step_assert_ne_i64(s, a, e) \
    T_flags_ne_ll(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ge_i64(a, e) T_flags_ge_ll(a, e, 0)
#define T_assert_ge_i64(a, e) T_flags_ge_ll(a, e, T_CHECK_STOP)
#define T_quiet_ge_i64(a, e) T_flags_ge_ll(a, e, T_CHECK_QUIET)
#define T_step_ge_i64(s, a, e) T_flags_ge_ll(a, e, T_CHECK_STEP(s))
#define T_step_assert_ge_i64(s, a, e) \
    T_flags_ge_ll(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_gt_i64(a, e) T_flags_gt_ll(a, e, 0)
#define T_assert_gt_i64(a, e) T_flags_gt_ll(a, e, T_CHECK_STOP)
#define T_quiet_gt_i64(a, e) T_flags_gt_ll(a, e, T_CHECK_QUIET)
#define T_step_gt_i64(s, a, e) T_flags_gt_ll(a, e, T_CHECK_STEP(s))
#define T_step_assert_gt_i64(s, a, e) \
    T_flags_gt_ll(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_le_i64(a, e) T_flags_le_ll(a, e, 0)
#define T_assert_le_i64(a, e) T_flags_le_ll(a, e, T_CHECK_STOP)
#define T_quiet_le_i64(a, e) T_flags_le_ll(a, e, T_CHECK_QUIET)
#define T_step_le_i64(s, a, e) T_flags_le_ll(a, e, T_CHECK_STEP(s))
#define T_step_assert_le_i64(s, a, e) \
    T_flags_le_ll(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_lt_i64(a, e) T_flags_lt_ll(a, e, 0)
#define T_assert_lt_i64(a, e) T_flags_lt_ll(a, e, T_CHECK_STOP)
#define T_quiet_lt_i64(a, e) T_flags_lt_ll(a, e, T_CHECK_QUIET)
#define T_step_lt_i64(s, a, e) T_flags_lt_ll(a, e, T_CHECK_STEP(s))
#define T_step_assert_lt_i64(s, a, e) \
    T_flags_lt_ll(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksUInt64 Unsigned 64-Bit Integer Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for unsigned 64-bit integers (uint64_t).
 *
 * @{
 */

#define T_eq_u64(a, e) T_flags_eq_ull(a, e, 0)
#define T_assert_eq_u64(a, e) T_flags_eq_ull(a, e, T_CHECK_STOP)
#define T_quiet_eq_u64(a, e) T_flags_eq_ull(a, e, T_CHECK_QUIET)
#define T_step_eq_u64(s, a, e) T_flags_eq_ull(a, e, T_CHECK_STEP(s))
#define T_step_assert_eq_u64(s, a, e) \
    T_flags_eq_ull(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_u64(a, e) T_flags_ne_ull(a, e, 0)
#define T_assert_ne_u64(a, e) T_flags_ne_ull(a, e, T_CHECK_STOP)
#define T_quiet_ne_u64(a, e) T_flags_ne_ull(a, e, T_CHECK_QUIET)
#define T_step_ne_u64(s, a, e) T_flags_ne_ull(a, e, T_CHECK_STEP(s))
#define T_step_assert_ne_u64(s, a, e) \
    T_flags_ne_ull(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ge_u64(a, e) T_flags_ge_ull(a, e, 0)
#define T_assert_ge_u64(a, e) T_flags_ge_ull(a, e, T_CHECK_STOP)
#define T_quiet_ge_u64(a, e) T_flags_ge_ull(a, e, T_CHECK_QUIET)
#define T_step_ge_u64(s, a, e) T_flags_ge_ull(a, e, T_CHECK_STEP(s))
#define T_step_assert_ge_u64(s, a, e) \
    T_flags_ge_ull(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_gt_u64(a, e) T_flags_gt_ull(a, e, 0)
#define T_assert_gt_u64(a, e) T_flags_gt_ull(a, e, T_CHECK_STOP)
#define T_quiet_gt_u64(a, e) T_flags_gt_ull(a, e, T_CHECK_QUIET)
#define T_step_gt_u64(s, a, e) T_flags_gt_ull(a, e, T_CHECK_STEP(s))
#define T_step_assert_gt_u64(s, a, e) \
    T_flags_gt_ull(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_le_u64(a, e) T_flags_le_ull(a, e, 0)
#define T_assert_le_u64(a, e) T_flags_le_ull(a, e, T_CHECK_STOP)
#define T_quiet_le_u64(a, e) T_flags_le_ull(a, e, T_CHECK_QUIET)
#define T_step_le_u64(s, a, e) T_flags_le_ull(a, e, T_CHECK_STEP(s))
#define T_step_assert_le_u64(s, a, e) \
    T_flags_le_ull(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_lt_u64(a, e) T_flags_lt_ull(a, e, 0)
#define T_assert_lt_u64(a, e) T_flags_lt_ull(a, e, T_CHECK_STOP)
#define T_quiet_lt_u64(a, e) T_flags_lt_ull(a, e, T_CHECK_QUIET)
#define T_step_lt_u64(s, a, e) T_flags_lt_ull(a, e, T_CHECK_STEP(s))
#define T_step_assert_lt_u64(s, a, e) \
    T_flags_lt_ull(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksIntptr Signed Pointer Value Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for signed pointer values (intptr_t).
 *
 * @{
 */

#define T_eq_iptr(a, e) T_flags_eq_long(a, e, 0)
#define T_assert_eq_iptr(a, e) T_flags_eq_long(a, e, T_CHECK_STOP)
#define T_quiet_eq_iptr(a, e) T_flags_eq_long(a, e, T_CHECK_QUIET)
#define T_step_eq_iptr(s, a, e) T_flags_eq_long(a, e, T_CHECK_STEP(s))
#define T_step_assert_eq_iptr(s, a, e) \
    T_flags_eq_long(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_iptr(a, e) T_flags_ne_long(a, e, 0)
#define T_assert_ne_iptr(a, e) T_flags_ne_long(a, e, T_CHECK_STOP)
#define T_quiet_ne_iptr(a, e) T_flags_ne_long(a, e, T_CHECK_QUIET)
#define T_step_ne_iptr(s, a, e) T_flags_ne_long(a, e, T_CHECK_STEP(s))
#define T_step_assert_ne_iptr(s, a, e) \
    T_flags_ne_long(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ge_iptr(a, e) T_flags_ge_long(a, e, 0)
#define T_assert_ge_iptr(a, e) T_flags_ge_long(a, e, T_CHECK_STOP)
#define T_quiet_ge_iptr(a, e) T_flags_ge_long(a, e, T_CHECK_QUIET)
#define T_step_ge_iptr(s, a, e) T_flags_ge_long(a, e, T_CHECK_STEP(s))
#define T_step_assert_ge_iptr(s, a, e) \
    T_flags_ge_long(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_gt_iptr(a, e) T_flags_gt_long(a, e, 0)
#define T_assert_gt_iptr(a, e) T_flags_gt_long(a, e, T_CHECK_STOP)
#define T_quiet_gt_iptr(a, e) T_flags_gt_long(a, e, T_CHECK_QUIET)
#define T_step_gt_iptr(s, a, e) T_flags_gt_long(a, e, T_CHECK_STEP(s))
#define T_step_assert_gt_iptr(s, a, e) \
    T_flags_gt_long(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_le_iptr(a, e) T_flags_le_long(a, e, 0)
#define T_assert_le_iptr(a, e) T_flags_le_long(a, e, T_CHECK_STOP)
#define T_quiet_le_iptr(a, e) T_flags_le_long(a, e, T_CHECK_QUIET)
#define T_step_le_iptr(s, a, e) T_flags_le_long(a, e, T_CHECK_STEP(s))
#define T_step_assert_le_iptr(s, a, e) \
    T_flags_le_long(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_lt_iptr(a, e) T_flags_lt_long(a, e, 0)
#define T_assert_lt_iptr(a, e) T_flags_lt_long(a, e, T_CHECK_STOP)
#define T_quiet_lt_iptr(a, e) T_flags_lt_long(a, e, T_CHECK_QUIET)
#define T_step_lt_iptr(s, a, e) T_flags_lt_long(a, e, T_CHECK_STEP(s))
#define T_step_assert_lt_iptr(s, a, e) \
    T_flags_lt_long(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksUIntptr Unsigned Pointer Value Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for unsigned pointer values (uintptr_t).
 *
 * @{
 */

#define T_eq_uptr(a, e) T_flags_eq_ulong(a, e, 0)
#define T_assert_eq_uptr(a, e) T_flags_eq_ulong(a, e, T_CHECK_STOP)
#define T_quiet_eq_uptr(a, e) T_flags_eq_ulong(a, e, T_CHECK_QUIET)
#define T_step_eq_uptr(s, a, e) T_flags_eq_ulong(a, e, T_CHECK_STEP(s))
#define T_step_assert_eq_uptr(s, a, e) \
    T_flags_eq_ulong(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_uptr(a, e) T_flags_ne_ulong(a, e, 0)
#define T_assert_ne_uptr(a, e) T_flags_ne_ulong(a, e, T_CHECK_STOP)
#define T_quiet_ne_uptr(a, e) T_flags_ne_ulong(a, e, T_CHECK_QUIET)
#define T_step_ne_uptr(s, a, e) T_flags_ne_ulong(a, e, T_CHECK_STEP(s))
#define T_step_assert_ne_uptr(s, a, e) \
    T_flags_ne_ulong(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ge_uptr(a, e) T_flags_ge_ulong(a, e, 0)
#define T_assert_ge_uptr(a, e) T_flags_ge_ulong(a, e, T_CHECK_STOP)
#define T_quiet_ge_uptr(a, e) T_flags_ge_ulong(a, e, T_CHECK_QUIET)
#define T_step_ge_uptr(s, a, e) T_flags_ge_ulong(a, e, T_CHECK_STEP(s))
#define T_step_assert_ge_uptr(s, a, e) \
    T_flags_ge_ulong(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_gt_uptr(a, e) T_flags_gt_ulong(a, e, 0)
#define T_assert_gt_uptr(a, e) T_flags_gt_ulong(a, e, T_CHECK_STOP)
#define T_quiet_gt_uptr(a, e) T_flags_gt_ulong(a, e, T_CHECK_QUIET)
#define T_step_gt_uptr(s, a, e) T_flags_gt_ulong(a, e, T_CHECK_STEP(s))
#define T_step_assert_gt_uptr(s, a, e) \
    T_flags_gt_ulong(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_le_uptr(a, e) T_flags_le_ulong(a, e, 0)
#define T_assert_le_uptr(a, e) T_flags_le_ulong(a, e, T_CHECK_STOP)
#define T_quiet_le_uptr(a, e) T_flags_le_ulong(a, e, T_CHECK_QUIET)
#define T_step_le_uptr(s, a, e) T_flags_le_ulong(a, e, T_CHECK_STEP(s))
#define T_step_assert_le_uptr(s, a, e) \
    T_flags_le_ulong(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_lt_uptr(a, e) T_flags_lt_ulong(a, e, 0)
#define T_assert_lt_uptr(a, e) T_flags_lt_ulong(a, e, T_CHECK_STOP)
#define T_quiet_lt_uptr(a, e) T_flags_lt_ulong(a, e, T_CHECK_QUIET)
#define T_step_lt_uptr(s, a, e) T_flags_lt_ulong(a, e, T_CHECK_STEP(s))
#define T_step_assert_lt_uptr(s, a, e) \
    T_flags_lt_ulong(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksSSZ Signed Size Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for signed sizes (ssize_t).
 *
 * @{
 */

#define T_eq_ssz(a, e) T_flags_eq_long(a, e, 0)
#define T_assert_eq_ssz(a, e) T_flags_eq_long(a, e, T_CHECK_STOP)
#define T_quiet_eq_ssz(a, e) T_flags_eq_long(a, e, T_CHECK_QUIET)
#define T_step_eq_ssz(s, a, e) T_flags_eq_long(a, e, T_CHECK_STEP(s))
#define T_step_assert_eq_ssz(s, a, e) \
    T_flags_eq_long(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_ssz(a, e) T_flags_ne_long(a, e, 0)
#define T_assert_ne_ssz(a, e) T_flags_ne_long(a, e, T_CHECK_STOP)
#define T_quiet_ne_ssz(a, e) T_flags_ne_long(a, e, T_CHECK_QUIET)
#define T_step_ne_ssz(s, a, e) T_flags_ne_long(a, e, T_CHECK_STEP(s))
#define T_step_assert_ne_ssz(s, a, e) \
    T_flags_ne_long(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ge_ssz(a, e) T_flags_ge_long(a, e, 0)
#define T_assert_ge_ssz(a, e) T_flags_ge_long(a, e, T_CHECK_STOP)
#define T_quiet_ge_ssz(a, e) T_flags_ge_long(a, e, T_CHECK_QUIET)
#define T_step_ge_ssz(s, a, e) T_flags_ge_long(a, e, T_CHECK_STEP(s))
#define T_step_assert_ge_ssz(s, a, e) \
    T_flags_ge_long(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_gt_ssz(a, e) T_flags_gt_long(a, e, 0)
#define T_assert_gt_ssz(a, e) T_flags_gt_long(a, e, T_CHECK_STOP)
#define T_quiet_gt_ssz(a, e) T_flags_gt_long(a, e, T_CHECK_QUIET)
#define T_step_gt_ssz(s, a, e) T_flags_gt_long(a, e, T_CHECK_STEP(s))
#define T_step_assert_gt_ssz(s, a, e) \
    T_flags_gt_long(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_le_ssz(a, e) T_flags_le_long(a, e, 0)
#define T_assert_le_ssz(a, e) T_flags_le_long(a, e, T_CHECK_STOP)
#define T_quiet_le_ssz(a, e) T_flags_le_long(a, e, T_CHECK_QUIET)
#define T_step_le_ssz(s, a, e) T_flags_le_long(a, e, T_CHECK_STEP(s))
#define T_step_assert_le_ssz(s, a, e) \
    T_flags_le_long(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_lt_ssz(a, e) T_flags_lt_long(a, e, 0)
#define T_assert_lt_ssz(a, e) T_flags_lt_long(a, e, T_CHECK_STOP)
#define T_quiet_lt_ssz(a, e) T_flags_lt_long(a, e, T_CHECK_QUIET)
#define T_step_lt_ssz(s, a, e) T_flags_lt_long(a, e, T_CHECK_STEP(s))
#define T_step_assert_lt_ssz(s, a, e) \
    T_flags_lt_long(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksSZ Size Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for sizes (size_t).
 *
 * @{
 */

#define T_eq_sz(a, e) T_flags_eq_ulong(a, e, 0)
#define T_assert_eq_sz(a, e) T_flags_eq_ulong(a, e, T_CHECK_STOP)
#define T_quiet_eq_sz(a, e) T_flags_eq_ulong(a, e, T_CHECK_QUIET)
#define T_step_eq_sz(s, a, e) T_flags_eq_ulong(a, e, T_CHECK_STEP(s))
#define T_step_assert_eq_sz(s, a, e) \
    T_flags_eq_ulong(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ne_sz(a, e) T_flags_ne_ulong(a, e, 0)
#define T_assert_ne_sz(a, e) T_flags_ne_ulong(a, e, T_CHECK_STOP)
#define T_quiet_ne_sz(a, e) T_flags_ne_ulong(a, e, T_CHECK_QUIET)
#define T_step_ne_sz(s, a, e) T_flags_ne_ulong(a, e, T_CHECK_STEP(s))
#define T_step_assert_ne_sz(s, a, e) \
    T_flags_ne_ulong(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_ge_sz(a, e) T_flags_ge_ulong(a, e, 0)
#define T_assert_ge_sz(a, e) T_flags_ge_ulong(a, e, T_CHECK_STOP)
#define T_quiet_ge_sz(a, e) T_flags_ge_ulong(a, e, T_CHECK_QUIET)
#define T_step_ge_sz(s, a, e) T_flags_ge_ulong(a, e, T_CHECK_STEP(s))
#define T_step_assert_ge_sz(s, a, e) \
    T_flags_ge_ulong(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_gt_sz(a, e) T_flags_gt_ulong(a, e, 0)
#define T_assert_gt_sz(a, e) T_flags_gt_ulong(a, e, T_CHECK_STOP)
#define T_quiet_gt_sz(a, e) T_flags_gt_ulong(a, e, T_CHECK_QUIET)
#define T_step_gt_sz(s, a, e) T_flags_gt_ulong(a, e, T_CHECK_STEP(s))
#define T_step_assert_gt_sz(s, a, e) \
    T_flags_gt_ulong(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_le_sz(a, e) T_flags_le_ulong(a, e, 0)
#define T_assert_le_sz(a, e) T_flags_le_ulong(a, e, T_CHECK_STOP)
#define T_quiet_le_sz(a, e) T_flags_le_ulong(a, e, T_CHECK_QUIET)
#define T_step_le_sz(s, a, e) T_flags_le_ulong(a, e, T_CHECK_STEP(s))
#define T_step_assert_le_sz(s, a, e) \
    T_flags_le_ulong(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_lt_sz(a, e) T_flags_lt_ulong(a, e, 0)
#define T_assert_lt_sz(a, e) T_flags_lt_ulong(a, e, T_CHECK_STOP)
#define T_quiet_lt_sz(a, e) T_flags_lt_ulong(a, e, T_CHECK_QUIET)
#define T_step_lt_sz(s, a, e) T_flags_lt_ulong(a, e, T_CHECK_STEP(s))
#define T_step_assert_lt_sz(s, a, e) \
    T_flags_lt_ulong(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

/**
 * @defgroup RTEMSTestFrameworkChecksPSX POSIX Status and Error Number Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for POSIX status and error numbers.
 *
 * @{
 */

const char *T_strerror(int);

#define T_eno(a, e) T_flags_eno(a, e, 0)
#define T_assert_eno(a, e) T_flags_eno(a, e, T_CHECK_STOP)
#define T_quiet_eno(a, e) T_flags_eno(a, e, T_CHECK_QUIET)
#define T_step_eno(s, a, e) T_flags_eno(a, e, T_CHECK_STEP(s))
#define T_step_assert_eno(s, a, e) \
    T_flags_eno(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_eno_success(a) T_flags_eno_success(a, 0)
#define T_assert_eno_success(a) T_flags_eno_success(a, T_CHECK_STOP)
#define T_quiet_eno_success(a) T_flags_eno_success(a, T_CHECK_QUIET)
#define T_step_eno_success(s, a) T_flags_eno_success(a, T_CHECK_STEP(s))
#define T_step_assert_eno_success(s, a) \
    T_flags_eno_success(a, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_psx_error(a, eno) T_flags_psx_error(a, eno, 0)
#define T_assert_psx_error(a, eno) T_flags_psx_error(a, eno, T_CHECK_STOP)
#define T_quiet_psx_error(a, eno) T_flags_psx_error(a, eno, T_CHECK_QUIET)
#define T_step_psx_error(s, a, eno) T_flags_psx_error(a, eno, T_CHECK_STEP(s))
#define T_step_assert_psx_error(s, a, eno) \
    T_flags_psx_error(a, eno, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_psx_success(a) T_flags_psx_success(a, 0)
#define T_assert_psx_success(a) T_flags_psx_success(a, T_CHECK_STOP)
#define T_quiet_psx_success(a) T_flags_psx_success(a, T_CHECK_QUIET)
#define T_step_psx_success(s, a) T_flags_psx_success(a, T_CHECK_STEP(s))
#define T_step_assert_psx_success(s, a) \
    T_flags_psx_success(a, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */

#ifdef __rtems__
void T_check_rsc(const T_check_context *, uint32_t, uint32_t);

#define T_flags_rsc(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_rsc(&T_check_instance, a, e);				\
}

void T_check_rsc_success(const T_check_context *, uint32_t);

#define T_flags_rsc_success(a, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_rsc_success(&T_check_instance, a);			\
}

/**
 * @defgroup RTEMSTestFrameworkChecksRSC RTEMS Status Code Checks
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Checks for RTEMS status codes (rtems_status_code).
 *
 * @{
 */

#define T_rsc(a, e) T_flags_rsc(a, e, 0)
#define T_assert_rsc(a, e) T_flags_rsc(a, e, T_CHECK_STOP)
#define T_quiet_rsc(a, e) T_flags_rsc(a, e, T_CHECK_QUIET)
#define T_step_rsc(s, a, e) T_flags_rsc(a, e, T_CHECK_STEP(s))
#define T_step_assert_rsc(s, a, e) \
    T_flags_rsc(a, e, T_CHECK_STEP(s) | T_CHECK_STOP)

#define T_rsc_success(a) T_flags_rsc_success(a, 0)
#define T_assert_rsc_success(a) T_flags_rsc_success(a, T_CHECK_STOP)
#define T_quiet_rsc_success(a) T_flags_rsc_success(a, T_CHECK_QUIET)
#define T_step_rsc_success(s, a) T_flags_rsc_success(a, T_CHECK_STEP(s))
#define T_step_assert_rsc_success(s, a) \
    T_flags_rsc_success(a, T_CHECK_STEP(s) | T_CHECK_STOP)

/** @} */
#endif /* __rtems__ */

void T_plan(unsigned int);

extern const T_fixture T_empty_fixture;

void T_push_plan(T_fixture_node *, unsigned int);

void T_pop_plan(void);

void T_check_step(const T_check_context *, unsigned int);

#define T_flags_step(a, flags)						\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, (flags) | T_CHECK_FMT };		\
	T_check_step(&T_check_instance, a);				\
}

#define T_step(e) T_flags_step(e, 0)
#define T_step_assert(e) T_flags_step(e, T_CHECK_STOP)

/**
 * @defgroup RTEMSTestFrameworkTime Time Services
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Time service functions.
 *
 * @{
 */

#ifdef __rtems__
typedef CPU_Counter_ticks T_ticks;
#else
typedef unsigned long T_ticks;
#endif

typedef uint64_t T_time;

/* More than enough for sizeof("18446744073.709552765") */
typedef char T_time_string[24];

const char *T_time_to_string_ns(T_time, T_time_string);

const char *T_time_to_string_us(T_time, T_time_string);

const char *T_time_to_string_ms(T_time, T_time_string);

const char *T_time_to_string_s(T_time, T_time_string);

const char *T_ticks_to_string_ns(T_ticks, T_time_string);

const char *T_ticks_to_string_us(T_ticks, T_time_string);

const char *T_ticks_to_string_ms(T_ticks, T_time_string);

const char *T_ticks_to_string_s(T_ticks, T_time_string);

T_time T_ticks_to_time(T_ticks);

T_ticks T_time_to_ticks(T_time);

T_time T_seconds_and_nanoseconds_to_time(uint32_t, uint32_t);

void T_time_to_seconds_and_nanoseconds(T_time, uint32_t *, uint32_t *);

T_time T_now(void);

#ifdef __rtems__
static inline T_ticks
T_tick(void)
{
	return _CPU_Counter_read();
}
#else
T_ticks T_tick(void);
#endif

T_time T_now_clock(void);

T_time T_now_dummy(void);

T_time T_now_tick(void);

T_time T_case_begin_time(void);

/** @} */

void *T_malloc(size_t);

void *T_calloc(size_t, size_t);

void *T_zalloc(size_t, void (*)(void *));

void T_free(void *);

void T_register(void);

typedef enum {
	T_EVENT_RUN_INITIALIZE_EARLY,
	T_EVENT_RUN_INITIALIZE_LATE,
	T_EVENT_CASE_EARLY,
	T_EVENT_CASE_BEGIN,
	T_EVENT_CASE_END,
	T_EVENT_CASE_LATE,
	T_EVENT_CASE_STOP,
	T_EVENT_RUN_FINALIZE_EARLY,
	T_EVENT_RUN_FINALIZE_LATE
} T_event;

typedef void (*T_action)(T_event, const char *);

typedef void (*T_putchar)(int, void *);

typedef struct {
	const char *name;
	char *buf;
	size_t buf_size;
	T_putchar putchar;
	void *putchar_arg;
	T_verbosity verbosity;
	T_time (*now)(void);
	void *(*allocate)(size_t);
	void (*deallocate)(void *);
	size_t action_count;
	const T_action *actions;
} T_config;

void T_putchar_default(int, void *);

int T_main(const T_config *);

void T_make_runner(void);

bool T_is_runner(void);

void T_run_initialize(const T_config *);

void T_run_all(void);

void T_run_by_name(const char *);

void T_case_begin(const char *, const T_fixture *);

void T_case_end(void);

bool T_run_finalize(void);

void T_set_putchar(T_putchar, void *, T_putchar *, void **);

void *T_fixture_context(void);

void T_set_fixture_context(void *);

void *T_push_fixture(T_fixture_node *, const T_fixture *);

void T_pop_fixture(void);

T_NO_RETURN void T_stop(void);

#define T_unreachable() \
    do { T_true(false, "Unreachable"); T_stop(); } while (0)

/**
 * @brief Gets the scope for nested fixtures.
 *
 * This function should help implementing scope fixture methods.  The parameter
 * layout allows efficient code generation for this method.
 *
 * @param desc is the description table.  It shall be a NULL-terminated array
 *   which references arrays of descriptive strings.
 *
 * @param buf is the buffer for the scope string.
 *
 * @param n is the size of the scope buffer in characters.
 *
 * @param second_indices is an array of indices defining which descriptive
 *   string is used for each entry in the description table.
 *
 * @return Returns the characters consumed from the buffer for the produced
 *   scope.
 */
size_t T_get_scope(
  const char * const * const *desc,
  char *buf,
  size_t n,
  const size_t *second_indices
);

size_t T_str_copy(char *, const char *, size_t);

#ifdef __rtems__
#define T_TEST_CASE_FIXTURE(name, fixture)			\
void T_case_body_##name(void);					\
T_case_context T_case_instance_##name = {			\
    #name,							\
    T_case_body_##name,						\
    fixture,							\
    NULL							\
};								\
static T_case_context * const T_case_item_##name		\
__attribute((__section__(".rtemsroset._T.content.0." #name)))	\
__attribute((__used__)) = &T_case_instance_##name;		\
void T_case_body_##name(void)
#else /* __rtems__ */
#define T_TEST_CASE_FIXTURE(name, fixture)			\
void T_case_body_##name(void);					\
T_case_context T_case_instance_##name = {			\
    #name,							\
    T_case_body_##name,						\
    fixture,							\
    NULL							\
};								\
__attribute((__constructor__)) static void			\
T_case_register_##name(void)					\
{								\
	T_case_register(&T_case_instance_##name);		\
}								\
void T_case_body_##name(void)
#endif /* __rtems__ */

#define T_TEST_CASE(name) T_TEST_CASE_FIXTURE(name, NULL)

void T_busy(uint_fast32_t);

uint_fast32_t T_get_one_clock_tick_busy(void);

typedef enum {
	T_INTERRUPT_TEST_INITIAL,
	T_INTERRUPT_TEST_ACTION,
	T_INTERRUPT_TEST_BLOCKED,
	T_INTERRUPT_TEST_CONTINUE,
	T_INTERRUPT_TEST_DONE,
	T_INTERRUPT_TEST_EARLY,
	T_INTERRUPT_TEST_INTERRUPT,
	T_INTERRUPT_TEST_LATE,
	T_INTERRUPT_TEST_TIMEOUT
} T_interrupt_test_state;

typedef struct {
	void (*prepare)(void *);
	void (*action)(void *);
	T_interrupt_test_state (*interrupt)(void *);
	void (*blocked)(void *);
	uint32_t max_iteration_count;
} T_interrupt_test_config;

T_interrupt_test_state T_interrupt_test_change_state(T_interrupt_test_state,
    T_interrupt_test_state);

T_interrupt_test_state T_interrupt_test_get_state(void);

void T_interrupt_test_busy_wait_for_interrupt(void);

T_interrupt_test_state T_interrupt_test(const T_interrupt_test_config *config,
    void *arg);

typedef struct {
	uint32_t executing;
	uint32_t heir;
	uint32_t cpu;
	T_time instant;
} T_thread_switch_event;

typedef struct {
	size_t recorded;
	size_t capacity;
	uint64_t switches;
	T_thread_switch_event events[T_ZERO_LENGTH_ARRAY];
} T_thread_switch_log;

typedef struct {
	T_thread_switch_log log;
	T_thread_switch_event events[T_ZERO_LENGTH_ARRAY_EXTENSION(2)];
} T_thread_switch_log_2;

typedef struct {
	T_thread_switch_log log;
	T_thread_switch_event events[T_ZERO_LENGTH_ARRAY_EXTENSION(4)];
} T_thread_switch_log_4;

typedef struct {
	T_thread_switch_log log;
	T_thread_switch_event events[T_ZERO_LENGTH_ARRAY_EXTENSION(10)];
} T_thread_switch_log_10;

T_thread_switch_log *T_thread_switch_record(T_thread_switch_log *);

T_thread_switch_log *T_thread_switch_record_2(T_thread_switch_log_2 *);

T_thread_switch_log *T_thread_switch_record_4(T_thread_switch_log_4 *);

T_thread_switch_log *T_thread_switch_record_10(T_thread_switch_log_10 *);

void T_report_hash_sha256(T_event, const char *);

void T_check_heap(T_event, const char *);

#ifdef __rtems__
void T_memory_action(T_event, const char *);

void *T_memory_allocate(size_t);

void T_memory_deallocate(void *);

void T_check_task_context(T_event, const char *);

void T_check_file_descriptors(T_event, const char *);

void T_check_rtems_barriers(T_event, const char *);

void T_check_rtems_extensions(T_event, const char *);

void T_check_rtems_message_queues(T_event, const char *);

void T_check_rtems_partitions(T_event, const char *);

void T_check_rtems_periods(T_event, const char *);

void T_check_rtems_regions(T_event, const char *);

void T_check_rtems_semaphores(T_event, const char *);

void T_check_rtems_tasks(T_event, const char *);

void T_check_rtems_timers(T_event, const char *);

void T_check_posix_keys(T_event, const char *);

void T_check_posix_message_queues(T_event, const char *);

void T_check_posix_semaphores(T_event, const char *);

void T_check_posix_shms(T_event, const char *);

void T_check_posix_threads(T_event, const char *);

void T_check_posix_timers(T_event, const char *);

typedef enum {
	T_THREAD_TIMER_NO_THREAD,
	T_THREAD_TIMER_SCHEDULED,
	T_THREAD_TIMER_PENDING,
	T_THREAD_TIMER_INACTIVE
} T_thread_timer_state;

T_thread_timer_state T_get_thread_timer_state(uint32_t);

void *T_seize_objects(rtems_status_code (*)(void *, uint32_t *), void *);

void T_surrender_objects(void **, rtems_status_code (*)(uint32_t));
#endif /* __rtems__ */

/**
 * @defgroup RTEMSTestFrameworkDestructors Destructors
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Support to run destructors at the end of a test case.
 *
 * @{
 */

typedef struct T_destructor {
	struct {
		struct T_destructor *le_next;
		struct T_destructor **le_prev;
	} node;
	void (*destroy)(struct T_destructor *);
} T_destructor;

void T_add_destructor(T_destructor *, void (*)(T_destructor *));

void T_remove_destructor(T_destructor *);

/** @} */

/**
 * @defgroup RTEMSTestFrameworkMeasureRuntime Runtime Measurements
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief Support to measure the runtime of code fragments.
 *
 * @{
 */

typedef struct T_measure_runtime_context T_measure_runtime_context;

typedef struct {
	size_t sample_count;
} T_measure_runtime_config;

typedef struct {
	const char *name;
	int flags;
	void (*setup)(void *);
	void (*body)(void *);
	bool (*teardown)(void *, T_ticks *, uint32_t, uint32_t, unsigned int);
	void *arg;
} T_measure_runtime_request;

#define T_MEASURE_RUNTIME_ALLOW_CLOCK_ISR 0x1

#define T_MEASURE_RUNTIME_REPORT_SAMPLES 0x2

#define T_MEASURE_RUNTIME_DISABLE_FULL_CACHE 0x10

#define T_MEASURE_RUNTIME_DISABLE_HOT_CACHE 0x20

#define T_MEASURE_RUNTIME_DISABLE_DIRTY_CACHE 0x40

#define T_MEASURE_RUNTIME_DISABLE_MINOR_LOAD 0x80

#define T_MEASURE_RUNTIME_DISABLE_MAX_LOAD 0x100

T_measure_runtime_context *T_measure_runtime_create(
    const T_measure_runtime_config *);

void T_measure_runtime(T_measure_runtime_context *,
    const T_measure_runtime_request *);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* THE_T_TEST_FRAMEWORK_H */
