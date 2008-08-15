/*  UART driver for Blackfin
 * 
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
 

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/termiostypes.h>
#include <termios.h>
#include <stdlib.h>

#include <libcpu/uartRegs.h>
#include "uart.h"


/* flags */
#define BFIN_UART_XMIT_BUSY 0x01


static bfin_uart_config_t *uartsConfig;


static void initializeHardware(int minor) {
  uint16_t divisor;
  char *base;
  uint16_t r;

  base = uartsConfig->channels[minor].base_address;
 
  *(uint16_t volatile *) (base + UART_IER_OFFSET) = 0;

  if (uartsConfig->channels[minor].force_baud)
    divisor = (uint16_t) (uartsConfig->freq /
                          (uartsConfig->channels[minor].force_baud * 16));
  else
    divisor = (uint16_t) (uartsConfig->freq / (9600 * 16));
  *(uint16_t volatile *) (base + UART_LCR_OFFSET) = UART_LCR_DLAB;
  *(uint16_t volatile *) (base + UART_DLL_OFFSET) = (divisor & 0xff);
  *(uint16_t volatile *) (base + UART_DLH_OFFSET) = ((divisor >> 8) & 0xff);

  *(uint16_t volatile *) (base + UART_LCR_OFFSET) = UART_LCR_WLS_8;

  *(uint16_t volatile *) (base + UART_GCTL_OFFSET) = UART_GCTL_UCEN;

  r = *(uint16_t volatile *) (base + UART_LSR_OFFSET);
  r = *(uint16_t volatile *) (base + UART_RBR_OFFSET);
  r = *(uint16_t volatile *) (base + UART_IIR_OFFSET);

  return;
}

static int pollRead(int minor) {
  int c;
  char *base;
 
  base = uartsConfig->channels[minor].base_address;
 
  /* check to see if driver is using interrupts so this call will be
     harmless (though non-functional) in case some debug code tries to
     use it */ 
  if (!uartsConfig->channels[minor].use_interrupts &&
      *((uint16_t volatile *) (base + UART_LSR_OFFSET)) & UART_LSR_DR)
    c = *((uint16_t volatile *) (base + UART_RBR_OFFSET));
  else
    c = -1;

  return c;
}

char bfin_uart_poll_read(int minor) {
  int c;

  do {
    c = pollRead(minor);
  } while (c == -1);

  return c;
}

void bfin_uart_poll_write(int minor, char c) {
  char *base;
 
  base = uartsConfig->channels[minor].base_address;

  while (!(*((uint16_t volatile *) (base + UART_LSR_OFFSET)) & UART_LSR_THRE))
    ;
  *(uint16_t volatile *) (base + UART_THR_OFFSET) = c;
}

/* begin BISON */
void debug_write_char(char c) {
  bfin_uart_poll_write(0, c);
}

void debug_write_string(char *s) {

  while (s && *s) {
    if (*s == '\n')
      debug_write_char('\r');
    debug_write_char(*s++);
  }
}

void debug_write_crlf(void) {

  debug_write_char('\r');
  debug_write_char('\n');
}

void debug_write_nybble(int nybble) {

  nybble &= 0x0f;
  debug_write_char((nybble > 9) ? 'a' + (nybble - 10) : '0' + nybble);
}

void debug_write_byte(int byte) {

  byte &= 0xff;
  debug_write_nybble(byte >> 4);
  debug_write_nybble(byte & 0x0f);
}

void debug_write_half(int half) {

  half &= 0xffff;
  debug_write_byte(half >> 8);
  debug_write_byte(half & 0xff);
}

void debug_write_word(int word) {

  word &= 0xffffffff;
  debug_write_half(word >> 16);
  debug_write_half(word & 0xffff);
}
/* end BISON */

/*
 *  Console Termios Support Entry Points
 *
 */

static int pollWrite(int minor, const char *buf, int len) {

  while (len-- > 0)
    bfin_uart_poll_write(minor, *buf++);

  return 0;
}

static void enableInterrupts(int minor) {
  char *base;
 
  base = uartsConfig->channels[minor].base_address;

  *(uint16_t volatile *) (base + UART_IER_OFFSET) = UART_IER_ETBEI |
                                                    UART_IER_ERBFI;
}

static void disableAllInterrupts(void) {
  int i;
  char *base;

  for (i = 0; i < uartsConfig->num_channels; i++) {
    base = uartsConfig->channels[i].base_address;
    *(uint16_t volatile *) (base + UART_IER_OFFSET) = 0;
  }
}

static int interruptWrite(int minor, const char *buf, int len) {
  char *base;

  base = uartsConfig->channels[minor].base_address;

  uartsConfig->channels[minor].flags |= BFIN_UART_XMIT_BUSY;
  *(uint16_t volatile *) (base + UART_THR_OFFSET) = *buf;

  return 0;
}

