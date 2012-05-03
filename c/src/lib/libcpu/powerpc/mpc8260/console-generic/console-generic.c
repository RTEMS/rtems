/*
 *  General Serial I/O functions.
 *
 *  This file contains the functions for performing serial I/O.
 *  The actual system calls (console_*) should be in the BSP part
 *  of the source tree. That way different BSPs can use whichever
 *  SMCs and SCCs they want. Originally, all the stuff was in
 *  this file, and it caused problems with one BSP using SCC2
 *  as /dev/console, others using SMC1 for /dev/console, etc.
 *
 *  On-chip resources used:
 *   resource   minor                note
 *    SMC1       0
 *    SMC2       1
 *    SCC1       2
 *    SCC2       3
 *    SCC3       4
 *    SCC4       5
 *    BRG1
 *    BRG2
 *    BRG3
 *    BRG4
 *  Author: Jay Monkman (jmonkman@frasca.com)
 *  Copyright (C) 1998 by Frasca International, Inc.
 *
 *  Derived from c/src/lib/libbsp/m68k/gen360/console/console.c written by:
 *    W. Eric Norum
 *    Saskatchewan Accelerator Laboratory
 *    University of Saskatchewan
 *    Saskatoon, Saskatchewan, CANADA
 *    eric@skatter.usask.ca
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications by Darlene Stewart <Darlene.Stewart@iit.nrc.ca>
 *  and Charles-Antoine Gauthier <charles.gauthier@iit.nrc.ca>
 *  Copyright (c) 1999, National Research Council of Canada
 *
 *  Modifications by Andy Dachs <a.dachs@sstl.co.uk> to add MPC8260
 *  support.
 *  Copyright (c) 2001, Surrey Satellite Technology Ltd
 *    SCC1 and SSC2 are used on MPC8260ADS board
 *    SMCs are unused
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/libio.h>
#include <mpc8260.h>
#include <mpc8260/console.h>
#include <mpc8260/cpm.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <bsp/irq.h>
#include <rtems/bspIo.h>   /* for printk */



/* BSP supplied routine */
extern int mbx8xx_console_get_configuration(void);


/*
 * Interrupt-driven input buffer
 */
#define RXBUFSIZE       16

/*
 *  I/O buffers and pointers to buffer descriptors.
 *  Currently, single buffered input is done. This will work only
 *  if the Rx interrupts are serviced quickly.
 *
 *  TODO: Add a least double buffering for safety.
 */
static volatile char rxBuf[NUM_PORTS][RXBUFSIZE];
static volatile char txBuf[NUM_PORTS];

/* SCC/SMC buffer descriptors */
static volatile m8260BufferDescriptor_t *RxBd[NUM_PORTS], *TxBd[NUM_PORTS];



/* Used to track termios private data for callbacks */
struct rtems_termios_tty *ttyp[NUM_PORTS];

#if 0
/* Used to record previous ISR */
static rtems_isr_entry old_handler[NUM_PORTS];
#endif

/*
 * Device-specific routines
 */
static int m8xx_smc_set_attributes(int, const struct termios*);
static int m8xx_scc_set_attributes(int, const struct termios*);
static rtems_isr m8xx_smc1_interrupt_handler(rtems_irq_hdl_param unused);
static rtems_isr m8xx_smc2_interrupt_handler(rtems_irq_hdl_param unused);
static rtems_isr m8xx_scc1_interrupt_handler(rtems_irq_hdl_param unused);
static rtems_isr m8xx_scc2_interrupt_handler(rtems_irq_hdl_param unused);
static rtems_isr m8xx_scc3_interrupt_handler(rtems_irq_hdl_param unused);
static rtems_isr m8xx_scc4_interrupt_handler(rtems_irq_hdl_param unused);



/*
 * Hardware-dependent portion of tcsetattr().
 */
