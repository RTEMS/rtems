/*
 *  @file
 *  @brief Test suite for ftw.h methods
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2020 Eshan Dhawan, embedded brains GmbH, Joel Sherrill
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

static char buffer[512];

static const T_action actions[] = {
  T_report_hash_sha256,
  T_check_task_context,
  T_check_file_descriptors,
  T_check_rtems_barriers,
  T_check_rtems_extensions,
  T_check_rtems_message_queues,
  T_check_rtems_partitions,
  T_check_rtems_periods,
  T_check_rtems_regions,
  T_check_rtems_semaphores,
  T_check_rtems_tasks,
  T_check_rtems_timers,
  T_check_posix_keys
};

static const T_config config = {
  .name = "psxftw01",
  .buf = buffer,
  .buf_size = sizeof(buffer),
  .putchar = rtems_put_char,
  .verbosity = T_VERBOSE,
  .now = T_now_clock,
  .action_count = T_ARRAY_SIZE(actions),
  .actions = actions
};


static int fn_function (const char *fpath, const struct stat *sb,
int tflag, struct FTW *ftwbuf)
{
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
  if (sc != RTEMS_SUCCESSFUL) {
     printf ("error: untar failed: %s\n", rtems_status_text (sc));
   }

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

  T_quiet_psx_success(r);

  /*comparing the nftw file tree to the expexted file tree traversal */
  for (i = 0; i < file_order; i++){
    r = strcmp( arr_ftw_depth[i], file_traverse_order[i]);
    if (r){
      printf( "Incorrect Order " );
    }
    T_quiet_psx_success(r);
  }
    /*----------------Test Block 2--------------------*/
  flags = 0;
  file_order = 0;
  flags |= FTW_PHYS;
  r = nftw( files_path, fn_function, 5, flags );
  T_quiet_psx_success(r);

  /*comparing the nftw file tree to the expected file tree traversal*/
  for (i = 0; i < file_order; i++){
    r = strcmp( arr_ftw_phys[i], file_traverse_order[i]);
    if (r){
      printf( "Incorrect Order " );
    }
    T_quiet_psx_success(r);
  }

}

void *POSIX_Init (void * argument)
{
  int exit_code;

  TEST_BEGIN();

  T_register();
  exit_code = T_main(&config);
  if (exit_code == 0) {
    TEST_END();
  }

 rtems_test_exit(exit_code);

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
