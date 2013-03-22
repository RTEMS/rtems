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
#include "image.h"
#include "image_bin_le_singlebyte.h"
#include "image_bin_le_multibyte.h"
#include "files.h"

#include <errno.h>

#define PRINT_DISK_IMAGE 0

#define MOUNT_DIR "/mnt"
#define MOUNT_DIR_SIZE 4
#define START_DIR_SIZE 4
#define RAMDISK_PATH "/dev/rda"
#define BLOCK_NUM 47
#define BLOCK_SIZE 512

#define NUMBER_OF_DIRECTORIES 8
#define NUMBER_OF_FILES 13
#define NUMBER_OF_DIRECTORIES_INVALID 25
#define NUMBER_OF_DIRECTORIES_DUPLICATED 2
#define NUMBER_OF_MULTIBYTE_NAMES_DUPLICATED 2
#define NUMBER_OF_FILES_DUPLICATED 2
#define NUMBER_OF_NAMES_MULTIBYTE 10
#define MAX_NAME_LENGTH ( 255 + 1 )
#define MAX_NAME_LENGTH_INVALID ( 255 + 2 )
#define MAX_DUPLICATES_PER_NAME 3
static const char UTF8_BOM[] = {0xEF, 0xBB, 0xBF};
#define UTF8_BOM_SIZE 3 /* Size of the UTF-8 byte-order-mark */

#define BLOCK_SIZE 512

#define BLOCK_COUNT ( sizeof( image_bin ) / BLOCK_SIZE )

static ramdisk                            disk_image = {
  .block_size             = BLOCK_SIZE,
  .block_num              = BLOCK_COUNT,
  .area                   = &image_bin[0],
  .initialized            = true,
  .malloced               = false,
  .trace                  = false,
  .free_at_delete_request = false
};

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
  {127},
  "э*_то длинное имя",
  "э:_то длинное имя",
  "э<_то длинное имя",
  "э>_то длинное имя",
  "э?_то длинное имя",
  "э|_то длинное имя"
};

static const char NAMES_MULTIBYTE[
  NUMBER_OF_NAMES_MULTIBYTE][MAX_NAME_LENGTH] = {
  "đây là một tên tập tin dài",
  "Bu uzun bir dosya adı",
  "هذا هو اسم ملف طويل",
  "αυτό είναι ένα μεγάλο όνομα αρχείου",
  "это длинное имя",
  "гэта доўгае імя",
  "това е дълго име на файла",
  "这是一个长文件名",
  "shrtname",
  "long_conventional_name"
};

