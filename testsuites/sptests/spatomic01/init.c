/*
 * Copyright (c) 2013-2015 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * Copyright (c) 2013 Deng Hengyi.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/atomic.h>
#include <stdio.h>
#include <rtems.h>

#include "tmacros.h"

const char rtems_test_name[] = "SPATOMIC 1";

typedef struct {
  Atomic_Uint uint_atomic;
  Atomic_Uintptr uintptr_atomic;
  Atomic_Ulong ulong_atomic;
} test_context;

static test_context test_instance;

static void test_static_and_dynamic_initialization(void)
{
  #if (__SIZEOF_INT__ == 2)
    #define UINT_CONSTANT 0xc0feU
  #else
    #define UINT_CONSTANT 0xc01dc0feU
  #endif

  static Atomic_Uint static_uint   = ATOMIC_INITIALIZER_UINT(UINT_CONSTANT);
  static Atomic_Ulong static_ulong = ATOMIC_INITIALIZER_ULONG(0xdeadbeefUL);
  static Atomic_Uintptr static_uintptr =
    ATOMIC_INITIALIZER_UINTPTR((uintptr_t) &static_uintptr);
  static Atomic_Flag static_flag  = ATOMIC_INITIALIZER_FLAG;

  Atomic_Uint stack_uint;
  Atomic_Ulong stack_ulong;
  Atomic_Uintptr stack_uintptr;
  Atomic_Flag stack_flag;

  puts("=== static and dynamic initialization test case ===");

  _Atomic_Init_uint(&stack_uint, UINT_CONSTANT);
  _Atomic_Init_ulong(&stack_ulong, 0xdeadbeefUL);
  _Atomic_Init_uintptr(&stack_uintptr, (uintptr_t) &static_uintptr);
  _Atomic_Flag_clear(&stack_flag, ATOMIC_ORDER_RELAXED);

  rtems_test_assert(
    memcmp(&stack_uint, &static_uint, sizeof(stack_uint)) == 0
  );
  rtems_test_assert(
    memcmp(&stack_ulong, &static_ulong, sizeof(stack_ulong)) == 0
  );
  rtems_test_assert(
    memcmp(&stack_uintptr, &static_uintptr, sizeof(stack_uintptr)) == 0
  );
  rtems_test_assert(
    memcmp(&stack_flag, &static_flag, sizeof(stack_flag)) == 0
  );

  rtems_test_assert(
    _Atomic_Load_uint(&stack_uint, ATOMIC_ORDER_RELAXED) == 0xc01dc0feU
  );
  rtems_test_assert(
    _Atomic_Load_ulong(&stack_ulong, ATOMIC_ORDER_RELAXED) == 0xdeadbeefUL
  );
  rtems_test_assert(
    _Atomic_Load_uintptr(&stack_uintptr, ATOMIC_ORDER_RELAXED)
      == (uintptr_t) &static_uintptr
  );
  rtems_test_assert(
    !_Atomic_Flag_test_and_set(&stack_flag, ATOMIC_ORDER_RELAXED)
  );
}

typedef void (*simple_test_body)(test_context *ctx);

static void test_simple_atomic_add_body(test_context *ctx)
{
  unsigned int ia = 8, ib = 4;
  unsigned int ic;
  uintptr_t pa = 42, pb = 24;
  uintptr_t pc;
  unsigned long a = 2, b = 1;
  unsigned long c;

  puts("=== atomic simple add test case ===");

  _Atomic_Store_uint(&ctx->uint_atomic, ia, ATOMIC_ORDER_RELAXED);
  _Atomic_Fetch_add_uint(&ctx->uint_atomic, ib, ATOMIC_ORDER_RELAXED);
  ic = _Atomic_Load_uint(&ctx->uint_atomic, ATOMIC_ORDER_RELAXED);
  rtems_test_assert(ic == (ia + ib));

  _Atomic_Store_uintptr(&ctx->uintptr_atomic, pa, ATOMIC_ORDER_RELAXED);
  _Atomic_Fetch_add_uintptr(&ctx->uintptr_atomic, pb, ATOMIC_ORDER_RELAXED);
  pc = _Atomic_Load_uintptr(&ctx->uintptr_atomic, ATOMIC_ORDER_RELAXED);
  rtems_test_assert(pc == (pa + pb));

  _Atomic_Store_ulong(&ctx->ulong_atomic, a, ATOMIC_ORDER_RELAXED);
  _Atomic_Fetch_add_ulong(&ctx->ulong_atomic, b, ATOMIC_ORDER_RELAXED);
  c = _Atomic_Load_ulong(&ctx->ulong_atomic, ATOMIC_ORDER_RELAXED);
  rtems_test_assert(c == (a + b));
}

static void test_simple_atomic_sub_body(test_context *ctx)
{
  unsigned int ia = 8, ib = 4;
  unsigned int ic;
  uintptr_t pa = 42, pb = 24;
  uintptr_t pc;
  unsigned long a = 2, b = 1;
  unsigned long c;

  puts("=== atomic simple sub test case ===");

  _Atomic_Store_uint(&ctx->uint_atomic, ia, ATOMIC_ORDER_RELAXED);
  _Atomic_Fetch_sub_uint(&ctx->uint_atomic, ib, ATOMIC_ORDER_RELAXED);
  ic = _Atomic_Load_uint(&ctx->uint_atomic, ATOMIC_ORDER_RELAXED);
  rtems_test_assert(ic == (ia - ib));

  _Atomic_Store_uintptr(&ctx->uintptr_atomic, pa, ATOMIC_ORDER_RELAXED);
  _Atomic_Fetch_sub_uintptr(&ctx->uintptr_atomic, pb, ATOMIC_ORDER_RELAXED);
  pc = _Atomic_Load_uintptr(&ctx->uintptr_atomic, ATOMIC_ORDER_RELAXED);
  rtems_test_assert(pc == (pa - pb));

  _Atomic_Store_ulong(&ctx->ulong_atomic, a, ATOMIC_ORDER_RELAXED);
  _Atomic_Fetch_sub_ulong(&ctx->ulong_atomic, b, ATOMIC_ORDER_RELAXED);
  c = _Atomic_Load_ulong(&ctx->ulong_atomic, ATOMIC_ORDER_RELAXED);
  rtems_test_assert(c == (a - b));
}

static void test_simple_atomic_or_body(test_context *ctx)
{
  unsigned int ia = 8, ib = 4;
  unsigned int ic;
  uintptr_t pa = 42, pb = 24;
  uintptr_t pc;
  unsigned long a = 2, b = 1;
  unsigned long c;

  puts("=== atomic simple or test case ===");

  _Atomic_Store_uint(&ctx->uint_atomic, ia, ATOMIC_ORDER_RELAXED);
  _Atomic_Fetch_or_uint(&ctx->uint_atomic, ib, ATOMIC_ORDER_RELAXED);
  ic = _Atomic_Load_uint(&ctx->uint_atomic, ATOMIC_ORDER_RELAXED);
  rtems_test_assert(ic == (ia | ib));

  _Atomic_Store_uintptr(&ctx->uintptr_atomic, pa, ATOMIC_ORDER_RELAXED);
  _Atomic_Fetch_or_uintptr(&ctx->uintptr_atomic, pb, ATOMIC_ORDER_RELAXED);
  pc = _Atomic_Load_uintptr(&ctx->uintptr_atomic, ATOMIC_ORDER_RELAXED);
  rtems_test_assert(pc == (pa | pb));

  _Atomic_Store_ulong(&ctx->ulong_atomic, a, ATOMIC_ORDER_RELAXED);
  _Atomic_Fetch_or_ulong(&ctx->ulong_atomic, b, ATOMIC_ORDER_RELAXED);
  c = _Atomic_Load_ulong(&ctx->ulong_atomic, ATOMIC_ORDER_RELAXED);
  rtems_test_assert(c == (a | b));
}

static void test_simple_atomic_and_body(test_context *ctx)
{
  unsigned int ia = 8, ib = 4;
  unsigned int ic;
  uintptr_t pa = 42, pb = 24;
  uintptr_t pc;
  unsigned long a = 2, b = 1;
  unsigned long c;

  puts("=== atomic simple and test case ===");

  _Atomic_Store_uint(&ctx->uint_atomic, ia, ATOMIC_ORDER_RELAXED);
  _Atomic_Fetch_and_uint(&ctx->uint_atomic, ib, ATOMIC_ORDER_RELAXED);
  ic = _Atomic_Load_uint(&ctx->uint_atomic, ATOMIC_ORDER_RELAXED);
  rtems_test_assert(ic == (ia & ib));

  _Atomic_Store_uintptr(&ctx->uintptr_atomic, pa, ATOMIC_ORDER_RELAXED);
  _Atomic_Fetch_and_uintptr(&ctx->uintptr_atomic, pb, ATOMIC_ORDER_RELAXED);
  pc = _Atomic_Load_uintptr(&ctx->uintptr_atomic, ATOMIC_ORDER_RELAXED);
  rtems_test_assert(pc == (pa & pb));

  _Atomic_Store_ulong(&ctx->ulong_atomic, a, ATOMIC_ORDER_RELAXED);
  _Atomic_Fetch_and_ulong(&ctx->ulong_atomic, b, ATOMIC_ORDER_RELAXED);
  c = _Atomic_Load_ulong(&ctx->ulong_atomic, ATOMIC_ORDER_RELAXED);
  rtems_test_assert(c == (a & b));
}

static void test_simple_atomic_exchange_body(test_context *ctx)
{
  unsigned int ia = 8, ib = 4;
  unsigned int ic;
  uintptr_t pa = 42, pb = 24;
  uintptr_t pc;
  unsigned long a = 2, b = 1;
  unsigned long c;

  puts("=== atomic simple exchange test case ===");

  _Atomic_Store_uint(&ctx->uint_atomic, ia, ATOMIC_ORDER_RELAXED);
  ic = _Atomic_Exchange_uint(&ctx->uint_atomic, ib, ATOMIC_ORDER_RELAXED);
  rtems_test_assert(ic == ia);
  ic = _Atomic_Load_uint(&ctx->uint_atomic, ATOMIC_ORDER_RELAXED);
  rtems_test_assert(ic == ib);

  _Atomic_Store_uintptr(&ctx->uintptr_atomic, pa, ATOMIC_ORDER_RELAXED);
  pc = _Atomic_Exchange_uintptr(&ctx->uintptr_atomic, pb, ATOMIC_ORDER_RELAXED);
  rtems_test_assert(pc == pa);
  pc = _Atomic_Load_uintptr(&ctx->uintptr_atomic, ATOMIC_ORDER_RELAXED);
  rtems_test_assert(pc == pb);

  _Atomic_Store_ulong(&ctx->ulong_atomic, a, ATOMIC_ORDER_RELAXED);
  c = _Atomic_Exchange_ulong(&ctx->ulong_atomic, b, ATOMIC_ORDER_RELAXED);
  rtems_test_assert(c == a);
  c = _Atomic_Load_ulong(&ctx->ulong_atomic, ATOMIC_ORDER_RELAXED);
  rtems_test_assert(c == b);
}

static void test_simple_atomic_compare_exchange_body(test_context *ctx)
{
  unsigned int ei;
  unsigned int vi;
  uintptr_t ep;
  uintptr_t vp;
  unsigned long el;
  unsigned long vl;
  bool success;

  puts("=== atomic simple compare exchange test case ===");

  _Atomic_Store_uint(&ctx->uint_atomic, 1, ATOMIC_ORDER_RELAXED);
  ei = 2;
  success = _Atomic_Compare_exchange_uint(
    &ctx->uint_atomic,
    &ei,
    3,
    ATOMIC_ORDER_RELAXED,
    ATOMIC_ORDER_RELAXED
  );
  rtems_test_assert(!success);
  rtems_test_assert(ei == 1);
  success = _Atomic_Compare_exchange_uint(
    &ctx->uint_atomic,
    &ei,
    3,
    ATOMIC_ORDER_RELAXED,
    ATOMIC_ORDER_RELAXED
  );
  rtems_test_assert(success);
  vi = _Atomic_Load_uint(&ctx->uint_atomic, ATOMIC_ORDER_RELAXED);
  rtems_test_assert(vi == 3);

  _Atomic_Store_uintptr(&ctx->uintptr_atomic, 111, ATOMIC_ORDER_RELAXED);
  ep = 211;
  success = _Atomic_Compare_exchange_uintptr(
    &ctx->uintptr_atomic,
    &ep,
    311,
    ATOMIC_ORDER_RELAXED,
    ATOMIC_ORDER_RELAXED
  );
  rtems_test_assert(!success);
  rtems_test_assert(ep == 111);
  success = _Atomic_Compare_exchange_uintptr(
    &ctx->uintptr_atomic,
    &ep,
    311,
    ATOMIC_ORDER_RELAXED,
    ATOMIC_ORDER_RELAXED
  );
  rtems_test_assert(success);
  vp = _Atomic_Load_uintptr(&ctx->uintptr_atomic, ATOMIC_ORDER_RELAXED);
  rtems_test_assert(vp == 311);

  _Atomic_Store_ulong(&ctx->ulong_atomic, 10, ATOMIC_ORDER_RELAXED);
  el = 11;
  success = _Atomic_Compare_exchange_ulong(
    &ctx->ulong_atomic,
    &el,
    12,
    ATOMIC_ORDER_RELAXED,
    ATOMIC_ORDER_RELAXED
  );
  rtems_test_assert(!success);
  rtems_test_assert(el == 10);
  success = _Atomic_Compare_exchange_ulong(
    &ctx->ulong_atomic,
    &el,
    12,
    ATOMIC_ORDER_RELAXED,
    ATOMIC_ORDER_RELAXED
  );
  rtems_test_assert(success);
  vl = _Atomic_Load_ulong(&ctx->ulong_atomic, ATOMIC_ORDER_RELAXED);
  rtems_test_assert(vl == 12);
}

static const simple_test_body simple_test_bodies[] = {
  test_simple_atomic_add_body,
  test_simple_atomic_sub_body,
  test_simple_atomic_or_body,
  test_simple_atomic_and_body,
  test_simple_atomic_exchange_body,
  test_simple_atomic_compare_exchange_body,
};

#define SIMPLE_TEST_COUNT RTEMS_ARRAY_SIZE(simple_test_bodies)

static void simple_tests(void)
{
  test_context *ctx = &test_instance;
  size_t test;

  for (test = 0; test < SIMPLE_TEST_COUNT; ++test) {
    const simple_test_body *test_body = &simple_test_bodies[test];

    (*test_body)(ctx);
  }
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test_static_and_dynamic_initialization();
  simple_tests();

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
