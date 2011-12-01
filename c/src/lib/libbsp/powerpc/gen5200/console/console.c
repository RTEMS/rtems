/*===============================================================*\
| Project: RTEMS generic MPC5200 BSP                              |
+-----------------------------------------------------------------+
| Partially based on the code references which are named below.   |
| Adaptions, modifications, enhancements and any recent parts of  |
| the code are:                                                   |
|                    Copyright (c) 2005                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the console driver functions                 |
\*===============================================================*/
/***********************************************************************/
/*                                                                     */
/*   Module:       console.c                                           */
/*   Date:         07/17/2003                                          */
/*   Purpose:      RTEMS MPC5x00 console driver                        */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Description:                                                      */
/*                                                                     */
/*  The PSCs of mpc5200 are assigned as follows                        */
/*                                                                     */
/*              Channel     Device      Minor   Note                   */
/*                PSC1      /dev/tty0      0                           */
/*                PSC2      /dev/tty1      1                           */
/*                PSC3      /dev/tty2      2                           */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Code                                                              */
/*   References:   Serial driver for MPC8260ads                        */
/*   Module:       console-generic.c                                   */
/*   Project:      RTEMS 4.6.0pre1 / MPC8260ads BSP                    */
/*   Version       1.3                                                 */
/*   Date:         2002/11/04                                          */
/*                                                                     */
/*   Author(s) / Copyright(s):                                         */
/*                                                                     */
/*   Author: Jay Monkman (jmonkman@frasca.com)                         */
/*   Copyright (C) 1998 by Frasca International, Inc.                  */
/*                                                                     */
/*   Derived from c/src/lib/libbsp/m68k/gen360/console/console.c       */
/*   written by:                                                       */
/*   W. Eric Norum                                                     */
/*   Saskatchewan Accelerator Laboratory                               */
/*   University of Saskatchewan                                        */
/*   Saskatoon, Saskatchewan, CANADA                                   */
/*   eric@skatter.usask.ca                                             */
/*                                                                     */
/*   COPYRIGHT (c) 1989-2008.                                          */
/*   On-Line Applications Research Corporation (OAR).                  */
/*                                                                     */
/*   Modifications by Darlene Stewart <Darlene.Stewart@iit.nrc.ca>     */
/*   and Charles-Antoine Gauthier <charles.gauthier@iit.nrc.ca>        */
/*   Copyright (c) 1999, National Research Council of Canada           */
/*                                                                     */
/*   Modifications by Andy Dachs <a.dachs@sstl.co.uk> to add MPC8260   */
/*   support.                                                          */
/*   Copyright (c) 2001, Surrey Satellite Technology Ltd               */
/*                                                                     */
/*   The license and distribution terms for this file may be           */
/*   found in the file LICENSE in this distribution or at              */
/*   http://www.rtems.com/license/LICENSE.                        */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Partially based on the code references which are named above.     */
/*   Adaptions, modifications, enhancements and any recent parts of    */
/*   the code are under the right of                                   */
/*                                                                     */
/*         IPR Engineering, Dachauer Straße 38, D-80335 München        */
/*                        Copyright(C) 2003                            */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   IPR Engineering makes no representation or warranties with        */
/*   respect to the performance of this computer program, and          */
/*   specifically disclaims any responsibility for any damages,        */
/*   special or consequential, connected with the use of this program. */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Version history:  1.0                                             */
/*                                                                     */
/***********************************************************************/

#include <assert.h>

#include <rtems.h>
#include "../include/mpc5200.h"
#include <bsp.h>
#include <bsp/irq.h>

#include <rtems/bspIo.h>
#include <rtems/libio.h>
#include <string.h>
#include <rtems/termiostypes.h>


#define NUM_PORTS       MPC5200_PSC_NO

#define PSC1_MINOR      0
#define PSC2_MINOR      1
#define PSC3_MINOR      2
#define PSC4_MINOR      3
#define PSC5_MINOR      4
#define PSC6_MINOR      5

uint32_t mpc5200_uart_avail_mask = BSP_UART_AVAIL_MASK;

#if defined(UARTS_USE_TERMIOS_INT)
  uint8_t psc_minor_to_irqname[NUM_PORTS] = {
     BSP_SIU_IRQ_PSC1,
     BSP_SIU_IRQ_PSC2,
     BSP_SIU_IRQ_PSC3,
     BSP_SIU_IRQ_PSC4,
     BSP_SIU_IRQ_PSC5,
     BSP_SIU_IRQ_PSC6
  };

  static int mpc5200_psc_irqname_to_minor(int name) {
    int      minor;
    uint8_t *chrptr;

    chrptr = memchr(psc_minor_to_irqname, name, sizeof(psc_minor_to_irqname));
    if (chrptr != NULL) {
      minor = chrptr - psc_minor_to_irqname;
    } else {
      minor = -1;
    }
    return minor;
  }
