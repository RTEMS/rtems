/**
 * @file rtems/score/types.h
 *
 * @brief Type Definitions Pertaining to the MIPS Processor Family
 *
 *  This include file contains type definitions pertaining to the MIPS
 *  processor family.
 */

/*
 *  COPYRIGHT (c) 1989-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */
/* @(#)mipstypes.h       08/20/96     1.4 */

#ifndef _RTEMS_SCORE_TYPES_H
#define _RTEMS_SCORE_TYPES_H

/**
 *  @defgroup ScoreTypes MIPS Processor Family Type Definitions
 *
 *  @ingroup Score
 *
 */
/**@{*/

#include <rtems/score/basedefs.h>

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This section defines the basic types for this processor.
 */

typedef uint16_t     Priority_bit_map_Word;
typedef void mips_isr;
typedef void ( *mips_isr_entry )( void );

#ifdef __cplusplus
}
#endif

#endif  /* !ASM */

/**@}*/
#endif
