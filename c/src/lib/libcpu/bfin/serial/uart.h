/*
 *  RTEMS driver for Blackfin UARTs
 *
 *  COPYRIGHT (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *            written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _uart_h_
#define _uart_h_


#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
  const char *name;
  void *base_address;
  boolean use_interrupts;
  int force_baud;
  /* the following are for internal use */
  void *termios;
  uint8_t volatile flags;
} bfin_uart_channel_t;

typedef struct {
  uint32_t freq;
  int num_channels;
  bfin_uart_channel_t *channels;
} bfin_uart_config_t;


char bfin_uart_poll_read(int minor);

void bfin_uart_poll_write(int minor, char c);

rtems_status_code bfin_uart_initialize(rtems_device_major_number major,
                                       bfin_uart_config_t *config);

rtems_device_driver bfin_uart_open(rtems_device_major_number major,
                                   rtems_device_minor_number minor,
                                   void *arg);

void bfin_uart_isr(int source);

 
#ifdef __cplusplus
}
#endif

#endif /* _uart_h_ */

