/*  milkymist_video.h
 *
 *  Milkymist video input driver for RTEMS
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2010, 2011 Sebastien Bourdeauducq
 */

#ifndef __MILKYMIST_VIDEO_H_
#define __MILKYMIST_VIDEO_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Ioctls */
#define VIDEO_BUFFER_LOCK      0x5600
#define VIDEO_BUFFER_UNLOCK    0x5601

#define VIDEO_SET_BRIGHTNESS   0x5602
#define VIDEO_GET_BRIGHTNESS   0x5603
#define VIDEO_SET_CONTRAST     0x5604
#define VIDEO_GET_CONTRAST     0x5605
#define VIDEO_SET_HUE          0x5606
#define VIDEO_GET_HUE          0x5607
#define VIDEO_GET_SIGNAL       0x5608

#define VIDEO_SET_REGISTER     0x5609
#define VIDEO_GET_REGISTER     0x560a

#define VIDEO_SET_FORMAT       0x560b

enum {
  VIDEO_FORMAT_CVBS6 = 0,
  VIDEO_FORMAT_CVBS5,
  VIDEO_FORMAT_CVBS4,
  VIDEO_FORMAT_SVIDEO,
  VIDEO_FORMAT_COMPONENT,
};

rtems_device_driver video_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver video_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver video_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver video_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

#define VIDEO_DRIVER_TABLE_ENTRY {video_initialize, \
video_open, video_close, NULL, NULL, video_control}

#ifdef __cplusplus
}
#endif

#endif /* __MILKYMIST_VIDEO_H_ */
