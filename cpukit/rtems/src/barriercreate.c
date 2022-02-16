/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicBarrier
 *
 * @brief This source file contains the implementation of
 *   rtems_barrier_create() and the Barrier Manager system initialization.
 */

/*
 *  COPYRIGHT (c) 1989-2006.
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

#include <rtems/rtems/barrierimpl.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/attrimpl.h>
#include <rtems/score/isr.h>
#include <rtems/sysinit.h>

rtems_status_code rtems_barrier_create(
  rtems_name           name,
  rtems_attribute      attribute_set,
  uint32_t             maximum_waiters,
  rtems_id            *id
)
{
  Barrier_Control *the_barrier;
  uint32_t         maximum_count;

  if ( !rtems_is_name_valid( name ) ) {
    return RTEMS_INVALID_NAME;
  }

  if ( id == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  if ( _Attributes_Is_barrier_automatic( attribute_set ) ) {
    if ( maximum_waiters == 0 ) {
      return RTEMS_INVALID_NUMBER;
    }

    maximum_count = maximum_waiters;
  } else {
    maximum_count = CORE_BARRIER_MANUAL_RELEASE_MAXIMUM_COUNT;
  }

  the_barrier = _Barrier_Allocate();

  if ( the_barrier == NULL ) {
    _Objects_Allocator_unlock();
    return RTEMS_TOO_MANY;
  }

  _CORE_barrier_Initialize( &the_barrier->Barrier, maximum_count );

  *id = _Objects_Open_u32( &_Barrier_Information, &the_barrier->Object, name );
  _Objects_Allocator_unlock();
  return RTEMS_SUCCESSFUL;
}

static void _Barrier_Manager_initialization( void )
{
  _Objects_Initialize_information( &_Barrier_Information );
}

RTEMS_SYSINIT_ITEM(
  _Barrier_Manager_initialization,
  RTEMS_SYSINIT_CLASSIC_BARRIER,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
