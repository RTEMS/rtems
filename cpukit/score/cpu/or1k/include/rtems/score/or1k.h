/**
 * @file
 */

/*
 *  This file contains information pertaining to the OR1K processor.
 *
 *  COPYRIGHT (c) 2014 Hesham ALMatary <heshamelmatary@gmail.com>
 *
 *  Based on code with the following copyright...
 *  COPYRIGHT (c) 1989-1999, 2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_OR1K_H
#define _RTEMS_SCORE_OR1K_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the OR1K family.
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

#define CPU_NAME "OR1K"
#define CPU_MODEL_NAME "OR1200"

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_OR1K_H */
