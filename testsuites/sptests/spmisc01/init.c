/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2018, 2019 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <string.h>

#include <tmacros.h>

#include "spmisc01.h"

const char rtems_test_name[] = "SPMISC 1";

RTEMS_INLINE_ROUTINE int inline_func(void)
{
  return 7;
}

RTEMS_NO_INLINE static int noinline_func(void)
{
  return 14;
}

RTEMS_NO_RETURN void noreturn_func(void);

RTEMS_PURE static int pure_func(void)
{
  return 21;
}

RTEMS_CONST static int const_func(void)
{
  return 23;
}

RTEMS_SECTION(".rtemsroset.test") static int section_variable = 28;

RTEMS_NOINIT static int noinit_variable;

RTEMS_USED static int used_func(void)
{
  return 35;
}

RTEMS_USED static int used_variable;

static int unused_arg_and_variable_func(RTEMS_UNUSED int arg)
{
  RTEMS_UNUSED int variable;

  return 42;
}

typedef struct {
  uint8_t c;
  uint32_t i;
} RTEMS_PACKED packed_struct;

static int alias_func(void) RTEMS_ALIAS(noinline_func);

int weak_alias_func(void) RTEMS_WEAK_ALIAS(noinline_func);

RTEMS_WEAK int weak_or_strong(void)
{
  return 99;
}

RTEMS_WEAK int weak_2(void)
{
  return 111;
}

static char aligned_variable RTEMS_ALIGNED(64);

typedef struct {
  uint8_t c;
  uint8_t aligned_member RTEMS_ALIGNED(64);
} aligned_member_struct;

static void unreachable(void)
{
  if (0) {
    RTEMS_UNREACHABLE();
  }
}

RTEMS_PRINTFLIKE(1, 2) static int printflike_func(const char *fmt, ...)
{
  return 56;
}

static int obfuscate_variable(int i)
{
  RTEMS_OBFUSCATE_VARIABLE(i);
  return i;
}

static int global_symbol_base;

RTEMS_DECLARE_GLOBAL_SYMBOL(a_global_symbol);

RTEMS_DEFINE_GLOBAL_SYMBOL(
  a_global_symbol,
  RTEMS_SYMBOL_NAME(global_symbol_base) + 0xabc
);

RTEMS_STATIC_ASSERT(0 != 1, zero_neq_one);

static int array[3];

typedef struct {
  uint32_t i;
  uint32_t j[RTEMS_ZERO_LENGTH_ARRAY];
} zero_length_array_struct;

typedef struct {
  int a;
  int b;
} container_of_struct;

static void container_of(void)
{
  container_of_struct s;
  int *b;

  b = &s.b;
  rtems_test_assert(RTEMS_CONTAINER_OF(b, container_of_struct, b) == &s);
}

static int deconst(void)
{
  const int i = 70;
  int *p;

  p = RTEMS_DECONST(int *, &i);
  return *p;
}

static int devolatile(void)
{
  volatile int i = 77;
  int *p;

  p = RTEMS_DEVOLATILE(int *, &i);
  return *p;
}

static int dequalify(void)
{
  volatile const int i = 84;
  int *p;

  p = RTEMS_DEQUALIFY(int *, &i);
  return *p;
}

typedef struct {
  char a;
  int b;
} same_member_type_struct;

typedef struct {
  char c;
  int d;
} same_member_type_struct_2;

RTEMS_STATIC_ASSERT(
  RTEMS_HAVE_MEMBER_SAME_TYPE(
    same_member_type_struct,
    b,
    same_member_type_struct_2,
    d
  ),
  same_member_type_struct_eq
);

RTEMS_STATIC_ASSERT(
  !RTEMS_HAVE_MEMBER_SAME_TYPE(
    same_member_type_struct,
    b,
    same_member_type_struct_2,
    c
  ),
  same_member_type_struct_neq
);

static int concat(void)
{
  return 91;
}

#define CON con

#define CAT cat

#define STR ing

static void Init(rtems_task_argument arg)
{
  void *p;
  int   v;

  TEST_BEGIN();
  rtems_test_assert(inline_func() == 7);
  RTEMS_COMPILER_MEMORY_BARRIER();
  rtems_test_assert(noinline_func() == 14);
  rtems_test_assert(pure_func() == 21);
  rtems_test_assert(const_func() == 23);
  rtems_test_assert(section_variable == 28);
  v = noinit_variable;
  RTEMS_OBFUSCATE_VARIABLE(v);
  rtems_test_assert(unused_arg_and_variable_func(49) == 42);
  rtems_test_assert(sizeof(packed_struct) == 5);
  rtems_test_assert(alias_func() == 14);
  rtems_test_assert(weak_alias_func() == 14);
  pull_in_strong();
  rtems_test_assert(weak_or_strong() == 77);
  rtems_test_assert(weak_2() == 111);
  rtems_test_assert(((uintptr_t) &aligned_variable) % 64 == 0);
  rtems_test_assert(offsetof(aligned_member_struct, aligned_member) % 64 == 0);
  unreachable();
  rtems_test_assert(printflike_func("%i", 0) == 56);
  rtems_test_assert(obfuscate_variable(63) == 63);
  rtems_test_assert(
    (uintptr_t) a_global_symbol - (uintptr_t) &global_symbol_base == 0xabc
  );
  rtems_test_assert(RTEMS_ARRAY_SIZE(array) == 3);
  rtems_test_assert(sizeof(zero_length_array_struct) == 4);
  container_of();
  rtems_test_assert(deconst() == 70);
  rtems_test_assert(devolatile() == 77);
  rtems_test_assert(dequalify() == 84);
  rtems_test_assert(
    RTEMS_HAVE_MEMBER_SAME_TYPE(
      same_member_type_struct,
      b,
      same_member_type_struct_2,
      d
    )
  );
  rtems_test_assert(
    !RTEMS_HAVE_MEMBER_SAME_TYPE(
      same_member_type_struct,
      b,
      same_member_type_struct_2,
      c
    )
  );
  rtems_test_assert(RTEMS_CONCAT(con, cat)() == 91);
  rtems_test_assert(RTEMS_XCONCAT(CON, CAT)() == 91);
  rtems_test_assert(strcmp(RTEMS_STRING(str), "str") == 0);
  rtems_test_assert(strcmp(RTEMS_XSTRING(STR), "ing") == 0);

  if (RTEMS_PREDICT_TRUE(true)) {
    rtems_test_assert(true);
  } else {
    rtems_test_assert(false);
  }

  if (RTEMS_PREDICT_FALSE(true)) {
    rtems_test_assert(true);
  } else {
    rtems_test_assert(false);
  }

  if (RTEMS_PREDICT_TRUE(false)) {
    rtems_test_assert(false);
  } else {
    rtems_test_assert(true);
  }

  if (RTEMS_PREDICT_FALSE(false)) {
    rtems_test_assert(false);
  } else {
    rtems_test_assert(true);
  }

  p = RTEMS_RETURN_ADDRESS();
  (void) p;

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
