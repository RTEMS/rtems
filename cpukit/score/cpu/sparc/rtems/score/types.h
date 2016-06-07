/**
 * @file
 *
 * @brief SPARC CPU Type Definitions
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
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_TYPES_H
#define _RTEMS_SCORE_TYPES_H

#include <rtems/score/basedefs.h>

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

/** Type that can store a 32-bit integer or a pointer. */
typedef uintptr_t CPU_Uint32ptr;

/**
 * @brief SPARC ISR handler return type.
 *
 * This is the type which SPARC ISR Handlers return.
 */
typedef void sparc_isr;

/**
 * @brief SPARC ISR handler prototype.
 *
 * This is the prototype for SPARC ISR Handlers.
 */
typedef void ( *sparc_isr_entry )( void );

#ifdef __cplusplus
}
#endif

#endif  /* !ASM */

#endif
