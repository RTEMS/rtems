/*  UART driver for Blackfin
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
 

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/termiostypes.h>
#include <termios.h>
#include <stdlib.h>

#include <libcpu/uartRegs.h>
#include <libcpu/dmaRegs.h>
#include "uart.h"

/* flags */
#define BFIN_UART_XMIT_BUSY 0x01


static bfin_uart_config_t *uartsConfig;


static int pollRead(int minor) {
  int c;
  uint32_t base;

  base = uartsConfig->channels[minor].uart_baseAddress;

  /* check to see if driver is using interrupts so this call will be
     harmless (though non-functional) in case some debug code tries to
     use it */
  if (!uartsConfig->channels[minor].uart_useInterrupts &&
      *((uint16_t volatile *) (base + UART_LSR_OFFSET)) & UART_LSR_DR)
    c = *((uint16_t volatile *) (base + UART_RBR_OFFSET));
  else
    c = -1;

  return c;
}

char bfin_uart_poll_read(rtems_device_minor_number minor) {
  int c;

  do {
    c = pollRead(minor);
  } while (c == -1);

  return c;
}

void bfin_uart_poll_write(int minor, char c) {
  uint32_t base;

  base = uartsConfig->channels[minor].uart_baseAddress;

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

static ssize_t pollWrite(int minor, const char *buf, size_t len) {

  size_t count;
  for ( count = 0; count < len; count++ )
    bfin_uart_poll_write(minor, *buf++);

  return count;
}


/**
 * Routine to initialize the hardware. It initialize the DMA,
 * interrupt if required.
 * @param channel channel information
 */
static void initializeHardware(bfin_uart_channel_t *channel) {
  uint16_t divisor        = 0;
  uint32_t base           = 0;
  uint32_t tx_dma_base    = 0;

  if ( NULL == channel ) {
    return;
  }

  base        = channel->uart_baseAddress;
  tx_dma_base = channel->uart_txDmaBaseAddress;
  /**
   * RX based DMA and interrupt is not supported yet
   * uint32_t tx_dma_base    = 0;
   *
   * rx_dma_base = channel->uart_rxDmaBaseAddress;
   */


  *(uint16_t volatile *) (base + UART_IER_OFFSET) = 0;

  if ( 0 != channel->uart_baud) {
    divisor = (uint16_t) (uartsConfig->freq /
        (channel->uart_baud * 16));
  } else {
    divisor = (uint16_t) (uartsConfig->freq / (9600 * 16));
  }

  *(uint16_t volatile *) (base + UART_LCR_OFFSET) = UART_LCR_DLAB;
  *(uint16_t volatile *) (base + UART_DLL_OFFSET) = (divisor & 0xff);
  *(uint16_t volatile *) (base + UART_DLH_OFFSET) = ((divisor >> 8) & 0xff);

  *(uint16_t volatile *) (base + UART_LCR_OFFSET) = UART_LCR_WLS_8;

  *(uint16_t volatile *) (base + UART_GCTL_OFFSET) = UART_GCTL_UCEN;

  /**
   * To clear previous status
   * divisor is a temp variable here
   */
  divisor = *(uint16_t volatile *) (base + UART_LSR_OFFSET);
  divisor = *(uint16_t volatile *) (base + UART_RBR_OFFSET);
  divisor = *(uint16_t volatile *) (base + UART_IIR_OFFSET);

  if ( channel->uart_useDma ) {
    *(uint16_t  volatile *)(tx_dma_base + DMA_CONFIG_OFFSET) = 0;
    *(uint16_t  volatile *)(tx_dma_base + DMA_CONFIG_OFFSET) = DMA_CONFIG_DI_EN
        | DMA_CONFIG_SYNC ;
    *(uint16_t  volatile *)(tx_dma_base + DMA_IRQ_STATUS_OFFSET) |=
        DMA_IRQ_STATUS_DMA_DONE | DMA_IRQ_STATUS_DMA_ERR;

  } else {
    /**
    * We use polling or interrupts only sending one char at a time :(
    */
  }

  return;
}


/**
 * Set the UART attributes.
 * @param minor
 * @param termios
 * @return
 */
static int setAttributes(int minor, const struct termios *termios) {
  uint32_t base;
  int baud;
  uint16_t divisor;
  uint16_t lcr;

  base = uartsConfig->channels[minor].uart_baseAddress;
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
  if (baud > 0 && uartsConfig->channels[minor].uart_baud)
    baud = uartsConfig->channels[minor].uart_baud;
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

/**
 * Interrupt based uart tx routine. The routine writes one character at a time.
 *
 * @param minor Minor number to indicate uart number
 * @param buf Character buffer which stores characters to be transmitted.
 * @param len Length of buffer to be transmitted.
 * @return
 */
static ssize_t uart_interruptWrite(int minor, const char *buf, size_t len) {
  uint32_t              base      = 0;
  bfin_uart_channel_t*  channel   = NULL;
  rtems_interrupt_level isrLevel;

  /**
   * Sanity Check
   */
  if (NULL == buf || NULL == channel || NULL == uartsConfig || minor < 0) {
    return 0;
  }

  channel = &(uartsConfig->channels[minor]);

  if ( NULL == channel || channel->flags &  BFIN_UART_XMIT_BUSY ) {
    return 0;
  }

  rtems_interrupt_disable(isrLevel);

  base = channel->uart_baseAddress;

  channel->flags |= BFIN_UART_XMIT_BUSY;
  channel->length = 1;
  *(uint16_t volatile *) (base + UART_THR_OFFSET) = *buf;
  *(uint16_t volatile *) (base + UART_IER_OFFSET) = UART_IER_ETBEI;

  rtems_interrupt_enable(isrLevel);

  return 0;
}

/**
* This function implements RX ISR
*/
void bfinUart_rxIsr(void *_arg)
{
  /**
   * TODO: UART RX ISR implementation.
   */

}


/**
 * This function implements TX ISR. The function gets called when the TX FIFO is
 * empty. It clears the interrupt and dequeues the character. It only tx one
 * character at a time.
 *
 * TODO: error handling.
 * @param _arg gets the channel information.
 */
void bfinUart_txIsr(void *_arg) {
  bfin_uart_channel_t*  channel = NULL;
  uint32_t              base    = 0;

  /**
   * Sanity check
   */
  if (NULL == _arg) {
    /** It should never be NULL */
    return;
  }

  channel = (bfin_uart_channel_t *) _arg;

  base = channel->uart_baseAddress;

  *(uint16_t volatile *) (base + UART_IER_OFFSET) &= ~UART_IER_ETBEI;
  channel->flags &= ~BFIN_UART_XMIT_BUSY;

  rtems_termios_dequeue_characters(channel->termios, channel->length);

  return;
}




/**
 * interrupt based DMA write Routine. It configure the DMA to write len bytes.
 * The DMA supports 64K data only.
 *
 * @param minor Identification number of the UART.
 * @param buf Character buffer pointer
 * @param len length of data items to be written
 * @return data already written
 */
static ssize_t uart_DmaWrite(int minor, const char *buf, size_t len) {
  uint32_t              base        = 0;
  bfin_uart_channel_t*  channel     = NULL;
  uint32_t              tx_dma_base = 0;
  rtems_interrupt_level isrLevel;

  /**
   * Sanity Check
   */
  if ( NULL == buf || 0 > minor || NULL == uartsConfig ) {
    return 0;
  }

  channel = &(uartsConfig->channels[minor]);

  /**
   * Sanity Check and check for transmit busy.
   */
  if ( NULL == channel || BFIN_UART_XMIT_BUSY & channel->flags ) {
    return 0;
  }

  rtems_interrupt_disable(isrLevel);

  base        = channel->uart_baseAddress;
  tx_dma_base = channel->uart_txDmaBaseAddress;

  channel->flags |= BFIN_UART_XMIT_BUSY;
  channel->length = len;

  *(uint16_t volatile *) (tx_dma_base + DMA_CONFIG_OFFSET) &= ~DMA_CONFIG_DMAEN;
  *(uint32_t volatile *) (tx_dma_base + DMA_START_ADDR_OFFSET) = (uint32_t)buf;
  *(uint16_t volatile *) (tx_dma_base + DMA_X_COUNT_OFFSET) = channel->length;
  *(uint16_t volatile *) (tx_dma_base + DMA_X_MODIFY_OFFSET) = 1;
  *(uint16_t volatile *) (tx_dma_base + DMA_CONFIG_OFFSET) |= DMA_CONFIG_DMAEN;
  *(uint16_t volatile *) (base + UART_IER_OFFSET) = UART_IER_ETBEI;

  rtems_interrupt_enable(isrLevel);

  return 0;
}


/**
 * RX DMA ISR.
 * The polling route is used for receiving the characters. This is a place
 * holder for future implementation.
 * @param _arg
 */
void bfinUart_rxDmaIsr(void *_arg) {
/**
 * TODO: Implementation of RX DMA
 */
}

/**
 * This function implements TX dma ISR. It clears the IRQ and dequeues a char
 * The channel argument will have the base address. Since there are two uart
 * and both the uarts can use the same tx dma isr.
 *
 * TODO: 1. Error checking 2. sending correct length ie after looking at the
 * number of elements the uart transmitted.
 *
 * @param _arg argument passed to the interrupt handler. It contains the
 * channel argument.
 */
void bfinUart_txDmaIsr(void *_arg) {
  bfin_uart_channel_t*  channel     = NULL;
  uint32_t              tx_dma_base = 0;

  /**
   * Sanity check
   */
  if (NULL == _arg) {
    /** It should never be NULL */
    return;
  }

  channel = (bfin_uart_channel_t *) _arg;

  tx_dma_base = channel->uart_txDmaBaseAddress;

  if ((*(uint16_t volatile *) (tx_dma_base + DMA_IRQ_STATUS_OFFSET)
      & DMA_IRQ_STATUS_DMA_DONE)) {

    *(uint16_t volatile *) (tx_dma_base + DMA_IRQ_STATUS_OFFSET)
        		    |= DMA_IRQ_STATUS_DMA_DONE | DMA_IRQ_STATUS_DMA_ERR;
    channel->flags &= ~BFIN_UART_XMIT_BUSY;
    rtems_termios_dequeue_characters(channel->termios, channel->length);
  } else {
    /* UART DMA did not generate interrupt.
     * This routine must not be called.
     */
  }

  return;
}

/**
 * Function called during exit
 */
void uart_exit(void)
{
  /**
   * TODO: Flushing of quques
   */

}

/**
 * Opens the device in different modes. The supported modes are
 * 1. Polling
 * 2. Interrupt
 * 3. DMA
 * At exit the uart_Exit function will be called to flush the device.
 *
 * @param major Major number of the device
 * @param minor Minor number of the device
 * @param arg
 * @return
 */
rtems_device_driver bfin_uart_open(rtems_device_major_number major,
    rtems_device_minor_number minor, void *arg) {
  rtems_status_code             sc    = RTEMS_NOT_DEFINED;
  rtems_libio_open_close_args_t *args = NULL;

  /**
   * Callback function for polling
   */
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

  /**
   * Callback function for interrupt based transfers without DMA.
   * We use interrupts for writing only. For reading we use polling.
   */
  static const rtems_termios_callbacks interruptCallbacks = {
      NULL,                        /* firstOpen */
      NULL,                        /* lastClose */
      pollRead,                    /* pollRead */
      uart_interruptWrite,              /* write */
      setAttributes,               /* setAttributes */
      NULL,                        /* stopRemoteTx */
      NULL,                        /* startRemoteTx */
      TERMIOS_IRQ_DRIVEN           /* outputUsesInterrupts */
  };

  /**
   * Callback function for interrupt based DMA transfers.
   * We use interrupts for writing only. For reading we use polling.
   */
  static const rtems_termios_callbacks interruptDmaCallbacks = {
      NULL,                        /* firstOpen */
      NULL,                        /* lastClose */
      NULL,                        /* pollRead */
      uart_DmaWrite,              /* write */
      setAttributes,               /* setAttributes */
      NULL,                        /* stopRemoteTx */
      NULL,                        /* startRemoteTx */
      TERMIOS_IRQ_DRIVEN           /* outputUsesInterrupts */
  };


  if ( NULL == uartsConfig || 0 > minor || minor >= uartsConfig->num_channels) {
    return RTEMS_INVALID_NUMBER;
  }

  /**
   * Opens device for handling uart send request either by
   * 1. interrupt with DMA
   * 2. interrupt based
   * 3. Polling
   */
  if ( uartsConfig->channels[minor].uart_useDma ) {
    sc = rtems_termios_open(major, minor, arg, &interruptDmaCallbacks);
  } else {
    sc = rtems_termios_open(major, minor, arg,
        uartsConfig->channels[minor].uart_useInterrupts ?
            &interruptCallbacks : &pollCallbacks);
  }

  args = arg;
  uartsConfig->channels[minor].termios = args->iop->data1;

  atexit(uart_exit);

  return sc;
}


/**
* Uart initialization function.
* @param major major number of the device
* @param config configuration parameters
* @return rtems status code
*/
rtems_status_code bfin_uart_initialize(rtems_device_major_number major,
    bfin_uart_config_t *config) {
  rtems_status_code sc = RTEMS_NOT_DEFINED;
  int               i  = 0;

  rtems_termios_initialize();

  /*
   *  Register Device Names
   */
  uartsConfig = config;
  for (i = 0; i < config->num_channels; i++) {
    config->channels[i].termios = NULL;
    config->channels[i].flags = 0;
    initializeHardware(&(config->channels[i]));
    sc = rtems_io_register_name(config->channels[i].name, major, i);
    if (RTEMS_SUCCESSFUL != sc) {
      return sc;
    }
  }

  return sc;
}
