/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2014 embedded brains GmbH
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

#include <bsp/cadence-i2c.h>
#include <bsp/cadence-i2c-regs.h>

#include <rtems/irq-extension.h>
#include <rtems/score/assert.h>

#include <dev/i2c/i2c.h>

#define CADENCE_I2C_DIV_A_MAX 4

#define CADENCE_I2C_DIV_B_MAX 64

#define CADENCE_I2C_FIFO_DEPTH 16

#define CADENCE_I2C_DATA_IRQ_DEPTH (CADENCE_I2C_FIFO_DEPTH - 2)

#define CADENCE_I2C_TRANSFER_SIZE_MAX 255

#define CADENCE_I2C_TRANSFER_SIZE_ONCE_MAX (18 * CADENCE_I2C_DATA_IRQ_DEPTH)

#define CADENCE_I2C_IRQ_ERROR \
  (CADENCE_I2C_IXR_ARB_LOST \
    | CADENCE_I2C_IXR_RX_UNF \
    | CADENCE_I2C_IXR_TX_OVR \
    | CADENCE_I2C_IXR_RX_OVR \
    | CADENCE_I2C_IXR_NACK)

#define CADENCE_I2C_IRQ_USED \
  (CADENCE_I2C_IRQ_ERROR \
    | CADENCE_I2C_IXR_DATA \
    | CADENCE_I2C_IXR_COMP)

typedef struct {
  i2c_bus base;
  volatile cadence_i2c *regs;
  i2c_msg *msgs;
  uint32_t msg_todo;
  uint32_t current_msg_todo;
  uint8_t *current_msg_byte;
  uint32_t current_todo;
  uint32_t irqstatus;
  bool read;
  bool hold;
  rtems_id task_id;
  uint32_t input_clock;
  rtems_vector_number irq;
} cadence_i2c_bus;

static void cadence_i2c_disable_interrupts(volatile cadence_i2c *regs)
{
  regs->irqdisable = 0xffff;
}

static void cadence_i2c_clear_irq_status(volatile cadence_i2c *regs)
{
  regs->irqstatus = regs->irqstatus;
}

static void cadence_i2c_reset(cadence_i2c_bus *bus)
{
  volatile cadence_i2c *regs = bus->regs;
  uint32_t val;

  cadence_i2c_disable_interrupts(regs);

  val = regs->control;
  val &= ~CADENCE_I2C_CONTROL_HOLD;
  val |= CADENCE_I2C_CONTROL_ACKEN
    | CADENCE_I2C_CONTROL_MS
    | CADENCE_I2C_CONTROL_CLR_FIFO;
  regs->control = val;

  regs->transfer_size = 0;
  regs->status = regs->status;

  cadence_i2c_clear_irq_status(regs);
}

static uint32_t cadence_i2c_set_address_size(
  const i2c_msg *msg,
  uint32_t control
)
{
  if ((msg->flags & I2C_M_TEN) == 0) {
    control |= CADENCE_I2C_CONTROL_NEA;
  } else {
    control &= ~CADENCE_I2C_CONTROL_NEA;
  }

  return control;
}

static void cadence_i2c_setup_read_transfer(
  cadence_i2c_bus *bus,
  volatile cadence_i2c *regs,
  uint32_t control
)
{
  control |= CADENCE_I2C_CONTROL_RW;
  regs->control = control;

  if (bus->current_todo <= CADENCE_I2C_TRANSFER_SIZE_MAX) {
    regs->transfer_size = bus->current_todo;
  } else {
    regs->transfer_size = CADENCE_I2C_TRANSFER_SIZE_ONCE_MAX;
  }
}

static void cadence_i2c_next_byte(cadence_i2c_bus *bus)
{
  --bus->current_msg_todo;
  ++bus->current_msg_byte;

  if (bus->current_msg_todo == 0) {
    i2c_msg *msg;

    ++bus->msgs;
    --bus->msg_todo;

    msg = &bus->msgs[0];

    bus->current_msg_todo = msg->len;
    bus->current_msg_byte = msg->buf;
  }
}

static void cadence_i2c_write_to_fifo(
  cadence_i2c_bus *bus,
  volatile cadence_i2c *regs
)
{
  uint32_t space_available;
  uint32_t todo_now;
  uint32_t i;

  space_available = CADENCE_I2C_FIFO_DEPTH - regs->transfer_size;

  if (bus->current_todo > space_available) {
    todo_now = space_available;
  } else {
    todo_now = bus->current_todo;
  }

  bus->current_todo -= todo_now;

  for (i = 0; i < todo_now; ++i) {
    regs->data = *bus->current_msg_byte;

    cadence_i2c_next_byte(bus);
  }
}

static void cadence_i2c_setup_write_transfer(
  cadence_i2c_bus *bus,
  volatile cadence_i2c *regs,
  uint32_t control
)
{
  control &= ~CADENCE_I2C_CONTROL_RW;
  regs->control = control;

  cadence_i2c_write_to_fifo(bus, regs);
}

