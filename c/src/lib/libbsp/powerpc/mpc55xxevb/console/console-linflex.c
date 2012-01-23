/**
 * @file
 *
 * @brief Console LINFlexD implementation.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp/console-linflex.h>

#include <bsp.h>
#include <bsp/irq.h>

#ifdef MPC55XX_HAS_LINFLEX

mpc55xx_linflex_context mpc55xx_linflex_devices [] = {
  {
    .regs = &LINFLEX0,
    .irq_rxi = MPC55XX_IRQ_LINFLEX_RXI(0),
    .irq_txi = MPC55XX_IRQ_LINFLEX_TXI(0),
    .irq_err = MPC55XX_IRQ_LINFLEX_ERR(0),
    .tx_pcr_register = &((SIU_tag *) &SIUL)->PCR18,
    .tx_pa_value = 1,
    .rx_pcr_register = &((SIU_tag *) &SIUL)->PCR19,
    .rx_psmi_register = &((SIU_tag *) &SIUL)->PSMI31,
    .rx_padsel_value = 0
  }, {
    .regs = &LINFLEX1,
    .irq_rxi = MPC55XX_IRQ_LINFLEX_RXI(1),
    .irq_txi = MPC55XX_IRQ_LINFLEX_TXI(1),
    .irq_err = MPC55XX_IRQ_LINFLEX_ERR(1),
    .tx_pcr_register = &((SIU_tag *) &SIUL)->PCR94,
    .tx_pa_value = 1,
    .rx_pcr_register = &((SIU_tag *) &SIUL)->PCR95,
    .rx_psmi_register = &((SIU_tag *) &SIUL)->PSMI32,
    .rx_padsel_value = 2
  }
};

void enter_init_mode(volatile LINFLEX_tag *regs)
{
  LINFLEX_LINCR1_32B_tag cr1 = { .R = regs->LINCR1.R };
  cr1.B.SLEEP = 0;
  cr1.B.INIT = 1;
  regs->LINCR1.R = cr1.R;
}

void enter_active_mode(volatile LINFLEX_tag *regs)
{
  LINFLEX_LINCR1_32B_tag cr1 = { .R = regs->LINCR1.R };
  cr1.B.SLEEP = 0;
  cr1.B.INIT = 0;
  regs->LINCR1.R = cr1.R;
}

void enter_sleep_mode(volatile LINFLEX_tag *regs)
{
  LINFLEX_LINCR1_32B_tag cr1 = { .R = regs->LINCR1.R };
  cr1.B.SLEEP = 1;
  cr1.B.INIT = 0;
  regs->LINCR1.R = cr1.R;
}

static void mpc55xx_linflex_poll_write(int minor, char c)
{
  mpc55xx_linflex_context *self = console_generic_get_context(minor);
  volatile LINFLEX_tag *regs = self->regs;
  const LINFLEX_UARTSR_32B_tag clear_dtf = { .B = { .DTF_TFF = 1 } };
  rtems_interrupt_level level;
  bool done = false;
  bool wait_for_transmit_done = false;

  rtems_interrupt_disable(level);
  if (self->transmit_nest_level == 0) {
	LINFLEX_LINIER_32B_tag ier = { .R = regs->LINIER.R };

	if (ier.B.DTIE != 0) {
	  ier.B.DTIE = 0;
	  regs->LINIER.R = ier.R;
	  wait_for_transmit_done = !self->transmit_in_progress;
	  self->transmit_nest_level = 1;
	}
  } else {
	++self->transmit_nest_level;
  }
  rtems_interrupt_enable(level);

  while (!done) {
	rtems_interrupt_disable(level);
	bool tx = self->transmit_in_progress;
	if (!tx || (tx && regs->UARTSR.B.DTF_TFF)) {
	  regs->UARTSR.R = clear_dtf.R;
	  regs->BDRL.B.DATA0 = c;
	  self->transmit_in_progress = true;
	  done = true;
	}
	rtems_interrupt_enable(level);
  }

  done = false;
  while (!done) {
	rtems_interrupt_disable(level);
	if (wait_for_transmit_done) {
	  if (regs->UARTSR.B.DTF_TFF) {
		regs->UARTSR.R = clear_dtf.R;
		self->transmit_in_progress = false;
		done = true;
	  }
	} else {
	  done = true;
	}

	if (done && self->transmit_nest_level > 0) {
	  --self->transmit_nest_level;

	  if (self->transmit_nest_level == 0) {
        LINFLEX_LINIER_32B_tag ier = { .R = regs->LINIER.R };

        ier.B.DTIE = 1;
		regs->LINIER.R = ier.R;
	  }
	}
	rtems_interrupt_enable(level);
  }
}

static void mpc55xx_linflex_rx_interrupt_handler(void *arg)
{
  mpc55xx_linflex_context *self = arg;
  volatile LINFLEX_tag *regs = self->regs;
  char c = regs->BDRM.B.DATA4;
  const LINFLEX_UARTSR_32B_tag clear_flags = { .B = { .RMB = 1, .DRF_RFE = 1 } };

  regs->UARTSR.R = clear_flags.R;

  rtems_termios_enqueue_raw_characters(self->tty, &c, 1);
}

static void mpc55xx_linflex_tx_interrupt_handler(void *arg)
{
  mpc55xx_linflex_context *self = arg;
  volatile LINFLEX_tag *regs = self->regs;

  regs->UARTSR.B.DTF_TFF = 1;	/* clear flag */
  self->transmit_in_progress = false;

  rtems_termios_dequeue_characters(self->tty, 1);
}

