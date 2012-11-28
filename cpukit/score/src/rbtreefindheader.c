/**
 * @file
 *
 * @brief Finds the Header of a Tree if it Exists
 *
 * @ingroup ScoreRBTree
 */

/*
 *  Copyright (c) 2010 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/address.h>
#include <rtems/score/rbtree.h>
#include <rtems/score/isr.h>

RBTree_Control *_RBTree_Find_header(
  RBTree_Node *the_node
)
{
  ISR_Level          level;
  RBTree_Control *return_header;

  return_header = NULL;
  _ISR_Disable( level );
      return_header = _RBTree_Find_header_unprotected( the_node );
  _ISR_Enable( level );
  return return_header;
}
