/**
 * @file
 * 
 * @brief Motorola M68K CPU Type Definitions
 * 
 * This include file contains type definitions pertaining to the Motorola
 * m68xxx processor family.
 */

/*
 *  COPYRIGHT (c) 1989-1999.
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

typedef uint16_t     Priority_bit_map_Word;

#ifdef __cplusplus
}
#endif

#endif  /* !ASM */

#endif
