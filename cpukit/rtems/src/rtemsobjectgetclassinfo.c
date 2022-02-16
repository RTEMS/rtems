/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicObject
 *
 * @brief This source file contains the implementation of
 *   rtems_object_get_class_information().
 */

/*
 *  COPYRIGHT (c) 1989-2008.
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

#include <rtems/rtems/object.h>
#include <rtems/score/objectimpl.h>

rtems_status_code rtems_object_get_class_information(
  int                                 the_api,
  int                                 the_class,
  rtems_object_api_class_information *info
)
{
  Objects_Information *obj_info;
  int                  unallocated;
  int                  i;

  /*
   * Validate parameters and look up information structure.
   */
  if ( !info )
    return RTEMS_INVALID_ADDRESS;

  obj_info = _Objects_Get_information( the_api, the_class );
  if ( !obj_info )
    return RTEMS_INVALID_NUMBER;

  /*
   * Return information about this object class to the user.
   */
  info->minimum_id  = _Objects_Get_minimum_id( obj_info->maximum_id );
  info->maximum_id  = obj_info->maximum_id;
  info->auto_extend = _Objects_Is_auto_extend( obj_info );
  info->maximum     = _Objects_Get_maximum_index( obj_info );

  for ( unallocated=0, i=1 ; i <= info->maximum ; i++ )
    if ( !obj_info->local_table[i] )
      unallocated++;

  info->unallocated = unallocated;

  return RTEMS_SUCCESSFUL;
}

