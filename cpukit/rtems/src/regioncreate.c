/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicRegion
 *
 * @brief This source file contains the implementation of
 *   rtems_region_create() and the Region Manager system initialization.
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
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

#include <rtems/rtems/regionimpl.h>
#include <rtems/rtems/attrimpl.h>
#include <rtems/rtems/support.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/sysinit.h>

rtems_status_code rtems_region_create(
  rtems_name          name,
  void               *starting_address,
  uintptr_t           length,
  uintptr_t           page_size,
  rtems_attribute     attribute_set,
  rtems_id           *id
)
{
  rtems_status_code  return_status;
  Region_Control    *the_region;

  if ( !rtems_is_name_valid( name ) ) {
    return RTEMS_INVALID_NAME;
  }

  if ( id == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  if ( starting_address == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  the_region = _Region_Allocate();

    if ( !the_region )
      return_status = RTEMS_TOO_MANY;

    else {
      _Thread_queue_Object_initialize( &the_region->Wait_queue );

      if ( _Attributes_Is_priority( attribute_set ) ) {
        the_region->wait_operations = &_Thread_queue_Operations_priority;
      } else {
        the_region->wait_operations = &_Thread_queue_Operations_FIFO;
      }

      the_region->maximum_segment_size = _Heap_Initialize(
        &the_region->Memory, starting_address, length, page_size
      );

      if ( !the_region->maximum_segment_size ) {
        _Region_Free( the_region );
        return_status = RTEMS_INVALID_SIZE;
      } else {
        the_region->attribute_set = attribute_set;

        *id = _Objects_Open_u32(
          &_Region_Information,
          &the_region->Object,
          name
        );

        return_status = RTEMS_SUCCESSFUL;
      }
    }

  _Objects_Allocator_unlock();

  return return_status;
}

static void _Region_Manager_initialization( void )
{
  _Objects_Initialize_information( &_Region_Information );
}

RTEMS_SYSINIT_ITEM(
  _Region_Manager_initialization,
  RTEMS_SYSINIT_CLASSIC_REGION,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
