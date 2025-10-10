/* SPDX-License-Identifier: BSD-2-Clause */


/*
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
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
  (void) pin;

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

  sc = rtems_interrupt_server_entry_initialize(ctx->irqs_index,
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
  atsam_sc16is752_spi_context      *ctx,
  const atsam_sc16is752_spi_config *config
)
{
  ctx->base.base.mode = config->mode;
  ctx->base.base.input_frequency = config->input_frequency;
  ctx->base.base.install_irq = atsam_sc16is752_install_interrupt;
  ctx->base.base.remove_irq = atsam_sc16is752_remove_interrupt;
  ctx->base.spi_path = config->spi_path;
  ctx->base.cs = config->spi_chip_select;
  ctx->base.speed_hz = config->spi_speed_hz;
  ctx->irq_pin = config->irq_pin;
  ctx->irqs_index = config->server_index;

  return sc16is752_spi_create(&ctx->base, config->device_path);
}
