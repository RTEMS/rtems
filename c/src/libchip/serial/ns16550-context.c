/**
 *  @file
 *  
 *  This file contains the TTY driver for the National Semiconductor NS16550.
 *
 *  This part is widely cloned and second sourced.  It is found in a number
 *  of "Super IO" controllers.
 *
 *  This driver uses the termios pseudo driver.
 */

/*
 *  COPYRIGHT (c) 1998 by Radstone Technology
 *
 *  THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 *  AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 *  You are hereby granted permission to use, copy, modify, and distribute
 *  this file, provided that this notice, plus the above copyright notice
 *  and disclaimer, appears in all copies. Radstone Technology will provide
 *  no support for this code.
 *
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <stdlib.h>

#include <rtems/bspIo.h>

#include <bsp.h>

#include "ns16550.h"
#include "ns16550_p.h"

#if defined(BSP_FEATURE_IRQ_EXTENSION)
  #include <bsp/irq.h>
#elif defined(BSP_FEATURE_IRQ_LEGACY)
  #include <bsp/irq.h>
#elif defined(__PPC__) || defined(__i386__)
  #include <bsp/irq.h>
  #define BSP_FEATURE_IRQ_LEGACY
  #ifdef BSP_SHARED_HANDLER_SUPPORT
    #define BSP_FEATURE_IRQ_LEGACY_SHARED_HANDLER_SUPPORT
  #endif
#endif

static uint32_t NS16550_GetBaudDivisor(ns16550_context *ctx, uint32_t baud)
{
  uint32_t clock = ctx->clock;
  uint32_t baudDivisor = (clock != 0 ? clock : 115200) / (baud * 16);

  if (ctx->has_fractional_divider_register) {
    uint32_t fractionalDivider = 0x10;
    uint32_t err = baud;
    uint32_t mulVal;
    uint32_t divAddVal;

    clock /= 16 * baudDivisor;
    for (mulVal = 1; mulVal < 16; ++mulVal) {
      for (divAddVal = 0; divAddVal < mulVal; ++divAddVal) {
        uint32_t actual = (mulVal * clock) / (mulVal + divAddVal);
        uint32_t newErr = actual > baud ? actual - baud : baud - actual;

        if (newErr < err) {
          err = newErr;
          fractionalDivider = (mulVal << 4) | divAddVal;
        }
      }
    }

    (*ctx->set_reg)(
      ctx->port,
      NS16550_FRACTIONAL_DIVIDER,
      fractionalDivider
    );
  }

  return baudDivisor;
}

/*
 *  ns16550_enable_interrupts
 *
 *  This routine initializes the port to have the specified interrupts masked.
 */
static void ns16550_enable_interrupts(
  ns16550_context *ctx,
  int              mask
)
{
  (*ctx->set_reg)(ctx->port, NS16550_INTERRUPT_ENABLE, mask);
}

static void ns16550_clear_and_set_interrupts(
  ns16550_context *ctx,
  uint8_t          clear,
  uint8_t          set
)
{
  rtems_interrupt_lock_context lock_context;
  ns16550_get_reg get_reg = ctx->get_reg;
  ns16550_set_reg set_reg = ctx->set_reg;
  uintptr_t port = ctx->port;
  uint8_t val;

  rtems_termios_device_lock_acquire(&ctx->base, &lock_context);
  val = (*get_reg)(port, NS16550_INTERRUPT_ENABLE);
  val &= ~clear;
  val |= set;
  (*set_reg)(port, NS16550_INTERRUPT_ENABLE, val);
  rtems_termios_device_lock_release(&ctx->base, &lock_context);
}

/*
 *  ns16550_probe
 */

