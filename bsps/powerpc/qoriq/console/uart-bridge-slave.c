/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCQorIQUartBridge
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
 * http://www.rtems.org/license/LICENSE.
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
  uart_bridge_slave_context *ctx = arg;
  struct rtems_termios_tty *tty = ctx->tty;

  /* Workaround for https://www.rtems.org/bugzilla/show_bug.cgi?id=1736 */
  rtems_mode prev_mode = disable_preemption();

  rtems_termios_enqueue_raw_characters(tty, packet->data, (int) packet->size);
  qoriq_intercom_free_packet(packet);

  restore_preemption(prev_mode);
}

static void transmit_task(rtems_task_argument arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  uart_bridge_slave_context *ctx = (uart_bridge_slave_context *) arg;
  rtems_chain_control *fifo = &ctx->transmit_fifo;
  struct rtems_termios_tty *tty = ctx->tty;

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
  uart_bridge_slave_context *ctx
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_id task = RTEMS_ID_NONE;
  char index = (char) ('0' + ctx->type - INTERCOM_TYPE_UART_0);

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
    (rtems_task_argument) ctx
  );
  assert(sc == RTEMS_SUCCESSFUL);

  ctx->transmit_task = task;
}

static bool first_open(
  struct rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
  uart_bridge_slave_context *ctx = (uart_bridge_slave_context *) base;
  intercom_type type = ctx->type;

  ctx->tty = tty;
  rtems_termios_set_initial_baud(tty, 115200);
  create_transmit_task(ctx);
  qoriq_intercom_service_install(type, uart_bridge_slave_service, ctx);

  return true;
}

static void last_close(
  struct rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  rtems_libio_open_close_args_t *args
)
{
  uart_bridge_slave_context *ctx = (uart_bridge_slave_context *) base;

  qoriq_intercom_service_remove(ctx->type);
}

static void write_with_interrupts(
  rtems_termios_device_context *base,
  const char *buf,
  size_t len
)
{
  if (len > 0) {
    rtems_status_code sc = RTEMS_SUCCESSFUL;
    uart_bridge_slave_context *ctx = (uart_bridge_slave_context *) base;
    intercom_packet *packet = qoriq_intercom_allocate_packet(
      ctx->type,
      INTERCOM_SIZE_64
    );

    packet->size = len;
    memcpy(packet->data, buf, len);

    /*
     * Due to the lovely Termios implementation we have to hand this over to
     * another context.
     */
    sc = rtems_chain_append_with_notification(
      &ctx->transmit_fifo,
      &packet->glue.node,
      ctx->transmit_task,
      TRANSMIT_EVENT
    );
    assert(sc == RTEMS_SUCCESSFUL);
  }
}

static bool set_attributes(
  rtems_termios_device_context *base,
  const struct termios *term
)
{
  return false;
}

const rtems_termios_device_handler qoriq_uart_bridge_slave = {
  .first_open = first_open,
  .last_close = last_close,
  .write = write_with_interrupts,
  .set_attributes = set_attributes,
  .mode = TERMIOS_IRQ_DRIVEN
};
