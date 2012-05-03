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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dirent.h>
#include <unistd.h>

#include "fstest.h"
#include "pmacros.h"

/*
 *  Test the umask
 */
static void umask_test01(void )
{

  mode_t previous_cmask;
  mode_t tmp_mode;
  mode_t file_mode;
  struct stat statbuf;
  int status = 0;
  int fd;

  char* file01="file01";
  char* file02="file02";
  char* directory01="dir01";

  const char* wd=__func__;

  mode_t mode=S_IRWXU|S_IRWXG|S_IRWXO ;



  /*
   *Create a new directory and change the current directory to  this
   */
  status=mkdir(wd,mode);
  rtems_test_assert(status==0);
  status=chdir(wd);
  rtems_test_assert(status==0);

/*
 *
 *  Call open creat and mkdir to create new files and directory
 */
  fd=open(file01,O_CREAT|O_RDWR,mode);
  status=close(fd);
  rtems_test_assert(status==0);

  fd=creat(file02,mode);
  status=close(fd);
  rtems_test_assert(status==0);

  status=mkdir(directory01,mode);
  rtems_test_assert(status==0);


  /*
   *Get the previous cmask and set it to a new one
   */
  previous_cmask = umask (0321);
  printf("The previous cmask is %03o\n",(unsigned int)previous_cmask);
  file_mode= mode & ~previous_cmask;

  status = stat (file01, &statbuf);
  rtems_test_assert (status == 0);
  tmp_mode = (statbuf.st_mode) & ALLPERMS;
  printf("The file mode of %s is %03o\n",file01,(unsigned int)tmp_mode);
  rtems_test_assert(tmp_mode==file_mode);


  status = stat (file02, &statbuf);
  rtems_test_assert (status == 0);
  tmp_mode = (statbuf.st_mode) & ALLPERMS;
  printf("The file mode of %s is %03o\n",file02,(unsigned int)tmp_mode);
  rtems_test_assert(tmp_mode==file_mode);

  status = stat (directory01, &statbuf);
  rtems_test_assert (status == 0);
  tmp_mode = (statbuf.st_mode) & ALLPERMS;
  printf("The file mode of %s is %03o\n",directory01,(unsigned int)tmp_mode);
  rtems_test_assert(tmp_mode==file_mode);

  /*
   *Remove them and recreate them with the same mode
   */

  status=unlink(file01);
  rtems_test_assert(status==0);
  fd=open(file01,O_CREAT|O_RDWR,mode);
  status=close(fd);
  rtems_test_assert(status==0);

  status=unlink(file02);
  rtems_test_assert(status==0);
  fd=creat(file02,mode);
  status=close(fd);
  rtems_test_assert(status==0);

  status=rmdir(directory01);
  rtems_test_assert(status==0);
  status=mkdir(directory01,mode);
  rtems_test_assert(status==0);

  /*
   *Check the file mode
   */

  previous_cmask = umask (00);
  printf("The previous cmask is %03o\n",(unsigned int)previous_cmask);
  file_mode= mode & ~previous_cmask;

  status = stat (file01, &statbuf);
  rtems_test_assert (status == 0);
  tmp_mode = (statbuf.st_mode) & ALLPERMS;
  printf("The file mode of %s is %03o\n",file01,(unsigned int)tmp_mode);
  rtems_test_assert(tmp_mode==file_mode);


  status = stat (file02, &statbuf);
  rtems_test_assert (status == 0);
  tmp_mode = (statbuf.st_mode) & ALLPERMS;
  printf("The file mode of %s is %03o\n",file02,(unsigned int)tmp_mode);
  rtems_test_assert(tmp_mode==file_mode);

  status = stat (directory01, &statbuf);
  rtems_test_assert (status == 0);
  tmp_mode = (statbuf.st_mode) & ALLPERMS;
  printf("The file mode of %s is %03o\n",directory01,(unsigned int)tmp_mode);
  rtems_test_assert(tmp_mode==file_mode);

  /*
   * Go back to parent directory
   */
  status=chdir("..");
  rtems_test_assert(status==0);

}
/*
 * Check the file mode in file and directory
 */
