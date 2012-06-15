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
 *  http://www.rtems.com/license/LICENSE.
 */

#include <stdlib.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/ringbuf.h>
#include <rtems/bspIo.h>
#include <rtems/termiostypes.h>

#include <libchip/serial.h>
#include <libchip/sersupp.h>

#include <bsp.h>

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

/*
 * Flow control is only supported when using interrupts
 */

console_flow ns16550_flow_RTSCTS = {
  ns16550_negate_RTS,             /* deviceStopRemoteTx */
  ns16550_assert_RTS              /* deviceStartRemoteTx */
};

console_flow ns16550_flow_DTRCTS = {
  ns16550_negate_DTR,             /* deviceStopRemoteTx */
  ns16550_assert_DTR              /* deviceStartRemoteTx */
};

console_fns ns16550_fns = {
  libchip_serial_default_probe,   /* deviceProbe */
  ns16550_open,                   /* deviceFirstOpen */
  ns16550_close,                  /* deviceLastClose */
  NULL,                           /* deviceRead */
  ns16550_write_support_int,      /* deviceWrite */
  ns16550_init,                   /* deviceInitialize */
  ns16550_write_polled,           /* deviceWritePolled */
  ns16550_set_attributes,         /* deviceSetAttributes */
  true                            /* deviceOutputUsesInterrupts */
};

console_fns ns16550_fns_polled = {
  libchip_serial_default_probe,        /* deviceProbe */
  ns16550_open,                        /* deviceFirstOpen */
  ns16550_close,                       /* deviceLastClose */
  ns16550_inbyte_nonblocking_polled,   /* deviceRead */
  ns16550_write_support_polled,        /* deviceWrite */
  ns16550_init,                        /* deviceInitialize */
  ns16550_write_polled,                /* deviceWritePolled */
  ns16550_set_attributes,              /* deviceSetAttributes */
  false                                /* deviceOutputUsesInterrupts */
};

static uint32_t NS16550_GetBaudDivisor(const console_tbl *c, uint32_t baud)
{
  uint32_t clock = c->ulClock;
  uint32_t baudDivisor = (clock != 0 ? clock : 115200) / (baud * 16);

  if (c->deviceType == SERIAL_NS16550_WITH_FDR) {
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

    (*c->setRegister)(
      c->ulCtrlPort1,
      NS16550_FRACTIONAL_DIVIDER,
      fractionalDivider
    );
  }

  return baudDivisor;
}

/*
 *  ns16550_init
 */

NS16550_STATIC void ns16550_init(int minor)
{
  uintptr_t               pNS16550;
  uint8_t                 ucDataByte;
  uint32_t                ulBaudDivisor;
  ns16550_context        *pns16550Context;
  setRegister_f           setReg;
  getRegister_f           getReg;
  console_tbl             *c = Console_Port_Tbl [minor];

  pns16550Context=(ns16550_context *)malloc(sizeof(ns16550_context));

  if (pns16550Context == NULL) {
    printk( "%s: Error: Not enough memory\n", __func__);
    rtems_fatal_error_occurred( 0xdeadbeef);
  }

  Console_Port_Data[minor].pDeviceContext=(void *)pns16550Context;
  pns16550Context->ucModemCtrl=SP_MODEM_IRQ;

  pNS16550 = c->ulCtrlPort1;    
  setReg   = c->setRegister;
  getReg   = c->getRegister;

  /* Clear the divisor latch, clear all interrupt enables,
   * and reset and
   * disable the FIFO's.
   */

  (*setReg)(pNS16550, NS16550_LINE_CONTROL, 0x0);
  ns16550_enable_interrupts( c, NS16550_DISABLE_ALL_INTR );

  /* Set the divisor latch and set the baud rate. */

  ulBaudDivisor = NS16550_GetBaudDivisor(c, (uintptr_t) c->pDeviceParams);
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

  ns16550_enable_interrupts(c, NS16550_DISABLE_ALL_INTR);

  /* Set data terminal ready. */
  /* And open interrupt tristate line */
  (*setReg)(pNS16550, NS16550_MODEM_CONTROL,pns16550Context->ucModemCtrl);

  (*getReg)(pNS16550, NS16550_LINE_STATUS );
  (*getReg)(pNS16550, NS16550_RECEIVE_BUFFER );
}