static int
m8xx_smc_set_attributes (int minor, const struct termios *t)
{
  int baud, brg=0, csize=0, ssize, psize;
  uint16_t   clen=0, cstopb, parenb, parodd, cread;

  /* Baud rate */
  switch (t->c_cflag & CBAUD) {
  default:      baud = -1;      break;
  case B50:     baud = 50;      break;
  case B75:     baud = 75;      break;
  case B110:    baud = 110;     break;
  case B134:    baud = 134;     break;
  case B150:    baud = 150;     break;
  case B200:    baud = 200;     break;
  case B300:    baud = 300;     break;
  case B600:    baud = 600;     break;
  case B1200:   baud = 1200;    break;
  case B1800:   baud = 1800;    break;
  case B2400:   baud = 2400;    break;
  case B4800:   baud = 4800;    break;
  case B9600:   baud = 9600;    break;
  case B19200:  baud = 19200;   break;
  case B38400:  baud = 38400;   break;
  case B57600:  baud = 57600;   break;
  case B115200: baud = 115200;  break;
  case B230400: baud = 230400;  break;
  case B460800: baud = 460800;  break;
  }
  if (baud > 0) {
   switch( minor ) {
      case SMC1_MINOR:
        /* SMC1 can only choose between BRG1 and 7 */
        brg = m8xx_get_brg( M8260_SMC1_BRGS, baud*16 ) + 1;
 	m8260.cmxsmr &= ~0x30;
	m8260.cmxsmr |= (brg==1? 0x00: 0x10 );
        break;
      case SMC2_MINOR:
        /* SMC2 can only choose between BRG2 and 8 */
	brg = m8xx_get_brg(  M8260_SMC2_BRGS, baud*16 ) + 1;
 	m8260.cmxsmr &= ~0x30;
	m8260.cmxsmr |= (brg==2? 0x00: 0x01 );
        break;
    }
  }

  /* Number of data bits */
  switch ( t->c_cflag & CSIZE ) {
    case CS5:     csize = 5;       break;
    case CS6:     csize = 6;       break;
    case CS7:     csize = 7;       break;
    case CS8:     csize = 8;       break;
  }

  /* Stop bits */
  if ( t->c_cflag & CSTOPB ) {
    cstopb = 0x0400;              /* Two stop bits */
    ssize  = 2;
  } else {
    cstopb = 0x0000;              /* One stop bit */
    ssize  = 1;
  }

  /* Parity */
  if ( t->c_cflag & PARENB ) {
    parenb = 0x0200;              /* Parity enabled on Tx and Rx */
    psize  = 1;
  } else {
    parenb = 0x0000;              /* No parity on Tx and Rx */
    psize  = 0;
  }

  if ( t->c_cflag & PARODD )
    parodd = 0x0000;              /* Odd parity */
  else
    parodd = 0x0100;

  /*
   * Character Length = start + data + parity + stop - 1
   */
  switch ( 1 + csize + psize + ssize - 1 ) {
    case 6:     clen = 0x3000;       break;
    case 7:     clen = 0x3800;       break;
    case 8:     clen = 0x4000;       break;
    case 9:     clen = 0x4800;       break;
    case 10:    clen = 0x5000;       break;
    case 11:    clen = 0x5800;       break;
  }

  if ( t->c_cflag & CREAD )
    cread = 0x0023;		/* UART normal operation, enable Rx and Tx */
  else
    cread = 0x0021;		/* UART normal operation, enable Tx */

  /* Write the SIMODE/SMCMR registers */
  switch (minor) {
    case SMC1_MINOR:
/*
      m8xx.simode = ( (m8xx.simode & 0xffff8fff) | (brg << 12) );
*/
      m8260.smc1.smcmr = clen | cstopb | parenb | parodd | cread;
      break;
    case SMC2_MINOR:
      /* CHECK THIS */
/*
      m8xx.simode = ( (m8xx.simode & 0x8fffffff) | (brg << 28) );
*/
      m8260.smc2.smcmr = clen | cstopb | parenb | parodd | cread;
      break;
  }
  return 0;
}


static int
m8xx_scc_set_attributes (int minor, const struct termios *t)
{
  int baud, brg=0;
  uint16_t   csize=0, cstopb, parenb, parodd;

  /* Baud rate */
  switch (t->c_cflag & CBAUD) {
  default:      baud = -1;      break;
  case B50:     baud = 50;      break;
  case B75:     baud = 75;      break;
  case B110:    baud = 110;     break;
  case B134:    baud = 134;     break;
  case B150:    baud = 150;     break;
  case B200:    baud = 200;     break;
  case B300:    baud = 300;     break;
  case B600:    baud = 600;     break;
  case B1200:   baud = 1200;    break;
  case B1800:   baud = 1800;    break;
  case B2400:   baud = 2400;    break;
  case B4800:   baud = 4800;    break;
  case B9600:   baud = 9600;    break;
  case B19200:  baud = 19200;   break;
  case B38400:  baud = 38400;   break;
  case B57600:  baud = 57600;   break;
  case B115200: baud = 115200;  break;
  case B230400: baud = 230400;  break;
  case B460800: baud = 460800;  break;
  }
  if (baud > 0) {
    brg = m8xx_get_brg( M8260_SCC_BRGS, baud*16 );
    m8260.cmxscr &= ~(0xFF000000 >> (8*(minor-SCC1_MINOR)) );
    m8260.cmxscr |= ((brg<<(3+8*(3-(minor-SCC1_MINOR)))) &
		     (brg<<(8*(3-(minor-SCC1_MINOR)))));
  }
  /* Number of data bits */
  switch ( t->c_cflag & CSIZE ) {
    case CS5:     csize = 0x0000;       break;
    case CS6:     csize = 0x1000;       break;
    case CS7:     csize = 0x2000;       break;
    case CS8:     csize = 0x3000;       break;
  }

  /* Stop bits */
  if ( t->c_cflag & CSTOPB )
    cstopb = 0x4000;              /* Two stop bits */
  else
    cstopb = 0x0000;              /* One stop bit */

  /* Parity */
  if ( t->c_cflag & PARENB )
    parenb = 0x0010;              /* Parity enabled on Tx and Rx */
  else
    parenb = 0x0000;              /* No parity on Tx and Rx */

  if ( t->c_cflag & PARODD )
    parodd = 0x0000;              /* Odd parity */
  else
    parodd = 0x000a;

  /* Write the SICR/PSMR Registers */
  switch (minor) {
    case SCC1_MINOR:
/*
      m8xx.sicr = ( (m8xx.sicr & 0xffffc0ff) | (brg << 11) | (brg << 8) );
*/
      m8260.scc1.psmr = ( (cstopb | csize | parenb | parodd) | (m8260.scc1.psmr & 0x8fe0) );
      break;
    case SCC2_MINOR:
/*
      m8xx.sicr = ( (m8xx.sicr & 0xffffc0ff) | (brg << 11) | (brg << 8) );
*/
      m8260.scc2.psmr = ( (cstopb | csize | parenb | parodd) | (m8260.scc2.psmr & 0x8fe0) );
      break;
    case SCC3_MINOR:
/*
      m8xx.sicr = ( (m8xx.sicr & 0xffc0ffff) | (brg << 19) | (brg << 16) );
*/
      m8260.scc3.psmr = ( (cstopb | csize | parenb | parodd) | (m8260.scc3.psmr & 0x8fe0) );
      break;
    case SCC4_MINOR:
/*
      m8xx.sicr = ( (m8xx.sicr & 0xc0ffffff) | (brg << 27) | (brg << 24) );
*/
      m8260.scc4.psmr = ( (cstopb | csize | parenb | parodd) | (m8260.scc4.psmr & 0x8fe0) );
      break;
  }

  return 0;
}


