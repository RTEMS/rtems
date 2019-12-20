/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2018, 2019 embedded brains GmbH
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
	void *initial_context;
} T_fixture;

#define T_ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

/*
 * The __FILE__ includes the full file path from the command line.  Enable the
 * build system to give a sorter file name via a command line define.
 */
#ifndef T_FILE_NAME
#define T_FILE_NAME __FILE__
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

typedef struct T_case_context {
	const char *name;
	void (*body)(void);
	const T_fixture *fixture;
	const struct T_case_context *next;
} T_case_context;

void T_case_register(T_case_context *);

#define T_CHECK_STOP 1U

#define T_CHECK_QUIET 2U

#define T_CHECK_STEP_FLAG 4U

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

void T_check_true(bool, const T_check_context *, const char *, ...);

#define T_flags_true(a, flags, ...)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_true(a, &T_check_instance, __VA_ARGS__);		\
}

#define T_flags_eq(a, e, flags, ...) \
    T_flags_true((a) == (e), flags, __VA_ARGS__)

#define T_flags_ne(a, e, flags, ...) \
    T_flags_true((a) != (e), flags, __VA_ARGS__)

void T_check_eq_ptr(const void *, const T_check_context_msg *, const void *);

#define T_flags_eq_ptr(a, e, flags, sa, se)				\
{									\
	static const T_check_context_msg T_check_instance = {		\
	    { T_FILE_NAME, __LINE__, flags }, sa " == " se };		\
	T_check_eq_ptr(a, &T_check_instance, e);			\
}

void T_check_ne_ptr(const void *, const T_check_context_msg *, const void *);

#define T_flags_ne_ptr(a, e, flags, sa, se)				\
{									\
	static const T_check_context_msg T_check_instance = {		\
	    { T_FILE_NAME, __LINE__, flags }, sa " != " se };		\
	T_check_ne_ptr(a, &T_check_instance, e);			\
}

void T_check_null(const void *, const T_check_context_msg *);

#define T_flags_null(a, flags, sa)					\
{									\
	static const T_check_context_msg T_check_instance = {		\
	    { T_FILE_NAME, __LINE__, flags }, sa };			\
	T_check_null(a, &T_check_instance);				\
}

void T_check_not_null(const void *, const T_check_context_msg *);

#define T_flags_not_null(a, flags, sa)					\
{									\
	static const T_check_context_msg T_check_instance = {		\
	    { T_FILE_NAME, __LINE__, flags }, sa };			\
	T_check_not_null(a, &T_check_instance);				\
}

void T_check_eq_mem(const void *, const T_check_context_msg *, const void *,
    size_t);

#define T_flags_eq_mem(a, e, n, flags, sa, se, sn)			\
{									\
	static const T_check_context_msg T_check_instance = {		\
	    { T_FILE_NAME, __LINE__, flags },				\
	    "memcmp(" sa ", " se ", " sn ") == 0" };			\
	T_check_eq_mem(a, &T_check_instance, e, n);			\
}

void T_check_ne_mem(const void *, const T_check_context_msg *, const void *,
    size_t);

#define T_flags_ne_mem(a, e, n, flags, sa, se, sn)			\
{									\
	static const T_check_context_msg T_check_instance = {		\
	    { T_FILE_NAME, __LINE__, flags },				\
	    "memcmp(" sa ", " se ", " sn ") != 0" };			\
	T_check_ne_mem(a, &T_check_instance, e, n);			\
}

void T_check_eq_str(const char *, const T_check_context *, const char *);

#define T_flags_eq_str(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_eq_str(a, &T_check_instance, e);			\
}

void T_check_ne_str(const char *, const T_check_context *, const char *);

#define T_flags_ne_str(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_ne_str(a, &T_check_instance, e);			\
}

void T_check_eq_nstr(const char *, const T_check_context *, const char *,
    size_t);

#define T_flags_eq_nstr(a, e, n, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_eq_nstr(a, &T_check_instance, e, n);			\
}

void T_check_ne_nstr(const char *, const T_check_context *, const char *,
    size_t);

#define T_flags_ne_nstr(a, e, n, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_ne_nstr(a, &T_check_instance, e, n);			\
}

void T_check_eq_char(char, const T_check_context *, char);

#define T_flags_eq_char(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_eq_char(a, &T_check_instance, e);			\
}

void T_check_ne_char(char, const T_check_context *, char);

#define T_flags_ne_char(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_ne_char(a, &T_check_instance, e);			\
}

void T_check_eq_int(int, const T_check_context *, int);

#define T_flags_eq_int(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_eq_int(a, &T_check_instance, e);			\
}

void T_check_ne_int(int, const T_check_context *, int);

#define T_flags_ne_int(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_ne_int(a, &T_check_instance, e);			\
}

void T_check_ge_int(int, const T_check_context *, int);

#define T_flags_ge_int(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_ge_int(a, &T_check_instance, e);			\
}

void T_check_gt_int(int, const T_check_context *, int);

#define T_flags_gt_int(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_gt_int(a, &T_check_instance, e);			\
}

void T_check_le_int(int, const T_check_context *, int);

