/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMSTM32H7
 *
 * @brief This source file contains the shared SPI support code.
 */

/*
 * Copyright (C) 2024 On-Line Applications Research (OAR) Corporation
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp.h>
#include <bsp/fatal.h>
#include <inttypes.h>
#include <rtems/bspIo.h>
#include <rtems/sysinit.h>
#include <stdio.h>

#include <stm32h7/hal.h>
#include <rtems/score/prioritybitmapimpl.h>

/* NULLs are included for disabled devices to preserve index */
static stm32h7_spi_context * const stm32h7_spi_instances[] = {
#ifdef STM32H7_SPI1_ENABLE
 #ifdef SPI1
  &stm32h7_spi1_instance,
 #else
  #error SPI1 configured, but not available
 #endif
#else
  NULL,
#endif

#ifdef STM32H7_SPI2_ENABLE
 #ifdef SPI2
  &stm32h7_spi2_instance,
 #else
  #error SPI2 configured, but not available
 #endif
#else
  NULL,
#endif

#ifdef STM32H7_SPI3_ENABLE
 #ifdef SPI3
  &stm32h7_spi3_instance,
 #else
  #error SPI3 configured, but not available
 #endif
#else
  NULL,
#endif

#ifdef STM32H7_SPI4_ENABLE
 #ifdef SPI4
  &stm32h7_spi4_instance,
 #else
  #error SPI4 configured, but not available
 #endif
#else
  NULL,
#endif

#ifdef STM32H7_SPI5_ENABLE
 #ifdef SPI5
  &stm32h7_spi5_instance,
 #else
  #error SPI5 configured, but not available
 #endif
#else
  NULL,
#endif

#ifdef STM32H7_SPI6_ENABLE
 #ifdef SPI6
  &stm32h7_spi6_instance,
 #else
  #error SPI6 configured, but not available
 #endif
#else
  NULL,
#endif

  /* NULL is included for consistent use of commas above */
  NULL
};

static int stm32h7_spi_set_prescaler(stm32h7_spi_context *ctx, uint32_t speed_hz)
{
  uint32_t prescaler_mask = SPI_BAUDRATEPRESCALER_256;

  /* check speed against max divider (2 is implicit in max_speed_hz) */
  if (speed_hz < (ctx->bus.max_speed_hz / 128)) {
    /* clock rate request too low */
    return 1;
  }

  if (speed_hz > ctx->bus.max_speed_hz) {
    ctx->spi.Instance->CFG1 &= ~prescaler_mask;
    ctx->spi.Instance->CFG1 |= SPI_BAUDRATEPRESCALER_2;
  } else {
    uint32_t divider = 2 * ctx->bus.max_speed_hz / speed_hz;
    uint32_t remainder = (2 * ctx->bus.max_speed_hz) % speed_hz;
    uint32_t prescaler_value;
    if (divider > 256) {
      /* not able to divide enough to accomodate clock rate request */
      return 1;
    }
    /* prescaler values with scale factor N are (log2(N)-1) << 24 */
    prescaler_value = 7 - _Bitfield_Leading_zeros[divider & 0xff];
    if (remainder) {
      prescaler_value++;
    }
    if (prescaler_value > SPI_BAUDRATEPRESCALER_256 >> 24) {
      /* not able to divide enough to accomodate clock rate request */
      return 1;
    }
    prescaler_value <<= 28;
    ctx->spi.Instance->CFG1 &= ~prescaler_mask;
    ctx->spi.Instance->CFG1 |= prescaler_value;
  }

  return 0;
}

static int stm32h7_spi_set_bpw(stm32h7_spi_context *ctx, uint32_t bits_per_word)
{
  uint32_t bits_per_word_mask = SPI_DATASIZE_32BIT;

  if (bits_per_word < 4 || bits_per_word > 32) {
    return 1;
  }

  /*
   * bits per word starts at 4 bpw with register value 3 and counts up to 32 bpw
   * with register value 0x1F (31)
   */
  ctx->spi.Instance->CFG1 &= ~bits_per_word_mask;
  ctx->spi.Instance->CFG1 |= (bits_per_word - 1);

  return 0;
}

