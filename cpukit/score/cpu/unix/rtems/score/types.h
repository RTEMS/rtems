/*  unixtypes.h
 *
 *  This include file contains type definitions which are appropriate
 *  for a typical modern UNIX box using GNU C for the RTEMS simulator.
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

#ifndef __UNIX_TYPES_h
#define __UNIX_TYPES_h

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

/*
 * some C++ compilers (eg: HP's) don't do 'signed' or 'volatile'
 */
#if defined(__cplusplus) && !defined(__GNUC__)
#define signed
#define volatile
#endif

/*
 *  This section defines the basic types for this processor.
 */

typedef unsigned char  unsigned8;      /* unsigned 8-bit  integer */
typedef unsigned short unsigned16;     /* unsigned 16-bit integer */
typedef unsigned int   unsigned32;     /* unsigned 32-bit integer */

typedef unsigned16     Priority_Bit_map_control;

typedef signed char      signed8;      /* 8-bit  signed integer */
typedef signed short     signed16;     /* 16-bit signed integer */
typedef signed int       signed32;     /* 32-bit signed integer */

/*
 * some C++ compilers (eg: HP's) don't do 'long long'
 */
#if defined(__GNUC__)
typedef unsigned long long unsigned64; /* unsigned 64-bit integer */
typedef signed long long signed64;     /* 64 bit signed integer */
#endif

typedef unsigned32 boolean;     /* Boolean value   */

typedef float          single_precision;     /* single precision float */
typedef double         double_precision;     /* double precision float */

typedef void unix_isr;

typedef unix_isr ( *unix_isr_entry )( void );

#ifdef __cplusplus
}
#endif

#endif  /* !ASM */

#endif
/* end of include file */
