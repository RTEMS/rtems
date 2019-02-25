/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC24XXI2C
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2009, 2019 embedded brains GmbH
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

#include <bsp/i2c.h>
#include <bsp.h>
#include <bsp/io.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>

#include <rtems/score/assert.h>

#include <dev/i2c/i2c.h>

RTEMS_STATIC_ASSERT(I2C_M_RD == 1, lpc24xx_i2c_read_flag);

typedef struct {
  i2c_bus base;
  volatile lpc24xx_i2c *regs;
  uint8_t *buf;
  const uint8_t *buf_end;
  size_t todo;
  const i2c_msg *msg;
  const i2c_msg *msg_end;
  int error;
  rtems_binary_semaphore sem;
  lpc24xx_module module;
  rtems_vector_number irq;
} lpc24xx_i2c_bus;

typedef struct {
  volatile lpc24xx_i2c *regs;
  lpc24xx_module module;
  rtems_vector_number irq;
} lpc24xx_i2c_config;

static const i2c_msg *lpc24xx_i2c_msg_inc(lpc24xx_i2c_bus *bus)
{
  const i2c_msg *msg;

  msg = bus->msg + 1;
  bus->msg = msg;
  return msg;
}

static void lpc24xx_i2c_msg_inc_and_set_buf(lpc24xx_i2c_bus *bus)
{
  const i2c_msg *msg;

  msg = lpc24xx_i2c_msg_inc(bus);
  bus->buf = msg->buf;
  bus->buf_end = bus->buf + msg->len;
}

static void lpc24xx_i2c_buf_inc(lpc24xx_i2c_bus *bus)
{
  ++bus->buf;
  --bus->todo;
}

static void lpc24xx_i2c_buf_push(lpc24xx_i2c_bus *bus, uint8_t c)
{
  while (true) {
    if (bus->buf != bus->buf_end) {
      bus->buf[0] = c;
      lpc24xx_i2c_buf_inc(bus);
      break;
    }

    lpc24xx_i2c_msg_inc_and_set_buf(bus);
  }
}

static uint8_t lpc24xx_i2c_buf_pop(lpc24xx_i2c_bus *bus)
{
  while (true) {
    if (bus->buf != bus->buf_end) {
      uint8_t c;

      c = bus->buf[0];
      lpc24xx_i2c_buf_inc(bus);
      return c;
    }

    lpc24xx_i2c_msg_inc_and_set_buf(bus);
  }
}

static void lpc24xx_i2c_setup_msg(lpc24xx_i2c_bus *bus, const i2c_msg *msg)
{
  int can_continue;
  size_t todo;

  bus->msg = msg;
  bus->buf = msg->buf;
  todo = msg->len;
  bus->buf_end = bus->buf + todo;

  can_continue = (msg->flags & I2C_M_RD) | I2C_M_NOSTART;
  ++msg;

  while (msg != bus->msg_end) {
    if ((msg->flags & (I2C_M_RD | I2C_M_NOSTART)) != can_continue) {
      break;
    }

    todo += msg->len;
    ++msg;
  }

  bus->todo = todo;
}

static int lpc24xx_i2c_next_msg(
  lpc24xx_i2c_bus *bus,
  volatile lpc24xx_i2c *regs
)
{
  const i2c_msg *msg;
  int error;

  msg = bus->msg + 1;
  error = 1;

  if (msg != bus->msg_end) {
    lpc24xx_i2c_setup_msg(bus, msg);

    if ((msg->flags & I2C_M_NOSTART) == 0) {
      regs->conset = LPC24XX_I2C_STA;
      regs->conclr = LPC24XX_I2C_SI;
    } else {
      regs->conset = LPC24XX_I2C_STO;
      regs->conclr = LPC24XX_I2C_SI;
      error = -EINVAL;
    }
  } else {
    regs->conset = LPC24XX_I2C_STO;
    regs->conclr = LPC24XX_I2C_SI;
    error = 0;
  }

  return error;
}

