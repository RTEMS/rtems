/*
 * Copyright (c) 2012, 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "tmacros.h"

#include <fcntl.h>
#include <dirent.h>

#include <bsp.h>
#include <rtems/io.h>
#include <rtems/libio.h>
#include <rtems/dosfs.h>
#include <rtems/ramdisk.h>
#include <rtems/libcsupport.h>
#include "ramdisk_support.h"

#define MOUNT_DIR "/mnt"
#define MOUNT_DIR_SIZE 4
#define START_DIR_SIZE 4

#define NUMBER_OF_DIRECTORIES 8
#define NUMBER_OF_FILES 13
#define NUMBER_OF_DIRECTORIES_INVALID 18
#define NUMBER_OF_DIRECTORIES_DUPLICATED 2
#define NUMBER_OF_FILES_DUPLICATED 2
#define MAX_NAME_LENGTH ( 255 + 1 )
#define MAX_NAME_LENGTH_INVALID ( 255 + 2 )
#define MAX_DUPLICATES_PER_NAME 3

#define BLOCK_SIZE 512

#define BLOCK_COUNT ( sizeof( image_bin ) / BLOCK_SIZE )

static rtems_resource_snapshot            before_mount;

static const msdos_format_request_param_t rqdata = {
  .OEMName             = "RTEMS",
  .VolLabel            = "RTEMSDisk",
  .sectors_per_cluster = 2,
  .fat_num             = 0,
  .files_per_root_dir  = 0,
  .media               = 0,
  .quick_format        = true,
  .skip_alignment      = 0,
  .info_level          = 0
};

static const char                         DIRECTORY_NAMES[NUMBER_OF_DIRECTORIES]
[MAX_NAME_LENGTH] = {
  "a dir",
  "Shortdir",
  "shrtdir",
  "shrt.dir",
  "long_conventional_dir",
  "long_conventional.dir",
  "LongConventionalDir",
  "This is a directory name with with 255 characters. The following numbers are aligned in that way, that the character 0 is the mentioned one. xxxxxx150xxxxxxx160xxxxxxx170xxxxxxx180xxxxxxx190xxxxxxx200xxxxxxx210xxxxxxx220xxxxxxx230xxxxxxx240xxxxxxx250xxxxx"
};

static const char DIRECTORY_NAMES_INVALID[
  NUMBER_OF_DIRECTORIES_INVALID][MAX_NAME_LENGTH_INVALID] = {
  "This is a directory name with with 256 characters. The following numbers are aligned in that way, that the character 0 is the mentioned one. xxxxxx150xxxxxxx160xxxxxxx170xxxxxxx180xxxxxxx190xxxxxxx200xxxxxxx210xxxxxxx220xxxxxxx230xxxxxxx240xxxxxxx250xxxxxx",
  ".",
  "..",
  "...",
  " ",
  "... ...",
  " ... ",
  "",
  "*",
  "/",
  ":",
  "<",
  ">",
  "?",
  "\\",
  "|",
  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 17, 28, 29, 30, 31},
  {127}
};

static const char FILE_NAMES[NUMBER_OF_FILES][
  MAX_NAME_LENGTH] = {
  "a file",
  "shrtfile",
  "ShrtFle",
  "The quick brown.fox",
  "long_conventional_file",
  "This is a filename with with 255 characters. The following numbers are aligned in that way, that the character 0 is the mentioned one. xx140xxxxxxx150xxxxxxx160xxxxxxx170xxxxxxx180xxxxxxx190xxxxxxx200xxxxxxx210xxxxxxx220xxxxxxx230xxxxxxx240xxxxxxx250xxxxx",
  "+",
  ",",
  "a.a",
  ";",
  "=",
  "[",
  "]"
};

typedef struct {
  char name[MAX_NAME_LENGTH];
  unsigned int number_of_duplicates;
  char name_duplicates[MAX_DUPLICATES_PER_NAME][MAX_NAME_LENGTH];
} name_duplicates;

static const name_duplicates DIRECTORY_DUPLICATES[
  NUMBER_OF_DIRECTORIES_DUPLICATED] = {
  {
    "shrtdir",
    3,
    {
      "shrtdir",
      "SHRTDIR",
      "Shrtdir"
    }
  },
  {
    "long_conventional_dir",
    3,
    {
      "long_conventional_dir",
      "LONG_CONVENTIONAL_DIR",
      "Long_conventional_dir"
    }
  }
};

static const name_duplicates MULTIBYTE_DUPLICATES[
  NUMBER_OF_MULTIBYTE_NAMES_DUPLICATED] = {
  {
    /* The angstroem encoded differently. These encodings might become short entries */
    {0xc3, 0x85}, /* '̊A' */
    2,
    {
      {0xc3, 0x85}, /* '̊A' */
      {0xe2, 0x84, 0xab} /* 'Å' */
    }
  },

  /* Again the angstroem encoded differently,
   * but this time with additional characters in order to enforce a long entry. */
  {
    {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', 0xc3,
     0x85},
    2,
    {
      {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', 0xc3,
       0x85},
      {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', 0xe2,
       0x84, 0xab}
    }
  }
};

