/*
 *  This file contains the TTY driver for the National Semiconductor NS16550.
 *
 *  This part is widely cloned and second sourced.  It is found in a number
 *  of "Super IO" controllers.
 *
 *  COPYRIGHT (c) 1998 by Radstone Technology
 *
 *
 * THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 * AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 * You are hereby granted permission to use, copy, modify, and distribute
 * this file, provided that this notice, plus the above copyright notice
 * and disclaimer, appears in all copies. Radstone Technology will provide
 * no support for this code.
 *
 *  This driver uses the termios pseudo driver.
 */

#include <rtems.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <rtems/ringbuf.h>

#include <libchip/serial.h>
#include <libchip/sersupp.h>
#include <rtems/bspIo.h>
#include "ns16550_p.h"

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
  NULL,                           /* deviceLastClose */
  NULL,                           /* deviceRead */
  ns16550_write_support_int,      /* deviceWrite */
  ns16550_initialize_interrupts,  /* deviceInitialize */
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

#if defined(__PPC__)
#ifdef _OLD_EXCEPTIONS
extern void set_vector( rtems_isr_entry, rtems_vector_number, int );
#else
#include <bsp/irq.h>
#endif
#endif

/*
 *  ns16550_init
 */

NS16550_STATIC void ns16550_init(int minor)
{
  uint32_t                pNS16550;
  uint8_t                 ucTrash;
  uint8_t                 ucDataByte;
  uint32_t                ulBaudDivisor;
  ns16550_context        *pns16550Context;
  setRegister_f           setReg;
  getRegister_f           getReg;

  pns16550Context=(ns16550_context *)malloc(sizeof(ns16550_context));

  Console_Port_Data[minor].pDeviceContext=(void *)pns16550Context;
  pns16550Context->ucModemCtrl=SP_MODEM_IRQ;

  pNS16550 = Console_Port_Tbl[minor].ulCtrlPort1;
  setReg   = Console_Port_Tbl[minor].setRegister;
  getReg   = Console_Port_Tbl[minor].getRegister;

  /* Clear the divisor latch, clear all interrupt enables,
   * and reset and
   * disable the FIFO's.
   */

  (*setReg)(pNS16550, NS16550_LINE_CONTROL, 0x0);
  ns16550_enable_interrupts(minor, NS16550_DISABLE_ALL_INTR);

  /* Set the divisor latch and set the baud rate. */

  ulBaudDivisor = NS16550_Baud(
    (uint32_t) Console_Port_Tbl[minor].ulClock,
    (uint32_t) Console_Port_Tbl[minor].pDeviceParams
  );
  ucDataByte = SP_LINE_DLAB;
  (*setReg)(pNS16550, NS16550_LINE_CONTROL, ucDataByte);

  /* XXX */
  (*setReg)(pNS16550, NS16550_TRANSMIT_BUFFER, ulBaudDivisor&0xff);
  (*setReg)(pNS16550, NS16550_INTERRUPT_ENABLE, (ulBaudDivisor>>8)&0xff);

  /* Clear the divisor latch and set the character size to eight bits */
  /* with one stop bit and no parity checking. */
  ucDataByte = EIGHT_BITS;
  (*setReg)(pNS16550, NS16550_LINE_CONTROL, ucDataByte);

  /* Enable and reset transmit and receive FIFOs. TJA     */
  ucDataByte = SP_FIFO_ENABLE;
  (*setReg)(pNS16550, NS16550_FIFO_CONTROL, ucDataByte);

  ucDataByte = SP_FIFO_ENABLE | SP_FIFO_RXRST | SP_FIFO_TXRST;
  (*setReg)(pNS16550, NS16550_FIFO_CONTROL, ucDataByte);

  ns16550_enable_interrupts(minor, NS16550_DISABLE_ALL_INTR);

  /* Set data terminal ready. */
  /* And open interrupt tristate line */
  (*setReg)(pNS16550, NS16550_MODEM_CONTROL,pns16550Context->ucModemCtrl);

  ucTrash = (*getReg)(pNS16550, NS16550_LINE_STATUS );
  ucTrash = (*getReg)(pNS16550, NS16550_RECEIVE_BUFFER );
}

/*
 *  ns16550_open
 */

NS16550_STATIC int ns16550_open(
  int      major,
  int      minor,
  void    * arg
)
{
  /*
   * Assert DTR
   */

  if(Console_Port_Tbl[minor].pDeviceFlow != &ns16550_flow_DTRCTS) {
    ns16550_assert_DTR(minor);
  }

  return(RTEMS_SUCCESSFUL);
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
  /*
   * Negate DTR
   */
  if(Console_Port_Tbl[minor].pDeviceFlow != &ns16550_flow_DTRCTS) {
    ns16550_negate_DTR(minor);
  }

  return(RTEMS_SUCCESSFUL);
}

