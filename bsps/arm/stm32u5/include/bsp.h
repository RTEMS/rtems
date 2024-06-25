/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMSTM32U5
 *
 * @brief Global BSP definitions for STM32U5
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_ARM_STM32U5_BSP_H
#define LIBBSP_ARM_STM32U5_BSP_H

#include <bsp/default-initial-extension.h>
#include <bspopts.h>

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSBSPsARMSTM32U5 STM32U5
 *
 * @ingroup RTEMSBSPsARM
 *
 * @brief STM32U5 Board Support Package.
 *
 * @{
 */

#define BSP_FEATURE_IRQ_EXTENSION

#define BSP_ARMV7M_IRQ_PRIORITY_DEFAULT ( 13 << 4 )

#define BSP_ARMV7M_SYSTICK_PRIORITY ( 14 << 4 )

#define BSP_ARMV7M_SYSTICK_FREQUENCY stm32u5_systick_frequency()

uint32_t stm32u5_systick_frequency( void );

/* default functions */
void stm32u5_init_oscillator( void );
void stm32u5_init_clocks( void );
void stm32u5_init_power( void );
void stm32u5_init_peripheral_clocks( void );
void stm32u5_rcc_power_clock_enable( void );
void stm32u5_init_octospi( void );

/* Linker symbols */
extern char stm32u5_memory_null_begin[];
extern char stm32u5_memory_null_end[];
extern char stm32u5_memory_null_size[];

extern char stm32u5_memory_flash_begin[];
extern char stm32u5_memory_flash_end[];
extern char stm32u5_memory_flash_size[];

extern char stm32u5_memory_int_sram_begin[];
extern char stm32u5_memory_int_sram_end[];
extern char stm32u5_memory_int_sram_size[];

extern char stm32u5_memory_int_sram_lpbam_begin[];
extern char stm32u5_memory_int_sram_lpbam_end[];
extern char stm32u5_memory_int_sram_lpbam_size[];

extern char stm32u5_memory_octospi_1_begin[];
extern char stm32u5_memory_octospi_1_end[];
extern char stm32u5_memory_octospi_1_size[];

extern char stm32u5_memory_octospi_2_begin[];
extern char stm32u5_memory_octospi_2_end[];
extern char stm32u5_memory_octospi_2_size[];

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* LIBBSP_ARM_STM32U5_BSP_H */
