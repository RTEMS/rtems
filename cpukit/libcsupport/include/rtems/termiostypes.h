/**
 * @file rtems/termiostypes.h
 *
 * RTEMS termios device support internal data structures
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef  __TERMIOSTYPES_H
#define  __TERMIOSTYPES_H

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/assoc.h>
#include <rtems/chain.h>
#include <stdint.h>
#include <termios.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup TermiostypesSupport RTEMS Termios Device Support
 *
 *  @ingroup libcsupport
 *
 *  @brief RTEMS Termios Device Support Internal Data Structures
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
  rtems_id    Semaphore;
};

typedef enum {
  TERMIOS_POLLED,
  TERMIOS_IRQ_DRIVEN,
  TERMIOS_TASK_DRIVEN,
  TERMIOS_IRQ_SERVER_DRIVEN
} rtems_termios_device_mode;

struct rtems_termios_tty;

/**
 * @brief Termios device context.
 *
 * @see RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER(),
 * rtems_termios_device_context_initialize() and
 * rtems_termios_device_install().
 */
typedef struct rtems_termios_device_context {
  union {
    /* Used for TERMIOS_POLLED and TERMIOS_IRQ_DRIVEN */
    rtems_interrupt_lock interrupt;

    /* Used for TERMIOS_IRQ_SERVER_DRIVEN or TERMIOS_TASK_DRIVEN */
    rtems_id mutex;
  } lock;

  void ( *lock_acquire )(
    struct rtems_termios_device_context *,
    rtems_interrupt_lock_context *
  );

  void ( *lock_release )(
    struct rtems_termios_device_context *,
    rtems_interrupt_lock_context *
  );
} rtems_termios_device_context;

void rtems_termios_device_lock_acquire_default(
  rtems_termios_device_context *ctx,
  rtems_interrupt_lock_context *lock_context
);

void rtems_termios_device_lock_release_default(
  rtems_termios_device_context *ctx,
  rtems_interrupt_lock_context *lock_context
);

/**
 * @brief Initializes a device context.
 *
 * @param[in] context The Termios device context.
 * @param[in] name The name for the interrupt lock.  This name must be a
 *   string persistent throughout the life time of this lock.  The name is only
 *   used if profiling is enabled.
 */
RTEMS_INLINE_ROUTINE void rtems_termios_device_context_initialize(
  rtems_termios_device_context *context,
  const char                   *name
)
{
  rtems_interrupt_lock_initialize( &context->lock.interrupt, name );
  context->lock_acquire = rtems_termios_device_lock_acquire_default;
  context->lock_release = rtems_termios_device_lock_release_default;
}

/**
 * @brief Initializer for static initialization of Termios device contexts.
 *
 * @param name The name for the interrupt lock.  It must be a string.  The name
 *   is only used if profiling is enabled.
 */
#define RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER( name ) \
  { \
    { RTEMS_INTERRUPT_LOCK_INITIALIZER( name ) }, \
    rtems_termios_device_lock_acquire_default, \
    rtems_termios_device_lock_release_default \
  }

/**
 * @brief Termios device handler.
 *
 * @see rtems_termios_device_install().
 */
