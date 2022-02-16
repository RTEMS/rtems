/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicObject
 *
 * @brief This source file contains the implementation of
 *   rtems_object_set_name().
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
#include <rtems/rtems/statusimpl.h>
#include <rtems/rtems/tasks.h>
#include <rtems/score/objectimpl.h>

/*
 *  This method will set the object name based upon the user string.
 *  If the object class uses 32-bit names, then only the first 4 bytes
 *  of the string will be used.
 */
rtems_status_code rtems_object_set_name(
  rtems_id       id,
  const char    *name
)
{
  Objects_Information *information;
  Objects_Control     *the_object;
  Objects_Id           tmpId;
  Status_Control       status;

  if ( !name )
    return RTEMS_INVALID_ADDRESS;

  tmpId = (id == OBJECTS_ID_OF_SELF) ? rtems_task_self() : id;

  information  = _Objects_Get_information_id( tmpId );
  if ( !information )
    return RTEMS_INVALID_ID;

  _Objects_Allocator_lock();
  the_object = _Objects_Get_no_protection( tmpId, information );

  if ( the_object == NULL ) {
    _Objects_Allocator_unlock();
    return RTEMS_INVALID_ID;
  }

  status = _Objects_Set_name( information, the_object, name );
  _Objects_Allocator_unlock();
  return _Status_Get( status );
}
