/*
 *  Multiprocessing Manager
 *
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
#if !defined(RTEMS_MULTIPROCESSING)
char rtems_no_multiprocessing;
#else
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
#include <rtems/score/interr.h>

void _Multiprocessing_Manager_initialization ( void )
{
}

void rtems_multiprocessing_announce ( void )
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
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
  MP_packet_Classes      the_class,
  MPCI_Packet_processor  the_packet_processor
 
)
{
}
 
MP_packet_Prefix *_MPCI_Get_packet ( void )
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return NULL;
}

void _MPCI_Return_packet (
  MP_packet_Prefix   *the_packet
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
}

void _MPCI_Send_process_packet (
  unsigned32          destination,
  MP_packet_Prefix   *the_packet
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
}

unsigned32 _MPCI_Send_request_packet (
  unsigned32          destination,
  MP_packet_Prefix   *the_packet,
  States_Control      extra_state
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return 0;
}

void _MPCI_Send_response_packet (
  unsigned32          destination,
  MP_packet_Prefix   *the_packet
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
}

MP_packet_Prefix  *_MPCI_Receive_packet ( void )
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return NULL;
}

Thread_Control *_MPCI_Process_response (
  MP_packet_Prefix  *the_packet
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return NULL;
}

Thread _MPCI_Receive_server(
  unsigned32 ignore
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return NULL;
}

void _MPCI_Announce ( void )
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
}


void _MPCI_Internal_packets_Send_process_packet (
   MPCI_Internal_Remote_operations operation
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
}
 
void _MPCI_Internal_packets_Process_packet (
  MP_packet_Prefix  *the_packet_prefix
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
}
 
MPCI_Internal_packet *_MPCI_Internal_packets_Get_packet ( void )
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return NULL;
}
#endif
/* end of file */