static const name_duplicates FILES_DUPLICATES[NUMBER_OF_FILES_DUPLICATED] = {
  {
    "shrtfile",
    3,
    {
      "shrtfile",
      "SHRTFILE",
      "Shrtfile"
    }
  },
  {
    "long_conventional_file",
    3,
    {
      "long_conventional_file",
      "LONG_CONVENTIONAL_FILE",
      "Long_conventional_file"
    }
  }
};

static int path_is_directory( const char *path )
{
  struct stat s_buf;


  if ( stat( path, &s_buf ) )
    return 0;

  return S_ISDIR( s_buf.st_mode );
}

static void delete_folder_tree( const char *directory_name )
{
  DIR           *dp;
  struct dirent *ep;
  char           p_buf[1024] = {0};
  int            rc          = 0;


  dp = opendir( directory_name );
  rtems_test_assert( dp != NULL );

  while ( ( ep = readdir( dp ) ) != NULL && rc == 0 ) {
    if ( 0 != strcmp( ".", ep->d_name )
         && 0 != strcmp( "..", ep->d_name ) ) {
      snprintf( p_buf, sizeof( p_buf ), "%s/%s", directory_name, ep->d_name );

      if ( path_is_directory( p_buf ) ) {
        delete_folder_tree( p_buf );
        rc = rmdir( p_buf );
        rtems_test_assert( rc == 0 );
        rewinddir( dp );
      } else {
        rc = unlink( p_buf );
        rtems_test_assert( rc == 0 );
        rewinddir( dp );
      }
    }
  }

  rc = closedir( dp );
  rtems_test_assert( rc == 0 );
}

static void mount_device( const char *start_dir,
  const rtems_dosfs_mount_options    *mount_opts )
{
  int rc;

  rc = mount(
    RAMDISK_PATH,
    MOUNT_DIR,
    "dosfs",
    RTEMS_FILESYSTEM_READ_WRITE,
    mount_opts );
  rtems_test_assert( rc == 0 );

  rc = mkdir( start_dir, S_IRWXU | S_IRWXG | S_IRWXO );
  rtems_test_assert( rc == 0 || ( rc == -1 && errno == EEXIST ) );
}

static void mount_device_with_defaults( const char *start_dir )
{
  int rc;


  rc = msdos_format( RAMDISK_PATH, &rqdata );
  rtems_test_assert( rc == 0 );

  rtems_resource_snapshot_take( &before_mount );

  mount_device( start_dir, NULL );
}

static void unmount_and_close_device( void )
{
  int                     rc;
  rtems_resource_snapshot now;
  bool                    are_resources_freed;


  delete_folder_tree( MOUNT_DIR );

  rc = unmount( MOUNT_DIR );
  rtems_test_assert( rc == 0 );

  are_resources_freed = rtems_resource_snapshot_check( &before_mount );

  if ( !are_resources_freed )
    rtems_resource_snapshot_take( &now );

  rtems_test_assert( are_resources_freed );
}

/*
 * Simply create a few directories. These tests should all succeed
 */
static void test_creating_directories(
  const char        *start_dir,
  const char        *directories,
  const unsigned int number_of_directories )
{
  unsigned int   index;
  int            rc;
  char           dirname[MAX_NAME_LENGTH + strlen( start_dir ) + 1];
  DIR           *dirp;
  struct dirent *dp;


  for ( index = 0; index < number_of_directories; ++index ) {
    snprintf( dirname, sizeof( dirname ), "%s/%s", start_dir, directories
              + ( index * MAX_NAME_LENGTH ) );
    rc = mkdir( dirname, S_IRWXU | S_IRWXG | S_IRWXO );
    rtems_test_assert( rc == 0 );
  }

  dirp = opendir( start_dir );
  rtems_test_assert( NULL != dirp );

  index = 0;
  dp    = readdir( dirp );
  rtems_test_assert( dp != NULL );
  rtems_test_assert( 0 == strcmp( ".", dp->d_name ) );

  dp = readdir( dirp );
  rtems_test_assert( dp != NULL );
  rtems_test_assert( 0 == strcmp( "..", dp->d_name ) );

  dp = readdir( dirp );
  rtems_test_assert( dp != NULL );

  while ( dp != NULL ) {
    rtems_test_assert( 0
                       == strcmp( directories + ( index * MAX_NAME_LENGTH ),
                                  dp->d_name ) );
    ++index;
    dp = readdir( dirp );
  }

  rtems_test_assert( number_of_directories == index );

  rc = closedir( dirp );
  rtems_test_assert( rc == 0 );
}

