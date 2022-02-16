/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicUserExt
 *
 * @brief This source file contains the implementation of
 *   rtems_extension_create() and the User Extensions Manager system
 *   initialization.
 */

/*
 *  COPYRIGHT (c) 1989-2007.
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

#include <rtems/extensionimpl.h>
#include <rtems/rtems/support.h>
#include <rtems/score/userextimpl.h>
#include <rtems/sysinit.h>

rtems_status_code rtems_extension_create(
  rtems_name                    name,
  const rtems_extensions_table *extension_table,
  rtems_id                     *id
)
{
  Extension_Control *the_extension;

  if ( !rtems_is_name_valid( name ) ) {
    return RTEMS_INVALID_NAME;
  }

  if ( extension_table == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  if ( id == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  the_extension = _Extension_Allocate();

  if ( the_extension == NULL ) {
    _Objects_Allocator_unlock();
    return RTEMS_TOO_MANY;
  }

  _User_extensions_Add_set_with_table( &the_extension->Extension, extension_table );

  *id = _Objects_Open_u32(
    &_Extension_Information,
    &the_extension->Object,
    name
  );
  _Objects_Allocator_unlock();
  return RTEMS_SUCCESSFUL;
}

static void _Extension_Manager_initialization( void )
{
  _Objects_Initialize_information( &_Extension_Information);
}

RTEMS_SYSINIT_ITEM(
  _Extension_Manager_initialization,
  RTEMS_SYSINIT_USER_EXTENSIONS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
