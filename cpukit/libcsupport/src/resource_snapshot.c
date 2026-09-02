/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of
 *   rtems_resource_snapshot_take(), rtems_resource_snapshot_equal() and
 *   rtems_resource_snapshot_check().
 */

/*
 * Copyright (C) 2012, 2014 embedded brains GmbH & Co. KG
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

#include <rtems/libcsupport.h>

#include <stddef.h>
#include <string.h>

#include <rtems/libio_.h>
#include <rtems/malloc.h>
#include <rtems/score/rbtreeimpl.h>
#include <rtems/score/protectedheap.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/wkspace.h>
#include <rtems/posix/keyimpl.h>

static const struct {
  Objects_APIs api;
  uint16_t     cls;
  uint16_t     offset;
} objects_info_table[] = {
  { OBJECTS_POSIX_API,
    OBJECTS_POSIX_KEYS,
    offsetof( rtems_resource_snapshot, active_posix_keys ) },
  { OBJECTS_CLASSIC_API,
    OBJECTS_RTEMS_BARRIERS,
    offsetof( rtems_resource_snapshot, rtems_api.active_barriers ) },
  { OBJECTS_CLASSIC_API,
    OBJECTS_RTEMS_EXTENSIONS,
    offsetof( rtems_resource_snapshot, rtems_api.active_extensions ) },
  { OBJECTS_CLASSIC_API,
    OBJECTS_RTEMS_MESSAGE_QUEUES,
    offsetof( rtems_resource_snapshot, rtems_api.active_message_queues ) },
  { OBJECTS_CLASSIC_API,
    OBJECTS_RTEMS_PARTITIONS,
    offsetof( rtems_resource_snapshot, rtems_api.active_partitions ) },
  { OBJECTS_CLASSIC_API,
    OBJECTS_RTEMS_PERIODS,
    offsetof( rtems_resource_snapshot, rtems_api.active_periods ) },
  { OBJECTS_CLASSIC_API,
    OBJECTS_RTEMS_PORTS,
    offsetof( rtems_resource_snapshot, rtems_api.active_ports ) },
  { OBJECTS_CLASSIC_API,
    OBJECTS_RTEMS_REGIONS,
    offsetof( rtems_resource_snapshot, rtems_api.active_regions ) },
  { OBJECTS_CLASSIC_API,
    OBJECTS_RTEMS_SEMAPHORES,
    offsetof( rtems_resource_snapshot, rtems_api.active_semaphores ) },
  { OBJECTS_CLASSIC_API,
    OBJECTS_RTEMS_TASKS,
    offsetof( rtems_resource_snapshot, rtems_api.active_tasks ) },
  { OBJECTS_CLASSIC_API,
    OBJECTS_RTEMS_TIMERS,
    offsetof( rtems_resource_snapshot, rtems_api.active_timers ) },
  { OBJECTS_POSIX_API,
    OBJECTS_POSIX_MESSAGE_QUEUES,
    offsetof( rtems_resource_snapshot, posix_api.active_message_queues ) },
  { OBJECTS_POSIX_API,
    OBJECTS_POSIX_SEMAPHORES,
    offsetof( rtems_resource_snapshot, posix_api.active_semaphores ) },
  { OBJECTS_POSIX_API,
    OBJECTS_POSIX_THREADS,
    offsetof( rtems_resource_snapshot, posix_api.active_threads ) }
  #ifdef RTEMS_POSIX_API
  ,
  { OBJECTS_POSIX_API,
    OBJECTS_POSIX_TIMERS,
    offsetof( rtems_resource_snapshot, posix_api.active_timers ) }
  #endif
};

RTEMS_STATIC_ASSERT(
  sizeof( rtems_resource_snapshot ) <= UINT16_MAX,
  RESOURCE_SNAPSHOT_OFFSET
);

static int open_files( void )
{
  int            free_count = 0;
  rtems_libio_t *iop;

  rtems_libio_lock();

  iop = rtems_libio_iop_free_head;
  while ( iop != NULL ) {
    ++free_count;

    iop = iop->data1;
  }

  rtems_libio_unlock();

  return (int) rtems_libio_number_iops - free_count;
}

static void get_heap_info( Heap_Control *heap, Heap_Information_block *info )
{
  _Heap_Get_information( heap, info );
  memset( &info->Stats, 0, sizeof( info->Stats ) );
}

static POSIX_Keys_Control *get_next_key( Objects_Id *id )
{
  return (
    POSIX_Keys_Control *
  ) _Objects_Get_next( *id, &_POSIX_Keys_Information, id );
}

static uint32_t get_active_posix_key_value_pairs( void )
{
  uint32_t            count = 0;
  Objects_Id          id = OBJECTS_ID_INITIAL_INDEX;
  POSIX_Keys_Control *the_key;

  while ( ( the_key = get_next_key( &id ) ) != NULL ) {
    count += _Chain_Node_count_unprotected( &the_key->Key_value_pairs );
    _Objects_Allocator_unlock();
  }

  return count;
}

void rtems_resource_snapshot_take( rtems_resource_snapshot *snapshot )
{
  size_t i;

  memset( snapshot, 0, sizeof( *snapshot ) );

  _RTEMS_Lock_allocator();

  _Thread_Kill_zombies();

  get_heap_info( RTEMS_Malloc_Heap, &snapshot->heap_info );
  get_heap_info( &_Workspace_Area, &snapshot->workspace_info );

  for ( i = 0; i < RTEMS_ARRAY_SIZE( objects_info_table ); ++i ) {
    const Objects_Information *information;

    information = _Objects_Get_information(
      objects_info_table[ i ].api,
      objects_info_table[ i ].cls
    );

    if ( information != NULL ) {
      uint32_t *active;

      active = (uint32_t *) (
        (char *) snapshot + objects_info_table[ i ].offset
      );
      *active = _Objects_Active_count( information );
    }
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
  return memcmp( a, b, sizeof( *a ) ) == 0;
}

bool rtems_resource_snapshot_check( const rtems_resource_snapshot *snapshot )
{
  rtems_resource_snapshot now;

  rtems_resource_snapshot_take( &now );

  return rtems_resource_snapshot_equal( &now, snapshot );
}
