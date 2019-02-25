/**
 * @file
 *
 * @ingroup arm_beagle
 *
 * @brief BeagleBoard SPI bus initialization and API Support.
 *
 * Based on bsps/m68k/gen68360/spi/m360_spi.c
 */

/*
 * Copyright (c) 2018 Pierre-Louis Garnier <garnie_a@epita.fr>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */
#include <bsp.h>
#include <bsp/bbb-gpio.h>
#include <bsp/spi.h>
#include <errno.h>
#include <rtems/bspIo.h>
#include <rtems/error.h>
#include <rtems/libi2c.h>
#include <stdio.h>
#include <stdlib.h>

// #define DEBUG
// #define TRACE

#define EVENT_TXEMPTY     RTEMS_EVENT_0
#define EVENT_RXFULL      RTEMS_EVENT_1

static void SPI0ModuleClkConfig(void)
{
  /* Writing to MODULEMODE field of AM335X_CM_PER_SPI0_CLKCTRL register. */
  REG( AM335X_CM_PER_ADDR + AM335X_CM_PER_SPI0_CLKCTRL ) |=
    AM335X_CM_PER_SPI0_CLKCTRL_MODULEMODE_ENABLE;

  /* Waiting for MODULEMODE field to reflect the written value. */
  while ( AM335X_CM_PER_SPI0_CLKCTRL_MODULEMODE_ENABLE !=
          ( REG( AM335X_CM_PER_ADDR + AM335X_CM_PER_SPI0_CLKCTRL ) &
            AM335X_CM_PER_SPI0_CLKCTRL_MODULEMODE ) )
    continue;

  /*
   * Waiting for IDLEST field in AM335X_CM_PER_SPI0_CLKCTRL
   * register to attain desired value.
   */
  while ( ( AM335X_CM_PER_CONTROL_CLKCTRL_IDLEST_FUNC <<
            AM335X_CM_PER_CONTROL_CLKCTRL_IDLEST_SHIFT ) !=
          ( REG( AM335X_CM_PER_ADDR + AM335X_CM_PER_SPI0_CLKCTRL ) &
            AM335X_CM_PER_CONTROL_CLKCTRL_IDLEST ) )
    continue;
}

static inline void am335x_spi_clear_irqstatus(uint32_t reg_base, uint32_t irqs)
{
  REG(reg_base + AM335X_SPI_SYST) &= ~AM335X_SPI_SYST_SSB;
  REG(reg_base + AM335X_SPI_IRQSTATUS) = irqs;
}

static void am335x_spi0_pinmux(void)
{
  REG(AM335X_PADCONF_BASE + AM335X_CONF_SPI0_SCLK) =
    (BBB_RXACTIVE | BBB_SLEWCTRL | BBB_PU_EN);

  REG(AM335X_PADCONF_BASE + AM335X_CONF_SPI0_D0) =
    (BBB_RXACTIVE | BBB_SLEWCTRL | BBB_PU_EN);

  REG(AM335X_PADCONF_BASE + AM335X_CONF_SPI0_D1) =
    (BBB_RXACTIVE | BBB_SLEWCTRL | BBB_PU_EN);

  REG(AM335X_PADCONF_BASE + AM335X_CONF_SPI0_CS0) =
    (BBB_RXACTIVE | BBB_PU_EN);
}

static void am335x_spi_reset(uint32_t reg_base)
{
  int timeout = BBB_SPI_TIMEOUT;

  REG(reg_base + AM335X_SPI_SYSCONFIG) |= AM335X_SPI_SYSCONFIG_SOFTRESET;

  while ((REG(reg_base + AM335X_SPI_SYSSTATUS) & AM335X_SPI_SYSSTATUS_RESETDONE) == 0 && timeout--) {
    if (timeout <= 0) {
      puts("ERROR: Timeout in soft-reset\n");

      return;
    }

    udelay(1000);
  }
}

