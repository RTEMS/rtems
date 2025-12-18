/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMEFM32GG11
 *
 * @brief EFM32GG11 I2C Driver
 */

/*
 * Copyright (C) 2025 Allan N. Hessenflow
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

#include <bsp.h>
#include <bsp/processor.h>
#include <bsp/efm32gg11_clock.h>
#include <bsp/efm32gg11_gpio.h>
#include <bsp/i2c.h>
#include <rtems/bspIo.h>
#include <rtems/sysinit.h>
#include <dev/i2c/i2c.h>


#define PASTE2(a,b)          a ## b
#define PASTE3(a,b,c)        a ## b ## c
#define PASTE4(a,b,c,d)      a ## b ## c ## d

#define REGS(i)              PASTE2(I2C, i)
#define IRQN(i)              PASTE3(I2C, i, _IRQn)
#define SCL_PORT(i, l)       PASTE3(AF_I2C, i, _SCL_PORT)(l)
#define SCL_PIN(i, l)        PASTE3(AF_I2C, i, _SCL_PIN)(l)
#define SDA_PORT(i, l)       PASTE3(AF_I2C, i, _SDA_PORT)(l)
#define SDA_PIN(i, l)        PASTE3(AF_I2C, i, _SDA_PIN)(l)
#define HFPERCLKEN0_MASK(i)  PASTE2(CMU_HFPERCLKEN0_I2C, i)


static struct i2c_context_s {
  i2c_bus bus; /* must be first */
  const char *name;
  i2c_msg *msgs;
  uint32_t msg_count;
  uint8_t *buf_ptr;
  uint16_t buf_remaining;
  bool earlyStop;
  int result;
  rtems_binary_semaphore sem;
  unsigned long clock;
  I2C_TypeDef *regs;
  rtems_vector_number irq;
  int scl_pin_loc;
  int scl_port;
  int scl_pin;
  int sda_pin_loc;
  int sda_port;
  int sda_pin;
  uint32_t hfperclken0_mask;
} i2c_instances[] = {
#if defined(EFM32GG11_I2C_0_INDEX) && EFM32GG11_I2C_0_INDEX >= 0
  {
    .name = "/dev/i2c-0",
    .msgs = NULL,
    .earlyStop = false,
    .sem = RTEMS_BINARY_SEMAPHORE_INITIALIZER("i2c0"),
    .clock = I2C_BUS_CLOCK_DEFAULT,
    .regs = REGS(EFM32GG11_I2C_0_INDEX),
    .irq = IRQN(EFM32GG11_I2C_0_INDEX),
    .scl_pin_loc = EFM32GG11_I2C_0_SCL_LOC,
    .scl_port = SCL_PORT(EFM32GG11_I2C_0_INDEX, EFM32GG11_I2C_0_SCL_LOC),
    .scl_pin = SCL_PIN(EFM32GG11_I2C_0_INDEX, EFM32GG11_I2C_0_SCL_LOC),
    .sda_pin_loc = EFM32GG11_I2C_0_SDA_LOC,
    .sda_port = SDA_PORT(EFM32GG11_I2C_0_INDEX, EFM32GG11_I2C_0_SDA_LOC),
    .sda_pin = SDA_PIN(EFM32GG11_I2C_0_INDEX, EFM32GG11_I2C_0_SDA_LOC),
    .hfperclken0_mask = HFPERCLKEN0_MASK(EFM32GG11_I2C_0_INDEX)
  },
#endif
#if defined(EFM32GG11_I2C_1_INDEX) && EFM32GG11_I2C_1_INDEX >= 0
  {
    .name = "/dev/i2c-1",
    .msgs = NULL,
    .earlyStop = false,
    .sem = RTEMS_BINARY_SEMAPHORE_INITIALIZER("i2c1"),
    .clock = I2C_BUS_CLOCK_DEFAULT,
    .regs = REGS(EFM32GG11_I2C_1_INDEX),
    .irq = IRQN(EFM32GG11_I2C_1_INDEX),
    .scl_pin_loc = EFM32GG11_I2C_1_SCL_LOC,
    .scl_port = SCL_PORT(EFM32GG11_I2C_1_INDEX, EFM32GG11_I2C_1_SCL_LOC),
    .scl_pin = SCL_PIN(EFM32GG11_I2C_1_INDEX, EFM32GG11_I2C_1_SCL_LOC),
    .sda_pin_loc = EFM32GG11_I2C_1_SDA_LOC,
    .sda_port = SDA_PORT(EFM32GG11_I2C_1_INDEX, EFM32GG11_I2C_1_SDA_LOC),
    .sda_pin = SDA_PIN(EFM32GG11_I2C_1_INDEX, EFM32GG11_I2C_1_SDA_LOC),
    .hfperclken0_mask = HFPERCLKEN0_MASK(EFM32GG11_I2C_1_INDEX)
  },
#endif
#if defined(EFM32GG11_I2C_2_INDEX) && EFM32GG11_I2C_2_INDEX >= 0
  {
    .name = "/dev/i2c-2",
    .msgs = NULL,
    .earlyStop = false,
    .sem = RTEMS_BINARY_SEMAPHORE_INITIALIZER("i2c2"),
    .clock = I2C_BUS_CLOCK_DEFAULT,
    .regs = REGS(EFM32GG11_I2C_2_INDEX),
    .irq = IRQN(EFM32GG11_I2C_2_INDEX),
    .scl_pin_loc = EFM32GG11_I2C_2_SCL_LOC,
    .scl_port = SCL_PORT(EFM32GG11_I2C_2_INDEX, EFM32GG11_I2C_2_SCL_LOC),
    .scl_pin = SCL_PIN(EFM32GG11_I2C_2_INDEX, EFM32GG11_I2C_2_SCL_LOC),
    .sda_pin_loc = EFM32GG11_I2C_2_SDA_LOC,
    .sda_port = SDA_PORT(EFM32GG11_I2C_2_INDEX, EFM32GG11_I2C_2_SDA_LOC),
    .sda_pin = SDA_PIN(EFM32GG11_I2C_2_INDEX, EFM32GG11_I2C_2_SDA_LOC),
    .hfperclken0_mask = HFPERCLKEN0_MASK(EFM32GG11_I2C_2_INDEX)
  },
#endif
};


