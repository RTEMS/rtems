/*
 * Copyright (c) 2008 by Matthieu Bucchianeri <mbucchia@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE
 */

#ifndef _SOUND_H_
#define _SOUND_H_

/* ioctls for sound management. 0x53 is 'S' */
#define SOUND_SVOL	0x5301  /* set sound volume */
#define SOUND_GVOL	0x5302  /* get sound volume */
#define SOUND_SFMT	0x5303  /* set output format */
#define SOUND_GFMT	0x5304  /* get output format */
#define SOUND_SPAN	0x5305  /* set pan */
#define SOUND_GPAN	0x5306  /* get pan */

#define SOUND_SRECLEN	0x5307  /* set recording buffer length (in bytes) */
#define SOUND_STARTREC	0x5308  /* start recording */
#define SOUND_STOPREC	0x5309  /* stop recording */

#define SOUND_FREQ_MASK		0x3
#define SOUND_FREQ_11025	0
#define SOUND_FREQ_22050	1
#define SOUND_FREQ_44100	2
#define SOUND_FREQ_RECORD	3       /* special value to play microphone
                                           recorded data */

#define SOUND_FORMAT_8		(0 << 2)
#define SOUND_FORMAT_16		(1 << 2)

#ifdef __cplusplus
extern "C"
{
#endif

  /*
   * sound prototype entry points
   */
  rtems_device_driver sound_initialize (rtems_device_major_number,
                                        rtems_device_minor_number, void *);

  rtems_device_driver sound_open (rtems_device_major_number,
                                  rtems_device_minor_number, void *);

  rtems_device_driver sound_control (rtems_device_major_number,
                                     rtems_device_minor_number, void *);

  rtems_device_driver sound_close (rtems_device_major_number,
                                   rtems_device_minor_number, void *);

  rtems_device_driver sound_read (rtems_device_major_number,
                                  rtems_device_minor_number, void *);

  rtems_device_driver sound_write (rtems_device_major_number,
                                   rtems_device_minor_number, void *);

  rtems_device_driver sound_control (rtems_device_major_number,
                                     rtems_device_minor_number, void *);

#define SOUND_DRIVER_TABLE_ENTRY \
  { sound_initialize, sound_open, sound_close, \
    sound_read, sound_write, sound_control }


#ifdef __cplusplus
}
#endif

#endif
