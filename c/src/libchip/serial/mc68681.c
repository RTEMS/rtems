/*
 *  This file contains the termios TTY driver for the Motorola MC68681.
 *
 *  This part is available from a number of secondary sources.
 *  In particular, we know about the following:
 *
 *     + Exar 88c681 and 68c681
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <ringbuf.h>

#include <libchip/serial.h>
#include "mc68681_p.h"
#include "mc68681.h"

/*
 * Flow control is only supported when using interrupts
 */

console_flow mc68681_flow_RTSCTS =
{
  mc68681_negate_RTS,             /* deviceStopRemoteTx */
  mc68681_assert_RTS              /* deviceStartRemoteTx */
};

console_flow mc68681_flow_DTRCTS =
{
  mc68681_negate_DTR,             /* deviceStopRemoteTx */
  mc68681_assert_DTR              /* deviceStartRemoteTx */
};

console_fns mc68681_fns =
{
  mc68681_probe,                  /* deviceProbe */
  mc68681_open,                   /* deviceFirstOpen */
  mc68681_flush,                  /* deviceLastClose */
  NULL,                           /* deviceRead */
  mc68681_write_support_int,      /* deviceWrite */
  mc68681_initialize_interrupts,  /* deviceInitialize */
  mc68681_write_polled,           /* deviceWritePolled */
  FALSE,                          /* deviceOutputUsesInterrupts */
};

console_fns mc68681_fns_polled =
{
  mc68681_probe,                       /* deviceProbe */
  mc68681_open,                        /* deviceFirstOpen */
  mc68681_close,                       /* deviceLastClose */
  mc68681_inbyte_nonblocking_polled,   /* deviceRead */
  mc68681_write_support_polled,        /* deviceWrite */
  mc68681_init,                        /* deviceInitialize */
  mc68681_write_polled,                /* deviceWritePolled */
  FALSE,                               /* deviceOutputUsesInterrupts */
};

extern void set_vector( rtems_isr_entry, rtems_vector_number, int );

/*
 *  Console Device Driver Entry Points
 */

static boolean mc68681_probe(int minor)
{
  /*
   * If the configuration dependent probe has located the device then
   * assume it is there
   */
  return(TRUE);
}

static void mc68681_init(int minor)
{
/* XXX */
  unsigned32              pMC68681;
  unsigned8               ucTrash;
  unsigned8               ucDataByte;
  unsigned32              ulBaudDivisor;
  mc68681_context        *pmc68681Context;
  setRegister_f           setReg;
  getRegister_f           getReg;

#if 1
ulBaudDivisor = ucDataByte = ucTrash =  0;
#endif
  pmc68681Context = (mc68681_context *) malloc(sizeof(mc68681_context));

  Console_Port_Data[minor].pDeviceContext = (void *)pmc68681Context;
#if 0
  pmc68681Context->ucModemCtrl = SP_MODEM_IRQ;
#endif

  pMC68681 = Console_Port_Tbl[minor].ulCtrlPort1;
  setReg   = Console_Port_Tbl[minor].setRegister;
  getReg   = Console_Port_Tbl[minor].getRegister;

#if 0
  /* Clear the divisor latch, clear all interrupt enables,
   * and reset and
   * disable the FIFO's.
   */

  (*setReg)(pMC68681, MC68681_LINE_CONTROL, 0x0);
  (*setReg)(pMC68681, MC68681_INTERRUPT_ENABLE, 0x0);

  /* Set the divisor latch and set the baud rate. */

  ulBaudDivisor=MC68681_Baud((unsigned32)Console_Port_Tbl[minor].pDeviceParams);
  ucDataByte = SP_LINE_DLAB;
  (*setReg)(pMC68681, MC68681_LINE_CONTROL, ucDataByte);
  (*setReg)(pMC68681, MC68681_TRANSMIT_BUFFER, ulBaudDivisor&0xff);
  (*setReg)(pMC68681, MC68681_INTERRUPT_ENABLE, (ulBaudDivisor>>8)&0xff);

  /* Clear the divisor latch and set the character size to eight bits */
  /* with one stop bit and no parity checking. */
  ucDataByte = EIGHT_BITS;
  (*setReg)(pMC68681, MC68681_LINE_CONTROL, ucDataByte);

  /* Enable and reset transmit and receive FIFOs. TJA     */
  ucDataByte = SP_FIFO_ENABLE;
  (*setReg)(pMC68681, MC68681_FIFO_CONTROL, ucDataByte);

  ucDataByte = SP_FIFO_ENABLE | SP_FIFO_RXRST | SP_FIFO_TXRST;
  (*setReg)(pMC68681, MC68681_FIFO_CONTROL, ucDataByte);

  /*
   * Disable interrupts
   */
  ucDataByte = 0;
  (*setReg)(pMC68681, MC68681_INTERRUPT_ENABLE, ucDataByte);

  /* Set data terminal ready. */
  /* And open interrupt tristate line */
  (*setReg)(pMC68681, MC68681_MODEM_CONTROL,pmc68681Context->ucModemCtrl);

  ucTrash = (*getReg)(pMC68681, MC68681_LINE_STATUS );
  ucTrash = (*getReg)(pMC68681, MC68681_RECEIVE_BUFFER );
#endif
}