int
m8xx_uart_setAttributes(
  int minor,
  const struct termios *t
)
{
  /*
   * Check that port number is valid
   */
  if ( (minor < SMC1_MINOR) || (minor > NUM_PORTS-1) )
    return 0;

  switch (minor) {
    case SMC1_MINOR:
    case SMC2_MINOR:
      return m8xx_smc_set_attributes( minor, t );

    case SCC1_MINOR:
    case SCC2_MINOR:
    case SCC3_MINOR:
    case SCC4_MINOR:
      return m8xx_scc_set_attributes( minor, t );
  }
  return 0;
}


/*
 * Interrupt handlers
 */

static void
m8xx_scc1_interrupt_handler (rtems_irq_hdl_param unused)
{
  /*
   * Buffer received?
   */
  if ((m8260.scc1.sccm & M8260_SCCE_RX) && (m8260.scc1.scce & M8260_SCCE_RX)) {
    m8260.scc1.scce = M8260_SCCE_RX;    /* Clear the event */


    /* Check that the buffer is ours */
    if ((RxBd[SCC1_MINOR]->status & M8260_BD_EMPTY) == 0) {
      rtems_cache_invalidate_multiple_data_lines(
        (const void *) RxBd[SCC1_MINOR]->buffer,
        RxBd[SCC1_MINOR]->length );
      rtems_termios_enqueue_raw_characters(
        (void *)ttyp[SCC1_MINOR],
        (char *)RxBd[SCC1_MINOR]->buffer,
        (int)RxBd[SCC1_MINOR]->length );
      RxBd[SCC1_MINOR]->status = M8260_BD_EMPTY | M8260_BD_WRAP |
                                 M8260_BD_INTERRUPT;
    }
  }

  /*
   * Buffer transmitted?
   */
  if (m8260.scc1.scce & M8260_SCCE_TX) {
    m8260.scc1.scce = M8260_SCCE_TX;  /* Clear the event */

    /* Check that the buffer is ours */
    if ((TxBd[SCC1_MINOR]->status & M8260_BD_READY) == 0)
      rtems_termios_dequeue_characters (
        (void *)ttyp[SCC1_MINOR],
        (int)TxBd[SCC1_MINOR]->length);
  }

#if 0
  m8260.sipnr_l |= M8260_SIMASK_SCC1;      /* Clear pending register */
#endif
}

static void
m8xx_scc2_interrupt_handler (rtems_irq_hdl_param unused)
{
  /*
   * Buffer received?
   */
  if ((m8260.scc2.sccm & M8260_SCCE_RX) && (m8260.scc2.scce & M8260_SCCE_RX)) {
    m8260.scc2.scce = M8260_SCCE_RX;    /* Clear the event */


    /* Check that the buffer is ours */
    if ((RxBd[SCC2_MINOR]->status & M8260_BD_EMPTY) == 0) {
      rtems_cache_invalidate_multiple_data_lines(
        (const void *) RxBd[SCC2_MINOR]->buffer,
        RxBd[SCC2_MINOR]->length );
      rtems_termios_enqueue_raw_characters(
        (void *)ttyp[SCC2_MINOR],
        (char *)RxBd[SCC2_MINOR]->buffer,
        (int)RxBd[SCC2_MINOR]->length );
      RxBd[SCC2_MINOR]->status = M8260_BD_EMPTY | M8260_BD_WRAP |
                                 M8260_BD_INTERRUPT;
    }
  }

  /*
   * Buffer transmitted?
   */
  if (m8260.scc2.scce & M8260_SCCE_TX) {
    m8260.scc2.scce = M8260_SCCE_TX;  /* Clear the event */

    /* Check that the buffer is ours */
    if ((TxBd[SCC2_MINOR]->status & M8260_BD_READY) == 0)
      rtems_termios_dequeue_characters (
        (void *)ttyp[SCC2_MINOR],
        (int)TxBd[SCC2_MINOR]->length);
  }

#if 0
  m8260.sipnr_l |= M8260_SIMASK_SCC2;      /* Clear pending register */
#endif
}


