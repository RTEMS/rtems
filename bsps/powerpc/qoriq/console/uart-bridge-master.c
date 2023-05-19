/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCQorIQUartBridge
 *
 * @brief UART bridge master implementation.
 */

/*
 * Copyright (C) 2011, 2015 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/stat.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include <bspopts.h>
#include <bsp/uart-bridge.h>

#define TRANSMIT_EVENT RTEMS_EVENT_13

static void serial_settings(int fd)
{
  struct termios term;
  int rv = tcgetattr(fd, &term);
  assert(rv == 0);

  term.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
  term.c_oflag &= ~OPOST;
  term.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  term.c_cflag &= ~(CSIZE | PARENB);
  term.c_cflag |= CS8;

  term.c_cc [VMIN] = 1;
  term.c_cc [VTIME] = 1;

  rv = tcsetattr(fd, TCSANOW, &term);
  assert(rv == 0);
}

static void uart_bridge_master_service(intercom_packet *packet, void *arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  uart_bridge_master_context *ctx = arg;

  sc = rtems_chain_append_with_notification(
    &ctx->transmit_fifo,
    &packet->glue.node,
    ctx->transmit_task,
    TRANSMIT_EVENT
  );
  assert(sc == RTEMS_SUCCESSFUL);
}

static void receive_task(rtems_task_argument arg)
{
  uart_bridge_master_context *ctx = (uart_bridge_master_context *) arg;
  intercom_type type = ctx->type;

  int fd = open(ctx->device_path, O_RDONLY);
  assert(fd >= 0);

  serial_settings(fd);

  while (true) {
    intercom_packet *packet = qoriq_intercom_allocate_packet(
      type,
      INTERCOM_SIZE_64
    );
    ssize_t in = read(fd, packet->data, packet->size - 1);
    if (in > 0) {
      packet->size = (size_t) in;
      qoriq_intercom_send_packet(QORIQ_UART_BRIDGE_SLAVE_CORE, packet);
    } else {
      qoriq_intercom_free_packet(packet);
    }
  }
}

static void transmit_task(rtems_task_argument arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  uart_bridge_master_context *ctx = (uart_bridge_master_context *) arg;
  rtems_chain_control *fifo = &ctx->transmit_fifo;

  int fd = open(ctx->device_path, O_WRONLY);
  assert(fd >= 0);

  serial_settings(fd);

  while (true) {
    intercom_packet *packet = NULL;
    sc = rtems_chain_get_with_wait(
      fifo,
      TRANSMIT_EVENT,
      RTEMS_NO_TIMEOUT,
      (rtems_chain_node **) &packet
    );
    assert(sc == RTEMS_SUCCESSFUL);
    write(fd, packet->data, packet->size);
    qoriq_intercom_free_packet(packet);
  }
}

static rtems_id create_task(
  char name,
  rtems_task_entry entry,
  uart_bridge_master_context *ctx
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_id task = RTEMS_ID_NONE;
  char index = (char) ('0' + ctx->type - INTERCOM_TYPE_UART_0);

  sc = rtems_task_create(
    rtems_build_name('U', 'B', name, index),
    QORIQ_UART_BRIDGE_TASK_PRIORITY,
    0,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task
  );
  assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(
    task,
    entry,
    (rtems_task_argument) ctx
  );
  assert(sc == RTEMS_SUCCESSFUL);

  return task;
}

bool qoriq_uart_bridge_master_probe(rtems_termios_device_context *base)
{
  uart_bridge_master_context *ctx = (uart_bridge_master_context *) base;
  intercom_type type = ctx->type;

  qoriq_intercom_service_install(type, uart_bridge_master_service, ctx);
  create_task('R', receive_task, ctx);
  ctx->transmit_task = create_task('T', transmit_task, ctx);

  return true;
}

static bool first_open(
  struct rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
  return false;
}

static bool set_attributes(
  rtems_termios_device_context *base,
  const struct termios *term
)
{
  return false;
}

const rtems_termios_device_handler qoriq_uart_bridge_master = {
  .first_open = first_open,
  .set_attributes = set_attributes,
  .mode = TERMIOS_POLLED
};