static const char NAMES_MULTIBYTE_IN_CODEPAGE_FORMAT[
  NUMBER_OF_NAMES_MULTIBYTE][MAX_NAME_LENGTH] = {
  "_\2030005~1._t",
  "bu0008~1.bir",
  "__000b~1.__",
  "__000f~1.__",
  "__0012~1.___",
  "__0015~1.___",
  "__0018~1.___",
  "__001a~1",
  "shrtname",
  "long_conventional_name"
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

static void mount_device_with_iconv(
  const char                *start_dir,
  rtems_dosfs_mount_options *mount_opts
)
{
  int                       rc;

  rc = msdos_format( RAMDISK_PATH, &rqdata );
  rtems_test_assert( rc == 0 );

  rtems_resource_snapshot_take( &before_mount );

  mount_opts->converter = rtems_dosfs_create_utf8_converter( "CP850" );
  rtems_test_assert( mount_opts->converter != NULL );

  mount_device( start_dir, mount_opts );
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
 * Try to find (and open) all of the file names from an
 * array in a given directory
 */
static void test_finding_files(
  const char        *dirname,
  const char        *file_names,
  const unsigned int number_of_files )
{
  int            rc;
  DIR           *dir_stream;
  struct dirent *dp;
  int            fd;
  unsigned int   index_file;
  char           filename[MAX_NAME_LENGTH * 2 + MOUNT_DIR_SIZE
                          + START_DIR_SIZE + 4];


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

    snprintf(
      filename,
      sizeof( filename ) - 1,
      "%s/%s",
      dirname,
      file_names + index_file * MAX_NAME_LENGTH );

    /* See if the file still exists and can be found */
    fd = open( filename, O_RDWR );
    rtems_test_assert( fd >= 0 );

    rc = close( fd );
    rtems_test_assert( rc == 0 );

    ++index_file;
    dp = readdir( dir_stream );
  }

  rtems_test_assert( number_of_files == index_file );

  rc = closedir( dir_stream );
  rtems_test_assert( rc == 0 );
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
 * Try to find all sub-directories from an array
 * in a given start directory.
 * In addition try to find and open files
 * in these sub-directories.
 */
static void test_finding_directories(
  const char        *start_dir,
  const char        *directory_names,
  const unsigned int number_of_directories,
  const char        *file_names,
  const unsigned int number_of_files )
{
  unsigned int   index_directory;
  int            rc;
  DIR           *dir_stream;
  struct dirent *dp;
  char           dirname[MAX_NAME_LENGTH * 2];


  dir_stream = opendir( start_dir );
  rtems_test_assert( dir_stream != NULL );

  dp = readdir( dir_stream );
  rtems_test_assert( dp != NULL );
  rtems_test_assert( 0 == strcmp( ".", dp->d_name ) );

  dp = readdir( dir_stream );
  rtems_test_assert( dp != NULL );
  rtems_test_assert( 0 == strcmp( "..", dp->d_name ) );

  dp              = readdir( dir_stream );
  rtems_test_assert( dp != NULL );
  index_directory = 0;

  while ( dp != NULL ) {
    rtems_test_assert( 0 == strcmp(
                         directory_names + index_directory * MAX_NAME_LENGTH,
                         dp->d_name ) );

    snprintf(
      dirname,
      sizeof( dirname ) - 1,
      "%s/%s",
      start_dir,
      directory_names + index_directory * MAX_NAME_LENGTH );

    test_finding_files(
      dirname,
      file_names,
      number_of_files );

    ++index_directory;
    dp = readdir( dir_stream );
  }

  rtems_test_assert( number_of_directories == index_directory );

  rc = closedir( dir_stream );
  rtems_test_assert( rc == 0 );
}

#if (PRINT_DISK_IMAGE != 0)
static void print_image(
  const char* include_guard,
  const char* image_name
)
{
  #define                   BYTES_PER_ROW     8
  int                       rc;
  int                       fd;
  ssize_t                   bytes_read;
  uint8_t                   buf[BLOCK_SIZE];
  unsigned int              index_block;
  unsigned int              index_row;
  unsigned int              index_column;
  unsigned int              index_buf;
#ifdef SHOW_LINE_NUMBERS
  size_t                    index_row_start = 1;
#endif /* SWOW_LINE_NUMBERS */
  size_t                    bytes_written   = 0;
  const size_t              DISK_SIZE       = BLOCK_SIZE * BLOCK_NUM;
  const unsigned int        ROWS_PER_BLOCK  = BLOCK_SIZE / BYTES_PER_ROW;

  printf ("/*\n"
          " *  Declarations for C structure representing a disk image\n"
          " *\n"
          " *  WARNING: Automatically generated by init.c -- do not edit!\n"
          " */\n"
          "#ifndef %s\n"
          "#define %s\n"
          "\n"
          "#include <sys/types.h>\n"
          "\n"
          "#ifdef __cplusplus\n"
          "extern \"C\" {\n"
          "#endif /* __cplusplus */\n"
          "\n"
          "static unsigned char %s[] = {\n",
          include_guard,
          include_guard,
          image_name);
  fd = open( RAMDISK_PATH, O_RDWR );
  rtems_test_assert( fd >= 0 );

  for ( index_block = 0; index_block < BLOCK_NUM; ++index_block )
  {
    bytes_read = read( fd, &buf[0], BLOCK_SIZE );
    rtems_test_assert( bytes_read = BLOCK_SIZE );

    index_buf = 0;

    for ( index_row = 0; index_row < ROWS_PER_BLOCK; ++index_row )
    {
      printf( "  " );
      for ( index_column = 0;
            index_column < BYTES_PER_ROW;
            ++index_column ) {
        printf("0x%02x", buf[index_buf]);
        ++bytes_written;
        ++index_buf;
        if ( bytes_written < DISK_SIZE ) {
          printf (", ");
        } else {
          printf ("  ");
        }
      }
#ifdef SHOW_LINE_NUMBERS
      printf( "/* %6u - %6u */", index_row_start, bytes_written );
      index_row_start = bytes_written + 1;
#endif /* SWOW_LINE_NUMBERS */
      printf( "\n" );
    }
  }

  rc = close( fd );
  rtems_test_assert( rc == 0 );

  printf ("};\n"
          "#ifdef __cplusplus\n"
          "}\n"
          "#endif /* __cplusplus */\n"
          "\n"
          "#endif /* %s */\n",
          include_guard);
}
#else /* PRINT_DISK_IMAGE */
static void print_image(
  const char* include_guard,
  const char* image_name
)
{
  /* Nothing to be done */
}
#endif /* PRINT_DISK_IMAGE */


static void compare_files(
  const char *file0,
  const char *file1
)
{
  struct stat  stat_buf[2];
  int          fd[2];
  unsigned int index;
  uint8_t      buf[2];
  ssize_t      bytes_read;
  int          rc;

  rc = stat( file0 , &stat_buf[0] );
  rtems_test_assert( rc == 0 );
  rc = stat( file1 , &stat_buf[1] );
  rtems_test_assert( rc == 0 );

  rtems_test_assert( stat_buf[0].st_size == stat_buf[1].st_size );

  fd[0] = open( file0, O_RDONLY );
  rtems_test_assert( fd[0] > 0 );
  fd[1] = open( file1, O_RDONLY );
  rtems_test_assert( fd[1] > 0 );

  for ( index = 0; index < stat_buf[0].st_size; ++index ) {
    bytes_read = read( fd[0], &buf[0], 1 );
    rtems_test_assert( bytes_read == 1 );
    bytes_read = read( fd[1], &buf[1], 1 );
    rtems_test_assert( bytes_read == 1 );
    rtems_test_assert( buf[0] == buf[1] );
  }
  rc = close( fd[0] );
  rtems_test_assert( rc == 0 );
  rc = close( fd[1] );
  rtems_test_assert( rc == 0 );
}

static void compare_directories(
  const char *dir0,
  const char *dir1)
{
  int                       rc;
  DIR                      *dir_stream[2];
  struct dirent            *dp;
  struct stat               stat_buf[2];
  char                     *path[2];
  const unsigned int        PATH_LENGTH = 1024;

  path[0] = calloc( PATH_LENGTH, sizeof(char) );
  rtems_test_assert( path[0] != NULL );
  path[1] = calloc( PATH_LENGTH, sizeof(char) );
  rtems_test_assert( path[1] != NULL );

  dir_stream[0] = opendir( dir0 );
  rtems_test_assert( dir_stream[0] != NULL );

  dir_stream[1] = opendir( dir1 );
  rtems_test_assert( dir_stream[1] != NULL );

  dp = readdir( dir_stream[0] );
  rtems_test_assert( dp != NULL );

  while ( dp != NULL ) {
    rc = snprintf(path[0], PATH_LENGTH, "%s/%s", dir0, dp->d_name);
    rtems_test_assert( rc < PATH_LENGTH );
    rtems_test_assert( rc >= 0 );
    rc = snprintf(path[1], PATH_LENGTH, "%s/%s", dir1, dp->d_name);
    rtems_test_assert( rc < PATH_LENGTH );
    rtems_test_assert( rc >= 0 );

    rc = stat( path[0] , &stat_buf[0] );
    rtems_test_assert( rc == 0 );

    if (   ( strcmp( dp->d_name, "."  ) != 0)
        && ( strcmp( dp->d_name, ".." ) != 0) ) {
      if ( S_ISDIR( stat_buf[0].st_mode ) ) {
        compare_directories( path[0], path[1] );
      } else {
        compare_files( path[0], path[1] );
      }
    }

    dp = readdir( dir_stream[0] );

  }
  rc = closedir( dir_stream[0] );
  rtems_test_assert( rc == 0 );

  rc = closedir( dir_stream[1] );
  rtems_test_assert( rc == 0 );

  free (path[0]);
  free (path[1]);
}

static void compare_image(
  const char                      *mount_dir,
  const char                      *dev,
  const rtems_dosfs_mount_options *mount_opts )
{
  #define MOUNT_DIR2 "/mnt2"
  int                       rc;

  rc = mount_and_make_target_path(
    dev,
    MOUNT_DIR2,
    RTEMS_FILESYSTEM_TYPE_DOSFS,
    RTEMS_FILESYSTEM_READ_WRITE,
    mount_opts );
  rtems_test_assert( rc == 0 );

  compare_directories(mount_dir, MOUNT_DIR2);

  rc = unmount( MOUNT_DIR2 );
  rtems_test_assert( rc == 0 );

  rc = rmdir( MOUNT_DIR2 );
  rtems_test_assert( rc == 0 );

}
/*
 * Test the compatibility with a genuine MS Windows FAT file system.
 */
static void test_compatibility( void )
{
  int                       rc;
  rtems_status_code         sc;
  dev_t                     dev;
  char                      diskpath[] = "/dev/ramdisk1";
  rtems_dosfs_mount_options mount_opts;
  rtems_device_major_number major;
  FILE                     *fp;
  int                       buffer;
  unsigned int              index_file = 0;
  unsigned int              index_char;
  unsigned int              offset;
  char                      content_buf[MAX_NAME_LENGTH + strlen( MOUNT_DIR )
                                        + 1];
  char                      file_path[MAX_NAME_LENGTH + strlen( MOUNT_DIR )
                                      + 1];
  DIR                      *dir_stream;
  struct dirent            *dp;


  mount_opts.converter = rtems_dosfs_create_utf8_converter( "CP850" );
  rtems_test_assert( mount_opts.converter != NULL );

  sc = rtems_io_register_driver( 0, &ramdisk_ops, &major );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  dev = rtems_filesystem_make_dev_t( major, 1 );

  sc  = rtems_disk_create_phys(
    dev,
    BLOCK_SIZE,
    BLOCK_COUNT,
    ramdisk_ioctl,
    &disk_image,
    diskpath );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  rc = mount_and_make_target_path(
    diskpath,
    MOUNT_DIR,
    RTEMS_FILESYSTEM_TYPE_DOSFS,
    RTEMS_FILESYSTEM_READ_WRITE,
    &mount_opts );
  rtems_test_assert( rc == 0 );

  dir_stream = opendir( MOUNT_DIR );
  rtems_test_assert( dir_stream != NULL );

  dp = readdir( dir_stream );
  rtems_test_assert( dp != NULL );

  while ( dp != NULL ) {
    index_char = 0;

    size_t len = strlen( filenames[index_file] );

    if ( filenames[index_file][len - 1] == '.' )
      rtems_test_assert( ( len - 1 ) == dp->d_namlen );
    else
      rtems_test_assert( len == dp->d_namlen );

    rtems_test_assert( 0
                       == memcmp( &filenames[index_file][0], &dp->d_name[0],
                                  dp->d_namlen ) );

    snprintf( file_path, sizeof( file_path ), "%s/%s", MOUNT_DIR,
              filenames[index_file] );
    fp = fopen( file_path, "r" );
    rtems_test_assert( fp != NULL );

    /* These files should contain their own file names. */
    while ( ( buffer = fgetc( fp ) ) != EOF ) {
      content_buf[index_char] = buffer;
      ++index_char;
    }

    if ( 0 == strncmp( content_buf, UTF8_BOM, UTF8_BOM_SIZE ) )
      offset = UTF8_BOM_SIZE;
    else
      offset = 0;

    rtems_test_assert( 0
                       == memcmp( filenames[index_file],
                                  &content_buf[offset],
                                  index_char - offset ) );

    rc = fclose( fp );
    rtems_test_assert( rc == 0 );

    ++index_file;
    dp = readdir( dir_stream );
  }

  rtems_test_assert( index_file == FILES_FILENAMES_NUMBER_OF );

  rc = closedir( dir_stream );
  rtems_test_assert( rc == 0 );

  rc = unmount( MOUNT_DIR );
  rtems_test_assert( rc == 0 );
}

/*
 * Main test method
 */
static void test( void )
{
  int  rc;
  char start_dir[MOUNT_DIR_SIZE + START_DIR_SIZE + 2];
  rtems_dosfs_mount_options mount_opts[2];

  rc = mkdir( MOUNT_DIR, S_IRWXU | S_IRWXG | S_IRWXO );
  rtems_test_assert( rc == 0 );

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

  compare_image(
    MOUNT_DIR,
    "/dev/rdb",
    NULL);

  rc = unmount( MOUNT_DIR );
  rtems_test_assert( rc == 0 );

  /*
   * Again tests with code page 850 compatible directory and file names
   * but with multibyte string compatible conversion methods which use
   * iconv and utf8proc
   */
  mount_opts[0].converter = rtems_dosfs_create_utf8_converter( "CP850" );
  rtems_test_assert( mount_opts[0].converter != NULL );

  rc                     = mount(
    RAMDISK_PATH,
    MOUNT_DIR,
    "dosfs",
    RTEMS_FILESYSTEM_READ_WRITE,
    &mount_opts );
  rtems_test_assert( rc == 0 );

  test_finding_directories(
    &start_dir[0],
    &DIRECTORY_NAMES[0][0],
    NUMBER_OF_DIRECTORIES,
    &FILE_NAMES[0][0],
    NUMBER_OF_FILES );
  unmount_and_close_device();

  mount_device_with_iconv( start_dir, &mount_opts[0] );
  test_creating_invalid_directories();

  test_creating_duplicate_directories(
    &start_dir[0],
    &DIRECTORY_DUPLICATES[0],
    NUMBER_OF_DIRECTORIES_DUPLICATED );

  unmount_and_close_device();

  mount_device_with_iconv( start_dir, &mount_opts[0] );

  test_duplicated_files(
    MOUNT_DIR,
    FILES_DUPLICATES,
    NUMBER_OF_FILES_DUPLICATED );

  unmount_and_close_device();

  mount_device_with_iconv( start_dir, &mount_opts[0] );

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

  mount_opts[1].converter = rtems_dosfs_create_utf8_converter( "CP850" );
  rtems_test_assert( mount_opts[1].converter != NULL );

  compare_image(
    MOUNT_DIR,
    "/dev/rdb",
    &mount_opts[1]);

  rc = unmount( MOUNT_DIR );
  rtems_test_assert( rc == 0 );

  print_image(
      "IMAGE_BIN_LE_SINGLEBYTE_H_",
      "IMAGE_BIN_LE_SINGLEBYTE");

  rc = mount(
    RAMDISK_PATH,
    MOUNT_DIR,
    "dosfs",
    RTEMS_FILESYSTEM_READ_WRITE,
    NULL );
  rtems_test_assert( rc == 0 );

  unmount_and_close_device();


  /*
   * Tests with multibyte directory and file names and
   * with multibyte string compatible conversion methods which use
   * iconv and utf8proc
   */
  mount_device_with_iconv( start_dir, &mount_opts[0] );

  test_creating_duplicate_directories(
    &start_dir[0],
    &MULTIBYTE_DUPLICATES[0],
    NUMBER_OF_MULTIBYTE_NAMES_DUPLICATED );

  unmount_and_close_device();

  mount_device_with_iconv( start_dir, &mount_opts[0] );

  test_duplicated_files(
    MOUNT_DIR,
    &MULTIBYTE_DUPLICATES[0],
    NUMBER_OF_MULTIBYTE_NAMES_DUPLICATED );

  unmount_and_close_device();

  mount_device_with_iconv( start_dir, &mount_opts[0] );

  test_creating_directories(
    &start_dir[0],
    &NAMES_MULTIBYTE[0][0],
    NUMBER_OF_NAMES_MULTIBYTE );

  test_handling_directories(
    &start_dir[0],
    &NAMES_MULTIBYTE[0][0],
    NUMBER_OF_NAMES_MULTIBYTE,
    &NAMES_MULTIBYTE[0][0],
    NUMBER_OF_NAMES_MULTIBYTE );

  mount_opts[1].converter = rtems_dosfs_create_utf8_converter( "CP850" );
  rtems_test_assert( mount_opts[1].converter != NULL );

  compare_image(
    MOUNT_DIR,
    "/dev/rdc",
    &mount_opts[1]);

  rc = unmount( MOUNT_DIR );
  rtems_test_assert( rc == 0 );

  print_image(
    "IMAGE_BIN_LE_MULTIBYTE_H_",
    "IMAGE_BIN_LE_MULTIBYTE");

  rc = mount(
    RAMDISK_PATH,
    MOUNT_DIR,
    "dosfs",
    RTEMS_FILESYSTEM_READ_WRITE,
    NULL );
  rtems_test_assert( rc == 0 );

  test_finding_directories(
    &start_dir[0],
    &NAMES_MULTIBYTE_IN_CODEPAGE_FORMAT[0][0],
    NUMBER_OF_NAMES_MULTIBYTE,
    &NAMES_MULTIBYTE_IN_CODEPAGE_FORMAT[0][0],
    NUMBER_OF_NAMES_MULTIBYTE );

  unmount_and_close_device();

  test_compatibility();
}

static void Init( rtems_task_argument arg )
{
  puts( "\n\n*** TEST fsdosfsname01 ***" );

  test();

  puts( "*** END OF TEST fsdosfsname01 ***" );

  rtems_test_exit( 0 );
}

rtems_ramdisk_config rtems_ramdisk_configuration [] = {
  { .block_size = BLOCK_SIZE, .block_num = BLOCK_NUM },
  { .block_size = BLOCK_SIZE, .block_num = BLOCK_NUM, .location = &IMAGE_BIN_LE_SINGLEBYTE[0] },
  { .block_size = BLOCK_SIZE, .block_num = BLOCK_NUM, .location = &IMAGE_BIN_LE_MULTIBYTE[0] }
};

size_t rtems_ramdisk_configuration_size = RTEMS_ARRAY_SIZE(rtems_ramdisk_configuration);

#define CONFIGURE_INIT_TASK_STACK_SIZE ( 1024 * 64 )
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_MAXIMUM_DRIVERS 4
#define CONFIGURE_MAXIMUM_SEMAPHORES (2 * RTEMS_DOSFS_SEMAPHORES_PER_INSTANCE)
#define CONFIGURE_APPLICATION_EXTRA_DRIVERS RAMDISK_DRIVER_TABLE_ENTRY

#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_FILESYSTEM_DOSFS

/* 2 RAM disk device files + 2 mount_dir + stdin + stdout + stderr +
 * 2 for open directories/files  + 4 * 2 for recursive tree compares*/
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS ( 7 + 2 + ( 4 * 2 ) )

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
