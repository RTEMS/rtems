/**
 * @file
 * TERMIOS serial line support
 */

/*
 *  Author:
 *    W. Eric Norum
 *    Saskatchewan Accelerator Laboratory
 *    University of Saskatchewan
 *    Saskatoon, Saskatchewan, CANADA
 *    eric@skatter.usask.ca
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/imfs.h>
#include <rtems/score/assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/fcntl.h>
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

extern rtems_id rtems_termios_ttyMutex;

static size_t rtems_termios_cbufsize = 256;
static size_t rtems_termios_raw_input_size = 256;
static size_t rtems_termios_raw_output_size = 64;

static const IMFS_node_control rtems_termios_imfs_node_control;

static struct rtems_termios_tty *rtems_termios_ttyHead;
static struct rtems_termios_tty *rtems_termios_ttyTail;

static RTEMS_CHAIN_DEFINE_EMPTY(rtems_termios_devices);

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

static rtems_status_code
rtems_termios_obtain (void)
{
  return rtems_semaphore_obtain (rtems_termios_ttyMutex, RTEMS_WAIT,
    RTEMS_NO_TIMEOUT);
}

static void
rtems_termios_release (void)
{
  rtems_status_code sc;

  sc = rtems_semaphore_release (rtems_termios_ttyMutex);
  _Assert (sc == RTEMS_SUCCESSFUL);
  (void) sc;
}

rtems_status_code rtems_termios_device_install(
  const char                         *device_file,
  const rtems_termios_device_handler *handler,
  const rtems_termios_device_flow    *flow,
  rtems_termios_device_context       *context
)
{
  rtems_termios_device_node *new_device_node;
  int rv;

  new_device_node = calloc (1, sizeof(*new_device_node));
  if (new_device_node == NULL) {
    return RTEMS_NO_MEMORY;
  }

  rtems_chain_initialize_node (&new_device_node->node);
  new_device_node->handler = handler;
  new_device_node->flow = flow;
  new_device_node->context = context;
  new_device_node->tty = NULL;

  rv = IMFS_make_generic_node(
    device_file,
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &rtems_termios_imfs_node_control,
    new_device_node
  );
  if (rv != 0) {
    free (new_device_node);
    return RTEMS_UNSATISFIED;
  }

  return RTEMS_SUCCESSFUL;
}

static rtems_termios_tty *
legacyContextToTTY (rtems_termios_device_context *ctx)
{
  return RTEMS_CONTAINER_OF (ctx, rtems_termios_tty, legacy_device_context);
}

static bool
rtems_termios_callback_firstOpen(
  rtems_termios_tty             *tty,
  rtems_termios_device_context  *ctx,
  struct termios                *term,
  rtems_libio_open_close_args_t *args
)
{
  (void) ctx;
  (void) term;

  (*tty->device.firstOpen) (tty->major, tty->minor, args);

  return true;
}

static void
rtems_termios_callback_lastClose(
  rtems_termios_tty             *tty,
  rtems_termios_device_context  *ctx,
  rtems_libio_open_close_args_t *args
)
{
  (void) ctx;

  (*tty->device.lastClose) (tty->major, tty->minor, args);
}

static int
rtems_termios_callback_pollRead (rtems_termios_device_context *ctx)
{
  rtems_termios_tty *tty = legacyContextToTTY (ctx);

  return (*tty->device.pollRead) (tty->minor);
}

static void
rtems_termios_callback_write(
  rtems_termios_device_context *ctx,
  const char                   *buf,
  size_t                        len
)
{
  rtems_termios_tty *tty = legacyContextToTTY (ctx);

  (*tty->device.write) (tty->minor, buf, len);
}

static bool
rtems_termios_callback_setAttributes(
  rtems_termios_device_context *ctx,
  const struct termios         *term
)
{
  rtems_termios_tty *tty = legacyContextToTTY (ctx);

  (*tty->device.setAttributes) (tty->minor, term);

  return true;
}

static void
rtems_termios_callback_stopRemoteTx (rtems_termios_device_context *ctx)
{
  rtems_termios_tty *tty = legacyContextToTTY (ctx);

  (*tty->device.stopRemoteTx) (tty->minor);
}

static void
rtems_termios_callback_startRemoteTx (rtems_termios_device_context *ctx)
{
  rtems_termios_tty *tty = legacyContextToTTY (ctx);

  (*tty->device.startRemoteTx) (tty->minor);
}

/*
 * Drain output queue
 */
