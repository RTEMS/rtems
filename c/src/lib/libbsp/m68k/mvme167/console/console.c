/*
 *  console.c
 *
 *  This file contains the MVME167 termios console package. Only asynchronous
 *  I/O is supported. Normal I/O uses DMA for output, interrupts for input.
 *  Very limited support is provided for polled I/O. Polled I/O is intended
 *  only for running the RTEMS test suites, and uses the 167Bug console only.
 *
 *  /dev/tty0 is channel 0, Serial Port 1/Console on the MVME712M.
 *  /dev/tty1 is channel 1, Serial Port 2/TTY01 on the MVME712M.
 *  /dev/tty2 is channel 2, Serial Port 3 on the MVME712M.
 *  /dev/tty3 is channel 3, Serial Port 4 on the MVME712M.
 *
 *  /dev/console is fixed to be /dev/tty01, Serial Port 2. 167Bug is given
 *  Serial Port 1/Console. Do not open /dev/tty00.
 *
 *  Modern I/O chips often contain a number of I/O devices that can operate
 *  almost independently of each other. Typically, in RTEMS, all devices in
 *  an I/O chip are handled by a single device driver, but that need not be
 *  always the case. Each device driver must supply six entry  points in the
 *  Device Driver Table: a device initialization function, as well as an open,
 *  close, read, write and a control function. RTEMS assigns a device major
 *  number to each device driver. This major device number is the index of the
 *  device driver entries in the Device Driver Table, and it used to identify
 *  a particular device driver. To distinguish multiple I/O sub-devices within
 *  an I/O chip, RTEMS supports device minor numbers. When a I/O device is
 *  initialized, the major number is supplied to the initialization function.
 *  That function must register each sub-device with a separate name and minor
 *  number (as well as the supplied major number). When an application opens a
 *  device by name, the corresponding major and minor numbers are returned to
 *  the caller to be used in subsequent I/O operations (although these details
 *  are typically hidden within the library functions).
 *
 *  Such a scheme recognizes that the initialization of the individual
 *  sub-devices is generally not completely independent. For example, the
 *  four serial ports of the CD2401 can be configured almost independently
 *  from each other. One port could be configured to operate in asynchronous
 *  mode with interrupt-driven I/O, while another port could be configured to
 *  operate in HDLC mode with DMA I/O. However, a device reset command will
 *  reset all four channels, and the width of DMA transfers and the number of
 *  retries following bus errors selected applies to all four channels.
 *  Consequently, when initializing one channel, one must be careful not to
 *  destroy the configuration of other channels that are already configured.
 *
 *  One problem with the RTEMS I/O initialization model is that no information
 *  other than a device major number is passed to the initialization function.
 *  Consequently, the sub-devices must be initialized with some pre-determined
 *  configuration. To change the configuration of a sub-device, it is
 *  necessary to either rewrite the initialization function, or to make a
 *  series of rtems_io_control() calls after initialization. The first
 *  approach is not very elegant. The second approach is acceptable if an
 *  application is simply changing baud rates, parity or other such
 *  asynchronous parameters (as supplied by the termios package). But what if
 *  an application requires one channel to run in HDLC or Bisync mode and
 *  another in async mode? With a single driver per I/O chip approach, the
 *  device driver must support multiple protocols. This is feasible, but it
 *  often means that an application that only does asynchronous I/O now links
 *  in code for other unused protocols, thus wasting precious ROM space.
 *  Worse, it requires that the sub-devices be initialized in some
 *  configuration, and that configuration then changed through a series of
 *  device driver control calls. There is no standard API in RTEMS to switch
 *   a serial line to some synchronous protocol.
 *
 *  A better approach is to treat each channel as a separate device, each with
 *  its own device device driver. The application then supplies its own device
 *  driver table with only the required protocols (drivers) on each line. The
 *  problem with this approach is that the device drivers are not really
 *  independent, given that the I/O sub-devices within a common chip are not
 *  independent themselves. Consequently, the related device drivers must
 *  share some information. In RTEMS, there is no standard location in which
 *  to share information.
 *
 *  This driver handles all four channels, i.e. it distinguishes the
 *  sub-devices using minor device numbers. Only asynchronous I/O is
 *  supported. The console is currently fixed to be channel 1 on the CD2401,
 *  which corresponds to the TTY01 port (Serial Port 2) on the MVME712M
 *  Transition Module.
 *
 *  The CD2401 does either interrupt-driven or DMA I/O; it does not support
 *  polling. In interrupt-driven or DMA I/O modes, interrupts from the CD2401
 *  are routed to the MC68040, and the processor generates an interrupt
 *  acknowledge cycle directly to the CD2401 to obtain an interrupt vector.
 *  The PCCchip2 supports a pseudo-polling mode in which interrupts from the
 *  CD2401 are not routed to the MC68040, but can be detected by the processor
 *  by reading the appropriate CD2401 registers. In this mode, interrupt
 *  acknowledge cycles must be generated to the CD2401 by reading the
 *  appropriate PCCchip2 registers.
 *
 *  Interrupts from the four channels cannot be routed independently; either
 *  all channels are used in the pseudo-polling mode, or all channels are used
 *  in interrupt-driven/DMA mode. There is no advantage in using the speudo-
 *  polling mode. Consenquently, this driver performs DMA input and output.
 *  Output is performed directly from the termios raw output buffer, while
 *  input is accumulated into a separate buffer.
 *
 *  THIS MODULE IS NOT RE-ENTRANT! Simultaneous access to a device from
 *  multiple tasks is likely to cause significant problems! Concurrency
 *  control is implemented in the termios package.
*
 *  THE INTERRUPT LEVEL IS SET TO 1 FOR ALL CHANNELS.
 *  If the CD2401 is to be used for high speed synchronous serial I/O, the
 *  interrupt priority might need to be increased.
 *
 *  ALL INTERRUPT HANDLERS ARE SHARED.
 *  When adding extra device drivers, either rewrite the interrupt handlers
 *  to demultiplex the interrupts, or install separate vectors. Common vectors
 *  are currently used to catch spurious interrupts. We could already have
 *  installed separate vectors for each channel and used the spurious
 *  interrupt handler defined in some other BSPs, but handling spurious
 *  interrupts from the CD2401 in this device driver allows us to record more
 *  information on the source of the interrupts. Furthermore, we have observed
 *  the occasional spurious interrupt from channel 0. We definitely do not
 *  to call a debugger for those.
 *
 *  All page references are to the MVME166/MVME167/MVME187 Single Board
 *  Computer Programmer's Reference Guide (MVME187PG/D2) with the April
 *  1993 supplements/addenda (MVME187PG/D2A1).
 *
 *  Copyright (c) 1998, National Research Council of Canada
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 */

