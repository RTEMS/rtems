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
  mc68681_set_attributes,         /* deviceSetAttributes */
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
  mc68681_set_attributes,              /* deviceSetAttributes */
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

static int mc68681_baud_rate(
  int           minor,
  int           baud,
  unsigned int *baud_mask_p,
  unsigned int *acr_bit_p
)
{
  unsigned int baud_mask;
  unsigned int acr_bit;
  int          status;

  baud_mask = 0;
  acr_bit = 0;
  status = 0;

  if ( !(Console_Port_Tbl[minor].ulDataPort & MC68681_DATA_BAUD_RATE_SET_1) )
    acr_bit = 1;

  if (!(baud & CBAUD)) {
    *baud_mask_p = 0x0B;     /* default to 9600 baud */
    *acr_bit_p   = acr_bit;
    return status;
  }

  if ( !acr_bit ) {
    switch (baud & CBAUD) {
      case B50:    baud_mask = 0x00; break;
      case B110:   baud_mask = 0x01; break;
      case B134:   baud_mask = 0x02; break;
      case B200:   baud_mask = 0x03; break;
      case B300:   baud_mask = 0x04; break;
      case B600:   baud_mask = 0x05; break;
      case B1200:  baud_mask = 0x06; break;
      case B2400:  baud_mask = 0x08; break;
      case B4800:  baud_mask = 0x09; break;
      case B9600:  baud_mask = 0x0B; break;
      case B38400: baud_mask = 0x0C; break;

      case B0:
      case B75:
      case B150:
      case B1800:
      case B19200:
      case B57600:
      case B115200:
      case B230400:
      case B460800:
        status = -1;
        break;
    }
  } else {
    switch (baud & CBAUD) {
      case B75:    baud_mask = 0x00; break;
      case B110:   baud_mask = 0x01; break;
      case B134:   baud_mask = 0x02; break;
      case B150:   baud_mask = 0x03; break;
      case B300:   baud_mask = 0x04; break;
      case B600:   baud_mask = 0x05; break;
      case B1200:  baud_mask = 0x06; break;
      case B1800:  baud_mask = 0x0A; break;
      case B2400:  baud_mask = 0x08; break;
      case B4800:  baud_mask = 0x09; break;
      case B9600:  baud_mask = 0x0B; break;
      case B19200: baud_mask = 0x0C; break;

      case B0:
      case B50:
      case B200:
      case B38400:
      case B57600:
      case B115200:
      case B230400:
      case B460800:
        status = -1;
        break;
    }
  }

  *baud_mask_p = baud_mask;
  *acr_bit_p   = acr_bit;
  return status;
}

#define MC68681_PORT_MASK( _port, _bits ) \
  ((_port) ? ((_bits) << 4) : (_bits))

static int mc68681_set_attributes( 
  int minor,
  const struct termios *t
)
{
  unsigned32             pMC68681_port;
  unsigned32             pMC68681;
  unsigned int           mode1;
  unsigned int           mode2;
  unsigned int           baud_mask;
  unsigned int           acr_bit;
  setRegister_f          setReg;
  rtems_interrupt_level  Irql;

  pMC68681_port = Console_Port_Tbl[minor].ulCtrlPort1;
  pMC68681      = Console_Port_Tbl[minor].ulCtrlPort2;
  setReg        = Console_Port_Tbl[minor].setRegister;

  /*
   *  Set the baud rate
   */

  if ( mc68681_baud_rate( minor, t->c_cflag, &baud_mask, &acr_bit ) == -1 )
    return -1;

  baud_mask |=  baud_mask << 4;
  acr_bit   <<= 7;

  /*
   *  Parity
   */

  mode1 = 0;
  mode2 = 0;

  if (t->c_cflag & PARENB) {
    if (t->c_cflag & PARODD)
      mode1 |= 0x04;
    else
      mode1 |= 0x04;
  } else {
   mode1 |= 0x10;
  }

  /*
   *  Character Size
   */

  if (t->c_cflag & CSIZE) {
    switch (t->c_cflag & CSIZE) {
      case CS5:  break;
      case CS6:  mode1 |= 0x01;  break;
      case CS7:  mode1 |= 0x02;  break;
      case CS8:  mode1 |= 0x03;  break;
    }
  } else {
    mode1 |= 0x03;       /* default to 9600,8,N,1 */
  }

  /*
   *  Stop Bits
   */
  
  if (t->c_cflag & CSTOPB) {
    mode2 |= 0x07;                      /* 2 stop bits */
  } else {
    if ((t->c_cflag & CSIZE) == CS5)    /* CS5 and 2 stop bits not supported */
      return -1;
    mode2 |= 0x0F;                      /* 1 stop bit */
  }

  rtems_interrupt_disable(Irql);
    (*setReg)( pMC68681, MC68681_AUX_CTRL_REG, acr_bit );
    (*setReg)( pMC68681_port, MC68681_CLOCK_SELECT, baud_mask );
    (*setReg)( pMC68681_port, MC68681_COMMAND, MC68681_MODE_REG_RESET_MR_PTR );
    (*setReg)( pMC68681_port, MC68681_MODE, mode1 );
    (*setReg)( pMC68681_port, MC68681_MODE, mode2 );
  rtems_interrupt_enable(Irql);
  return 0;
}

