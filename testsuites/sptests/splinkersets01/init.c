/*
 * Copyright (c) 2015, 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "tmacros.h"

#include "splinkersets01.h"

const char rtems_test_name[] = "SPLINKERSETS 1";

RTEMS_LINKER_RWSET(test_rw_i, const int *);

RTEMS_LINKER_ROSET(test_ro_i, const int *);

RTEMS_LINKER_RWSET_DECLARE(test_rw_i, const int *);

RTEMS_LINKER_ROSET_DECLARE(test_ro_i, const int *);

const int a[4];

const int ca[5];

RTEMS_LINKER_RWSET_ITEM(test_rw, const int *, a3) = &a[3];
RTEMS_LINKER_RWSET_ITEM_ORDERED(test_rw, const int *, a2, 2) = &a[2];
RTEMS_LINKER_RWSET_ITEM_REFERENCE(test_rw, const int *, a1);
RTEMS_LINKER_RWSET_ITEM_ORDERED(test_rw, const int *, a0, 0) = &a[0];

/* Items are ordered lexicographically */
RTEMS_LINKER_RWSET_ITEM(test_rw_i, const int *, a3) = &a[3];
RTEMS_LINKER_RWSET_ITEM_ORDERED(test_rw_i, const int *, a2, 2) = &a[2];
RTEMS_LINKER_RWSET_ITEM_ORDERED(test_rw_i, const int *, a1, 11) = &a[1];
RTEMS_LINKER_RWSET_ITEM_ORDERED(test_rw_i, const int *, a0, 0) = &a[0];

#define OZ OA

RTEMS_LINKER_ROSET_ITEM(test_ro, const int *, ca4) = &ca[4];
RTEMS_LINKER_ROSET_ITEM_ORDERED(test_ro, const int *, ca3, OD) = &ca[3];
RTEMS_LINKER_ROSET_ITEM_REFERENCE(test_ro, const int *, ca2);
RTEMS_LINKER_ROSET_ITEM_ORDERED(test_ro, const int *, ca1, OB) = &ca[1];
RTEMS_LINKER_ROSET_ITEM_ORDERED(test_ro, const int *, ca0, OZ) = &ca[0];

RTEMS_LINKER_ROSET_ITEM(test_ro_i, const int *, ca4) = &ca[4];
RTEMS_LINKER_ROSET_ITEM_ORDERED(test_ro_i, const int *, ca3, OD) = &ca[3];
RTEMS_LINKER_ROSET_ITEM_ORDERED(test_ro_i, const int *, ca2, OC) = &ca[2];
RTEMS_LINKER_ROSET_ITEM_ORDERED(test_ro_i, const int *, ca1, OB) = &ca[1];
RTEMS_LINKER_ROSET_ITEM_ORDERED(test_ro_i, const int *, ca0, OZ) = &ca[0];

/*
 * Demonstrate safe (= define must exist) order definitions, otherwise typos
 * are undetected and may lead to an unpredictable order.  See also above OB
 * vs. OZ.
 */

#define SAFE_ORDER_A 00000
#define SAFE_ORDER_B 00001
#define SAFE_ORDER_C 00002

