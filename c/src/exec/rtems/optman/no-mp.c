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
#include <rtems/rtems/status.h>
#include <rtems/rtems/mp.h>
#include <rtems/score/cpu.h>
#include <rtems/score/interr.h>
#include <rtems/score/mpci.h>
#include <rtems/score/mppkt.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tqdata.h>
#include <rtems/score/watchdog.h>
#include <rtems/score/sysstate.h>

void _Multiprocessing_Manager_initialization ( void )
{
}

void rtems_multiprocessing_announce ( void )
{
}

void _MPCI_Handler_initialization( 
  MPCI_Control            *users_mpci_table,
  unsigned32               timeout_status
)
{
}

void _MPCI_Create_server( void )
{
}

void _MPCI_Initialization ( void )
{
}

void _MPCI_Register_packet_processor(
  Objects_Classes        the_class,
  MPCI_Packet_processor  the_packet_processor
 
)
{
}
 
MP_packet_Prefix *_MPCI_Get_packet ( void )
{
  return NULL;
}

void _MPCI_Return_packet (
  MP_packet_Prefix   *the_packet
)
{
}

void _MPCI_Send_process_packet (
  unsigned32          destination,
  MP_packet_Prefix   *the_packet
)
{
}

unsigned32 _MPCI_Send_request_packet (
  unsigned32          destination,
  MP_packet_Prefix   *the_packet,
  States_Control      extra_state
)
{
  return 0;
}

void _MPCI_Send_response_packet (
  unsigned32          destination,
  MP_packet_Prefix   *the_packet
)
{
}

MP_packet_Prefix  *_MPCI_Receive_packet ( void )
{
  return NULL;
}

Thread_Control *_MPCI_Process_response (
  MP_packet_Prefix  *the_packet
)
{
  return NULL;
}

Thread _MPCI_Receive_server(
  unsigned32 ignore
)
{
}

void _MPCI_Announce ( void )
{
}


void _MPCI_Internal_packets_Send_process_packet (
   MPCI_Internal_Remote_operations operation
)
{
}
 
void _MPCI_Internal_packets_Process_packet (
  MP_packet_Prefix  *the_packet_prefix
)
{
}
 
MPCI_Internal_packet *_MPCI_Internal_packets_Get_packet ( void )
{
  return NULL;
}

/* end of file */
