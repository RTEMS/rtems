/**
 * @file
 *
 * @brief Console ESCI implementation.
 */

/*
 * Copyright (c) 2008-2012 embedded brains GmbH.  All rights reserved.
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

#include <bsp/console-esci.h>

#include <bsp.h>
#include <bsp/irq.h>

#ifdef MPC55XX_HAS_ESCI

mpc55xx_esci_context mpc55xx_esci_devices [] = {
  {
    .regs = &ESCI_A,
    .irq = MPC55XX_IRQ_ESCI(0)
  }
  #ifdef ESCI_B
    , {
      .regs = &ESCI_B,
      .irq = MPC55XX_IRQ_ESCI(1)
    }
  #endif
  #ifdef ESCI_C
    , {
      .regs = &ESCI_C,
      .irq = MPC55XX_IRQ_ESCI(2)
    }
  #endif
  #ifdef ESCI_D
    , {
      .regs = &ESCI_D,
      .irq = MPC55XX_IRQ_ESCI(3)
    }
  #endif
};

static void mpc55xx_esci_poll_write(int minor, char c)
{
  mpc55xx_esci_context *self = console_generic_get_context(minor);
  const union ESCI_SR_tag clear_tdre = { .B = { .TDRE = 1 } };
  volatile struct ESCI_tag *regs = self->regs;
  rtems_interrupt_level level;
  bool done = false;
  bool wait_for_transmit_done = false;

  rtems_interrupt_disable(level);
  if (self->transmit_nest_level == 0) {
    union ESCI_CR1_tag cr1 = { .R = regs->CR1.R };

    if (cr1.B.TIE != 0) {
      cr1.B.TIE = 0;
      regs->CR1.R = cr1.R;
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
    if (!tx || (tx && regs->SR.B.TDRE)) {
      regs->SR.R = clear_tdre.R;
      regs->DR.B.D = c;
      self->transmit_in_progress = true;
      done = true;
    }
    rtems_interrupt_enable(level);
  }

  done = false;
  while (!done) {
    rtems_interrupt_disable(level);
    if (wait_for_transmit_done) {
      if (regs->SR.B.TDRE) {
        regs->SR.R = clear_tdre.R;
        self->transmit_in_progress = false;
        done = true;
      }
    } else {
      done = true;
    }

    if (done && self->transmit_nest_level > 0) {
      --self->transmit_nest_level;

      if (self->transmit_nest_level == 0) {
        union ESCI_CR1_tag cr1 = { .R = regs->CR1.R };

        cr1.B.TIE = 1;
        regs->CR1.R = cr1.R;
      }
    }
    rtems_interrupt_enable(level);
  }
}

static inline void mpc55xx_esci_interrupts_clear_and_enable(
  mpc55xx_esci_context *self
)
{
  volatile struct ESCI_tag *regs = self->regs;
  union ESCI_CR1_tag cr1 = MPC55XX_ZERO_FLAGS;
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  cr1.R = regs->CR1.R;
  cr1.B.RIE = 1;
  cr1.B.TIE = 1;
  regs->CR1.R = cr1.R;
  regs->SR.R = regs->SR.R;
  rtems_interrupt_enable(level);
}

static inline void mpc55xx_esci_interrupts_disable(mpc55xx_esci_context *self)
{
  volatile struct ESCI_tag *regs = self->regs;
  union ESCI_CR1_tag cr1 = MPC55XX_ZERO_FLAGS;
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  cr1.R = regs->CR1.R;
  cr1.B.RIE = 0;
  cr1.B.TIE = 0;
  regs->CR1.R = cr1.R;
  rtems_interrupt_enable(level);
}

static void mpc55xx_esci_interrupt_handler(void *arg)
{
  mpc55xx_esci_context *self = arg;
  volatile struct ESCI_tag *regs = self->regs;
  union ESCI_SR_tag sr = MPC55XX_ZERO_FLAGS;
  union ESCI_SR_tag active = MPC55XX_ZERO_FLAGS;
  rtems_interrupt_level level;

  /* Status */
  sr.R = regs->SR.R;

  /* Receive data register full? */
  if (sr.B.RDRF != 0) {
    active.B.RDRF = 1;
  }

  /* Transmit data register empty? */
  if (sr.B.TDRE != 0) {
    active.B.TDRE = 1;
  }

  /* Clear flags */
  rtems_interrupt_disable(level);
  regs->SR.R = active.R;
  self->transmit_in_progress = false;
  rtems_interrupt_enable(level);

  /* Enqueue */
  if (active.B.RDRF != 0) {
    char c = regs->DR.B.D;
    rtems_termios_enqueue_raw_characters(self->tty, &c, 1);
  }

  /* Dequeue */
  if (active.B.TDRE != 0) {
    rtems_termios_dequeue_characters(self->tty, 1);
  }
}