/*
 *  ns16550_open
 */

NS16550_STATIC int ns16550_open(
  int major,
  int minor,
  void *arg
)
{
  rtems_libio_open_close_args_t *oc = (rtems_libio_open_close_args_t *) arg;
  struct rtems_termios_tty *tty = (struct rtems_termios_tty *) oc->iop->data1;
  console_tbl *c = Console_Port_Tbl [minor];
  console_data *d = &Console_Port_Data [minor];

  d->termios_data = tty;

  /* Assert DTR */
  if (c->pDeviceFlow != &ns16550_flow_DTRCTS) {
    ns16550_assert_DTR( minor);
  }

  /* Set initial baud */
  rtems_termios_set_initial_baud( tty, (intptr_t) c->pDeviceParams);

  if (c->pDeviceFns->deviceOutputUsesInterrupts) {
    ns16550_initialize_interrupts( minor);
    ns16550_enable_interrupts( c, NS16550_ENABLE_ALL_INTR_EXCEPT_TX);
  }

  return RTEMS_SUCCESSFUL;
}

/*
 *  ns16550_close
 */

NS16550_STATIC int ns16550_close(
  int      major,
  int      minor,
  void    * arg
)
{
  console_tbl *c = Console_Port_Tbl [minor];

  /*
   * Negate DTR
   */
  if (c->pDeviceFlow != &ns16550_flow_DTRCTS) {
    ns16550_negate_DTR(minor);
  }

  ns16550_enable_interrupts(c, NS16550_DISABLE_ALL_INTR);

  if (c->pDeviceFns->deviceOutputUsesInterrupts) {
    ns16550_cleanup_interrupts(minor);
  }

  return(RTEMS_SUCCESSFUL);
}

/**
 * @brief Polled write for NS16550.
 */
void ns16550_outch_polled(console_tbl *c, char out)
{
  uintptr_t port = c->ulCtrlPort1;
  getRegister_f get = c->getRegister;
  setRegister_f set = c->setRegister;
  uint32_t status = 0;
  rtems_interrupt_level level;

  /* Save port interrupt mask */
  uint32_t interrupt_mask = get( port, NS16550_INTERRUPT_ENABLE);

  /* Disable port interrupts */
  ns16550_enable_interrupts( c, NS16550_DISABLE_ALL_INTR);

  while (true) {
    /* Try to transmit the character in a critical section */
    rtems_interrupt_disable( level);

    /* Read the transmitter holding register and check it */
    status = get( port, NS16550_LINE_STATUS);
    if ((status & SP_LSR_THOLD) != 0) {
      /* Transmit character */
      set( port, NS16550_TRANSMIT_BUFFER, out);

      /* Finished */
      rtems_interrupt_enable( level);
      break;
    } else {
      rtems_interrupt_enable( level);
    }

    /* Wait for transmitter holding register to be empty */
    do {
      status = get( port, NS16550_LINE_STATUS);
    } while ((status & SP_LSR_THOLD) == 0);
  }

  /* Restore port interrupt mask */
  set( port, NS16550_INTERRUPT_ENABLE, interrupt_mask);
}

NS16550_STATIC void ns16550_write_polled(int minor, char out)
{
  console_tbl *c = Console_Port_Tbl [minor];
  
  ns16550_outch_polled( c, out );
}

/*
 * These routines provide control of the RTS and DTR lines
 */

/*
 *  ns16550_assert_RTS
 */