static int mc68681_open(
  int      major,
  int      minor,
  void    * arg
)
{
/* XXX */
  /*
   * Assert DTR
   */

  if(Console_Port_Tbl[minor].pDeviceFlow != &mc68681_flow_DTRCTS) {
    mc68681_assert_DTR(minor);
  }

  return(RTEMS_SUCCESSFUL);
}

static int mc68681_close(
  int      major,
  int      minor,
  void    * arg
)
{
/* XXX */
  /*
   * Negate DTR
   */
  if(Console_Port_Tbl[minor].pDeviceFlow != &mc68681_flow_DTRCTS) {
    mc68681_negate_DTR(minor);
  }

  return(RTEMS_SUCCESSFUL);
}

/* 
 *  mc68681_write_polled
 */

static void mc68681_write_polled(
  int   minor, 
  char  cChar
)
{
  unsigned32              pMC68681;
  unsigned char           ucLineStatus;
  int                     iTimeout;
  getRegister_f           getReg;
  setData_f               setData;

  pMC68681 = Console_Port_Tbl[minor].ulCtrlPort1;
  getReg   = Console_Port_Tbl[minor].getRegister;
  setData  = Console_Port_Tbl[minor].setData;

  /*
   * wait for transmitter holding register to be empty
   */
  iTimeout = 1000;
  ucLineStatus = (*getReg)(pMC68681, MC68681_STATUS_REG);
  while ((ucLineStatus & MC68681_TX_READY) == 0) {

    /*
     * Yield while we wait
     */

     if(_System_state_Is_up(_System_state_Get())) {
       rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
     }
     ucLineStatus = (*getReg)(pMC68681, MC68681_STATUS_REG);
     if(!--iTimeout) {
       break;
     }
  }

  /*
   * transmit character
   */

  (*setData)(pMC68681, cChar);
}

/*
 * These routines provide control of the RTS and DTR lines
 */

/*
 *  mc68681_assert_RTS
 */

static int mc68681_assert_RTS(int minor)
{
/* XXX */

  unsigned32              pMC68681;
  unsigned32              Irql;
  mc68681_context        *pmc68681Context;
  setRegister_f           setReg;


  pmc68681Context = (mc68681_context *) Console_Port_Data[minor].pDeviceContext;

  pMC68681 = Console_Port_Tbl[minor].ulCtrlPort1;
  setReg   = Console_Port_Tbl[minor].setRegister;

  /*
   * Assert RTS
   */
  rtems_interrupt_disable(Irql);
#if 0
  pmc68681Context->ucModemCtrl |= SP_MODEM_RTS;
  (*setReg)(pMC68681, MC68681_MODEM_CONTROL, pmc68681Context->ucModemCtrl);
#endif
  rtems_interrupt_enable(Irql);
  return 0;
}

/*
 *  mc68681_negate_RTS
 */
