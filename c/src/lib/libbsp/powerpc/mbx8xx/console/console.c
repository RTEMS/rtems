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
#include <rtems/bspIo.h>
#include <rtems/libio.h>
#include <termios.h>
#include <bsp/mbx.h>

static int _EPPCBug_pollRead( int minor );
static ssize_t _EPPCBug_pollWrite( int minor, const char *buf, size_t len );
static void _BSP_output_char( char c );
static rtems_status_code do_poll_read( rtems_device_major_number major, rtems_device_minor_number minor, void * arg);
static rtems_status_code do_poll_write( rtems_device_major_number major, rtems_device_minor_number minor, void * arg);

static void _BSP_null_char( char c ) {return;}
static void serial_putchar(const char c);

BSP_output_char_function_type     BSP_output_char = _BSP_null_char;
BSP_polling_getchar_function_type BSP_poll_char = NULL;

extern volatile m8xx_t m8xx;
extern struct rtems_termios_tty *ttyp[];

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
static int _EPPCBug_pollRead(
  int minor
)
{
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
  __asm__ volatile( "li 10,0x202\n\
                 mr 3, %0\n\
                 mr 4, %1\n\
                 sc"
    :: "g" (&input_params), "g" (&output_params) : "3", "4", "10" );

  if ( (output_params.status == 0) && output_params.u.stat.input_char_available) {

    /* Read the char and return it */
    input_params.inbuf = &c;
    input_params.nbytes_requested = 1;

    __asm__ volatile( "li     10,0x200     /* Code for .CIO_READ */\n\
                   mr    3, %0         /* Address of input_params */\n\
                   mr    4, %1         /* Address of output_params */\n\
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
static ssize_t _EPPCBug_pollWrite(
  int minor,
  const char *buf,
  size_t len
)
{
  volatile int simask;
  int i;
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
      __asm__ volatile( "li 10,0x202        /* Code for .CIO_STAT */\n\
                     mr 3, %0           /* Address of input_params */\n\
                     mr 4, %1           /* Address of output_params */\n\
                     sc"            /* Call EPPCBUG */
        :: "g" (&input_params), "g" (&output_params) : "3", "4", "10" );

      if (output_params.status)
        goto error;
    } while (!output_params.u.stat.output_possible);

    /* Output the characters until done */
    input_params.outbuf = &buf[i];
    input_params.nbytes_to_output = len - i;

    __asm__ volatile( "li 10,0x201          /* Code for .CIO_WRITE */\n\
                   mr 3, %0             /* Address of input_params */\n\
                   mr 4, %1             /* Address of output_params */\n\
                   sc"                  /* Call EPPCBUG */
      :: "g" (&input_params), "g" (&output_params) : "3", "4", "10" );

    if (output_params.status)
      goto error;

    i += output_params.u.write.nbytes_sent;
  }

  /* Return something */
  m8xx.simask = simask;
  _ISR_Enable( level );
  return len;

error:
  m8xx.simask = simask;
  _ISR_Enable( level );
  return -1;
}

/*
 *  do_poll_read
 *
 *  Input characters through polled I/O. Returns has soon as a character has
 *  been received. Otherwise, if we wait for the number of requested characters,
 *  we could be here forever!
 *
 *  CR is converted to LF on input. The terminal should not send a CR/LF pair
 *  when the return or enter key is pressed.
 *
 *  Input parameters:
 *    major - ignored. Should be the major number for this driver.
 *    minor - selected channel.
 *    arg->buffer - where to put the received characters.
 *    arg->count  - number of characters to receive before returning--Ignored.
 *
 *  Output parameters:
 *    arg->bytes_moved - the number of characters read. Always 1.
 *
 *  Return value: RTEMS_SUCCESSFUL
 *
 *  CANNOT BE COMBINED WITH INTERRUPT DRIVEN I/O!
 */
static rtems_status_code do_poll_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_libio_rw_args_t *rw_args = arg;
  int c;

#if NVRAM_CONFIGURE == 1

  int (*pollRead)( int minor );

  if ( (nvram->console_mode & 0x06) == 0x04 )
    pollRead = _EPPCBug_pollRead;
  else
    pollRead = m8xx_uart_pollRead;

  while( (c = (*pollRead)(minor)) == -1 );
  rw_args->buffer[0] = (uint8_t)c;
  if( rw_args->buffer[0] == '\r' )
      rw_args->buffer[0] = '\n';
  rw_args->bytes_moved = 1;
  return RTEMS_SUCCESSFUL;

#else

#if UARTS_IO_MODE == 2
#define BSP_READ  _EPPCBug_pollRead
#else
#define BSP_READ  m8xx_uart_pollRead
#endif

  while( (c = BSP_READ(minor)) == -1 );
  rw_args->buffer[0] = (uint8_t)c;
  if( rw_args->buffer[0] == '\r' )
      rw_args->buffer[0] = '\n';
  rw_args->bytes_moved = 1;
  return RTEMS_SUCCESSFUL;

#endif
}