NS16550_STATIC int ns16550_assert_RTS(int minor)
{
  uint32_t                pNS16550;
  uint32_t                Irql;
  ns16550_context        *pns16550Context;
  setRegister_f           setReg;

  pns16550Context=(ns16550_context *) Console_Port_Data[minor].pDeviceContext;

  pNS16550 = Console_Port_Tbl[minor]->ulCtrlPort1;
  setReg   = Console_Port_Tbl[minor]->setRegister;

  /*
   * Assert RTS
   */
  rtems_interrupt_disable(Irql);
  pns16550Context->ucModemCtrl|=SP_MODEM_RTS;
  (*setReg)(pNS16550, NS16550_MODEM_CONTROL, pns16550Context->ucModemCtrl);
  rtems_interrupt_enable(Irql);
  return 0;
}

/*
 *  ns16550_negate_RTS
 */

NS16550_STATIC int ns16550_negate_RTS(int minor)
{
  uint32_t                pNS16550;
  uint32_t                Irql;
  ns16550_context        *pns16550Context;
  setRegister_f           setReg;

  pns16550Context=(ns16550_context *) Console_Port_Data[minor].pDeviceContext;

  pNS16550 = Console_Port_Tbl[minor]->ulCtrlPort1;
  setReg   = Console_Port_Tbl[minor]->setRegister;

  /*
   * Negate RTS
   */
  rtems_interrupt_disable(Irql);
  pns16550Context->ucModemCtrl&=~SP_MODEM_RTS;
  (*setReg)(pNS16550, NS16550_MODEM_CONTROL, pns16550Context->ucModemCtrl);
  rtems_interrupt_enable(Irql);
  return 0;
}

/*
 * These flow control routines utilise a connection from the local DTR
 * line to the remote CTS line
 */

/*
 *  ns16550_assert_DTR
 */

NS16550_STATIC int ns16550_assert_DTR(int minor)
{
  uint32_t                pNS16550;
  uint32_t                Irql;
  ns16550_context        *pns16550Context;
  setRegister_f           setReg;

  pns16550Context=(ns16550_context *) Console_Port_Data[minor].pDeviceContext;

  pNS16550 = Console_Port_Tbl[minor]->ulCtrlPort1;
  setReg   = Console_Port_Tbl[minor]->setRegister;

  /*
   * Assert DTR
   */
  rtems_interrupt_disable(Irql);
  pns16550Context->ucModemCtrl|=SP_MODEM_DTR;
  (*setReg)(pNS16550, NS16550_MODEM_CONTROL, pns16550Context->ucModemCtrl);
  rtems_interrupt_enable(Irql);
  return 0;
}

/*
 *  ns16550_negate_DTR
 */

NS16550_STATIC int ns16550_negate_DTR(int minor)
{
  uint32_t                pNS16550;
  uint32_t                Irql;
  ns16550_context        *pns16550Context;
  setRegister_f           setReg;

  pns16550Context=(ns16550_context *) Console_Port_Data[minor].pDeviceContext;

  pNS16550 = Console_Port_Tbl[minor]->ulCtrlPort1;
  setReg   = Console_Port_Tbl[minor]->setRegister;

  /*
   * Negate DTR
   */
  rtems_interrupt_disable(Irql);
  pns16550Context->ucModemCtrl&=~SP_MODEM_DTR;
  (*setReg)(pNS16550, NS16550_MODEM_CONTROL,pns16550Context->ucModemCtrl);
  rtems_interrupt_enable(Irql);
  return 0;
}

/*
 *  ns16550_set_attributes
 *
 *  This function sets the channel to reflect the requested termios
 *  port settings.
 */