/*
 * Try creating directories with invalid names.
 */
static void test_creating_invalid_directories( void )
{
  unsigned int index;
  int          rc;
  char         dirname[MAX_NAME_LENGTH_INVALID + MOUNT_DIR_SIZE + 1];


  for ( index = 0; index < NUMBER_OF_DIRECTORIES_INVALID; ++index ) {
    snprintf( dirname,
              sizeof( dirname ),
              "%s/%s",
              MOUNT_DIR,
              DIRECTORY_NAMES_INVALID[index] );
    rc = mkdir( dirname, S_IRWXU | S_IRWXG | S_IRWXO );
    rtems_test_assert( rc == -1 );
  }
}

/*
 * Try creating directories which do already exist
 * (although names may have different capitalization/encoding)
 */
static void test_creating_duplicate_directories(
  const char            *start_dir,
  const name_duplicates *duplicates,
  const unsigned int     number_of_duplicates )
{
  unsigned int index_dir;
  unsigned int index_duplicate;
  int          rc;
  char         dirname[MAX_NAME_LENGTH + MOUNT_DIR_SIZE + START_DIR_SIZE + 2];


  for ( index_dir = 0; index_dir < number_of_duplicates; ++index_dir ) {
    snprintf( dirname, sizeof( dirname ), "%s/%s", start_dir,
              duplicates[index_dir].name );
    rc = mkdir( dirname, S_IRWXU | S_IRWXG | S_IRWXO );
    rtems_test_assert( rc == 0 );

    for ( index_duplicate = 0;
          index_duplicate < duplicates[index_dir].number_of_duplicates;
          ++index_duplicate ) {
      snprintf( dirname, sizeof( dirname ), "%s/%s", start_dir,
                duplicates[index_dir].name_duplicates[index_duplicate] );
      rc = mkdir( dirname, S_IRWXU | S_IRWXG | S_IRWXO );
      rtems_test_assert( rc < 0 );
    }
  }
}

/*
 * Try creating and opening files with valid names
 */
static void test_handling_files(
  const char        *dirname,
  const char        *file_names,
  const unsigned int number_of_files )
{
  unsigned int index;
  int          rc;
  char         filename[MAX_NAME_LENGTH * 2 + MOUNT_DIR_SIZE + START_DIR_SIZE
                        + 4];
  int          fd;


  for ( index = 0; index < number_of_files; ++index ) {
    snprintf(
      filename,
      sizeof( filename ) - 1,
      "%s/%s",
      dirname,
      file_names + index * MAX_NAME_LENGTH );
    fd = open( filename,
               O_RDWR | O_CREAT,
               S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH );
    rtems_test_assert( fd >= 0 );

    rc = close( fd );
    rtems_test_assert( rc == 0 );

    /* See if the file still exists and can be found */
    fd = open( filename, O_RDWR );
    rtems_test_assert( fd >= 0 );

    rc = close( fd );
    rtems_test_assert( rc == 0 );
  }
}

/*
 * Try opening files which do already exist (with different capitalization in their names)
 */
static void test_duplicated_files( const char *dirname,
  const name_duplicates                       *files_duplicated,
  const unsigned int                           number_of_files_duplicated )
{
  unsigned int index_file;
  unsigned int index_duplicate;
  int          rc;
  char         filename[MAX_NAME_LENGTH + strlen( dirname ) + 1];
  int          fd;


  for ( index_file = 0; index_file < number_of_files_duplicated;
        ++index_file ) {
    snprintf( filename,
              sizeof( filename ) - 1,
              "%s/%s",
              dirname,
              files_duplicated[index_file].name );
    fd = open( filename,
               O_RDWR | O_CREAT,
               S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH );
    rtems_test_assert( fd >= 0 );

    rc = close( fd );
    rtems_test_assert( rc == 0 );

    for ( index_duplicate = 0;
          index_duplicate < files_duplicated[index_file].number_of_duplicates;
          ++index_duplicate ) {
      snprintf( filename,
                sizeof( filename ) - 1,
                "%s/%s",
                dirname,
                files_duplicated[index_file].name_duplicates[index_duplicate] );
      fd = open( filename, O_RDWR );
      rtems_test_assert( fd >= 0 );

      rc = close( fd );
      rtems_test_assert( rc == 0 );
    }

    rc = remove( filename );
    rtems_test_assert( rc == 0 );
  }
}

