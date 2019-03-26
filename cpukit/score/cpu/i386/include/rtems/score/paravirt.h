/**
 * @file
 *
 * @brief i386 Paravirtualization Definitions
 *
 * This include file contains definitions pertaining to paravirtualization
 * of the i386 port.
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
 * @defgroup RTEMSScoreCPUi386Paravirt i386 Paravirtualization Support
 *
 * @ingroup RTEMSScoreCPUi386
 *
 * @{
 * 
 * This handler encapulates the functionality (primarily conditional
 * feature defines) related to paravirtualization on the i386.
 *
 * Paravirtualization on the i386 makes the following assumptions:
 *
 *   - RTEMS executes in user space
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
#define I386_DISABLE_INLINE_ISR_DISABLE_ENABLE

#endif

/** @} */
