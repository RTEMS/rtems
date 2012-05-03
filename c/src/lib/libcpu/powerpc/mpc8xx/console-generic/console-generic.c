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
 *    SCC1       2                   N/A. Hardwired as ethernet port
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
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications by Darlene Stewart <Darlene.Stewart@iit.nrc.ca>
 *  and Charles-Antoine Gauthier <charles.gauthier@iit.nrc.ca>
 *  Copyright (c) 1999, National Research Council of Canada
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/libio.h>
#include <mpc8xx.h>
#include <mpc8xx/console.h>
#include <mpc8xx/cpm.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <bsp/irq.h>
#include <rtems/bspIo.h>   /* for printk */

int BSP_irq_enabled_at_cpm(const rtems_irq_number irqLine);

/* BSP supplied routine */
extern int mbx8xx_console_get_configuration(void);

extern uint32_t bsp_clock_speed;

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
static volatile m8xxBufferDescriptor_t *RxBd[NUM_PORTS], *TxBd[NUM_PORTS];

/* Used to track the usage of the baud rate generators */
static unsigned long brg_spd[4];
static char  brg_used[4];

/* Used to track termios private data for callbacks */
struct rtems_termios_tty *ttyp[NUM_PORTS];

/*
 * Device-specific routines
 */
static int m8xx_get_brg_cd(int);
static unsigned char m8xx_get_brg_clk(int);
static int m8xx_smc_set_attributes(int, const struct termios*);
static int m8xx_scc_set_attributes(int, const struct termios*);
static void m8xx_smc1_interrupt_handler(void *unused);
static void m8xx_smc2_interrupt_handler(void *unused);
static void m8xx_scc2_interrupt_handler(void *unused);
#if defined(mpc860)
static void m8xx_scc3_interrupt_handler(void *unused);
static void m8xx_scc4_interrupt_handler(void *unused);
#endif

/*
 * Compute baud-rate-generator configuration register value
 */
static int
m8xx_get_brg_cd (int baud)
{
  int divisor;
  int div16 = 0;

  divisor = ((bsp_clock_speed / 16) + (baud / 2)) / baud;
  if (divisor > 4096) {
    div16 = 1;
    divisor = (divisor + 8) / 16;
  }
  return M8xx_BRG_EN | M8xx_BRG_EXTC_BRGCLK |
    ((divisor - 1) << 1) | div16;
}


/*
 *  This function will fail if more that 4 baud rates have been selected
 *  at any time since the OS started. It needs to be fixed. FIXME
 */
