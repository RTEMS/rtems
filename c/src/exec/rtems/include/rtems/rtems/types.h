/*  types.h
 *
 *  This include file defines the types used by the RTEMS API.
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

#ifndef __RTEMS_RTEMS_TYPES_h
#define __RTEMS_RTEMS_TYPES_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/object.h>

/*
 *  RTEMS basic type definitions
 */

typedef unsigned8       rtems_unsigned8;  /* unsigned 8-bit value  */
typedef unsigned16      rtems_unsigned16; /* unsigned 16-bit value */
typedef unsigned32      rtems_unsigned32; /* unsigned 32-bit value */
typedef unsigned64      rtems_unsigned64; /* unsigned 64-bit value */

typedef signed8         rtems_signed8;    /* signed 8-bit value  */
typedef signed16        rtems_signed16;   /* signed 16-bit value */
typedef signed32        rtems_signed32;   /* signed 32-bit value */
typedef signed64        rtems_signed64;   /* signed 64-bit value */

typedef single_precision rtems_single;    /* single precision float */
typedef double_precision rtems_double;    /* double precision float */

typedef boolean          rtems_boolean;

typedef unsigned32       rtems_name;
typedef Objects_Id       rtems_id;

typedef Context_Control            rtems_context;
typedef Context_Control_fp         rtems_context_fp;
typedef CPU_Interrupt_frame        rtems_interrupt_frame;

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
