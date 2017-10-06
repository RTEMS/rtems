/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/fdt.h>
#include <libfdt.h>
#include <arm/freescale/imx/imx_ccmvar.h>
#include <arm/freescale/imx/imx_i2creg.h>
#include <dev/i2c/i2c.h>
#include <rtems/irq-extension.h>

#define IMX_I2C_TRANSMIT (IMX_I2C_I2CR_IEN | IMX_I2C_I2CR_IIEN \
  | IMX_I2C_I2CR_MSTA | IMX_I2C_I2CR_MTX)

#define IMX_I2C_RECEIVE (IMX_I2C_I2CR_IEN | IMX_I2C_I2CR_IIEN \
  | IMX_I2C_I2CR_MSTA)

typedef struct {
  i2c_bus base;
  volatile imx_i2c *regs;
  uint32_t msg_todo;
  const i2c_msg *msg;
  bool read;
  bool start;
  uint16_t restart;
  uint32_t chunk_total;
  uint32_t chunk_done;
  uint16_t buf_todo;
  uint8_t *buf;
  rtems_id task_id;
  int eno;
  rtems_vector_number irq;
} imx_i2c_bus;

typedef struct {
  uint16_t divisor;
  uint8_t ifdr;
} imx_i2c_clock_divisor;

static const imx_i2c_clock_divisor imx_i2c_clock_divisor_table[] = {
  {    0, 0x20 }, {   22, 0x20 }, {   24, 0x21 }, {     26, 0x22 },
  {   28, 0x23 }, {   30, 0x00 }, {   32, 0x24 }, {     36, 0x25 },
  {   40, 0x26 }, {   42, 0x03 }, {   44, 0x27 }, {     48, 0x28 },
  {   52, 0x05 }, {   56, 0x29 }, {   60, 0x06 }, {     64, 0x2a },
  {   72, 0x2b }, {   80, 0x2c }, {   88, 0x09 }, {     96, 0x2d },
  {  104, 0x0a }, {  112, 0x2e }, {  128, 0x2f }, {    144, 0x0c },
  {  160, 0x30 }, {  192, 0x31 }, {  224, 0x32 }, {    240, 0x0f },
  {  256, 0x33 }, {  288, 0x10 }, {  320, 0x34 }, {    384, 0x35 },
  {  448, 0x36 }, {  480, 0x13 }, {  512, 0x37 }, {    576, 0x14 },
  {  640, 0x38 }, {  768, 0x39 }, {  896, 0x3a }, {    960, 0x17 },
  { 1024, 0x3b }, { 1152, 0x18 }, { 1280, 0x3c }, {   1536, 0x3d },
  { 1792, 0x3e }, { 1920, 0x1b }, { 2048, 0x3f }, {   2304, 0x1c },
  { 2560, 0x1d }, { 3072, 0x1e }, { 3840, 0x1f }, { 0xffff, 0x1f }
};

static void imx_i2c_stop(volatile imx_i2c *regs)
{
  regs->i2cr = IMX_I2C_I2CR_IEN;
  regs->i2sr = 0;
  regs->i2sr;
}

static void imx_i2c_trigger_receive(imx_i2c_bus *bus, volatile imx_i2c *regs)
{
  uint16_t i2cr;

  i2cr = IMX_I2C_RECEIVE;

  if (bus->chunk_total == 1) {
    i2cr |= IMX_I2C_I2CR_TXAK;
  }

  regs->i2cr = i2cr;
  regs->i2dr;
}

static void imx_i2c_done(imx_i2c_bus *bus, int eno)
{
  /*
   * Generates a stop in case of transmit, otherwise, only disables interrupts
   * (IMX_I2C_I2CR_MSTA is already cleared).
   */
  imx_i2c_stop(bus->regs);

  bus->eno = eno;
  rtems_event_transient_send(bus->task_id);
}

static const i2c_msg *imx_i2c_msg_inc(imx_i2c_bus *bus)
{
  const i2c_msg *next;

  next = bus->msg + 1;
  bus->msg = next;
  --bus->msg_todo;
  return next;
}

static void imx_i2c_msg_inc_and_set_buf(imx_i2c_bus *bus)
{
  const i2c_msg *next;

  next = imx_i2c_msg_inc(bus);
  bus->buf_todo = next->len;
  bus->buf = next->buf;
}

static void imx_i2c_buf_inc(imx_i2c_bus *bus)
{
  ++bus->buf;
  --bus->buf_todo;
  ++bus->chunk_done;
}

