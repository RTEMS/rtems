/**
 * @file
 *
 * @ingroup lpc24xx_libi2c
 *
 * @brief LibI2C bus driver for the I2C modules.
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>

#include <bsp.h>
#include <bsp/i2c.h>
#include <bsp/io.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/lpc24xx.h>
#include <bsp/system-clocks.h>

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <rtems/status-checks.h>

typedef struct {
  rtems_libi2c_bus_t bus;
  volatile lpc24xx_i2c *regs;
  unsigned index;
  unsigned config;
  rtems_vector_number vector;
  rtems_id state_update;
  uint8_t * volatile data;
  uint8_t * volatile end;
} lpc24xx_i2c_bus_entry;

static void lpc24xx_i2c_handler(void *arg)
{
  lpc24xx_i2c_bus_entry *e = arg;
  volatile lpc24xx_i2c *regs = e->regs;
  unsigned state = regs->stat;
  uint8_t *data = e->data;
  uint8_t *end = e->end;
  bool notify = true;

  switch (state) {
    case 0x28U:
      /* Data has been transmitted successfully */
      if (data != end) {
        regs->dat = *data;
        ++data;
        regs->conset = LPC24XX_I2C_AA;
        regs->conclr = LPC24XX_I2C_SI;
        notify = false;
        e->data = data;
      }
      break;
    case 0x50U:
      /* Data has been received */
      if (data != end) {
        *data = (uint8_t) regs->dat;
        ++data;
        if (data != end) {
          if (data + 1 != end) {
            regs->conset = LPC24XX_I2C_AA;
          } else {
            regs->conclr = LPC24XX_I2C_AA;
          }
          regs->conclr = LPC24XX_I2C_SI;
          notify = false;
          e->data = data;
        } else {
          /* This is an error and should never happen */
        }
      }
      break;
    case 0x58U:
      /* Last data has been received */
      if (data != end) {
        *data = (uint8_t) regs->dat;
      }
      break;
    default:
      /* Do nothing */
      break;
  }

  /* Notify task if necessary */
  if (notify) {
    bsp_interrupt_vector_disable(e->vector);

    rtems_semaphore_release(e->state_update);
  }
}

