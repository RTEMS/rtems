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
 *  FreeBSD does not support a full POSIX termios so we have to help it out
 */

#if defined(__FreeBSD__)
#define XTABS   0
#define ONLRET  0
#define ONOCR   0
#define TABDLY  0
#define OLCUC   0
#define ILCUC   0
#define OCRNL   0
#define IUCLC   0
#endif

/*
 *  Cygwin does not define these
 */

#if defined(__CYGWIN__)
#define ECHOPRT 0
#endif

/*
 * The size of the cooked buffer
 */
#define CBUFSIZE	256

/*
 * The sizes of the raw message buffers.
 * On most architectures it is quite a bit more
 * efficient if these are powers of two.
 */
#define RAW_INPUT_BUFFER_SIZE	128
#define RAW_OUTPUT_BUFFER_SIZE	64

/*
 * Variables associated with each termios instance.
 * One structure for each hardware I/O device.
 */
struct rtems_termios_tty {
	/*
	 * Linked-list of active TERMIOS devices
	 */
	struct rtems_termios_tty	*forw;
	struct rtems_termios_tty	*back;

	/*
	 * How many times has this device been opened
	 */
	int		refcount;

	/*
	 * This device
	 */
	rtems_device_major_number	major;
	rtems_device_major_number	minor;

	/*
	 * Mutual-exclusion semaphores
	 */
	rtems_id	isem;
	rtems_id	osem;

	/*
	 * The canonical (cooked) character buffer
	 */
	char		cbuf[CBUFSIZE];
	int		ccount;
	int		cindex;

	/*
	 * Keep track of cursor (printhead) position
	 */
	int		column;
	int		read_start_column;

	/*
	 * The ioctl settings
	 */
	struct termios	termios;
	rtems_interval	vtimeTicks;

	/*
	 * Raw input character buffer
	 */
	volatile char		rawInBuf[RAW_INPUT_BUFFER_SIZE];
	volatile unsigned int	rawInBufHead;
	volatile unsigned int	rawInBufTail;
	rtems_id		rawInBufSemaphore;
	rtems_unsigned32	rawInBufSemaphoreOptions;
	rtems_interval		rawInBufSemaphoreTimeout;
	rtems_interval		rawInBufSemaphoreFirstTimeout;
	unsigned int		rawInBufDropped;	/* Statistics */

	/*
	 * Raw output character buffer
	 */
	volatile char		rawOutBuf[RAW_OUTPUT_BUFFER_SIZE];
	volatile unsigned int	rawOutBufHead;
	volatile unsigned int	rawOutBufTail;
	rtems_id		rawOutBufSemaphore;
	enum {rob_idle, rob_busy, rob_wait }	rawOutBufState;

	/*
	 * Callbacks to device-specific routines
	 */
	rtems_termios_callbacks	device;
        volatile unsigned int   flow_ctrl;
        unsigned int            lowwater,highwater;
};

/* fields for "flow_ctrl" status */
#define FL_IREQXOF 1        /* input queue requests stop of incoming data */
#define FL_ISNTXOF 2        /* XOFF has been sent to other side of line   */
#define FL_IRTSOFF 4        /* RTS has been turned off for other side..   */

#define FL_ORCVXOF 0x10     /* XOFF has been received                     */
#define FL_OSTOP   0x20     /* output has been stopped due to XOFF        */

#define FL_MDRTS   0x100    /* input controlled with RTS/CTS handshake    */
#define FL_MDXON   0x200    /* input controlled with XON/XOFF protocol    */
#define FL_MDXOF   0x400    /* output controlled with XON/XOFF protocol   */