static int mc68681_negate_RTS(int minor)
{
/* XXX */
  unsigned32              pMC68681;
  unsigned32              Irql;
  mc68681_context        *pmc68681Context;
  setRegister_f           setReg;

  pmc68681Context = (mc68681_context *) Console_Port_Data[minor].pDeviceContext;

  pMC68681 = Console_Port_Tbl[minor].ulCtrlPort1;
  setReg   = Console_Port_Tbl[minor].setRegister;

  /*
   * Negate RTS
   */
  rtems_interrupt_disable(Irql);
#if 0
  pmc68681Context->ucModemCtrl &= ~SP_MODEM_RTS;
  (*setReg)(pMC68681, MC68681_MODEM_CONTROL, pmc68681Context->ucModemCtrl);
#endif
  rtems_interrupt_enable(Irql);
  return 0;
}

/*
 * These flow control routines utilise a connection from the local DTR
 * line to the remote CTS line
 */

/*
 *  mc68681_assert_DTR
 */

static int mc68681_assert_DTR(int minor)
{
/* XXX */
  unsigned32              pMC68681;
  unsigned32              Irql;
  mc68681_context        *pmc68681Context;
  setRegister_f           setReg;

  pmc68681Context = (mc68681_context *) Console_Port_Data[minor].pDeviceContext;

  pMC68681 = Console_Port_Tbl[minor].ulCtrlPort1;
  setReg   = Console_Port_Tbl[minor].setRegister;

  /*
   * Assert DTR
   */
  rtems_interrupt_disable(Irql);
#if 0
  pmc68681Context->ucModemCtrl |= SP_MODEM_DTR;
  (*setReg)(pMC68681, MC68681_MODEM_CONTROL, pmc68681Context->ucModemCtrl);
#endif
  rtems_interrupt_enable(Irql);
  return 0;
}

/*
 *  mc68681_negate_DTR
 */

static int mc68681_negate_DTR(int minor)
{
/* XXX */
  unsigned32              pMC68681;
  unsigned32              Irql;
  mc68681_context        *pmc68681Context;
  setRegister_f           setReg;

  pmc68681Context = (mc68681_context *) Console_Port_Data[minor].pDeviceContext;

  pMC68681 = Console_Port_Tbl[minor].ulCtrlPort1;
  setReg   = Console_Port_Tbl[minor].setRegister;

  /*
   * Negate DTR
   */
  rtems_interrupt_disable(Irql);
#if 0
  pmc68681Context->ucModemCtrl &= ~SP_MODEM_DTR;
  (*setReg)(pMC68681, MC68681_MODEM_CONTROL,pmc68681Context->ucModemCtrl);
#endif
  rtems_interrupt_enable(Irql);
  return 0;
}

/*
 *  mc68681_isr
 *
 *  This routine is the console interrupt handler.
 *
 *  Input parameters:
 *    vector - vector number
 *
 *  Output parameters: NONE
 *
 *  Return values:     NONE
 */

static void mc68681_process(
        int             minor
)
{
/* XXX */
  unsigned32              pMC68681;
  volatile unsigned8      ucLineStatus; 
  volatile unsigned8      ucInterruptId;
  char                    cChar;
  getRegister_f           getReg;
  setRegister_f           setReg;

#if 1
cChar = ucInterruptId = ucLineStatus = 0;
#endif
  pMC68681 = Console_Port_Tbl[minor].ulCtrlPort1;
  getReg   = Console_Port_Tbl[minor].getRegister;
  setReg   = Console_Port_Tbl[minor].setRegister;

#if 0
  do {
    /*
     * Deal with any received characters
     */
    while(TRUE) {
      ucLineStatus = (*getReg)(pMC68681, MC68681_LINE_STATUS);
      if(~ucLineStatus & SP_LSR_RDY) {
        break;
      }
      cChar = (*getReg)(pMC68681, MC68681_RECEIVE_BUFFER);
      rtems_termios_enqueue_raw_characters( 
        Console_Port_Data[minor].termios_data,
        &cChar, 
        1 
      );
    }

    while(TRUE) {
      if(Ring_buffer_Is_empty(&Console_Port_Data[minor].TxBuffer)) {
        Console_Port_Data[minor].bActive = FALSE;
        if(Console_Port_Tbl[minor].pDeviceFlow != &mc68681_flow_RTSCTS) {
          mc68681_negate_RTS(minor);
        }

        /*
         * There is no data to transmit
         */
        break;
      }

      ucLineStatus = (*getReg)(pMC68681, MC68681_LINE_STATUS);
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
      (*setReg)(pMC68681, MC68681_TRANSMIT_BUFFER, cChar);
    }

    ucInterruptId = (*getReg)(pMC68681, MC68681_INTERRUPT_ID);
  }
  while((ucInterruptId&0xf) != 0x1);
#endif
}

