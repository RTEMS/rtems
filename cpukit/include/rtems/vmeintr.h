/**
 * @file
 * 
 * @brief VMEbus Interface Library
 *
 * This file is the specification for the VMEbus interface library
 * which should be provided by all BSPs for VMEbus Single Board
 * Computers but currently only a few do so.
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_VMEINTR_H
#define _RTEMS_VMEINTR_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This defines the mask which is used to determine which
 *  interrupt levels are affected by a call to this package.
 *  The LSB corresponds to VME interrupt 0 and the MSB
 *  to VME interrupt 7.
 *
 */

typedef uint8_t   VME_interrupt_Mask;

/*
 *  VME_interrupt_Disable
 *
 */

void VME_interrupt_Disable (
  VME_interrupt_Mask                mask                        /* IN  */
);

/*
 *  VME_interrupt_Disable
 *
 */

void VME_interrupt_Enable (
  VME_interrupt_Mask                mask                        /* IN  */
);

#ifdef __cplusplus
}
#endif

#endif /* end of include file */
