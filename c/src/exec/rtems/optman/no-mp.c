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
#include <rtems/core/cpu.h>
#include <rtems/core/interr.h>
#include <rtems/core/mpci.h>
#include <rtems/core/mppkt.h>
#include <rtems/core/states.h>
#include <rtems/core/thread.h>
#include <rtems/core/threadq.h>
#include <rtems/core/tqdata.h>
#include <rtems/core/watchdog.h>
#include <rtems/sysstate.h>

void _Multiprocessing_Manager_initialization ( void )
{
}

void rtems_multiprocessing_announce ( void )
{
}

void _MPCI_Handler_initialization( 
  MPCI_Control            *users_mpci_table
)
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

void _MPCI_Receive_server( void )
{
}

void _MPCI_Announce ( void )
{
}

/* end of file */
