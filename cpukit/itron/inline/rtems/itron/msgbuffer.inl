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
 *
 *  DESCRIPTION:
 *
 *  This routine allocates the message buffer associated with the specified
 *  message buffer ID from the pool of inactive message buffers.
 *
 *  Input parameters:
 *    mbfid   - id of message buffer to allocate
 *    status  - pointer to status variable
 *
 *  Output parameters:
 *    returns - pointer to the message buffer control block
 *    *status - status
 */

RTEMS_INLINE_ROUTINE ITRON_Message_buffer_Control
 *_ITRON_Message_buffer_Allocate(
  ID   mbfid
)
{
  return (ITRON_Message_buffer_Control *)_ITRON_Objects_Allocate_by_index(
    &_ITRON_Message_buffer_Information,
    mbfid,
    sizeof(ITRON_Message_buffer_Control)
  );
}

/*
 *  _ITRON_Message_buffer_Clarify_allocation_id_error
 *
 *  This function is invoked when an object allocation ID error
 *  occurs to determine the specific ITRON error code to return.
 */

#define _ITRON_Message_buffer_Clarify_allocation_id_error( _id ) \
  _ITRON_Objects_Clarify_allocation_id_error( \
      &_ITRON_Message_buffer_Information, (_id) )

/*
 *  _ITRON_Message_buffer_Clarify_get_id_error
 *
 *  This function is invoked when an object get ID error
 *  occurs to determine the specific ITRON error code to return.
 */

#define _ITRON_Message_buffer_Clarify_get_id_error( _id ) \
 _ITRON_Objects_Clarify_get_id_error( &_ITRON_Message_buffer_Information, (_id) )

/*
 *  _ITRON_Message_buffer_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a message buffer control block to the
 *  inactive chain of free message buffer control blocks.
 *
 *  Input parameters:
 *    the_message_buffer - pointer to message_buffer control block
 *
 *  Output parameters: NONE
 */

RTEMS_INLINE_ROUTINE void _ITRON_Message_buffer_Free (
  ITRON_Message_buffer_Control *the_message_buffer
)
{
  _ITRON_Objects_Free(
    &_ITRON_Message_buffer_Information,
    &the_message_buffer->Object
  );
}

/*PAGE
 *
 *  _ITRON_Message_buffer_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps message buffer IDs to message buffer control blocks.
 *  If ID corresponds to a local message buffer, then it returns
 *  the message buffer control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the message buffer ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the message buffer is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the message buffer is undefined.
 *
 *  Input parameters:
 *    id            - ITRON message_buffer ID.
 *    the_location  - pointer to a location variable
 *
 *  Output parameters:
 *    *the_location  - location of the object
 */

RTEMS_INLINE_ROUTINE ITRON_Message_buffer_Control *_ITRON_Message_buffer_Get (
  ID                 id,
  Objects_Locations *location
)
{
  return (ITRON_Message_buffer_Control *)
    _ITRON_Objects_Get( &_ITRON_Message_buffer_Information, id, location );
}

/*PAGE
 *
 *  _ITRON_Message_buffer_Is_null
 *
 *  This function returns TRUE if the_message_buffer is NULL
 *  and FALSE otherwise.
 *
 *  Input parameters:
 *    the_message_buffer - pointer to message buffer control block
 *
 *  Output parameters:
 *    TRUE  - if the_message_buffer is NULL
 *    FALSE - otherwise
 */

RTEMS_INLINE_ROUTINE boolean _ITRON_Message_buffer_Is_null (
  ITRON_Message_buffer_Control *the_message_buffer
)
{
  return ( the_message_buffer == NULL );
}

/*  
 *  XXX insert inline routines here
 */ 

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

