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

#ifndef __ITRON_EVENTFLAGS_inl_
#define __ITRON_EVENTFLAGS_inl_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  _ITRON_Eventflags_Allocate
 *
 *  DESCRIPTION:
 *
 *  This routine allocates the eventflags associated with the specified
 *  eventflags ID from the pool of inactive eventflagss.
 *
 *  Input parameters:
 *    flgid   - id of eventflags to allocate
 *    status  - pointer to status variable
 *
 *  Output parameters:
 *    returns - pointer to the eventflags control block
 *    *status - status
 */

RTEMS_INLINE_ROUTINE ITRON_Eventflags_Control *_ITRON_Eventflags_Allocate(
  ID   flgid
)
{
  return (ITRON_Eventflags_Control *)_ITRON_Objects_Allocate_by_index(
    &_ITRON_Eventflags_Information,
    flgid,
    sizeof(ITRON_Eventflags_Control)
  );
}

/*
 *  _ITRON_Eventflags_Clarify_allocation_id_error
 *
 *  This function is invoked when an object allocation ID error
 *  occurs to determine the specific ITRON error code to return.
 */

#define _ITRON_Eventflags_Clarify_allocation_id_error( _id ) \
  _ITRON_Objects_Clarify_allocation_id_error( \
      &_ITRON_Eventflags_Information, (_id) )

/*
 *  _ITRON_Eventflags_Clarify_get_id_error
 *
 *  This function is invoked when an object get ID error
 *  occurs to determine the specific ITRON error code to return.
 */

#define _ITRON_Eventflags_Clarify_get_id_error( _id ) \
 _ITRON_Objects_Clarify_get_id_error( &_ITRON_Eventflags_Information, (_id) )

/*
 *  _ITRON_Eventflags_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a eventflags control block to the
 *  inactive chain of free eventflags control blocks.
 *
 *  Input parameters:
 *    the_eventflags - pointer to eventflags control block
 *
 *  Output parameters: NONE
 */

RTEMS_INLINE_ROUTINE void _ITRON_Eventflags_Free (
  ITRON_Eventflags_Control *the_eventflags
)
{
  _ITRON_Objects_Free( &_ITRON_Eventflags_Information, &the_eventflags->Object );
}

/*PAGE
 *
 *  _ITRON_Eventflags_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps eventflags IDs to eventflags control blocks.
 *  If ID corresponds to a local eventflags, then it returns
 *  the_eventflags control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the eventflags ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_eventflags is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_eventflags is undefined.
 *
 *  Input parameters:
 *    id            - ITRON eventflags ID.
 *    the_location  - pointer to a location variable
 *
 *  Output parameters:
 *    *the_location  - location of the object
 */

RTEMS_INLINE_ROUTINE ITRON_Eventflags_Control *_ITRON_Eventflags_Get (
  ID                 id,
  Objects_Locations *location
)
{
  return (ITRON_Eventflags_Control *)
    _ITRON_Objects_Get( &_ITRON_Eventflags_Information, id, location );
}

/*PAGE
 *
 *  _ITRON_Eventflags_Is_null
 *
 *  This function returns TRUE if the_eventflags is NULL and FALSE otherwise.
 *
 *  Input parameters:
 *    the_eventflags - pointer to eventflags control block
 *
 *  Output parameters:
 *    TRUE  - if the_eventflags is NULL
 *    FALSE - otherwise
 */

RTEMS_INLINE_ROUTINE boolean _ITRON_Eventflags_Is_null (
  ITRON_Eventflags_Control *the_eventflags
)
{
  return ( the_eventflags == NULL );
}

/*  
 *  XXX insert inline routines here
 */ 

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