#define M167_INIT

#include <termios.h>
#include <bsp.h>                /* Must be before libio.h */
#include <rtems/libio.h>

#define CD2401_INT_LEVEL 1      /* Interrupt level for the CD2401 */
#define CD2401_POLLED_IO 0      /* 0 for interrupt-driven, 1 for polled I/O */


/* Channel info */
/* static */ struct {
  void *tty;                    /* Really a struct rtems_termios_tty * */
  int len;                      /* Record nb of chars being TX'ed */
  const char *buf;              /* Record where DMA is coming from */
  rtems_unsigned16 used_buf_A;  /* Nb of times we used output DMA channel A */
  rtems_unsigned16 used_buf_B;  /* Nb of times we used output DMA channel B */
  rtems_unsigned16 wait_buf_A;  /* Nb of times we waited for output DMA channel A */
  rtems_unsigned16 wait_buf_B;  /* Nb of times we waited for output DMA channel B */
  rtems_unsigned32 spur_cnt;    /* Nb of spurious ints so far */
  rtems_unsigned32 spur_dev;    /* Indo on last spurious int */
  rtems_unsigned32 buserr_addr; /* Faulting address */
  rtems_unsigned32 buserr_type; /* Reason of bus error during DMA */
} CD2401_Channel_Info[4];

/*
 *  The number of channels already opened. If zero, enable the interrupts. The
 *  initial value must be 0. If initialized explicitly, the variable ends up
 *  in the .data section. Its value is not re-initialized on system restart.
 *  Furthermore, because the variable is changed, the .data section would not
 *  be ROMable. We thus leave the variable uninitialized, which causes it to
 *  be allocated in the .bss section, and rely on RTEMS to zero the .bss
 *  section on every startup.
 */
rtems_unsigned8 Init_count;


/* Record previous handlers */
rtems_isr_entry Prev_re_isr;        /* Previous rx exception isr */
rtems_isr_entry Prev_rx_isr;        /* Previous rx isr */
rtems_isr_entry Prev_tx_isr;        /* Previous tx isr */
rtems_isr_entry Prev_modem_isr;     /* Previous modem/timer isr */

/* Utility functions */
void cd2401_chan_cmd( rtems_unsigned8 channel, rtems_unsigned8 cmd, rtems_unsigned8 wait );
rtems_unsigned16 cd2401_bitrate_divisor( rtems_unsigned32 clkrate, rtems_unsigned32* bitrate );
void cd2401_initialize( void );
void cd2401_interrupts_initialize( rtems_boolean enable );

/* ISRs */
rtems_isr cd2401_modem_isr( rtems_vector_number vector );
rtems_isr cd2401_re_isr( rtems_vector_number vector );
rtems_isr cd2401_rx_isr( rtems_vector_number vector );
rtems_isr cd2401_tx_isr( rtems_vector_number vector );

/* Termios callbacks */
int cd2401_firstOpen( int major, int minor, void *arg );
int cd2401_lastClose( int major, int minor, void *arg );
int cd2401_setAttributes( int minor, const struct termios *t );
int cd2401_startRemoteTx( int minor );
int cd2401_stopRemoteTx( int minor );
int cd2401_write( int minor, const char *buf, int len );
int _167Bug_pollRead( int minor );
int _167Bug_pollWrite( int minor, const char *buf, int len );


/*
 *  Utility functions.
 */

/*
 *  cd2401_chan_cmd
 *
 *  Sends a CCR command to the specified channel. Waits for any unfinished
 *  previous command to complete, then sends the specified command. Optionally
 *  wait for the current command to finish before returning.
 *
 *  Input parameters:
 *    channel - CD2401 channel number
 *    cmd  - command byte
 *    wait - if non-zero, wait for specified command to complete before
 *          returning.
 *
 *  Output parameters: NONE
 *
 *  Return values: NONE
 */
void cd2401_chan_cmd(
  rtems_unsigned8 channel,
  rtems_unsigned8 cmd,
  rtems_unsigned8 wait
)
{
  if ( channel < 4 ) {
    cd2401->car = channel;      /* Select channel */

    while ( cd2401->ccr != 0 ); /* Wait for completion of any previous command */
    cd2401->ccr = cmd;          /* Send command */
    if ( wait )
      while( cd2401->ccr != 0 );/* Wait for completion */
  }
  else {
    /* This may not be the best error message */
    rtems_fatal_error_occurred( RTEMS_INVALID_NUMBER );
  }
}


/*
 *  cd2401_bitrate_divisor
 *
 *  Compute the divisor and clock source to use to obtain the desired bitrate.
 *
 *  Input parameters:
 *    clkrate - system clock rate (CLK input frequency)
 *    bitrate - the desired bitrate
 *
 *  Output parameters:
 *    bitrate - The actual bitrate achievable, to the nearest bps.
 *
 *  Return values:
 *    Returns divisor in lower byte and clock source in upper byte for the
 *    specified bitrate.
 */
