/*
 *  This file contains the console driver chip level routines for the
 *  Zilog z85c30 chip.
 *
 *  The Zilog Z8530 is also available as:
 *
 *    + Intel 82530
 *    + AMD ???
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
 *  COPYRIGHT (c) 1989-1997.
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
#include <libchip/sersupp.h>
#include "z85c30_p.h"

/*
 * Flow control is only supported when using interrupts
 */

console_flow z85c30_flow_RTSCTS = {
  z85c30_negate_RTS,    /* deviceStopRemoteTx */
  z85c30_assert_RTS     /* deviceStartRemoteTx */
};

console_flow z85c30_flow_DTRCTS = {
  z85c30_negate_DTR,    /* deviceStopRemoteTx */
  z85c30_assert_DTR     /* deviceStartRemoteTx */
};

/*
 * Exported driver function table
 */

console_fns z85c30_fns = {
  libchip_serial_default_probe,  /* deviceProbe */
  z85c30_open,                   /* deviceFirstOpen */
  NULL,                          /* deviceLastClose */
  NULL,                          /* deviceRead */
  z85c30_write_support_int,      /* deviceWrite */
  z85c30_initialize_interrupts,  /* deviceInitialize */
  z85c30_write_polled,           /* deviceWritePolled */
  NULL,                          /* deviceSetAttributes */
  true                           /* deviceOutputUsesInterrupts */
};

console_fns z85c30_fns_polled = {
  libchip_serial_default_probe,      /* deviceProbe */
  z85c30_open,                       /* deviceFirstOpen */
  z85c30_close,                      /* deviceLastClose */
  z85c30_inbyte_nonblocking_polled,  /* deviceRead */
  z85c30_write_support_polled,       /* deviceWrite */
  z85c30_init,                       /* deviceInitialize */
  z85c30_write_polled,               /* deviceWritePolled */
  NULL,                              /* deviceSetAttributes */
  false                              /* deviceOutputUsesInterrupts */
};

#if (CPU_SIMPLE_VECTORED_INTERRUPTS == TRUE)
  extern void set_vector( rtems_isr_entry, rtems_vector_number, int );
#endif

/*
 *  z85c30_initialize_port
 *
 *  initialize a z85c30 Port
 */

Z85C30_STATIC void z85c30_initialize_port(
  int minor
)
{
  uintptr_t       ulCtrlPort;
  uintptr_t       ulBaudDivisor;
  setRegister_f   setReg;

  ulCtrlPort = Console_Port_Tbl[minor]->ulCtrlPort1;
  setReg   = Console_Port_Tbl[minor]->setRegister;

  /*
   * Using register 4
   * Set up the clock rate is 16 times the data
   * rate, 8 bit sync char, 1 stop bit, no parity
   */

  (*setReg)( ulCtrlPort, SCC_WR0_SEL_WR4, SCC_WR4_1_STOP | SCC_WR4_16_CLOCK );

  /*
   * Set up for 8 bits/character on receive with
   * receiver disable via register 3
   */
  (*setReg)( ulCtrlPort, SCC_WR0_SEL_WR3, SCC_WR3_RX_8_BITS );

  /*
   * Set up for 8 bits/character on transmit
   * with transmitter disable via register 5
   */
  (*setReg)( ulCtrlPort, SCC_WR0_SEL_WR5, SCC_WR5_TX_8_BITS );

  /*
   * Clear misc control bits
   */
  (*setReg)( ulCtrlPort, SCC_WR0_SEL_WR10, 0x00 );

  /*
   * Setup the source of the receive and xmit
   * clock as BRG output and the transmit clock
   * as the output source for TRxC pin via register 11
   */
  (*setReg)(
    ulCtrlPort,
    SCC_WR0_SEL_WR11,
    SCC_WR11_OUT_BR_GEN | SCC_WR11_TRXC_OI |
      SCC_WR11_TX_BR_GEN | SCC_WR11_RX_BR_GEN
  );

  ulBaudDivisor = Z85C30_Baud(
    (uint32_t) Console_Port_Tbl[minor]->ulClock,
    (uint32_t) ((uintptr_t)Console_Port_Tbl[minor]->pDeviceParams)
  );

  /*
   * Setup the lower 8 bits time constants=1E.
   * If the time constans=1E, then the desire
   * baud rate will be equilvalent to 9600, via register 12.
   */
  (*setReg)( ulCtrlPort, SCC_WR0_SEL_WR12, ulBaudDivisor & 0xff );

  /*
   * using register 13
   * Setup the upper 8 bits time constant
   */
  (*setReg)( ulCtrlPort, SCC_WR0_SEL_WR13, (ulBaudDivisor>>8) & 0xff );

  /*
   * Enable the baud rate generator enable with clock from the
   * SCC's PCLK input via register 14.
   */
  (*setReg)(
    ulCtrlPort,
    SCC_WR0_SEL_WR14,
    SCC_WR14_BR_EN | SCC_WR14_BR_SRC | SCC_WR14_NULL
  );

  /*
   * We are only interested in CTS state changes
   */
  (*setReg)( ulCtrlPort, SCC_WR0_SEL_WR15, SCC_WR15_CTS_IE );

  /*
   * Reset errors
   */
  (*setReg)( ulCtrlPort, SCC_WR0_SEL_WR0, SCC_WR0_RST_INT );

  (*setReg)( ulCtrlPort, SCC_WR0_SEL_WR0, SCC_WR0_ERR_RST );

  /*
   * Enable the receiver via register 3
   */
  (*setReg)( ulCtrlPort, SCC_WR0_SEL_WR3, SCC_WR3_RX_8_BITS | SCC_WR3_RX_EN );

  /*
   * Enable the transmitter pins set via register 5.
   */
  (*setReg)( ulCtrlPort, SCC_WR0_SEL_WR5, SCC_WR5_TX_8_BITS | SCC_WR5_TX_EN );

  /*
   * Disable interrupts
   */
  (*setReg)( ulCtrlPort, SCC_WR0_SEL_WR1, 0 );

  /*
   * Reset TX CRC
   */
  (*setReg)( ulCtrlPort, SCC_WR0_SEL_WR0, SCC_WR0_RST_TX_CRC );

  /*
   * Reset interrupts
   */
  (*setReg)( ulCtrlPort, SCC_WR0_SEL_WR0, SCC_WR0_RST_INT );
}

