/**
 *  @file  rtems/score/mppkt.h
 *
 *  This package is the specification for the Packet Handler.
 *  This handler defines the basic packet and provides
 *  mechanisms to utilize packets based on this prefix.
 *  Packets are the fundamental basis for messages passed between
 *  nodes in an MP system.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_MPPKT_H
#define _RTEMS_SCORE_MPPKT_H

/**
 *  @defgroup ScoreMPPacket MP Packet Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates the primary definition of MPCI packets.  This
 *  handler defines the part of the packet that is common to all remote
 *  operations.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/watchdog.h>

/**
 *  The following enumerated type defines the packet classes.
 *
 *  @note  In general, each class corresponds to a manager
 *         which supports global operations.  Each manager
 *         defines the set of supported operations.
 */
typedef enum {
  MP_PACKET_MPCI_INTERNAL    = 0,
  MP_PACKET_TASKS            = 1,
  MP_PACKET_MESSAGE_QUEUE    = 2,
  MP_PACKET_SEMAPHORE        = 3,
  MP_PACKET_PARTITION        = 4,
  MP_PACKET_REGION           = 5,
  MP_PACKET_EVENT            = 6,
  MP_PACKET_SIGNAL           = 7
}   MP_packet_Classes;

/**
 *  This constant defines the first entry in the MP_packet_Classes enumeration.
 */
#define MP_PACKET_CLASSES_FIRST  MP_PACKET_MPCI_INTERNAL

/**
 *  This constant defines the last entry in the MP_packet_Classes enumeration.
 */
#define MP_PACKET_CLASSES_LAST   MP_PACKET_SIGNAL

/**
 *  The following record contains the prefix for every packet
 *  passed between nodes in an MP system.
 *
 *  @note This structure is padded to ensure that anything following it
 *        is on a 16 byte boundary.  This is the most stringent structure
 *        alignment rule encountered yet.
 */
typedef struct {
  /** This field indicates the API class of the operation being performed. */
  MP_packet_Classes       the_class;
  /** This field is the id of the object to be acted upon. */
  Objects_Id              id;
  /** This field is the ID of the originating thread. */
  Objects_Id              source_tid;
  /** This field is the priority of the originating thread. */
  Priority_Control        source_priority;
  /** This field is where the status of the operation will be returned. */
  uint32_t                return_code;
  /** This field is the length of the data following the prefix. */
  uint32_t                length;
  /** This field is the length of the data which required network conversion. */
  uint32_t                to_convert;
  /** This field is the requested timeout for this operation. */
  Watchdog_Interval       timeout;
}   MP_packet_Prefix;

/**
 *  An MPCI must support packets of at least this size.
 */
#define MP_PACKET_MINIMUM_PACKET_SIZE  64

/**
 *  The following constant defines the number of uint32_t's
 *  in a packet which must be converted to native format in a
 *  heterogeneous system.  In packets longer than
 *  MP_PACKET_MINIMUN_HETERO_CONVERSION uint32_t's, some of the "extra" data
 *  may a user message buffer which is not automatically endian swapped.
 */
#define MP_PACKET_MINIMUN_HETERO_CONVERSION  \
  ( sizeof( MP_packet_Prefix ) / sizeof( uint32_t   ) )

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/mppkt.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