/*
 *  do_poll_write
 *
 *  Output characters through polled I/O. Returns only once every character has
 *  been sent.
 *
 *  CR is transmitted AFTER a LF on output.
 *
 *  Input parameters:
 *    major - ignored. Should be the major number for this driver.
 *    minor - selected channel
 *    arg->buffer - where to get the characters to transmit.
 *    arg->count  - the number of characters to transmit before returning.
 *
 *  Output parameters:
 *    arg->bytes_moved - the number of characters read
 *
 *  Return value: RTEMS_SUCCESSFUL
 *
 *  CANNOT BE COMBINED WITH INTERRUPT DRIVEN I/O!
 */
static rtems_status_code do_poll_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_libio_rw_args_t *rw_args = arg;
  uint32_t   i;
  char cr ='\r';

#if NVRAM_CONFIGURE == 1

  ssize_t (*pollWrite)(int minor, const char *buf, size_t len);

  if ( (nvram->console_mode & 0x06) == 0x04 )
    pollWrite = _EPPCBug_pollWrite;
  else
    pollWrite = m8xx_uart_pollWrite;

  for( i = 0; i < rw_args->count; i++ ) {
    (*pollWrite)(minor, &(rw_args->buffer[i]), 1);
    if ( rw_args->buffer[i] == '\n' )
      (*pollWrite)(minor, &cr, 1);
  }
  rw_args->bytes_moved = i;
  return RTEMS_SUCCESSFUL;

#else

#if UARTS_IO_MODE == 2
#define BSP_WRITE _EPPCBug_pollWrite
#else
#define BSP_WRITE m8xx_uart_pollWrite
#endif

  for( i = 0; i < rw_args->count; i++ ) {
    BSP_WRITE(minor, &(rw_args->buffer[i]), 1);
    if ( rw_args->buffer[i] == '\n' )
      BSP_WRITE(minor, &cr, 1);
  }
  rw_args->bytes_moved = i;
  return RTEMS_SUCCESSFUL;

#endif
}

/*
 *  Print functions prototyped in bspIo.h
 */

static void _BSP_output_char( char c )
{
  char cr = '\r';

  /*
   *  Can't rely on console_initialize having been called before this function
   *  is used, so it may fail unless output is done through EPPC-Bug.
   */
#if NVRAM_CONFIGURE == 1

  rtems_device_minor_number printk_minor;

  /* Use NVRAM info for configuration */
  printk_minor = (nvram->console_printk_port & 0x70) >> 4;
  if( (nvram->console_mode & 0x30) == 0x20 ) {
    _EPPCBug_pollWrite( printk_minor, &c, 1 );
    if( c == '\n' )
      _EPPCBug_pollWrite( printk_minor, &cr, 1 );
  }
  else {
    m8xx_uart_pollWrite( printk_minor, &c, 1 );
    if( c == '\n' )
      m8xx_uart_pollWrite( PRINTK_MINOR, &cr, 1 );
	}

#else

#if PRINTK_IO_MODE == 2
#define PRINTK_WRITE _EPPCBug_pollWrite
#else
#define PRINTK_WRITE m8xx_uart_pollWrite
#endif

  PRINTK_WRITE( PRINTK_MINOR, &c, 1 );
  if( c == '\n' )
    PRINTK_WRITE( PRINTK_MINOR, &cr, 1 );

#endif
}

