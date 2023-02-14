/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsMicroblaze
 *
 * @brief MicroBlaze AXI GPIO implementation
 */

/*
 * Copyright (C) 2022 On-Line Applications Research Corporation (OAR)
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

#include <assert.h>

#include <bsp/fatal.h>
#include <bsp/microblaze-gpio.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

Microblaze_GPIO_context gpio1_context = {
  .regs = (Microblaze_GPIO_registers *) BSP_MICROBLAZE_FPGA_GPIO1_BASE,
#ifdef BSP_MICROBLAZE_FPGA_GPIO1_DUAL_CHANNEL
  .is_dual = true,
#else
  .is_dual = false,
#endif
  .irq = BSP_MICROBLAZE_FPGA_GPIO1_IRQ,
#ifdef BSP_MICROBLAZE_FPGA_GPIO1_INTERRUPT
  .has_interrupts = true
#else
  .has_interrupts = false
#endif
};

#if BSP_MICROBLAZE_FPGA_GPIO2_ENABLED
Microblaze_GPIO_context gpio2_context = {
  .regs = (Microblaze_GPIO_registers *) BSP_MICROBLAZE_FPGA_GPIO2_BASE,
#ifdef BSP_MICROBLAZE_FPGA_GPIO2_DUAL_CHANNEL
  .is_dual = true,
#else
  .is_dual = false,
#endif
  .irq = BSP_MICROBLAZE_FPGA_GPIO2_IRQ,
#ifdef BSP_MICROBLAZE_FPGA_GPIO2_INTERRUPT
  .has_interrupts = true
#else
  .has_interrupts = false
#endif
};
#endif

void microblaze_gpio_set_data_direction(
  Microblaze_GPIO_context *ctx,
  uint32_t                 channel,
  uint32_t                 mask
)
{
  assert( channel == 1 || (ctx->is_dual && channel == 2) );

  if ( channel == 1 ) {
    ctx->regs->gpio_tri = mask;
  } else if ( ctx->is_dual && channel == 2 ) {
    ctx->regs->gpio2_tri = mask;
  }
}

uint32_t microblaze_gpio_get_data_direction(
  Microblaze_GPIO_context *ctx,
  uint32_t                 channel
)
{
  assert( channel == 1 || (ctx->is_dual && channel == 2) );

  if ( channel == 1 ) {
    return ctx->regs->gpio_tri;
  } else if ( ctx->is_dual && channel == 2 ) {
    return ctx->regs->gpio2_tri;
  }

  return 0;
}

uint32_t microblaze_gpio_discrete_read(
  Microblaze_GPIO_context *ctx,
  uint32_t                 channel
)
{
  assert( channel == 1 || (ctx->is_dual && channel == 2) );

  if ( channel == 1 ) {
    return ctx->regs->gpio_data;
  } else if ( ctx->is_dual && channel == 2 ) {
    return ctx->regs->gpio2_tri;
  }

  return 0;
}

void microblaze_gpio_discrete_write(
  Microblaze_GPIO_context *ctx,
  uint32_t                 channel,
  uint32_t                 mask
)
{
  assert( channel == 1 || (ctx->is_dual && channel == 2) );

  if ( channel == 1 ) {
    ctx->regs->gpio_data = mask;
  } else if ( ctx->is_dual && channel == 2 ) {
    ctx->regs->gpio2_tri = mask;
  }
}

void microblaze_gpio_discrete_set(
  Microblaze_GPIO_context *ctx,
  uint32_t                 channel,
  uint32_t                 mask
)
{
  assert( channel == 1 || (ctx->is_dual && channel == 2) );

  if ( channel == 1 ) {
    ctx->regs->gpio_data |= mask;
  } else if ( ctx->is_dual && channel == 2 ) {
    ctx->regs->gpio2_tri |= mask;
  }
}

void microblaze_gpio_discrete_clear(
  Microblaze_GPIO_context *ctx,
  uint32_t                 channel,
  uint32_t                 mask
)
{
  assert( channel == 1 || (ctx->is_dual && channel == 2) );

  if ( channel == 1 ) {
    ctx->regs->gpio_data &= ~mask;
  } else if ( ctx->is_dual && channel == 2 ) {
    ctx->regs->gpio2_tri &= ~mask;
  }
}

rtems_vector_number microblaze_gpio_get_irq( Microblaze_GPIO_context *ctx )
{
  return ctx->irq;
}

void microblaze_gpio_interrupt_global_enable( Microblaze_GPIO_context *ctx )
{
  assert( ctx->has_interrupts );

  if ( ctx->has_interrupts ) {
    ctx->regs->gier = GLOBAL_INTERRUPT_REGISTER_ENABLE;
  }
}

void microblaze_gpio_interrupt_global_disable( Microblaze_GPIO_context *ctx )
{
  assert( ctx->has_interrupts );

  if ( ctx->has_interrupts ) {
    ctx->regs->gier = 0x0;
  }
}

void microblaze_gpio_interrupt_enable(
  Microblaze_GPIO_context *ctx,
  uint32_t                 channel
)
{
  assert( ctx->has_interrupts );
  assert( channel == 1 || (ctx->is_dual && channel == 2) );

  if ( ctx->has_interrupts ) {
    if ( channel == 1 ) {
      ctx->regs->ip_ier |= CHANNEL_1_INTERRUPT_REGISTER;
    } else if ( ctx->is_dual && channel == 2 ) {
      ctx->regs->ip_ier |= CHANNEL_2_INTERRUPT_REGISTER;
    }
  }
}

void microblaze_gpio_interrupt_disable(
  Microblaze_GPIO_context *ctx,
  uint32_t                 channel
)
{
  assert( channel == 1 || (ctx->is_dual && channel == 2) );

  if ( channel == 1 ) {
    ctx->regs->ip_ier &= ~CHANNEL_1_INTERRUPT_REGISTER;
  } else if ( ctx->is_dual && channel == 2 ) {
    ctx->regs->ip_ier &= ~CHANNEL_2_INTERRUPT_REGISTER;
  }
}

void microblaze_gpio_interrupt_clear(
  Microblaze_GPIO_context *ctx,
  uint32_t                 channel
)
{
  assert( channel == 1 || (ctx->is_dual && channel == 2) );

  if ( channel == 1 ) {
    ctx->regs->ip_isr &= CHANNEL_1_INTERRUPT_REGISTER;
  } else if ( ctx->is_dual && channel == 2 ) {
    ctx->regs->ip_isr &= CHANNEL_2_INTERRUPT_REGISTER;
  }
}

uint32_t microblaze_gpio_interrupt_get_enabled( Microblaze_GPIO_context *ctx )
{
  assert( ctx->has_interrupts );

  if ( ctx->has_interrupts ) {
    return ctx->regs->ip_ier;
  }

  return 0;
}

uint32_t microblaze_gpio_interrupt_get_status( Microblaze_GPIO_context *ctx )
{
  assert( ctx->has_interrupts );

  if ( ctx->has_interrupts ) {
    return ctx->regs->ip_isr;
  }

  return 0;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
