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
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "fstest.h"
#include "fs_config.h"
#include <tmacros.h>

#define BUF_SIZE 100

const char rtems_test_name[] = "FSPATHEVAL " FILESYSTEM;

static void make_multiple_files (char **files,int is_directory)
{
  int i;
  int status;
  int fd;

  i = 0;
  if (is_directory){
    while (files[i]) {
      printf ("Making directory %s\n", files[i]);
      status = mkdir (files[i], S_IRWXU);
      rtems_test_assert (!status);
      i++;
    }
  }else {
    while (files[i]) {
      printf ("Create file %s\n", files[i]);
      fd=creat(files[i],S_IRWXU);
      status=close(fd);
      rtems_test_assert (!status);
      i++;
    }
  }

  puts ("");
}

static void remove_multiple_files (char **files,int is_directory)
{
  int i;
  int status;

  i = 0;
  while (files[i]) {
    i++;
  }

  if (is_directory){
    while (i) {
      i--;
      printf ("Removing directory %s\n", files[i]);
      status = rmdir (files[i]);
      rtems_test_assert (!status);
    }
  }else {
    while (i) {
      i--;
      printf ("Removing file %s\n", files[i]);
      status = unlink (files[i]);
      rtems_test_assert (!status);
    }
  }

  puts ("");
}

static void path_eval_test01 (void)
{
  char *valid_path[] = {
    "/test1/",
    "tets2",
    "///test3",
    "test4////",
    "../../test5",
    "/test1/../test6",
    "./test7/",
    ".././test8",
    "test8/./../test9",
    "///test9/../test10",
    0
  };
  char *valid_file[]={
    "/test1",
    "tets2",
    "///test3",
    "test4",
    "../../test5",
    "/../test6",
    "./test7",
    ".././test8",
    "/./../test9",
    "//../test10",
    0
  };

  char *valid_relative_path[]={
    "test1",
    "tets2",
    "test3",
    "test4",
    "test5",
    "test6",
    "test7",
    "test8",
    "test9",
    "test10",
    0

  };

  char *valid_name[] = {
    "!#$%&()-@^_`{}~'",
    "0_1_A",
    "aaa bbb",
    "ccc....ddd",
    " fff",
    0
  };


  make_multiple_files(valid_path,1);
  make_multiple_files (valid_name,1);

  remove_multiple_files(valid_relative_path,1);
  remove_multiple_files(valid_name,1);

  make_multiple_files(valid_file,0);
  make_multiple_files (valid_name,0);

  remove_multiple_files(valid_relative_path,0);
  remove_multiple_files(valid_name,0);

}
static void path_eval_test02(void )
{

  int status;
  char buf[BUF_SIZE];
  char* cwd;

  mode_t mode = S_IRWXU|S_IRWXG|S_IRWXO;
  puts("mkdir /tmp/a/b");
  status=mkdir("/tmp",mode);
  rtems_test_assert(status==0);
  status=mkdir("/tmp/a",mode);
  rtems_test_assert(status==0);
  status=mkdir("/tmp/a/b",mode);
  rtems_test_assert(status==0);

  cwd=getcwd(buf,BUF_SIZE);
  rtems_test_assert(cwd!=NULL);

  puts("cd /tmp");
  status=chdir("/tmp");
  rtems_test_assert(status==0);

  status=chdir("a/b");
  rtems_test_assert(status==0);

  status=chdir("../b");
  rtems_test_assert(status==0);

  status=chdir("../b/.");
  rtems_test_assert(status==0);

}

void test (void )
{
  path_eval_test01();
  path_eval_test02();
}
