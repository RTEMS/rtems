/*
 * TERMIOS serial line support
 *
 *  Author:
 *    W. Eric Norum
 *    Saskatchewan Accelerator Laboratory
 *    University of Saskatchewan
 *    Saskatoon, Saskatchewan, CANADA
 *    eric@skatter.usask.ca
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/libio.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ttycom.h>

#include <rtems/termiostypes.h>

/*
 * The size of the cooked buffer
 */
#define CBUFSIZE  (rtems_termios_cbufsize)

/*
 * The sizes of the raw message buffers.
 * On most architectures it is quite a bit more
 * efficient if these are powers of two.
 */
#define RAW_INPUT_BUFFER_SIZE  (rtems_termios_raw_input_size)
#define RAW_OUTPUT_BUFFER_SIZE  (rtems_termios_raw_output_size)

/* fields for "flow_ctrl" status */
#define FL_IREQXOF 1U        /* input queue requests stop of incoming data */
#define FL_ISNTXOF 2U        /* XOFF has been sent to other side of line   */
#define FL_IRTSOFF 4U        /* RTS has been turned off for other side..   */

#define FL_ORCVXOF 0x10U     /* XOFF has been received                     */
#define FL_OSTOP   0x20U     /* output has been stopped due to XOFF        */

#define FL_MDRTS   0x100U    /* input controlled with RTS/CTS handshake    */
#define FL_MDXON   0x200U    /* input controlled with XON/XOFF protocol    */
#define FL_MDXOF   0x400U    /* output controlled with XON/XOFF protocol   */

#define NODISC(n) \
  { NULL,  NULL,  NULL,  NULL, \
    NULL,  NULL,  NULL,  NULL }
/*
 * FIXME: change rtems_termios_linesw entries consistent
 *        with rtems_termios_linesw entry usage...
 */
struct  rtems_termios_linesw rtems_termios_linesw[MAXLDISC] =
{
  NODISC(0),    /* 0- termios-built-in */
  NODISC(1),    /* 1- defunct */
  NODISC(2),    /* 2- NTTYDISC */
  NODISC(3),    /* TABLDISC */
  NODISC(4),    /* SLIPDISC */
  NODISC(5),    /* PPPDISC */
  NODISC(6),    /* loadable */
  NODISC(7),    /* loadable */
};

int  rtems_termios_nlinesw =
       sizeof (rtems_termios_linesw) / sizeof (rtems_termios_linesw[0]);

extern struct rtems_termios_tty *rtems_termios_ttyHead;
extern struct rtems_termios_tty *rtems_termios_ttyTail;
extern rtems_id rtems_termios_ttyMutex;

static size_t rtems_termios_cbufsize = 256;
static size_t rtems_termios_raw_input_size = 128;
static size_t rtems_termios_raw_output_size = 64;

static rtems_task rtems_termios_rxdaemon(rtems_task_argument argument);
static rtems_task rtems_termios_txdaemon(rtems_task_argument argument);
/*
 * some constants for I/O daemon task creation
 */
#define TERMIOS_TXTASK_PRIO 10
#define TERMIOS_RXTASK_PRIO 9
#define TERMIOS_TXTASK_STACKSIZE 1024
#define TERMIOS_RXTASK_STACKSIZE 1024
/*
 * some events to be sent to the I/O tasks
 */
#define TERMIOS_TX_START_EVENT     RTEMS_EVENT_1
#define TERMIOS_TX_TERMINATE_EVENT RTEMS_EVENT_0

#define TERMIOS_RX_PROC_EVENT      RTEMS_EVENT_1
#define TERMIOS_RX_TERMINATE_EVENT RTEMS_EVENT_0

/*
 * Open a termios device
 */
