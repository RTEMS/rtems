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
#include "sersupp.h"
#include "mc68681_p.h"
#include "mc68681.h"

/*
 * Flow control is only supported when using interrupts
 */

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

/*
 *  mc68681_probe
 *
 *  Default probe routine which simply say the port is present.
 */

MC68681_STATIC boolean mc68681_probe(int minor)
{
  /*
   * If the configuration dependent probe has located the device then
   * assume it is there
   */
  return(TRUE);
}

/*
 *  mc68681_baud_rate
 *
 *  This routine returns the proper ACR bit and baud rate field values
 *  based on the requested baud rate.  The baud rate set to be used
 *  must be configured by the user.
 */

/* major index of 0 : ACR[7] = 0, X = 0 -- 68c681 only has these */
/* major index of 1 : ACR[7] = 1, X = 0 -- 68c681 only has these */
/* major index of 2 : ACR[7] = 0, X = 1 */
/* major index of 3 : ACR[7] = 1, X = 1 */

/* mc68681_baud_table_t mc68681_baud_rate_table[4] = { */
mc68681_baud_t mc68681_baud_rate_table[4][RTEMS_TERMIOS_NUMBER_BAUD_RATES] = {
  { /* ACR[7] = 0, X = 0 */
    MC68681_BAUD_NOT_VALID,    /* B0 */
    0x00,                      /* B50 */
    MC68681_BAUD_NOT_VALID,    /* B75 */
    0x01,                      /* B110 */
    0x02,                      /* B134 */
    MC68681_BAUD_NOT_VALID,    /* B150 */
    0x03,                      /* B200 */
    0x04,                      /* B300 */
    0x05,                      /* B600 */
    0x06,                      /* B1200 */
    MC68681_BAUD_NOT_VALID,    /* B1800 */
    0x08,                      /* B2400 */
    0x09,                      /* B4800 */
    0x0B,                      /* B9600 */
    MC68681_BAUD_NOT_VALID,    /* B19200 */
    0x0C,                      /* B38400 */
    MC68681_BAUD_NOT_VALID,    /* B57600 */
    MC68681_BAUD_NOT_VALID,    /* B115200 */
    MC68681_BAUD_NOT_VALID,    /* B230400 */
    MC68681_BAUD_NOT_VALID     /* B460800 */
  },
  { /* ACR[7] = 1, X = 0 */
    MC68681_BAUD_NOT_VALID,    /* B0 */
    MC68681_BAUD_NOT_VALID,    /* B50 */
    0x00,                      /* B75 */
    0x01,                      /* B110 */
    0x02,                      /* B134 */
    0x03,                      /* B150 */
    MC68681_BAUD_NOT_VALID,    /* B200 */
    0x04,                      /* B300 */
    0x05,                      /* B600 */
    0x06,                      /* B1200 */
    0x0A,                      /* B1800 */
    0x08,                      /* B2400 */
    0x09,                      /* B4800 */
    0x0B,                      /* B9600 */
    0x0C,                      /* B19200 */
    MC68681_BAUD_NOT_VALID,    /* B38400 */
    MC68681_BAUD_NOT_VALID,    /* B57600 */
    MC68681_BAUD_NOT_VALID,    /* B115200 */
    MC68681_BAUD_NOT_VALID,    /* B230400 */
    MC68681_BAUD_NOT_VALID     /* B460800 */
  },
  { /* ACR[7] = 0, X = 1 */
    MC68681_BAUD_NOT_VALID,    /* B0 */
    MC68681_BAUD_NOT_VALID,    /* B50 */
    0x00,                      /* B75 */
    0x01,                      /* B110 */
    0x02,                      /* B134 */
    0x03,                      /* B150 */
    MC68681_BAUD_NOT_VALID,    /* B200 */
    MC68681_BAUD_NOT_VALID,    /* B300 */
    MC68681_BAUD_NOT_VALID,    /* B600 */
    MC68681_BAUD_NOT_VALID,    /* B1200 */
    0x0A,                      /* B1800 */
    MC68681_BAUD_NOT_VALID,    /* B2400 */
    0x08,                      /* B4800 */
    0x0B,                      /* B9600 */
    0x0C,                      /* B19200 */
    MC68681_BAUD_NOT_VALID,    /* B38400 */
    0x07,                      /* B57600 */
    0x08,                      /* B115200 */
    MC68681_BAUD_NOT_VALID,    /* B230400 */
    MC68681_BAUD_NOT_VALID     /* B460800 */
  },
  { /* ACR[7] = 1, X = 1 */
    MC68681_BAUD_NOT_VALID,    /* B0 */
    0x00,                      /* B50 */
    MC68681_BAUD_NOT_VALID,    /* B75 */
    0x01,                      /* B110 */
    0x02,                      /* B134 */
    MC68681_BAUD_NOT_VALID,    /* B150 */
    0x03,                      /* B200 */
    MC68681_BAUD_NOT_VALID,    /* B300 */
    MC68681_BAUD_NOT_VALID,    /* B600 */
    MC68681_BAUD_NOT_VALID,    /* B1200 */
    MC68681_BAUD_NOT_VALID,    /* B1800 */
    MC68681_BAUD_NOT_VALID,    /* B2400 */
    0x09,                      /* B4800 */
    0x0B,                      /* B9600 */
    MC68681_BAUD_NOT_VALID,    /* B19200 */
    0x0C,                      /* B38400 */
    0x07,                      /* B57600 */
    0x08,                      /* B115200 */
    MC68681_BAUD_NOT_VALID,    /* B230400 */
    MC68681_BAUD_NOT_VALID     /* B460800 */
  },
};

