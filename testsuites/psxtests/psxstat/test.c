/**
 *  @file
 *
 *  This test exercises stat() via fstat() and generates as many of the
 *  path evaluation cases as possible.
 *
 *  This test also exercises lstat() and lchown() when symlinks are
 *  involved.
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

#include <tmacros.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <reent.h>
#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/imfs.h>
#include <pmacros.h>

#include "primode.h"

/* forward declarations to avoid warnings */
int test_main(void);
void stat_a_file_helper(const char *file, int follow_link);
void stat_a_file(const char *file);
void lstat_a_file(const char *file);
void stat_multiple_files(char **files);
void chown_multiple_files_helper(char **files, int follow_link);
void chown_multiple_files(char **files);
void lchown_multiple_files(char **files);
void make_multiple_files(char **files);
void make_multiple_bad_files(char **files);
void make_multiple_links(char **existing, char **new);
void make_too_many_links(void);
void make_a_symlink(char *existing, char *new);
void make_multiple_symlinks(void);
void make_many_symlinks(char  *real_file, int link_count);
void make_multiple_directories(char **files);
void Cause_faults(void);
void Show_Time(void);
void test_statvfs(void);

#define MAXSYMLINK 5   /* There needs to be a better way of getting this. */
#define TIMEOUT_VALUE  ( 5 * rtems_clock_get_ticks_per_second() )

int _lstat_r(struct _reent *, const char *, struct stat *);

/*
 *  List of files which should exist.
 */

char *Files[] = {
  "/////my_mount_point/dir1/\\//file1",
  "/my_mount_point/dir1/file2",
  "/my_mount_point/dir1/file3",
  "/my_mount_point/dir1/file4",
  "/my_mount_point/dir1/dir1/file1",
  "../../..//my_mount_point/dir1/./././dir1/ file1",
  "main.c",
  0
};

/*
 *  List of directories which should exist.
 */

char *Directories[] = {
  "/my_mount_point/dir1\\\\//",
  "/my_mount_point/dir2",
  "/my_mount_point/dir3",
  "/my_mount_point/dir4",
  "/my_mount_point/dir1/dir1",
  "/./././my_mount_point/dir1/ dir1",
  "/./././my_mount_point/links",
  "///my_mount_point/dir1/dir1/../../dir1/../symlinks/////",
  0
};

char *Links_to_Dirs[]= {
  "dir1/dir1/../../links/dir1",
  "links/dir2",
  "links/dir3",
  "links/dir4",
  "links/dir1_dir1",
  "links/dir1_ dir1",
  "links/../links/../links/links",
  0
};

char *Links_to_Files[]= {
  "links/dir1_file1",
  "links/dir1_file2",
  "links/dir1_file3",
  "links/dir1_file4",
  "links/dir1_dir1_f1",
  "links/dir1_dir1 f1",
  0
};

char *Links_to_dirlinks[]= {
  "links/links/links/links_dir1",
  "links//links_dir2",
  "links//links_dir3",
  "links//links_dir4",
  "links//links_dir1_d1",
  "links//links_dir1 d1",
  "links//links_links",
  0
};

char *Links_to_filelinks[]= {
  "links///links_d1_file1",
  "links///links_d1_file2",
  "links///links_d1_file3",
  "links///links_d1_file4",
  "links///links_d1_d1_f1",
  "links///links_r1_d1 f1",
  0
};

char *SymLinks[]= {
  "/my_mount_point/symlinks/a_file_symlink",
  "/my_mount_point/symlinks/a_dir_symlink",
  "/my_mount_point/symlinks/a_link_symlink",
  "../symlinks/no_file",
  "/my_mount_point/symlinks/a_dir_symlink/a_file_symlink",
  0
};

/*
 *  List of absolute paths to stat.
 */

char *Good_absolute_paths[] = {
  "/my_mount_point/dev",
  "////my_mount_point/dir1/\\//file1\\\\//",
  "/my_mount_point/dir1/\\\\/file2",
  "/my_mount_point/dir1/file3/////\\\\\\",
  "/my_mount_point/dir1/file4",
  "/my_mount_point/dir1/dir1/file1",
  "/my_mount_point/dir1/dir1/ file1",
  "/my_mount_point/dir1",
  "/my_mount_point/dir2//////\\",
  "/my_mount_point/dir3",
  "/my_mount_point/dir4",
  "/my_mount_point/dir1/dir1",
  "/my_mount_point/dir1/ dir1///\\\\",
  "/my_mount_point/\\/\\/\\/\\/\\/\\/links\\/\\/\\/\\/\\/\\",
  0
};