static void beagle_spi_irq_handler(void *arg)
{
  const uint32_t handled_irqs = AM335X_SPI_IRQSTATUS_TX0_EMPTY | AM335X_SPI_IRQSTATUS_RX0_FULL;

  beagle_spi_softc_t *softc_ptr = arg;

  rtems_status_code sc = -1;

  uint32_t irq = 0;
  uint32_t events = 0;

  uint32_t tmp;
  while ((tmp = (REG(softc_ptr->regs_base + AM335X_SPI_IRQSTATUS)) & handled_irqs) != 0) {
    irq |= tmp;
    am335x_spi_clear_irqstatus(softc_ptr->regs_base, tmp);
  }

#if defined(TRACE)
    printk("beagle_spi_irq_handler: AM335X_SPI_IRQSTATUS = 0x%04lx\r\n", irq);
#endif

  if (irq & AM335X_SPI_IRQSTATUS_TX0_EMPTY) {
#if defined(TRACE)
    printk("beagle_spi_irq_handler: sending event TXEMPTY to task_id = %ld\r\n", softc_ptr->task_id);
#endif

    events |= EVENT_TXEMPTY;
  }

  if (irq & AM335X_SPI_IRQSTATUS_RX0_FULL) {
#if defined(TRACE)
    printk("beagle_spi_irq_handler: sending event RXFULL to task_id = %ld\r\n", softc_ptr->task_id);
#endif

    events |= EVENT_RXFULL;
  }

  sc = rtems_event_send(softc_ptr->task_id, events);
  _Assert(sc == RTEMS_SUCCESSFUL);
  (void)sc;
}

/* Initialize the driver
 *
 * Returns: o = ok or error code
 */
rtems_status_code beagle_spi_init
(
 rtems_libi2c_bus_t *bh                  /* bus specifier structure        */
)
{
  beagle_spi_softc_t *softc_ptr = &(((beagle_spi_desc_t *)(bh))->softc);

  rtems_status_code rc = RTEMS_SUCCESSFUL;

#if defined(DEBUG)
  printk("beagle_spi_init called...\r\n");
#endif

  SPI0ModuleClkConfig();
  am335x_spi0_pinmux();
  am335x_spi_reset(softc_ptr->regs_base);

  REG(softc_ptr->regs_base + AM335X_SPI_MODULCTRL) &= ~AM335X_SPI_MODULCTRL_PIN34;
  REG(softc_ptr->regs_base + AM335X_SPI_MODULCTRL) &= ~AM335X_SPI_MODULCTRL_MS; // Master mode
  REG(softc_ptr->regs_base + AM335X_SPI_MODULCTRL) |= AM335X_SPI_MODULCTRL_SINGLE; // Single channel
  REG(softc_ptr->regs_base + AM335X_SPI_MODULCTRL) &= ~AM335X_SPI_MODULCTRL_PIN34; // SPIEN is usedas a chip select
  // REG(softc_ptr->regs_base + AM335X_SPI_MODULCTRL) |= (1 << 3); // Test mode

  // REG(softc_ptr->regs_base + AM335X_SPI_SYST) |= AM335X_SPI_SYST_SPIEN_0; // Not sure about this
  // REG(softc_ptr->regs_base + AM335X_SPI_SYST) |= AM335X_SPI_SYST_SPIDATDIR0; // Input
  // REG(softc_ptr->regs_base + AM335X_SPI_SYST) &= ~AM335X_SPI_SYST_SPIDATDIR1; // Output

  REG(softc_ptr->regs_base + AM335X_SPI_CH0CONF) &= ~AM335X_SPI_CH0CONF_TRM_MASK;
  REG(softc_ptr->regs_base + AM335X_SPI_CH0CONF) |= AM335X_SPI_CH0CONF_DPE0;
  REG(softc_ptr->regs_base + AM335X_SPI_CH0CONF) &= ~AM335X_SPI_CH0CONF_DPE1;
  REG(softc_ptr->regs_base + AM335X_SPI_CH0CONF) &= ~AM335X_SPI_CH0CONF_IS;

  // REG(softc_ptr->regs_base + AM335X_SPI_CH0CONF) |= AM335X_SPI_CH0CONF_FFEW;
  // REG(softc_ptr->regs_base + AM335X_SPI_CH0CONF) |= AM335X_SPI_CH0CONF_FFER;
  REG(softc_ptr->regs_base + AM335X_SPI_CH0CONF) |= AM335X_SPI_CH0CONF_WL(8 - 1);

  REG(softc_ptr->regs_base + AM335X_SPI_CH0CONF) &= ~AM335X_SPI_CH0CONF_PHA;
  REG(softc_ptr->regs_base + AM335X_SPI_CH0CONF) &= ~AM335X_SPI_CH0CONF_POL;

  REG(softc_ptr->regs_base + AM335X_SPI_CH0CONF) |= AM335X_SPI_CH0CONF_EPOL;
  REG(softc_ptr->regs_base + AM335X_SPI_CH0CONF) |= AM335X_SPI_CH0CONF_CLKD(0x1);


  // Setup interrupt
  rc = rtems_interrupt_handler_install(
    softc_ptr->irq,
    NULL,
    RTEMS_INTERRUPT_UNIQUE,
    (rtems_interrupt_handler)beagle_spi_irq_handler,
    softc_ptr
  );

#if defined(DEBUG)
  printk("beagle_spi_init done\r\n");
#endif

  if (rc == RTEMS_SUCCESSFUL) {
    softc_ptr->initialized = TRUE;
  }

  return rc;
}