MC68681_STATIC int mc68681_baud_rate(
  int           minor,
  int           baud,
  unsigned int *baud_mask_p,
  unsigned int *acr_bit_p,
  unsigned int *command
);

/*
 *  mc68681_set_attributes
 *
 *  This function sets the DUART channel to reflect the requested termios
 *  port settings.
 */

MC68681_STATIC int mc68681_set_attributes( 
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
  unsigned int           cmd;
  setRegister_f          setReg;
  rtems_interrupt_level  Irql;

  pMC68681      = Console_Port_Tbl[minor].ulCtrlPort1;
  pMC68681_port = Console_Port_Tbl[minor].ulCtrlPort2;
  setReg        = Console_Port_Tbl[minor].setRegister;

  /*
   *  Set the baud rate
   */

  if (mc68681_baud_rate( minor, t->c_cflag, &baud_mask, &acr_bit, &cmd ) == -1)
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
    (*setReg)( pMC68681_port, MC68681_COMMAND, cmd );
    (*setReg)( pMC68681_port, MC68681_COMMAND, MC68681_MODE_REG_RESET_MR_PTR );
    (*setReg)( pMC68681_port, MC68681_MODE, mode1 );
    (*setReg)( pMC68681_port, MC68681_MODE, mode2 );
  rtems_interrupt_enable(Irql);
  return 0;
}

/*
 *  mc68681_initialize_context
 *
 *  This function sets the default values of the per port context structure.
 */

MC68681_STATIC void mc68681_initialize_context(
  int               minor,
  mc68681_context  *pmc68681Context
)
{
  int          port;
  unsigned int pMC68681;
  
  pMC68681 = Console_Port_Tbl[minor].ulCtrlPort1;

  pmc68681Context->mate = -1;

  for (port=0 ; port<Console_Port_Count ; port++ ) {
    if ( Console_Port_Tbl[port].ulCtrlPort1 == pMC68681 ) {
      pmc68681Context->mate = port;
      break;
    }
  }

}

/*
 *  mc68681_build_imr
 *
 *  This function returns the value for the interrupt mask register for this
 *  DUART.  Since this is a shared register, we must look at the other port
 *  on this chip to determine whether or not it is using interrupts.
 */