static void imx_i2c_buf_push(imx_i2c_bus *bus, uint8_t c)
{
  while (true) {
    if (bus->buf_todo > 0) {
      bus->buf[0] = c;
      imx_i2c_buf_inc(bus);
      break;
    }

    imx_i2c_msg_inc_and_set_buf(bus);
  }
}

static uint8_t imx_i2c_buf_pop(imx_i2c_bus *bus)
{
  while (true) {
    if (bus->buf_todo > 0) {
      uint8_t c;

      c = bus->buf[0];
      imx_i2c_buf_inc(bus);
      return c;
    }

    imx_i2c_msg_inc_and_set_buf(bus);
  }
}

RTEMS_STATIC_ASSERT(I2C_M_RD == 1, imx_i2c_read_flag);

static void imx_i2c_setup_chunk(imx_i2c_bus *bus, volatile imx_i2c *regs)
{
  while (true) {
    const i2c_msg *msg;
    int flags;
    int can_continue;
    uint32_t i;

    if (bus->msg_todo == 0) {
      imx_i2c_done(bus, 0);
      break;
    }

    msg = bus->msg;
    flags = msg->flags;

    bus->read = (flags & I2C_M_RD) != 0;
    bus->start = (flags & I2C_M_NOSTART) == 0;
    bus->chunk_total = msg->len;
    bus->chunk_done = 0;
    bus->buf_todo = msg->len;
    bus->buf = msg->buf;

    can_continue = (flags & I2C_M_RD) | I2C_M_NOSTART;

    for (i = 1; i < bus->msg_todo; ++i) {
      if ((msg[i].flags & (I2C_M_RD | I2C_M_NOSTART)) != can_continue) {
        break;
      }

      bus->chunk_total += msg[i].len;
    }

    if (bus->start) {
      regs->i2cr = IMX_I2C_TRANSMIT | bus->restart;
      regs->i2dr = (uint8_t) ((msg->addr << 1) | (flags & I2C_M_RD));
      bus->restart = IMX_I2C_I2CR_RSTA;
      break;
    } else if (bus->chunk_total > 0) {
      if (bus->read) {
        imx_i2c_trigger_receive(bus, regs);
      } else {
        regs->i2cr = IMX_I2C_TRANSMIT;
        regs->i2dr = imx_i2c_buf_pop(bus);
      }

      break;
    } else {
      ++bus->msg;
      --bus->msg_todo;
    }
  }
}

static void imx_i2c_transfer_complete(
  imx_i2c_bus *bus,
  volatile imx_i2c *regs,
  uint16_t i2sr
)
{
  if (bus->start) {
    bus->start = false;

    if ((i2sr & IMX_I2C_I2SR_RXAK) != 0) {
      imx_i2c_done(bus, EIO);
      return;
    }

    if (bus->read) {
      imx_i2c_trigger_receive(bus, regs);
      return;
    }
  }

  if (bus->chunk_done < bus->chunk_total) {
    if (bus->read) {
      if (bus->chunk_done + 2 == bus->chunk_total) {
        /* Receive second last byte with NACK */
        regs->i2cr = IMX_I2C_RECEIVE | IMX_I2C_I2CR_TXAK;
      } else if (bus->chunk_done + 1 == bus->chunk_total) {
        /* Receive last byte with STOP */
        bus->restart = 0;
        regs->i2cr = (IMX_I2C_RECEIVE | IMX_I2C_I2CR_TXAK)
          & ~IMX_I2C_I2CR_MSTA;
      }

      imx_i2c_buf_push(bus, (uint8_t) regs->i2dr);

      if (bus->chunk_done == bus->chunk_total) {
        imx_i2c_msg_inc(bus);
        imx_i2c_setup_chunk(bus, regs);
      }
    } else {
      if (bus->chunk_done > 0 && (i2sr & IMX_I2C_I2SR_RXAK) != 0) {
        imx_i2c_done(bus, EIO);
        return;
      }

      regs->i2dr = imx_i2c_buf_pop(bus);
    }
  } else {
    imx_i2c_msg_inc(bus);
    imx_i2c_setup_chunk(bus, regs);
  }
}

static void imx_i2c_interrupt(void *arg)
{
  imx_i2c_bus *bus;
  volatile imx_i2c *regs;
  uint16_t i2sr;

  bus = arg;
  regs = bus->regs;

  i2sr = regs->i2sr;
  regs->i2sr = 0;

  if ((i2sr & (IMX_I2C_I2SR_IAL | IMX_I2C_I2SR_ICF)) == IMX_I2C_I2SR_ICF) {
    imx_i2c_transfer_complete(bus, regs, i2sr);
  } else {
    imx_i2c_done(bus, EIO);
  }
}

