/**
 * @file
 *
 * @ingroup RTEMSMedia
 *
 * @brief Media implementation.
 */

/*
 * Copyright (c) 2009, 2010 embedded brains GmbH.  All rights reserved.
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

#include <rtems/media.h>

static const char *const rtems_media_event_desc_table [] = {
  [RTEMS_MEDIA_EVENT_DISK_ATTACH] = "DISK ATTACH",
  [RTEMS_MEDIA_EVENT_DISK_DETACH] = "DISK DETACH",
  [RTEMS_MEDIA_EVENT_MOUNT] = "MOUNT",
  [RTEMS_MEDIA_EVENT_UNMOUNT] = "UNMOUNT",
  [RTEMS_MEDIA_EVENT_PARTITION_INQUIRY] = "PARTITION INQUIRY",
  [RTEMS_MEDIA_EVENT_PARTITION_ATTACH] = "PARTITION ATTACH",
  [RTEMS_MEDIA_EVENT_PARTITION_DETACH] = "PARTITION DETACH"
};

static const char *const rtems_media_state_desc_table [] = {
  [RTEMS_MEDIA_STATE_INQUIRY] = "INQUIRY",
  [RTEMS_MEDIA_STATE_ABORTED] = "ABORTED",
  [RTEMS_MEDIA_STATE_READY] = "SUCCESS",
  [RTEMS_MEDIA_STATE_FAILED] = "FAILED",
  [RTEMS_MEDIA_STATE_SUCCESS] = "SUCCESS",
  [RTEMS_MEDIA_ERROR_DISK_UNKNOWN] = "ERROR DISK UNKNOWN",
  [RTEMS_MEDIA_ERROR_DISK_EXISTS] = "ERROR DISK EXISTS",
  [RTEMS_MEDIA_ERROR_DISK_OR_PARTITION_UNKNOWN] = "ERROR DISK OR PARTITION UNKNOWN",
  [RTEMS_MEDIA_ERROR_DISK_OR_PARTITION_EXISTS] = "ERROR DISK OR PARTITION EXISTS",
  [RTEMS_MEDIA_ERROR_PARTITION_UNKNOWN] = "ERROR PARTITION UNKNOWN",
  [RTEMS_MEDIA_ERROR_PARTITION_ORPHAN] = "ERROR PARTITION ORPHAN",
  [RTEMS_MEDIA_ERROR_PARTITION_DETACH_WITH_MOUNT] = "ERROR PARTITION DETACH WITH MOUNT",
  [RTEMS_MEDIA_ERROR_PARTITION_WITH_UNKNOWN_DISK] = "ERROR PARTITION WITH UNKNOWN DISK",
  [RTEMS_MEDIA_ERROR_MOUNT_POINT_UNKNOWN] = "ERROR MOUNT POINT UNKNOWN",
  [RTEMS_MEDIA_ERROR_MOUNT_POINT_EXISTS] = "ERROR MOUNT POINT EXISTS",
  [RTEMS_MEDIA_ERROR_MOUNT_POINT_ORPHAN] = "ERROR MOUNT POINT ORPHAN"
};

#define TC(table) (sizeof(table) / sizeof(table [0]))

const char *rtems_media_event_description(rtems_media_event event)
{
  if ((size_t) event < TC(rtems_media_event_desc_table)) {
    return rtems_media_event_desc_table [event];
  } else {
    return "INVALID";
  } 
}

const char *rtems_media_state_description(rtems_media_state state)
{
  if ((size_t) state < TC(rtems_media_state_desc_table)) {
    return rtems_media_state_desc_table [state];
  } else {
    return "INVALID";
  }
}
