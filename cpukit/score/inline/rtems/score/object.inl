/**
 * @file rtems/score/object.inl
 *
 * This include file contains the static inline implementation of all
 * of the inlined routines in the Object Handler.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_OBJECT_H
# error "Never use <rtems/score/object.inl> directly; include <rtems/score/object.h> instead."
#endif

#ifndef _RTEMS_SCORE_OBJECT_INL
#define _RTEMS_SCORE_OBJECT_INL

/**
 *  This function builds an object's id from the processor node and index
 *  values specified.
 *
 *  @param[in] the_api indicates the API associated with this Id.
 *  @param[in] the_class indicates the class of object.
 *             It is specific to @a the_api.
 *  @param[in] node is the node where this object resides.
 *  @param[in] index is the instance number of this object.
 *
 *  @return This method returns an object Id constructed from the arguments.
 */
RTEMS_INLINE_ROUTINE Objects_Id _Objects_Build_id(
  Objects_APIs     the_api,
  uint32_t         the_class,
  uint32_t         node,
  uint32_t         index
)
{
  return (( (Objects_Id) the_api )   << OBJECTS_API_START_BIT)   |
         (( (Objects_Id) the_class ) << OBJECTS_CLASS_START_BIT) |
         #if !defined(RTEMS_USE_16_BIT_OBJECT)
           (( (Objects_Id) node )    << OBJECTS_NODE_START_BIT)  |
         #endif
         (( (Objects_Id) index )     << OBJECTS_INDEX_START_BIT);
}

/**
 *  This function returns the API portion of the ID.
 *
 *  @param[in] id is the object Id to be processed.
 *
 *  @return This method returns an object Id constructed from the arguments.
 */
RTEMS_INLINE_ROUTINE Objects_APIs _Objects_Get_API(
  Objects_Id id
)
{
  return (Objects_APIs) ((id >> OBJECTS_API_START_BIT) & OBJECTS_API_VALID_BITS);
}

/**
 *  This function returns the class portion of the ID.
 *
 *  @param[in] id is the object Id to be processed
 */
RTEMS_INLINE_ROUTINE uint32_t _Objects_Get_class(
  Objects_Id id
)
{
  return (uint32_t) 
    ((id >> OBJECTS_CLASS_START_BIT) & OBJECTS_CLASS_VALID_BITS);
}
 
/**
 *  This function returns the node portion of the ID.
 *
 *  @param[in] id is the object Id to be processed
 *
 *  @return This method returns the node portion of an object ID.
 */
RTEMS_INLINE_ROUTINE uint32_t _Objects_Get_node(
  Objects_Id id
)
{
  /*
   * If using 16-bit Ids, then there is no node field and it MUST
   * be a single processor system.
   */
  #if defined(RTEMS_USE_16_BIT_OBJECT)
    return 1;
  #else
    return (id >> OBJECTS_NODE_START_BIT) & OBJECTS_NODE_VALID_BITS;
  #endif
}

/**
 *  This function returns the index portion of the ID.
 *
 *  @param[in] id is the Id to be processed
 *
 *  @return This method returns the class portion of the specified object ID.
 */
RTEMS_INLINE_ROUTINE Objects_Maximum _Objects_Get_index(
  Objects_Id id
)
{
  return
    (Objects_Maximum)((id >> OBJECTS_INDEX_START_BIT) &
                                          OBJECTS_INDEX_VALID_BITS);
}

/**
 *  This function returns true if the api is valid.
 *
 *  @param[in] the_api is the api portion of an object ID.
 *
 *  @return This method returns true if the specified api value is valid
 *          and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Objects_Is_api_valid(
  uint32_t   the_api
)
{
  if ( !the_api || the_api > OBJECTS_APIS_LAST )
   return false;
  return true;
}
   
/**
 *  This function returns true if the node is of the local object, and
 *  false otherwise.
 *
 *  @param[in] node is the node number and corresponds to the node number
 *         portion of an object ID.
 *
 *  @return This method returns true if the specified node is the local node
 *          and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Objects_Is_local_node(
  uint32_t   node
)
{
  return ( node == _Objects_Local_node );
}

/**
 *  This function returns true if the id is of a local object, and
 *  false otherwise.
 *
 *  @param[in] id is an object ID
 *
 *  @return This method returns true if the specified object Id is local
 *          and false otherwise.
 *
 *  @note On a single processor configuration, this always returns true.
 */
RTEMS_INLINE_ROUTINE bool _Objects_Is_local_id(
#if defined(RTEMS_MULTIPROCESSING)
  Objects_Id id
#else
  Objects_Id id __attribute__((unused))
#endif
)
{
#if defined(RTEMS_MULTIPROCESSING)
  return _Objects_Is_local_node( _Objects_Get_node(id) );
#else
  return true;
#endif
}

/**
 *  This function returns true if left and right are equal,
 *  and false otherwise.
 *
 *  @param[in] left is the Id on the left hand side of the comparison
 *  @param[in] right is the Id on the right hand side of the comparison
 *
 *  @return This method returns true if the specified object IDs are equal
 *          and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Objects_Are_ids_equal(
  Objects_Id left,
  Objects_Id right
)
{
  return ( left == right );
}

/**
 *  This function returns a pointer to the local_table object
 *  referenced by the index.
 *
 *  @param[in] information points to an Object Information Table
 *  @param[in] index is the index of the object the caller wants to access
 *
 *  @return This method returns a pointer to a local object or NULL if the
 *          index is invalid and RTEMS_DEBUG is enabled.
 */