NS16550_STATIC int ns16550_set_attributes(
  int                   minor,
  const struct termios *t
)
{
  uint32_t                pNS16550;
  uint32_t                ulBaudDivisor;
  uint8_t                 ucLineControl;
  uint32_t                baud_requested;
  setRegister_f           setReg;
  getRegister_f           getReg;
  uint32_t                Irql;
  const console_tbl      *c = Console_Port_Tbl [minor];

  pNS16550 = c->ulCtrlPort1;
  setReg   = c->setRegister;
  getReg   = c->getRegister;

  /*
   *  Calculate the baud rate divisor
   */

  baud_requested = rtems_termios_baud_to_number(t->c_cflag);
  ulBaudDivisor = NS16550_GetBaudDivisor(c, baud_requested);

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

  rtems_interrupt_disable(Irql);

    /*
     *  Set the baud rate
     */

    (*setReg)(pNS16550, NS16550_LINE_CONTROL, SP_LINE_DLAB);
    /* XXX are these registers right? */
    (*setReg)(pNS16550, NS16550_TRANSMIT_BUFFER, ulBaudDivisor&0xff);
    (*setReg)(pNS16550, NS16550_INTERRUPT_ENABLE, (ulBaudDivisor>>8)&0xff);

    /*
     *  Now write the line control
     */
    (*setReg)(pNS16550, NS16550_LINE_CONTROL, ucLineControl );

  rtems_interrupt_enable(Irql);

  return 0;
}

#if defined(BSP_FEATURE_IRQ_EXTENSION) || defined(BSP_FEATURE_IRQ_LEGACY)

/**
 * @brief Process interrupt.
 */
NS16550_STATIC void ns16550_process( int minor)
{
  console_tbl *c = Console_Port_Tbl [minor];
  console_data *d = &Console_Port_Data [minor];
  ns16550_context *ctx = d->pDeviceContext;
  uint32_t port = c->ulCtrlPort1;
  getRegister_f get = c->getRegister;
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
    rtems_termios_enqueue_raw_characters( d->termios_data, buf, i);

    /* Check if we can dequeue transmitted characters */
    if (ctx->transmitFifoChars > 0
        && (get( port, NS16550_LINE_STATUS) & SP_LSR_THOLD) != 0) {
      unsigned chars = ctx->transmitFifoChars;

      /*
       * We finished the transmission, so clear the number of characters in the
       * transmit FIFO.
       */
      ctx->transmitFifoChars = 0;

      /* Dequeue transmitted characters */
      if (rtems_termios_dequeue_characters( d->termios_data, chars) == 0) {
        /* Nothing to do */
        d->bActive = false;
        ns16550_enable_interrupts( c, NS16550_ENABLE_ALL_INTR_EXCEPT_TX);
      }
    }
  } while ((get( port, NS16550_INTERRUPT_ID) & SP_IID_0) == 0);
}
#endif

/**
 * @brief Transmits up to @a len characters from @a buf.
 *
 * This routine is invoked either from task context with disabled interrupts to
 * start a new transmission process with exactly one character in case of an
 * idle output state or from the interrupt handler to refill the transmitter.
 *
 * Returns always zero.
 */
NS16550_STATIC ssize_t ns16550_write_support_int(
  int minor,
  const char *buf,
  size_t len
)
{
  console_tbl *c = Console_Port_Tbl [minor];
  console_data *d = &Console_Port_Data [minor];
  ns16550_context *ctx = d->pDeviceContext;
  uint32_t port = c->ulCtrlPort1;
  setRegister_f set = c->setRegister;
  int i = 0;
  int out = len > SP_FIFO_SIZE ? SP_FIFO_SIZE : len;

  for (i = 0; i < out; ++i) {
    set( port, NS16550_TRANSMIT_BUFFER, buf [i]);
  }

  if (len > 0) {
    ctx->transmitFifoChars = out;
    d->bActive = true;
    ns16550_enable_interrupts( c, NS16550_ENABLE_ALL_INTR);
  }

  return 0;
}

/*
 *  ns16550_enable_interrupts
 *
 *  This routine initializes the port to have the specified interrupts masked.
 */
NS16550_STATIC void ns16550_enable_interrupts(
  console_tbl *c,
  int         mask
)
{
  uint32_t       pNS16550;
  setRegister_f  setReg;

  pNS16550 = c->ulCtrlPort1;
  setReg   = c->setRegister;

  (*setReg)(pNS16550, NS16550_INTERRUPT_ENABLE, mask);
}