/*
 *  z85c30_open
 */

Z85C30_STATIC int z85c30_open(
  int   major,
  int   minor,
  void *arg
)
{

  z85c30_initialize_port(minor);

  /*
   * Assert DTR
   */

  if (Console_Port_Tbl[minor]->pDeviceFlow !=&z85c30_flow_DTRCTS) {
    z85c30_assert_DTR(minor);
  }

  return(RTEMS_SUCCESSFUL);
}

/*
 *  z85c30_close
 */

Z85C30_STATIC int z85c30_close(
  int   major,
  int   minor,
  void *arg
)
{
  /*
   * Negate DTR
   */

  if (Console_Port_Tbl[minor]->pDeviceFlow !=&z85c30_flow_DTRCTS) {
    z85c30_negate_DTR(minor);
  }

  return(RTEMS_SUCCESSFUL);
}

/*
 *  z85c30_init
 */

Z85C30_STATIC void z85c30_init(int minor)
{
  uintptr_t        ulCtrlPort;
  z85c30_context  *pz85c30Context;
  setRegister_f    setReg;
  getRegister_f    getReg;

  ulCtrlPort = Console_Port_Tbl[minor]->ulCtrlPort1;
  setReg     = Console_Port_Tbl[minor]->setRegister;
  getReg     = Console_Port_Tbl[minor]->getRegister;

  pz85c30Context = (z85c30_context *)malloc(sizeof(z85c30_context));

  Console_Port_Data[minor].pDeviceContext = (void *)pz85c30Context;

  pz85c30Context->ucModemCtrl = SCC_WR5_TX_8_BITS | SCC_WR5_TX_EN;

  ulCtrlPort = Console_Port_Tbl[minor]->ulCtrlPort1;
  if ( ulCtrlPort == Console_Port_Tbl[minor]->ulCtrlPort2 ) {
    /*
     * This is channel A
     */
    /*
     * Ensure port state machine is reset
     */
    (*getReg)(ulCtrlPort, SCC_WR0_SEL_RD0);

    (*setReg)(ulCtrlPort, SCC_WR0_SEL_WR9, SCC_WR9_CH_A_RST);

  } else {
    /*
     * This is channel B
     */
    /*
     * Ensure port state machine is reset
     */
    (*getReg)(ulCtrlPort, SCC_WR0_SEL_RD0);

    (*setReg)(ulCtrlPort, SCC_WR0_SEL_WR9, SCC_WR9_CH_B_RST);
  }
}

/*
 * These routines provide control of the RTS and DTR lines
 */

/*
 *  z85c30_assert_RTS
 */

