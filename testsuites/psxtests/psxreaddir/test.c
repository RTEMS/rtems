/**
 *  @file
 *
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
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <rtems/imfs.h>

/* forward declarations to avoid warnings */
int test_main(void);
void printdir(DIR *directory);
void complete_printdir(char *path);
int select1(const struct dirent *entry);
int select2(const struct dirent *entry);
int compare_ascending(const struct dirent **a, const struct dirent **b);
int compare_descending( const struct dirent **a, const struct dirent **b);
void test_across_mount(void);
void test_across_mount(void);

DIR *directory;
DIR *directory2;
DIR *directory3;
DIR *directory_not;

/*
int scandir ( const char *dirname,
   struct dirent *** namelist,
   int (*select) __P((struct dirent *)),
   int (*dcomp) __P((const struct dirent **, const struct dirent **))
);
*/

#if defined(__rtems__)
#define d_type d_reclen
#include <pmacros.h>
#endif

void printdir( DIR *directory )
{
  struct dirent *d;

  printf( "%-20s %8s %8s %8s %4s\n",
     "name", "    inode", " offset", "reclen", " type" );
  d = readdir(directory);

  while (d) {
    printf( "%-20s %8ld %8" PRIdoff_t " %6d   0x%04x\n",
       d->d_name, d->d_ino, d->d_off, d->d_reclen, d->d_type );
    d = readdir(directory);

  }
}

void complete_printdir( char *path )
{
  DIR *the_dir;
  int status;

  the_dir = opendir( path );
  rtems_test_assert( the_dir );

  printdir( the_dir );
  status = closedir( the_dir );
  rtems_test_assert( !status );
}

char *many_files[] = {
	"a",
        "b",
        "c",
        "d",
        "e",
        "f",
        "g",
        "h",
        "i",
        "j",
        "k",
        "l",
        "m",
        "n",
        "o",
        "p",
        "q",
        "r",
        "s",
        "t",
        "u",
        "v",
        "w",
        "x",
        "y",
        "z",
        "aa",
        "ab",
        "ac",
        "ad",
        "ae",
        "af",
        "ag",
        "ah",
        "ai",
        "aj",
        "ak",
        "al",
        "am",
        "an",
        "ao",
        "ap",
        "aq",
        "ar"
};

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
        "END"
};

int select1 ( const struct dirent *entry )
{
   printf("SCANDIR SELECT1 accepts  nodename: %s\n", entry->d_name );
   return 1;
}

int select2 ( const struct dirent *entry )
{
   if( strcmp( entry->d_name, "y") == 0 ) {
      printf("SCANDIR SELECT accepted nodename: %s\n", entry->d_name );
      return 1;
   }
   printf("SCANDIR SELECT rejected nodename: %s\n", entry->d_name );
   return 0;
}

int compare_ascending(const struct dirent **a, const struct dirent **b )
{
   int i;

   i = strcmp (
      (char *)((struct dirent *)(*a)->d_name),
      (char *)((struct dirent *)(*b)->d_name)
   );
   return i;
}

int compare_descending( const struct dirent **a, const struct dirent **b )
{
   int i;

   i = strcmp (
      (char *)((const struct dirent *)(*b)->d_name),
      (char *)((const struct dirent *)(*a)->d_name)
   );

   return i;
}

void test_across_mount(void)
{
  int status;

  /*
   * Verify Readdir works correctly over mount points.
   */

  printf("Validate readdir across mount point\n");
  rtems_test_assert( mkdir( "/imfs", 0777 ) == 0 );
  rtems_test_assert( mkdir( "/imfs/should_be_hidden", 0777 ) == 0 );
  complete_printdir("/imfs" );
  printf("Attempting to mount IMFS file system at /imfs \n");
  status = mount(
    "null",
    "/imfs",
    "imfs",
     RTEMS_FILESYSTEM_READ_WRITE,
     NULL );
  rtems_test_assert( status == 0 );
  printf( "create /imfs/testdir and /imfs/testdir/testsubdir\n");

  status = mkdir( "/imfs/testdir", 0777 );
  rtems_test_assert( status == 0 );
  status = mkdir( "/imfs/testdir/testsubdir", 0777 );
  rtems_test_assert( status == 0 );

  complete_printdir("/imfs" );
  complete_printdir("/imfs/" );
  complete_printdir("/imfs/." );
  complete_printdir("/imfs/testdir" );
  complete_printdir("/imfs/testdir/.." );
}

