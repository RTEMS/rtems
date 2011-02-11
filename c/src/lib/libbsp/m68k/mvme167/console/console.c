/*
 *  console.c
 *
 *  This file contains the MVME167 termios console package. Only asynchronous
 *  I/O is supported.
 *
 *  /dev/tty0 is channel 0, Serial Port 1/Console on the MVME712M.
 *  /dev/tty1 is channel 1, Serial Port 2/TTY01 on the MVME712M.
 *  /dev/tty2 is channel 2, Serial Port 3 on the MVME712M.
 *  /dev/tty3 is channel 3, Serial Port 4 on the MVME712M.
 *
 *  Normal I/O uses DMA for output, interrupts for input. /dev/console is
 *  fixed to be /dev/tty01, Serial Port 2. Very limited support is provided
 *  for polled I/O. Polled I/O is intended only for running the RTEMS test
 *  suites. In all cases, Serial Port 1/Console is allocated to 167Bug and
 *  is the dedicated debugger port. We configure GDB to use 167Bug for
 *  debugging. When debugging with GDB or 167Bug, do not open /dev/tty00.
 *
 *  Modern I/O chips often contain a number of I/O devices that can operate
 *  almost independently of each other. Typically, in RTEMS, all devices in
 *  an I/O chip are handled by a single device driver, but that need not be
 *  always the case. Each device driver must supply six entry points in the
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
 *  a serial line to some synchronous protocol.
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
 *  http://www.rtems.com/license/LICENSE.
 */

#define M167_INIT

#include <stdarg.h>
#include <stdio.h>
#include <termios.h>
#include <rtems/termiostypes.h>
#include <bsp.h>                /* Must be before libio.h */
#include <rtems/libio.h>

/* Utility functions */
void cd2401_udelay( unsigned long delay );
void cd2401_chan_cmd( uint8_t         channel, uint8_t         cmd, uint8_t         wait );
uint16_t         cd2401_bitrate_divisor( uint32_t         clkrate, uint32_t        * bitrate );
void cd2401_initialize( void );
void cd2401_interrupts_initialize( bool enable );

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
ssize_t cd2401_write( int minor, const char *buf, size_t len );
int cd2401_drainOutput( int minor );
int _167Bug_pollRead( int minor );
ssize_t _167Bug_pollWrite( int minor, const char *buf, size_t len );

/* Printk function */
static void _BSP_output_char( char c );
BSP_output_char_function_type     BSP_output_char = _BSP_output_char;
BSP_polling_getchar_function_type BSP_poll_char = NULL;

/* '\r' character in memory. This used to live on
 * the stack but storing the '\r' character is
 * optimized away by gcc-4.3.2 (since it seems to
 * be unused [only referenced from inline assembly
 * code in _167Bug_pollWrite()]).
 * Hence we make it a global constant.
 */
static const char cr_char = '\r';