Z85C30_STATIC int z85c30_assert_RTS(int minor)
{
  rtems_interrupt_level  Irql;
  z85c30_context        *pz85c30Context;
  setRegister_f          setReg;

  setReg = Console_Port_Tbl[minor]->setRegister;

  pz85c30Context = (z85c30_context *) Console_Port_Data[minor].pDeviceContext;

  /*
   * Assert RTS
   */

  rtems_interrupt_disable(Irql);
    pz85c30Context->ucModemCtrl|=SCC_WR5_RTS;
    (*setReg)(
      Console_Port_Tbl[minor]->ulCtrlPort1,
      SCC_WR0_SEL_WR5,
      pz85c30Context->ucModemCtrl
    );
  rtems_interrupt_enable(Irql);
  return 0;
}

/*
 *  z85c30_negate_RTS
 */

Z85C30_STATIC int z85c30_negate_RTS(int minor)
{
  rtems_interrupt_level  Irql;
  z85c30_context        *pz85c30Context;
  setRegister_f          setReg;

  setReg = Console_Port_Tbl[minor]->setRegister;

  pz85c30Context = (z85c30_context *) Console_Port_Data[minor].pDeviceContext;

  /*
   * Negate RTS
   */

  rtems_interrupt_disable(Irql);
    pz85c30Context->ucModemCtrl&=~SCC_WR5_RTS;
    (*setReg)(
      Console_Port_Tbl[minor]->ulCtrlPort1,
      SCC_WR0_SEL_WR5,
      pz85c30Context->ucModemCtrl
    );
  rtems_interrupt_enable(Irql);
  return 0;
}

/*
 * These flow control routines utilise a connection from the local DTR
 * line to the remote CTS line
 */

/*
 *  z85c30_assert_DTR
 */

Z85C30_STATIC int z85c30_assert_DTR(int minor)
{
  rtems_interrupt_level  Irql;
  z85c30_context        *pz85c30Context;
  setRegister_f          setReg;

  setReg = Console_Port_Tbl[minor]->setRegister;

  pz85c30Context = (z85c30_context *) Console_Port_Data[minor].pDeviceContext;

  /*
   * Assert DTR
   */

  rtems_interrupt_disable(Irql);
    pz85c30Context->ucModemCtrl|=SCC_WR5_DTR;
    (*setReg)(
      Console_Port_Tbl[minor]->ulCtrlPort1,
      SCC_WR0_SEL_WR5,
      pz85c30Context->ucModemCtrl
  );
  rtems_interrupt_enable(Irql);
  return 0;
}

/*
 *  z85c30_negate_DTR
 */

Z85C30_STATIC int z85c30_negate_DTR(int minor)
{
  rtems_interrupt_level  Irql;
  z85c30_context        *pz85c30Context;
  setRegister_f          setReg;

  setReg = Console_Port_Tbl[minor]->setRegister;

  pz85c30Context = (z85c30_context *) Console_Port_Data[minor].pDeviceContext;

  /*
   * Negate DTR
   */

  rtems_interrupt_disable(Irql);
    pz85c30Context->ucModemCtrl&=~SCC_WR5_DTR;
    (*setReg)(
      Console_Port_Tbl[minor]->ulCtrlPort1,
      SCC_WR0_SEL_WR5,
      pz85c30Context->ucModemCtrl
  );
  rtems_interrupt_enable(Irql);
  return 0;
}

/*
 *  z85c30_set_attributes
 *
 *  This function sets the SCC channel to reflect the requested termios
 *  port settings.
 */

