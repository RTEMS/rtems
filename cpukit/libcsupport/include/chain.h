/*  chain.h
 * 
 *  This include file contains all the constants and structures associated
 *  with doubly linked chains.  This file actually just provides an
 *  interface to the chain object in rtems.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 * $ld:
 */

#ifndef __CHAIN_h
#define __CHAIN_h

#include <rtems.h>

/* 
 *  Chain_Initialize
 *
 *  This routine initializes the_chain structure to manage the
 *  contiguous array of number_nodes nodes which starts at
 *  starting_address.  Each node is of node_size bytes.
 *
 *  Chain_Control                    *the_chain,		 * IN  * 
 *  void                             *starting_address,		 * IN  * 
 *  rtems_unsigned32                  number_nodes,		 * IN  * 
 *  rtems_unsigned32                  node_size			 * IN  * 
 */

#define Chain_Initialize( the_chain, starting_address, \
                          number_nodes, node_size ) \
       _Chain_Initialize( the_chain, starting_address, \
                          number_nodes, node_size ) \


/* 
 *  Chain_Initialize_empty
 *
 *  This routine initializes the specified chain to contain zero nodes.
 *
 *  Chain_Control                    *the_chain			 * IN  * 
 */

#define Chain_Initialize_empty( the_chain ) \
       _Chain_Initialize_empty( the_chain )
 

/*
 *  Chain_Are_nodes_equal
 *
 *  This function returns TRUE if LEFT and RIGHT are equal,
 *  and FALSE otherwise.
 *
 *  Chain_Node                       *left,			 * IN  * 
 *  Chain_Node                       *right			 * IN  * 
 */
 
#define Chain_Are_nodes_equal( left, right ) \
       _Chain_Are_nodes_equal( left, right )


/* 
 *  Chain_Extract_unprotected
 *
 *  This routine extracts the_node from the chain on which it resides.
 *  It does NOT disable interrupts to insure the atomicity of the
 *  extract operation.
 *
 *  Chain_Node                       *the_node			 * IN  * 
 */

#define Chain_Extract_unprotected( the_node ) \
       _Chain_Extract_unprotected( the_node )


/* 
 *  Chain_Extract
 *
 *  This routine extracts the_node from the chain on which it resides.
 *  It disables interrupts to insure the atomicity of the
 *  extract operation.
 *
 *  Chain_Node                       *the_node			 * IN  * 
 */

#define Chain_Extract( the_node ) \
       _Chain_Extract( the_node )


/* 
 *  Chain_Get_unprotected
 *
 *  This function removes the first node from the_chain and returns
 *  a pointer to that node.  If the_chain is empty, then NULL is returned.
 *  It does NOT disable interrupts to insure the atomicity of the
 *  get operation.
 *
 *  Chain_Control                    *the_chain			 * IN  * 
 */

#define Chain_Get_unprotected( the_chain ) \
       _Chain_Get_unprotected( the_chain )


/* 
 *  Chain_Get
 *
 *  This function removes the first node from the_chain and returns
 *  a pointer to that node.  If the_chain is empty, then NULL is returned.
 *  It disables interrupts to insure the atomicity of the
 *  get operation.
 *
 *  Chain_Control                    *the_chain			 * IN  * 
 */

#define Chain_Get( the_chain ) \
       _Chain_Get( the_chain )


/* 
 *  Chain_Get_first_unprotected
 *
 *  This function removes the first node from the_chain and returns
 *  a pointer to that node.  It does NOT disable interrupts to insure
 *  the atomicity of the get operation.
 *
 *  Chain_Control                    *the_chain			 * IN  * 
 */

#define Chain_Get_first_unprotected( the_chain ) \
       _Chain_Get_first_unprotected( the_chain )


/* 
 *  Chain_Insert_unprotected
 *
 *  This routine inserts the_node on a chain immediately following
 *  after_node.  It does NOT disable interrupts to insure the atomicity
 *  of the extract operation.
 *
 *  Chain_Node                       *after_node,		 * IN  * 
 *  Chain_Node                       *the_node			 * IN  * 
 */

#define Chain_Insert_unprotected( after_node, the_node ) \
       _Chain_Insert_unprotected( after_node, the_node )


/* 
 *  Chain_Insert
 *
 *  This routine inserts the_node on a chain immediately following
 *  after_node.  It disables interrupts to insure the atomicity
 *  of the extract operation.
 *
 *  Chain_Node                       *after_node,		 * IN  * 
 *  Chain_Node                       *the_node			 * IN  * 
 */

#define Chain_Insert( after_node, the_node ) \
       _Chain_Insert( after_node, the_node )


/* 
 *  Chain_Append_unprotected
 *
 *  This routine appends the_node onto the end of the_chain.
 *  It does NOT disable interrupts to insure the atomicity of the
 *  append operation.
 *
 *  Chain_Control                    *the_chain,		 * IN  * 
 *  Chain_Node                       *the_node			 * IN  * 
 */