static void
drainOutput (struct rtems_termios_tty *tty)
{
  rtems_termios_device_context *ctx = tty->device_context;
  rtems_interrupt_lock_context lock_context;
  rtems_status_code sc;

  if (tty->handler.mode != TERMIOS_POLLED) {
    rtems_termios_device_lock_acquire (ctx, &lock_context);
    while (tty->rawOutBuf.Tail != tty->rawOutBuf.Head) {
      tty->rawOutBufState = rob_wait;
      rtems_termios_device_lock_release (ctx, &lock_context);
      sc = rtems_semaphore_obtain(
        tty->rawOutBuf.Semaphore, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
      if (sc != RTEMS_SUCCESSFUL)
        rtems_fatal_error_occurred (sc);
      rtems_termios_device_lock_acquire (ctx, &lock_context);
    }
    rtems_termios_device_lock_release (ctx, &lock_context);
  }
}

static bool
needDeviceMutex (rtems_termios_tty *tty)
{
  return tty->handler.mode == TERMIOS_IRQ_SERVER_DRIVEN
    || tty->handler.mode == TERMIOS_TASK_DRIVEN;
}

static void
rtems_termios_destroy_tty (rtems_termios_tty *tty, void *arg, bool last_close)
{
  rtems_status_code sc;

  if (rtems_termios_linesw[tty->t_line].l_close != NULL) {
    /*
     * call discipline-specific close
     */
    (void) rtems_termios_linesw[tty->t_line].l_close(tty);
  } else if (last_close) {
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

  if (tty->handler.mode == TERMIOS_TASK_DRIVEN) {
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
  if (last_close && tty->handler.last_close)
     (*tty->handler.last_close)(tty, tty->device_context, arg);

  if (tty->device_node != NULL)
    tty->device_node->tty = NULL;

  rtems_semaphore_delete (tty->isem);
  rtems_semaphore_delete (tty->osem);
  rtems_semaphore_delete (tty->rawOutBuf.Semaphore);
  if ((tty->handler.poll_read == NULL) ||
      (tty->handler.mode == TERMIOS_TASK_DRIVEN))
    rtems_semaphore_delete (tty->rawInBuf.Semaphore);

  if (needDeviceMutex (tty)) {
    rtems_semaphore_delete (tty->device_context->lock.mutex);
  } else if (tty->device_context == &tty->legacy_device_context) {
    rtems_interrupt_lock_destroy (&tty->legacy_device_context.lock.interrupt);
  }

  free (tty->rawInBuf.theBuf);
  free (tty->rawOutBuf.theBuf);
  free (tty->cbuf);
  free (tty);
}

static void
deviceAcquireMutex(
  rtems_termios_device_context *ctx,
  rtems_interrupt_lock_context *lock_context
)
{
  rtems_status_code sc;

  sc = rtems_semaphore_obtain (ctx->lock.mutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  _Assert (sc == RTEMS_SUCCESSFUL);
  (void) sc;
}

static void
deviceReleaseMutex(
  rtems_termios_device_context *ctx,
  rtems_interrupt_lock_context *lock_context
)
{
  rtems_status_code sc;

  sc = rtems_semaphore_release (ctx->lock.mutex);
  _Assert (sc == RTEMS_SUCCESSFUL);
  (void) sc;
}

void
rtems_termios_device_lock_acquire_default(
  rtems_termios_device_context *ctx,
  rtems_interrupt_lock_context *lock_context
)
{
  rtems_interrupt_lock_acquire (&ctx->lock.interrupt, lock_context);
}

void
rtems_termios_device_lock_release_default(
  rtems_termios_device_context *ctx,
  rtems_interrupt_lock_context *lock_context
)
{
  rtems_interrupt_lock_release (&ctx->lock.interrupt, lock_context);
}

static rtems_termios_tty *
rtems_termios_open_tty(
  rtems_device_major_number      major,
  rtems_device_minor_number      minor,
  rtems_libio_open_close_args_t *args,
  rtems_termios_tty             *tty,
  rtems_termios_device_node     *device_node,
  const rtems_termios_callbacks *callbacks
)
{
  rtems_status_code sc;

  if (tty == NULL) {
    static char c = 'a';
    rtems_termios_device_context *ctx;

    /*
     * Create a new device
     */
    tty = calloc (1, sizeof (struct rtems_termios_tty));
    if (tty == NULL) {
      return NULL;
    }
    /*
     * allocate raw input buffer
     */
    tty->rawInBuf.Size = RAW_INPUT_BUFFER_SIZE;
    tty->rawInBuf.theBuf = malloc (tty->rawInBuf.Size);
    if (tty->rawInBuf.theBuf == NULL) {
            free(tty);
      return NULL;
    }
    /*
     * allocate raw output buffer
     */
    tty->rawOutBuf.Size = RAW_OUTPUT_BUFFER_SIZE;
    tty->rawOutBuf.theBuf = malloc (tty->rawOutBuf.Size);
    if (tty->rawOutBuf.theBuf == NULL) {
            free((void *)(tty->rawInBuf.theBuf));
            free(tty);
      return NULL;
    }
    /*
     * allocate cooked buffer
     */
    tty->cbuf  = malloc (CBUFSIZE);
    if (tty->cbuf == NULL) {
            free((void *)(tty->rawOutBuf.theBuf));
            free((void *)(tty->rawInBuf.theBuf));
            free(tty);
      return NULL;
    }
    /*
     * Initialize wakeup callbacks
     */
    tty->tty_snd.sw_pfn = NULL;
    tty->tty_snd.sw_arg = NULL;
    tty->tty_rcv.sw_pfn = NULL;
    tty->tty_rcv.sw_arg = NULL;
    tty->tty_rcvwakeup  = false;

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
    if (device_node != NULL) {
      device_node->tty = tty;
      tty->handler = *device_node->handler;

      if (device_node->flow != NULL) {
        tty->flow = *device_node->flow;
      }

      tty->device_node = device_node;
      tty->device_context = device_node->context;
      memset(&tty->device, 0, sizeof(tty->device));
    } else {
      tty->handler.first_open = callbacks->firstOpen != NULL ?
        rtems_termios_callback_firstOpen : NULL;
      tty->handler.last_close = callbacks->lastClose != NULL ?
        rtems_termios_callback_lastClose : NULL;
      tty->handler.poll_read = callbacks->pollRead != NULL ?
        rtems_termios_callback_pollRead : NULL;
      tty->handler.write = callbacks->write != NULL ?
        rtems_termios_callback_write : NULL;
      tty->handler.set_attributes = callbacks->setAttributes != NULL ?
        rtems_termios_callback_setAttributes : NULL;
      tty->flow.stop_remote_tx = callbacks->stopRemoteTx != NULL ?
        rtems_termios_callback_stopRemoteTx : NULL;
      tty->flow.start_remote_tx = callbacks->startRemoteTx != NULL ?
        rtems_termios_callback_startRemoteTx : NULL;
      tty->handler.mode = callbacks->outputUsesInterrupts;
      tty->device_context = NULL;
      tty->device_node = NULL;
      tty->device = *callbacks;
    }

    if (tty->device_context == NULL) {
      tty->device_context = &tty->legacy_device_context;
      rtems_termios_device_context_initialize (tty->device_context, "Termios");
    }

    ctx = tty->device_context;

    if (needDeviceMutex (tty)) {
      sc = rtems_semaphore_create (
        rtems_build_name ('T', 'l', 'k', c),
        1,
        RTEMS_BINARY_SEMAPHORE | RTEMS_INHERIT_PRIORITY | RTEMS_PRIORITY,
        0,
        &ctx->lock.mutex);
      if (sc != RTEMS_SUCCESSFUL) {
        rtems_fatal_error_occurred (sc);
      }

      ctx->lock_acquire = deviceAcquireMutex;
      ctx->lock_release = deviceReleaseMutex;
    } else {
      ctx->lock_acquire = rtems_termios_device_lock_acquire_default;
      ctx->lock_release = rtems_termios_device_lock_release_default;
    }

    /*
     * Create I/O tasks
     */
    if (tty->handler.mode == TERMIOS_TASK_DRIVEN) {
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
    if ((tty->handler.poll_read == NULL) ||
        (tty->handler.mode == TERMIOS_TASK_DRIVEN)){
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
    if (tty->handler.first_open && !(*tty->handler.first_open)(
        tty, tty->device_context, &tty->termios, args)) {
      rtems_termios_destroy_tty(tty, args, false);
      return NULL;
    }

    /*
     * start I/O tasks, if needed
     */
    if (tty->handler.mode == TERMIOS_TASK_DRIVEN) {
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

  return tty;
}

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
  struct rtems_termios_tty *tty;

  /*
   * See if the device has already been opened
   */
  sc = rtems_termios_obtain ();
  if (sc != RTEMS_SUCCESSFUL)
    return sc;

  for (tty = rtems_termios_ttyHead ; tty != NULL ; tty = tty->forw) {
    if ((tty->major == major) && (tty->minor == minor))
      break;
  }

  tty = rtems_termios_open_tty(
    major, minor, arg, tty, NULL, callbacks);
  if (tty == NULL) {
    rtems_termios_release ();
    return RTEMS_NO_MEMORY;
  }

  if (tty->refcount == 1) {
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
  }

  rtems_termios_release ();

  return RTEMS_SUCCESSFUL;
}

static void
flushOutput (struct rtems_termios_tty *tty)
{
  rtems_termios_device_context *ctx = tty->device_context;
  rtems_interrupt_lock_context lock_context;

  rtems_termios_device_lock_acquire (ctx, &lock_context);
  tty->rawOutBuf.Tail = 0;
  tty->rawOutBuf.Head = 0;
  tty->rawOutBufState = rob_idle;
  rtems_termios_device_lock_release (ctx, &lock_context);
}

static void
flushInput (struct rtems_termios_tty *tty)
{
  rtems_termios_device_context *ctx = tty->device_context;
  rtems_interrupt_lock_context lock_context;

  rtems_termios_device_lock_acquire (ctx, &lock_context);
  tty->rawInBuf.Tail = 0;
  tty->rawInBuf.Head = 0;
  rtems_termios_device_lock_release (ctx, &lock_context);
}

static void
rtems_termios_close_tty (rtems_termios_tty *tty, void *arg)
{
  if (--tty->refcount == 0) {
    rtems_termios_destroy_tty (tty, arg, true);
  }
}

rtems_status_code
rtems_termios_close (void *arg)
{
  rtems_status_code sc;
  rtems_libio_open_close_args_t *args = arg;
  struct rtems_termios_tty *tty = args->iop->data1;

  sc = rtems_termios_obtain ();
  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (sc);

  if (tty->refcount == 1) {
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
  }

  rtems_termios_close_tty (tty, arg);

  rtems_termios_release ();

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
  rtems_termios_device_context *ctx = tty->device_context;
  rtems_interrupt_lock_context lock_context;
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
      rtems_termios_device_lock_acquire (ctx, &lock_context);
      tty->flow_ctrl &= ~FL_OSTOP;
      /* check for chars in output buffer (or rob_state?) */
      if (tty->rawOutBufState != rob_idle) {
        /* if chars available, call write function... */
        (*tty->handler.write)(
          ctx, &tty->rawOutBuf.theBuf[tty->rawOutBuf.Tail],1);
      }
      /* reenable interrupts */
      rtems_termios_device_lock_release (ctx, &lock_context);
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
    if ((tty->flow_ctrl & FL_IRTSOFF) &&
        (tty->flow.start_remote_tx != NULL)) {
      tty->flow.start_remote_tx(ctx);
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
  int flags;

  args->ioctl_return = 0;
  sc = rtems_semaphore_obtain (tty->osem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }
  switch (args->command) {
  default:
    if (rtems_termios_linesw[tty->t_line].l_ioctl != NULL) {
      sc = rtems_termios_linesw[tty->t_line].l_ioctl(tty,args);
    } else if (tty->handler.ioctl) {
      args->ioctl_return = (*tty->handler.ioctl) (tty->device_context,
        args->command, args->buffer);
      sc = RTEMS_SUCCESSFUL;
    } else {
      sc = RTEMS_INVALID_NUMBER;
    }
    break;

  case TIOCGETA:
    *(struct termios *)args->buffer = tty->termios;
    break;

  case TIOCSETA:
  case TIOCSETAW:
  case TIOCSETAF:
    tty->termios = *(struct termios *)args->buffer;

    if (args->command == TIOCSETAW || args->command == TIOCSETAF) {
      drainOutput (tty);
      if (args->command == TIOCSETAF) {
        flushInput (tty);
      }
    }
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
    if (tty->handler.set_attributes) {
      sc = (*tty->handler.set_attributes)(tty->device_context, &tty->termios) ?
        RTEMS_SUCCESSFUL : RTEMS_IO_ERROR;
    }
    break;

  case TIOCDRAIN:
    drainOutput (tty);
    break;

  case TIOCFLUSH:
    flags = *((int *)args->buffer);

    if (flags == 0) {
      flags = FREAD | FWRITE;
    } else {
      flags &= FREAD | FWRITE;
    }
    if (flags & FWRITE) {
      flushOutput (tty);
    }
    if (flags & FREAD) {
      flushInput (tty);
    }
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
 * Send as many chars at once as possible to device-specific code.
 * If transmitting==true then assume transmission is already running and
 * an explicit write(0) is needed if output has to stop for flow control.
 */
static unsigned int
startXmit (
  struct rtems_termios_tty *tty,
  unsigned int newTail,
  bool transmitting
)
{
  unsigned int nToSend;

  tty->rawOutBufState = rob_busy;

  /* if XOFF was received, do not (re)start output */
  if (tty->flow_ctrl & FL_ORCVXOF) {
    /* set flag, that output has been stopped */
    tty->flow_ctrl |= FL_OSTOP;
    nToSend = 0;
    /* stop transmitter */
    if (transmitting) {
      (*tty->handler.write) (tty->device_context, NULL, 0);
    }
  } else {
    /* when flow control XON or XOF, don't send blocks of data     */
    /* to allow fast reaction on incoming flow ctrl and low latency*/
    /* for outgoing flow control                                   */
    if (tty->flow_ctrl & (FL_MDXON | FL_MDXOF))
      nToSend = 1;
    else if (newTail > tty->rawOutBuf.Head)
      nToSend = tty->rawOutBuf.Size - newTail;
    else
      nToSend = tty->rawOutBuf.Head - newTail;

    (*tty->handler.write)(
        tty->device_context, &tty->rawOutBuf.theBuf[newTail], nToSend);
  }

  return nToSend;
}

/*
 * Send characters to device-specific code
 */
static size_t
doTransmit (const char *buf, size_t len, rtems_termios_tty *tty,
            bool wait, bool nextWait)
{
  unsigned int newHead;
  rtems_termios_device_context *ctx = tty->device_context;
  rtems_interrupt_lock_context lock_context;
  rtems_status_code sc;
  size_t todo;

  if (tty->handler.mode == TERMIOS_POLLED) {
    (*tty->handler.write)(ctx, buf, len);
    return len;
  }

  todo = len;

  while (todo > 0) {
    size_t nToCopy;
    size_t nAvail;

    /* Check space for at least one char */
    newHead = tty->rawOutBuf.Head + 1;
    if (newHead >= tty->rawOutBuf.Size)
      newHead -= tty->rawOutBuf.Size;

    rtems_termios_device_lock_acquire (ctx, &lock_context);
    if (newHead == tty->rawOutBuf.Tail) {
      if (wait) {
        do {
          tty->rawOutBufState = rob_wait;
          rtems_termios_device_lock_release (ctx, &lock_context);
          sc = rtems_semaphore_obtain(
            tty->rawOutBuf.Semaphore, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
          if (sc != RTEMS_SUCCESSFUL)
            rtems_fatal_error_occurred (sc);
          rtems_termios_device_lock_acquire (ctx, &lock_context);
        } while (newHead == tty->rawOutBuf.Tail);
      } else {
        rtems_termios_device_lock_release (ctx, &lock_context);
        return len - todo;
      }
    }

    /* Determine free space up to current tail or end of ring buffer */
    nToCopy = todo;
    if (tty->rawOutBuf.Tail > tty->rawOutBuf.Head) {
      /* Available space is contiguous from Head to Tail */
      nAvail = tty->rawOutBuf.Tail - tty->rawOutBuf.Head - 1;
    } else {
      /* Available space wraps at buffer end. To keep it simple, utilize
         only the free space from Head to end during this iteration */
      nAvail = tty->rawOutBuf.Size - tty->rawOutBuf.Head;
      /* Head may not touch Tail after wraparound */
      if (tty->rawOutBuf.Tail == 0)
        nAvail--;
    }
    if (nToCopy > nAvail)
      nToCopy = nAvail;

    /* To minimize latency, the memcpy could be done
     * with interrupts enabled or with limit on nToCopy (TBD)
     */
    memcpy(&tty->rawOutBuf.theBuf[tty->rawOutBuf.Head], buf, nToCopy);

    newHead = tty->rawOutBuf.Head + nToCopy;
    if (newHead >= tty->rawOutBuf.Size)
      newHead -= tty->rawOutBuf.Size;
    tty->rawOutBuf.Head = newHead;

    if (tty->rawOutBufState == rob_idle) {
      startXmit (tty, tty->rawOutBuf.Tail, false);
    }

    rtems_termios_device_lock_release (ctx, &lock_context);

    buf += nToCopy;
    todo -= nToCopy;
    wait = nextWait;
  }

  return len;
}

void
rtems_termios_puts (
  const void *_buf, size_t len, struct rtems_termios_tty *tty)
{
  doTransmit (_buf, len, tty, true, true);
}

static bool
canTransmit (rtems_termios_tty *tty, bool wait, size_t len)
{
  rtems_termios_device_context *ctx;
  rtems_interrupt_lock_context lock_context;
  unsigned int capacity;

  if (wait || tty->handler.mode == TERMIOS_POLLED) {
    return true;
  }

  ctx = tty->device_context;
  rtems_termios_device_lock_acquire (ctx, &lock_context);
  capacity = (tty->rawOutBuf.Tail - tty->rawOutBuf.Head - 1) %
    tty->rawOutBuf.Size;
  rtems_termios_device_lock_release (ctx, &lock_context);
  return capacity >= len;
}

/*
 * Handle output processing
 */
static bool
oproc (unsigned char c, rtems_termios_tty *tty, bool wait)
{
  char buf[8];
  size_t len;

  buf[0] = c;
  len = 1;

  if (tty->termios.c_oflag & OPOST) {
    int oldColumn = tty->column;
    int columnAdj = 0;

    switch (c) {
    case '\n':
      if (tty->termios.c_oflag & ONLRET)
        columnAdj = -oldColumn;
      if (tty->termios.c_oflag & ONLCR) {
        len = 2;

        if (!canTransmit (tty, wait, len)) {
          return false;
        }

        columnAdj = -oldColumn;
        buf[0] = '\r';
        buf[1] = c;
      }
      break;

    case '\r':
      if ((tty->termios.c_oflag & ONOCR) && (oldColumn == 0))
        return true;
      if (tty->termios.c_oflag & OCRNL) {
        buf[0] = '\n';
        if (tty->termios.c_oflag & ONLRET)
          columnAdj = -oldColumn;
      } else {
        columnAdj = -oldColumn;
      }
      break;

    case '\t':
      columnAdj = 8 - (oldColumn & 7);
      if ((tty->termios.c_oflag & TABDLY) == XTABS) {
        int i;

        len = (size_t) columnAdj;

        if (!canTransmit (tty, wait, len)) {
          return false;
        }

        for (i = 0; i < columnAdj; ++i) {
          buf[i] = ' ';
        }
      }
      break;

    case '\b':
      if (oldColumn > 0)
        columnAdj = -1;
      break;

    default:
      if (tty->termios.c_oflag & OLCUC) {
        c = toupper(c);
        buf[0] = c;
      }
      if (!iscntrl(c))
        columnAdj = 1;
      break;
    }

    tty->column = oldColumn + columnAdj;
  }

  return doTransmit (buf, len, tty, wait, true) > 0;
}

static uint32_t
rtems_termios_write_tty (rtems_libio_t *iop, rtems_termios_tty *tty,
                         const char *buf, uint32_t len)
{
  bool wait = ((iop->flags & LIBIO_FLAGS_NO_DELAY) == 0);

  if (tty->termios.c_oflag & OPOST) {
    uint32_t todo = len;

    while (todo > 0) {
      if (!oproc (*buf, tty, wait)) {
        break;
      }

      ++buf;
      --todo;
      wait = false;
    }

    return len - todo;
  } else {
    return doTransmit (buf, len, tty, wait, false);
  }
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
  args->bytes_moved = rtems_termios_write_tty (args->iop, tty,
                                               args->buffer, args->count);
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
    doTransmit (echobuf, 2, tty, true, true);
    tty->column += 2;
  } else {
    oproc (c, tty, true);
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
          doTransmit ("\b", 1, tty, true, true);
          tty->column--;
        }
      }
      else {
        if (iscntrl (c) && (tty->termios.c_lflag & ECHOCTL)) {
          doTransmit ("\b \b", 3, tty, true, true);
          if (tty->column)
            tty->column--;
        }
        if (!iscntrl (c) || (tty->termios.c_lflag & ECHOCTL)) {
          doTransmit ("\b \b", 3, tty, true, true);
          if (tty->column)
            tty->column--;
        }
      }
    }
    if (!lineFlag)
      break;
  }
}

static unsigned char
iprocEarly (unsigned char c, rtems_termios_tty *tty)
{
  if (tty->termios.c_iflag & ISTRIP)
    c &= 0x7f;

  if (tty->termios.c_iflag & IUCLC)
    c = tolower (c);

  if (c == '\r') {
    if (tty->termios.c_iflag & ICRNL)
      c = '\n';
  } else if (c == '\n') {
    if (tty->termios.c_iflag & INLCR)
      c = '\r';
  }

  return c;
}

/*
 * Process a single input character
 */
static int
iproc (unsigned char c, struct rtems_termios_tty *tty)
{
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
    rtems_status_code sc;
    sc = rtems_semaphore_obtain (tty->osem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred (sc);
    i = iproc (c, tty);
    sc = rtems_semaphore_release (tty->osem);
    if (sc != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred (sc);
  }
  else {
    i = iproc (c, tty);
  }
  return i;
}

static int
siprocPoll (unsigned char c, rtems_termios_tty *tty)
{
  if (c == '\r' && (tty->termios.c_iflag & IGNCR) != 0) {
    return 0;
  }

  c = iprocEarly (c, tty);
  return siproc (c, tty);
}

/*
 * Fill the input buffer by polling the device
 */
static void
fillBufferPoll (struct rtems_termios_tty *tty)
{
  int n;

  if (tty->termios.c_lflag & ICANON) {
    for (;;) {
      n = (*tty->handler.poll_read)(tty->device_context);
      if (n < 0) {
        rtems_task_wake_after (1);
      } else {
        if  (siprocPoll (n, tty))
          break;
      }
    }
  } else {
    rtems_interval then, now;

    then = rtems_clock_get_ticks_since_boot();
    for (;;) {
      n = (*tty->handler.poll_read)(tty->device_context);
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
        siprocPoll (n, tty);
        if (tty->ccount >= tty->termios.c_cc[VMIN])
          break;
        if (tty->termios.c_cc[VMIN] && tty->termios.c_cc[VTIME])
          then = rtems_clock_get_ticks_since_boot();
      }
    }
  }
}

/*
 * Fill the input buffer from the raw input queue
 */
static void
fillBufferQueue (struct rtems_termios_tty *tty)
{
  rtems_termios_device_context *ctx = tty->device_context;
  rtems_interval timeout = tty->rawInBufSemaphoreFirstTimeout;
  bool wait = true;

  while ( wait ) {
    rtems_interrupt_lock_context lock_context;

    /*
     * Process characters read from raw queue
     */

    rtems_termios_device_lock_acquire (ctx, &lock_context);

    while ((tty->rawInBuf.Head != tty->rawInBuf.Tail) &&
                       (tty->ccount < (CBUFSIZE-1))) {
      unsigned char c;
      unsigned int newHead;

      newHead = (tty->rawInBuf.Head + 1) % tty->rawInBuf.Size;
      c = tty->rawInBuf.theBuf[newHead];
      tty->rawInBuf.Head = newHead;

      if(((tty->rawInBuf.Tail - newHead) % tty->rawInBuf.Size)
         < tty->lowwater) {
        tty->flow_ctrl &= ~FL_IREQXOF;
        /* if tx stopped and XON should be sent... */
        if (((tty->flow_ctrl & (FL_MDXON | FL_ISNTXOF))
             ==                (FL_MDXON | FL_ISNTXOF))
            && ((tty->rawOutBufState == rob_idle)
          || (tty->flow_ctrl & FL_OSTOP))) {
          /* XON should be sent now... */
          (*tty->handler.write)(
            tty->device_context, (void *)&(tty->termios.c_cc[VSTART]), 1);
        } else if (tty->flow_ctrl & FL_MDRTS) {
          tty->flow_ctrl &= ~FL_IRTSOFF;
          /* activate RTS line */
          if (tty->flow.start_remote_tx != NULL) {
            tty->flow.start_remote_tx(tty->device_context);
          }
        }
      }

      rtems_termios_device_lock_release (ctx, &lock_context);

      /* continue processing new character */
      if (tty->termios.c_lflag & ICANON) {
        if (siproc (c, tty))
          wait = false;
      } else {
        siproc (c, tty);
        if (tty->ccount >= tty->termios.c_cc[VMIN])
          wait = false;
      }
      timeout = tty->rawInBufSemaphoreTimeout;

      rtems_termios_device_lock_acquire (ctx, &lock_context);
    }

    rtems_termios_device_lock_release (ctx, &lock_context);

    /*
     * Wait for characters
     */
    if (wait) {
      if (tty->ccount < CBUFSIZE - 1) {
        rtems_status_code sc;

        sc = rtems_semaphore_obtain(
          tty->rawInBuf.Semaphore, tty->rawInBufSemaphoreOptions, timeout);
        if (sc != RTEMS_SUCCESSFUL)
          break;
      } else {
        break;
      }
    }
  }
}

static uint32_t
rtems_termios_read_tty (struct rtems_termios_tty *tty, char *buffer,
  uint32_t initial_count)
{
  uint32_t count;

  count = initial_count;

  if (tty->cindex == tty->ccount) {
    tty->cindex = tty->ccount = 0;
    tty->read_start_column = tty->column;
    if (tty->handler.poll_read != NULL && tty->handler.mode == TERMIOS_POLLED)
      fillBufferPoll (tty);
    else
      fillBufferQueue (tty);
  }
  while (count && (tty->cindex < tty->ccount)) {
    *buffer++ = tty->cbuf[tty->cindex++];
    count--;
  }
  tty->tty_rcvwakeup = false;
  return initial_count - count;
}

rtems_status_code
rtems_termios_read (void *arg)
{
  rtems_libio_rw_args_t *args = arg;
  struct rtems_termios_tty *tty = args->iop->data1;
  rtems_status_code sc;

  sc = rtems_semaphore_obtain (tty->isem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  if (sc != RTEMS_SUCCESSFUL)
    return sc;

  if (rtems_termios_linesw[tty->t_line].l_read != NULL) {
    sc = rtems_termios_linesw[tty->t_line].l_read(tty,args);
    tty->tty_rcvwakeup = false;
    rtems_semaphore_release (tty->isem);
    return sc;
  }

  args->bytes_moved = rtems_termios_read_tty (tty, args->buffer, args->count);
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

static bool
mustCallReceiveCallback (const rtems_termios_tty *tty, unsigned char c,
                         unsigned int newTail, unsigned int head)
{
  if ((tty->termios.c_lflag & ICANON) != 0) {
    return c == '\n' || c == tty->termios.c_cc[VEOF] ||
      c == tty->termios.c_cc[VEOL] || c == tty->termios.c_cc[VEOL2];
  } else {
    unsigned int rawContentSize = (newTail - head) % tty->rawInBuf.Size;

    return rawContentSize >= tty->termios.c_cc[VMIN];
  }
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
  char c;
  int dropped = 0;
  bool flow_rcv = false; /* true, if flow control char received */
  rtems_termios_device_context *ctx = tty->device_context;
  rtems_interrupt_lock_context lock_context;

  if (rtems_termios_linesw[tty->t_line].l_rint != NULL) {
    while (len--) {
      c = *buf++;
      rtems_termios_linesw[tty->t_line].l_rint(c,tty);
    }

    /*
     * check to see if rcv wakeup callback was set
     */
    if (tty->tty_rcv.sw_pfn != NULL && !tty->tty_rcvwakeup) {
      tty->tty_rcvwakeup = true;
      (*tty->tty_rcv.sw_pfn)(&tty->termios, tty->tty_rcv.sw_arg);
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
        rtems_termios_device_lock_acquire (ctx, &lock_context);
        tty->flow_ctrl &= ~FL_OSTOP;
        /* check for chars in output buffer (or rob_state?) */
        if (tty->rawOutBufState != rob_idle) {
          /* if chars available, call write function... */
          (*tty->handler.write)(
            ctx, &tty->rawOutBuf.theBuf[tty->rawOutBuf.Tail], 1);
        }
        /* reenable interrupts */
        rtems_termios_device_lock_release (ctx, &lock_context);
      }
    } else {
      unsigned int head;
      unsigned int oldTail;
      unsigned int newTail;
      bool callReciveCallback;

      if (c == '\r' && (tty->termios.c_iflag & IGNCR) != 0) {
        continue;
      }

      c = iprocEarly (c, tty);

      rtems_termios_device_lock_acquire (ctx, &lock_context);

      head = tty->rawInBuf.Head;
      oldTail = tty->rawInBuf.Tail;
      newTail = (oldTail + 1) % tty->rawInBuf.Size;

      /* if chars_in_buffer > highwater                */
      if ((tty->flow_ctrl & FL_IREQXOF) != 0 && (((newTail - head) %
          tty->rawInBuf.Size) > tty->highwater)) {
        /* incoming data stream should be stopped */
        tty->flow_ctrl |= FL_IREQXOF;
        if ((tty->flow_ctrl & (FL_MDXOF | FL_ISNTXOF))
            ==                (FL_MDXOF             ) ) {
          if ((tty->flow_ctrl & FL_OSTOP) ||
              (tty->rawOutBufState == rob_idle)) {
            /* if tx is stopped due to XOFF or out of data */
            /*    call write function here                 */
            tty->flow_ctrl |= FL_ISNTXOF;
            (*tty->handler.write)(ctx,
                (void *)&(tty->termios.c_cc[VSTOP]), 1);
          }
        } else if ((tty->flow_ctrl & (FL_MDRTS | FL_IRTSOFF)) == (FL_MDRTS) ) {
          tty->flow_ctrl |= FL_IRTSOFF;
          /* deactivate RTS line */
          if (tty->flow.stop_remote_tx != NULL) {
            tty->flow.stop_remote_tx(ctx);
          }
        }
      }

      callReciveCallback = false;

      if (newTail != head) {
        tty->rawInBuf.theBuf[newTail] = c;
        tty->rawInBuf.Tail = newTail;

        /*
         * check to see if rcv wakeup callback was set
         */
        if (tty->tty_rcv.sw_pfn != NULL && !tty->tty_rcvwakeup) {
          if (mustCallReceiveCallback (tty, c, newTail, head)) {
            tty->tty_rcvwakeup = true;
            callReciveCallback = true;
          }
        }
      } else {
        ++dropped;

        if (tty->tty_rcv.sw_pfn != NULL && !tty->tty_rcvwakeup) {
          tty->tty_rcvwakeup = true;
          callReciveCallback = true;
        }
      }

      rtems_termios_device_lock_release (ctx, &lock_context);

      if (callReciveCallback) {
        (*tty->tty_rcv.sw_pfn)(&tty->termios, tty->tty_rcv.sw_arg);
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
  bool wakeUpWriterTask = false;
  unsigned int newTail;
  int nToSend;
  rtems_termios_device_context *ctx = tty->device_context;
  rtems_interrupt_lock_context lock_context;
  int len;

  rtems_termios_device_lock_acquire (ctx, &lock_context);

  /* check for XOF/XON to send */
  if ((tty->flow_ctrl & (FL_MDXOF | FL_IREQXOF | FL_ISNTXOF))
      == (FL_MDXOF | FL_IREQXOF)) {
    /* XOFF should be sent now... */
    (*tty->handler.write)(ctx, (void *)&(tty->termios.c_cc[VSTOP]), 1);

    tty->t_dqlen--;
    tty->flow_ctrl |= FL_ISNTXOF;

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
    (*tty->handler.write)(ctx, (void *)&(tty->termios.c_cc[VSTART]), 1);

    tty->t_dqlen--;
    tty->flow_ctrl &= ~FL_ISNTXOF;

    nToSend = 1;
  } else if ( tty->rawOutBuf.Head == tty->rawOutBuf.Tail ) {
    /*
     * buffer was empty
     */
    if (tty->rawOutBufState == rob_wait) {
      /*
       * this should never happen...
       */
      wakeUpWriterTask = true;
    }

    (*tty->handler.write) (ctx, NULL, 0);
    nToSend = 0;
  } else {
    len = tty->t_dqlen;
    tty->t_dqlen = 0;

    newTail = (tty->rawOutBuf.Tail + len) % tty->rawOutBuf.Size;
    tty->rawOutBuf.Tail = newTail;
    if (tty->rawOutBufState == rob_wait) {
      /*
       * wake up any pending writer task
       */
      wakeUpWriterTask = true;
    }

    if (newTail == tty->rawOutBuf.Head) {
      /*
       * Buffer has become empty
       */
      tty->rawOutBufState = rob_idle;
      (*tty->handler.write) (ctx, NULL, 0);
      nToSend = 0;

      /*
       * check to see if snd wakeup callback was set
       */
      if ( tty->tty_snd.sw_pfn != NULL) {
        (*tty->tty_snd.sw_pfn)(&tty->termios, tty->tty_snd.sw_arg);
      }
    } else {
      /*
       * Buffer not empty, check flow control, start transmitter
       */
      nToSend = startXmit (tty, newTail, true);
    }
  }

  rtems_termios_device_lock_release (ctx, &lock_context);

  if (wakeUpWriterTask) {
    rtems_semaphore_release (tty->rawOutBuf.Semaphore);
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

  if (tty->handler.mode == TERMIOS_TASK_DRIVEN) {
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
     * call PPP line discipline start function
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

      if (tty->t_line == PPPDISC) {
        /*
         * Do not call rtems_termios_refill_transmitter() in this case similar
         * to rtems_termios_dequeue_characters().
         */
        continue;
      }
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
  rtems_termios_device_context *ctx = tty->device_context;
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
    c = tty->handler.poll_read(ctx);
    if (c != EOF) {
      /*
       * poll_read did call enqueue on its own
       */
      c_buf = c;
      rtems_termios_enqueue_raw_characters ( tty,&c_buf,1);
    }
  }
}

static int
rtems_termios_imfs_open (rtems_libio_t *iop,
  const char *path, int oflag, mode_t mode)
{
  rtems_termios_device_node *device_node;
  rtems_status_code sc;
  rtems_libio_open_close_args_t args;
  struct rtems_termios_tty *tty;

  device_node = IMFS_generic_get_context_by_iop (iop);

  memset (&args, 0, sizeof (args));
  args.iop = iop;
  args.flags = iop->flags;
  args.mode = mode;

  sc = rtems_termios_obtain ();
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_set_errno_and_return_minus_one (ENXIO);
  }

  tty = rtems_termios_open_tty (device_node->major, device_node->minor, &args,
      device_node->tty, device_node, NULL);
  if (tty == NULL) {
    rtems_termios_release ();
    rtems_set_errno_and_return_minus_one (ENOMEM);
  }

  rtems_termios_release ();
  return 0;
}

static int
rtems_termios_imfs_close (rtems_libio_t *iop)
{
  rtems_status_code sc;
  rtems_libio_open_close_args_t args;
  struct rtems_termios_tty *tty;

  memset (&args, 0, sizeof (args));
  args.iop = iop;

  tty = iop->data1;

  sc = rtems_termios_obtain ();
  _Assert (sc == RTEMS_SUCCESSFUL);
  (void) sc;

  rtems_termios_close_tty (tty, &args);
  rtems_termios_release ();
  return 0;
}

static ssize_t
rtems_termios_imfs_read (rtems_libio_t *iop, void *buffer, size_t count)
{
  struct rtems_termios_tty *tty;
  rtems_status_code sc;
  uint32_t bytes_moved;

  tty = iop->data1;

  sc = rtems_semaphore_obtain (tty->isem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  _Assert (sc == RTEMS_SUCCESSFUL);

  if (rtems_termios_linesw[tty->t_line].l_read != NULL) {
    rtems_libio_rw_args_t args;

    memset (&args, 0, sizeof (args));
    args.iop = iop;
    args.buffer = buffer;
    args.count = count;
    args.flags = iop->flags;

    sc = rtems_termios_linesw[tty->t_line].l_read (tty, &args);
    tty->tty_rcvwakeup = false;
    rtems_semaphore_release (tty->isem);

    if (sc != RTEMS_SUCCESSFUL) {
      return rtems_status_code_to_errno (sc);
    }

    return (ssize_t) args.bytes_moved;
  }

  bytes_moved = rtems_termios_read_tty (tty, buffer, count);
  rtems_semaphore_release (tty->isem);
  return (ssize_t) bytes_moved;
}

static ssize_t
rtems_termios_imfs_write (rtems_libio_t *iop, const void *buffer, size_t count)
{
  struct rtems_termios_tty *tty;
  rtems_status_code sc;
  uint32_t bytes_moved;

  tty = iop->data1;

  sc = rtems_semaphore_obtain (tty->osem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  _Assert (sc == RTEMS_SUCCESSFUL);

  if (rtems_termios_linesw[tty->t_line].l_write != NULL) {
    rtems_libio_rw_args_t args;

    memset (&args, 0, sizeof (args));
    args.iop = iop;
    args.buffer = RTEMS_DECONST (void *, buffer);
    args.count = count;
    args.flags = iop->flags;

    sc = rtems_termios_linesw[tty->t_line].l_write (tty, &args);
    rtems_semaphore_release (tty->osem);

    if (sc != RTEMS_SUCCESSFUL) {
      return rtems_status_code_to_errno (sc);
    }

    return (ssize_t) args.bytes_moved;
  }

  bytes_moved = rtems_termios_write_tty (iop, tty, buffer, count);
  rtems_semaphore_release (tty->osem);
  return (ssize_t) bytes_moved;
}

static int
rtems_termios_imfs_ioctl (rtems_libio_t *iop, uint32_t request, void *buffer)
{
  rtems_status_code sc;
  rtems_libio_ioctl_args_t args;

  memset (&args, 0, sizeof (args));
  args.iop = iop;
  args.command = request;
  args.buffer = buffer;

  sc = rtems_termios_ioctl (&args);
  if ( sc == RTEMS_SUCCESSFUL ) {
    return args.ioctl_return;
  } else {
    return rtems_status_code_to_errno (sc);
  }
}

static const rtems_filesystem_file_handlers_r rtems_termios_imfs_handler = {
  .open_h = rtems_termios_imfs_open,
  .close_h = rtems_termios_imfs_close,
  .read_h = rtems_termios_imfs_read,
  .write_h = rtems_termios_imfs_write,
  .ioctl_h = rtems_termios_imfs_ioctl,
  .lseek_h = rtems_filesystem_default_lseek,
  .fstat_h = IMFS_stat,
  .ftruncate_h = rtems_filesystem_default_ftruncate,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fcntl_h = rtems_filesystem_default_fcntl,
  .kqfilter_h = rtems_termios_kqfilter,
  .poll_h = rtems_termios_poll,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev
};

static IMFS_jnode_t *
rtems_termios_imfs_node_initialize (IMFS_jnode_t *node, void *arg)
{
  rtems_termios_device_node *device_node;
  dev_t dev;

  node = IMFS_node_initialize_generic (node, arg);
  device_node = IMFS_generic_get_context_by_node (node);
  dev = IMFS_generic_get_device_identifier_by_node (node);
  device_node->major = rtems_filesystem_dev_major_t (dev);
  device_node->minor = rtems_filesystem_dev_minor_t (dev);

  return node;
}

static void
rtems_termios_imfs_node_destroy (IMFS_jnode_t *node)
{
  rtems_termios_device_node *device_node;

  device_node = IMFS_generic_get_context_by_node (node);
  free (device_node);
  IMFS_node_destroy_default (node);
}

static const IMFS_node_control rtems_termios_imfs_node_control =
  IMFS_GENERIC_INITIALIZER(
    &rtems_termios_imfs_handler,
    rtems_termios_imfs_node_initialize,
    rtems_termios_imfs_node_destroy
  );