static void lpc24xx_i2c_interrupt(void *arg)
{
  lpc24xx_i2c_bus *bus;
  volatile lpc24xx_i2c *regs;
  const i2c_msg *msg;
  int error;

  bus = arg;
  regs = bus->regs;
  error = 1;

  switch (regs->stat) {
    case 0x00:
      /* Bus error */
    case 0x20:
      /* Slave address plus write sent, NACK received */
    case 0x48:
      /* Slave address plus read sent, NACK received */
      regs->conset = LPC24XX_I2C_STO | LPC24XX_I2C_AA;
      regs->conclr = LPC24XX_I2C_SI;
      error = -EIO;
      break;
    case 0x08:
      /* Start sent */
    case 0x10:
      /* Repeated start sent */
      msg = bus->msg;
      regs->dat = (uint8_t) ((msg->addr << 1) | (msg->flags & I2C_M_RD));
      regs->conset = LPC24XX_I2C_AA;
      regs->conclr = LPC24XX_I2C_STA | LPC24XX_I2C_SI;
      break;
    case 0x18:
      /* Slave address plus write sent, ACK received */
    case 0x28:
      /* Data sent, ACK received */
      if (bus->todo > 0) {
        regs->dat = lpc24xx_i2c_buf_pop(bus);
        regs->conset = LPC24XX_I2C_AA;
        regs->conclr = LPC24XX_I2C_SI;
      } else {
        error = lpc24xx_i2c_next_msg(bus, regs);
      }
      break;
    case 0x30:
      /* Data sent, NACK received */
      if (bus->todo == 0) {
        error = lpc24xx_i2c_next_msg(bus, regs);
      } else {
        regs->conset = LPC24XX_I2C_STO;
        regs->conclr = LPC24XX_I2C_SI;
        error = -EIO;
      }
      break;
    case 0x40:
      /* Slave address plus read sent, ACK received */
      if (bus->todo > 1) {
        regs->conset = LPC24XX_I2C_AA;
        regs->conclr = LPC24XX_I2C_SI;
      } else {
        regs->conclr = LPC24XX_I2C_SI | LPC24XX_I2C_AA;
      }
      break;
    case 0x50:
      /* Data received, ACK returned */
    case 0x58:
      /* Data received, NACK returned */
      lpc24xx_i2c_buf_push(bus, regs->dat);

      if (bus->todo > 1) {
        regs->conset = LPC24XX_I2C_AA;
        regs->conclr = LPC24XX_I2C_SI;
      } else if (bus->todo == 1) {
        regs->conclr = LPC24XX_I2C_SI | LPC24XX_I2C_AA;
      } else {
        error = lpc24xx_i2c_next_msg(bus, regs);
      }
      break;
    case 0xF8:
      /* Do nothing */
      break;
    default:
      error = -EIO;
      break;
  }

  if (error <= 0) {
    bus->error = error;
    bsp_interrupt_vector_disable(bus->irq);
    rtems_binary_semaphore_post(&bus->sem);
  }
}

static int
lpc24xx_i2c_transfer(i2c_bus *base, i2c_msg *msgs, uint32_t msg_count)
{
  lpc24xx_i2c_bus *bus;
  volatile lpc24xx_i2c *regs;
  uint16_t supported;
  uint32_t i;
  int eno;

  if (msg_count == 0){
    return 0;
  }

  supported = I2C_M_RD;

  for (i = 0; i < msg_count; ++i) {
    if ((msgs[i].flags & ~supported) != 0) {
      return -EINVAL;
    }

    supported |= I2C_M_NOSTART;
  }

  bus = (lpc24xx_i2c_bus *) base;
  bus->msg_end = msgs + msg_count;
  lpc24xx_i2c_setup_msg(bus, msgs);

  regs = bus->regs;

  /* Start */
  regs->conset = LPC24XX_I2C_STA;

  bsp_interrupt_vector_enable(bus->irq);
  eno = rtems_binary_semaphore_wait_timed_ticks(
    &bus->sem,
    bus->base.timeout
  );
  if (eno != 0) {
    regs->conclr = LPC24XX_I2C_EN;
    regs->conset = LPC24XX_I2C_EN;
    rtems_binary_semaphore_try_wait(&bus->sem);
    return -ETIMEDOUT;
  }

  return bus->error;
}

/* I2C-Bus Specification and User Manual, Table 10 */
static const uint16_t lpc24xx_i2c_t_low_high[3][2] = {
  { 4700, 4000 },
  { 1300,  600 },
  {  500,  260 }
};

static uint32_t lpc24xx_i2c_cycle_count(uint32_t scl, uint32_t x, uint32_t t)
{
  scl = (scl * x + t - 1) / t;

  if (scl <= 4) {
    scl = 4;
  } else if (scl >= 0xffff) {
    scl = 0xffff;
  }

  return scl;
}