static void stm32h7_spi_set_mode(stm32h7_spi_context *ctx, uint32_t mode)
{
  uint32_t mode_mask = SPI_POLARITY_HIGH | SPI_PHASE_2EDGE;
  ctx->spi.Instance->CFG2 &= ~mode_mask;
  if (mode & SPI_CPOL) {
    ctx->spi.Instance->CFG2 |= SPI_POLARITY_HIGH;
  }
  if (mode & SPI_CPHA) {
    ctx->spi.Instance->CFG2 |= SPI_PHASE_2EDGE;
  }

}

static int stm32h7_spi_setup(spi_bus *base)
{
  stm32h7_spi_context *ctx = RTEMS_CONTAINER_OF(base, stm32h7_spi_context, bus);

  if (stm32h7_spi_set_prescaler(ctx, ctx->bus.speed_hz)) {
    return 1;
  }

  if (stm32h7_spi_set_bpw(ctx, ctx->bus.bits_per_word)) {
    return 1;
  }

  stm32h7_spi_set_mode(ctx, ctx->bus.mode);

  return 0;
}

static void stm32h7_spi_destroy(spi_bus *base)
{
  stm32h7_spi_context *ctx = RTEMS_CONTAINER_OF(base, stm32h7_spi_context, bus);

  HAL_SPI_DeInit(&ctx->spi);

  spi_bus_destroy(base);
}

static int stm32h7_spi_get_chip_select(
  stm32h7_spi_context *ctx,
  uint8_t cs,
  GPIO_TypeDef **gpio,
  uint16_t *pin)
{
  const stm32h7_gpio_config *cs_gpio;
  if (cs >= STM32H7_NUM_SOFT_CS) {
    return 1;
  }
  cs_gpio = &ctx->config->cs_gpio[cs];
  if (cs_gpio->regs == NULL || cs_gpio->config.Pin == 0) {
    /* The requested chip select is not configured */
    return 1;
  }
  *gpio = cs_gpio->regs;
  *pin = cs_gpio->config.Pin;
  return 0;
}

static int stm32h7_spi_apply_premessage_settings(
  stm32h7_spi_context *ctx,
  const spi_ioc_transfer *msg
)
{
  uint32_t mode_width = SPI_TX_DUAL | SPI_TX_QUAD | SPI_RX_DUAL | SPI_RX_QUAD;

  if (msg->rx_nbits > 1 || msg->tx_nbits > 1 || (msg->mode & mode_width)) {
    /* This device does not support dual or quad SPI */
    return 1;
  }

  if (stm32h7_spi_set_prescaler(ctx, msg->speed_hz)) {
    return 1;
  }

  if (stm32h7_spi_set_bpw(ctx, msg->bits_per_word)) {
    return 1;
  }

  stm32h7_spi_set_mode(ctx, msg->mode);

  GPIO_TypeDef *gpio_block = NULL;
  uint16_t gpio_pin = 0;
  if (stm32h7_spi_get_chip_select(ctx, msg->cs, &gpio_block, &gpio_pin)) {
    /* Selected GPIO pin not available */
    return 1;
  }
  /* pull chip select low to activate selected device */
  HAL_GPIO_WritePin(gpio_block, gpio_pin, GPIO_PIN_RESET);
  return 0;
}

static void stm32h7_spi_apply_postmessage_settings(
  stm32h7_spi_context *ctx,
  const spi_ioc_transfer *msg,
  bool final
)
{
  usleep(msg->delay_usecs);
  if (msg->cs_change || final) {
    GPIO_TypeDef *gpio_block = NULL;
    uint16_t gpio_pin = 0;

    /*
     * It shouldn't be possible for this to fail since it was already checked in
     * the premessage application
     */
    (void) stm32h7_spi_get_chip_select(ctx, msg->cs, &gpio_block, &gpio_pin);
    /* bring chip select high */
    HAL_GPIO_WritePin(gpio_block, gpio_pin, GPIO_PIN_SET);
  }
}