static void start(struct i2c_context_s *ctx, uint32_t extra_commands)
{
  I2C_TypeDef *regs = ctx->regs;
  rtems_interrupt_level level;
  uint32_t r;

  r = ctx->msgs->addr << 1;
  if (ctx->msgs->flags & I2C_M_RD)
    r |= 0x01;
  r = (r << _I2C_TXDATA_TXDATA_SHIFT) & _I2C_TXDATA_TXDATA_MASK;
  rtems_interrupt_disable(level);
  regs->CMD = I2C_CMD_START | extra_commands;
  regs->TXDATA = r;
  rtems_interrupt_enable(level);
}

static void continuation_buffer(struct i2c_context_s *ctx) {

  while (ctx->msg_count > 1 && !(ctx->msgs->flags & I2C_M_STOP) &&
         (ctx->msgs[1].flags & I2C_M_NOSTART) && ctx->buf_remaining == 0) {
    ctx->msgs++;
    ctx->msg_count--;
    ctx->buf_ptr = ctx->msgs->buf;
    ctx->buf_remaining = ctx->msgs->len;
  }
}

static void next_message(struct i2c_context_s *ctx) {

  while (ctx->msg_count) {
    ctx->msgs++;
    ctx->msg_count--;
    if (ctx->msgs->len ||
        (ctx->msgs->flags & I2C_M_STOP) || !(ctx->msgs->flags & I2C_M_NOSTART))
      break;
  }
  if (ctx->msg_count) {
    ctx->buf_ptr = ctx->msgs->buf;
    ctx->buf_remaining = ctx->msgs->len;
  } else {
    ctx->buf_remaining = 0;
  }
}

