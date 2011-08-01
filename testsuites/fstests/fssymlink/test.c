
#include <sys/stat.h>
#include <limits.h>
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

/*
 * Test the function of symlink 
 */
void symlink_test01()
{
  int   fd;
  char* name0="file";
  char* name1="symlink";
  int   status;
  struct stat statbuf;
  int   len=strlen(name0);

  printf("Create a file named %s\n",name0);
  fd=creat(name0,0777);
  status=close(fd);
  rtems_test_assert(status==0);

  printf("Create a symlink named %s to %s\n",name1,name0);
  status=symlink(name0,name1);
  rtems_test_assert(status==0);

  status=stat(name0,&statbuf);
  rtems_test_assert(status==0);
  rtems_test_assert(S_ISREG(statbuf.st_mode));
  rtems_test_assert(0==statbuf.st_size);
  

  status=lstat(name1,&statbuf);
  rtems_test_assert(status==0);
  rtems_test_assert(S_ISLNK(statbuf.st_mode));
#if !defined(IMFS_TEST) && !defined(MIMFS_TEST)
  rtems_test_assert(len==statbuf.st_size);
#endif 


  puts("Unlink the file");
  
  status=unlink(name0);
  rtems_test_assert(status==0);


  status=lstat(name1,&statbuf);
  rtems_test_assert(status==0);
  rtems_test_assert(S_ISLNK(statbuf.st_mode));

  status=unlink(name1);
  rtems_test_assert(status==0);

  printf("Create a dir named %s\n",name0);
  status=mkdir (name0,0777);
  
  printf("Create a symlink named %s to %s\n",name1,name0);
  status=symlink(name0,name1);
  rtems_test_assert(status==0);



  status=lstat(name1,&statbuf);
  rtems_test_assert(status==0);
  rtems_test_assert(S_ISLNK(statbuf.st_mode));
#if !defined(IMFS_TEST) && !defined(MIMFS_TEST)
  rtems_test_assert(len==statbuf.st_size);
#endif 

  puts("rmdir the dir");
  status=rmdir(name0);
  rtems_test_assert(status==0);
  

  status=lstat(name1,&statbuf);
  rtems_test_assert(status==0);
  rtems_test_assert(S_ISLNK(statbuf.st_mode));

  status=unlink(name1);
  rtems_test_assert(status==0);

}
/*
 *  symlink loop test
 */
void symlink_test02()
{

  char* name0="symlink0";
  char* name1="symlink1";
  int   status;


  puts("symlink loop test");
  status=symlink(name0,name1);
  rtems_test_assert(status==0);
  status=symlink(name1,name0);
  rtems_test_assert(status==0);

  
  puts("create a file Should fail with ELOOP");
  status=creat(name0,0777);
  rtems_test_assert(status!=0);
#if !defined(MRFS_TEST)
  rtems_test_assert(errno==ELOOP);
#endif 
  puts("truncate a file Should with ELOOP");
  status=truncate(name0,0777);
  rtems_test_assert(status!=0);
#if !defined(MRFS_TEST)
  rtems_test_assert(errno==ELOOP);
#endif 
}

void test()
{

  puts( "\n\n*** SYMLINK TEST ***" );
#if defined(MDOSFS_TEST) 
#else
  symlink_test01();
  symlink_test02();
#endif 

  puts( "*** END OF SYMLINK TEST ***" );

}

