/*  mppkt.h
 *
 *  This package is the specification for the Packet Handler.
 *  This handler defines the basic packet and provides
 *  mechanisms to utilize packets based on this prefix.
 *  Packets are the fundamental basis for messages passed between
 *  nodes in an MP system.
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

#ifndef __MP_PACKET_h
#define __MP_PACKET_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/watchdog.h>

/*
 *  The following enumerated type defines the packet classes.
 *
 *  NOTE:  In general, each class corresponds to a manager
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

#define MP_PACKET_CLASSES_FIRST  MP_PACKET_MPCI_INTERNAL
#define MP_PACKET_CLASSES_LAST   MP_PACKET_SIGNAL

/*
 *  The following record contains the prefix for every packet
 *  passed between nodes in an MP system.
 *
 *  NOTE: This structure is padded to insure that anything
 *        following it is on a 16 byte boundary.  This is
 *        the most stringent structure alignment rule
 *        encountered yet (i960CA).
 */

typedef struct {
  MP_packet_Classes       the_class;
  Objects_Id              id;
  Objects_Id              source_tid;
  Priority_Control        source_priority;
  unsigned32              return_code;
  unsigned32              length;
  unsigned32              to_convert;
  Watchdog_Interval       timeout;
}   MP_packet_Prefix;

/*
 *  An MPCI must support packets of at least this size.
 */

#define MP_PACKET_MINIMUM_PACKET_SIZE  64

/*
 *  The following constant defines the number of unsigned32's
 *  in a packet which must be converted to native format in a
 *  heterogeneous system.  In packets longer than
 *  MP_PACKET_MINIMUN_HETERO_CONVERSION unsigned32's, some of the "extra" data
 *  may a user message buffer which is not automatically endian swapped.
 */

#define MP_PACKET_MINIMUN_HETERO_CONVERSION  \
  ( sizeof( MP_packet_Prefix ) / sizeof( unsigned32 ) )

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/mppkt.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
