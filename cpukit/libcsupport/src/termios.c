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
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems.h>
#include <rtems/libio.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*
 * The size of the cooked buffer
 */
#define CBUFSIZE  256

/*
 * The size of the raw input message queue
 */
#define RAW_MESSAGE_QUEUE_COUNT 10
#define RAW_MESSAGE_QUEUE_SIZE  16

/*
 * Variables associated with each termios instance.
 * One structure for each hardware I/O device.
 */
struct rtems_termios_tty {
  /*
   * Linked-list of active TERMIOS devices
   */
  struct rtems_termios_tty  *forw;
  struct rtems_termios_tty  *back;

  /*
   * How many times has this device been opened
   */
  int   refcount;

  /*
   * This device
   */
  rtems_device_major_number major;
  rtems_device_major_number minor;

  /*
   * Mutual-exclusion semaphores
   */
  rtems_id  isem;
  rtems_id  osem;

  /*
   * The canonical (cooked) character buffer
   */
  char    cbuf[CBUFSIZE];
  int   ccount;
  int   cindex;

  /*
   * Keep track of cursor (printhead) position
   */
  int   column;
  int   read_start_column;

  /*
   * The ioctl settings
   */
  struct termios  termios;
  rtems_interval  vtimeTicks;

  /*
   * Raw character buffer
   */
  rtems_id  rawInputQueue;
  char    rawBuf[RAW_MESSAGE_QUEUE_SIZE];
  int   rawBufCount;
  int   rawBufIndex;
  rtems_unsigned32  rawMessageOptions;
  rtems_interval  rawMessageTimeout;
  rtems_interval  rawMessageFirstTimeout;

  /*
   * Callbacks to device-specific routines
   */
  int   (*lastClose)(int major, int minor, void *arg);
  int   (*read)(int minor, char *buf /*, int len */);
  int   (*write)(int minor, char *buf, int len);
};
static struct rtems_termios_tty *ttyHead, *ttyTail;
static rtems_id ttyMutex;

