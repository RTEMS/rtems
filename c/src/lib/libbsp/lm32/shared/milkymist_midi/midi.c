/*  midi.c
 *
 *  Milkymist MIDI driver for RTEMS
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
#include "milkymist_midi.h"

#define DEVICE_NAME "/dev/midi"

static rtems_id midi_q;
static unsigned char *midi_p = NULL;
static unsigned char midi_msg[3];

static rtems_isr interrupt_handler(rtems_vector_number n)
{
  unsigned char msg;

  while (MM_READ(MM_MIDI_STAT) & MIDI_STAT_RX_EVT) {
    msg = MM_READ(MM_MIDI_RXTX);
    MM_WRITE(MM_MIDI_STAT, MIDI_STAT_RX_EVT);

    if ((msg & 0xf8) == 0xf8)
      continue; /* ignore system real-time */

    if (msg & 0x80)
      midi_p = midi_msg; /* status byte */

    if (!midi_p)
      continue; /* ignore extra or unsynchronized data */

    *midi_p++ = msg;

    if (midi_p == midi_msg+3) {
      /* received a complete MIDI message */
      rtems_message_queue_send(midi_q, midi_msg, 3);
      midi_p = NULL;
    }
  }
  lm32_interrupt_ack(1 << MM_IRQ_MIDI);
}

rtems_device_driver midi_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code sc;
  rtems_isr_entry dummy;

  sc = rtems_io_register_name(DEVICE_NAME, major, 0);
  RTEMS_CHECK_SC(sc, "create MIDI input device");

 sc = rtems_message_queue_create(
    rtems_build_name('M', 'I', 'D', 'I'),
    32,
    3,
    0,
    &midi_q
  );
  RTEMS_CHECK_SC(sc, "create MIDI queue");

  rtems_interrupt_catch(interrupt_handler, MM_IRQ_MIDI, &dummy);
  bsp_interrupt_vector_enable(MM_IRQ_MIDI);
  /* Only MIDI THRU mode is supported atm */
  MM_WRITE(MM_MIDI_CTRL, MIDI_CTRL_RX_INT|MIDI_CTRL_THRU);

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver midi_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  uint32_t count;

  rtems_message_queue_flush(midi_q, &count);
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver midi_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  rtems_status_code sc;

  sc = rtems_message_queue_receive(
    midi_q,
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
