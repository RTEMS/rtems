/*  chain.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Doubly Linked Chain Handler.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __RTEMS_CHAIN_h
#define __RTEMS_CHAIN_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/address.h>

/*
 *  This is used to manage each element (node) which is placed
 *  on a chain.
 *
 *  NOTE:  Typically, a more complicated structure will use the
 *         chain package.  The more complicated structure will
 *         include a chain node as the first element in its
 *         control structure.  It will then call the chain package
 *         with a pointer to that node element.  The node pointer
 *         and the higher level structure start at the same address
 *         so the user can cast the pointers back and forth.
 *
 */

typedef struct Chain_Node_struct Chain_Node;

struct Chain_Node_struct {
  Chain_Node *next;
  Chain_Node *previous;
};

/*
 *  This is used to manage a chain.  A chain consists of a doubly
 *  linked list of zero or more nodes.
 *
 *  NOTE:  This implementation does not require special checks for
 *         manipulating the first and last elements on the chain.
 *         To accomplish this the chain control structure is
 *         treated as two overlapping chain nodes.  The permanent
 *         head of the chain overlays a node structure on the
 *         first and permanent_null fields.  The permanent tail
 *         of the chain overlays a node structure on the
 *         permanent_null and last elements of the structure.
 *
 */

typedef struct {
  Chain_Node *first;
  Chain_Node *permanent_null;
  Chain_Node *last;
} Chain_Control;

/*
 *  _Chain_Initialize
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the_chain structure to manage the
 *  contiguous array of number_nodes nodes which starts at
 *  starting_address.  Each node is of node_size bytes.
 *
 */

void _Chain_Initialize(
  Chain_Control *the_chain,
  void          *starting_address,
  unsigned32     number_nodes,
  unsigned32     node_size
);

/*
 *  _Chain_Initialize_empty
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the specified chain to contain zero nodes.
 *
 */

STATIC INLINE void _Chain_Initialize_empty(
  Chain_Control *the_chain
);

/*
 *  _Chain_Are_nodes_equal
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if LEFT and RIGHT are equal,
 *  and FALSE otherwise.
 *
 */

STATIC INLINE boolean _Chain_Are_nodes_equal(
  Chain_Node *left,
  Chain_Node *right
);

/*
 *  _Chain_Extract_unprotected
 *
 *  DESCRIPTION:
 *
 *  This routine extracts the_node from the chain on which it resides.
 *  It does NOT disable interrupts to insure the atomicity of the
 *  extract operation.
 *
 */

STATIC INLINE void _Chain_Extract_unprotected(
  Chain_Node *the_node
);

/*
 *  _Chain_Extract
 *
 *  DESCRIPTION:
 *
 *  This routine extracts the_node from the chain on which it resides.
 *  It disables interrupts to insure the atomicity of the
 *  extract operation.
 *
 */

void _Chain_Extract(
  Chain_Node *the_node
);

/*
 *  _Chain_Get_unprotected
 *
 *  DESCRIPTION:
 *
 *  This function removes the first node from the_chain and returns
 *  a pointer to that node.  If the_chain is empty, then NULL is returned.
 *  It does NOT disable interrupts to insure the atomicity of the
 *  get operation.
 *
 */

STATIC INLINE Chain_Node *_Chain_Get_unprotected(
  Chain_Control *the_chain
);

/*
 *  _Chain_Get
 *
 *  DESCRIPTION:
 *
 *  This function removes the first node from the_chain and returns
 *  a pointer to that node.  If the_chain is empty, then NULL is returned.
 *  It disables interrupts to insure the atomicity of the
 *  get operation.
 *
 */

Chain_Node *_Chain_Get(
  Chain_Control *the_chain
);

/*
 *  _Chain_Get_first_unprotected
 *
 *  DESCRIPTION:
 *
 *  This function removes the first node from the_chain and returns
 *  a pointer to that node.  It does NOT disable interrupts to insure
 *  the atomicity of the get operation.
 *
 */

STATIC INLINE Chain_Node *_Chain_Get_first_unprotected(
  Chain_Control *the_chain
);

/*
 *  _Chain_Insert_unprotected
 *
 *  DESCRIPTION:
 *
 *  This routine inserts the_node on a chain immediately following
 *  after_node.  It does NOT disable interrupts to insure the atomicity
 *  of the extract operation.
 *
 */

