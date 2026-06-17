/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @ingroup stm32f4_spi
 * @brief STM32F4 SPI bus driver interface.
 */

/*
 * Copyright (C) 2026 Moksh Panicker
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

#ifndef LIBBSP_ARM_STM32F4_STM32F4_SPI_H
#define LIBBSP_ARM_STM32F4_STM32F4_SPI_H

#include <bsp/stm32f4xxxx_spi.h>
#include <bsp/rcc.h>
#include <dev/spi/spi.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup stm32f4_spi
 * @{
 */

/**
 * @brief Register and initialise an STM32F4 SPI bus.
 *
 * @param regs   Pointer to the SPI peripheral registers.
 * @param irq    IRQ vector number (STM32F4_IRQ_SPI1, _SPI2, or _SPI3).
 * @param rcc    RCC clock enable index for this peripheral.
 * @param pclk   APB clock driving this peripheral in Hz.
 * @param path   RTEMS filesystem path (e.g. "/dev/spi0").
 *
 * @retval 0      Success.
 * @retval non-0  Error; the bus was not registered.
 */
int stm32f4_spi_init(
  volatile stm32f4_spi *regs,
  rtems_vector_number   irq,
  stm32f4_rcc_index     rcc,
  uint32_t              pclk,
  const char           *path
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* LIBBSP_ARM_STM32F4_STM32F4_SPI_H */