static unsigned
char m8xx_get_brg_clk(int baud)
{
  int i;

  /* first try to find a BRG that is already at the right speed */
  for ( i = 0; i < 4; i++ ) {
    if ( brg_spd[i] == baud ) {
      break;
    }
  }

  if ( i == 4 ) { /* I guess we didn't find one */
    for ( i = 0; i < 4; i++ ) {
      if ( brg_used[i] == 0 ) {
        break;
      }
    }
  }
  if (i != 4) {
    brg_used[i]++;
    brg_spd[i]=baud;
    switch (i) {
    case 0:
      m8xx.brgc1 = M8xx_BRG_RST;
      m8xx.brgc1 = m8xx_get_brg_cd(baud);
      break;
    case 1:
      m8xx.brgc2 = M8xx_BRG_RST;
      m8xx.brgc2 = m8xx_get_brg_cd(baud);
      break;
    case 2:
      m8xx.brgc3 = M8xx_BRG_RST;
      m8xx.brgc3 = m8xx_get_brg_cd(baud);
      break;
    case 3:
      m8xx.brgc4 = M8xx_BRG_RST;
      m8xx.brgc4 = m8xx_get_brg_cd(baud);
      break;
    }
    return i;
  }

  else
    return 0xff;
}


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
  if (baud > 0)
    brg = m8xx_get_brg_clk(baud);     /* 4 BRGs, 6 serial ports - hopefully */
                                      /*  at least 2 ports will be the same */

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
      m8xx.simode = ( (m8xx.simode & 0xffff8fff) | (brg << 12) );
      m8xx.smc1.smcmr = clen | cstopb | parenb | parodd | cread;
      break;
    case SMC2_MINOR:
      m8xx.simode = ( (m8xx.simode & 0x8fffffff) | (brg << 28) );
      m8xx.smc2.smcmr = clen | cstopb | parenb | parodd | cread;
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
  if (baud > 0)
    brg = m8xx_get_brg_clk(baud);     /* 4 BRGs, 5 serial ports - hopefully */
                                      /*  at least 2 ports will be the same */
                                      /*  Write the SICR register below */

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
    case SCC2_MINOR:
      m8xx.sicr = ( (m8xx.sicr & 0xffffc0ff) | (brg << 11) | (brg << 8) );
      m8xx.scc2.psmr = ( (cstopb | csize | parenb | parodd) | (m8xx.scc2.psmr & 0x8fe0) );
      break;
  #if defined(mpc860)
    case SCC3_MINOR:
      m8xx.sicr = ( (m8xx.sicr & 0xffc0ffff) | (brg << 19) | (brg << 16) );
      m8xx.scc3.psmr = ( (cstopb | csize | parenb | parodd) | (m8xx.scc3.psmr & 0x8fe0) );
      break;
    case SCC4_MINOR:
      m8xx.sicr = ( (m8xx.sicr & 0xc0ffffff) | (brg << 27) | (brg << 24) );
      m8xx.scc4.psmr = ( (cstopb | csize | parenb | parodd) | (m8xx.scc4.psmr & 0x8fe0) );
      break;
  #endif
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
static void m8xx_scc2_interrupt_handler (void *unused)
{
  /*
   * Buffer received?
   */
  if ((m8xx.scc2.sccm & M8xx_SCCE_RX) && (m8xx.scc2.scce & M8xx_SCCE_RX)) {
    m8xx.scc2.scce = M8xx_SCCE_RX;    /* Clear the event */


    /* Check that the buffer is ours */
    if ((RxBd[SCC2_MINOR]->status & M8xx_BD_EMPTY) == 0) {
      rtems_cache_invalidate_multiple_data_lines(
        (const void *) RxBd[SCC2_MINOR]->buffer,
        RxBd[SCC2_MINOR]->length );
      rtems_termios_enqueue_raw_characters(
        (void *)ttyp[SCC2_MINOR],
        (char *)RxBd[SCC2_MINOR]->buffer,
        (int)RxBd[SCC2_MINOR]->length );
      RxBd[SCC2_MINOR]->status = M8xx_BD_EMPTY | M8xx_BD_WRAP |
                                 M8xx_BD_INTERRUPT;
    }
  }

  /*
   * Buffer transmitted?
   */
  if (m8xx.scc2.scce & M8xx_SCCE_TX) {
    m8xx.scc2.scce = M8xx_SCCE_TX;  /* Clear the event */

    /* Check that the buffer is ours */
    if ((TxBd[SCC2_MINOR]->status & M8xx_BD_READY) == 0)
      rtems_termios_dequeue_characters (
        (void *)ttyp[SCC2_MINOR],
        (int)TxBd[SCC2_MINOR]->length);
  }
}


#ifdef mpc860
static void
m8xx_scc3_interrupt_handler (void *unused)
{
  /*
   * Buffer received?
   */
  if ((m8xx.scc3.sccm & M8xx_SCCE_RX) && (m8xx.scc3.scce & M8xx_SCCE_RX)) {
    m8xx.scc3.scce = M8xx_SCCE_RX;  /* Clear the event */


    /* Check that the buffer is ours */
    if ((RxBd[SCC3_MINOR]->status & M8xx_BD_EMPTY) == 0) {
      rtems_cache_invalidate_multiple_data_lines(
        (const void *) RxBd[SCC3_MINOR]->buffer,
        RxBd[SCC3_MINOR]->length );
      rtems_termios_enqueue_raw_characters(
        (void *)ttyp[SCC3_MINOR],
        (char *)RxBd[SCC3_MINOR]->buffer,
        (int)RxBd[SCC3_MINOR]->length );
      RxBd[SCC3_MINOR]->status = M8xx_BD_EMPTY | M8xx_BD_WRAP |
                                 M8xx_BD_INTERRUPT;
    }
  }

  /*
   * Buffer transmitted?
   */
  if (m8xx.scc3.scce & M8xx_SCCE_TX) {
    m8xx.scc3.scce = M8xx_SCCE_TX;    /* Clear the event */

    /* Check that the buffer is ours */
    if ((TxBd[SCC3_MINOR]->status & M8xx_BD_READY) == 0)
      rtems_termios_dequeue_characters (
        (void *)ttyp[SCC3_MINOR],
        (int)TxBd[SCC3_MINOR]->length);
  }
}


