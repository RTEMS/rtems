/*
 *  This is a native test to explore how the readdir() family works.
 *  Newlib supports the following readdir() family members:
 *
 *    closedir()   - 
 *    readdir()    - 
 *    scandir()    - 
 *    opendir()    - 
 *    rewinddir()  - 
 *    telldir()    - BSD not in POSIX 
 *    seekdir()    - BSD not in POSIX
 *
 *
 *  seekdir() takes an offset which is a byte offset.  The Linux
 *  implementation of this appears to seek to the ((off/DIRENT_SIZE) + 1)
 *  record where DIRENT_SIZE seems to be 12 bytes.
 *
 *
 *
 *  $Id$
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <rtems.h>
#include <rtems/libio.h>

extern rtems_filesystem_location_info_t rtems_filesystem_current;

DIR *directory;
DIR *directory2;
DIR *directory3;
DIR *directory_not;

#ifndef __P
#define __P(args)()
#endif

char *dnames[] = {
	"a",
	"b",
	"c",
	"d",
	"e",
	"f",
	"c/y",
	"c/z",
	"c/x",
	"c/y/a3333",
	"c/y/j123",
        "c/y/my_mount_point", 
        "c/y/my_mount_point/my_dir",
        "c/z/my_mount_point",
        "END"
};

char *fnames[] = {
	"a",
	"b",
	"c",
	"d",
	"e",
	"f",
	"c/y",
	"c/z",
	"c/x",
	"c/y/a3333",
	"c/y/j123",
        "c/y/my_mount_point",
        "c/y/my_mount_point/my_dir",
        "c/y/my_mount_point/my_dir/d",
        "c/z/my_mount_point",
	"/c/z/my_mount_point/a/../../my_mount_point/a/g",
        "END"
};

#if defined(__rtems__)
int test_main(void)
#else
int main(
  int argc,
  char **argv
)
#endif
{
  int i;
  int fd;
  int status;
  struct stat statbuf;
  rtems_filesystem_mount_table_entry_t *mt_entry;
  static char mount_point_string[25] = { "/c/z/my_mount_point" };


  printf( "\n\n*** MOUNT/UNMOUNT TEST ***\n" );

  printf( "\nchdir to the root directory\n" );
  status = chdir( "/" );
  printf( "chdir() status : %d\n\n", status );

  printf( "\nCreating a series of directories under /\n" );
  i=0;
  while ( strcmp(dnames[i], "END") != 0 )
  {
     status = mkdir( dnames[i], 0777 );
     printf("Creating directory: %s      %d %d   ", dnames[i], status, errno );
     if ( status == 0 )
        printf(" Success\n");
     else
        printf(" Failure\n");

     i++;
  }

  fd = open ("/b/my_file", O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO);
  close (fd);
  fd = open("/b/my_file", S_IRWXU|S_IRWXG|S_IRWXO);
  close( fd );

  fd = open ("c/y/my_mount_point/my_dir/d", O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO);
  close (fd);
  fd = open("c/y/my_mount_point/my_dir/d", S_IRWXU|S_IRWXG|S_IRWXO);
  close( fd );

  printf("Attempting to mount IMFS file system at /c/z/my_mount_point \n");
  status = mount(
     &mt_entry,
     &IMFS_ops,
     "RW",
     NULL,
     mount_point_string );
  assert( status == 0 );
  if( mt_entry == NULL ){
     printf(" NULL mount table entry was returned\n");
  }
  else {
     printf("2nd file system successfully mounted at /c/z/my_mount_point \n");
  }

  printf( "\nchdir to /c/z/my_mount_point the mount point of the \n" );
  printf( "second file system \n" );
  status = chdir( "/c/z/my_mount_point" );
  printf( "chdir() status : %d\n\n", status );

  printf( "\nCreating a series of directories under /c/z/my_mount_point\n" );
  i=0;
  while ( strcmp(fnames[i], "END") != 0 )
  {
     status = mkdir( fnames[i], 0777 );
     printf("Creating directory: %s      %d %d   ", fnames[i], status, errno );
     if ( status == 0 )
        printf(" Success\n");
     else {
        printf(" Failure\n");
        perror("errno");
     }

     status = stat( fnames[i], &statbuf );
     if ( status == -1 )
       printf( ": %s\n", strerror( errno ) );

     i++;
  }

  printf( "\nchdir to / the mount point of the first file system \n" );
  status = chdir( "/" );
  printf( "chdir() status : %d\n\n", status );

  /*
   * Unmount the first file system we mounted
   */

  printf( "Unmount status:");
  status = unmount( "/c/z/my_mount_point" );
  printf( " %d\n", status );

