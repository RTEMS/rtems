/**
 * @file rtems/score/types.h
 *
 * This include file contains type definitions pertaining to the
 * SPARC processor family.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_TYPES_H
#define _RTEMS_SCORE_TYPES_H

#include <rtems/score/basedefs.h>

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Priority Bit Map Type
 *
 * On the SPARC, there is no bitscan instruction and no penalty associated
 * for using 16-bit variables.  With no overriding architectural factors,
 * just using a uint16_t.
 */
typedef uint16_t Priority_bit_map_Control;

/**
 *  @brief SPARC ISR Handler Return Type
 *
 *  This is the type which SPARC ISR Handlers return.
 */
typedef void sparc_isr;

/**
 *  @brief SPARC ISR Handler Prototype
 *
 *  This is the prototype for SPARC ISR Handlers.
 */
typedef void ( *sparc_isr_entry )( void );

#ifdef __cplusplus
}
#endif

#endif  /* !ASM */

#endif