static void
m8xx_scc4_interrupt_handler (void *unused)
{
  /*
   * Buffer received?
   */
  if ((m8xx.scc4.sccm & M8xx_SCCE_RX) && (m8xx.scc4.scce & M8xx_SCCE_RX)) {
    m8xx.scc4.scce = M8xx_SCCE_RX;  /* Clear the event */


    /* Check that the buffer is ours */
    if ((RxBd[SCC4_MINOR]->status & M8xx_BD_EMPTY) == 0) {
      rtems_cache_invalidate_multiple_data_lines(
        (const void *) RxBd[SCC4_MINOR]->buffer,
        RxBd[SCC4_MINOR]->length );
      rtems_termios_enqueue_raw_characters(
        (void *)ttyp[SCC4_MINOR],
        (char *)RxBd[SCC4_MINOR]->buffer,
        (int)RxBd[SCC4_MINOR]->length );
      RxBd[SCC4_MINOR]->status = M8xx_BD_EMPTY | M8xx_BD_WRAP |
                                 M8xx_BD_INTERRUPT;
    }
  }

  /*
   * Buffer transmitted?
   */
  if (m8xx.scc4.scce & M8xx_SCCE_TX) {
    m8xx.scc4.scce = M8xx_SCCE_TX;    /* Clear the event */

    /* Check that the buffer is ours */
    if ((TxBd[SCC4_MINOR]->status & M8xx_BD_READY) == 0)
      rtems_termios_dequeue_characters (
        (void *)ttyp[SCC4_MINOR],
        (int)TxBd[SCC4_MINOR]->length);
  }
}
#endif

static void
m8xx_smc1_interrupt_handler (void *unused)
{
  /*
   * Buffer received?
   */
  if (m8xx.smc1.smce & M8xx_SMCE_RX) {
    m8xx.smc1.smce = M8xx_SMCE_RX;  /* Clear the event */


    /* Check that the buffer is ours */
    if ((RxBd[SMC1_MINOR]->status & M8xx_BD_EMPTY) == 0) {
      rtems_cache_invalidate_multiple_data_lines(
        (const void *) RxBd[SMC1_MINOR]->buffer,
        RxBd[SMC1_MINOR]->length );
      rtems_termios_enqueue_raw_characters(
        (void *)ttyp[SMC1_MINOR],
        (char *)RxBd[SMC1_MINOR]->buffer,
        (int)RxBd[SMC1_MINOR]->length );
      RxBd[SMC1_MINOR]->status = M8xx_BD_EMPTY | M8xx_BD_WRAP |
                                 M8xx_BD_INTERRUPT;
    }
  }

  /*
   * Buffer transmitted?
   */
  if (m8xx.smc1.smce & M8xx_SMCE_TX) {
    m8xx.smc1.smce = M8xx_SMCE_TX;    /* Clear the event */

    /* Check that the buffer is ours */
    if ((TxBd[SMC1_MINOR]->status & M8xx_BD_READY) == 0)
      rtems_termios_dequeue_characters (
        (void *)ttyp[SMC1_MINOR],
        (int)TxBd[SMC1_MINOR]->length);
  }
}


