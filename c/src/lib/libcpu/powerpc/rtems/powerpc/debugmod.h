/*  debugmod.h
 *
 *  This file contains definitions for the IBM/Motorola PowerPC
 *  family members.
 *
 *  Author:	Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  MPC860 support code was added by Jay Monkman <jmonkman@frasca.com>
 *  MPC8260 support added by Andy Dachs <a.dachs@sstl.co.uk>
 *  Surrey Satellite Technology Limited
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/exec/cpu/no_cpu/no_cpu.h:
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *
 * Note:
 *      This file is included by both C and assembler code ( -DASM )
 */

/*
 * FIXME: This file is not used anywhere inside of RTEMS source-tree.
 * Notify OAR if you actually use it, otherwise it might be removed in
 * future versions of RTEMS
 */

#ifndef _RTEMS_POWERPC_DEBUGMOD_H
#define _RTEMS_POWERPC_DEBUGMOD_H

#warning "please read the FIXME inside of this file"

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the PowerPC family.  It does
 *  this by setting variables to indicate which implementation
 *  dependent features are present in a particular member
 *  of the family.
 *
 *  The following architectural feature definitions are defaulted
 *  unless specifically set by the model definition:
 *
 *    + PPC_DEBUG_MODEL          - PPC_DEBUG_MODEL_STANDARD
 */

/*
 *  Define the debugging assistance models found in the PPC family.
 *
 *  Standard:         single step and branch trace
 *  Single Step Only: single step only
 *  IBM 4xx:          debug exception
 */

#define PPC_DEBUG_MODEL_STANDARD         1
#define PPC_DEBUG_MODEL_SINGLE_STEP_ONLY 2
#define PPC_DEBUG_MODEL_IBM4xx           3

#elif defined(ppc403) || defined(ppc405) || defined(ppc440)

#define PPC_DEBUG_MODEL          PPC_DEBUG_MODEL_IBM4xx

#elif defined(ppc601)

#define PPC_DEBUG_MODEL PPC_DEBUG_MODEL_SINGLE_STEP_ONLY

#endif

/*
 *  Use the default debug scheme defined in the architectural specification
 *  if another model has not been specified.
 */

#ifndef PPC_DEBUG_MODEL
#define PPC_DEBUG_MODEL PPC_DEBUG_MODEL_STANDARD
#endif

/*
 *  Interrupt/exception MSR bits set as defined on p. 2-20 in "The Programming
 *  Environments" and the manuals for various PPC models.
 */

#if (PPC_DEBUG_MODEL == PPC_DEBUG_MODEL_STANDARD)
#define PPC_MSR_DE       0x000000000 /* bit 22 - debug exception enable */
#define PPC_MSR_BE       0x000000200 /* bit 22 - branch trace enable */
#define PPC_MSR_SE       0x000000400 /* bit 21 - single step trace enable */
#elif (PPC_DEBUG_MODEL == PPC_DEBUG_MODEL_SINGLE_STEP_ONLY)
#define PPC_MSR_DE       0x000000000 /* bit 22 - debug exception enable */
#define PPC_MSR_BE       0x000000200 /* bit 22 - branch trace enable */
#define PPC_MSR_SE       0x000000000 /* bit 21 - single step trace enable */
#elif (PPC_DEBUG_MODEL == PPC_DEBUG_MODEL_IBM4xx)
#define PPC_MSR_DE       0x000000200 /* bit 22 - debug exception enable */
#define PPC_MSR_BE       0x000000000 /* bit 22 - branch trace enable */
#define PPC_MSR_SE       0x000000000 /* bit 21 - single step trace enable */
#else
#error "MSR constants -- unknown PPC_DEBUG_MODEL!!"
#endif

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_POWERPC_DEBUGMOD_H */
/* end of include file */
