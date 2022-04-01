/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 2015.
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
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>

#include <tmacros.h>

#include "fstest.h"
#include "fs_config.h"

const char rtems_test_name[] = "FSSCANDIR " FILESYSTEM;

#define FILE_NAME "aaa"

#define DIR_NAME "bbb"

void test(void)
{
  struct dirent **namelist;
  struct dirent *d;
  int rv;
  int n;
  int i;

  rtems_test_assert(MAXNAMLEN == NAME_MAX);

  rv = mknod(FILE_NAME, S_IFREG | S_IRWXU | S_IRWXG | S_IRWXO, 0);
  rtems_test_assert(rv == 0);

  rv = mkdir(DIR_NAME, S_IRWXU | S_IRWXG | S_IRWXO );
  rtems_test_assert(rv == 0);

  n = scandir(".", &namelist, NULL, alphasort);
  rtems_test_assert(2 <= n || n == 4);

  i = 0;
  d = namelist[i];

  if (n >= 3) {
    rtems_test_assert(strcmp(d->d_name, ".") == 0);
#ifdef DT_UNKNOWN
    rtems_test_assert(d->d_type == DT_DIR || d->d_type == DT_UNKNOWN);
#endif
    free(d);
    ++i;
    d = namelist[i];
  }

  if (n == 4) {
    rtems_test_assert(strcmp(d->d_name, "..") == 0);
#ifdef DT_UNKNOWN
    rtems_test_assert(d->d_type == DT_DIR || d->d_type == DT_UNKNOWN);
#endif
    free(d);
    ++i;
    d = namelist[i];
  }

  rtems_test_assert(strcmp(d->d_name, FILE_NAME) == 0);
#ifdef DT_UNKNOWN
  rtems_test_assert(d->d_type == DT_REG || d->d_type == DT_UNKNOWN);
#endif
  free(d);
  ++i;
  d = namelist[i];

  rtems_test_assert(strcmp(d->d_name, DIR_NAME) == 0);
#ifdef DT_UNKNOWN
  rtems_test_assert(d->d_type == DT_DIR || d->d_type == DT_UNKNOWN);
#endif
  free(d);

  free(namelist);
}
