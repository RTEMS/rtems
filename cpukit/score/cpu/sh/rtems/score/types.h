/**
 * @file rtems/score/types.h
 */

/*
 *  This include file contains information pertaining to the Hitachi SH
 *  processor.
 *
 *  Authors: Ralf Corsepius (corsepiu@faw.uni-ulm.de) and
 *           Bernd Becker (becker@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *  COPYRIGHT (c) 1998-2001.
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

#include <rtems/stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This section defines the basic types for this processor.
 */

typedef unsigned long long unsigned64; 		/* unsigned 64-bit integer */

typedef unsigned16     Priority_Bit_map_control;

typedef signed long long signed64;     		/* 64 bit signed integer */

typedef unsigned16 boolean;     		/* Boolean value, external */
						/* data bus has 16 bits  */

typedef float          single_precision;     	/* single precision float */
typedef double         double_precision;     	/* double precision float */

typedef void sh_isr;
typedef void ( *sh_isr_entry )( void );

#ifdef __cplusplus
}
#endif

#endif  /* !ASM */

#endif
