/*
 *  General Serial I/O functions.
 *
 *  This file contains the functions for performing serial I/O.  The actual
 *  system calls (console_*) should be in the BSP part of the source tree.
 *  That way different BSPs can use whichever SCI they wish for /dev/console.
 *
 *  On-chip resources used:
 *   resource   minor                note
 *	SCI1	  0
 *	SCI2	  1
 *
 *
 *  MPC5xx port sponsored by Defence Research and Development Canada - Suffield
 *  Copyright (C) 2004, Real-Time Systems Inc. (querbach@realtime.bc.ca)
 *
 *  Derived from
 *    c/src/lib/libcpu/powerpc/mpc8xx/console_generic/console_generic.c:
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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/bspIo.h>   /* for printk */
#include <mpc5xx.h>
#include <mpc5xx/console.h>
#include <bsp/irq.h>


/*
 * SCI port descriptor table.
 */
typedef struct
{
  volatile m5xxSCIRegisters_t *regs;	/* hardware registers */
  struct rtems_termios_tty *ttyp;	/* termios data for this port */
} sci_desc;

static sci_desc sci_descs[] = {
  { &imb.qsmcm.sci1, 0 },		/* SCI 1 */
  { &imb.qsmcm.sci2, 0 },		/* SCI 2 */
};

/*
 * Number of SCI port initialization calls made so far.  Used to avoid
 * installing the common interrupt handler more than once.
 */
int init_calls = 0;

/*
 * Default configuration.
 */
static struct termios default_termios = {
  0, 					/* input mode flags */
  0, 					/* output mode flags */
  CS8 | CREAD | CLOCAL | B9600, 	/* control mode flags */
  0, 					/* local mode flags */
  0,					/* line discipline */
  { 0 }					/* control characters */
};


extern uint32_t bsp_clock_speed;

/*
 * Termios callback functions
 */

int
m5xx_uart_firstOpen(
  int major,
  int minor,
  void *arg
)
{
  rtems_libio_open_close_args_t *args = arg;
  sci_desc* desc = &sci_descs[minor];
  struct rtems_termios_tty *tty = args->iop->data1;

  desc->ttyp = tty;				/* connect tty */
  if ( tty->device.outputUsesInterrupts == TERMIOS_IRQ_DRIVEN)
    desc->regs->sccr1 |= QSMCM_SCI_RIE;		/* enable rx interrupt */

  return RTEMS_SUCCESSFUL;
}

int
m5xx_uart_lastClose(
  int major,
  int minor,
  void* arg
)
{
  sci_desc* desc = &sci_descs[minor];

  desc->regs->sccr1 &= ~(QSMCM_SCI_RIE | QSMCM_SCI_TIE);  /* disable all */
  desc->ttyp = NULL;					  /* disconnect tty */

  return RTEMS_SUCCESSFUL;
}

int
m5xx_uart_pollRead(
  int minor
)
{
  volatile m5xxSCIRegisters_t *regs = sci_descs[minor].regs;
  int c = -1;

  if ( regs ) {
    while ( (regs->scsr & QSMCM_SCI_RDRF) == 0 )
      ;
    c = regs->scdr;
  }

  return c;
}

ssize_t m5xx_uart_write(
  int minor,
  const char *buf,
  size_t len
)
{
  volatile m5xxSCIRegisters_t *regs = sci_descs[minor].regs;

  regs->scdr = *buf;			/* start transmission */
  regs->sccr1 |= QSMCM_SCI_TIE;		/* enable interrupt */
  return 0;
}

ssize_t m5xx_uart_pollWrite(
  int minor,
  const char *buf,
  size_t len
)
{
  volatile m5xxSCIRegisters_t *regs = sci_descs[minor].regs;
  size_t retval = len;

  while ( len-- ) {
    while ( (regs->scsr & QSMCM_SCI_TDRE) == 0 )
      ;
    regs->scdr = *buf++;
  }

  return retval;
}