static void mc68681_init(int minor)
{
/* XXX */
  unsigned32              pMC68681_port;
  unsigned32              pMC68681;
  mc68681_context        *pmc68681Context;
  setRegister_f           setReg;
  getRegister_f           getReg;
  unsigned int            port;

  pmc68681Context = (mc68681_context *) malloc(sizeof(mc68681_context));

  Console_Port_Data[minor].pDeviceContext = (void *)pmc68681Context;
#if 0
  pmc68681Context->ucModemCtrl = SP_MODEM_IRQ;
#endif

  pMC68681_port = Console_Port_Tbl[minor].ulCtrlPort1;
  pMC68681      = Console_Port_Tbl[minor].ulCtrlPort2;
  setReg        = Console_Port_Tbl[minor].setRegister;
  getReg        = Console_Port_Tbl[minor].getRegister;
  port          = Console_Port_Tbl[minor].ulDataPort;

  /*
   *  Reset everything and leave this port disabled.
   */

  (*setReg)( pMC68681, MC68681_COMMAND, MC68681_MODE_REG_RESET_RX );
  (*setReg)( pMC68681, MC68681_COMMAND, MC68681_MODE_REG_RESET_TX );
  (*setReg)( pMC68681, MC68681_COMMAND, MC68681_MODE_REG_RESET_ERROR );
  (*setReg)( pMC68681, MC68681_COMMAND, MC68681_MODE_REG_RESET_BREAK );
  (*setReg)( pMC68681, MC68681_COMMAND, MC68681_MODE_REG_STOP_BREAK );
  (*setReg)( pMC68681, MC68681_COMMAND, MC68681_MODE_REG_DISABLE_TX );
  (*setReg)( pMC68681, MC68681_COMMAND, MC68681_MODE_REG_DISABLE_RX );


  (*setReg)( pMC68681, MC68681_MODE_REG_1A, 0x00 );
  (*setReg)( pMC68681, MC68681_MODE_REG_2A, 0x02 );
}

/*
 *  Initialize to 9600, 8, N, 1
 */

static int mc68681_open(
  int      major,
  int      minor,
  void    *arg
)
{
/* XXX */
  unsigned32             pMC68681;
  unsigned32             pMC68681_port;
  unsigned int           baud;
  unsigned int           acr;
  unsigned int           port;
  unsigned int           vector;
  rtems_interrupt_level  Irql;
  setRegister_f          setReg;
  getRegister_f          getReg;

  pMC68681      = Console_Port_Tbl[minor].ulCtrlPort1;
  pMC68681_port = Console_Port_Tbl[minor].ulCtrlPort2;
  getReg        = Console_Port_Tbl[minor].getRegister;
  setReg        = Console_Port_Tbl[minor].setRegister;
  port          = Console_Port_Tbl[minor].ulDataPort;
  vector        = Console_Port_Tbl[minor].ulIntVector;

  (void) mc68681_baud_rate( minor, B9600, &baud, &acr );

  rtems_interrupt_disable(Irql);
    (*setReg)( pMC68681, MC68681_AUX_CTRL_REG, acr );
    (*setReg)( pMC68681_port, MC68681_CLOCK_SELECT, baud );
    (*setReg)( pMC68681_port, MC68681_COMMAND, MC68681_MODE_REG_RESET_MR_PTR );
    (*setReg)( pMC68681_port, MC68681_MODE, 0x13 );
    (*setReg)( pMC68681_port, MC68681_MODE, 0x07 );
  rtems_interrupt_enable(Irql);

  (*setReg)(
     pMC68681,
     MC68681_INTERRUPT_MASK_REG,
     MC68681_PORT_MASK( port, 0x03 )  /* intr on RX and TX -- not break */
  );

  (*setReg)( pMC68681_port, MC68681_COMMAND, MC68681_MODE_REG_ENABLE_TX );
  (*setReg)( pMC68681_port, MC68681_COMMAND, MC68681_MODE_REG_ENABLE_RX );

  (*setReg)( pMC68681, MC68681_INTERRUPT_VECTOR_REG, vector );

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
  void    *arg
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
  unsigned32              pMC68681_port;
  unsigned char           ucLineStatus;
  int                     iTimeout;
  getRegister_f           getReg;
  setRegister_f           setReg;

  pMC68681_port = Console_Port_Tbl[minor].ulCtrlPort2;
  getReg        = Console_Port_Tbl[minor].getRegister;
  setReg        = Console_Port_Tbl[minor].setRegister;

  /*
   * wait for transmitter holding register to be empty
   */
  iTimeout = 1000;
  ucLineStatus = (*getReg)(pMC68681_port, MC68681_STATUS);
  while ((ucLineStatus & MC68681_TX_READY) == 0) {

    /*
     * Yield while we wait
     */

     if(_System_state_Is_up(_System_state_Get())) {
       rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
     }
     ucLineStatus = (*getReg)(pMC68681_port, MC68681_STATUS);
     if(!--iTimeout) {
       break;
     }
  }

  /*
   * transmit character
   */

  (*setReg)(pMC68681_port, MC68681_TX_BUFFER, cChar);
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
  unsigned32           pMC68681_port;
  unsigned char        ucLineStatus;
  char                 cChar;
  getRegister_f        getReg;

  pMC68681_port = Console_Port_Tbl[minor].ulCtrlPort2;
  getReg        = Console_Port_Tbl[minor].getRegister;

  ucLineStatus = (*getReg)(pMC68681_port, MC68681_STATUS);
  if(ucLineStatus & MC68681_RX_READY) {
    cChar = (*getReg)(pMC68681_port, MC68681_RX_BUFFER);
    return (int)cChar;
  } else {
    return -1;
  }
}