static void test_premission01(void )
{
  mode_t tmp_mode;
  struct stat statbuf;
  int status = 0;
  int fd;

  char* file01="file01";
  char* file02="file02";
  char* directory01="dir01";

  char path[20];
  char* test_data="Test Data";
  char* data_buf;
  size_t len=strlen(test_data);

  int n;
  DIR *dp;

  const char* wd=__func__;

  mode_t mode=S_IRWXU|S_IRWXG|S_IRWXO ;
  uid_t user_id =65534;
  gid_t group_id =65534;

  uid_t another_user_id =65533;
  gid_t another_group_id =65533;


  /*
   *Create a new directory and change the current directory to this
   */
  umask(00);
  status=mkdir(wd,mode);
  rtems_test_assert(status==0);
  status=chdir(wd);
  rtems_test_assert(status==0);

  status=seteuid(user_id);
  rtems_test_assert(status==0);
  status=setegid(group_id);
  rtems_test_assert(status==0);


  /*
   *Create a file with mode 0777
   */
  fd=creat(file01,mode);
  status=close(fd);
  rtems_test_assert(status==0);
  /*
   *Create a file with mode 0240
   */

  fd=creat(file02,0240);
  status=close(fd);
  rtems_test_assert(status==0);


  /*
   *Check the file mode uid and gid
   */
  status = stat (file01, &statbuf);
  rtems_test_assert (status == 0);
  tmp_mode = (statbuf.st_mode) & ALLPERMS;
  printf("The file mode of %s is %03o\n",file01,(unsigned int)tmp_mode);
  rtems_test_assert(tmp_mode==mode);
  rtems_test_assert(statbuf.st_uid==user_id);
  rtems_test_assert(statbuf.st_gid==group_id);

  status = stat (file02, &statbuf);
  rtems_test_assert (status == 0);
  tmp_mode = (statbuf.st_mode) & ALLPERMS;
  printf("The file mode of %s is %03o\n",file02,(unsigned int)tmp_mode);
  rtems_test_assert(tmp_mode==0240);
  rtems_test_assert(statbuf.st_uid==user_id);
  rtems_test_assert(statbuf.st_gid==group_id);

  /*
   * Create directory and a file in it for tese
   */

  status=mkdir(directory01,0777);
  rtems_test_assert(status==0);
  sprintf(path,"%s/%s",directory01,file01);
  fd=creat(path,0777);

  status=chmod(directory01,0340);
  rtems_test_assert (status == 0);
  status = stat (directory01, &statbuf);
  rtems_test_assert (status == 0);
  tmp_mode = (statbuf.st_mode) & ALLPERMS;
  printf("The file mode of %s is %03o\n",directory01,(unsigned int)tmp_mode);
  rtems_test_assert(tmp_mode==0340);
  rtems_test_assert(statbuf.st_uid==user_id);
  rtems_test_assert(statbuf.st_gid==group_id);

  /*
   * Check the file with open and write
   */

  /*
   * Test write
   */
  fd=open(file01,O_WRONLY);
  n=write(fd,test_data,len);
  rtems_test_assert(n==len);
  status=close(fd);
  rtems_test_assert(status==0);

  fd=open(file02,O_WRONLY);
  n=write(fd,test_data,len);
  rtems_test_assert(n==len);
  status=close(fd);
  rtems_test_assert(status==0);

  /*
   * Test read
   */
  data_buf=(char*)malloc(len+1);
  fd=open(file01,O_RDWR);
  rtems_test_assert(fd!=-1);
  n=read(fd,data_buf,len);
  rtems_test_assert(n==len);
  status=close(fd);
  rtems_test_assert(status==0);

  EXPECT_ERROR(EACCES,open,file02,O_RDONLY);
  EXPECT_ERROR(EACCES,open,file02,O_RDWR);

  /*
   * Test read directory
   */
  dp= opendir(directory01);
  rtems_test_assert(dp==NULL);
  rtems_test_assert(errno==EACCES);

  /*
   * Test write directory
   */
  status = lstat (path, &statbuf);
  rtems_test_assert (status == 0);

  status=unlink(path);
  rtems_test_assert(status==0);


  /*
   * Change euid and check
   */
  puts("Change euid and check");
  status=seteuid(0);
  rtems_test_assert(status==0);

  status=seteuid(another_user_id);
  rtems_test_assert(status==0);

  fd=open(file01,O_WRONLY);
  n=write(fd,test_data,len);
  rtems_test_assert(n==len);
  status=close(fd);
  rtems_test_assert(status==0);

  EXPECT_ERROR(EACCES,open,file02,O_WRONLY);
  EXPECT_ERROR(EACCES,open,file02,O_RDWR);

  /*
   * Test read directory
   */
  dp= opendir(directory01);
  rtems_test_assert(dp!=NULL);
  status=closedir(dp);
  rtems_test_assert(status==0);

  /*
   * Test write directory
   */
  EXPECT_ERROR(EACCES,creat,path,mode);
  EXPECT_ERROR(EACCES,rename,path,"test");
  EXPECT_ERROR(EACCES,truncate,path,0);
  EXPECT_ERROR(EACCES,link,path,"test");
  EXPECT_ERROR(EACCES,unlink,path);
  /*
   * Change egid and check
   */
  puts("Change egid and check");
  status=seteuid(0);
  rtems_test_assert(status==0);

  status=setegid(another_group_id);
  rtems_test_assert(status==0);

  status=seteuid(another_user_id);
  rtems_test_assert(status==0);

  EXPECT_ERROR(EACCES,open,file02,O_WRONLY);
  EXPECT_ERROR(EACCES,open,file02,O_RDONLY);
  EXPECT_ERROR(EACCES,open,file02,O_RDWR);

  /*
   * Test read directory
   */
  dp= opendir(directory01);
  rtems_test_assert(dp==NULL);
  rtems_test_assert(errno==EACCES);

   /*
   * Test write directory
   */
  EXPECT_ERROR(EACCES,creat,path,mode);

  /*
   * Go back to parent directory
   */
  status=seteuid(0);
  rtems_test_assert(status==0);
  status=setegid(0);
  rtems_test_assert(status==0);
  free(data_buf);

  status=chdir("..");
  rtems_test_assert(status==0);
}

