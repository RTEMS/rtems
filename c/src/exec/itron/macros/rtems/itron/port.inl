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

#ifndef __ITRON_PORT_inl_
#define __ITRON_PORT_inl_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  _ITRON_Port_Allocate
 */

#define _ITRON_Port_Allocate( _porid ) \
  (ITRON_Port_Control *)_ITRON_Objects_Allocate_by_index( \
    &_ITRON_Port_Information, \
    (_porid), \
    sizeof(ITRON_Port_Control) \
  )

/*
 *  _ITRON_Port_Clarify_allocation_id_error
 */

#define _ITRON_Port_Clarify_allocation_id_error( _id ) \
  _ITRON_Objects_Clarify_allocation_id_error( \
      &_ITRON_Port_Information, (_id) )

/*
 *  _ITRON_Port_Clarify_get_id_error
 */

#define _ITRON_Port_Clarify_get_id_error( _id ) \
 _ITRON_Objects_Clarify_get_id_error( &_ITRON_Port_Information, (_id) )

/*
 *  _ITRON_Port_Free
 */

#define _ITRON_Port_Free( _the_port ) \
  _ITRON_Objects_Free( &_ITRON_Port_Information, &(_the_port)->Object )

/*PAGE
 *
 *  _ITRON_Port_Get
 */

#define _ITRON_Port_Get( _id, _location ) \
  (ITRON_Port_Control *) \
    _ITRON_Objects_Get( &_ITRON_Port_Information, (_id), (_location) )

/*PAGE
 *
 *  _ITRON_Port_Is_null
 */

#define _ITRON_Port_Is_null( _the_port ) \
  ( (_the_port) == NULL )

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

