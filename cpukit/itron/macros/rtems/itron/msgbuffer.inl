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

#ifndef __ITRON_MESSAGE_BUFFER_inl_
#define __ITRON_MESSAGE_BUFFER_inl_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  _ITRON_Message_buffer_Allocate
 */

#define _ITRON_Message_buffer_Allocate( _mbfid ) \
  (ITRON_Message_buffer_Control *)_ITRON_Objects_Allocate_by_index( \
    &_ITRON_Message_buffer_Information, \
    (_mbfid), \
    sizeof(ITRON_Message_buffer_Control) \
  )

/*
 *  _ITRON_Message_buffer_Clarify_allocation_id_error
 */

#define _ITRON_Message_buffer_Clarify_allocation_id_error( _id ) \
  _ITRON_Objects_Clarify_allocation_id_error( \
      &_ITRON_Message_buffer_Information, (_id) )

/*
 *  _ITRON_Message_buffer_Clarify_get_id_error
 */

#define _ITRON_Message_buffer_Clarify_get_id_error( _id ) \
 _ITRON_Objects_Clarify_get_id_error( &_ITRON_Message_buffer_Information, (_id) )

/*
 *  _ITRON_Message_buffer_Free
 */

#define _ITRON_Message_buffer_Free( _the_message_buffer ) \
  _ITRON_Objects_Free( \
    &_ITRON_Message_buffer_Information, \
    &(_the_message_buffer)->Object \
  )

/*PAGE
 *
 *  _ITRON_Message_buffer_Get
 */

#define _ITRON_Message_buffer_Get( _id, _location ) \
  (ITRON_Message_buffer_Control *) \
    _ITRON_Objects_Get( &_ITRON_Message_buffer_Information, (_id), (_location) )

/*PAGE
 *
 *  _ITRON_Message_buffer_Is_null
 */

#define _ITRON_Message_buffer_Is_null( _the_message_buffer ) \
  ( (_the_message_buffer) == NULL )

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

