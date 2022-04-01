/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Note on the used tar file: Generate the file on a system that supports
 * symlinks with the following commands (tested on Linux - you might have to
 * adapt on other systems):
 *
 * export WORK=some_work_directory
 * rm -r ${WORK}
 * mkdir -p ${WORK}/home/abc/def
 * mkdir -p ${WORK}/home/dir
 * cd ${WORK}
 * echo "#! joel" > home/abc/def/test_script
 * echo "ls -las /dev" >> home/abc/def/test_script
 * chmod 755 home/abc/def/test_script
 * echo "This is a test of loading an RTEMS filesystem from an" > home/test_file
 * echo "initial tar image." >> home/test_file
 * echo "Hello world" >> home/dir/file
 * ln -s home/test_file symlink
 * tar cf tar01.tar --format=ustar \
 *     symlink \
 *     home/test_file \
 *     home/abc/def/test_script \
 *     home/dir
 *
 * Note that "home/dir" is in the archive as separate directory. "home/abc" is
 * only in the archive as a parent of the file "test_script".
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

static void assert_file_content(
    const char *name,
    const char *expected_content,
    ssize_t expected_size
)
{
  char buf[16];
  int fd;
  int rd;

  fd = open(name, O_RDONLY);
  rtems_test_assert( fd >= 0 );
  do {
    rd = read(fd, buf, sizeof(buf));
    rtems_test_assert( rd >= 0 );
    if (rd > 0) {
      rtems_test_assert( expected_size - rd >= 0 );
      rtems_test_assert( memcmp(buf, expected_content, rd) == 0 );
      expected_content += rd;
      expected_size -= rd;
    }
  } while(rd > 0);
  rtems_test_assert( expected_size == 0 );
  close(fd);
}

static void assert_content_like_expected(void)
{
  const char *directories[] = {
    "home",
    "home/abc",
    "home/abc/def",
    "home/dir",
  };
  const char *symlinks[] = {
    "symlink",
  };
  const struct {
    const char *name;
    const char *content;
  } files[] = {
    {
      .name = "home/abc/def/test_script",
      .content = "#! joel\nls -las /dev\n",
    }, {
      .name = "home/test_file",
      .content = "This is a test of loading an RTEMS filesystem from an\n"
                 "initial tar image.\n",
    }, {
      .name = "home/dir/file",
      .content = "Hello world\n",
    }
  };
  size_t i;
  struct stat st;

  for(i = 0; i < RTEMS_ARRAY_SIZE(directories); ++i) {
    lstat(directories[i], &st);
    rtems_test_assert( S_ISDIR(st.st_mode) );
  }

  for(i = 0; i < RTEMS_ARRAY_SIZE(symlinks); ++i) {
    lstat(symlinks[i], &st);
    rtems_test_assert( S_ISLNK(st.st_mode) );
  }

  for(i = 0; i < RTEMS_ARRAY_SIZE(files); ++i) {
    lstat(files[i].name, &st);
    rtems_test_assert( S_ISREG(st.st_mode) );

    assert_file_content(
        files[i].name,
        files[i].content,
        strlen(files[i].content)
        );
  }
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

  /* Case 1: Untar it into empty directory */
  rv = Untar_FromFile( "/test.tar" );
  printf("Untaring from file - ");
  if (rv != UNTAR_SUCCESSFUL) {
    printf ("error: untar failed: %i\n", rv);
    exit(1);
  }
  assert_content_like_expected();
  printf ("successful\n");

  /* Case 2: Most files exist */
  rv = unlink("/dest/home/test_file");
  rtems_test_assert( rv == 0 );

  rv = Untar_FromFile( "/test.tar" );
  printf("Untar from file into existing structure with one missing file - ");
  if (rv != UNTAR_SUCCESSFUL) {
    printf ("error: untar failed: %i\n", rv);
    exit(1);
  }
  assert_content_like_expected();
  printf ("successful\n");

  /* Case 3: An empty directory exists where a file should be */
  rv = unlink("/dest/home/test_file");
  rtems_test_assert( rv == 0 );
  rv = mkdir("/dest/home/test_file", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  rtems_test_assert( rv == 0 );

  rv = Untar_FromFile( "/test.tar" );
  printf("Untar from file; overwrite empty directory with file - ");
  if (rv != UNTAR_SUCCESSFUL) {
    printf ("error: untar failed: %i\n", rv);
    exit(1);
  }
  assert_content_like_expected();
  printf ("successful\n");

  /* Case 4: A file exists where a parent directory should be created */
  rv = unlink("/dest/home/abc/def/test_script");
  rtems_test_assert( rv == 0 );
  rv = unlink("/dest/home/abc/def");
  rtems_test_assert( rv == 0 );
  rv = unlink("/dest/home/abc");
  rtems_test_assert( rv == 0 );
  fd = creat("/dest/home/abc", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  rtems_test_assert( fd >= 0 );
  close(fd);

  rv = Untar_FromFile( "/test.tar" );
  printf("Untar from file; file exists where parent dir should be created - ");
  if (rv != UNTAR_FAIL) {
    printf ("error: untar didn't fail like expected: %i\n", rv);
    exit(1);
  }
  printf ("expected fail\n");
  /* cleanup so that the next one works */
  rv = unlink("/dest/home/abc");
  rtems_test_assert( rv == 0 );

  /* Case 5: A non-empty directory exists where a file should be created */
  rv = unlink("/dest/home/test_file");
  rtems_test_assert( rv == 0 );
  rv = mkdir("/dest/home/test_file", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  rtems_test_assert( rv == 0 );
  fd = creat("/dest/home/test_file/file",
      S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  rtems_test_assert( fd >= 0 );
  close(fd);

  rv = Untar_FromFile( "/test.tar" );
  printf("Untar from file; non-empty dir where file should be created - ");
  if (rv != UNTAR_FAIL) {
    printf ("error: untar didn't fail like expected: %i\n", rv);
    exit(1);
  }
  printf ("expected fail\n");
  /* cleanup so that the next one works */
  rv = unlink("/dest/home/test_file/file");
  rtems_test_assert( rv == 0 );
  rv = unlink("/dest/home/test_file");
  rtems_test_assert( rv == 0 );

  /* Case 6: A file exists where a directory is explicitly in the archive */
  rv = unlink("/dest/home/dir/file");
  rtems_test_assert( rv == 0 );
  rv = unlink("/dest/home/dir");
  rtems_test_assert( rv == 0 );
  fd = creat("/dest/home/dir", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  rtems_test_assert( fd >= 0 );
  close(fd);

  rv = Untar_FromFile( "/test.tar" );
  printf("Untar from file; overwrite file with explicit directory - ");
  if (rv != UNTAR_SUCCESSFUL) {
    printf ("error: untar failed: %i\n", rv);
    exit(1);
  }
  assert_content_like_expected();
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
