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

#ifndef __ITRON_SEMAPHORE_inl_
#define __ITRON_SEMAPHORE_inl_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  _ITRON_Semaphore_Allocate
 *
 *  DESCRIPTION:
 *
 *  This routine allocates the semaphore associated with the specified
 *  semaphore ID from the pool of inactive semaphores.
 *
 *  Input parameters:
 *    semid   - id of semaphore to allocate
 *    status  - pointer to status variable
 *
 *  Output parameters:
 *    returns - pointer to the semaphore control block
 *    *status - status
 */

RTEMS_INLINE_ROUTINE ITRON_Semaphore_Control *_ITRON_Semaphore_Allocate(
  ID   semid
)
{
  return (ITRON_Semaphore_Control *)_ITRON_Objects_Allocate_by_index(
    &_ITRON_Semaphore_Information,
    semid,
    sizeof(ITRON_Semaphore_Control)
  );
}

/*
 *  _ITRON_Semaphore_Clarify_allocation_id_error
 *
 *  This function is invoked when an object allocation ID error
 *  occurs to determine the specific ITRON error code to return.
 */

#define _ITRON_Semaphore_Clarify_allocation_id_error( _id ) \
  _ITRON_Objects_Clarify_allocation_id_error( \
      &_ITRON_Semaphore_Information, (_id) )

/*
 *  _ITRON_Semaphore_Clarify_get_id_error
 *
 *  This function is invoked when an object get ID error
 *  occurs to determine the specific ITRON error code to return.
 */

#define _ITRON_Semaphore_Clarify_get_id_error( _id ) \
 _ITRON_Objects_Clarify_get_id_error( &_ITRON_Semaphore_Information, (_id) )

/*
 *  _ITRON_Semaphore_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a semaphore control block to the
 *  inactive chain of free semaphore control blocks.
 *
 *  Input parameters:
 *    the_semaphore - pointer to semaphore control block
 *
 *  Output parameters: NONE
 */

RTEMS_INLINE_ROUTINE void _ITRON_Semaphore_Free (
  ITRON_Semaphore_Control *the_semaphore
)
{
  _ITRON_Objects_Free( &_ITRON_Semaphore_Information, &the_semaphore->Object );
}

/*PAGE
 *
 *  _ITRON_Semaphore_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps semaphore IDs to semaphore control blocks.
 *  If ID corresponds to a local semaphore, then it returns
 *  the_semaphore control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the semaphore ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_semaphore is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_semaphore is undefined.
 *
 *  Input parameters:
 *    id            - ITRON semaphore ID.
 *    the_location  - pointer to a location variable
 *
 *  Output parameters:
 *    *the_location  - location of the object
 */

RTEMS_INLINE_ROUTINE ITRON_Semaphore_Control *_ITRON_Semaphore_Get (
  ID                 id,
  Objects_Locations *location
)
{
  return (ITRON_Semaphore_Control *)
    _ITRON_Objects_Get( &_ITRON_Semaphore_Information, id, location );
}

/*PAGE
 *
 *  _ITRON_Semaphore_Is_null
 *
 *  This function returns TRUE if the_semaphore is NULL and FALSE otherwise.
 *
 *  Input parameters:
 *    the_semaphore - pointer to semaphore control block
 *
 *  Output parameters:
 *    TRUE  - if the_semaphore is NULL
 *    FALSE - otherwise
 */

RTEMS_INLINE_ROUTINE boolean _ITRON_Semaphore_Is_null (
  ITRON_Semaphore_Control *the_semaphore
)
{
  return ( the_semaphore == NULL );
}

/*
 *  _ITRON_Semaphore_Translate_core_semaphore_return_code
 *
 *  This function returns a ITRON status code based on the semaphore
 *  status code specified.
 *
 *  Input parameters:
 *    the_semaphore_status - semaphore status code to translate
 *
 *  Output parameters:
 *    ITRON status code - translated ITRON status code
 *
 */

RTEMS_INLINE_ROUTINE ER  _ITRON_Semaphore_Translate_core_semaphore_return_code (
  unsigned32 the_semaphore_status
)
{
/* XXX need to be able to return "E_RLWAI" */
  switch ( the_semaphore_status ) {
    case  CORE_SEMAPHORE_STATUS_SUCCESSFUL:
      return E_OK;
    case CORE_SEMAPHORE_STATUS_UNSATISFIED_NOWAIT:
      return E_TMOUT;
    case CORE_SEMAPHORE_WAS_DELETED:
      return E_DLT;
    case CORE_SEMAPHORE_TIMEOUT:
      return E_TMOUT;
    case CORE_SEMAPHORE_MAXIMUM_COUNT_EXCEEDED:
      return E_QOVR;
    case THREAD_STATUS_PROXY_BLOCKING:
      return THREAD_STATUS_PROXY_BLOCKING;
  }
  return E_OK;   /* unreached - only to remove warnings */
}

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

