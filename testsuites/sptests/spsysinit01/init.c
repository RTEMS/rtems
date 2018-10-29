/*
 * Copyright (c) 2015, 2018 embedded brains GmbH.  All rights reserved.
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

#include <sys/types.h>
#include <sys/stat.h>

#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/ioimpl.h>
#include <rtems/libio_.h>
#include <rtems/sysinit.h>

#include <rtems/extensionimpl.h>
#include <rtems/posix/barrierimpl.h>
#include <rtems/posix/mqueueimpl.h>
#include <rtems/posix/muteximpl.h>
#ifdef RTEMS_POSIX_API
#include <rtems/posix/psignalimpl.h>
#endif /* RTEMS_POSIX_API */
#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/rwlockimpl.h>
#include <rtems/posix/semaphoreimpl.h>
#include <rtems/posix/shmimpl.h>
#ifdef RTEMS_POSIX_API
#include <rtems/posix/timerimpl.h>
#endif /* RTEMS_POSIX_API */
#include <rtems/posix/keyimpl.h>
#include <rtems/rtems/barrierimpl.h>
#include <rtems/rtems/dpmemimpl.h>
#include <rtems/rtems/messageimpl.h>
#include <rtems/rtems/partimpl.h>
#include <rtems/rtems/ratemonimpl.h>
#include <rtems/rtems/regionimpl.h>
#include <rtems/rtems/semimpl.h>
#include <rtems/rtems/tasksimpl.h>
#include <rtems/rtems/timerimpl.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/userextimpl.h>
#include <rtems/score/wkspace.h>

const char rtems_test_name[] = "SPSYSINIT 1";

