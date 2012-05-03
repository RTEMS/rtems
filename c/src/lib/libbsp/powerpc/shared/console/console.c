/*
 *  console.c  -- console I/O package
 *
 *  Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 * This code is based on the pc386 BSP console.c so the following
 * copyright also applies :
 *
 * (C) Copyright 1997 -
 * - NavIST Group - Real-Time Distributed Systems and Industrial Automation
 *
 * Till Straumann, <strauman@slac.stanford.edu>, 12/20/2001
 * separate BSP specific stuff from generics...
 *
 * http://pandora.ist.utl.pt
 *
 * Instituto Superior Tecnico * Lisboa * PORTUGAL
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <stdlib.h>
#include <assert.h>
#include <stdlib.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/bspIo.h>
#include <rtems/libio.h>
#include <rtems/termiostypes.h>
#include <termios.h>
#include <bsp/uart.h>
#include <rtems/bspIo.h>  /* printk */

/* Definitions for BSPConsolePort */
/*
 * Possible value for console input/output :
 *    BSP_CONSOLE_PORT_CONSOLE
 *    BSP_UART_COM1
 *    BSP_UART_COM2
 */
int BSPConsolePort = BSP_CONSOLE_PORT;

int BSPBaseBaud    = BSP_UART_BAUD_BASE;

/*
 * TERMIOS_OUTPUT_MODE should be a 'bspopts.h/configure'-able option;
 * we could even make it a link-time option (but that would require
 * small changes)...
 */
#ifndef TERMIOS_OUTPUT_MODE
  #if 1
    #define TERMIOS_OUTPUT_MODE TERMIOS_IRQ_DRIVEN
  #else
    #define TERMIOS_OUTPUT_MODE TERMIOS_TASK_DRIVEN
  #endif
#endif

#if ! defined(USE_POLLED_IO) && (TERMIOS_OUTPUT_MODE == TERMIOS_POLLED)
  #define USE_POLLED_IO
#endif

/*-------------------------------------------------------------------------+
| External Prototypes
+--------------------------------------------------------------------------*/

static int  conSetAttr(int minor, const struct termios *);

typedef struct TtySTblRec_ {
  char          *name;
  rtems_irq_hdl  isr;
} TtySTblRec, *TtySTbl;

static TtySTblRec ttyS[]={
    { "/dev/ttyS0",
#ifdef BSP_UART_IOBASE_COM1
      BSP_uart_termios_isr_com1
#else
      0
#endif
    },
    { "/dev/ttyS1",
#ifdef BSP_UART_IOBASE_COM2
      BSP_uart_termios_isr_com2
#else
      0
#endif
    },
};

/*-------------------------------------------------------------------------+
| Console device driver INITIALIZE entry point.
+--------------------------------------------------------------------------+
| Initilizes the I/O console (keyboard + VGA display) driver.
+--------------------------------------------------------------------------*/
rtems_device_driver console_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                      *arg
)
{
  rtems_status_code status;

  /*
   *  The video was initialized in the start.s code and does not need
   *  to be reinitialized.
   */

  /*
   * Set up TERMIOS
   */
  rtems_termios_initialize();

  /*
   * Do device-specific initialization
   */

  /* RTEMS calls this routine once with 'minor'==0; loop through
   * all known instances...
   */

  for (minor=0; minor < sizeof(ttyS)/sizeof(ttyS[0]); minor++) {
    char *nm;
    /*
     * Skip ports (possibly not supported by BSP...) we have no ISR for
     */
    if ( ! ttyS[minor].isr )
      continue;
    /*
     * Register the device
     */
    status = rtems_io_register_name ((nm=ttyS[minor].name), major, minor);
    if ( RTEMS_SUCCESSFUL==status && BSPConsolePort == minor) {
      printk("Registering /dev/console as minor %d (==%s)\n",
              minor,
              ttyS[minor].name);
      /* also register an alias */
      status = rtems_io_register_name ( (nm="/dev/console"), major, minor);
    }

    if (status != RTEMS_SUCCESSFUL) {
      printk("Error registering %s!\n",nm);
      rtems_fatal_error_occurred (status);
    }
  }

  return RTEMS_SUCCESSFUL;
} /* console_initialize */

