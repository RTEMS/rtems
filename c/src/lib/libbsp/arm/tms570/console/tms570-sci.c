/**
 * @file tms570-sci.c
 *
 * @ingroup tms570
 *
 * @brief Serial communication interface (SCI) functions definitions.
 */

/*
 * Copyright (c) 2014 Premysl Houdek <kom541000@gmail.com>
 *
 * Google Summer of Code 2014 at
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * Based on LPC24xx and LPC1768 BSP
 * by embedded brains GmbH and others
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bspopts.h>
#include <termios.h>
#include <rtems/termiostypes.h>
#include <bsp/tms570-sci.h>
#include <bsp/tms570-sci-driver.h>
#include <rtems/console.h>
#include <bsp.h>
#include <bsp/fatal.h>
#include <bsp/irq.h>

#define TMS570_SCI_BUFFER_SIZE 1

/**
 * @brief Table including all serial drivers
 *
 * Definitions of all serial drivers
 */
tms570_sci_context driver_context_table[] = {
  {
    .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("TMS570 SCI1"),
    .device_name = "/dev/console",
    /* TMS570 UART peripheral use subset of LIN registers which are equivalent
     * to SCI ones
     */
    .regs = (volatile tms570_sci_t *) &TMS570_LIN,
    .irq = TMS570_IRQ_SCI_LEVEL_0,
  },
  {
    .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("TMS570 SCI2"),
    .device_name = "/dev/ttyS1",
    .regs = &TMS570_SCI,
    .irq = TMS570_IRQ_SCI2_LEVEL_0,
  }
};

void tms570_sci_initialize(tms570_sci_context *ctx)
{
  uint32_t rx_pin = 1 << 1;
  uint32_t tx_pin = 1 << 2;

  /* Resec SCI peripheral */
  ctx->regs->GCR0 = TMS570_SCI_GCR0_RESET * 0;
  ctx->regs->GCR0 = TMS570_SCI_GCR0_RESET * 1;

  /* Clear all interrupt sources */
  ctx->regs->CLEARINT = 0xffffffff;

  /* Map all interrupts to SCI INT0 line */
  ctx->regs->CLEARINTLVL = 0xffffffff;

  ctx->regs->GCR1 = TMS570_SCI_GCR1_TXENA * 0 |
                    TMS570_SCI_GCR1_RXENA * 0 |
                    TMS570_SCI_GCR1_CONT * 0 | /* continue operation when debugged */
                    TMS570_SCI_GCR1_LOOP_BACK * 0 |
                    TMS570_SCI_GCR1_POWERDOWN * 0 |
                    TMS570_SCI_GCR1_SLEEP * 0 |
                    TMS570_SCI_GCR1_SWnRST * 0 | /* reset state */
                    TMS570_SCI_GCR1_CLOCK * 1 | /* internal clock */
                    TMS570_SCI_GCR1_TIMING_MODE * 1 |
                    TMS570_SCI_GCR1_COMM_MODE * 0;

  /* Setup connection of SCI peripheral Rx and Tx  pins */
  ctx->regs->PIO0 = rx_pin * 1 | tx_pin * 1; /* Rx and Tx pins are not GPIO */
  ctx->regs->PIO3 = rx_pin * 0 | tx_pin * 0; /* Default output low  */
  ctx->regs->PIO1 = rx_pin * 0 | tx_pin * 0; /* Input when not used by SCI */
  ctx->regs->PIO6 = rx_pin * 0 | tx_pin * 0; /* No open drain */
  ctx->regs->PIO7 = rx_pin * 0 | tx_pin * 0; /* Pull-up/down enabled */
  ctx->regs->PIO8 = rx_pin * 1 | tx_pin * 1; /* Select pull-up */

  /* Bring device out of software reset */
  ctx->regs->GCR1 |= TMS570_SCI_GCR1_SWnRST;
}

/**
 * @brief Serial drivers init function
 *
 * Initialize all serial drivers specified in driver_context_table
 *
 * @param[in] major
 * @param[in] minor
 * @param[in] arg
 * @retval RTEMS_SUCCESSFUL Initialization completed
 */
rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_status_code sc;
#if CONSOLE_USE_INTERRUPTS
  const rtems_termios_device_handler *handler = &tms570_sci_handler_interrupt;
#else
  const rtems_termios_device_handler *handler = &tms570_sci_handler_polled;
#endif

  /*
   * Initialize the Termios infrastructure.  If Termios has already
   * been initialized by another device driver, then this call will
   * have no effect.
   */
  rtems_termios_initialize();

  /* Initialize each device */
  for (
    minor = 0;
    minor < RTEMS_ARRAY_SIZE(driver_context_table);
    ++minor
  ) {
    tms570_sci_context *ctx = &driver_context_table[minor];

    tms570_sci_initialize(ctx);

    /*
     * Install this device in the file system and Termios.  In order
     * to use the console (i.e. being able to do printf, scanf etc.
     * on stdin, stdout and stderr), one device must be registered as
     * "/dev/console" (CONSOLE_DEVICE_NAME).
     */
    sc = rtems_termios_device_install(
        ctx->device_name,
        handler,
        NULL,
        &ctx->base
    );
    if ( sc != RTEMS_SUCCESSFUL ) {
      bsp_fatal(BSP_FATAL_CONSOLE_NO_DEV);
    }
  }
  return RTEMS_SUCCESSFUL;
}

/**
 * @brief Reads chars from HW
 *
 * Reads chars from HW peripheral specified in driver context.
 * TMS570 does not have HW buffer for serial line so this function can
 * return only 0 or 1 char
 *
 * @param[in] ctx context of the driver
 * @param[out] buf read data buffer
 * @param[in] N size of buffer
 * @retval x Number of read chars from peripherals
 */
static int tms570_sci_read_received_chars(
  tms570_sci_context * ctx,
  char * buf,
  int N)
{
  if ( N < 1 ) {
    return 0;
  }
  if ( ctx->regs->RD != 0 ) {
     buf[0] = ctx->regs->RD;
    return 1;
  }
  return 0;
}

/**
 * @brief Enables RX interrupt
 *
 * Enables RX interrupt source of SCI peripheral
 * specified in the driver context.
 *
 * @param[in] ctx context of the driver
 * @retval Void
 */
static void tms570_sci_enable_interrupts(tms570_sci_context * ctx)
{
  ctx->regs->SETINT = TMS570_SCI_SETINT_SET_RX_INT;
}

/**
 * @brief Disables RX interrupt
 *
 * Disables RX interrupt source of SCI peripheral specified in the driver
 * context.
 *
 * @param[in] ctx context of the driver
 * @retval Void
 */
static void tms570_sci_disable_interrupts(tms570_sci_context * ctx)
{
  ctx->regs->CLEARINT = TMS570_SCI_CLEARINT_CLR_RX_INT;
}

/**
 * @brief Check whether driver has put char in HW
 *
 * Check whether driver has put char in HW.
 * This information is read from the driver context not from a peripheral.
 * TMS570 does not have write data buffer asociated with SCI
 * so the return can be only 0 or 1.
 *
 * @param[in] ctx context of the driver
 * @retval x
 */
static int tms570_sci_transmitted_chars(tms570_sci_context * ctx)
{
  int ret;

  ret = ctx->tx_chars_in_hw;
  if ( ret == 1 ) {
    ctx->tx_chars_in_hw = 0;
    return 1;
  }
  return ret;
}

/**
 * @brief Set attributes of the HW peripheral
 *
 * Sets attributes of the HW peripheral (parity, baud rate, etc.)
 *
 * @param[in] base context of the driver
 * @param[in] t termios driver
 * @retval true peripheral setting is changed
 */
