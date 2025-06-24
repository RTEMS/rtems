/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2024 On-Line Applications Research Corporation (OAR)
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

#ifndef JFFS2_FLASHDEV_H
#define JFFS2_FLASHDEV_H

#include <dev/flash/flashdev.h>
#include <rtems/jffs2.h>
#include <rtems/rtems/status.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * This function mounts a JFFS2 partition over the specified region on a
 * flashdev-controlled device file.
 *
 * @param  flashdev_path is a device file created by the flashdev backend
 * @param  mount_dir is the location at which to mount the JFFS2 filesystem
 * @param  region is a description of the segment of the device file to mount
 * @param  compressor_control is the JFFS2 compression control
 * @param  read_only is a flag indicating whether the mounted JFFS2 filesystem
 *         can be written
 *
 * @return RTEMS_SUCCESSFUL on success.
 */
rtems_status_code jffs2_flashdev_mount(
  const char *flashdev_path,
  const char *mount_dir,
  rtems_flashdev_region *region,
  rtems_jffs2_compressor_control *compressor_control,
  bool read_only
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* JFFS2_FLASHDEV_H */
