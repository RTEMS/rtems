/**
 * @file
 *
 * @brief Signal Manager Initialization
 * @ingroup ClassicSignal Signals
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/signalimpl.h>

/*
 *  _Signal_Manager_initialization
 *
 *  This routine initializes all signal manager related data structures.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 */

void _Signal_Manager_initialization( void )
{
  /*
   *  Register the MP Process Packet routine.
   */

#if defined(RTEMS_MULTIPROCESSING)
  _MPCI_Register_packet_processor(
    MP_PACKET_SIGNAL,
    _Signal_MP_Process_packet
  );
#endif
}