bool tms570_sci_set_attributes(
  rtems_termios_device_context *base,
  const struct termios *t
)
{
  tms570_sci_context *ctx = (tms570_sci_context *) base;
  rtems_interrupt_lock_context lock_context;
  int32_t bauddiv;
  int32_t baudrate;
  uint32_t flr_tx_ready = TMS570_SCI_FLR_TX_EMPTY | TMS570_SCI_FLR_TX_EMPTY;

  /* Baud rate */
  baudrate = rtems_termios_baud_to_number(cfgetospeed(t));

  rtems_termios_device_lock_acquire(base, &lock_context);

  while ( (ctx->regs->GCR1 & TMS570_SCI_GCR1_TXENA) &&
          (ctx->regs->FLR & flr_tx_ready) != flr_tx_ready) {
    /*
     * There are pending characters in the hardware,
     * change in the middle of the character Tx leads
     * to disturb of the character and SCI engine
     */
    rtems_interval tw;

    rtems_termios_device_lock_release(base, &lock_context);

    tw = rtems_clock_get_ticks_per_second();
    tw = tw * 5 / baudrate + 1;
    rtems_task_wake_after( tw );

    rtems_termios_device_lock_acquire(base, &lock_context);
  }

  ctx->regs->GCR1 &= ~( TMS570_SCI_GCR1_SWnRST | TMS570_SCI_GCR1_TXENA |
                        TMS570_SCI_GCR1_RXENA );

  ctx->regs->GCR1 &= ~TMS570_SCI_GCR1_STOP;    /*one stop bit*/
  ctx->regs->FORMAT = TMS570_SCI_FORMAT_CHAR(0x7);

  switch ( t->c_cflag & ( PARENB|PARODD ) ) {
    case ( PARENB|PARODD ):
      /* Odd parity */
      ctx->regs->GCR1 &= ~TMS570_SCI_GCR1_PARITY;
      ctx->regs->GCR1 |= TMS570_SCI_GCR1_PARITY_ENA;
      break;

    case PARENB:
      /* Even parity */
      ctx->regs->GCR1 |= TMS570_SCI_GCR1_PARITY;
      ctx->regs->GCR1 |= TMS570_SCI_GCR1_PARITY_ENA;
      break;

    default:
    case 0:
    case PARODD:
      /* No Parity */
      ctx->regs->GCR1 &= ~TMS570_SCI_GCR1_PARITY_ENA;
  }

  /* Apply baudrate to the hardware */
  baudrate *= 2 * 16;
  bauddiv = (BSP_PLL_OUT_CLOCK + baudrate / 2) / baudrate;
  ctx->regs->BRS = bauddiv;

  ctx->regs->GCR1 |= TMS570_SCI_GCR1_SWnRST | TMS570_SCI_GCR1_TXENA |
                     TMS570_SCI_GCR1_RXENA;

  rtems_termios_device_lock_release(base, &lock_context);

  return true;
}

/**
 * @brief sci interrupt handler
 *
 * Handler checks which interrupt occured and provides nessesary maintenance
 * dequeue characters in termios driver whether character is send succesfully
 * enqueue characters in termios driver whether character is recieved
 *
 * @param[in] arg rtems_termios_tty
 * @retval Void
 */
static void tms570_sci_interrupt_handler(void * arg)
{
  rtems_termios_tty *tty = arg;
  tms570_sci_context *ctx = rtems_termios_get_device_context(tty);
  char buf[TMS570_SCI_BUFFER_SIZE];
  size_t n;

  /*
   * Check if we have received something.
   */
   if ( (ctx->regs->FLR & TMS570_SCI_FLR_RXRDY ) == TMS570_SCI_FLR_RXRDY ) {
      n = tms570_sci_read_received_chars(ctx, buf, TMS570_SCI_BUFFER_SIZE);
      if ( n > 0 ) {
        /* Hand the data over to the Termios infrastructure */
        rtems_termios_enqueue_raw_characters(tty, buf, n);
      }
    }
  /*
   * Check if we have something transmitted.
   */
  if ( (ctx->regs->FLR & TMS570_SCI_FLR_TXRDY ) == TMS570_SCI_FLR_TXRDY ) {
    n = tms570_sci_transmitted_chars(ctx);
    if ( n > 0 ) {
      /*
       * Notify Termios that we have transmitted some characters.  It
       * will call now the interrupt write function if more characters
       * are ready for transmission.
       */
      rtems_termios_dequeue_characters(tty, n);
    }
  }
}

