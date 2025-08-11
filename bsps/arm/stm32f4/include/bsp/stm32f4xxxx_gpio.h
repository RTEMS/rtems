/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @ingroup stm32f4_gpio
 * @brief STM32F4XXXX GPIO support.
 */

/*
 * Copyright (C) 2012 Sebastian Huber
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

#ifndef LIBBSP_ARM_STM32F4_STM32F4XXXX_GPIO_H
#define LIBBSP_ARM_STM32F4_STM32F4XXXX_GPIO_H

#include <bsp/utility.h>

typedef struct {
	uint32_t moder;
	uint32_t otyper;
	uint32_t ospeedr;
	uint32_t pupdr;
	uint32_t idr;
	uint32_t odr;
	uint32_t bsrr;
	uint32_t lckr;
	uint32_t afr [2];
	uint32_t reserved_28 [246];
} stm32f4_gpio;

#endif /* LIBBSP_ARM_STM32F4_STM32F4XXXX_GPIO_H */
