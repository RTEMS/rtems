/**
 * @file
 *
 * @brief Blackfin CPU Type Definitions
 *
 * This include file contains type definitions pertaining to the
 * Blackfin processor family.
 */

/*
 *  COPYRIGHT (c) 1989-2006.
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

/*
 *  This section defines the basic types for this processor.
 */

/** Type that can store a 32-bit integer or a pointer. */
typedef uintptr_t CPU_Uint32ptr;

/** This defines the return type for an ISR entry point. */
typedef void blackfin_isr;

/** This defines the prototype for an ISR entry point. */
typedef blackfin_isr ( *blackfin_isr_entry )( void );

#ifdef __cplusplus
}
#endif

#endif  /* !ASM */

#endif
