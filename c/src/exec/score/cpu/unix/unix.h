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
 *  This file contains the information required to build
 *  RTEMS for a particular member of the "unix"
 *  family when executing in protected mode.  It does
 *  this by setting variables to indicate which implementation
 *  dependent features are present in a particular member
 *  of the family.
 */
 
#if defined(hpux)
 
#define CPU_MODEL_NAME  "HP-UX"
 
#elif defined(solaris)
 
#define CPU_MODEL_NAME  "Solaris"
 
#elif defined(__linux__)
 
#define CPU_MODEL_NAME  "Linux"
 
#elif defined(linux)
 
#define CPU_MODEL_NAME  "Linux"
 
#else
 
#error "Unsupported CPU Model"
 
#endif
 
#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

