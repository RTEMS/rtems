/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreObject
 *
 * @brief This source file contains the implementation of
 *   _Objects_Name_to_id_u32().
 */

/*
 *  COPYRIGHT (c) 1989-2010.
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

static bool _Objects_Is_local_node_search( uint32_t node )
{
  return node == OBJECTS_SEARCH_LOCAL_NODE || _Objects_Is_local_node( node );
}

Status_Control _Objects_Name_to_id_u32(
  uint32_t                   name,
  uint32_t                   node,
  Objects_Id                *id,
  const Objects_Information *information
)
{
#if defined(RTEMS_MULTIPROCESSING)
  Objects_Name name_for_mp;
#endif

  _Assert( !_Objects_Has_string_name( information ) );

  if ( id == NULL ) {
    return STATUS_INVALID_ADDRESS;
  }

  if (
    node == OBJECTS_SEARCH_ALL_NODES ||
    _Objects_Is_local_node_search( node )
  ) {
    Objects_Maximum maximum;
    Objects_Maximum index;

    maximum = _Objects_Get_maximum_index( information );

    for ( index = 0; index < maximum; ++index ) {
      const Objects_Control *the_object;

      the_object = information->local_table[ index ];

      if ( the_object != NULL && name == the_object->name.name_u32 ) {
        *id = the_object->id;
        _Assert( name != 0 );
        return STATUS_SUCCESSFUL;
      }
    }
  }

#if defined(RTEMS_MULTIPROCESSING)
  if ( _Objects_Is_local_node_search( node ) ) {
    return STATUS_INVALID_NAME;
  }

  name_for_mp.name_u32 = name;
  return _Objects_MP_Global_name_search( information, name_for_mp, node, id );
#else
  return STATUS_INVALID_NAME;
#endif
}
