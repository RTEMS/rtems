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

#include <rtems/extensionimpl.h>
#include <rtems/rtems/tasksimpl.h>
#include <rtems/rtems/timerimpl.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/userextimpl.h>
#include <rtems/score/wkspace.h>

const char rtems_test_name[] = "SPSYSINIT 1";

typedef enum {
  BSP_WORK_AREAS_PRE,
  BSP_WORK_AREAS_POST,
  BSP_START_PRE,
  BSP_START_POST,
  INITIAL_EXTENSIONS_PRE,
  INITIAL_EXTENSIONS_POST,
  DATA_STRUCTURES_PRE,
  DATA_STRUCTURES_POST,
  USER_EXTENSIONS_PRE,
  USER_EXTENSIONS_POST,
  CLASSIC_TASKS_PRE,
  CLASSIC_TASKS_POST,
  CLASSIC_TIMER_PRE,
  CLASSIC_TIMER_POST,
  CLASSIC_SIGNAL_PRE,
  CLASSIC_SIGNAL_POST,
  IDLE_THREADS_PRE,
  IDLE_THREADS_POST,
  BSP_LIBC_PRE,
  BSP_LIBC_POST,
  BEFORE_DRIVERS_PRE,
  BEFORE_DRIVERS_POST,
  BSP_PRE_DRIVERS_PRE,
  BSP_PRE_DRIVERS_POST,
  DEVICE_DRIVERS_PRE,
  DEVICE_DRIVERS_POST,
  BSP_POST_DRIVERS_PRE,
  BSP_POST_DRIVERS_POST,
  INIT_TASK,
  DONE
} init_step;

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

static init_step step;

static void next_step(init_step expected)
{
  assert(step == expected);
  step = expected + 1;
}

FIRST(RTEMS_SYSINIT_BSP_WORK_AREAS)
{
  rtems_test_begink();
  assert(_Workspace_Area.area_begin == 0);
  next_step(BSP_WORK_AREAS_PRE);
}

LAST(RTEMS_SYSINIT_BSP_WORK_AREAS)
{
  assert(_Workspace_Area.area_begin != 0);
  next_step(BSP_WORK_AREAS_POST);
}

FIRST(RTEMS_SYSINIT_BSP_START)
{
  /*
   * Since the work performed here is BSP-specific, there is no way to test pre
   * and post conditions.
   */
  next_step(BSP_START_PRE);
}

LAST(RTEMS_SYSINIT_BSP_START)
{
  next_step(BSP_START_POST);
}

FIRST(RTEMS_SYSINIT_INITIAL_EXTENSIONS)
{
  assert(_Chain_Is_empty(&_User_extensions_Switches_list));
  next_step(INITIAL_EXTENSIONS_PRE);
}

LAST(RTEMS_SYSINIT_INITIAL_EXTENSIONS)
{
  assert(!_Chain_Is_empty(&_User_extensions_Switches_list));
  next_step(INITIAL_EXTENSIONS_POST);
}

FIRST(RTEMS_SYSINIT_DATA_STRUCTURES)
{
  assert(_RTEMS_Allocator_Mutex == NULL);
  next_step(DATA_STRUCTURES_PRE);
}

LAST(RTEMS_SYSINIT_DATA_STRUCTURES)
{
  assert(_RTEMS_Allocator_Mutex != NULL);
  next_step(DATA_STRUCTURES_POST);
}

FIRST(RTEMS_SYSINIT_USER_EXTENSIONS)
{
  assert(_Extension_Information.maximum == 0);
  next_step(USER_EXTENSIONS_PRE);
}

LAST(RTEMS_SYSINIT_USER_EXTENSIONS)
{
  assert(_Extension_Information.maximum != 0);
  next_step(USER_EXTENSIONS_POST);
}

FIRST(RTEMS_SYSINIT_CLASSIC_TASKS)
{
  assert(_RTEMS_tasks_Information.Objects.maximum == 0);
  next_step(CLASSIC_TASKS_PRE);
}

LAST(RTEMS_SYSINIT_CLASSIC_TASKS)
{
  assert(_RTEMS_tasks_Information.Objects.maximum != 0);
  next_step(CLASSIC_TASKS_POST);
}

FIRST(RTEMS_SYSINIT_CLASSIC_TIMER)
{
  assert(_Timer_Information.maximum == 0);
  next_step(CLASSIC_TIMER_PRE);
}

LAST(RTEMS_SYSINIT_CLASSIC_TIMER)
{
  assert(_Timer_Information.maximum != 0);
  next_step(CLASSIC_TIMER_POST);
}

FIRST(RTEMS_SYSINIT_CLASSIC_SIGNAL)
{
  /* There is nothing to do in case RTEMS_MULTIPROCESSING is not defined */
  next_step(CLASSIC_SIGNAL_PRE);
}

LAST(RTEMS_SYSINIT_CLASSIC_SIGNAL)
{
  next_step(CLASSIC_SIGNAL_POST);
}

FIRST(RTEMS_SYSINIT_IDLE_THREADS)
{
  assert(_System_state_Is_before_initialization(_System_state_Get()));
  next_step(IDLE_THREADS_PRE);
}

LAST(RTEMS_SYSINIT_IDLE_THREADS)
{
  assert(_System_state_Is_before_multitasking(_System_state_Get()));
  next_step(IDLE_THREADS_POST);
}

FIRST(RTEMS_SYSINIT_BSP_LIBC)
{
  assert(rtems_libio_semaphore == 0);
  next_step(BSP_LIBC_PRE);
}

LAST(RTEMS_SYSINIT_BSP_LIBC)
{
  assert(rtems_libio_semaphore != 0);
  next_step(BSP_LIBC_POST);
}

FIRST(RTEMS_SYSINIT_BEFORE_DRIVERS)
{
  /* Omit test of build configuration specific pre and post conditions */
  next_step(BEFORE_DRIVERS_PRE);
}

LAST(RTEMS_SYSINIT_BEFORE_DRIVERS)
{
  next_step(BEFORE_DRIVERS_POST);
}

FIRST(RTEMS_SYSINIT_BSP_PRE_DRIVERS)
{
  /*
   * Since the work performed here is BSP-specific, there is no way to test pre
   * and post conditions.
   */
  next_step(BSP_PRE_DRIVERS_PRE);
}

LAST(RTEMS_SYSINIT_BSP_PRE_DRIVERS)
{
  next_step(BSP_PRE_DRIVERS_POST);
}

FIRST(RTEMS_SYSINIT_DEVICE_DRIVERS)
{
  assert(!_IO_All_drivers_initialized);
  next_step(DEVICE_DRIVERS_PRE);
}

LAST(RTEMS_SYSINIT_DEVICE_DRIVERS)
{
  assert(_IO_All_drivers_initialized);
  next_step(DEVICE_DRIVERS_POST);
}

FIRST(RTEMS_SYSINIT_BSP_POST_DRIVERS)
{
  assert(rtems_libio_iop_freelist != NULL);
  next_step(BSP_POST_DRIVERS_PRE);
}

LAST(RTEMS_SYSINIT_BSP_POST_DRIVERS)
{
  assert(rtems_libio_iop_freelist == NULL);
  next_step(BSP_POST_DRIVERS_POST);
}

static void Init(rtems_task_argument arg)
{
  next_step(INIT_TASK);
  rtems_test_endk();
  exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_MAXIMUM_TIMERS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_STACK_CHECKER_ENABLED

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