/**
 * @brief sci write function called from interrupt
 *
 * Nonblocking write function. Writes characters to HW peripheral
 * TMS570 does not have write data buffer asociated with SCI
 * so only one character can be written.
 *
 * @param[in] base context of the driver
 * @param[in] buf buffer of characters pending to send
 * @param[in] len size of the buffer
 * @retval Void
 */
static void tms570_sci_interrupt_write(
  rtems_termios_device_context *base,
  const char *buf,
  size_t len
)
{
  tms570_sci_context *ctx = (tms570_sci_context *) base;

  if ( len > 0 ) {
    /* start UART TX, this will result in an interrupt when done */
    ctx->regs->TD = *buf;
    /* character written - raise count*/
    ctx->tx_chars_in_hw = 1;
    /* Enable TX interrupt (interrupt is edge-triggered) */
    ctx->regs->SETINT = (1<<8);

  } else {
    /* No more to send, disable TX interrupts */
    ctx->regs->CLEARINT = (1<<8);
    /* Tell close that we sent everything */
  }
}

/**
 * @brief sci write function
 *
 * Blocking write function. Waits until HW peripheral is ready and then writes
 * character to HW peripheral. Writes all characters in the buffer.
 *
 * @param[in] base context of the driver
 * @param[in] buf buffer of characters pending to send
 * @param[in] len size of the buffer
 * @retval Void
 */
static void tms570_sci_poll_write(
  rtems_termios_device_context *base,
  const char *buf,
  size_t n
)
{
  tms570_sci_context *ctx = (tms570_sci_context *) base;
  size_t i;

  /* Write */

  for ( i = 0; i < n; ++i ) {
    while ( (ctx->regs->FLR & TMS570_SCI_FLR_TX_EMPTY ) == 0) {
      ;
    }
    ctx->regs->TD = buf[i];
  }
}

/**
 * @brief See if there is recieved charakter to read
 *
 * read the RX flag from peripheral specified in context
 *
 * @param[in] ctx context of the driver
 * @retval 0 No character to read
 * @retval x Character ready to read
 */
static int TMS570_sci_can_read_char(
  tms570_sci_context * ctx
)
{
  return ctx->regs->FLR & TMS570_SCI_FLR_RXRDY;
}

/**
 * @brief reads character from peripheral
 *
 * reads the recieved character from peripheral specified in context
 *
 * @param[in] ctx context of the driver
 * @retval x Character
 */
static char TMS570_sci_read_char(
  tms570_sci_context * ctx
)
{
  return ctx->regs->RD;
}

/**
 * @brief sci read function
 *
 * check if there is recieved character to be read and reads it.
 *
 * @param[in] base context of the driver
 * @retval -1 No character to be read
 * @retval x Read character
 */
static int tms570_sci_poll_read(rtems_termios_device_context *base)
{
  tms570_sci_context *ctx = (tms570_sci_context *) base;

  /* Check if a character is available */
  if ( TMS570_sci_can_read_char(ctx) ) {
    return TMS570_sci_read_char(ctx);
  } else {
    return -1;
  }
}

/**
 * @brief initialization of the driver
 *
 * initialization of the HW peripheral specified in contex of the driver.
 * This function is called only once when opening the driver.
 *
 * @param[in] tty Termios control
 * @param[in] ctx context of the driver
 * @param[in] term Termios attributes
 * @param[in] args
 * @retval false Error occured during initialization
 * @retval true Driver is open and ready
 */
static bool tms570_sci_poll_first_open(
  rtems_termios_tty             *tty,
  rtems_termios_device_context  *ctx,
  struct termios                *term,
  rtems_libio_open_close_args_t *args
)
{
  bool ok;

  rtems_termios_set_best_baud(term, TMS570_SCI_BAUD_RATE);
  ok = tms570_sci_set_attributes(ctx, term);
  if ( !ok ) {
    return false;
  }
  return true;
}

