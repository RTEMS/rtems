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

#include <assert.h>
#include <stdlib.h>

#include <rtems.h>
#include <rtems/libio_.h>
#include <rtems/sysinit.h>
#include <rtems/test.h>

#include <rtems/score/sysstate.h>
#include <rtems/score/wkspace.h>

const char rtems_test_name[] = "SPSYSINIT 1";

#define FIRST(x) \
  static void x##_first(void); \
  RTEMS_SYSINIT_ITEM( \
    x##_first, \
    x, \
    RTEMS_SYSINIT_ORDER_FIRST \
  ); \
  static void x##_first(void)

#define LAST(x) \
  static void x##_last(void); \
  RTEMS_SYSINIT_ITEM( \
    x##_last, \
    x, \
    RTEMS_SYSINIT_ORDER_LAST \
  ); \
  static void x##_last(void)

static int step;

static void next_step(int expected)
{
  assert(step == expected);
  step = expected + 1;
}

FIRST(RTEMS_SYSINIT_BSP_WORK_AREAS)
{
  rtems_test_begink();
  assert(_Workspace_Area.area_begin == 0);
  next_step(0);
}

LAST(RTEMS_SYSINIT_BSP_WORK_AREAS)
{
  assert(_Workspace_Area.area_begin != 0);
  next_step(1);
}

FIRST(RTEMS_SYSINIT_BSP_START)
{
  /*
   * Since the work performed here is BSP-specific, there is no way to test pre
   * and post conditions.
   */
  next_step(2);
}

LAST(RTEMS_SYSINIT_BSP_START)
{
  next_step(3);
}

FIRST(RTEMS_SYSINIT_DATA_STRUCTURES)
{
  assert(_System_state_Is_before_initialization(_System_state_Get()));
  next_step(4);
}

LAST(RTEMS_SYSINIT_DATA_STRUCTURES)
{
  assert(_System_state_Is_before_multitasking(_System_state_Get()));
  next_step(5);
}

FIRST(RTEMS_SYSINIT_BSP_LIBC)
{
  assert(rtems_libio_semaphore == 0);
  next_step(6);
}

LAST(RTEMS_SYSINIT_BSP_LIBC)
{
  assert(rtems_libio_semaphore != 0);
  next_step(7);
}

FIRST(RTEMS_SYSINIT_BEFORE_DRIVERS)
{
  /* Omit test of build configuration specific pre and post conditions */
  next_step(8);
}

LAST(RTEMS_SYSINIT_BEFORE_DRIVERS)
{
  next_step(9);
}

FIRST(RTEMS_SYSINIT_BSP_PRE_DRIVERS)
{
  /*
   * Since the work performed here is BSP-specific, there is no way to test pre
   * and post conditions.
   */
  next_step(10);
}

LAST(RTEMS_SYSINIT_BSP_PRE_DRIVERS)
{
  next_step(11);
}

FIRST(RTEMS_SYSINIT_DEVICE_DRIVERS)
{
  assert(!_IO_All_drivers_initialized);
  next_step(12);
}

LAST(RTEMS_SYSINIT_DEVICE_DRIVERS)
{
  assert(_IO_All_drivers_initialized);
  next_step(13);
}

FIRST(RTEMS_SYSINIT_BSP_POST_DRIVERS)
{
  assert(rtems_libio_iop_freelist != NULL);
  next_step(14);
}

LAST(RTEMS_SYSINIT_BSP_POST_DRIVERS)
{
  assert(rtems_libio_iop_freelist == NULL);
  next_step(15);
}

static void Init(rtems_task_argument arg)
{
  next_step(16);
  rtems_test_endk();
  exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
