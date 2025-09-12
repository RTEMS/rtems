
/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMSMDK2410 
 *
 * @brief Global BSP definitons.
 */

/*
 * Copyright (C) 2008 Ray Xu <rayx.cn@gmail.com>
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LIBBSP_ARM_SMDK2410_BSP_H
#define LIBBSP_ARM_SMDK2410_BSP_H

/**
 * @defgroup RTEMSBSPsARMSMDK2410 SMDK2410
 *
 * @ingroup RTEMSBSPsARM
 *
 * @brief SMDK2410 Board Support Package.
 *
 * @{
 */

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <s3c24xx.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_FEATURE_IRQ_EXTENSION

/**
 * @brief functions to get the differents s3c2400 clks
 * @{
 */

uint32_t get_FCLK(void);
uint32_t get_HCLK(void);
uint32_t get_PCLK(void);
uint32_t get_UCLK(void);

/** @} */

/* What is the input clock freq in hertz? */
/** @brief 12 MHz oscillator */
#define BSP_OSC_FREQ  12000000
/** @brief FCLK=133Mhz */
#define M_MDIV 81
#define M_PDIV 2
#define M_SDIV 1
/** @brief HCLK=FCLK/2, PCLK=FCLK/2 */
#define M_CLKDIVN 2
/** @brief enable refresh */
#define REFEN	0x1
/** @brief CBR(CAS before RAS)/auto refresh */
#define TREFMD	0x0
/** @brief 2 clk */
#define Trp	0x0
/** @brief 7 clk */
#define Trc	0x3
/** @brief 3 clk */
#define Tchr	0x2

/**
 * @brief This BSP provides its own IDLE thread to override the RTEMS one.
 *
 *  So we prototype it and define the constant confdefs.h expects
 *  to configure a BSP specific one.
 */
void *bsp_idle_thread(uintptr_t ignored);

/** @} */

#define BSP_IDLE_TASK_BODY bsp_idle_thread

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _BSP_H */