static void
m8xx_scc3_interrupt_handler (rtems_irq_hdl_param unused)
{
  /*
   * Buffer received?
   */
  if ((m8260.scc3.sccm & M8260_SCCE_RX) && (m8260.scc3.scce & M8260_SCCE_RX)) {
    m8260.scc3.scce = M8260_SCCE_RX;  /* Clear the event */


    /* Check that the buffer is ours */
    if ((RxBd[SCC3_MINOR]->status & M8260_BD_EMPTY) == 0) {
      rtems_cache_invalidate_multiple_data_lines(
        (const void *) RxBd[SCC3_MINOR]->buffer,
        RxBd[SCC3_MINOR]->length );
      rtems_termios_enqueue_raw_characters(
        (void *)ttyp[SCC3_MINOR],
        (char *)RxBd[SCC3_MINOR]->buffer,
        (int)RxBd[SCC3_MINOR]->length );
      RxBd[SCC3_MINOR]->status = M8260_BD_EMPTY | M8260_BD_WRAP |
                                 M8260_BD_INTERRUPT;
    }
  }

  /*
   * Buffer transmitted?
   */
  if (m8260.scc3.scce & M8260_SCCE_TX) {
    m8260.scc3.scce = M8260_SCCE_TX;    /* Clear the event */

    /* Check that the buffer is ours */
    if ((TxBd[SCC3_MINOR]->status & M8260_BD_READY) == 0)
      rtems_termios_dequeue_characters (
        (void *)ttyp[SCC3_MINOR],
        (int)TxBd[SCC3_MINOR]->length);
  }


#if 0
  m8260.sipnr_l |= M8260_SIMASK_SCC3;      /* Clear pending register */
#endif
}


static void
m8xx_scc4_interrupt_handler (rtems_irq_hdl_param unused)
{
  /*
   * Buffer received?
   */
  if ((m8260.scc4.sccm & M8260_SCCE_RX) && (m8260.scc4.scce & M8260_SCCE_RX)) {
    m8260.scc4.scce = M8260_SCCE_RX;  /* Clear the event */


    /* Check that the buffer is ours */
    if ((RxBd[SCC4_MINOR]->status & M8260_BD_EMPTY) == 0) {
      rtems_cache_invalidate_multiple_data_lines(
        (const void *) RxBd[SCC4_MINOR]->buffer,
        RxBd[SCC4_MINOR]->length );
      rtems_termios_enqueue_raw_characters(
        (void *)ttyp[SCC4_MINOR],
        (char *)RxBd[SCC4_MINOR]->buffer,
        (int)RxBd[SCC4_MINOR]->length );
      RxBd[SCC4_MINOR]->status = M8260_BD_EMPTY | M8260_BD_WRAP |
                                 M8260_BD_INTERRUPT;
    }
  }

  /*
   * Buffer transmitted?
   */
  if (m8260.scc4.scce & M8260_SCCE_TX) {
    m8260.scc4.scce = M8260_SCCE_TX;    /* Clear the event */

    /* Check that the buffer is ours */
    if ((TxBd[SCC4_MINOR]->status & M8260_BD_READY) == 0)
      rtems_termios_dequeue_characters (
        (void *)ttyp[SCC4_MINOR],
        (int)TxBd[SCC4_MINOR]->length);
  }

#if 0
  m8260.sipnr_l |= M8260_SIMASK_SCC4;      /* Clear pending register */
#endif
}

static void
m8xx_smc1_interrupt_handler (rtems_irq_hdl_param unused)
{
  /*
   * Buffer received?
   */
  if (m8260.smc1.smce & M8260_SMCE_RX) {
    m8260.smc1.smce = M8260_SMCE_RX;  /* Clear the event */


    /* Check that the buffer is ours */
    if ((RxBd[SMC1_MINOR]->status & M8260_BD_EMPTY) == 0) {
      rtems_cache_invalidate_multiple_data_lines(
        (const void *) RxBd[SMC1_MINOR]->buffer,
        RxBd[SMC1_MINOR]->length );
      rtems_termios_enqueue_raw_characters(
        (void *)ttyp[SMC1_MINOR],
        (char *)RxBd[SMC1_MINOR]->buffer,
        (int)RxBd[SMC1_MINOR]->length );
      RxBd[SMC1_MINOR]->status = M8260_BD_EMPTY | M8260_BD_WRAP |
                                 M8260_BD_INTERRUPT;
    }
  }

  /*
   * Buffer transmitted?
   */
  if (m8260.smc1.smce & M8260_SMCE_TX) {
    m8260.smc1.smce = M8260_SMCE_TX;    /* Clear the event */

    /* Check that the buffer is ours */
    if ((TxBd[SMC1_MINOR]->status & M8260_BD_READY) == 0)
      rtems_termios_dequeue_characters (
        (void *)ttyp[SMC1_MINOR],
        (int)TxBd[SMC1_MINOR]->length);
  }

#if 0
  m8260.sipnr_l = 0x00001000; /* Clear SMC1 interrupt-in-service bit */
#endif
}