typedef enum {
  BSP_WORK_AREAS_PRE,
  BSP_WORK_AREAS_POST,
  BSP_START_PRE,
  BSP_START_POST,
  CPU_COUNTER_PRE,
  CPU_COUNTER_POST,
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
  CLASSIC_EVENT_PRE,
  CLASSIC_EVENT_POST,
  CLASSIC_MESSAGE_QUEUE_PRE,
  CLASSIC_MESSAGE_QUEUE_POST,
  CLASSIC_SEMAPHORE_PRE,
  CLASSIC_SEMAPHORE_POST,
  CLASSIC_PARTITION_PRE,
  CLASSIC_PARTITION_POST,
  CLASSIC_REGION_PRE,
  CLASSIC_REGION_POST,
  CLASSIC_DUAL_PORTED_MEMORY_PRE,
  CLASSIC_DUAL_PORTED_MEMORY_POST,
  CLASSIC_RATE_MONOTONIC_PRE,
  CLASSIC_RATE_MONOTONIC_POST,
  CLASSIC_BARRIER_PRE,
  CLASSIC_BARRIER_POST,
#ifdef RTEMS_POSIX_API
  POSIX_SIGNALS_PRE,
  POSIX_SIGNALS_POST,
#endif /* RTEMS_POSIX_API */
  POSIX_THREADS_PRE,
  POSIX_THREADS_POST,
  POSIX_MESSAGE_QUEUE_PRE,
  POSIX_MESSAGE_QUEUE_POST,
  POSIX_SEMAPHORE_PRE,
  POSIX_SEMAPHORE_POST,
#ifdef RTEMS_POSIX_API
  POSIX_TIMER_PRE,
  POSIX_TIMER_POST,
#endif /* RTEMS_POSIX_API */
  POSIX_SHM_PRE,
  POSIX_SHM_POST,
  POSIX_KEYS_PRE,
  POSIX_KEYS_POST,
  POSIX_CLEANUP_PRE,
  POSIX_CLEANUP_POST,
  IDLE_THREADS_PRE,
  IDLE_THREADS_POST,
  LIBIO_PRE,
  LIBIO_POST,
  ROOT_FILESYSTEM_PRE,
  ROOT_FILESYSTEM_POST,
  BSP_PRE_DRIVERS_PRE,
  BSP_PRE_DRIVERS_POST,
  DEVICE_DRIVERS_PRE,
  DEVICE_DRIVERS_POST,
  CLASSIC_USER_TASKS_PRE,
  CLASSIC_USER_TASKS_POST,
  POSIX_USER_THREADS_PRE,
  POSIX_USER_THREADS_POST,
  STD_FILE_DESCRIPTORS_PRE,
  STD_FILE_DESCRIPTORS_POST,
  LAST_FIRST,
  LAST_SECOND,
  LAST_THIRD,
  LAST_FOURTH,
  LAST_FIFTH,
  LAST_SIXTH,
  LAST_SEVENTH,
  LAST_EIGHTH,
  LAST_NINETH,
  LAST_TENTH,
  LAST_MIDDLE,
  LAST_LAST,
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

#define LAST_STEP(x) \
  static void last_##x(void) \
  { \
    next_step(LAST_##x); \
  } \
  RTEMS_SYSINIT_ITEM( \
    last_##x, \
    RTEMS_SYSINIT_LAST, \
    RTEMS_SYSINIT_ORDER_##x \
  )

static init_step step;

static void next_step(init_step expected)
{
  assert(step == expected);
  step = expected + 1;
}

FIRST(RTEMS_SYSINIT_BSP_WORK_AREAS)
{
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
  /*
   * Some BSPs initialize the printk() support in bsp_start().  So, print begin
   * of test after bsp_start().
   */
  TEST_BEGIN();

  next_step(BSP_START_POST);
}

FIRST(RTEMS_SYSINIT_CPU_COUNTER)
{
  /*
   * Since the work performed here is BSP-specific, there is no way to test pre
   * and post conditions.
   */
  next_step(CPU_COUNTER_PRE);
}

LAST(RTEMS_SYSINIT_CPU_COUNTER)
{
  next_step(CPU_COUNTER_POST);
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
  assert(_Thread_Internal_information.Objects.maximum == 0);
  next_step(DATA_STRUCTURES_PRE);
}

LAST(RTEMS_SYSINIT_DATA_STRUCTURES)
{
  assert(_Thread_Internal_information.Objects.maximum != 0);
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

FIRST(RTEMS_SYSINIT_CLASSIC_EVENT)
{
  /* There is nothing to do in case RTEMS_MULTIPROCESSING is not defined */
  next_step(CLASSIC_EVENT_PRE);
}

LAST(RTEMS_SYSINIT_CLASSIC_EVENT)
{
  next_step(CLASSIC_EVENT_POST);
}

FIRST(RTEMS_SYSINIT_CLASSIC_MESSAGE_QUEUE)
{
  assert(_Message_queue_Information.maximum == 0);
  next_step(CLASSIC_MESSAGE_QUEUE_PRE);
}

LAST(RTEMS_SYSINIT_CLASSIC_MESSAGE_QUEUE)
{
  assert(_Message_queue_Information.maximum != 0);
  next_step(CLASSIC_MESSAGE_QUEUE_POST);
}

FIRST(RTEMS_SYSINIT_CLASSIC_SEMAPHORE)
{
  assert(_Semaphore_Information.maximum == 0);
  next_step(CLASSIC_SEMAPHORE_PRE);
}

LAST(RTEMS_SYSINIT_CLASSIC_SEMAPHORE)
{
  assert(_Semaphore_Information.maximum != 0);
  next_step(CLASSIC_SEMAPHORE_POST);
}

FIRST(RTEMS_SYSINIT_CLASSIC_PARTITION)
{
  assert(_Partition_Information.maximum == 0);
  next_step(CLASSIC_PARTITION_PRE);
}

LAST(RTEMS_SYSINIT_CLASSIC_PARTITION)
{
  assert(_Partition_Information.maximum != 0);
  next_step(CLASSIC_PARTITION_POST);
}

FIRST(RTEMS_SYSINIT_CLASSIC_REGION)
{
  assert(_Region_Information.maximum == 0);
  next_step(CLASSIC_REGION_PRE);
}

LAST(RTEMS_SYSINIT_CLASSIC_REGION)
{
  assert(_Region_Information.maximum != 0);
  next_step(CLASSIC_REGION_POST);
}

FIRST(RTEMS_SYSINIT_CLASSIC_DUAL_PORTED_MEMORY)
{
  assert(_Dual_ported_memory_Information.maximum == 0);
  next_step(CLASSIC_DUAL_PORTED_MEMORY_PRE);
}

LAST(RTEMS_SYSINIT_CLASSIC_DUAL_PORTED_MEMORY)
{
  assert(_Dual_ported_memory_Information.maximum != 0);
  next_step(CLASSIC_DUAL_PORTED_MEMORY_POST);
}

FIRST(RTEMS_SYSINIT_CLASSIC_RATE_MONOTONIC)
{
  assert(_Rate_monotonic_Information.maximum == 0);
  next_step(CLASSIC_RATE_MONOTONIC_PRE);
}

LAST(RTEMS_SYSINIT_CLASSIC_RATE_MONOTONIC)
{
  assert(_Rate_monotonic_Information.maximum != 0);
  next_step(CLASSIC_RATE_MONOTONIC_POST);
}

FIRST(RTEMS_SYSINIT_CLASSIC_BARRIER)
{
  assert(_Barrier_Information.maximum == 0);
  next_step(CLASSIC_BARRIER_PRE);
}

LAST(RTEMS_SYSINIT_CLASSIC_BARRIER)
{
  assert(_Barrier_Information.maximum != 0);
  next_step(CLASSIC_BARRIER_POST);
}

#ifdef RTEMS_POSIX_API
FIRST(RTEMS_SYSINIT_POSIX_SIGNALS)
{
  assert(
    memcmp(
      &_POSIX_signals_Vectors,
      _POSIX_signals_Default_vectors,
      sizeof(_POSIX_signals_Vectors)
    ) != 0
  );
  next_step(POSIX_SIGNALS_PRE);
}

LAST(RTEMS_SYSINIT_POSIX_SIGNALS)
{
  assert(
    memcmp(
      &_POSIX_signals_Vectors,
      _POSIX_signals_Default_vectors,
      sizeof(_POSIX_signals_Vectors)
    ) == 0
  );
  next_step(POSIX_SIGNALS_POST);
}
#endif /* RTEMS_POSIX_API */

FIRST(RTEMS_SYSINIT_POSIX_THREADS)
{
  assert(_POSIX_Threads_Information.Objects.maximum == 0);
  next_step(POSIX_THREADS_PRE);
}

LAST(RTEMS_SYSINIT_POSIX_THREADS)
{
  assert(_POSIX_Threads_Information.Objects.maximum != 0);
  next_step(POSIX_THREADS_POST);
}

FIRST(RTEMS_SYSINIT_POSIX_MESSAGE_QUEUE)
{
  assert(_POSIX_Message_queue_Information.maximum == 0);
  next_step(POSIX_MESSAGE_QUEUE_PRE);
}

LAST(RTEMS_SYSINIT_POSIX_MESSAGE_QUEUE)
{
  assert(_POSIX_Message_queue_Information.maximum != 0);
  next_step(POSIX_MESSAGE_QUEUE_POST);
}

FIRST(RTEMS_SYSINIT_POSIX_SEMAPHORE)
{
  assert(_POSIX_Semaphore_Information.maximum == 0);
  next_step(POSIX_SEMAPHORE_PRE);
}

LAST(RTEMS_SYSINIT_POSIX_SEMAPHORE)
{
  assert(_POSIX_Semaphore_Information.maximum != 0);
  next_step(POSIX_SEMAPHORE_POST);
}

#ifdef RTEMS_POSIX_API
FIRST(RTEMS_SYSINIT_POSIX_TIMER)
{
  assert(_POSIX_Timer_Information.maximum == 0);
  next_step(POSIX_TIMER_PRE);
}

LAST(RTEMS_SYSINIT_POSIX_TIMER)
{
  assert(_POSIX_Timer_Information.maximum != 0);
  next_step(POSIX_TIMER_POST);
}
#endif /* RTEMS_POSIX_API */

FIRST(RTEMS_SYSINIT_POSIX_SHM)
{
  assert(_POSIX_Shm_Information.maximum == 0);
  next_step(POSIX_SHM_PRE);
}

LAST(RTEMS_SYSINIT_POSIX_SHM)
{
  assert(_POSIX_Shm_Information.maximum != 0);
  next_step(POSIX_SHM_POST);
}

static size_t user_extensions_pre_posix_cleanup;

FIRST(RTEMS_SYSINIT_POSIX_CLEANUP)
{
  user_extensions_pre_posix_cleanup =
    _Chain_Node_count_unprotected(&_User_extensions_List.Active);
  next_step(POSIX_CLEANUP_PRE);
}

LAST(RTEMS_SYSINIT_POSIX_CLEANUP)
{
  assert(
    user_extensions_pre_posix_cleanup + 1 ==
      _Chain_Node_count_unprotected(&_User_extensions_List.Active)
  );
  next_step(POSIX_CLEANUP_POST);
}

FIRST(RTEMS_SYSINIT_POSIX_KEYS)
{
  assert(_POSIX_Keys_Information.maximum == 0);
  next_step(POSIX_KEYS_PRE);
}

LAST(RTEMS_SYSINIT_POSIX_KEYS)
{
  assert(_POSIX_Keys_Information.maximum != 0);
  next_step(POSIX_KEYS_POST);
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

FIRST(RTEMS_SYSINIT_LIBIO)
{
  assert(rtems_libio_iop_free_head == NULL);
  next_step(LIBIO_PRE);
}

LAST(RTEMS_SYSINIT_LIBIO)
{
  assert(rtems_libio_iop_free_head == &rtems_libio_iops[0]);
  next_step(LIBIO_POST);
}

FIRST(RTEMS_SYSINIT_ROOT_FILESYSTEM)
{
  struct stat st;
  int rv;

  rv = stat("/", &st);
  assert(rv == -1);
  next_step(ROOT_FILESYSTEM_PRE);
}

LAST(RTEMS_SYSINIT_ROOT_FILESYSTEM)
{
  struct stat st;
  int rv;

  rv = stat("/", &st);
  assert(rv == 0);
  next_step(ROOT_FILESYSTEM_POST);
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

FIRST(RTEMS_SYSINIT_CLASSIC_USER_TASKS)
{
  _Objects_Allocator_lock();
  assert(_Objects_Active_count(&_RTEMS_tasks_Information.Objects) == 0);
  _Objects_Allocator_unlock();
  next_step(CLASSIC_USER_TASKS_PRE);
}

LAST(RTEMS_SYSINIT_CLASSIC_USER_TASKS)
{
  _Objects_Allocator_lock();
  assert(_Objects_Active_count(&_RTEMS_tasks_Information.Objects) == 1);
  _Objects_Allocator_unlock();
  next_step(CLASSIC_USER_TASKS_POST);
}

FIRST(RTEMS_SYSINIT_POSIX_USER_THREADS)
{
  _Objects_Allocator_lock();
  assert(_Objects_Active_count(&_POSIX_Threads_Information.Objects) == 0);
  _Objects_Allocator_unlock();
  next_step(POSIX_USER_THREADS_PRE);
}

LAST(RTEMS_SYSINIT_POSIX_USER_THREADS)
{
  _Objects_Allocator_lock();
  assert(_Objects_Active_count(&_POSIX_Threads_Information.Objects) == 1);
  _Objects_Allocator_unlock();
  next_step(POSIX_USER_THREADS_POST);
}

FIRST(RTEMS_SYSINIT_STD_FILE_DESCRIPTORS)
{
  struct stat st;
  int rv;

  rv = fstat(0, &st);
  assert(rv == -1);
  next_step(STD_FILE_DESCRIPTORS_PRE);
}

LAST(RTEMS_SYSINIT_STD_FILE_DESCRIPTORS)
{
  struct stat st;
  int rv;

  rv = fstat(0, &st);
  assert(rv == 0);
  next_step(STD_FILE_DESCRIPTORS_POST);
}

LAST_STEP(FIRST);
LAST_STEP(SECOND);
LAST_STEP(THIRD);
LAST_STEP(FOURTH);
LAST_STEP(FIFTH);
LAST_STEP(SIXTH);
LAST_STEP(SEVENTH);
LAST_STEP(EIGHTH);
LAST_STEP(NINETH);
LAST_STEP(TENTH);
LAST_STEP(MIDDLE);
LAST_STEP(LAST);

static void Init(rtems_task_argument arg)
{
  pthread_cleanup_push(NULL, NULL);
  pthread_cleanup_pop(0);
  next_step(INIT_TASK);
  TEST_END();
  exit(0);
}

static void *POSIX_Init(void *arg)
{
  return NULL;
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1

#define CONFIGURE_MAXIMUM_BARRIERS 1

#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES 1

#define CONFIGURE_MAXIMUM_PARTITIONS 1

#define CONFIGURE_MAXIMUM_PERIODS 1

#define CONFIGURE_MAXIMUM_PORTS 1

#define CONFIGURE_MAXIMUM_REGIONS 1

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_MAXIMUM_TIMERS 1

#define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES 1

#define CONFIGURE_MAXIMUM_POSIX_SEMAPHORES 1

#define CONFIGURE_MAXIMUM_POSIX_SHMS 1

#ifdef RTEMS_POSIX_API
#define CONFIGURE_MAXIMUM_POSIX_TIMERS 1
#endif /* RTEMS_POSIX_API */

#define CONFIGURE_MAXIMUM_POSIX_THREADS 1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_MAXIMUM_POSIX_KEYS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_STACK_CHECKER_ENABLED

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
