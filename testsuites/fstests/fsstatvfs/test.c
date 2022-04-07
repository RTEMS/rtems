/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 2013 Andrey Mozzhuhin
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

#include <sys/statvfs.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "fstest.h"
#include "fs_config.h"
#include <tmacros.h>

const char rtems_test_name[] = "FSSTATVFS " FILESYSTEM;

static void statvfs_validate(struct statvfs *stat)
{
  rtems_test_assert(stat->f_bsize > 0);
  rtems_test_assert(stat->f_frsize > 0);
  rtems_test_assert(stat->f_blocks > 0);
  rtems_test_assert(stat->f_bfree <= stat->f_blocks);
  rtems_test_assert(stat->f_bavail <= stat->f_blocks);
  rtems_test_assert(stat->f_ffree <= stat->f_files);
  rtems_test_assert(stat->f_favail <= stat->f_files);
  rtems_test_assert(stat->f_namemax > 0);
}

static void statvfs_test01(void)
{
  struct statvfs statbuf1, statbuf2;
  int status;
  int fd;
  ssize_t n;
  const char *databuf = "STATVFS";
  int datalen = strlen(databuf);
  const char *filename = __func__;

  /*
   * Get current filesystem statistics
   */
  status = statvfs("/", &statbuf1);
  rtems_test_assert(status == 0);
  statvfs_validate(&statbuf1);

  /*
   * Create one file
   */
  fd = open(filename, O_CREAT | O_WRONLY, 0775);
  rtems_test_assert(fd >= 0);
  n = write(fd, databuf, datalen);
  rtems_test_assert(n == datalen);
  status = close(fd);
  rtems_test_assert(status == 0);

  /*
   * Get new filesystem statistics
   */
  status = statvfs("/", &statbuf2);
  rtems_test_assert(status == 0);
  statvfs_validate(&statbuf2);

  /*
   * Compare old and new statistics
   */
  rtems_test_assert(statbuf1.f_bsize == statbuf2.f_bsize);
  rtems_test_assert(statbuf1.f_frsize == statbuf2.f_frsize);
  rtems_test_assert(statbuf1.f_blocks == statbuf2.f_blocks);
  rtems_test_assert(statbuf1.f_bfree >= statbuf2.f_bfree);
  rtems_test_assert(statbuf1.f_bavail >= statbuf2.f_bavail);
  rtems_test_assert(statbuf1.f_ffree >= statbuf2.f_ffree);
  rtems_test_assert(statbuf1.f_favail >= statbuf2.f_favail);
  rtems_test_assert(statbuf1.f_namemax == statbuf2.f_namemax);
}

/*
 * These tests only get time_t value, and test
 * if they are changed. Thest tests don't check atime
 */
void test(void)
{
  statvfs_test01();
}
