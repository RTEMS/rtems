/*
 *  console.c
 *
 *  This file contains the MBX8xx termios serial I/O package.
 *  Only asynchronous I/O is supported.
 *
 *  The SCCs and SMCs are assigned as follows
 *
 *   Channel     Device      Minor   Note
 *    SMC1      /dev/tty0      0
 *    SMC2      /dev/tty1      1
 *    SCC1                     2     N/A. Hardwired as ethernet port
 *    SCC2      /dev/tty2      3
 *    SCC3      /dev/tty3      4
 *    SCC4      /dev/tty4      5
 *
 *  All ports support termios. All I/O is interrupt-driven, unless EPPCBug
 *  is used to do the I/O. To use EPPCBug, define the EPPCBUG_SMC1
 *  manifest constant in the configuration file (mbx8xx.cfg). EPPCBug I/O
 *  is currently limited to the EPPCBug debug console. This is a limitation
 *  of firmware revision 1.1. Later firmware should be able to do I/O
 *  through any port.This code assumes that the EPPCBug console is the
 *  default: SMC1.
 * 
 *  TO RUN THE TESTS, USE POLLED I/O THROUGH EPPCBUG. Some tests play with
 *  the interrupt masks and turn off I/O. Those tests will hang with when
 *  interrupt-driven I/O is used.
 *  
 *  Set CONSOLE_MINOR to the appropriate device minor number in the
 *  config file. This allows the RTEMS application console to be different
 *  from the EPPBug debug console or the GDB stup I/O port.
 * 
 *  This driver handles all five available serial ports: it distinguishes
 *  the sub-devices using minor device numbers. It is not possible to have
 *  other protocols running on the other ports when this driver is used as
 *  currently written.
 *  
 *  Based on code (alloc860.c in eth_comm port) by
 *  Jay Monkman (jmonkman@frasca.com),
 *  Copyright (C) 1998 by Frasca International, Inc.
 *
 *  Modifications by Darlene Stewart <Darlene.Stewart@iit.nrc.ca>
 *  and Charles-Antoine Gauthier <charles.gauthier@iit.nrc.ca>.
 *  Copyright (c) 1999, National Research Council of Canada
 *
 */
#include <stdarg.h>
#include <stdio.h>
#include <bsp.h>                /* Must be before libio.h */
#include <rtems/libio.h>
#include <termios.h>

static int _EPPCBug_pollRead( int minor );
static int _EPPCBug_pollWrite( int minor, const char *buf, int len );


/*
 * _EPPCBug_pollRead
 *
 *  Read a character from the EPPCBug console, and return it. Return -1
 *  if there is no character in the input FIFO.
 *
 *  Input parameters:
 *    minor - selected channel
 *
 *  Output parameters:  NONE
 *
 *  Return value: char returned as positive signed int
 *                -1 if no character is present in the input FIFO.
 */
int _EPPCBug_pollRead(
  int minor
)
{
  extern volatile m8xx_t m8xx;

  char c;
  volatile int simask;		/* We must read and write m8xx.simask */
  int retval;
  ISR_Level level;
  
  struct {
    int clun;
    int dlun;
    char * inbuf;
    int nbytes_requested;
    int reserved;
  } volatile input_params;
  
  struct {
    int status;
    union {
      struct {
        int input_char_available;
        int output_possible;
        int break_detected;
        int modem_status;
      } stat;
      struct {
        int nbytes_received;
      } read;
    } u;
  } volatile output_params;

  retval = -1;

  /* Input through EPPCBug console  */
  input_params.clun = 0;
  input_params.dlun = 0;
  input_params.reserved = 0;
  
  _ISR_Disable( level );
  simask = m8xx.simask;

  /* Check for a char in the input FIFO using .CIO_STAT */
  asm volatile( "li 10,0x202
                 mr 3, %0
                 mr 4, %1
                 sc"
    :: "g" (&input_params), "g" (&output_params) : "3", "4", "10" );

  if ( (output_params.status == 0) && output_params.u.stat.input_char_available) {
  
    /* Read the char and return it */
    input_params.inbuf = &c;
    input_params.nbytes_requested = 1;
  
    asm volatile( "li     10,0x200     /* Code for .CIO_READ */
                   mr    3, %0         /* Address of input_params */
                   mr    4, %1         /* Address of output_params */
                   sc"             /* Call EPPCBUG */ 
      :: "g" (&input_params), "g" (&output_params) : "3", "4", "10" );

    if ( (output_params.status == 0) && output_params.u.read.nbytes_received)
      retval = (int)c;
  }
  
  m8xx.simask = simask;
  _ISR_Enable( level );
  return retval;
}


