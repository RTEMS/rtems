/*
 *  This file contains the termios TTY driver for the Motorola MC68681.
 *
 *  This part is available from a number of secondary sources.
 *  In particular, we know about the following:
 *
 *     + Exar 88c681 and 68c681
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/libio.h>
#include <stdlib.h>

#include <libchip/serial.h>
#include <libchip/mc68681.h>
#include <libchip/sersupp.h>
#include "mc68681_p.h"

/*
 * Flow control is only supported when using interrupts
 */

console_fns mc68681_fns =
{
  libchip_serial_default_probe,   /* deviceProbe */
  mc68681_open,                   /* deviceFirstOpen */
  NULL,                           /* deviceLastClose */
  NULL,                           /* deviceRead */
  mc68681_write_support_int,      /* deviceWrite */
  mc68681_initialize_interrupts,  /* deviceInitialize */
  mc68681_write_polled,           /* deviceWritePolled */
  mc68681_set_attributes,         /* deviceSetAttributes */
  true                            /* deviceOutputUsesInterrupts */
};

console_fns mc68681_fns_polled =
{
  libchip_serial_default_probe,        /* deviceProbe */
  mc68681_open,                        /* deviceFirstOpen */
  mc68681_close,                       /* deviceLastClose */
  mc68681_inbyte_nonblocking_polled,   /* deviceRead */
  mc68681_write_support_polled,        /* deviceWrite */
  mc68681_init,                        /* deviceInitialize */
  mc68681_write_polled,                /* deviceWritePolled */
  mc68681_set_attributes,              /* deviceSetAttributes */
  false,                               /* deviceOutputUsesInterrupts */
};


#if (CPU_SIMPLE_VECTORED_INTERRUPTS == TRUE)
  extern void set_vector( rtems_isr_entry, rtems_vector_number, int );
#endif

/*
 *  Console Device Driver Entry Points
 */

/*
 *  mc68681_baud_rate
 *
 *  This routine returns the proper ACR bit and baud rate field values
 *  based on the requested baud rate.  The baud rate set to be used
 *  must be configured by the user.
 */

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
  uint32_t               pMC68681_port;
  uint32_t               pMC68681;
  unsigned int           mode1;
  unsigned int           mode2;
  unsigned int           baud_mask;
  unsigned int           acr_bit;
  unsigned int           cmd = 0;
  setRegister_f          setReg;
  rtems_interrupt_level  Irql;

  pMC68681      = Console_Port_Tbl[minor]->ulCtrlPort1;
  pMC68681_port = Console_Port_Tbl[minor]->ulCtrlPort2;
  setReg        = Console_Port_Tbl[minor]->setRegister;

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
    /* else
		mode1 |= 0x04; */
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
    mode2 |= 0x0F;                      /* 2 stop bits */
  } else {
    if ((t->c_cflag & CSIZE) == CS5)    /* CS5 and 1 stop bits not supported */
      return -1;
    mode2 |= 0x07;                      /* 1 stop bit */
  }

 /*
  *   Hardware Flow Control
  */

  if(t->c_cflag & CRTSCTS) {
	  mode1 |= 0x80; /* Enable Rx RTS Control */
	  mode2 |= 0x10; /* Enable CTS Enable Tx */
  }


  rtems_interrupt_disable(Irql);
    (*setReg)( pMC68681, MC68681_AUX_CTRL_REG, acr_bit );
    (*setReg)( pMC68681_port, MC68681_CLOCK_SELECT, baud_mask );
    if ( cmd ) {
      (*setReg)( pMC68681_port, MC68681_COMMAND, cmd );         /* RX */
      (*setReg)( pMC68681_port, MC68681_COMMAND, cmd | 0x20 );  /* TX */
    }
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
  unsigned int pMC68681_port;

  pMC68681      = Console_Port_Tbl[minor]->ulCtrlPort1;
  pMC68681_port = Console_Port_Tbl[minor]->ulCtrlPort2;

  pmc68681Context->mate = -1;

  for (port=0 ; port<Console_Port_Count ; port++ ) {
    if ( Console_Port_Tbl[port]->ulCtrlPort1 == pMC68681 &&
         Console_Port_Tbl[port]->ulCtrlPort2 != pMC68681_port ) {
      pmc68681Context->mate = port;
      pmc68681Context->imr  = 0;
      break;
    }
  }

}

