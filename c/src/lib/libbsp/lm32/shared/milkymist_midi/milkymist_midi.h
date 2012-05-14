/*  milkymist_midi.h
 *
 *  Milkymist MIDI driver for RTEMS
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2010 Sebastien Bourdeauducq
 */

#ifndef __MILKYMIST_MIDI_H_
#define __MILKYMIST_MIDI_H_

#ifdef __cplusplus
extern "C" {
#endif

rtems_device_driver midi_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver midi_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver midi_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

#define MIDI_DRIVER_TABLE_ENTRY {midi_initialize, \
midi_open, NULL, midi_read, NULL, NULL}

#ifdef __cplusplus
}
#endif

#endif /* __MILKYMIST_MIDI_H_ */
