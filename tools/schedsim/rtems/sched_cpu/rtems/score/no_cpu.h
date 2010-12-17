/*  no_cpu.h
 *
 *  BASED UPON SOURCE IN RTEMS, MODIFIED FOR SIMULATOR
 *
 *  This file sets up basic CPU dependency settings based on
 *  compiler settings.  For example, it can determine if
 *  floating point is available.  This particular implementation
 *  is specified to the NO CPU port.
 *
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 *
 */

#ifndef _RTEMS_SCORE_NO_CPU_H
#define _RTEMS_SCORE_NO_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the NO CPU family.
 *  It does this by setting variables to indicate which
 *  implementation dependent features are present in a particular
 *  member of the family.
 *
 *  This is a good place to list all the known CPU models
 *  that this port supports and which RTEMS CPU model they correspond
 *  to.
 */

/*
 *  Figure out all CPU Model Feature Flags based upon compiler
 *  predefines.
 */

#define CPU_MODEL_NAME  ""
#define NOCPU_HAS_FPU   1

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "RTEMS Sheduler Simulator"

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_NO_CPU_H */
