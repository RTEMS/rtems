/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2026 Gedare Bloom.
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

#include <bsp.h>
#include "tmacros.h"
#include <rtems/imfs.h>
#include <rtems/error.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <libfdt.h>
#include <rtems/rtems-fdt.h>

#include <rtems/rtems-fdtimpl.h>

#include "fdt02-tar.h"

const char rtems_test_name[] = "FDT 02";

rtems_task Init( rtems_task_argument argument );

rtems_task Init( rtems_task_argument ignored )
{
  (void) ignored;
  rtems_status_code sc;
  rtems_fdt_handle  handle;
  int               rc;

  TEST_BEGIN();

  sc = rtems_tarfs_load( "/", (void *) fdt02_tar, fdt02_tar_size );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  rc = rtems_fdt_load_blob_file_binary( "/test01.dtb", &handle );
  rtems_test_assert( rc == 0 );

  rc = rtems_fdt_unload( &handle );
  rtems_test_assert( rc == 0 );

  TEST_END();
  rtems_test_exit( 0 );
}

/* NOTICE: the clock driver is explicitly disabled */
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS            1
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS ( 3 + 1 )

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