static void
m8xx_smc2_interrupt_handler (void *unused)
{
  /*
   * Buffer received?
   */
  if (m8xx.smc2.smce & M8xx_SMCE_RX) {
    m8xx.smc2.smce = M8xx_SMCE_RX;  /* Clear the event */


    /* Check that the buffer is ours */
    if ((RxBd[SMC2_MINOR]->status & M8xx_BD_EMPTY) == 0) {
      rtems_cache_invalidate_multiple_data_lines(
        (const void *) RxBd[SMC2_MINOR]->buffer,
        RxBd[SMC2_MINOR]->length );
      rtems_termios_enqueue_raw_characters(
        (void *)ttyp[SMC2_MINOR],
        (char *)RxBd[SMC2_MINOR]->buffer,
        (int)RxBd[SMC2_MINOR]->length );
      RxBd[SMC2_MINOR]->status = M8xx_BD_EMPTY | M8xx_BD_WRAP |
                                 M8xx_BD_INTERRUPT;
    }
  }

  /*
   * Buffer transmitted?
   */
  if (m8xx.smc2.smce & M8xx_SMCE_TX) {
    m8xx.smc2.smce = M8xx_SMCE_TX;    /* Clear the event */

    /* Check that the buffer is ours */
    if ((TxBd[SMC2_MINOR]->status & M8xx_BD_READY) == 0)
      rtems_termios_dequeue_characters (
        (void *)ttyp[SMC2_MINOR],
        (int)TxBd[SMC2_MINOR]->length);
  }
}

void m8xx_scc_enable(const rtems_irq_connect_data* ptr)
{
  volatile m8xxSCCRegisters_t *sccregs = 0;
  switch (ptr->name) {
#if defined(mpc860)
  case BSP_CPM_IRQ_SCC4 :
    sccregs = &m8xx.scc4;
    break;
  case BSP_CPM_IRQ_SCC3 :
    sccregs = &m8xx.scc3;
    break;
#endif
  case BSP_CPM_IRQ_SCC2 :
    sccregs = &m8xx.scc2;
    break;
  case BSP_CPM_IRQ_SCC1 :
    sccregs = &m8xx.scc1;
    break;
  default:
    break;
  }
  sccregs->sccm = 3;
}