MC68681_STATIC unsigned int mc68681_build_imr(
  int  minor,
  int  enable_flag
)
{
  int              mate;
  unsigned int     mask;
  unsigned int     mate_mask;
  unsigned int     pMC68681;
  unsigned int     pMC68681_port;
  mc68681_context *pmc68681Context;
  
  pMC68681       = Console_Port_Tbl[minor].ulCtrlPort1;
  pMC68681_port  = Console_Port_Tbl[minor].ulCtrlPort2;
  pmc68681Context = (mc68681_context *) Console_Port_Data[minor].pDeviceContext;
  mate            = pmc68681Context->mate;

  mate_mask = 0;

  /*
   *  Decide if the other port on this DUART is using interrupts
   */

  if ( mate != -1 ) {
    if ( Console_Port_Tbl[mate].pDeviceFns->deviceOutputUsesInterrupts )
      mate_mask = 0x03;

    /*
     *  If equal, then minor is A so the mate must be B
     */

    if ( pMC68681 == pMC68681_port )
      mate_mask <<= 4;
  }

  /*
   *  Add in minor's mask
   */

  mask = 0;
  if ( enable_flag ) {
    if ( Console_Port_Tbl[minor].pDeviceFns->deviceOutputUsesInterrupts ) {
      if ( pMC68681 == pMC68681_port )
         mask = 0x03;
      else
         mask = 0x30;
    }
  }

  return mask | mate_mask;
}

/*
 *  mc68681_init
 *
 *  This function initializes the DUART to a quiecsent state.
 */

MC68681_STATIC void mc68681_init(int minor)
{
  unsigned32              pMC68681_port;
  unsigned32              pMC68681;
  mc68681_context        *pmc68681Context;
  setRegister_f           setReg;
  getRegister_f           getReg;

  pmc68681Context = (mc68681_context *) malloc(sizeof(mc68681_context));

  Console_Port_Data[minor].pDeviceContext = (void *)pmc68681Context;

  mc68681_initialize_context( minor, pmc68681Context );

  pMC68681      = Console_Port_Tbl[minor].ulCtrlPort1;
  pMC68681_port = Console_Port_Tbl[minor].ulCtrlPort2;
  setReg        = Console_Port_Tbl[minor].setRegister;
  getReg        = Console_Port_Tbl[minor].getRegister;

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

  /*
   *  Disable interrupts on RX and TX for this port
   */

  (*setReg)( pMC68681, MC68681_INTERRUPT_MASK_REG, mc68681_build_imr(minor, 0));
}

/*
 *  mc68681_open
 *
 *  This function opens a port for communication.
 *
 *  Default state is 9600 baud, 8 bits, No parity, and 1 stop bit.
 */

MC68681_STATIC int mc68681_open(
  int      major,
  int      minor,
  void    *arg
)
{
  unsigned32             pMC68681;
  unsigned32             pMC68681_port;
  unsigned int           baud;
  unsigned int           acr;
  unsigned int           vector;
  unsigned int           command;
  rtems_interrupt_level  Irql;
  setRegister_f          setReg;

  pMC68681      = Console_Port_Tbl[minor].ulCtrlPort1;
  pMC68681_port = Console_Port_Tbl[minor].ulCtrlPort2;
  setReg        = Console_Port_Tbl[minor].setRegister;
  vector        = Console_Port_Tbl[minor].ulIntVector;

  (void) mc68681_baud_rate( minor, B9600, &baud, &acr, &command );

  /*
   *  Set the DUART channel to a default useable state
   */

  rtems_interrupt_disable(Irql);
    (*setReg)( pMC68681, MC68681_AUX_CTRL_REG, acr );
    (*setReg)( pMC68681_port, MC68681_CLOCK_SELECT, baud );
    (*setReg)( pMC68681_port, MC68681_COMMAND, MC68681_MODE_REG_RESET_MR_PTR );
    (*setReg)( pMC68681, MC68681_COMMAND, command );
    (*setReg)( pMC68681_port, MC68681_MODE, 0x13 );
    (*setReg)( pMC68681_port, MC68681_MODE, 0x07 );
  rtems_interrupt_enable(Irql);

  (*setReg)( pMC68681_port, MC68681_COMMAND, MC68681_MODE_REG_ENABLE_TX );
  (*setReg)( pMC68681_port, MC68681_COMMAND, MC68681_MODE_REG_ENABLE_RX );

  (*setReg)( pMC68681, MC68681_INTERRUPT_VECTOR_REG, vector );

  return RTEMS_SUCCESSFUL;
}