Z85C30_STATIC int z85c30_set_attributes(
  int                   minor,
  const struct termios *t
)
{
  uintptr_t              ulCtrlPort;
  uint32_t               ulBaudDivisor;
  uint32_t               wr3;
  uint32_t               wr4;
  uint32_t               wr5;
  int                    baud_requested;
  setRegister_f          setReg;
  rtems_interrupt_level  Irql;

  ulCtrlPort = Console_Port_Tbl[minor]->ulCtrlPort1;
  setReg     = Console_Port_Tbl[minor]->setRegister;

  /*
   *  Calculate the baud rate divisor
   */

  baud_requested = t->c_cflag & CBAUD;
  if (!baud_requested)
    baud_requested = B9600;              /* default to 9600 baud */

  ulBaudDivisor = Z85C30_Baud(
    (uint32_t) Console_Port_Tbl[minor]->ulClock,
    (uint32_t) rtems_termios_baud_to_number( baud_requested )
  );

  wr3 = SCC_WR3_RX_EN;
  wr4 = SCC_WR4_16_CLOCK;
  wr5 = SCC_WR5_TX_EN;

  /*
   *  Parity
   */

  if (t->c_cflag & PARENB) {
    wr4 |= SCC_WR4_PAR_EN;
    if (!(t->c_cflag & PARODD))
      wr4 |= SCC_WR4_PAR_EVEN;
  }

  /*
   *  Character Size
   */

  if (t->c_cflag & CSIZE) {
    switch (t->c_cflag & CSIZE) {
      case CS5:   break;
      case CS6:  wr3 |= SCC_WR3_RX_6_BITS;  wr5 |= SCC_WR5_TX_6_BITS;  break;
      case CS7:  wr3 |= SCC_WR3_RX_7_BITS;  wr5 |= SCC_WR5_TX_7_BITS;  break;
      case CS8:  wr3 |= SCC_WR3_RX_8_BITS;  wr5 |= SCC_WR5_TX_8_BITS;  break;
    }
  } else {
    wr3 |= SCC_WR3_RX_8_BITS;       /* default to 9600,8,N,1 */
    wr5 |= SCC_WR5_TX_8_BITS;       /* default to 9600,8,N,1 */
  }

  /*
   *  Stop Bits
   */

  if (t->c_cflag & CSTOPB) {
    wr4 |= SCC_WR4_2_STOP;                      /* 2 stop bits */
  } else {
    wr4 |= SCC_WR4_1_STOP;                      /* 1 stop bits */
  }

  /*
   *  Now actually set the chip
   */

  rtems_interrupt_disable(Irql);
    (*setReg)( ulCtrlPort, SCC_WR0_SEL_WR4, wr4 );
    (*setReg)( ulCtrlPort, SCC_WR0_SEL_WR3, wr3 );
    (*setReg)( ulCtrlPort, SCC_WR0_SEL_WR5, wr5 );

    /*
     * Setup the lower 8 bits time constants=1E.
     * If the time constans=1E, then the desire
     * baud rate will be equilvalent to 9600, via register 12.
     */

    (*setReg)( ulCtrlPort, SCC_WR0_SEL_WR12, ulBaudDivisor & 0xff );

    /*
     * using register 13
     * Setup the upper 8 bits time constant
     */

    (*setReg)( ulCtrlPort, SCC_WR0_SEL_WR13, (ulBaudDivisor>>8) & 0xff );

  rtems_interrupt_enable(Irql);

  return 0;
}

/*
 *  z85c30_process
 *
 *  This is the per port ISR handler.
 */

Z85C30_STATIC void z85c30_process(
  int        minor,
  uint8_t    ucIntPend
)
{
  uint32_t            ulCtrlPort;
  volatile uint8_t    z85c30_status;
  char                cChar;
  setRegister_f       setReg;
  getRegister_f       getReg;

  ulCtrlPort = Console_Port_Tbl[minor]->ulCtrlPort1;
  setReg     = Console_Port_Tbl[minor]->setRegister;
  getReg     = Console_Port_Tbl[minor]->getRegister;

  /*
   * Deal with any received characters
   */

  while (ucIntPend&SCC_RR3_B_RX_IP)
  {
    z85c30_status = (*getReg)(ulCtrlPort, SCC_WR0_SEL_RD0);
    if (!Z85C30_Status_Is_RX_character_available(z85c30_status)) {
      break;
    }

    /*
     * Return the character read.
     */

    cChar = (*getReg)(ulCtrlPort, SCC_WR0_SEL_RD8);

    rtems_termios_enqueue_raw_characters(
      Console_Port_Data[minor].termios_data,
      &cChar,
      1
    );
  }

  /*
   *  There could be a race condition here if there is not yet a TX
   *  interrupt pending but the buffer is empty.  This condition has
   *  been seen before on other z8530 drivers but has not been seen
   *  with this one.  The typical solution is to use "vector includes
   *  status" or to only look at the interrupts actually pending
   *  in RR3.
   */

  while (true) {
    z85c30_status = (*getReg)(ulCtrlPort, SCC_WR0_SEL_RD0);
    if (!Z85C30_Status_Is_TX_buffer_empty(z85c30_status)) {
      /*
       * We'll get another interrupt when
       * the transmitter holding reg. becomes
       * free again and we are clear to send
       */
      break;
    }

#if 0
    if (!Z85C30_Status_Is_CTS_asserted(z85c30_status)) {
      /*
       * We can't transmit yet
       */
      (*setReg)(ulCtrlPort, SCC_WR0_SEL_WR0, SCC_WR0_RST_TX_INT);
      /*
       * The next state change of CTS will wake us up
       */
      break;
    }
#endif

    rtems_termios_dequeue_characters(Console_Port_Data[minor].termios_data, 1);
    if (rtems_termios_dequeue_characters(
         Console_Port_Data[minor].termios_data, 1)) {
      if (Console_Port_Tbl[minor]->pDeviceFlow != &z85c30_flow_RTSCTS) {
        z85c30_negate_RTS(minor);
      }
      Console_Port_Data[minor].bActive = FALSE;
      z85c30_enable_interrupts(minor, SCC_ENABLE_ALL_INTR_EXCEPT_TX);
      (*setReg)(ulCtrlPort, SCC_WR0_SEL_WR0, SCC_WR0_RST_TX_INT);
      break;
    }

  }

  if (ucIntPend & SCC_RR3_B_EXT_IP) {
    /*
     * Clear the external status interrupt
     */
    (*setReg)(ulCtrlPort, SCC_WR0_SEL_WR0, SCC_WR0_RST_INT);
    z85c30_status = (*getReg)(ulCtrlPort, SCC_WR0_SEL_RD0);
  }

  /*
   * Reset interrupts
   */
  (*setReg)(ulCtrlPort, SCC_WR0_SEL_WR0, SCC_WR0_RST_HI_IUS);
}

