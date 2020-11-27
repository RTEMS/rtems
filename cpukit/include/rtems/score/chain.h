/**
 * @file
 *
 * @ingroup RTEMSScoreChain
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreChain which are used by the implementation and the API.
 */

/*
 *  Copyright (c) 2010 embedded brains GmbH.
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CHAIN_H
#define _RTEMS_SCORE_CHAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScoreChain Chain Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the Chain Handler implementation.
 *
 * The Chain Handler is used to manage sets of entities.  This handler
 * provides two data structures.  The Chain Node data structure is included
 * as the first part of every data structure that will be placed on
 * a chain.  The second data structure is Chain Control which is used
 * to manage a set of Chain Nodes.
 *
 * @{
 */

/**
 *  @typedef Chain_Node
 *
 *  This type definition promotes the name for the Chain Node used by
 *  all RTEMS code.  It is a separate type definition because a forward
 *  reference is required to define it.  See @ref Chain_Node_struct for
 *  detailed information.
 */
typedef struct Chain_Node_struct Chain_Node;

/**
 *  @struct Chain_Node_struct
 *
 *  This is used to manage each element (node) which is placed
 *  on a chain.
 *
 *  @note Typically, a more complicated structure will use the
 *        chain package.  The more complicated structure will
 *        include a chain node as the first element in its
 *        control structure.  It will then call the chain package
 *        with a pointer to that node element.  The node pointer
 *        and the higher level structure start at the same address
 *        so the user can cast the pointers back and forth.
 *
 */
struct Chain_Node_struct {
  /** This points to the node after this one on this chain. */
  Chain_Node *next;
  /** This points to the node immediate prior to this one on this chain. */
  Chain_Node *previous;
};

/**
 *  @struct Chain_Control
 *
 * This is used to manage a chain.  A chain consists of a doubly
 * linked list of zero or more nodes.
 *
 * @note This implementation does not require special checks for
 *   manipulating the first and last elements on the chain.
 *   To accomplish this the @a Chain_Control structure is
 *   treated as two overlapping @ref Chain_Node structures.
 */
typedef union {
  struct {
    Chain_Node Node;
    Chain_Node *fill;
  } Head;

  struct {
    Chain_Node *fill;
    Chain_Node Node;
  } Tail;
} Chain_Control;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