/*
  status = chmod( "c/y/j123", S_IRUSR );
  assert( status == 0 );

  printf("Attempting to mount IMFS file system at c/y/j123\n");
  status = mount(
     &mt_entry,
     &IMFS_ops,
     "RO",
     NULL,
     "c/y/j123" );
  assert( status == 0 );

  status = mkdir( "c/y/j123/new_dir", S_IRUSR );
  assert( status == -1 );

  printf("Unmount c/y/j123\n");
  status = unmount( "c/y/j123" );
  assert( status == 0 );
*/

  printf(" File system type should be invalid.\n");
  status = mount(
   &mt_entry,
   NULL,
   "RW",
   NULL,
   mount_point_string );
  assert( status == -1 );
  assert( errno == EINVAL );

  printf("Attempting to mount IMFS file system at /c/y/my_mount_point \n");
  status = mount(
     &mt_entry,
     &IMFS_ops,
     "RO",
     NULL,
     "/c/y/my_mount_point" );
  assert( status == 0 );
  if( mt_entry == NULL ){
     printf(" NULL mount table entry was returned\n");
  }
  else {
     printf("3rd file system successfully mounted at /c/y/my_mount_point \n");
  }

  status = mkdir("c/y/my_mount_point/../../y/my_mount_point/new_dir",S_IRWXU );
  assert( status == 0 );

  status = stat("c/y/my_mount_point/../../y/my_mount_point/new_dir",&statbuf );
  assert( status == 0 );
  status = stat("c/y/my_mount_point/new_dir/..", &statbuf );
  assert( status == 0 );

  printf("Mount another file system at /c/y/my_mount_point should fail with EBUSY\n");
  status = mount(
     &mt_entry,
     &IMFS_ops,
     "RO",
     NULL,
     "/c/y/my_mount_point" );
  assert( status == -1 );
  assert( errno == EBUSY);

  printf("Mount /b/my_file should fail in rtems_filesystem_evaluate_path\n");
  status = mount(
     &mt_entry,
     &IMFS_ops,
     "RO",
     NULL,
     "/b/my_file" );
  assert( status == -1 );
 
  printf("Unmount /c/y/my_mount_point\n");
  status = unmount( "/c/y/my_mount_point" );
  assert( status == 0 );

