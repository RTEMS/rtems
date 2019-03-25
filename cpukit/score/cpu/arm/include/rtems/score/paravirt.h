/**
 * @file
 *
 * @brief ARM Paravirtualization Definitions
 *
 * This include file contains definitions pertaining to paravirtualization
 * of the ARM port.
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
 * @defgroup RTEMSScoreCPUARMParavirt ARM Paravirtualization Support
 *
 * @ingroup RTEMSScoreCPUARM
 *
 * This handler encapulates the functionality (primarily conditional
 * feature defines) related to paravirtualization on the ARM.
 *
 * Paravirtualization on the ARM makes the following assumptions:
 *
 *   - RTEMS executes in user space
 *   - Interrupt enable/disable support using the MSR must be disabled
 *     and replaced with BSP provided methods which are adapted to the
 *     hosting environment.
 *
 * @{
 */

/**
 * In a paravirtualized environment, RTEMS executes in user space
 * and cannot disable/enable external exceptions (e.g. interrupts).
 * The BSP which acts as an adapter to the hosting environment will
 * provide the interrupt enable/disable methods.
 */
#define ARM_DISABLE_INLINE_ISR_DISABLE_ENABLE

/**
 * In a paravirtualized environment, RTEMS executes in user space
 * and cannot write to the the Thread ID register which is normally
 * used. CP15 C13 has three variants of a Thread ID register.
 *
 * - Opcode_2 = 2: This register is both user and privileged R/W accessible.
 * - Opcode_2 = 3: This register is user read-only and privileged
 *   R/W accessible.
 * - Opcode_2 = 4: This register is privileged R/W accessible only.
 */
#define ARM_DISABLE_THREAD_ID_REGISTER_USE

/** @} */

#endif