bd_t *eppcbugInfo= (bd_t *)0xdeadbeef;
bd_t fakeEppcBugInfo = {
  	0x42444944,		/* Should be 0x42444944 "BDID" */
	sizeof(bd_t),		/* Size of this structure */
	0,			/* revision of this structure */
	0,			/* EPPCbug date, i.e. 0x11061997 */
	0,			/* Memory start address */
	0x1000000,		/* Memory (end) size in bytes */
	0x28,			/* Internal Freq, in Hz */
	0,			/* Bus Freq, in Hz */
	0,			/* Boot device controller */
	0			/* Boot device logical dev */
};

#define EARLY_CONSOLE
#ifdef EARLY_CONSOLE

#define MBX_CSR1	((volatile unsigned char *)0xfa100000)
#define CSR1_COMEN	(unsigned char)0x02
#define PROFF_CONS	PROFF_SMC1
#define CPM_CR_CH_CONS	CPM_CR_CH_SMC1
#define SMC_INDEX	0

#include <bsp/commproc.h>

static cpm8xx_t	*cpmp = (cpm8xx_t *)&(((immap_t *)IMAP_ADDR)->im_cpm);

void
serial_init(void)
{
	volatile smc_t		*sp;
	volatile smc_uart_t	*up;
	volatile cbd_t	*tbdf,  *rbdf;
	volatile cpm8xx_t	*cp;
	unsigned int		dpaddr, memaddr;
	bd_t	*bd;

#if NVRAM_CONFIGURE == 1
	if ( ((nvram->console_mode & 0x06) != 0x04 ) ||
	     ((nvram->console_mode & 0x30) != 0x20 )) {
	  /*
	   * FIXME: refine this condition...
	   */
#endif
	bd = eppcbugInfo;

	cp = cpmp;
	sp = (smc_t*)&(cp->cp_smc[SMC_INDEX]);
	up = (smc_uart_t *)&cp->cp_dparam[PROFF_CONS];

	/* Disable transmitter/receiver.
	*/
	sp->smc_smcmr &= ~(SMCMR_REN | SMCMR_TEN);

	if (*MBX_CSR1 & CSR1_COMEN) {
		/* COM1 is enabled.  Initialize SMC1 and use it for
		 * the console port.
		 */

		/* Enable SDMA.
		*/
		((immap_t *)IMAP_ADDR)->im_siu_conf.sc_sdcr = 1;

		/* Use Port B for SMCs instead of other functions.
		*/
		cp->cp_pbpar |= 0x00000cc0;
		cp->cp_pbdir &= ~0x00000cc0;
		cp->cp_pbodr &= ~0x00000cc0;

		/* Allocate space for two buffer descriptors in the DP ram.
		 * For now, this address seems OK, but it may have to
		 * change with newer versions of the firmware.
		 */
		dpaddr = 0x0800;

		/* Grab a few bytes from the top of memory.  EPPC-Bug isn't
		 * running any more, so we can do this.
		 */
		memaddr = (bd->bi_memsize - 32) & ~15;

		/* Set the physical address of the host memory buffers in
		 * the buffer descriptors.
		 */
		rbdf = (cbd_t *)&cp->cp_dpmem[dpaddr];
		rbdf->cbd_bufaddr = memaddr;
		rbdf->cbd_sc = 0;
		tbdf = rbdf + 1;
		tbdf->cbd_bufaddr = memaddr+4;
		tbdf->cbd_sc = 0;

		/* Set up the uart parameters in the parameter ram.
		*/
		up->smc_rbase = dpaddr;
		up->smc_tbase = dpaddr+sizeof(cbd_t);
		up->smc_rfcr = SMC_EB;
		up->smc_tfcr = SMC_EB;

		/* Set UART mode, 8 bit, no parity, one stop.
		 * Enable receive and transmit.
		 */
		sp->smc_smcmr = smcr_mk_clen(9) |  SMCMR_SM_UART;

		/* Mask all interrupts and remove anything pending.
		*/
		sp->smc_smcm = 0;
		sp->smc_smce = 0xff;

		/* Set up the baud rate generator.
		 * See 8xx_io/commproc.c for details.
		 */
		cp->cp_simode = 0x10000000;
		cp->cp_brgc1 =
			((((bd->bi_intfreq * 1000000)/16) / 9600) << 1) | CPM_BRG_EN;

		/* Enable SMC1 for console output.
		*/
		*MBX_CSR1 &= ~CSR1_COMEN;
	}
	else {
		/* SMCx is used as console port.
		*/
		tbdf = (cbd_t *)&cp->cp_dpmem[up->smc_tbase];
		rbdf = (cbd_t *)&cp->cp_dpmem[up->smc_rbase];

		/* Issue a stop transmit, and wait for it.
		*/
		cp->cp_cpcr = mk_cr_cmd(CPM_CR_CH_CONS,
					CPM_CR_STOP_TX) | CPM_CR_FLG;
		while (cp->cp_cpcr & CPM_CR_FLG);
	}

	/* Make the first buffer the only buffer.
	*/
	tbdf->cbd_sc |= BD_SC_WRAP;
	rbdf->cbd_sc |= BD_SC_EMPTY | BD_SC_WRAP;

	/* Single character receive.
	*/
	up->smc_mrblr = 1;
	up->smc_maxidl = 0;

	/* Initialize Tx/Rx parameters.
	*/
	cp->cp_cpcr = mk_cr_cmd(CPM_CR_CH_CONS, CPM_CR_INIT_TRX) | CPM_CR_FLG;
	while (cp->cp_cpcr & CPM_CR_FLG);

	/* Enable transmitter/receiver.
	*/
	sp->smc_smcmr |= SMCMR_REN | SMCMR_TEN;
#if NVRAM_CONFIGURE == 1
	}
	else {
	  const char bootmsg_text[]= "using EPPC bug for console I/O\n";
	  _EPPCBug_pollWrite((nvram->console_printk_port & 0x70) >> 4,
			     bootmsg_text,
			     sizeof(bootmsg_text)-1);
	}
#endif
#if NVRAM_CONFIGURE == 1
	if ((nvram->console_mode & 0x30) == 0x20 ) {
	  BSP_output_char = _BSP_output_char;
	}
	else {
	  BSP_output_char = serial_putchar;
	}
#else

	BSP_output_char = serial_putchar;
#endif
}

