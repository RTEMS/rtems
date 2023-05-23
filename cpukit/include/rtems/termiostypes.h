/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * RTEMS termios device support internal data structures
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef  __TERMIOSTYPES_H
#define  __TERMIOSTYPES_H

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/assoc.h>
#include <rtems/chain.h>
#include <rtems/termiosdevice.h>
#include <stdint.h>
#include <termios.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TermiostypesSupport
 *
 * @{
 */

/*
 * Wakeup callback data structure
 */
struct ttywakeup {
  void      (*sw_pfn)(struct termios *tty, void *arg);
  void      *sw_arg;
};

/*
 * Variables associated with the character buffer
 */
struct rtems_termios_rawbuf {
  char *theBuf;
  volatile unsigned int  Head;
  volatile unsigned int  Tail;
  volatile unsigned int  Size;
  rtems_binary_semaphore Semaphore;
};

/**
 * @brief Termios device node for installed devices.
 *
 * @see rtems_termios_device_install().
 */
typedef struct rtems_termios_device_node {
  rtems_chain_node                    node;
  rtems_device_major_number           major;
  rtems_device_minor_number           minor;
  const rtems_termios_device_handler *handler;
  const rtems_termios_device_flow    *flow;
  rtems_termios_device_context       *context;
  struct rtems_termios_tty           *tty;
} rtems_termios_device_node;

/*
 * Variables associated with each termios instance.
 * One structure for each hardware I/O device.
 */
typedef struct rtems_termios_tty {
  /*
   * Linked-list of active TERMIOS devices
   */
  struct rtems_termios_tty  *forw;
  struct rtems_termios_tty  *back;

  /*
   * How many times has this device been opened
   */
  int    refcount;

  /*
   * This device
   */
  rtems_device_major_number  major;
  rtems_device_minor_number  minor;

  /*
   * Mutual-exclusion semaphores
   */
  rtems_mutex isem;
  rtems_mutex osem;

  /*
   * The canonical (cooked) character buffer
   */
  char    *cbuf;
  int    ccount;
  int    cindex;

  /*
   * Keep track of cursor (printhead) position
   */
  int    column;
  int    read_start_column;

  /*
   * The ioctl settings
   */
  struct termios  termios;
  rtems_interval  vtimeTicks;

  /*
   * Raw input character buffer
   */
  struct rtems_termios_rawbuf rawInBuf;
  bool                        rawInBufSemaphoreWait;
  rtems_interval              rawInBufSemaphoreTimeout;
  rtems_interval              rawInBufSemaphoreFirstTimeout;
  unsigned int                rawInBufDropped;  /* Statistics */

  /*
   * Raw output character buffer
   */
  struct rtems_termios_rawbuf rawOutBuf;
  int  t_dqlen; /* count of characters dequeued from device */
  enum {rob_idle, rob_busy, rob_wait }  rawOutBufState;

  /*
   * Callbacks to device-specific routines
   */
  rtems_termios_callbacks  device;

  /**
   * @brief Context for legacy devices using the callbacks.
   */
  rtems_termios_device_context legacy_device_context;

  /**
   * @brief The device handler.
   */
  rtems_termios_device_handler handler;

  /**
   * @brief The device flow control handler.
   */
  rtems_termios_device_flow flow;

  volatile unsigned int    flow_ctrl;
  unsigned int             lowwater,highwater;

  /*
   * I/O task IDs (for task-driven drivers)
   */
  rtems_id                rxTaskId;
  rtems_id                txTaskId;
  /*
   * Information for the tx task how many characters have been dequeued.
   */
  int                     txTaskCharsDequeued;

  /*
   * line discipline related stuff
   */
  int t_line;   /* id of line discipline                       */
  void *t_sc;   /* hook for discipline-specific data structure */

  /*
   * Wakeup callback variables
   */
  struct ttywakeup tty_snd;
  struct ttywakeup tty_rcv;
  bool             tty_rcvwakeup;

  /**
   * @brief Corresponding device node.
   */
  rtems_termios_device_node *device_node;

  /**
   * @brief Context for device driver.
   */
  rtems_termios_device_context *device_context;
} rtems_termios_tty;

/**
 * @brief Installs a Termios device.
 *
 * The installed Termios device may be removed via unlink().
 *
 * @param[in] device_file The device file path.
 * @param[in] handler The device handler.  It must be persistent throughout the
 *   installed time of the device.
 * @param[in] flow The device flow control handler.  The device flow control
 *   handler are optional and may be @c NULL.  If present must be persistent
 *   throughout the installed time of the device.
 * @param[in] context The device context.  It must be persistent throughout the
 *   installed time of the device.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_NO_MEMORY Not enough memory to create a device node.
 * @retval RTEMS_UNSATISFIED Creation of the device file failed.
 * @retval RTEMS_INCORRECT_STATE Termios is not initialized.
 *
 * @see rtems_termios_get_device_context().
 */
rtems_status_code rtems_termios_device_install(
  const char                         *device_file,
  const rtems_termios_device_handler *handler,
  const rtems_termios_device_flow    *flow,
  rtems_termios_device_context       *context
);

/**
 * @brief Returns the device context of an installed Termios device.
 *
 * @param[in] tty The Termios control.
 */
static inline void *rtems_termios_get_device_context(
  const rtems_termios_tty *tty
)
{
  return tty->device_context;
}

/**
 * @brief Sets the best baud value in the Termios control.
 *
 * The valid Termios baud values are between 0 and 460800.  The Termios baud
 * value is chosen which minimizes the difference to the value specified.
 *
 * @param[in] term The Termios attributes.
 * @param[in] baud The current baud setting of the device.
 */
void rtems_termios_set_best_baud(
  struct termios *term,
  uint32_t        baud
);

struct rtems_termios_linesw {
  int (*l_open) (struct rtems_termios_tty *tp);
  int (*l_close)(struct rtems_termios_tty *tp);
  int (*l_read )(struct rtems_termios_tty *tp,rtems_libio_rw_args_t *args);
  int (*l_write)(struct rtems_termios_tty *tp,rtems_libio_rw_args_t *args);
  int (*l_rint )(int c,struct rtems_termios_tty *tp);
  int (*l_start)(struct rtems_termios_tty *tp,int len);
  int (*l_ioctl)(struct rtems_termios_tty *tp,rtems_libio_ioctl_args_t *args);
  int (*l_modem)(struct rtems_termios_tty *tp,int flags);
};

/*
 * FIXME: this should move to termios.h!
 */
void rtems_termios_rxirq_occured(struct rtems_termios_tty *tty);

/*
 * FIXME: this should move to termios.h!
 * put a string to output ring buffer
 */
void rtems_termios_puts (
  const void               *buf,
  size_t                    len,
  struct rtems_termios_tty *tty
);

/*
 * global hooks for line disciplines
 */
extern struct rtems_termios_linesw rtems_termios_linesw[];
extern int   rtems_termios_nlinesw;

#define TTYDISC   0    /* termios tty line discipline */
#define TABLDISC  3    /* tablet discipline */
#define SLIPDISC  4    /* serial IP discipline */
#define PPPDISC   5    /* PPP discipline */
#define MAXLDISC  8

/* baudrate xxx integer type */
typedef uint32_t rtems_termios_baud_t;

/**
 *  @brief RTEMS Termios Baud Table
 */
extern const rtems_assoc_t rtems_termios_baud_table [];

/**
 *  @brief Converts the Integral Baud value @a baud to the Termios Control Flag
 *  Representation
 *
 *  @retval B0 Invalid baud value or a baud value of 0.
 *  @retval other Baud constant according to @a baud.
 */
speed_t rtems_termios_number_to_baud(rtems_termios_baud_t baud);

/**
 *  @brief Converts the baud flags to an integral baud value.
 *
 *  @retval 0 Invalid baud value or a baud value of @c B0.
 *  @retval other Integral baud value.
 */
rtems_termios_baud_t rtems_termios_baud_to_number(speed_t baud);

/**
 *  @brief Convert Bxxx Constant to Index
 */
int  rtems_termios_baud_to_index(rtems_termios_baud_t termios_baud);

/**
 * @brief Sets the initial @a baud in the Termios context @a tty.
 *
 * @retval 0 Successful operation.
 * @retval -1 Invalid baud value.
 */
int rtems_termios_set_initial_baud(
  struct rtems_termios_tty *tty,
  rtems_termios_baud_t baud
);

/**
 * @brief Termios kqueue() filter filesystem node handler
 *
 * Real implementation is provided by libbsd.
 */
int rtems_termios_kqfilter(
  rtems_libio_t *iop,
  struct knote  *kn
);

/**
 * @brief Termios mmap() filter filesystem node handler
 *
 * Real implementation is provided by libbsd.
 */
int rtems_termios_mmap(
  rtems_libio_t *iop,
  void         **addr,
  size_t         len,
  int            prot,
  off_t          off
);

/**
 * @brief Termios poll() filesystem node handler.
 *
 * Real implementation is provided by libbsd.
 */
int rtems_termios_poll(
  rtems_libio_t *iop,
  int            events
);

#define RTEMS_IO_SNDWAKEUP _IOW('t', 11, struct ttywakeup ) /* send tty wakeup */
#define RTEMS_IO_RCVWAKEUP _IOW('t', 12, struct ttywakeup ) /* recv tty wakeup */

#define	OLCUC		0x00000100	/* map lower case to upper case on output */
#define	IUCLC		0x00004000	/* map upper case to lower case on input */

#define RTEMS_TERMIOS_NUMBER_BAUD_RATES 25

#ifdef __cplusplus
}
#endif

#endif  /* TERMIOSTYPES_H */