bool ns16550_probe(rtems_termios_device_context *base)
{
  ns16550_context        *ctx = (ns16550_context *) base;
  uintptr_t               pNS16550;
  uint8_t                 ucDataByte;
  uint32_t                ulBaudDivisor;
  ns16550_set_reg         setReg;
  ns16550_get_reg         getReg;

  ctx->modem_control = SP_MODEM_IRQ;

  pNS16550 = ctx->port;    
  setReg   = ctx->set_reg;
  getReg   = ctx->get_reg;

  /* Clear the divisor latch, clear all interrupt enables,
   * and reset and
   * disable the FIFO's.
   */

  (*setReg)(pNS16550, NS16550_LINE_CONTROL, 0x0);
  ns16550_enable_interrupts(ctx, NS16550_DISABLE_ALL_INTR );

  /* Set the divisor latch and set the baud rate. */

  ulBaudDivisor = NS16550_GetBaudDivisor(ctx, ctx->initial_baud);
  ucDataByte = SP_LINE_DLAB;
  (*setReg)(pNS16550, NS16550_LINE_CONTROL, ucDataByte);

  /* XXX */
  (*setReg)(pNS16550,NS16550_TRANSMIT_BUFFER,(uint8_t)(ulBaudDivisor & 0xffU));
  (*setReg)(
    pNS16550,NS16550_INTERRUPT_ENABLE,
    (uint8_t)(( ulBaudDivisor >> 8 ) & 0xffU )
  );

  /* Clear the divisor latch and set the character size to eight bits */
  /* with one stop bit and no parity checking. */
  ucDataByte = EIGHT_BITS;
  (*setReg)(pNS16550, NS16550_LINE_CONTROL, ucDataByte);

  /* Enable and reset transmit and receive FIFOs. TJA     */
  ucDataByte = SP_FIFO_ENABLE;
  (*setReg)(pNS16550, NS16550_FIFO_CONTROL, ucDataByte);

  ucDataByte = SP_FIFO_ENABLE | SP_FIFO_RXRST | SP_FIFO_TXRST;
  (*setReg)(pNS16550, NS16550_FIFO_CONTROL, ucDataByte);

  ns16550_enable_interrupts(ctx, NS16550_DISABLE_ALL_INTR);

  /* Set data terminal ready. */
  /* And open interrupt tristate line */
  (*setReg)(pNS16550, NS16550_MODEM_CONTROL,ctx->modem_control);

  (*getReg)(pNS16550, NS16550_LINE_STATUS );
  (*getReg)(pNS16550, NS16550_RECEIVE_BUFFER );

  return true;
}

static size_t ns16550_write_to_fifo(
  const ns16550_context *ctx,
  const char *buf,
  size_t len
)
{
  uint32_t port = ctx->port;
  ns16550_set_reg set = ctx->set_reg;
  size_t out = len > SP_FIFO_SIZE ? SP_FIFO_SIZE : len;
  size_t i;

  for (i = 0; i < out; ++i) {
    (*set)(port, NS16550_TRANSMIT_BUFFER, buf[i]);
  }

  return out;
}

/**
 * @brief Process interrupt.
 */
static void ns16550_isr(void *arg)
{
  rtems_termios_tty *tty = arg;
  ns16550_context *ctx = rtems_termios_get_device_context(tty);
  uint32_t port = ctx->port;
  ns16550_get_reg get = ctx->get_reg;
  int i = 0;
  char buf [SP_FIFO_SIZE];

  /* Iterate until no more interrupts are pending */
  do {
    /* Fetch received characters */
    for (i = 0; i < SP_FIFO_SIZE; ++i) {
      if ((get( port, NS16550_LINE_STATUS) & SP_LSR_RDY) != 0) {
        buf [i] = (char) get(port, NS16550_RECEIVE_BUFFER);
      } else {
        break;
      }
    }

    /* Enqueue fetched characters */
    rtems_termios_enqueue_raw_characters(tty, buf, i);

    /* Do transmit */
    if (ctx->out_total > 0
        && (get(port, NS16550_LINE_STATUS) & SP_LSR_THOLD) != 0) {
      size_t current = ctx->out_current;

      ctx->out_buf += current;
      ctx->out_remaining -= current;

      if (ctx->out_remaining > 0) {
        ctx->out_current =
          ns16550_write_to_fifo(ctx, ctx->out_buf, ctx->out_remaining);
      } else {
        rtems_termios_dequeue_characters(tty, ctx->out_total);
      }
    }
  } while ((get( port, NS16550_INTERRUPT_ID) & SP_IID_0) == 0);
}

