/**
 * @file rtems/score/types.h
 */

/*
 *  $Id$
 *
 *  This include file contains type definitions pertaining to the
 *  arm processor family.
 *
 *  COPYRIGHT (c) 2000 Canon Research Centre France SA.
 *  Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
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

typedef bool             boolean;            /* Boolean value   */
typedef uint16_t         Priority_Bit_map_control;

typedef float              single_precision;   /* single precision float */
typedef double             double_precision;   /* double precision float */

typedef void               arm_cpu_isr;
typedef void            ( *arm_cpu_isr_entry )( void );

#ifdef __cplusplus
}
#endif

#endif  /* !ASM */

#endif