#define T_flags_le_int(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_le_int(a, &T_check_instance, e);			\
}

void T_check_lt_int(int, const T_check_context *, int);

#define T_flags_lt_int(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_lt_int(a, &T_check_instance, e);			\
}

void T_check_eq_uint(unsigned int, const T_check_context *, unsigned int);

#define T_flags_eq_uint(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_eq_uint(a, &T_check_instance, e);			\
}

void T_check_ne_uint(unsigned int, const T_check_context *, unsigned int);

#define T_flags_ne_uint(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_ne_uint(a, &T_check_instance, e);			\
}

void T_check_ge_uint(unsigned int, const T_check_context *, unsigned int);

#define T_flags_ge_uint(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_ge_uint(a, &T_check_instance, e);			\
}

void T_check_gt_uint(unsigned int, const T_check_context *, unsigned int);

#define T_flags_gt_uint(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_gt_uint(a, &T_check_instance, e);			\
}

void T_check_le_uint(unsigned int, const T_check_context *, unsigned int);

#define T_flags_le_uint(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_le_uint(a, &T_check_instance, e);			\
}

void T_check_lt_uint(unsigned int, const T_check_context *, unsigned int);

#define T_flags_lt_uint(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_lt_uint(a, &T_check_instance, e);			\
}

void T_check_eq_long(long, const T_check_context *, long);

#define T_flags_eq_long(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_eq_long(a, &T_check_instance, e);			\
}

void T_check_ne_long(long, const T_check_context *, long);

#define T_flags_ne_long(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_ne_long(a, &T_check_instance, e);			\
}

void T_check_ge_long(long, const T_check_context *, long);

#define T_flags_ge_long(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_ge_long(a, &T_check_instance, e);			\
}

void T_check_gt_long(long, const T_check_context *, long);

#define T_flags_gt_long(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_gt_long(a, &T_check_instance, e);			\
}

void T_check_le_long(long, const T_check_context *, long);

#define T_flags_le_long(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_le_long(a, &T_check_instance, e);			\
}

void T_check_lt_long(long, const T_check_context *, long);

#define T_flags_lt_long(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_lt_long(a, &T_check_instance, e);			\
}

void T_check_eq_ulong(unsigned long, const T_check_context *, unsigned long);

#define T_flags_eq_ulong(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_eq_ulong(a, &T_check_instance, e);			\
}

void T_check_ne_ulong(unsigned long, const T_check_context *, unsigned long);

#define T_flags_ne_ulong(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_ne_ulong(a, &T_check_instance, e);			\
}

void T_check_ge_ulong(unsigned long, const T_check_context *, unsigned long);

#define T_flags_ge_ulong(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_ge_ulong(a, &T_check_instance, e);			\
}

void T_check_gt_ulong(unsigned long, const T_check_context *, unsigned long);

#define T_flags_gt_ulong(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_gt_ulong(a, &T_check_instance, e);			\
}

void T_check_le_ulong(unsigned long, const T_check_context *, unsigned long);

#define T_flags_le_ulong(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_le_ulong(a, &T_check_instance, e);			\
}

void T_check_lt_ulong(unsigned long, const T_check_context *, unsigned long);

#define T_flags_lt_ulong(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_lt_ulong(a, &T_check_instance, e);			\
}

void T_check_eq_ll(long long, const T_check_context *, long long);

#define T_flags_eq_ll(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_eq_ll(a, &T_check_instance, e);				\
}

void T_check_ne_ll(long long, const T_check_context *, long long);

#define T_flags_ne_ll(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_ne_ll(a, &T_check_instance, e);				\
}

void T_check_ge_ll(long long, const T_check_context *, long long);

#define T_flags_ge_ll(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_ge_ll(a, &T_check_instance, e);				\
}

void T_check_gt_ll(long long, const T_check_context *, long long);

#define T_flags_gt_ll(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_gt_ll(a, &T_check_instance, e);				\
}

void T_check_le_ll(long long, const T_check_context *, long long);

#define T_flags_le_ll(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_le_ll(a, &T_check_instance, e);				\
}

void T_check_lt_ll(long long, const T_check_context *, long long);

#define T_flags_lt_ll(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_lt_ll(a, &T_check_instance, e);				\
}

void T_check_eq_ull(unsigned long long, const T_check_context *,
    unsigned long long);

#define T_flags_eq_ull(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_eq_ull(a, &T_check_instance, e);			\
}

void T_check_ne_ull(unsigned long long, const T_check_context *,
    unsigned long long);

#define T_flags_ne_ull(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_ne_ull(a, &T_check_instance, e);			\
}

void T_check_ge_ull(unsigned long long, const T_check_context *,
    unsigned long long);

#define T_flags_ge_ull(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_ge_ull(a, &T_check_instance, e);			\
}

void T_check_gt_ull(unsigned long long, const T_check_context *,
    unsigned long long);

#define T_flags_gt_ull(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_gt_ull(a, &T_check_instance, e);			\
}

void T_check_le_ull(unsigned long long, const T_check_context *,
    unsigned long long);

#define T_flags_le_ull(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_le_ull(a, &T_check_instance, e);			\
}

