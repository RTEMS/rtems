/**
 * @file
 *
 * @ingroup QorIQUartBridge
 *
 * @brief UART bridge master implementation.
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

#include <sys/stat.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include <libchip/sersupp.h>

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
  uart_bridge_master_control *control = arg;

  sc = rtems_chain_append_with_notification(
    &control->transmit_fifo,
    &packet->glue.node,
    control->transmit_task,
    TRANSMIT_EVENT
  );
  assert(sc == RTEMS_SUCCESSFUL);
}

static void receive_task(rtems_task_argument arg)
{
  uart_bridge_master_control *control = (uart_bridge_master_control *) arg;
  intercom_type type = control->type;

  int fd = open(control->device_path, O_RDONLY);
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
  uart_bridge_master_control *control = (uart_bridge_master_control *) arg;
  rtems_chain_control *fifo = &control->transmit_fifo;

  int fd = open(control->device_path, O_WRONLY);
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
  uart_bridge_master_control *control
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_id task = RTEMS_ID_NONE;
  char index = (char) ('0' + control->type - INTERCOM_TYPE_UART_0);

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
    (rtems_task_argument) control
  );
  assert(sc == RTEMS_SUCCESSFUL);

  return task;
}

static void initialize(int minor)
{
  console_tbl *ct = Console_Port_Tbl [minor];
  uart_bridge_master_control *control = ct->pDeviceParams;
  intercom_type type = control->type;

  qoriq_intercom_service_install(type, uart_bridge_master_service, control);
  create_task('R', receive_task, control);
  control->transmit_task = create_task('T', transmit_task, control);
}

static int first_open(int major, int minor, void *arg)
{
  return -1;
}

static int last_close(int major, int minor, void *arg)
{
  return -1;
}

static int read_polled(int minor)
{
  return -1;
}

static void write_polled(int minor, char c)
{
  /* Do nothing */
}

static int set_attributes(int minor, const struct termios *term)
{
  return -1;
}

console_fns qoriq_uart_bridge_master = {
  .deviceProbe = libchip_serial_default_probe,
  .deviceFirstOpen = first_open,
  .deviceLastClose = last_close,
  .deviceRead = read_polled,
  .deviceWrite = NULL,
  .deviceInitialize = initialize,
  .deviceWritePolled = write_polled,
  .deviceSetAttributes = set_attributes,
  .deviceOutputUsesInterrupts = false
};