static int beagle_spi_read_write_bytes(
 rtems_libi2c_bus_t *bh,                 /* bus specifier structure        */
 unsigned char *rx_buf,                  /* buffer to store bytes          */
 unsigned char *tx_buf,                  /* buffer to send                 */
 int len                                 /* number of bytes to send        */
)
{
  beagle_spi_softc_t *softc_ptr = &(((beagle_spi_desc_t *)(bh))->softc);

  rtems_status_code sc;
  rtems_event_set received_events;

#if defined(TRACE)
  printk("beagle_spi_read_write_bytes called...\r\n");
#endif

  softc_ptr->task_id = rtems_task_self();

  // Enable IRQs

  if (rx_buf) {
    am335x_spi_clear_irqstatus(softc_ptr->regs_base, AM335X_SPI_IRQSTATUS_RX0_FULL);
    REG(softc_ptr->regs_base + AM335X_SPI_IRQENABLE) |= AM335X_SPI_IRQENABLE_RX0_FULL;
  }

  if (tx_buf) {
    am335x_spi_clear_irqstatus(softc_ptr->regs_base, AM335X_SPI_IRQSTATUS_TX0_EMPTY);
    REG(softc_ptr->regs_base + AM335X_SPI_IRQENABLE) |= AM335X_SPI_IRQENABLE_TX0_EMPTY;
  }

  // Enable Channel
  REG(softc_ptr->regs_base + AM335X_SPI_CH0CONF) |= AM335X_SPI_CH0CONF_FORCE;
  REG(softc_ptr->regs_base + AM335X_SPI_CH0CTRL) |= AM335X_SPI_CH0CTRL_EN;

  // Main loop
  for (int i = 0; i < len; i++) {
    received_events = 0;

    if (tx_buf) {
      // Wait for IRQ to wake us up (room in TX FIFO)
#if defined(TRACE)
      printk("beagle_spi_read_write_bytes: waiting (task_id = %ld)\r\n", softc_ptr->task_id);
#endif

      sc = rtems_event_receive(EVENT_TXEMPTY, RTEMS_WAIT, BBB_SPI_TIMEOUT, &received_events);
      if (sc != RTEMS_SUCCESSFUL) {
        printk("ERROR: beagle_spi_read_write_bytes timed out on tx byte number %d\n", i);
        return i > 0 ? i : -RTEMS_TIMEOUT;
      }

      _Assert(received_events == EVENT_TXEMPTY);

#if defined(TRACE)
      printk("beagle_spi_read_write_bytes: sending byte: i = %d, tx_buf[i] = 0x%x\r\n", i, tx_buf[i]);
#endif

      REG(softc_ptr->regs_base + AM335X_SPI_TX0) = tx_buf[i];
    }

    if (rx_buf) {
      // Wait for IRQ to wake us up (data in RX FIFO)
      if ((received_events & EVENT_RXFULL) == 0) {
        sc = rtems_event_receive(EVENT_RXFULL, RTEMS_WAIT, BBB_SPI_TIMEOUT, &received_events);
        if (sc != RTEMS_SUCCESSFUL) {
          printk("ERROR: beagle_spi_read_write_bytes timed out on rx byte number %d\n", i);
          return i > 0 ? i : -RTEMS_TIMEOUT;
        }

        _Assert(received_events == EVENT_RXFULL);
      }

      rx_buf[i] = REG(softc_ptr->regs_base + AM335X_SPI_RX0);

#if defined(TRACE)
      printk("beagle_spi_read_write_bytes: received byte: i = %d, rx_buf[i] = 0x%x\r\n", i, rx_buf[i]);
#endif
    }
  }

  // REG(softc_ptr->regs_base + AM335X_SPI_IRQENABLE) &= ~AM335X_SPI_IRQENABLE_RX0_FULL;
  // REG(softc_ptr->regs_base + AM335X_SPI_IRQENABLE) &= ~AM335X_SPI_IRQENABLE_TX0_EMPTY;

  REG(softc_ptr->regs_base + AM335X_SPI_CH0CTRL) &= ~AM335X_SPI_CH0CTRL_EN;
  REG(softc_ptr->regs_base + AM335X_SPI_CH0CONF) &= ~AM335X_SPI_CH0CONF_FORCE;

#if defined(TRACE)
  printk("beagle_spi_read_write_bytes done\r\n");
#endif

  return len;
}

