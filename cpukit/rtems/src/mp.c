/*
 *  Multiprocessing Manager
 *
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/cpu.h>
#include <rtems/event.h>
#include <rtems/fatal.h>
#include <rtems/intthrd.h>
#include <rtems/message.h>
#include <rtems/mp.h>
#include <rtems/mpci.h>
#include <rtems/mppkt.h>
#include <rtems/part.h>
#include <rtems/sem.h>
#include <rtems/signal.h>
#include <rtems/states.h>
#include <rtems/tasks.h>
#include <rtems/thread.h>
#include <rtems/threadq.h>
#include <rtems/watchdog.h>

/*PAGE
 *
 *  _Multiprocessing_Manager_initialization
 *
 */

void _Multiprocessing_Manager_initialization ( void )
{
  if ( _Configuration_MP_table->node < 1 ||
       _Configuration_MP_table->node > _Configuration_MP_table->maximum_nodes )
     rtems_fatal_error_occurred( RTEMS_INVALID_NODE );

  _Internal_threads_Set_MP_receive_server( _Multiprocessing_Receive_server );
}

/*PAGE
 *
 *  rtems_multiprocessing_announce
 *
 */

void rtems_multiprocessing_announce ( void )
{
  _Thread_MP_Ready();
}

/*PAGE
 *
 *  _Multiprocessing_Receive_server
 *
 */

Thread _Multiprocessing_Receive_server (
  Thread_Argument ignored
)
{

  rtems_packet_prefix *the_packet;

  _Thread_Dispatch_disable_level = 1;

  for ( ; ; ) {

    _Internal_threads_System_initialization_thread->Notepads[ 0 ] = 1;

    the_packet = _MPCI_Receive_packet();

    if ( ! the_packet ) {
      _Thread_MP_Block();
      _Thread_Dispatch_disable_level = 1;
    }
    else {

      _Thread_Executing->receive_packet = the_packet;

      switch ( the_packet->the_class ) {

        case RTEMS_MP_PACKET_INTERNAL_THREADS:
          _Internal_threads_MP_Process_packet( the_packet );
          break;

        case RTEMS_MP_PACKET_TASKS:
          _RTEMS_tasks_MP_Process_packet( the_packet );
          break;

        case RTEMS_MP_PACKET_MESSAGE_QUEUE:
          _Message_queue_MP_Process_packet( the_packet );
          break;

        case RTEMS_MP_PACKET_SEMAPHORE:
          _Semaphore_MP_Process_packet( the_packet );
          break;

        case RTEMS_MP_PACKET_PARTITION:
          _Partition_MP_Process_packet( the_packet );
          break;

        case RTEMS_MP_PACKET_REGION:
          /* Global regions are unsupported at this time */
          break;

        case RTEMS_MP_PACKET_EVENT:
          _Event_MP_Process_packet( the_packet );
          break;

        case RTEMS_MP_PACKET_SIGNAL:
          _Signal_MP_Process_packet( the_packet );
          break;
      }
    }
  }
}

/* end of file */
