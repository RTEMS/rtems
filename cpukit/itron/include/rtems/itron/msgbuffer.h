/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __RTEMS_ITRON_MESSAGEBUFFER_h_
#define __RTEMS_ITRON_MESSAGEBUFFER_h_

#ifdef __cplusplus
extern "C" {
#endif
    
#include <rtems/itron/object.h>
#include <rtems/score/coremsg.h>
    
/*
 *  The following defines the control block used to manage each 
 *  message buffer.
 */

typedef struct {
    ITRON_Objects_Control               Object;
    boolean                             is_priority_blocking;
    CORE_message_queue_Control          message_queue;
}   ITRON_Message_buffer_Control;

/*
 *  The following defines the information control block used to manage
 *  this class of objects.
 */

ITRON_EXTERN Objects_Information  _ITRON_Message_buffer_Information;

/*
 *  _ITRON_Message_buffer_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */

void _ITRON_Message_buffer_Manager_initialization(
  unsigned32 maximum_message_buffers
);

/*
 *  XXX insert private stuff here
 */

#include <rtems/itron/msgbuffer.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