/*
 * Receive some bytes from SPI device
 *
 * Returns: number of bytes received or (negative) error code
 */
int beagle_spi_read_bytes
(
 rtems_libi2c_bus_t *bh,                 /* bus specifier structure        */
 unsigned char *buf,                     /* buffer to store bytes          */
 int len                                 /* number of bytes to receive     */
)
{
  // FIXME
#if defined(DEBUG)
  printk("beagle_spi_read_bytes called...\r\n");
#endif

  int n = beagle_spi_read_write_bytes(bh, buf, NULL, len);

#if defined(DEBUG)
  printk("beagle_spi_read_bytes done\r\n");
#endif

  return n;
}

/*
 * Send some bytes to SPI device
 *
 * Returns: number of bytes sent or (negative) error code
 */
int beagle_spi_write_bytes
(
 rtems_libi2c_bus_t *bh,                 /* bus specifier structure        */
 unsigned char *buf,                     /* buffer to send                 */
 int len                                 /* number of bytes to send        */
)
{
#if defined(DEBUG)
  printk("beagle_spi_write_bytes called...\r\n");
#endif

  int n = beagle_spi_read_write_bytes(bh, NULL, buf, len);

#if defined(DEBUG)
  printk("beagle_spi_write_bytes done\r\n");
#endif

  return n;
}

/*
 * Perform selected ioctl function for SPI
 *
 * Returns: rtems_status_code
 */
int beagle_spi_ioctl
(
 rtems_libi2c_bus_t *bh,                 /* bus specifier structure        */
 int                 cmd,                /* ioctl command code             */
 void               *arg                 /* additional argument array      */
)
{
  int ret_val = -1;

#if defined(DEBUG)
  printk("beagle_spi_ioctl called...\r\n");
#endif

  switch(cmd) { // FIXME: other ioctls
  case RTEMS_LIBI2C_IOCTL_SET_TFRMODE:
#if defined(TRACE)
    printk("cmd == RTEMS_LIBI2C_IOCTL_SET_TFRMODE\r\n");
#endif
    // FIXME
    // ret_val =
    //   -m360_spi_set_tfr_mode(bh,
    //     (const rtems_libi2c_tfr_mode_t *)arg);
    break;
  case RTEMS_LIBI2C_IOCTL_READ_WRITE: {
#if defined(TRACE)
    printk("cmd == RTEMS_LIBI2C_IOCTL_READ_WRITE\r\n");
#endif
    const rtems_libi2c_read_write_t *cmd = (const rtems_libi2c_read_write_t *)arg;

    ret_val = beagle_spi_read_write_bytes(
      bh,
      (unsigned char *)cmd->rd_buf,
      (unsigned char *)cmd->wr_buf,
      cmd->byte_cnt
    );
    break;
  }
  default:
    ret_val = -RTEMS_NOT_DEFINED;
    break;
  }

#if defined(DEBUG)
  printk("beagle_spi_ioctl done\r\n");
#endif

  return ret_val;
}

