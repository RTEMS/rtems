/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
#include "pmacros.h"

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

  puts("symlink loop erro test");

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

  puts( "\n\n*** SYMLINK TEST ***" );
  symlink_test01();
  symlink_loop_error_test();
  puts( "*** END OF SYMLINK TEST ***" );

}