/*
 *  mc68681_init
 *
 *  This function initializes the DUART to a quiecsent state.
 */

MC68681_STATIC void mc68681_init(int minor)
{
  uint32_t                pMC68681_port;
  uint32_t                pMC68681;
  mc68681_context        *pmc68681Context;
  setRegister_f           setReg;

  pmc68681Context = (mc68681_context *) malloc(sizeof(mc68681_context));

  Console_Port_Data[minor].pDeviceContext = (void *)pmc68681Context;

  mc68681_initialize_context( minor, pmc68681Context );

  pMC68681      = Console_Port_Tbl[minor]->ulCtrlPort1;
  pMC68681_port = Console_Port_Tbl[minor]->ulCtrlPort2;
  setReg        = Console_Port_Tbl[minor]->setRegister;

  /*
   *  Reset everything and leave this port disabled.
   */

  (*setReg)( pMC68681_port, MC68681_COMMAND, MC68681_MODE_REG_RESET_RX );
  (*setReg)( pMC68681_port, MC68681_COMMAND, MC68681_MODE_REG_RESET_TX );
  (*setReg)( pMC68681_port, MC68681_COMMAND, MC68681_MODE_REG_RESET_ERROR );
  (*setReg)( pMC68681_port, MC68681_COMMAND, MC68681_MODE_REG_RESET_BREAK );
  (*setReg)( pMC68681_port, MC68681_COMMAND, MC68681_MODE_REG_STOP_BREAK );
  (*setReg)( pMC68681_port, MC68681_COMMAND, MC68681_MODE_REG_DISABLE_TX );
  (*setReg)( pMC68681_port, MC68681_COMMAND, MC68681_MODE_REG_DISABLE_RX );


  (*setReg)( pMC68681_port, MC68681_MODE_REG_1A, 0x00 );
  (*setReg)( pMC68681_port, MC68681_MODE_REG_2A, 0x02 );

  /*
   *  Disable interrupts on RX and TX for this port
   */

  mc68681_enable_interrupts( minor, MC68681_IMR_DISABLE_ALL );
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
  uint32_t               pMC68681;
  uint32_t               pMC68681_port;
  unsigned int           baud;
  unsigned int           acr_bit;
  unsigned int           vector;
  unsigned int           command = 0;
  rtems_interrupt_level  Irql;
  setRegister_f          setReg;
  int			 status;


  pMC68681      = Console_Port_Tbl[minor]->ulCtrlPort1;
  pMC68681_port = Console_Port_Tbl[minor]->ulCtrlPort2;
  setReg        = Console_Port_Tbl[minor]->setRegister;
  vector        = Console_Port_Tbl[minor]->ulIntVector;

  /* XXX default baud rate should be from configuration table */

  status = mc68681_baud_rate( minor, B9600, &baud, &acr_bit, &command );
  if (status < 0) rtems_fatal_error_occurred (RTEMS_NOT_DEFINED);

  /*
   *  Set the DUART channel to a default useable state
   */

  rtems_interrupt_disable(Irql);
    (*setReg)( pMC68681, MC68681_AUX_CTRL_REG, acr_bit << 7 );
    (*setReg)( pMC68681_port, MC68681_CLOCK_SELECT, baud );
    if ( command ) {
      (*setReg)( pMC68681_port, MC68681_COMMAND, command );         /* RX */
      (*setReg)( pMC68681_port, MC68681_COMMAND, command | 0x20 );  /* TX */
    }
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

MC68681_STATIC int mc68681_close(
  int      major,
  int      minor,
  void    *arg
)
{
  uint32_t        pMC68681;
  uint32_t        pMC68681_port;
  setRegister_f   setReg;

  pMC68681      = Console_Port_Tbl[minor]->ulCtrlPort1;
  pMC68681_port = Console_Port_Tbl[minor]->ulCtrlPort2;
  setReg        = Console_Port_Tbl[minor]->setRegister;

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
  uint32_t                pMC68681_port;
  unsigned char           ucLineStatus;
  int                     iTimeout;
  getRegister_f           getReg;
  setRegister_f           setReg;

  pMC68681_port = Console_Port_Tbl[minor]->ulCtrlPort2;
  getReg        = Console_Port_Tbl[minor]->getRegister;
  setReg        = Console_Port_Tbl[minor]->setRegister;

  /*
   * wait for transmitter holding register to be empty
   */
  iTimeout = 1000;
  ucLineStatus = (*getReg)(pMC68681_port, MC68681_STATUS);
  while ((ucLineStatus & (MC68681_TX_READY|MC68681_TX_EMPTY)) == 0) {

    if ((ucLineStatus & 0xF0))
      (*setReg)( pMC68681_port, MC68681_COMMAND, MC68681_MODE_REG_RESET_ERROR );

    /*
     * Yield while we wait
     */

#if 0
     if(_System_state_Is_up(_System_state_Get())) {
       rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
     }
#endif
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
    if(Console_Port_Tbl[minor]->ulIntVector == vector &&
       Console_Port_Tbl[minor]->deviceType == SERIAL_MC68681 ) {
      mc68681_process(minor);
    }
  }
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

  Console_Port_Data[minor].bActive = FALSE;

#if (CPU_SIMPLE_VECTORED_INTERRUPTS == TRUE)
  set_vector(mc68681_isr, Console_Port_Tbl[minor]->ulIntVector, 1);
#endif

  mc68681_enable_interrupts(minor,MC68681_IMR_ENABLE_ALL_EXCEPT_TX);
}

/*
 *  mc68681_write_support_int
 *
 *  Console Termios output entry point when using interrupt driven output.
 */

MC68681_STATIC ssize_t mc68681_write_support_int(
  int         minor,
  const char *buf,
  size_t      len
)
{
  uint32_t        Irql;
  uint32_t        pMC68681_port;
  setRegister_f   setReg;

  pMC68681_port = Console_Port_Tbl[minor]->ulCtrlPort2;
  setReg        = Console_Port_Tbl[minor]->setRegister;

  /*
   *  We are using interrupt driven output and termios only sends us
   *  one character at a time.
   */

  if ( !len )
    return 0;

  /*
   *  Put the character out and enable interrupts if necessary.
   */

  rtems_interrupt_disable(Irql);
    if ( Console_Port_Data[minor].bActive == FALSE ) {
      Console_Port_Data[minor].bActive = TRUE;
      mc68681_enable_interrupts(minor, MC68681_IMR_ENABLE_ALL);
    }
    (*setReg)(pMC68681_port, MC68681_TX_BUFFER, *buf);
  rtems_interrupt_enable(Irql);

  return 0;
}

/*
 *  mc68681_write_support_polled
 *
 *  Console Termios output entry point when using polled output.
 *
 */

MC68681_STATIC ssize_t mc68681_write_support_polled(
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
  uint32_t             pMC68681_port;
  unsigned char        ucLineStatus;
  unsigned char        cChar;
  getRegister_f        getReg;

  pMC68681_port = Console_Port_Tbl[minor]->ulCtrlPort2;
  getReg        = Console_Port_Tbl[minor]->getRegister;

  ucLineStatus = (*getReg)(pMC68681_port, MC68681_STATUS);
  if(ucLineStatus & MC68681_RX_READY) {
    cChar = (*getReg)(pMC68681_port, MC68681_RX_BUFFER);
    return (int)cChar;
  } else {
    return -1;
  }
}

/*
 *  mc68681_baud_rate
 */

MC68681_STATIC int mc68681_baud_rate(
  int           minor,
  int           baud,
  unsigned int *baud_mask_p,
  unsigned int *acr_bit_p,
  unsigned int *command
)
{
  unsigned int           baud_mask;
  unsigned int           acr_bit;
  int                    status;
  int                    is_extended;
  int                    baud_requested;
  mc68681_baud_table_t  *baud_tbl;

  baud_mask = 0;
  acr_bit = 0;
  status = 0;

  if (Console_Port_Tbl[minor]->ulDataPort & MC68681_DATA_BAUD_RATE_SET_2)
  {
    acr_bit = 1;
  }

  is_extended = 0;

  switch (Console_Port_Tbl[minor]->ulDataPort & MC68681_XBRG_MASK) {
    case MC68681_XBRG_IGNORED:
      *command = 0x00;
      break;
    case MC68681_XBRG_ENABLED:
      *command = 0x80;
      is_extended = 1;
      break;
    case MC68681_XBRG_DISABLED:
      *command = 0x90;
      break;
  }

  baud_requested = baud & CBAUD;
  if (!baud_requested)
    baud_requested = B9600;              /* default to 9600 baud */

  baud_requested = rtems_termios_baud_to_index( baud_requested );

  baud_tbl = (mc68681_baud_table_t *)
     ((uintptr_t)Console_Port_Tbl[minor]->ulClock);
  if (!baud_tbl)
    rtems_fatal_error_occurred(RTEMS_INVALID_ADDRESS);

  if ( is_extended )
    baud_mask = (unsigned int)baud_tbl[ acr_bit + 2 ][ baud_requested ];
  else
    baud_mask = baud_tbl[ acr_bit ][ baud_requested ];

  if ( baud_mask == MC68681_BAUD_NOT_VALID )
    status = -1;

  /*
   *  upper nibble is receiver and lower nibble is transmitter
   */

  *baud_mask_p = (baud_mask << 4) | baud_mask;
  *acr_bit_p   = acr_bit;
  return status;
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
  uint32_t                pMC68681;
  uint32_t                pMC68681_port;
  volatile uint8_t        ucLineStatus;
  volatile uint8_t        ucISRStatus;
  char                    cChar;
  getRegister_f           getReg;
  setRegister_f           setReg;

  pMC68681      = Console_Port_Tbl[minor]->ulCtrlPort1;
  pMC68681_port = Console_Port_Tbl[minor]->ulCtrlPort2;
  getReg        = Console_Port_Tbl[minor]->getRegister;
  setReg        = Console_Port_Tbl[minor]->setRegister;

  /* Get ISR at the beginning of the IT routine */
  ucISRStatus = (*getReg)(pMC68681, MC68681_INTERRUPT_STATUS_REG);

  /* Get good ISR a or b channel */
  if (pMC68681 != pMC68681_port){
    ucISRStatus >>= 4;
  }

  /* See if is usefull to call rtems_termios_dequeue */
  if(Console_Port_Data[minor].bActive == FALSE) {
		ucISRStatus = ucISRStatus & ~MC68681_IR_TX_READY;
  }

  /*
   * Deal with any received characters
   */
  while(true) {
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

  if (ucISRStatus & MC68681_IR_TX_READY) {
    if (!rtems_termios_dequeue_characters(
          Console_Port_Data[minor].termios_data, 1)) {
	  /* If no more char to send, disable TX interrupt */
      Console_Port_Data[minor].bActive = FALSE;
      mc68681_enable_interrupts(minor, MC68681_IMR_ENABLE_ALL_EXCEPT_TX);
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
  int              is_a;
  unsigned int     mask;
  unsigned int     mate_mask;
  unsigned int     pMC68681;
  unsigned int     pMC68681_port;
  mc68681_context *pmc68681Context;
  mc68681_context *mateContext;

  pMC68681        = Console_Port_Tbl[minor]->ulCtrlPort1;
  pMC68681_port   = Console_Port_Tbl[minor]->ulCtrlPort2;
  pmc68681Context = (mc68681_context *) Console_Port_Data[minor].pDeviceContext;
  mate            = pmc68681Context->mate;

  mask = 0;
  mate_mask = 0;

  is_a = (pMC68681 == pMC68681_port);

  /*
   *  If there is a mate for this port, get its IMR mask.
   */

  if ( mate != -1 ) {
    mateContext = Console_Port_Data[mate].pDeviceContext;

    if (mateContext)
      mate_mask = mateContext->imr;
  }

  /*
   *  Calculate this port's IMR mask and save it in the context area.
   */

  if ( Console_Port_Tbl[minor]->pDeviceFns->deviceOutputUsesInterrupts )
    mask = enable_flag;

  pmc68681Context->imr = mask;

  /*
   *  Now return the full IMR value
   */

  if (is_a)
    return (mate_mask << 4) | mask;

  return (mask << 4) | mate_mask;
}

/*
 *  mc68681_enable_interrupts
 *
 *  This function enables specific interrupt sources on the DUART.
 */

MC68681_STATIC void mc68681_enable_interrupts(
  int minor,
  int imr_mask
)
{
  uint32_t              pMC68681;
  setRegister_f         setReg;

  pMC68681 = Console_Port_Tbl[minor]->ulCtrlPort1;
  setReg   = Console_Port_Tbl[minor]->setRegister;

  /*
   *  Enable interrupts on RX and TX -- not break
   */

  (*setReg)(
     pMC68681,
     MC68681_INTERRUPT_MASK_REG,
     mc68681_build_imr(minor, imr_mask)
  );
}