/*
 *  z85c30_isr
 *
 *  This is the ISR handler for each Z8530.
 */

Z85C30_STATIC rtems_isr z85c30_isr(
  rtems_vector_number vector
)
{
  int                 minor;
  uint32_t            ulCtrlPort;
  volatile uint8_t    ucIntPend;
  volatile uint8_t    ucIntPendPort;
  getRegister_f       getReg;

  for (minor=0;minor<Console_Port_Count;minor++) {
    if(Console_Port_Tbl[minor]->ulIntVector == vector &&
       Console_Port_Tbl[minor]->deviceType == SERIAL_Z85C30 ) {
      ulCtrlPort = Console_Port_Tbl[minor]->ulCtrlPort2;
      getReg     = Console_Port_Tbl[minor]->getRegister;
      do {
        ucIntPend = (*getReg)(ulCtrlPort, SCC_WR0_SEL_RD3);

          /*
           * If this is channel A select channel A status
           */

          if (ulCtrlPort == Console_Port_Tbl[minor]->ulCtrlPort1) {
            ucIntPendPort = ucIntPend >> 3;
            ucIntPendPort &= 7;
          } else {
            ucIntPendPort = ucIntPend &= 7;
          }

          if (ucIntPendPort) {
            z85c30_process(minor, ucIntPendPort);
          }
      } while (ucIntPendPort);
    }
  }
}

/*
 *  z85c30_enable_interrupts
 *
 *  This routine enables the specified interrupts for this minor.
 */

Z85C30_STATIC void z85c30_enable_interrupts(
  int minor,
  int interrupt_mask
)
{
  uint32_t       ulCtrlPort;
  setRegister_f  setReg;

  ulCtrlPort = Console_Port_Tbl[minor]->ulCtrlPort1;
  setReg     = Console_Port_Tbl[minor]->setRegister;

  (*setReg)(ulCtrlPort, SCC_WR0_SEL_WR1, interrupt_mask);
}

/*
 *  z85c30_initialize_interrupts
 *
 *  This routine initializes the port to use interrupts.
 */

Z85C30_STATIC void z85c30_initialize_interrupts(
  int minor
)
{
  uint32_t       ulCtrlPort1;
  uint32_t       ulCtrlPort2;
  setRegister_f  setReg;

  ulCtrlPort1 = Console_Port_Tbl[minor]->ulCtrlPort1;
  ulCtrlPort2 = Console_Port_Tbl[minor]->ulCtrlPort2;
  setReg      = Console_Port_Tbl[minor]->setRegister;


  z85c30_init(minor);

  Console_Port_Data[minor].bActive=FALSE;

  z85c30_initialize_port( minor );

  if (Console_Port_Tbl[minor]->pDeviceFlow != &z85c30_flow_RTSCTS) {
    z85c30_negate_RTS(minor);
  }

#if (CPU_SIMPLE_VECTORED_INTERRUPTS == TRUE)
  set_vector(z85c30_isr, Console_Port_Tbl[minor]->ulIntVector, 1);
#endif

  z85c30_enable_interrupts(minor, SCC_ENABLE_ALL_INTR_EXCEPT_TX);

  (*setReg)(ulCtrlPort1, SCC_WR0_SEL_WR2, 0);              /* XXX vector */
  (*setReg)(ulCtrlPort1, SCC_WR0_SEL_WR9, SCC_WR9_MIE);

  /*
   * Reset interrupts
   */

  (*setReg)(ulCtrlPort1, SCC_WR0_SEL_WR0, SCC_WR0_RST_INT);
}

