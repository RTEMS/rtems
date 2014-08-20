/**
 * @file tms570-vim.h
 *
 * @ingroup tms570
 *
 * @brief Vectored Interrupt Module (VIM) header file.
 */

/*
 * Copyright (c) 2014 Premysl Houdek <kom541000@gmail.com>
 *
 * Google Summer of Code 2014 at
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * Based on LPC24xx and LPC1768 BSP
 * by embedded brains GmbH and others
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_TMS570_VIM_H
#define LIBBSP_ARM_TMS570_VIM_H

#ifndef ASM
#include <rtems.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct{
    uint32_t PARFLG;            /* InterruptVectorTableParityFlagRegister */
    uint32_t PARCTL;            /* InterruptVectorTableParityControlRegister */
    uint32_t ADDERR;            /* AddressParityErrorRegister */
    uint32_t FBPARERR;          /* Fall-BackAddressParityErrorRegister */
    uint32_t reserved1 [0x4/4];
    uint32_t IRQINDEX;          /* IRQIndexOffsetVectorRegister */
    uint32_t FIQINDEX;          /* FIQIndexOffsetVectorRegister */
    uint32_t reserved2 [0x8/4];
    uint32_t FIRQPR[3];         /* FIQ/IRQProgramControlRegister0 */
    uint32_t reserved3 [0x4/4];
    uint32_t INTREQ[3];         /* PendingInterruptReadLocationRegister0 */
    uint32_t reserved4 [0x4/4];
    uint32_t REQENASET[3];      /* InterruptEnableSetRegister0 */
    uint32_t reserved5 [0x4/4];
    uint32_t REQENACLR[3];      /* InterruptEnableClearRegister0 */
    uint32_t reserved6 [0x4/4];
    uint32_t WAKEENASET[3];     /* Wake-upEnableSetRegister0 */
    uint32_t reserved7 [0x4/4];
    uint32_t WAKEENACLR[3];     /* Wake-upEnableClearRegister0 */
    uint32_t reserved8 [0x4/4];
    uint32_t IRQVECREG;         /* IRQInterruptVectorRegister */
    uint32_t FIQVECREG;         /* FIQInterruptVectorRegister */
    uint32_t CAPEVT;            /* CaptureEventRegister */
    uint32_t reserved9 [0x4/4];
    uint32_t CHANCTRL [0x5c/4]; /* VIM Interrupt Control Register (PARSER ERROR) */
}tms570_vim_t;

#define TMS570_VIM (*(volatile tms570_vim_t*)0xFFFFFDEC)

#endif

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_TMS570_IRQ_H */