static void ns16550_isr_task(void *arg)
{
  rtems_termios_tty *tty = arg;
  ns16550_context *ctx = rtems_termios_get_device_context(tty);
  uint8_t status = (*ctx->get_reg)(ctx->port, NS16550_LINE_STATUS);

  if ((status & SP_LSR_RDY) != 0) {
    ns16550_clear_and_set_interrupts(ctx, SP_INT_RX_ENABLE, 0);
    rtems_termios_rxirq_occured(tty);
  }

  if (ctx->out_total > 0 && (status & SP_LSR_THOLD) != 0) {
    size_t current = ctx->out_current;

    ctx->out_buf += current;
    ctx->out_remaining -= current;

    if (ctx->out_remaining > 0) {
      ctx->out_current =
        ns16550_write_to_fifo(ctx, ctx->out_buf, ctx->out_remaining);
    } else {
      size_t done = ctx->out_total;

      ctx->out_total = 0;
      ns16550_clear_and_set_interrupts(ctx, SP_INT_TX_ENABLE, 0);
      rtems_termios_dequeue_characters(tty, done);
    }
  }
}

static int ns16550_read_task(rtems_termios_device_context *base)
{
  ns16550_context *ctx = (ns16550_context *) base;
  uint32_t port = ctx->port;
  ns16550_get_reg get = ctx->get_reg;
  char buf[SP_FIFO_SIZE];
  int i;

  for (i = 0; i < SP_FIFO_SIZE; ++i) {
    if ((get(port, NS16550_LINE_STATUS) & SP_LSR_RDY) != 0) {
      buf[i] = (char) get(port, NS16550_RECEIVE_BUFFER);
    } else {
      break;
    }
  }

  rtems_termios_enqueue_raw_characters(ctx->tty, buf, i);
  ns16550_clear_and_set_interrupts(ctx, 0, SP_INT_RX_ENABLE);

  return -1;
}

/*
 *  ns16550_initialize_interrupts
 *
 *  This routine initializes the port to operate in interrupt driver mode.
 */
static void ns16550_initialize_interrupts(
  struct rtems_termios_tty *tty,
  ns16550_context          *ctx,
  void                    (*isr)(void *)
)
{
  #ifdef BSP_FEATURE_IRQ_EXTENSION
    {
      rtems_status_code sc = RTEMS_SUCCESSFUL;
      sc = rtems_interrupt_handler_install(
        ctx->irq,
        "NS16550",
        RTEMS_INTERRUPT_SHARED,
        isr,
        tty
      );
      if (sc != RTEMS_SUCCESSFUL) {
        /* FIXME */
        printk( "%s: Error: Install interrupt handler\n", __func__);
        rtems_fatal_error_occurred( 0xdeadbeef);
      }
    }
  #elif defined(BSP_FEATURE_IRQ_LEGACY)
    {
      int rv = 0;
      #ifdef BSP_FEATURE_IRQ_LEGACY_SHARED_HANDLER_SUPPORT
        rtems_irq_connect_data cd = {
          ctx->irq,
          isr,
          tty,
          NULL,
          NULL,
          NULL,
          NULL
        };
        rv = BSP_install_rtems_shared_irq_handler( &cd);
      #else
        rtems_irq_connect_data cd = {
          ctx->irq,
          isr,
          tty,
          NULL,
          NULL,
          NULL
        };
        rv = BSP_install_rtems_irq_handler( &cd);
      #endif
      if (rv == 0) {
        /* FIXME */
        printk( "%s: Error: Install interrupt handler\n", __func__);
        rtems_fatal_error_occurred( 0xdeadbeef);
      }
    }
  #endif
}

/*
 *  ns16550_open
 */

static bool ns16550_open(
  struct rtems_termios_tty      *tty,
  rtems_termios_device_context  *base,
  struct termios                *term,
  rtems_libio_open_close_args_t *args
)
{
  ns16550_context *ctx = (ns16550_context *) base;

  ctx->tty = tty;

  /* Set initial baud */
  rtems_termios_set_initial_baud(tty, ctx->initial_baud);

  if (tty->handler.mode == TERMIOS_IRQ_DRIVEN) {
    ns16550_initialize_interrupts(tty, ctx, ns16550_isr);
    ns16550_enable_interrupts(ctx, NS16550_ENABLE_ALL_INTR_EXCEPT_TX);
  } else if (tty->handler.mode == TERMIOS_TASK_DRIVEN) {
    ns16550_initialize_interrupts(tty, ctx, ns16550_isr_task);
    ns16550_enable_interrupts(ctx, NS16550_ENABLE_ALL_INTR_EXCEPT_TX);
  }

  return true;
}

