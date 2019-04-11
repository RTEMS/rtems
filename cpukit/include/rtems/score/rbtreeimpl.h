/**
 * @file
 *
 * @ingroup RTEMSScoreRBTree
 *
 * @brief Inlined Routines Associated with Red-Black Trees
 *
 * This include file contains the bodies of the routines which are
 * associated with Red-Black Trees and inlined.
 *
 * @note  The routines in this file are ordered from simple
 *        to complex.  No other RBTree Handler routine is referenced
 *        unless it has already been defined.
 */

/*
 *  Copyright (c) 2010-2012 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_RBTREEIMPL_H
#define _RTEMS_SCORE_RBTREEIMPL_H

#include <rtems/score/rbtree.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreRBTree
 *
 * @{
 */

/**
 * @brief Red-black tree visitor.
 *
 * @param[in] node The node.
 * @param[in] visitor_arg The visitor argument.
 *
 * @retval true Stop the iteration.
 * @retval false Continue the iteration.
 *
 * @see _RBTree_Iterate().
 */
typedef bool (*RBTree_Visitor)(
  const RBTree_Node *node,
  void *visitor_arg
);

/**
 * @brief Red-black tree iteration.
 *
 * @param rbtree The red-black tree.
 * @param visitor The visitor.
 * @param visitor_arg The visitor argument.
 */
void _RBTree_Iterate(
  const RBTree_Control *rbtree,
  RBTree_Visitor visitor,
  void *visitor_arg
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
