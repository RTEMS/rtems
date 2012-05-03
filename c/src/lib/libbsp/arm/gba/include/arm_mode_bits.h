/**
 * @file arm_mode_bits.h
 *
 *  ARM statusregister mode bits.
 *
 *  This include file contains definitions related to the ARM BSP.
 */
/*
 *  RTEMS GBA BSP
 *
 *  Copyright (c) 2004  Markku Puro <markku.puro@kopteri.net>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __ARMMODEBITS_H
#define __ARMMODEBITS_H

/*-----------------------------------------------------------------------------
 * Definitions
 ----------------------------------------------------------------------------*/
#define Mode_USR             0x10
#define Mode_FIQ             0x11
#define Mode_IRQ             0x12
#define Mode_SVC             0x13
#define Mode_ABT             0x17
#define Mode_ABORT           0x17
#define Mode_UNDEF           0x1B
#define Mode_SYS             0x1F      /**< only available on ARM Arch v4 */
#define Mode_Bits            0x1F      /**< mask for mode bits */
#define ModePriv             Mode_SVC  /**< used supervisor mode */

#define I_Bit                0x80
#define F_Bit                0x40
#define Int_Bits             0xC0

#define Mode_SVC_MIRQ        (Mode_SVC | I_Bit | F_Bit)
#define Mode_SVC_UIRQ        (Mode_SVC)
#define Mode_IRQ_MIRQ        (Mode_SVC | I_Bit | F_Bit)
#define Mode_IRQ_UIRQ        (Mode_SVC)

#endif /* __ARMMODEBITS_H */