static void
m8xx_smc2_interrupt_handler (rtems_irq_hdl_param unused)
{
  /*
   * Buffer received?
   */
  if (m8260.smc2.smce & M8260_SMCE_RX) {
    m8260.smc2.smce = M8260_SMCE_RX;  /* Clear the event */


    /* Check that the buffer is ours */
    if ((RxBd[SMC2_MINOR]->status & M8260_BD_EMPTY) == 0) {
      rtems_cache_invalidate_multiple_data_lines(
        (const void *) RxBd[SMC2_MINOR]->buffer,
        RxBd[SMC2_MINOR]->length );
      rtems_termios_enqueue_raw_characters(
        (void *)ttyp[SMC2_MINOR],
        (char *)RxBd[SMC2_MINOR]->buffer,
        (int)RxBd[SMC2_MINOR]->length );
      RxBd[SMC2_MINOR]->status = M8260_BD_EMPTY | M8260_BD_WRAP |
                                 M8260_BD_INTERRUPT;
    }
  }

  /*
   * Buffer transmitted?
   */
  if (m8260.smc2.smce & M8260_SMCE_TX) {
    m8260.smc2.smce = M8260_SMCE_TX;    /* Clear the event */

    /* Check that the buffer is ours */
    if ((TxBd[SMC2_MINOR]->status & M8260_BD_READY) == 0)
      rtems_termios_dequeue_characters (
        (void *)ttyp[SMC2_MINOR],
        (int)TxBd[SMC2_MINOR]->length);
  }

#if 0
  m8260.sipnr_l = 0x00000800; /* Clear SMC2 interrupt-in-service bit */
#endif
}


void m8xx_scc_enable(const rtems_irq_connect_data* ptr)
{
  volatile m8260SCCRegisters_t *sccregs = 0;
  switch (ptr->name) {
  case BSP_CPM_IRQ_SCC4 :
    m8260.sipnr_l |= M8260_SIMASK_SCC4;
    sccregs = &m8260.scc4;
    break;
  case BSP_CPM_IRQ_SCC3 :
    m8260.sipnr_l |= M8260_SIMASK_SCC3;
    sccregs = &m8260.scc3;
    break;
  case BSP_CPM_IRQ_SCC2 :
    m8260.sipnr_l |= M8260_SIMASK_SCC2;
    sccregs = &m8260.scc2;
    break;
  case BSP_CPM_IRQ_SCC1 :
    m8260.sipnr_l |= M8260_SIMASK_SCC1;
    sccregs = &m8260.scc1;
    break;
  default:
    break;
  }
  sccregs->sccm = 3;
}

void m8xx_scc_disable(const rtems_irq_connect_data* ptr)
{
  volatile m8260SCCRegisters_t *sccregs = 0;
  switch (ptr->name) {
  case BSP_CPM_IRQ_SCC4 :
    sccregs = &m8260.scc4;
    break;
  case BSP_CPM_IRQ_SCC3 :
    sccregs = &m8260.scc3;
    break;
  case BSP_CPM_IRQ_SCC2 :
    sccregs = &m8260.scc2;
    break;
  case BSP_CPM_IRQ_SCC1 :
    sccregs = &m8260.scc1;
    break;
  default:
    break;
  }
  sccregs->sccm &= (~3);
}

int m8xx_scc_isOn(const rtems_irq_connect_data* ptr)
{
 return BSP_irq_enabled_at_cpm (ptr->name);
}

static rtems_irq_connect_data consoleIrqData =
{
  BSP_CPM_IRQ_SCC1,
  (rtems_irq_hdl)m8xx_scc1_interrupt_handler,
  NULL,
  (rtems_irq_enable) m8xx_scc_enable,
  (rtems_irq_disable) m8xx_scc_disable,
  (rtems_irq_is_enabled) m8xx_scc_isOn
};