char *Bad_paths[] = {
  "/my_mount_point/links/ENAMETOOLONG__________________________",
  "/my_mount_point/dir1/file4/NOTADIR",
  "/my_mount_point/dir1/dir1/EACCES__",
  0
};

/*
 *  List of relative paths to stat.
 */

char *Good_relative_paths[] = {
  "dev",
  "dir1/\\//file1\\\\//",
  "dir1/\\\\/file2",
  "dir1/file3/////\\\\\\",
  "dir1/file4",
  "dir1/dir1/file1",
  "dir1/dir1/ file1",
  "dir1",
  "dir2//////\\",
  "dir3",
  "dir4",
  "dir1/dir1",
  "dir1/ dir1///\\\\",
  "main.c",
  0
};

/*
 *  Do a stat/lstat on a single file and report the status.
 */
void stat_a_file_helper(
  const char *file,
  int         follow_link
)
{
  int         status;
  struct stat statbuf;
  int         major1;
  int         minor1;
  int         major2;
  int         minor2;


  rtems_test_assert( file );

  if ( follow_link ) {
    printf( "stat( %s ) returned ", file );
    fflush( stdout );
    status = stat( file, &statbuf );
  } else {
    printf( "lstat( %s ) returned ", file );
    fflush( stdout );
    status = lstat( file, &statbuf );
  }

  if ( status == -1 ) {
    printf( ": %s\n", strerror( errno ) );
  } else {

    rtems_filesystem_split_dev_t( statbuf.st_dev, major1, minor1 );
    rtems_filesystem_split_dev_t( statbuf.st_rdev, major2, minor2 );


    printf("\n...st_dev     (0x%x:0x%x)\n", major1, minor1 );
    printf(  "...st_ino     %" PRIxino_t "\n", statbuf.st_ino );
    printf(  "...st_mode    %" PRIomode_t "\n", statbuf.st_mode );
    printf(  "...st_nlink   %x\n", statbuf.st_nlink );
    printf(  "...st_uid     %d\n", statbuf.st_uid );
    printf(  "...st_gid     %d\n", statbuf.st_gid );
    printf(  "...st_rdev    (0x%x:0x%x)\n", major2, minor2 );
    printf(  "...st_size    %" PRIdoff_t "\n", statbuf.st_size );
    printf(  "...st_atime   %s", ctime( &statbuf.st_atime ) );
    printf(  "...st_mtime   %s", ctime( &statbuf.st_mtime ) );
    printf(  "...st_ctime   %s", ctime( &statbuf.st_ctime ) );
    printf(  "...st_blksize %" PRIxblksize_t "\n", statbuf.st_blksize );
    printf(  "...st_blocks  %" PRIxblkcnt_t "\n", statbuf.st_blocks );
  }
}

/*
 *  Do a stat on a single file and report the status.
*/
void stat_a_file(
  const char *file
)
{
  stat_a_file_helper( file, true );
}

/*
 *  Do a lstat on a single file and report the status.
 */
void lstat_a_file(
  const char *file
)
{
  stat_a_file_helper( file, false );
}

/*
 *  stat() multiple files at a time
 */
void stat_multiple_files(
  char **files
)
{
  int    i;

  i = 0;
  while ( files[i] ) {
    stat_a_file( files[i] );
    i++;
  }
}

/*
 *  chown()/lchown() multiple files at a time
 */
void chown_multiple_files_helper(
  char **files,
  int    follow_link
)
{
  int    i;
  uid_t  st_uid;
  gid_t  st_gid;

  st_uid = geteuid();
  st_gid = getegid();

  i = 0;
  while ( files[i] ) {
    printf("Change group of %s\n", files[i]);
    if ( follow_link ) {
      chown( files[i], st_uid, (st_gid+1) );
      stat_a_file( files[i] );
    } else {
      lchown( files[i], st_uid, (st_gid+1) );
      lstat_a_file( files[i] );
    }

    printf("Change owner of %s\n", files[i]);
    if ( follow_link ) {
      chown( files[i], (st_uid+1), st_gid );
      stat_a_file( files[i] );
    } else {
      lchown( files[i], (st_uid+1), st_gid );
      lstat_a_file( files[i] );
    }
    i++;
  }

}

