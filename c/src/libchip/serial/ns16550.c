/*
 *  This file contains the TTY driver for the NS16550
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
#include <ringbuf.h>

#include <libchip/serial.h>
#include "ns16550_p.h"

/*
 * Flow control is only supported when using interrupts
 */
console_flow ns16550_flow_RTSCTS =
{
  ns16550_negate_RTS,             /* deviceStopRemoteTx */
  ns16550_assert_RTS              /* deviceStartRemoteTx */
};

console_flow ns16550_flow_DTRCTS =
{
  ns16550_negate_DTR,             /* deviceStopRemoteTx */
  ns16550_assert_DTR              /* deviceStartRemoteTx */
};

console_fns ns16550_fns =
{
  ns16550_probe,                  /* deviceProbe */
  ns16550_open,                   /* deviceFirstOpen */
  ns16550_flush,                  /* deviceLastClose */
  NULL,                           /* deviceRead */
  ns16550_write_support_int,      /* deviceWrite */
  ns16550_initialize_interrupts,  /* deviceInitialize */
  ns16550_write_polled,           /* deviceWritePolled */
  FALSE,                          /* deviceOutputUsesInterrupts */
};

console_fns ns16550_fns_polled =
{
  ns16550_probe,                       /* deviceProbe */
  ns16550_open,                        /* deviceFirstOpen */
  ns16550_close,                       /* deviceLastClose */
  ns16550_inbyte_nonblocking_polled,   /* deviceRead */
  ns16550_write_support_polled,        /* deviceWrite */
  ns16550_init,                        /* deviceInitialize */
  ns16550_write_polled,                /* deviceWritePolled */
  FALSE,                               /* deviceOutputUsesInterrupts */
};

extern void set_vector( rtems_isr_entry, rtems_vector_number, int );

/*
 *  Types for get and set register routines
 */

typedef unsigned8 (*getRegister_f)(unsigned32 port, unsigned8 register);
typedef void      (*setRegister_f)(
                            unsigned32 port, unsigned8 reg, unsigned8 value);
/*
 *  Console Device Driver Entry Points
 */
static boolean ns16550_probe(int minor)
{
  /*
   * If the configuration dependant probe has located the device then
   * assume it is there
   */
  return(TRUE);
}

static void ns16550_init(int minor)
{
  unsigned32              pNS16550;
  unsigned8               ucTrash;
  unsigned8               ucDataByte;
  unsigned32              ulBaudDivisor;
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
  (*setReg)(pNS16550, NS16550_INTERRUPT_ENABLE, 0x0);

  /* Set the divisor latch and set the baud rate. */

  ulBaudDivisor=NS16550_Baud((unsigned32)Console_Port_Tbl[minor].pDeviceParams);
  ucDataByte = SP_LINE_DLAB;
  (*setReg)(pNS16550, NS16550_LINE_CONTROL, ucDataByte);
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

  /*
   * Disable interrupts
   */
  ucDataByte = 0;
  (*setReg)(pNS16550, NS16550_INTERRUPT_ENABLE, ucDataByte);

  /* Set data terminal ready. */
  /* And open interrupt tristate line */
  (*setReg)(pNS16550, NS16550_MODEM_CONTROL,pns16550Context->ucModemCtrl);

  ucTrash = (*getReg)(pNS16550, NS16550_LINE_STATUS );
  ucTrash = (*getReg)(pNS16550, NS16550_RECEIVE_BUFFER );
}