static rtems_isr mc68681_isr(
  rtems_vector_number vector
)
{
  int     minor;

  for(minor=0 ; minor<Console_Port_Count ; minor++) {
    if(vector == Console_Port_Tbl[minor].ulIntVector) {
      mc68681_process(minor);
    }
  }
}

/*
 *  mc68681_flush
 */

static int mc68681_flush(int major, int minor, void *arg)
{
  while(!Ring_buffer_Is_empty(&Console_Port_Data[minor].TxBuffer)) {
    /*
     * Yield while we wait
     */
    if(_System_state_Is_up(_System_state_Get())) {
      rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
    }
  }

  mc68681_close(major, minor, arg);

  return(RTEMS_SUCCESSFUL);
}

/*
 *  mc68681_initialize_interrupts
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

static void mc68681_enable_interrupts(
  int minor
)
{
/* XXX */
  unsigned32            pMC68681;
  unsigned8             ucDataByte;
  setRegister_f         setReg;

#if 1
ucDataByte = 0;
#endif
  pMC68681 = Console_Port_Tbl[minor].ulCtrlPort1;
  setReg   = Console_Port_Tbl[minor].setRegister;

#if 0
  /*
   * Enable interrupts
   */
  ucDataByte = SP_INT_RX_ENABLE | SP_INT_TX_ENABLE;
  (*setReg)(pMC68681, MC68681_INTERRUPT_ENABLE, ucDataByte);
#endif
}

static void mc68681_initialize_interrupts(int minor)
{
  mc68681_init(minor);

  Ring_buffer_Initialize(&Console_Port_Data[minor].TxBuffer);

  Console_Port_Data[minor].bActive = FALSE;

  set_vector(mc68681_isr, Console_Port_Tbl[minor].ulIntVector, 1);

  mc68681_enable_interrupts(minor);
}

/* 
 *  mc68681_write_support_int
 *
 *  Console Termios output entry point.
 */

static int mc68681_write_support_int(
  int   minor, 
  const char *buf, 
  int   len
)
{
  int i;
  unsigned32 Irql;

  for(i=0 ; i<len ;) {
    if(Ring_buffer_Is_full(&Console_Port_Data[minor].TxBuffer)) {
      if(!Console_Port_Data[minor].bActive) {
        /*
         * Wake up the device
         */
        rtems_interrupt_disable(Irql);
        Console_Port_Data[minor].bActive = TRUE;
        if(Console_Port_Tbl[minor].pDeviceFlow != &mc68681_flow_RTSCTS) {
          mc68681_assert_RTS(minor);
        }
        mc68681_process(minor);
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
    if(Console_Port_Tbl[minor].pDeviceFlow != &mc68681_flow_RTSCTS) {
      mc68681_assert_RTS(minor);
    }
    mc68681_process(minor);
    rtems_interrupt_enable(Irql);
  }

  return (len);
}

/* 
 *  mc68681_write_support_polled
 *
 *  Console Termios output entry point.
 *
 */

static int mc68681_write_support_polled(
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
    mc68681_write_polled(minor, *buf++);
    nwrite++;
  }

  /*
   * return the number of bytes written.
   */
  return nwrite;
}

/* 
 *  mc68681_inbyte_nonblocking_polled 
 *
 *  Console Termios polling input entry point.
 */

static int mc68681_inbyte_nonblocking_polled( 
  int minor 
)
{
  unsigned32           pMC68681;
  unsigned char        ucLineStatus;
  char                 cChar;
  getRegister_f        getReg;
  getData_f            getData;

  pMC68681 = Console_Port_Tbl[minor].ulCtrlPort1;
  getReg   = Console_Port_Tbl[minor].getRegister;
  getData  = Console_Port_Tbl[minor].getData;

  ucLineStatus = (*getReg)(pMC68681, MC68681_STATUS_REG);
  if(ucLineStatus & MC68681_RX_READY) {
    cChar = (*getData)(pMC68681);
    return (int)cChar;
  } else {
    return(-1);
  }
}
