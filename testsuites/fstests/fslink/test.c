
/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id Exp $
 */

#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <stdint.h>
#include <math.h>

#include "fstest.h"

mode_t        mode = 0644;
/*
 * Test if the successful call works as expect
 */
void link_test01(void )
{
  char *name0="t0";
  char *name1="t1";
  char *name2="t2";
  char *name3="t3";

  int status;
  int fd;

  struct stat statbuf;
  mode_t tmp_mode; 

  time_t ctime1,dctime1,dmtime1;
  time_t ctime2,dctime2,dmtime2;

  puts("link creates hardlinks");

  status=mkdir(name3,0755);
  rtems_test_assert(status==0);

  status=chdir(name3);
  rtems_test_assert(status==0);
  fd=creat(name0,mode);
  status=close(fd);
  rtems_test_assert(status==0);

  status=stat(name0,&statbuf);
  rtems_test_assert(status==0);
  rtems_test_assert(S_ISREG(statbuf.st_mode));
  tmp_mode=(statbuf.st_mode)&ALLPERMS;
  rtems_test_assert(tmp_mode==0644);
  rtems_test_assert(statbuf.st_nlink==1);

  printf("the arg is %04o and the is %04o \n",tmp_mode,mode);
  puts("test if the stat is the same");
  status=link(name0,name1);
  rtems_test_assert(status==0);

  status=stat(name0,&statbuf);
  rtems_test_assert(status==0);

  rtems_test_assert(S_ISREG(statbuf.st_mode));
  tmp_mode=(statbuf.st_mode)&ALLPERMS;
  rtems_test_assert(tmp_mode==0644);
  rtems_test_assert(statbuf.st_nlink==2);

  status=stat(name1,&statbuf);
  rtems_test_assert(status==0);

  rtems_test_assert(S_ISREG(statbuf.st_mode));
  tmp_mode=(statbuf.st_mode)&ALLPERMS;
  rtems_test_assert(tmp_mode==0644);
  rtems_test_assert(statbuf.st_nlink==2);

  status=link(name1,name2);

  status=stat(name0,&statbuf);
  rtems_test_assert(status==0);

  rtems_test_assert(S_ISREG(statbuf.st_mode));
  tmp_mode=(statbuf.st_mode)&ALLPERMS;
  rtems_test_assert(tmp_mode==0644);
  rtems_test_assert(statbuf.st_nlink==3);

  status=stat(name1,&statbuf);
  rtems_test_assert(status==0);

  rtems_test_assert(S_ISREG(statbuf.st_mode));
  tmp_mode=(statbuf.st_mode)&ALLPERMS;
  rtems_test_assert(tmp_mode==0644);
  rtems_test_assert(statbuf.st_nlink==3);


  status=stat(name2,&statbuf);
  rtems_test_assert(status==0);

  rtems_test_assert(S_ISREG(statbuf.st_mode));
  rtems_test_assert(statbuf.st_nlink==3);
  tmp_mode=(statbuf.st_mode)&ALLPERMS;
  rtems_test_assert(tmp_mode==0644);
/*
 *  call chmod and chown and test.
 */

  puts("chmod and chown");

  chmod(name1,0201);
  chown(name1,65534,65533);


  status=stat(name0,&statbuf);
  rtems_test_assert(status==0);

  rtems_test_assert(S_ISREG(statbuf.st_mode));
  rtems_test_assert(statbuf.st_nlink==3);
  tmp_mode=(statbuf.st_mode)&ALLPERMS;
  rtems_test_assert(tmp_mode==0201);
  rtems_test_assert(statbuf.st_uid=65534);
  rtems_test_assert(statbuf.st_gid=65533);

  status=stat(name1,&statbuf);
  rtems_test_assert(status==0);

  rtems_test_assert(S_ISREG(statbuf.st_mode));
  tmp_mode=(statbuf.st_mode)&ALLPERMS;
  rtems_test_assert(tmp_mode==0201);
  rtems_test_assert(statbuf.st_nlink==3);
  rtems_test_assert(statbuf.st_uid=65534);
  rtems_test_assert(statbuf.st_gid=65533);

  status=stat(name2,&statbuf);
  rtems_test_assert(status==0);

  rtems_test_assert(S_ISREG(statbuf.st_mode));
  tmp_mode=(statbuf.st_mode)&ALLPERMS;
  rtems_test_assert(tmp_mode==0201);
  rtems_test_assert(statbuf.st_nlink==3);
  rtems_test_assert(statbuf.st_nlink==3);
  rtems_test_assert(statbuf.st_uid=65534);
  rtems_test_assert(statbuf.st_gid=65533);
/*
 *
 *  unlink then test if the nlink changes
 */

  puts("unlink then stat the file ");

  status=unlink(name0);
  rtems_test_assert(status==0);

  status=stat(name0,&statbuf);
  rtems_test_assert(status==-1);
  rtems_test_assert(errno=ENOENT);


  status=stat(name1,&statbuf);
  rtems_test_assert(status==0);

  rtems_test_assert(S_ISREG(statbuf.st_mode));
  tmp_mode=(statbuf.st_mode)&ALLPERMS;
  rtems_test_assert(tmp_mode==0201);
  rtems_test_assert(statbuf.st_nlink==2);


  status=stat(name2,&statbuf);
  rtems_test_assert(status==0);

  rtems_test_assert(S_ISREG(statbuf.st_mode));
  tmp_mode=(statbuf.st_mode)&ALLPERMS;
  rtems_test_assert(tmp_mode==0201);
  rtems_test_assert(statbuf.st_nlink==2);


  status=unlink(name1);
  rtems_test_assert(status==0);

  status=unlink(name2);
  rtems_test_assert(status==0);


  /*
   * successful link() updates ctime 
   * and the ctime and mtime of the 
   * directory.
   */
  fd=creat(name0,0644);
  status=close(fd);
  rtems_test_assert(status==0);

  status=stat(name0,&statbuf);
  ctime1=statbuf.st_ctime;
  status=stat(".",&statbuf);
  rtems_test_assert(status==0);
  dctime1=statbuf.st_ctime;
  dmtime1=statbuf.st_mtime;

  puts("sleep a few seconds");
  sleep(TIME_PRECISION );

  status=link(name0,name1);
  rtems_test_assert(status==0);
  status=stat(name0,&statbuf);
  ctime2=statbuf.st_ctime;
  status=stat(".",&statbuf);
  dctime2=statbuf.st_ctime;
  dmtime2=statbuf.st_mtime;

  puts("test if the time changes");
  rtems_test_assert(!time_equal(ctime1,ctime2));
#if !defined(IMFS_TEST) && !defined(MIMFS_TEST)
  rtems_test_assert(!time_equal(dctime1,dctime2));
#endif 
#if !defined(IMFS_TEST) && !defined(MIMFS_TEST)
  rtems_test_assert(!time_equal(dmtime1,dmtime2));
#endif 

}
void test(void)
{
  puts( "\n\n*** LINK TEST ***" );
#if defined(MDOSFS_TEST)
#else
  link_test01();
#endif 
  puts( "*** END OF LINK TEST ***" );
}

