/*  chain.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Doubly Linked Chain Handler.
 *
 *  COPYRIGHT (c) 1988-2002.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights reserved.
 */

#ifndef __CHAIN_h
#define __CHAIN_h

#include <stddef.h>

#include "address.h"

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
 */

typedef struct {
  Chain_Node *first;
  Chain_Node *permanent_null;
  Chain_Node *last;
} Chain_Control;

/*
 *  _Chain_Initialize
 *
 *  This routine initializes the_chain structure to manage the
 *  contiguous array of number_nodes nodes which starts at
 *  starting_address.  Each node is of node_size bytes.
 */

void _Chain_Initialize(
  Chain_Control *the_chain,
  void          *starting_address,
  size_t         number_nodes,
  size_t         node_size
);

/*
 *  _Chain_Initialize_empty
 *
 *  This routine initializes the specified chain to contain zero nodes.
 */

STATIC INLINE void _Chain_Initialize_empty(
  Chain_Control *the_chain
);

/*
 *  _Chain_Extract_unprotected
 *
 *  This routine extracts the_node from the chain on which it resides.
 *  It does NOT disable interrupts to insure the atomicity of the
 *  extract operation.
 */

STATIC INLINE void _Chain_Extract_unprotected(
  Chain_Node *the_node
);

/*
 *  _Chain_Extract
 *
 *  This routine extracts the_node from the chain on which it resides.
 *  It disables interrupts to insure the atomicity of the
 *  extract operation.
 */

void _Chain_Extract(
  Chain_Node *the_node
);

/*
 *  _Chain_Get_unprotected
 *
 *  This function removes the first node from the_chain and returns
 *  a pointer to that node.  If the_chain is empty, then NULL is returned.
 *  It does NOT disable interrupts to insure the atomicity of the
 *  get operation.
 */

STATIC INLINE Chain_Node *_Chain_Get_unprotected(
  Chain_Control *the_chain
);

/*
 *  _Chain_Get
 *
 *  This function removes the first node from the_chain and returns
 *  a pointer to that node.  If the_chain is empty, then NULL is returned.
 *  It disables interrupts to insure the atomicity of the
 *  get operation.
 */

Chain_Node *_Chain_Get(
  Chain_Control *the_chain
);

/*
 *  _Chain_Get_first_unprotected
 *
 *  This function removes the first node from the_chain and returns
 *  a pointer to that node.  It does NOT disable interrupts to insure
 *  the atomicity of the get operation.
 */

STATIC INLINE Chain_Node *_Chain_Get_first_unprotected(
  Chain_Control *the_chain
);

/*
 *  _Chain_Insert_unprotected
 *
 *  This routine inserts the_node on a chain immediately following
 *  after_node.  It does NOT disable interrupts to insure the atomicity
 *  of the extract operation.
 */

STATIC INLINE void _Chain_Insert_unprotected(
  Chain_Node *after_node,
  Chain_Node *the_node
);

/*
 *  _Chain_Insert
 *
 *  This routine inserts the_node on a chain immediately following
 *  after_node.  It disables interrupts to insure the atomicity
 *  of the extract operation.
 */

void _Chain_Insert(
  Chain_Node *after_node,
  Chain_Node *the_node
);

/*
 *  _Chain_Append_unprotected
 *
 *  This routine appends the_node onto the end of the_chain.
 *  It does NOT disable interrupts to insure the atomicity of the
 *  append operation.
 */

STATIC INLINE void _Chain_Append_unprotected(
  Chain_Control *the_chain,
  Chain_Node    *the_node
);

/*
 *  _Chain_Append
 *
 *  This routine appends the_node onto the end of the_chain.
 *  It disables interrupts to insure the atomicity of the
 *  append operation.
 */

void _Chain_Append(
  Chain_Control *the_chain,
  Chain_Node    *the_node
);

/*
 *  _Chain_Prepend_unprotected
 *
 *  This routine prepends the_node onto the front of the_chain.
 *  It does NOT disable interrupts to insure the atomicity of the
 *  prepend operation.
 */

STATIC INLINE void _Chain_Prepend_unprotected(
  Chain_Control *the_chain,
  Chain_Node    *the_node
);

/*
 *  _Chain_Prepend
 *
 *  This routine prepends the_node onto the front of the_chain.
 *  It disables interrupts to insure the atomicity of the
 *  prepend operation.
 */

STATIC INLINE void _Chain_Prepend(
  Chain_Control *the_chain,
  Chain_Node    *the_node
);

/*
 *  _Chain_Insert_chain
 *
 *  This routine inserts a chain after the specified node in another
 *  chain. It is assumed that the insert after node is not on the
 *  second chain.
 */

void _Chain_Insert_chain(
  Chain_Node    *insert_after,
  Chain_Control *to_insert
);

/*
 *  _Chain_Head
 *
 *  This function returns a pointer to the first node on the chain.
 */

STATIC INLINE Chain_Node *_Chain_Head(
  Chain_Control *the_chain
);

/*
 *  _Chain_Tail
 *
 *  This function returns a pointer to the last node on the chain.
 */

STATIC INLINE Chain_Node *_Chain_Tail(
  Chain_Control *the_chain
);

/*
 *  _Chain_Is_head
 *
 *  This function returns TRUE if the_node is the head of the_chain and
 *  FALSE otherwise.
 */

STATIC INLINE boolean _Chain_Is_head(
  Chain_Control *the_chain,
  Chain_Node    *the_node
);

/*
 *  _Chain_Is_tail
 *
 *  This function returns TRUE if the_node is the tail of the_chain and
 *  FALSE otherwise.
 */

STATIC INLINE boolean _Chain_Is_tail(
  Chain_Control *the_chain,
  Chain_Node    *the_node
);

/*
 *  _Chain_Is_first
 *
 *  This function returns TRUE if the_node is the first node on a chain and
 *  FALSE otherwise.
 */

STATIC INLINE boolean _Chain_Is_first(
  Chain_Node *the_node
);

/*
 *  _Chain_Is_last
 *
 *  This function returns TRUE if the_node is the last node on a chain and
 *  FALSE otherwise.
 */

STATIC INLINE boolean _Chain_Is_last(
  Chain_Node *the_node
);

/*
 *  _Chain_Is_empty
 *
 *  This function returns TRUE if there a no nodes on the_chain and
 *  FALSE otherwise.
 */

STATIC INLINE boolean _Chain_Is_empty(
  Chain_Control *the_chain
);

/*
 *  _Chain_Has_only_one_node
 *
 *  This function returns TRUE if there is only one node on the_chain and
 *  FALSE otherwise.
 */

STATIC INLINE boolean _Chain_Has_only_one_node(
  Chain_Control *the_chain
);

/*
 *  _Chain_Is_null
 *
 *  This function returns TRUE if the_chain is NULL and FALSE otherwise.
 */

STATIC INLINE boolean _Chain_Is_null(
  Chain_Control *the_chain
);

/*
 *  _Chain_Is_null_node
 *
 *  This function returns TRUE if the_node is NULL and FALSE otherwise.
 */

STATIC INLINE boolean _Chain_Is_null_node(
  Chain_Node *the_node
);

#include "chain.inl"

#endif
/* end of include file */