static int stm32h7_spi_transfer(
  spi_bus *base,
  const spi_ioc_transfer *msgs,
  uint32_t msg_count
)
{
  stm32h7_spi_context *ctx = RTEMS_CONTAINER_OF(base, stm32h7_spi_context, bus);

  for (int i = 0; i < msg_count; i++) {
    const spi_ioc_transfer *msg = &msgs[i];
    HAL_StatusTypeDef status;

    if (stm32h7_spi_apply_premessage_settings(ctx, msg)) {
      return 1;
    }
    /* perform transfer */
    if (msg->tx_buf != NULL && msg->rx_buf != NULL) {
      status = HAL_SPI_TransmitReceive(
        &ctx->spi, msg->tx_buf, msg->rx_buf, msg->len, 100
      );
      if (status != HAL_OK) {
        return 1;
      }
    } else if (msg->tx_buf != NULL) {
      status = HAL_SPI_Transmit(&ctx->spi, msg->tx_buf, msg->len, 100);
      if (status != HAL_OK) {
        return 1;
      }
    } else if (msg->rx_buf != NULL) {
      status = HAL_SPI_Receive(&ctx->spi, msg->rx_buf, msg->len, 100);
      if (status != HAL_OK) {
        return 1;
      }
    }
    /* set final to true on last iteration */
    stm32h7_spi_apply_postmessage_settings(ctx, msg, i == msg_count);
  }

  return 0;
}

static int stm32h7_register_spi_device(
  stm32h7_spi_context *ctx,
  uint8_t device_index
)
{
  char path[sizeof("/dev/spiXXX")];
  int rv;
  spi_bus *bus = &ctx->bus;

  rv = spi_bus_init(bus);
  if (rv) {
    return rv;
  }

  bus->transfer = stm32h7_spi_transfer;
  bus->destroy = stm32h7_spi_destroy;
  bus->setup = stm32h7_spi_setup;
  /*
   * Max speed for these peripherals is 150MHz, but other clock limitations
   * determined by the BSP clock configuration bring that down. The minimum
   * required SPI internal divider is 2 which should be accounted for in the
   * configuration's max_speed_hz parameter.
   */
  bus->max_speed_hz = ctx->config->max_speed_hz;
  /*
   * The stm32h7 SPI peripherals support a single hardware chip select which is
   * not required to be routed to a pin by the configuration since peripheral
   * drivers using the SPI bus will often need to use GPIO to enable
   * peripherals. Since any hardware chip select pin can also be used as GPIO,
   * all chip selects are used in GPIO mode.
   */
  bus->speed_hz = bus->max_speed_hz;
  bus->cs_change = 0;
  bus->cs = 0;
  bus->bits_per_word = ctx->spi.Init.DataSize + 1;
  bus->lsb_first = false;
  if (ctx->spi.Init.FirstBit == SPI_FIRSTBIT_LSB) {
    bus->lsb_first = true;
  }
  bus->mode = 0;
  if (ctx->spi.Init.CLKPolarity == SPI_POLARITY_HIGH) {
    bus->mode |= SPI_CPOL;
  }
  if (ctx->spi.Init.CLKPhase == SPI_PHASE_2EDGE) {
    bus->mode |= SPI_CPHA;
  }

  if (HAL_SPI_Init(&ctx->spi)) {
    return 1;
  }

  snprintf(path, sizeof(path), "/dev/spi%" PRIu8, device_index);
  rv = spi_bus_register(bus, path);
  if (rv) {
    return rv;
  }

  return 0;
}

void stm32h7_register_spi_devices(void)
{
  int i;

  for (i = 0; i < (RTEMS_ARRAY_SIZE(stm32h7_spi_instances)); i++) {
    if (stm32h7_spi_instances[i] == NULL) {
      continue;
    }
    if (stm32h7_register_spi_device(stm32h7_spi_instances[i], i)) {
      bsp_fatal(STM32H7_FATAL_MMU_CANNOT_REGISTER_SPI);
    }
  }
}