static void ns16550_cleanup_interrupts(
  struct rtems_termios_tty *tty,
  ns16550_context          *ctx,
  void                    (*isr)(void *)
)
{
  #if defined(BSP_FEATURE_IRQ_EXTENSION)
    rtems_status_code sc = RTEMS_SUCCESSFUL;
    sc = rtems_interrupt_handler_remove(
      ctx->irq,
      isr,
      tty
    );
    if (sc != RTEMS_SUCCESSFUL) {
      /* FIXME */
      printk("%s: Error: Remove interrupt handler\n", __func__);
      rtems_fatal_error_occurred(0xdeadbeef);
    }
  #elif defined(BSP_FEATURE_IRQ_LEGACY)
    int rv = 0;
    rtems_irq_connect_data cd = {
      .name = ctx->irq,
      .hdl = isr,
      .handle = tty
    };
    rv = BSP_remove_rtems_irq_handler(&cd);
    if (rv == 0) {
      /* FIXME */
      printk("%s: Error: Remove interrupt handler\n", __func__);
      rtems_fatal_error_occurred(0xdeadbeef);
    }
  #endif
}

/*
 *  ns16550_close
 */

static void ns16550_close(
  struct rtems_termios_tty      *tty,
  rtems_termios_device_context  *base,
  rtems_libio_open_close_args_t *args
)
{
  ns16550_context *ctx = (ns16550_context *) base;

  ns16550_enable_interrupts(ctx, NS16550_DISABLE_ALL_INTR);

  if (tty->handler.mode == TERMIOS_IRQ_DRIVEN) {
    ns16550_cleanup_interrupts(tty, ctx, ns16550_isr);
  } else if (tty->handler.mode == TERMIOS_TASK_DRIVEN) {
    ns16550_cleanup_interrupts(tty, ctx, ns16550_isr_task);
  }
}

/**
 * @brief Polled write for NS16550.
 */
void ns16550_polled_putchar(rtems_termios_device_context *base, char out)
{
  ns16550_context *ctx = (ns16550_context *) base;
  uintptr_t port = ctx->port;
  ns16550_get_reg get = ctx->get_reg;
  ns16550_set_reg set = ctx->set_reg;
  uint32_t status = 0;
  rtems_interrupt_lock_context lock_context;

  /* Save port interrupt mask */
  uint32_t interrupt_mask = get( port, NS16550_INTERRUPT_ENABLE);

  /* Disable port interrupts */
  ns16550_enable_interrupts(ctx, NS16550_DISABLE_ALL_INTR);

  while (true) {
    /* Try to transmit the character in a critical section */
    rtems_termios_device_lock_acquire(&ctx->base, &lock_context);

    /* Read the transmitter holding register and check it */
    status = get( port, NS16550_LINE_STATUS);
    if ((status & SP_LSR_THOLD) != 0) {
      /* Transmit character */
      set( port, NS16550_TRANSMIT_BUFFER, out);

      /* Finished */
      rtems_termios_device_lock_release(&ctx->base, &lock_context);
      break;
    } else {
      rtems_termios_device_lock_release(&ctx->base, &lock_context);
    }

    /* Wait for transmitter holding register to be empty */
    do {
      status = get( port, NS16550_LINE_STATUS);
    } while ((status & SP_LSR_THOLD) == 0);
  }

  /* Restore port interrupt mask */
  set( port, NS16550_INTERRUPT_ENABLE, interrupt_mask);
}

/*
 * These routines provide control of the RTS and DTR lines
 */

/*
 *  ns16550_assert_RTS
 */

static void ns16550_assert_RTS(rtems_termios_device_context *base)
{
  ns16550_context             *ctx = (ns16550_context *) base;
  rtems_interrupt_lock_context lock_context;

  /*
   * Assert RTS
   */
  rtems_termios_device_lock_acquire(base, &lock_context);
  ctx->modem_control |= SP_MODEM_RTS;
  (*ctx->set_reg)(ctx->port, NS16550_MODEM_CONTROL, ctx->modem_control);
  rtems_termios_device_lock_release(base, &lock_context);
}

/*
 *  ns16550_negate_RTS
 */

