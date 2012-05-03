/*
 * RTEMS for Nintendo DS sound driver.
 *
 * Copyright (c) 2008 by Matthieu Bucchianeri <mbucchia@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>

#include <bsp.h>
#include <rtems/irq.h>
#include <rtems/libio.h>
#include <nds.h>

#include "sound.h"
#include "../include/my_ipc.h"

/*
 * default values.
 */

static int volume = 64;
static int pan = 64;
static int freq = 11025;
static int format = 0;          // 0 is 8 bits and 1 is 16 bits
static int offset = 0;

/*
 * sound device driver initialize entry point.
 */

rtems_device_driver
sound_initialize (rtems_device_major_number major,
                  rtems_device_minor_number minor, void *arg)
{
  rtems_status_code status;

  printk ("[+] sound started\n");

  /* register the devices */
  status = rtems_io_register_name ("/dev/pcm", major, 0);
  if (status != RTEMS_SUCCESSFUL) {
    printk ("[!] error registering sound\n");
    rtems_fatal_error_occurred (status);
  }

  /* reset IPCs structs */
  IPC->soundData = NULL;
  memset ((void *)my_IPC, 0, sizeof (*my_IPC));
  DC_FlushAll ();

  return RTEMS_SUCCESSFUL;
}

/*
 * sound device driver open operation.
 */

rtems_device_driver
sound_open (rtems_device_major_number major,
            rtems_device_minor_number minor, void *arg)
{
  return RTEMS_SUCCESSFUL;
}

/*
 * sound device driver close operation.
 */

rtems_device_driver
sound_close (rtems_device_major_number major,
             rtems_device_minor_number minor, void *arg)
{
  return RTEMS_SUCCESSFUL;
}

/*
 * sound device driver read operation. read data from recorded microphone
 * samples.
 */

rtems_device_driver
sound_read (rtems_device_major_number major,
            rtems_device_minor_number minor, void *arg)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *) arg;

  /* check end of file */
  if (offset >= my_IPC->recorded_length) {
    rw_args->bytes_moved = 0;

    return RTEMS_SUCCESSFUL;
  }

  /* read buffer */
  memcpy ((void *)rw_args->buffer, (void *)my_IPC->record_buffer + offset, rw_args->count);
  offset += rw_args->count;
  rw_args->bytes_moved = rw_args->count;
  return RTEMS_SUCCESSFUL;
}

/*
 * sound device driver write operation. read a pcm sound.
 */

rtems_device_driver
sound_write (rtems_device_major_number major,
             rtems_device_minor_number minor, void *arg)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *) arg;
  static TransferSound snd;

  /* XXX not exclusive */
  while (IPC->soundData != NULL);

  snd.count = 1;
  snd.data[0].data = rw_args->buffer;
  snd.data[0].len = rw_args->count;
  snd.data[0].rate = freq;
  snd.data[0].vol = volume;
  snd.data[0].pan = pan;
  snd.data[0].format = format;

  IPC->soundData = &snd;
  DC_FlushAll ();

  rw_args->bytes_moved = rw_args->count;
  return RTEMS_SUCCESSFUL;
}

/*
 * ioctl entry point.
 */

rtems_device_driver
sound_control (rtems_device_major_number major,
               rtems_device_minor_number minor, void *arg)
{
  rtems_libio_ioctl_args_t *args = arg;
  uint32_t *val = (uint32_t *) args->buffer;

  switch (args->command) {
    /* sound volume management */
  case SOUND_SVOL:
    volume = *val;
    break;
  case SOUND_GVOL:
    *val = volume;
    break;
    /* sound format settings */
  case SOUND_SFMT:
    switch (*val & SOUND_FREQ_MASK) {
    case SOUND_FREQ_11025:
      freq = 11025;
      break;
    case SOUND_FREQ_22050:
      freq = 22050;
      break;
    case SOUND_FREQ_44100:
      freq = 44100;
      break;
    case SOUND_FREQ_RECORD:
      freq = 16384;
      break;
    }
    format = (*val & SOUND_FORMAT_16) ? 1 : 0;
    printk ("[+] pcm %d bits @ %d hz\n", format ? 16 : 8, freq);
    break;
  case SOUND_GFMT:
    if (freq == 11025) {
      *val = SOUND_FREQ_11025;
    } else if (freq == 22050) {
      *val = SOUND_FREQ_22050;
    } else if (freq == 44100) {
      *val = SOUND_FREQ_44100;
    } else if (freq == 16384) {
      *val = SOUND_FREQ_RECORD;
    }
    *val |= format ? SOUND_FORMAT_16 : SOUND_FORMAT_8;
    break;
    /* sound pan settings */
  case SOUND_SPAN:
    pan = *val;
    break;
  case SOUND_GPAN:
    *val = pan;
    break;
    /* set recording buffer length */
  case SOUND_SRECLEN:
    my_IPC->record_buffer = malloc (*val);
    my_IPC->record_length_max = *val;
    printk ("[+] mic buffer %u B @ %p\n", my_IPC->record_length_max,
            my_IPC->record_buffer);
    break;
    /* start recording */
  case SOUND_STARTREC:
    my_IPC->recorded_length = 0;
    my_IPC->record = 1;
    break;
    /* stop recording */
  case SOUND_STOPREC:
    if (!my_IPC->record) {
      printk ("[!] STOPREC while not recording\n");
    }
    my_IPC->record = 0;
    while (my_IPC->recorded_length == 0);
    offset = 0;
    if (val != NULL) {
      *val = my_IPC->recorded_length;
    }
    break;
  default:
    break;
  }
  return RTEMS_SUCCESSFUL;
}