#if defined(BSP_FEATURE_IRQ_EXTENSION) || defined(BSP_FEATURE_IRQ_LEGACY)
  NS16550_STATIC rtems_isr ns16550_isr(void *arg)
  {
    int minor = (int) arg;

    ns16550_process( minor);
  }
#endif

/*
 *  ns16550_initialize_interrupts
 *
 *  This routine initializes the port to operate in interrupt driver mode.
 */
NS16550_STATIC void ns16550_initialize_interrupts( int minor)
{
#if defined(BSP_FEATURE_IRQ_EXTENSION) || defined(BSP_FEATURE_IRQ_LEGACY)
  console_tbl *c = Console_Port_Tbl [minor];
#endif
  console_data *d = &Console_Port_Data [minor];

  d->bActive = false;

  #ifdef BSP_FEATURE_IRQ_EXTENSION
    {
      rtems_status_code sc = RTEMS_SUCCESSFUL;
      sc = rtems_interrupt_handler_install(
        c->ulIntVector,
        "NS16550",
        RTEMS_INTERRUPT_SHARED,
        ns16550_isr,
        (void *) minor
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
          c->ulIntVector,
          ns16550_isr,
          (void *) minor,
          NULL,
          NULL,
          NULL,
          NULL
        };
        rv = BSP_install_rtems_shared_irq_handler( &cd);
      #else
        rtems_irq_connect_data cd = {
          c->ulIntVector,
          ns16550_isr,
          (void *) minor,
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

NS16550_STATIC void ns16550_cleanup_interrupts(int minor)
{
  #if defined(BSP_FEATURE_IRQ_EXTENSION)
    rtems_status_code sc = RTEMS_SUCCESSFUL;
    console_tbl *c = Console_Port_Tbl [minor];
    sc = rtems_interrupt_handler_remove(
      c->ulIntVector,
      ns16550_isr,
      (void *) minor
    );
    if (sc != RTEMS_SUCCESSFUL) {
      /* FIXME */
      printk("%s: Error: Remove interrupt handler\n", __func__);
      rtems_fatal_error_occurred(0xdeadbeef);
    }
  #elif defined(BSP_FEATURE_IRQ_LEGACY)
    int rv = 0;
    console_tbl *c = Console_Port_Tbl [minor];
    rtems_irq_connect_data cd = {
      .name = c->ulIntVector,
      .hdl = ns16550_isr,
      .handle = (void *) minor
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
 *  ns16550_write_support_polled
 *
 *  Console Termios output entry point.
 *
 */

NS16550_STATIC ssize_t ns16550_write_support_polled(
  int         minor,
  const char *buf,
  size_t      len
)
{
  int nwrite = 0;

  /*
   * poll each byte in the string out of the port.
   */
  while (nwrite < len) {
    /*
     * transmit character
     */
    ns16550_write_polled(minor, *buf++);
    nwrite++;
  }

  /*
   * return the number of bytes written.
   */
  return nwrite;
}

/*
 *  Debug gets() support
 */
int ns16550_inch_polled(
  console_tbl *c
)
{
  uint32_t             pNS16550;
  unsigned char        ucLineStatus;
  uint8_t              cChar;
  getRegister_f        getReg;

  pNS16550 = c->ulCtrlPort1;
  getReg   = c->getRegister;

  ucLineStatus = (*getReg)(pNS16550, NS16550_LINE_STATUS);
  if (ucLineStatus & SP_LSR_RDY) {
    cChar = (*getReg)(pNS16550, NS16550_RECEIVE_BUFFER);
    return (int)cChar;
  }
  return -1;
}

/*
 *  ns16550_inbyte_nonblocking_polled
 *
 *  Console Termios polling input entry point.
 */
NS16550_STATIC int ns16550_inbyte_nonblocking_polled(int minor)
{
  console_tbl *c = Console_Port_Tbl [minor];
  
  return ns16550_inch_polled( c );
}
