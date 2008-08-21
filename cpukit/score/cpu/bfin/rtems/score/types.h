/*
 *  This include file contains type definitions pertaining to the
 *  Blackfin processor family.
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_TYPES_H
#define _RTEMS_SCORE_TYPES_H

#ifndef ASM

#include <stdbool.h>
#include <rtems/stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This section defines the basic types for this processor.
 */

/** This defines the type for a priority bit map entry. */
typedef uint16_t Priority_Bit_map_control;

/** This defines the type for a Boolean value, */
typedef bool boolean;     /* Boolean value   */

/** This defines the type for a single precision float. */
typedef float          single_precision;
/** This defines the type for a double precision float. */
typedef double         double_precision;

/** This defines the return type for an ISR entry point. */
typedef void blackfin_isr;

/** This defines the prototype for an ISR entry point. */
typedef blackfin_isr ( *blackfin_isr_entry )( void );

#ifdef __cplusplus
}
#endif

#endif  /* !ASM */

#endif