void T_check_lt_ull(unsigned long long, const T_check_context *,
    unsigned long long);

#define T_flags_lt_ull(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_lt_ull(a, &T_check_instance, e);			\
}

void T_check_eno(int, const T_check_context *, int);

#define T_flags_eno(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_eno(a, &T_check_instance, e);				\
}

void T_check_eno_success(int, const T_check_context *);

#define T_flags_eno_success(a, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_eno_success(a, &T_check_instance);			\
}

void T_check_psx_error(int, const T_check_context *, int);

#define T_flags_psx_error(a, eno, flags)				\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_psx_error(a, &T_check_instance, eno);			\
}

void T_check_psx_success(int, const T_check_context *);

#define T_flags_psx_success(a, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_psx_success(a, &T_check_instance);			\
}

/** @} */

int T_printf(char const *, ...);

int T_vprintf(char const *, va_list);

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

#define T_true(a, ...) T_flags_true(a, 0, __VA_ARGS__)
#define T_assert_true(a, ...) T_flags_true(a, T_CHECK_STOP, __VA_ARGS__)
#define T_quiet_true(a, ...) T_flags_true(a, T_CHECK_QUIET, __VA_ARGS__)
#define T_step_true(s, a, ...) T_flags_true(a, T_CHECK_STEP(s), __VA_ARGS__)
#define T_step_assert_true(s, a, ...) \
    T_flags_true(a, T_CHECK_STEP(s) | T_CHECK_STOP, __VA_ARGS__)

#define T_false(a, ...) T_flags_true(!(a), 0, __VA_ARGS__)
#define T_assert_false(a, ...) T_flags_true(!(a), T_CHECK_STOP, __VA_ARGS__)
#define T_quiet_false(a, ...) T_flags_true(!(a), T_CHECK_QUIET, __VA_ARGS__)
#define T_step_false(s, a, ...) T_flags_true(!(a), T_CHECK_STEP(s), __VA_ARGS__)
#define T_step_assert_false(s, a, ...) \
    T_flags_true(!(a), T_CHECK_STEP(s) | T_CHECK_STOP, __VA_ARGS__)

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

#define T_eq(a, e, ...) T_flags_eq(a, e, 0, __VA_ARGS__)
#define T_assert_eq(a, e, ...) T_flags_eq(a, e, T_CHECK_STOP, __VA_ARGS__)
#define T_quiet_eq(a, e, ...) T_flags_eq(a, e, T_CHECK_QUIET, __VA_ARGS__)
#define T_step_eq(s, a, e, ...) T_flags_eq(a, e, T_CHECK_STEP(s), __VA_ARGS__)
#define T_step_assert_eq(s, a, e, ...) \
    T_flags_eq(a, e, T_CHECK_STEP(s) | T_CHECK_STOP, __VA_ARGS__)

#define T_ne(a, e, ...) T_flags_ne(a, e, 0, __VA_ARGS__)
#define T_assert_ne(a, e, ...) T_flags_ne(a, e, T_CHECK_STOP, __VA_ARGS__)
#define T_quiet_ne(a, e, ...) T_flags_ne(a, e, T_CHECK_QUIET, __VA_ARGS__)
#define T_step_ne(s, a, e, ...) T_flags_ne(a, e, T_CHECK_STEP(s), __VA_ARGS__)
#define T_step_assert_ne(s, a, e, ...) \
    T_flags_ne(a, e, T_CHECK_STEP(s) | T_CHECK_STOP, __VA_ARGS__)

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
#define T_step_null(s, a) T_flags_null(a, T_CHECK_STEP(s), #a)
#define T_step_assert_null(s, a) \
    T_flags_null(a, T_CHECK_STEP(s) | T_CHECK_STOP, #a)

#define T_not_null(a) T_flags_not_null(a, 0, #a)
#define T_assert_not_null(a) T_flags_not_null(a, T_CHECK_STOP, #a)
#define T_quiet_not_null(a) T_flags_not_null(a, T_CHECK_QUIET, #a)
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
void T_check_rsc(uint32_t, const T_check_context *, uint32_t);

#define T_flags_rsc(a, e, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_rsc(a, &T_check_instance, e);				\
}

void T_check_rsc_success(uint32_t, const T_check_context *);

#define T_flags_rsc_success(a, flags)					\
{									\
	static const T_check_context T_check_instance = {		\
	    T_FILE_NAME, __LINE__, flags };				\
	T_check_rsc_success(a, &T_check_instance);			\
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

#define T_step(s, ...) T_flags_true(true, T_CHECK_STEP(s), __VA_ARGS__)
#define T_step_assert(s, ...) \
    T_flags_true(true, T_CHECK_STEP(s) | T_CHECK_STOP, __VA_ARGS__)

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
	size_t action_count;
	const T_action *actions;
} T_config;

void T_putchar_default(int, void *);

int T_main(const T_config *);

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

void T_report_hash_sha256(T_event, const char *);

void T_check_heap(T_event, const char *);

#ifdef __rtems__
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

#define T_MEASURE_RUNTIME_DISABLE_VALID_CACHE 0x10

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
