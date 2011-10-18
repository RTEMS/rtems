/**
 * @file
 *
 * @ingroup lpc32xx
 *
 * @brief High speed UART driver (14-clock).
 */

/*
 * Copyright (c) 2010
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
#include <rtems/libio.h>
#include <rtems/termiostypes.h>

#include <libchip/serial.h>
#include <libchip/sersupp.h>

#include <bsp.h>
#include <bsp/lpc32xx.h>
#include <bsp/irq.h>

typedef struct {
  uint32_t fifo;
  uint32_t level;
  uint32_t iir;
  uint32_t ctrl;
  uint32_t rate;
} lpc32xx_hsu;

#define HSU_FIFO_SIZE 64

#define HSU_LEVEL_RX_MASK 0xffU
#define HSU_LEVEL_TX_MASK 0xff00U
#define HSU_LEVEL_TX_SHIFT 8

#define HSU_RX_DATA_MASK 0xffU
#define HSU_RX_EMPTY (1U << 8)
#define HSU_RX_ERROR (1U << 9)
#define HSU_RX_BREAK (1U << 10)

#define HSU_IIR_TX (1U << 0)
#define HSU_IIR_RX_TRIG (1U << 1)
#define HSU_IIR_RX_TIMEOUT (1U << 2)

#define HSU_CTRL_INTR_DISABLED 0x1280fU
#define HSU_CTRL_RX_INTR_ENABLED 0x1284fU
#define HSU_CTRL_RX_AND_TX_INTR_ENABLED 0x1286fU

/* We are interested in RX timeout, RX trigger and TX trigger interrupts */
#define HSU_IIR_MASK 0x7U

static int lpc32xx_hsu_first_open(int major, int minor, void *arg)
{
  rtems_libio_open_close_args_t *oca = arg;
  struct rtems_termios_tty *tty = oca->iop->data1;
  console_tbl *ct = Console_Port_Tbl [minor];
  console_data *cd = &Console_Port_Data [minor];
  volatile lpc32xx_hsu *hsu = (volatile lpc32xx_hsu *) ct->ulCtrlPort1;

  cd->termios_data = tty;
  rtems_termios_set_initial_baud(tty, (int32_t) ct->pDeviceParams);
  hsu->ctrl = HSU_CTRL_RX_INTR_ENABLED;

  return 0;
}

static ssize_t lpc32xx_hsu_write(int minor, const char *buf, size_t len)
{
  console_tbl *ct = Console_Port_Tbl [minor];
  console_data *cd = &Console_Port_Data [minor];
  volatile lpc32xx_hsu *hsu = (volatile lpc32xx_hsu *) ct->ulCtrlPort1;
  size_t tx_level = (hsu->level & HSU_LEVEL_TX_MASK) >> HSU_LEVEL_TX_SHIFT;
  size_t tx_free = HSU_FIFO_SIZE - tx_level;
  size_t i = 0;
  size_t out = len > tx_free ? tx_free : len;

  for (i = 0; i < out; ++i) {
    hsu->fifo = buf [i];
  }

  if (len > 0) {
    cd->pDeviceContext = (void *) out;
    cd->bActive = true;
    hsu->ctrl = HSU_CTRL_RX_AND_TX_INTR_ENABLED;
  }

  return 0;
}

static void lpc32xx_hsu_interrupt_handler(void *arg)
{
  int minor = (int) arg;
  console_tbl *ct = Console_Port_Tbl [minor];
  console_data *cd = &Console_Port_Data [minor];
  volatile lpc32xx_hsu *hsu = (volatile lpc32xx_hsu *) ct->ulCtrlPort1;

  /* Iterate until no more interrupts are pending */
  do {
    int chars_to_dequeue = (int) cd->pDeviceContext;
    int rv = 0;
    int i = 0;
    char buf [HSU_FIFO_SIZE];

    /* Enqueue received characters */
    while (i < HSU_FIFO_SIZE) {
      uint32_t in = hsu->fifo;

      if ((in & HSU_RX_EMPTY) == 0) {
        if ((in & HSU_RX_BREAK) == 0) {
          buf [i] = in & HSU_RX_DATA_MASK;
          ++i;
        }
      } else {
        break;
      }
    }
    rtems_termios_enqueue_raw_characters(cd->termios_data, buf, i);

    /* Dequeue transmitted characters */
    cd->pDeviceContext = 0;
    rv = rtems_termios_dequeue_characters(cd->termios_data, chars_to_dequeue);
    if (rv == 0) {
      /* Nothing to transmit */
      cd->bActive = false;
      hsu->ctrl = HSU_CTRL_RX_INTR_ENABLED;
      hsu->iir = HSU_IIR_TX;
    }
  } while ((hsu->iir & HSU_IIR_MASK) != 0);
}

static void lpc32xx_hsu_initialize(int minor)
{
  console_tbl *ct = Console_Port_Tbl [minor];
  console_data *cd = &Console_Port_Data [minor];
  volatile lpc32xx_hsu *hsu = (volatile lpc32xx_hsu *) ct->ulCtrlPort1;

  hsu->ctrl = HSU_CTRL_INTR_DISABLED;

  cd->bActive = false;
  cd->pDeviceContext = 0;

  /* Drain FIFOs */
  while (hsu->level != 0) {
    hsu->fifo;
  }

  rtems_interrupt_handler_install(
    ct->ulIntVector,
    "HSU",
    RTEMS_INTERRUPT_UNIQUE,
    lpc32xx_hsu_interrupt_handler,
    (void *) minor
  );
}

static int lpc32xx_hsu_set_attributes(int minor, const struct termios *term)
{
  console_tbl *ct = Console_Port_Tbl [minor];
  volatile lpc32xx_hsu *hsu = (volatile lpc32xx_hsu *) ct->ulCtrlPort1;
  int baud_flags = term->c_cflag & CBAUD;

  if (baud_flags != 0) {
    int32_t baud = rtems_termios_baud_to_number(baud_flags);

    if (baud > 0) {
      uint32_t baud_divisor = 14 * (uint32_t) baud;
      uint32_t rate = LPC32XX_PERIPH_CLK / baud_divisor;
      uint32_t remainder = LPC32XX_PERIPH_CLK - rate * baud_divisor;

      if (2 * remainder >= baud_divisor) {
        ++rate;
      }

      hsu->rate = rate - 1;
    }
  }

  return 0;
}

console_fns lpc32xx_hsu_fns = {
  .deviceProbe = libchip_serial_default_probe,
  .deviceFirstOpen = lpc32xx_hsu_first_open,
  .deviceLastClose = NULL,
  .deviceRead = NULL,
  .deviceWrite = lpc32xx_hsu_write,
  .deviceInitialize = lpc32xx_hsu_initialize,
  .deviceWritePolled = NULL,
  .deviceSetAttributes = lpc32xx_hsu_set_attributes,
  .deviceOutputUsesInterrupts = true
};
