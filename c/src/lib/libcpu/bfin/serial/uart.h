/*
 *  RTEMS driver for Blackfin UARTs
 *
 *  COPYRIGHT (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *            written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
 

#ifndef _UART_H_
#define _UART_H_


#ifdef __cplusplus
extern "C" {
#endif

/** bfin_uart_channel object
 */
typedef struct {
  const char        *name;                 /** Holds name of the device */
  uint32_t          uart_baseAddress;           /** UART base address */
  uint32_t          uart_rxDmaBaseAddress;      /** RX DMA base address */
  uint32_t          uart_txDmaBaseAddress;      /** TX DMA base address */
  bool              uart_useInterrupts;         /** are interrupts used */
  bool              uart_useDma;                /** is dma used */
  int               uart_baud;                  /** baud rate, 0 for default */

  void              *termios;                   /** termios associated */
  uint8_t volatile  flags;                      /** flags for internal use */
  uint16_t          length;                     /** length for internal use */
} bfin_uart_channel_t;


typedef struct {
  uint32_t freq;
  int num_channels;
  bfin_uart_channel_t *channels;
} bfin_uart_config_t;

/**
 * @param base_address defines the UART base address
 * @param source defines the source that caused the interrupt. This argument
 * will help us in identifying if Rx or TX caused the interrupt.
 */
typedef struct {
  uint32_t base_address;
  int source;
} bfin_uart_arg_t;



char bfin_uart_poll_read(rtems_device_minor_number minor);

void bfin_uart_poll_write(int minor, char c);


/**
* Uart initialization function.
* @param major major number of the device
* @param config configuration parameters
* @return rtems status code
*/
rtems_status_code bfin_uart_initialize(rtems_device_major_number major,
    bfin_uart_config_t *config);



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
    rtems_device_minor_number minor, void *arg);



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
void bfinUart_txDmaIsr(void *_arg);



/**
 * RX DMA ISR.
 * The polling route is used for receiving the characters. This is a place
 * holder for future implementation.
 * @param _arg
 */
void bfinUart_rxDmaIsr(void *_arg);


/**
 * This function implements TX ISR. The function gets called when the TX FIFO is
 * empty. It clears the interrupt and dequeues the character. It only tx one
 * character at a time.
 *
 * TODO: error handling.
 * @param _arg gets the channel information.
 */
void bfinUart_txIsr(void *_arg);


/**
* This function implements RX ISR
*/
void bfinUart_rxIsr(void *_arg);


#ifdef __cplusplus
}
#endif

#endif /* _UART_H_ */

