/*
 *  Event Manager
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

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/event.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/tasks.h>

/*PAGE
 *
 *  _Event_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */
 
void _Event_Manager_initialization( void )
{
  _Event_Sync_state = EVENT_SYNC_SYNCHRONIZED;
 
  /*
   *  Register the MP Process Packet routine.
   */
 
#if defined(RTEMS_MULTIPROCESSING)
  _MPCI_Register_packet_processor( MP_PACKET_EVENT, _Event_MP_Process_packet );
#endif
}
