/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @ingroup stm32f4_spi
 * @brief STM32F4XXXX SPI register definitions.
 *
 * Register layout matches STM32F4xx Reference Manual (RM0090) section 28.5.
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

#ifndef LIBBSP_ARM_STM32F4_STM32F4XXXX_SPI_H
#define LIBBSP_ARM_STM32F4_STM32F4XXXX_SPI_H

#include <bsp/utility.h>

/**
 * @defgroup stm32f4_spi STM32F4 SPI Support
 * @ingroup RTEMSBSPsARMSTM32F4
 * @brief STM32F4 SPI register definitions.
 * @{
 */

typedef struct {
  uint32_t cr1;
/* Control register 1 (RM0090 28.5.1) */
#define STM32F4_SPI_CR1_BIDIMODE  BSP_BIT32(15) /* Bidirectional mode enable */
#define STM32F4_SPI_CR1_BIDIOE    BSP_BIT32(14) /* Output enable in bidi mode */
#define STM32F4_SPI_CR1_CRCEN     BSP_BIT32(13) /* Hardware CRC enable */
#define STM32F4_SPI_CR1_CRCNEXT   BSP_BIT32(12) /* CRC transfer next */
#define STM32F4_SPI_CR1_DFF       BSP_BIT32(11) /* Data frame format: 0=8b 1=16b */
#define STM32F4_SPI_CR1_RXONLY    BSP_BIT32(10) /* Receive only */
#define STM32F4_SPI_CR1_SSM       BSP_BIT32(9)  /* Software slave management */
#define STM32F4_SPI_CR1_SSI       BSP_BIT32(8)  /* Internal slave select */
#define STM32F4_SPI_CR1_LSBFIRST  BSP_BIT32(7)  /* LSB transmitted first */
#define STM32F4_SPI_CR1_SPE       BSP_BIT32(6)  /* SPI enable */
#define STM32F4_SPI_CR1_BR(val)   BSP_FLD32(val, 3, 5) /* Baud rate control */
#define STM32F4_SPI_CR1_BR_GET(reg) BSP_FLD32GET(reg, 3, 5)
#define STM32F4_SPI_CR1_BR_DIV2   STM32F4_SPI_CR1_BR(0) /* fPCLK/2 */
#define STM32F4_SPI_CR1_BR_DIV4   STM32F4_SPI_CR1_BR(1) /* fPCLK/4 */
#define STM32F4_SPI_CR1_BR_DIV8   STM32F4_SPI_CR1_BR(2) /* fPCLK/8 */
#define STM32F4_SPI_CR1_BR_DIV16  STM32F4_SPI_CR1_BR(3) /* fPCLK/16 */
#define STM32F4_SPI_CR1_BR_DIV32  STM32F4_SPI_CR1_BR(4) /* fPCLK/32 */
#define STM32F4_SPI_CR1_BR_DIV64  STM32F4_SPI_CR1_BR(5) /* fPCLK/64 */
#define STM32F4_SPI_CR1_BR_DIV128 STM32F4_SPI_CR1_BR(6) /* fPCLK/128 */
#define STM32F4_SPI_CR1_BR_DIV256 STM32F4_SPI_CR1_BR(7) /* fPCLK/256 */
#define STM32F4_SPI_CR1_MSTR      BSP_BIT32(2)  /* Master selection */
#define STM32F4_SPI_CR1_CPOL      BSP_BIT32(1)  /* Clock polarity */
#define STM32F4_SPI_CR1_CPHA      BSP_BIT32(0)  /* Clock phase */

  uint32_t cr2;
/* Control register 2 (RM0090 28.5.2) */
#define STM32F4_SPI_CR2_TXEIE     BSP_BIT32(7)  /* TX buffer empty interrupt */
#define STM32F4_SPI_CR2_RXNEIE    BSP_BIT32(6)  /* RX buffer not empty interrupt */
#define STM32F4_SPI_CR2_ERRIE     BSP_BIT32(5)  /* Error interrupt */
#define STM32F4_SPI_CR2_FRF       BSP_BIT32(4)  /* Frame format: 0=Motorola 1=TI */
#define STM32F4_SPI_CR2_SSOE      BSP_BIT32(2)  /* SS output enable */
#define STM32F4_SPI_CR2_TXDMAEN   BSP_BIT32(1)  /* TX DMA enable */
#define STM32F4_SPI_CR2_RXDMAEN   BSP_BIT32(0)  /* RX DMA enable */

  uint32_t sr;
/* Status register (RM0090 28.5.3) */
#define STM32F4_SPI_SR_FRE        BSP_BIT32(8)  /* Frame format error */
#define STM32F4_SPI_SR_BSY        BSP_BIT32(7)  /* Busy flag */
#define STM32F4_SPI_SR_OVR        BSP_BIT32(6)  /* Overrun flag */
#define STM32F4_SPI_SR_MODF       BSP_BIT32(5)  /* Mode fault */
#define STM32F4_SPI_SR_CRCERR     BSP_BIT32(4)  /* CRC error flag */
#define STM32F4_SPI_SR_UDR        BSP_BIT32(3)  /* Underrun flag (I2S only) */
#define STM32F4_SPI_SR_CHSIDE     BSP_BIT32(2)  /* Channel side (I2S only) */
#define STM32F4_SPI_SR_TXE        BSP_BIT32(1)  /* Transmit buffer empty */
#define STM32F4_SPI_SR_RXNE       BSP_BIT32(0)  /* Receive buffer not empty */

  uint32_t dr;       /* Data register */
  uint32_t crcpr;    /* CRC polynomial register (reset: 0x0007) */
  uint32_t rxcrcr;   /* RX CRC register */
  uint32_t txcrcr;   /* TX CRC register */
  uint32_t i2scfgr;  /* I2S configuration register */
  uint32_t i2spr;    /* I2S prescaler register */
} stm32f4_spi;

/** @} */

#endif /* LIBBSP_ARM_STM32F4_STM32F4XXXX_SPI_H */
