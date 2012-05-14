/**
 * @file
 *
 * @ingroup QorIQUartBridge
 *
 * @brief UART bridge slave implementation.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <assert.h>

#include <libchip/sersupp.h>

#include <bspopts.h>
#include <bsp/uart-bridge.h>

#define TRANSMIT_EVENT RTEMS_EVENT_13

static rtems_mode disable_preemption(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_mode prev_mode = 0;

  sc = rtems_task_mode (RTEMS_NO_PREEMPT, RTEMS_PREEMPT_MASK, &prev_mode);
  assert(sc == RTEMS_SUCCESSFUL);

  return prev_mode;
}

static void restore_preemption(rtems_mode prev_mode)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_task_mode (prev_mode, RTEMS_PREEMPT_MASK, &prev_mode);
  assert(sc == RTEMS_SUCCESSFUL);
}

static void uart_bridge_slave_service(intercom_packet *packet, void *arg)
{
  uart_bridge_slave_control *control = arg;
  struct rtems_termios_tty *tty = control->tty;

  /* Workaround for https://www.rtems.org/bugzilla/show_bug.cgi?id=1736 */
  rtems_mode prev_mode = disable_preemption();

  rtems_termios_enqueue_raw_characters(tty, packet->data, (int) packet->size);
  qoriq_intercom_free_packet(packet);

  restore_preemption(prev_mode);
}

static void transmit_task(rtems_task_argument arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  uart_bridge_slave_control *control = (uart_bridge_slave_control *) arg;
  rtems_chain_control *fifo = &control->transmit_fifo;
  struct rtems_termios_tty *tty = control->tty;

  while (true) {
    intercom_packet *packet = NULL;
    sc = rtems_chain_get_with_wait(
      fifo,
      TRANSMIT_EVENT,
      RTEMS_NO_TIMEOUT,
      (rtems_chain_node **) &packet
    );
    assert(sc == RTEMS_SUCCESSFUL);

    /* Workaround for https://www.rtems.org/bugzilla/show_bug.cgi?id=1736 */
    rtems_mode prev_mode = disable_preemption();

    size_t size = packet->size;
    qoriq_intercom_send_packet(QORIQ_UART_BRIDGE_MASTER_CORE, packet);
    rtems_termios_dequeue_characters(tty, (int) size);

    restore_preemption(prev_mode);
  }
}

static void create_transmit_task(
  uart_bridge_slave_control *control
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_id task = RTEMS_ID_NONE;
  char index = (char) ('0' + control->type - INTERCOM_TYPE_UART_0);

  sc = rtems_task_create(
    rtems_build_name('U', 'B', 'T', index),
    QORIQ_UART_BRIDGE_TASK_PRIORITY,
    0,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task
  );
  assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(
    task,
    transmit_task,
    (rtems_task_argument) control
  );
  assert(sc == RTEMS_SUCCESSFUL);

  control->transmit_task = task;
}

static void initialize(int minor)
{
  /* Do nothing */
}

static int first_open(int major, int minor, void *arg)
{
  rtems_libio_open_close_args_t *oc = (rtems_libio_open_close_args_t *) arg;
  struct rtems_termios_tty *tty = (struct rtems_termios_tty *) oc->iop->data1;
  console_tbl *ct = Console_Port_Tbl[minor];
  console_data *cd = &Console_Port_Data [minor];
  uart_bridge_slave_control *control = ct->pDeviceParams;
  intercom_type type = control->type;

  control->tty = tty;
  cd->termios_data = tty;
  rtems_termios_set_initial_baud(tty, 115200);
  create_transmit_task(control);
  qoriq_intercom_service_install(type, uart_bridge_slave_service, control);

  return 0;
}

static int last_close(int major, int minor, void *arg)
{
  console_tbl *ct = Console_Port_Tbl[minor];
  uart_bridge_slave_control *control = ct->pDeviceParams;

  qoriq_intercom_service_remove(control->type);

  return 0;
}

static ssize_t write_with_interrupts(int minor, const char *buf, size_t len)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  console_tbl *ct = Console_Port_Tbl[minor];
  uart_bridge_slave_control *control = ct->pDeviceParams;
  intercom_packet *packet = qoriq_intercom_allocate_packet(
    control->type,
    INTERCOM_SIZE_64
  );

  packet->size = len;
  memcpy(packet->data, buf, len);

  /*
   * Due to the lovely Termios implementation we have to hand this over to
   * another context.
   */
  sc = rtems_chain_append_with_notification(
    &control->transmit_fifo,
    &packet->glue.node,
    control->transmit_task,
    TRANSMIT_EVENT
  );
  assert(sc == RTEMS_SUCCESSFUL);

  return 0;
}

static void write_polled(int minor, char c)
{
  console_tbl *ct = Console_Port_Tbl[minor];
  uart_bridge_slave_control *control = ct->pDeviceParams;
  intercom_packet *packet = qoriq_intercom_allocate_packet(
    control->type,
    INTERCOM_SIZE_64
  );
  char *data = packet->data;
  data [0] = c;
  packet->size = 1;
  qoriq_intercom_send_packet(QORIQ_UART_BRIDGE_MASTER_CORE, packet);
}

static int set_attribues(int minor, const struct termios *term)
{
  return -1;
}

console_fns qoriq_uart_bridge_slave = {
  .deviceProbe = libchip_serial_default_probe,
  .deviceFirstOpen = first_open,
  .deviceLastClose = last_close,
  .deviceRead = NULL,
  .deviceWrite = write_with_interrupts,
  .deviceInitialize = initialize,
  .deviceWritePolled = write_polled,
  .deviceSetAttributes = set_attribues,
  .deviceOutputUsesInterrupts = true
};
