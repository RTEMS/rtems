/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __ITRON_VARIABLE_MEMORY_POOL_inl_
#define __ITRON_VARIABLE_MEMORY_POOL_inl_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  _ITRON_Variable_memory_pool_Allocate
 *
 *  DESCRIPTION:
 *
 *  This routine allocates the variable memory pool associated with
 *  the specified variable memory pool ID from the pool of inactive
 *  variable memory pools.
 *
 *  Input parameters:
 *    mplid   - id of variable memory pool to allocate
 *    status  - pointer to status variable
 *
 *  Output parameters:
 *    returns - pointer to the variable memory pool control block
 *    *status - status
 */

RTEMS_INLINE_ROUTINE ITRON_Variable_memory_pool_Control
  *_ITRON_Variable_memory_pool_Allocate(
  ID   mplid
)
{
  return (ITRON_Variable_memory_pool_Control *)_ITRON_Objects_Allocate_by_index(
    &_ITRON_Variable_memory_pool_Information,
    mplid,
    sizeof(ITRON_Variable_memory_pool_Control)
  );
}

/*
 *  _ITRON_Variable_memory_pool_Clarify_allocation_id_error
 *
 *  This function is invoked when an object allocation ID error
 *  occurs to determine the specific ITRON error code to return.
 */

#define _ITRON_Variable_memory_pool_Clarify_allocation_id_error( _id ) \
  _ITRON_Objects_Clarify_allocation_id_error( \
      &_ITRON_Variable_memory_pool_Information, (_id) )

/*
 *  _ITRON_Variable_memory_pool_Clarify_get_id_error
 *
 *  This function is invoked when an object get ID error
 *  occurs to determine the specific ITRON error code to return.
 */

#define _ITRON_Variable_memory_pool_Clarify_get_id_error( _id ) \
 _ITRON_Objects_Clarify_get_id_error( &_ITRON_Variable_memory_pool_Information, (_id) )

/*
 *  _ITRON_Variable_memory_pool_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a variable memory pool control block to the
 *  inactive chain of free variable memory pool control blocks.
 *
 *  Input parameters:
 *    the_variable_memory_pool - pointer to variable memory pool control block
 *
 *  Output parameters: NONE
 */

RTEMS_INLINE_ROUTINE void _ITRON_Variable_memory_pool_Free (
  ITRON_Variable_memory_pool_Control *the_variable_memory_pool
)
{
  _ITRON_Objects_Free(
    &_ITRON_Variable_memory_pool_Information,
    &the_variable_memory_pool->Object
  );
}

/*PAGE
 *
 *  _ITRON_Variable_memory_pool_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps variable memory pool IDs to variable memory pool
 *  control blocks.  If ID corresponds to a local variable memory pool,
 *  then it returns the variable memory pool control pointer which maps
 *  to ID and location is set to OBJECTS_LOCAL.  if the variable memory
 *  pool ID is global and resides on a remote node, then location is set
 *  to OBJECTS_REMOTE, and the variable memory pool is undefined.  Otherwise,
 *  location is set to OBJECTS ERROR and the variable memory pool is undefined.
 *
 *  Input parameters:
 *    id            - ITRON variable memory pool ID.
 *    the_location  - pointer to a location variable
 *
 *  Output parameters:
 *    *the_location  - location of the object
 */

RTEMS_INLINE_ROUTINE ITRON_Variable_memory_pool_Control
  *_ITRON_Variable_memory_pool_Get (
  ID                 id,
  Objects_Locations *location
)
{
  return (ITRON_Variable_memory_pool_Control *) _ITRON_Objects_Get(
    &_ITRON_Variable_memory_pool_Information,
    id,
    location
  );
}

/*PAGE
 *
 *  _ITRON_Variable_memory_pool_Is_null
 *
 *  This function returns TRUE if the_variable_memory_pool is NULL
 *  and FALSE otherwise.
 *
 *  Input parameters:
 *    the_variable_memory_pool - pointer to variable memory pool control block
 *
 *  Output parameters:
 *    TRUE  - if the_variable_memory_pool is NULL
 *    FALSE - otherwise
 */

RTEMS_INLINE_ROUTINE boolean _ITRON_Variable_memory_pool_Is_null (
  ITRON_Variable_memory_pool_Control *the_variable_memory_pool
)
{
  return ( the_variable_memory_pool == NULL );
}

/*  
 *  XXX insert inline routines here
 */ 

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

