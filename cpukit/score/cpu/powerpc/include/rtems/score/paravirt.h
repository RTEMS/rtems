/**
 * @file
 *
 * @brief PowerPC Paravirtualization Definitions
 *
 * This include file contains definitions pertaining to paravirtualization
 * of the PowerPC port.
 */

/*
 *  COPYRIGHT (c) 2018.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */


#ifndef RTEMS_PARAVIRT
#error "This file should only be included with paravirtualization is enabled."
#endif

#ifndef _RTEMS_SCORE_PARAVIRT_H
#define _RTEMS_SCORE_PARAVIRT_H

/**
 * @defgroup RTEMSScoreCPUPowerPCParavirt PowerPC Paravirtualization Support
 *
 * @ingroup RTEMSScoreCPUPowerPC
 * 
 * @brief PowerPC Paravirtualization Support
 *
 * This handler encapulates the functionality (primarily conditional
 * feature defines) related to paravirtualization on the PowerPC.
 *
 * Paravirtualization on the PowerPC makes the following assumptions:
 *
 *   - RTEMS executes in user space
 *   - In user space there is no access to the MSR.
 *   - Interrupt enable/disable support using the MSR must be disabled
 *     and replaced with BSP provided methods which are adapted to the
 *     hosting environment.
 */

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif  /* !ASM */

/**
 * In a paravirtualized environment, RTEMS executes in user space
 * and cannot disable/enable external exceptions (e.g. interrupts).
 * The BSP which acts as an adapter to the hosting environment will
 * provide the interrupt enable/disable methods.
 */
#define PPC_DISABLE_INLINE_ISR_DISABLE_ENABLE

/**
 * In a paravirtualized environment, RTEMS executes in user space
 * and cannot access the MSR.
 *
 * Try to have as little impact as possible with this define.  Leave
 * the msr in the thread context because that would impact the definition
 * of offsets for assembly code.
 */
#define PPC_DISABLE_MSR_ACCESS

#endif