/*
 *  chown() multiple files at a time
 */
void chown_multiple_files(
  char **files
)
{
  chown_multiple_files_helper( files, true );
}

/*
 *  lchown() multiple files at a time
 */
void lchown_multiple_files(
  char **files
)
{
  chown_multiple_files_helper( files, false );
}

/*
 *  mknod() multiple files at a time
 */
void make_multiple_files(
  char **files
)
{
  int    i;
  int    status;

  i = 0;
  while ( files[i] ) {
    printf( "Making file %s\n", files[i] );
    status = mknod( files[i], ( S_IFREG | S_IROTH|S_IWOTH ), 0LL );
    rtems_test_assert( !status );
    i++;
  }
  puts( "" );
}

void make_multiple_bad_files(
  char **files
)
{
  int    i;
  int    status;

  i = 0;
  while ( files[i] ) {
    printf( "Making file %s ", files[i] );
    status = mknod( files[i], ( S_IFREG | S_IROTH|S_IWOTH ), 0LL );
    rtems_test_assert( status );
    printf( ": %s\n", strerror( errno ) );
    i++;
  }
  puts( "" );
}

void make_multiple_links(
  char **existing,
  char **new
)
{
  int    i;
  int    status;

  i = 0;
  while ( new[i] && existing[i] ) {
    printf( "Making file %s\n", new[i] );
    status = link( existing[i], new[i] );
    rtems_test_assert( !status );
    i++;
  }
  puts( "" );

  status = link( "fred", "bob" );
  rtems_test_assert( status == -1 );

  status = link( existing[1], "doug/bob" );
  rtems_test_assert( status == -1 );
}

void make_too_many_links(void)
{
  int    i;
  int    status;
  char   name [20];

  status = mkdir("/dummy", S_IRWXU );
  rtems_test_assert( status == 0 );

  for (i=1; i<= LINK_MAX; i++) {

    sprintf(name,"/LinkName%d",i);
    printf( "Making file %s\n", name );
    status = link("/dummy" , name );
    if( i < LINK_MAX )
       rtems_test_assert( !status );
    else
       rtems_test_assert( status == -1 );

  }
}

void make_a_symlink(
  char *existing,
  char *new
)
{
  int    status;
  char   buf[100];
  int    len;

  memset( buf, 0, 100 );

  printf( "Making file %s\n", new );
  status = symlink( existing, new );
  rtems_test_assert( !status );

  printf( "Verify with readlink\n");
  status = readlink( new, buf, 100 );
  len = strlen( existing );
  rtems_test_assert ( status == len );

  status = readlink( new, buf, 3 );
  len = strlen( existing );
  if (len < 3 )
    rtems_test_assert( status == len );
  else
    rtems_test_assert( status == 3 );

  status = strcmp( existing, buf );
  rtems_test_assert( !status );
}

void make_multiple_symlinks(void)
{
 int  status, i;

 make_a_symlink( Files[0],             SymLinks[0] );
 make_a_symlink( Directories[0],       SymLinks[1] );
 make_a_symlink( Links_to_dirlinks[0], SymLinks[2] );
 make_a_symlink( "No_File",            SymLinks[3] );
 make_a_symlink( SymLinks[1],          SymLinks[4] );
 make_a_symlink( "//my_mount_point/links","/my_mount_point/symlinks/links" );

 for (i = 0; i < 5; i++) {
   stat_a_file( SymLinks[i] );
   lstat_a_file( SymLinks[i] );
 }

 status = symlink(  "//links", "bob/frank" );
 rtems_test_assert (status == -1);

}
/*
void make_too_many_symlinks()
{
  int  i, status;
  char name1[8];

  for (i=1; i <= MAXSYMLINK; i++) {
    sprintf( name1, "SymLink%d", i );
    status = symlink( "/dummy", name1 );
    if( i < MAXSYMLINK )
       rtems_test_assert( !status );
    else
       rtems_test_assert( status == -1 );
  }
}
*/

