/*  object.inl
 *
 *  This include file contains the macro implementation of all
 *  of the inlined routines in the Object Handler.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  object.inl,v 1.2 1995/05/31 16:49:20 joel Exp
 */

#ifndef __OBJECTS_inl
#define __OBJECTS_inl

/*PAGE
 *
 *  _Objects_Is_name_valid
 *
 */

#define _Objects_Is_name_valid( _name ) \
  ( (_name) != 0 )

/*
 *  rtems_name_to_characters
 *
 */

#define rtems_name_to_characters( _name, _c1, _c2, _c3, _c4 ) \
  { \
    (*(_c1) = ((_name) >> 24) & 0xff; \
    (*(_c2) = ((_name) >> 16) & 0xff; \
    (*(_c3) = ((_name) >> 8) & 0xff; \
    (*(_c4) = ((_name)) & 0xff; \
  }

/*PAGE
 *
 *  _Objects_Build_id
 *
 */

#define _Objects_Build_id( _the_class, _node, _index ) \
  ( ((_the_class) << OBJECTS_CLASS_START_BIT) | \
    ((_node) << OBJECTS_NODE_START_BIT)       | \
    ((_index) << OBJECTS_INDEX_START_BIT) )

/*PAGE
 *
 *  rtems_get_class
 */
 
#define rtems_get_class( _id ) \
  (Objects_Classes) \
    (((_id) >> OBJECTS_CLASS_START_BIT) & OBJECTS_CLASS_VALID_BITS)

/*PAGE
 *
 *  rtems_get_node
 *
 */

#define rtems_get_node( _id ) \
  (((_id) >> OBJECTS_NODE_START_BIT) & OBJECTS_NODE_VALID_BITS)

/*PAGE
 *
 *  rtems_get_index
 *
 */

#define rtems_get_index( _id ) \
  (((_id) >> OBJECTS_INDEX_START_BIT) & OBJECTS_INDEX_VALID_BITS)

/*PAGE
 *
 *  _Objects_Is_local_node
 *
 */

#define _Objects_Is_local_node( _node ) \
  ( (_node) == _Objects_Local_node )

/*PAGE
 *
 *  _Objects_Is_local_id
 *
 */

#define _Objects_Is_local_id( _id ) \
  _Objects_Is_local_node( rtems_get_node(_id) )

/*PAGE
 *
 *  _Objects_Are_ids_equal
 *
 */

#define _Objects_Are_ids_equal( _left, _right ) \
  ( (_left) == (_right) )

/*PAGE
 *
 *  _Objects_Allocate
 *
 */

#define _Objects_Allocate( _information )  \
  (Objects_Control *) _Chain_Get( &(_information)->Inactive )

/*PAGE
 *
 *  _Objects_Free
 *
 */

#define _Objects_Free( _information, _the_object )  \
  _Chain_Append( &(_information)->Inactive, &(_the_object)->Node )

/*PAGE
 *
 *  _Objects_Open
 *
 */

#define _Objects_Open( _information, _the_object, _name ) \
  { \
    unsigned32 _index; \
    \
    _index = rtems_get_index( (_the_object)->id ); \
    (_information)->local_table[ _index ] = (_the_object); \
    (_information)->name_table[ _index ]  = (_name); \
    (_the_object)->name = &(_information)->name_table[ _index ]; \
  }

/*PAGE
 *
 *  _Objects_Close
 *
 */

#define _Objects_Close( _information, _the_object ) \
  { \
    unsigned32 _index; \
    \
    _index = rtems_get_index( (_the_object)->id ); \
    (_information)->local_table[ _index ] = NULL; \
    (_information)->name_table[ _index ]  = 0; \
    (_the_object)->name = 0; \
  }

#endif
/* end of include file */
