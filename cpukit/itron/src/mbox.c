/*
 *  ITRON 3.0 Mailbox Manager
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <itron.h>

#include <rtems/itron/mbox.h>
#include <rtems/itron/task.h>

/*    
 *  _ITRON_Mailbox_Manager_initialization
 *  
 *  This routine initializes all mailboxes manager related data structures.
 *
 *  Input parameters:
 *    maximum_mailboxes - maximum configured mailboxes
 *
 *  Output parameters:  NONE
 */

void _ITRON_Mailbox_Manager_initialization(
  unsigned32 maximum_mailboxes
) 
{
  _Objects_Initialize_information(
    &_ITRON_Mailbox_Information, /* object information table */
    OBJECTS_ITRON_API,           /* object API */
    OBJECTS_ITRON_MAILBOXES,     /* object class */
    maximum_mailboxes,           /* maximum objects of this class */
    sizeof( ITRON_Mailbox_Control ), /* size of this object's control block */
    FALSE,                       /* TRUE if names for this object are strings */
    ITRON_MAXIMUM_NAME_LENGTH    /* maximum length of each object's name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    FALSE,                       /* TRUE if this is a global object class */
    NULL                         /* Proxy extraction support callout */
#endif
  );
    
  /*
   *  Register the MP Process Packet routine.
   *
   *  NOTE: No MP Support YET in RTEMS ITRON implementation.
   */
 
} 
