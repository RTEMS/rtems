/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
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
 */

#define _ITRON_Eventflags_Allocate( _flgid ) \
  (ITRON_Eventflags_Control *)_ITRON_Objects_Allocate_by_index( \
    &_ITRON_Eventflags_Information,  \
    (_flgid), \
    sizeof(ITRON_Eventflags_Control) \
  )

/*
 *  _ITRON_Eventflags_Clarify_allocation_id_error
 */

#define _ITRON_Eventflags_Clarify_allocation_id_error( _id ) \
  _ITRON_Objects_Clarify_allocation_id_error( \
      &_ITRON_Eventflags_Information, (_id) )

/*
 *  _ITRON_Eventflags_Clarify_get_id_error
 */

#define _ITRON_Eventflags_Clarify_get_id_error( _id ) \
 _ITRON_Objects_Clarify_get_id_error( &_ITRON_Eventflags_Information, (_id) )

/*
 *  _ITRON_Eventflags_Free
 */

#define _ITRON_Eventflags_Free( _the_eventflags ) \
  _ITRON_Objects_Free( &_ITRON_Eventflags_Information, \
      &(_the_eventflags)->Object )


/*
 *  _ITRON_Eventflags_Get
 */

#define _ITRON_Eventflags_Get( _id, _location ) \
  (ITRON_Eventflags_Control *) \
    _ITRON_Objects_Get( &_ITRON_Eventflags_Information, (_id), (_location) )

/*PAGE
 *
 *  _ITRON_Eventflags_Is_null
 */

#define _ITRON_Eventflags_Is_null( _the_eventflags ) \
  ( (_the_eventflags) == NULL );

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

