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
 *  _Objects_Build_id
 *
 *  DESCRIPTION:
 *
 *  This function builds an object's id from the processor node and index
 *  values specified.
 */

RTEMS_INLINE_ROUTINE Objects_Id _Objects_Build_id(
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
 *  _Objects_Get_class
 *
 *  DESCRIPTION:
 *
 *  This function returns the class portion of the ID.
 */
 
RTEMS_INLINE_ROUTINE Objects_Classes _Objects_Get_class(
  Objects_Id id
)
{
  return (Objects_Classes) 
    ((id >> OBJECTS_CLASS_START_BIT) & OBJECTS_CLASS_VALID_BITS);
}
 

/*PAGE
 *
 *  _Objects_Get_node
 *
 *  DESCRIPTION:
 *
 *  This function returns the node portion of the ID.
 */

RTEMS_INLINE_ROUTINE unsigned32 _Objects_Get_node(
  Objects_Id id
)
{
  return (id >> OBJECTS_NODE_START_BIT) & OBJECTS_NODE_VALID_BITS;
}

/*PAGE
 *
 *  _Objects_Get_index
 *
 *  DESCRIPTION:
 *
 *  This function returns the index portion of the ID.
 */

RTEMS_INLINE_ROUTINE unsigned32 _Objects_Get_index(
  Objects_Id id
)
{
  return (id >> OBJECTS_INDEX_START_BIT) & OBJECTS_INDEX_VALID_BITS;
}

/*PAGE
 *
 *  _Objects_Is_class_valid
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the class is valid.
 */
 
RTEMS_INLINE_ROUTINE boolean _Objects_Is_class_valid(
  Objects_Classes the_class
)
{
  return the_class <= OBJECTS_CLASSES_LAST;
}

/*PAGE
 *
 *  _Objects_Is_local_node
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the node is of the local object, and
 *  FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Objects_Is_local_node(
  unsigned32 node
)
{
  return ( node == _Objects_Local_node );
}

/*PAGE
 *
 *  _Objects_Is_local_id
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the id is of a local object, and
 *  FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Objects_Is_local_id(
  Objects_Id id
)
{
  return _Objects_Is_local_node( _Objects_Get_node(id) );
}

/*PAGE
 *
 *  _Objects_Are_ids_equal
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if left and right are equal,
 *  and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Objects_Are_ids_equal(
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
 *  DESCRIPTION:
 *
 *  This function allocates a object control block from
 *  the inactive chain of free object control blocks.
 */

RTEMS_INLINE_ROUTINE Objects_Control *_Objects_Allocate(
  Objects_Information *information
)
{
  return (Objects_Control *) _Chain_Get( &information->Inactive );
}

/*PAGE
 *
 *  _Objects_Free
 *
 *  DESCRIPTION:
 *
 *  This function frees a object control block to the
 *  inactive chain of free object control blocks.
 */

RTEMS_INLINE_ROUTINE void _Objects_Free(
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
 *  DESCRIPTION:
 *
 *  This function places the_object control pointer and object name
 *  in the Local Pointer and Local Name Tables, respectively.
 */

RTEMS_INLINE_ROUTINE void _Objects_Open(
  Objects_Information *information,
  Objects_Control     *the_object,
  Objects_Name         name
)
{
  unsigned32  index;

  index = _Objects_Get_index( the_object->id );
  information->local_table[ index ] = the_object;

  if ( information->is_string ) 
    _Objects_Copy_name_string( name, the_object->name );
  else
    _Objects_Copy_name_raw( name, the_object->name, information->name_length );
}

/*PAGE
 *
 *  _Objects_Close
 *
 *  DESCRIPTION:
 *
 *  This function removes the_object control pointer and object name
 *  in the Local Pointer and Local Name Tables.
 */

RTEMS_INLINE_ROUTINE void _Objects_Close(
  Objects_Information  *information,
  Objects_Control      *the_object
)
{
  unsigned32 index;

  index = _Objects_Get_index( the_object->id );
  information->local_table[ index ] = NULL;
  _Objects_Clear_name( the_object->name, information->name_length );
}

#endif
/* end of include file */
