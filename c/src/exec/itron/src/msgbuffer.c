/*
 *  ITRON Message Buffer Manager
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <itron.h>

#include <rtems/itron/msgbuffer.h>
#include <rtems/itron/task.h>

/*    
 *  _ITRON_Message_buffer_Manager_initialization
 *  
 *  This routine initializes all message buffer manager related data
 *  structures. 
 *
 *  Input parameters:
 *    maximum_message_buffers - maximum configured message buffers
 *
 *  Output parameters:  NONE
 */

void _ITRON_Message_buffer_Manager_initialization(
  unsigned32 maximum_message_buffers
) 
{
  _Objects_Initialize_information(
    &_ITRON_Message_buffer_Information, /* object information table */
    OBJECTS_ITRON_MESSAGE_BUFFERS,      /* object class */
    FALSE,                              /* TRUE if this is a
                                           global object class */ 
    maximum_message_buffers,            /* maximum objects of this class */
    sizeof( ITRON_Message_buffer_Control ),  /* size of this
                                                object's control
                                                block */ 
    FALSE,                         /* TRUE if names for this
                                      object are strings */ 
    ITRON_MAXIMUM_NAME_LENGTH,     /* maximum length of each
                                      object's name */ 
    FALSE                          /* TRUE if this class is threads */
  );
  
  /*
   *  Register the MP Process Packet routine.
   *
   *  NOTE: No MP Support YET in RTEMS ITRON implementation.
   */
} 