static int mpc55xx_esci_set_attributes(int minor, const struct termios *t)
{
  mpc55xx_esci_context *self = console_generic_get_context(minor);
  volatile struct ESCI_tag *regs = self->regs;
  union ESCI_CR1_tag cr1 = { .R = regs->CR1.R };
  union ESCI_CR2_tag cr2 = MPC55XX_ZERO_FLAGS;
  rtems_termios_baud_t br = rtems_termios_baud_to_number(t->c_cflag);

  /* Enable module */
  cr2.B.MDIS = 0;

  /* Interrupts */
  cr1.B.TCIE = 0;
  cr1.B.ILIE = 0;
  cr2.B.IEBERR = 0;
  cr2.B.ORIE = 0;
  cr2.B.NFIE = 0;
  cr2.B.FEIE = 0;
  cr2.B.PFIE = 0;

  /* Disable receiver wake-up standby */
  cr1.B.RWU = 0;

  /* Disable DMA channels */
  cr2.B.RXDMA = 0;
  cr2.B.TXDMA = 0;

  /* Idle line type */
  cr1.B.ILT = 0;

  /* Disable loops */
  cr1.B.LOOPS = 0;

  /* Enable or disable receiver */
  cr1.B.RE = (t->c_cflag & CREAD) ? 1 : 0;

  /* Enable transmitter */
  cr1.B.TE = 1;

  /* Baud rate */
  if (br > 0) {
    br = bsp_clock_speed / (16 * br);
    br = (br > 8191) ? 8191 : br;
  } else {
    br = 0;
  }
  cr1.B.SBR = br;

  /* Number of data bits */
  if ((t->c_cflag & CSIZE) != CS8) {
    return -1;
  }
  cr1.B.M = 0;

  /* Parity */
  cr1.B.PE = (t->c_cflag & PARENB) ? 1 : 0;
  cr1.B.PT = (t->c_cflag & PARODD) ? 1 : 0;

  /* Stop bits */
  if (t->c_cflag & CSTOPB ) {
    /* Two stop bits */
    return -1;
  }

  /* Disable LIN */
  regs->LCR.R = 0;

  /* Set control registers */
  regs->CR2.R = cr2.R;
  regs->CR1.R = cr1.R;

  return 0;
}

static int mpc55xx_esci_first_open(int major, int minor, void *arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  int rv = 0;
  mpc55xx_esci_context *self = console_generic_get_context(minor);
  struct rtems_termios_tty *tty = console_generic_get_tty_at_open(arg);

  self->tty = tty;

  rv = rtems_termios_set_initial_baud(tty, 115200);
  if (rv != 0) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }

  rv = mpc55xx_esci_set_attributes(minor, &tty->termios);
  if (rv != 0) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }

  sc = mpc55xx_interrupt_handler_install(
    self->irq,
    "eSCI",
    RTEMS_INTERRUPT_UNIQUE,
    MPC55XX_INTC_DEFAULT_PRIORITY,
    mpc55xx_esci_interrupt_handler,
    self
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }

  mpc55xx_esci_interrupts_clear_and_enable(self);
  self->transmit_in_progress = false;

  return 0;
}

static int mpc55xx_esci_last_close(int major, int minor, void* arg)
{
  mpc55xx_esci_context *self = console_generic_get_context(minor);

  mpc55xx_esci_interrupts_disable(self);
  self->tty = NULL;

  return 0;
}

static int mpc55xx_esci_poll_read(int minor)
{
  mpc55xx_esci_context *self = console_generic_get_context(minor);
  volatile struct ESCI_tag *regs = self->regs;
  union ESCI_SR_tag sr = MPC55XX_ZERO_FLAGS;
  rtems_interrupt_level level;
  int c = -1;

  rtems_interrupt_disable(level);
  if (regs->SR.B.RDRF != 0) {
    /* Clear flag */
    sr.B.RDRF = 1;
    regs->SR.R = sr.R;

    /* Read */
    c = regs->DR.B.D;
  }
  rtems_interrupt_enable(level);

  return c;
}

static int mpc55xx_esci_write(int minor, const char *out, size_t n)
{
  mpc55xx_esci_context *self = console_generic_get_context(minor);
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  self->regs->DR.B.D = out [0];
  self->transmit_in_progress = true;
  rtems_interrupt_enable(level);

  return 0;
}

const console_generic_callbacks mpc55xx_esci_callbacks = {
  .termios_callbacks = {
    .firstOpen = mpc55xx_esci_first_open,
    .lastClose = mpc55xx_esci_last_close,
    .write = mpc55xx_esci_write,
    .setAttributes = mpc55xx_esci_set_attributes,
    .outputUsesInterrupts = TERMIOS_IRQ_DRIVEN
  },
  .poll_read = mpc55xx_esci_poll_read,
  .poll_write = mpc55xx_esci_poll_write
};

#endif /* MPC55XX_HAS_ESCI */
