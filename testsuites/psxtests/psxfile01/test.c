/**
 *  @file
 *
 *  Simple test program to exercise some of the basic functionality of
 *  POSIX Files and Directories Support.
 *
 *  This test assumes that the file system is initialized with the
 *  following directory structure:
 *
 *  XXXX fill this in.
 *    /
 *    /dev
 *    /dev/XXX   [where XXX includes at least console]
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

#include <pmacros.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <reent.h>
#include <rtems/imfs.h>

#include <rtems.h>
#include <rtems/libio.h>

#include "primode.h"

/* forward declarations to avoid warnings */
void test_case_reopen_append(void);
void dump_statbuf(struct stat *buf);
void stat_a_file(const char *file);
int test_main(void);

char test_write_buffer[ 1024 ];
rtems_filesystem_operations_table  IMFS_ops_no_evalformake;
rtems_filesystem_operations_table  IMFS_ops_no_rename;

/*
 *  File test support routines.
 */

void test_cat(
  char *file,
  int   offset_arg,
  int   length
);

void test_write(
  char   *file,
  off_t  offset,
  char  *buffer
);

void test_extend(
  char *file,
  off_t new_len
);

/*
 *  dump_statbuf
 */
void dump_statbuf( struct stat *buf )
{
  int         major1;
  int         minor1;
  int         major2;
  int         minor2;

  rtems_filesystem_split_dev_t( buf->st_dev, major1, minor1 );
  rtems_filesystem_split_dev_t( buf->st_rdev, major2, minor2 );

  printf( "....st_dev     (0x%x:0x%x)\n", major1, minor1 );
  printf( "....st_ino     %" PRIxino_t "  may vary by small amount\n", buf->st_ino );
  printf( "....mode  = %08" PRIomode_t "\n", buf->st_mode );
  printf( "....nlink = %d\n", buf->st_nlink );

  printf( "....uid = %d\n", buf->st_uid );
  printf( "....gid = %d\n", buf->st_gid );

  printf( "....atime = %s", ctime(&buf->st_atime) );
  printf( "....mtime = %s", ctime(&buf->st_mtime) );
  printf( "....ctime = %s", ctime(&buf->st_ctime) );

  printf( "....st_blksize %" PRIxblksize_t "\n", buf->st_blksize );
  printf( "....st_blocks  %" PRIxblkcnt_t "\n", buf->st_blocks );
}

void stat_a_file(
  const char *file
)
{
  int         status;
  struct stat statbuf;

  rtems_test_assert( file );

  printf( "stat( %s ) returned ", file );
  fflush( stdout );

  status = stat( file, &statbuf );

  if ( status == -1 ) {
    printf( ": %s\n", strerror( errno ) );
  } else {
    puts("");
    dump_statbuf( &statbuf );
  }

}

/*
 *  Main entry point of the test
 */

