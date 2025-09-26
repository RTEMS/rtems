/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  @file
 *  @brief Test suite for ftw.h methods
 */

/*
 * Copyright (C) 2020 Eshan Dhawan, embedded brains GmbH & Co. KG
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

/* Header files */
#include <bsp.h> /* for device driver prototypes */
#include "tmacros.h"
#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/untar.h>
#include <rtems/error.h>
#include <ftw.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <rtems/test.h>

#include "psxftw01-tar.h"
#include "psxftw01-tar-gz.h"

#define TARFILE_START    psxftw01_tar
#define TARFILE_SIZE     psxftw01_tar_size
#define TARFILE_GZ_START psxftw01_tar_gz
#define TARFILE_GZ_SIZE  psxftw01_tar_gz_size

const char rtems_test_name[] = "psxftw01";
/* Function decleration to avoid warning */
void *POSIX_Init (void * argument);

static char  file_traverse_order[6][20];
static int file_order;

static int fn_function (const char *fpath, const struct stat *sb,
int tflag, struct FTW *ftwbuf)
{
  (void) sb;
  (void) tflag;

  strcpy(file_traverse_order [file_order], fpath + ftwbuf->base) ;
  file_order = file_order + 1;
  return 0; /* to make nftw run further */
}


T_TEST_CASE(ftw)
{

  /* VARIABLE DECLEARATION */
  int r;
  char *files_path;
  int flags;
  int i;
  /* setting up filesystem */
  rtems_status_code sc;

  /*Untaring from memory */
  sc = Untar_FromMemory_Print(
   (void *)TARFILE_START,
   TARFILE_SIZE,
   &rtems_test_printer
   );
  T_rsc_success( sc );

  /* Array with expected file order */
  char arr_ftw_depth[5][20] = { "test_file", "test_script", "def", "abc", "home" };
  char arr_ftw_phys[5][20] = { "home", "test_file", "abc", "def", "test_script" };
  /* defining the path to run nftw*/
  files_path = "/home";
  /* nftw test with FTW_DEPTH flag*/
  flags = 0;
  file_order = 0;
  flags |= FTW_DEPTH;
  r = nftw( files_path, fn_function, 5, flags );
  T_psx_success( r );

  /*comparing the nftw file tree to the expexted file tree traversal */
  for (i = 0; i < file_order; i++){
    r = strcmp( arr_ftw_depth[i], file_traverse_order[i]);
    T_eq_int( r, 0 );
  }
    /*----------------Test Block 2--------------------*/
  flags = 0;
  file_order = 0;
  flags |= FTW_PHYS;
  r = nftw( files_path, fn_function, 5, flags );
  T_psx_success(r);

  /*comparing the nftw file tree to the expected file tree traversal*/
  for (i = 0; i < file_order; i++){
    r = strcmp( arr_ftw_phys[i], file_traverse_order[i]);
    T_eq_int( r, 0 );
  }

}

void *POSIX_Init (void * argument)
{
  rtems_test_run( (rtems_task_argument) argument, TEST_STATE );
}

/* NOTICE: the clock driver is explicitly disabled */

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS                  1

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 6

#define CONFIGURE_MAXIMUM_POSIX_THREADS  2

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT


#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
