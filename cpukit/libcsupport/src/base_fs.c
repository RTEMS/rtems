/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Base File System Initialization
 *  @ingroup LibIO
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/libio_.h>

/*
 *  Default mode for created files.
 */

void rtems_filesystem_initialize( void )
{
  int rv = 0;
  const rtems_filesystem_mount_configuration *root_config =
    &rtems_filesystem_root_configuration;

  rv = mount(
    root_config->source,
    root_config->target,
    root_config->filesystemtype,
    root_config->options,
    root_config->data
  );
  if ( rv != 0 )
    rtems_fatal_error_occurred( 0xABCD0002 );

  /*
   *  Traditionally RTEMS devices are under "/dev" so install this directory.
   *
   *  If the mkdir() fails, we can't print anything so just fatal error.
   */

  rv = mkdir( "/dev", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH );
  if ( rv != 0 )
    rtems_fatal_error_occurred( 0xABCD0003 );

  /*
   *  You can't mount another filesystem properly until the mount point
   *  it will be mounted onto is created.  Moreover, if it is going to
   *  use a device, then it is REALLY unfair to attempt this
   *  before device drivers are initialized.  So we return via a base
   *  filesystem image and nothing auto-mounted at this point.
   */
}
