/*  i960types.h
 *
 *  This include file contains type definitions pertaining to the Intel
 *  i960 processor family.
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

#ifndef __i960_TYPES_h
#define __i960_TYPES_h

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This section defines the basic types for this processor.
 */

typedef unsigned char  unsigned8;      /* unsigned 8-bit  integer */
typedef unsigned short unsigned16;     /* unsigned 16-bit integer */
typedef unsigned int   unsigned32;     /* unsigned 32-bit integer */
typedef unsigned long long unsigned64; /* unsigned 64-bit integer */

typedef unsigned32     Priority_Bit_map_control;

typedef char           signed8;       /* signed 8-bit integer  */
typedef short          signed16;      /* signed 16-bit integer */
typedef int            signed32;      /* signed 32-bit integer */
typedef long long      signed64;      /* signed 64-bit integer */

typedef unsigned32 boolean;     /* Boolean value   */

typedef float          single_precision;     /* single precision float */
typedef double         double_precision;     /* double precision float */

typedef void i960_isr;

typedef void ( *i960_isr_entry )( void );

#ifdef __cplusplus
}
#endif

#endif  /* !ASM */

#endif
/* end of include file */