void
m8xx_uart_scc_initialize (int minor)
{
  unsigned char brg;
  volatile m8260SCCparms_t *sccparms = 0;
  volatile m8260SCCRegisters_t *sccregs = 0;

  /*
   * Check that minor number is valid
   */
  if ( (minor < SCC1_MINOR) || (minor > NUM_PORTS-1) )
    return;

  /* Get the sicr clock source bit values for 9600 bps */
  brg = m8xx_get_brg(M8260_SCC_BRGS, 9600*16);

  m8260.cmxscr &= ~(0xFF000000 >> (8*(minor-SCC1_MINOR)) );
  m8260.cmxscr |= (brg<<(3+8*(3-(minor-SCC1_MINOR))));
  m8260.cmxscr |= (brg<<(8*(3-(minor-SCC1_MINOR))));

  /*
   * Allocate buffer descriptors
   */
  RxBd[minor] = m8xx_bd_allocate(1);
  TxBd[minor] = m8xx_bd_allocate(1);

  /*
   * Configure ports to enable TXDx and RXDx pins
   */

  m8260.ppard |=  (0x07 << ((minor-SCC1_MINOR)*3));
  m8260.psord &= ~(0x07 << ((minor-SCC1_MINOR)*3));
  if( minor == SCC1_MINOR )
    m8260.psord |= 0x02;
  m8260.pdird |=  (0x06 << ((minor-SCC1_MINOR)*3));
  m8260.pdird &= ~(0x01 << ((minor-SCC1_MINOR)*3));


  /*
   * Set up SMC1 parameter RAM common to all protocols
   */
  if( minor == SCC1_MINOR ) {
    sccparms = (m8260SCCparms_t*)&m8260.scc1p;
    sccregs  = (m8260SCCRegisters_t*)&m8260.scc1;
  }
  else if( minor == SCC2_MINOR ) {
    sccparms = (m8260SCCparms_t*)&m8260.scc2p;
    sccregs  = (m8260SCCRegisters_t*)&m8260.scc2;
  }
  else if( minor == SCC3_MINOR ) {
    sccparms = (m8260SCCparms_t*)&m8260.scc3p;
    sccregs  = (m8260SCCRegisters_t*)&m8260.scc3;
  }
  else {
    sccparms = (m8260SCCparms_t*)&m8260.scc4p;
    sccregs  = (m8260SCCRegisters_t*)&m8260.scc4;
  }

  sccparms->rbase = (char *)RxBd[minor] - (char *)&m8260;
  sccparms->tbase = (char *)TxBd[minor] - (char *)&m8260;




  sccparms->rfcr = M8260_RFCR_MOT | M8260_RFCR_60X_BUS;
  sccparms->tfcr = M8260_TFCR_MOT | M8260_TFCR_60X_BUS;
  if ( (mbx8xx_console_get_configuration() & 0x06) == 0x02 )
    sccparms->mrblr = RXBUFSIZE;    /* Maximum Rx buffer size */
  else
    sccparms->mrblr = 1;            /* Maximum Rx buffer size */
  sccparms->un.uart.max_idl = 10;   /* Set nb of idle chars to close buffer */
  sccparms->un.uart.brkcr = 0;      /* Set nb of breaks to send for STOP Tx */

  sccparms->un.uart.parec = 0;      /* Clear parity error counter */
  sccparms->un.uart.frmec = 0;      /* Clear framing error counter */
  sccparms->un.uart.nosec = 0;      /* Clear noise counter */
  sccparms->un.uart.brkec = 0;      /* Clear break counter */

  sccparms->un.uart.uaddr[0] = 0;   /* Not in multidrop mode, so clear */
  sccparms->un.uart.uaddr[1] = 0;   /* Not in multidrop mode, so clear */
  sccparms->un.uart.toseq  = 0;     /* Tx Out-Of-SEQuence--no XON/XOFF now */

  sccparms->un.uart.character[0] = 0x8000; /* Entry is invalid */
  sccparms->un.uart.character[1] = 0x8000; /* Entry is invalid */
  sccparms->un.uart.character[2] = 0x8000; /* Entry is invalid */
  sccparms->un.uart.character[3] = 0x8000; /* Entry is invalid */
  sccparms->un.uart.character[4] = 0x8000; /* Entry is invalid */
  sccparms->un.uart.character[5] = 0x8000; /* Entry is invalid */
  sccparms->un.uart.character[6] = 0x8000; /* Entry is invalid */
  sccparms->un.uart.character[7] = 0x8000; /* Entry is invalid */


  sccparms->un.uart.rccm = 0xc0ff;  /* No masking */

  /*
   * Set up the Receive Buffer Descriptor
   */
  RxBd[minor]->status = M8260_BD_EMPTY | M8260_BD_WRAP | M8260_BD_INTERRUPT;
  RxBd[minor]->length = 0;
  RxBd[minor]->buffer = rxBuf[minor];

  /*
   * Setup the Transmit Buffer Descriptor
   */
  TxBd[minor]->status = M8260_BD_WRAP;

 /*
   * Set up SCCx general and protocol-specific mode registers
   */
  sccregs->gsmr_h = 0x00000020;     /* RFW=low latency operation */
  sccregs->gsmr_l = 0x00028004;     /* TDCR=RDCR=16x clock mode, MODE=uart*/
  sccregs->scce = ~0;               /* Clear any pending event */
  sccregs->sccm = 0;                /* Mask all interrupt/event sources */
  sccregs->psmr = 0x3000;           /* Normal operation & mode, 1 stop bit,
                                       8 data bits, no parity */
  sccregs->dsr = 0x7E7E;            /* No fractional stop bits */
  sccregs->gsmr_l = 0x00028034;     /* ENT=enable Tx, ENR=enable Rx */

  /*
   *  Initialize the Rx and Tx with the new parameters.
   */
  switch (minor) {
    case SCC1_MINOR:
      m8xx_cp_execute_cmd (M8260_CR_OP_INIT_RX_TX | M8260_CR_SCC1);
      break;
    case SCC2_MINOR:
      m8xx_cp_execute_cmd (M8260_CR_OP_INIT_RX_TX | M8260_CR_SCC2);
      break;
    case SCC3_MINOR:
      m8xx_cp_execute_cmd (M8260_CR_OP_INIT_RX_TX | M8260_CR_SCC3);
      break;
    case SCC4_MINOR:
      m8xx_cp_execute_cmd (M8260_CR_OP_INIT_RX_TX | M8260_CR_SCC4);
      break;
  }

  if ( (mbx8xx_console_get_configuration() & 0x06) == 0x02 ) {
    switch (minor) {
      case SCC1_MINOR:
	consoleIrqData.name = BSP_CPM_IRQ_SCC1;
        consoleIrqData.hdl = m8xx_scc1_interrupt_handler;
	break;
      case SCC2_MINOR:
	consoleIrqData.name = BSP_CPM_IRQ_SCC2;
        consoleIrqData.hdl = m8xx_scc2_interrupt_handler;
	break;
      case SCC3_MINOR:
	consoleIrqData.name = BSP_CPM_IRQ_SCC3;
        consoleIrqData.hdl = m8xx_scc3_interrupt_handler;
	break;
      case SCC4_MINOR:
	consoleIrqData.name = BSP_CPM_IRQ_SCC4;
        consoleIrqData.hdl = m8xx_scc4_interrupt_handler;
	break;

    }
    if (!BSP_install_rtems_irq_handler (&consoleIrqData)) {
      printk("Unable to connect SCC Irq handler\n");
      rtems_fatal_error_occurred(1);
    }
  }
}