#define Chain_Append_unprotected( the_chain, the_node ) \
       _Chain_Append_unprotected( the_chain, the_node )


/* 
 *  Chain_Append
 *
 *  This routine appends the_node onto the end of the_chain.
 *  It disables interrupts to insure the atomicity of the
 *  append operation.
 *
 *  Chain_Control                    *the_chain,		 * IN  * 
 *  Chain_Node                       *the_node			 * IN  * 
 */

#define Chain_Append( the_chain, the_node ) \
       _Chain_Append( the_chain, the_node )


/* 
 *  Chain_Prepend_unprotected
 *
 *  This routine prepends the_node onto the front of the_chain.
 *  It does NOT disable interrupts to insure the atomicity of the
 *  prepend operation.
 *
 *  Chain_Control                    *the_chain,		 * IN  * 
 *  Chain_Node                       *the_node			 * IN  * 
 */

#define Chain_Prepend_unprotected( the_chain, the_node ) \
       _Chain_Prepend_unprotected( the_chain, the_node )


/* 
 *  Chain_Prepend
 *
 *  This routine prepends the_node onto the front of the_chain.
 *  It disables interrupts to insure the atomicity of the
 *  prepend operation.
 *
 *  Chain_Control                    *the_chain,		 * IN  * 
 *  Chain_Node                       *the_node			 * IN  * 
 */

#define Chain_Prepend( the_chain, the_node ) \
       _Chain_Prepend( the_chain, the_node )


/* 
 *  Chain_Head
 *
 *  This function returns a pointer to the first node on the chain.
 *
 *  Chain_Control                    *the_chain			 * IN  * 
 */

#define Chain_Head( the_chain ) \
       _Chain_Head( the_chain )


/* 
 *  Chain_Tail
 *
 *  This function returns a pointer to the last node on the chain.
 *
 *  Chain_Control                    *the_chain			 * IN  * 
 */

#define Chain_Tail( the_chain ) \
       _Chain_Tail( the_chain )


/* 
 *  Chain_Is_head
 *
 *  This function returns TRUE if the_node is the head of the_chain and
 *  FALSE otherwise.
 *
 *  Chain_Control                    *the_chain,		 * IN  * 
 *  Chain_Node                       *the_node			 * IN  * 
 */

#define Chain_Is_head( the_chain, the_node ) \
       _Chain_Is_head( the_chain, the_node )


/* 
 *  Chain_Is_tail
 *
 *  This function returns TRUE if the_node is the tail of the_chain and
 *  FALSE otherwise.
 *
 *  Chain_Control                    *the_chain,		 * IN  * 
 *  Chain_Node                       *the_node			 * IN  * 
 */

#define Chain_Is_tail( the_chain, the_node ) \
       _Chain_Is_tail( the_chain, the_node )


/* 
 *  Chain_Is_first
 *
 *  This function returns TRUE if the_node is the first node on a chain and
 *  FALSE otherwise.
 *
 *  Chain_Node                       *the_node			 * IN  * 
 */

#define Chain_Is_first( the_node ) \
       _Chain_Is_first( the_node )


/* 
 *  Chain_Is_last
 *
 *  This function returns TRUE if the_node is the last node on a chain and
 *  FALSE otherwise.
 *
 *  Chain_Node                       *the_node			 * IN  * 
 */

#define Chain_Is_last( the_node ) \
       _Chain_Is_last( the_node )


/* 
 *  Chain_Is_empty
 *
 *  This function returns TRUE if there are no nodes on the_chain and
 *  FALSE otherwise.
 *
 *  Chain_Control                    *the_chain			 * IN  * 
 */

#define Chain_Is_empty( the_chain ) \
       _Chain_Is_empty( the_chain )


/* 
 *  Chain_Has_only_one_node
 *
 *  This function returns TRUE if there is only one node on the_chain and
 *  FALSE otherwise.
 *
 *  Chain_Control                    *the_chain			 * IN  * 
 */

#define Chain_Has_only_one_node( the_chain ) \
       _Chain_Has_only_one_node( the_chain )


/* 
 *  Chain_Is_null
 *
 *  This function returns TRUE if the_chain is NULL and FALSE otherwise.
 *
 *  Chain_Control                    *the_chain			 * IN  * 
 */

#define Chain_Is_null( the_chain ) \
       _Chain_Is_null( the_chain )


/* 
 *  Chain_Is_null_node
 *
 *  This function returns TRUE if the_node is NULL and FALSE otherwise.
 *
 *  Chain_Node                       *the_node			 * IN  * 
 */

#define Chain_Is_null_node( the_node ) \
       _Chain_Is_null_node( the_node )


#undef __RTEMS_APPLICATION__
#include <rtems/score/chain.inl>
#define __RTEMS_APPLICATION__
#endif
/* end of include file */