void m8xx_scc_disable(const rtems_irq_connect_data* ptr)
{
  volatile m8xxSCCRegisters_t *sccregs = 0;
  switch (ptr->name) {
#if defined(mpc860)
  case BSP_CPM_IRQ_SCC4 :
    sccregs = &m8xx.scc4;
    break;
  case BSP_CPM_IRQ_SCC3 :
    sccregs = &m8xx.scc3;
    break;
#endif
  case BSP_CPM_IRQ_SCC2 :
    sccregs = &m8xx.scc2;
    break;
  case BSP_CPM_IRQ_SCC1 :
    sccregs = &m8xx.scc1;
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

static rtems_irq_connect_data consoleIrqData;

void
m8xx_uart_scc_initialize (int minor)
{
  unsigned char brg;
  volatile m8xxSCCparms_t *sccparms = 0;
  volatile m8xxSCCRegisters_t *sccregs = 0;

  /*
   * Check that minor number is valid
   */
  if ( (minor < SCC2_MINOR) || (minor > NUM_PORTS-1) )
    return;

  /* Get the sicr clock source bit values for 9600 bps */
  brg = m8xx_get_brg_clk(9600);

  /*
   * Allocate buffer descriptors
   */
  RxBd[minor] = m8xx_bd_allocate(1);
  TxBd[minor] = m8xx_bd_allocate(1);

  /*
   *  Get the address of the parameter RAM for the specified port,
   *  configure I/O port A,C & D and put SMC in NMSI mode, connect
   *  the SCC to the appropriate BRG.
   *
   *  SCC2 TxD is shared with port A bit 12
   *  SCC2 RxD is shared with port A bit 13
   *  SCC1 TxD is shared with port A bit 14
   *  SCC1 RxD is shared with port A bit 15
   *  SCC4 DCD is shared with port C bit 4
   *  SCC4 CTS is shared with port C bit 5
   *  SCC3 DCD is shared with port C bit 6
   *  SCC3 CTS is shared with port C bit 7
   *  SCC2 DCD is shared with port C bit 8
   *  SCC2 CTS is shared with port C bit 9
   *  SCC1 DCD is shared with port C bit 10
   *  SCC1 CTS is shared with port C bit 11
   *  SCC2 RTS is shared with port C bit 14
   *  SCC1 RTS is shared with port C bit 15
   *  SCC4 RTS is shared with port D bit 6
   *  SCC3 RTS is shared with port D bit 7
   *  SCC4 TxD is shared with port D bit 8
   *  SCC4 RxD is shared with port D bit 9
   *  SCC3 TxD is shared with port D bit 10
   *  SCC3 RxD is shared with port D bit 11
   */
  switch (minor) {
    case SCC2_MINOR:
      sccparms = &m8xx.scc2p;
      sccregs = &m8xx.scc2;

      m8xx.papar |=  0x000C;        /* PA12 & PA13 are dedicated peripheral pins */
      m8xx.padir &= ~0x000C;        /* PA13 & PA12 must not drive the UART lines */
      m8xx.paodr &= ~0x000C;        /* PA12 & PA13 are not open drain */
      m8xx.pcpar |=  0x0002;        /* PC14 is SCC2 RTS */
      m8xx.pcpar &= ~0x00C0;        /* PC8 & PC9 are SCC2 DCD and CTS */
      m8xx.pcdir &= ~0x00C2;        /* PC8, PC9 & PC14 must not drive the UART lines */
      m8xx.pcso  |=  0x00C0;        /* Enable DCD and CTS inputs */

      m8xx.sicr &= 0xFFFF00FF;      /* Clear TCS2 & RCS2, GR2=no grant, SC2=NMSI mode */
      m8xx.sicr |= (brg<<11) | (brg<<8); /* TCS2 = RCS2 = brg */
      break;

#ifdef mpc860
    case SCC3_MINOR:
      sccparms = &m8xx.scc3p;
      sccregs = &m8xx.scc3;

      m8xx.pcpar &= ~0x0300;        /* PC6 & PC7 are SCC3 DCD and CTS */
      m8xx.pcdir &= ~0x0300;        /* PC6 & PC7 must not drive the UART lines */
      m8xx.pcso  |=  0x0300;        /* Enable DCD and CTS inputs */
      m8xx.pdpar |=  0x0130;        /* PD7, PD10 & PD11 are dedicated peripheral pins */

      m8xx.sicr &= 0xFF00FFFF;      /* Clear TCS3 & RCS3, GR3=no grant, SC3=NMSI mode */
      m8xx.sicr |= (brg<<19) | (brg<<16); /* TCS3 = RCS3 = brg */
      break;

    case SCC4_MINOR:
      sccparms = &m8xx.scc4p;
      sccregs = &m8xx.scc4;

      m8xx.pcpar &= ~0x0C00;        /* PC4 & PC5 are SCC4 DCD and CTS */
      m8xx.pcdir &= ~0x0C00;        /* PC4 & PC5 must not drive the UART lines */
      m8xx.pcso  |=  0x0C00;        /* Enable DCD and CTS inputs */
      m8xx.pdpar |=  0x02C0;        /* PD6, PD8 & PD9 are dedicated peripheral pins */

      m8xx.sicr &= 0x00FFFFFF;      /* Clear TCS4 & RCS4, GR4=no grant, SC4=NMSI mode */
      m8xx.sicr |= (brg<<27) | (brg<<24); /* TCS4 = RCS4 = brg */
      break;
#endif
  }

  /*
   *  Set up SDMA
   */
  m8xx.sdcr = 0x01;                 /* as per section 16.10.2.1 MPC821UM/AD */

  /*
   *  Set up the SCC parameter RAM.
   */
  sccparms->rbase = (char *)RxBd[minor] - (char *)&m8xx;
  sccparms->tbase = (char *)TxBd[minor] - (char *)&m8xx;

  sccparms->rfcr = M8xx_RFCR_MOT | M8xx_RFCR_DMA_SPACE(0);
  sccparms->tfcr = M8xx_TFCR_MOT | M8xx_TFCR_DMA_SPACE(0);
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
  RxBd[minor]->status = M8xx_BD_EMPTY | M8xx_BD_WRAP | M8xx_BD_INTERRUPT;
  RxBd[minor]->length = 0;
  RxBd[minor]->buffer = rxBuf[minor];

  /*
   * Setup the Transmit Buffer Descriptor
   */
  TxBd[minor]->status = M8xx_BD_WRAP;

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
    case SCC2_MINOR:
      m8xx_cp_execute_cmd (M8xx_CR_OP_INIT_RX_TX | M8xx_CR_CHAN_SCC2);
      break;

#ifdef mpc860
    case SCC3_MINOR:
      m8xx_cp_execute_cmd (M8xx_CR_OP_INIT_RX_TX | M8xx_CR_CHAN_SCC3);
      break;
    case SCC4_MINOR:
      m8xx_cp_execute_cmd (M8xx_CR_OP_INIT_RX_TX | M8xx_CR_CHAN_SCC4);
      break;
#endif
  }
  if ( (mbx8xx_console_get_configuration() & 0x06) == 0x02 ) {
    consoleIrqData.on = m8xx_scc_enable;
    consoleIrqData.off = m8xx_scc_disable;
    consoleIrqData.isOn = m8xx_scc_isOn;

    switch (minor) {
    case SCC2_MINOR:
      consoleIrqData.name = BSP_CPM_IRQ_SCC2;
      consoleIrqData.hdl = m8xx_scc2_interrupt_handler;
      break;

#ifdef mpc860
    case SCC3_MINOR:
      consoleIrqData.name = BSP_CPM_IRQ_SCC3;
      consoleIrqData.hdl = m8xx_scc3_interrupt_handler;
      break;

    case SCC4_MINOR:
      consoleIrqData.name = BSP_CPM_IRQ_SCC4;
      consoleIrqData.hdl = m8xx_scc4_interrupt_handler;
      break;
#endif /* mpc860 */
    }
    if (!BSP_install_rtems_irq_handler (&consoleIrqData)) {
        printk("Unable to connect SCC Irq handler\n");
	rtems_fatal_error_occurred(1);
    }
  }
}

void m8xx_smc_enable(const rtems_irq_connect_data* ptr)
{
  volatile m8xxSMCRegisters_t *smcregs = 0;
  switch (ptr->name) {
  case BSP_CPM_IRQ_SMC1 :
    smcregs = &m8xx.smc1;
    break;
  case BSP_CPM_IRQ_SMC2_OR_PIP :
    smcregs = &m8xx.smc2;
    break;
  default:
    break;
  }
  smcregs->smcm = 3;
}

void m8xx_smc_disable(const rtems_irq_connect_data* ptr)
{
  volatile m8xxSMCRegisters_t *smcregs = 0;
  switch (ptr->name) {
  case BSP_CPM_IRQ_SMC1 :
    smcregs = &m8xx.smc1;
    break;
  case BSP_CPM_IRQ_SMC2_OR_PIP :
    smcregs = &m8xx.smc2;
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
  volatile m8xxSMCparms_t *smcparms = 0;
  volatile m8xxSMCRegisters_t *smcregs = 0;

  /*
   * Check that minor number is valid
   */
  if ( (minor < SMC1_MINOR) || (minor > SMC2_MINOR) )
    return;

  m8xx.sdcr = 0x01;                 /* as per section 16.10.2.1 MPC821UM/AD */
  /* Get the simode clock source bit values for 9600 bps */
  brg = m8xx_get_brg_clk(9600);

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
      smcparms = &m8xx.smc1p;
      smcregs = &m8xx.smc1;

      m8xx.pbpar |=  0x000000C0;    /* PB24 & PB25 are dedicated peripheral pins */
      m8xx.pbdir &= ~0x000000C0;    /* PB24 & PB25 must not drive UART lines */
      m8xx.pbodr &= ~0x000000C0;    /* PB24 & PB25 are not open drain */

      m8xx.simode &= 0xFFFF0FFF;    /* Clear SMC1CS & SMC1 for NMSI mode */
      m8xx.simode |= brg << 12;     /* SMC1CS = brg */
      break;

    case SMC2_MINOR:
      smcparms = &m8xx.smc2p;
      smcregs = &m8xx.smc2;

      m8xx.pbpar |=  0x00000C00;    /* PB20 & PB21 are dedicated peripheral pins */
      m8xx.pbdir &= ~0x00000C00;    /* PB20 & PB21 must not drive the UART lines */
      m8xx.pbodr &= ~0x00000C00;    /* PB20 & PB21 are not open drain */

      m8xx.simode &= 0x0FFFFFFF;    /* Clear SMC2CS & SMC2 for NMSI mode */
      m8xx.simode |= brg << 28;     /* SMC2CS = brg */
      break;
  }

  /*
   * Set up SMC1 parameter RAM common to all protocols
   */
  smcparms->rbase = (char *)RxBd[minor] - (char *)&m8xx;
  smcparms->tbase = (char *)TxBd[minor] - (char *)&m8xx;
  smcparms->rfcr = M8xx_RFCR_MOT | M8xx_RFCR_DMA_SPACE(0);
  smcparms->tfcr = M8xx_TFCR_MOT | M8xx_TFCR_DMA_SPACE(0);
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
  RxBd[minor]->status = M8xx_BD_EMPTY | M8xx_BD_WRAP | M8xx_BD_INTERRUPT;
  RxBd[minor]->length = 0;
  RxBd[minor]->buffer = rxBuf[minor];

  /*
   * Setup the Transmit Buffer Descriptor
   */
  TxBd[minor]->status = M8xx_BD_WRAP;

  /*
   * Set up SMCx general and protocol-specific mode registers
   */
  smcregs->smce = ~0;               /* Clear any pending events */
  smcregs->smcm = 0;                /* Enable SMC Rx & Tx interrupts */
  smcregs->smcmr = M8xx_SMCMR_CLEN(9) | M8xx_SMCMR_SM_UART;

  /*
   * Send "Init parameters" command
   */
  switch (minor) {
    case SMC1_MINOR:
      m8xx_cp_execute_cmd (M8xx_CR_OP_INIT_RX_TX | M8xx_CR_CHAN_SMC1);
      break;

    case SMC2_MINOR:
      m8xx_cp_execute_cmd (M8xx_CR_OP_INIT_RX_TX | M8xx_CR_CHAN_SMC2);
      break;
  }

  /*
   * Enable receiver and transmitter
   */
  smcregs->smcmr |= M8xx_SMCMR_TEN | M8xx_SMCMR_REN;
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
	  consoleIrqData.name = BSP_CPM_IRQ_SMC2_OR_PIP;
	  consoleIrqData.hdl  = m8xx_smc2_interrupt_handler;
	  break;
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
  int i;

  for (i=0; i < 4; i++) {
    brg_spd[i] = 0;
    brg_used[i] = 0;
  }
}



int
m8xx_uart_pollRead(
  int minor
)
{
  unsigned char c;

  if (RxBd[minor]->status & M8xx_BD_EMPTY) {
    return -1;
  }
  rtems_cache_invalidate_multiple_data_lines(
    (const void *) RxBd[minor]->buffer,
    RxBd[minor]->length
  );
  c = ((char *)RxBd[minor]->buffer)[0];
  RxBd[minor]->status = M8xx_BD_EMPTY | M8xx_BD_WRAP;
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
  rtems_cache_flush_multiple_data_lines( buf, len );
  TxBd[minor]->buffer = (char *) buf;
  TxBd[minor]->length = len;
  TxBd[minor]->status = M8xx_BD_READY | M8xx_BD_WRAP | M8xx_BD_INTERRUPT;
  return 0;
}


ssize_t
m8xx_uart_pollWrite(
  int minor,
  const char *buf,
  size_t len
)
{
  while (len--) {
    while (TxBd[minor]->status & M8xx_BD_READY)
      continue;
    txBuf[minor] = *buf++;
    rtems_cache_flush_multiple_data_lines( (void *)&txBuf[minor], 1 );
    TxBd[minor]->buffer = &txBuf[minor];
    TxBd[minor]->length = 1;
    TxBd[minor]->status = M8xx_BD_READY | M8xx_BD_WRAP;
  }
  return len;
}
