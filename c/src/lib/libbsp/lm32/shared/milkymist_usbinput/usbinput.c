/*  usbinput.c
 *
 *  Milkymist USB input devices driver for RTEMS
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2010, 2011, 2012 Sebastien Bourdeauducq
 */

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <rtems.h>
#include <bsp.h>
#include <bsp/irq-generic.h>
#include <rtems/libio.h>
#include <rtems/status-checks.h>
#include "../include/system_conf.h"
#include "milkymist_usbinput.h"

#include "comloc.h"

#define DEVICE_NAME "/dev/usbinput"

static int mouse_consume;
static int keyboard_consume;
static int midi_consume;

static rtems_id event_q;

static rtems_isr interrupt_handler(rtems_vector_number n)
{
  unsigned char msg[8];
  int i;

  lm32_interrupt_ack(1 << MM_IRQ_USB);

  while(mouse_consume != COMLOC_MEVT_PRODUCE) {
    for(i=0;i<4;i++)
      msg[i] = COMLOC_MEVT(4*mouse_consume+i);
    rtems_message_queue_send(event_q, msg, 4);
    mouse_consume = (mouse_consume + 1) & 0x0f;
  }

  while(keyboard_consume != COMLOC_KEVT_PRODUCE) {
    for(i=0;i<8;i++)
      msg[i] = COMLOC_KEVT(8*keyboard_consume+i);
    rtems_message_queue_send(event_q, msg, 8);
    keyboard_consume = (keyboard_consume + 1) & 0x07;
  }

  while(midi_consume != COMLOC_MIDI_PRODUCE) {
    for(i=0;i<3;i++)
      msg[i] = COMLOC_MIDI(4*midi_consume+i+1);
    rtems_message_queue_send(event_q, msg, 3);
    midi_consume = (midi_consume + 1) & 0x0f;
  }

}

rtems_device_driver usbinput_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code sc;
  rtems_isr_entry dummy;

  MM_WRITE(MM_SOFTUSB_CONTROL, SOFTUSB_CONTROL_RESET);

  mouse_consume = 0;
  keyboard_consume = 0;
  midi_consume = 0;

  sc = rtems_io_register_name(DEVICE_NAME, major, 0);
  RTEMS_CHECK_SC(sc, "create USB input device");

  sc = rtems_message_queue_create(
    rtems_build_name('U', 'S', 'B', 'I'),
    64,
    8,
    0,
    &event_q
  );
  RTEMS_CHECK_SC(sc, "create USB event queue");

  rtems_interrupt_catch(interrupt_handler, MM_IRQ_USB, &dummy);
  bsp_interrupt_vector_enable(MM_IRQ_USB);

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver usbinput_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  uint32_t count;

  rtems_message_queue_flush(event_q, &count);
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver usbinput_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  rtems_status_code sc;

  if(rw_args->count < 8) {
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

static void load_firmware(const unsigned char *firmware, int length)
{
  int i, nwords;
  volatile unsigned int *usb_dmem
    = (volatile unsigned int *)MM_SOFTUSB_DMEM_BASE;
  volatile unsigned int *usb_pmem
    = (volatile unsigned int *)MM_SOFTUSB_PMEM_BASE;

  MM_WRITE(MM_SOFTUSB_CONTROL, SOFTUSB_CONTROL_RESET);
  for(i=0;i<SOFTUSB_DMEM_SIZE/4;i++)
    usb_dmem[i] = 0;
  for(i=0;i<SOFTUSB_PMEM_SIZE/2;i++)
    usb_pmem[i] = 0;
  nwords = (length+1)/2;
  for(i=0;i<nwords;i++)
    usb_pmem[i] = ((unsigned int)(firmware[2*i]))
      |((unsigned int)(firmware[2*i+1]) << 8);
  MM_WRITE(MM_SOFTUSB_CONTROL, 0);
}

rtems_device_driver usbinput_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_libio_ioctl_args_t *args = arg;
  struct usbinput_firmware_description *fd
    = (struct usbinput_firmware_description *)args->buffer;

  if(args->command == USBINPUT_LOAD_FIRMWARE) {
    load_firmware(fd->data, fd->length);
    args->ioctl_return = 0;
    return RTEMS_SUCCESSFUL;
  } else {
    args->ioctl_return = -1;
    return RTEMS_UNSATISFIED;
  }
}