static void cadence_i2c_setup_transfer(
  cadence_i2c_bus *bus,
  volatile cadence_i2c *regs
)
{
  const i2c_msg *msgs = bus->msgs;
  uint32_t msg_todo = bus->msg_todo;
  uint32_t i;
  uint32_t control;

  bus->current_todo = msgs[0].len;
  for (i = 1; i < msg_todo && (msgs[i].flags & I2C_M_NOSTART) != 0; ++i) {
    bus->current_todo += msgs[i].len;
  }

  regs = bus->regs;

  control = regs->control;
  control |= CADENCE_I2C_CONTROL_CLR_FIFO;

  bus->hold = i < msg_todo;

  if (bus->hold || bus->current_todo > CADENCE_I2C_FIFO_DEPTH) {
    control |= CADENCE_I2C_CONTROL_HOLD;
  } else {
    control &= ~CADENCE_I2C_CONTROL_HOLD;
  }

  control = cadence_i2c_set_address_size(msgs, control);

  bus->read = (msgs->flags & I2C_M_RD) != 0;
  if (bus->read) {
    cadence_i2c_setup_read_transfer(bus, regs, control);
  } else {
    cadence_i2c_setup_write_transfer(bus, regs, control);
  }

  cadence_i2c_clear_irq_status(regs);

  regs->address = CADENCE_I2C_ADDRESS(msgs->addr);
}

static void cadence_i2c_continue_read_transfer(
  cadence_i2c_bus *bus,
  volatile cadence_i2c *regs
)
{
  uint32_t i;

  bus->current_todo -= CADENCE_I2C_DATA_IRQ_DEPTH;

  /*
   * This works since CADENCE_I2C_TRANSFER_SIZE_ONCE_MAX is an integral
   * multiple of CADENCE_I2C_DATA_IRQ_DEPTH.
   *
   * FIXME: Tests with a 1024 byte EEPROM show that this doesn't work.  Needs
   * further investigations with an I2C analyser or an oscilloscope.
   */
  if (regs->transfer_size == 0) {
    if (bus->current_todo <= CADENCE_I2C_TRANSFER_SIZE_MAX) {
      regs->transfer_size = bus->current_todo;
    } else {
      regs->transfer_size = CADENCE_I2C_TRANSFER_SIZE_ONCE_MAX;
    }
  }

  for (i = 0; i < CADENCE_I2C_DATA_IRQ_DEPTH; ++i) {
    *bus->current_msg_byte = (uint8_t) regs->data;

    cadence_i2c_next_byte(bus);
  }

  if (!bus->hold && bus->current_todo <= CADENCE_I2C_FIFO_DEPTH) {
    regs->control &= ~CADENCE_I2C_CONTROL_HOLD;
  }
}

static void cadence_i2c_interrupt(void *arg)
{
  cadence_i2c_bus *bus = arg;
  volatile cadence_i2c *regs = bus->regs;
  uint32_t irqstatus = regs->irqstatus;
  bool done = false;

  /* Clear interrupts */
  regs->irqstatus = irqstatus;

  if ((irqstatus & (CADENCE_I2C_IXR_ARB_LOST | CADENCE_I2C_IXR_NACK)) != 0) {
    done = true;
  }

  if (
    (irqstatus & CADENCE_I2C_IXR_DATA) != 0
      && bus->read
      && bus->current_todo >= CADENCE_I2C_DATA_IRQ_DEPTH
  ) {
    cadence_i2c_continue_read_transfer(bus, regs);
  }

  if ((irqstatus & CADENCE_I2C_IXR_COMP) != 0) {
    if (bus->read) {
      uint32_t todo_now = bus->current_todo;
      uint32_t i;

      for (i = 0; i < todo_now; ++i) {
        *bus->current_msg_byte = (uint8_t) regs->data;

        cadence_i2c_next_byte(bus);
      }

      bus->current_todo = 0;

      done = true;
    } else {
      if (bus->current_todo > 0) {
        cadence_i2c_write_to_fifo(bus, regs);
      } else {
        done = true;
      }

      if (!bus->hold && bus->current_todo == 0) {
        regs->control &= ~CADENCE_I2C_CONTROL_HOLD;
      }
    }
  }

  if (done) {
    uint32_t err = irqstatus & CADENCE_I2C_IRQ_ERROR;

    if (bus->msg_todo == 0 || err != 0) {
      rtems_status_code sc;

      cadence_i2c_disable_interrupts(regs);

      bus->irqstatus = err;

      sc = rtems_event_transient_send(bus->task_id);
      _Assert(sc == RTEMS_SUCCESSFUL);
      (void) sc;
    } else {
      cadence_i2c_setup_transfer(bus, regs);
    }
  }
}

