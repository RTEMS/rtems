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
 *
 *  DESCRIPTION:
 *
 *  This routine allocates the mailbox associated with the specified
 *  mailbox ID from the pool of inactive mailboxs.
 *
 *  Input parameters:
 *    mbxid   - id of mailbox to allocate
 *    status  - pointer to status variable
 *
 *  Output parameters:
 *    returns - pointer to the mailbox control block
 *    *status - status
 */

RTEMS_INLINE_ROUTINE ITRON_Mailbox_Control *_ITRON_Mailbox_Allocate(
  ID   mbxid
)
{
  return (ITRON_Mailbox_Control *)_ITRON_Objects_Allocate_by_index(
    &_ITRON_Mailbox_Information,
    mbxid,
    sizeof(ITRON_Mailbox_Control)
  );
}

/*
 *  _ITRON_Mailbox_Clarify_allocation_id_error
 *
 *  This function is invoked when an object allocation ID error
 *  occurs to determine the specific ITRON error code to return.
 */

#define _ITRON_Mailbox_Clarify_allocation_id_error( _id ) \
  _ITRON_Objects_Clarify_allocation_id_error( \
      &_ITRON_Mailbox_Information, (_id) )

/*
 *  _ITRON_Mailbox_Clarify_get_id_error
 *
 *  This function is invoked when an object get ID error
 *  occurs to determine the specific ITRON error code to return.
 */

#define _ITRON_Mailbox_Clarify_get_id_error( _id ) \
 _ITRON_Objects_Clarify_get_id_error( &_ITRON_Mailbox_Information, (_id) )

/*
 *  _ITRON_Mailbox_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a mailbox control block to the
 *  inactive chain of free mailbox control blocks.
 *
 *  Input parameters:
 *    the_mailbox - pointer to mailbox control block
 *
 *  Output parameters: NONE
 */

RTEMS_INLINE_ROUTINE void _ITRON_Mailbox_Free (
  ITRON_Mailbox_Control *the_mailbox
)
{
  _ITRON_Objects_Free( &_ITRON_Mailbox_Information, &the_mailbox->Object );
}

/*PAGE
 *
 *  _ITRON_Mailbox_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps mailbox IDs to mailbox control blocks.
 *  If ID corresponds to a local mailbox, then it returns
 *  the_mailbox control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the mailbox ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_mailbox is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_mailbox is undefined.
 *
 *  Input parameters:
 *    id            - ITRON mailbox ID.
 *    the_location  - pointer to a location variable
 *
 *  Output parameters:
 *    *the_location  - location of the object
 */

RTEMS_INLINE_ROUTINE ITRON_Mailbox_Control *_ITRON_Mailbox_Get (
  ID                 id,
  Objects_Locations *location
)
{
  return (ITRON_Mailbox_Control *)
    _ITRON_Objects_Get( &_ITRON_Mailbox_Information, id, location );
}

/*PAGE
 *
 *  _ITRON_Mailbox_Is_null
 *
 *  This function returns TRUE if the_mailbox is NULL and FALSE otherwise.
 *
 *  Input parameters:
 *    the_mailbox - pointer to mailbox control block
 *
 *  Output parameters:
 *    TRUE  - if the_mailbox is NULL
 *    FALSE - otherwise
 */

RTEMS_INLINE_ROUTINE boolean _ITRON_Mailbox_Is_null (
  ITRON_Mailbox_Control *the_mailbox
)
{
  return ( the_mailbox == NULL );
}

/*  
 *  XXX insert inline routines here
 */ 

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