#endif

static void A_BSP_output_char(char c);
static int A_BSP_get_char(void);
BSP_output_char_function_type BSP_output_char = A_BSP_output_char;

BSP_polling_getchar_function_type BSP_poll_char = A_BSP_get_char;

/* Used to handle premature outputs of printk */
bool console_initialized = false;

/* per channel info structure */
struct per_channel_info {
  uint16_t shadow_imr;
  uint8_t shadow_mode1;
  uint8_t shadow_mode2;
  int cur_tx_len;
  int rx_interrupts;
  int tx_interrupts;
  int rx_characters;
  int tx_characters;
  int breaks_detected;
  int framing_errors;
  int parity_errors;
  int overrun_errors;
};

/* Used to handle more than one channel */
struct per_channel_info channel_info[NUM_PORTS];

/*
 * XXX: there are only 6 PSCs, but PSC6 has an extra register gap
 *      from PSC5, therefore we instantiate seven(!) PSC register sets
 */
uint8_t psc_minor_to_regset[MPC5200_PSC_NO] = {0,1,2,3,4,6};

/* Used to track termios private data for callbacks */
struct rtems_termios_tty *ttyp[NUM_PORTS];

int mpc5200_psc_setAttributes(
  int                   minor,
  const struct termios *t
)
{
  int baud;
  uint8_t csize=0, cstopb, parenb, parodd;
  struct mpc5200_psc *psc =
    (struct mpc5200_psc *)(&mpc5200.psc[psc_minor_to_regset[minor]]);

  /* Baud rate */
  baud = rtems_termios_baud_to_number(t->c_cflag & CBAUD);
  if (baud > 0) {
   /*
    * Calculate baud rate
    * round divider to nearest!
    */
    baud = (IPB_CLOCK + baud *16) / (baud * 32);
  }

  /* Number of data bits */
  switch ( t->c_cflag & CSIZE ) {
    case CS5:     csize = 0x00;  break;
    case CS6:     csize = 0x01;  break;
    case CS7:     csize = 0x02;  break;
    case CS8:     csize = 0x03;  break;
  }

  /* Stop bits */
  if(csize == 0) {
    if(t->c_cflag & CSTOPB)
      cstopb = 0x0F;           /* Two stop bits */
    else
      cstopb = 0x00;           /* One stop bit */
  } else {
    if(t->c_cflag & CSTOPB)
      cstopb = 0x0F;           /* Two stop bits */
    else
      cstopb = 0x07;           /* One stop bit */
  }

  /* Parity */
  if (t->c_cflag & PARENB)
    parenb = 0x00;             /* Parity enabled on Tx and Rx */
  else
    parenb = 0x10;             /* No parity on Tx and Rx */

  if (t->c_cflag & PARODD)
    parodd = 0x04;             /* Odd parity */
  else
    parodd = 0x00;

 /*
  * Set upper timer counter
  */
  psc->ctur = (uint8_t) (baud >> 8);

 /*
  * Set lower timer counter
  */
  psc->ctlr = (uint8_t) baud;

 /*
  * Reset mode pointer
  */
  psc->cr = ((1 << 4) << 8);

 /*
  * Set mode1 register
  */
  channel_info[minor].shadow_mode1 &= ~(0x1F);
  psc->mr = channel_info[minor].shadow_mode1 | (csize | parenb | parodd);

 /*
  * Set mode2 register
  */
  channel_info[minor].shadow_mode2 &= ~(0x0F);
  psc->mr = channel_info[minor].shadow_mode2 | cstopb;

  return 0;

}


int mpc5200_uart_setAttributes(int minor, const struct termios *t)
{
  /*
   * Check that port number is valid
   */
  if( (minor < PSC1_MINOR) || (minor > NUM_PORTS-1) )
    return 0;

  return mpc5200_psc_setAttributes(minor, t);

}

#ifdef UARTS_USE_TERMIOS_INT
/*
 * Interrupt handlers
 */