static rtems_status_code lpc24xx_i2c_wait(lpc24xx_i2c_bus_entry *e)
{
  bsp_interrupt_vector_enable(e->vector);

  return rtems_semaphore_obtain(e->state_update, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
}

static rtems_status_code lpc24xx_i2c_init(rtems_libi2c_bus_t *bus)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  lpc24xx_i2c_bus_entry *e = (lpc24xx_i2c_bus_entry *) bus;
  volatile lpc24xx_i2c *regs = e->regs;
  unsigned cycles = LPC24XX_CCLK / (8U * 100000U * 2U);

  /* Create semaphore */
  sc = rtems_semaphore_create (
    rtems_build_name ('I', '2', 'C', '0' + e->index),
    0,
    RTEMS_SIMPLE_BINARY_SEMAPHORE,
    0,
    &e->state_update
  );
  RTEMS_CHECK_SC(sc, "create status update semaphore");

  /* Enable module power */
  sc = lpc24xx_module_enable(LPC24XX_MODULE_I2C_0 + e->index, LPC24XX_MODULE_CCLK_8);
  RTEMS_CHECK_SC(sc, "enable module");

  /* IO configuration */
  sc = lpc24xx_io_config(LPC24XX_MODULE_I2C_0 + e->index, e->config);
  RTEMS_CHECK_SC(sc, "IO configuration");

  /* Clock high and low duty cycles */
  regs->sclh = cycles;
  regs->scll = cycles;

  /* Disable module */
  regs->conclr = LPC24XX_I2C_EN;

  /* Install interrupt handler and disable this vector */
  sc = rtems_interrupt_handler_install(
    e->vector,
    "I2C",
    RTEMS_INTERRUPT_UNIQUE,
    lpc24xx_i2c_handler,
    e
  );
  RTEMS_CHECK_SC(sc, "install interrupt handler");
  bsp_interrupt_vector_disable(e->vector);

  /* Enable module in master mode */
  regs->conset = LPC24XX_I2C_EN;

  /* Set self address */
  regs->adr = 0;

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code lpc24xx_i2c_send_start(rtems_libi2c_bus_t *bus)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  lpc24xx_i2c_bus_entry *e = (lpc24xx_i2c_bus_entry *) bus;
  volatile lpc24xx_i2c *regs = e->regs;

  /* Start */
  regs->conclr = LPC24XX_I2C_STA | LPC24XX_I2C_AA | LPC24XX_I2C_SI;
  regs->conset = LPC24XX_I2C_STA;

  /* Wait */
  sc = lpc24xx_i2c_wait(e);
  RTEMS_CHECK_SC(sc, "wait for state update");

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code lpc24xx_i2c_send_stop(rtems_libi2c_bus_t *bus)
{
  lpc24xx_i2c_bus_entry *e = (lpc24xx_i2c_bus_entry *) bus;
  volatile lpc24xx_i2c *regs = e->regs;

  /* Stop */
  regs->conset = LPC24XX_I2C_STO | LPC24XX_I2C_AA;
  regs->conclr = LPC24XX_I2C_STA | LPC24XX_I2C_SI;

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code lpc24xx_i2c_send_addr(
  rtems_libi2c_bus_t *bus,
  uint32_t addr,
  int rw
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  lpc24xx_i2c_bus_entry *e = (lpc24xx_i2c_bus_entry *) bus;
  volatile lpc24xx_i2c *regs = e->regs;
  unsigned state = regs->stat;

  /* Check state */
  if (state != 0x8U && state != 0x10U) {
    return -RTEMS_IO_ERROR;
  }

  /* Send address */
  regs->dat = (uint8_t) ((addr << 1U) | ((rw != 0) ? 1U : 0U));
  regs->conset = LPC24XX_I2C_AA;
  regs->conclr = LPC24XX_I2C_STA | LPC24XX_I2C_SI;

  /* Wait */
  sc = lpc24xx_i2c_wait(e);
  RTEMS_CHECK_SC_RV(sc, "wait for state update");

  /* Check state */
  state = regs->stat;
  if (state != 0x18U && state != 0x40U) {
    return -RTEMS_IO_ERROR;
  }

  return RTEMS_SUCCESSFUL;
}

static int lpc24xx_i2c_read(rtems_libi2c_bus_t *bus, unsigned char *in, int n)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  lpc24xx_i2c_bus_entry *e = (lpc24xx_i2c_bus_entry *) bus;
  volatile lpc24xx_i2c *regs = e->regs;
  unsigned state = regs->stat;
  uint8_t *data = in;
  uint8_t *end = in + n;

  if (n <= 0) {
    return n;
  } else if (state != 0x40U) {
    return -RTEMS_IO_ERROR;
  }

  /* Setup receive buffer */
  e->data = data;
  e->end = end;

  /* Ready to receive data */
  if (data + 1 != end) {
    regs->conset = LPC24XX_I2C_AA;
  } else {
    regs->conclr = LPC24XX_I2C_AA;
  }
  regs->conclr = LPC24XX_I2C_SI;

  /* Wait */
  sc = lpc24xx_i2c_wait(e);
  RTEMS_CHECK_SC_RV(sc, "wait for state update");

  /* Check state */
  state = regs->stat;
  if (state != 0x58U) {
    return -RTEMS_IO_ERROR;
  }

  return n;
}

static int lpc24xx_i2c_write(
  rtems_libi2c_bus_t *bus,
  unsigned char *out,
  int n
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  lpc24xx_i2c_bus_entry *e = (lpc24xx_i2c_bus_entry *) bus;
  volatile lpc24xx_i2c *regs = e->regs;
  unsigned state = 0;

  if (n <= 0) {
    return n;
  }

  /* Setup transmit buffer */
  e->data = out + 1;
  e->end = out + n;

  /* Transmit first byte */
  regs->dat = *out;
  regs->conset = LPC24XX_I2C_AA;
  regs->conclr = LPC24XX_I2C_SI;

  /* Wait */
  sc = lpc24xx_i2c_wait(e);
  RTEMS_CHECK_SC_RV(sc, "wait for state update");

  /* Check state */
  state = regs->stat;
  if (state != 0x28U) {
    return -RTEMS_IO_ERROR;
  }

  return n;
}

static int lpc24xx_i2c_set_transfer_mode(
  rtems_libi2c_bus_t *bus,
  const rtems_libi2c_tfr_mode_t *mode
)
{
  return -RTEMS_NOT_IMPLEMENTED;
}

static int lpc24xx_i2c_ioctl(rtems_libi2c_bus_t *bus, int cmd, void *arg)
{
  int rv = -1;
  const rtems_libi2c_tfr_mode_t *tm = (const rtems_libi2c_tfr_mode_t *) arg;

  switch (cmd) {
    case RTEMS_LIBI2C_IOCTL_SET_TFRMODE:
      rv = lpc24xx_i2c_set_transfer_mode(bus, tm);
      break;
    default:
      rv = -RTEMS_NOT_DEFINED;
      break;
  }

  return rv;
}

static const rtems_libi2c_bus_ops_t lpc24xx_i2c_ops = {
  .init = lpc24xx_i2c_init,
  .send_start = lpc24xx_i2c_send_start,
  .send_stop = lpc24xx_i2c_send_stop,
  .send_addr = lpc24xx_i2c_send_addr,
  .read_bytes = lpc24xx_i2c_read,
  .write_bytes = lpc24xx_i2c_write,
  .ioctl = lpc24xx_i2c_ioctl
};

#ifdef LPC24XX_CONFIG_I2C_0
  static lpc24xx_i2c_bus_entry lpc24xx_i2c_entry_0 = {
    .bus = {
      .ops = &lpc24xx_i2c_ops,
      .size = sizeof(lpc24xx_i2c_bus_entry)
    },
    .regs = (volatile lpc24xx_i2c *) I2C0_BASE_ADDR,
    .index = 0,
    .config = LPC24XX_CONFIG_I2C_0,
    .vector = LPC24XX_IRQ_I2C_0
  };

  rtems_libi2c_bus_t * const lpc24xx_i2c_0 =
    (rtems_libi2c_bus_t *) &lpc24xx_i2c_entry_0;
#endif

#ifdef LPC24XX_CONFIG_I2C_1
  static lpc24xx_i2c_bus_entry lpc24xx_i2c_entry_1 = {
    .bus = {
      .ops = &lpc24xx_i2c_ops,
      .size = sizeof(lpc24xx_i2c_bus_entry)
    },
    .regs = (volatile lpc24xx_i2c *) I2C1_BASE_ADDR,
    .index = 1,
    .config = LPC24XX_CONFIG_I2C_1,
    .vector = LPC24XX_IRQ_I2C_1
  };

  rtems_libi2c_bus_t * const lpc24xx_i2c_1 =
    (rtems_libi2c_bus_t *) &lpc24xx_i2c_entry_1;
#endif

#ifdef LPC24XX_CONFIG_I2C_2
  static lpc24xx_i2c_bus_entry lpc24xx_i2c_entry_2 = {
    .bus = {
      .ops = &lpc24xx_i2c_ops,
      .size = sizeof(lpc24xx_i2c_bus_entry)
    },
    .regs = (volatile lpc24xx_i2c *) I2C2_BASE_ADDR,
    .index = 2,
    .config = LPC24XX_CONFIG_I2C_2,
    .vector = LPC24XX_IRQ_I2C_2
  };

  rtems_libi2c_bus_t * const lpc24xx_i2c_2 =
    (rtems_libi2c_bus_t *) &lpc24xx_i2c_entry_2;
#endif