static void i2c_interrupt(void *arg)
{
  i2c_bus *bus = (i2c_bus *) arg;
  struct i2c_context_s *ctx = (struct i2c_context_s *) bus;
  I2C_TypeDef *regs = ctx->regs;
  uint32_t iflags;
  uint32_t r;
  uint8_t c;

  iflags = regs->IF;
  regs->IFC = iflags & (I2C_IF_CLERR | I2C_IF_RXFULL | I2C_IF_BUSERR |
                        I2C_IF_ARBLOST | I2C_IF_MSTOP |
                        I2C_IF_NACK | I2C_IF_ACK);

  if (iflags & (I2C_IF_RXDATAV | I2C_IF_RXFULL))
    c = regs->RXDATA;

  if (ctx->earlyStop) {
    if (iflags & I2C_IF_MSTOP) {
      ctx->result = -EIO;
      ctx->earlyStop = false;
      rtems_binary_semaphore_post(&ctx->sem);
    }
  } else if (iflags & (I2C_IF_ARBLOST | I2C_IF_CLERR | I2C_IF_BUSERR)) {
    regs->CMD = I2C_CMD_ABORT;
    ctx->result = -EIO;
    rtems_binary_semaphore_post(&ctx->sem);
  } else {
    if (iflags & I2C_IF_RXDATAV) {
      if (ctx->buf_remaining == 0)
        continuation_buffer(ctx);
      if (ctx->buf_remaining) {
        *ctx->buf_ptr++ = c;
        ctx->buf_remaining--;
        if (ctx->buf_remaining == 0)
          continuation_buffer(ctx);
      } else {
        /* it should be impossible to get here */
      }
      if (ctx->buf_remaining) {
        regs->CMD = I2C_CMD_ACK;
      } else {
        if (ctx->msg_count == 1 || (ctx->msgs->flags & I2C_M_STOP)) {
          regs->CMD = I2C_CMD_NACK | I2C_CMD_STOP;
        } else {
          next_message(ctx);
          start(ctx, I2C_CMD_NACK);
        }
      }
    }
    if ((iflags & I2C_IF_NACK) &&
        !(ctx->msgs->flags & I2C_M_IGNORE_NAK)) {
      ctx->earlyStop = true;
      regs->CMD = I2C_CMD_STOP;
    } else if (iflags & (I2C_IF_ACK | I2C_IF_NACK)) {
      if (ctx->buf_remaining == 0)
        continuation_buffer(ctx);
      if (ctx->buf_remaining == 0) {
        if ((ctx->msgs->flags & I2C_M_STOP) || ctx->msg_count == 1) {
          regs->CMD = I2C_CMD_STOP;
        } else {
          next_message(ctx);
          start(ctx, 0);
        }
      } else if (!(ctx->msgs->flags & I2C_M_RD)) {
        r = *ctx->buf_ptr++;
        ctx->buf_remaining--;
        r = (r << _I2C_TXDATA_TXDATA_SHIFT) &
            _I2C_TXDATA_TXDATA_MASK;
        regs->TXDATA = r;
      } else {
        /* address sent, transition to reading */
        if (iflags & I2C_IF_NACK)
          regs->CMD = I2C_CMD_CONT;
      }
    }
    if (iflags & I2C_IF_MSTOP) {
      next_message(ctx);
      if (ctx->msg_count == 0) {
        ctx->result = 0;
        rtems_binary_semaphore_post(&ctx->sem);
      } else {
        start(ctx, 0);
      }
    }
  }
}

#define NHIGH 4
#define NLOW  4

static void clock_set(struct i2c_context_s *ctx)
{
  I2C_TypeDef *regs = ctx->regs;
  uint32_t div;

  div = (efm32gg11_clock.hfpercclk - 8 * ctx->clock - 1) /
        ((NHIGH + NLOW) * ctx->clock);

  regs->CLKDIV = div;
}

static void i2c_init(struct i2c_context_s *ctx)
{
  I2C_TypeDef *regs = ctx->regs;
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  CMU->HFPERCLKEN0 |= ctx->hfperclken0_mask;
  rtems_interrupt_enable(level);

  clock_set(ctx);

  regs->IEN = 0;
#if NHIGH != 4 || NLOW != 4
  /* should set CLHR based on these and only error out if they're an
     unsupported combination */
#error
#endif
  regs->CTRL =
    I2C_CTRL_CLTO_OFF |
    I2C_CTRL_BITO_OFF |
    I2C_CTRL_CLHR_STANDARD |
    I2C_CTRL_TXBIL_EMPTY |
    I2C_CTRL_EN;
  regs->CMD = I2C_CMD_ABORT;

  regs->ROUTELOC0 = ((ctx->scl_pin_loc << _I2C_ROUTELOC0_SCLLOC_SHIFT) &
                     _I2C_ROUTELOC0_SCLLOC_MASK) |
                    ((ctx->sda_pin_loc << _I2C_ROUTELOC0_SDALOC_SHIFT) &
                     _I2C_ROUTELOC0_SDALOC_MASK);

  efm32gg11_gpio_clear_set(ctx->scl_port, 0, 1 << ctx->scl_pin);
  efm32gg11_gpio_mode(ctx->scl_port, ctx->scl_pin,
                      GPIO_P_MODEL_MODE0_WIREDAND);
  efm32gg11_gpio_clear_set(ctx->sda_port, 0, 1 << ctx->sda_pin);
  efm32gg11_gpio_mode(ctx->sda_port, ctx->sda_pin,
                      GPIO_P_MODEL_MODE0_WIREDAND);
  regs->ROUTEPEN = I2C_ROUTEPEN_SCLPEN | I2C_ROUTEPEN_SDAPEN;

  regs->IEN = I2C_IEN_CLERR | I2C_IEN_RXFULL | I2C_IEN_BUSERR |
              I2C_IEN_ARBLOST | I2C_IEN_MSTOP |
              I2C_IEN_NACK | I2C_IEN_ACK |
              I2C_IEN_RXDATAV;
}