static int ns16550_open(
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

static int ns16550_close(
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

/* 
 *  ns16550_write_polled
 */
static void ns16550_write_polled(
  int   minor, 
  char  cChar
)
{
  unsigned32              pNS16550;
  unsigned char           ucLineStatus;
  int                     iTimeout;
  getRegister_f           getReg;
  setRegister_f           setReg;

  pNS16550 = Console_Port_Tbl[minor].ulCtrlPort1;
  getReg   = Console_Port_Tbl[minor].getRegister;
  setReg   = Console_Port_Tbl[minor].setRegister;

  /*
   * wait for transmitter holding register to be empty
   */
  iTimeout=1000;
  ucLineStatus = (*getReg)(pNS16550, NS16550_LINE_STATUS);
  while ((ucLineStatus & SP_LSR_THOLD) == 0) {
    /*
     * Yield while we wait
     */
     if(_System_state_Is_up(_System_state_Get())) {
       rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
     }
     ucLineStatus = (*getReg)(pNS16550, NS16550_LINE_STATUS);
     if(!--iTimeout) {
       break;
     }
  }

  /*
   * transmit character
   */
  (*setReg)(pNS16550, NS16550_TRANSMIT_BUFFER, cChar);
}

/*
 * These routines provide control of the RTS and DTR lines
 */
/*
 *  ns16550_assert_RTS
 */
static int ns16550_assert_RTS(int minor)
{
  unsigned32              pNS16550;
  unsigned32              Irql;
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
static int ns16550_negate_RTS(int minor)
{
  unsigned32              pNS16550;
  unsigned32              Irql;
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
static int ns16550_assert_DTR(int minor)
{
  unsigned32              pNS16550;
  unsigned32              Irql;
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
static int ns16550_negate_DTR(int minor)
{
  unsigned32              pNS16550;
  unsigned32              Irql;
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
 *  ns16550_isr
 *
 *  This routine is the console interrupt handler for COM1 and COM2
 *
 *  Input parameters:
 *    vector - vector number
 *
 *  Output parameters: NONE
 *
 *  Return values:     NONE
 */

static void ns16550_process(
        int             minor
)
{
  unsigned32              pNS16550;
  volatile unsigned8      ucLineStatus; 
  volatile unsigned8      ucInterruptId;
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
    while(TRUE) {
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

    while(TRUE) {
      if(Ring_buffer_Is_empty(&Console_Port_Data[minor].TxBuffer)) {
        Console_Port_Data[minor].bActive=FALSE;
        if(Console_Port_Tbl[minor].pDeviceFlow !=&ns16550_flow_RTSCTS) {
          ns16550_negate_RTS(minor);
        }

        /*
         * There is no data to transmit
         */
        break;
      }

      ucLineStatus = (*getReg)(pNS16550, NS16550_LINE_STATUS);
      if(~ucLineStatus & SP_LSR_THOLD) {
        /*
         * We'll get another interrupt when
         * the transmitter holding reg. becomes
         * free again
         */
        break;
      }

      Ring_buffer_Remove_character( &Console_Port_Data[minor].TxBuffer, cChar);
      /*
       * transmit character
       */
      (*setReg)(pNS16550, NS16550_TRANSMIT_BUFFER, cChar);
    }

    ucInterruptId = (*getReg)(pNS16550, NS16550_INTERRUPT_ID);
  }
  while((ucInterruptId&0xf)!=0x1);
}

static rtems_isr ns16550_isr(
  rtems_vector_number vector
)
{
  int     minor;

  for(minor=0;minor<Console_Port_Count;minor++) {
    if(vector==Console_Port_Tbl[minor].ulIntVector) {
      ns16550_process(minor);
    }
  }
}

/*
 *  ns16550_flush
 */
static int ns16550_flush(int major, int minor, void *arg)
{
  while(!Ring_buffer_Is_empty(&Console_Port_Data[minor].TxBuffer)) {
    /*
     * Yield while we wait
     */
    if(_System_state_Is_up(_System_state_Get())) {
      rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
    }
  }

  ns16550_close(major, minor, arg);

  return(RTEMS_SUCCESSFUL);
}

/*
 *  ns16550_initialize_interrupts
 *
 *  This routine initializes the console's receive and transmit
 *  ring buffers and loads the appropriate vectors to handle the interrupts.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters: NONE
 *
 *  Return values:     NONE
 */

static void ns16550_enable_interrupts(
  int minor
)
{
  unsigned32            pNS16550;
  unsigned8             ucDataByte;
  setRegister_f           setReg;

  pNS16550 = Console_Port_Tbl[minor].ulCtrlPort1;
  setReg   = Console_Port_Tbl[minor].setRegister;

  /*
   * Enable interrupts
   */
  ucDataByte = SP_INT_RX_ENABLE | SP_INT_TX_ENABLE;
  (*setReg)(pNS16550, NS16550_INTERRUPT_ENABLE, ucDataByte);

}

static void ns16550_initialize_interrupts(int minor)
{
  ns16550_init(minor);

  Ring_buffer_Initialize(&Console_Port_Data[minor].TxBuffer);

  Console_Port_Data[minor].bActive = FALSE;

  set_vector(ns16550_isr, Console_Port_Tbl[minor].ulIntVector, 1);

  ns16550_enable_interrupts(minor);
}

/* 
 *  ns16550_write_support_int
 *
 *  Console Termios output entry point.
 *
 */
static int ns16550_write_support_int(
  int   minor, 
  const char *buf, 
  int   len
)
{
  int i;
  unsigned32 Irql;

  for(i=0; i<len;) {
    if(Ring_buffer_Is_full(&Console_Port_Data[minor].TxBuffer)) {
      if(!Console_Port_Data[minor].bActive) {
        /*
         * Wake up the device
         */
        rtems_interrupt_disable(Irql);
        Console_Port_Data[minor].bActive = TRUE;
        if(Console_Port_Tbl[minor].pDeviceFlow != &ns16550_flow_RTSCTS) {
          ns16550_assert_RTS(minor);
        }
        ns16550_process(minor);
        rtems_interrupt_enable(Irql);
      } else {
        /*
         * Yield
         */
        rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
      }

      /*
       * Wait for ring buffer to empty
       */
      continue;
    }
    else {
      Ring_buffer_Add_character( &Console_Port_Data[minor].TxBuffer, buf[i]);
      i++;
    }
  }

  /*
   * Ensure that characters are on the way
   */
  if(!Console_Port_Data[minor].bActive) {
    /*
     * Wake up the device
     */
    rtems_interrupt_disable(Irql);
    Console_Port_Data[minor].bActive = TRUE;
    if(Console_Port_Tbl[minor].pDeviceFlow !=&ns16550_flow_RTSCTS) {
      ns16550_assert_RTS(minor);
    }
    ns16550_process(minor);
    rtems_interrupt_enable(Irql);
  }

  return (len);
}

/* 
 *  ns16550_write_support_polled
 *
 *  Console Termios output entry point.
 *
 */
static int ns16550_write_support_polled(
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

static int ns16550_inbyte_nonblocking_polled( 
  int minor 
)
{
  unsigned32           pNS16550;
  unsigned char        ucLineStatus;
  char                 cChar;
  getRegister_f        getReg;

  pNS16550 = Console_Port_Tbl[minor].ulCtrlPort1;
  getReg   = Console_Port_Tbl[minor].getRegister;

  ucLineStatus = (*getReg)(pNS16550, NS16550_LINE_STATUS);
  if(ucLineStatus & SP_LSR_RDY) {
    cChar = (*getReg)(pNS16550, NS16550_RECEIVE_BUFFER);
    return((int)cChar);
  } else {
    return(-1);
  }
}