rtems_unsigned16 cd2401_bitrate_divisor(
  rtems_unsigned32 clkrate,
  rtems_unsigned32* bitrate
)
{
  rtems_unsigned32 divisor;
  rtems_unsigned16 clksource;

  divisor = *bitrate << 3;          /* temporary; multiply by 8 for CLK/8 */
  divisor = (clkrate + (divisor>>1)) / divisor; /* divisor for clk0 (CLK/8) */

  /*  Use highest speed clock source for best precision - try from clk0 to clk4:  */
  for( clksource = 0; clksource < 0x0400 && divisor > 0x100; clksource += 0x0100 )
      divisor >>= 2;
  divisor--;                        /* adjustment, see specs */
  if( divisor < 1 )
    divisor = 1;
  else if( divisor > 0xFF )
    divisor = 0xFF;
  *bitrate = clkrate / (1 << ((clksource >> 7)+3)) / (divisor+1);
  return( clksource | divisor );
}


/*
 *  cd2401_initialize
 *
 *  Initializes the CD2401 device. Individual channels on the chip are left in
 *  their default reset state, and should be subsequently configured.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values: NONE
 */
void cd2401_initialize( void )
{
  int i;

  for ( i = 3; i >= 0; i-- ) {
    /*
     *  Paranoia -- Should already be blank because array should be in bss
     *  section, which is explicitly zeroed at boot time.
     */
    CD2401_Channel_Info[i].tty = NULL;
    CD2401_Channel_Info[i].len = 0;
    CD2401_Channel_Info[i].buf = NULL;
    CD2401_Channel_Info[i].used_buf_A = 0;
    CD2401_Channel_Info[i].used_buf_B = 0;
    CD2401_Channel_Info[i].wait_buf_A = 0;
    CD2401_Channel_Info[i].wait_buf_B = 0;
    CD2401_Channel_Info[i].spur_cnt = 0;
    CD2401_Channel_Info[i].spur_dev = 0;
    CD2401_Channel_Info[i].buserr_type = 0;
    CD2401_Channel_Info[i].buserr_addr = 0;
  }

 /*
  *  Normally, do a device reset here. If we do it, we will most likely clober
  *  the port settings for 167Bug on channel 0. So we just shut up all the
  *  ports by disabling their interrupts.
  */
#if 0
  cd2401->gfrcr = 0;            /* So we can detect that device init is done */
  cd2401_chan_cmd( 0x10, 0);    /* Reset all */
  while(cd2401->gfrcr == 0);    /* Wait for reset all */
#endif

  /*
   *  The CL-CD2400/2401 manual (part no 542400-003) states on page 87 that
   *  the LICR "contains the number of the interrupting channel being served.
   *  The channel number is always that of the current acknowledged interrupt."
   *  THE USER MUST PROGRAM CHANNEL NUMBER IN LICR! It is not set automatically
   *  by the hardware, as suggested by the manual.
   *
   *  The updated manual (part no 542400-007) has the story strait. The CD2401
   *  automatically initializes the LICR to contain the channel number in bits
   *  2 and 3. However, these bits are not preserved when the user defined bits
   *  are written.
   *
   *  The same vector number is used for all four channels. Different vector
   *  numbers could be programmed for each channel, thus avoiding the need to
   *  demultiplex the interrupts in the ISR.
   */
  for ( i = 0; i < 4; i++ ) {
    cd2401->car = i;            /* Select channel */
    cd2401->livr = 0x5C;        /* Motorola suggested value p. 3-15 */
    cd2401->licr = i << 2;      /* Don't rely on reset value */
    cd2401->ier = 0;            /* Disable all interrupts */
  }

  /*
   *  The content of the CD2401 xpilr registers must match the A7-A0 addresses
   *  generated by the PCCchip2 during interrupt acknowledge cycles in order
   *  for the CD2401 to recognize the IACK cycle and clear its interrupt
   *  request.
   */
  cd2401->mpilr = 0x01;         /* Match pccchip2->modem_piack p. 3-27 */
  cd2401->tpilr = 0x02;         /* Match pccchip2->tx_piack p. 3-28 */
  cd2401->rpilr = 0x03;         /* Match pccchip2->rx_piack p. 3-29 */

  /* Global CD2401 registers */
  cd2401->dmr = 0;              /* 16-bit DMA transfers when possible */
  cd2401->bercnt = 0;           /* Do not retry DMA upon bus errors */

  /*
   *  Setup timer prescaler period, which clocks timers 1 and 2 (or rx timeout
   *  and tx delay). The prescaler is clocked by the system clock) / 2048. The
   *  register must be in the range 0x0A..0xFF, ie. a rescaler period range of
   *  about 1ms..26ms for a nominal system clock rate  of 20MHz.
   */
  cd2401->tpr  = 0x0A;          /* Same value as 167Bug */
}


/*
 *  cd2401_interrupts_initialize
 *
 *  This routine enables or disables the CD2401 interrupts to the MC68040.
 *  Interrupts cannot be enabled/disabled on a per-channel basis.
 *
 *  Input parameters:
 *    enable - if true, enable the interrupts, else disable them.
 *
 *  Output parameters:  NONE
 *
 *  Return values: NONE
 *
 *  THE FIRST CD2401 CHANNEL OPENED SHOULD ENABLE INTERRUPTS.
 *  THE LAST CD2401 CHANNEL CLOSED SHOULD DISABLE INTERRUPTS.
 */
