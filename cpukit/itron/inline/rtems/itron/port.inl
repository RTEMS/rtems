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

#ifndef __ITRON_PORT_inl_
#define __ITRON_PORT_inl_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  _ITRON_Port_Allocate
 *
 *  DESCRIPTION:
 *
 *  This routine allocates the port associated with the specified
 *  port ID from the pool of inactive ports.
 *
 *  Input parameters:
 *    porid   - id of port to allocate
 *    status  - pointer to status variable
 *
 *  Output parameters:
 *    returns - pointer to the port control block
 *    *status - status
 */

RTEMS_INLINE_ROUTINE ITRON_Port_Control *_ITRON_Port_Allocate(
  ID   porid
)
{
  return (ITRON_Port_Control *)_ITRON_Objects_Allocate_by_index(
    &_ITRON_Port_Information,
    porid,
    sizeof(ITRON_Port_Control)
  );
}

/*
 *  _ITRON_Port_Clarify_allocation_id_error
 *
 *  This function is invoked when an object allocation ID error
 *  occurs to determine the specific ITRON error code to return.
 */

#define _ITRON_Port_Clarify_allocation_id_error( _id ) \
  _ITRON_Objects_Clarify_allocation_id_error( \
      &_ITRON_Port_Information, (_id) )

/*
 *  _ITRON_Port_Clarify_get_id_error
 *
 *  This function is invoked when an object get ID error
 *  occurs to determine the specific ITRON error code to return.
 */

#define _ITRON_Port_Clarify_get_id_error( _id ) \
 _ITRON_Objects_Clarify_get_id_error( &_ITRON_Port_Information, (_id) )

/*
 *  _ITRON_Port_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a port control block to the
 *  inactive chain of free port control blocks.
 *
 *  Input parameters:
 *    the_port - pointer to port control block
 *
 *  Output parameters: NONE
 */

RTEMS_INLINE_ROUTINE void _ITRON_Port_Free (
  ITRON_Port_Control *the_port
)
{
  _ITRON_Objects_Free( &_ITRON_Port_Information, &the_port->Object );
}

/*PAGE
 *
 *  _ITRON_Port_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps port IDs to port control blocks.
 *  If ID corresponds to a local port, then it returns
 *  the_port control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the port ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_port is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_port is undefined.
 *
 *  Input parameters:
 *    id            - ITRON port ID.
 *    the_location  - pointer to a location variable
 *
 *  Output parameters:
 *    *the_location  - location of the object
 */

RTEMS_INLINE_ROUTINE ITRON_Port_Control *_ITRON_Port_Get (
  ID                 id,
  Objects_Locations *location
)
{
  return (ITRON_Port_Control *)
    _ITRON_Objects_Get( &_ITRON_Port_Information, id, location );
}

/*PAGE
 *
 *  _ITRON_Port_Is_null
 *
 *  This function returns TRUE if the_port is NULL and FALSE otherwise.
 *
 *  Input parameters:
 *    the_port - pointer to port control block
 *
 *  Output parameters:
 *    TRUE  - if the_port is NULL
 *    FALSE - otherwise
 */

RTEMS_INLINE_ROUTINE boolean _ITRON_Port_Is_null (
  ITRON_Port_Control *the_port
)
{
  return ( the_port == NULL );
}

/*  
 *  XXX insert inline routines here
 */ 

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

