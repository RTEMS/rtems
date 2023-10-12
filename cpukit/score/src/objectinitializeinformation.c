/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreObject
 *
 * @brief This source file contains the implementation of
 *   _Objects_Initialize_information().
 */

/*
 *  COPYRIGHT (c) 1989-2011.
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
#include <rtems/score/address.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/sysstate.h>

void _Objects_Initialize_information(
  Objects_Information *information
)
{
  Objects_Id       maximum_id;
  Objects_Id       api_class_and_node;
  Objects_Maximum  maximum;
  Objects_Maximum  index;
  Chain_Node      *head;
  Chain_Node      *tail;
  Chain_Node      *current;
  Objects_Control *next;

  maximum_id = information->maximum_id;

#if defined(RTEMS_MULTIPROCESSING)
  maximum_id &= ~OBJECTS_NODE_MASK;
  maximum_id |= _Objects_Local_node << OBJECTS_NODE_START_BIT;
  information->maximum_id = maximum_id;
#endif

  maximum = _Objects_Get_index( maximum_id );
  api_class_and_node = maximum_id & ~OBJECTS_INDEX_MASK;

  /*
   *  Register this Object Class in the Object Information Table.
   */
  _Objects_Information_table[ _Objects_Get_API( maximum_id ) ]
    [ _Objects_Get_class( maximum_id ) ] = information;

  head = _Chain_Head( &information->Inactive );
  tail = _Chain_Tail( &information->Inactive );
  current = head;
  next = information->initial_objects;

  head->previous = NULL;

  for ( index = OBJECTS_INDEX_MINIMUM; index <= maximum ; ++index ) {
    current->next = &next->Node;
    next->Node.previous = current;
    current = &next->Node;
    next->id = api_class_and_node | ( index << OBJECTS_INDEX_START_BIT );
    next = _Addresses_Add_offset( next, information->object_size );
  }

  current->next = tail;
  tail->previous = current;
}