/*
 * Test chown and chmod
 */
static void  test_premission02(void )
{
  struct stat statbuf;
  int status = 0;
  int fd;

  char* file01="file01";
  char* directory01="dir01";

  mode_t tmp_mode;
  mode_t file_mode=0321;

  const char* wd=__func__;

  mode_t mode=S_IRWXU|S_IRWXG|S_IRWXO ;
  uid_t user_id =65534;
  gid_t group_id =65534;

  /*
   *Create a new directory and change the current directory to  this
   */
  status=mkdir(wd,mode);
  rtems_test_assert(status==0);
  status=chdir(wd);
  rtems_test_assert(status==0);

  umask(00);

  fd=creat(file01,mode);
  status=close(fd);
  rtems_test_assert(status==0);
  status=stat(file01,&statbuf);
  tmp_mode = (statbuf.st_mode) & ALLPERMS;
  rtems_test_assert(tmp_mode==mode);

  /*
   *Change the file mode uid and gid
   */

  status=chmod(file01,file_mode);
  rtems_test_assert(status==0);
  status=chown(file01,user_id,group_id);
  rtems_test_assert(status==0);
  status=stat(file01,&statbuf);
  tmp_mode = (statbuf.st_mode) & ALLPERMS;
  rtems_test_assert(tmp_mode==file_mode);
  rtems_test_assert(user_id==statbuf.st_uid);
  rtems_test_assert(group_id==statbuf.st_gid);

  status=mkdir(directory01,mode);
  rtems_test_assert(status==0);
  status=stat(directory01,&statbuf);
  tmp_mode = (statbuf.st_mode) & ALLPERMS;
  printf("The directory file mode is %0o\n",(unsigned int)tmp_mode);
  rtems_test_assert(tmp_mode==mode);

  /*
   *Change the directory file mode
   */
  status=chmod(directory01,file_mode);
  rtems_test_assert(status==0);
  status=stat(directory01,&statbuf);
  tmp_mode = (statbuf.st_mode) & ALLPERMS;
  rtems_test_assert(tmp_mode==file_mode);
  printf("The directory file mode is %0o\n",(unsigned int)tmp_mode);

  /*
   * Go back to parent directory
   */
  status=chdir("..");
  rtems_test_assert(status==0);
}
static void root_test(void )
{
  int fd;
  int sc;

  fd =creat("test",0000);
  sc=close(fd);
  rtems_test_assert(sc==0);

  fd=open("test",O_RDONLY);
  rtems_test_assert(fd==-1);
}

void test(void )
{
  puts( "\n\n*** PERMISSION TEST ***" );
  umask_test01();
  test_premission01();
  test_premission02();
  root_test();
  puts( "*** END OF PERMISSION TEST ***" );
}
