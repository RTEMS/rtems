/*
 * Copyright (c) 2012-2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/libcsupport.h>

#include <string.h>

#include <rtems/libio_.h>
#include <rtems/malloc.h>
#include <rtems/score/rbtreeimpl.h>
#include <rtems/score/protectedheap.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/wkspace.h>

#include <rtems/posix/keyimpl.h>

#include <rtems/rtems/barrierimpl.h>
#include <rtems/extensionimpl.h>
#include <rtems/rtems/dpmemimpl.h>
#include <rtems/rtems/messageimpl.h>
#include <rtems/rtems/partimpl.h>
#include <rtems/rtems/ratemonimpl.h>
#include <rtems/rtems/regionimpl.h>
#include <rtems/rtems/semimpl.h>
#include <rtems/rtems/tasksimpl.h>
#include <rtems/rtems/timerimpl.h>

#ifdef RTEMS_POSIX_API
  #include <rtems/posix/barrierimpl.h>
  #include <rtems/posix/condimpl.h>
  #include <rtems/posix/mqueueimpl.h>
  #include <rtems/posix/muteximpl.h>
  #include <rtems/posix/psignal.h>
  #include <rtems/posix/pthreadimpl.h>
  #include <rtems/posix/rwlockimpl.h>
  #include <rtems/posix/semaphoreimpl.h>
  #include <rtems/posix/spinlockimpl.h>
  #include <rtems/posix/timerimpl.h>
#endif

static const Objects_Information *const objects_info_table[] = {
  &_POSIX_Keys_Information,
  &_Barrier_Information,
  &_Extension_Information,
  &_Message_queue_Information,
  &_Partition_Information,
  &_Rate_monotonic_Information,
  &_Dual_ported_memory_Information,
  &_Region_Information,
  &_Semaphore_Information,
  &_RTEMS_tasks_Information.Objects,
  &_Timer_Information
  #ifdef RTEMS_POSIX_API
    ,
    &_POSIX_Barrier_Information,
    &_POSIX_Condition_variables_Information,
    &_POSIX_Message_queue_Information,
    &_POSIX_Message_queue_Information_fds,
    &_POSIX_Mutex_Information,
    &_POSIX_RWLock_Information,
    &_POSIX_Semaphore_Information,
    &_POSIX_Spinlock_Information,
    &_POSIX_Threads_Information.Objects,
    &_POSIX_Timer_Information
  #endif
};

static int open_files(void)
{
  int free_count = 0;
  rtems_libio_t *iop;

  rtems_libio_lock();

  iop = rtems_libio_iop_freelist;
  while (iop != NULL) {
    ++free_count;

    iop = iop->data1;
  }

  rtems_libio_unlock();

  return (int) rtems_libio_number_iops - free_count;
}

static void get_heap_info(Heap_Control *heap, Heap_Information_block *info)
{
  _Heap_Get_information(heap, info);
  memset(&info->Stats, 0, sizeof(info->Stats));
}

static bool count_posix_key_value_pairs(
  const RBTree_Node *node,
  RBTree_Direction dir,
  void *visitor_arg
)
{
  uint32_t *count = visitor_arg;

  (void) node;
  (void) dir;

  ++(*count);

  return false;
}

static uint32_t get_active_posix_key_value_pairs(void)
{
  uint32_t count = 0;

  _Thread_Disable_dispatch();
  _RBTree_Iterate(
    &_POSIX_Keys_Key_value_lookup_tree,
    RBT_LEFT,
    count_posix_key_value_pairs,
    &count
  );
  _Thread_Enable_dispatch();

  return count;
}

void rtems_resource_snapshot_take(rtems_resource_snapshot *snapshot)
{
  uint32_t *active = &snapshot->active_posix_keys;
  size_t i;

  memset(snapshot, 0, sizeof(*snapshot));

  _RTEMS_Lock_allocator();

  _Thread_Kill_zombies();

  get_heap_info(RTEMS_Malloc_Heap, &snapshot->heap_info);
  get_heap_info(&_Workspace_Area, &snapshot->workspace_info);

  for (i = 0; i < RTEMS_ARRAY_SIZE(objects_info_table); ++i) {
    active [i] = _Objects_Active_count(objects_info_table[i]);
  }

  _RTEMS_Unlock_allocator();

  snapshot->active_posix_key_value_pairs = get_active_posix_key_value_pairs();
  snapshot->open_files = open_files();
}

bool rtems_resource_snapshot_equal(
  const rtems_resource_snapshot *a,
  const rtems_resource_snapshot *b
)
{
  return memcmp(a, b, sizeof(*a)) == 0;
}

bool rtems_resource_snapshot_check(const rtems_resource_snapshot *snapshot)
{
  rtems_resource_snapshot now;

  rtems_resource_snapshot_take(&now);

  return rtems_resource_snapshot_equal(&now, snapshot);
}