/**
 * @brief initialization of the interrupt driven driver
 *
 * calls tms570_sci_poll_first_open function.
 * install and enables interrupts.
 *
 * @param[in] tty Termios control
 * @param[in] base context of the driver
 * @param[in] args
 * @retval false Error occured during initialization
 * @retval true Driver is open and ready
 */
static bool tms570_sci_interrupt_first_open(
  rtems_termios_tty             *tty,
  rtems_termios_device_context  *base,
  struct termios                *term,
  rtems_libio_open_close_args_t *args
)
{
  tms570_sci_context *ctx = (tms570_sci_context *) base;
  rtems_status_code sc;
  bool ret;

  ret = tms570_sci_poll_first_open(tty, base, term, args);
  if ( ret == false ) {
    return false;
  }

  /* Register Interrupt handler */
  sc = rtems_interrupt_handler_install(ctx->irq,
      ctx->device_name,
      RTEMS_INTERRUPT_SHARED,
      tms570_sci_interrupt_handler,
      tty
  );
  if ( sc != RTEMS_SUCCESSFUL ) {
    return false;
  }
  tms570_sci_enable_interrupts(ctx);
  return true;
}

/**
 * @brief closes sci peripheral
 *
 * @param[in] tty Termios control
 * @param[in] base context of the driver
 * @param[in] args
 * @retval false Error occured during initialization
 * @retval true Driver is open and ready
 */
static void tms570_sci_poll_last_close(
  rtems_termios_tty             *tty,
  rtems_termios_device_context  *base,
  rtems_libio_open_close_args_t *args
)
{
  ;
}

/**
 * @brief closes sci peripheral of interrupt driven driver
 *
 * calls tms570_sci_poll_last_close and disables interrupts
 *
 * @param[in] tty Termios control
 * @param[in] base context of the driver
 * @param[in] args
 * @retval false Error occured during initialization
 * @retval true Driver is open and ready
 */
static void tms570_sci_interrupt_last_close(
  rtems_termios_tty             *tty,
  rtems_termios_device_context  *base,
  rtems_libio_open_close_args_t *args
)
{
  tms570_sci_context *ctx = (tms570_sci_context *) base;
  rtems_interrupt_lock_context lock_context;
  rtems_interval tw;
  int32_t baudrate;

  /* Turn off RX interrupts */
  rtems_termios_device_lock_acquire(base, &lock_context);
  tms570_sci_disable_interrupts(ctx);
  rtems_termios_device_lock_release(base, &lock_context);

  tw = rtems_clock_get_ticks_per_second();
  baudrate = rtems_termios_baud_to_number(cfgetospeed(&tty->termios));
  tw = tw * 10 / baudrate + 1;
  while ( ( ctx->regs->FLR & TMS570_SCI_FLR_TX_EMPTY ) == 0 ) {
     rtems_task_wake_after(tw);
  }

  /* uninstall ISR */
  rtems_interrupt_handler_remove(ctx->irq, tms570_sci_interrupt_handler, tty);

  tms570_sci_poll_last_close(tty, base, args);
}

/**
 * @brief Struct containing definitions of polled driver functions.
 *
 * Encapsulates polled driver functions.
 * Use of this table is determited by not defining TMS570_USE_INTERRUPTS
 */
const rtems_termios_device_handler tms570_sci_handler_polled = {
  .first_open = tms570_sci_poll_first_open,
  .last_close = tms570_sci_poll_last_close,
  .poll_read = tms570_sci_poll_read,
  .write = tms570_sci_poll_write,
  .set_attributes = tms570_sci_set_attributes,
  .mode = TERMIOS_POLLED
};

/**
 * @brief Struct containing definitions of interrupt driven driver functions.
 *
 * Encapsulates interrupt driven driver functions.
 * Use of this table is determited by defining TMS570_USE_INTERRUPTS
 */
const rtems_termios_device_handler tms570_sci_handler_interrupt  = {
  .first_open = tms570_sci_interrupt_first_open,
  .last_close = tms570_sci_interrupt_last_close,
  .poll_read = NULL,
  .write = tms570_sci_interrupt_write,
  .set_attributes = tms570_sci_set_attributes,
  .mode = TERMIOS_IRQ_DRIVEN
};
