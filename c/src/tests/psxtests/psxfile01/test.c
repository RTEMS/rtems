/*
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
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <stdio.h>

#include <tmacros.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include <assert.h>
#include <rtems.h>
#include <rtems/libio.h>

void test_case_reopen_append(void);

char test_write_buffer[ 1024 ];

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

void IMFS_dump( void );
int IMFS_memfile_maximum_size( void );

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
  printf( "....st_ino     %x  may vary by small amount\n",
      (unsigned int) buf->st_ino );
  printf( "....mode  = %08o\n", buf->st_mode );
  printf( "....nlink = %d\n", buf->st_nlink );

  printf( "....uid = %d\n", buf->st_uid );
  printf( "....gid = %d\n", buf->st_gid );

  printf( "....atime = %s", ctime(&buf->st_atime) );
  printf( "....mtime = %s", ctime(&buf->st_mtime) );
  printf( "....ctime = %s", ctime(&buf->st_ctime) );

#if defined(__svr4__) && !defined(__PPC__) && !defined(__sun__)
  printf( "....st_blksize %x\n", buf.st_blksize );
  printf( "....st_blocks  %x\n", buf.st_blocks );
#endif

}

void stat_a_file(
  const char *file
)
{
  int         status;
  struct stat statbuf;

  assert( file );

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
  int               max_size;
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

  /*
   *  Dump an empty file system
   */

  IMFS_dump();

  /*
   *  Simple stat() of /dev/console.
   */

  puts( "stat of /dev/console" );
  status = stat( "/dev/console", &buf );
  assert( !status );

  dump_statbuf( &buf );

  /*
   *  Exercise mkdir() and some path evaluation.
   */

  puts( "" );
  puts( "mkdir /dev/tty" );
  status = mkdir( "/dev/tty", S_IRWXU );
  assert( !status );

  puts( "" );
  puts( "mkdir /usr" );
  status = mkdir( "/usr", S_IRWXU );
  assert( !status );
  puts( "mkdir /etc" );
  status = mkdir( "/etc", S_IRWXU );
  assert( !status );

  puts( "mkdir /tmp" );
  status = mkdir( "/tmp", S_IRWXU );
  assert( !status );

  /* this tests the ".." path in path name evaluation */
  puts( "mkdir /tmp/.." );
  status = mkdir( "/tmp/..", S_IRWXU );
  assert( status == -1 );
  assert( errno == EEXIST );

  /* now check out trailing separators */
  puts( "mkdir /tmp/" );
  status = mkdir( "/tmp/", S_IRWXU );
  assert( status == -1 );
  assert( errno == EEXIST );

  /* try to make a directory under a non-existent subdirectory */
  puts( "mkdir /j/j1" );
  status = mkdir( "/j/j1", S_IRWXU );
  assert( status == -1 );
  assert( errno == ENOENT );

  /* this tests the ability to make a directory in the current one */
  puts( "mkdir tmp" );
  status = mkdir( "tmp", S_IRWXU );
  assert( status == -1 );
  assert( errno == EEXIST );

  /* test rtems_filesystem_evaluate_path by sending NULL path */
  status = chdir( NULL );
  assert( status == -1 );

  /*
   *  Now switch gears and exercise rmdir().
   */

  puts( "" );
  puts( "rmdir /usr" );
  status = rmdir( "/usr" );
  assert( !status );

  puts( "rmdir /dev" );
  status = rmdir( "/dev" );
  assert( status == -1 );
  assert( errno ==  ENOTEMPTY);

  puts( "rmdir /fred" );
  status = rmdir ("/fred");
  assert (status == -1);
  assert( errno == ENOENT );

  puts( "mknod /dev/test_console" );
  status = mknod( "/dev/test_console", S_IFCHR, 0LL );
  assert( !status );

  puts( "mknod /dev/tty/S3" );
  status = mknod( "/dev/tty/S3", S_IFCHR, 0xFF00000080LL );
  assert( !status );

  puts ("mknod /etc/passwd");
  status = mknod( "/etc/passwd", (S_IFREG | S_IRWXU), 0LL );
  assert( !status );

  puts( "mkdir /tmp/my_dir");
  status = mkdir( "/tmp/my_dir", S_IRWXU );
  assert( status == 0 );

  puts("mkfifo /c/my_dir" );
  status = mkfifo( "/c/my_dir", S_IRWXU );
  assert( status == -1 );

  /*
   *  Try to make a directory under a file -- ERROR
   */

  puts( "mkdir /etc/passwd/j" );
  status = mkdir( "/etc/passwd/j", S_IRWXU );
  assert( status == -1 );
  assert( errno == ENOTDIR );

  /*
   *  Simple open failure case on non-existent file
   */

  puts( "open /tmp/joel - should fail with ENOENT" );
  fd = open( "/tmp/joel", O_RDONLY );
  assert( fd == -1 );
  assert( errno == ENOENT );

  /*
   *  Simple open case where the file is created.
   */

  puts( "open /tmp/j" );
  fd = open( "/tmp/j", O_RDWR|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO );
  assert( fd != -1 );
  printf( "open returned file descriptor %d\n", fd );

  puts( "close /tmp/j" );
  status = close( fd );
  assert( !status );

  puts( "close /tmp/j again" );
  status = close( fd );
  assert( status == -1 );

  puts( "unlink /tmp/j" );
  status = unlink( "/tmp/j" );
  assert( !status );

  puts( "unlink /tmp" );
  status = unlink( "/tmp" );
  assert( status );

  /*
   *  Simple open failure. Trying to create an existing file.
   */

  puts("create and close /tmp/tom");
  fd = open( "/tmp/tom", O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO );
  assert( fd != -1 );
  status = close( fd );
  assert( status == 0 );

  puts("Attempt to recreate /tmp/tom");
  fd = open( "/tmp/tom", O_CREAT | O_EXCL, S_IRWXU|S_IRWXG|S_IRWXO );
  assert( fd == -1 );
  assert( errno == EEXIST );

  puts("create /tmp/john");
  fd = open( "/tmp/john", O_RDWR|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO );
  assert( fd != -1 );

  puts("tcdrain /tmp/john" );
  status = tcdrain( fd );
  assert( status == 0 );

  /*
   *  Test simple write to a file at offset 0
   */

  puts( "mknod /tmp/joel" );
  status = mknod( "/tmp/joel", (S_IFREG | S_IRWXU), 0LL );
  test_write( "/tmp/joel", 0, "the first write!!!\n" );
  test_cat( "/tmp/joel", 0, 0 );

  /*
   *  Test simple write to a file at a non-0 offset in the first block
   */

  status = unlink( "/tmp/joel" );
  assert( !status );

  status = mknod( "/tmp/joel", (S_IFREG | S_IRWXU), 0LL );
  assert( !status );

  test_write( "/tmp/joel", 10, "the first write!!!\n" );
  test_cat( "/tmp/joel", 0, 0 );
  stat_a_file( "/tmp/joel" );

  /*
   *  Test simple write to a file at a non-0 offset in the second block.  Then
   *  try to read from various offsets and lengths.
   */

  puts("unlink /tmp/joel");
  status = unlink( "/tmp/joel" );
  assert( !status );

  /* Test a failure path */

  puts( "unlink /tmp/joel" );
  status = unlink( "/tmp/joel" );
  assert( status == -1 );

  puts( "mknod /tmp/joel");
  status = mknod( "/tmp/joel", (S_IFREG | S_IRWXU), 0LL );
  assert( !status );

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

  if ( max_size < 300 * 1024 ) {
    test_extend( "/tmp/joel", max_size - 1 );
    test_cat( "/tmp/joel", max_size / 2, 1024 );
  } else {
    printf( "Skipping maximum file size test since max_size is %d bytes\n", max_size );
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
  assert( file );

  puts( "fprintf to /tmp/j" );
  for (i=1 ; i<=5 ; i++) {
    status = fprintf( file, "This is call %d to fprintf\n", i );
    assert( status );
    printf( "(%d) %d characters written to the file\n", i, status );
  }

  fflush( file );

  status = stat( "/tmp/j", &buf );
  assert( !status );
  dump_statbuf( &buf );
  atime2 = buf.st_atime;
  mtime2 = buf.st_mtime;
  ctime2 = buf.st_ctime;


  status = rtems_task_wake_after( 1 * TICKS_PER_SECOND );
  rewind( file );
  while ( fgets(buffer, 128, file) )
    printf( "%s", buffer );

  /* 
   * Verify only atime changed for a read.
   */
  status = stat( "/tmp/j", &buf );
  assert( !status );
  dump_statbuf( &buf );
  atime1 = buf.st_atime;
  mtime1 = buf.st_mtime;
  ctime1 = buf.st_ctime;
  assert( atime1 != atime2);
  assert( mtime1 == mtime2);
  assert( ctime1 == ctime2);

  IMFS_dump();

  unlink( "/tmp/joel" );

  /*
   *  Now truncate a file
   */

  status = rtems_task_wake_after( 1 * TICKS_PER_SECOND );
  puts( "truncate /tmp/j to length of 40" );
  status = truncate( "/tmp/j", 40 );
  assert( !status );

  /* 
   * Verify truncate changed only atime.
   */
  status = stat( "/tmp/j", &buf );
  assert( !status );
  dump_statbuf( &buf );
  atime2 = buf.st_atime;
  mtime2 = buf.st_mtime;
  ctime2 = buf.st_ctime;
  assert( atime1 != atime2);
  assert( mtime1 == mtime2);
  assert( ctime1 == ctime2);

  IMFS_dump();

  /* try to truncate the console and see what happens */
  status = truncate( "/dev/console", 40 );
  assert(status == -1 );

  puts( "truncate /tmp/j to length of 0" );
  status = truncate( "/tmp/j", 0 );
  assert( !status );

  puts( "truncate /tmp to length of 0 should fail with EISDIR\n");
  status = truncate( "/tmp", 0 );
  assert( status == -1 );
  printf( "%d: %s\n", errno, strerror( errno ) );
  assert( errno == EISDIR );

  IMFS_dump();

  status = truncate( "/tmp/fred", 10 );
  assert( status == -1);

  rtems_status = rtems_io_register_name( "/dev/console", 0, 0 );

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


