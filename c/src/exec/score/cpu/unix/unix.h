/*  unix.h
 *
 *  This include file contains the definitions required by RTEMS
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
 
#ifdef unix
#undef unix
#endif
#define unix

#ifdef REPLACE_THIS_WITH_THE_CPU_FAMILY
#undef REPLACE_THIS_WITH_THE_CPU_FAMILY
#endif
#define REPLACE_THIS_WITH_THE_CPU_FAMILY

#ifdef REPLACE_THIS_WITH_THE_BSP
#undef REPLACE_THIS_WITH_THE_BSP
#endif
#define REPLACE_THIS_WITH_THE_BSP

#ifdef REPLACE_THIS_WITH_THE_CPU_MODEL
#undef REPLACE_THIS_WITH_THE_CPU_MODEL
#endif
#define REPLACE_THIS_WITH_THE_CPU_MODEL

#ifdef REPLACE_THIS_WITH_THE_UNIX_FLAVOR
#undef REPLACE_THIS_WITH_THE_UNIX_FLAVOR
#endif
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
 
#define RTEMS_MODEL_NAME  "HP-UX"
 
#elif defined(solaris)
 
#define RTEMS_MODEL_NAME  "Solaris"
 
#elif defined(linux)
 
#define RTEMS_MODEL_NAME  "Linux"
 
#else
 
#error "Unsupported CPU Model"
 
#endif
 
#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

