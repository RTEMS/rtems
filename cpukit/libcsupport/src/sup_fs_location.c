/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__

#include <stdlib.h>

#include <rtems/libio_.h>
#include <rtems/score/thread.h>

static rtems_filesystem_global_location_t *deferred_released_global_locations;

rtems_filesystem_location_info_t *rtems_filesystem_location_copy(
  rtems_filesystem_location_info_t *dst,
  const rtems_filesystem_location_info_t *src
)
{
  dst->node_access = src->node_access;
  dst->node_access_2 = src->node_access_2;
  dst->handlers = src->handlers;
  dst->mt_entry = src->mt_entry;
  rtems_filesystem_location_add_to_mt_entry(dst);

  return dst;
}

void rtems_filesystem_location_detach(
  rtems_filesystem_location_info_t *detach
)
{
  rtems_filesystem_location_free(detach);
  rtems_filesystem_location_initialize_to_null(detach);
}

void rtems_filesystem_location_copy_and_detach(
  rtems_filesystem_location_info_t *copy,
  rtems_filesystem_location_info_t *detach
)
{
  rtems_filesystem_location_copy(copy, detach);
  rtems_filesystem_location_remove_from_mt_entry(detach);
  rtems_filesystem_location_initialize_to_null(detach);
}

rtems_filesystem_global_location_t *rtems_filesystem_location_transform_to_global(
  rtems_filesystem_location_info_t *loc
)
{
  rtems_filesystem_global_location_t *global_loc = malloc(sizeof(*global_loc));

  if (global_loc != NULL) {
    global_loc->reference_count = 1;
    global_loc->deferred_released_next = NULL;
    global_loc->deferred_released_count = 0;
    rtems_filesystem_location_copy(&global_loc->location, loc);
    rtems_filesystem_location_remove_from_mt_entry(loc);
  } else {
    rtems_filesystem_location_free(loc);
    global_loc = rtems_filesystem_global_location_obtain_null();
    errno = ENOMEM;
  }

  return global_loc;
}

void rtems_filesystem_global_location_assign(
  rtems_filesystem_global_location_t **lhs_global_loc_ptr,
  rtems_filesystem_global_location_t *rhs_global_loc
)
{
  rtems_filesystem_mt_entry_declare_lock_context(lock_context);
  rtems_filesystem_global_location_t *lhs_global_loc;

  rtems_filesystem_mt_entry_lock(lock_context);
  lhs_global_loc = *lhs_global_loc_ptr;
  *lhs_global_loc_ptr = rhs_global_loc;
  rtems_filesystem_mt_entry_unlock(lock_context);

  rtems_filesystem_global_location_release(lhs_global_loc);
}

static void release_with_count(
  rtems_filesystem_global_location_t *global_loc,
  int count
)
{
  rtems_filesystem_mount_table_entry_t *mt_entry =
    global_loc->location.mt_entry;
  rtems_filesystem_mt_entry_declare_lock_context(lock_context);
  bool do_free;
  bool do_unmount;

  rtems_filesystem_mt_entry_lock(lock_context);
  global_loc->reference_count -= count;
  do_free = global_loc->reference_count == 0;
  do_unmount = rtems_filesystem_is_ready_for_unmount(mt_entry);
  rtems_filesystem_mt_entry_unlock(lock_context);

  if (do_free) {
    rtems_filesystem_location_free(&global_loc->location);
    free(global_loc);
  }

  if (do_unmount) {
    rtems_filesystem_do_unmount(mt_entry);
  }
}

static void deferred_release(void)
{
  rtems_filesystem_global_location_t *current = NULL;

  do {
    int count = 0;

    _Thread_Disable_dispatch();
    current = deferred_released_global_locations;
    if (current != NULL) {
      deferred_released_global_locations = current->deferred_released_next;
      count = current->deferred_released_count;
      current->deferred_released_next = NULL;
      current->deferred_released_count = 0;
    }
    _Thread_Enable_dispatch();

    if (current != NULL) {
      release_with_count(current, count);
    }
  } while (current != NULL);
}

rtems_filesystem_global_location_t *rtems_filesystem_global_location_obtain(
  rtems_filesystem_global_location_t *const *global_loc_ptr
)
{
  rtems_filesystem_mt_entry_declare_lock_context(lock_context);
  rtems_filesystem_global_location_t *global_loc;

  if (deferred_released_global_locations != NULL) {
    deferred_release();
  }

  rtems_filesystem_mt_entry_lock(lock_context);
  global_loc = *global_loc_ptr;
  if (global_loc == NULL || !global_loc->location.mt_entry->mounted) {
    global_loc = &rtems_filesystem_global_location_null;
    errno = ENXIO;
  }
  ++global_loc->reference_count;
  rtems_filesystem_mt_entry_unlock(lock_context);

  return global_loc;
}

void rtems_filesystem_global_location_release(
  rtems_filesystem_global_location_t *global_loc
)
{
  if (!_Thread_Dispatch_in_critical_section()) {
    release_with_count(global_loc, 1);
  } else {
    if (global_loc->deferred_released_count == 0) {
      rtems_filesystem_global_location_t *head =
        deferred_released_global_locations;

      global_loc->deferred_released_count = 1;
      global_loc->deferred_released_next = head;
      deferred_released_global_locations = global_loc;
    } else {
      ++global_loc->deferred_released_count;
    }
  }
}

void rtems_filesystem_location_remove_from_mt_entry(
  rtems_filesystem_location_info_t *loc
)
{
  rtems_filesystem_mt_entry_declare_lock_context(lock_context);
  bool do_unmount;

  rtems_filesystem_mt_entry_lock(lock_context);
  rtems_chain_extract_unprotected(&loc->mt_entry_node);
  do_unmount = rtems_filesystem_is_ready_for_unmount(loc->mt_entry);
  rtems_filesystem_mt_entry_unlock(lock_context);

  if (do_unmount) {
    rtems_filesystem_do_unmount(loc->mt_entry);
  }
}

void rtems_filesystem_do_unmount(
  rtems_filesystem_mount_table_entry_t *mt_entry
)
{
  rtems_filesystem_mt_lock();
  rtems_chain_extract_unprotected(&mt_entry->mt_node);
  rtems_filesystem_mt_unlock();
  rtems_filesystem_global_location_release(mt_entry->mt_point_node);
  (*mt_entry->ops->fsunmount_me_h)(mt_entry);

  if (mt_entry->unmount_task != 0) {
    rtems_status_code sc =
      rtems_event_send(mt_entry->unmount_task, RTEMS_FILESYSTEM_UNMOUNT_EVENT);
    if (sc != RTEMS_SUCCESSFUL) {
      rtems_fatal_error_occurred(0xdeadbeef);
    }
  }

  free(mt_entry);
}
