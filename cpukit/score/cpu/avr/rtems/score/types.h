/*  types.h
 *
 *  This include file contains type definitions pertaining to the Intel
 *  avr processor family.
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

#ifndef __rtems_score_types_h
#define __rtems_score_types_h

#ifndef ASM

#include <rtems/stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This section defines the basic types for this processor.
 */

typedef uint8_t  unsigned8;	/* unsigned 8-bit  integer */
typedef uint16_t unsigned16;	/* unsigned 16-bit integer */
typedef uint32_t unsigned32;	/* unsigned 32-bit integer */
typedef uint64_t unsigned64;	/* unsigned 64-bit integer */

typedef int8_t  signed8;	/* 8-bit  signed integer */
typedef int16_t signed16;	/* 16-bit signed integer */
typedef int32_t	signed32;	/* 32-bit signed integer */
typedef int64_t signed64;	/* 64 bit signed integer */

typedef unsigned16     Priority_Bit_map_control;

typedef unsigned32 boolean;     /* Boolean value   */

typedef float          single_precision;     /* single precision float */
typedef double         double_precision;     /* double precision float */

typedef void avr_isr;
typedef void ( *avr_isr_entry )( void );

#ifdef __cplusplus
}
#endif

#endif  /* !ASM */

#endif
