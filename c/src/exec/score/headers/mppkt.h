/*  mppkt.h
 *
 *  This package is the specification for the Packet Handler.
 *  This handler defines the basic packet and provides
 *  mechanisms to utilize packets based on this prefix.
 *  Packets are the fundamental basis for messages passed between
 *  nodes in an MP system.
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

#ifndef __MP_PACKET_h
#define __MP_PACKET_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/core/object.h>
#include <rtems/core/priority.h>
#include <rtems/core/watchdog.h>

/*
 *  The following enumerated type defines the packet classes.
 *
 *  NOTE:  In general, each class corresponds to a manager
 *         which supports global operations.  Each manager
 *         defines the set of supported operations.
 */

typedef enum {
  MP_PACKET_INTERNAL_THREADS = 0,
  MP_PACKET_TASKS            = 1,
  MP_PACKET_MESSAGE_QUEUE    = 2,
  MP_PACKET_SEMAPHORE        = 3,
  MP_PACKET_PARTITION        = 4,
  MP_PACKET_REGION           = 5,
  MP_PACKET_EVENT            = 6,
  MP_PACKET_SIGNAL           = 7
}   MP_packet_Classes;

#define MP_PACKET_CLASSES_FIRST  MP_PACKET_INTERNAL_THREADS
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

#define MP_PACKET_MINIMUN_HETERO_CONVERSION  ( sizeof( MP_packet_Prefix ) / 4 )

/*
 *  _Mp_packet_Is_valid_packet_class
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the the_packet_class is valid,
 *  and FALSE otherwise.
 */

STATIC INLINE boolean _Mp_packet_Is_valid_packet_class (
  MP_packet_Classes the_packet_class
);

/*
 *  _Mp_packet_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the the_packet_class is null,
 *  and FALSE otherwise.
 */

STATIC INLINE boolean _Mp_packet_Is_null (
  MP_packet_Prefix   *the_packet
);

#include <rtems/core/mppkt.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
