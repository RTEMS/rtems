/*  macros/chain.h
 *
 *  This include file contains the bodies of the routines which are
 *  associated with doubly linked chains and inlined.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __MACROS_CHAIN_h
#define __MACROS_CHAIN_h

/*PAGE
 *
 *  _Chain_Are_nodes_equal
 */

#define _Chain_Are_nodes_equal( _left, _right ) \
  ( (_left) == (_right) )

/*PAGE
 *
 *  _Chain_Is_null
 */

#define _Chain_Is_null( _the_chain ) \
  ( (_the_chain) == NULL )

/*PAGE
 *
 *  _Chain_Is_null_node
 */

#define _Chain_Is_null_node( _the_node ) \
  ( (_the_node) == NULL )

/*PAGE
 *
 *  _Chain_Head
 */

#define _Chain_Head( _the_chain ) \
   ((Chain_Node *) (_the_chain))

/*PAGE
 *
 *  _Chain_Tail
 */

#define _Chain_Tail( _the_chain ) \
   ((Chain_Node *) &(_the_chain)->permanent_null)

/*PAGE
 *
 *  _Chain_Is_empty
 */

#define _Chain_Is_empty( _the_chain ) \
  ( (_the_chain)->first == _Chain_Tail( (_the_chain) ) )

/*PAGE
 *
 *  _Chain_Is_first
 */

#define _Chain_Is_first( _the_node ) \
  ( (the_node)->previous == NULL )

/*PAGE
 *
 *  _Chain_Is_last
 */

#define _Chain_Is_last( _the_node ) \
  ( (_the_node)->next == NULL )

/*PAGE
 *
 *  _Chain_Has_only_one_node
 */

#define _Chain_Has_only_one_node( _the_chain ) \
  ( (_the_chain)->first == (_the_chain)->last )

/*PAGE
 *
 *  _Chain_Is_head
 */

#define _Chain_Is_head( _the_chain, _the_node ) \
   ( (_the_node) == _Chain_Head( (_the_chain) ) )

/*PAGE
 *
 *  _Chain_Is_tail
 */

#define _Chain_Is_tail( _the_chain, _the_node ) \
   ( (_the_node) == _Chain_Tail( (_the_chain) ) )

/*PAGE
 *
 *  Chain_Initialize_empty
 */

#define _Chain_Initialize_empty( _the_chain ) \
{ \
  (_the_chain)->first          = _Chain_Tail( (_the_chain) ); \
  (_the_chain)->permanent_null = NULL; \
  (_the_chain)->last           = _Chain_Head( (_the_chain) ); \
}

/*PAGE
 *
 *  _Chain_Extract_unprotected
 */

#define _Chain_Extract_unprotected( _the_node ) \
{ \
  Chain_Node *_next; \
  Chain_Node *_previous; \
   \
  _next           = (_the_node)->next; \
  _previous       = (_the_node)->previous; \
  _next->previous = _previous; \
  _previous->next = _next; \
}

/*PAGE
 *
 *  _Chain_Get_unprotected
 */

/*PAGE
 *
 *  Chain_Get_unprotected
 */

#define _Chain_Get_unprotected( _the_chain ) \
  (( !_Chain_Is_empty( (_the_chain) ) ) \
    ? _Chain_Get_first_unprotected( (_the_chain) ) \
    : NULL)

/*PAGE
 *
 *  _Chain_Insert_unprotected
 */

#define _Chain_Insert_unprotected( _after_node, _the_node ) \
do {  \
  Chain_Node *_before_node; \
   \
  (_the_node)->previous  = (_after_node); \
  _before_node           = (_after_node)->next; \
  (_after_node)->next    = (_the_node);  \
  (_the_node)->next      = _before_node;  \
  _before_node->previous = (_the_node);  \
} while (0)

/*PAGE
 *
 *  _Chain_Append_unprotected
 */

#define _Chain_Append_unprotected( _the_chain, _the_node ) \
{ \
  Chain_Node *_old_last_node; \
    \
  (_the_node)->next     = _Chain_Tail( (_the_chain) ); \
  _old_last_node        = (_the_chain)->last; \
  (_the_chain)->last    = (_the_node); \
  _old_last_node->next  = (_the_node); \
  (_the_node)->previous = _old_last_node; \
}

/*PAGE
 *
 *  _Chain_Prepend_unprotected
 */

#define _Chain_Prepend_unprotected( _the_chain, _the_node ) \
  _Chain_Insert_unprotected( _Chain_Head( (_the_chain) ), (_the_node) )

/*PAGE
 *
 *  _Chain_Prepend
 */

#define _Chain_Prepend( _the_chain, _the_node ) \
  _Chain_Insert( _Chain_Head( (_the_chain) ), (_the_node) )

#endif
/* end of include file */