#if defined(__rtems__)
int test_main(void)
#else
int main(
  int argc,
  char **argv
)
#endif
{
  int               status;
  size_t            max_size;
  int               fd;
  int               i;
  struct stat       buf;
  char              buffer[128];
  FILE             *file;
  time_t            atime1;
  time_t            mtime1;
  time_t            ctime1;
  time_t            atime2;
  time_t            mtime2;
  time_t            ctime2;
  rtems_status_code rtems_status;
  rtems_time_of_day time;

  printf( "\n\n*** FILE TEST 1 ***\n" );

  /*
   *  Grab the maximum size of an in-memory file.
   */

  max_size = IMFS_memfile_maximum_size();

  build_time( &time, 12, 31, 1988, 9, 0, 0, 0 );
  rtems_status = rtems_clock_set( &time );
  directive_failed( rtems_status, "clock set" );

  /*
   *  Dump an empty file system
   */

  IMFS_dump();

  /*
   *  Simple stat() of /dev/console.
   */

  puts( "stat of /dev/console" );
  status = stat( "/dev/console", &buf );
  rtems_test_assert( !status );

  dump_statbuf( &buf );

  /*
   *  Exercise mkdir() and some path evaluation.
   */

  puts( "" );
  puts( "mkdir /dev/tty" );
  status = mkdir( "/dev/tty", S_IRWXU );
  rtems_test_assert( !status );

  puts( "" );
  puts( "mkdir /usr" );
  status = mkdir( "/usr", S_IRWXU );
  rtems_test_assert( !status );
  puts( "mkdir /etc" );
  status = mkdir( "/etc", S_IRWXU );
  rtems_test_assert( !status );

  puts( "mkdir /tmp" );
  status = mkdir( "/tmp", S_IRWXU );
  rtems_test_assert( !status );

  /* this tests the ".." path in path name evaluation */
  puts( "mkdir /tmp/.." );
  status = mkdir( "/tmp/..", S_IRWXU );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EEXIST );

  /* now check out trailing separators */
  puts( "mkdir /tmp/" );
  status = mkdir( "/tmp/", S_IRWXU );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EEXIST );

  /* try to make a directory under a non-existent subdirectory */
  puts( "mkdir /j/j1" );
  status = mkdir( "/j/j1", S_IRWXU );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOENT );

  /* this tests the ability to make a directory in the current one */
  puts( "mkdir tmp" );
  status = mkdir( "tmp", S_IRWXU );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EEXIST );

  /*
   *  Now switch gears and exercise rmdir().
   */

  puts( "" );
  puts( "rmdir /usr" );
  status = rmdir( "/usr" );
  rtems_test_assert( !status );

  puts( "rmdir /dev" );
  status = rmdir( "/dev" );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno ==  ENOTEMPTY);

  puts( "rmdir /fred" );
  status = rmdir ("/fred");
  rtems_test_assert (status == -1);
  rtems_test_assert( errno == ENOENT );

  puts( "rmdir /tmp/bha" );
  status = rmdir( "/tmp/bha" );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOENT );

  puts( "mknod /dev/test_console" );
  status = mknod( "/dev/test_console", S_IFCHR, 0LL );
  rtems_test_assert( !status );

  puts( "mknod /dev/tty/S3" );
  status = mknod( "/dev/tty/S3", S_IFCHR, 0xFF00000080LL );
  rtems_test_assert( !status );

  puts ("mknod /etc/passwd");
  status = mknod( "/etc/passwd", (S_IFREG | S_IRWXU), 0LL );
  rtems_test_assert( !status );

  puts( "mkdir /tmp/my_dir");
  status = mkdir( "/tmp/my_dir", S_IRWXU );
  rtems_test_assert( status == 0 );

  puts("mkfifo /c/my_dir" );
  status = mkfifo( "/c/my_dir", S_IRWXU );
  rtems_test_assert( status == -1 );

  /*
   *  Try to make a directory under a file -- ERROR
   */

  puts( "mkdir /etc/passwd/j" );
  status = mkdir( "/etc/passwd/j", S_IRWXU );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOTDIR );

  /*
   *  Simple open failure case on non-existent file
   */

  puts( "open /tmp/joel - should fail with ENOENT" );
  fd = open( "/tmp/joel", O_RDONLY );
  rtems_test_assert( fd == -1 );
  rtems_test_assert( errno == ENOENT );

  /*
   *  Simple open case where the file is created.
   */

  puts( "open /tmp/j" );
  fd = open( "/tmp/j", O_RDWR|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO );
  rtems_test_assert( fd != -1 );
  printf( "open returned file descriptor %d\n", fd );

  puts( "close /tmp/j" );
  status = close( fd );
  rtems_test_assert( !status );

  puts( "close /tmp/j again" );
  status = close( fd );
  rtems_test_assert( status == -1 );

  puts( "unlink /tmp/j" );
  status = unlink( "/tmp/j" );
  rtems_test_assert( !status );

  puts( "unlink /tmp" );
  status = unlink( "/tmp" );
  rtems_test_assert( status );

  /*
   *  Simple open failure. Trying to create an existing file.
   */

  puts("create and close /tmp/tom");
  fd = open( "/tmp/tom", O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO );
  rtems_test_assert( fd != -1 );
  status = close( fd );
  rtems_test_assert( status == 0 );

  puts("Attempt to recreate /tmp/tom");
  fd = open( "/tmp/tom", O_CREAT | O_EXCL, S_IRWXU|S_IRWXG|S_IRWXO );
  rtems_test_assert( fd == -1 );
  rtems_test_assert( errno == EEXIST );

  puts("create /tmp/john");
  fd = open( "/tmp/john", O_RDWR|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO );
  rtems_test_assert( fd != -1 );
  status = close( fd );
  rtems_test_assert( status == 0 );

  /* 
   * Open a file in read-only mode and try to truncate
   */

  puts( "Attempt to create a file, open in read-only mode and truncate it" );
  fd = open( "/tmp/bha", O_CREAT | O_RDONLY | O_TRUNC, S_IRUSR );
  rtems_test_assert( fd == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "Exercise the reentrant version _link_r -- Expect ENOENT" );
  status = _link_r( NULL, "/tmp/notexist", "/tmp/cannotexist" );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOENT );

  puts( "Unlink /tmp/bha using the reentrant version -- OK" );
  status = _unlink_r( NULL, "/tmp/bha" );
  rtems_test_assert( status == 0 );

  /*
   * Simple test case for mknod
   */

  puts( "mknod with bad type - expect EINVAL" );
  status = mknod( "/tmp/bha", 0, 0LL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EINVAL );

  /* 
   * Read from filedes opened for write
   */

  puts( "open /tmp/bha in write only mode -- OK" );
  fd = open( "/tmp/bha", O_CREAT | O_WRONLY, S_IRWXU|S_IRWXG|S_IRWXO );
  rtems_test_assert( fd != -1 );

  puts( "attempt fcntl on opened file -- OK" );
  status = fcntl( fd, F_SETFD, 0 );
  rtems_test_assert( status == 0 );

  puts( "attempt to read from /tmp/bha - expect EBADF" );
  status = read( fd, buffer, 10 );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EBADF );

  puts( "closing and unlinking /tmp/bha" );
  status = close( fd );
  status |= unlink( "/tmp/bha" );
  rtems_test_assert( status == 0 );

  puts( "open /tmp/bha in read only mode -- OK" );
  fd = open( "/tmp/bha", O_CREAT | O_RDONLY, S_IRWXU|S_IRWXG|S_IRWXO );
  rtems_test_assert( fd != -1 );
  
  puts( "attempt to read from /tmp/bha - expect EBADF" );
  status = write( fd, buffer, 10 );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EBADF );

  puts( "closing and unlinking /tmp/bha" );
  status = close( fd );
  status |= unlink( "/tmp/bha" );
  rtems_test_assert( status == 0 );

  /* 
   * Read/write from an unopened filedes
   */
  puts( "attempt to read from an unopened filedes - expect EBADF" );
  status = read( 5, buffer, 10 );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EBADF );

  puts( "attempt to write to an unopened filedes - expect EBADF" );
  status = write( 5, buffer, 10 );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EBADF );

  /*
   *  Test simple write to a file at offset 0
   */

  puts( "mknod /tmp/joel" );
  status = mknod( "/tmp/joel", (S_IFREG | S_IRWXU), 0LL );
  test_write( "/tmp/joel", 0, "the first write!!!\n" );
  test_cat( "/tmp/joel", 0, 0 );

  /* Exercise _rename_r */

  /* Simple rename test */
  puts( "rename /tmp/joel to /tmp/drjoel");
  status = _rename_r(NULL,"/tmp/joel","/tmp/drjoel");
  rtems_test_assert(status == 0);

  /* Simple rename test */
  puts("rename /tmp/drjoel to /tmp/joel");
  status = _rename_r(NULL,"/tmp/drjoel","/tmp/joel");
  rtems_test_assert(status == 0);

  /* Invalid old path */
  puts("rename /tmp/drjoel to /tmp/joel - Should result in an error \
since old path is not valid");
  status = _rename_r(NULL,"/tmp/drjoel","/tmp/joel");
  rtems_test_assert(status == -1);

  /* Invalid new path */
  puts("rename /tmp/joel to /tmp/drjoel/joel - Should result in an error \
since new path is not valid");
  status = _rename_r(NULL,"/tmp/joel","/tmp/drjoel/joel");
  rtems_test_assert(status == -1);

  puts("changing dir to /tmp");
  status = chdir("/tmp/");
  rtems_test_assert(status == 0);

  puts("rename joel to drjoel");
  status = _rename_r(NULL,"joel","drjoel");
  rtems_test_assert(status == 0);

  puts("rename drjoel to joel");
  status = _rename_r(NULL,"drjoel","joel");
  rtems_test_assert(status == 0);

  /* Rename across file systems */
  puts("creating directory /imfs");
  status = mkdir("/imfs",0777);
  rtems_test_assert(status == 0);
  puts("creating directory /imfs/hidden_on_mount");
  status = mkdir("/imfs/hidden_on_mount",0777);
  rtems_test_assert(status == 0);

  puts("mounting filesystem with IMFS_ops at /imfs");
  status = mount("null", "/imfs", "imfs", RTEMS_FILESYSTEM_READ_WRITE, NULL);
  rtems_test_assert(status == 0);
  puts("creating directory /imfs/test (on newly mounted filesystem)");
  status = mkdir("/imfs/test", 0777);
  rtems_test_assert(status == 0);

  puts("attempt to rename directory joel to /imfs/test/joel - should fail with EXDEV");
  status = _rename_r(NULL, "joel", "/imfs/test/joel");
  rtems_test_assert(status == -1);
  rtems_test_assert(errno == EXDEV);

  puts("changing dir to /");
  status = chdir("/");
  rtems_test_assert(status == 0);

  puts("attempt to rename across filesystem, with old path having a parent node");
  puts("attempt to rename tmp/joel to /imfs/test/joel");
  status = _rename_r(NULL, "tmp/joel", "/imfs/test/joel");
  rtems_test_assert(status == -1);
  rtems_test_assert(errno == EXDEV);

  puts("End of _rename_r tests");

  /*
   *  Test simple write to a file at a non-0 offset in the first block
   */

  status = unlink( "/tmp/joel" );
  rtems_test_assert( !status );

  status = mknod( "/tmp/joel", (S_IFREG | S_IRWXU), 0LL );
  rtems_test_assert( !status );

  test_write( "/tmp/joel", 10, "the first write!!!\n" );
  test_cat( "/tmp/joel", 0, 0 );
  stat_a_file( "/tmp/joel" );

  /*
   *  Test simple write to a file at a non-0 offset in the second block.  Then
   *  try to read from various offsets and lengths.
   */

  puts("unlink /tmp/joel");
  status = unlink( "/tmp/joel" );
  rtems_test_assert( !status );

  /* Test a failure path */

  puts( "unlink /tmp/joel" );
  status = unlink( "/tmp/joel" );
  rtems_test_assert( status == -1 );

  puts( "mknod /tmp/joel");
  status = mknod( "/tmp/joel", (S_IFREG | S_IRWXU), 0LL );
  rtems_test_assert( !status );

  test_write( "/tmp/joel", 514, "the first write!!!\n" );
  test_write( "/tmp/joel", 1, test_write_buffer );
  test_write( "/tmp/joel", 63, test_write_buffer );
  test_cat( "/tmp/joel", 0, 1 );
  test_cat( "/tmp/joel", 1, 1 );
  test_cat( "/tmp/joel", 490, 1 );
  test_cat( "/tmp/joel", 512, 1 );
  test_cat( "/tmp/joel", 513, 1 );
  test_cat( "/tmp/joel", 514, 1 );
  test_cat( "/tmp/joel", 520, 1 );
  test_cat( "/tmp/joel", 1, 1024 );

  /*
   *  Read from a much longer file so we can descend into doubly and
   *  triply indirect blocks.
   */

  if ( max_size < (size_t) 300 * 1024 ) {
    test_extend( "/tmp/joel", max_size - 1 );
    test_cat( "/tmp/joel", max_size / 2, 1024 );
  } else {
    printf( "Skipping maximum file size test since max_size is %zu bytes\n", max_size );
    puts("That is likely to be bigger than the available RAM on many targets." );
  }

  stat_a_file( "/tmp/joel" );

  /*
   *  Now try to use a FILE * descriptor
   *
   *  /tmp/j should not exist at this point.
   */

  puts( "stat of /tmp/j" );
  errno = 0;
  status = stat( "/tmp/j", &buf );
  printf( "stat(/tmp/j) returned %d (errno=%d)\n", status, errno );
  dump_statbuf( &buf );

  puts( "fopen of /tmp/j" );
  file = fopen( "/tmp/j", "w+" );
  rtems_test_assert( file );

  puts( "fprintf to /tmp/j" );
  for (i=1 ; i<=5 ; i++) {
    status = fprintf( file, "This is call %d to fprintf\n", i );
    rtems_test_assert( status );
    printf( "(%d) %d characters written to the file\n", i, status );
  }

  fflush( file );

  status = stat( "/tmp/j", &buf );
  rtems_test_assert( !status );
  dump_statbuf( &buf );
  atime2 = buf.st_atime;
  mtime2 = buf.st_mtime;
  ctime2 = buf.st_ctime;


  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  rewind( file );
  while ( fgets(buffer, 128, file) )
    printf( "%s", buffer );

  /*
   * Verify only atime changed for a read.
   */
  status = stat( "/tmp/j", &buf );
  rtems_test_assert( !status );
  dump_statbuf( &buf );
  atime1 = buf.st_atime;
  mtime1 = buf.st_mtime;
  ctime1 = buf.st_ctime;
  rtems_test_assert( atime1 != atime2);
  rtems_test_assert( mtime1 == mtime2);
  rtems_test_assert( ctime1 == ctime2);

  IMFS_dump();

  unlink( "/tmp/joel" );

  /*
   *  Now truncate a file
   */

  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  puts( "truncate /tmp/j to length of 40" );
  status = truncate( "/tmp/j", 40 );
  rtems_test_assert( !status );

  /*
   * Verify truncate changed only atime.
   */
  status = stat( "/tmp/j", &buf );
  rtems_test_assert( !status );
  dump_statbuf( &buf );
  atime2 = buf.st_atime;
  mtime2 = buf.st_mtime;
  ctime2 = buf.st_ctime;
  rtems_test_assert( atime1 != atime2);
  rtems_test_assert( mtime1 == mtime2);
  rtems_test_assert( ctime1 == ctime2);

  IMFS_dump();

  /* try to truncate the console and see what happens */
  status = truncate( "/dev/console", 40 );
  rtems_test_assert( status == 0 );

  puts( "truncate /tmp/j to length of 0" );
  status = truncate( "/tmp/j", 0 );
  rtems_test_assert( !status );

  puts( "truncate /tmp to length of 0 should fail with EISDIR\n");
  status = truncate( "/tmp", 0 );
  rtems_test_assert( status == -1 );
  printf( "%d: %s\n", errno, strerror( errno ) );
  rtems_test_assert( errno == EISDIR );

  IMFS_dump();

  status = truncate( "/tmp/fred", 10 );
  rtems_test_assert( status == -1);

  rtems_status = rtems_io_register_name( "/dev/not_console", 0, 0 );
  directive_failed( rtems_status, "io register" );

  test_case_reopen_append();

  printf( "*** END OF FILE TEST 1 ***\n" );
  rtems_test_exit( 0 );
}

/*
 *  Open/Create a File and write to it
 *
 *  Test case submitted by Andrew Bythell <abythell@nortelnetworks.com>.
 *
 */

void test_file (char *filename, char *mode);

void test_case_reopen_append(void)
{
  printf ("Writing First File\n");
  test_file ("/one.txt", "a");
  test_file ("/one.txt", "a");

  /* but not the second time - this will insert junk.
     the number of ^@'s seems to equal the number of
     actual characters in the file */

  printf ("Writing Second File\n");
  test_file ("/two.txt", "a");
  test_file ("/two.txt", "a");

  test_cat( "/one.txt", 0, 1024 );
  test_cat( "/two.txt", 0, 1024 );
}

void test_file (char *filename, char *mode)
{
  FILE *fp;
  fp = fopen (filename, mode);
  if (!fp)
      perror ("fopen");
  fprintf (fp, "this is a test line\n");
  if (fclose (fp))
      perror ("fclose");
}