/*
 * Open and read existing valid directories
 */
static void test_handling_directories(
  const char        *start_dir,
  const char        *directory_names,
  const unsigned int number_of_directories,
  const char        *file_names,
  const unsigned int number_of_files )
{
  unsigned int   index_directory;
  unsigned int   index_file;
  int            rc;
  DIR           *dir_stream;
  char           dirname[MAX_NAME_LENGTH * 2];
  struct dirent *dp;


  for ( index_directory = 0;
        index_directory < number_of_directories;
        ++index_directory ) {
    snprintf(
      dirname,
      sizeof( dirname ) - 1,
      "%s/%s",
      start_dir,
      directory_names + index_directory * MAX_NAME_LENGTH );

    test_handling_files(
      dirname,
      file_names,
      number_of_files );

    dir_stream = opendir( dirname );
    rtems_test_assert( dir_stream != NULL );

    dp = readdir( dir_stream );
    rtems_test_assert( dp != NULL );
    rtems_test_assert( 0 == strcmp( ".", dp->d_name ) );

    dp = readdir( dir_stream );
    rtems_test_assert( dp != NULL );
    rtems_test_assert( 0 == strcmp( "..", dp->d_name ) );

    dp         = readdir( dir_stream );
    rtems_test_assert( dp != NULL );
    index_file = 0;

    while ( dp != NULL ) {
      rtems_test_assert( 0 == strcmp(
                           file_names + index_file * MAX_NAME_LENGTH,
                           dp->d_name ) );
      ++index_file;
      dp = readdir( dir_stream );
    }

    rtems_test_assert( number_of_files == index_file );

    rc = closedir( dir_stream );
    rtems_test_assert( rc == 0 );
  }
}

/*
 * Main test method
 */
static void test( void )
{
  int  rc;
  char start_dir[MOUNT_DIR_SIZE + START_DIR_SIZE + 2];


  rc = mkdir( MOUNT_DIR, S_IRWXU | S_IRWXG | S_IRWXO );
  rtems_test_assert( rc == 0 );

  init_ramdisk();

  snprintf( start_dir, sizeof( start_dir ), "%s/%s", MOUNT_DIR, "strt" );

  /*
   * Tests with code page 850 compatible directory and file names
   * and the code page 850 backwards compatible default mode mode of the
   * FAT file system
   */
  mount_device_with_defaults( start_dir );

  test_creating_duplicate_directories(
    &start_dir[0],
    &DIRECTORY_DUPLICATES[0],
    NUMBER_OF_DIRECTORIES_DUPLICATED );

  unmount_and_close_device();

  mount_device_with_defaults( start_dir );

  test_duplicated_files(
    MOUNT_DIR,
    FILES_DUPLICATES,
    NUMBER_OF_FILES_DUPLICATED );

  unmount_and_close_device();

  mount_device_with_defaults( start_dir );

  test_creating_invalid_directories();

  test_creating_directories(
    &start_dir[0],
    &DIRECTORY_NAMES[0][0],
    NUMBER_OF_DIRECTORIES );

  test_handling_directories(
    &start_dir[0],
    &DIRECTORY_NAMES[0][0],
    NUMBER_OF_DIRECTORIES,
    &FILE_NAMES[0][0],
    NUMBER_OF_FILES );

  unmount_and_close_device();

  del_ramdisk();
}

static void Init( rtems_task_argument arg )
{
  puts( "\n\n*** TEST fsdosfsname01 ***" );

  test();

  puts( "*** END OF TEST fsdosfsname01 ***" );

  rtems_test_exit( 0 );
}

#define CONFIGURE_INIT_TASK_STACK_SIZE ( 1024 * 64 )
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_MAXIMUM_DRIVERS 4
#define CONFIGURE_MAXIMUM_SEMAPHORES RTEMS_DOSFS_SEMAPHORES_PER_INSTANCE

#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_FILESYSTEM_DOSFS

/* 1 RAM disk device file + 1 mount_dir + stdin + stdout + stderr + 2 for open directories/files */
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS ( 5 + 2 )

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