rtems_status_code
rtems_termios_open (
  rtems_device_major_number      major,
  rtems_device_minor_number      minor,
  void                          *arg,
  const rtems_termios_callbacks *callbacks
)
{
  rtems_status_code sc;
  rtems_libio_open_close_args_t *args = arg;
  struct rtems_termios_tty *tty;

  /*
   * See if the device has already been opened
   */
  sc = rtems_semaphore_obtain(
    rtems_termios_ttyMutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  if (sc != RTEMS_SUCCESSFUL)
    return sc;

  for (tty = rtems_termios_ttyHead ; tty != NULL ; tty = tty->forw) {
    if ((tty->major == major) && (tty->minor == minor))
      break;
  }

  if (tty == NULL) {
    static char c = 'a';

    /*
     * Create a new device
     */
    tty = calloc (1, sizeof (struct rtems_termios_tty));
    if (tty == NULL) {
      rtems_semaphore_release (rtems_termios_ttyMutex);
      return RTEMS_NO_MEMORY;
    }
    /*
     * allocate raw input buffer
     */
    tty->rawInBuf.Size = RAW_INPUT_BUFFER_SIZE;
    tty->rawInBuf.theBuf = malloc (tty->rawInBuf.Size);
    if (tty->rawInBuf.theBuf == NULL) {
            free(tty);
      rtems_semaphore_release (rtems_termios_ttyMutex);
      return RTEMS_NO_MEMORY;
    }
    /*
     * allocate raw output buffer
     */
    tty->rawOutBuf.Size = RAW_OUTPUT_BUFFER_SIZE;
    tty->rawOutBuf.theBuf = malloc (tty->rawOutBuf.Size);
    if (tty->rawOutBuf.theBuf == NULL) {
            free((void *)(tty->rawInBuf.theBuf));
            free(tty);
      rtems_semaphore_release (rtems_termios_ttyMutex);
      return RTEMS_NO_MEMORY;
    }
    /*
     * allocate cooked buffer
     */
    tty->cbuf  = malloc (CBUFSIZE);
    if (tty->cbuf == NULL) {
            free((void *)(tty->rawOutBuf.theBuf));
            free((void *)(tty->rawInBuf.theBuf));
            free(tty);
      rtems_semaphore_release (rtems_termios_ttyMutex);
      return RTEMS_NO_MEMORY;
    }
    /*
     * Initialize wakeup callbacks
     */
    tty->tty_snd.sw_pfn = NULL;
    tty->tty_snd.sw_arg = NULL;
    tty->tty_rcv.sw_pfn = NULL;
    tty->tty_rcv.sw_arg = NULL;
    tty->tty_rcvwakeup  = 0;

    /*
     * link tty
     */
    tty->forw = rtems_termios_ttyHead;
    tty->back = NULL;
    if (rtems_termios_ttyHead != NULL)
      rtems_termios_ttyHead->back = tty;
    rtems_termios_ttyHead = tty;
    if (rtems_termios_ttyTail == NULL)
      rtems_termios_ttyTail = tty;

    tty->minor = minor;
    tty->major = major;

    /*
     * Set up mutex semaphores
     */
    sc = rtems_semaphore_create (
      rtems_build_name ('T', 'R', 'i', c),
      1,
      RTEMS_BINARY_SEMAPHORE | RTEMS_INHERIT_PRIORITY | RTEMS_PRIORITY,
      RTEMS_NO_PRIORITY,
      &tty->isem);
    if (sc != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred (sc);
    sc = rtems_semaphore_create (
      rtems_build_name ('T', 'R', 'o', c),
      1,
      RTEMS_BINARY_SEMAPHORE | RTEMS_INHERIT_PRIORITY | RTEMS_PRIORITY,
      RTEMS_NO_PRIORITY,
      &tty->osem);
    if (sc != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred (sc);
    sc = rtems_semaphore_create (
      rtems_build_name ('T', 'R', 'x', c),
      0,
      RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_FIFO,
      RTEMS_NO_PRIORITY,
      &tty->rawOutBuf.Semaphore);
    if (sc != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred (sc);
    tty->rawOutBufState = rob_idle;

    /*
     * Set callbacks
     */
    tty->device = *callbacks;

    /*
     * Create I/O tasks
     */
    if (tty->device.outputUsesInterrupts == TERMIOS_TASK_DRIVEN) {
      sc = rtems_task_create (
                                   rtems_build_name ('T', 'x', 'T', c),
           TERMIOS_TXTASK_PRIO,
           TERMIOS_TXTASK_STACKSIZE,
           RTEMS_NO_PREEMPT | RTEMS_NO_TIMESLICE |
           RTEMS_NO_ASR,
           RTEMS_NO_FLOATING_POINT | RTEMS_LOCAL,
           &tty->txTaskId);
      if (sc != RTEMS_SUCCESSFUL)
        rtems_fatal_error_occurred (sc);
      sc = rtems_task_create (
                                   rtems_build_name ('R', 'x', 'T', c),
           TERMIOS_RXTASK_PRIO,
           TERMIOS_RXTASK_STACKSIZE,
           RTEMS_NO_PREEMPT | RTEMS_NO_TIMESLICE |
           RTEMS_NO_ASR,
           RTEMS_NO_FLOATING_POINT | RTEMS_LOCAL,
           &tty->rxTaskId);
      if (sc != RTEMS_SUCCESSFUL)
        rtems_fatal_error_occurred (sc);

    }
    if ((tty->device.pollRead == NULL) ||
        (tty->device.outputUsesInterrupts == TERMIOS_TASK_DRIVEN)){
      sc = rtems_semaphore_create (
        rtems_build_name ('T', 'R', 'r', c),
        0,
        RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_PRIORITY,
        RTEMS_NO_PRIORITY,
        &tty->rawInBuf.Semaphore);
      if (sc != RTEMS_SUCCESSFUL)
        rtems_fatal_error_occurred (sc);
    }

    /*
     * Set default parameters
     */
    tty->termios.c_iflag = BRKINT | ICRNL | IXON | IMAXBEL;
    tty->termios.c_oflag = OPOST | ONLCR | XTABS;
    tty->termios.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
    tty->termios.c_lflag =
       ISIG | ICANON | IEXTEN | ECHO | ECHOK | ECHOE | ECHOCTL;

    tty->termios.c_cc[VINTR] = '\003';
    tty->termios.c_cc[VQUIT] = '\034';
    tty->termios.c_cc[VERASE] = '\177';
    tty->termios.c_cc[VKILL] = '\025';
    tty->termios.c_cc[VEOF] = '\004';
    tty->termios.c_cc[VEOL] = '\000';
    tty->termios.c_cc[VEOL2] = '\000';
    tty->termios.c_cc[VSTART] = '\021';
    tty->termios.c_cc[VSTOP] = '\023';
    tty->termios.c_cc[VSUSP] = '\032';
    tty->termios.c_cc[VREPRINT] = '\022';
    tty->termios.c_cc[VDISCARD] = '\017';
    tty->termios.c_cc[VWERASE] = '\027';
    tty->termios.c_cc[VLNEXT] = '\026';

    /* start with no flow control, clear flow control flags */
    tty->flow_ctrl = 0;
    /*
     * set low/highwater mark for XON/XOFF support
     */
    tty->lowwater  = tty->rawInBuf.Size * 1/2;
    tty->highwater = tty->rawInBuf.Size * 3/4;
    /*
     * Bump name characer
     */
    if (c++ == 'z')
      c = 'a';

  }
  args->iop->data1 = tty;
  if (!tty->refcount++) {
    if (tty->device.firstOpen)
      (*tty->device.firstOpen)(major, minor, arg);

    /*
     * start I/O tasks, if needed
     */
    if (tty->device.outputUsesInterrupts == TERMIOS_TASK_DRIVEN) {
      sc = rtems_task_start(
        tty->rxTaskId, rtems_termios_rxdaemon, (rtems_task_argument)tty);
      if (sc != RTEMS_SUCCESSFUL)
        rtems_fatal_error_occurred (sc);

      sc = rtems_task_start(
        tty->txTaskId, rtems_termios_txdaemon, (rtems_task_argument)tty);
      if (sc != RTEMS_SUCCESSFUL)
        rtems_fatal_error_occurred (sc);
    }
  }
  rtems_semaphore_release (rtems_termios_ttyMutex);
  return RTEMS_SUCCESSFUL;
}

/*
 * Drain output queue
 */
static void
drainOutput (struct rtems_termios_tty *tty)
{
  rtems_interrupt_level level;
  rtems_status_code sc;

  if (tty->device.outputUsesInterrupts != TERMIOS_POLLED) {
    rtems_interrupt_disable (level);
    while (tty->rawOutBuf.Tail != tty->rawOutBuf.Head) {
      tty->rawOutBufState = rob_wait;
      rtems_interrupt_enable (level);
      sc = rtems_semaphore_obtain(
        tty->rawOutBuf.Semaphore, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
      if (sc != RTEMS_SUCCESSFUL)
        rtems_fatal_error_occurred (sc);
      rtems_interrupt_disable (level);
    }
    rtems_interrupt_enable (level);
  }
}

rtems_status_code
rtems_termios_close (void *arg)
{
  rtems_libio_open_close_args_t *args = arg;
  struct rtems_termios_tty *tty = args->iop->data1;
  rtems_status_code sc;

  sc = rtems_semaphore_obtain(
    rtems_termios_ttyMutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (sc);
  if (--tty->refcount == 0) {
    if (rtems_termios_linesw[tty->t_line].l_close != NULL) {
      /*
       * call discipline-specific close
       */
      sc = rtems_termios_linesw[tty->t_line].l_close(tty);
    } else {
      /*
       * default: just flush output buffer
       */
      sc = rtems_semaphore_obtain(tty->osem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
      if (sc != RTEMS_SUCCESSFUL) {
        rtems_fatal_error_occurred (sc);
      }
      drainOutput (tty);
      rtems_semaphore_release (tty->osem);
    }

    if (tty->device.outputUsesInterrupts == TERMIOS_TASK_DRIVEN) {
      /*
       * send "terminate" to I/O tasks
       */
      sc = rtems_event_send( tty->rxTaskId, TERMIOS_RX_TERMINATE_EVENT );
      if (sc != RTEMS_SUCCESSFUL)
        rtems_fatal_error_occurred (sc);
      sc = rtems_event_send( tty->txTaskId, TERMIOS_TX_TERMINATE_EVENT );
      if (sc != RTEMS_SUCCESSFUL)
        rtems_fatal_error_occurred (sc);
    }
    if (tty->device.lastClose)
       (*tty->device.lastClose)(tty->major, tty->minor, arg);
    if (tty->forw == NULL) {
      rtems_termios_ttyTail = tty->back;
      if ( rtems_termios_ttyTail != NULL ) {
        rtems_termios_ttyTail->forw = NULL;
      }
    } else {
      tty->forw->back = tty->back;
    }

    if (tty->back == NULL) {
      rtems_termios_ttyHead = tty->forw;
      if ( rtems_termios_ttyHead != NULL ) {
        rtems_termios_ttyHead->back = NULL;
      }
    } else {
      tty->back->forw = tty->forw;
    }

    rtems_semaphore_delete (tty->isem);
    rtems_semaphore_delete (tty->osem);
    rtems_semaphore_delete (tty->rawOutBuf.Semaphore);
    if ((tty->device.pollRead == NULL) ||
        (tty->device.outputUsesInterrupts == TERMIOS_TASK_DRIVEN))
      rtems_semaphore_delete (tty->rawInBuf.Semaphore);
    free (tty->rawInBuf.theBuf);
    free (tty->rawOutBuf.theBuf);
    free (tty->cbuf);
    free (tty);
  }
  rtems_semaphore_release (rtems_termios_ttyMutex);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_termios_bufsize (
  size_t cbufsize,
  size_t raw_input,
  size_t raw_output
)
{
  rtems_termios_cbufsize        = cbufsize;
  rtems_termios_raw_input_size  = raw_input;
  rtems_termios_raw_output_size = raw_output;
  return RTEMS_SUCCESSFUL;
}

static void
termios_set_flowctrl(struct rtems_termios_tty *tty)
{
  rtems_interrupt_level level;
  /*
   * check for flow control options to be switched off
   */

  /* check for outgoing XON/XOFF flow control switched off */
  if (( tty->flow_ctrl & FL_MDXON) &&
      !(tty->termios.c_iflag & IXON)) {
    /* clear related flags in flow_ctrl */
    tty->flow_ctrl &= ~(FL_MDXON | FL_ORCVXOF);

    /* has output been stopped due to received XOFF? */
    if (tty->flow_ctrl & FL_OSTOP) {
      /* disable interrupts    */
      rtems_interrupt_disable(level);
      tty->flow_ctrl &= ~FL_OSTOP;
      /* check for chars in output buffer (or rob_state?) */
      if (tty->rawOutBufState != rob_idle) {
        /* if chars available, call write function... */
        (*tty->device.write)(
          tty->minor, &tty->rawOutBuf.theBuf[tty->rawOutBuf.Tail],1);
      }
      /* reenable interrupts */
      rtems_interrupt_enable(level);
    }
  }
  /* check for incoming XON/XOFF flow control switched off */
  if (( tty->flow_ctrl & FL_MDXOF) && !(tty->termios.c_iflag & IXOFF)) {
    /* clear related flags in flow_ctrl */
    tty->flow_ctrl &= ~(FL_MDXOF);
    /* FIXME: what happens, if we had sent XOFF but not yet XON? */
    tty->flow_ctrl &= ~(FL_ISNTXOF);
  }

  /* check for incoming RTS/CTS flow control switched off */
  if (( tty->flow_ctrl & FL_MDRTS) && !(tty->termios.c_cflag & CRTSCTS)) {
    /* clear related flags in flow_ctrl */
    tty->flow_ctrl &= ~(FL_MDRTS);

    /* restart remote Tx, if it was stopped */
    if ((tty->flow_ctrl & FL_IRTSOFF) && (tty->device.startRemoteTx != NULL)) {
      tty->device.startRemoteTx(tty->minor);
    }
    tty->flow_ctrl &= ~(FL_IRTSOFF);
  }

  /*
   * check for flow control options to be switched on
   */
  /* check for incoming RTS/CTS flow control switched on */
  if (tty->termios.c_cflag & CRTSCTS) {
    tty->flow_ctrl |= FL_MDRTS;
  }
  /* check for incoming XON/XOF flow control switched on */
  if (tty->termios.c_iflag & IXOFF) {
    tty->flow_ctrl |= FL_MDXOF;
  }
  /* check for outgoing XON/XOF flow control switched on */
  if (tty->termios.c_iflag & IXON) {
    tty->flow_ctrl |= FL_MDXON;
  }
}

rtems_status_code
rtems_termios_ioctl (void *arg)
{
  rtems_libio_ioctl_args_t *args = arg;
  struct rtems_termios_tty *tty = args->iop->data1;
  struct ttywakeup         *wakeup = (struct ttywakeup *)args->buffer;
  rtems_status_code sc;

  args->ioctl_return = 0;
  sc = rtems_semaphore_obtain (tty->osem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }
  switch (args->command) {
  default:
    if (rtems_termios_linesw[tty->t_line].l_ioctl != NULL) {
      sc = rtems_termios_linesw[tty->t_line].l_ioctl(tty,args);
    }
    else {
      sc = RTEMS_INVALID_NUMBER;
    }
    break;

  case RTEMS_IO_GET_ATTRIBUTES:
    *(struct termios *)args->buffer = tty->termios;
    break;

  case RTEMS_IO_SET_ATTRIBUTES:
    tty->termios = *(struct termios *)args->buffer;

    /* check for and process change in flow control options */
    termios_set_flowctrl(tty);

    if (tty->termios.c_lflag & ICANON) {
      tty->rawInBufSemaphoreOptions = RTEMS_WAIT;
      tty->rawInBufSemaphoreTimeout = RTEMS_NO_TIMEOUT;
      tty->rawInBufSemaphoreFirstTimeout = RTEMS_NO_TIMEOUT;
    } else {
      tty->vtimeTicks = tty->termios.c_cc[VTIME] * 
                    rtems_clock_get_ticks_per_second() / 10;
      if (tty->termios.c_cc[VTIME]) {
        tty->rawInBufSemaphoreOptions = RTEMS_WAIT;
        tty->rawInBufSemaphoreTimeout = tty->vtimeTicks;
        if (tty->termios.c_cc[VMIN])
          tty->rawInBufSemaphoreFirstTimeout = RTEMS_NO_TIMEOUT;
        else
          tty->rawInBufSemaphoreFirstTimeout = tty->vtimeTicks;
      } else {
        if (tty->termios.c_cc[VMIN]) {
          tty->rawInBufSemaphoreOptions = RTEMS_WAIT;
          tty->rawInBufSemaphoreTimeout = RTEMS_NO_TIMEOUT;
          tty->rawInBufSemaphoreFirstTimeout = RTEMS_NO_TIMEOUT;
        } else {
          tty->rawInBufSemaphoreOptions = RTEMS_NO_WAIT;
        }
      }
    }
    if (tty->device.setAttributes)
      (*tty->device.setAttributes)(tty->minor, &tty->termios);
    break;

  case RTEMS_IO_TCDRAIN:
    drainOutput (tty);
    break;

  case RTEMS_IO_SNDWAKEUP:
    tty->tty_snd = *wakeup;
    break;

  case RTEMS_IO_RCVWAKEUP:
    tty->tty_rcv = *wakeup;
    break;

    /*
     * FIXME: add various ioctl code handlers
     */

#if 1 /* FIXME */
  case TIOCSETD:
    /*
     * close old line discipline
     */
    if (rtems_termios_linesw[tty->t_line].l_close != NULL) {
      sc = rtems_termios_linesw[tty->t_line].l_close(tty);
    }
    tty->t_line=*(int*)(args->buffer);
    tty->t_sc = NULL; /* ensure that no more valid data */
    /*
     * open new line discipline
     */
    if (rtems_termios_linesw[tty->t_line].l_open != NULL) {
      sc = rtems_termios_linesw[tty->t_line].l_open(tty);
    }
    break;
  case TIOCGETD:
    *(int*)(args->buffer)=tty->t_line;
    break;
#endif
   case FIONREAD: {
      int rawnc = tty->rawInBuf.Tail - tty->rawInBuf.Head;
      if ( rawnc < 0 )
        rawnc += tty->rawInBuf.Size;
      /* Half guess that this is the right operation */
      *(int *)args->buffer = tty->ccount - tty->cindex + rawnc;
    }
    break;
  }

  rtems_semaphore_release (tty->osem);
  return sc;
}

/*
 * Send characters to device-specific code
 */
void
rtems_termios_puts (
  const void *_buf, size_t len, struct rtems_termios_tty *tty)
{
  const unsigned char *buf = _buf;
  unsigned int newHead;
  rtems_interrupt_level level;
  rtems_status_code sc;

  if (tty->device.outputUsesInterrupts == TERMIOS_POLLED) {
    (*tty->device.write)(tty->minor, buf, len);
    return;
  }
  newHead = tty->rawOutBuf.Head;
  while (len) {
    /*
     * Performance improvement could be made here.
     * Copy multiple bytes to raw buffer:
     * if (len > 1) && (space to buffer end, or tail > 1)
     *  ncopy = MIN (len, space to buffer end or tail)
     *  memcpy (raw buffer, buf, ncopy)
     *  buf += ncopy
     *  len -= ncopy
     *
     * To minimize latency, the memcpy should be done
     * with interrupts enabled.
     */
    newHead = (newHead + 1) % tty->rawOutBuf.Size;
    rtems_interrupt_disable (level);
    while (newHead == tty->rawOutBuf.Tail) {
      tty->rawOutBufState = rob_wait;
      rtems_interrupt_enable (level);
      sc = rtems_semaphore_obtain(
        tty->rawOutBuf.Semaphore, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
      if (sc != RTEMS_SUCCESSFUL)
        rtems_fatal_error_occurred (sc);
      rtems_interrupt_disable (level);
    }
    tty->rawOutBuf.theBuf[tty->rawOutBuf.Head] = *buf++;
    tty->rawOutBuf.Head = newHead;
    if (tty->rawOutBufState == rob_idle) {
      /* check, whether XOFF has been received */
      if (!(tty->flow_ctrl & FL_ORCVXOF)) {
        (*tty->device.write)(
          tty->minor, &tty->rawOutBuf.theBuf[tty->rawOutBuf.Tail],1);
      } else {
        /* remember that output has been stopped due to flow ctrl*/
        tty->flow_ctrl |= FL_OSTOP;
      }
      tty->rawOutBufState = rob_busy;
    }
    rtems_interrupt_enable (level);
    len--;
  }
}

/*
 * Handle output processing
 */
static void
oproc (unsigned char c, struct rtems_termios_tty *tty)
{
  int  i;

  if (tty->termios.c_oflag & OPOST) {
    switch (c) {
    case '\n':
      if (tty->termios.c_oflag & ONLRET)
        tty->column = 0;
      if (tty->termios.c_oflag & ONLCR) {
        rtems_termios_puts ("\r", 1, tty);
        tty->column = 0;
      }
      break;

    case '\r':
      if ((tty->termios.c_oflag & ONOCR) && (tty->column == 0))
        return;
      if (tty->termios.c_oflag & OCRNL) {
        c = '\n';
        if (tty->termios.c_oflag & ONLRET)
          tty->column = 0;
        break;
      }
      tty->column = 0;
      break;

    case '\t':
      i = 8 - (tty->column & 7);
      if ((tty->termios.c_oflag & TABDLY) == XTABS) {
        tty->column += i;
        rtems_termios_puts ( "        ",  i, tty);
        return;
      }
      tty->column += i;
      break;

    case '\b':
      if (tty->column > 0)
        tty->column--;
      break;

    default:
      if (tty->termios.c_oflag & OLCUC)
        c = toupper(c);
      if (!iscntrl(c))
        tty->column++;
      break;
    }
  }
  rtems_termios_puts (&c, 1, tty);
}

rtems_status_code
rtems_termios_write (void *arg)
{
  rtems_libio_rw_args_t *args = arg;
  struct rtems_termios_tty *tty = args->iop->data1;
  rtems_status_code sc;

  sc = rtems_semaphore_obtain (tty->osem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  if (sc != RTEMS_SUCCESSFUL)
    return sc;
  if (rtems_termios_linesw[tty->t_line].l_write != NULL) {
    sc = rtems_termios_linesw[tty->t_line].l_write(tty,args);
    rtems_semaphore_release (tty->osem);
    return sc;
  }
  if (tty->termios.c_oflag & OPOST) {
    uint32_t   count = args->count;
    char      *buffer = args->buffer;
    while (count--)
      oproc (*buffer++, tty);
    args->bytes_moved = args->count;
  } else {
    rtems_termios_puts (args->buffer, args->count, tty);
    args->bytes_moved = args->count;
  }
  rtems_semaphore_release (tty->osem);
  return sc;
}

/*
 * Echo a typed character
 */
static void
echo (unsigned char c, struct rtems_termios_tty *tty)
{
  if ((tty->termios.c_lflag & ECHOCTL) &&
       iscntrl(c) && (c != '\t') && (c != '\n')) {
    char echobuf[2];

    echobuf[0] = '^';
    echobuf[1] = c ^ 0x40;
    rtems_termios_puts (echobuf, 2, tty);
    tty->column += 2;
  } else {
    oproc (c, tty);
  }
}

/*
 * Erase a character or line
 * FIXME: Needs support for WERASE and ECHOPRT.
 * FIXME: Some of the tests should check for IEXTEN, too.
 */
static void
erase (struct rtems_termios_tty *tty, int lineFlag)
{
  if (tty->ccount == 0)
    return;
  if (lineFlag) {
    if (!(tty->termios.c_lflag & ECHO)) {
      tty->ccount = 0;
      return;
    }
    if (!(tty->termios.c_lflag & ECHOE)) {
      tty->ccount = 0;
      echo (tty->termios.c_cc[VKILL], tty);
      if (tty->termios.c_lflag & ECHOK)
        echo ('\n', tty);
      return;
    }
  }

  while (tty->ccount) {
    unsigned char c = tty->cbuf[--tty->ccount];

    if (tty->termios.c_lflag & ECHO) {
      if (!lineFlag && !(tty->termios.c_lflag & ECHOE)) {
        echo (tty->termios.c_cc[VERASE], tty);
      } else if (c == '\t') {
        int col = tty->read_start_column;
        int i = 0;

        /*
         * Find the character before the tab
         */
        while (i != tty->ccount) {
          c = tty->cbuf[i++];
          if (c == '\t') {
            col = (col | 7) + 1;
          } else if (iscntrl (c)) {
            if (tty->termios.c_lflag & ECHOCTL)
              col += 2;
          } else {
            col++;
          }
        }

        /*
         * Back up over the tab
         */
        while (tty->column > col) {
          rtems_termios_puts ("\b", 1, tty);
          tty->column--;
        }
      }
      else {
        if (iscntrl (c) && (tty->termios.c_lflag & ECHOCTL)) {
          rtems_termios_puts ("\b \b", 3, tty);
          if (tty->column)
            tty->column--;
        }
        if (!iscntrl (c) || (tty->termios.c_lflag & ECHOCTL)) {
          rtems_termios_puts ("\b \b", 3, tty);
          if (tty->column)
            tty->column--;
        }
      }
    }
    if (!lineFlag)
      break;
  }
}

/*
 * Process a single input character
 */
static int
iproc (unsigned char c, struct rtems_termios_tty *tty)
{
  if (tty->termios.c_iflag & ISTRIP)
    c &= 0x7f;

  if (tty->termios.c_iflag & IUCLC)
    c = tolower (c);

  if (c == '\r') {
    if (tty->termios.c_iflag & IGNCR)
      return 0;
    if (tty->termios.c_iflag & ICRNL)
      c = '\n';
  } else if ((c == '\n') && (tty->termios.c_iflag & INLCR)) {
    c = '\r';
  }

  if ((c != '\0') && (tty->termios.c_lflag & ICANON)) {
    if (c == tty->termios.c_cc[VERASE]) {
      erase (tty, 0);
      return 0;
    }
    else if (c == tty->termios.c_cc[VKILL]) {
      erase (tty, 1);
      return 0;
    }
    else if (c == tty->termios.c_cc[VEOF]) {
      return 1;
    } else if (c == '\n') {
      if (tty->termios.c_lflag & (ECHO | ECHONL))
        echo (c, tty);
      tty->cbuf[tty->ccount++] = c;
      return 1;
    } else if ((c == tty->termios.c_cc[VEOL]) ||
               (c == tty->termios.c_cc[VEOL2])) {
      if (tty->termios.c_lflag & ECHO)
        echo (c, tty);
      tty->cbuf[tty->ccount++] = c;
      return 1;
    }
  }

  /*
   * FIXME: Should do IMAXBEL handling somehow
   */
  if (tty->ccount < (CBUFSIZE-1)) {
    if (tty->termios.c_lflag & ECHO)
      echo (c, tty);
    tty->cbuf[tty->ccount++] = c;
  }
  return 0;
}

/*
 * Process input character, with semaphore.
 */
static int
siproc (unsigned char c, struct rtems_termios_tty *tty)
{
  int i;

  /*
   * Obtain output semaphore if character will be echoed
   */
  if (tty->termios.c_lflag & (ECHO|ECHOE|ECHOK|ECHONL|ECHOPRT|ECHOCTL|ECHOKE)) {
    rtems_semaphore_obtain (tty->osem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    i = iproc (c, tty);
    rtems_semaphore_release (tty->osem);
  }
  else {
    i = iproc (c, tty);
  }
  return i;
}

/*
 * Fill the input buffer by polling the device
 */
static rtems_status_code
fillBufferPoll (struct rtems_termios_tty *tty)
{
  int n;

  if (tty->termios.c_lflag & ICANON) {
    for (;;) {
      n = (*tty->device.pollRead)(tty->minor);
      if (n < 0) {
        rtems_task_wake_after (1);
      } else {
        if  (siproc (n, tty))
          break;
      }
    }
  } else {
    rtems_interval then, now;

    then = rtems_clock_get_ticks_since_boot();
    for (;;) {
      n = (*tty->device.pollRead)(tty->minor);
      if (n < 0) {
        if (tty->termios.c_cc[VMIN]) {
          if (tty->termios.c_cc[VTIME] && tty->ccount) {
            now = rtems_clock_get_ticks_since_boot();
            if ((now - then) > tty->vtimeTicks) {
              break;
            }
          }
        } else {
          if (!tty->termios.c_cc[VTIME])
            break;
          now = rtems_clock_get_ticks_since_boot();
          if ((now - then) > tty->vtimeTicks) {
            break;
          }
        }
        rtems_task_wake_after (1);
      } else {
        siproc (n, tty);
        if (tty->ccount >= tty->termios.c_cc[VMIN])
          break;
        if (tty->termios.c_cc[VMIN] && tty->termios.c_cc[VTIME])
          then = rtems_clock_get_ticks_since_boot();
      }
    }
  }
  return RTEMS_SUCCESSFUL;
}

/*
 * Fill the input buffer from the raw input queue
 */
static rtems_status_code
fillBufferQueue (struct rtems_termios_tty *tty)
{
  rtems_interval timeout = tty->rawInBufSemaphoreFirstTimeout;
  rtems_status_code sc;
  int               wait = 1;

  while ( wait ) {
    /*
     * Process characters read from raw queue
     */
    while ((tty->rawInBuf.Head != tty->rawInBuf.Tail) &&
                       (tty->ccount < (CBUFSIZE-1))) {
      unsigned char c;
      unsigned int newHead;

      newHead = (tty->rawInBuf.Head + 1) % tty->rawInBuf.Size;
      c = tty->rawInBuf.theBuf[newHead];
      tty->rawInBuf.Head = newHead;
      if(((tty->rawInBuf.Tail-newHead+tty->rawInBuf.Size)
          % tty->rawInBuf.Size)
         < tty->lowwater) {
        tty->flow_ctrl &= ~FL_IREQXOF;
        /* if tx stopped and XON should be sent... */
        if (((tty->flow_ctrl & (FL_MDXON | FL_ISNTXOF))
             ==                (FL_MDXON | FL_ISNTXOF))
            && ((tty->rawOutBufState == rob_idle)
          || (tty->flow_ctrl & FL_OSTOP))) {
          /* XON should be sent now... */
          (*tty->device.write)(
            tty->minor, (void *)&(tty->termios.c_cc[VSTART]), 1);
        } else if (tty->flow_ctrl & FL_MDRTS) {
          tty->flow_ctrl &= ~FL_IRTSOFF;
          /* activate RTS line */
          if (tty->device.startRemoteTx != NULL) {
            tty->device.startRemoteTx(tty->minor);
          }
        }
      }

      /* continue processing new character */
      if (tty->termios.c_lflag & ICANON) {
        if (siproc (c, tty))
          wait = 0;
      } else {
        siproc (c, tty);
        if (tty->ccount >= tty->termios.c_cc[VMIN])
          wait = 0;
      }
      timeout = tty->rawInBufSemaphoreTimeout;
    }

    /*
     * Wait for characters
     */
    if ( wait ) {
      sc = rtems_semaphore_obtain(
        tty->rawInBuf.Semaphore, tty->rawInBufSemaphoreOptions, timeout);
      if (sc != RTEMS_SUCCESSFUL)
        break;
    }
  }
  return RTEMS_SUCCESSFUL;
}

rtems_status_code
rtems_termios_read (void *arg)
{
  rtems_libio_rw_args_t *args = arg;
  struct rtems_termios_tty *tty = args->iop->data1;
  uint32_t   count = args->count;
  char      *buffer = args->buffer;
  rtems_status_code sc;

  sc = rtems_semaphore_obtain (tty->isem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  if (sc != RTEMS_SUCCESSFUL)
    return sc;

  if (rtems_termios_linesw[tty->t_line].l_read != NULL) {
    sc = rtems_termios_linesw[tty->t_line].l_read(tty,args);
    tty->tty_rcvwakeup = 0;
    rtems_semaphore_release (tty->isem);
    return sc;
  }

  if (tty->cindex == tty->ccount) {
    tty->cindex = tty->ccount = 0;
    tty->read_start_column = tty->column;
    if (tty->device.pollRead != NULL &&
        tty->device.outputUsesInterrupts == TERMIOS_POLLED)
      sc = fillBufferPoll (tty);
    else
      sc = fillBufferQueue (tty);

    if (sc != RTEMS_SUCCESSFUL)
      tty->cindex = tty->ccount = 0;
  }
  while (count && (tty->cindex < tty->ccount)) {
    *buffer++ = tty->cbuf[tty->cindex++];
    count--;
  }
  args->bytes_moved = args->count - count;
  tty->tty_rcvwakeup = 0;
  rtems_semaphore_release (tty->isem);
  return sc;
}

/*
 * signal receive interrupt to rx daemon
 * NOTE: This routine runs in the context of the
 *       device receive interrupt handler.
 */
void rtems_termios_rxirq_occured(struct rtems_termios_tty *tty)
{
  /*
   * send event to rx daemon task
   */
  rtems_event_send(tty->rxTaskId,TERMIOS_RX_PROC_EVENT);
}

/*
 * Place characters on raw queue.
 * NOTE: This routine runs in the context of the
 *       device receive interrupt handler.
 * Returns the number of characters dropped because of overflow.
 */
int
rtems_termios_enqueue_raw_characters (void *ttyp, const char *buf, int len)
{
  struct rtems_termios_tty *tty = ttyp;
  unsigned int newTail;
  char c;
  int dropped = 0;
  bool flow_rcv = false; /* true, if flow control char received */
  rtems_interrupt_level level;

  if (rtems_termios_linesw[tty->t_line].l_rint != NULL) {
    while (len--) {
      c = *buf++;
      rtems_termios_linesw[tty->t_line].l_rint(c,tty);
    }

    /*
     * check to see if rcv wakeup callback was set
     */
    if (( !tty->tty_rcvwakeup ) && ( tty->tty_rcv.sw_pfn != NULL )) {
      (*tty->tty_rcv.sw_pfn)(&tty->termios, tty->tty_rcv.sw_arg);
      tty->tty_rcvwakeup = 1;
        }
    return 0;
  }

  while (len--) {
    c = *buf++;
    /* FIXME: implement IXANY: any character restarts output */
    /* if incoming XON/XOFF controls outgoing stream: */
    if (tty->flow_ctrl & FL_MDXON) {
      /* if received char is V_STOP and V_START (both are equal value) */
      if (c == tty->termios.c_cc[VSTOP]) {
        if (c == tty->termios.c_cc[VSTART]) {
          /* received VSTOP and VSTART==VSTOP? */
          /* then toggle "stop output" status  */
          tty->flow_ctrl = tty->flow_ctrl ^ FL_ORCVXOF;
        }
        else {
          /* VSTOP received (other code than VSTART) */
          /* stop output                             */
          tty->flow_ctrl |= FL_ORCVXOF;
        }
        flow_rcv = true;
      }
      else if (c == tty->termios.c_cc[VSTART]) {
        /* VSTART received */
        /* restart output  */
        tty->flow_ctrl &= ~FL_ORCVXOF;
        flow_rcv = true;
      }
    }
    if (flow_rcv) {
      /* restart output according to FL_ORCVXOF flag */
      if ((tty->flow_ctrl & (FL_ORCVXOF | FL_OSTOP)) == FL_OSTOP) {
        /* disable interrupts    */
        rtems_interrupt_disable(level);
        tty->flow_ctrl &= ~FL_OSTOP;
        /* check for chars in output buffer (or rob_state?) */
        if (tty->rawOutBufState != rob_idle) {
          /* if chars available, call write function... */
          (*tty->device.write)(
            tty->minor, &tty->rawOutBuf.theBuf[tty->rawOutBuf.Tail], 1);
        }
        /* reenable interrupts */
        rtems_interrupt_enable(level);
      }
    } else {
      newTail = (tty->rawInBuf.Tail + 1) % tty->rawInBuf.Size;
      /* if chars_in_buffer > highwater                */
      rtems_interrupt_disable(level);
      if ((((newTail - tty->rawInBuf.Head + tty->rawInBuf.Size)
            % tty->rawInBuf.Size) > tty->highwater) &&
          !(tty->flow_ctrl & FL_IREQXOF)) {
        /* incoming data stream should be stopped */
        tty->flow_ctrl |= FL_IREQXOF;
        if ((tty->flow_ctrl & (FL_MDXOF | FL_ISNTXOF))
            ==                (FL_MDXOF             ) ) {
          if ((tty->flow_ctrl & FL_OSTOP) ||
              (tty->rawOutBufState == rob_idle)) {
            /* if tx is stopped due to XOFF or out of data */
            /*    call write function here                 */
            tty->flow_ctrl |= FL_ISNTXOF;
            (*tty->device.write)(tty->minor,
                (void *)&(tty->termios.c_cc[VSTOP]), 1);
          }
        } else if ((tty->flow_ctrl & (FL_MDRTS | FL_IRTSOFF)) == (FL_MDRTS) ) {
          tty->flow_ctrl |= FL_IRTSOFF;
          /* deactivate RTS line */
          if (tty->device.stopRemoteTx != NULL) {
            tty->device.stopRemoteTx(tty->minor);
          }
        }
      }

      /* reenable interrupts */
      rtems_interrupt_enable(level);

      if (newTail == tty->rawInBuf.Head) {
        dropped++;
      } else {
        tty->rawInBuf.theBuf[newTail] = c;
        tty->rawInBuf.Tail = newTail;

        /*
         * check to see if rcv wakeup callback was set
         */
        if (( !tty->tty_rcvwakeup ) && ( tty->tty_rcv.sw_pfn != NULL )) {
          (*tty->tty_rcv.sw_pfn)(&tty->termios, tty->tty_rcv.sw_arg);
          tty->tty_rcvwakeup = 1;
        }
      }
    }
  }

  tty->rawInBufDropped += dropped;
  rtems_semaphore_release (tty->rawInBuf.Semaphore);
  return dropped;
}

/*
 * in task-driven mode, this function is called in Tx task context
 * in interrupt-driven mode, this function is called in TxIRQ context
 */
static int
rtems_termios_refill_transmitter (struct rtems_termios_tty *tty)
{
  unsigned int newTail;
  int nToSend;
  rtems_interrupt_level level;
  int len;

  /* check for XOF/XON to send */
  if ((tty->flow_ctrl & (FL_MDXOF | FL_IREQXOF | FL_ISNTXOF))
      == (FL_MDXOF | FL_IREQXOF)) {
    /* XOFF should be sent now... */
    (*tty->device.write)(tty->minor, (void *)&(tty->termios.c_cc[VSTOP]), 1);

    rtems_interrupt_disable(level);
    tty->t_dqlen--;
    tty->flow_ctrl |= FL_ISNTXOF;
    rtems_interrupt_enable(level);

    nToSend = 1;

  } else if ((tty->flow_ctrl & (FL_IREQXOF | FL_ISNTXOF)) == FL_ISNTXOF) {
    /* NOTE: send XON even, if no longer in XON/XOFF mode... */
    /* XON should be sent now... */
    /*
     * FIXME: this .write call will generate another
     * dequeue callback. This will advance the "Tail" in the data
     * buffer, although the corresponding data is not yet out!
     * Therefore the dequeue "length" should be reduced by 1
     */
    (*tty->device.write)(tty->minor, (void *)&(tty->termios.c_cc[VSTART]), 1);

    rtems_interrupt_disable(level);
    tty->t_dqlen--;
    tty->flow_ctrl &= ~FL_ISNTXOF;
    rtems_interrupt_enable(level);

    nToSend = 1;
  } else {
    if ( tty->rawOutBuf.Head == tty->rawOutBuf.Tail ) {
      /*
       * buffer was empty
       */
      if (tty->rawOutBufState == rob_wait) {
        /*
         * this should never happen...
         */
        rtems_semaphore_release (tty->rawOutBuf.Semaphore);
      }
      return 0;
    }

    rtems_interrupt_disable(level);
    len = tty->t_dqlen;
    tty->t_dqlen = 0;
    rtems_interrupt_enable(level);

    newTail = (tty->rawOutBuf.Tail + len) % tty->rawOutBuf.Size;
    tty->rawOutBuf.Tail = newTail;
    if (tty->rawOutBufState == rob_wait) {
      /*
       * wake up any pending writer task
       */
      rtems_semaphore_release (tty->rawOutBuf.Semaphore);
    }

    if (newTail == tty->rawOutBuf.Head) {
      /*
       * Buffer has become empty
       */
      tty->rawOutBufState = rob_idle;
      nToSend = 0;

      /*
       * check to see if snd wakeup callback was set
       */
      if ( tty->tty_snd.sw_pfn != NULL) {
        (*tty->tty_snd.sw_pfn)(&tty->termios, tty->tty_snd.sw_arg);
      }
    }
    /* check, whether output should stop due to received XOFF */
    else if ((tty->flow_ctrl & (FL_MDXON | FL_ORCVXOF))
       ==                (FL_MDXON | FL_ORCVXOF)) {
      /* Buffer not empty, but output stops due to XOFF */
      /* set flag, that output has been stopped */
      rtems_interrupt_disable(level);
      tty->flow_ctrl |= FL_OSTOP;
      tty->rawOutBufState = rob_busy; /*apm*/
      rtems_interrupt_enable(level);
      nToSend = 0;
    } else {
      /*
       * Buffer not empty, start tranmitter
       */
      if (newTail > tty->rawOutBuf.Head)
        nToSend = tty->rawOutBuf.Size - newTail;
      else
        nToSend = tty->rawOutBuf.Head - newTail;
      /* when flow control XON or XOF, don't send blocks of data     */
      /* to allow fast reaction on incoming flow ctrl and low latency*/
      /* for outgoing flow control                                   */
      if (tty->flow_ctrl & (FL_MDXON | FL_MDXOF)) {
        nToSend = 1;
      }
      tty->rawOutBufState = rob_busy; /*apm*/
      (*tty->device.write)(
        tty->minor, &tty->rawOutBuf.theBuf[newTail], nToSend);
    }
    tty->rawOutBuf.Tail = newTail; /*apm*/
  }
  return nToSend;
}

/*
 * Characters have been transmitted
 * NOTE: This routine runs in the context of the
 *       device transmit interrupt handler.
 * The second argument is the number of characters transmitted so far.
 * This value will always be 1 for devices which generate an interrupt
 * for each transmitted character.
 * It returns number of characters left to transmit
 */
int
rtems_termios_dequeue_characters (void *ttyp, int len)
{
  struct rtems_termios_tty *tty = ttyp;
  rtems_status_code sc;

  /*
   * sum up character count already sent
   */
  tty->t_dqlen += len;

  if (tty->device.outputUsesInterrupts == TERMIOS_TASK_DRIVEN) {
    /*
     * send wake up to transmitter task
     */
    sc = rtems_event_send(tty->txTaskId, TERMIOS_TX_START_EVENT);
    if (sc != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred (sc);
    return 0; /* nothing to output in IRQ... */
  }

  if (tty->t_line == PPPDISC ) {
    /*
     * call any line discipline start function
     */
    if (rtems_termios_linesw[tty->t_line].l_start != NULL) {
      rtems_termios_linesw[tty->t_line].l_start(tty);
    }
    return 0; /* nothing to output in IRQ... */
  }

  return rtems_termios_refill_transmitter(tty);
}

/*
 * this task actually processes any transmit events
 */
static rtems_task rtems_termios_txdaemon(rtems_task_argument argument)
{
  struct rtems_termios_tty *tty = (struct rtems_termios_tty *)argument;
  rtems_event_set the_event;

  while (1) {
    /*
     * wait for rtems event
     */
    rtems_event_receive(
       (TERMIOS_TX_START_EVENT | TERMIOS_TX_TERMINATE_EVENT),
       RTEMS_EVENT_ANY | RTEMS_WAIT,
       RTEMS_NO_TIMEOUT,
       &the_event
    );
    if ((the_event & TERMIOS_TX_TERMINATE_EVENT) != 0) {
      tty->txTaskId = 0;
      rtems_task_delete(RTEMS_SELF);
    }

    /*
     * call any line discipline start function
     */
    if (rtems_termios_linesw[tty->t_line].l_start != NULL) {
      rtems_termios_linesw[tty->t_line].l_start(tty);
    }

    /*
     * try to push further characters to device
     */
    rtems_termios_refill_transmitter(tty);
  }
}

/*
 * this task actually processes any receive events
 */
static rtems_task rtems_termios_rxdaemon(rtems_task_argument argument)
{
  struct rtems_termios_tty *tty = (struct rtems_termios_tty *)argument;
  rtems_event_set the_event;
  int c;
  char c_buf;

  while (1) {
    /*
     * wait for rtems event
     */
    rtems_event_receive(
      (TERMIOS_RX_PROC_EVENT | TERMIOS_RX_TERMINATE_EVENT),
      RTEMS_EVENT_ANY | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &the_event
    );
    if ((the_event & TERMIOS_RX_TERMINATE_EVENT) != 0) {
      tty->rxTaskId = 0;
      rtems_task_delete(RTEMS_SELF);
    }

    /*
     * do something
     */
    c = tty->device.pollRead(tty->minor);
    if (c != EOF) {
      /*
       * pollRead did call enqueue on its own
       */
      c_buf = c;
      rtems_termios_enqueue_raw_characters ( tty,&c_buf,1);
    }
  }
}
