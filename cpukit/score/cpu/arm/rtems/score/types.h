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
 *  http://www.OARcorp.com/rtems/license.html.
 *
 */

#ifndef __ARM_TYPES_h
#define __ARM_TYPES_h

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This section defines the basic types for this processor.
 */

typedef unsigned char      unsigned8;          /* unsigned 8-bit  integer */
typedef unsigned short     unsigned16;         /* unsigned 16-bit integer */
typedef unsigned int       unsigned32;         /* unsigned 32-bit integer */
typedef unsigned long long unsigned64;         /* unsigned 64-bit integer */

typedef signed char        signed8;            /* 8-bit  signed integer */
typedef signed short       signed16;           /* 16-bit signed integer */
typedef signed int         signed32;           /* 32-bit signed integer */
typedef signed long long   signed64;           /* 64 bit signed integer */

typedef unsigned32         boolean;            /* Boolean value   */
typedef unsigned16         Priority_Bit_map_control;

typedef float              single_precision;   /* single precision float */
typedef double             double_precision;   /* double precision float */

typedef void               arm_cpu_isr;
typedef void            ( *arm_cpu_isr_entry )( void );

#ifdef __cplusplus
}
#endif

#endif  /* !ASM */

#endif
/* end of include file */
