/*  macros/event.h
 *
 *  This include file contains the implementation of macros for
 *  the Event Manager.
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

#ifndef __MACROS_EVENT_h
#define __MACROS_EVENT_h

/*
 *  Event_Manager_initialization
 */

#define _Event_Manager_initialization() \
  do { \
    \
    _Event_Sync_state = EVENT_SYNC_SYNCHRONIZED; \
    \
    /* \
     *  Register the MP Process Packet routine. \
     */ \
    \
    _MPCI_Register_packet_processor( \
      MP_PACKET_EVENT, \
      _Event_MP_Process_packet \
    ); \
  } while ( 0 )

#endif
/* end of include file */