static void mpc5200_psc_interrupt_handler(rtems_irq_hdl_param handle)
{
  unsigned char c;
  uint16_t isr;
  int minor = (int)handle;
  struct mpc5200_psc *psc =
    (struct mpc5200_psc *)(&mpc5200.psc[psc_minor_to_regset[minor]]);

  /*
   * get content of psc interrupt status
   */
  isr = psc->isr_imr;

  /*
   * Character received?
   */
  if (isr & ISR_RX_RDY_FULL) {

    channel_info[minor].rx_interrupts++;


#ifndef SINGLE_CHAR_MODE
    while(psc->rfnum) {
#endif
      /*
       * get the character
       */
       c = (psc->rb_tb >> 24);

      if (ttyp[minor] != NULL) {
        rtems_termios_enqueue_raw_characters(
           (void *)ttyp[minor], (char *)&c, (int)1);
        channel_info[minor].rx_characters++;
      }

#ifndef SINGLE_CHAR_MODE
    }
#endif

  }

  /*
   * Character transmitted ?
   */
  if (isr & ISR_TX_RDY & channel_info[minor].shadow_imr) {
    channel_info[minor].tx_interrupts++;

    /*
     * mask interrupt
     */
    psc->isr_imr = channel_info[minor].shadow_imr &= ~(IMR_TX_RDY);

    if (ttyp[minor] != NULL) {
      #ifndef SINGLE_CHAR_MODE
        rtems_termios_dequeue_characters(
           (void *)ttyp[minor], channel_info[minor].cur_tx_len);
        channel_info[minor].tx_characters += channel_info[minor].cur_tx_len;
      #else
        rtems_termios_dequeue_characters((void *)ttyp[minor], (int)1);
        channel_info[minor].tx_characters++;
      #endif
    }
  }

  if(isr & ISR_ERROR) {
    if(isr & ISR_RB)
      channel_info[minor].breaks_detected++;
    if(isr & ISR_FE)
      channel_info[minor].framing_errors++;
    if(isr & ISR_PE)
      channel_info[minor].parity_errors++;
    if(isr & ISR_OE)
      channel_info[minor].overrun_errors++;

    /*
     *  Reset error status
     */
    psc->cr = ((4 << 4) << 8);
  }
}

void mpc5200_psc_enable(
  const rtems_irq_connect_data* ptr
)
{
  struct mpc5200_psc *psc;
  int minor =  mpc5200_psc_irqname_to_minor(ptr->name);

  if (minor >= 0) {
    psc = (struct mpc5200_psc *)(&mpc5200.psc[psc_minor_to_regset[minor]]);
    psc->isr_imr = channel_info[minor].shadow_imr |=
      (IMR_RX_RDY_FULL | IMR_TX_RDY);
  }
}


void mpc5200_psc_disable(
  const rtems_irq_connect_data* ptr
)
{
  struct mpc5200_psc *psc;
  int minor =  mpc5200_psc_irqname_to_minor(ptr->name);

  if (minor >= 0) {
    psc = (struct mpc5200_psc *)(&mpc5200.psc[psc_minor_to_regset[minor]]);
    psc->isr_imr = channel_info[minor].shadow_imr &=
      ~(IMR_RX_RDY_FULL | IMR_TX_RDY);
  }
}

int mpc5200_psc_isOn(
  const rtems_irq_connect_data* ptr
)
{
  struct mpc5200_psc *psc;
  int minor =  mpc5200_psc_irqname_to_minor(ptr->name);

  if (minor >= 0) {
    psc = (struct mpc5200_psc *)(&mpc5200.psc[psc_minor_to_regset[minor]]);
    return ((psc->isr_imr & IMR_RX_RDY_FULL) & (psc->isr_imr & IMR_TX_RDY));
  }
  return false;
}


static rtems_irq_connect_data consoleIrqData;
#endif