void make_many_symlinks(
  char  *real_file,
  int    link_count
)
{
  int  i;
  char name1[5];
  char name2[5];
  char *link_file;

  link_file = real_file;
  for (i=1; i < link_count; i++) {
    sprintf( name1, "%d", i );
    make_a_symlink( link_file, name1 );
    strcpy( name2, name1 );
    link_file = name2;
  }

  for (i=1; i < link_count; i++) {
    sprintf( name1, "%d", i );
    stat_a_file( name1 );
    lstat_a_file( name1 );
  }

}

/*
 *  mkdir() multiple directories at a time
 */
void make_multiple_directories(
  char **files
)
{
  int    i;
  int    status;

  i = 0;
  while ( files[i] ) {
    printf( "Making directory %s\n", files[i] );
    status = mkdir( files[i], S_IRWXU );
    rtems_test_assert( !status );
    i++;
  }
  puts( "" );
}

/*
 * Cause faults.
 */
void Cause_faults(void)
{
  int                                   fd;
  int                                   status;
  char                                  longer_name[100];

  /*
   * Verify chmod with an invalid type.
   */

#if 0
  printf("\n\nPass an invalid mode to chmod should fail with EPERM \n" );
  status = chmod( Files[0], S_IFREG );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EPERM );
#endif

  /*
   * Change file to executable then try to chdir to it.
   */

  status = chmod( Files[0], S_IXUSR );
  rtems_test_assert( status != -1 );

  printf("chdir to a file should fail with ENOTDIR\n");
  status = chdir( Files[0] );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOTDIR );

  /*
   * Change mode to read/write on a directory.
   * Verify directory works properly.
   */

  printf("Verify RWX permission on %s via access\n", Directories[0]);
  status = access( Directories[0], ( R_OK | W_OK | X_OK )  );
  rtems_test_assert( status == 0 );

  printf( "chmod of %s to Read/Write\n", Directories[0] );
  status = chmod( Directories[0], (S_IXGRP | S_IXOTH) );
  rtems_test_assert( status == 0 );

  printf( "chmod fred should fail with ENOENT\n" );
  status = chmod( "fred", (S_IXGRP | S_IXOTH) );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOENT );

  strcpy(longer_name, Directories[0] );
  strcat(longer_name, "/BADNAME" );
  printf( "Create under %s should fail with EACCES\n", Directories[0] );
  status = mkdir( longer_name , S_IRWXU );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EACCES );

  printf("chdir to %s should fail with EACCES\n", Directories[4] );
  status = chdir( Directories[4] );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EACCES );

  /*
   * Set current to a directory with no owner permissions.
   * Verify it works properly.
   */

  printf( "\n\nchmod of %s to Read/Write\n", Directories[0] );
  status = chmod( Directories[0], (S_IXGRP | S_IXOTH) );
  rtems_test_assert( status == 0 );

  printf("mkdir %s should fail with EACCESS\n", longer_name );
  status = mkdir( longer_name , S_IRWXU );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EACCES );

  printf("\n%s Should exist ( access )\n",Directories[0] );
  status = access( Directories[0], F_OK );
  rtems_test_assert( status == 0 );
  printf("\n%s Should have read  permission( access )\n",Directories[0] );
  status = access( Directories[0], R_OK );
  rtems_test_assert( status != 0 );
  printf("\n%s Should have write permission( access )\n",Directories[0] );
  status = access( Directories[0], W_OK );
  rtems_test_assert( status != 0 );
  printf("\n%s Should not have execute permission( access )\n",Directories[0] );
  status = access( Directories[0], X_OK );
  rtems_test_assert( status != 0 );

  printf("\nRestore %s to RWX\n",Directories[0] );
  status = chmod( Directories[0], S_IRWXU );
  rtems_test_assert( status == 0 );

  printf("chdir to /my_mount_point \n");
  status = chdir( "/my_mount_point" );
  rtems_test_assert( status == 0 );

  /*
   * Remove one of the directories.
   * Verify links to the removed directory still work.
   */

  printf( "Remove %s\n", Directories[5] );
  status = rmdir( Directories[5] );
  rtems_test_assert( status == 0 );

  stat_a_file( Directories[5] );
  status = access( Directories[5], F_OK );
  rtems_test_assert( status != 0 );

  stat_a_file( Links_to_Dirs[5] );
  status = readlink( Links_to_Dirs[5], longer_name, 3 );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EINVAL );

  stat_a_file( Links_to_dirlinks[5] );
  printf("Chdir to %s\n", Links_to_Dirs[5] );
  status = chdir( Links_to_Dirs[5] );
  rtems_test_assert( status == 0 );

  /*
   * Verify we cannot move up from a node with no parent node.
   */

  printf("Chdir to .. should fail with ENOENT\n" );
  status = chdir( ".." );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOENT );

  /*
   * Create a subdirectory under the dangling node.
   */

  printf("mkdir ../t should fail with ENOENT\n" );
  status = mkdir( "../t" , S_IRWXU );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOENT );

  printf("mkdir t\n");
  status = mkdir( "t" , S_IRWXU );
  rtems_test_assert( status == 0 );

  printf("chdir to /my_mount_point\n");
  status = chdir( "/my_mount_point" );
  rtems_test_assert( status == 0 );

  /*
   * Check rmdir, rmnod, and unlink
   */

  printf("rmdir %s\n", Links_to_Dirs[5] );
  status = rmdir( Links_to_Dirs[5] );
  rtems_test_assert( status == 0 );

  printf("unlink %s should fail with ENOTEMPTY\n", Links_to_dirlinks[5] );
  status = unlink(  Links_to_dirlinks[5] );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOTEMPTY );

  strcpy( longer_name,  Links_to_dirlinks[5] );
  strcat( longer_name, "/t");
  printf("rmdir %s\n", longer_name );
  status = rmdir( longer_name );
  rtems_test_assert( status == 0 );

  printf("unlink %s\n", Links_to_dirlinks[5]);
  status = unlink( Links_to_dirlinks[5] );
  rtems_test_assert( status == 0 );

  status = chdir( Directories[0] );
  status = mkdir ( "my_mount_point", S_IRWXU );
  rtems_test_assert( status == 0 );

  printf("Attempting to mount IMFS file system at /dir1/my_mount_point \n");
  status = mount(
    "null",
    "/my_mount_point/dir1/my_mount_point",
    "imfs",
     RTEMS_FILESYSTEM_READ_WRITE,
     NULL );
  rtems_test_assert( status == 0 );

  printf("rmdir /dir1/my_mount_point should fail with EBUSY\n");
  status = rmdir ("/my_mount_point/dir1/my_mount_point" );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EBUSY );

  printf( "Unmount /my_mount_point/dir1/my_mount_point\n");
  status = unmount( "/my_mount_point/dir1/my_mount_point" );
  rtems_test_assert( status == 0 );

  /*
   * Verify write permission is checked.
   */

  printf("chmod of %s to group and other execute\n", Files[0] );
  status = chmod (Files[0], (S_IXGRP | S_IXOTH) );
  rtems_test_assert( status == 0 );

  printf("Open %s for write should fail with EACCES\n", Files[0] );
  fd = open (Files[0], O_WRONLY);
  rtems_test_assert( fd == -1 );
  rtems_test_assert( errno == EACCES );

  printf("chmod of %s to User Execute and Read\n", Directories[3] );
  status = chmod (Directories[3], (S_IXUSR | S_IRUSR) );
  rtems_test_assert( status == 0 );
  strcpy(longer_name, Directories[3] );
  strcat(longer_name, "/NewFile" );
  printf("Mkdir of %s should fail with EACCES\n",longer_name );
  status = mkdir( longer_name, S_IRWXU );
  rtems_test_assert( status != 0 );
  rtems_test_assert( errno == EACCES );

  printf("Making too many hard links.\n" );
  make_too_many_links( );

  /*
   * The current directory MUST be restored at the end of this test.
   */

  printf("chdir to /my_mount_point \n");
  status = chdir( "/my_mount_point" );
  rtems_test_assert( status == 0 );

}

