/*  mipstypes.h
 *
 *  This include file contains type definitions pertaining to the MIPS
 *  processor family.
 *
 *  COPYRIGHT (c) 1989-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */
/* @(#)mipstypes.h       08/20/96     1.4 */

#ifndef __MIPS_TYPES_h
#define __MIPS_TYPES_h

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This section defines the basic types for this processor.
 */

typedef unsigned char      unsigned8;  /* unsigned 8-bit  integer */
typedef unsigned short     unsigned16; /* unsigned 16-bit integer */
typedef unsigned int       unsigned32; /* unsigned 32-bit integer */
typedef unsigned long long unsigned64; /* unsigned 64-bit integer */

typedef unsigned16     Priority_Bit_map_control;

typedef signed char      signed8;      /* 8-bit  signed integer */
typedef signed short     signed16;     /* 16-bit signed integer */
typedef signed int       signed32;     /* 32-bit signed integer */
typedef signed long long signed64;     /* 64 bit signed integer */

typedef unsigned32 boolean;     /* Boolean value   */

typedef float          single_precision;     /* single precision float */
typedef double         double_precision;     /* double precision float */

typedef void mips_isr;
typedef void ( *mips_isr_entry )( void );

#ifdef __cplusplus
}
#endif

#endif  /* !ASM */

#endif
/* end of include file */
