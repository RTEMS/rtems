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
 *  All ports support termios. The use of termios is recommended for real-time
 *  applications. Termios provides buffering and input processing. When not
 *  using termios, processing is limited to the substitution of LF for CR on
 *  input, and the output of a CR following the output of a LF character.
 *  Note that the terminal should not send CR/LF pairs when the return key
 *  is pressed, and that output lines are terminated with LF/CR, not CR/LF
 *  (although that would be easy to change).
 *
 *  I/O may be interrupt-driven (recommended for real-time applications) or
 *  polled. Polled I/O may be performed by this device driver entirely, or
 *  in part by EPPCBug. With EPPCBug 1.1, polled I/O is limited to the
 *  EPPCBug debug console. This is a limitation of the firmware. Later
 *  firmware may be able to do I/O through any port. This code assumes
 *  that the EPPCBug console is the default: SMC1. If the console and
 *  printk ports are set to anything else with EPPCBug polled I/O, the
 *  system will hang. Only port SMC1 is usable with EPPCBug polled I/O.
 *
 *  LIMITATIONS:
 *
 *  It is not possible to use different I/O modes on the different ports. The
 *  exception is with printk. The printk port can use a different mode from
 *  the other ports. If this is done, it is important not to open the printk
 *  port from an RTEMS application.
 *
 *  Currently, the I/O modes are determined at build time. It would be much
 *  better to have the mode selected at boot time based on parameters in
 *  NVRAM.
 *
 *  Interrupt-driven I/O requires termios.
 *
 *  TESTS:
 * 
 *  TO RUN THE TESTS, USE POLLED I/O WITHOUT TERMIOS SUPPORT. Some tests
 *  play with the interrupt masks and turn off I/O. Those tests will hang
 *  when interrupt-driven I/O is used. Other tests, such as cdtest, do I/O
 *  from the static constructors before the console is open. This test
 *  will not work with interrupt-driven I/O. Because of the buffering
 *  performed in termios, test output may not be in sequence.The tests
 *  should all be fixed to work with interrupt-driven I/O and to
 *  produce output in the expected sequence. Obviously, the termios test
 *  requires termios support in the driver.
 *  
 *  Set CONSOLE_MINOR to the appropriate device minor number in the
 *  config file. This allows the RTEMS application console to be different
 *  from the EPPBug debug console or the GDB port.
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
 *  Copyright (c) 2000, National Research Council of Canada
 *
 */
#include <stdarg.h>
#include <stdio.h>
#include <bsp.h>                /* Must be before libio.h */
#include <bspIo.h>
#include <rtems/libio.h>
#include <termios.h>

#if UARTS_IO_MODE == 0
#define BSP_WRITE m8xx_uart_pollWrite
#define BSP_READ  m8xx_uart_pollRead
#elif UARTS_IO_MODE == 1
#define BSP_WRITE m8xx_uart_write
#elif UARTS_IO_MODE == 2
#define BSP_WRITE _EPPCBug_pollWrite
#define BSP_READ  _EPPCBug_pollRead
#endif

static int _EPPCBug_pollRead( int minor );
static int _EPPCBug_pollWrite( int minor, const char *buf, int len );
static void _BSP_output_char( char c );

BSP_output_char_function_type BSP_output_char = _BSP_output_char;


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

  input_params.clun = 0;
  input_params.dlun = 0;
  
  switch( minor ) {
    case SMC1_MINOR:    
      input_params.dlun = 0;  /* Should be 4, but doesn't work with EPPCBug 1.1 */
      break;
    case SMC2_MINOR:    
      input_params.dlun = 5;
      break;
    case SCC2_MINOR:    
      input_params.dlun = 1;
      break;
#ifdef mpc860
    case SCC3_MINOR:    
      input_params.dlun = 2;
      break;
    case SCC4_MINOR:    
      input_params.dlun = 3;
      break;
#endif
    default:    
      input_params.dlun = 0;
      break;
  }
  
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

  input_params.clun = 0;
  input_params.reserved = 0;
  
  switch( minor ) {
    case SMC1_MINOR:    
      input_params.dlun = 0;  /* Should be 4, but doesn't work with EPPCBug 1.1 */
      break;
    case SMC2_MINOR:    
      input_params.dlun = 5;
      break;
    case SCC2_MINOR:    
      input_params.dlun = 1;
      break;
#ifdef mpc860
    case SCC3_MINOR:    
      input_params.dlun = 2;
      break;
    case SCC4_MINOR:    
      input_params.dlun = 3;
      break;
#endif
    default:    
      input_params.dlun = 0;
      break;
  }

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
                   sc"                  /* Call EPPCBUG */ 
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
 *  Print functions prototyped in bspIo.h
 */

