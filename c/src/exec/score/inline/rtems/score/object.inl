/*  object.inl
 *
 *  This include file contains the static inline implementation of all
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
 *  $Id$
 */

#ifndef __OBJECTS_inl
#define __OBJECTS_inl

/*PAGE
 *
 *  _Objects_Is_name_valid
 *
 */

STATIC INLINE boolean _Objects_Is_name_valid (
  Objects_Name name
)
{
  return ( name != 0 );
}

/*PAGE
 *
 *  rtems_name_to_characters
 *
 */

STATIC INLINE void rtems_name_to_characters(
  Objects_Name  name,
  char         *c1,
  char         *c2,
  char         *c3,
  char         *c4
)
{
  *c1 = (name >> 24) & 0xff;
  *c2 = (name >> 16) & 0xff;
  *c3 = (name >> 8) & 0xff;
  *c4 =  name & 0xff;
}

/*PAGE
 *
 *  _Objects_Build_id
 *
 */

STATIC INLINE Objects_Id _Objects_Build_id(
  Objects_Classes  the_class,
  unsigned32       node,
  unsigned32       index
)
{
  return ( (the_class << OBJECTS_CLASS_START_BIT) |
           (node << OBJECTS_NODE_START_BIT)       |
           (index << OBJECTS_INDEX_START_BIT) );
}

/*PAGE
 *
 *  rtems_get_class
 */
 
STATIC INLINE Objects_Classes rtems_get_class(
  Objects_Id id
)
{
  return (Objects_Classes) 
    ((id >> OBJECTS_CLASS_START_BIT) & OBJECTS_CLASS_VALID_BITS);
}
 

/*PAGE
 *
 *  rtems_get_node
 *
 */

STATIC INLINE unsigned32 rtems_get_node(
  Objects_Id id
)
{
  return (id >> OBJECTS_NODE_START_BIT) & OBJECTS_NODE_VALID_BITS;
}

/*PAGE
 *
 *  rtems_get_index
 *
 */

STATIC INLINE unsigned32 rtems_get_index(
  Objects_Id id
)
{
  return (id >> OBJECTS_INDEX_START_BIT) & OBJECTS_INDEX_VALID_BITS;
}

/*PAGE
 *
 *  _Objects_Is_local_node
 *
 */

STATIC INLINE boolean _Objects_Is_local_node(
  unsigned32 node
)
{
  return ( node == _Objects_Local_node );
}

/*PAGE
 *
 *  _Objects_Is_local_id
 *
 */

STATIC INLINE boolean _Objects_Is_local_id(
  Objects_Id id
)
{
  return _Objects_Is_local_node( rtems_get_node(id) );
}

/*PAGE
 *
 *  _Objects_Are_ids_equal
 *
 */

STATIC INLINE boolean _Objects_Are_ids_equal(
  Objects_Id left,
  Objects_Id right
)
{
  return ( left == right );
}

/*PAGE
 *
 *  _Objects_Allocate
 *
 */

STATIC INLINE Objects_Control *_Objects_Allocate(
  Objects_Information *information
)
{
  return (Objects_Control *) _Chain_Get( &information->Inactive );
}

/*PAGE
 *
 *  _Objects_Free
 *
 */

STATIC INLINE void _Objects_Free(
  Objects_Information *information,
  Objects_Control     *the_object
)
{
  _Chain_Append( &information->Inactive, &the_object->Node );
}

/*PAGE
 *
 *  _Objects_Open
 *
 */

STATIC INLINE void _Objects_Open(
  Objects_Information *information,
  Objects_Control     *the_object,
  Objects_Name         name
)
{
  unsigned32 index;

  index = rtems_get_index( the_object->id );
  information->local_table[ index ] = the_object;
  information->name_table[ index ]  = name;
  the_object->name = &information->name_table[ index ];
}

/*PAGE
 *
 *  _Objects_Close
 *
 */

STATIC INLINE void _Objects_Close(
  Objects_Information  *information,
  Objects_Control      *the_object
)
{
  unsigned32 index;

  index = rtems_get_index( the_object->id );
  information->local_table[ index ] = NULL;
  information->name_table[ index ]  = 0;
  the_object->name = 0;
}

#endif
/* end of include file */
