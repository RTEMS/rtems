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

void _Multiprocessing_Manager_initialization ( void )
{
}

void rtems_multiprocessing_announce ( void )
{
}

Thread _Multiprocessing_Receive_server (
  Thread_Argument ignored
)
{
}

void _MPCI_Handler_initialization ( void )
{
}

void _MPCI_Initialization ( void )
{
}

rtems_packet_prefix *_MPCI_Get_packet ( void )
{
  return NULL;
}

void _MPCI_Return_packet (
  rtems_packet_prefix   *the_packet
)
{
}

void _MPCI_Send_process_packet (
  unsigned32          destination,
  rtems_packet_prefix   *the_packet
)
{
}

rtems_status_code _MPCI_Send_request_packet (
  unsigned32          destination,
  rtems_packet_prefix   *the_packet,
  States_Control      extra_state
)
{
  return( RTEMS_NOT_CONFIGURED );
}

void _MPCI_Send_response_packet (
  unsigned32          destination,
  rtems_packet_prefix   *the_packet
)
{
}

rtems_packet_prefix  *_MPCI_Receive_packet ( void )
{
  return NULL;
}

Thread_Control *_MPCI_Process_response (
  rtems_packet_prefix  *the_packet
)
{
  return NULL;
}