static int lpc24xx_i2c_set_clock(i2c_bus *base, unsigned long clock)
{
  lpc24xx_i2c_bus *bus;
  volatile lpc24xx_i2c *regs;
  size_t i;
  uint32_t low;
  uint32_t high;
  uint32_t t;
  uint32_t scl;

  if (clock <= 100000) {
    i = 0;
  } else if (clock <= 400000) {
    i = 1;
  } else {
    i = 2;
  }

  low = lpc24xx_i2c_t_low_high[i][0];
  high = lpc24xx_i2c_t_low_high[i][1];
  t = low + high;
  scl = (LPC24XX_PCLK + clock - 1) / clock;

  bus = (lpc24xx_i2c_bus *) base;
  regs = bus->regs;

  regs->scll = lpc24xx_i2c_cycle_count(scl, low, t);
  regs->sclh = lpc24xx_i2c_cycle_count(scl, high, t);

  return 0;
}

static void
lpc24xx_i2c_destroy(i2c_bus *base)
{
  lpc24xx_i2c_bus *bus;
  rtems_status_code sc;

  bus = (lpc24xx_i2c_bus *) base;

  sc = rtems_interrupt_handler_remove(bus->irq, lpc24xx_i2c_interrupt, bus);
  _Assert(sc == RTEMS_SUCCESSFUL);
  (void) sc;

  /* Disable I2C module */
  bus->regs->conclr = LPC24XX_I2C_EN;

  sc = lpc24xx_module_disable(bus->module);
  _Assert(sc == RTEMS_SUCCESSFUL);
  (void) sc;

  rtems_binary_semaphore_destroy(&bus->sem);
  i2c_bus_destroy_and_free(&bus->base);
}

static int lpc24xx_i2c_init(lpc24xx_i2c_bus *bus)
{
  rtems_status_code sc;

  sc = lpc24xx_module_enable(bus->module, LPC24XX_MODULE_PCLK_DEFAULT);
  _Assert(sc == RTEMS_SUCCESSFUL);
  (void) sc;

  /* Disable I2C module */
  bus->regs->conclr = LPC24XX_I2C_EN;

  sc = rtems_interrupt_handler_install(
    bus->irq,
    "I2C",
    RTEMS_INTERRUPT_UNIQUE,
    lpc24xx_i2c_interrupt,
    bus
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return EAGAIN;
  }

  rtems_binary_semaphore_init(&bus->sem, "I2C");

  lpc24xx_i2c_set_clock(&bus->base, I2C_BUS_CLOCK_DEFAULT);

  /* Initialize I2C module */
  bus->regs->conset = LPC24XX_I2C_EN;

  return 0;
}

static int i2c_bus_register_lpc24xx(
  const char *bus_path,
  const lpc24xx_i2c_config *config
)
{
  lpc24xx_i2c_bus *bus;
  int eno;

  bus = (lpc24xx_i2c_bus *) i2c_bus_alloc_and_init(sizeof(*bus));
  if (bus == NULL){
    return -1;
  }

  bus->regs = config->regs;
  bus->module = config->module;
  bus->irq = config->irq;

  eno = lpc24xx_i2c_init(bus);
  if (eno != 0) {
    (*bus->base.destroy)(&bus->base);
    rtems_set_errno_and_return_minus_one(eno);
  }

  bus->base.transfer = lpc24xx_i2c_transfer;
  bus->base.set_clock = lpc24xx_i2c_set_clock;
  bus->base.destroy = lpc24xx_i2c_destroy;

  return i2c_bus_register(&bus->base, bus_path);
}

int lpc24xx_register_i2c_0(void)
{
  static const lpc24xx_i2c_config config = {
    .regs = (volatile lpc24xx_i2c *) I2C0_BASE_ADDR,
    .module = LPC24XX_MODULE_I2C_0,
    .irq = LPC24XX_IRQ_I2C_0
  };

  return i2c_bus_register_lpc24xx(
    LPC24XX_I2C_0_BUS_PATH,
    &config
  );
}

int lpc24xx_register_i2c_1(void)
{
  static const lpc24xx_i2c_config config = {
    .regs = (volatile lpc24xx_i2c *) I2C1_BASE_ADDR,
    .module = LPC24XX_MODULE_I2C_1,
    .irq = LPC24XX_IRQ_I2C_1
  };

  return i2c_bus_register_lpc24xx(
    LPC24XX_I2C_2_BUS_PATH,
    &config
  );
}

int lpc24xx_register_i2c_2(void)
{
  static const lpc24xx_i2c_config config = {
    .regs = (volatile lpc24xx_i2c *) I2C2_BASE_ADDR,
    .module = LPC24XX_MODULE_I2C_2,
    .irq = LPC24XX_IRQ_I2C_2
  };

  return i2c_bus_register_lpc24xx(
    LPC24XX_I2C_2_BUS_PATH,
    &config
  );
}
