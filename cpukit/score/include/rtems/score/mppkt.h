/*  mppkt.h
 *
 *  This package is the specification for the Packet Handler.
 *  This handler defines the basic RTEMS packet and provides
 *  mechanisms to utilize packets based on this prefix.
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

#ifndef __RTEMS_MP_PACKET_h
#define __RTEMS_MP_PACKET_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/object.h>
#include <rtems/priority.h>
#include <rtems/watchdog.h>

/*
 * The following enumerated type defines the packet classes
 *  supported by RTEMS.
 *
 *  NOTE:  In general, each class corresponds to a manager
 *         which supports global operations.  Each manager
 *         defines the set of supported operations.
 */

typedef enum {
  RTEMS_MP_PACKET_INTERNAL_THREADS = 0,
  RTEMS_MP_PACKET_TASKS            = 1,
  RTEMS_MP_PACKET_MESSAGE_QUEUE    = 2,
  RTEMS_MP_PACKET_SEMAPHORE        = 3,
  RTEMS_MP_PACKET_PARTITION        = 4,
  RTEMS_MP_PACKET_REGION           = 5,
  RTEMS_MP_PACKET_EVENT            = 6,
  RTEMS_MP_PACKET_SIGNAL           = 7
}   rtems_mp_packet_classes;

#define MP_PACKET_CLASSES_FIRST  RTEMS_MP_PACKET_INTERNAL_THREADS
#define MP_PACKET_CLASSES_LAST   RTEMS_MP_PACKET_SIGNAL

/*
 *  The following record contains the prefix for every packet
 *  passed between RTEMS nodes.
 *
 *  NOTE: This structure is padded to insure that anything
 *        following it is on a 16 byte boundary.  This is
 *        the most stringent structure alignment rule
 *        the RTEMS project has encountered yet (i960CA).
 */

typedef struct {
  rtems_mp_packet_classes the_class;
  Objects_Id              id;
  Objects_Id              source_tid;
  Priority_Control        source_priority;
  rtems_status_code       return_code;
  unsigned32              length;
  unsigned32              to_convert;
  rtems_interval          timeout;
}   rtems_packet_prefix;

/*
 *  An MPCI must support packets of at least this size.
 */

#define RTEMS_MINIMUM_PACKET_SIZE  64

/*
 *  The following constant defines the number of unsigned32's
 *  in a packet which must be converted to native format in a
 *  heterogeneous system.  In packets longer than
 *  RTEMS_MINIMUN_HETERO_CONVERSION unsigned32's, some of the "extra" data
 *  may a user message buffer which is not automatically endian swapped.
 */

#define RTEMS_MINIMUN_HETERO_CONVERSION  ( sizeof( rtems_packet_prefix ) / 4 )

/*
 *  _Mp_packet_Is_valid_packet_class
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the the_packet_class is valid,
 *  and FALSE otherwise.
 */

STATIC INLINE boolean _Mp_packet_Is_valid_packet_class (
  rtems_mp_packet_classes the_packet_class
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
  rtems_packet_prefix   *the_packet
);

#include <rtems/mppkt.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
