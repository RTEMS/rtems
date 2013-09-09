/*
 * Copyright © 2013 Eugeniy Meshcheryakov <eugen@debian.org>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bspopts.h>
#include <bsp/ssi.h>
#include <bsp/syscon.h>
#include <bsp/io.h>
#include <bsp/lm3s69xx.h>

typedef struct {
  rtems_libi2c_bus_t bus;
  volatile lm3s69xx_ssi *regs;
  int bus_number;
  uint16_t idle_char;
  uint8_t cs_pin;
  lm3s69xx_gpio_config io_configs[3];
} lm3s69xx_ssi_bus_entry;

static rtems_status_code lm3s69xx_ssi_init(rtems_libi2c_bus_t *bus)
{
  lm3s69xx_ssi_bus_entry *e = (lm3s69xx_ssi_bus_entry *)bus;
  volatile lm3s69xx_ssi* regs = e->regs;
  uint32_t clock_div = LM3S69XX_SYSTEM_CLOCK / 2 / LM3S69XX_SSI_CLOCK;

  lm3s69xx_gpio_set_config_array(e->io_configs, 3);

  lm3s69xx_syscon_enable_ssi_clock(e->bus_number, true);
  regs->cr1 = 0;
  regs->cpsr = SSI_CPSRDIV(2);
  regs->cr0 = SSICR0_SCR(clock_div - 1) | SSICR0_SPO | SSICR0_SPH | SSICR0_FRF(0) | SSICR0_DSS(7);
  regs->cr1 = SSICR1_SSE;

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code lm3s69xx_ssi_send_start(rtems_libi2c_bus_t *bus)
{
  return RTEMS_SUCCESSFUL;
}

static rtems_status_code lm3s69xx_ssi_send_stop(rtems_libi2c_bus_t *bus)
{
  lm3s69xx_ssi_bus_entry *e = (lm3s69xx_ssi_bus_entry *)bus;
  volatile lm3s69xx_ssi* regs = e->regs;

  while ((regs->sr & SSISR_BSY) != 0)
    /* wait */;

  lm3s69xx_gpio_set_pin(e->cs_pin, true);

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code lm3s69xx_ssi_send_addr(rtems_libi2c_bus_t *bus,
        uint32_t addr, int rw)
{
  lm3s69xx_ssi_bus_entry *e = (lm3s69xx_ssi_bus_entry *)bus;
  e->cs_pin = addr;
  lm3s69xx_gpio_set_pin(e->cs_pin, false);

  return RTEMS_SUCCESSFUL;
}

static int lm3s69xx_ssi_read(rtems_libi2c_bus_t *bus, unsigned char *in, int n)
{
  lm3s69xx_ssi_bus_entry *e = (lm3s69xx_ssi_bus_entry *)bus;
  volatile lm3s69xx_ssi* regs = e->regs;
  int i;

  for (i = 0; i < n; i++) {
    while ((regs->sr & SSISR_TNF) == 0)
      /* wait */;

    regs->dr = e->idle_char;

    while ((regs->sr & SSISR_RNE) == 0)
      /* wait */;

    in[i] = regs->dr & 0xff;
  }

  return n;
}

static int lm3s69xx_ssi_write(rtems_libi2c_bus_t *bus, unsigned char *out, int n)
{
  lm3s69xx_ssi_bus_entry *e = (lm3s69xx_ssi_bus_entry *)bus;
  volatile lm3s69xx_ssi* regs = e->regs;
  int i;

  for (i = 0; i < n; i++) {
    while ((regs->sr & SSISR_TNF) == 0)
      /* wait */;

    regs->dr = out[i];

    while ((regs->sr & SSISR_RNE) == 0)
      /* wait */;

    uint32_t dummy = regs->dr;
    (void)dummy;
  }

  return n;
}

static int lm3s69xx_ssi_ioctl(rtems_libi2c_bus_t *bus, int cmd, void *arg)
{
  return -RTEMS_NOT_DEFINED;
}

static const rtems_libi2c_bus_ops_t lm3s69xx_ssi_ops = {
  .init = lm3s69xx_ssi_init,
  .send_start = lm3s69xx_ssi_send_start,
  .send_stop = lm3s69xx_ssi_send_stop,
  .send_addr = lm3s69xx_ssi_send_addr,
  .read_bytes = lm3s69xx_ssi_read,
  .write_bytes = lm3s69xx_ssi_write,
  .ioctl = lm3s69xx_ssi_ioctl
};

static lm3s69xx_ssi_bus_entry ssi_0_bus = {
  .bus = {
    .ops = &lm3s69xx_ssi_ops,
    .size = sizeof(lm3s69xx_ssi_bus_entry)
  },
  .regs = (volatile lm3s69xx_ssi *)LM3S69XX_SSI_0_BASE,
  .bus_number = 0,
  .idle_char = 0xffff,
  .io_configs = {
#if defined(LM3S69XX_MCU_LM3S3749) || defined(LM3S69XX_MCU_LM3S6965) || defined(LM3S69XX_MCU_LM4F120)
    LM3S69XX_PIN_SSI_TX(LM3S69XX_PORT_A, 2), /* CLK */
    LM3S69XX_PIN_SSI_TX(LM3S69XX_PORT_A, 5), /* TX */
    LM3S69XX_PIN_SSI_RX(LM3S69XX_PORT_A, 4)  /* RX */
#else
#error No GPIO pin definitions for SSI 0
#endif
  }
};

rtems_libi2c_bus_t * const lm3s69xx_ssi_0 = &ssi_0_bus.bus;

#if LM3S69XX_NUM_SSI_BLOCKS > 1
static lm3s69xx_ssi_bus_entry ssi_1_bus = {
  .bus = {
    .ops = &lm3s69xx_ssi_ops,
    .size = sizeof(lm3s69xx_ssi_bus_entry)
  },
  .regs = (volatile lm3s69xx_ssi *)LM3S69XX_SSI_1_BASE,
  .bus_number = 1,
  .idle_char = 0xffff,
  .io_configs = {
#if defined(LM3S69XX_MCU_LM3S3749) || defined(LM3S69XX_MCU_LM4F120)
    LM3S69XX_PIN_SSI_TX(LM3S69XX_PORT_E, 0), /* CLK */
    LM3S69XX_PIN_SSI_TX(LM3S69XX_PORT_E, 3), /* TX */
    LM3S69XX_PIN_SSI_RX(LM3S69XX_PORT_E, 2)  /* RX */
#else
#error No GPIO pin definitions for SSI 1
#endif
  }
};

rtems_libi2c_bus_t * const lm3s69xx_ssi_1 = &ssi_1_bus.bus;
#endif /* LM3S69XX_NUM_SSI_BLOCKS > 1 */