void cd2401_interrupts_initialize(
  rtems_boolean enable
)
{
  if ( enable ) {
   /*
    *  Enable interrupts from the CD2401 in the PCCchip2.
    *  During DMA transfers, the MC68040 supplies dirty data during read cycles
    *  from the CD2401 and leaves the data dirty in its data cache if there is
    *  a cache hit. The MC68040 updates the data cache during write cycles from
    *  the CD2401 if there is a cache hit.
    */
    pccchip2->SCC_error = 0x01;
    pccchip2->SCC_modem_int_ctl = 0x10 | CD2401_INT_LEVEL;
    pccchip2->SCC_tx_int_ctl = 0x10 | CD2401_INT_LEVEL;
    pccchip2->SCC_rx_int_ctl = 0x50 | CD2401_INT_LEVEL;

    pccchip2->gen_control |= 0x02;      /* Enable pccchip2 interrupts */
  }
  else {
    /* Disable interrupts */
    pccchip2->SCC_modem_int_ctl &= 0xEF;
    pccchip2->SCC_tx_int_ctl &= 0xEF;
    pccchip2->SCC_rx_int_ctl &= 0xEF;
  }
}


/* ISRs */

/*
 *  cd2401_modem_isr
 *
 *  Modem/timer interrupt (group 1) from CD2401. These are not used, and not
 *  expected. Record as spurious and clear.
 *
 *  Input parameters:
 *    vector - vector number
 *
 *  Output parameters: NONE
 *
 *  Return values: NONE
 */
rtems_isr cd2401_modem_isr(
  rtems_vector_number vector
)
{
  rtems_unsigned8 ch;

  /* Get interrupting channel ID */
  ch = cd2401->licr >> 2;

  /* Record interrupt info for debugging */
  CD2401_Channel_Info[ch].spur_dev =
      (vector << 24) | (cd2401->stk << 16) | (cd2401->mir << 8) | cd2401->misr;
  CD2401_Channel_Info[ch].spur_cnt++;

  cd2401->meoir = 0;            /* EOI */
}


/*
 *  cd2401_re_isr
 *
 *  RX exception interrupt (group 3, receiver exception) from CD2401. These are
 *  not used, and not expected. Record as spurious and clear.
 *
 *  FIX THIS ISR TO DETECT BREAK CONDITIONS AND RAISE SIGINT
 *
 *  Input parameters:
 *    vector - vector number
 *
 *  Output parameters: NONE
 *
 *  Return values: NONE
 */
rtems_isr cd2401_re_isr(
  rtems_vector_number vector
)
{
  rtems_unsigned8 ch;

  /* Get interrupting channel ID */
  ch = cd2401->licr >> 2;

  /* Record interrupt info for debugging */
  CD2401_Channel_Info[ch].spur_dev =
      (vector << 24) | (cd2401->stk << 16) | (cd2401->rir << 8) | cd2401->u5.b.risrl;
  CD2401_Channel_Info[ch].spur_cnt++;

  if ( cd2401->u5.b.risrl & 0x80 )  /* Timeout interrupt? */
    cd2401->ier &= 0xDF;            /* Disable rx timeout interrupt */
  cd2401->reoir = 0x08;             /* EOI; exception char not read */
}


/*
 *  cd2401_rx_isr
 *
 *  RX interrupt (group 3, receiver data) from CD2401.
 *
 *  Input parameters:
 *     vector - vector number
 *
 *  Output parameters: NONE
 *
 *  Return values: NONE
 */
rtems_isr cd2401_rx_isr(
  rtems_vector_number vector
)
{
  char c;
  rtems_unsigned8 ch, nchars;

  ch = cd2401->licr >> 2;

  /* Has this channel been initialized? */
  if (CD2401_Channel_Info[ch].tty) {
    /* Yes, read chars, enqueue them, and issue EOI */
    nchars = cd2401->rfoc;      /* Number of chars to retrieve from rx FIFO */
    while ( nchars-- > 0 ) {
      c = (char)cd2401->dr;     /* Next char in rx FIFO */
      rtems_termios_enqueue_raw_characters (
        CD2401_Channel_Info[ch].tty,
        &c,
        1 );
    }
    cd2401->reoir = 0;          /* EOI */
  } else {
    /* No, record as spurious interrupt */
    CD2401_Channel_Info[ch].spur_dev =
        (vector << 24) | (cd2401->stk << 16) | (cd2401->rir << 8) | cd2401->u5.b.risrl;
    CD2401_Channel_Info[ch].spur_cnt++;
    cd2401->reoir = 0x04;       /* EOI - character not read */
  }
}


/*
 *  cd2401_tx_isr
 *
 *  TX interrupt (group 2) from CD2401.
 *
 *  Input parameters:
 *    vector - vector number
 *
 *  Output parameters: NONE
 *
 *  Return values: NONE
 */
rtems_isr cd2401_tx_isr(
  rtems_vector_number vector
)
{
  rtems_unsigned8 ch, status, buserr;

  status = cd2401->tisr;
  ch = cd2401->licr >> 2;

  /* Has this channel been initialized? */
  if ( !CD2401_Channel_Info[ch].tty ) {
    /* No, record as spurious interrupt */
    CD2401_Channel_Info[ch].spur_dev =
        (vector << 24) | (cd2401->stk << 16) | (cd2401->tir << 8) | cd2401->tisr;
    CD2401_Channel_Info[ch].spur_cnt++;
    cd2401->ier &= 0xFC;        /* Shut up, whoever you are */
    cd2401->teoir = 0x88;       /* EOI - Terminate buffer and no transfer */
    return;
  }

  if ( status & 0x80 ) {
    /*
     *  Bus error occurred during DMA transfer. For now, just record.
     *  Get reason for DMA bus error and clear the report for the next occurrence
     */
    buserr = pccchip2->SCC_error;
    pccchip2->SCC_error = 0x01;
    CD2401_Channel_Info[ch].buserr_type =
         (vector << 24) | (buserr << 16) | (cd2401->tir << 8) | cd2401->tisr;
    CD2401_Channel_Info[ch].buserr_addr =
        (((rtems_unsigned32)cd2401->tcbadru) << 16) | cd2401->tcbadrl;

    cd2401->teoir = 0x80;       /* EOI - terminate bad buffer */
    return;
  }

  if ( status & 0x20 ) {
    /* DMA done */
    cd2401->ier &= 0xFC;        /* Shut up the interrupts */
    
    /* This call can result in a call to cd2401_write() */
    rtems_termios_dequeue_characters (
        CD2401_Channel_Info[ch].tty,
        CD2401_Channel_Info[ch].len );
    cd2401->teoir = 0x08;       /* EOI - no data transfered */
  }
  else {
    /* Why did we get a Tx interrupt? */
    CD2401_Channel_Info[ch].spur_dev =
        (vector << 24) | (cd2401->stk << 16) | (cd2401->tir << 8) | cd2401->tisr;
    CD2401_Channel_Info[ch].spur_cnt++;
    cd2401->teoir = 0x08;       /* EOI - no data transfered */
  }
}