int
m5xx_uart_setAttributes(
  int minor,
  const struct termios *t
)
{
  uint16_t sccr0 = sci_descs[minor].regs->sccr0;
  uint16_t sccr1 = sci_descs[minor].regs->sccr1;
  int baud;

  /*
   * Check that port number is valid
   */
  if ( (minor < SCI1_MINOR) || (minor > SCI2_MINOR) )
    return RTEMS_INVALID_NUMBER;

  /* Baud rate */
  baud = rtems_termios_baud_to_number( t->c_cflag & CBAUD );
  if (baud > 0) {
    sccr0 &= ~QSMCM_SCI_BAUD(-1);
    sccr0 |= QSMCM_SCI_BAUD((bsp_clock_speed + (16 * baud)) / (32 * baud));
  }

  /* Number of data bits -- not available with MPC5xx SCI */
  switch ( t->c_cflag & CSIZE ) {
    case CS5:     break;
    case CS6:     break;
    case CS7:     break;
    case CS8:     break;
  }

  /* Stop bits -- not easily available with MPC5xx SCI */
  if ( t->c_cflag & CSTOPB ) {
    /* Two stop bits */
  } else {
    /* One stop bit */
  }

  /* Parity */
  if ( t->c_cflag & PARENB )
    sccr1 |= QSMCM_SCI_PE;
  else
    sccr1 &= ~QSMCM_SCI_PE;

  if ( t->c_cflag & PARODD )
    sccr1 |= QSMCM_SCI_PT;
  else
    sccr1 &= ~QSMCM_SCI_PT;

  /* Transmitter and receiver enable */
  sccr1 |= QSMCM_SCI_TE;
  if ( t->c_cflag & CREAD )
    sccr1 |= QSMCM_SCI_RE;
  else
    sccr1 &= ~QSMCM_SCI_RE;

  /* Write hardware registers */
  sci_descs[minor].regs->sccr0 = sccr0;
  sci_descs[minor].regs->sccr1 = sccr1;

  return RTEMS_SUCCESSFUL;
}


/*
 * Interrupt handling.
 */
static void
m5xx_sci_interrupt_handler (rtems_irq_hdl_param unused)
{
  int minor;

  for ( minor = 0; minor < NUM_PORTS; minor++ ) {
    sci_desc *desc = &sci_descs[minor];
    int sccr1 = desc->regs->sccr1;
    int scsr = desc->regs->scsr;

    /*
     * Character received?
     */
    if ((sccr1 & QSMCM_SCI_RIE) && (scsr & QSMCM_SCI_RDRF)) {
      char c = desc->regs->scdr;
      rtems_termios_enqueue_raw_characters(desc->ttyp, &c, 1);
    }
    /*
     * Transmitter empty?
     */
    if ((sccr1 & QSMCM_SCI_TIE) && (scsr & QSMCM_SCI_TDRE)) {
      desc->regs->sccr1 &= ~QSMCM_SCI_TIE;
      rtems_termios_dequeue_characters (desc->ttyp, 1);
    }
  }
}

void m5xx_sci_nop(const rtems_irq_connect_data* ptr)
{
}

int m5xx_sci_isOn(const rtems_irq_connect_data* ptr)
{
  return 1;
}

/*
 * Basic initialization.
 */

void
m5xx_uart_initialize (int minor)
{
  /*
   * Check that minor number is valid.
   */
  if ( (minor < SCI1_MINOR) || (minor > SCI2_MINOR) )
    return;

  /*
   * Configure and enable receiver and transmitter.
   */
  m5xx_uart_setAttributes(minor, &default_termios);

  /*
   * Connect interrupt if not yet done.
   */
  if ( init_calls++ == 0 ) {
    rtems_irq_connect_data irq_data;

    irq_data.name = CPU_IRQ_SCI;
    irq_data.hdl  = m5xx_sci_interrupt_handler;
    irq_data.on   = m5xx_sci_nop;	/* can't enable both channels here */
    irq_data.off  = m5xx_sci_nop;	/* can't disable both channels here */
    irq_data.isOn = m5xx_sci_isOn;

    if (!CPU_install_rtems_irq_handler (&irq_data)) {
      printk("Unable to connect SCI Irq handler\n");
      rtems_fatal_error_occurred(1);
    }

    imb.qsmcm.qdsci_il = 		/* set interrupt level in port */
      QSMCM_ILDSCI(CPU_irq_level_from_symbolic_name(CPU_IRQ_SCI));
  }
}