extern struct rtems_termios_tty *rtems_termios_ttyHead;
extern struct rtems_termios_tty *rtems_termios_ttyTail;
extern rtems_id rtems_termios_ttyMutex;

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
	sc = rtems_semaphore_obtain (rtems_termios_ttyMutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
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
		if (rtems_termios_ttyHead)
			rtems_termios_ttyHead->back = tty;
		tty->forw = rtems_termios_ttyHead;
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
			RTEMS_COUNTING_SEMAPHORE | RTEMS_PRIORITY,
			RTEMS_NO_PRIORITY,
			&tty->rawOutBufSemaphore);
		if (sc != RTEMS_SUCCESSFUL)
			rtems_fatal_error_occurred (sc);
		tty->rawOutBufState = rob_idle;

		/*
		 * Set callbacks
		 */
		tty->device = *callbacks;
		if (!tty->device.pollRead) {
			sc = rtems_semaphore_create (
				rtems_build_name ('T', 'R', 'r', c),
				0,
				RTEMS_COUNTING_SEMAPHORE | RTEMS_PRIORITY,
				RTEMS_NO_PRIORITY,
				&tty->rawInBufSemaphore);
			if (sc != RTEMS_SUCCESSFUL)
				rtems_fatal_error_occurred (sc);
		}

		/*
		 * Set default parameters
		 */
		tty->termios.c_iflag = BRKINT | ICRNL | IMAXBEL;
		tty->termios.c_oflag = OPOST | ONLCR | XTABS;
		tty->termios.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
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

		/* start with no flow control, clear flow control flags */
		tty->flow_ctrl = 0;
		/*
		 * set low/highwater mark for XON/XOFF support
		 */
		tty->lowwater  = RAW_INPUT_BUFFER_SIZE * 1/2;
		tty->highwater = RAW_INPUT_BUFFER_SIZE * 3/4;
		/*
		 * Bump name characer
		 */
		if (c++ == 'z')
			c = 'a';

	}
	args->iop->data1 = tty;
	if (!tty->refcount++ && tty->device.firstOpen)
		(*tty->device.firstOpen)(major, minor, arg);
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

	if (tty->device.outputUsesInterrupts) {
		rtems_interrupt_disable (level);
		while (tty->rawOutBufTail != tty->rawOutBufHead) {
			tty->rawOutBufState = rob_wait;
			rtems_interrupt_enable (level);
			sc = rtems_semaphore_obtain (tty->rawOutBufSemaphore,
							RTEMS_WAIT,
							RTEMS_NO_TIMEOUT);
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

	sc = rtems_semaphore_obtain (rtems_termios_ttyMutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	if (sc != RTEMS_SUCCESSFUL)
		rtems_fatal_error_occurred (sc);
	if (--tty->refcount == 0) {
		drainOutput (tty);
		if (tty->device.lastClose)
			 (*tty->device.lastClose)(tty->major, tty->minor, arg);
		if (tty->forw == NULL)
			rtems_termios_ttyTail = tty->back;
		else
			tty->forw->back = tty->back;
		if (tty->back == NULL)
			rtems_termios_ttyHead = tty->forw;
		else
			tty->back->forw = tty->forw;
		rtems_semaphore_delete (tty->isem);
		rtems_semaphore_delete (tty->osem);
		rtems_semaphore_delete (tty->rawOutBufSemaphore);
		if (!tty->device.pollRead)
			rtems_semaphore_delete (tty->rawInBufSemaphore);
		free (tty);
	}
	rtems_semaphore_release (rtems_termios_ttyMutex);
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
	(*tty->device.write)(tty->minor,
			     (char *)&tty->rawOutBuf[tty->rawOutBufTail], 1);
      }
      /* reenable interrupts */
      rtems_interrupt_enable(level);
    }
  }
  /* check for incoming XON/XOFF flow control switched off */
  if (( tty->flow_ctrl & FL_MDXOF) &&
      !(tty->termios.c_iflag & IXOFF)) {
    /* clear related flags in flow_ctrl */
    tty->flow_ctrl &= ~(FL_MDXOF);
    /* FIXME: what happens, if we had sent XOFF but not yet XON? */
    tty->flow_ctrl &= ~(FL_ISNTXOF);
  }

  /* check for incoming RTS/CTS flow control switched off */
  if (( tty->flow_ctrl & FL_MDRTS) &&
      !(tty->termios.c_cflag & CRTSCTS)) {
    /* clear related flags in flow_ctrl */
    tty->flow_ctrl &= ~(FL_MDRTS);
    
    /* restart remote Tx, if it was stopped */
    if ((tty->flow_ctrl & FL_IRTSOFF) &&
	(tty->device.startRemoteTx != NULL)) {
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
	rtems_status_code sc;

 	args->ioctl_return = 0;
	sc = rtems_semaphore_obtain (tty->osem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	if (sc != RTEMS_SUCCESSFUL) {
		args->ioctl_return = sc;
		return sc;
	}
	switch (args->command) {
	default:
		sc = RTEMS_INVALID_NUMBER;
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
		}
		else {
			rtems_interval ticksPerSecond;
			rtems_clock_get (RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticksPerSecond);
			tty->vtimeTicks = tty->termios.c_cc[VTIME] * ticksPerSecond / 10;
			if (tty->termios.c_cc[VTIME]) {
				tty->rawInBufSemaphoreOptions = RTEMS_WAIT;
				tty->rawInBufSemaphoreTimeout = tty->vtimeTicks;
				if (tty->termios.c_cc[VMIN])
					tty->rawInBufSemaphoreFirstTimeout = RTEMS_NO_TIMEOUT;
				else
					tty->rawInBufSemaphoreFirstTimeout = tty->vtimeTicks;
			}
			else {
				if (tty->termios.c_cc[VMIN]) {
					tty->rawInBufSemaphoreOptions = RTEMS_WAIT;
					tty->rawInBufSemaphoreTimeout = RTEMS_NO_TIMEOUT;
					tty->rawInBufSemaphoreFirstTimeout = RTEMS_NO_TIMEOUT;
				}
				else {
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
	}
	rtems_semaphore_release (tty->osem);
	args->ioctl_return = sc;
	return sc;
}

/*
 * Send characters to device-specific code
 */
static void
osend (const char *buf, int len, struct rtems_termios_tty *tty)
{
	unsigned int newHead;
	rtems_interrupt_level level;
	rtems_status_code sc;

	if (!tty->device.outputUsesInterrupts) {
		(*tty->device.write)(tty->minor, buf, len);
		return;
	}
	newHead = tty->rawOutBufHead;
	while (len) {
		/*
		 * Performance improvement could be made here.
		 * Copy multiple bytes to raw buffer:
		 * if (len > 1) && (space to buffer end, or tail > 1)
		 *	ncopy = MIN (len, space to buffer end or tail)
		 *	memcpy (raw buffer, buf, ncopy)
		 *	buf += ncopy
		 *	len -= ncopy
		 *
		 * To minimize latency, the memcpy should be done
		 * with interrupts enabled.
		 */
		newHead = (newHead + 1) % RAW_OUTPUT_BUFFER_SIZE;
		rtems_interrupt_disable (level);
		while (newHead == tty->rawOutBufTail) {
			tty->rawOutBufState = rob_wait;
			rtems_interrupt_enable (level);
			sc = rtems_semaphore_obtain (tty->rawOutBufSemaphore,
							RTEMS_WAIT,
							RTEMS_NO_TIMEOUT);
			if (sc != RTEMS_SUCCESSFUL)
				rtems_fatal_error_occurred (sc);
			rtems_interrupt_disable (level);
		}
		tty->rawOutBuf[tty->rawOutBufHead] = *buf++;
		tty->rawOutBufHead = newHead;
		if (tty->rawOutBufState == rob_idle) {
		  /* check, whether XOFF has been received */
		  if (!(tty->flow_ctrl & FL_ORCVXOF)) {
			(*tty->device.write)(tty->minor,
				(char *)&tty->rawOutBuf[tty->rawOutBufTail],1);
		  }
		  else {
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
	int	i;

	if (tty->termios.c_oflag & OPOST) {
		switch (c) {
		case '\n':
			if (tty->termios.c_oflag & ONLRET)
				tty->column = 0;
			if (tty->termios.c_oflag & ONLCR) {
				osend ("\r", 1, tty);
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
				osend ( "        ",  i, tty);
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
	osend (&c, 1, tty);
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
		osend (args->buffer, args->count, tty);
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
		osend (echobuf, 2, tty);
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
					osend ("\b", 1, tty);
					tty->column--;
				}
			}
			else {
				if (iscntrl (c) && (tty->termios.c_lflag & ECHOCTL)) {
					osend ("\b \b", 3, tty);
					if (tty->column)
						tty->column--;
				}
				if (!iscntrl (c) || (tty->termios.c_lflag & ECHOCTL)) {
					osend ("\b \b", 3, tty);
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
	int n;

	if (tty->termios.c_lflag & ICANON) {
		for (;;) {
			n = (*tty->device.pollRead)(tty->minor);
			if (n < 0) {
				rtems_task_wake_after (1);
			}
			else {
				if  (siproc (n, tty))
					break;
			}
		}
	}
	else {
		rtems_interval then, now;
		if (!tty->termios.c_cc[VMIN] && tty->termios.c_cc[VTIME])
			rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &then);
		for (;;) {
			n = (*tty->device.pollRead)(tty->minor);
			if (n < 0) {
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
				siproc (n, tty);
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
	rtems_interval timeout = tty->rawInBufSemaphoreFirstTimeout;
	rtems_status_code sc;

	for (;;) {
		/*
		 * Process characters read from raw queue
		 */
		while (tty->rawInBufHead != tty->rawInBufTail) {
			unsigned char c;
			unsigned int newHead;

			newHead = (tty->rawInBufHead + 1) % RAW_INPUT_BUFFER_SIZE;
			c = tty->rawInBuf[newHead];
			tty->rawInBufHead = newHead;
			if(((tty->rawInBufTail-newHead+RAW_INPUT_BUFFER_SIZE)
			    % RAW_INPUT_BUFFER_SIZE) 
			   < tty->lowwater) {
			  tty->flow_ctrl &= ~FL_IREQXOF;
			  /* if tx stopped and XON should be sent... */
			  if (((tty->flow_ctrl & (FL_MDXON | FL_ISNTXOF))
			       ==                (FL_MDXON | FL_ISNTXOF))
			      && ((tty->rawOutBufState == rob_idle)
				  || (tty->flow_ctrl & FL_OSTOP))) {
			    /* XON should be sent now... */
			    (*tty->device.write)(tty->minor, 
						 &(tty->termios.c_cc[VSTART]),
						 1);
			  }
			  else if (tty->flow_ctrl & FL_MDRTS) {		    
			    tty->flow_ctrl &= ~FL_IRTSOFF;		
			    /* activate RTS line */
			    if (tty->device.startRemoteTx != NULL) {
			      tty->device.startRemoteTx(tty->minor);
			    }
			  }
			}

			/* continue processing new character */
			if (tty->termios.c_lflag & ICANON) {
				if  (siproc (c, tty))
					return RTEMS_SUCCESSFUL;
			}
			else {
				siproc (c, tty);
				if (tty->ccount >= tty->termios.c_cc[VMIN])
					return RTEMS_SUCCESSFUL;
			}
			timeout = tty->rawInBufSemaphoreTimeout;
		}

		/*
		 * Wait for characters
		 */
		sc = rtems_semaphore_obtain (tty->rawInBufSemaphore,
						tty->rawInBufSemaphoreOptions,
						timeout);
		if (sc != RTEMS_SUCCESSFUL)
			break;
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
		if (tty->device.pollRead)
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
 * NOTE: This routine runs in the context of the
 *       device receive interrupt handler.
 * Returns the number of characters dropped because of overflow.
 */
int
rtems_termios_enqueue_raw_characters (void *ttyp, char *buf, int len)
{
	struct rtems_termios_tty *tty = ttyp;
	unsigned int newTail;
	char c;
	int dropped = 0;
	boolean flow_rcv = FALSE; /* TRUE, if flow control char received */
	rtems_interrupt_level level;

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
	      flow_rcv = TRUE;
	    }
	    else if (c == tty->termios.c_cc[VSTART]) {
	      /* VSTART received */
	      /* restart output  */
	      tty->flow_ctrl &= ~FL_ORCVXOF;
	      flow_rcv = TRUE;
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
		(*tty->device.write)(tty->minor,
				     (char *)&tty->rawOutBuf[tty->rawOutBufTail], 1);
	      }
	      /* reenable interrupts */
	      rtems_interrupt_enable(level);
	    }
	  }	
	  else {
		newTail = (tty->rawInBufTail + 1) % RAW_INPUT_BUFFER_SIZE;
		/* if chars_in_buffer > highwater                */
		rtems_interrupt_disable(level);
		if ((((newTail - tty->rawInBufHead + RAW_INPUT_BUFFER_SIZE) 
		      % RAW_INPUT_BUFFER_SIZE)
		     > tty->highwater) && 
		    !(tty->flow_ctrl & FL_IREQXOF)) {
		  /* incoming data stream should be stopped */
		  tty->flow_ctrl |= FL_IREQXOF; 
		  if ((tty->flow_ctrl & (FL_MDXOF | FL_ISNTXOF)) 
		      ==                (FL_MDXOF             ) ){
		    if ((tty->flow_ctrl & FL_OSTOP) || 
			(tty->rawOutBufState == rob_idle)) {
		      /* if tx is stopped due to XOFF or out of data */
		      /*    call write function here                 */
		      tty->flow_ctrl |= FL_ISNTXOF;
		      (*tty->device.write)(tty->minor,
					   &(tty->termios.c_cc[VSTOP]),
					   1);
		    }
		  }
		  else if ((tty->flow_ctrl & (FL_MDRTS | FL_IRTSOFF)) 
			   ==                (FL_MDRTS             ) ) {
		    tty->flow_ctrl |= FL_IRTSOFF;		
		    /* deactivate RTS line */
		    if (tty->device.stopRemoteTx != NULL) {
		      tty->device.stopRemoteTx(tty->minor);
		    }
		  }
		}
		/* reenable interrupts */
		rtems_interrupt_enable(level);

		if (newTail == tty->rawInBufHead) {
		        dropped++;
		}
		else {
		        tty->rawInBuf[newTail] = c;
		        tty->rawInBufTail = newTail;
		}		
	  }
	}
	tty->rawInBufDropped += dropped;
	rtems_semaphore_release (tty->rawInBufSemaphore);
	return dropped;
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
	unsigned int newTail;
	int nToSend;

	/* check for XOF/XON to send */
	if ((tty->flow_ctrl & (FL_MDXOF | FL_IREQXOF | FL_ISNTXOF))
	    == (FL_MDXOF | FL_IREQXOF)) {
	  /* XOFF should be sent now... */
	  (*tty->device.write)(tty->minor, 
			       &(tty->termios.c_cc[VSTOP]), 1);
	  tty->flow_ctrl |= FL_ISNTXOF;
	  nToSend = 1;
	}
	else if ((tty->flow_ctrl & (FL_IREQXOF | FL_ISNTXOF))
		 == FL_ISNTXOF) {
	  /* NOTE: send XON even, if no longer in XON/XOFF mode... */
	  /* XON should be sent now... */
	  (*tty->device.write)(tty->minor, 
			       &(tty->termios.c_cc[VSTART]), 1);
	  tty->flow_ctrl &= ~FL_ISNTXOF;
	  nToSend = 1;
	}
	else {
	  if (tty->rawOutBufState == rob_wait)
	    rtems_semaphore_release (tty->rawOutBufSemaphore);
	  if ( tty->rawOutBufHead == tty->rawOutBufTail )
	    return 0;	
	  newTail = (tty->rawOutBufTail + len) % RAW_OUTPUT_BUFFER_SIZE;
	  if (newTail == tty->rawOutBufHead) {
	    /*
	     * Buffer empty
	     */
	    tty->rawOutBufState = rob_idle;
	    nToSend = 0;
	  }
	  /* check, whether output should stop due to received XOFF */
	  else if ((tty->flow_ctrl & (FL_MDXON | FL_ORCVXOF)) 
		   ==                (FL_MDXON | FL_ORCVXOF)) {
            /* Buffer not empty, but output stops due to XOFF */
	    /* set flag, that output has been stopped */
	    tty->flow_ctrl |= FL_OSTOP;
	    nToSend = 0;
	  }
	  else {
	    /*
	     * Buffer not empty, start tranmitter
	     */
	    if (newTail > tty->rawOutBufHead)
	      nToSend = RAW_OUTPUT_BUFFER_SIZE - newTail;
	    else
	      nToSend = tty->rawOutBufHead - newTail;
	    /* when flow control XON or XOF, don't send blocks of data     */
	    /* to allow fast reaction on incoming flow ctrl and low latency*/
	    /* for outgoing flow control                                   */
	    if (tty->flow_ctrl & (FL_MDXON | FL_MDXOF)) {
	      nToSend = 1;
	    }
	    (*tty->device.write)(tty->minor, (char *)&tty->rawOutBuf[newTail], nToSend);
	    tty->rawOutBufState = rob_busy;
	  }
	  tty->rawOutBufTail = newTail;
	}
	return nToSend;
}


