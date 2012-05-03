/*  milkymist_ac97.h
 *
 *  Milkymist AC97 driver for RTEMS
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2010 Sebastien Bourdeauducq
 */

#ifndef __MILKYMIST_AC97_H_
#define __MILKYMIST_AC97_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Ioctls. 0x41 is 'A' */
#define SOUND_MIXER_MIC             0x0
#define SOUND_MIXER_LINE            0x1
#define SOUND_MIXER_READ(x)         (0x4100+x)
#define SOUND_MIXER_WRITE(x)        (0x4110+x)

#define SOUND_SND_SUBMIT_PLAY       0x4120
#define SOUND_SND_COLLECT_PLAY      0x4121
#define SOUND_SND_SUBMIT_RECORD     0x4122
#define SOUND_SND_COLLECT_RECORD    0x4123

struct snd_buffer {
  unsigned int nsamples;
  void *user;
  unsigned int samples[];
};

rtems_device_driver ac97_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver ac97_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver ac97_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

#define AC97_DRIVER_TABLE_ENTRY {ac97_initialize, \
ac97_open, NULL, NULL, NULL, ac97_control}

#ifdef __cplusplus
}
#endif

#endif /* __MILKYMIST_AC97_H_ */