/*
 * _EPPCBug_pollWrite
 *
 *  Output buffer through EPPCBug. Returns only once every character has been
 *  sent (polled output).
 *
 *  Input parameters:
 *    minor - selected channel
 *    buf - output buffer
 *    len - number of chars to output
 *
 *  Output parameters:  NONE
 *
 *  Return value: IGNORED
 */
int _EPPCBug_pollWrite(
  int minor,
  const char *buf,
  int len
)
{
  extern volatile m8xx_t m8xx;

  volatile int simask;
  int i, retval;
  ISR_Level level;
  
  struct {
    int clun;
    int dlun;
    const char * outbuf;
    int nbytes_to_output;
    int reserved;
  } volatile input_params;
  
  struct {
    int status;
    union {
      struct {
        int input_char_available;
        int output_possible;
        int break_detected;
        int modem_status;
      } stat;
      struct {
        int nbytes_sent;
      } write;
    } u;
  } volatile output_params;

  retval = -1;

  /* Output through EPPCBug console  */
  input_params.clun = 0;
  input_params.dlun = 0;
  input_params.reserved = 0;

  i = 0;

  _ISR_Disable( level );
  simask = m8xx.simask;

  while (i < len) {
    /* Wait for space in the output buffer */
    do {
      /* Check for space in the output FIFO */
      asm volatile( "li 10,0x202        /* Code for .CIO_STAT */
                     mr 3, %0           /* Address of input_params */
                     mr 4, %1           /* Address of output_params */
                     sc"            /* Call EPPCBUG */ 
        :: "g" (&input_params), "g" (&output_params) : "3", "4", "10" );

      if (output_params.status)
        goto error;
    } while (!output_params.u.stat.output_possible);

    /* Output the characters until done */
    input_params.outbuf = &buf[i];
    input_params.nbytes_to_output = len - i;
  
    asm volatile( "li 10,0x201          /* Code for .CIO_WRITE */
                   mr 3, %0             /* Address of input_params */
                   mr 4, %1             /* Address of output_params */
                   sc"          /* Call EPPCBUG */ 
      :: "g" (&input_params), "g" (&output_params) : "3", "4", "10" );

    if (output_params.status)
      goto error;

    i += output_params.u.write.nbytes_sent;
  }
  
  /* Return something */
  m8xx.simask = simask;
  _ISR_Enable( level );
  return RTEMS_SUCCESSFUL;

error:
  m8xx.simask = simask;
  _ISR_Enable( level );
  return -1;
}


/*
 *  Print functions: prototyped in bsp.h
 *  Debug printing on Channel 1
 */
 
void printk( char *fmt, ... )
{
  va_list  ap;                  /* points to each unnamed argument in turn */
  static char buf[256];
  unsigned int level;
  
  _CPU_ISR_Disable(level);
  
  va_start(ap, fmt);            /* make ap point to 1st unnamed arg */
  vsprintf(buf, fmt, ap);       /* send output to buffer */
  
  BSP_output_string(buf);       /* print buffer -- Channel 1 */
  
  va_end(ap);               /* clean up and re-enable interrupts */
  _CPU_ISR_Enable(level);
}