/*
static void mpc55xx_linflex_err_interrupt_handler(void *arg)
{
  mpc55xx_linflex_context *self = arg;
}
*/

static int mpc55xx_linflex_set_attributes(int minor, const struct termios *t)
{
  mpc55xx_linflex_context *self = console_generic_get_context(minor);
  volatile LINFLEX_tag *regs = self->regs;
  LINFLEX_UARTCR_32B_tag uartcr = { .R = 0 };
  LINFLEX_GCR_32B_tag gcr = { .R = 0 };
  LINFLEX_LINIER_32B_tag ier = { .R = 0 };
  rtems_termios_baud_t br = rtems_termios_baud_to_number(t->c_cflag);
  LINFLEX_LINFBRR_32B_tag fbrr = { .R = 0 };
  LINFLEX_LINIBRR_32B_tag ibrr = { .R = 0 };

  enter_init_mode(regs);

  /* Set to UART-mode */
  uartcr.B.UART = 1;
  regs->UARTCR.R = uartcr.R;

  /* Set to buffer mode with size 1 */
  uartcr.B.TDFL_TFC = 0;
  uartcr.B.RDFL_RFC0 = 0;
  uartcr.B.RFBM = 0;
  uartcr.B.TFBM = 0;

  /* Enable receiver and transmitter */
  uartcr.B.RXEN = 1;
  uartcr.B.TXEN = 1;

  /* Number of data bits */
  uartcr.B.WL1 = 0;
  if ((t->c_cflag & CSIZE) == CS8) {
	uartcr.B.WL0 = 1;
  } else if ((t->c_cflag & CSIZE) == CS7) {
	uartcr.B.WL0 = 0;
  } else {
    return -1;
  }

  /* Parity */
  uartcr.B.PCE = (t->c_cflag & PARENB) ? 1 : 0;
  uartcr.B.PC1 = 0;
  uartcr.B.PC0 = (t->c_cflag & PARODD) ? 1 : 0;

  /* Stop bits */
  gcr.B.STOP = (t->c_cflag & CSTOPB) ? 1 : 0;

  /* Set control registers */
  regs->UARTCR.R = uartcr.R;
  regs->GCR.R = gcr.R;

  /* Interrupts */
  ier.B.DTIE = 1;
  ier.B.DRIE = 1;
  regs->LINIER.R = ier.R;

  /* Baud rate */
  if (br > 0) {
	uint32_t lfdiv_mult_32 = bsp_clock_speed * 2 / br;
	if((lfdiv_mult_32 & 0x1) != 0) {
		++lfdiv_mult_32;
	}
	fbrr.B.FBR = (lfdiv_mult_32 >> 1) & 0xF;
	ibrr.B.IBR = lfdiv_mult_32 >> 5;
  } else {
    return -1;
  }
  regs->LINFBRR.R = fbrr.R;
  regs->LINIBRR.R = ibrr.R;

  enter_active_mode(regs);

  return 0;
}