static void ns16550_negate_RTS(rtems_termios_device_context *base)
{
  ns16550_context             *ctx = (ns16550_context *) base;
  rtems_interrupt_lock_context lock_context;

  /*
   * Negate RTS
   */
  rtems_termios_device_lock_acquire(base, &lock_context);
  ctx->modem_control &= ~SP_MODEM_RTS;
  (*ctx->set_reg)(ctx->port, NS16550_MODEM_CONTROL, ctx->modem_control);
  rtems_termios_device_lock_release(base, &lock_context);
}

/*
 * These flow control routines utilise a connection from the local DTR
 * line to the remote CTS line
 */

/*
 *  ns16550_assert_DTR
 */

static void ns16550_assert_DTR(rtems_termios_device_context *base)
{
  ns16550_context             *ctx = (ns16550_context *) base;
  rtems_interrupt_lock_context lock_context;

  /*
   * Assert DTR
   */
  rtems_termios_device_lock_acquire(base, &lock_context);
  ctx->modem_control |= SP_MODEM_DTR;
  (*ctx->set_reg)(ctx->port, NS16550_MODEM_CONTROL, ctx->modem_control);
  rtems_termios_device_lock_release(base, &lock_context);
}

/*
 *  ns16550_negate_DTR
 */

static void ns16550_negate_DTR(rtems_termios_device_context *base)
{
  ns16550_context             *ctx = (ns16550_context *) base;
  rtems_interrupt_lock_context lock_context;

  /*
   * Negate DTR
   */
  rtems_termios_device_lock_acquire(base, &lock_context);
  ctx->modem_control &=~SP_MODEM_DTR;
  (*ctx->set_reg)(ctx->port, NS16550_MODEM_CONTROL,ctx->modem_control);
  rtems_termios_device_lock_release(base, &lock_context);
}

/*
 *  ns16550_set_attributes
 *
 *  This function sets the channel to reflect the requested termios
 *  port settings.
 */

static bool ns16550_set_attributes(
  rtems_termios_device_context *base,
  const struct termios         *t
)
{
  ns16550_context        *ctx = (ns16550_context *) base;
  uint32_t                pNS16550;
  uint32_t                ulBaudDivisor;
  uint8_t                 ucLineControl;
  uint32_t                baud_requested;
  ns16550_set_reg         setReg;
  rtems_interrupt_lock_context lock_context;

  pNS16550 = ctx->port;
  setReg   = ctx->set_reg;

  /*
   *  Calculate the baud rate divisor
   *
   *  Assert ensures there is no division by 0.
   */

  baud_requested = rtems_termios_baud_to_number(t->c_ospeed);
  _Assert( baud_requested != 0 );

  ulBaudDivisor = NS16550_GetBaudDivisor(ctx, baud_requested);

  ucLineControl = 0;

  /*
   *  Parity
   */

  if (t->c_cflag & PARENB) {
    ucLineControl |= SP_LINE_PAR;
    if (!(t->c_cflag & PARODD))
      ucLineControl |= SP_LINE_ODD;
  }

  /*
   *  Character Size
   */

  if (t->c_cflag & CSIZE) {
    switch (t->c_cflag & CSIZE) {
      case CS5:  ucLineControl |= FIVE_BITS;  break;
      case CS6:  ucLineControl |= SIX_BITS;   break;
      case CS7:  ucLineControl |= SEVEN_BITS; break;
      case CS8:  ucLineControl |= EIGHT_BITS; break;
    }
  } else {
    ucLineControl |= EIGHT_BITS;               /* default to 9600,8,N,1 */
  }

  /*
   *  Stop Bits
   */

  if (t->c_cflag & CSTOPB) {
    ucLineControl |= SP_LINE_STOP;              /* 2 stop bits */
  } else {
    ;                                           /* 1 stop bit */
  }

  /*
   *  Now actually set the chip
   */

  rtems_termios_device_lock_acquire(base, &lock_context);

    /*
     *  Set the baud rate
     *
     *  NOTE: When the Divisor Latch Access Bit (DLAB) is set to 1,
     *        the transmit buffer and interrupt enable registers
     *        turn into the LSB and MSB divisor latch registers.
     */

    (*setReg)(pNS16550, NS16550_LINE_CONTROL, SP_LINE_DLAB);
    (*setReg)(pNS16550, NS16550_TRANSMIT_BUFFER, ulBaudDivisor&0xff);
    (*setReg)(pNS16550, NS16550_INTERRUPT_ENABLE, (ulBaudDivisor>>8)&0xff);

    /*
     *  Now write the line control
     */
    (*setReg)(pNS16550, NS16550_LINE_CONTROL, ucLineControl );

  rtems_termios_device_lock_release(base, &lock_context);

  return true;
}

