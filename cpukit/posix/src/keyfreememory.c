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
  POSIX_Keys_List_node *i;

  /** delete node both from rbtree and list.
   *  can't use i = i->Next in the for loop, because i
   *  is deallocated in the for body.
   */
  for ( i = the_key->Head ; i != NULL; i = the_key->Head )
    {
      /** problem: _RBTree_Extract() has no return, then can't check
       *  wheck the deletion is successful.
       */
      _RBTree_Extract(&_POSIX_Keys_Rbtree, i->Rbnode->Node);
      _Workspace_Free(i->Rbnode);

      /** delete this node from node list */
      the_key->Head = i->Next;
      _Workspace_Free(i);
    }
}