static int setAttributes(int minor, const struct termios *termios) {
  char *base;
  int baud;
  uint16_t divisor;
  uint16_t lcr;

  base = uartsConfig->channels[minor].base_address;
  switch (termios->c_cflag & CBAUD) {
  case B0:
    baud = 0;
    break;
  case B50:
    baud = 50;
    break;
  case B75:
    baud = 75;
    break;
  case B110:
    baud = 110;
    break;
  case B134:
    baud = 134;
    break;
  case B150:
    baud = 150;
    break;
  case B200:
    baud = 200;
    break;
  case B300:
    baud = 300;
    break;
  case B600:
    baud = 600;
    break;
  case B1200:
    baud = 1200;
    break;
  case B1800:
    baud = 1800;
    break;
  case B2400:
    baud = 2400;
    break;
  case B4800:
    baud = 4800;
    break;
  case B9600:
    baud = 9600;
    break;
  case B19200:
    baud = 19200;
    break;
  case B38400:
    baud = 38400;
    break;
  case B57600:
    baud = 57600;
    break;
  case B115200:
    baud = 115200;
    break;
  case B230400:
    baud = 230400;
    break;
  case B460800:
    baud = 460800;
    break;
  default:
    baud = -1;
    break;
  }
  if (baud > 0 && uartsConfig->channels[minor].force_baud)
    baud = uartsConfig->channels[minor].force_baud;
  switch (termios->c_cflag & CSIZE) {
  case CS5:
    lcr = UART_LCR_WLS_5;
    break;
  case CS6:
    lcr = UART_LCR_WLS_6;
    break;
  case CS7:
    lcr = UART_LCR_WLS_7;
    break;
  case CS8:
  default:
    lcr = UART_LCR_WLS_8;
    break;
  }
  switch (termios->c_cflag & (PARENB | PARODD)) {
  case PARENB:
    lcr |= UART_LCR_PEN | UART_LCR_EPS;
    break;
  case PARENB | PARODD:
    lcr |= UART_LCR_PEN;
    break;
  default:
    break;
  }
  if (termios->c_cflag & CSTOPB)
    lcr |= UART_LCR_STB;

  if (baud > 0) {
    divisor = (uint16_t) (uartsConfig->freq / (baud * 16));
    *(uint16_t volatile *) (base + UART_LCR_OFFSET) = lcr | UART_LCR_DLAB;
    *(uint16_t volatile *) (base + UART_DLL_OFFSET) = (divisor & 0xff);
    *(uint16_t volatile *) (base + UART_DLH_OFFSET) = ((divisor >> 8) & 0xff);
  }
  *(uint16_t volatile *) (base + UART_LCR_OFFSET) = lcr;

  return 0;
}

void bfin_uart_isr(int source) {
  int i;
  char *base;
  uint16_t uartStat;
  char c;
  uint8_t uartLSR;

  /* Just use one ISR and check for all UART interrupt sources in it.
     This is less efficient than making use of the vector to narrow down
     the things we need to check, but not all Blackfins separate the
     UART interrupt sources in the same ways.  This way we don't have
     to make this code dependent on the type of Blackfin.  */
  for (i = 0; i < uartsConfig->num_channels; i++) {
    if (uartsConfig->channels[i].use_interrupts) {
      base = uartsConfig->channels[i].base_address;
      uartStat = *(uint16_t volatile *) (base + UART_IIR_OFFSET);
      if ((uartStat & UART_IIR_NINT) == 0) {
        switch (uartStat & UART_IIR_STATUS_MASK) {
        case UART_IIR_STATUS_THRE:
          if (uartsConfig->channels[i].termios &&
              (uartsConfig->channels[i].flags & BFIN_UART_XMIT_BUSY)) {
            uartsConfig->channels[i].flags &= ~BFIN_UART_XMIT_BUSY;
            rtems_termios_dequeue_characters(uartsConfig->channels[i].termios,
                                             1);
          }
          break;
        case UART_IIR_STATUS_RDR:
          c = *(uint16_t volatile *) (base + UART_RBR_OFFSET);
          if (uartsConfig->channels[i].termios)
            rtems_termios_enqueue_raw_characters(
                uartsConfig->channels[i].termios, &c, 1);
          break;
        case UART_IIR_STATUS_LS:
          uartLSR = *(uint16_t volatile *) (base + UART_LSR_OFFSET);
          /* break, framing error, parity error, or overrun error
             has been detected */
          break;
        default:
          break;
        }
      }
    }
  }
}

rtems_status_code bfin_uart_initialize(rtems_device_major_number major,
                                       bfin_uart_config_t *config) {
  rtems_status_code status;
  int i;

  status = RTEMS_SUCCESSFUL;

  rtems_termios_initialize();

  /*
   *  Register Device Names
   */

  uartsConfig = config;
  for (i = 0; i < config->num_channels; i++) {
    config->channels[i].termios = NULL;
    config->channels[i].flags = 0;
    initializeHardware(i);
    status = rtems_io_register_name(config->channels[i].name, major, i);
  }

   return RTEMS_SUCCESSFUL;
}

rtems_device_driver bfin_uart_open(rtems_device_major_number major,
                                   rtems_device_minor_number minor,
                                   void *arg) {
  rtems_status_code sc;
  rtems_libio_open_close_args_t *args;
  static const rtems_termios_callbacks pollCallbacks = {
    NULL,                        /* firstOpen */
    NULL,                        /* lastClose */
    pollRead,                    /* pollRead */
    pollWrite,                   /* write */
    setAttributes,               /* setAttributes */
    NULL,                        /* stopRemoteTx */
    NULL,                        /* startRemoteTx */
    TERMIOS_POLLED               /* outputUsesInterrupts */
  };
  static const rtems_termios_callbacks interruptCallbacks = {
    NULL,                        /* firstOpen */
    NULL,                        /* lastClose */
    NULL,                        /* pollRead */
    interruptWrite,              /* write */
    setAttributes,               /* setAttributes */
    NULL,                        /* stopRemoteTx */
    NULL,                        /* startRemoteTx */
    TERMIOS_IRQ_DRIVEN           /* outputUsesInterrupts */
  };

  if (uartsConfig == NULL || minor < 0 || minor >= uartsConfig->num_channels)
    return RTEMS_INVALID_NUMBER;

  sc = rtems_termios_open(major, minor, arg,
                          uartsConfig->channels[minor].use_interrupts ?
                          &interruptCallbacks : &pollCallbacks);
  args = arg;
  uartsConfig->channels[minor].termios = args->iop->data1;

  if (uartsConfig->channels[minor].use_interrupts)
    enableInterrupts(minor);
  atexit(disableAllInterrupts);

  return sc;
}