void m8xx_smc_enable(const rtems_irq_connect_data* ptr)
{
  volatile m8260SMCRegisters_t *smcregs = 0;
  switch (ptr->name) {
  case BSP_CPM_IRQ_SMC1 :
    smcregs = &m8260.smc1;
    break;
  case BSP_CPM_IRQ_SMC2 :
    smcregs = &m8260.smc2;
    break;
  default:
    break;
  }
  smcregs->smcm = 3;
}

void m8xx_smc_disable(const rtems_irq_connect_data* ptr)
{
  volatile m8260SMCRegisters_t *smcregs = 0;
  switch (ptr->name) {
  case BSP_CPM_IRQ_SMC1 :
    smcregs = &m8260.smc1;
    break;
  case BSP_CPM_IRQ_SMC2 :
    smcregs = &m8260.smc2;
    break;
  default:
    break;
  }
  smcregs->smcm &= (~3);
}

int m8xx_smc_isOn(const rtems_irq_connect_data* ptr)
{
 return BSP_irq_enabled_at_cpm (ptr->name);
}


void
m8xx_uart_smc_initialize (int minor)
{
  unsigned char brg;
  volatile m8260SMCparms_t *smcparms = 0;
  volatile m8260SMCRegisters_t *smcregs = 0;

  /*
   * Check that minor number is valid
   */
  if ( (minor < SMC1_MINOR) || (minor > SMC2_MINOR) )
    return;

  /* Get the simode clock source bit values for 9600 bps */
  if( minor == SMC1_MINOR )
    brg = m8xx_get_brg(M8260_SMC1_BRGS, 9600*16);
  else
    brg = m8xx_get_brg(M8260_SMC2_BRGS, 9600*16);

  /*
   * Allocate buffer descriptors
   */
  RxBd[minor] = m8xx_bd_allocate (1);
  TxBd[minor] = m8xx_bd_allocate (1);

  /*
   *  Get the address of the parameter RAM for the specified port,
   *  configure I/O port B and put SMC in NMSI mode, connect the
   *  SMC to the appropriate BRG.
   *
   *  SMC2 RxD is shared with port B bit 20
   *  SMC2 TxD is shared with port B bit 21
   *  SMC1 RxD is shared with port B bit 24
   *  SMC1 TxD is shared with port B bit 25
   */
  switch (minor) {
    case SMC1_MINOR:
      smcparms = &m8260.smc1p;
      smcregs  = &m8260.smc1;

#if 0
      m8260.pbpar |=  0x000000C0;    /* PB24 & PB25 are dedicated peripheral pins */
      m8260.pbdir &= ~0x000000C0;    /* PB24 & PB25 must not drive UART lines */
      m8260.pbodr &= ~0x000000C0;    /* PB24 & PB25 are not open drain */

      m8260.simode &= 0xFFFF0FFF;    /* Clear SMC1CS & SMC1 for NMSI mode */
      m8260.simode |= brg << 12;     /* SMC1CS = brg */
#endif
      break;

    case SMC2_MINOR:
      smcparms = &m8260.smc2p;
      smcregs = &m8260.smc2;
#if 0
      m8260.pbpar |=  0x00000C00;    /* PB20 & PB21 are dedicated peripheral pins */
      m8260.pbdir &= ~0x00000C00;    /* PB20 & PB21 must not drive the UART lines */
      m8260.pbodr &= ~0x00000C00;    /* PB20 & PB21 are not open drain */

      m8260.simode &= 0x0FFFFFFF;    /* Clear SMC2CS & SMC2 for NMSI mode */
      m8260.simode |= brg << 28;     /* SMC2CS = brg */
#endif
      break;
  }

  /*
   * Set up SMC parameter RAM common to all protocols
   */
  smcparms->rbase = (char *)RxBd[minor] - (char *)&m8260;
  smcparms->tbase = (char *)TxBd[minor] - (char *)&m8260;
  smcparms->rfcr = M8260_RFCR_MOT | M8260_RFCR_60X_BUS;
  smcparms->tfcr = M8260_TFCR_MOT | M8260_TFCR_60X_BUS;
  if ( (mbx8xx_console_get_configuration() & 0x06) == 0x02 )
    smcparms->mrblr = RXBUFSIZE;    /* Maximum Rx buffer size */
  else
    smcparms->mrblr = 1;            /* Maximum Rx buffer size */

  /*
   * Set up SMC1 parameter RAM UART-specific parameters
   */
  smcparms->un.uart.max_idl = 10;   /* Set nb of idle chars to close buffer */
  smcparms->un.uart.brkcr = 0;      /* Set nb of breaks to send for STOP Tx */
  smcparms->un.uart.brkec = 0;      /* Clear break counter */

  /*
   * Set up the Receive Buffer Descriptor
   */
  RxBd[minor]->status = M8260_BD_EMPTY | M8260_BD_WRAP | M8260_BD_INTERRUPT;
  RxBd[minor]->length = 0;
  RxBd[minor]->buffer = rxBuf[minor];

  /*
   * Setup the Transmit Buffer Descriptor
   */
  TxBd[minor]->status = M8260_BD_WRAP;

  /*
   * Set up SMCx general and protocol-specific mode registers
   */
  smcregs->smce = ~0;               /* Clear any pending events */
  smcregs->smcm = 0;                /* Enable SMC Rx & Tx interrupts */
  smcregs->smcmr = M8260_SMCMR_CLEN(9) | M8260_SMCMR_SM_UART;

  /*
   * Send "Init parameters" command
   */
  switch (minor) {
    case SMC1_MINOR:
      m8xx_cp_execute_cmd (M8260_CR_OP_INIT_RX_TX | M8260_CR_SMC1);
      break;

    case SMC2_MINOR:
      m8xx_cp_execute_cmd (M8260_CR_OP_INIT_RX_TX | M8260_CR_SMC2);
      break;
  }

  /*
   * Enable receiver and transmitter
   */
  smcregs->smcmr |= M8260_SMCMR_TEN | M8260_SMCMR_REN;
  if ( (mbx8xx_console_get_configuration() & 0x06) == 0x02 ) {
    consoleIrqData.on = m8xx_smc_enable;
    consoleIrqData.off = m8xx_smc_disable;
    consoleIrqData.isOn = m8xx_smc_isOn;
    switch (minor) {
      case SMC1_MINOR:
	consoleIrqData.name = BSP_CPM_IRQ_SMC1;
	consoleIrqData.hdl  = m8xx_smc1_interrupt_handler;
	break;

      case SMC2_MINOR:
	consoleIrqData.name = BSP_CPM_IRQ_SMC2;
	consoleIrqData.hdl  = m8xx_smc2_interrupt_handler;
	break;
#if 0
      case SMC1_MINOR:
        rtems_interrupt_catch (m8xx_smc1_interrupt_handler,
                                    PPC_IRQ_CPM_SMC1,
                                    &old_handler[minor]);

        smcregs->smcm = 3;            /* Enable SMC1 Rx & Tx interrupts */
        m8260.sipnr_l |= M8260_SIMASK_SMC1;      /* Clear pending register */
        m8260.simr_l  |= M8260_SIMASK_SMC1;      /* Enable SMC1 interrupts */
        break;

      case SMC2_MINOR:
        rtems_interrupt_catch (m8xx_smc2_interrupt_handler,
                                    PPC_IRQ_CPM_SMC2,
                                    &old_handler[minor]);

        smcregs->smcm = 3;            /* Enable SMC2 Rx & Tx interrupts */
        m8260.sipnr_l |= M8260_SIMASK_SMC2;      /* Clear pending register */
        m8260.simr_l  |= M8260_SIMASK_SMC2;      /* Enable SMC2 interrupts */
        break;
#endif
    }
    if (!BSP_install_rtems_irq_handler (&consoleIrqData)) {
      printk("Unable to connect SMC Irq handler\n");
      rtems_fatal_error_occurred(1);
    }
  }
}

