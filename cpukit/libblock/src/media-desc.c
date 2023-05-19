/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSMedia
 *
 * @brief Media implementation.
 */

/*
 * Copyright (C) 2009, 2010 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
