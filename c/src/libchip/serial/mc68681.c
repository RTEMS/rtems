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

static boolean mc68681_probe(int minor)
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
    /*
     *  Baud Rate Set 1 
     */

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
    /*
     *  Baud Rate Set 2
     */

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

/*
 *  mc68681_set_attributes
 *
 *  This function sets the DUART channel to reflect the requested termios
 *  port settings.
 */

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

  pMC68681      = Console_Port_Tbl[minor].ulCtrlPort1;
  pMC68681_port = Console_Port_Tbl[minor].ulCtrlPort2;
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

/*
 *  mc68681_initialize_context
 *
 *  This function sets the default values of the per port context structure.
 */

static void mc68681_initialize_context(
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

static unsigned int mc68681_build_imr(
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

static void mc68681_init(int minor)
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

static int mc68681_open(
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
  rtems_interrupt_level  Irql;
  setRegister_f          setReg;

  pMC68681      = Console_Port_Tbl[minor].ulCtrlPort1;
  pMC68681_port = Console_Port_Tbl[minor].ulCtrlPort2;
  setReg        = Console_Port_Tbl[minor].setRegister;
  vector        = Console_Port_Tbl[minor].ulIntVector;

  (void) mc68681_baud_rate( minor, B9600, &baud, &acr );

  /*
   *  Set the DUART channel to a default useable state
   */

  rtems_interrupt_disable(Irql);
    (*setReg)( pMC68681, MC68681_AUX_CTRL_REG, acr );
    (*setReg)( pMC68681_port, MC68681_CLOCK_SELECT, baud );
    (*setReg)( pMC68681_port, MC68681_COMMAND, MC68681_MODE_REG_RESET_MR_PTR );
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

static int mc68681_close(
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
 *  mc68681_process
 *
 *  This routine is the per port console interrupt handler.
 */

static void mc68681_process(
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
 *
 *  This routine waits before all output is completed before closing
 *  the requested port.
 *
 *  NOTE:  This is the "interrupt mode" close entry point.
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
 *  mc68681_enable_interrupts
 *
 *  This function initializes the hardware for this port to use interrupts.
 */

static void mc68681_enable_interrupts(
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
 *  Console Termios output entry point when using interrupt driven output.
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
