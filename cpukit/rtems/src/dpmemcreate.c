/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicDPMem
 *
 * @brief This source file contains the implementation of
 *   rtems_port_create() and the Dual-Ported Memory Manager system
 *   initialization.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
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

#include <rtems/rtems/dpmemimpl.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/address.h>
#include <rtems/score/thread.h>
#include <rtems/sysinit.h>

rtems_status_code rtems_port_create(
  rtems_name    name,
  void         *internal_start,
  void         *external_start,
  uint32_t      length,
  rtems_id     *id
)
{
  Dual_ported_memory_Control *the_port;

  if ( !rtems_is_name_valid( name ) )
    return RTEMS_INVALID_NAME;

  if ( !id )
    return RTEMS_INVALID_ADDRESS;

  if ( !_Addresses_Is_aligned( internal_start ) ||
       !_Addresses_Is_aligned( external_start ) )
    return RTEMS_INVALID_ADDRESS;

  the_port = _Dual_ported_memory_Allocate();

  if ( !the_port ) {
    _Objects_Allocator_unlock();
    return RTEMS_TOO_MANY;
  }

  the_port->internal_base = internal_start;
  the_port->external_base = external_start;
  the_port->length        = length - 1;

  *id = _Objects_Open_u32(
    &_Dual_ported_memory_Information,
    &the_port->Object,
    name
  );
  _Objects_Allocator_unlock();
  return RTEMS_SUCCESSFUL;
}

static void _Dual_ported_memory_Manager_initialization( void )
{
  _Objects_Initialize_information( &_Dual_ported_memory_Information );
}

RTEMS_SYSINIT_ITEM(
  _Dual_ported_memory_Manager_initialization,
  RTEMS_SYSINIT_CLASSIC_DUAL_PORTED_MEMORY,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
