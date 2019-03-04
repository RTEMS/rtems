/**
 * @file
 *
 * @ingroup RTEMSBSPsRISCVGRLIB
 *
 * @brief Global BSP definitions.
 */

/*
 *
 * Copyright (c) 2015 University of York.
 * Hesham Almatary <hesham@alumni.york.ac.uk>
 *
 * COPYRIGHT (c) 1989-1999.
 * On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef LIBBSP_RISCV_GRISCV_H
#define LIBBSP_RISCV_GRISCV_H

/**
 * @defgroup RTEMSBSPsRISCVGRLIB GRLIB
 *
 * @ingroup RTEMSBSPsRISCV
 *
 * @brief GRLIB RISC-V Board Support Package.
 *
 * @{
 */


#include <rtems.h>
#include <rtems/clockdrv.h>
#include <rtems/console.h>
#include <rtems/irq-extension.h>

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems/devnull.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_FEATURE_IRQ_EXTENSION

/* Maximum supported APBUARTs by BSP */
#define BSP_NUMBER_OF_TERMIOS_PORTS 8

/* Make sure maximum number of consoles fit in filesystem */
#define BSP_MAXIMUM_DEVICES 8

/* GRLIB driver functions */

extern void BSP_shared_interrupt_mask(int irq);
extern void BSP_shared_interrupt_clear(int irq);
extern void BSP_shared_interrupt_unmask(int irq);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* LIBBSP_RISCV_GRISCV_H */