void mpc5200_uart_psc_initialize(
  int minor
)
{
  uint32_t baud_divider;
  struct mpc5200_psc *psc =
    (struct mpc5200_psc *)(&mpc5200.psc[psc_minor_to_regset[minor]]);

  /*
   * Check that minor number is valid
   */
  if ((minor < PSC1_MINOR) || (minor >= (PSC1_MINOR + NUM_PORTS)))
    return;

  /*
   * Clear per channel info
   */
  memset((void *)&channel_info[minor], 0, sizeof(struct per_channel_info));

  /*
   * Reset receiver and transmitter
   */
  psc->cr = ((2 << 4) << 8);
  psc->cr = ((3 << 4) << 8);

  /*
   * Reset mode pointer
   */
  psc->cr = ((1 << 4) << 8);

  /*
   * Set clock select register
   */
  psc->sr_csr = 0;

  /*
   * Set mode1 register
   */
  psc->mr = channel_info[minor].shadow_mode1 = 0x33; /* 8Bit / no parity */

  /*
   * Set mode2 register
   */
  psc->mr = channel_info[minor].shadow_mode2 = 7; /* 1 stop bit */

  /*
   * Set rx FIFO alarm
   */
  psc->rfalarm = RX_FIFO_SIZE - 1;

  /*
   * Set tx FIFO alarm
   */
  psc->tfalarm = 1;

  baud_divider =
    (IPB_CLOCK + GEN5200_CONSOLE_BAUD *16) / (GEN5200_CONSOLE_BAUD * 32);

  /*
   * Set upper timer counter
   */
  psc->ctur = baud_divider >> 16;

  /*
   * Set lower timer counter
   */

  psc->ctlr = baud_divider & 0x0000ffff;

  /*
   * Disable Frame mode / set granularity 0
   */
  psc->tfcntl = 0;

#ifdef UARTS_USE_TERMIOS_INT
  /*
   * Tie interrupt dependent routines
   */
  consoleIrqData.on     = mpc5200_psc_enable;
  consoleIrqData.off    = mpc5200_psc_disable;
  consoleIrqData.isOn   = mpc5200_psc_isOn;
  consoleIrqData.handle = (rtems_irq_hdl_param)minor;
  consoleIrqData.hdl    = (rtems_irq_hdl)mpc5200_psc_interrupt_handler;

  /*
   * Tie interrupt handler
   */
  consoleIrqData.name = psc_minor_to_irqname[minor];

  /*
   * Install rtems irq handler
   */
  assert(BSP_install_rtems_irq_handler(&consoleIrqData) == 1);
#endif

  /*
   * Reset rx fifo errors Error/UF/OF
   */
  psc->rfstat |= 0x70;

  /*
   * Reset tx fifo errors Error/UF/OF
   */
  psc->tfstat |= 0x70;

#ifdef UARTS_USE_TERMIOS_INT
  /*
   * Unmask receive interrupt
   */
  psc->isr_imr = channel_info[minor].shadow_imr = IMR_RX_RDY_FULL;
#endif

  /*
   * Enable receiver
   */
  psc->cr = ((1 << 0) << 8);

  /*
   * Enable transmitter
   */
  psc->cr = ((1 << 2) << 8);
}


int mpc5200_uart_pollRead(
  int minor
)
{
  unsigned char c;
  struct mpc5200_psc *psc =
    (struct mpc5200_psc *)(&mpc5200.psc[psc_minor_to_regset[minor]]);

  if (psc->sr_csr & (1 << 8))
     c = (psc->rb_tb >> 24);
  else
     return -1;

  return c;
}


ssize_t mpc5200_uart_pollWrite(
  int minor,
  const char *buf,
  size_t len
)
{
  size_t retval = len;
  const char *tmp_buf = buf;
  struct mpc5200_psc *psc =
    (struct mpc5200_psc *)(&mpc5200.psc[psc_minor_to_regset[minor]]);

  while(len--) {
    while(!(psc->sr_csr & (1 << 11)))
      continue;

    /*rtems_cache_flush_multiple_data_lines( (void *)buf, 1);*/

    psc->rb_tb = (*tmp_buf << 24);

    tmp_buf++;

  }
  return retval;

}

ssize_t mpc5200_uart_write(
  int         minor,
  const char *buf,
  size_t len
)
{
  int frame_len = len;
  const char *frame_buf = buf;
  struct mpc5200_psc *psc =
    (struct mpc5200_psc *)(&mpc5200.psc[psc_minor_to_regset[minor]]);

 /*
  * Check tx fifo space
  */
  if(len > (TX_FIFO_SIZE - psc->tfnum))
    frame_len = TX_FIFO_SIZE - psc->tfnum;

#ifndef SINGLE_CHAR_MODE
  channel_info[minor].cur_tx_len = frame_len;
#else
  frame_len = 1;
#endif

 /*rtems_cache_flush_multiple_data_lines( (void *)frame_buf, frame_len);*/

  while (frame_len--)
    /* perform byte write to avoid extra NUL characters */
    (* (volatile char *)&(psc->rb_tb)) = *frame_buf++;

 /*
  * unmask interrupt
  */
  psc->isr_imr = channel_info[minor].shadow_imr |= IMR_TX_RDY;

  return 0;
}

/*
 *  Print functions prototyped in bspIo.h
 */
static void A_BSP_output_char(
  char c
)
{
  char cr = '\r';

  /*
   *  If we are using U-Boot, then the console is already initialized
   *  and we can just poll bytes out at any time.
   */
  #if !defined(HAS_UBOOT)
    if (console_initialized == false)
     return;
  #endif

#define PRINTK_WRITE mpc5200_uart_pollWrite

    PRINTK_WRITE(PRINTK_MINOR, &c, 1 );

    if( c == '\n' )
      PRINTK_WRITE( PRINTK_MINOR, &cr, 1 );
}