void
serial_putchar(const char c)
{
	volatile cbd_t		*tbdf;
	volatile char		*buf;
	volatile smc_uart_t	*up;

	up = (smc_uart_t *)&cpmp->cp_dparam[PROFF_CONS];
	tbdf = (cbd_t *)&cpmp->cp_dpmem[up->smc_tbase];

	/* Wait for last character to go.
	*/
	buf = (char *)tbdf->cbd_bufaddr;
	while (tbdf->cbd_sc & BD_SC_READY);

	*buf = c;
	tbdf->cbd_datlen = 1;
	tbdf->cbd_sc |= BD_SC_READY;
}

char
serial_getc(void)
{
	volatile cbd_t		*rbdf;
	volatile char		*buf;
	volatile smc_uart_t	*up;
	char			c;

	up = (smc_uart_t *)&cpmp->cp_dparam[PROFF_CONS];
	rbdf = (cbd_t *)&cpmp->cp_dpmem[up->smc_rbase];

	/* Wait for character to show up.
	*/
	buf = (char *)rbdf->cbd_bufaddr;
	while (rbdf->cbd_sc & BD_SC_EMPTY);
	c = *buf;
	rbdf->cbd_sc |= BD_SC_EMPTY;

	return(c);
}

int
serial_tstc(void)
{
	volatile cbd_t		*rbdf;
	volatile smc_uart_t	*up;

	up = (smc_uart_t *)&cpmp->cp_dparam[PROFF_CONS];
	rbdf = (cbd_t *)&cpmp->cp_dpmem[up->smc_rbase];

	return(!(rbdf->cbd_sc & BD_SC_EMPTY));
}

