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

#ifndef __ITRON_MAILBOX_inl_
#define __ITRON_MAILBOX_inl_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  _ITRON_Mailbox_Allocate
 */

#define _ITRON_Mailbox_Allocate( _mbxid ) \
  (ITRON_Mailbox_Control *)_ITRON_Objects_Allocate_by_index( \
    &_ITRON_Mailbox_Information, \
    (_mbxid), \
    sizeof(ITRON_Mailbox_Control) \
  )

/*
 *  _ITRON_Mailbox_Clarify_allocation_id_error
 */

#define _ITRON_Mailbox_Clarify_allocation_id_error( _id ) \
  _ITRON_Objects_Clarify_allocation_id_error( \
      &_ITRON_Mailbox_Information, (_id) )

/*
 *  _ITRON_Mailbox_Clarify_get_id_error
 */

#define _ITRON_Mailbox_Clarify_get_id_error( _id ) \
 _ITRON_Objects_Clarify_get_id_error( &_ITRON_Mailbox_Information, (_id) )

/*
 *  _ITRON_Mailbox_Free
 */

#define _ITRON_Mailbox_Free( _the_mailbox ) \
  _ITRON_Objects_Free( &_ITRON_Mailbox_Information, &(_the_mailbox)->Object )

/*PAGE
 *
 *  _ITRON_Mailbox_Get
 */

#define _ITRON_Mailbox_Get( _id, _location ) \
  (ITRON_Mailbox_Control *) \
    _ITRON_Objects_Get( &_ITRON_Mailbox_Information, (_id), (_location) )

/*PAGE
 *
 *  _ITRON_Mailbox_Is_null
 */

#define _ITRON_Mailbox_Is_null( _the_mailbox ) \
  ( (_the_mailbox) == NULL )

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

