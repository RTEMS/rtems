/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp.h> /* for device driver prototypes */
#include "tmacros.h"
#include <rtems/untar.h>
#include <rtems/error.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "tar01-tar.h"
#include "tar01-tar-gz.h"
#if HAVE_XZ
#include "tar01-tar-xz.h"
#endif

const char rtems_test_name[] = "TAR 1";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
void test_untar_from_memory(void);
void test_untar_from_file(void);
void test_untar_chunks_from_memory(void);
void test_untar_unzip_tgz(void);
void test_untar_unzip_txz(void);

#define TARFILE_START    tar01_tar
#define TARFILE_SIZE     tar01_tar_size
#define TARFILE_GZ_START tar01_tar_gz
#define TARFILE_GZ_SIZE  tar01_tar_gz_size
#if HAVE_XZ
#define TARFILE_XZ_START tar01_tar_xz
#define TARFILE_XZ_SIZE  tar01_tar_xz_size
#endif

void test_cat(
  char *file,
  int   offset_arg,
  int   length
);

static void test_untar_check_mode(const char* file, int mode)
{
  struct stat sb;
  int         fmode;
  rtems_test_assert(stat(file, &sb) == 0);
  fmode = sb.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
  printf(" %s: mode: %04o want: %04o\n", file, fmode, mode);
  rtems_test_assert(fmode == mode);
}

void test_untar_from_memory(void)
{
  rtems_status_code sc;

  printf("Untaring from memory - ");
  sc = Untar_FromMemory_Print(
    (void *)TARFILE_START,
    TARFILE_SIZE,
    &rtems_test_printer
  );
  if (sc != RTEMS_SUCCESSFUL) {
    printf ("error: untar failed: %s\n", rtems_status_text (sc));
    exit(1);
  }
  printf ("successful\n");

  /******************/
  printf( "========= /home/test_file =========\n" );
  test_cat( "/home/test_file", 0, 0 );

  /******************/
  printf( "========= /home/abc/def/test_script =========\n" );
  test_cat( "/home/abc/def/test_script", 0, 0 );
  test_untar_check_mode("/home/abc/def/test_script", 0755);

  /******************/
  printf( "========= /symlink =========\n" );
  test_cat( "/symlink", 0, 0 );

}

void test_untar_from_file(void)
{
  int                fd;
  int                rv;
  ssize_t            n;

  puts( "" );

  puts( "Copy tar image to test.tar" );
  /* Copy tar image from object to file in IMFS */
  fd = open( "/test.tar", O_CREAT|O_TRUNC|O_WRONLY, 0777 );
  rtems_test_assert( fd != -1 );

  n = write( fd, TARFILE_START, TARFILE_SIZE );
  rtems_test_assert( n == TARFILE_SIZE );
  close( fd );

  /* make a directory to untar it into */
  rv = mkdir( "/dest", 0777 );
  rtems_test_assert( rv == 0 );

  rv = chdir( "/dest" );
  rtems_test_assert( rv == 0 );

  /* Untar it */
  rv = Untar_FromFile( "/test.tar" );
  printf("Untaring from file - ");
  if (rv != UNTAR_SUCCESSFUL) {
    printf ("error: untar failed: %i\n", rv);
    exit(1);
  }
  printf ("successful\n");

  /******************/
  printf( "========= /dest/home/test_file =========\n" );
  test_cat( "/dest/home/test_file", 0, 0 );

  /******************/
  printf( "========= /dest/home/abc/def/test_script =========\n" );
  test_cat( "/dest/home/abc/def/test_script", 0, 0 );
  test_untar_check_mode("/dest/home/abc/def/test_script", 0755);

  /******************/
  printf( "========= /dest/symlink =========\n" );
  test_cat( "/dest/symlink", 0, 0 );
}

