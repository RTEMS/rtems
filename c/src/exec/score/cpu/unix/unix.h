/*  unix.h
 *
 *  This include file contains the basic type definitions required by RTEMS
 *  which are typical for a modern UNIX computer using GCC.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __UNIX_h
#define __UNIX_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  The following define the CPU Family and Model within the family
 *
 *  NOTE: The string "REPLACE_THIS_WITH_THE_CPU_MODEL" is replaced
 *        with the name of the appropriate macro for this target CPU.
 */
 
#define unix
#define REPLACE_THIS_WITH_THE_CPU_FAMILY
#define REPLACE_THIS_WITH_THE_BSP
#define REPLACE_THIS_WITH_THE_CPU_MODEL
#define REPLACE_THIS_WITH_THE_UNIX_FLAVOR
 
/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the "unix"
 *  family when executing in protected mode.  It does
 *  this by setting variables to indicate which implementation
 *  dependent features are present in a particular member
 *  of the family.
 */
 
#if defined(hpux)
 
#define RTEMS_MODEL_NAME  "hpux"
 
#elif defined(solaris)
 
#define RTEMS_MODEL_NAME  "solaris"
 
#else
 
#error "Unsupported CPU Model"
 
#endif
 
#ifndef ASM

/* type definitions */

typedef unsigned char  unsigned8;       /* 8-bit  unsigned integer */
typedef unsigned short unsigned16;      /* 16-bit unsigned integer */
typedef unsigned int   unsigned32;      /* 32-bit unsigned integer */
typedef unsigned long long unsigned64;  /* 64-bit unsigned integer */

typedef unsigned16     Priority_Bit_map_control;

typedef char           signed8;    /* 8-bit signed integer  */
typedef short          signed16;   /* 16-bit signed integer */
typedef int            signed32;   /* 32-bit signed integer */
typedef long long      signed64;   /* 64-bit signed integer */

typedef unsigned32 boolean;     /* Boolean value   */

typedef float          single_precision;     /* single precision float */
typedef double         double_precision;     /* double precision float */

typedef void ( *unix_isr_entry )( void );

#ifdef __cplusplus
}
#endif

#endif /* !ASM */
#endif
/* end of include file */

