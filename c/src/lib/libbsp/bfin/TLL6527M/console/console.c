/**
 *@file console.c
 *
 *@brief
 *  - This file implements uart console for TLL6527M. TLL6527M has BF527 with
 *  second uart (uart-1) connected to the console.
 *
 * Target:   TLL6527v1-0
 * Compiler:
 *
 * COPYRIGHT (c) 2010 by ECE Northeastern University.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license
 *
 * @author Rohan Kangralkar, ECE, Northeastern University
 *         (kangralkar.r@husky.neu.edu)
 *
 * LastChange:
 */

#include <rtems.h>
#include <rtems/libio.h>
#include <bsp.h>
#include <rtems/bspIo.h>

#include <bsp/interrupt.h>
#include <libcpu/uart.h>

/***************************************************
LOCAL DEFINES
 ***************************************************/


/***************************************************
STATIC GLOBALS
 ***************************************************/
/**
 * Declaration of UART
 */
static bfin_uart_channel_t channels[] = {
  {"/dev/console",
    UART1_BASE_ADDRESS,
    DMA10_BASE_ADDRESS,
    DMA11_BASE_ADDRESS,
    CONSOLE_USE_INTERRUPTS,
    UART_USE_DMA,
    CONSOLE_BAUDRATE,
    NULL,
    0,
    0}
};

/**
 * Over all configuration
 */
static bfin_uart_config_t config = {
    SCLK,
    sizeof(channels) / sizeof(channels[0]),
    channels
};


#if CONSOLE_USE_INTERRUPTS
/**
 * The Rx and Tx isr will get the same argument
 * The isr will have to find if it was the rx that caused the interrupt or
 * the tx
 */
static bfin_isr_t bfinUARTISRs[] = {
#if UART_USE_DMA
    /* For First uart */
    {IRQ_DMA10_UART1_RX, bfinUart_rxDmaIsr, (void *)&channels[0], 0},
    {IRQ_DMA11_UART1_TX, bfinUart_txDmaIsr, (void *)&channels[0], 0},
    /* For second uart */
#else
    /* For First uart */
    {IRQ_DMA10_UART1_RX, bfinUart_rxIsr, &channels[0], 0},
    {IRQ_DMA11_UART1_TX, bfinUart_txIsr, &channels[0], 0},
    /* For second uart */
#endif
};
#endif


static void TLL6527_BSP_output_char(char c) {

  bfin_uart_poll_write(0, c);
}

static int TLL6527_BSP_poll_char(void) {

  return bfin_uart_poll_read(0);
}

BSP_output_char_function_type     BSP_output_char = TLL6527_BSP_output_char;
BSP_polling_getchar_function_type BSP_poll_char   = TLL6527_BSP_poll_char;



rtems_device_driver console_close(rtems_device_major_number major,
    rtems_device_minor_number minor,
    void *arg) {

  return rtems_termios_close(arg);
}

rtems_device_driver console_read(rtems_device_major_number major,
    rtems_device_minor_number minor,
    void *arg) {

  return rtems_termios_read(arg);
}

rtems_device_driver console_write(rtems_device_major_number major,
    rtems_device_minor_number minor,
    void *arg) {

  return rtems_termios_write(arg);
}

rtems_device_driver console_control(rtems_device_major_number major,
    rtems_device_minor_number minor,
    void *arg) {

  return rtems_termios_ioctl(arg);
}



/*
 *  Open entry point
 */
rtems_device_driver console_open(rtems_device_major_number major,
    rtems_device_minor_number minor,
    void *arg) {

  return bfin_uart_open(major, minor, arg);
}



/**
 *
 * This routine initializes the console IO driver.
 *
 * Parameters
 * @param major major number
 * @param minor minor number
 *
 * Output parameters:  NONE
 *
 * @return void
 */
rtems_device_driver console_initialize(rtems_device_major_number major,
    rtems_device_minor_number minor,
    void *arg) {
  rtems_status_code status = RTEMS_NOT_DEFINED;
#if CONSOLE_USE_INTERRUPTS
  int               i      = 0;
#endif

  status = bfin_uart_initialize(major, &config);
  if (status != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(status);
  }

#if CONSOLE_USE_INTERRUPTS
  for (i = 0; i < sizeof(bfinUARTISRs) / sizeof(bfinUARTISRs[0]); i++) {
    bfin_interrupt_register(&bfinUARTISRs[i]);
#if INTERRUPT_USE_TABLE
#else
    bfin_interrupt_enable(&bfinUARTISRs[i], 1);
#endif
  }
#endif

  return RTEMS_SUCCESSFUL;
}