static int cadence_i2c_transfer(
  i2c_bus *base,
  i2c_msg *msgs,
  uint32_t msg_count
)
{
  cadence_i2c_bus *bus = (cadence_i2c_bus *) base;
  volatile cadence_i2c *regs;
  rtems_status_code sc;
  uint32_t i;

  _Assert(msg_count > 0);

  for (i = 0; i < msg_count; ++i) {
    /* FIXME: Not sure if we can support this. */
    if ((msgs[i].flags & I2C_M_RECV_LEN) != 0) {
      return -EINVAL;
    }
  }

  bus->msgs = &msgs[0];
  bus->msg_todo = msg_count;
  bus->current_msg_todo = msgs[0].len;
  bus->current_msg_byte = msgs[0].buf;
  bus->task_id = rtems_task_self();

  regs = bus->regs;
  cadence_i2c_setup_transfer(bus, regs);
  regs->irqenable = CADENCE_I2C_IRQ_USED;

  sc = rtems_event_transient_receive(RTEMS_WAIT, bus->base.timeout);
  if (sc != RTEMS_SUCCESSFUL) {
    cadence_i2c_reset(bus);
    rtems_event_transient_clear();

    return -ETIMEDOUT;
  }

  return bus->irqstatus == 0 ? 0 : -EIO;
}

static int cadence_i2c_set_clock(i2c_bus *base, unsigned long clock)
{
  cadence_i2c_bus *bus = (cadence_i2c_bus *) base;
  volatile cadence_i2c *regs = bus->regs;
  uint32_t error = 0xffffffff;
  uint32_t best_div_a = CADENCE_I2C_DIV_A_MAX - 1;
  uint32_t best_div_b = CADENCE_I2C_DIV_B_MAX - 1;
  uint32_t div = bus->input_clock / (22 * clock);
  uint32_t div_a;
  uint32_t control;

  if (div <= 0 || div > (CADENCE_I2C_DIV_A_MAX * CADENCE_I2C_DIV_B_MAX)) {
    return -EIO;
  }

  for (div_a = 0; div_a < CADENCE_I2C_DIV_A_MAX; ++div_a) {
    uint32_t a = 22 * clock * (div_a + 1);
    uint32_t b = (bus->input_clock + a - 1) / a;

    if (b > 0 && b <= CADENCE_I2C_DIV_B_MAX) {
      uint32_t actual_clock = bus->input_clock / (22 * (div_a + 1) * b);
      uint32_t e = clock < actual_clock ?
        actual_clock - clock : clock - actual_clock;

      /*
       * Favour greater div_a values according to UG585, Zynq-7000 AP SoC
       * Technical Reference Manual, Table 20-1: Calculated Values for Standard
       * and High Speed SCL Clock Values".
       */
      if (e <= error && actual_clock <= clock) {
        error = e;
        best_div_a = div_a;
        best_div_b = b - 1;
      }
    }
  }

  control = regs->control;
  control = CADENCE_I2C_CONTROL_DIV_A_SET(control, best_div_a);
  control = CADENCE_I2C_CONTROL_DIV_B_SET(control, best_div_b);
  regs->control = control;

  return 0;
}

static void cadence_i2c_destroy(i2c_bus *base)
{
  cadence_i2c_bus *bus = (cadence_i2c_bus *) base;
  rtems_status_code sc;

  sc = rtems_interrupt_handler_remove(bus->irq, cadence_i2c_interrupt, bus);
  _Assert(sc == RTEMS_SUCCESSFUL);
  (void) sc;

  i2c_bus_destroy_and_free(&bus->base);
}

int i2c_bus_register_cadence(
  const char *bus_path,
  uintptr_t register_base,
  uint32_t input_clock,
  rtems_vector_number irq
)
{
  cadence_i2c_bus *bus;
  rtems_status_code sc;
  int err;

  bus = (cadence_i2c_bus *) i2c_bus_alloc_and_init(sizeof(*bus));
  if (bus == NULL) {
    return -1;
  }

  bus->regs = (volatile cadence_i2c *) register_base;
  bus->input_clock = input_clock;
  bus->irq = irq;

  cadence_i2c_reset(bus);

  err = cadence_i2c_set_clock(&bus->base, I2C_BUS_CLOCK_DEFAULT);
  if (err != 0) {
    (*bus->base.destroy)(&bus->base);

    rtems_set_errno_and_return_minus_one(-err);
  }

  sc = rtems_interrupt_handler_install(
    irq,
    "Cadence I2C",
    RTEMS_INTERRUPT_UNIQUE,
    cadence_i2c_interrupt,
    bus
  );
  if (sc != RTEMS_SUCCESSFUL) {
    (*bus->base.destroy)(&bus->base);

    rtems_set_errno_and_return_minus_one(EIO);
  }

  bus->base.transfer = cadence_i2c_transfer;
  bus->base.set_clock = cadence_i2c_set_clock;
  bus->base.destroy = cadence_i2c_destroy;

  return i2c_bus_register(&bus->base, bus_path);
}
