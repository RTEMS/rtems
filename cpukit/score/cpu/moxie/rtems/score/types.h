/**
 * @file rtems/score/types.h
 */

/*
 *  This file contains information pertaining to the Moxie processor.
 *
 *  COPYRIGHT (c) 2011
 *  Anthony Green
 *
 *  Based on code with the following copyright...
 *  COPYRIGHT (c) 1989-1999, 2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_TYPES_H
#define _RTEMS_SCORE_TYPES_H

#include <rtems/score/basedefs.h>

#ifndef ASM

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This section defines the basic types for this processor.
 */
typedef uint16_t     Priority_bit_map_Word;
typedef void moxie_isr;
typedef void ( *moxie_isr_entry )( void );

#ifdef RTEMS_DEPRECATED_TYPES
typedef bool    boolean;                /* Boolean value   */
typedef float   single_precision;       /* single precision float */
typedef double  double_precision;       /* double precision float */
#endif

#ifdef __cplusplus
}
#endif

#endif  /* !ASM */

#endif