static int console_first_open(int major, int minor, void *arg)
{
  rtems_status_code status;

  /* must not open a minor device we have no ISR for */
  assert( minor>=0 && minor < sizeof(ttyS)/sizeof(ttyS[0]) && ttyS[minor].isr );

  /* 9600-8-N-1 */
  BSP_uart_init(minor, 9600, 0);
  status = BSP_uart_install_isr(minor, ttyS[minor].isr);
  if (!status) {
    printk("Error installing serial console interrupt handler for '%s'!\n",
      ttyS[minor].name);
    rtems_fatal_error_occurred(status);
  }

  /*
   * Pass data area info down to driver
   */
  BSP_uart_termios_set(minor, ((rtems_libio_open_close_args_t *)arg)->iop->data1);

  /* Enable interrupts  on channel */
  BSP_uart_intr_ctrl(minor, BSP_UART_INTR_CTRL_TERMIOS);

  return 0;
}

static int console_last_close(int major, int minor, void *arg)
{
  BSP_uart_remove_isr(minor, ttyS[minor].isr);
  return 0;
}

/*-------------------------------------------------------------------------+
| Console device driver OPEN entry point
+--------------------------------------------------------------------------*/
rtems_device_driver console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                      *arg
)
{
  rtems_status_code              status;
  static rtems_termios_callbacks cb =
#if defined(USE_POLLED_IO)
  {
     NULL,                              /* firstOpen */
     NULL,                              /* lastClose */
     NULL,                              /* pollRead */
     BSP_uart_termios_write_polled,     /* write */
     conSetAttr,                        /* setAttributes */
     NULL,                              /* stopRemoteTx */
     NULL,                              /* startRemoteTx */
     TERMIOS_POLLED                     /* outputUsesInterrupts */
  };
#else
  {
     console_first_open,                /* firstOpen */
     console_last_close,                /* lastClose */
#if ( TERMIOS_OUTPUT_MODE == TERMIOS_TASK_DRIVEN )
     BSP_uart_termios_read_com,         /* pollRead */
#else
     NULL,                              /* pollRead */
#endif
     BSP_uart_termios_write_com,        /* write */
     conSetAttr,                        /* setAttributes */
     NULL,                              /* stopRemoteTx */
     NULL,                              /* startRemoteTx */
     TERMIOS_OUTPUT_MODE                /* outputUsesInterrupts */
  };
#endif

  status = rtems_termios_open (major, minor, arg, &cb);

  if (status != RTEMS_SUCCESSFUL) {
    printk("Error opening console device\n");
    return status;
  }

  return RTEMS_SUCCESSFUL;
}

/*-------------------------------------------------------------------------+
| Console device driver CLOSE entry point
+--------------------------------------------------------------------------*/
rtems_device_driver
console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                      *arg
)
{
  rtems_device_driver res = RTEMS_SUCCESSFUL;

  res =  rtems_termios_close (arg);

  return res;
} /* console_close */

/*-------------------------------------------------------------------------+
| Console device driver READ entry point.
+--------------------------------------------------------------------------+
| Read characters from the I/O console. We only have stdin.
+--------------------------------------------------------------------------*/
rtems_device_driver console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                      *arg
)
{
  return rtems_termios_read (arg);
} /* console_read */

/*-------------------------------------------------------------------------+
| Console device driver WRITE entry point.
+--------------------------------------------------------------------------+
| Write characters to the I/O console. Stderr and stdout are the same.
+--------------------------------------------------------------------------*/
rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                      *arg
)
{
  return rtems_termios_write (arg);
} /* console_write */

/*
 * Handle ioctl request.
 */
rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                      *arg
)
{
/* does the BSP support break callbacks ? */
#if defined(BIOCSETBREAKCB) && defined(BIOCGETBREAKCB)
  rtems_libio_ioctl_args_t  *ioa=arg;
  switch (ioa->command) {
    case BIOCSETBREAKCB: return BSP_uart_set_break_cb(minor, ioa);
    case BIOCGETBREAKCB: return BSP_uart_get_break_cb(minor, ioa);
    default:             break;
  }
#endif
  return rtems_termios_ioctl (arg);
}

static int conSetAttr(
  int                   minor,
  const struct termios *t
)
{
  rtems_termios_baud_t baud;

  baud = rtems_termios_baud_to_number(t->c_cflag & CBAUD);
  if ( baud > 115200 )
    rtems_fatal_error_occurred (RTEMS_INTERNAL_ERROR);

  BSP_uart_set_baud(minor, baud);

  return 0;
}
