/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
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

static void test(void)
{
  const int * volatile *b = RTEMS_LINKER_SET_BEGIN(test_rw);
  const int * volatile *e = RTEMS_LINKER_SET_END(test_rw);
  const int * volatile const *cb = RTEMS_LINKER_SET_BEGIN(test_ro);
  const int * volatile const *ce = RTEMS_LINKER_SET_END(test_ro);
  const int * volatile *bi = RTEMS_LINKER_SET_BEGIN(test_rw_i);
  const int * volatile *ei = RTEMS_LINKER_SET_END(test_rw_i);
  const int * volatile const *cbi = RTEMS_LINKER_SET_BEGIN(test_ro_i);
  const int * volatile const *cei = RTEMS_LINKER_SET_END(test_ro_i);
  const int * volatile const *sb = RTEMS_LINKER_SET_BEGIN(test_ro_s);
  const int * volatile const *se = RTEMS_LINKER_SET_END(test_ro_s);
  size_t i;

  rtems_test_assert((size_t) (e - b) == RTEMS_ARRAY_SIZE(a));
  rtems_test_assert((size_t) (ce - cb) == RTEMS_ARRAY_SIZE(ca));
  rtems_test_assert(RTEMS_LINKER_SET_SIZE(test_rw) == RTEMS_ARRAY_SIZE(a));
  rtems_test_assert(RTEMS_LINKER_SET_SIZE(test_ro) == RTEMS_ARRAY_SIZE(ca));

  rtems_test_assert((size_t) (ei - bi) == RTEMS_ARRAY_SIZE(a));
  rtems_test_assert((size_t) (cei - cbi) == RTEMS_ARRAY_SIZE(ca));
  rtems_test_assert((size_t) (se - sb) == 3);
  rtems_test_assert(RTEMS_LINKER_SET_SIZE(test_rw_i) == RTEMS_ARRAY_SIZE(a));
  rtems_test_assert(RTEMS_LINKER_SET_SIZE(test_ro_i) == RTEMS_ARRAY_SIZE(ca));
  rtems_test_assert(RTEMS_LINKER_SET_SIZE(test_ro_s) == 3);

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

  rtems_test_assert(&s0 == sb[0]);
  rtems_test_assert(&s1 == sb[1]);
  rtems_test_assert(&s2 == sb[2]);
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