static int A_BSP_get_char(void)
{
  /*
   *  If we are using U-Boot, then the console is already initialized
   *  and we can just poll bytes in at any time.
   */
  #if !defined(HAS_UBOOT)
    if (console_initialized == false)
     return -1;
  #endif

  return mpc5200_uart_pollRead(0);
}

/*
 ***************
 * BOILERPLATE *
 ***************
 *
 *  All these functions are prototyped in rtems/c/src/lib/include/console.h.
 */

/*
 * Initialize and register the device
 */
rtems_device_driver console_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code status;
  rtems_device_minor_number console_minor;
  char dev_name[] = "/dev/ttyx";
  uint32_t tty_num = 0;
  bool first = true;

  /*
   * Always use and set up TERMIOS
   */
  console_minor = PSC1_MINOR;
  rtems_termios_initialize();

  for (console_minor = PSC1_MINOR;
       console_minor < PSC1_MINOR + NUM_PORTS;
       console_minor++) {
     /*
      * check, whether UART is available for this board
      */
    if (0 != ((1 << console_minor) & (mpc5200_uart_avail_mask))) {
      /*
       * Do device-specific initialization and registration for Motorola IceCube
       */
      mpc5200_uart_psc_initialize(console_minor); /* /dev/tty0 */
      dev_name[8] = '0' + tty_num;
      status = rtems_io_register_name (dev_name, major, console_minor);
      assert(status == RTEMS_SUCCESSFUL);

      #ifdef MPC5200_PSC_INDEX_FOR_GPS_MODULE
        if (console_minor == MPC5200_PSC_INDEX_FOR_GPS_MODULE) {
          status = rtems_io_register_name("/dev/gps", major, console_minor);
          assert(status == RTEMS_SUCCESSFUL);
        }
      #endif

      if (first) {
        first = false;

        /* Now register the RTEMS console */
        status = rtems_io_register_name ("/dev/console", major, console_minor);
        assert(status == RTEMS_SUCCESSFUL);
      }

      tty_num++;
    }
  }

  console_initialized = true;
  return RTEMS_SUCCESSFUL;
}

/*
 * Open the device
 */
rtems_device_driver console_open(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_libio_open_close_args_t *args = arg;
  rtems_status_code sc;

#ifdef UARTS_USE_TERMIOS_INT
  static const rtems_termios_callbacks intrCallbacks = {
    NULL,                           /* firstOpen */
    NULL,                           /* lastClose */
    NULL,                           /* pollRead */
    mpc5200_uart_write,             /* write */
    mpc5200_uart_setAttributes,     /* setAttributes */
    NULL,
    NULL,
    1                               /* outputUsesInterrupts */
  };
#else
  static const rtems_termios_callbacks pollCallbacks = {
    NULL,                           /* firstOpen */
    NULL,                           /* lastClose */
    mpc5200_uart_pollRead,          /* pollRead */
    mpc5200_uart_pollWrite,         /* write */
    mpc5200_uart_setAttributes,     /* setAttributes */
    NULL,
    NULL,
    0                               /* output don't use Interrupts */
  };
#endif

  if(minor > NUM_PORTS - 1)
    return RTEMS_INVALID_NUMBER;

#ifdef UARTS_USE_TERMIOS_INT
  sc = rtems_termios_open( major, minor, arg, &intrCallbacks );
#else                               /* RTEMS polled I/O with termios */
  sc = rtems_termios_open( major, minor, arg, &pollCallbacks );
#endif

  ttyp[minor] = args->iop->data1;   /* Keep cookie returned by termios_open */

  if ( !sc )
    rtems_termios_set_initial_baud( ttyp[minor], GEN5200_CONSOLE_BAUD );

  return sc;
}


/*
 * Close the device
 */
rtems_device_driver console_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  if ( minor > NUM_PORTS-1 )
    return RTEMS_INVALID_NUMBER;

  ttyp[minor] = NULL; /* mark for int handler: tty no longer open */

  return rtems_termios_close( arg );
}


/*
 * Read from the device
 */
rtems_device_driver console_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  if(minor > NUM_PORTS-1)
    return RTEMS_INVALID_NUMBER;

  return rtems_termios_read(arg);
}

/*
 * Write to the device
 */
rtems_device_driver console_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  if ( minor > NUM_PORTS-1 )
    return RTEMS_INVALID_NUMBER;
  return rtems_termios_write(arg);
}

/*
 * Handle ioctl request.
 */
rtems_device_driver console_control(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  if ( minor > NUM_PORTS-1 )
    return RTEMS_INVALID_NUMBER;

  return rtems_termios_ioctl(arg);
}