#define ITEM(i, o) \
  enum { test_ro_s_##i = o - o }; \
  RTEMS_LINKER_RWSET_ITEM_ORDERED(test_ro_s, const int *, i, o) = &i

RTEMS_LINKER_RWSET(test_ro_s, const int *);

static const int s0;
static const int s1;
static const int s2;

ITEM(s2, SAFE_ORDER_C);
ITEM(s1, SAFE_ORDER_B);
ITEM(s0, SAFE_ORDER_A);

RTEMS_LINKER_RWSET(test_rw_empty, const int *);

RTEMS_LINKER_ROSET(test_ro_empty, const int *);

static void test(void)
{
  const int **b;
  const int **e;
  const int * const *cb;
  const int * const *ce;
  const int **bi;
  const int **ei;
  const int * const *cbi;
  const int * const *cei;
  const int * const *sb;
  const int * const *se;
  size_t i;
  const int **item;

  b = RTEMS_LINKER_SET_BEGIN(test_rw);
  e = RTEMS_LINKER_SET_END(test_rw);
  cb = RTEMS_LINKER_SET_BEGIN(test_ro);
  ce = RTEMS_LINKER_SET_END(test_ro);
  bi = RTEMS_LINKER_SET_BEGIN(test_rw_i);
  ei = RTEMS_LINKER_SET_END(test_rw_i);
  cbi = RTEMS_LINKER_SET_BEGIN(test_ro_i);
  cei = RTEMS_LINKER_SET_END(test_ro_i);
  sb = RTEMS_LINKER_SET_BEGIN(test_ro_s);
  se = RTEMS_LINKER_SET_END(test_ro_s);
  RTEMS_OBFUSCATE_VARIABLE(b);
  RTEMS_OBFUSCATE_VARIABLE(e);
  RTEMS_OBFUSCATE_VARIABLE(cb);
  RTEMS_OBFUSCATE_VARIABLE(ce);
  RTEMS_OBFUSCATE_VARIABLE(bi);
  RTEMS_OBFUSCATE_VARIABLE(ei);
  RTEMS_OBFUSCATE_VARIABLE(cbi);
  RTEMS_OBFUSCATE_VARIABLE(cei);
  RTEMS_OBFUSCATE_VARIABLE(sb);
  RTEMS_OBFUSCATE_VARIABLE(se);

  rtems_test_assert((size_t) (e - b) == RTEMS_ARRAY_SIZE(a));
  rtems_test_assert((size_t) (ce - cb) == RTEMS_ARRAY_SIZE(ca));
  rtems_test_assert(
    RTEMS_LINKER_SET_SIZE(test_rw)
      == sizeof(const int *) * RTEMS_ARRAY_SIZE(a)
  );
  rtems_test_assert(
    RTEMS_LINKER_SET_SIZE(test_ro)
      == sizeof(const int *) * RTEMS_ARRAY_SIZE(ca)
  );
  rtems_test_assert(
    RTEMS_LINKER_SET_ITEM_COUNT(test_rw) == RTEMS_ARRAY_SIZE(a)
  );
  rtems_test_assert(
    RTEMS_LINKER_SET_ITEM_COUNT(test_ro) == RTEMS_ARRAY_SIZE(ca)
  );
  rtems_test_assert(!RTEMS_LINKER_SET_IS_EMPTY(test_rw));
  rtems_test_assert(!RTEMS_LINKER_SET_IS_EMPTY(test_ro));

  rtems_test_assert((size_t) (ei - bi) == RTEMS_ARRAY_SIZE(a));
  rtems_test_assert((size_t) (cei - cbi) == RTEMS_ARRAY_SIZE(ca));
  rtems_test_assert((size_t) (se - sb) == 3);
  rtems_test_assert(
    RTEMS_LINKER_SET_SIZE(test_rw_i)
      == sizeof(const int *) * RTEMS_ARRAY_SIZE(a)
  );
  rtems_test_assert(
    RTEMS_LINKER_SET_SIZE(test_ro_i)
      == sizeof(const int *) * RTEMS_ARRAY_SIZE(ca)
  );
  rtems_test_assert(RTEMS_LINKER_SET_SIZE(test_ro_s) == 3 * sizeof(int *));
  rtems_test_assert(
    RTEMS_LINKER_SET_ITEM_COUNT(test_rw_i) == RTEMS_ARRAY_SIZE(a)
  );
  rtems_test_assert(
    RTEMS_LINKER_SET_ITEM_COUNT(test_ro_i) == RTEMS_ARRAY_SIZE(ca)
  );
  rtems_test_assert(RTEMS_LINKER_SET_ITEM_COUNT(test_ro_s) == 3);
  rtems_test_assert(!RTEMS_LINKER_SET_IS_EMPTY(test_rw_i));
  rtems_test_assert(!RTEMS_LINKER_SET_IS_EMPTY(test_ro_i));
  rtems_test_assert(!RTEMS_LINKER_SET_IS_EMPTY(test_ro_s));

  rtems_test_assert(RTEMS_LINKER_SET_SIZE(test_rw_empty) == 0);
  rtems_test_assert(RTEMS_LINKER_SET_SIZE(test_ro_empty) == 0);
  rtems_test_assert(RTEMS_LINKER_SET_ITEM_COUNT(test_rw_empty) == 0);
  rtems_test_assert(RTEMS_LINKER_SET_ITEM_COUNT(test_ro_empty) == 0);
  rtems_test_assert(RTEMS_LINKER_SET_IS_EMPTY(test_rw_empty));
  rtems_test_assert(RTEMS_LINKER_SET_IS_EMPTY(test_ro_empty));

  for (i = 0; i < RTEMS_ARRAY_SIZE(a); ++i) {
    rtems_test_assert(&a[i] == b[i]);
  }

  for (i = 0; i < RTEMS_ARRAY_SIZE(ca); ++i) {
    rtems_test_assert(&ca[i] == cb[i]);
  }

  for (i = 0; i < RTEMS_ARRAY_SIZE(a); ++i) {
    rtems_test_assert(&a[i] == bi[i]);
  }

  for (i = 0; i < RTEMS_ARRAY_SIZE(ca); ++i) {
    rtems_test_assert(&ca[i] == cbi[i]);
  }

  i = 0;
  RTEMS_LINKER_SET_FOREACH(test_rw, item) {
    rtems_test_assert(&a[i] == *item);
    ++i;
  }
  rtems_test_assert(i == RTEMS_ARRAY_SIZE(a));

  i = 0;
  RTEMS_LINKER_SET_FOREACH(test_ro, item) {
    rtems_test_assert(&ca[i] == *item);
    ++i;
  }
  rtems_test_assert(i == RTEMS_ARRAY_SIZE(ca));

  i = 0;
  RTEMS_LINKER_SET_FOREACH(test_rw_i, item) {
    rtems_test_assert(&a[i] == *item);
    ++i;
  }
  rtems_test_assert(i == RTEMS_ARRAY_SIZE(a));

  i = 0;
  RTEMS_LINKER_SET_FOREACH(test_ro_i, item) {
    rtems_test_assert(&ca[i] == *item);
    ++i;
  }
  rtems_test_assert(i == RTEMS_ARRAY_SIZE(ca));

  rtems_test_assert(&s0 == sb[0]);
  rtems_test_assert(&s1 == sb[1]);
  rtems_test_assert(&s2 == sb[2]);

  i = 0;
  RTEMS_LINKER_SET_FOREACH(test_rw_empty, item) {
    ++i;
  }
  rtems_test_assert(i == 0);

  i = 0;
  RTEMS_LINKER_SET_FOREACH(test_ro_empty, item) {
    ++i;
  }
  rtems_test_assert(i == 0);
}

static void test_content(void)
{
  const char *b_rw;
  const char *e_rw;
  const char *b_ro;
  const char *e_ro;

  b_rw = RTEMS_LINKER_SET_BEGIN(test_content_rw);
  e_rw = RTEMS_LINKER_SET_END(test_content_rw);
  b_ro = RTEMS_LINKER_SET_BEGIN(test_content_ro);
  e_ro = RTEMS_LINKER_SET_END(test_content_ro);
  RTEMS_OBFUSCATE_VARIABLE(b_rw);
  RTEMS_OBFUSCATE_VARIABLE(e_rw);
  RTEMS_OBFUSCATE_VARIABLE(b_ro);
  RTEMS_OBFUSCATE_VARIABLE(e_ro);

  rtems_test_assert((uintptr_t) &content_rw_1 >= (uintptr_t) b_rw);
  rtems_test_assert((uintptr_t) &content_rw_2 >= (uintptr_t) b_rw);
  rtems_test_assert((uintptr_t) &content_rw_3 >= (uintptr_t) b_rw);
  rtems_test_assert((uintptr_t) &content_rw_1 <= (uintptr_t) e_rw);
  rtems_test_assert((uintptr_t) &content_rw_2 <= (uintptr_t) e_rw);
  rtems_test_assert((uintptr_t) &content_rw_3 <= (uintptr_t) e_rw);

  rtems_test_assert((uintptr_t) &content_ro_1 >= (uintptr_t) b_ro);
  rtems_test_assert((uintptr_t) &content_ro_2 >= (uintptr_t) b_ro);
  rtems_test_assert((uintptr_t) &content_ro_3 >= (uintptr_t) b_ro);
  rtems_test_assert((uintptr_t) &content_ro_1 <= (uintptr_t) e_ro);
  rtems_test_assert((uintptr_t) &content_ro_2 <= (uintptr_t) e_ro);
  rtems_test_assert((uintptr_t) &content_ro_3 <= (uintptr_t) e_ro);
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();
  test_content();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
