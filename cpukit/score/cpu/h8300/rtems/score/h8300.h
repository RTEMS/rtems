/*  h8300.h
 *
 *  This file contains information pertaining to the Hitachi H8/300
 *  processor family.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _INCLUDE_H8300_h
#define _INCLUDE_H8300_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the "h8300"
 *  family when executing in protected mode.  It does
 *  this by setting variables to indicate which implementation
 *  dependent features are present in a particular member
 *  of the family.
 */

/*
 *  RTEMS compiles for the base H8 with numerous warnings but has never
 *  been tested on a CPU with 16 bit address space.
 *
 *  FIXME:
 *  This macro is defined to handle a couple of places where 
 *  addresses are cast to pointers.  There really should be 
 *  a "int-pointer" type that pointers are cast to before being
 *  mathematcically manipulated.  When that is added, search
 *  for all references to this macro and remove them.
 */

#if defined(__H8300__)
#define RTEMS_CPU_HAS_16_BIT_ADDRESSES 1
#endif
 
#define CPU_NAME  "Hitachi H8300"
#define CPU_MODEL_NAME  "h8300"
#define H8300_HAS_FPU     0

#ifdef __cplusplus
}
#endif

#endif 
/* end of include file */