/**
 * @brief Transmits up to @a len characters from @a buf.
 *
 * This routine is invoked either from task context with disabled interrupts to
 * start a new transmission process with exactly one character in case of an
 * idle output state or from the interrupt handler to refill the transmitter.
 *
 * Returns always zero.
 */
static void ns16550_write_support_int(
  rtems_termios_device_context *base,
  const char                   *buf,
  size_t                        len
)
{
  ns16550_context *ctx = (ns16550_context *) base;

  ctx->out_total = len;

  if (len > 0) {
    ctx->out_remaining = len;
    ctx->out_buf = buf;
    ctx->out_current = ns16550_write_to_fifo(ctx, buf, len);

    ns16550_enable_interrupts(ctx, NS16550_ENABLE_ALL_INTR);
  } else {
    ns16550_enable_interrupts(ctx, NS16550_ENABLE_ALL_INTR_EXCEPT_TX);
  }
}

static void ns16550_write_support_task(
  rtems_termios_device_context *base,
  const char                   *buf,
  size_t                        len
)
{
  ns16550_context *ctx = (ns16550_context *) base;

  ctx->out_total = len;

  if (len > 0) {
    ctx->out_remaining = len;
    ctx->out_buf = buf;
    ctx->out_current = ns16550_write_to_fifo(ctx, buf, len);

    ns16550_clear_and_set_interrupts(ctx, 0, SP_INT_TX_ENABLE);
  }
}

/*
 *  ns16550_write_support_polled
 *
 *  Console Termios output entry point.
 *
 */

static void ns16550_write_support_polled(
  rtems_termios_device_context *base,
  const char                   *buf,
  size_t                        len
)
{
  size_t nwrite = 0;

  /*
   * poll each byte in the string out of the port.
   */
  while (nwrite < len) {
    /*
     * transmit character
     */
    ns16550_polled_putchar(base, *buf++);
    nwrite++;
  }
}

/*
 *  Debug gets() support
 */
int ns16550_polled_getchar(rtems_termios_device_context *base)
{
  ns16550_context     *ctx = (ns16550_context *) base;
  uint32_t             pNS16550;
  unsigned char        ucLineStatus;
  uint8_t              cChar;
  ns16550_get_reg      getReg;

  pNS16550 = ctx->port;
  getReg   = ctx->get_reg;

  ucLineStatus = (*getReg)(pNS16550, NS16550_LINE_STATUS);
  if (ucLineStatus & SP_LSR_RDY) {
    cChar = (*getReg)(pNS16550, NS16550_RECEIVE_BUFFER);
    return (int)cChar;
  }
  return -1;
}

/*
 * Flow control is only supported when using interrupts
 */

const rtems_termios_device_flow ns16550_flow_rtscts = {
  .stop_remote_tx = ns16550_negate_RTS,
  .start_remote_tx = ns16550_assert_RTS
};

const rtems_termios_device_flow ns16550_flow_dtrcts = {
  .stop_remote_tx = ns16550_negate_DTR,
  .start_remote_tx = ns16550_assert_DTR
};

const rtems_termios_device_handler ns16550_handler_interrupt = {
  .first_open = ns16550_open,
  .last_close = ns16550_close,
  .poll_read = NULL,
  .write = ns16550_write_support_int,
  .set_attributes = ns16550_set_attributes,
  .mode = TERMIOS_IRQ_DRIVEN
};

const rtems_termios_device_handler ns16550_handler_polled = {
  .first_open = ns16550_open,
  .last_close = ns16550_close,
  .poll_read = ns16550_polled_getchar,
  .write = ns16550_write_support_polled,
  .set_attributes = ns16550_set_attributes,
  .mode = TERMIOS_POLLED
};

const rtems_termios_device_handler ns16550_handler_task = {
  .first_open = ns16550_open,
  .last_close = ns16550_close,
  .poll_read = ns16550_read_task,
  .write = ns16550_write_support_task,
  .set_attributes = ns16550_set_attributes,
  .mode = TERMIOS_TASK_DRIVEN
};