void _BSP_output_char( char c )
{
  char cr = '\r';
  
  BSP_WRITE( PRINTK_MINOR, &c, 1 );
  if( c == '\n' )
    BSP_WRITE( PRINTK_MINOR, &cr, 1 );
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
#if UARTS_USE_TERMIOS == 1
  rtems_termios_initialize();
#endif
  /*
   *  Do common initialization.
   */
  m8xx_uart_initialize();
  
  /*
   * Do device-specific initialization
   */
#if !defined(EPPCBUG_SMC1) && ( PRINTK_IO_MODE != 2 || PRINTK_MINOR != SMC1_MINOR )
  m8xx_uart_smc_initialize(SMC1_MINOR); /* /dev/tty0 */
#endif

#if PRINTK_IO_MODE != 2 || PRINTK_MINOR != SMC2_MINOR
  m8xx_uart_smc_initialize(SMC2_MINOR); /* /dev/tty1 */                             
#endif

  #if PRINTK_IO_MODE != 2 || PRINTK_MINOR != SCC2_MINOR
  m8xx_uart_scc_initialize(SCC2_MINOR); /* /dev/tty2    */
   #endif
                           
#ifdef mpc860

#if PRINTK_IO_MODE != 2 || PRINTK_MINOR != SCC3_MINOR
  m8xx_uart_scc_initialize(SCC3_MINOR); /* /dev/tty3    */
#endif

#if PRINTK_IO_MODE != 2 || PRINTK_MINOR != SCC4_MINOR
  m8xx_uart_scc_initialize(SCC4_MINOR); /* /dev/tty4    */
#endif

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
#if UARTS_IO_MODE == 1
  extern struct rtems_termios_tty *ttyp[];
  rtems_libio_open_close_args_t *args = arg;
#endif

  rtems_status_code sc;

#if UARTS_USE_TERMIOS == 1
  static const rtems_termios_callbacks sccEPPCBugCallbacks = {
    NULL,                       	/* firstOpen */
    NULL,                       	/* lastClose */
    _EPPCBug_pollRead,          	/* pollRead */
    _EPPCBug_pollWrite,         	/* write */
    NULL,                       	/* stopRemoteTx */
    NULL,                       	/* startRemoteTx */
    0                           	/* outputUsesInterrupts */
  };
  static const rtems_termios_callbacks intrCallbacks = {
    NULL,                       	/* firstOpen */
    NULL,                       	/* lastClose */
    NULL,                         /* pollRead */
    m8xx_uart_write,       	      /* write */
    m8xx_uart_setAttributes,    	/* setAttributes */
    NULL,                       	/* stopRemoteTx */
    NULL,                       	/* startRemoteTx */
    1                           	/* outputUsesInterrupts */
  };
  static const rtems_termios_callbacks pollCallbacks = {
    NULL,                       	/* firstOpen */
    NULL,                       	/* lastClose */
    m8xx_uart_pollRead,        	  /* pollRead */
    m8xx_uart_pollWrite,       	  /* write */
    m8xx_uart_setAttributes,      /* setAttributes */
    NULL,                       	/* stopRemoteTx */
    NULL,                       	/* startRemoteTx */
    0                           	/* outputUsesInterrupts */
  };
#endif
   
  if ( minor > NUM_PORTS-1 ) 
    return RTEMS_INVALID_NUMBER;

#if UARTS_USE_TERMIOS == 1

#if UARTS_IO_MODE == 2    /* EPPCBug polled I/O with termios */

  sc = rtems_termios_open (major, minor, arg, &sccEPPCBugCallbacks);
  
#elif UARTS_IO_MODE == 1  /* RTEMS interrupt-driven I/O with termios */

  sc = rtems_termios_open (major, minor, arg, &intrCallbacks);
  ttyp[minor] = args->iop->data1;        /* Keep cookie returned by termios_open */
  
#else                     /* RTEMS polled I/O with termios */

  sc = rtems_termios_open (major, minor, arg, &pollCallbacks);
  
#endif

#else                     /* Nothing to do if termios is not used */

  sc = RTEMS_SUCCESSFUL;
  
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
  if ( minor > NUM_PORTS-1 )
    return RTEMS_INVALID_NUMBER;

#if UARTS_USE_TERMIOS == 1
  return rtems_termios_close (arg);
#else
  return RTEMS_SUCCESSFUL;
#endif
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
#if UARTS_USE_TERMIOS != 1
  rtems_libio_rw_args_t *rw_args = arg;
  int i;
#endif
  
  if ( minor > NUM_PORTS-1 )
    return RTEMS_INVALID_NUMBER;

#if UARTS_USE_TERMIOS == 1

  return rtems_termios_read(arg);
  
#else

#if UARTS_IO_MODE != 1    /* Polled I/O without termios */

  for( i = 0; i < rw_args->count; i++ ) {
    rw_args->buffer[i] = BSP_READ( minor );
    if( rw_args->buffer[i] == '\r' )
      rw_args->buffer[i] = '\n';
  }
  rw_args->bytes_moved = i;
  return RTEMS_SUCCESSFUL;
  
#else                     /* RTEMS interrupt-driven I/O without termios */

  #error "Interrupt-driven input without termios is not yet supported"
  
#endif

#endif
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
#if UARTS_USE_TERMIOS != 1
  rtems_libio_rw_args_t *rw_args = arg;
  int i;
  char cr = '\r';
#endif

  if ( minor > NUM_PORTS-1 )
    return RTEMS_INVALID_NUMBER;

#if UARTS_USE_TERMIOS == 1

  return rtems_termios_write(arg);
  
#else

#if UARTS_IO_MODE != 1     /* Polled I/O without termios*/

  /* Must add carriage return to line feeds */
  for( i = 0; i < rw_args->count; i++ ) {
    BSP_WRITE( minor, &(rw_args->buffer[i]), 1 );
    if( rw_args->buffer[i] == '\n' )
      BSP_WRITE( minor, &cr, 1 );
  }
  rw_args->bytes_moved = rw_args->count;
  return RTEMS_SUCCESSFUL;
    
#else                     /* RTEMS interrupt-driven I/O without termios */

  #error "Interrupt-driven output without termios is not yet supported"
  
#endif

#endif
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
  if ( minor > NUM_PORTS-1 )
    return RTEMS_INVALID_NUMBER;

#if UARTS_USE_TERMIOS == 1
  return rtems_termios_ioctl (arg);
#else
  return RTEMS_SUCCESSFUL;
#endif
}

