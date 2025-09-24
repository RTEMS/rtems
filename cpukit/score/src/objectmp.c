/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreObjectMP
 *
 * @brief This source file contains the definition of
 *   ::_Objects_Local_node, ::_Objects_Maximum_nodes,
 *   ::_Objects_MP_Maximum_global_objects and the implementation of
 *   _Objects_MP_Handler_early_initialization(),
 *   _Objects_MP_Handler_initialization(), _Objects_MP_Open(),
 *   _Objects_MP_Allocate_and_open(), _Objects_MP_Close(),
 *   _Objects_MP_Global_name_search(), _Objects_MP_Is_remote(),
 *   _Objects_MP_Allocate_global_object(), and
 *   _Objects_MP_Free_global_object().
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

#include <rtems/config.h>
#include <rtems/score/interr.h>
#include <rtems/score/isrlock.h>
#include <rtems/score/objectimpl.h>

#define OBJECTS_MP_CONTROL_OF_ID_LOOKUP_NODE( node ) \
  RTEMS_CONTAINER_OF( node, Objects_MP_Control, Nodes.Active.Id_lookup )

#define OBJECTS_MP_CONTROL_OF_NAME_LOOKUP_NODE( node ) \
  RTEMS_CONTAINER_OF( node, Objects_MP_Control, Nodes.Active.Name_lookup )

typedef struct {
  uint32_t name;
  uint32_t node;
} Objects_MP_Name_and_node;

uint16_t _Objects_Local_node;

uint16_t _Objects_Maximum_nodes;

uint32_t _Objects_MP_Maximum_global_objects;

static CHAIN_DEFINE_EMPTY( _Objects_MP_Inactive_global_objects );

#if ISR_LOCK_NEEDS_OBJECT
static ISR_lock_Control _Objects_MP_Global_lock = ISR_LOCK_INITIALIZER(
  "MP Objects"
);
#endif

static void _Objects_MP_Global_acquire( ISR_lock_Context *lock_context )
{
  _ISR_lock_ISR_disable_and_acquire( &_Objects_MP_Global_lock, lock_context );
}

static void _Objects_MP_Global_release( ISR_lock_Context *lock_context )
{
  _ISR_lock_Release_and_ISR_enable( &_Objects_MP_Global_lock, lock_context );
}

static bool _Objects_MP_Id_equal( const void *left, const RBTree_Node *right )
{
  const Objects_Id         *the_left;
  const Objects_MP_Control *the_right;

  the_left = left;
  the_right = OBJECTS_MP_CONTROL_OF_ID_LOOKUP_NODE( right );

  return *the_left == the_right->id;
}

static bool _Objects_MP_Id_less( const void *left, const RBTree_Node *right )
{
  const Objects_Id         *the_left;
  const Objects_MP_Control *the_right;

  the_left = left;
  the_right = OBJECTS_MP_CONTROL_OF_ID_LOOKUP_NODE( right );

  return *the_left < the_right->id;
}

static void *_Objects_MP_Id_map( RBTree_Node *node )
{
  return OBJECTS_MP_CONTROL_OF_ID_LOOKUP_NODE( node );
}

static bool _Objects_MP_Name_equal( const void *left, const RBTree_Node *right )
{
  const uint32_t           *the_left;
  const Objects_MP_Control *the_right;

  the_left = left;
  the_right = OBJECTS_MP_CONTROL_OF_NAME_LOOKUP_NODE( right );

  return *the_left == the_right->name;
}

static bool _Objects_MP_Name_less( const void *left, const RBTree_Node *right )
{
  const uint32_t           *the_left;
  const Objects_MP_Control *the_right;

  the_left = left;
  the_right = OBJECTS_MP_CONTROL_OF_NAME_LOOKUP_NODE( right );

  return *the_left < the_right->name;
}

static void *_Objects_MP_Name_map( RBTree_Node *node )
{
  return OBJECTS_MP_CONTROL_OF_NAME_LOOKUP_NODE( node );
}