/*
 *  termios callbacks
 */

/*
 *  cd2401_firstOpen
 *
 *  This is the first time that this minor device (channel) is opened.
 *  Complete the asynchronous initialization.
 *
 *  Input parameters:
 *    major - device major number
 *    minor - channel number
 *    arg - pointer to a struct rtems_libio_open_close_args_t
 *
 *  Output parameters: NONE
 *
 *  Return value: IGNORED
 */
int cd2401_firstOpen(
  int major,
  int minor,
  void *arg
)
{
  rtems_libio_open_close_args_t *args = arg;
  rtems_libio_ioctl_args_t newarg;
  struct termios termios;
  rtems_status_code sc;

  /*
   * Set up the line with the specified parameters. The difficulty is that
   * the line parameters are stored in the struct termios field of a
   * struct rtems_termios_tty that is not defined in a public header file.
   * Therefore, we do not have direct access to the termios passed in with
   * arg. So we make a rtems_termios_ioctl() call to get a pointer to the
   * termios structure.
   *
   * THIS KLUDGE MAY BREAK IN THE FUTURE!
   *
   * We could have made a tcgetattr() call if we had our fd.
   */
  newarg.iop = args->iop;
  newarg.command = RTEMS_IO_GET_ATTRIBUTES;
  newarg.buffer = &termios;
  sc = rtems_termios_ioctl (&newarg);
  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (sc);
    
  /*
   *  Turn off hardware flow control. It is a pain with 3-wire cables.
   *  The rtems_termios_ioctl() call below results in a call to
   *  cd2401_setAttributes to initialize the line. The caller will "wait"
   *  on the ttyMutex that it already owns; this is safe in RTEMS.
   */
  termios.c_cflag |= CLOCAL;    /* Ignore modem status lines */
  newarg.command = RTEMS_IO_SET_ATTRIBUTES;
  sc = rtems_termios_ioctl (&newarg);
  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (sc);
  
  /* Mark that the channel as initialized */
  CD2401_Channel_Info[minor].tty = args->iop->data1;

  /* If the first of the four channels to open, set up the interrupts */
  if ( !Init_count++ ) {
    /* Install the interrupt handlers */
    Prev_re_isr    = (rtems_isr_entry) set_vector( cd2401_re_isr,    0x5C, 1 );
    Prev_modem_isr = (rtems_isr_entry) set_vector( cd2401_modem_isr, 0x5D, 1 );
    Prev_tx_isr    = (rtems_isr_entry) set_vector( cd2401_tx_isr,    0x5E, 1 );
    Prev_rx_isr    = (rtems_isr_entry) set_vector( cd2401_rx_isr,    0x5F, 1 );

    cd2401_interrupts_initialize( TRUE );
  }

  /* Return something */
  return RTEMS_SUCCESSFUL;
}


/*
 * cd2401_lastClose
 *
 *  There are no more opened file descriptors to this device. Close it down.
 *
 *  Input parameters:
 *    major - device major number
 *    minor - channel number
 *    arg - pointer to a struct rtems_libio_open_close_args_t
 */
int cd2401_lastClose(
  int major,
  int minor,
  void *arg
)
{
  /* Mark that the channel is no longer is use */
  CD2401_Channel_Info[minor].tty = NULL;

  /* If the last of the four channels to close, disable the interrupts */
  if ( !--Init_count ) {
    cd2401_interrupts_initialize( FALSE );

    /* De-install the interrupt handlers */
    set_vector( Prev_re_isr,    0x5C, 1 );
    set_vector( Prev_modem_isr, 0x5D, 1 );
    set_vector( Prev_tx_isr,    0x5E, 1 );
    set_vector( Prev_rx_isr,    0x5F, 1 );
  }

  /* return something */
  return RTEMS_SUCCESSFUL;
}


/*
 *  cd2401_setAttributes
 *
 *  Set up the selected channel of the CD2401 chip for doing asynchronous
 *  I/O with DMA.
 *
 *  The chip must already have been initialized by cd2401_initialize().
 *
 *  This code was written for clarity. The code space it occupies could be
 *  reduced. The code could also be compiled with aggressive optimization
 *  turned on.
 *
 *  Input parameters:
 *    minor - the selected channel
 *    t - the termios parameters
 *
 *  Output parameters: NONE
 *
 *  Return value: IGNORED
 */
