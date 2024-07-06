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

const char rtems_test_name[] = "FSRENAMEEXISTING " FILESYSTEM;
const RTEMS_TEST_STATE rtems_test_state = TEST_STATE;

static void rename_error (void)
{

  int fd;
  int status;
  char *name01 = "name01";
  char *name02 = "name02";
  char *name03 = "name03";
  char *nonexistence = "name04";

  char name[20];

  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
  const char *wd = __func__;

  /*
   *Create a new directory and change the current directory to  this
   */

  status = mkdir (wd, mode);
  rtems_test_assert (status == 0);
  status = chdir (wd);
  rtems_test_assert (status == 0);


  /*
   * Create a new directory and a new directory in it
   */

  status = mkdir (name01, mode);
  rtems_test_assert (status == 0);
  status = mkdir (name02, mode);
  rtems_test_assert (status == 0);
  sprintf (name, "%s/%s", name01, name03);
  status = mkdir (name, mode);
  rtems_test_assert (status == 0);

  /*
   * The new directory pathname contains a path prefix
   *  that names the old directory.
   */
  EXPECT_ERROR (EINVAL, rename, name01, name);
  /*
   * The new argument points to a directory and
   *  the old argument points to a file that is not a directory.
   */
  fd = creat (name03, mode);
  status = close (fd);
  rtems_test_assert (status == 0);
  EXPECT_ERROR (EISDIR, rename, name03, name02);

  /*
   * The link named by old does not name an existing file,
   *    or either old or new points to an empty string.
   */

  EXPECT_ERROR (ENOENT, rename, nonexistence, name01);
  EXPECT_ERROR (ENOENT, rename, "", name01);

  /*
   * A component of either path prefix is not a directory;
   * or the old argument names a directory and new argument names
   *  a non-directory file.
   */

  sprintf (name, "%s/%s", name03, name01);
  EXPECT_ERROR (ENOTDIR, rename, name, name03);
  EXPECT_ERROR (ENOTDIR, rename, name02, name03);

  /*
   * Go back to parent directory
   */
  status = chdir ("..");
  rtems_test_assert (status == 0);
}

static void same_file_test (void)
{
  int fd;
  int status;

  const char *name01 = "name01";

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
   * The new argument points to a file and
   * the old argument points to the same file on the same directory.
   */

  puts ("\nRename file with itself\n");

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  EXPECT_EQUAL (0, rename, name01, name01);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, name01);

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


static void directory_test (void)
{
  int fd;
  int status;
  int rv;

  const char *name01 = "name01";

  const char *dir01 = "dir01";
  const char *dir02 = "dir02";

  char path01[30];

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
   * The new argument points to a file and
   * the old argument points to a directory.
   */

  puts ("\nRename directory with file\n");

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  status = mkdir (dir01, mode);
  rtems_test_assert (status == 0);

  EXPECT_ERROR (ENOTDIR, rename, dir01, name01);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, name01);
  EXPECT_EQUAL (0, rmdir, dir01);

  /*
   * The new argument points to a directory and
   * the old argument points to a file.
   */

  puts ("\nRename file with directory\n");

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  status = mkdir (dir01, mode);
  rtems_test_assert (status == 0);

  EXPECT_ERROR (EISDIR, rename, name01, dir01);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, name01);
  EXPECT_EQUAL (0, rmdir, dir01);

  /*
   * The new argument points to an empty directory and
   * the old argument points to an ancestor directory of new.
   */

  puts ("\nRename directory with ancestor directory\n");

  status = mkdir (dir02, mode);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "%s/%s", dir02, dir01);
  rtems_test_assert (rv < sizeof(path01));
  status = mkdir (path01, mode);
  rtems_test_assert (status == 0);

  EXPECT_ERROR (EINVAL, rename, dir02, path01);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, rmdir, path01);
  EXPECT_EQUAL (0, rmdir, dir02);

  /*
   * The new argument points to an empty directory and
   * the old argument points to other empty directory.
   */

  puts ("\nRename empty directory with another empty directory\n");

  status = mkdir (dir01, mode);
  rtems_test_assert (status == 0);

  status = mkdir (dir02, mode);
  rtems_test_assert (status == 0);

  EXPECT_EQUAL (0, rename, dir01, dir02);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (-1, rmdir, dir01);
  EXPECT_EQUAL (0, rmdir, dir02);

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

static void arg_format_test (void)
{
  int fd;
  int status;
  const char *name01 = "name01";

  const char *dir01 = "dir01";

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
   * The new argument points to a directory and
   * the old argument points to current directory.
   */

  puts ("\nRename directory with current directory\n");

  status = mkdir (dir01, mode);
  rtems_test_assert (status == 0);

  EXPECT_EQUAL (-1, rename, "." , dir01);

  puts("Testing errno for EINVAL or EBUSY");

  if (errno == EINVAL || errno == EBUSY) {
    FS_PASS ();
  } else {
    FS_FAIL ();
  }

  /*
   * The new argument points to current directory and
   * the old argument points to a directory.
   */

  EXPECT_EQUAL (-1, rename, dir01, ".");

  puts("Testing errno for EINVAL or EBUSY");

  if (errno == EINVAL || errno == EBUSY) {
    FS_PASS ();
  } else {
    FS_FAIL ();
  }

  /*
   * The new argument points to a directory and
   * the old argument points to previous directory.
   */

  puts ("\nRename directory with previous directory\n");

  EXPECT_EQUAL (-1, rename, ".." , dir01);

  puts("Testing errno for EINVAL or EBUSY");

  if (errno == EINVAL || errno == EBUSY) {
    FS_PASS ();
  } else {
    FS_FAIL ();
  }

  /*
   * The new argument points to previous directory and
   * the old argument points to a directory.
   */

  EXPECT_EQUAL (-1, rename, dir01, "..");

  puts("Testing errno for EINVAL or EBUSY");

  if (errno == EINVAL || errno == EBUSY) {
    FS_PASS ();
  } else {
    FS_FAIL ();
  }

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, rmdir, dir01);

  /*
   * The new argument points to a file and
   * the old argument is an empty string.
   */

  puts("\nTesting empty filepaths\n");

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  EXPECT_ERROR (ENOENT, rename, name01, "");

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, name01);

  /*
   * The new argument is an empty string and
   * the old argument points to a file.
   */

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  EXPECT_ERROR (ENOENT, rename, "", name01);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, name01);

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

void test (void)
{
  rename_error ();
  same_file_test ();
  directory_test ();
  arg_format_test ();
}