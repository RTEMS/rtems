/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreObject
 *
 * @brief This source file contains the implementation of
 *   _Objects_Get_information().
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

#include <rtems/score/objectimpl.h>

Objects_Information *_Objects_Get_information(
  Objects_APIs   the_api,
  uint16_t       the_class
)
{
  Objects_Information *info;
  int the_class_api_maximum;

  if ( !the_class )
    return NULL;

  /*
   *  This call implicitly validates the_api so we do not call
   *  _Objects_Is_api_valid above here.
   */
  the_class_api_maximum = _Objects_API_maximum_class( the_api );
  if ( the_class_api_maximum == 0 )
    return NULL;

  if ( the_class > (uint32_t) the_class_api_maximum )
    return NULL;

  if ( !_Objects_Information_table[ the_api ] )
    return NULL;

  info = _Objects_Information_table[ the_api ][ the_class ];
  if ( !info )
    return NULL;

  /*
   * In a multiprocessing configuration, we may access remote objects.
   * Thus we may have 0 local instances and still have a valid object
   * pointer.
   */
#if !defined(RTEMS_MULTIPROCESSING)
  if ( _Objects_Get_maximum_index( info ) == 0 ) {
    return NULL;
  }
#endif

  return info;
}