int cd2401_setAttributes(
  int minor,
  const struct termios *t
)
{
  rtems_unsigned8 csize, cstopb, parodd, parenb, ignpar, inpck;
  rtems_unsigned8 hw_flow_ctl, sw_flow_ctl, extra_flow_ctl;
  rtems_unsigned8 icrnl, igncr, inlcr, brkint, ignbrk, parmrk, istrip;
  rtems_unsigned16 tx_period, rx_period;
  rtems_unsigned32 out_baud, in_baud;

  /* Set up the line parameters */

  /* Output baud rate */
  switch ( cfgetospeed (t) ) {
    default:      out_baud = 9600;    break;
    case B50:     out_baud = 50;      break;
    case B75:     out_baud = 75;      break;
    case B110:    out_baud = 110;     break;
    case B134:    out_baud = 134;     break;
    case B150:    out_baud = 150;     break;
    case B200:    out_baud = 200;     break;
    case B300:    out_baud = 300;     break;
    case B600:    out_baud = 600;     break;
    case B1200:   out_baud = 1200;    break;
    case B1800:   out_baud = 1800;    break;
    case B2400:   out_baud = 2400;    break;
    case B4800:   out_baud = 4800;    break;
    case B9600:   out_baud = 9600;    break;
    case B19200:  out_baud = 19200;   break;
    case B38400:  out_baud = 38400;   break;
    case B57600:  out_baud = 57600;   break;
    case B115200: out_baud = 115200;  break;
    case B230400: out_baud = 230400;  break;
    case B460800: out_baud = 460800;  break;
 }

  /* Input baud rate */
  switch ( cfgetispeed (t) ) {
    default:      in_baud = out_baud; break;
    case B50:     in_baud = 50;       break;
    case B75:     in_baud = 75;       break;
    case B110:    in_baud = 110;      break;
    case B134:    in_baud = 134;      break;
    case B150:    in_baud = 150;      break;
    case B200:    in_baud = 200;      break;
    case B300:    in_baud = 300;      break;
    case B600:    in_baud = 600;      break;
    case B1200:   in_baud = 1200;     break;
    case B1800:   in_baud = 1800;     break;
    case B2400:   in_baud = 2400;     break;
    case B4800:   in_baud = 4800;     break;
    case B9600:   in_baud = 9600;     break;
    case B19200:  in_baud = 19200;    break;
    case B38400:  in_baud = 38400;    break;
    case B57600:  in_baud = 57600;    break;
    case B115200: in_baud = 115200;   break;
    case B230400: in_baud = 230400;   break;
    case B460800: in_baud = 460800;   break;
  }

  /* Number of bits per char */
  switch ( t->c_cflag & CSIZE ) {
    case CS5:     csize = 0x04;       break;
    case CS6:     csize = 0x05;       break;
    case CS7:     csize = 0x06;       break;
    case CS8:     csize = 0x07;       break;
  }

  /* Parity */
  if ( t->c_cflag & PARODD )
    parodd = 0x80;              /* Odd parity */
  else
    parodd = 0;

  if ( t->c_cflag & PARENB )
    parenb = 0x40;              /* Parity enabled on Tx and Rx */
  else
    parenb = 0x00;              /* No parity on Tx and Rx */

  /* CD2401 IGNPAR and INPCK bits are inverted wrt POSIX standard? */
  if ( t->c_iflag & INPCK )
    ignpar = 0;                 /* Check parity on input */
  else
    ignpar = 0x10;              /* Do not check parity on input */
  if ( t->c_iflag & IGNPAR ) {
    inpck = 0x03;               /* Discard error character */
    parmrk = 0;
  } else {
    if ( t->c_iflag & PARMRK ) {
      inpck = 0x01;             /* Translate to 0xFF 0x00 <char> */
      parmrk = 0x04;
    } else {
      inpck = 0x01;             /* Translate to 0x00 */
      parmrk = 0;
    }
  }

  /* Stop bits */
  if ( t->c_cflag & CSTOPB )
    cstopb = 0x04;              /* Two stop bits */
  else
    cstopb = 0x02;              /* One stop bit */

  /* Modem flow control */
  if ( t->c_cflag & CLOCAL )
    hw_flow_ctl = 0x04;         /* Always assert RTS before Tx */
  else
    hw_flow_ctl = 0x07;         /* Always assert RTS before Tx,
                                   wait for CTS and DSR */

  /* XON/XOFF Tx flow control */
  if ( t->c_iflag & IXON ) {
    sw_flow_ctl = 0x40;         /* Tx in-band flow ctl enabled, wait for XON */
    extra_flow_ctl = 0x30;      /* Eat XON/XOFF, XON/XOFF in SCHR1, SCHR2 */
  }
  else {
    sw_flow_ctl = 0;            /* Tx in-band flow ctl disabled */
    extra_flow_ctl = 0;         /* Pass on XON/XOFF */
  }

  /* CL/LF translation */
  if ( t->c_iflag & ICRNL )
    icrnl = 0x40;               /* Map CR to NL on input */
  else
    icrnl = 0;                  /* Pass on CR */
  if ( t->c_iflag & INLCR )
    inlcr = 0x20;               /* Map NL to CR on input */
  else
    inlcr = 0;                  /* Pass on NL */
  if ( t->c_iflag & IGNCR )
    igncr = 0x80;               /* CR discarded on input */
  else
    igncr = 0;

  /* Break handling */
  if ( t->c_iflag & IGNBRK ) {
    ignbrk = 0x10;              /* Ignore break on input */
    brkint = 0x08;
  } else {
    if ( t->c_iflag & BRKINT ) {
      ignbrk = 0;               /* Generate SIGINT (interrupt ) */
      brkint = 0;
    } else {
      ignbrk = 0;               /* Convert to 0x00 */
      brkint = 0x08;
    }
  }

  /* Stripping */
  if ( t->c_iflag & ISTRIP )
    istrip = 0x80;              /* Strip to 7 bits */
  else
    istrip = 0;                 /* Leave as 8 bits */

  
  /* Clear channel and disable rx and tx */
  cd2401_chan_cmd (minor, 0x40, 1);
  
  /* Write to the ports */
  cd2401->car = minor;          /* Select channel */
  cd2401->cmr = 0x42;           /* Interrupt Rx, DMA Tx, async mode */
  cd2401->cor1 = parodd | parenb | ignpar | csize;
  cd2401->cor2 = sw_flow_ctl | hw_flow_ctl;
  cd2401->cor3 = extra_flow_ctl | cstopb;
  cd2401->cor4 = 0x0A;          /* No DSR/DCD/CTS detect; FIFO threshold of 10 */
  cd2401->cor5 = 0x0A;          /* No DSR/DCD/CTS detect; DTR threshold of 10 */
  cd2401->cor6 = igncr | icrnl | inlcr | ignbrk | brkint | parmrk | inpck;
  cd2401->cor7 = istrip;        /* No LNext; ignore XON/XOFF if frame error; no tx translations */
  cd2401->u1.async.schr1 =
      t->c_cc[VSTART];          /* Special char 1: XON character */
  cd2401->u1.async.schr2 =
      t->c_cc[VSTOP];           /* special char 2: XOFF character */
  /* Special chars 3 and 4, char range, LNext, RFAR[1..4] and CRC are unused, left as is. */

  /* Set baudrates for receiver and transmitter */
  rx_period = cd2401_bitrate_divisor( 20000000Ul, &in_baud );
  cd2401->rbpr = (unsigned char)rx_period;
  cd2401->rcor = (unsigned char)(rx_period >> 8); /* no DPLL */
  tx_period = cd2401_bitrate_divisor( 20000000Ul, &out_baud );
  cd2401->tbpr = (unsigned char)tx_period;
  cd2401->tcor = (tx_period >> 3) & 0xE0;         /* no x1 ext clk, no loopback */

  /* NEED TO LOOK AT THIS LINE! */
  /* Timeout for 4 chars at 9600, 8 bits per char, 1 stop bit */
  cd2401->u2.w.rtpr  = 0x04;

  /*  And finally:  */
  if ( t->c_cflag & CREAD ) {
    /* Re-initialize channel, enable rx and tx */
    cd2401_chan_cmd (minor, 0x2A, 1);
    /* Enable rx data ints */
    cd2401->ier = 0x08;
  } else {
    /* Re-initialize channel, enable tx, disable rx */
    cd2401_chan_cmd (minor, 0x29, 1);
  }

  /* Return something */
  return RTEMS_SUCCESSFUL;
}


