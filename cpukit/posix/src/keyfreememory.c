/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/posix/key.h>

void _POSIX_Keys_Free_memory(
  POSIX_Keys_Control *the_key
)
{
  POSIX_Keys_List_node *p, *q;

  /** delete node both from rbtree and list.
   */
  for ( p = the_key->head ; p != NULL; p = q )
    {
      /** problem: _RBTree_Extract() has no return, then can't check
       *  wheck the deletion is successful.
       */
      _RBTree_Extract( &_POSIX_Keys_Rbtree, &(p->rbnode->node) );
      _Workspace_Free( p->rbnode );

      /** delete this node from node list */
      q = p->next;
      _Workspace_Free( p );
    }
}