STATIC INLINE void _Chain_Insert_unprotected(
  Chain_Node *after_node,
  Chain_Node *the_node
);

/*
 *  _Chain_Insert
 *
 *  DESCRIPTION:
 *
 *  This routine inserts the_node on a chain immediately following
 *  after_node.  It disables interrupts to insure the atomicity
 *  of the extract operation.
 *
 */

void _Chain_Insert(
  Chain_Node *after_node,
  Chain_Node *the_node
);

/*
 *  _Chain_Append_unprotected
 *
 *  DESCRIPTION:
 *
 *  This routine appends the_node onto the end of the_chain.
 *  It does NOT disable interrupts to insure the atomicity of the
 *  append operation.
 *
 */

STATIC INLINE void _Chain_Append_unprotected(
  Chain_Control *the_chain,
  Chain_Node    *the_node
);

/*
 *  _Chain_Append
 *
 *  DESCRIPTION:
 *
 *  This routine appends the_node onto the end of the_chain.
 *  It disables interrupts to insure the atomicity of the
 *  append operation.
 *
 */

void _Chain_Append(
  Chain_Control *the_chain,
  Chain_Node    *the_node
);

/*
 *  _Chain_Prepend_unprotected
 *
 *  DESCRIPTION:
 *
 *  This routine prepends the_node onto the front of the_chain.
 *  It does NOT disable interrupts to insure the atomicity of the
 *  prepend operation.
 *
 */

STATIC INLINE void _Chain_Prepend_unprotected(
  Chain_Control *the_chain,
  Chain_Node    *the_node
);

/*
 *  _Chain_Prepend
 *
 *  DESCRIPTION:
 *
 *  This routine prepends the_node onto the front of the_chain.
 *  It disables interrupts to insure the atomicity of the
 *  prepend operation.
 *
 */

STATIC INLINE void _Chain_Prepend(
  Chain_Control *the_chain,
  Chain_Node    *the_node
);

/*
 *  _Chain_Head
 *
 *  DESCRIPTION:
 *
 *  This function returns a pointer to the first node on the chain.
 *
 */

STATIC INLINE Chain_Node *_Chain_Head(
  Chain_Control *the_chain
);

/*
 *  _Chain_Tail
 *
 *  DESCRIPTION:
 *
 *  This function returns a pointer to the last node on the chain.
 *
 */

STATIC INLINE Chain_Node *_Chain_Tail(
  Chain_Control *the_chain
);

/*
 *  _Chain_Is_head
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_node is the head of the_chain and
 *  FALSE otherwise.
 *
 */

STATIC INLINE boolean _Chain_Is_head(
  Chain_Control *the_chain,
  Chain_Node    *the_node
);

/*
 *  _Chain_Is_tail
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_node is the tail of the_chain and
 *  FALSE otherwise.
 *
 */

STATIC INLINE boolean _Chain_Is_tail(
  Chain_Control *the_chain,
  Chain_Node    *the_node
);

/*
 *  _Chain_Is_first
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_node is the first node on a chain and
 *  FALSE otherwise.
 *
 */

STATIC INLINE boolean _Chain_Is_first(
  Chain_Node *the_node
);

/*
 *  _Chain_Is_last
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_node is the last node on a chain and
 *  FALSE otherwise.
 *
 */

STATIC INLINE boolean _Chain_Is_last(
  Chain_Node *the_node
);

/*
 *  _Chain_Is_empty
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if there a no nodes on the_chain and
 *  FALSE otherwise.
 *
 */

STATIC INLINE boolean _Chain_Is_empty(
  Chain_Control *the_chain
);

/*
 *  _Chain_Has_only_one_node
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if there is only one node on the_chain and
 *  FALSE otherwise.
 *
 */

STATIC INLINE boolean _Chain_Has_only_one_node(
  Chain_Control *the_chain
);

/*
 *  _Chain_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_chain is NULL and FALSE otherwise.
 *
 */

STATIC INLINE boolean _Chain_Is_null(
  Chain_Control *the_chain
);

/*
 *  _Chain_Is_null_node
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_node is NULL and FALSE otherwise.
 *
 */

STATIC INLINE boolean _Chain_Is_null_node(
  Chain_Node *the_node
);

#include <rtems/score/chain.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