void
m8xx_uart_initialize(void)
{

}


void
m8xx_uart_interrupts_initialize(void)
{
#ifdef mpc8260
  /* CHECK THIS */

#else

#if defined(mpc860)
  m8xx.cicr = 0x00E43F80;           /* SCaP=SCC1, SCbP=SCC2, SCcP=SCC3,
                                       SCdP=SCC4, IRL=1, HP=PC15, IEN=1 */
#else
  m8xx.cicr = 0x00043F80;           /* SCaP=SCC1, SCbP=SCC2, IRL=1, HP=PC15, IEN=1 */
#endif
  m8xx.simask |= M8xx_SIMASK_LVM1;  /* Enable level interrupts */

#endif
}


int
m8xx_uart_pollRead(
  int minor
)
{
  unsigned char c;

  if (RxBd[minor]->status & M8260_BD_EMPTY) {
    return -1;
  }
  rtems_cache_invalidate_multiple_data_lines(
    (const void *) RxBd[minor]->buffer,
    RxBd[minor]->length
  );
  c = ((char *)RxBd[minor]->buffer)[0];
  RxBd[minor]->status = M8260_BD_EMPTY | M8260_BD_WRAP;
  return c;
}


/*
 *  TODO: Get a free buffer and set it up.
 */
ssize_t
m8xx_uart_write(
  int minor,
  const char *buf,
  size_t len
)
{
  while( (TxBd[minor]->status) & M8260_BD_READY );

  rtems_cache_flush_multiple_data_lines( buf, len );
  TxBd[minor]->buffer = (char *) buf;
  TxBd[minor]->length = len;
  TxBd[minor]->status = M8260_BD_READY | M8260_BD_WRAP | M8260_BD_INTERRUPT;
  return 0;
}


ssize_t
m8xx_uart_pollWrite(
  int minor,
  const char *buf,
  size_t len
)
{
  size_t retval = len;

  while (len--) {
    while (TxBd[minor]->status & M8260_BD_READY)
      continue;
    txBuf[minor] = *buf++;
    rtems_cache_flush_multiple_data_lines( (void *)&txBuf[minor], 1 );
    TxBd[minor]->buffer = &txBuf[minor];
    TxBd[minor]->length = 1;
    TxBd[minor]->status = M8260_BD_READY | M8260_BD_WRAP;
  }

  return retval;
}
