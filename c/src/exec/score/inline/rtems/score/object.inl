/*  object.inl
 *
 *  This include file contains the static inline implementation of all
 *  of the inlined routines in the Object Handler.
 *
 *  COPYRIGHT (c) 1989-2002.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
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
  Objects_APIs     the_api,
  unsigned32       the_class,
  unsigned32       node,
  unsigned32       index
)
{
  return (( (Objects_Id) the_api )   << OBJECTS_API_START_BIT)   |
         (( (Objects_Id) the_class ) << OBJECTS_CLASS_START_BIT) |
         (( (Objects_Id) node )      << OBJECTS_NODE_START_BIT)  |
         (( (Objects_Id) index )     << OBJECTS_INDEX_START_BIT);
}

/*PAGE
 *
 *  _Objects_Get_API
 *
 *  DESCRIPTION:
 *
 *  This function returns the API portion of the ID.
 */

RTEMS_INLINE_ROUTINE Objects_APIs _Objects_Get_API(
  Objects_Id id
)
{
  return (Objects_APIs) ((id >> OBJECTS_API_START_BIT) & OBJECTS_API_VALID_BITS);
}

/*PAGE
 *
 *  _Objects_Get_class
 *
 *  DESCRIPTION:
 *
 *  This function returns the class portion of the ID.
 */
 
RTEMS_INLINE_ROUTINE unsigned32 _Objects_Get_class(
  Objects_Id id
)
{
  return (unsigned32) 
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
  unsigned32 the_class
)
{
  /* XXX how do we determine this now? */
  return TRUE; /* the_class && the_class <= OBJECTS_CLASSES_LAST; */
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
 *  _Objects_Get_local_object
 *
 *  DESCRIPTION:
 *
 *  This function returns a pointer to the local_table object
 *  referenced by the index.
 */

RTEMS_INLINE_ROUTINE Objects_Control *_Objects_Get_local_object(
  Objects_Information *information,
  unsigned32           index
)
{
  if ( index > information->maximum )
    return NULL;
  return information->local_table[ index ];
}

/*PAGE
 *
 *  _Objects_Set_local_object
 *
 *  DESCRIPTION:
 *
 *  This function sets the pointer to the local_table object
 *  referenced by the index.
 */

RTEMS_INLINE_ROUTINE void _Objects_Set_local_object(
  Objects_Information *information,
  unsigned32           index,
  Objects_Control     *the_object
)
{
  if ( index <= information->maximum )
    information->local_table[ index ] = the_object;
}


/*PAGE
 *
 *  _Objects_Get_information
 *
 *  DESCRIPTION:
 *
 *  This function return the information structure given
 *  an id of an object.
 */
 
RTEMS_INLINE_ROUTINE Objects_Information *_Objects_Get_information(
  Objects_Id  id
)
{
  Objects_APIs  the_api;
  unsigned32    the_class;


  the_class = _Objects_Get_class( id );

  if ( !_Objects_Is_class_valid( the_class ) )
    return NULL;

  the_api = _Objects_Get_API( id );
  return _Objects_Information_table[ the_api ][ the_class ];
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
  _Objects_Set_local_object( information, index, the_object );

  if ( information->is_string ) 
    /* _Objects_Copy_name_string( name, the_object->name ); */
    the_object->name = name;
  else
    /* _Objects_Copy_name_raw( name, the_object->name, information->name_length ); */
    the_object->name = name;
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
  _Objects_Set_local_object( information, index, NULL );
  /* _Objects_Clear_name( the_object->name, information->name_length ); */
  the_object->name = 0;
}

/*PAGE
 *
 *  _Objects_Namespace_remove
 *
 *  DESCRIPTION:
 *
 *  This function removes the_object from the namespace.
 */

RTEMS_INLINE_ROUTINE void _Objects_Namespace_remove(
  Objects_Information  *information,
  Objects_Control      *the_object
)
{
  /* _Objects_Clear_name( the_object->name, information->name_length ); */
  the_object->name = 0;
}

#endif
/* end of include file */
