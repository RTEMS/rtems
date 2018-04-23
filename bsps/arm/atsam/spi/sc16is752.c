
/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp/sc16is752.h>

#include <rtems/irq-extension.h>

static void atsam_sc16i752_interrupt(void *arg)
{
  atsam_sc16is752_spi_context *ctx = arg;

  sc16is752_interrupt_handler(&ctx->base.base);

  /* Interrupt Status Register shall be read to clear the interrupt flag */
  ctx->irq_pin.pio->PIO_ISR;
}

static bool atsam_sc16is752_install_interrupt(sc16is752_context *base)
{
  atsam_sc16is752_spi_context *ctx = (atsam_sc16is752_spi_context *)base;
  rtems_status_code sc;

  PIO_Configure(&ctx->irq_pin, 1);
  PIO_EnableIt(&ctx->irq_pin);

  sc = rtems_interrupt_server_handler_install(
    RTEMS_ID_NONE,
    ctx->irq_pin.id,
    "Test",
    RTEMS_INTERRUPT_SHARED,
    atsam_sc16i752_interrupt,
    ctx
  );

  return sc == RTEMS_SUCCESSFUL;
}

static void atsam_sc16is752_remove_interrupt(sc16is752_context *base)
{
  atsam_sc16is752_spi_context *ctx = (atsam_sc16is752_spi_context *)base;
  rtems_status_code sc;

  sc = rtems_interrupt_server_handler_remove(
    RTEMS_ID_NONE,
    ctx->irq_pin.id,
    atsam_sc16i752_interrupt,
    ctx
  );
  assert(sc == RTEMS_SUCCESSFUL);
}

int atsam_sc16is752_spi_create(
  atsam_sc16is752_spi_context *ctx,
  const char *device_path,
  sc16is752_mode mode,
  uint32_t input_frequency,
  const char *spi_path,
  uint8_t spi_chip_select,
  uint32_t spi_speed_hz,
  const Pin *irq_pin
)
{
  ctx->base.base.mode = mode;
  ctx->base.base.input_frequency = input_frequency;
  ctx->base.base.install_irq = atsam_sc16is752_install_interrupt;
  ctx->base.base.remove_irq = atsam_sc16is752_remove_interrupt;
  ctx->base.spi_path = spi_path;
  ctx->base.cs = spi_chip_select;
  ctx->base.speed_hz = spi_speed_hz;
  ctx->irq_pin = *irq_pin;

  return sc16is752_spi_create(&ctx->base, device_path);
}