RTEMS_INLINE_ROUTINE Objects_Control *_Objects_Get_local_object(
  Objects_Information *information,
  uint16_t             index
)
{
  /*
   *  This routine is ONLY to be called from places in the code
   *  where the Id is known to be good.  Therefore, this should NOT
   *  occur in normal situations.
   */ 
  #if defined(RTEMS_DEBUG)
    if ( index > information->maximum )
      return NULL;
  #endif
  return information->local_table[ index ];
}

/**
 *  This function sets the pointer to the local_table object
 *  referenced by the index.
 *
 *  @param[in] information points to an Object Information Table
 *  @param[in] index is the index of the object the caller wants to access
 *  @param[in] the_object is the local object pointer
 *
 *  @note This routine is ONLY to be called in places where the
 *        index portion of the Id is known to be good.  This is
 *        OK since it is normally called from object create/init
 *        or delete/destroy operations.
 */

RTEMS_INLINE_ROUTINE void _Objects_Set_local_object(
  Objects_Information *information,
  uint32_t             index,
  Objects_Control     *the_object
)
{
  /*
   *  This routine is ONLY to be called from places in the code
   *  where the Id is known to be good.  Therefore, this should NOT
   *  occur in normal situations.
   */ 
  #if defined(RTEMS_DEBUG)
    if ( index > information->maximum )
      return;
  #endif

  information->local_table[ index ] = the_object;
}

/**
 *  This function sets the pointer to the local_table object
 *  referenced by the index to a NULL so the object Id is invalid
 *  after this call.
 *
 *  @param[in] information points to an Object Information Table
 *  @param[in] the_object is the local object pointer
 *
 *  @note This routine is ONLY to be called in places where the
 *        index portion of the Id is known to be good.  This is
 *        OK since it is normally called from object create/init
 *        or delete/destroy operations.
 */

RTEMS_INLINE_ROUTINE void _Objects_Invalidate_Id(
  Objects_Information  *information,
  Objects_Control      *the_object
)
{
  _Objects_Set_local_object(
    information,
    _Objects_Get_index( the_object->id ),
    NULL
  );
}

/**
 *  This function places the_object control pointer and object name
 *  in the Local Pointer and Local Name Tables, respectively.
 *
 *  @param[in] information points to an Object Information Table
 *  @param[in] the_object is a pointer to an object
 *  @param[in] name is the name of the object to make accessible
 */
RTEMS_INLINE_ROUTINE void _Objects_Open(
  Objects_Information *information,
  Objects_Control     *the_object,
  Objects_Name         name
)
{
  _Objects_Set_local_object(
    information,
    _Objects_Get_index( the_object->id ),
    the_object
  );

  the_object->name = name;
}

/**
 *  This function places the_object control pointer and object name
 *  in the Local Pointer and Local Name Tables, respectively.
 *
 *  @param[in] information points to an Object Information Table
 *  @param[in] the_object is a pointer to an object
 *  @param[in] name is the name of the object to make accessible
 */
RTEMS_INLINE_ROUTINE void _Objects_Open_u32(
  Objects_Information *information,
  Objects_Control     *the_object,
  uint32_t             name
)
{
  _Objects_Set_local_object(
    information,
    _Objects_Get_index( the_object->id ),
    the_object
  );

  /* ASSERT: information->is_string == false */ 
  the_object->name.name_u32 = name;
}

/**
 *  This function places the_object control pointer and object name
 *  in the Local Pointer and Local Name Tables, respectively.
 *
 *  @param[in] information points to an Object Information Table
 *  @param[in] the_object is a pointer to an object
 *  @param[in] name is the name of the object to make accessible
 */
RTEMS_INLINE_ROUTINE void _Objects_Open_string(
  Objects_Information *information,
  Objects_Control     *the_object,
  const char          *name
)
{
  _Objects_Set_local_object(
    information,
    _Objects_Get_index( the_object->id ),
    the_object
  );

  #if defined(RTEMS_SCORE_OBJECT_ENABLE_STRING_NAMES)
    /* ASSERT: information->is_string */ 
    the_object->name.name_p = name;
  #endif
}

/**
 *  Returns if the object maximum specifies unlimited objects.
 *
 *  @param[in] maximum The object maximum specification.
 *
 *  @retval true Unlimited objects are available.
 *  @retval false The object count is fixed.
 */
RTEMS_INLINE_ROUTINE bool _Objects_Is_unlimited( uint32_t maximum )
{
  return (maximum & OBJECTS_UNLIMITED_OBJECTS) != 0;
}

/*
 * We cannot use an inline function for this since it may be evaluated at
 * compile time.
 */
#define _Objects_Maximum_per_allocation( maximum ) \
  ((Objects_Maximum) ((maximum) & ~OBJECTS_UNLIMITED_OBJECTS))

#endif
/* end of include file */