/* What's wrong with this? It should be causing failure at unmount.c:87,
 * instead, it's returning a status of 0.
 */

  printf("Mount /c/y/my_mount_point to cause error\n");
  status = mount(
     &mt_entry,
     &IMFS_ops,
     "RO",
     NULL,
     "/c/y/my_mount_point" );

  assert( status == 0 );
  status = mkdir( "/c/y/my_mount_point/mydir", 0777);
  assert( status == 0 );

  status = chdir( "/c/y/my_mount_point/mydir" );
  assert( status == 0 );

  printf(" unmount of /c/y/my_mount_point should fail with EBUSY\n");
  status = unmount( "/c/y/my_mount_point" );
  assert( status == -1 );
  assert( errno == EBUSY );

  status = chdir( "/" );
  assert( status == 0 );

  printf(" unmount /c/y/my_mount_point \n");
  status = unmount( "/c/y/my_mount_point" );
  assert( status == 0 );

  printf(" unmount /b/mount_point should fail\n");
  status = unmount( "/b/mount_point" );
  assert( status == -1 );

  printf("Mount /c/y/my_mount_point\n");
  status = mount(
     &mt_entry,
     &IMFS_ops,
     "RO",
     NULL,
     "/c/y/my_mount_point" );
  assert( status == 0 );

  /* XXX - There is an error in open that calculates incorrect mode. */
  printf("Create and open /c/y/my_mount_point/my_file\n");
  fd = open( "/c/y/my_mount_point/my_file", O_CREAT ); 
  assert( fd != -1 );
  status = close( fd );
  assert( status == 0 );

  printf("\nmkdir /c/y/my_mount_point/my_dir\n");
  status = mkdir( "/c/y/my_mount_point/my_dir", 0x1c0 );
  printf("Open /c/y/my_mount_point/my_dir\n");
  directory = opendir( "/c/y/my_mount_point/my_dir" );
  assert( directory ); 
  
  printf("Unmount /c/y/my_mount_point should fail with EBUSY\n");
  status = unmount( "/c/y/my_mount_point" );
  assert( status == -1 );

  printf("Close /c/y/my_mount_point/my_dir\n");
  status = closedir( directory ); 
  assert( status == 0 );

  printf("Unmount /c/y/my_mount_point/d should fail at 107\n");
  status = unmount( "/c/y/my_mount_point/d" );
  assert( status == -1 );

  printf("unmount /c/y/my_mount_point\n");
  status = unmount( "/c/y/my_mount_point" );
  assert( status == 0 );

  printf("mount with option RA should fail with EINVAL\n");
  status = mount(
     &mt_entry,
     &IMFS_ops,
     "RA",
     NULL,
     "/c/y/my_mount_point" );
  assert( status == -1 );

  printf("Mount a file system at /c/y/my_mount_point/my_dir\n");
  status = mount(
     &mt_entry,
     &IMFS_ops,
     "RW",
     NULL,
     "/c/y/my_mount_point/my_dir");
  assert( status == 0 );

  printf("unmount /c/y/my_mount_point/my_dir should fail in ");
  printf("file_systems_below_this_mountpoint \n");
  status = unmount( "/c/y/my_mount_point/my_dir" );
  assert( status == 0 );

  printf("mount first filesystem /c/y/my_mount_point/\n");
  status = mount(
     &mt_entry,
     &IMFS_ops,
     "RW",
     NULL,
     "/c/y/my_mount_point" );
  assert( status == 0 );

  printf("\nmkdir /c/y/my_mount_point/my_dir\n");
  status = mkdir( "/c/y/my_mount_point/my_dir", S_IRWXU );
  assert( status == 0 );

  printf("Mount another filesystem at /c/y/my_mount_point/my_dir\n");
  status = mount(
     &mt_entry,
     &IMFS_ops,
     "RW",
     NULL,
     "/c/y/my_mount_point/my_dir");
  assert( status == 0 );

  status = mkdir( "/c/y/my_mount_point/my_dir2", S_IRWXU  );
  assert( status != -1 ); 

  status = link( "/c/y/my_mount_point/my_dir2", "/c/y/my_mount_point/my_dir/my_link" );
  assert( status == -1 );

  printf("unmount /c/y/my_mount_point\n");
  status = unmount( "/c/y/my_mount_point" );
  assert( status == -1 );

  printf("unmount /c/y/my_mount_point/my_dir\n");
  status = unmount( "/c/y/my_mount_point/my_dir" );
  assert( status == 0 );

  printf("unmount /c/y/my_mount_point\n");
  status = unmount( "/c/y/my_mount_point" );
  assert( status == 0 );

/*  printf("Mount /c/y/my_mount_point\n");
  status = mount(
     &mt_entry,
     &IMFS_ops,
     "RO",
     NULL,
     "/c/y/my_mount_point" );
  assert( status == 0 );
*/

  printf( "\n\n*** END OF MOUNT/UNMOUNT TEST ***\n" );
  exit(0);
}

