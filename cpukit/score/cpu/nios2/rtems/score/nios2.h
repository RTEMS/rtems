/*  nios2.h
 *
 *  This file sets up basic CPU dependency settings based on
 *  compiler settings.  For example, it can determine if
 *  floating point is available.  This particular implementation
 *  is specific to the NIOS2 port.
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#ifndef _RTEMS_SCORE_NIOS2_H
#define _RTEMS_SCORE_NIOS2_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the NIOS2 family.
 *  It does this by setting variables to indicate which
 *  implementation dependent features are present in a particular
 *  member of the family.
 *
 *  This is a good place to list all the known CPU models
 *  that this port supports and which RTEMS CPU model they correspond
 *  to.
 */

/*
 *  Define the name of the CPU family and specific model.
 */

#define CPU_NAME "NIOS2"
#define CPU_MODEL_NAME "nios2"

/*
 *  See also nios2-rtems-gcc -print-multi-lib for all valid combinations of
 *
 *    -mno-hw-mul
 *    -mhw-mulx
 *    -mstack-check
 *    -pg
 *    -EB
 *    -mcustom-fpu-cfg=60-1
 *    -mcustom-fpu-cfg=60-2
 */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_NIOS2_H */