/*
 *  cd2401_startRemoreTx
 *
 *  Defined as a callback, but it would appear that it is never called. The
 *  POSIX standard states that when the tcflow() function is called with the
 *  TCION action, the system wall transmit a START character. Presumably,
 *  tcflow() is called internally when IXOFF is set in the termios c_iflag
 *  field when the input buffer can accomodate enough characters. It should
 *  probably be called from fillBufferQueue(). Clearly, the function is also
 *  explicitly callable by user code. The action is clearly to send the START
 *  character, regardless of whether START/STOP flow control is in effect.
 *
 *  Input parameters:
 *    minor - selected channel
 *
 *  Output parameters: NONE
 *
 *  Return value: IGNORED
 *
 *  PROPER START CHARACTER MUST BE PROGRAMMED IN SCHR1.
 */
int cd2401_startRemoteTx(
  int minor
)
{
  cd2401->car = minor;          /* Select channel */
  cd2401->stcr = 0x01;          /* Send SCHR1 ahead of chars in FIFO */

  /* Return something */
  return RTEMS_SUCCESSFUL;
}


/*
 *  cd2401_stopRemoreTx
 *
 *  Defined as a callback, but it would appear that it is never called. The
 *  POSIX standard states that when the tcflow() function is called with the
 *  TCIOFF function, the system wall transmit a STOP character. Presumably,
 *  tcflow() is called internally when IXOFF is set in the termios c_iflag
 *  field as the input buffer is about to overflow. It should probably be
 *  called from rtems_termios_enqueue_raw_characters(). Clearly, the function
 *  is also explicitly callable by user code. The action is clearly to send
 *  the STOP character, regardless of whether START/STOP flow control is in
 *  effect.
 *
 *  Input parameters:
 *    minor - selected channel
 *
 *  Output parameters: NONE
 *
 *  Return value: IGNORED
 *
 *  PROPER STOP CHARACTER MUST BE PROGRAMMED IN SCHR2.
 */
int cd2401_stopRemoteTx(
  int minor
)
{
  cd2401->car = minor;          /* Select channel */
  cd2401->stcr = 0x02;          /* Send SCHR2 ahead of chars in FIFO */

  /* Return something */
  return RTEMS_SUCCESSFUL;
}


/*
 * cd2401_write
 *
 *  Initiate DMA output. Termios guarantees that the buffer does not wrap
 *  around, so we can do DMA strait from the supplied buffer.
 *
 *  Input parameters:
 *    minor - selected channel
 *    buf - output buffer
 *    len - number of chars to output
 *
 *  Output parameters:  NONE
 *
 *  Return value: IGNORED
 *
 *  MUST BE EXECUTED WITH THE CD2401 INTERRUPTS DISABLED!
 *  The processor is placed at interrupt level CD2401_INT_LEVEL explicitly in
 *  console_write(). The processor is necessarily at interrupt level 1 in
 *  cd2401_tx_isr().
 */