void
rtems_termios_initialize (void)
{
  rtems_status_code sc;

  /*
   * Create the mutex semaphore for the tty list
   */
  if (!ttyMutex) {
    sc = rtems_semaphore_create (
      rtems_build_name ('T', 'R', 'm', 'i'),
      1,
      RTEMS_BINARY_SEMAPHORE | RTEMS_INHERIT_PRIORITY | RTEMS_PRIORITY,
      RTEMS_NO_PRIORITY,
      &ttyMutex);
    if (sc != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred (sc);
  }
}
  
/*
 * Open a termios device
 */
rtems_status_code
rtems_termios_open (
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg,
  int                      (*deviceFirstOpen)(int major, int minor, void *arg),
  int                      (*deviceLastClose)(int major, int minor, void *arg),
  int                      (*deviceRead)(int minor, char *buf/*, int len*/),
  int                      (*deviceWrite)(int minor, char *buf, int len)
  )
{
  rtems_status_code sc;
  rtems_libio_open_close_args_t *args = arg;
  struct rtems_termios_tty *tty;

  /*
   * See if the device has already been opened
   */
  sc = rtems_semaphore_obtain (ttyMutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  if (sc != RTEMS_SUCCESSFUL)
    return sc;
  for (tty = ttyHead ; tty != NULL ; tty = tty->forw) {
    if ((tty->major == major) && (tty->minor == minor))
      break;
  }
  if (tty == NULL) {
    static char c = 'a';

    /*
     * Create a new device
     */
    tty = malloc (sizeof (struct rtems_termios_tty));
    if (tty == NULL) {
      rtems_semaphore_release (ttyMutex);
      return RTEMS_NO_MEMORY;
    }
    tty->forw = ttyHead;
    ttyHead = tty;
    tty->back = NULL;
    if (ttyTail == NULL)
      ttyTail = tty;

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

    /*
     * Set callbacks
     */
    tty->write = deviceWrite;
    tty->lastClose = deviceLastClose;
    if ((tty->read = deviceRead) == NULL) {
      sc = rtems_message_queue_create (
        rtems_build_name ('T', 'R', 'i', c),
        RAW_MESSAGE_QUEUE_COUNT,
        RAW_MESSAGE_QUEUE_SIZE,
        RTEMS_FIFO | RTEMS_LOCAL,
        &tty->rawInputQueue);
      if (sc != RTEMS_SUCCESSFUL)
        rtems_fatal_error_occurred (sc);
      tty->rawBufCount = tty->rawBufIndex = 0;
    }

    /*
     * Initialize variables
     */
    tty->column = 0;
    tty->cindex = tty->ccount = 0;

    /*
     * Set default parameters
     */
    tty->termios.c_iflag = BRKINT | ICRNL | IXON | IMAXBEL;
    tty->termios.c_oflag = OPOST | ONLCR | XTABS;
    tty->termios.c_cflag = B9600 | CS8 | CREAD;
    tty->termios.c_lflag = ISIG | ICANON | IEXTEN | ECHO | ECHOK | ECHOE | ECHOCTL;
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

    /*
     * Device-specific open
     */
    if (deviceFirstOpen)
      (*deviceFirstOpen) (major, minor, arg);

    /*
     * Bump name characer
     */
    if (c++ == 'z')
      c = 'a';
  }
  tty->refcount++;
  args->iop->data1 = tty;
  rtems_semaphore_release (ttyMutex);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code
rtems_termios_close (void *arg)
{
  rtems_libio_open_close_args_t *args = arg;
  struct rtems_termios_tty *tty = args->iop->data1;
  rtems_status_code sc;

  sc = rtems_semaphore_obtain (ttyMutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (sc);
  if (--tty->refcount == 0) {
    if (tty->lastClose)
       (*tty->lastClose) (tty->major, tty->minor, arg);
    if (tty->forw == NULL)
      ttyTail = tty->back;
    else
      tty->forw->back = tty->back;
    if (tty->back == NULL)
      ttyHead = tty->forw;
    else
      tty->back->forw = tty->forw;
    rtems_semaphore_delete (tty->isem);
    rtems_semaphore_delete (tty->osem);
    if (tty->read == NULL)
      rtems_message_queue_delete (tty->rawInputQueue);
    free (tty);
  }
  rtems_semaphore_release (ttyMutex);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code
rtems_termios_ioctl (void *arg)
{
  rtems_libio_ioctl_args_t *args = arg;
  struct rtems_termios_tty *tty = args->iop->data1;
  rtems_status_code sc;

  sc = rtems_semaphore_obtain (tty->osem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  if (sc != RTEMS_SUCCESSFUL)
    return sc;
  switch (args->command) {
  default:
    sc = RTEMS_INVALID_NUMBER;
    break;

  case RTEMS_IO_GET_ATTRIBUTES:
    *(struct termios *)args->buffer = tty->termios;
    break;

  case RTEMS_IO_SET_ATTRIBUTES:
    tty->termios = *(struct termios *)args->buffer;
    if (tty->termios.c_lflag & ICANON) {
      tty->rawMessageOptions = RTEMS_WAIT;
      tty->rawMessageTimeout = RTEMS_NO_TIMEOUT;
      tty->rawMessageFirstTimeout = RTEMS_NO_TIMEOUT;
    }
    else {
      rtems_interval ticksPerSecond;
      rtems_clock_get (RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticksPerSecond);
      tty->vtimeTicks = tty->termios.c_cc[VTIME] * ticksPerSecond / 10;
      if (tty->termios.c_cc[VTIME]) {
        tty->rawMessageOptions = RTEMS_WAIT;
        tty->rawMessageTimeout = tty->vtimeTicks;
        if (tty->termios.c_cc[VMIN])
          tty->rawMessageFirstTimeout = RTEMS_NO_TIMEOUT;
        else
          tty->rawMessageFirstTimeout = tty->vtimeTicks;
      }
      else {
        if (tty->termios.c_cc[VMIN]) {
          tty->rawMessageOptions = RTEMS_WAIT;
          tty->rawMessageTimeout = RTEMS_NO_TIMEOUT;
          tty->rawMessageFirstTimeout = RTEMS_NO_TIMEOUT;
        }
        else {
          tty->rawMessageOptions = RTEMS_NO_WAIT;
        }
      }
    }
    break;
  }
  rtems_semaphore_release (tty->osem);
  return sc;
}

/*
 * Handle output processing
 */
static void
oproc (unsigned char c, struct rtems_termios_tty *tty)
{
  int i;

  if (tty->termios.c_oflag & OPOST) {
    switch (c) {
    case '\n':
      if (tty->termios.c_oflag & ONLRET)
        tty->column = 0;
      if (tty->termios.c_oflag & ONLCR) {
        (*tty->write)(tty->minor, "\r", 1);
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
        (*tty->write)(tty->minor,  "        ",  i);
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
  (*tty->write)(tty->minor, &c, 1);
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
  if (tty->termios.c_oflag & OPOST) {
    unsigned32 count = args->count;
    unsigned8 *buffer = args->buffer;
    while (count--)
      oproc (*buffer++, tty);
    args->bytes_moved = args->count;
  }
  else {
    if ((*tty->write)(tty->minor, args->buffer, args->count) < 0)
      sc = RTEMS_UNSATISFIED;
    else
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
  if ((tty->termios.c_lflag & ECHOCTL) && iscntrl(c) && (c != '\t') && (c != '\n')) {
    char echobuf[2];

    echobuf[0] = '^';
    echobuf[1] = c ^ 0x40;
    (*tty->write)(tty->minor, echobuf, 2);
    tty->column += 2;
  }
  else {
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
      }
      else if (c == '\t') {
        int col = tty->read_start_column;
        int i = 0;

        /*
         * Find the character before the tab
         */
        while (i != tty->ccount) {
          c = tty->cbuf[i++];
          if (c == '\t') {
            col = (col | 7) + 1;
          }
          else if (iscntrl (c)) {
            if (tty->termios.c_lflag & ECHOCTL)
              col += 2;
          }
          else {
            col++;
          }
        }

        /*
         * Back up over the tab
         */
        while (tty->column > col) {
          (*tty->write)(tty->minor, "\b", 1);
          tty->column--;
        }
      }
      else {
        if (iscntrl (c) && (tty->termios.c_lflag & ECHOCTL)) {
          (*tty->write)(tty->minor, "\b \b", 3);
          if (tty->column)
            tty->column--;
        }
        if (!iscntrl (c) || (tty->termios.c_lflag & ECHOCTL)) {
          (*tty->write)(tty->minor, "\b \b", 3);
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
  }
  else if ((c == '\n') && (tty->termios.c_iflag & INLCR)) {
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
    }
    else if (c == '\n') {
      if (tty->termios.c_lflag & (ECHO | ECHONL))
        echo (c, tty);
      tty->cbuf[tty->ccount++] = c;
      return 1;
    }
    else if ((c == tty->termios.c_cc[VEOL])
          || (c == tty->termios.c_cc[VEOL2])) {
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
  unsigned char c;
  int n;

  if (tty->termios.c_lflag & ICANON) {
    for (;;) {
      n = (*tty->read)(tty->minor, &c);
      if (n < 0) {
        return RTEMS_UNSATISFIED;
      }
      else if (n == 0) {
        rtems_task_wake_after (1);
      }
      else {
        if  (siproc (c, tty))
          break;
      }
    }
  }
  else {
    rtems_interval then, now;
    if (!tty->termios.c_cc[VMIN] && tty->termios.c_cc[VTIME])
      rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &then);
    for (;;) {
      n = (*tty->read)(tty->minor, &c);
      if (n < 0) {
        return RTEMS_UNSATISFIED;
      }
      else if (n == 0) {
        if (tty->termios.c_cc[VMIN]) {
          if (tty->termios.c_cc[VTIME] && tty->ccount) {
            rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &now);
            if ((now - then) > tty->vtimeTicks) {
              break;
            }
          }
        }
        else {
          if (!tty->termios.c_cc[VTIME])
            break;
          rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &now);
          if ((now - then) > tty->vtimeTicks) {
            break;
          }
        }
        rtems_task_wake_after (1);
      }
      else {
        siproc (c, tty);
        if (tty->ccount >= tty->termios.c_cc[VMIN])
          break;
        if (tty->termios.c_cc[VMIN] && tty->termios.c_cc[VTIME])
          rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &then);
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
  rtems_interval timeout = tty->rawMessageFirstTimeout;
  rtems_status_code sc;
  rtems_unsigned32 msgSize;

  for (;;) {
    /*
     * Process characters read from raw queue
     */
    while (tty->rawBufIndex < tty->rawBufCount) {
      unsigned char c = tty->rawBuf[tty->rawBufIndex++];
      if (tty->termios.c_lflag & ICANON) {
        if  (siproc (c, tty))
          return RTEMS_SUCCESSFUL;
      }
      else {
        siproc (c, tty);
        if (tty->ccount >= tty->termios.c_cc[VMIN])
          return RTEMS_SUCCESSFUL;
      }
    }

    /*
     * Read characters from raw queue
     */
    msgSize = RAW_MESSAGE_QUEUE_SIZE;
    sc = rtems_message_queue_receive (tty->rawInputQueue,
              tty->rawBuf,
              &msgSize,
              tty->rawMessageOptions,
              timeout);
    if (sc != RTEMS_SUCCESSFUL)
      break;
    tty->rawBufIndex = 0;
    tty->rawBufCount = msgSize;
    timeout = tty->rawMessageTimeout;
  }
  return RTEMS_SUCCESSFUL;
}

rtems_status_code
rtems_termios_read (void *arg)
{
  rtems_libio_rw_args_t *args = arg;
  struct rtems_termios_tty *tty = args->iop->data1;
  unsigned32 count = args->count;
  unsigned8 *buffer = args->buffer;
  rtems_status_code sc;

  sc = rtems_semaphore_obtain (tty->isem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  if (sc != RTEMS_SUCCESSFUL)
    return sc;
  if (tty->cindex == tty->ccount) {
    tty->cindex = tty->ccount = 0;
    tty->read_start_column = tty->column;
    if (tty->read)
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
  rtems_semaphore_release (tty->isem);
  return sc;
}

/*
 * Place characters on raw queue.
 * NOTE: This routine runs in the context of the device interrupt handler.
 */
void
rtems_termios_enqueue_raw_characters (void *ttyp, char *buf, int len)
{
  struct rtems_termios_tty *tty = ttyp;
  int ncopy;

  while (len) {
    if (len < RAW_MESSAGE_QUEUE_SIZE)
      ncopy = len;
    else
      ncopy = RAW_MESSAGE_QUEUE_SIZE;
    if (rtems_message_queue_send (tty->rawInputQueue, buf, ncopy) != RTEMS_SUCCESSFUL)
      break;
    len -= ncopy;
    buf += ncopy;
  }
}