void BSP_output_string( char * buf )
{
  int len = strlen(buf);
  int minor;                /* will be ignored */
  rtems_status_code sc;

  sc = _EPPCBug_pollWrite(minor, buf, len);
  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (sc);
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
  
  /*
   * Set up TERMIOS
   */
  rtems_termios_initialize();

  /*
   *  Do common initialization.
   */
  m8xx_uart_initialize();
  
  /*
   * Do device-specific initialization
   */
#ifndef EPPCBUG_SMC1
  m8xx_uart_smc_initialize(SMC1_MINOR); /* /dev/tty0 */
#endif /* EPPCBUG_SMC1 */

  m8xx_uart_smc_initialize(SMC2_MINOR); /* /dev/tty1 */                             
  m8xx_uart_scc_initialize(SCC2_MINOR); /* /dev/tty2    */
                              
#ifdef mpc860
  m8xx_uart_scc_initialize(SCC3_MINOR); /* /dev/tty3    */
  m8xx_uart_scc_initialize(SCC4_MINOR); /* /dev/tty4    */
#endif /* mpc860 */

  /*
   * Set up interrupts
   */
   m8xx_uart_interrupts_initialize();

  status = rtems_io_register_name ("/dev/tty0", major, SMC1_MINOR);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);
    
  status = rtems_io_register_name ("/dev/tty1", major, SMC2_MINOR);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);
    
  status = rtems_io_register_name ("/dev/tty2", major, SCC2_MINOR);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);
    
#ifdef mpc860
  status = rtems_io_register_name ("/dev/tty3", major, SCC3_MINOR);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);
                              
  status = rtems_io_register_name ("/dev/tty4", major, SCC4_MINOR);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);
    
#endif /* mpc860 */
    
  /* Now register the RTEMS console */
  status = rtems_io_register_name ("/dev/console", major, CONSOLE_MINOR);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);
    
  return RTEMS_SUCCESSFUL;
}


/*
 * Open the device
 */
rtems_device_driver console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  /* Used to track termios private data for callbacks */
  extern struct rtems_termios_tty *ttyp[];

  volatile m8xxSCCRegisters_t *sccregs;
  rtems_status_code sc;
  rtems_libio_open_close_args_t *args = arg;
  
#ifdef EPPCBUG_SMC1
  static const rtems_termios_callbacks sccEPPCBugCallbacks = {
    NULL,                       	/* firstOpen */
    NULL,                       	/* lastClose */
    _EPPCBug_pollRead,          	/* pollRead */
    _EPPCBug_pollWrite,         	/* write */
    NULL,                       	/* stopRemoteTx */
    NULL,                       	/* startRemoteTx */
    0                           	/* outputUsesInterrupts */
  };  
#endif

#ifdef UARTS_USE_INTERRUPTS  
  static const rtems_termios_callbacks intrCallbacks = {
    NULL,                       	/* firstOpen */
    NULL,                       	/* lastClose */
    NULL,                               /* pollRead */
    m8xx_uart_write,       	        /* write */
    m8xx_uart_setAttributes,    	/* setAttributes */
    NULL,                       	/* stopRemoteTx */
    NULL,                       	/* startRemoteTx */
    1                           	/* outputUsesInterrupts */
  };
#else
  static const rtems_termios_callbacks pollCallbacks = {
    NULL,                       	/* firstOpen */
    NULL,                       	/* lastClose */
    m8xx_uart_pollRead,        	        /* pollRead */
    m8xx_uart_pollWrite,       	        /* write */
    m8xx_uart_setAttributes,      	/* setAttributes */
    NULL,                       	/* stopRemoteTx */
    NULL,                       	/* startRemoteTx */
    0                           	/* outputUsesInterrupts */
  };
#endif

  if ( (minor < SMC1_MINOR) || (minor > NUM_PORTS-1) )
    return RTEMS_INVALID_NUMBER;
    
#ifdef EPPCBUG_SMC1
  if (minor == SMC1_MINOR)
    return rtems_termios_open (major, minor, arg, &sccEPPCBugCallbacks);
#endif /* EPPCBUG_SMC1 */

#ifdef UARTS_USE_INTERRUPTS  
  sc = rtems_termios_open (major, minor, arg, &intrCallbacks);
  ttyp[minor] = args->iop->data1;        /* Keep cookie returned by termios_open */
#else
  sc = rtems_termios_open (major, minor, arg, &pollCallbacks);
#endif
  return sc;
}


/*
 * Close the device
 */
rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  return rtems_termios_close (arg);
}


/*
 * Read from the device
 */
rtems_device_driver console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  return rtems_termios_read(arg);
}


/*
 * Write to the device
 */
rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  return rtems_termios_write(arg);
}


/*
 * Handle ioctl request.
 */
rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{ 
  return rtems_termios_ioctl (arg);
}

