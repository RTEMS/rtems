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
 */

#define _ITRON_Semaphore_Allocate( _semid ) \
  (ITRON_Semaphore_Control *)_ITRON_Objects_Allocate_by_index( \
    &_ITRON_Semaphore_Information, \
    (_semid), \
    sizeof(ITRON_Semaphore_Control) \
  )

/*
 *  _ITRON_Semaphore_Clarify_allocation_id_error
 */

#define _ITRON_Semaphore_Clarify_allocation_id_error( _id ) \
  _ITRON_Objects_Clarify_allocation_id_error( \
      &_ITRON_Semaphore_Information, (_id) )

/*
 *  _ITRON_Semaphore_Clarify_get_id_error
 */

#define _ITRON_Semaphore_Clarify_get_id_error( _id ) \
 _ITRON_Objects_Clarify_get_id_error( &_ITRON_Semaphore_Information, (_id) )

/*
 *  _ITRON_Semaphore_Free
 */

#define _ITRON_Semaphore_Free( _the_semaphore ) \
 _ITRON_Objects_Free( &_ITRON_Semaphore_Information, &(_the_semaphore)->Object )

/*PAGE
 *
 *  _ITRON_Semaphore_Get
 */

#define _ITRON_Semaphore_Get( _id, _location ) \
  (ITRON_Semaphore_Control *) \
    _ITRON_Objects_Get( &_ITRON_Semaphore_Information, (_id), (_location) )

/*PAGE
 *
 *  _ITRON_Semaphore_Is_null
 */

#define _ITRON_Semaphore_Is_null( _the_semaphore ) \
  ( (_the_semaphore) == NULL )

/*
 *  _ITRON_Semaphore_Translate_core_semaphore_return_code
 */

/* XXX fix me */
static  ER _ITRON_Semaphore_Translate_core_semaphore_return_code (
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