#endif
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

  /*
   * Set up TERMIOS if needed
   */
#if NVRAM_CONFIGURE == 1
  /* Use NVRAM info for configuration */
  console_minor = nvram->console_printk_port & 0x07;

  if ( nvram->console_mode & 0x01 )
    /* termios */
    rtems_termios_initialize ();

  /*
   *  Do common initialization.
   */
  m8xx_uart_initialize();

  /*
   * Do device-specific initialization
   */
  if ( !nvram->eppcbug_smc1 &&
    ( ((nvram->console_mode & 0x30) != 0x20) ||
     (((nvram->console_printk_port & 0x30) >> 4) != SMC1_MINOR) ) )
    m8xx_uart_smc_initialize(SMC1_MINOR); /* /dev/tty0 */

  if ( ((nvram->console_mode & 0x30) != 0x20) ||
      (((nvram->console_printk_port & 0x30) >> 4) != SMC2_MINOR) )
    m8xx_uart_smc_initialize(SMC2_MINOR); /* /dev/tty1 */

  if ( ((nvram->console_mode & 0x30) != 0x20) ||
      (((nvram->console_printk_port & 0x30) >> 4) != SCC2_MINOR) )
    m8xx_uart_scc_initialize(SCC2_MINOR); /* /dev/tty2    */

#ifdef mpc860

  if ( ((nvram->console_mode & 0x30) != 0x20) ||
      (((nvram->console_printk_port & 0x30) >> 4) != SCC3_MINOR) )
    m8xx_uart_scc_initialize(SCC3_MINOR); /* /dev/tty3    */

  if ( ((nvram->console_mode & 0x30) != 0x20) ||
      (((nvram->console_printk_port & 0x30) >> 4) != SCC4_MINOR) )
    m8xx_uart_scc_initialize(SCC4_MINOR); /* /dev/tty4    */

#endif /* mpc860 */
  BSP_output_char = _BSP_output_char;
#else /* NVRAM_CONFIGURE != 1 */

    console_minor = CONSOLE_MINOR;

#if UARTS_USE_TERMIOS == 1

    rtems_termios_initialize ();

#endif /* UARTS_USE_TERMIOS */

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

  BSP_output_char = _BSP_output_char;

#endif /* NVRAM_CONFIGURE != 1 */

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
  status = rtems_io_register_name ("/dev/console", major, console_minor);
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
#if NVRAM_CONFIGURE == 1
  /* Used to track termios private data for callbacks */
  rtems_libio_open_close_args_t *args = arg;
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
  static const rtems_termios_callbacks pollCallbacks = {
    NULL,                       	/* firstOpen */
    NULL,                       	/* lastClose */
    m8xx_uart_pollRead,        	        /* pollRead */
    m8xx_uart_pollWrite,       	        /* write */
    m8xx_uart_setAttributes,            /* setAttributes */
    NULL,                       	/* stopRemoteTx */
    NULL,                       	/* startRemoteTx */
    0                           	/* outputUsesInterrupts */
  };
  rtems_status_code sc;

#if (NVRAM_CONFIGURE == 1) || \
    ((NVRAM_CONFIGURE != 1) && (UARTS_USE_TERMIOS == 1) && \
      (UARTS_IO_MODE == 1))

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
#endif

  if ( minor > NUM_PORTS-1 )
    return RTEMS_INVALID_NUMBER;

