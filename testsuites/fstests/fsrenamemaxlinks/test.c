/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
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

#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "fstest.h"
#include "fs_config.h"
#include <tmacros.h>

const char rtems_test_name[] = "FSRENAMEMAXLINKS " FILESYSTEM;
const RTEMS_TEST_STATE rtems_test_state = TEST_STATE;

void test (void)
{
  int status;
  int rv;
  int i;

  const char *dir01 = "dir01";
  const char *dir02 = "dir02";

  char path01[30];

  char link_name[10];

  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
  const char *wd = __func__;

  struct stat statbuf;

  long LINK_MAX_val;

  /*
   * Create a new directory and change the current directory to this
   */

  status = mkdir (wd, mode);
  rtems_test_assert (status == 0);
  status = chdir (wd);
  rtems_test_assert (status == 0);

  /*
   * The new argument points to a non existant directory and
   * the old argument points to an existant directory at LINK_MAX.
   */

  status = mkdir (dir01, mode);
  rtems_test_assert (status == 0);

  LINK_MAX_val = pathconf (dir01, _PC_LINK_MAX);
  rtems_test_assert (LINK_MAX_val >= 0);

  status = stat (dir01, &statbuf);
  rtems_test_assert (status == 0);

  for(i = statbuf.st_nlink; i < LINK_MAX_val; i++)
  {
    rv = snprintf (link_name, sizeof(link_name), "%s/%d", dir01, i);
    rtems_test_assert (rv < sizeof(link_name));

    status = mkdir (link_name, mode);
    rtems_test_assert (status == 0);
  }

  status = mkdir (dir02, mode);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "%s/%s", dir01, dir01);
  rtems_test_assert (rv < sizeof(path01));
  EXPECT_ERROR (EMLINK, rename, dir02, path01);

  /*
   * Clear directory
   */

  for(i = statbuf.st_nlink; i < LINK_MAX_val; i++)
  {
    rv = snprintf (link_name, sizeof(link_name), "%s/%d", dir01, i);
    rtems_test_assert (rv < sizeof(link_name));

    status = rmdir (link_name);
    rtems_test_assert (status == 0);
  }

  EXPECT_EQUAL (-1, rmdir, path01);
  EXPECT_EQUAL (0, rmdir, dir02);
  EXPECT_EQUAL (0, rmdir, dir01);

  /*
   * Go back to parent directory
   */

  status = chdir ("..");
  rtems_test_assert (status == 0);

  /*
   * Remove test directory
   */

  status = rmdir (wd);
  rtems_test_assert (status == 0);
}
