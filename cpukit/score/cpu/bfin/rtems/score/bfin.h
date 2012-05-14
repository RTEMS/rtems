/*  bfin.h
 *
 *  This file sets up basic CPU dependency settings based on
 *  compiler settings.  For example, it can determine if
 *  floating point is available.  This particular implementation
 *  is specified to the Blackfin port.
 *
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *             modified by Alain Schaefer <alain.schaefer@easc.ch>
 *                     and Antonio Giovanini <antonio@atos.com.br>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#ifndef _RTEMS_SCORE_BFIN_H
#define _RTEMS_SCORE_BFIN_H

#ifdef __cplusplus
extern "C" {
#endif


/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the Blackfin family.
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
#if defined(__BFIN__)
#define CPU_MODEL_NAME  "BF533"
#define BF_HAS_FPU   0
#else

#error "Unsupported CPU Model"

#endif

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "BFIN"

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_BFIN_H */
