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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#include "fstest.h"
#include "fs_config.h"
#include <tmacros.h>

const char rtems_test_name[] = "FSSYMLINK " FILESYSTEM;

/*
 * Test the function of symlink
 */

static void symlink_test01(void )
{
  int   fd;
  char* file01="file";
  char* symlink_file01="symlink";
  char name[20];
  int   status;
  struct stat statbuf;
  size_t   len=strlen(file01);
  size_t   name_len;


  printf("Create a file named %s\n",file01);
  fd=creat(file01,0777);
  status=close(fd);
  rtems_test_assert(status==0);

  printf("Create a symlink named %s to %s\n",symlink_file01,file01);
  status=symlink(file01,symlink_file01);
  rtems_test_assert(status==0);

  status=stat(file01,&statbuf);
  rtems_test_assert(status==0);
  rtems_test_assert(S_ISREG(statbuf.st_mode));
  rtems_test_assert(0==statbuf.st_size);

  status=lstat(symlink_file01,&statbuf);
  rtems_test_assert(status==0);
  rtems_test_assert(S_ISLNK(statbuf.st_mode));
  rtems_test_assert(len==statbuf.st_size);


  puts("call readlink ");
  name_len=readlink(symlink_file01,name,sizeof(name)-1);
  rtems_test_assert(name_len!=-1);
  name[name_len]='\0';
  rtems_test_assert(!strncmp(name,file01,name_len));
  puts(name);

  puts("Unlink the file");

  status=unlink(file01);
  rtems_test_assert(status==0);

  status=lstat(symlink_file01,&statbuf);
  rtems_test_assert(status==0);
  rtems_test_assert(S_ISLNK(statbuf.st_mode));
  rtems_test_assert(len==statbuf.st_size);

  puts("call readlink ");
  name_len=readlink(symlink_file01,name,sizeof(name)-1);
  rtems_test_assert(name_len!=-1);
  name[name_len]='\0';
  rtems_test_assert(!strncmp(name,file01,name_len));
  status=unlink(symlink_file01);
  rtems_test_assert(status==0);

  printf("Create a dir named %s\n",file01);
  status=mkdir (file01,0777);

  printf("Create a symlink named %s to %s\n",symlink_file01,file01);
  status=symlink(file01,symlink_file01);
  rtems_test_assert(status==0);

  status=lstat(symlink_file01,&statbuf);
  rtems_test_assert(status==0);
  rtems_test_assert(S_ISLNK(statbuf.st_mode));
  rtems_test_assert(len==statbuf.st_size);


  puts("call readlink ");
  name_len=readlink(symlink_file01,name,sizeof(name)-1);
  rtems_test_assert(name_len!=-1);
  name[name_len]='\0';
  rtems_test_assert(!strncmp(name,file01,name_len));

  name_len=readlink(symlink_file01,name,3);
  rtems_test_assert(name_len!=-1);
  name[name_len]='\0';
  rtems_test_assert(!strncmp(name,file01,name_len));

  puts("rmdir the dir");
  status=rmdir(file01);
  rtems_test_assert(status==0);

  status=lstat(symlink_file01,&statbuf);
  rtems_test_assert(status==0);
  rtems_test_assert(S_ISLNK(statbuf.st_mode));

  status=unlink(symlink_file01);
  rtems_test_assert(status==0);

}
/*
 *  symlink loop error test
 */
static void symlink_loop_error_test(void )
{
  char* file01="file01";
  char* file02="file02";

  char* file04="file04";
  char* path="file01/t";

  int   status;

  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;

  puts("symlink loop error test");

  status=symlink(file01,file02);
  rtems_test_assert(status==0);
  status=symlink(file02,file01);
  rtems_test_assert(status==0);


  EXPECT_ERROR(ELOOP,creat,path,mode);
  EXPECT_ERROR(ELOOP,open,path,O_CREAT|O_WRONLY,mode);
  EXPECT_ERROR(ELOOP,truncate,path,0);
  EXPECT_ERROR(ELOOP,rename,path,file04);
  EXPECT_ERROR(ELOOP,unlink,path);
  EXPECT_ERROR(ELOOP,mkdir,path,mode);
  EXPECT_ERROR(ELOOP,rmdir,path);
}

void test(void )
{

  symlink_test01();
  symlink_loop_error_test();

}

