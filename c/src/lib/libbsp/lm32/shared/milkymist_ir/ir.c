/*  ir.c
 *
 *  Milkymist RC5 IR driver for RTEMS
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2010, 2011 Sebastien Bourdeauducq
 */

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <stdlib.h>
#include <sys/types.h>
#include <rtems.h>
#include <rtems/status-checks.h>
#include <bsp.h>
#include <bsp/irq-generic.h>
#include <rtems/libio.h>
#include "../include/system_conf.h"
#include "milkymist_ir.h"

#define DEVICE_NAME "/dev/ir"

static rtems_id ir_q;

static rtems_isr interrupt_handler(rtems_vector_number n)
{
  unsigned short int msg;

  lm32_interrupt_ack(1 << MM_IRQ_IR);
  msg = MM_READ(MM_IR_RX);
  rtems_message_queue_send(ir_q, &msg, 2);
}

rtems_device_driver ir_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code sc;
  rtems_isr_entry dummy;

  sc = rtems_io_register_name(DEVICE_NAME, major, 0);
  RTEMS_CHECK_SC(sc, "create IR input device");

 sc = rtems_message_queue_create(
    rtems_build_name('R', 'C', '5', 'Q'),
    64,
    2,
    0,
    &ir_q
  );
  RTEMS_CHECK_SC(sc, "create IR queue");

  rtems_interrupt_catch(interrupt_handler, MM_IRQ_IR, &dummy);
  bsp_interrupt_vector_enable(MM_IRQ_IR);

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver ir_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  uint32_t count;

  rtems_message_queue_flush(ir_q, &count);
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver ir_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  rtems_status_code sc;

  if (rw_args->count < 2) {
    rw_args->bytes_moved = 0;
    return RTEMS_UNSATISFIED;
  }

  sc = rtems_message_queue_receive(
    ir_q,
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