#if NVRAM_CONFIGURE == 1

  /* Use NVRAM info for configuration */
  if ( nvram->console_mode & 0x01 ) {
    /* Use termios */
    if ( (nvram->console_mode & 0x06) == 0x02 ) {
      /* interrupt-driven I/O */
      sc = rtems_termios_open( major, minor, arg, &intrCallbacks );
      ttyp[minor] = args->iop->data1;        /* Keep cookie returned by termios_open */
      return sc;
    }
    else if ( (nvram->console_mode & 0x06) == 0x04 )
      /* polled I/O through EPPC-Bug, better be through SMC1 */
      return rtems_termios_open( major, minor, arg, &sccEPPCBugCallbacks );
    else
      /* normal polled I/O */
      return rtems_termios_open( major, minor, arg, &pollCallbacks );
  }
  else
    /* no termios -- default to polled I/O */
    return RTEMS_SUCCESSFUL;

#else /* NVRAM_CONFIGURE != 1 */

#if UARTS_USE_TERMIOS == 1

#if UARTS_IO_MODE == 2    /* EPPCBug polled I/O with termios */
  sc = rtems_termios_open( major, minor, arg, &sccEPPCBugCallbacks );
#elif UARTS_IO_MODE == 1  /* RTEMS interrupt-driven I/O with termios */
  sc = rtems_termios_open( major, minor, arg, &intrCallbacks );
  ttyp[minor] = args->iop->data1;        /* Keep cookie returned by termios_open */
#else                     /* RTEMS polled I/O with termios */
  sc = rtems_termios_open( major, minor, arg, &pollCallbacks );
#endif

#else /* UARTS_USE_TERMIOS != 1 */
  /* no termios -- default to polled I/O */
  sc = RTEMS_SUCCESSFUL;
#endif /* UARTS_USE_TERMIOS != 1 */

  return sc;

#endif /* NVRAM_CONFIGURE != 1 */
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

#if NVRAM_CONFIGURE == 1

  /* Use NVRAM info for configuration */
  if ( nvram->console_mode & 0x01 )
    /* use termios */
    return rtems_termios_close( arg );
  else
    /* no termios */
    return RTEMS_SUCCESSFUL;

#else /* NVRAM_CONFIGURE != 1 */

#if UARTS_USE_TERMIOS == 1
  return rtems_termios_close( arg );
#else
  return RTEMS_SUCCESSFUL;
#endif

#endif /* NVRAM_CONFIGURE != 1 */
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
  if ( minor > NUM_PORTS-1 )
    return RTEMS_INVALID_NUMBER;

#if NVRAM_CONFIGURE == 1

  /* Use NVRAM info for configuration */
  if ( nvram->console_mode & 0x01 )
    /* use termios */
    return rtems_termios_read( arg );
  else
    /* no termios -- default to polled */
    return do_poll_read( major, minor, arg );

#else

#if UARTS_USE_TERMIOS == 1
  return rtems_termios_read( arg );
#else
  return do_poll_read( major, minor, arg );
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
  if ( minor > NUM_PORTS-1 )
    return RTEMS_INVALID_NUMBER;

#if NVRAM_CONFIGURE == 1

  /* Use NVRAM info for configuration */
  if ( nvram->console_mode & 0x01 )
    /* use termios */
    return rtems_termios_write( arg );
  else
    /* no termios -- default to polled */
    return do_poll_write( major, minor, arg );

#else

#if UARTS_USE_TERMIOS == 1
  return rtems_termios_write( arg );
#else
    /* no termios -- default to polled */
  return do_poll_write( major, minor, arg );
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

#if NVRAM_CONFIGURE == 1

  /* Uuse NVRAM info for configuration */
  if ( nvram->console_mode & 0x01 )
    /* termios */
    return rtems_termios_ioctl( arg );
  else
    /* no termios -- default to polled */
    return RTEMS_SUCCESSFUL;

#else

#if UARTS_USE_TERMIOS == 1
  return rtems_termios_ioctl( arg );
#else
  return RTEMS_SUCCESSFUL;
#endif

#endif
}

/*
 *  Support routine for console-generic
 */

int mbx8xx_console_get_configuration(void)
{
#if NVRAM_CONFIGURE == 1
  return nvram->console_mode;
#else
#if UARTS_IO_MODE == 1
  return 0x02;
#else
  return 0;
#endif
#endif

}
