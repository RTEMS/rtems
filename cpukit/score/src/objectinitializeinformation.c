/**
 * @file
 *
 * @ingroup RTEMSScoreObject
 *
 * @brief Initialize Object Information
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/objectimpl.h>
#include <rtems/score/address.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/interr.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/wkspace.h>

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
