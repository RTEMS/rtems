/*  or1k.h
 *
 *  This include file contains Or1k definitions pertaining to the Opencores
 *  or1k processor family.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  This file adapted from no_cpu example of the RTEMS distribution.
 *  The body has been modified for the Opencores Or1k implementation by
 *  Chris Ziomkowski. <chris@asics.ws>
 *
 */

#ifndef _OR1K_H
#define _OR1K_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the or1k CPU family.
 *  It does this by setting variables to indicate which
 *  implementation dependent features are present in a particular
 *  member of the family.
 *
 *  This is a good place to list all the known CPU models
 *  that this port supports and which RTEMS CPU model they correspond
 *  to.
 */
 
#if defined(rtems_multilib)
/*
 *  Figure out all CPU Model Feature Flags based upon compiler 
 *  predefines. 
 */

#define CPU_MODEL_NAME  "rtems_multilib"
#define OR1K_HAS_FPU     1

#elif defined(or1200)
 
#define CPU_MODEL_NAME  "OR1200"
#define OR1K_HAS_FPU     0
 
#else
 
#define CPU_MODEL_NAME "Generic Or1k Compatible"
#define OR1K_HAS_FPU    0
 
#endif

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "OpenRisc 1000"

#ifdef __cplusplus
}
#endif

#endif /* ! _INCLUDE_NO_CPU_h */
/* end of include file */