/* Channel info */
/* static */ volatile struct {
  void *tty;                    /* Really a struct rtems_termios_tty * */
  int len;                      /* Record nb of chars being TX'ed */
  const char *buf;              /* Record where DMA is coming from */
  uint32_t         spur_cnt;    /* Nb of spurious ints so far */
  uint32_t         spur_dev;    /* Indo on last spurious int */
  uint32_t         buserr_addr; /* Faulting address */
  uint32_t         buserr_type; /* Reason of bus error during DMA */
  uint8_t          own_buf_A;   /* If true, buffer A belongs to the driver */
  uint8_t          own_buf_B;   /* If true, buffer B belongs to the driver */
  uint8_t          txEmpty;     /* If true, the output FIFO should be empty */
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
uint8_t         Init_count;

/* Record previous handlers */
rtems_isr_entry Prev_re_isr;        /* Previous rx exception isr */
rtems_isr_entry Prev_rx_isr;        /* Previous rx isr */
rtems_isr_entry Prev_tx_isr;        /* Previous tx isr */
rtems_isr_entry Prev_modem_isr;     /* Previous modem/timer isr */

/* Define the following symbol to trace the calls to this driver */
/* #define CD2401_RECORD_DEBUG_INFO */
#include "console-recording.h"

/*
 *  Utility functions.
 */

/*
 *  Assumes that clock ticks 1 million times per second.
 *
 *  MAXIMUM DELAY IS ABOUT 20 ms
 *
 *  Input parameters:
 *    delay: Number of microseconds to delay.
 *
 *  Output parameters: NONE
 *
 *  Return values: NONE
 */
 void cd2401_udelay
(
  unsigned long delay
)
{
  unsigned long i = 20000;  /* In case clock is off */
  rtems_interval ticks_per_second, start_ticks, end_ticks, current_ticks;

  ticks_per_second = rtems_clock_get_ticks_per_second();
  start_ticks = rtems_clock_get_ticks_since_boot();
  end_ticks = start_ticks + delay;

  do {
    current_ticks = rtems_clock_get_ticks_since_boot();
  } while ( --i && (current_ticks <= end_ticks) );

  CD2401_RECORD_DELAY_INFO(( start_ticks, end_ticks, current_ticks, i ));
}

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
  uint8_t         channel,
  uint8_t         cmd,
  uint8_t         wait
)
{
  if ( channel < 4 ) {
    cd2401->car = channel;      /* Select channel */

    while ( cd2401->ccr != 0 ); /* Wait for completion of previous command */
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
uint16_t         cd2401_bitrate_divisor(
  uint32_t         clkrate,
  uint32_t        * bitrate
)
{
  uint32_t         divisor;
  uint16_t         clksource;

  divisor = *bitrate << 3;          /* temporary; multiply by 8 for CLK/8 */
  divisor = (clkrate + (divisor>>1)) / divisor; /* divisor for clk0 (CLK/8) */

  /* Use highest speed clock source for best precision - try clk0 to clk4 */
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
    CD2401_Channel_Info[i].tty = NULL;
    CD2401_Channel_Info[i].len = 0;
    CD2401_Channel_Info[i].buf = NULL;
    CD2401_Channel_Info[i].spur_cnt = 0;
    CD2401_Channel_Info[i].spur_dev = 0;
    CD2401_Channel_Info[i].buserr_type = 0;
    CD2401_Channel_Info[i].buserr_addr = 0;
    CD2401_Channel_Info[i].own_buf_A = TRUE;
    CD2401_Channel_Info[i].own_buf_B = TRUE;
    CD2401_Channel_Info[i].txEmpty = TRUE;
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
   *  The updated manual (part no 542400-007) has the story straight. The
   *  CD2401 automatically initializes the LICR to contain the channel number
   *  in bits 2 and 3. However, these bits are not preserved when the user
   *  defined bits are written.
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
  bool enable
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
  uint8_t         ch;

  /* Get interrupting channel ID */
  ch = cd2401->licr >> 2;

  /* Record interrupt info for debugging */
  CD2401_Channel_Info[ch].spur_dev =
      (vector << 24) | (cd2401->stk << 16) | (cd2401->mir << 8) | cd2401->misr;
  CD2401_Channel_Info[ch].spur_cnt++;

  cd2401->meoir = 0;            /* EOI */
  CD2401_RECORD_MODEM_ISR_SPURIOUS_INFO(( ch,
                                          CD2401_Channel_Info[ch].spur_dev,
                                          CD2401_Channel_Info[ch].spur_cnt ));
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
  uint8_t         ch;

  /* Get interrupting channel ID */
  ch = cd2401->licr >> 2;

  /* Record interrupt info for debugging */
  CD2401_Channel_Info[ch].spur_dev =
      (vector << 24) | (cd2401->stk << 16) | (cd2401->rir << 8) | cd2401->u5.b.risrl;
  CD2401_Channel_Info[ch].spur_cnt++;

  if ( cd2401->u5.b.risrl & 0x80 )  /* Timeout interrupt? */
    cd2401->ier &= 0xDF;            /* Disable rx timeout interrupt */
  cd2401->reoir = 0x08;             /* EOI; exception char not read */
  CD2401_RECORD_RE_ISR_SPURIOUS_INFO(( ch,
                                       CD2401_Channel_Info[ch].spur_dev,
                                       CD2401_Channel_Info[ch].spur_cnt ));
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
  uint8_t         ch, status, nchars, i, total;
  char buffer[256];

  status = cd2401->u5.b.risrl;
  ch = cd2401->licr >> 2;

  /* Has this channel been initialized or is it a condition we ignore? */
  if ( CD2401_Channel_Info[ch].tty && !status ) {
    /* Normal Rx Int, read chars, enqueue them, and issue EOI */
    total = nchars = cd2401->rfoc;  /* Nb of chars to retrieve from rx FIFO */
    i = 0;
    while ( nchars-- > 0 ) {
      c = (char)cd2401->dr;         /* Next char in rx FIFO */
      rtems_termios_enqueue_raw_characters( CD2401_Channel_Info[ch].tty ,&c, 1 );
      buffer[i++] = c;
    }
    cd2401->reoir = 0;              /* EOI */
    CD2401_RECORD_RX_ISR_INFO(( ch, total, buffer ));
  } else {
    /* No, record as spurious interrupt */
    CD2401_Channel_Info[ch].spur_dev =
        (vector << 24) | (cd2401->stk << 16) | (cd2401->rir << 8) | cd2401->u5.b.risrl;
    CD2401_Channel_Info[ch].spur_cnt++;
    cd2401->reoir = 0x04;           /* EOI - character not read */
    CD2401_RECORD_RX_ISR_SPURIOUS_INFO(( ch, status,
                                         CD2401_Channel_Info[ch].spur_dev,
                                         CD2401_Channel_Info[ch].spur_cnt ));
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
  uint8_t         ch, status, buserr, initial_ier, final_ier;

  status = cd2401->tisr;
  ch = cd2401->licr >> 2;
  initial_ier = cd2401->ier;

  /* Has this channel been initialized? */
  if ( !CD2401_Channel_Info[ch].tty ) {
    /* No, record as spurious interrupt */
    CD2401_Channel_Info[ch].spur_dev =
        (vector << 24) | (cd2401->stk << 16) | (cd2401->tir << 8) | cd2401->tisr;
    CD2401_Channel_Info[ch].spur_cnt++;
    final_ier = cd2401->ier &= 0xFC;/* Shut up, whoever you are */
    cd2401->teoir = 0x88;           /* EOI - Terminate buffer and no transfer */
    CD2401_RECORD_TX_ISR_SPURIOUS_INFO(( ch, status, initial_ier, final_ier,
                                         CD2401_Channel_Info[ch].spur_dev,
                                         CD2401_Channel_Info[ch].spur_cnt ));
    return;
  }

  if ( status & 0x80 ) {
    /*
     *  Bus error occurred during DMA transfer. For now, just record.
     *  Get reason for DMA bus error and clear the report for the next
     *  occurrence
     */
    buserr = pccchip2->SCC_error;
    pccchip2->SCC_error = 0x01;
    CD2401_Channel_Info[ch].buserr_type =
         (vector << 24) | (buserr << 16) | (cd2401->tir << 8) | cd2401->tisr;
    CD2401_Channel_Info[ch].buserr_addr =
        (((uint32_t)cd2401->tcbadru) << 16) | cd2401->tcbadrl;

    cd2401->teoir = 0x80;           /* EOI - terminate bad buffer */
    CD2401_RECORD_TX_ISR_BUSERR_INFO(( ch, status, initial_ier, buserr,
                                       CD2401_Channel_Info[ch].buserr_type,
                                       CD2401_Channel_Info[ch].buserr_addr ));
    return;
  }

  if ( status & 0x20 ) {
    /* DMA done -- Turn off TxD int, turn on TxMpty */
    final_ier = cd2401->ier = (cd2401->ier & 0xFE) | 0x02;
    if( status & 0x08 ) {
      /* Transmit buffer B was released */
      CD2401_Channel_Info[ch].own_buf_B = TRUE;
    }
    else {
      /* Transmit buffer A was released */
      CD2401_Channel_Info[ch].own_buf_A = TRUE;
    }
    CD2401_RECORD_TX_ISR_INFO(( ch, status, initial_ier, final_ier,
                                CD2401_Channel_Info[ch].txEmpty ));

    /* This call can result in a call to cd2401_write() */
    rtems_termios_dequeue_characters (
        CD2401_Channel_Info[ch].tty,
        CD2401_Channel_Info[ch].len );
    cd2401->teoir = 0x08;           /* EOI - no data transfered */
  }
  else if ( status & 0x02 ) {
    /* TxEmpty */
    CD2401_Channel_Info[ch].txEmpty = TRUE;
    final_ier = cd2401->ier &= 0xFD;/* Shut up the interrupts */
    cd2401->teoir = 0x08;           /* EOI - no data transfered */
    CD2401_RECORD_TX_ISR_INFO(( ch, status, initial_ier, final_ier,
                                CD2401_Channel_Info[ch].txEmpty ));
  }
  else {
    /* Why did we get a Tx interrupt? */
    CD2401_Channel_Info[ch].spur_dev =
        (vector << 24) | (cd2401->stk << 16) | (cd2401->tir << 8) | cd2401->tisr;
    CD2401_Channel_Info[ch].spur_cnt++;
    cd2401->teoir = 0x08;           /* EOI - no data transfered */
    CD2401_RECORD_TX_ISR_SPURIOUS_INFO(( ch, status, initial_ier, 0xFF,
                                         CD2401_Channel_Info[ch].spur_dev,
                                         CD2401_Channel_Info[ch].spur_cnt ));
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
  rtems_interrupt_level level;

  rtems_interrupt_disable (level);

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

  CD2401_RECORD_FIRST_OPEN_INFO(( minor, Init_count ));

  rtems_interrupt_enable (level);

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
  rtems_interrupt_level level;

  rtems_interrupt_disable (level);

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

  CD2401_RECORD_LAST_CLOSE_INFO(( minor, Init_count ));

  rtems_interrupt_enable (level);

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
  uint8_t         csize, cstopb, parodd, parenb, ignpar, inpck;
  uint8_t         hw_flow_ctl, sw_flow_ctl, extra_flow_ctl;
  uint8_t         icrnl, igncr, inlcr, brkint, ignbrk, parmrk, istrip;
  uint8_t         need_reinitialization = FALSE;
  uint8_t         read_enabled;
  uint16_t         tx_period, rx_period;
  uint32_t         out_baud, in_baud;
  rtems_interrupt_level level;

  /* Determine what the line parameters should be */

  /* baud rates */
  out_baud = rtems_termios_baud_to_number(t->c_cflag & CBAUD);
  in_baud  = rtems_termios_baud_to_number(t->c_cflag & CBAUD);

  /* Number of bits per char */
  csize = 0x07; /* to avoid a warning */
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

  rx_period = cd2401_bitrate_divisor( 20000000Ul, &in_baud );
  tx_period = cd2401_bitrate_divisor( 20000000Ul, &out_baud );

  /*
   *  If this is the first time that the line characteristics are set up, then
   *  the device must be re-initialized.
   *  Also check if we need to change anything. It is preferable to not touch
   *  the device if nothing changes. As soon as we touch it, it tends to
   *  glitch. If anything changes, we reprogram all registers. This is
   *  harmless.
   */
  if ( ( CD2401_Channel_Info[minor].tty == 0 ) ||
       ( cd2401->cor1 != (parodd | parenb | ignpar | csize) ) ||
       ( cd2401->cor2 != (sw_flow_ctl | hw_flow_ctl) ) ||
       ( cd2401->cor3 != (extra_flow_ctl | cstopb) )  ||
       ( cd2401->cor6 != (igncr | icrnl | inlcr | ignbrk | brkint | parmrk | inpck) ) ||
       ( cd2401->cor7 != istrip ) ||
       ( cd2401->u1.async.schr1 != t->c_cc[VSTART] ) ||
       ( cd2401->u1.async.schr2 != t->c_cc[VSTOP] ) ||
       ( cd2401->rbpr != (unsigned char)rx_period ) ||
       ( cd2401->rcor != (unsigned char)(rx_period >> 8) ) ||
       ( cd2401->tbpr != (unsigned char)tx_period ) ||
       ( cd2401->tcor != ( (tx_period >> 3) & 0xE0 ) ) )
    need_reinitialization = TRUE;

  /* Write to the ports */
  rtems_interrupt_disable (level);

  cd2401->car = minor;          /* Select channel */
  read_enabled = cd2401->csr & 0x80 ? TRUE : FALSE;

  if ( (t->c_cflag & CREAD ? TRUE : FALSE ) != read_enabled ) {
    /* Read enable status is changing */
    need_reinitialization = TRUE;
  }

  if ( need_reinitialization ) {
    /*
     *  Could not find a way to test whether the CD2401 was done transmitting.
     *  The TxEmpty interrupt does not seem to indicate that the FIFO is empty
     *  in DMA mode. So, just wait a while for output to drain. May not be
     *  enough, but it will have to do (should be long enough for 1 char at
     *  9600 bsp)...
     */
    cd2401_udelay( 2000L );

    /* Clear channel */
    cd2401_chan_cmd (minor, 0x40, 1);

    cd2401->car = minor;    /* Select channel */
    cd2401->cmr = 0x42;     /* Interrupt Rx, DMA Tx, async mode */
    cd2401->cor1 = parodd | parenb | ignpar | csize;
    cd2401->cor2 = sw_flow_ctl | hw_flow_ctl;
    cd2401->cor3 = extra_flow_ctl | cstopb;
    cd2401->cor4 = 0x0A;    /* No DSR/DCD/CTS detect; FIFO threshold of 10 */
    cd2401->cor5 = 0x0A;    /* No DSR/DCD/CTS detect; DTR threshold of 10 */
    cd2401->cor6 = igncr | icrnl | inlcr | ignbrk | brkint | parmrk | inpck;
    cd2401->cor7 = istrip;  /* No LNext; ignore XON/XOFF if frame error; no tx translations */
    /* Special char 1: XON character */
    cd2401->u1.async.schr1 = t->c_cc[VSTART];
    /* special char 2: XOFF character */
    cd2401->u1.async.schr2 = t->c_cc[VSTOP];

    /*
     *  Special chars 3 and 4, char range, LNext, RFAR[1..4] and CRC
     *  are unused, left as is.
     */

    /* Set baudrates for receiver and transmitter */
    cd2401->rbpr = (unsigned char)rx_period;
    cd2401->rcor = (unsigned char)(rx_period >> 8); /* no DPLL */
    cd2401->tbpr = (unsigned char)tx_period;
    cd2401->tcor = (tx_period >> 3) & 0xE0; /* no x1 ext clk, no loopback */

    /* Timeout for 4 chars at 9600, 8 bits per char, 1 stop bit */
    cd2401->u2.w.rtpr  = 0x04;  /* NEED TO LOOK AT THIS LINE! */

    if ( t->c_cflag & CREAD ) {
      /* Re-initialize channel, enable rx and tx */
      cd2401_chan_cmd (minor, 0x2A, 1);
      /* Enable rx data ints */
      cd2401->ier = 0x08;
    } else {
      /* Re-initialize channel, enable tx, disable rx */
      cd2401_chan_cmd (minor, 0x29, 1);
    }
  }

  CD2401_RECORD_SET_ATTRIBUTES_INFO(( minor, need_reinitialization, csize,
                                      cstopb, parodd, parenb, ignpar, inpck,
                                      hw_flow_ctl, sw_flow_ctl, extra_flow_ctl,
                                      icrnl, igncr, inlcr, brkint, ignbrk,
                                      parmrk, istrip, tx_period, rx_period,
                                      out_baud, in_baud ));

  rtems_interrupt_enable (level);

  /*
   *  Looks like the CD2401 needs time to settle after initialization. Give it
   *  10 ms. I don't really believe it, but if output resumes to quickly after
   *  this call, the first few characters are not right.
   */
  if ( need_reinitialization )
    cd2401_udelay( 10000L );

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
  rtems_interrupt_level level;

  rtems_interrupt_disable (level);

  cd2401->car = minor;              /* Select channel */
  cd2401->stcr = 0x01;              /* Send SCHR1 ahead of chars in FIFO */

  CD2401_RECORD_START_REMOTE_TX_INFO(( minor ));

  rtems_interrupt_enable (level);

  /* Return something */
  return RTEMS_SUCCESSFUL;
}

/*
 *  cd2401_stopRemoteTx
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
  rtems_interrupt_level level;

  rtems_interrupt_disable (level);

  cd2401->car = minor;              /* Select channel */
  cd2401->stcr = 0x02;              /* Send SCHR2 ahead of chars in FIFO */

  CD2401_RECORD_STOP_REMOTE_TX_INFO(( minor ));

  rtems_interrupt_enable (level);

  /* Return something */
  return RTEMS_SUCCESSFUL;
}

/*
 *  cd2401_write
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
ssize_t cd2401_write(
  int minor,
  const char *buf,
  size_t len
)
{
  cd2401->car = minor;              /* Select channel */

  if ( (cd2401->dmabsts & 0x08) == 0 ) {
    /* Next buffer is A. Wait for it to be ours. */
    while ( cd2401->atbsts & 0x01 );

    CD2401_Channel_Info[minor].own_buf_A = FALSE;
    CD2401_Channel_Info[minor].len = len;
    CD2401_Channel_Info[minor].buf = buf;
    cd2401->atbadru = (uint16_t)( ( (uint32_t) buf ) >> 16 );
    cd2401->atbadrl = (uint16_t)( (uint32_t) buf );
    cd2401->atbcnt = len;
    CD2401_RECORD_WRITE_INFO(( len, buf, 'A' ));
    cd2401->atbsts = 0x03;          /* CD2401 owns buffer, int when empty */
  }
  else {
    /* Next buffer is B. Wait for it to be ours. */
    while ( cd2401->btbsts & 0x01 );

    CD2401_Channel_Info[minor].own_buf_B = FALSE;
    CD2401_Channel_Info[minor].len = len;
    CD2401_Channel_Info[minor].buf = buf;
    cd2401->btbadru = (uint16_t)( ( (uint32_t) buf ) >> 16 );
    cd2401->btbadrl = (uint16_t)( (uint32_t) buf );
    cd2401->btbcnt = len;
    CD2401_RECORD_WRITE_INFO(( len, buf, 'B' ));
    cd2401->btbsts = 0x03;          /* CD2401 owns buffer, int when empty */
  }
  /* Nuts -- Need TxD ints */
  CD2401_Channel_Info[minor].txEmpty = FALSE;
  cd2401->ier |= 0x01;

  /* Return something */
  return len;
}

#if 0
/*
 *  cd2401_drainOutput
 *
 *  Wait for the txEmpty indication on the specified channel.
 *
 *  Input parameters:
 *    minor - selected channel
 *
 *  Output parameters:  NONE
 *
 *  Return value: IGNORED
 *
 *  MUST NOT BE EXECUTED WITH THE CD2401 INTERRUPTS DISABLED!
 *  The txEmpty flag is set by the tx ISR.
 *
 *  DOES NOT WORK! DO NOT ENABLE THIS CODE. THE CD2401 DOES NOT COOPERATE!
 *  The code is here to document that the output FIFO is NOT empty when
 *  the CD2401 reports that the Tx buffer is empty.
 */
int cd2401_drainOutput(
  int minor
)
{
  CD2401_RECORD_DRAIN_OUTPUT_INFO(( CD2401_Channel_Info[minor].txEmpty,
                                    CD2401_Channel_Info[minor].own_buf_A,
                                    CD2401_Channel_Info[minor].own_buf_B ));

  while( ! (CD2401_Channel_Info[minor].txEmpty &&
            CD2401_Channel_Info[minor].own_buf_A &&
            CD2401_Channel_Info[minor].own_buf_B) );

  /* Return something */
  return RTEMS_SUCCESSFUL;
}
#endif

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
 */
int _167Bug_pollRead(
  int minor
)
{
  int char_not_available;
  unsigned char c;
  rtems_interrupt_level previous_level;

  /*
   *  Redirection of .INSTAT does not work: 167-Bug crashes.
   *  Switch the input stream to the specified port.
   *  Make sure this is atomic code.
   */
  rtems_interrupt_disable( previous_level );

  __asm__ volatile( "movew  %1, -(%%sp)\n\t"/* Channel */
                "trap   #15\n\t"        /* Trap to 167Bug */
                ".short 0x61\n\t"       /* Code for .REDIR_I */
                "trap   #15\n\t"        /* Trap to 167Bug */
		".short 0x01\n\t"       /* Code for .INSTAT */
                "move   %%cc, %0\n\t"   /* Get condition codes */
                "andil  #4, %0"         /* Keep the Zero bit */
    : "=d" (char_not_available) : "d" (minor): "%%cc" );

  if (char_not_available) {
    rtems_interrupt_enable( previous_level );
    return -1;
  }

  /* Read the char and return it */
  __asm__ volatile( "subq.l #2,%%a7\n\t"    /* Space for result */
                "trap   #15\n\t"        /* Trap to 167 Bug */
                ".short 0x00\n\t"       /* Code for .INCHR */
                "moveb  (%%a7)+, %0"    /* Pop char into c */
    : "=d" (c) : );

  rtems_interrupt_enable( previous_level );

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
 */
ssize_t _167Bug_pollWrite(
  int minor,
  const char *buf,
  size_t len
)
{
  const char *endbuf = buf + len;

  __asm__ volatile( "pea    (%0)\n\t"            /* endbuf */
                "pea    (%1)\n\t"            /* buf */
                "movew  #0x21, -(%%sp)\n\t"  /* Code for .OUTSTR */
                "movew  %2, -(%%sp)\n\t"     /* Channel */
                "trap   #15\n\t"             /* Trap to 167Bug */
                ".short 0x60"                /* Code for .REDIR */
    :: "a" (endbuf), "a" (buf), "d" (minor) );

  /* Return something */
  return len;
}

/*
 *  do_poll_read
 *
 *  Input characters through 167Bug. Returns has soon as a character has been
 *  received. Otherwise, if we wait for the number of requested characters, we
 *  could be here forever!
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
rtems_status_code do_poll_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_libio_rw_args_t *rw_args = arg;
  int c;

  while( (c = _167Bug_pollRead (minor)) == -1 );
  rw_args->buffer[0] = (uint8_t)c;
  if( rw_args->buffer[0] == '\r' )
      rw_args->buffer[0] = '\n';
  rw_args->bytes_moved = 1;
  return RTEMS_SUCCESSFUL;
}

/*
 *  do_poll_write
 *
 *  Output characters through 167Bug. Returns only once every character has
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
rtems_status_code do_poll_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_libio_rw_args_t *rw_args = arg;
  uint32_t   i;

  for( i = 0; i < rw_args->count; i++ ) {
    _167Bug_pollWrite(minor, &(rw_args->buffer[i]), 1);
    if ( rw_args->buffer[i] == '\n' )
      _167Bug_pollWrite(minor, &cr_char, 1);
  }
  rw_args->bytes_moved = i;
  return RTEMS_SUCCESSFUL;
}

/*
 *  _BSP_output_char
 *
 *  printk() function prototyped in bspIo.h. Does not use termios.
 */
void _BSP_output_char(char c)
{
  rtems_device_minor_number printk_minor;

  /*
   *  Can't rely on console_initialize having been called before this function
   *  is used.
   */
  if ( NVRAM_CONFIGURE )
    /* J1-4 is on, use NVRAM info for configuration */
    printk_minor = (nvram->console_printk_port & 0x30) >> 4;
  else
    printk_minor = PRINTK_MINOR;

  _167Bug_pollWrite(printk_minor, &c, 1);
  if ( c == '\n' )
      _167Bug_pollWrite(printk_minor, &cr_char, 1);
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
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_status_code status;
  rtems_device_minor_number console_minor;

  /*
   * Set up TERMIOS if needed
   */
  if ( NVRAM_CONFIGURE ) {
    /* J1-4 is on, use NVRAM info for configuration */
    console_minor = nvram->console_printk_port & 0x03;

    if ( nvram->console_mode & 0x01 )
      /* termios */
      rtems_termios_initialize ();
  }
  else {
    console_minor = CONSOLE_MINOR;
#if CD2401_USE_TERMIOS == 1
    rtems_termios_initialize ();
#endif
  }

  /*
   * Do device-specific initialization
   * Does not affect 167-Bug.
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

  status = rtems_io_register_name ("/dev/console", major, console_minor);
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
  static const rtems_termios_callbacks pollCallbacks = {
    NULL,                       /* firstOpen */
    NULL,                       /* lastClose */
    _167Bug_pollRead,           /* pollRead */
    _167Bug_pollWrite,          /* write */
    NULL,                       /* setAttributes */
    NULL,                       /* stopRemoteTx */
    NULL,                       /* startRemoteTx */
    0                           /* outputUsesInterrupts */
  };

  static const rtems_termios_callbacks intrCallbacks = {
    cd2401_firstOpen,           /* firstOpen */
    cd2401_lastClose,           /* lastClose */
    NULL,                       /* pollRead */
    cd2401_write,               /* write */
    cd2401_setAttributes,       /* setAttributes */
    cd2401_stopRemoteTx,        /* stopRemoteTx */
    cd2401_startRemoteTx,       /* startRemoteTx */
    1                           /* outputUsesInterrupts */
  };

  if ( NVRAM_CONFIGURE )
    /* J1-4 is on, use NVRAM info for configuration */
    if ( nvram->console_mode & 0x01 )
      /* termios */
      if ( nvram->console_mode & 0x02 )
        /* interrupt-driven I/O */
        return rtems_termios_open (major, minor, arg, &intrCallbacks);
	    else
        /* polled I/O */
        return rtems_termios_open (major, minor, arg, &pollCallbacks);
	  else
	    /* no termios -- default to polled I/O */
	    return RTEMS_SUCCESSFUL;
#if CD2401_USE_TERMIOS == 1
#if CD2401_IO_MODE != 1
  else
    /* termios & polled I/O*/
    return rtems_termios_open (major, minor, arg, &pollCallbacks);
#else
  else
    /* termios & interrupt-driven I/O*/
    return rtems_termios_open (major, minor, arg, &intrCallbacks);
#endif
#else
  else
    /* no termios -- default to polled I/O */
    return RTEMS_SUCCESSFUL;
#endif
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
  if ( NVRAM_CONFIGURE ) {
    /* J1-4 is on, use NVRAM info for configuration */
    if ( nvram->console_mode & 0x01 )
      /* termios */
      return rtems_termios_close (arg);
    else
      /* no termios */
      return RTEMS_SUCCESSFUL;
  }
#if CD2401_USE_TERMIOS == 1
  else
    /* termios */
    return rtems_termios_close (arg);
#else
  else
    /* no termios */
    return RTEMS_SUCCESSFUL;
#endif
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
  if ( NVRAM_CONFIGURE ) {
    /* J1-4 is on, use NVRAM info for configuration */
    if ( nvram->console_mode & 0x01 )
      /* termios */
      return rtems_termios_read (arg);
    else
      /* no termios -- default to polled */
      return do_poll_read (major, minor, arg);
  }
#if CD2401_USE_TERMIOS == 1
  else
    /* termios */
    return rtems_termios_read (arg);
#else
  else
    /* no termios -- default to polled */
    return do_poll_read (major, minor, arg);
#endif
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
  if ( NVRAM_CONFIGURE ) {
    /* J1-4 is on, use NVRAM info for configuration */
    if ( nvram->console_mode & 0x01 )
      /* termios */
      return rtems_termios_write (arg);
    else
      /* no termios -- default to polled */
      return do_poll_write (major, minor, arg);
  }
#if CD2401_USE_TERMIOS == 1
  else
    /* termios */
    return rtems_termios_write (arg);
#else
  else
    /* no termios -- default to polled */
    return do_poll_write (major, minor, arg);
#endif
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
  if ( NVRAM_CONFIGURE ) {
    /* J1-4 is on, use NVRAM info for configuration */
    if ( nvram->console_mode & 0x01 )
      /* termios */
      return rtems_termios_ioctl (arg);
    else
      /* no termios -- default to polled */
      return RTEMS_SUCCESSFUL;
  }
#if CD2401_USE_TERMIOS == 1
  else
    /* termios */
    return rtems_termios_ioctl (arg);
#else
  else
    /* no termios -- default to polled */
    return RTEMS_SUCCESSFUL;
#endif
}