#if defined(__rtems__)
int test_main(void)
#else
int main(
  int argc,
  char **argv
)
#endif
{
  static const char *my_file = "/b/my_file";

  int fd;
  int i;
  int status;
  off_t off;
  struct dirent *d_not;
  struct dirent **namelist;
  struct stat s;


  printf( "\n\n*** READDIR TEST ***\n" );

  printf( "\nchdir to the root directory\n" );
  status = chdir( "/" );
  printf( "chdir() status : %d\n\n", status );

  printf( "\nCreating a series of directories under /\n" );
  i=0;
  while ( strcmp(dnames[i], "END") != 0 )
  {
     status = mkdir( dnames[i], 0x1c0 );
     printf("Creating directory: %s      %d %d   ", dnames[i], status, errno );
     if ( errno == 0 )
        printf(" Success\n");
     else
        printf(" Failure\n");

     i++;
  }

  /*
   * Create files under many and open the directory.
   */

  printf("Create a lot of files\n");
  status = mkdir( "/many", 0x1c0 );
  status = chdir( "/many" );
  for (i = 0; i<44; i++) {
    printf("Create %s\n", many_files[i]);
    fd = open (many_files[i], O_CREAT, S_IRWXU);
    close (fd);
  }
  printf("Open /many and print the directory\n");
  directory_not = opendir( "/many" );
  printdir ( directory_not );
  d_not = readdir( directory_not );
  rtems_test_assert( d_not == 0 );

  printf("open %s\n", my_file);
  fd = open (my_file, O_CREAT, S_IRWXU);
  rtems_test_assert( fd != -1 );
  close (fd);

  printf("scandir a file status: ");
  status = scandir(
     my_file,
     &namelist,
     select1,
     NULL
  );
  printf("%d\n", status);

  printf("Open /b/new_file\n");
  fd  = open( "/b/new_file", O_CREAT, S_IRWXU );
  rtems_test_assert( fd != -1 );

  printf("fcntl F_SETFD should return 0\n");
  status = fcntl( fd, F_SETFD, 1 );
  rtems_test_assert( status == 0 );

  printf("fcntl F_SETFD should return 1\n");
  status = fcntl( fd, F_GETFD, 1 );
  rtems_test_assert( status == 1 );

  printf("fcntl F_DUPFD should return a file descriptor\n");
  status = fcntl( fd, F_DUPFD, 0 );
  rtems_test_assert ( status >= 0 );

  printf("close duplicate should return 0\n");
  status = close( status );
  rtems_test_assert ( status == 0 );

  printf("fcntl F_GETFL returns current flags\n");
  status = fcntl( fd, F_GETFL, 1 );
  printf("fcntl F_GETFL returned 0x%x\n", status );
  rtems_test_assert( status != -1 );

  printf("fcntl F_SETFL to add O_APPEND and O_NONBLOCK\n");
  status = fcntl( fd, F_SETFL, O_APPEND|O_NONBLOCK );
  rtems_test_assert ( status != -1 );

  printf("fcntl F_GETFL return current flags to see changes\n");
  status = fcntl( fd, F_GETFL, 1 );
  printf("fcntl F_GETFL returned 0x%x\n", status );
  rtems_test_assert( status != -1 );

  printf("fcntl F_GETLK should return -1\n");
  status = fcntl( fd, F_GETLK, 1 );
  rtems_test_assert ( status == -1 );

  printf("fcntl F_SETLK should return -1\n");
  status = fcntl( fd, F_SETLK, 1 );
  rtems_test_assert ( status == -1 );

  printf("fcntl F_SETLKW should return -1\n");
  status = fcntl( fd, F_SETLKW, 1 );
  rtems_test_assert ( status == -1 );

  printf("fcntl F_SETOWN should return -1\n");
  status = fcntl( fd, F_SETOWN, 1 );
  rtems_test_assert ( status == -1 );

  printf("fcntl F_GETOWN should return -1\n");
  status = fcntl( fd, F_GETOWN, 1 );
  rtems_test_assert ( status == -1 );

  printf("fcntl invalid argument should return -1\n");
  status = fcntl( fd, 0xb, 1 );
  printf("Status %d\n",status);
  rtems_test_assert( status == -1 );

  printf("close should return 0\n");
  status = close( fd );
  rtems_test_assert ( status == 0 );

  printf("opendir, readdir and closedir %s\n", my_file);
  directory_not = opendir (my_file);
  rtems_test_assert( directory_not != NULL );
  d_not = readdir(directory_not);
  rtems_test_assert( d_not == NULL );
  status = closedir (directory_not);
  rtems_test_assert (status == 0);

  printf("opendir, readdir and closedir\n");
  directory_not = opendir ("/a");
  rtems_test_assert( directory_not != NULL );
  d_not = readdir (directory_not);
  rtems_test_assert( d_not == NULL );
  status = closedir (directory_not);
  rtems_test_assert (status == 0);

  printf("chdir to %s\n", my_file);
  status = chdir (my_file);
  rtems_test_assert (status == -1);

  printf( "\nPerforming stat of directory /\n");
  status = stat( "/", &s );
  printf("status for stat : %d, size of directory: %" PRIdoff_t "\n\n", status, s.st_size);

  puts( "\nOpen and print directory /" );
  directory = opendir("/");
  rtems_test_assert( directory );
  printdir(directory);

  printf("\nmkdir /d/my_dir\n");
  status = mkdir( "/d/my_dir", 0x1c0 );
  printf("Open /d/my_dir\n");
  directory_not = opendir( "/d/my_dir" );
  rtems_test_assert( directory_not );

  printf( "remove /d/my_dir.\n" );
  status = rmdir( "/d/my_dir" );
  rtems_test_assert( status == 0 );

  printf( "close /d/my_dir.\n" );
  closedir( directory_not );

  printf( "\nOpening directory /c\n" );
  directory2 = opendir("/c");

  rtems_test_assert( directory2 );

  printdir(directory2);
  status = closedir( directory2 );

  printf( "\nOpening directory /c/y\n" );
  directory3 = opendir("/c/y");
  rtems_test_assert( directory3 );
  printdir(directory3);
  status = closedir( directory3 );

  printf( "\nLSEEK to the start of the open directory\n" );
  lseek( directory->dd_fd, 0, SEEK_SET );
  printdir(directory);

  lseek( directory->dd_fd, 0, SEEK_CUR );

  lseek( directory->dd_fd, 0, SEEK_END );

  lseek( directory->dd_fd, 0, -99 );

  printf( "\nRewinding directory\n" );
  rewinddir( directory );
  printdir(directory);

#if 0
  /* Newlib's implementation does not check for NULL */
  printf( "Send rewinddir a NULL pointer\n");
  rewinddir( NULL );
#endif

  printf( "\nSeek directory\n" );
  printf( "telldir() should report only sizeof(struct dirent) increments \n" );
  printf( "in position. Sizeof(struct dirent): %zd\n",
                          sizeof(struct dirent) );
  rewinddir( directory );
  for( off=0 ; off<=200 ; off=off + sizeof(struct dirent) / 4 ) {
    seekdir( directory, off );
    printf(
       "seeked to %2" PRIdoff_t " -- currently at %2ld\n",
       off,
       telldir(directory)
    );
  }

  printf( "Send seekdir a NULL pointer\n");
  seekdir( NULL, off );

  printf( "\nClosing directory\n" );
  status = closedir( directory );

  printf( "\nSCANDIR TEST\n");
  printf( "\nselection rule 1\n");
  printf( "scanning for any entry under directory /c\n\n");
  status = scandir(
     "/c",
     &namelist,
     select1,
     NULL
  );
  printf("\nscandir status: %d\n", status );
  for ( i=0; i<status; i++)
  {
     printf("Selected Node Name: %s\n", namelist[i]->d_name );
  }

  printf( "\nselection rule 2\n");
  printf( "scanning for any entry under directory /c whose name = y\n\n");
  status = scandir(
     "/c",
     &namelist,
     select2,
     NULL
  );
  printf("\nscandir status: %d\n", status );
  for ( i=0; i<status; i++)
  {
     printf("Selected Node Name: %s\n", namelist[i]->d_name );
  }

  printf( "\nSCANDIR with sorting\n" );
  printf( "\nselection rule 1\n");
  printf( "scanning for any entry under directory /c\n");
  printf( "sort in ascending order\n\n");
  status = scandir(
     "/c",
     &namelist,
     select1,
     compare_ascending
  );
  printf("\nscandir status: %d\n", status );
  for ( i=0; i<status; i++)
  {
     printf("Selected and Sorted Node Name: %s\n", namelist[i]->d_name );
  }


  printf( "\nSCANDIR with sorting\n" );
  printf( "\nselection rule 1\n");
  printf( "scanning for any entry under directory /c\n");
  printf( "sort in descending order\n\n");
  status = scandir(
     "/c",
     &namelist,
     select1,
     compare_descending
  );
  printf("scandir status: %d\n", status );
  for ( i=0; i<status; i++)
  {
     printf("Selected and Sorted Node Name: %s\n", namelist[i]->d_name );
  }

  printf("unlink %s should return 0\n", my_file);
  status = unlink( my_file );
  rtems_test_assert ( status == 0 );

  test_across_mount();
  printf( "\n\n*** END OF READDIR TEST ***\n" );
  rtems_test_exit(0);
}