void test_untar_chunks_from_memory(void)
{
  rtems_status_code sc;
  int rv;
  Untar_ChunkContext ctx;
  unsigned long counter = 0;
  char *buffer = (char *)TARFILE_START;
  size_t buflen = TARFILE_SIZE;

  puts( "" );

  /* make a directory to untar it into */
  rv = mkdir( "/dest2", 0777 );
  rtems_test_assert( rv == 0 );

  rv = chdir( "/dest2" );
  rtems_test_assert( rv == 0 );

  printf( "Untaring chunks from memory - " );
  Untar_ChunkContext_Init(&ctx);
  do {
    sc = Untar_FromChunk_Print(
      &ctx,
      &buffer[counter],
      (size_t)1 ,
      &rtems_test_printer
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    counter ++;
  } while (counter < buflen);
  printf("successful\n");

  /******************/
  printf( "========= /dest2/home/test_file =========\n" );
  test_cat( "/dest2/home/test_file", 0, 0 );

  /******************/
  printf( "========= /dest2/home/abc/def/test_script =========\n" );
  test_cat( "/dest2/home/abc/def/test_script", 0, 0 );
  test_untar_check_mode("/dest2/home/abc/def/test_script", 0755);

  /******************/
  printf( "========= /dest2/symlink =========\n" );
  test_cat( "/dest2/symlink", 0, 0 );

}

void test_untar_unzip_tgz(void)
{
  int status;
  int rv;
  Untar_GzChunkContext ctx;
  size_t i = 0;
  char *buffer = (char *)TARFILE_GZ_START;
  size_t buflen = TARFILE_GZ_SIZE;
  char inflate_buffer;

  puts( "" );

  rtems_test_assert( buflen != 0 );

  /* make a directory to untar it into */
  rv = mkdir( "/dest3", 0777 );
  rtems_test_assert( rv == 0 );

  rv = chdir( "/dest3" );
  rtems_test_assert( rv == 0 );

  printf( "Untaring chunks from tgz - " );

  status = Untar_GzChunkContext_Init(&ctx, &inflate_buffer, 1);
  rtems_test_assert(status == UNTAR_SUCCESSFUL);
  for(i = 0; i < buflen; i++) {
    status = Untar_FromGzChunk_Print(&ctx, &buffer[i], 1, &rtems_test_printer);
    rtems_test_assert(status == UNTAR_SUCCESSFUL);
  }
  printf( "successful\n" );

  /******************/
  printf( "========= /dest3/home/test_file =========\n" );
  test_cat( "/dest3/home/test_file", 0, 0 );

  /******************/
  printf( "========= /dest3/home/abc/def/test_script =========\n" );
  test_cat( "/dest3/home/abc/def/test_script", 0, 0 );
  test_untar_check_mode("/dest3/home/abc/def/test_script", 0755);

  /******************/
  printf( "========= /dest3/symlink =========\n" );
  test_cat( "/dest3/symlink", 0, 0 );
}

void test_untar_unzip_txz(void)
{
#if HAVE_XZ
  int status;
  int rv;
  Untar_XzChunkContext ctx;
  size_t i = 0;
  char *buffer = (char *)TARFILE_XZ_START;
  size_t buflen = TARFILE_XZ_SIZE;
  char inflate_buffer;

  puts( "" );

  rtems_test_assert( buflen != 0 );

  /* make a directory to untar it into */
  rv = mkdir( "/dest4", 0777 );
  rtems_test_assert( rv == 0 );

  rv = chdir( "/dest4" );
  rtems_test_assert( rv == 0 );

  printf( "Untaring chunks from txz - " );

  /*
   * Use 8K dict, this is set on the command line of xz when compressing.
   */
  status = Untar_XzChunkContext_Init(&ctx, XZ_DYNALLOC,
                                     8 * 1024, &inflate_buffer, 1);
  rtems_test_assert(status == UNTAR_SUCCESSFUL);
  for(i = 0; i < buflen; i++) {
    status = Untar_FromXzChunk_Print(&ctx, &buffer[i], 1, &rtems_test_printer);
    rtems_test_assert(status == UNTAR_SUCCESSFUL);
  }
  printf( "successful\n" );

  /******************/
  printf( "========= /dest4/home/test_file =========\n" );
  test_cat( "/dest4/home/test_file", 0, 0 );

  /******************/
  printf( "========= /dest4/home/abc/def/test_script =========\n" );
  test_cat( "/dest4/home/abc/def/test_script", 0, 0 );
  test_untar_check_mode("/dest4/home/abc/def/test_script", 0755);

  /******************/
  printf( "========= /dest4/symlink =========\n" );
  test_cat( "/dest4/symlink", 0, 0 );
#endif
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  TEST_BEGIN();

  test_untar_from_memory();
  test_untar_from_file();
  test_untar_chunks_from_memory();
  test_untar_unzip_tgz();
  test_untar_unzip_txz();

  TEST_END();
  exit( 0 );
}


/* NOTICE: the clock driver is explicitly disabled */
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS            1
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 5

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