/*
 *  z85c30_write_support_int
 *
 *  Console Termios output entry point.
 *
 */

Z85C30_STATIC ssize_t z85c30_write_support_int(
  int   minor,
  const char *buf,
  size_t len)
{
  uint32_t       Irql;
  uint32_t       ulCtrlPort;
  setRegister_f  setReg;

  ulCtrlPort = Console_Port_Tbl[minor]->ulCtrlPort1;
  setReg     = Console_Port_Tbl[minor]->setRegister;

  /*
   *  We are using interrupt driven output and termios only sends us
   *  one character at a time.
   */

  if ( !len )
    return 0;

  /*
   *  Put the character out and enable interrupts if necessary.
   */

  if (Console_Port_Tbl[minor]->pDeviceFlow != &z85c30_flow_RTSCTS) {
    z85c30_assert_RTS(minor);
  }
  rtems_interrupt_disable(Irql);
    if ( Console_Port_Data[minor].bActive == FALSE) {
      Console_Port_Data[minor].bActive = TRUE;
      z85c30_enable_interrupts(minor, SCC_ENABLE_ALL_INTR);
    }
    (*setReg)(ulCtrlPort, SCC_WR0_SEL_WR8, *buf);
  rtems_interrupt_enable(Irql);

  return 0;
}

/*
 *  z85c30_inbyte_nonblocking_polled
 *
 *  This routine polls for a character.
 */

Z85C30_STATIC int z85c30_inbyte_nonblocking_polled(
  int  minor
)
{
  volatile uint8_t    z85c30_status;
  uint32_t            ulCtrlPort;
  getRegister_f       getReg;

  ulCtrlPort = Console_Port_Tbl[minor]->ulCtrlPort1;
  getReg     = Console_Port_Tbl[minor]->getRegister;

  /*
   * return -1 if a character is not available.
   */
  z85c30_status = (*getReg)(ulCtrlPort, SCC_WR0_SEL_RD0);
  if (!Z85C30_Status_Is_RX_character_available(z85c30_status)) {
    return -1;
  }

  /*
   * Return the character read.
   */

  return (*getReg)(ulCtrlPort, SCC_WR0_SEL_RD8);
}

/*
 *  z85c30_write_support_polled
 *
 *  Console Termios output entry point.
 *
 */

Z85C30_STATIC ssize_t z85c30_write_support_polled(
  int   minor,
  const char *buf,
  size_t len)
{
  int nwrite=0;

  /*
   * poll each byte in the string out of the port.
   */
  while (nwrite < len) {
    z85c30_write_polled(minor, *buf++);
    nwrite++;
  }

  /*
   * return the number of bytes written.
   */
  return nwrite;
}

/*
 *  z85c30_write_polled
 *
 *  This routine transmits a character using polling.
 */

Z85C30_STATIC void z85c30_write_polled(
  int   minor,
  char  cChar
)
{
  volatile uint8_t   z85c30_status;
  uint32_t           ulCtrlPort;
  getRegister_f      getReg;
  setRegister_f      setReg;

  ulCtrlPort = Console_Port_Tbl[minor]->ulCtrlPort1;
  getReg     = Console_Port_Tbl[minor]->getRegister;
  setReg     = Console_Port_Tbl[minor]->setRegister;

  /*
   * Wait for the Transmit buffer to indicate that it is empty.
   */

  z85c30_status = (*getReg)( ulCtrlPort, SCC_WR0_SEL_RD0 );

  while (!Z85C30_Status_Is_TX_buffer_empty(z85c30_status)) {
    /*
     * Yield while we wait
     */
#if 0
    if (_System_state_Is_up(_System_state_Get())) {
      rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
    }
#endif
    z85c30_status = (*getReg)(ulCtrlPort, SCC_WR0_SEL_RD0);
  }

  /*
   * Write the character.
   */

  (*setReg)( ulCtrlPort, SCC_WR0_SEL_WR8, cChar );
}