static int mpc55xx_linflex_first_open(int major, int minor, void *arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  int rv = 0;
  mpc55xx_linflex_context *self = console_generic_get_context(minor);
  struct rtems_termios_tty *tty = console_generic_get_tty_at_open(arg);
  SIU_PCR_tag pcr = { .R = 0 };
  SIUL_PSMI_8B_tag psmi = { .R = 0 };

  self->tty = tty;

  pcr.B.IBE = 1;
  self->rx_pcr_register->R = pcr.R;
  psmi.B.PADSEL = self->rx_padsel_value;
  self->rx_psmi_register->R = psmi.R;
  pcr.R = 0;
  pcr.B.OBE = 1;
  pcr.B.PA = self->tx_pa_value;
  self->tx_pcr_register->R = pcr.R;

  rv = rtems_termios_set_initial_baud(tty, 115200);
  if (rv != 0) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }

  rv = mpc55xx_linflex_set_attributes(minor, &tty->termios);
  if (rv != 0) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }

  sc = mpc55xx_interrupt_handler_install(
    self->irq_rxi,
    "LINFlexD RXI",
    RTEMS_INTERRUPT_UNIQUE,
    MPC55XX_INTC_DEFAULT_PRIORITY,
    mpc55xx_linflex_rx_interrupt_handler,
    self
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }

  sc = mpc55xx_interrupt_handler_install(
    self->irq_txi,
    "LINFlexD TXI",
    RTEMS_INTERRUPT_UNIQUE,
    MPC55XX_INTC_DEFAULT_PRIORITY,
    mpc55xx_linflex_tx_interrupt_handler,
    self
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }

  /*
  sc = mpc55xx_interrupt_handler_install(
    self->irq_err,
    "LINFlexD ERR",
    RTEMS_INTERRUPT_UNIQUE,
    MPC55XX_INTC_DEFAULT_PRIORITY,
    mpc55xx_linflex_err_interrupt_handler,
    self
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
  */

  return 0;
}

static int mpc55xx_linflex_last_close(int major, int minor, void* arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  mpc55xx_linflex_context *self = console_generic_get_context(minor);
  volatile LINFLEX_tag *regs = self->regs;
  SIU_PCR_tag pcr = { .R = 0 };
  SIUL_PSMI_8B_tag psmi = { .R = 0 };

  /* enter initialization mode */
  enter_init_mode(regs);

  /* disable interrupts */
  regs->LINIER.R = 0;

  /* set module to sleep mode */
  enter_sleep_mode(regs);

  sc = rtems_interrupt_handler_remove(
    self->irq_rxi,
    mpc55xx_linflex_rx_interrupt_handler,
    self
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }

  sc = rtems_interrupt_handler_remove(
    self->irq_txi,
    mpc55xx_linflex_tx_interrupt_handler,
    self
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }

  /*
  sc = rtems_interrupt_handler_remove(
    self->irq_err,
    mpc55xx_linflex_err_interrupt_handler,
    self
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
  */

  pcr.B.IBE = 1;
  self->rx_pcr_register->R = pcr.R;
  self->tx_pcr_register->R = pcr.R;
  psmi.R = 0;
  self->rx_psmi_register->R = psmi.R;

  self->tty = NULL;

  return 0;
}

static int mpc55xx_linflex_poll_read(int minor)
{
  mpc55xx_linflex_context *self = console_generic_get_context(minor);
  volatile LINFLEX_tag *regs = self->regs;
  rtems_interrupt_level level;
  int c = -1;

  rtems_interrupt_disable(level);
  if (regs->UARTSR.B.DRF_RFE != 0) {
    /* Clear flag */
    regs->UARTSR.B.DRF_RFE = 1;

    /* Read */
    c = regs->BDRM.B.DATA4;
  }
  rtems_interrupt_enable(level);

  return c;
}

static int mpc55xx_linflex_write(int minor, const char *out, size_t n)
{
  mpc55xx_linflex_context *self = console_generic_get_context(minor);
  volatile LINFLEX_tag *regs = self->regs;
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);

  regs->BDRL.B.DATA0 = out [0];
  self->transmit_in_progress = true;
  /* TODO: send more then one byte */

  rtems_interrupt_enable(level);

  return 0;
}

const console_generic_callbacks mpc55xx_linflex_callbacks = {
  .termios_callbacks = {
    .firstOpen = mpc55xx_linflex_first_open,
    .lastClose = mpc55xx_linflex_last_close,
    .write = mpc55xx_linflex_write,
    .setAttributes = mpc55xx_linflex_set_attributes,
    .outputUsesInterrupts = TERMIOS_IRQ_DRIVEN
  },
  .poll_read = mpc55xx_linflex_poll_read,
  .poll_write = mpc55xx_linflex_poll_write
};

#endif /* MPC55XX_HAS_LINFLEX */
