/**
 * @file rtems/score/types.h
 */

/*
 *  This include file contains type definitions pertaining to the 
 *  SPARC processor family.
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

typedef unsigned long long unsigned64;           /* unsigned 64-bit integer */

typedef unsigned16         Priority_Bit_map_control;

typedef signed long long   signed64;             /* 64 bit signed integer */

typedef unsigned32         boolean;              /* Boolean value   */

typedef float              single_precision;     /* single precision float */
typedef double             double_precision;     /* double precision float */

typedef void sparc_isr;
typedef void ( *sparc_isr_entry )( void );

#ifdef __cplusplus
}
#endif

#endif  /* !ASM */

#endif