static bool _Objects_MP_Name_and_node_equal(
  const void        *left,
  const RBTree_Node *right
)
{
  const Objects_MP_Name_and_node *the_left;
  const Objects_MP_Control       *the_right;

  the_left = left;
  the_right = OBJECTS_MP_CONTROL_OF_NAME_LOOKUP_NODE( right );

  return the_left->name == the_right->name &&
         the_left->node == _Objects_Get_node( the_right->id );
}

static bool _Objects_MP_Name_and_node_less(
  const void        *left,
  const RBTree_Node *right
)
{
  const Objects_MP_Name_and_node *the_left;
  const Objects_MP_Control       *the_right;

  the_left = left;
  the_right = OBJECTS_MP_CONTROL_OF_NAME_LOOKUP_NODE( right );

  /*
   * Use > for the node to find smaller numbered nodes first in case of equal
   * names.
   */
  return the_left->name < the_right->name ||
         ( the_left->name == the_right->name &&
           the_left->node > _Objects_Get_node( the_right->id ) );
}

void _Objects_MP_Handler_early_initialization( void )
{
  uint32_t node;
  uint32_t maximum_nodes;

  node = _MPCI_Configuration.node;
  maximum_nodes = _MPCI_Configuration.maximum_nodes;

  if ( node < 1 || node > maximum_nodes ) {
    _Internal_error( INTERNAL_ERROR_INVALID_NODE );
  }

  _Objects_Local_node = node;
  _Objects_Maximum_nodes = maximum_nodes;
}

void _Objects_MP_Handler_initialization( void )
{
  uint32_t maximum_global_objects;

  maximum_global_objects = _MPCI_Configuration.maximum_global_objects;

  _Objects_MP_Maximum_global_objects = maximum_global_objects;

  if ( maximum_global_objects == 0 ) {
    return;
  }

  _Chain_Initialize(
    &_Objects_MP_Inactive_global_objects,
    &_Objects_MP_Controls[ 0 ],
    maximum_global_objects,
    sizeof( _Objects_MP_Controls[ 0 ] )
  );
}

void _Objects_MP_Open(
  Objects_Information *information,
  Objects_MP_Control  *the_global_object,
  uint32_t             the_name, /* XXX -- wrong for variable */
  Objects_Id           the_id
)
{
  Objects_MP_Name_and_node name_and_node;
  ISR_lock_Context         lock_context;

  the_global_object->id = the_id;
  the_global_object->name = the_name;

  name_and_node.name = the_name;
  name_and_node.node = _Objects_Get_node( the_id );

  _Objects_MP_Global_acquire( &lock_context );

  _RBTree_Insert_inline(
    &information->Global_by_id,
    &the_global_object->Nodes.Active.Id_lookup,
    &the_id,
    _Objects_MP_Id_less
  );
  _RBTree_Insert_inline(
    &information->Global_by_name,
    &the_global_object->Nodes.Active.Name_lookup,
    &name_and_node,
    _Objects_MP_Name_and_node_less
  );

  _Objects_MP_Global_release( &lock_context );
}

bool _Objects_MP_Allocate_and_open(
  Objects_Information *information,
  uint32_t             the_name, /* XXX -- wrong for variable */
  Objects_Id           the_id,
  bool                 is_fatal_error
)
{
  Objects_MP_Control *the_global_object;

  the_global_object = _Objects_MP_Allocate_global_object();

  if ( the_global_object == NULL ) {
    if ( !is_fatal_error ) {
      return false;
    }

    _Internal_error( INTERNAL_ERROR_OUT_OF_GLOBAL_OBJECTS );
  }

  _Objects_MP_Open( information, the_global_object, the_name, the_id );

  return true;
}