int cd2401_write(
  int minor,
  const char *buf,
  int len
)
{
  cd2401->car = minor;          /* Select channel */

  if ( (cd2401->dmabsts & 0x08) == 0 ) {
    /* Next buffer is A. Wait for it to be ours. */
    if ( cd2401->atbsts & 0x01 ) {
      CD2401_Channel_Info[minor].wait_buf_A++;
      while ( cd2401->atbsts & 0x01 );
    }
    CD2401_Channel_Info[minor].used_buf_A++;
    CD2401_Channel_Info[minor].len = len;
    CD2401_Channel_Info[minor].buf = buf;
    cd2401->atbadru = (rtems_unsigned16)( ( (rtems_unsigned32) buf ) >> 16 );
    cd2401->atbadrl = (rtems_unsigned16)( (rtems_unsigned32) buf );
    cd2401->atbcnt = len;
    cd2401->atbsts = 0x03;      /* CD2401 owns buffer, int when empty */
  }
  else {
    /* Next buffer is B. Wait for it to be ours. */
    if ( cd2401->btbsts & 0x01 ) {
      CD2401_Channel_Info[minor].wait_buf_B++;
      while ( cd2401->btbsts & 0x01 );
    }
    CD2401_Channel_Info[minor].used_buf_B++;
    CD2401_Channel_Info[minor].len = len;
    CD2401_Channel_Info[minor].buf = buf;
    cd2401->btbadru = (rtems_unsigned16)( ( (rtems_unsigned32) buf ) >> 16 );
    cd2401->btbadrl = (rtems_unsigned16)( (rtems_unsigned32) buf );
    cd2401->btbcnt = len;
    cd2401->btbsts = 0x03;      /* CD2401 owns buffer, int when empty */
  }
  /* Should TxD interrupts be enabled before I set up the DMA transfer? */
  cd2401->ier |= 0x01;          /* enable TxD ints */

  /* Return something */
  return RTEMS_SUCCESSFUL;
}


/*
 * _167Bug_pollRead
 *
 *  Read a character from the 167Bug console, and return it. Return -1
 *  if there is no character in the input FIFO.
 *
 *  Input parameters:
 *    minor - selected channel
 *
 *  Output parameters:  NONE
 *
 *  Return value: char returned as positive signed int
 *                -1 if no character is present in the input FIFO.
 *
 *  CANNOT BE COMBINED WITH INTERRUPT DRIVEN I/O!
 *  This function is invoked when the device driver is compiled with
 *  CD2401_POLLED_IO set to 1 above. All I/O is then done through 167Bug.
 */
int _167Bug_pollRead(
  int minor
)
{
  int char_not_available;
  unsigned char c;
  
  /* Check for a char in the input FIFO */
  asm volatile( "trap   #15       /* Trap to 167Bug (.INSTAT) */
                 .short 0x01
                 move   %%cc, %0  /* Get condition codes */
                 andil  #4, %0"
    : "=d" (char_not_available) :: "%%cc" );
    
  if (char_not_available)
    return -1;
    
  /* Read the char and return it */
  asm volatile( "subq.l #2,%%a7   /* Space for result */
                 trap   #15       /* Trap to 167 Bug (.INCHR) */
                 .short 0x00
                 moveb  (%%a7)+, %0"
    : "=d" (c) );
    
  return (int)c;
}


/*
 * _167Bug_pollWrite
 *
 *  Output buffer through 167Bug. Returns only once every character has been
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
 *
 *  CANNOT BE COMBINED WITH INTERRUPT DRIVEN I/O!
 *  This function is invoked when the device driver is compiled with
 *  CD2401_POLLED_IO set to 1 above. All I/O is then done through 167Bug.
 */
int _167Bug_pollWrite(
  int minor,
  const char *buf,
  int len
)
{
  const char *endbuf = buf + len;
  
  asm volatile( "pea    (%0)
                 pea    (%1)
                 trap   #15     /* trap to 167Bug (.OUTSTR) */
                 .short 0x21"
    :: "a" (endbuf), "a" (buf) );
    
  /* Return something */
  return RTEMS_SUCCESSFUL;
}


/*
 ***************
 * BOILERPLATE *
 ***************
 *
 *  All these functions are prototyped in rtems/c/src/lib/include/console.h,
 *  except console_reserve_resources(), which is prototyped in
 *  rtems/c/src/lib/libbsp/m68k/mvme167/include/bsp.h
 */

/*
 * Reserve resources consumed by this driver. Allocate enough space in the
 * object table to hold semaphores for 4 minor devices.
 */
void console_reserve_resources(
  rtems_configuration_table *configuration
)
{
  rtems_termios_reserve_resources (configuration, 4);
}

/*
 * Initialize and register the device
 */
rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_status_code status;

  /*
   * Set up TERMIOS
   */
  rtems_termios_initialize ();

  /*
   * Do device-specific initialization
   */
  cd2401_initialize ();

  /*
   * Register the devices
   */
  status = rtems_io_register_name ("/dev/tty0", major, 0);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);

  status = rtems_io_register_name ("/dev/tty1", major, 1);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);

  status = rtems_io_register_name ("/dev/console", major, 1);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);

  status = rtems_io_register_name ("/dev/tty2", major, 2);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);

  status = rtems_io_register_name ("/dev/tty3", major, 3);
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
  void                    * arg
)
{
#if CD2401_POLLED_IO

  /* I/O is limited to 167Bug console. minor is ignored! */
  static const rtems_termios_callbacks callbacks = {
    NULL,                       /* firstOpen */
    NULL,                       /* lastClose */
    _167Bug_pollRead,           /* pollRead */
    _167Bug_pollWrite,          /* write */
    NULL,                       /* setAttributes */
    NULL,                       /* stopRemoteTx */
    NULL,                       /* startRemoteTx */
    0                           /* outputUsesInterrupts */
  };
  
#else

  static const rtems_termios_callbacks callbacks = {
    cd2401_firstOpen,           /* firstOpen */
    cd2401_lastClose,           /* lastClose */
    NULL,                       /* pollRead */
    cd2401_write,               /* write */
    cd2401_setAttributes,       /* setAttributes */
    cd2401_stopRemoteTx,        /* stopRemoteTx */
    cd2401_startRemoteTx,       /* startRemoteTx */
    1                           /* outputUsesInterrupts */
  };
  
#endif

  return rtems_termios_open (major, minor, arg, &callbacks);
}

/*
 * Close the device
 */
rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
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
  void                    * arg
)
{
  return rtems_termios_read (arg);
}

/*
 * Write to the device
 */
rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_write (arg);
}

/*
 * Handle ioctl request.
 */
rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_ioctl (arg);
}
