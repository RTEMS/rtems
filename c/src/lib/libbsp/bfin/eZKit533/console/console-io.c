/*  console-io.c
 *
 *  This file contains the hardware specific portions of the TTY driver
 *  for the serial ports for ezkit533.
 *
 *  Copyright (c) 2006 by Atos Automacao Industrial Ltda.
 *             written by Alain Schaefer <alain.schaefer@easc.ch>
 *                    and Antonio Giovanini <antonio@atos.com.br>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */


#include <rtems.h>
#include <rtems/libio.h>
#include <bsp.h>
#include <rtems/bspIo.h>

#include <libcpu/bf533.h>
#include <libcpu/interrupt.h>
#include <libcpu/uart.h>

static bfin_uart_channel_t channels[] = {
  {"/dev/console",
   UART0_BASE_ADDRESS,
   0,
   0,
   CONSOLE_USE_INTERRUPTS,
   0,
#ifdef CONSOLE_FORCE_BAUD
   CONSOLE_FORCE_BAUD,
#else
   0,
#endif
   NULL,
   0,
   0}
};

static bfin_uart_config_t config = {
  SCLK,
  sizeof(channels) / sizeof(channels[0]),
  channels
};

#if CONSOLE_USE_INTERRUPTS
static bfin_isr_t bfinUARTISRs[] = {
  {SIC_DMA6_UART0_RX_VECTOR, bfinUart_rxIsr, 0, 0, NULL},
  {SIC_DMA7_UART0_TX_VECTOR, bfinUart_txIsr, 0, 0, NULL},
};
#endif


static void eZKit533_BSP_output_char(char c) {

  bfin_uart_poll_write(0, c);
}

static int eZKit533_BSP_poll_char(void) {

  return bfin_uart_poll_read(0);
}

BSP_output_char_function_type BSP_output_char = eZKit533_BSP_output_char;
BSP_polling_getchar_function_type BSP_poll_char = eZKit533_BSP_poll_char;

rtems_device_driver console_initialize(rtems_device_major_number major,
                                       rtems_device_minor_number minor,
                                       void *arg) {
  rtems_status_code status;
#if CONSOLE_USE_INTERRUPTS
  int i;
#endif

  status = bfin_uart_initialize(major, &config);
#if CONSOLE_USE_INTERRUPTS
  for (i = 0; i < sizeof(bfinUARTISRs) / sizeof(bfinUARTISRs[0]); i++) {
    bfin_interrupt_register(&bfinUARTISRs[i]);
    bfin_interrupt_enable(&bfinUARTISRs[i], TRUE);
  }
#endif

  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver console_open(rtems_device_major_number major,
                                 rtems_device_minor_number minor,
                                 void *arg) {

  return bfin_uart_open(major, minor, arg);
}

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

