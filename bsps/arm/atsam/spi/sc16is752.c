
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

static void atsam_sc16i752_irqs_handler(void *arg)
{
  atsam_sc16is752_spi_context *ctx = arg;

    sc16is752_interrupt_handler(&ctx->base.base);
    PIO_EnableIt(&ctx->irq_pin);
}

static void atsam_sc16i752_interrupt(const Pin *pin, void *arg)
{
  atsam_sc16is752_spi_context *ctx = arg;

  if(PIO_ItIsActive(&ctx->irq_pin)) {
    PIO_DisableIt(&ctx->irq_pin);
    rtems_interrupt_server_entry_submit(&ctx->irqs_entry);
  }
}

static bool atsam_sc16is752_install_interrupt(sc16is752_context *base)
{
  atsam_sc16is752_spi_context *ctx = (atsam_sc16is752_spi_context *)base;
  rtems_status_code sc;
  uint8_t rv;

  sc = rtems_interrupt_server_entry_initialize(RTEMS_INTERRUPT_SERVER_DEFAULT,
    &ctx->irqs_entry);
  rtems_interrupt_server_action_prepend(&ctx->irqs_entry,
    &ctx->irqs_action, atsam_sc16i752_irqs_handler, ctx);

  rv = PIO_Configure(&ctx->irq_pin, 1);
  PIO_ConfigureIt(&ctx->irq_pin, atsam_sc16i752_interrupt, ctx);
  PIO_EnableIt(&ctx->irq_pin);

  return (sc == RTEMS_SUCCESSFUL) && (rv == 1);
}

static void atsam_sc16is752_remove_interrupt(sc16is752_context *base)
{
  atsam_sc16is752_spi_context *ctx = (atsam_sc16is752_spi_context *)base;
  rtems_status_code sc;

  PIO_DisableIt(&ctx->irq_pin);
  sc = PIO_RemoveIt(&ctx->irq_pin, atsam_sc16i752_interrupt, ctx);
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