/*
 *  mc68681_close
 *
 *  This function shuts down the requested port.
 */

MC68681_STATIC int mc68681_close(
  int      major,
  int      minor,
  void    *arg
)
{
  unsigned32      pMC68681;
  unsigned32      pMC68681_port;
  setRegister_f   setReg;

  pMC68681      = Console_Port_Tbl[minor].ulCtrlPort1;
  pMC68681_port = Console_Port_Tbl[minor].ulCtrlPort2;
  setReg        = Console_Port_Tbl[minor].setRegister;

  /*
   *  Disable interrupts from this channel and then disable it totally.
   */

  (*setReg)( pMC68681_port, MC68681_COMMAND, MC68681_MODE_REG_DISABLE_TX );
  (*setReg)( pMC68681_port, MC68681_COMMAND, MC68681_MODE_REG_DISABLE_RX );

  return(RTEMS_SUCCESSFUL);
}

/* 
 *  mc68681_write_polled
 *
 *  This routine polls out the requested character.
 */

MC68681_STATIC void mc68681_write_polled(
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
 *  mc68681_process
 *
 *  This routine is the per port console interrupt handler.
 */

MC68681_STATIC void mc68681_process(
  int  minor
)
{
  unsigned32              pMC68681;
  unsigned32              pMC68681_port;
  volatile unsigned8      ucLineStatus; 
  char                    cChar;
  getRegister_f           getReg;
  setRegister_f           setReg;

  pMC68681      = Console_Port_Tbl[minor].ulCtrlPort1;
  pMC68681_port = Console_Port_Tbl[minor].ulCtrlPort2;
  getReg        = Console_Port_Tbl[minor].getRegister;
  setReg        = Console_Port_Tbl[minor].setRegister;

  /*
   * Deal with any received characters
   */
  while(TRUE) {
    ucLineStatus = (*getReg)(pMC68681_port, MC68681_STATUS);
    if(!(ucLineStatus & MC68681_RX_READY)) {
      break;
    }
    /*
     *  If there is a RX error, then dump all the data.
     */
    if ( ucLineStatus & MC68681_RX_ERRORS ) {
      do {
        cChar = (*getReg)(pMC68681_port, MC68681_RX_BUFFER);
        ucLineStatus = (*getReg)(pMC68681_port, MC68681_STATUS);
      } while ( ucLineStatus & MC68681_RX_READY );
      continue;
    }
    cChar = (*getReg)(pMC68681_port, MC68681_RX_BUFFER);
    rtems_termios_enqueue_raw_characters( 
      Console_Port_Data[minor].termios_data,
      &cChar, 
      1 
    );
  }

  /*
   *  Deal with the transmitter
   */

  while(TRUE) {
    if(Ring_buffer_Is_empty(&Console_Port_Data[minor].TxBuffer)) {
      Console_Port_Data[minor].bActive = FALSE;

      /*
       * There is no data to transmit
       */
      break;
    }

    ucLineStatus = (*getReg)(pMC68681_port, MC68681_STATUS);
    if(!(ucLineStatus & (MC68681_TX_EMPTY|MC68681_TX_READY))) {
      /*
       *  We'll get another interrupt when the TX can take another character.
       */
      break;
    }

    Ring_buffer_Remove_character( &Console_Port_Data[minor].TxBuffer, cChar);
    /*
     * transmit character
     */
    (*setReg)(pMC68681_port, MC68681_TX_BUFFER, cChar);
  }

}

/*
 *  mc68681_isr
 *
 *  This is the single interrupt entry point which parcels interrupts
 *  out to the various ports.
 */

MC68681_STATIC rtems_isr mc68681_isr(
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
 *
 *  This routine waits before all output is completed before closing
 *  the requested port.
 *
 *  NOTE:  This is the "interrupt mode" close entry point.
 */

MC68681_STATIC int mc68681_flush(int major, int minor, void *arg)
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
 *  mc68681_enable_interrupts
 *
 *  This function initializes the hardware for this port to use interrupts.
 */

MC68681_STATIC void mc68681_enable_interrupts(
  int minor
)
{
  unsigned32            pMC68681;
  setRegister_f         setReg;

  pMC68681 = Console_Port_Tbl[minor].ulCtrlPort1;
  setReg   = Console_Port_Tbl[minor].setRegister;

  /*
   *  Enable interrupts on RX and TX -- not break
   */

  (*setReg)( pMC68681, MC68681_INTERRUPT_MASK_REG, mc68681_build_imr(minor, 1));
}

/*
 *  mc68681_initialize_interrupts
 *
 *  This routine initializes the console's receive and transmit
 *  ring buffers and loads the appropriate vectors to handle the interrupts.
 */

MC68681_STATIC void mc68681_initialize_interrupts(int minor)
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
 *  Console Termios output entry point when using interrupt driven output.
 */

MC68681_STATIC int mc68681_write_support_int(
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
    mc68681_process(minor);
    rtems_interrupt_enable(Irql);
  }

  return (len);
}

/* 
 *  mc68681_write_support_polled
 *
 *  Console Termios output entry point when using polled output.
 *
 */

MC68681_STATIC int mc68681_write_support_polled(
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

MC68681_STATIC int mc68681_inbyte_nonblocking_polled( 
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

MC68681_STATIC int mc68681_baud_rate(
  int           minor,
  int           baud,
  unsigned int *baud_mask_p,
  unsigned int *acr_bit_p,
  unsigned int *command
)
{
  unsigned int     baud_mask;
  unsigned int     acr_bit;
  int              status;
  int              is_a;
  int              is_extended;
  int              baud_requested;
  mc68681_baud_table_t  *baud_tbl;

  baud_mask = 0;
  acr_bit = 0;
  status = 0;

  if (Console_Port_Tbl[minor].ulCtrlPort1 ==
      Console_Port_Tbl[minor].ulCtrlPort2)
    is_a = 1;
  else
    is_a = 0;
    
  if ( !(Console_Port_Tbl[minor].ulDataPort & MC68681_DATA_BAUD_RATE_SET_1) )
    acr_bit = 1;

  is_extended = 0;

  switch (Console_Port_Tbl[minor].ulDataPort & MC68681_XBRG_MASK) {
    case MC68681_XBRG_IGNORED:
      *command = 0x00;
      break;
    case MC68681_XBRG_ENABLED:
      *command = (is_a) ? 0x08 : 0x09;
      is_extended = 1;
      break;
    case MC68681_XBRG_DISABLED:
      *command = (is_a) ? 0x0A : 0x0B;
      break;
  }

  baud_requested = baud & CBAUD;
  if (!baud_requested)
    baud_requested = B9600;
  
  baud_requested = termios_baud_to_index( baud_requested );

  baud_tbl = (mc68681_baud_table_t *) Console_Port_Tbl[minor].ulClock;
  if (!baud_tbl)
    baud_tbl = (mc68681_baud_table_t *)mc68681_baud_rate_table;

  if ( is_extended )
    baud_mask = (unsigned int)baud_tbl[ acr_bit + 2 ][ baud_requested ];
  else
    baud_mask = baud_tbl[ acr_bit ][ baud_requested ];

  if ( baud_mask == MC68681_BAUD_NOT_VALID )
    status = -1;

  *baud_mask_p = baud_mask;     /* default to 9600 baud */
  *acr_bit_p   = acr_bit;
  return status;
}

