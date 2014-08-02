/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/score/resourceimpl.h>

static Resource_Control *_Resource_Rival_head_to_resource( Chain_Node *head )
{
  return RTEMS_CONTAINER_OF( head, Resource_Control, Rivals.Head.Node );
}

static Resource_Node *_Resource_Resource_tail_to_rival( Chain_Node *tail )
{
  return RTEMS_CONTAINER_OF( tail, Resource_Node, Resources.Tail.Node );
}

void _Resource_Iterate(
  Resource_Node         *top,
  Resource_Node_visitor  visitor,
  void                  *arg
)
{
  Chain_Node *resource_tail = _Chain_Tail( &top->Resources );
  Resource_Control *resource =
    (Resource_Control *) _Chain_Head( &top->Resources );

  Chain_Node *rival_stop = NULL;
  Resource_Node *rival = NULL;

  enum {
    NODE_FORWARD,
    NODE_BACKWARD,
    RESOURCE_FORWARD
  } dir = RESOURCE_FORWARD;

  bool stop = false;

  do {
    switch ( dir ) {
      case NODE_FORWARD:
        while ( !stop && &rival->Node != rival_stop ) {
          Resource_Node *current = rival;

          rival = (Resource_Node *) _Chain_Next( &rival->Node );
          stop = ( *visitor )( current, arg );
        }

        rival_stop = _Chain_Head( &resource->Rivals );
        dir = NODE_BACKWARD;
        break;
      case NODE_BACKWARD:
        rival = (Resource_Node *) _Chain_Previous( &rival->Node );

        while (
          &rival->Node != rival_stop
            && _Chain_Is_empty( &rival->Resources )
        ) {
          rival = (Resource_Node *) _Chain_Previous( &rival->Node );
        }

        if ( &rival->Node != rival_stop ) {
          resource_tail = _Chain_Tail( &rival->Resources );
          resource = (Resource_Control *) _Chain_Head( &rival->Resources );
        } else {
          resource = _Resource_Rival_head_to_resource( rival_stop );
          resource_tail = _Chain_Tail( &resource->owner->Resources );
        }

        dir = RESOURCE_FORWARD;
        break;
      case RESOURCE_FORWARD:
        resource = (Resource_Control *) _Chain_Next( &resource->Node );

        while (
          &resource->Node != resource_tail
            && _Chain_Is_empty( &resource->Rivals )
        ) {
          resource = (Resource_Control *) _Chain_Next( &resource->Node );
        }

        if ( &resource->Node != resource_tail ) {
          rival_stop = _Chain_Tail( &resource->Rivals );
          rival = (Resource_Node *) _Chain_First( &resource->Rivals );
          dir = NODE_FORWARD;
        } else {
          rival = _Resource_Resource_tail_to_rival( resource_tail );
          rival_stop = _Chain_Head( &rival->dependency->Rivals );
          dir = NODE_BACKWARD;
        }

        break;
    }
  } while ( !stop && rival != top );
}