void _Objects_MP_Close( Objects_Information *information, Objects_Id the_id )
{
  Objects_MP_Control *the_global_object;
  ISR_lock_Context    lock_context;

  _Objects_MP_Global_acquire( &lock_context );

  the_global_object = _RBTree_Find_inline(
    &information->Global_by_id,
    &the_id,
    _Objects_MP_Id_equal,
    _Objects_MP_Id_less,
    _Objects_MP_Id_map
  );

  if ( the_global_object != NULL ) {
    _RBTree_Extract(
      &information->Global_by_id,
      &the_global_object->Nodes.Active.Id_lookup
    );
    _RBTree_Extract(
      &information->Global_by_name,
      &the_global_object->Nodes.Active.Name_lookup
    );
    _Objects_MP_Free_global_object( the_global_object );
    _Objects_MP_Global_release( &lock_context );
  } else {
    _Objects_MP_Global_release( &lock_context );

    _Internal_error( INTERNAL_ERROR_INVALID_GLOBAL_ID );
  }
}

Status_Control _Objects_MP_Global_name_search(
  const Objects_Information *information,
  Objects_Name               the_name,
  uint32_t                   nodes_to_search,
  Objects_Id                *the_id
)
{
  Status_Control      status;
  Objects_MP_Control *the_global_object;
  ISR_lock_Context    lock_context;

  if ( nodes_to_search > _Objects_Maximum_nodes ) {
    return STATUS_INVALID_NODE;
  }

  _Objects_MP_Global_acquire( &lock_context );

  if ( nodes_to_search == OBJECTS_SEARCH_ALL_NODES ) {
    the_global_object = _RBTree_Find_inline(
      &information->Global_by_name,
      &the_name.name_u32,
      _Objects_MP_Name_equal,
      _Objects_MP_Name_less,
      _Objects_MP_Name_map
    );
  } else {
    Objects_MP_Name_and_node name_and_node;

    name_and_node.name = the_name.name_u32;
    name_and_node.node = nodes_to_search;

    the_global_object = _RBTree_Find_inline(
      &information->Global_by_name,
      &name_and_node,
      _Objects_MP_Name_and_node_equal,
      _Objects_MP_Name_and_node_less,
      _Objects_MP_Name_map
    );
  }

  if ( the_global_object != NULL ) {
    *the_id = the_global_object->id;
    _Assert( the_global_object->name != 0 );
    status = STATUS_SUCCESSFUL;
  } else {
    status = STATUS_INVALID_NAME;
  }

  _Objects_MP_Global_release( &lock_context );

  return status;
}

bool _Objects_MP_Is_remote(
  Objects_Id                 the_id,
  const Objects_Information *information
)
{
  Objects_MP_Control *the_global_object;
  ISR_lock_Context    lock_context;

  _Objects_MP_Global_acquire( &lock_context );

  the_global_object = _RBTree_Find_inline(
    &information->Global_by_id,
    &the_id,
    _Objects_MP_Id_equal,
    _Objects_MP_Id_less,
    _Objects_MP_Id_map
  );

  _Objects_MP_Global_release( &lock_context );

  return the_global_object != NULL;
}

Objects_MP_Control *_Objects_MP_Allocate_global_object( void )
{
  Objects_MP_Control *the_global_object;
  ISR_lock_Context    lock_context;

  _Objects_MP_Global_acquire( &lock_context );

  the_global_object = (Objects_MP_Control *) _Chain_Get_unprotected(
    &_Objects_MP_Inactive_global_objects
  );

  _Objects_MP_Global_release( &lock_context );
  return the_global_object;
}

void _Objects_MP_Free_global_object( Objects_MP_Control *the_global_object )
{
  ISR_lock_Context lock_context;

  _Objects_MP_Global_acquire( &lock_context );

  _Chain_Initialize_node( &the_global_object->Nodes.Inactive );
  _Chain_Append_unprotected(
    &_Objects_MP_Inactive_global_objects,
    &the_global_object->Nodes.Inactive
  );

  _Objects_MP_Global_release( &lock_context );
}
