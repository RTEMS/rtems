/*  buttons.c
 *
 *  Buttons driver for the Milkymist One board
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2011 Sebastien Bourdeauducq
 */

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <rtems.h>
#include <rtems/status-checks.h>
#include <bsp.h>
#include <bsp/irq-generic.h>
#include <rtems/libio.h>
#include "../include/system_conf.h"
#include "milkymist_buttons.h"

#define DEVICE_NAME "/dev/buttons"

static rtems_id event_q;

static void send_byte(char b)
{
  rtems_message_queue_send(event_q, &b, 1);
}

static rtems_isr interrupt_handler(rtems_vector_number n)
{
  static unsigned int previous_keys;
  unsigned int keys, pushed_keys, released_keys;
  
  keys = MM_READ(MM_GPIO_IN) & (GPIO_BTN1|GPIO_BTN2|GPIO_BTN3);
  pushed_keys = keys & ~previous_keys;
  released_keys = previous_keys & ~keys;
  previous_keys = keys;
  
  if(pushed_keys & GPIO_BTN1)
    send_byte('A');
  if(pushed_keys & GPIO_BTN2)
    send_byte('B');
  if(pushed_keys & GPIO_BTN3)
    send_byte('C');

  if(released_keys & GPIO_BTN1)
    send_byte('a');
  if(released_keys & GPIO_BTN2)
    send_byte('b');
  if(released_keys & GPIO_BTN3)
    send_byte('c');
  
  lm32_interrupt_ack(1 << MM_IRQ_GPIO);
}

rtems_device_driver buttons_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code sc;
  rtems_isr_entry dummy;

  sc = rtems_io_register_name(DEVICE_NAME, major, 0);
  RTEMS_CHECK_SC(sc, "create buttons device");
  
  sc = rtems_message_queue_create(
    rtems_build_name('B', 'T', 'N', 'Q'),
    24,
    1,
    0,
    &event_q
  );
  RTEMS_CHECK_SC(sc, "create buttons event queue");
  
  rtems_interrupt_catch(interrupt_handler, MM_IRQ_GPIO, &dummy);
  bsp_interrupt_vector_enable(MM_IRQ_GPIO);
  MM_WRITE(MM_GPIO_INTEN, GPIO_BTN1|GPIO_BTN2|GPIO_BTN3);

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver buttons_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  uint32_t count;

  rtems_message_queue_flush(event_q, &count);
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver buttons_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  rtems_status_code sc;

  if(rw_args->count < 1) {
    rw_args->bytes_moved = 0;
    return RTEMS_UNSATISFIED;
  }

  sc = rtems_message_queue_receive(
    event_q,
    rw_args->buffer,
    (size_t *)&rw_args->bytes_moved,
    RTEMS_WAIT,
    RTEMS_NO_TIMEOUT
  );

  if(sc == RTEMS_SUCCESSFUL)
    return RTEMS_SUCCESSFUL;
  else {
    rw_args->bytes_moved = 0;
    return RTEMS_UNSATISFIED;
  }
}
