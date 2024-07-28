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
#include <stdint.h>
#include <unistd.h>

#include "fstest.h"
#include "fs_config.h"
#include <tmacros.h>

const char rtems_test_name[] = "FSRMDIRPARENT " FILESYSTEM;
const RTEMS_TEST_STATE rtems_test_state = TEST_STATE;

static void rmdir_error (void)
{
  int status;
  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;

  const char *wd = __func__;

  /*
   * Create a new directory and change the current directory to this
   */
  status = mkdir (wd, mode);
  rtems_test_assert (status == 0);
  status = chdir (wd);
  rtems_test_assert (status == 0);

  /*
   * Try to remove the relative parent directory
  */
  EXPECT_EQUAL (-1, rmdir, "..");
  puts ("Testing errno for ENOTEMPTY or EBUSY");
  if ( errno == ENOTEMPTY || errno == EBUSY ) {
    FS_PASS ();
  } else {
    FS_FAIL ();
  }

  /*
   * Create a new directory for test
   */
  status = mkdir ("tmp", mode);
  rtems_test_assert (status == 0);

  /*
   * The path argument names a directory that is not an empty directory,
   * or there are hard links to the directory other than
   * dot or a single entry in dot-dot.
   */
  EXPECT_ERROR (ENOTEMPTY, rmdir, "tmp/..");

  /*
   * Go back to parent directory
   */
  status = chdir ("..");
  rtems_test_assert (status == 0);


}

void test (void)
{
  rmdir_error ();
}
