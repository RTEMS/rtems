/*
 * RTEMS Project (http://www.rtems.org/)
 *
 * Copyright 2007 Chris Johns (chrisj@rtems.org)
 */

/**
 * Flash Disk Device Driver.
 *
 * Am29LV160D 16 Megabit (2M x 8bit) 3.0 Volt-only
 * Boot Sctor Flash Memory.
 */

#if !defined (_RTEMS_AM29LV160_H_)
#define _RTEMS_AM29LV160_H_

#include <rtems/flashdisk.h>

/**
 * The segments in the AM29LV160 top boot block device.
 */
#define rtems_am29lv160t_segment_count (4)
extern const rtems_fdisk_segment_desc rtems_am29lv160t_segments[4];

/**
 * The segments in the AM29LV160 bottom boot block device.
 */
#define rtems_am29lv160b_segment_count (4)
extern const rtems_fdisk_segment_desc rtems_am29lv160b_segments[4];

/**
 * The segments in the AM29LV160 top boot block device.
 */
extern const rtems_fdisk_driver_handlers rtems_am29lv160_handlers;

/**
 * The device configuration.
 */
typedef struct rtems_am29lv160_config
{
  int   bus_8bit;
  void* base;
} rtems_am29lv160_config;

/**
 * External reference to the configuration.
 */
extern const rtems_am29lv160_config rtems_am29lv160_configuration[];

/**
 * External reference to the configuration size
 */
extern uint32_t rtems_am29lv160_configuration_size;

#endif