static int i2c_transfer(i2c_bus *bus, i2c_msg *msgs, uint32_t msg_count)
{
  struct i2c_context_s *ctx = (struct i2c_context_s *) bus;
  uint16_t prev_flags;
  uint16_t supported_flags;
  uint32_t i;
  int sc;

  /* Several of the additional flags could be supported, but I don't
     need them and even if I were to write code to support them I wouldn't
     have a quick and easy way to test. */
  supported_flags = I2C_M_RD | I2C_M_STOP | I2C_M_IGNORE_NAK;
  prev_flags = msgs[0].flags; /* silence compiler warning */
  for (i = 0; i < msg_count; i++) {
    if (msgs[i].flags & ~supported_flags)
      return -EINVAL;
    if (msgs[i].flags & I2C_M_NOSTART) {
      if ((prev_flags & I2C_M_RD) != (msgs[i].flags & I2C_M_RD) ||
          (prev_flags & I2C_M_STOP))
        return -EINVAL;
    }
    prev_flags = msgs[i].flags;
    supported_flags |= I2C_M_NOSTART;
  }
  ctx->msgs = msgs;
  ctx->msg_count = msg_count;
  ctx->buf_ptr = msgs->buf;
  ctx->buf_remaining = msgs->len;
  ctx->result = 0;

  rtems_binary_semaphore_try_wait(&ctx->sem);
  start(ctx, 0);

  sc = rtems_binary_semaphore_wait_timed_ticks(&ctx->sem, bus->timeout);
  if (sc) {
    ctx->regs->CMD = I2C_CMD_ABORT;
    sc = -ETIMEDOUT;
  }

  if (sc == 0)
    sc = ctx->result;

  return sc;
}

static int i2c_set_clock(i2c_bus *bus, unsigned long clock)
{
  struct i2c_context_s *ctx = (struct i2c_context_s *) bus;

  ctx->clock = clock;
  clock_set(ctx);

  return 0;
}

static void i2c_destroy(i2c_bus *bus)
{
  struct i2c_context_s *ctx = (struct i2c_context_s *) bus;
  rtems_interrupt_level level;

  ctx->regs->IEN = 0;
  ctx->regs->CTRL = 0;
  __DSB();
  rtems_interrupt_handler_remove(ctx->irq, i2c_interrupt, bus);
  rtems_binary_semaphore_destroy(&ctx->sem);

  rtems_interrupt_disable(level);
  CMU->HFPERCLKEN0 &= ~ctx->hfperclken0_mask;
  rtems_interrupt_enable(level);
}

int efm32gg11_i2c_register()
{
  rtems_status_code sc;
  struct i2c_context_s *ctx;
  size_t i;

  sc = RTEMS_SUCCESSFUL;
  for (i = 0; i < RTEMS_ARRAY_SIZE(i2c_instances); i++) {
    ctx = &i2c_instances[i];
    i2c_bus_init(&ctx->bus);
    rtems_interrupt_handler_install(ctx->irq, "I2C", RTEMS_INTERRUPT_UNIQUE,
                                    i2c_interrupt, &ctx->bus);
    i2c_init(ctx);
    ctx->bus.transfer = i2c_transfer;
    ctx->bus.set_clock = i2c_set_clock;
    ctx->bus.destroy = i2c_destroy;

    sc = i2c_bus_register(&ctx->bus, ctx->name);
  }

  return sc;
}
