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

#ifndef __CPU_SH_TYPES_h
#define __CPU_SH_TYPES_h

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This section defines the basic types for this processor.
 */

typedef unsigned char  unsigned8;      		/* unsigned 8-bit  integer */
typedef unsigned short unsigned16;     		/* unsigned 16-bit integer */
typedef unsigned int   unsigned32;     		/* unsigned 32-bit integer */
typedef unsigned long long unsigned64; 		/* unsigned 64-bit integer */

typedef unsigned16     Priority_Bit_map_control;

typedef signed char      signed8;      		/* 8-bit  signed integer */
typedef signed short     signed16;     		/* 16-bit signed integer */
typedef signed int       signed32;     		/* 32-bit signed integer */
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