/*=========================================================================*\
| Board-specific adaptation functions                                       |
\*=========================================================================*/

/*
 * Address a slave device on the bus
 *
 * Returns: o = ok or error code
 */
static rtems_status_code bsp_spi_sel_addr
(
 rtems_libi2c_bus_t *bh,                 /* bus specifier structure        */
 uint32_t addr,                          /* address to send on bus         */
 int rw                                  /* 0=write,1=read                 */
)
{
  if (addr != 0)
    return RTEMS_NOT_IMPLEMENTED;

  return  RTEMS_SUCCESSFUL;
}

/*
 * Dummy function, SPI has no start condition
 *
 * Returns: o = ok or error code
 */
static rtems_status_code bsp_spi_send_start_dummy
(
 rtems_libi2c_bus_t *bh                  /* bus specifier structure        */
)
{
#if defined(DEBUG)
  printk("bsp_spi_send_start_dummy OK\r\n");
#endif

  return 0;
}

/*
 * Deselect SPI
 *
 * Returns: o = ok or error code
 */
static rtems_status_code bsp_spi_send_stop
(
 rtems_libi2c_bus_t *bh                  /* bus specifier structure        */
)
{
#if defined(DEBUG)
  printk("bsp_spi_send_stop called... ");
#endif

  // FIXME

#if defined(DEBUG)
  printk("... exit OK\r\n");
#endif
  return 0;
}

/*=========================================================================*\
| list of handlers                                                          |
\*=========================================================================*/

rtems_libi2c_bus_ops_t bsp_spi_ops = {
  init:             beagle_spi_init,
  send_start:       bsp_spi_send_start_dummy,
  send_stop:        bsp_spi_send_stop,
  send_addr:        bsp_spi_sel_addr,
  read_bytes:       beagle_spi_read_bytes,
  write_bytes:      beagle_spi_write_bytes,
  ioctl:            beagle_spi_ioctl
};

static beagle_spi_desc_t bsp_spi_bus_desc = {
  {/* public fields */
    ops:    &bsp_spi_ops,
    size:   sizeof(bsp_spi_bus_desc)
  },
  { /* our private fields */
    initialized: FALSE,
  }
};

/*=========================================================================*\
| initialization                                                            |
\*=========================================================================*/

/*
 * Register SPI bus and devices
 *
 * Returns: Bus number or error code
 */
rtems_status_code bsp_register_spi
(
  const char         *bus_path,
  uintptr_t           register_base,
  rtems_vector_number irq
)
{
  int ret_code;
  int spi_busno;

  beagle_spi_softc_t *softc_ptr = &bsp_spi_bus_desc.softc;

  if (softc_ptr->initialized) {
    printk("ERROR: Only one SPI bus at a time is supported\n");
    return -RTEMS_RESOURCE_IN_USE;
  }

  softc_ptr->regs_base = register_base;
  softc_ptr->irq = irq;

  /*
   * init I2C library (if not already done)
   */
  rtems_libi2c_initialize();

  /*
   * register SPI bus
   */
  ret_code = rtems_libi2c_register_bus(bus_path,
                       &(bsp_spi_bus_desc.bus_desc));
  if (ret_code < 0) {
    return -ret_code;
  }
  spi_busno = ret_code;

#if IS_AM335X
  // TODO: register board devices
#endif

  return spi_busno;
}