static int imx_i2c_wait_for_not_busy(volatile imx_i2c *regs)
{
  rtems_interval timeout;
  bool before;

  if ((regs->i2sr & IMX_I2C_I2SR_IBB) == 0) {
    return 0;
  }

  timeout = rtems_clock_tick_later(10);

  do {
    before = rtems_clock_tick_before(timeout);

    if ((regs->i2sr & IMX_I2C_I2SR_IBB) == 0) {
      return 0;
    }
  } while (before);

  return ETIMEDOUT;
}

static int imx_i2c_transfer(i2c_bus *base, i2c_msg *msgs, uint32_t n)
{
  imx_i2c_bus *bus;
  int supported_flags;
  uint32_t i;
  volatile imx_i2c *regs;
  int eno;
  rtems_status_code sc;

  supported_flags = I2C_M_RD;

  for (i = 0; i < n; ++i) {
    if ((msgs[i].flags & ~supported_flags) != 0) {
      return -EINVAL;
    }

    supported_flags |= I2C_M_NOSTART;
  }

  bus = (imx_i2c_bus *) base;
  regs = bus->regs;

  eno = imx_i2c_wait_for_not_busy(regs);
  if (eno != 0) {
    return -eno;
  }

  bus->msg_todo = n;
  bus->msg = &msgs[0];
  bus->restart = 0;
  bus->task_id = rtems_task_self();
  bus->eno = 0;

  regs->i2sr = 0;
  imx_i2c_setup_chunk(bus, regs);

  sc = rtems_event_transient_receive(RTEMS_WAIT, bus->base.timeout);
  if (sc != RTEMS_SUCCESSFUL) {
    imx_i2c_stop(bus->regs);
    rtems_event_transient_clear();
    return -ETIMEDOUT;
  }

  return -bus->eno;
}

static int imx_i2c_set_clock(i2c_bus *base, unsigned long clock)
{
  imx_i2c_bus *bus;
  uint32_t ipg_clock;
  uint16_t div;
  size_t i;
  const imx_i2c_clock_divisor *clock_divisor;

  bus = (imx_i2c_bus *) base;
  ipg_clock = imx_ccm_ipg_hz();
  div = (uint16_t) ((ipg_clock + clock - 1) / clock);

  for (i = 0; i < RTEMS_ARRAY_SIZE(imx_i2c_clock_divisor_table); ++i) {
    clock_divisor = &imx_i2c_clock_divisor_table[i];

    if (clock_divisor->divisor >= div) {
      break;
    }
  }

  bus->regs->ifdr = clock_divisor->ifdr;
  return 0;
}

static void imx_i2c_destroy(i2c_bus *base)
{
  imx_i2c_bus *bus;

  bus = (imx_i2c_bus *) base;
  rtems_interrupt_handler_remove(bus->irq, imx_i2c_interrupt, bus);
  i2c_bus_destroy_and_free(&bus->base);
}

static int imx_i2c_init(imx_i2c_bus *bus)
{
  rtems_status_code sc;

  imx_i2c_set_clock(&bus->base, I2C_BUS_CLOCK_DEFAULT);
  bus->regs->i2cr = IMX_I2C_I2CR_IEN;

  sc = rtems_interrupt_handler_install(
    bus->irq,
    "I2C",
    RTEMS_INTERRUPT_UNIQUE,
    imx_i2c_interrupt,
    bus
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return EAGAIN;
  }

  return 0;
}

int i2c_bus_register_imx(const char *bus_path, const char *alias_or_path)
{
  const void *fdt;
  const char *path;
  int node;
  imx_i2c_bus *bus;
  int eno;

  fdt = bsp_fdt_get();
  path = fdt_get_alias(fdt, alias_or_path);

  if (path == NULL) {
    path = alias_or_path;
  }

  node = fdt_path_offset(fdt, path);
  if (node < 0) {
    rtems_set_errno_and_return_minus_one(ENXIO);
  }

  bus = (imx_i2c_bus *) i2c_bus_alloc_and_init(sizeof(*bus));
  if (bus == NULL){
    return -1;
  }

  bus->regs = imx_get_reg_of_node(fdt, node);
  bus->irq = imx_get_irq_of_node(fdt, node, 0);

  eno = imx_i2c_init(bus);
  if (eno != 0) {
    (*bus->base.destroy)(&bus->base);
    rtems_set_errno_and_return_minus_one(eno);
  }

  bus->base.transfer = imx_i2c_transfer;
  bus->base.set_clock = imx_i2c_set_clock;
  bus->base.destroy = imx_i2c_destroy;

  return i2c_bus_register(&bus->base, bus_path);
}
