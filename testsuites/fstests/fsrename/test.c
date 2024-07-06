/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 2014
 *  Andre Marques <andre.lousa.marques at gmail.com>
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

#include "fstest.h"
#include "fs_config.h"
#include "fstest_support.h"
#include <tmacros.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

const char rtems_test_name[] = "FSRENAME " FILESYSTEM;
const RTEMS_TEST_STATE rtems_test_state = TEST_STATE;

static void rename_file_twice_test (void)
{
  const char *name01 = "name01";
  const char *name02 = "name02";
  const char *name03 = "name03";
  mode_t      mode;
  int         fd;
  int         status;

  puts ("\nRename file twice\n");

  mode = S_IRWXU | S_IRWXG | S_IRWXO;
  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  EXPECT_EQUAL (0, rename, name01, name02);
  EXPECT_EQUAL (0, rename, name02, name03);

  errno = 0;
  EXPECT_EQUAL (-1, unlink, name01);
  rtems_test_assert (errno == ENOENT);
  EXPECT_EQUAL (-1, unlink, name02);
  rtems_test_assert (errno == ENOENT);
  EXPECT_EQUAL (0, unlink, name03);
}

static void rename_opened_file_test (void)
{
  const char *name01 = "name01.txt";
  const char *name02 = "name02.txt";
  const char *message = "Test file content";
  mode_t      mode;
  int         fd1;
  int         fd2;
  int         result;
  char        buffer[16];

  puts ("\nRename opened file and open a new file\n");

  mode = S_IRWXU | S_IRWXG | S_IRWXO;
  fd1 = open (name01, O_RDWR | O_CREAT, mode);
  rtems_test_assert (fd1 >= 0);
  result = write (fd1, message, strlen (message));
  rtems_test_assert (result == strlen (message));

  EXPECT_EQUAL (0, rename, name01, name02);

  fd2 = open (name01, O_RDWR | O_CREAT, mode);
  rtems_test_assert (fd1 >= 0);
  EXPECT_EQUAL (0, read, fd2, buffer, sizeof (buffer));

  result = close (fd1);
  rtems_test_assert (result == 0);

  result = close (fd2);
  rtems_test_assert (result == 0);

  EXPECT_EQUAL (0, unlink, name01);
  EXPECT_EQUAL (0, unlink, name02);
}

static void directory_test (void)
{
  int fd;
  int status;
  int rv;

  const char *name02 = "name02";

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
   * The new argument points to an empty directory and
   * the old argument points to a non empty directory.
   */

  puts ("\nRename directory with non empty directory\n");

  status = mkdir (dir01, mode);
  rtems_test_assert (status == 0);

  status = mkdir (dir02, mode);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "%s/%s", dir02, name02);
  rtems_test_assert (rv < sizeof(path01));
  fd = creat (path01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  EXPECT_EQUAL (-1, rename, dir01, dir02);

  puts("Testing errno for EEXIST or ENOTEMPTY");

  if (errno == EEXIST || errno == ENOTEMPTY) {
    FS_PASS ();
  } else {
    FS_FAIL ();
  }

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, path01);
  EXPECT_EQUAL (0, rmdir, dir01);
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

static void arg_test (void)
{
  int fd;
  int status;
  int rv;

  const char *name01 = "name01";
  const char *name02 = "name02";

  const char *dir01 = "dir01";
  const char *dir02 = "dir02";

  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
  const char *wd = __func__;
  
  char path01[20];

  /*
   * Create a new directory and change the current directory to this
   */

  status = mkdir (wd, mode);
  rtems_test_assert (status == 0);
  status = chdir (wd);
  rtems_test_assert (status == 0);

  /*
   * The new argument points to a non existant file and
   * the old argument points to a file.
   */

  puts ("\nRename file with non existant file\n");

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  EXPECT_EQUAL (0, rename, name01, name02);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (-1, unlink, name01);
  EXPECT_EQUAL (0, unlink, name02);

  /*
   * The new argument points to a non existant file and
   * the old argument points to a file where a component of the
   * filepath does not exist.
   */

  puts ("\nRename file with non existant filepath\n");

  status = mkdir (dir01, mode);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "%s/%s/%s", dir01, name01, name02);
  rtems_test_assert (rv < sizeof(path01));
  EXPECT_ERROR (ENOENT, rename, path01, name01);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (-1, unlink, name01);
  EXPECT_EQUAL (0, rmdir, dir01);

  /*
   * The new argument points to a non existant directory and
   * the old argument points to a directory.
   */

  puts ("\nRename directory with non existant directory\n");

  status = mkdir (dir01, mode);
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

static void filesystem_test (void)
{
  int fd;
  int status;
  int rv;

  const char *name01 = "name01";
  const char *name02 = "name02";

  char path01[20];

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
   * The new argument points to a file on another instance of the filesystem and
   * the old argument points to a file on the base filesystem.
   */

  puts ("\nRename files across different filesystems\n");

  rv = chroot ("/");
  rtems_test_assert (rv == 0);

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "%s/%s", BASE_FOR_TEST, name02);
  rtems_test_assert (rv < sizeof(path01));
  EXPECT_ERROR (EXDEV, rename, name01, path01);

  EXPECT_EQUAL (-1, unlink, path01);
  EXPECT_EQUAL (0, unlink, name01);

  rv = chroot (BASE_FOR_TEST);
  rtems_test_assert (rv == 0);

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
  rename_file_twice_test ();
  directory_test ();
  rename_opened_file_test ();
  arg_test ();
  filesystem_test ();
}