/**
 * @brief Polled write for NS16550.
 */
NS16550_STATIC void ns16550_write_polled( int minor, char c)
{
  uint32_t port = Console_Port_Tbl [minor].ulCtrlPort1;
  getRegister_f get = Console_Port_Tbl [minor].getRegister;
  setRegister_f set = Console_Port_Tbl [minor].setRegister;
  uint32_t status;
  rtems_interrupt_level level;

  while (1) {
    /* Try to transmit the character in a critical section */
    rtems_interrupt_disable( level);

    /* Read the transmitter holding register and check it */
    status = get( port, NS16550_LINE_STATUS);
    if ((status & SP_LSR_THOLD) != 0) {
      /* Transmit character */
      set( port, NS16550_TRANSMIT_BUFFER, c);

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

  pNS16550 = Console_Port_Tbl[minor].ulCtrlPort1;
  setReg   = Console_Port_Tbl[minor].setRegister;

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

  pNS16550 = Console_Port_Tbl[minor].ulCtrlPort1;
  setReg   = Console_Port_Tbl[minor].setRegister;

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

  pNS16550 = Console_Port_Tbl[minor].ulCtrlPort1;
  setReg   = Console_Port_Tbl[minor].setRegister;

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

  pNS16550 = Console_Port_Tbl[minor].ulCtrlPort1;
  setReg   = Console_Port_Tbl[minor].setRegister;

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

  pNS16550 = Console_Port_Tbl[minor].ulCtrlPort1;
  setReg   = Console_Port_Tbl[minor].setRegister;
  getReg   = Console_Port_Tbl[minor].getRegister;

  /*
   *  Calculate the baud rate divisor
   */

  baud_requested = t->c_cflag & CBAUD;
  if (!baud_requested)
    baud_requested = B9600;              /* default to 9600 baud */

  ulBaudDivisor = NS16550_Baud(
    (uint32_t) Console_Port_Tbl[minor].ulClock,
    termios_baud_to_number(baud_requested)
  );

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

/*
 *  ns16550_process
 *
 *  This routine is the console interrupt handler for A port.
 */

NS16550_STATIC void ns16550_process(
        int             minor
)
{
  uint32_t                pNS16550;
  volatile uint8_t        ucLineStatus;
  volatile uint8_t        ucInterruptId;
  char                    cChar;
  getRegister_f           getReg;
  setRegister_f           setReg;

  pNS16550 = Console_Port_Tbl[minor].ulCtrlPort1;
  getReg   = Console_Port_Tbl[minor].getRegister;
  setReg   = Console_Port_Tbl[minor].setRegister;

  do {
    /*
     * Deal with any received characters
     */
    while(true) {
      ucLineStatus = (*getReg)(pNS16550, NS16550_LINE_STATUS);
      if(~ucLineStatus & SP_LSR_RDY) {
        break;
      }
      cChar = (*getReg)(pNS16550, NS16550_RECEIVE_BUFFER);
      rtems_termios_enqueue_raw_characters(
        Console_Port_Data[minor].termios_data,
        &cChar,
        1
      );
    }

    /*
     *  TX all the characters we can
     */

    while(true) {
        ucLineStatus = (*getReg)(pNS16550, NS16550_LINE_STATUS);
        if(~ucLineStatus & SP_LSR_THOLD) {
          /*
           * We'll get another interrupt when
           * the transmitter holding reg. becomes
           * free again
           */
          break;
        }

#if 0
        /* XXX flow control not completely supported in libchip */

        if(Console_Port_Tbl[minor].pDeviceFlow != &ns16550_flow_RTSCTS) {
          ns16550_negate_RTS(minor);
        }
#endif

    rtems_termios_dequeue_characters(Console_Port_Data[minor].termios_data, 1);
    if (rtems_termios_dequeue_characters(
         Console_Port_Data[minor].termios_data, 1)) {
        if (Console_Port_Tbl[minor].pDeviceFlow != &ns16550_flow_RTSCTS) {
          ns16550_negate_RTS(minor);
        }
        Console_Port_Data[minor].bActive = FALSE;
        ns16550_enable_interrupts(minor, NS16550_ENABLE_ALL_INTR_EXCEPT_TX);
        break;
      }

      ucInterruptId = (*getReg)(pNS16550, NS16550_INTERRUPT_ID);
    }
  } while((ucInterruptId&0xf)!=0x1);
}

#if defined(__PPC__)
#ifdef _OLD_EXCEPTIONS 

/*
 *  ns16550_isr
 */

NS16550_STATIC rtems_isr ns16550_isr(
  rtems_vector_number vector
)
{
  int     minor;

  for(minor=0;minor<Console_Port_Count;minor++) {
    if(Console_Port_Tbl[minor].ulIntVector == vector &&
       Console_Port_Tbl[minor].deviceType == SERIAL_NS16550 ) {
      ns16550_process(minor);
    }
  }
}

#else

NS16550_STATIC rtems_isr ns16550_isr(
  void *entry 
)
{
  console_tbl *ptr = entry;
  int         minor;

  for(minor=0;minor<Console_Port_Count;minor++) {
    if( &Console_Port_Tbl[minor] == ptr ) {
      ns16550_process(minor);
    }
  }

}

#endif
#endif

/*
 *  ns16550_enable_interrupts
 *
 *  This routine initializes the port to have the specified interrupts masked.
 */

NS16550_STATIC void ns16550_enable_interrupts(
  int minor,
  int mask
)
{
  uint32_t       pNS16550;
  setRegister_f  setReg;

  pNS16550 = Console_Port_Tbl[minor].ulCtrlPort1;
  setReg   = Console_Port_Tbl[minor].setRegister;

  (*setReg)(pNS16550, NS16550_INTERRUPT_ENABLE, mask);
}

/*
 *  ns16550_initialize_interrupts
 *
 *  This routine initializes the port to operate in interrupt driver mode.
 */

#if defined(__PPC__)
#ifdef _OLD_EXCEPTIONS
NS16550_STATIC void ns16550_initialize_interrupts(int minor)
{
  ns16550_init(minor);

  Console_Port_Data[minor].bActive = FALSE;

  set_vector(ns16550_isr, Console_Port_Tbl[minor].ulIntVector, 1);
  
  ns16550_enable_interrupts(minor, NS16550_ENABLE_ALL_INTR);
}
#else

NS16550_STATIC void ns16550_initialize_interrupts(int minor)
{
#ifdef BSP_SHARED_HANDLER_SUPPORT
  rtems_irq_connect_data IrqData = {0,
                                    ns16550_isr,
                                    &Console_Port_Data[minor],
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL
                                   };
#else
  rtems_irq_connect_data IrqData = {0,
                                    ns16550_isr,
                                    &Console_Port_Data[minor],
                                    NULL,
                                    NULL,
                                    NULL
                                   };
#endif

  ns16550_init(minor);

  Console_Port_Data[minor].bActive = FALSE;

  IrqData.name  = (rtems_irq_number)(Console_Port_Tbl[minor].ulIntVector );

#ifdef BSP_SHARED_HANDLER_SUPPORT
  if (!BSP_install_rtems_shared_irq_handler (&IrqData)) {
#else
  if (!BSP_install_rtems_irq_handler(&IrqData)) {
#endif
    printk("Error installing interrupt handler!\n");
    rtems_fatal_error_occurred(1);
  }

  ns16550_enable_interrupts(minor, NS16550_ENABLE_ALL_INTR);
}

#endif
#endif

/*
 *  ns16550_write_support_int
 *
 *  Console Termios output entry point.
 */

NS16550_STATIC int ns16550_write_support_int(
  int   minor,
  const char *buf,
  int   len
)
{
  uint32_t       Irql;
  uint32_t       pNS16550;
  setRegister_f  setReg;

  setReg   = Console_Port_Tbl[minor].setRegister;
  pNS16550 = Console_Port_Tbl[minor].ulCtrlPort1;

  /*
   *  We are using interrupt driven output and termios only sends us
   *  one character at a time.
   */

  if ( !len )
    return 0;

  if(Console_Port_Tbl[minor].pDeviceFlow != &ns16550_flow_RTSCTS) {
    ns16550_assert_RTS(minor);
  }

  rtems_interrupt_disable(Irql);
    if ( Console_Port_Data[minor].bActive == FALSE) {
      Console_Port_Data[minor].bActive = TRUE;
      ns16550_enable_interrupts(minor, NS16550_ENABLE_ALL_INTR);
    }
    (*setReg)(pNS16550, NS16550_TRANSMIT_BUFFER, *buf);
  rtems_interrupt_enable(Irql);

  return 1;
}

/*
 *  ns16550_write_support_polled
 *
 *  Console Termios output entry point.
 *
 */

NS16550_STATIC int ns16550_write_support_polled(
  int         minor,
  const char *buf,
  int         len
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
 *  ns16550_inbyte_nonblocking_polled
 *
 *  Console Termios polling input entry point.
 */

NS16550_STATIC int ns16550_inbyte_nonblocking_polled(
  int minor
)
{
  uint32_t             pNS16550;
  unsigned char        ucLineStatus;
  char                 cChar;
  getRegister_f        getReg;

  pNS16550 = Console_Port_Tbl[minor].ulCtrlPort1;
  getReg   = Console_Port_Tbl[minor].getRegister;

  ucLineStatus = (*getReg)(pNS16550, NS16550_LINE_STATUS);
  if(ucLineStatus & SP_LSR_RDY) {
    cChar = (*getReg)(pNS16550, NS16550_RECEIVE_BUFFER);
    return (int)cChar;
  } else {
    return -1;
  }
}
