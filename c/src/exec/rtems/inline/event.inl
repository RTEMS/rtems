/*  inline/event.inl
 *
 *  This include file contains the static inline implementation of
 *  macros for the Event Manager.
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

#ifndef __MACROS_EVENT_inl
#define __MACROS_EVENT_inl

/*
 *  Event_Manager_initialization
 */

STATIC INLINE void _Event_Manager_initialization( void )
{
  _Event_Sync = FALSE;

  /*
   *  Register the MP Process Packet routine.
   */
 
  _MPCI_Register_packet_processor( MP_PACKET_EVENT, _Event_MP_Process_packet );
}

#endif
/* end of include file */