void Show_Time(void)
{
  rtems_status_code sc;
  rtems_time_of_day time;

  sc = rtems_clock_get_tod( &time );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  printf("--->Current Time: ");
  print_time( " - rtems_clock_get_tod - ", &time, "\n" );
}

void test_statvfs(void)
{

  int status = 0;
  struct statvfs stat;

  puts( "statvfs, with invalid path - expect ENOENT" );
  status = statvfs( "" , &stat );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOENT );

  puts( "create /tmp -- OK" );
  status = mkdir( "/tmp", 0777 );
  rtems_test_assert( status == 0 );

  puts( "statvfs, with valid path - expect ENOTSUP" );
  status = statvfs( "/tmp", &stat );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOTSUP );

  puts( "statvfs tested!" );
}

/*
 *  main entry point to the test
 */

#if defined(__rtems__)
int test_main(void)
#else
int main(
  int    argc,
  char **argv
)
#endif
{
  rtems_status_code sc;
  rtems_time_of_day time;
  int status;

  puts( "\n\n*** STAT TEST 01 ***" );

  build_time( &time, 12, 31, 1988, 9, 0, 0, 0 );
  sc = rtems_clock_set( &time );
  Show_Time();

  /*
   * Create and mount another version of the filesyste.
   * This allows expected node id's to be consistant across
   * platforms and bsp's.
   */

  status = mkdir("/my_mount_point",  S_IRWXU );
  rtems_test_assert( status == 0 );
  status = mount(
    "null",
    "my_mount_point",
    "imfs",
     RTEMS_FILESYSTEM_READ_WRITE,
     NULL );
  rtems_test_assert( status == 0 );
  status = chdir( "/my_mount_point" );
  rtems_test_assert( status == 0 );
  status = mkdir("dev",  S_IRWXU );
  rtems_test_assert( status == 0 );


  /*
   *  Create the files and directories for the test.
   */

  make_multiple_directories( Directories );
  make_multiple_files( Files );
  make_multiple_links( Directories,    Links_to_Dirs );
  make_multiple_links( Files,          Links_to_Files );

  sc = rtems_task_wake_after( TIMEOUT_VALUE );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  make_multiple_links( Links_to_Dirs,  Links_to_dirlinks );
  sc = rtems_task_wake_after( TIMEOUT_VALUE );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  make_multiple_links( Links_to_Files, Links_to_filelinks );

  sc = rtems_task_wake_after( TIMEOUT_VALUE );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  /*
   *  Now go through all the absolute path.
   */

  puts( "Doing the stat() on all the good absolute paths" );
  stat_multiple_files( Good_absolute_paths );

  /*
   *  run through the relative paths.
   */

  puts( "\nDoing the stat() on all the good relative paths" );
  stat_multiple_files( Good_relative_paths );

  /*
   * Change directory and releative paths are now bad.
   */

  puts("\nchdir to dev");
  chdir("dev");
  puts("\nstat relative paths that are now bad");
  stat_multiple_files( Good_relative_paths );

  /*
   * Change directory to the link directory and follow links.
   */

  puts("\nchdir to ../links");
  chdir("../links");
  puts("Doing the stat() on good links\n");
  stat_multiple_files( Links_to_Dirs );
  stat_multiple_files( Links_to_Files );
  stat_multiple_files( Links_to_dirlinks  );
  stat_multiple_files( Links_to_filelinks );

  /*
   * Chmod on dir1/dir1.  This allows the error path to be hit.
   */

  printf( "chmod of %s to Read/Write\n", Directories[4] );
  chmod( Directories[4], (S_IROTH|S_IWOTH) );
  puts( "\nDoing the stat() on all the bad paths" );

  stat_multiple_files( Bad_paths );
  make_multiple_bad_files( Bad_paths );

  printf( "Return %s to RWX\n", Directories[4] );
  chmod( Directories[4], S_IRWXU );


  /*
   * Check out symbolic links.
   */

  make_multiple_symlinks();
  make_many_symlinks( "/symlinks", 10 );

  sc = rtems_task_wake_after( TIMEOUT_VALUE );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  Cause_faults();

  sc = rtems_task_wake_after( TIMEOUT_VALUE );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  chown_multiple_files( Files );

  sc = rtems_task_wake_after( TIMEOUT_VALUE );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  chown_multiple_files( Links_to_Dirs );

  sc = rtems_task_wake_after( TIMEOUT_VALUE );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  lchown_multiple_files( SymLinks );

  test_statvfs();

  puts( "\n\n*** END OF STAT TEST 01 ***" );
  rtems_test_exit(0);
}
