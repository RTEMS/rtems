/**
 * @file
 *
 * @ingroup RTEMSScoreRBTree
 *
 * @brief _RBTree_Next() and _RBTree_Next() implementation.
 */

/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/rbtreeimpl.h>
#include <rtems/score/basedefs.h>

RB_GENERATE_MINMAX( RBTree_Control, RBTree_Node, Node, static )

RB_GENERATE_NEXT( RBTree_Control, RBTree_Node, Node, static )

RB_GENERATE_PREV( RBTree_Control, RBTree_Node, Node, static )

RBTree_Node *_RBTree_Minimum( const RBTree_Control *tree )
{
  return RB_MIN( RBTree_Control, RTEMS_DECONST( RBTree_Control *, tree ) );
}

RBTree_Node *_RBTree_Maximum( const RBTree_Control *tree )
{
  return RB_MAX( RBTree_Control, RTEMS_DECONST( RBTree_Control *, tree ) );
}

RBTree_Node *_RBTree_Successor( const RBTree_Node *node )
{
  return RB_NEXT( RBTree_Control, NULL, RTEMS_DECONST( RBTree_Node *, node ) );
}

RBTree_Node *_RBTree_Predecessor( const RBTree_Node *node )
{
  return RB_PREV( RBTree_Control, NULL, RTEMS_DECONST( RBTree_Node *, node ) );
}