typedef struct {
  /**
   * @brief First open of this device.
   *
   * @param[in] tty The Termios control.  This parameter may be passed to
   *   interrupt service routines since it must be provided for the
   *   rtems_termios_enqueue_raw_characters() and
   *   rtems_termios_dequeue_characters() functions.
   * @param[in] context The Termios device context.
   * @param[in] term The current Termios attributes.
   * @param[in] args The open/close arguments.  This is parameter provided to
   *   support legacy drivers.  It must not be used by new drivers.
   *
   * @retval true Successful operation.
   * @retval false Cannot open device.
   *
   * @see rtems_termios_get_device_context() and rtems_termios_set_best_baud().
   */
  bool (*first_open)(
    struct rtems_termios_tty      *tty,
    rtems_termios_device_context  *context,
    struct termios                *term,
    rtems_libio_open_close_args_t *args
  );

  /**
   * @brief Last close of this device.
   *
   * @param[in] tty The Termios control.
   * @param[in] context The Termios device context.
   * @param[in] args The open/close arguments.  This is parameter provided to
   *   support legacy drivers.  It must not be used by new drivers.
   */
  void (*last_close)(
    struct rtems_termios_tty      *tty,
    rtems_termios_device_context  *context,
    rtems_libio_open_close_args_t *args
  );

  /**
   * @brief Polled read.
   *
   * In case mode is TERMIOS_IRQ_DRIVEN, TERMIOS_IRQ_SERVER_DRIVEN or
   * TERMIOS_TASK_DRIVEN, then data is received via
   * rtems_termios_enqueue_raw_characters().
   *
   * @param[in] context The Termios device context.
   *
   * @retval char The received data encoded as unsigned char.
   * @retval -1 No data currently available.
   */
  int (*poll_read)(rtems_termios_device_context *context);

  /**
   * @brief Polled write in case mode is TERMIOS_POLLED or write support
   * otherwise.
   *
   * @param[in] context The Termios device context.
   * @param[in] buf The output buffer.
   * @param[in] len The output buffer length in characters.
   */
  void (*write)(
    rtems_termios_device_context *context,
    const char *buf,
    size_t len
  );

  /**
   * @brief Set attributes after a Termios settings change.
   *
   * @param[in] context The Termios device context.
   * @param[in] term The new Termios attributes.
   *
   * @retval true Successful operation.
   * @retval false Invalid attributes.
   */
  bool (*set_attributes)(
    rtems_termios_device_context *context,
    const struct termios         *term
  );

  /**
   * @brief IO control handler.
   *
   * Invoked in case the Termios layer cannot deal with the IO request.
   *
   * @param[in] context The Termios device context.
   * @param[in] request The IO control request.
   * @param[in] buffer The IO control buffer.
   */
  int (*ioctl)(
    rtems_termios_device_context *context,
    ioctl_command_t               request,
    void                         *buffer
  );

  /**
   * @brief Termios device mode.
   */
  rtems_termios_device_mode mode;
} rtems_termios_device_handler;

/**
 * @brief Termios device flow control handler.
 *
 * @see rtems_termios_device_install().
 */
typedef struct {
  /**
   * @brief Indicate to stop remote transmitter.
   *
   * @param[in] context The Termios device context.
   */
  void (*stop_remote_tx)(rtems_termios_device_context *context);

  /**
   * @brief Indicate to start remote transmitter.
   *
   * @param[in] context The Termios device context.
   */
  void (*start_remote_tx)(rtems_termios_device_context *context);
} rtems_termios_device_flow;

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
  rtems_id  isem;
  rtems_id  osem;

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
  uint32_t                    rawInBufSemaphoreOptions;
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
RTEMS_INLINE_ROUTINE void *rtems_termios_get_device_context(
  const rtems_termios_tty *tty
)
{
  return tty->device_context;
}

/**
 * @brief Acquires the device lock.
 *
 * @param[in] context The device context.
 * @param[in] lock_context The local interrupt lock context for an acquire and
 *   release pair.
 */
RTEMS_INLINE_ROUTINE void rtems_termios_device_lock_acquire(
  rtems_termios_device_context *context,
  rtems_interrupt_lock_context *lock_context
)
{
  ( *context->lock_acquire )( context, lock_context );
}

/**
 * @brief Releases the device lock.
 *
 * @param[in] context The device context.
 * @param[in] lock_context The local interrupt lock context for an acquire and
 *   release pair.
 */
RTEMS_INLINE_ROUTINE void rtems_termios_device_lock_release(
  rtems_termios_device_context *context,
  rtems_interrupt_lock_context *lock_context
)
{
  ( *context->lock_release )( context, lock_context );
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
  int (*l_start)(struct rtems_termios_tty *tp);
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
 * @brief Termios poll() filesystem node handler.
 *
 * Real implementation is provided by libbsd.
 */
int rtems_termios_poll(
  rtems_libio_t *iop,
  int            events
);

#ifdef __cplusplus
}
#endif

#endif  /* TERMIOSTYPES_H */
