/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuiteTestsTFTPFS
 *
 * @brief This source file contains the implementation of tests for libtftpfs.
 *
 * The tested source files are:
 *   + @ref tftpfs.c "tftpfs.c: TFTP file system"
 *   + @ref tftpDriver.c "tftpDriver.c: TFTP client library"
 * These tests focus on testing the UDP network interaction of libtftpfs.
 */

/*
 * Copyright (C) 2022 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h> /* malloc(), free() */
#include <ctype.h> /* isprint() */
#include <errno.h>
#include <sys/stat.h> /* mkdir(), open() */
#include <sys/types.h> /* mkdir(), open() */
#include <sys/socket.h> /* AF_INET, SOCK_DGRAM */
#include <fcntl.h> /* open() */
#include <unistd.h> /* read(), close(), rmdir() */

#include <rtems/tftp.h>
#include <rtems/libio.h> /* mount(), RTEMS_FILESYSTEM_TYPE_TFTPFS */
#include <rtems/test.h>
#include <rtems/test-info.h>
#include <rtems/testopts.h> /* RTEMS_TEST_VERBOSITY */
#include <rtems.h>

#include "tftpfs_udp_network_fake.h"
#include "tftpfs_interactions.h"
#include "tftp_driver.h"

#define SERV_PORT 12345
#define FIRST_TIMEOUT_MILLISECONDS  400
#define TIMEOUT_MILLISECONDS        1000
#define LARGE_BLOCK_SIZE            TFTP_BLOCK_SIZE_MAX
#define SMALL_BLOCK_SIZE            12
#define SMALL_WINDOW_SIZE           4
#define T_no_more_interactions() T_assert_true( \
  _Tftp_Has_no_more_interactions(), \
  "The TFTP client skiped some final network interactions." \
)

/*
 * Test fixture and text context
 */

typedef struct tftp_test_context {
  int fd0; /* File descriptor of a file read from or written to the tftpsfs */
  void *tftp_handle; /* TFTP client handle for this file transfer */
} tftp_test_context;

static const char *tftpfs_mount_point   = "/tftp";
static const char *tftpfs_ipv4_loopback = TFTP_KNOWN_IPV4_ADDR0_STR;
static const char *tftpfs_server0_name  = TFTP_KNOWN_SERVER0_NAME;
static const char *tftpfs_server0_ipv4  = TFTP_KNOWN_SERVER0_IPV4;
static const char *tftpfs_file          = "file.txt";
static tftp_test_context tftp_context;

static void mount_tftp_fs( const char *mount_point, const char *options )
{
  int result;

  result = mkdir( mount_point, S_IRWXU | S_IRWXG | S_IRWXO );
  T_assert_eq_int( result, 0 );

  result = mount(
    "",
    mount_point,
    RTEMS_FILESYSTEM_TYPE_TFTPFS,
    RTEMS_FILESYSTEM_READ_WRITE,
    options
  );
  T_assert_eq_int( result, 0 );
}

static void umount_tftp_fs( const char *mount_point )
{
  int result;

  result = unmount( mount_point );
  T_assert_eq_int( result, 0 );

  result = rmdir( mount_point );
  T_assert_eq_int( result, 0 );
}

static void setup_rfc1350( void *context )
{
  tftp_test_context *ctx = context;
  _Tftp_Reset();
  ctx->fd0 = -1;
  ctx->tftp_handle = NULL;
  mount_tftp_fs( tftpfs_mount_point, "verbose,rfc1350" );
}

static void teardown( void *context )
{
  tftp_test_context *ctx = context;
  if ( ctx->fd0 >= 0 ) {
    close( ctx->fd0 );
  }
  tftp_close( ctx->tftp_handle ); /* is a no-op if NULL */
  umount_tftp_fs( tftpfs_mount_point );
  _Tftp_Reset();
}

static const T_fixture fixture_rfc1350 = {
  .setup = setup_rfc1350,
  .stop = NULL,
  .teardown = teardown,
  .scope = NULL,
  .initial_context = &tftp_context
};

static void setup_default_options( void *context )
{
  tftp_test_context *ctx = context;
  _Tftp_Reset();
  ctx->fd0 = -1;
  ctx->tftp_handle = NULL;
  mount_tftp_fs( tftpfs_mount_point, NULL );
}

static const T_fixture fixture_default_options = {
  .setup = setup_default_options,
  .stop = NULL,
  .teardown = teardown,
  .scope = NULL,
  .initial_context = &tftp_context
};

static void setup_large_blocksize( void *context )
{
  tftp_test_context *ctx = context;
  _Tftp_Reset();
  ctx->fd0 = -1;
  ctx->tftp_handle = NULL;
  mount_tftp_fs(
    tftpfs_mount_point,
    "verbose,blocksize=" RTEMS_XSTRING(LARGE_BLOCK_SIZE) ",windowsize=1"
  );
}

static const T_fixture fixture_large_blocksize = {
  .setup = setup_large_blocksize,
  .stop = NULL,
  .teardown = teardown,
  .scope = NULL,
  .initial_context = &tftp_context
};

static void setup_small_opt_size( void *context )
{
  tftp_test_context *ctx = context;
  _Tftp_Reset();
  ctx->fd0 = -1;
  ctx->tftp_handle = NULL;
  mount_tftp_fs(
    tftpfs_mount_point,
    "blocksize=" RTEMS_XSTRING(SMALL_BLOCK_SIZE)
    ",windowsize=" RTEMS_XSTRING(SMALL_WINDOW_SIZE)
  );
}

static const T_fixture fixture_small_opt_size = {
  .setup = setup_small_opt_size,
  .stop = NULL,
  .teardown = teardown,
  .scope = NULL,
  .initial_context = &tftp_context
};

static void setup_mount_point( void *context )
{
  int result;

  _Tftp_Reset();
  result = mkdir( tftpfs_mount_point, S_IRWXU | S_IRWXG | S_IRWXO );
  T_assert_eq_int( result, 0 );
}

static void teardown_mount_point( void *context )
{
  int result;

  result = rmdir( tftpfs_mount_point );
  T_assert_eq_int( result, 0 );
  _Tftp_Reset();
}

static const T_fixture fixture_mount_point = {
  .setup = setup_mount_point,
  .stop = NULL,
  .teardown = teardown_mount_point,
  .scope = NULL,
  .initial_context = &tftp_context
};

/*
 * Test helper functions
 */

/*
 * Produce an artificial file content to be able to compare the
 * sent and the received file later on.
 */
static uint8_t get_file_content( size_t pos )
{
  static const size_t frame_size = 100;
  static const size_t num_size = 11;
  static const size_t alpha_size = 53;
  char buf[10];
  size_t remainder = pos % frame_size;

  switch ( remainder ) {
    case 0:
    case 1:
    case 2:
      sprintf( buf, "%9zu", pos - remainder );
      return buf[remainder];
    case 3:
    case 7:
      return '\'';
    case 4:
    case 5:
    case 6:
      sprintf( buf, "%9zu", pos - remainder );
      return buf[remainder-1];
    case 8:
    case 9:
    case 10:
      sprintf( buf, "%9zu", pos - remainder );
      return buf[remainder-2];
    default:
      pos -= ( pos / frame_size + 1 ) * num_size;
      remainder = pos % alpha_size;
      return ( remainder <= 'Z' - '@' ) ?
        remainder + '@' : remainder - ( 'Z' - '@' + 1) + 'a';
  }
}

/*
 * Produce bad file content.
 */
static uint8_t get_bad_file_content( size_t pos )
{
  static const char buf[] = "BAD!";
  return (uint8_t) buf[ pos % strlen( buf ) ];
}

static const char *create_tftpfs_path(
  const char *sever_addr,
  const char *file_name
)
{
  static char buffer[100];
  int len;

  len = snprintf(
    buffer,
    sizeof( buffer ),
    "%s/%s:%s",
    tftpfs_mount_point,
    sever_addr,
    file_name
  );

  T_quiet_gt_int( len, 0 );
  T_quiet_lt_int( len, (int) sizeof( buffer ) );
  return buffer;
}

static int read_tftp_file(
  const char *path,
  size_t buffer_size,
  size_t max_bytes,
  int *fd
)
{
  char *data_buffer;
  int result = 0;
  int res;
  ssize_t i;
  ssize_t bytes = 1;
  ssize_t bytes_total = 0;
  int errno_store;

  T_log( T_VERBOSE, "File system: open( %s, O_RDONLY )", path );
  errno = 0;
  *fd = open( path, O_RDONLY );
  errno_store = errno;
  T_log(
    T_VERBOSE,
    "File system: [open( %s, O_RDONLY )] = fd:%d (errno = %d)",
    path,
    *fd,
    errno
  );

  if ( *fd < 0 ) {
    /* open() may intentionally fail (e.g. test for invalid server address) */
    T_log( T_VERBOSE, "File system: cannot open \"%s\" for reading", path );
    errno = errno_store;
    result = -1;
  }

  if ( *fd >= 0 ) {
    data_buffer = malloc( buffer_size );

    while ( bytes > 0 && max_bytes >= bytes ) {
      errno = 0;
      bytes = read(
        *fd,
        data_buffer,
        ( max_bytes > buffer_size ) ? buffer_size : max_bytes
      );
      errno_store = errno;
      T_log(
        T_VERBOSE,
        "File system: [read( fd:%d, size=%zu )] = %zd (errno = %d)",
        *fd,
        ( max_bytes > buffer_size ) ? buffer_size : max_bytes,
        bytes,
        errno
      );

      if ( bytes > 0 ) {
        max_bytes -= bytes;
        for ( i = 0; i < bytes; ++i ) {
          if ( data_buffer[i] != get_file_content( bytes_total + i ) ) {
            T_true(
              false,
              "File system: wrong file content '%c' (expected '%c') "
                "at position %zd",
              (int) ( isprint( (int) data_buffer[i] ) ? data_buffer[i] : '?' ),
              (int) get_file_content( bytes_total + i ),
              bytes_total + i
            );
            bytes = 0;
            break;
          }
        } /* for */
        bytes_total += bytes;
      }
      if ( bytes == 0 ) {
        result = (int) bytes_total;
      }
      if ( bytes < 0 ) {
        /* read() may intentionally fail (e.g. test lost network connection) */
        T_log(
          T_VERBOSE,
          "File system: error reading from \"%s\" after %zd bytes",
          path,
          bytes_total
        );
        result = (int) bytes_total;
      }
    } /* while */

    free( data_buffer );
  } /* if */

  if ( bytes > 0 ) {
    T_log(
      T_VERBOSE,
      "File system: reader closes \"%s\" after %zd bytes",
      path,
      bytes_total
    );
    result = (int) bytes_total;
  }

  if ( *fd >= 0 ) {
    res = close( *fd );
    T_log(
      T_VERBOSE,
      "File system: [close( %s (fd:%d) )] = %d",
      path,
      *fd,
      res
    );
    *fd = -1;
    T_eq_int( res, 0 );
  }

  errno = errno_store;
  return result;
}

static int write_tftp_file(
  const char *path,
  size_t file_size,
  size_t buffer_size,
  int *fd )
{
  char *data_buffer;
  int result = 0;
  int res;
  ssize_t i;
  ssize_t bytes;
  ssize_t bytes_total = 0;
  int errno_store;

  errno = 0;
  T_log( T_VERBOSE, "File system: open( %s, O_WRONLY )", path );
  *fd = open( path, O_WRONLY );
  errno_store = errno;
  T_log(
    T_VERBOSE,
    "File system: [open( %s, O_WRONLY )] = fd:%d (errno = %d)",
    path,
    *fd,
    errno
  );
  if ( *fd < 0 ) {
    /* open() may intentionally fail (e.g. test for invalid server address) */
    T_log( T_VERBOSE, "File system: cannot open \"%s\" for writing", path );
    errno = errno_store;
    result = -1;
  }

  if ( *fd >= 0 ) {
    data_buffer = malloc( buffer_size );

    do { /* Try also to write files with 0 bytes size */
      bytes = ( file_size - bytes_total >= buffer_size ) ?
        buffer_size : file_size - bytes_total;
      for ( i = 0; i < bytes; ++i ) {
        data_buffer[i] = get_file_content( bytes_total + i );
      }
      errno = 0;
      bytes = write( *fd, data_buffer, i );
      errno_store = errno;
      T_log(
        T_VERBOSE,
        "File system: [write( fd:%d, size=%zd )] = %zd (errno = %d)",
        *fd,
        i,
        bytes,
        errno
      );
      if ( bytes > 0 ) {
        bytes_total += bytes;
        result = (int) bytes_total;
      }
      if ( bytes != i ) {
        /* write() may intentionally fail (e.g. test lost network connection) */
        T_log(
          T_VERBOSE,
          "File system: error writing to \"%s\" after %zd bytes",
          path,
          bytes_total
        );
        break;
      }
    } while( bytes_total < file_size );

    free( data_buffer );
  } /* if */

  if ( *fd >= 0 ) {
    res = close( *fd );
    if (res != 0) {
      errno_store = errno;
      result = res;
    }
    T_log(
      T_VERBOSE,
      "File system: [close( %s (fd:%d) )] = %d",
      path,
      *fd,
      res
    );
    *fd = -1;
  }

  errno = errno_store;
  return result;
}

static int rdwt_tftp_client_file(
  const char *hostname,
  const char *filename,
  bool is_for_reading,
  ssize_t file_size, /* Only used when `is_for_reading == false` */
  const tftp_net_config *config,
  void **tftp_handle
)
{
  const static size_t buffer_size = 4001;
  char *data_buffer;
  int res = 0;
  ssize_t i;
  ssize_t bytes = 1;
  ssize_t bytes_total = 0;
  int errno_store = 0;

  if ( *tftp_handle == NULL ) {
    T_log(
      T_VERBOSE,
      "TFTP Client: tftp_open( \"%s\", \"%s\", %s, ... )",
      hostname,
      filename,
      is_for_reading ? "read" : "write"
    );
    res = tftp_open(
      hostname,
      filename,
      is_for_reading,
      config,
      tftp_handle
    );
    T_log(
      T_VERBOSE,
      "TFTP Client: [tftp_open( \"%s\", \"%s\", %s, ... )] = %d (handle:%p)",
      hostname,
      filename,
      is_for_reading ? "read" : "write",
      res,
      *tftp_handle
    );
  } else {
    T_log(
      T_VERBOSE,
      "TFTP Client: \"%s\":\"%s\" already open for %s, handle: %p ",
      hostname,
      filename,
      is_for_reading ? "read" : "write",
      *tftp_handle
    );
  }

  if ( res != 0 ) {
    /* open() may intentionally fail (e.g. test for invalid server address) */
    T_log(
      T_VERBOSE,
      "TFTP client: cannot open \"%s\":\"%s\" for %s",
      hostname,
      filename,
      is_for_reading ? "reading" : "writing"
    );
    errno_store = res;
  } else {
    T_assert_not_null( *tftp_handle );
  }

  if ( *tftp_handle != NULL ) {
    data_buffer = malloc( buffer_size );

    if ( is_for_reading ) {

      /* Read file */
      while ( bytes > 0 ) {
        errno = 0;
        bytes = tftp_read(
          *tftp_handle,
          data_buffer,
          buffer_size
        );
        T_log(
          T_VERBOSE,
          "TFTP Client: [tftp_read( %p, size=%zu )] = %zd",
          *tftp_handle,
          buffer_size,
          bytes
        );

        if ( bytes > 0 ) {
          for ( i = 0; i < bytes; ++i ) {
            if ( data_buffer[i] != get_file_content( bytes_total + i ) ) {
              T_true(
                false,
                "FTP Client: wrong file content '%c' (expected '%c') at positon %zd",
                (int) ( isprint( (int) data_buffer[i] ) ? data_buffer[i] : '?' ),
                (int) get_file_content( bytes_total + i ),
                bytes_total + i
              );
              bytes = 0;
              break;
            }
          } /* for */
          bytes_total += bytes;
        }
        if ( bytes < 0 ) {
          /* read() may intentionally fail (e.g. test lost network connection) */
          T_log(
            T_VERBOSE,
            "TFTP Client: error reading from \"%s\":\"%s\" after %zd bytes",
            hostname,
            filename,
            bytes_total
          );
          errno_store = -bytes;
        }
      } /* while */
    } else {

      /* Write file */
      do { /* Try also to write files with 0 bytes size */
        bytes = ( file_size - bytes_total >= buffer_size ) ?
          buffer_size : file_size - bytes_total;
        for ( i = 0; i < bytes; ++i ) {
          data_buffer[i] = get_file_content( bytes_total + i );
        }
        errno = 0;
        bytes = tftp_write( *tftp_handle, data_buffer, i );
        T_log(
          T_VERBOSE,
          "TFTP Client: [tftp_write( %p, size=%zd )] = %zd",
          *tftp_handle,
          i,
          bytes
        );
        if ( bytes > 0 ) {
          bytes_total += bytes;
        } else {
          errno_store = -bytes;
        }
        if ( bytes != i ) {
          /* write() may intentionally fail (e.g. test lost network connection) */
          T_log(
            T_VERBOSE,
            "TFTP Client: error writing to \"%s\":\"%s\" after %zd bytes",
            hostname,
            filename,
            bytes_total
          );
          break;
        }
      } while( bytes_total < file_size );
    } /* if ( is_for_reading ) */

    free( data_buffer );
  } /* if ( *tftp_handle != NULL ) */

  if ( *tftp_handle != NULL ) {
    res = tftp_close( *tftp_handle );
    T_log(
      T_VERBOSE,
      "TFTP Client: [tftp_close( \"%s\":\"%s\" (handle:%p) )] = %d",
      hostname,
      filename,
      *tftp_handle,
      res
    );
    *tftp_handle = NULL; /* Avoid that the fixture closes it again */
    T_eq_int( res, 0 );
  } /* if ( *tftp_handle != NULL ) */

  errno = errno_store;
  return (int) bytes_total;
}

/*
 * Unit test cases
 */

/*
 * This is a classical unit test for the function tftp_initialize_net_config().
 * Tests:
 *   * tftp_initialize_net_config() sets correct default values as defined
 *     in the documentation of the data structures tftp_net_config
 *     and tftp_options.
 */
T_TEST_CASE( tftp_initialize_net_config )
{
  tftp_net_config config;
  memset( &config, 0, sizeof( config ) );
  tftp_initialize_net_config( &config );
  T_eq_u16( config.retransmissions, 6 );
  T_eq_u16( config.server_port, 69 );
  T_eq_u32( config.timeout, 1000 );
  T_eq_u32( config.first_timeout, 400 );
  T_eq_u16( config.options.block_size, 1456 );
  T_eq_u16( config.options.window_size, 8 );
}

/*
 * This is a classical unit test for the function tftp_initialize_net_config().
 * Tests:
 *   * tftp_initialize_net_config() does not crash when called with a
 *     NULL pointer.
 */
T_TEST_CASE( tftp_initialize_net_config_null )
{
  tftp_initialize_net_config( NULL );
}

/*
 * This is a classical unit test for the function tftp_open().
 * Tests:
 *   * tftp_open() returns an error when called with a NULL pointer
 *     for hostname.
 */
T_TEST_CASE_FIXTURE( tftp_open_null_hostname, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int res;

  res = tftp_open(
    NULL, /* hostname */
    tftpfs_file,
    true, /* is_for_reading */
    NULL, /* config */
    &ctx->tftp_handle
  );
  T_eq_int( res, EINVAL );
  T_null( ctx->tftp_handle );
}

/*
 * This is a classical unit test for the function tftp_open().
 * Tests:
 *   * tftp_open() returns an error when called with a NULL pointer
 *     for filename.
 */
T_TEST_CASE_FIXTURE( tftp_open_null_filename, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int res;

  res = tftp_open(
    tftpfs_ipv4_loopback,
    NULL, /* filename */
    true, /* is_for_reading */
    NULL, /* config */
    &ctx->tftp_handle
  );
  T_eq_int( res, EINVAL );
  T_null( ctx->tftp_handle );
}

/*
 * This is a classical unit test for the function tftp_open().
 * Tests:
 *   * tftp_open() returns an error when called with a NULL pointer
 *     for tftp_handle.
 */
T_TEST_CASE( tftp_open_null_tftp_handle )
{
  int res;

  res = tftp_open(
    tftpfs_ipv4_loopback,
    tftpfs_file,
    true, /* is_for_reading */
    NULL, /* config */
    NULL /* tftp_handle */
  );
  T_eq_int( res, EINVAL );
}

/*
 * This is a classical unit test for the function tftp_open().
 * Tests:
 *   * tftp_open() returns an error when called with value 0 for
 *     option window_size.
 */
T_TEST_CASE_FIXTURE( tftp_open_illegal_window_size, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  tftp_net_config config;
  int res;

  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  tftp_initialize_net_config( &config );
  config.options.window_size = 1 - 1;

  res = tftp_open(
    tftpfs_ipv4_loopback,
    tftpfs_file,
    true, /* is_for_reading */
    &config,
    &ctx->tftp_handle
  );
  T_eq_int( res, EINVAL );
  T_null( ctx->tftp_handle );
  T_no_more_interactions();
}

/*
 * This is a classical unit test for the function tftp_open().
 * Tests:
 *   * tftp_open() returns an error when called with a too small
 *     value for option block_size.
 */
T_TEST_CASE_FIXTURE( tftp_open_block_size_too_small, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  tftp_net_config config;
  int res;

  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  tftp_initialize_net_config( &config );
  config.options.block_size = 8 - 1;

  res = tftp_open(
    tftpfs_ipv4_loopback,
    tftpfs_file,
    true, /* is_for_reading */
    &config,
    &ctx->tftp_handle
  );
  T_eq_int( res, EINVAL );
  T_null( ctx->tftp_handle );
  T_no_more_interactions();
}

/*
 * This is a classical unit test for the function tftp_open().
 * Tests:
 *   * tftp_open() returns an error when called with a too large
 *     value for option block_size.
 */
T_TEST_CASE_FIXTURE( tftp_open_block_size_too_large, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  tftp_net_config config;
  int res;

  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  tftp_initialize_net_config( &config );
  config.options.block_size = 65464 + 1;

  res = tftp_open(
    tftpfs_ipv4_loopback,
    tftpfs_file,
    false, /* is_for_reading */
    &config,
    &ctx->tftp_handle
  );
  T_eq_int( res, EINVAL );
  T_null( ctx->tftp_handle );
  T_no_more_interactions();
}

/*
 * This is a classical unit test for the function tftp_read().
 * Tests:
 *   * tftp_read() returns an error when called with a NULL pointer
 *     for tftp_handle.
 */
T_TEST_CASE( tftp_read_null_tftp_handle )
{
  char data_buffer[10];
  ssize_t res;

  res = tftp_read(
    NULL, /* tftp_handle */
    data_buffer,
    sizeof( data_buffer)
  );
  T_eq_int( res, -EIO );
}

/*
 * This is a classical unit test for the function tftp_read().
 * Tests:
 *   * tftp_read() returns an error when called with a NULL pointer
 *     for buffer.
 */
T_TEST_CASE( tftp_read_null_buffer )
{
  int tftp_handle;
  ssize_t res;

  res = tftp_read(
    &tftp_handle,
    NULL, /* buffer */
    8
  );
  T_eq_int( res, -EIO );
}

/*
 * This is a classical unit test for the function tftp_write().
 * Tests:
 *   * tftp_write() returns an error when called with a NULL pointer
 *     for tftp_handle.
 */
T_TEST_CASE( tftp_write_null_tftp_handle )
{
  char data_buffer[10] = { 0 };
  ssize_t res;

  res = tftp_write(
    NULL, /* tftp_handle */
    data_buffer,
    sizeof( data_buffer)
  );
  T_eq_int( res, -EIO );
}

/*
 * This is a classical unit test for the function tftp_write().
 * Tests:
 *   * tftp_write() returns an error when called with a NULL pointer
 *     for buffer.
 */
T_TEST_CASE( tftp_write_null_buffer )
{
  int tftp_handle;
  ssize_t res;

  res = tftp_write(
    &tftp_handle,
    NULL, /* buffer */
    8
  );
  T_eq_int( res, -EIO );
}

/*
 * This is a classical unit test for the function tftp_close().
 * Tests:
 *   * tftp_close() returns 0 when called with a NULL pointer.
 */
T_TEST_CASE( tftp_close_null )
{
  T_eq_int( tftp_close( NULL ), 0 );
}

/*
 * This is a classical unit test for the function _Tftpfs_Parse_options().
 * Tests:
 *   * Parsing an empty string has no effects.
 */
T_TEST_CASE( _Tftpfs_Parse_options_empty )
{
  size_t err_pos;
  uint32_t flags = 0;
  tftp_net_config config;

  tftp_initialize_net_config( &config );
  err_pos = _Tftpfs_Parse_options( "", &config, &flags );
  T_eq_sz( err_pos, 0 );
  T_eq_u16( config.options.block_size, TFTP_DEFAULT_BLOCK_SIZE );
  T_eq_u16( config.options.window_size, TFTP_DEFAULT_WINDOW_SIZE );
  T_eq_u32( flags, 0 );
}

/*
 * This is a classical unit test for the function _Tftpfs_Parse_options().
 * Tests:
 *   * Providing an NULL pointer instead of a string has no effect.
 */
T_TEST_CASE( _Tftpfs_Parse_options_null )
{
  size_t err_pos;
  uint32_t flags = 0;
  tftp_net_config config;

  tftp_initialize_net_config( &config );
  err_pos = _Tftpfs_Parse_options( NULL, &config, &flags );
  T_eq_sz( err_pos, 0 );
  T_eq_u16( config.options.block_size, TFTP_DEFAULT_BLOCK_SIZE );
  T_eq_u16( config.options.window_size, TFTP_DEFAULT_WINDOW_SIZE );
  T_eq_u32( flags, 0 );
}

/*
 * This is a classical unit test for the function _Tftpfs_Parse_options().
 * Tests:
 *   * Option "verbose" has the desired effect.
 */
T_TEST_CASE( _Tftpfs_Parse_options_verbose )
{
  size_t err_pos;
  uint32_t flags = 0;
  tftp_net_config config;

  tftp_initialize_net_config( &config );
  err_pos = _Tftpfs_Parse_options( "verbose", &config, &flags );
  T_eq_sz( err_pos, 0 );
  T_eq_u16( config.options.block_size, TFTP_DEFAULT_BLOCK_SIZE );
  T_eq_u16( config.options.window_size, TFTP_DEFAULT_WINDOW_SIZE );
  T_gt_u32( flags, 0 );
}

/*
 * This is a classical unit test for the function _Tftpfs_Parse_options().
 * Tests:
 *   * Option "rfc1350" has the desired effect.
 */
T_TEST_CASE( _Tftpfs_Parse_options_rfc1350 )
{
  size_t err_pos;
  uint32_t flags = 0;
  tftp_net_config config;

  tftp_initialize_net_config( &config );
  err_pos = _Tftpfs_Parse_options( "rfc1350", &config, &flags );
  T_eq_sz( err_pos, 0 );
  T_eq_u16( config.options.block_size, TFTP_RFC1350_BLOCK_SIZE );
  T_eq_u16( config.options.window_size, TFTP_RFC1350_WINDOW_SIZE );
  T_eq_u32( flags, 0 );
}

/*
 * This is a classical unit test for the function _Tftpfs_Parse_options().
 * Tests:
 *   * Option "blocksize" has the desired effect.
 */
T_TEST_CASE( _Tftpfs_Parse_options_blocksize )
{
  size_t err_pos;
  uint32_t flags = 0;
  tftp_net_config config;

  tftp_initialize_net_config( &config );
  err_pos = _Tftpfs_Parse_options( "blocksize=21", &config, &flags );
  T_eq_sz( err_pos, 0 );
  T_eq_u16( config.options.block_size, 21 );
  T_eq_u16( config.options.window_size, TFTP_DEFAULT_WINDOW_SIZE );
  T_eq_u32( flags, 0 );
}

/*
 * This is a classical unit test for the function _Tftpfs_Parse_options().
 * Tests:
 *   * Option "windowsize" has the desired effect.
 */
T_TEST_CASE( _Tftpfs_Parse_options_windowsize )
{
  size_t err_pos;
  uint32_t flags = 0;
  tftp_net_config config;

  tftp_initialize_net_config( &config );
  err_pos = _Tftpfs_Parse_options( "windowsize=13", &config, &flags );
  T_eq_sz( err_pos, 0 );
  T_eq_u16( config.options.block_size, TFTP_DEFAULT_BLOCK_SIZE );
  T_eq_u16( config.options.window_size, 13 );
  T_eq_u32( flags, 0 );
}

/*
 * This is a classical unit test for the function _Tftpfs_Parse_options().
 * Tests:
 *   * Processing of all options in one string works as expected.
 */
T_TEST_CASE( _Tftpfs_Parse_options_all )
{
  size_t err_pos;
  uint32_t flags = 0;
  tftp_net_config config;

  tftp_initialize_net_config( &config );
  err_pos = _Tftpfs_Parse_options( "rfc1350,blocksize=1234,windowsize=4567,verbose", &config, &flags );
  T_eq_sz( err_pos, 0 );
  T_eq_u16( config.options.block_size, 1234 );
  T_eq_u16( config.options.window_size, 4567 );
  T_gt_u32( flags, 0 );
}

/*
 * This is a classical unit test for the function _Tftpfs_Parse_options().
 * Tests:
 *   * Parser ignores unnecessary commas.
 */
T_TEST_CASE( _Tftpfs_Parse_options_surplus_comma )
{
  size_t err_pos;
  uint32_t flags = 0;
  tftp_net_config config;

  tftp_initialize_net_config( &config );
  err_pos = _Tftpfs_Parse_options( ",blocksize=1234,,,,windowsize=4567,,", &config, &flags );
  T_eq_sz( err_pos, 0 );
  T_eq_u16( config.options.block_size, 1234 );
  T_eq_u16( config.options.window_size, 4567 );
  T_eq_u32( flags, 0 );
}

/*
 * This is a classical unit test for the function _Tftpfs_Parse_options().
 * Tests:
 *   * Parser detects a bad value.
 */
T_TEST_CASE( _Tftpfs_Parse_options_bad_value )
{
  size_t err_pos;
  uint32_t flags = 0;
  tftp_net_config config;

  tftp_initialize_net_config( &config );
  err_pos = _Tftpfs_Parse_options( "blocksize=123.4,windowsize=4567", &config, &flags );
  T_eq_sz( err_pos, 14 );
}

/*
 * This is a classical unit test for the function _Tftpfs_Parse_options().
 * Tests:
 *   * Parser detects an illegal option.
 */
T_TEST_CASE( _Tftpfs_Parse_options_illegal_option )
{
  size_t err_pos;
  uint32_t flags = 0;
  tftp_net_config config;

  tftp_initialize_net_config( &config );
  err_pos = _Tftpfs_Parse_options( "blocksize=123,illegal", &config, &flags );
  T_eq_sz( err_pos, 15 );
}

/*
 * This is a classical unit test for the function _Tftpfs_Parse_options().
 * Tests:
 *   * Parser detects a truncated option.
 */
T_TEST_CASE( _Tftpfs_Parse_options_truncated_option )
{
  size_t err_pos;
  uint32_t flags = 0;
  tftp_net_config config;

  tftp_initialize_net_config( &config );
  err_pos = _Tftpfs_Parse_options( "blocksize", &config, &flags );
  T_eq_sz( err_pos, 1 );
}

/*
 * This is a classical unit test for the function rtems_tftpfs_initialize().
 * Tests:
 *   * Correct error handling in case mount options cannot be parsed.
 */
T_TEST_CASE_FIXTURE( mount_with_bad_options, &fixture_mount_point )
{
  int result;

  result = mount(
    "",
    tftpfs_mount_point,
    RTEMS_FILESYSTEM_TYPE_TFTPFS,
    RTEMS_FILESYSTEM_READ_WRITE,
    "windowsize=4567,blocksize=123bad"
  );
  T_assert_le_int( result, -1 );
  T_assert_eq_int( errno, EINVAL );
}

/*
 * Test cases for the TFTP client interface
 *
 * Since the TFTP file system uses the TFTP client interface for all
 * file transfers, the function of the TFTP client is almost
 * completely tested by the tests for the file system interface.
 * The test cases here - for the TFTP client interface - test only
 * those aspects not (easily) testable through the file system interface.
 */

/*
 * Read a file from the server using the TFTP client interface.
 * The file is one byte long.  No timeouts, packet loss, ...
 * Tests:
 *   * tftp_open() called with NULL for config uses
 *     default configuration values.
 *   * Read a file using only the TFTP client (i.e. not using the
 *     file system)
 */
T_TEST_CASE_FIXTURE( client_open_with_NULL_config, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  uint16_t block_num = 1;
  size_t pos_in_file = 0;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    TFTP_DEFAULT_BLOCK_SIZE,
    TFTP_DEFAULT_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    1, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += 1;
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num++,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = rdwt_tftp_client_file(
    tftpfs_ipv4_loopback,
    tftpfs_file,
    true, /* is_for_reading */
    -1, /* file_size for writing files only */
    NULL, /* config */
    &ctx->tftp_handle
  );
  T_eq_sz( bytes_read, pos_in_file );
  T_eq_int( errno, 0 );
  T_no_more_interactions();
}

/*
 * Read a very short file from the server using the TFTP client interface.
 * The file is one data packet long.  Use none-default configuration values.
 * The second and the third DATA packets are lost.  This causes
 * a termination of the connection because only two retransmissions are
 * configured.
 * Tests:
 *   * tftp_open() called with all configuration values having
 *     none default values.
 *   * The test writes a file using only the TFTP client (i.e. not using the
 *     file system API).
 *   * The client uses the none default configuration values:
 *     retransmissions, server_port, timeout, first_timeout,
 *     block_size, window_size.
 *   * The server sends the options in a different order than the client.
 *   * The option names in the OACK can be upper or lower case.
 *   * If windowsize > 1, the client sends ACK only each windowsize packet.
 *   * If windowsize > 1 and no packet is received in the timeout period,
 *     the client retransmits the last ACK.
 *   * The client makes a limited number of retransmissions attempts
 *     and then terminates the connections with an error.
 */
T_TEST_CASE_FIXTURE( client_open_with_none_default_config, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  tftp_net_config config;
  int bytes_read;
  uint16_t block_num = 0;
  size_t pos_in_file = 0;
  uint16_t retransmissions = 2;
  uint16_t server_port = 3456;
  uint32_t timeout = 300;
  uint32_t first_timeout = 200;
  uint16_t block_size = 8;
  uint16_t window_size = 2;
  const char options[] =
    "WINDOWSIZE" "\0" "2\0"
    TFTP_OPTION_BLKSIZE "\0" "8";

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    server_port,
    tftpfs_ipv4_loopback,
    block_size,
    window_size,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    first_timeout,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    options,
    sizeof( options ),
    true
  );
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num++,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    first_timeout,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    pos_in_file,
    block_size, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE;
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    first_timeout,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    block_size, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE;
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num++,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_nothing(
    TFTP_FIRST_FD,
    first_timeout /* Timeout: No packet received within timeout period */
  );
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num - 1, /* Block number OK: Last block successfully received */
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_nothing(
    TFTP_FIRST_FD,
    timeout /* Timeout: No packet received within timeout period */
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_NO_USER,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  tftp_initialize_net_config( &config );
  config.retransmissions = retransmissions;
  config.server_port = server_port;
  config.timeout = timeout;
  config.first_timeout = first_timeout;
  config.options.block_size = block_size;
  config.options.window_size = window_size;

  bytes_read = rdwt_tftp_client_file(
    tftpfs_ipv4_loopback,
    tftpfs_file,
    true, /* is_for_reading */
    -1, /* file_size for writing files only */
    &config,
    &ctx->tftp_handle
  );

  /*
   * Not a bug but not nice: The client has received data before the connection
   * breaks down due to timeout and this data is not provided to the user.
   */

  T_eq_sz( bytes_read, 0 );
  T_eq_int( errno, EIO );
  T_no_more_interactions();
}

/*
 * Attempt to write to a file open for reading using the TFTP client interface.
 * Tests:
 *   * tftp_open() called with NULL for config uses
 *     default configuration values.
 *   * Read a file using only the TFTP client (i.e. not using the
 *     file system)
 *   * The attempt to write to a file open for reading is rejected
 *     with an error.
 *   * The server receives an error message to indicate that the client
 *     closes the connection without having transferred data.
 */
T_TEST_CASE_FIXTURE( client_write_to_file_opened_for_reading, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int res = 0;
  const char options[] =
    TFTP_OPTION_BLKSIZE "\0"
    RTEMS_XSTRING( TFTP_DEFAULT_BLOCK_SIZE ) "\0"
    TFTP_OPTION_WINDOWSIZE"\0"
    RTEMS_XSTRING( TFTP_DEFAULT_WINDOW_SIZE );

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    TFTP_DEFAULT_BLOCK_SIZE,
    TFTP_DEFAULT_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    options,
    sizeof( options ),
    true
  );
  /* Sending an ACK at this place before the ERROR would be OK, too. */
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_NO_USER,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  res = tftp_open(
    tftpfs_ipv4_loopback,
    tftpfs_file,
    true, /* is_for_reading */
    NULL, /* Config */
    &ctx->tftp_handle
  );
  T_eq_int( res, 0 );
  T_assert_not_null( ctx->tftp_handle );

  res = (int) tftp_write( ctx->tftp_handle, &res, 1 );
  T_eq_int( res, -EIO );

  res = tftp_close( ctx->tftp_handle );
  ctx->tftp_handle = NULL; /* Avoid that the fixture closes it again */
  T_eq_int( res, 0 );
  T_no_more_interactions();
}

/*
 * Attempt to read from a file open for writing using the TFTP client
 * interface.
 * Tests:
 *   * tftp_open() called with NULL for config uses
 *     default configuration values.
 *   * Read a file using only the TFTP client (i.e. not using the
 *     file system)
 *   * Attempt to read from a file open for writing is rejected with an error.
 */
T_TEST_CASE_FIXTURE( client_read_to_file_opened_for_writing, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int res = 0;
  uint16_t block_num = 1;
  size_t pos_in_file = 0;
  const char options[] =
    TFTP_OPTION_BLKSIZE "\0"
    RTEMS_XSTRING( TFTP_DEFAULT_BLOCK_SIZE ) "\0"
    TFTP_OPTION_WINDOWSIZE"\0"
    RTEMS_XSTRING( TFTP_DEFAULT_WINDOW_SIZE );

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    TFTP_DEFAULT_BLOCK_SIZE,
    TFTP_DEFAULT_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    options,
    sizeof( options ),
    true
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    0, /* Data size */
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  pos_in_file += 0;
  _Tftp_Add_interaction_recv_ack(
     TFTP_FIRST_FD,
     FIRST_TIMEOUT_MILLISECONDS,
     SERV_PORT,
     tftpfs_ipv4_loopback,
     block_num++,
     true
   );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  res = tftp_open(
    tftpfs_ipv4_loopback,
    tftpfs_file,
    false, /* is_for_reading */
    NULL, /* config */
    &ctx->tftp_handle
  );
  T_eq_int( res, 0 );
  T_assert_not_null( ctx->tftp_handle );

  res = (int) tftp_read( ctx->tftp_handle, &res, 1 );
  T_eq_int( res, -EIO );

  res = tftp_close( ctx->tftp_handle );
  ctx->tftp_handle = NULL; /* Avoid that the fixture closes it again */
  T_eq_int( res, 0 );
  T_no_more_interactions();
}

/*
 * Write a file to the server using the TFTP client interface.
 * The test uses the default options.
 * The file is 2 and a half data packet long.  No timeouts, packet loss, ...
 * Tests:
 *   * The default options (windowsize = 8 and blocksize = 1456) are used.
 *   * tftp_open() is called with default configuration values.
 *   * The test writes a file using only the TFTP client (i.e. not using the
 *     file system)
 *   * The code supports the use of a server name instead of an IP address.
 *   * The first window is also the last window.
 *   * The only ACK packet is the one at the end of window.
 *   * Between sending data packets, the client checks whether any packets
 *     are received.
 *   * The client handles files correctly which end in the middle of a window.
 */
T_TEST_CASE_FIXTURE( client_write_simple_file, &fixture_default_options )
{
  tftp_test_context *ctx = T_fixture_context();
  tftp_net_config config;
  int bytes_written;
  uint16_t block_num = 1;
  size_t pos_in_file = 0;
  const char options[] =
    TFTP_OPTION_BLKSIZE "\0"
    RTEMS_XSTRING( TFTP_DEFAULT_BLOCK_SIZE ) "\0"
    TFTP_OPTION_WINDOWSIZE "\0"
    RTEMS_XSTRING( TFTP_DEFAULT_WINDOW_SIZE );

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_server0_ipv4,
    TFTP_DEFAULT_BLOCK_SIZE,
    TFTP_DEFAULT_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_server0_ipv4,
    options,
    sizeof( options ),
    true
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num++,
    pos_in_file,
    TFTP_DEFAULT_BLOCK_SIZE,
    get_file_content,
    SERV_PORT,
    tftpfs_server0_ipv4,
    true
  );
  pos_in_file += TFTP_DEFAULT_BLOCK_SIZE;
  _Tftp_Add_interaction_recv_nothing(
    TFTP_FIRST_FD,
    DO_NOT_WAIT_FOR_ANY_TIMEOUT
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num++,
    pos_in_file,
    TFTP_DEFAULT_BLOCK_SIZE,
    get_file_content,
    SERV_PORT,
    tftpfs_server0_ipv4,
    true
  );
  pos_in_file += TFTP_DEFAULT_BLOCK_SIZE;
  _Tftp_Add_interaction_recv_nothing(
    TFTP_FIRST_FD,
    DO_NOT_WAIT_FOR_ANY_TIMEOUT
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    TFTP_DEFAULT_BLOCK_SIZE / 2, /* Data bytes in this block */
    get_file_content,
    SERV_PORT,
    tftpfs_server0_ipv4,
    true
  );
  pos_in_file += TFTP_DEFAULT_BLOCK_SIZE / 2;
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_server0_ipv4,
    block_num++,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  tftp_initialize_net_config( &config );
  bytes_written = rdwt_tftp_client_file(
    tftpfs_server0_name,
    tftpfs_file,
    false, /* is_for_reading */
    pos_in_file, /* file_size for writing files only */
    &config,
    &ctx->tftp_handle
  );
  T_eq_sz( bytes_written, pos_in_file );
  T_eq_int( errno, 0 );
  T_no_more_interactions();
}

/*
 * Test cases for the file system interface
 */

/*
 * Read a file from the server using only RFC1350.
 * The file is two and a half data packet long.  No timeouts, packet loss, ...
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The first and second data packet are full.
 *   * The third data packet signals the end of transfer.
 *   * Read the file from file system in one big chunk of exactly
 *     the size of the file.
 */
T_TEST_CASE_FIXTURE( read_simple_file, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  uint16_t block_num = 1;
  size_t pos_in_file = 0;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE;
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num++,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE;
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num++,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE / 2, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE / 2;
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num++,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    /* Bytes read per call to read() */
    2 * TFTP_RFC1350_BLOCK_SIZE + TFTP_RFC1350_BLOCK_SIZE / 2,
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( bytes_read, pos_in_file );
  T_no_more_interactions();
}

/*
 * Read a file from the server using only RFC1350.
 * The file is one byte long.  No timeouts, packet loss, ...
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The first data packet is not full and signals the end of the transfer.
 *   * The test reads a file from the file system in one-byte chunks.
 */
T_TEST_CASE_FIXTURE( read_tiny_file, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  uint16_t block_num = 1;
  size_t pos_in_file = 0;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    1, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += 1;
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num++,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    1, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( bytes_read, pos_in_file );
  T_no_more_interactions();
}

/*
 * Read a file from the server using only RFC1350.
 * The file is one data packet long.  No timeouts, packet loss, ...
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The first data packet is full.
 *   * The second data packet is empty and signals the end of the transfer.
 *   * The client handles an empty data packet correctly as end
 *     of file indicator.
 *   * The test reads a file from the file system in chunks of three quarters
 *     of the block size.
 */
T_TEST_CASE_FIXTURE( read_one_block_file, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  uint16_t block_num = 1;
  size_t pos_in_file = 0;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE;
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num++,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    0, /* Number of bytes transferred */
    get_file_content,
    true
  );
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num++,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    /* Bytes read per call to read() */
    TFTP_RFC1350_BLOCK_SIZE / 4 * 3,
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( bytes_read, pos_in_file );
  T_no_more_interactions();
}

/*
 * Read a file from the server using only RFC1350.
 * The file is one data packet long.
 * The client receives stray packets:
 *   * A packet from an unknown server (wrong TID)
 *   * An ACK packet instead of a DATA packet
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The first data packet is full.
 *   * The next received packet originates from a wrong TID
 *     (i.e. wrong connection).
 *   * Upon reception of a packet with a wrong TID, the client sends
 *     an ERROR message with code 5 and does not terminate the current
 *     transfer.
 *   * The final received packet is an ACK packet instead or the expected
 *     DATA packet.
 *   * Upon the reception of an unexpected packet, the client terminates
 *     the connection by sending an error packet to the server.
 */
T_TEST_CASE_FIXTURE( read_file_stray_packets, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  uint16_t block_num = 1;
  size_t pos_in_file = 0;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE;
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num++,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT + 1, /* Stray packet with wrong server TID */
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_UNKNOWN_ID,
    SERV_PORT + 1,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_ack( /* Stray ACK packet */
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num - 1,
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_ILLEGAL,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    /* Bytes read per call to read() */
    TFTP_RFC1350_BLOCK_SIZE / 4 * 3,
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( errno, EPROTO );
  /*
   * The client received one packet with TFTP_RFC1350_BLOCK_SIZE
   * before the error occurred.  The test reads in chunks of
   * TFTP_RFC1350_BLOCK_SIZE /4 * 3.  Thus, after the first chunk
   * the client signals the error to the test.
   *
   * It would be a little improvement if the client would return all
   * bytes received before signaling the error.
   */
  T_eq_int( bytes_read, TFTP_RFC1350_BLOCK_SIZE / 4 * 3 );
  T_no_more_interactions();
}

/*
 * Read a file from the server using only RFC1350.
 * The file is one data packet long.
 * The server sends an error message after the first DATA packet.
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The client uses a short time out for all packets.
 *   * The client handles error packets from the server and stops the
 *     connection by signaling an error to the user on the file system side.
 *   * The test reads a file from the file system in chunks of three quarters
 *     of the block size.
 */
T_TEST_CASE_FIXTURE( read_one_block_file_server_error, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  uint16_t block_num = 1;
  size_t pos_in_file = 0;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE;
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num++,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_error(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    TFTP_ERROR_CODE_NO_ACCESS,
    "Cannot read more",
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    /* Bytes read per call to read() */
    TFTP_RFC1350_BLOCK_SIZE / 4 * 3,
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( bytes_read, TFTP_RFC1350_BLOCK_SIZE / 4 * 3 );
  T_eq_int( errno, EPERM );
  T_no_more_interactions();
}

/*
 * Read a file from the server using only RFC1350.
 * The file is one data packet long.
 * The server sends a malformed error packet after the first DATA packet.
 * The error message in the packet is not a 0 terminated string
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The client handles malformed errors from the server and does not crash.
 *   * The test reads a file from the file system in chunks of three quarters
 *     of the block size.
 */
T_TEST_CASE_FIXTURE( read_one_block_file_malformed_server_error, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  uint16_t block_num = 1;
  size_t pos_in_file = 0;
  static const uint8_t packet_malformed_error[] = {
  0x00, 0x05, /* Opcode = TFTP_OPCODE_ERROR */
  0x00, 0x02, /* Error code = TFTP_ERROR_CODE_NO_ACCESS */
  'n', 'o', ' ', 'a', 'c', 'c', 'e', 's', 's' /* missing '\0' at the end */
  };

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE;
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num++,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_raw(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    sizeof( packet_malformed_error ), /* Malformed ERROR packet */
    packet_malformed_error,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    /* Bytes read per call to read() */
    TFTP_RFC1350_BLOCK_SIZE / 4 * 3,
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( bytes_read, TFTP_RFC1350_BLOCK_SIZE / 4 * 3 );
  T_eq_int( errno, EPERM );
  T_no_more_interactions();
}

/*
 * Read a file from the server using only RFC1350.
 * The reader on the file system side stops after having read half a
 * data packet and before having received the whole file and closes the file.
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The first data packet is full.
 *   * The client handles the closing of the file by the user correctly.
 *   * The client sends an error to the server after the user stops reading
 *     the file.
 *   * The test reads a file from the file system in chunks of block size.
 */
T_TEST_CASE_FIXTURE( read_one_block_close_file, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  uint16_t block_num = 1;
  size_t pos_in_file = 0;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  /* Sending an ACK at this place before the ERROR would be OK, too. */
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_NO_USER,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    TFTP_RFC1350_BLOCK_SIZE / 4, /* Bytes read per call to read() */
    TFTP_RFC1350_BLOCK_SIZE / 2, /* Max bytes read from this file */
    &ctx->fd0
  );
  T_eq_int( bytes_read, TFTP_RFC1350_BLOCK_SIZE / 2 );
  T_no_more_interactions();
}

/*
 * Read a file from the server using only RFC1350.
 * The reader on the file system side just open()s and then immediately closes
 * the file without ever reading a single byte.
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The first data packet is full.
 *   * The client handles the closing of the file by the user correctly.
 *   * The client sends an error to the server when the user closes the file.
 */
T_TEST_CASE_FIXTURE( read_close_file_immediately, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  uint16_t block_num = 1;
  size_t pos_in_file = 0;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  /* Sending an ACK at this place before the ERROR would be OK, too. */
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_NO_USER,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    TFTP_RFC1350_BLOCK_SIZE / 4, /* Bytes read per call to read() */
    0, /* Max bytes read from this file */
    &ctx->fd0
  );
  T_eq_int( bytes_read, 0 );
  T_no_more_interactions();
}

/*
 * Read an empty file from the server using only RFC1350.
 * No timeouts, packet loss, ...
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of a server name instead of an IP address.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The client uses a short time out for all packets.
 *   * The first data packet has length 0.
 *   * The client can read empty files from the server.
 *   * The test reads a file from the file system in one big chunk which
 *     is larger than the file.
 */
T_TEST_CASE_FIXTURE( read_empty_file, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  uint16_t block_num = 1;
  size_t pos_in_file = 0;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_server0_ipv4,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_server0_ipv4,
    block_num,
    pos_in_file,
    0, /* Number of bytes transferred */
    get_file_content,
    true
  );
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num++,
    SERV_PORT,
    tftpfs_server0_ipv4,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_server0_name, tftpfs_file ),
    /* Bytes read per call to read() */
    TFTP_RFC1350_BLOCK_SIZE,
    SIZE_MAX,
    &ctx->fd0
  );
  T_assert_eq_int( bytes_read, pos_in_file );
  T_no_more_interactions();
}

/*
 * Read an empty file from the server using only RFC1350.
 * The first two RRQ packets are lost.
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of a server name instead of an IP address.
 *   * The first packet is sent to standard port 69 of server.
 *   * The client uses a short time out for first packets.
 *   * The client uses a longer time out for repeated packets.
 *   * The client repeats lost RRQs packets.
 *   * The client does not repeat the ACK packet for the last DATA packet
 *     which signals the end of transfer.
 *   * The first data packet is empty and signals the end of the transfer.
 *   * It is possible to read a file with 0 bytes content from
 *     the file system.
 */
T_TEST_CASE_FIXTURE( read_empty_file_looing_rrq, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  uint16_t block_num = 1;
  size_t pos_in_file = 0;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_server0_ipv4,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_nothing(
    TFTP_FIRST_FD, /* Timeout: No packet received within timeout period */
    FIRST_TIMEOUT_MILLISECONDS
  );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_server0_ipv4,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_nothing(
    TFTP_FIRST_FD, /* Timeout: No packet received within timeout period */
    TIMEOUT_MILLISECONDS
  );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_server0_ipv4,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_server0_ipv4,
    block_num,
    pos_in_file,
    0, /* Number of bytes transferred */
    get_file_content,
    true
  );
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num++,
    SERV_PORT,
    tftpfs_server0_ipv4,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_server0_name, tftpfs_file ),
    /* Bytes read per call to read() */
    TFTP_RFC1350_BLOCK_SIZE,
    SIZE_MAX,
    &ctx->fd0
  );
  T_assert_eq_int( bytes_read, pos_in_file );
  T_no_more_interactions();
}

/*
 * Read a file from the server using only RFC1350.
 * The file is one and a half data packet long.
 * Two data packet are lost (timeout) and the client must repeat the ACK.
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * The client uses a short time out for first packets.
 *   * The client uses a longer time out for repeated packets.
 *   * The client repeats the ACK packets which are supposed
 *     to be lost.
 *   * The client does not repeat the ACK packet for the last DATA packet
 *     which signals the end of transfer.
 *   * The test reads a file in chunks of 17 bytes from file system.
 */
T_TEST_CASE_FIXTURE( read_small_file_lost_packets, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  uint16_t block_num = 1;
  size_t pos_in_file = 0;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE;
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_nothing(
    TFTP_FIRST_FD, /* Timeout: No packet received within timeout period */
    FIRST_TIMEOUT_MILLISECONDS
  );
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_nothing(
    TFTP_FIRST_FD, /* Timeout: No packet received within timeout period */
    TIMEOUT_MILLISECONDS
  );
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num++,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE / 2, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE / 2;
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num++,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    17, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( bytes_read, pos_in_file );
  T_no_more_interactions();
}

/*
 * Read a file from the server using only RFC1350.
 * The file is exactly one data packet long.
 * The client receives a malformed DATA packet (wrong op code).
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The client uses a short time out for all packets.
 *   * The first data packet is full.
 *   * The client terminates the connection with an error message upon
 *     reception of a malformed packet.
 */
T_TEST_CASE_FIXTURE( read_small_file_malformed_packet_1, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  uint16_t block_num = 1;
  size_t pos_in_file = 0;
  static const uint8_t packet_illegal_opcode_1[] = { 0x00, 0xFF, 0x00, 0x00 };

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE;
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_raw(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    sizeof( packet_illegal_opcode_1 ), /* Malformed DATA packet */
    packet_illegal_opcode_1,
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_ILLEGAL,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    TFTP_RFC1350_BLOCK_SIZE / 4, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( bytes_read, pos_in_file );
  T_eq_int( errno, EPROTO );
  T_no_more_interactions();
}

/*
 * Read a file from the server using only RFC1350.
 * The file is exactly one data packet long.
 * The client receives a malformed DATA packet (wrong op code).
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The first data packet is full.
 *   * The second data packet is malformed.
 *   * The client terminates the connection with an error message upon
 *     reception of a malformed packet.
 */
T_TEST_CASE_FIXTURE( read_small_file_malformed_packet_2, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  uint16_t block_num = 1;
  size_t pos_in_file = 0;
  static const uint8_t packet_illegal_opcode_2[] = { 0x03, 0x00, 0x00, 0x01 };

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE;
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_raw(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    sizeof( packet_illegal_opcode_2 ), /* Malformed DATA packet */
    packet_illegal_opcode_2,
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_ILLEGAL,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    TFTP_RFC1350_BLOCK_SIZE / 4, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( bytes_read, pos_in_file );
  T_eq_int( errno, EPROTO );
  T_no_more_interactions();
}

/*
 * Read a file from the server using only RFC1350.
 * The file is exactly one data packet long.
 * The client receives a malformed packet.
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The first DATA packet received after the RRQ packet is malformed.
 *     It is too short with only one byte length.
 *   * The client sends an error and terminates the file transfer upon
 *     reception of a malformed packet.
 */
T_TEST_CASE_FIXTURE( read_file_malformed_ack_1, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  size_t pos_in_file = 0;
  static const uint8_t packet_too_short_1[] = { 0x03 };

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_raw(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    sizeof( packet_too_short_1 ), /* Malformed DATA packet */
    packet_too_short_1,
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_ILLEGAL,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    TFTP_RFC1350_BLOCK_SIZE / 4, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( bytes_read, pos_in_file );
  T_eq_int( errno, EPROTO );
  T_no_more_interactions();
}

/*
 * Read a file from the server using only RFC1350.
 * The file is exactly one data packet long.
 * The client receives a malformed packet.
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The first DATA packet received after the RRQ packet is malformed.
 *     It is too short with only two bytes length.
 *   * The client sends an error and terminates the file transfer upon
 *     reception of a malformed packet.
 */
T_TEST_CASE_FIXTURE( read_file_malformed_ack_2, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  size_t pos_in_file = 0;
  static const uint8_t packet_too_short_2[] = { 0x00, 0x03 };

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_raw(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    sizeof( packet_too_short_2 ), /* Malformed DATA packet */
    packet_too_short_2,
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_ILLEGAL,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    TFTP_RFC1350_BLOCK_SIZE / 4, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( bytes_read, pos_in_file );
  T_eq_int( errno, EPROTO );
  T_no_more_interactions();
}

/*
 * Read a file from the server using only RFC1350.
 * The file is exactly one data packet long.
 * The client receives a malformed packet.
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The first DATA packet received after the RRQ packet is malformed.
 *     It is too short with only three bytes length.
 *   * The client sends an error and terminates the file transfer upon
 *     reception of a malformed packet.
 */
T_TEST_CASE_FIXTURE( read_file_malformed_ack_3, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  size_t pos_in_file = 0;
  static const uint8_t packet_too_short_3[] = { 0x00, 0x03, 0x00 };

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_raw(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    sizeof( packet_too_short_3 ), /* Malformed DATA packet */
    packet_too_short_3,
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_ILLEGAL,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    TFTP_RFC1350_BLOCK_SIZE / 4, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( bytes_read, pos_in_file );
  T_eq_int( errno, EPROTO );
  T_no_more_interactions();
}

/*
 * Read a file from the server using only RFC1350.
 * The file is exactly one data packet long.
 * The client receives a data packet with block number 0.
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The first DATA packet received after the RRQ packet is malformed.
 *     It has block number 0.
 *   * The client sends an error and terminates the file transfer upon
 *     reception of a malformed packet.
 */
T_TEST_CASE_FIXTURE( read_file_block_number_0, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  size_t pos_in_file = 0;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    0, /* Wrong block number 0 */
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_ILLEGAL,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    TFTP_RFC1350_BLOCK_SIZE / 4, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( bytes_read, pos_in_file );
  T_eq_int( errno, EPROTO );
  T_no_more_interactions();
}

/*
 * Read a file from the server using only RFC1350.
 * The file is exactly one data packet long.
 * The client receives a malformed packet.
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The first DATA packet received after the RRQ packet is malformed.
 *     The packet contains an illegal op code.
 *   * The client sends an error and terminates the file transfer upon
 *     reception of a malformed packet.
 */
T_TEST_CASE_FIXTURE( read_file_illegal_opcode_1, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  size_t pos_in_file = 0;
  static const uint8_t packet_illegal_opcode_1[] = { 0x00, 0xFF, 0x00, 0x00 };

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_raw(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    sizeof( packet_illegal_opcode_1 ), /* Malformed DATA packet */
    packet_illegal_opcode_1,
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_ILLEGAL,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    TFTP_RFC1350_BLOCK_SIZE / 4, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( bytes_read, pos_in_file );
  T_eq_int( errno, EPROTO );
  T_no_more_interactions();
}

/*
 * Read a file from the server using only RFC1350.
 * The file is exactly two data packet long.
 * The client receives DATA packets with wrong block numbers.
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * The second RRQ is sent to the TFTP server port 69 and not to the
 *     port from which the first packet with the wrong block number came from.
 *   * The client uses a short time out for all packets.
 *   * The client uses a longer time out for repeated packets.
 *   * The client handles DATA packets with the wrong block numbers
 *     appropriately.
 *   * The third data packet is empty and signals the end of the transfer.
 *   * Old data packets are ignored (i.e. do not cause a retransmission).
 *   * Duplicates of the last data packet cause a retransmission of the
 *     last ACK.
 *   * The first data packet with a block number larger than the expected one,
 *     cause a retransmission of ACK or RRQ.  (They can appear together
 *     with the windowsize option.)
 *   * The test reads a file from the file system in one big chunk with is larger
 *     than the files size.
 */
T_TEST_CASE_FIXTURE( read_two_block_file_wrong_block_numbers, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  uint16_t block_num = 1;
  size_t pos_in_file = 0;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num + 1, /* Wrong block number */
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num + 1, /* Wrong block number */
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE;
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num++,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num - 1, /*  Wrong block number / duplicates last packet */
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num - 1, /* Client assumes last ACK got lost and retransmits it. */
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num + 1, /*  Wrong block number */
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num - 1, /* Client assumes last ACK got lost and retransmits it. */
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num + 1, /*  Wrong block number */
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE;
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num++,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num - 2, /*  Wrong block number */
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num + 1, /*  Wrong block number */
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num - 1, /* Client assumes last ACK got lost and retransmits it. */
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    0, /* Number of bytes transferred */
    get_file_content,
    true
  );
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num++,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    /* Bytes read per call to read() */
    3 * TFTP_RFC1350_BLOCK_SIZE,
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( bytes_read, pos_in_file );
  T_no_more_interactions();
}

/*
 * Attempt to read a file from the server using filename without ':'.
 * Tests:
 *   * The TFTP FS rejects malformed file names (i.e. it does not crash).
 */
T_TEST_CASE_FIXTURE( read_malformed_filename, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  char buffer[100];
  int len;

  len = snprintf(
    buffer,
    sizeof( buffer ),
    "%s/%s",
    tftpfs_mount_point,
    tftpfs_server0_name
  );

  T_quiet_gt_int( len, 0 );
  T_quiet_lt_int( len, (int) sizeof( buffer ) );

  bytes_read = read_tftp_file(
    buffer,
    /* Bytes read per call to read() */
    TFTP_RFC1350_BLOCK_SIZE,
    SIZE_MAX,
    &ctx->fd0
  );
  T_assert_eq_int( bytes_read, 0 );
  T_assert_eq_int( errno, EINVAL );
}

/*
 * Attempt to read a file from a none exiting server address.
 * Tests:
 *   * TFTP FS returns an error if the server name cannot be resolved.
 */
T_TEST_CASE_FIXTURE( read_from_unknown_ip_address, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;

  bytes_read = read_tftp_file(
    create_tftpfs_path( "not-existing-server-address", tftpfs_file ),
    /* Bytes read per call to read() */
    TFTP_RFC1350_BLOCK_SIZE,
    SIZE_MAX,
    &ctx->fd0
  );
  T_assert_eq_int( bytes_read, 0 );
  T_assert_eq_int( errno, ENOENT );
}

/*
 * Attempt to read a file which the server does not know
 * No timeouts, packet loss, ...
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of a server name instead of an IP address.
 *   * The client handles an ERROR packet received upon sending an RRQ
 *     correctly.
 *   * TFTP FS returns an error upon the reception of the ERROR packet.
 */
T_TEST_CASE_FIXTURE( read_not_existing_file, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_server0_ipv4,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_error(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_server0_ipv4,
    TFTP_ERROR_CODE_NOT_FOUND,
    "No such file",
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_server0_name, tftpfs_file ),
    /* Bytes read per call to read() */
    TFTP_RFC1350_BLOCK_SIZE,
    SIZE_MAX,
    &ctx->fd0
  );
  T_assert_eq_int( bytes_read, 0 );
  T_assert_eq_int( errno, ENOENT );
  T_no_more_interactions();
}

/*
 * Write an empty file to the server using only RFC1350.
 * The first two WRQ as well as the first two DATA packets are lost.
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of a server name instead of an IP address.
 *   * The all WRQ are sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The client uses a short time out for first packets.
 *   * The client uses a longer time out for repeated packets.
 *   * The client repeats the WRQs and DATA packets which are supposed
 *     to be lost.
 *   * When a timeout occurs, the client repeats the last and empty packet.
 *   * The first data packet is empty and signals the end of the transfer.
 *   * The test writes a file with 0 bytes content to the file system.
 */
T_TEST_CASE_FIXTURE( write_empty_file_packet_losts, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_written;
  uint16_t block_num = 0;
  size_t pos_in_file = 0;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_server0_ipv4,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_nothing(
    TFTP_FIRST_FD, /* Timeout: No packet received within timeout period */
    FIRST_TIMEOUT_MILLISECONDS
  );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_server0_ipv4,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_nothing(
    TFTP_FIRST_FD, /* Timeout: No packet received within timeout period */
    TIMEOUT_MILLISECONDS
  );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_server0_ipv4,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_server0_ipv4,
    block_num++,
    true
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    0, /* Number of bytes transferred */
    get_file_content,
    SERV_PORT,
    tftpfs_server0_ipv4,
    true
  );
  _Tftp_Add_interaction_recv_nothing(
    TFTP_FIRST_FD, /* Timeout: No packet received within timeout period */
    FIRST_TIMEOUT_MILLISECONDS
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    0, /* Number of bytes transferred */
    get_file_content,
    SERV_PORT,
    tftpfs_server0_ipv4,
    true
  );
  _Tftp_Add_interaction_recv_nothing(
    TFTP_FIRST_FD, /* Timeout: No packet received within timeout period */
    TIMEOUT_MILLISECONDS
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    0, /* Number of bytes transferred */
    get_file_content,
    SERV_PORT,
    tftpfs_server0_ipv4,
    true
  );
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_server0_ipv4,
    block_num++,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_written = write_tftp_file(
    create_tftpfs_path( tftpfs_server0_name, tftpfs_file ),
    0, /* Size of file */
    4, /* Bytes written per call to write() */
    &ctx->fd0
  );
  T_eq_int( bytes_written, pos_in_file );
  T_no_more_interactions();
}

/*
 * Write a very short file to the server using only RFC1350.
 * The file is one and half data packets long.
 * The first two DATA packets and one ACK packet are lost.
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of a server name instead of an IP address.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The client uses a short time out for first packets.
 *   * The client uses a longer time out for repeated packets.
 *   * The client repeats sending DATA packets which are supposed
 *     to be lost.
 *   * The client also repeats the last DATA packet when it is supposed
 *     to be lost.
 *   * The client repeats sending DATA packets when an ACK packet is repeated
 *     (presumably the DATA packet has been lost).
 *   * The test writes a file to the file system in chunks of a quarter of the block size.
 */
T_TEST_CASE_FIXTURE( write_tiny_file_packet_losts, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_written;
  uint16_t block_num = 0;
  size_t pos_in_file = 0;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_server0_ipv4,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_server0_ipv4,
    block_num++,
    true
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE,
    get_file_content,
    SERV_PORT,
    tftpfs_server0_ipv4,
    true
  );
  _Tftp_Add_interaction_recv_nothing(
    TFTP_FIRST_FD, /* Timeout: No packet received within timeout period */
    FIRST_TIMEOUT_MILLISECONDS
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE,
    get_file_content,
    SERV_PORT,
    tftpfs_server0_ipv4,
    true
  );
  _Tftp_Add_interaction_recv_nothing(
    TFTP_FIRST_FD, /* Timeout: No packet received within timeout period */
    TIMEOUT_MILLISECONDS
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE,
    get_file_content,
    SERV_PORT,
    tftpfs_server0_ipv4,
    true
  );
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_server0_ipv4,
    block_num++,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE;
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE / 2,
    get_file_content,
    SERV_PORT,
    tftpfs_server0_ipv4,
    true
  );
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_server0_ipv4,
    block_num - 1, /* Repeated ACK packet received */
    true
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE / 2,
    get_file_content,
    SERV_PORT,
    tftpfs_server0_ipv4,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE / 2;
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_server0_ipv4,
    block_num++,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_written = write_tftp_file(
    create_tftpfs_path( tftpfs_server0_name, tftpfs_file ),
    TFTP_RFC1350_BLOCK_SIZE / 2 * 3, /* Size of file */
    TFTP_RFC1350_BLOCK_SIZE / 4, /* Bytes written per call to write() */
    &ctx->fd0
  );
  T_eq_int( bytes_written, pos_in_file );
  T_no_more_interactions();
}

/*
 * Write a file to the server using only RFC1350.
 * The file is 2 data packet and 1 byte long.  No timeouts, packet loss, ...
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * First and second data packet is full.
 *   * Third data packet signals the end of transfer.
 *   * The test writes a file to the file system in one big chunk
 *     of exactly the files size.
 */
T_TEST_CASE_FIXTURE( write_simple_file, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_written;
  uint16_t block_num = 0;
  size_t pos_in_file = 0;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE,
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE;
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE,
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE;
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    1, /* Data bytes in this block */
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  pos_in_file += 1;
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_written = write_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    pos_in_file, /* Size of file */
    pos_in_file, /* Bytes written per call to write() */
    &ctx->fd0
  );
  T_eq_int( bytes_written, pos_in_file );
  T_no_more_interactions();
}

/*
 * Write a file to the server using only RFC1350.
 * As response to the first DATA packet, the server sends an error packet.
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The first data packet is full.
 *   * The second packet from the server is an error packet.
 *   * The TFTP client ends the connection after receiving an error packet.
 *   * The test writes a file to the file system with a call to write()
 *     for each byte.
 */
T_TEST_CASE_FIXTURE( write_simple_file_disk_full, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_written;
  uint16_t block_num = 0;
  size_t pos_in_file = 0;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE,
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE;
  _Tftp_Add_interaction_recv_error(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    TFTP_ERROR_CODE_DISK_FULL,
    "disk full",
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_written = write_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    pos_in_file, /* Size of file */
    1, /* Bytes written per call to write() */
    &ctx->fd0
  );
  T_eq_int( errno, ENOSPC );
  T_eq_int( bytes_written, pos_in_file - 1 );
  T_no_more_interactions();
}

/*
 * Write a file to the server using only RFC1350.
 * The file is one and a half data packet long.
 * The server sends a malformed packet.
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The first ACK to the WRQ packet is malformed.
 *     It is only one byte long.
 *   * The client sends an error upon the reception of a malformed packet
 *     and terminates the file transfer.
 */
T_TEST_CASE_FIXTURE( write_file_malformed_ack_1, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_written;
  size_t pos_in_file = 0;
  static const uint8_t packet_too_short_1[] = { 0x04 };

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_raw(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    sizeof( packet_too_short_1 ), /* Malformed ACK packet */
    packet_too_short_1,
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_ILLEGAL,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_written = write_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    pos_in_file, /* Size of file */
    17, /* Bytes written per call to write() */
    &ctx->fd0
  );
  T_eq_int( bytes_written, -1 );
  T_eq_int( errno, EPROTO );
  T_no_more_interactions();
}

/*
 * Write a file to the server using only RFC1350.
 * The file is one and a half data packet long.
 * The server sends a malformed packet.
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The first ACK to the WRQ packet is malformed.
 *     It is only two bytes long.
 *   * The client sends an error upon the reception of a malformed packet
 *     and terminates the file transfer.
 */
T_TEST_CASE_FIXTURE( write_file_malformed_ack_2, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_written;
  size_t pos_in_file = 0;
  static const uint8_t packet_too_short_2[] = { 0x00, 0x04 };

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_raw(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    sizeof( packet_too_short_2 ), /* Malformed ACK packet */
    packet_too_short_2,
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_ILLEGAL,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_written = write_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    pos_in_file, /* Size of file */
    17, /* Bytes written per call to write() */
    &ctx->fd0
  );
  T_eq_int( bytes_written, -1 );
  T_eq_int( errno, EPROTO );
  T_no_more_interactions();
}

/*
 * Write a file to the server using only RFC1350.
 * The file is one and a half data packet long.
 * The server sends a malformed packet.
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The first ACK to the WRQ packet is malformed.
 *     It is only three bytes long.
 *   * The client sends an error upon the reception of a malformed packet
 *     and terminates the file transfer.
 */
T_TEST_CASE_FIXTURE( write_file_malformed_ack_3, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_written;
  size_t pos_in_file = 0;
  static const uint8_t packet_too_short_3[] = { 0x00, 0x04, 0x00 };

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_raw(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    sizeof( packet_too_short_3 ), /* Malformed ACK packet */
    packet_too_short_3,
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_ILLEGAL,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_written = write_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    pos_in_file, /* Size of file */
    17, /* Bytes written per call to write() */
    &ctx->fd0
  );
  T_eq_int( bytes_written, -1 );
  T_eq_int( errno, EPROTO );
  T_no_more_interactions();
}

/*
 * Write a file to the server using only RFC1350.
 * The file is one and a half data packet long.
 * The server sends a malformed packet.
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The first ACK to the WRQ packet is malformed.
 *     The packet contains an illegal op code.
 *   * The client sends an error upon the reception of a malformed packet
 *     and terminates the file transfer.
 */
T_TEST_CASE_FIXTURE( write_file_illegal_opcode_1, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_written;
  size_t pos_in_file = 0;
  static const uint8_t packet_illegal_opcode_1[] = { 0x00, 0xFF, 0x00, 0x00 };

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_raw(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    sizeof( packet_illegal_opcode_1 ), /* Malformed ACK packet */
    packet_illegal_opcode_1,
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_ILLEGAL,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_written = write_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    pos_in_file, /* Size of file */
    17, /* Bytes written per call to write() */
    &ctx->fd0
  );
  T_eq_int( bytes_written, -1 );
  T_eq_int( errno, EPROTO );
  T_no_more_interactions();
}

/*
 * Write a file to the server using only RFC1350.
 * The server sends a malformed packet.
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * A malformed ACK packet is received by the TFTP client.
 *     The packet is only three bytes long.
 *   * The client sends an error upon the reception of a malformed packet
 *     and terminates the file transfer.
 */
T_TEST_CASE_FIXTURE( write_short_file_malformed_ACK_1, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_written;
  uint16_t block_num = 0;
  size_t pos_in_file = 0;
  static const uint8_t packet_too_short_3[] = { 0x00, 0x04, 0x00 };

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE,
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_raw(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    sizeof( packet_too_short_3 ), /* Malformed ACK packet */
    packet_too_short_3,
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_ILLEGAL,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_written = write_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    2 * TFTP_RFC1350_BLOCK_SIZE, /* Size of file */
    17, /* Bytes written per call to write() */
    &ctx->fd0
  );
  T_eq_int( errno, EPROTO );
  T_eq_int( bytes_written, 510 );
  T_no_more_interactions();
}

/*
 * Write a file to the server using only RFC1350.
 * The server sends a malformed packet.
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * A malformed ACK packet is received by the TFTP client after the first
 *     DATA packet has been exchanged successfully.
 *     The packet is only one byte long.
 *   * The client sends an error upon the reception of a malformed packet
 *     and terminates the file transfer.
 */
T_TEST_CASE_FIXTURE( write_short_file_malformed_ACK_2, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_written;
  uint16_t block_num = 0;
  size_t pos_in_file = 0;
  static const uint8_t packet_too_short_1[] = { 0x04 };

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE,
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE;
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE / 4,
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE / 4;
  _Tftp_Add_interaction_recv_raw(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    sizeof( packet_too_short_1 ), /* Malformed ACK packet */
    packet_too_short_1,
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_ILLEGAL,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_written = write_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    pos_in_file, /* Size of file */
    17, /* Bytes written per call to write() */
    &ctx->fd0
  );
  T_eq_int( errno, EPROTO );
  T_eq_int( bytes_written, -1 );
  T_no_more_interactions();
}

/*
 * Write a file to the server using only RFC1350.
 * The server sends a malformed packet.
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * A malformed ACK packet is received by the TFTP client after the first
 *     DATA packet has been exchanged successfully.
 *     The packet contains an illegal op code.
 *   * The client sends an error upon the reception of a malformed packet
 *     and terminates the file transfer.
 */
T_TEST_CASE_FIXTURE( write_short_file_malformed_opcode, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_written;
  uint16_t block_num = 0;
  size_t pos_in_file = 0;
  static const uint8_t packet_illegal_opcode_2[] = { 0x04, 0x00, 0x00, 0x01 };

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE,
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_raw(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    sizeof( packet_illegal_opcode_2 ), /* Malformed ACK packet */
    packet_illegal_opcode_2,
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_ILLEGAL,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_written = write_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    2 * TFTP_RFC1350_BLOCK_SIZE, /* Size of file */
    17, /* Bytes written per call to write() */
    &ctx->fd0
  );
  T_eq_int( errno, EPROTO );
  T_eq_int( bytes_written, 510 );
  T_no_more_interactions();
}

/*
 * Write a file to the server using only RFC1350.
 * The file is two and a half data packet long.
 * The server sends packets with wrong block numbers.
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The first ACK packet contains a wrong block number.
 *   * The client repeats the WRQ upon reception of an ACK with
 *     an too high block number.
 *   * The client uses a short time out for waiting on the answer of the
 *     first WRQ or DATA packets.
 *   * The client uses a long time out for waiting on the answer of
 *     repeated WRQ or DATA packets.
 *   * The first DATA packet is full.
 *   * The second DATA packet signals the end of transfer.
 *   * The client handles DATA packets with wrong block numbers
 *     appropriately.
 *   * The test writes a file to the file system with calls to write() of
 *     exactly block size.
 */
T_TEST_CASE_FIXTURE( write_short_file_bad_block_numbers, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_written;
  uint16_t block_num = 0;
  size_t pos_in_file = 0;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num + 1, /* Wrong block number */
    true
  );
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE,
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num - 1, /* Wrong block number */
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE;
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE,
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num - 2, /* Wrong block number */
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE;
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE / 4,
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num - 2, /* Wrong block number */
    true
  );
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num + 1, /* Wrong block number */
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE / 4;
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_written = write_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    pos_in_file, /* Size of file */
    TFTP_RFC1350_BLOCK_SIZE, /* Bytes written per call to write() */
    &ctx->fd0
  );
  T_eq_int( bytes_written, pos_in_file );
  T_no_more_interactions();
}

/*
 * Write a file to the server using only RFC1350.
 * The file is one data packet long.
 * The client receives a stray packet from an unknown server (wrong TID).
 * Directly afterwards the expected ACK packet is lost and
 * the client must retransmit the original DATA packet.
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The code supports the use of an IPv4 address instead of a server name.
 *   * The first packet is sent to standard port 69 of server.
 *   * All other packets are sent to the port used for this connection.
 *   * The client uses a short time out for waiting on the answer of the
 *     first DATA packet.
 *   * The client uses a long time out for waiting on the answer of
 *     the repeated DATA.
 *   * Upon reception of a packet with a wrong TID, the client sends
 *     an ERROR message with code 5 but does not terminate the current
 *     transfer.
 *   * When re-transmitting the an packet, the data is intact (i.e.
 *     not corrupted by the reception of any packet in-between).
 */
T_TEST_CASE_FIXTURE( write_one_block_file_stray_packets, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_written;
  uint16_t block_num = 0;
  size_t pos_in_file = 0;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE,
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT + 1, /* Stray packet with wrong server TID */
    tftpfs_ipv4_loopback,
    block_num,
    0,
    TFTP_RFC1350_BLOCK_SIZE / 2, /* Number of bytes transferred */
    get_bad_file_content,
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_UNKNOWN_ID,
    SERV_PORT + 1,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_nothing(
    TFTP_FIRST_FD, /* Timeout: No packet received within timeout period */
    FIRST_TIMEOUT_MILLISECONDS
  );
  _Tftp_Add_interaction_send_data( /* Retransmission of the DATA packet */
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE,
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE;
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    0, /* Number of bytes */
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  pos_in_file += 0;
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_written = write_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    pos_in_file, /* Size of file */
    TFTP_RFC1350_BLOCK_SIZE, /* Bytes written per call to write() */
    &ctx->fd0
  );
  T_eq_int( bytes_written, pos_in_file );
  T_no_more_interactions();
}

/*
 * Read a file from the server using option to increase the block size.
 * The file is one data packet long.  No timeouts, packet loss, ...
 * Tests:
 *   * Only the blksize option appears in RRQ and OACK.
 *   * The client uses a block size which is larger than the default size.
 *   * The first data packet is full.
 *   * The second data packet is empty and signals the end of the transfer.
 *   * Client handles the empty data packet correctly as
 *     end of file indicator.
 *   * The test reads a file from the file system in chunks of block size.
 */
T_TEST_CASE_FIXTURE( read_file_one_large_block, &fixture_large_blocksize )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  uint16_t block_num = 0;
  size_t pos_in_file = 0;
  const char options[] =
    TFTP_OPTION_BLKSIZE "\0"
    RTEMS_XSTRING( LARGE_BLOCK_SIZE );

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    LARGE_BLOCK_SIZE,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    options,
    sizeof( options ),
    true
  );
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num++,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    LARGE_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += LARGE_BLOCK_SIZE;
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num++,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    0, /* Number of bytes transferred */
    get_file_content,
    true
  );
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num++,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    LARGE_BLOCK_SIZE, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( bytes_read, pos_in_file );
  T_no_more_interactions();
}

/*
 * Try to read a file from the server using a file name too large for a RRQ.
 * Tests:
 *   * The client rejects an attempt to open a file with a too long
 *     file name is with an error.
 */
T_TEST_CASE_FIXTURE( read_too_long_file_name, &fixture_default_options )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  char buffer[TFTP_RFC1350_BLOCK_SIZE -
              strlen( TFTP_MODE_OCTET ) - 1 - 5];
  int len;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  len = sizeof( buffer ) - strlen( tftpfs_mount_point ) -
        strlen( tftpfs_ipv4_loopback ) - 2 - 4;
  len = snprintf(
    buffer,
    sizeof( buffer ),
    "%s/%s:%0*d",
    tftpfs_mount_point,
    tftpfs_ipv4_loopback,
    len,
    123
  );
  T_quiet_gt_int( len, 0 );
  T_quiet_lt_int( len, (int) sizeof( buffer ) );

  bytes_read = read_tftp_file(
    buffer,
    TFTP_DEFAULT_BLOCK_SIZE, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( bytes_read, 0 );
  T_eq_int( errno, ENAMETOOLONG );
  T_no_more_interactions();
}

/*
 * Read a file using options but the server sends a DATA packet.
 * The file is one byte long.  No timeouts, packet loss, ...
 * Tests:
 *   * The client uses windowsize and blksize option in the RRQ.
 *   * For the data transfer the client uses the RFC1350 option values
 *     because the server responded with a DATA packet.
 *   * The whole package sequence behaves as if only RFC1350 was used.
 *   * The first data packet contains a single byte and signals the end of the transfer.
 *   * The test reads a file from the file system in chunks of half block size.
 */
T_TEST_CASE_FIXTURE( read_file_DATA_instead_of_OACK, &fixture_default_options )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  uint16_t block_num = 1;
  size_t pos_in_file = 0;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    TFTP_DEFAULT_BLOCK_SIZE,
    TFTP_DEFAULT_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    1, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += 1;
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num++,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    TFTP_DEFAULT_BLOCK_SIZE / 2, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( bytes_read, pos_in_file );
  T_no_more_interactions();
}

/*
 * Read a file using RFC1350 but the server sends an OACK packet.
 * Tests:
 *   * The code supports requests without options (RFC1350 only).
 *   * The server wrongly responds with an OACK which contains no options.
 *   * The client sends an error upon reception of an unexpected packet.
 */
T_TEST_CASE_FIXTURE( read_tiny_file_OACK_instead_of_DATA, &fixture_rfc1350 )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  const char options[] = {};

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    options,
    sizeof( options ),
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_ILLEGAL,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    1, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( bytes_read, 0 );
  T_eq_int( errno, EPROTO );
  T_no_more_interactions();
}

/*
 * Read a file from the server using the default options.
 * The file is 18 and a half data packet long.  No timeouts, packet loss, ...
 * Tests:
 *   * The client uses the default options
 *     (windowsize = 8 and blocksize = 1456).
 *   * The server send the options in the same order as the client did
 *     send them.
 *   * The ninetenth data packet signals the end of transfer.
 *   * The test reads a file from the file system in chunks of 2000 bytes.
 */
T_TEST_CASE_FIXTURE( read_file_with_default_options, &fixture_default_options )
{
  tftp_test_context *ctx = T_fixture_context();
  int i;
  int bytes_read;
  uint16_t block_num = 0;
  size_t pos_in_file = 0;
  const char options[] =
    TFTP_OPTION_BLKSIZE "\0"
    RTEMS_XSTRING( TFTP_DEFAULT_BLOCK_SIZE ) "\0"
    TFTP_OPTION_WINDOWSIZE"\0"
    RTEMS_XSTRING( TFTP_DEFAULT_WINDOW_SIZE );

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    TFTP_DEFAULT_BLOCK_SIZE,
    TFTP_DEFAULT_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    options,
    sizeof( options ),
    true
  );
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  while ( block_num < 16 ) {
    for ( i = 0; i < TFTP_DEFAULT_WINDOW_SIZE; ++i ) {
      _Tftp_Add_interaction_recv_data(
        TFTP_FIRST_FD,
        FIRST_TIMEOUT_MILLISECONDS,
        SERV_PORT,
        tftpfs_ipv4_loopback,
        ++block_num,
        pos_in_file,
        TFTP_DEFAULT_BLOCK_SIZE, /* Number of bytes transferred */
        get_file_content,
        true
      );
      pos_in_file += TFTP_DEFAULT_BLOCK_SIZE;
    }
    _Tftp_Add_interaction_send_ack(
      TFTP_FIRST_FD,
      block_num,
      SERV_PORT,
      tftpfs_ipv4_loopback,
      true
    );
  }
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    ++block_num,
    pos_in_file,
    TFTP_DEFAULT_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += TFTP_DEFAULT_BLOCK_SIZE;
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    ++block_num,
    pos_in_file,
    TFTP_DEFAULT_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += TFTP_DEFAULT_BLOCK_SIZE;
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    ++block_num,
    pos_in_file,
    TFTP_DEFAULT_BLOCK_SIZE / 2, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += TFTP_DEFAULT_BLOCK_SIZE / 2;
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    2000, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( bytes_read, pos_in_file );
  T_no_more_interactions();
}

/*
 * Read a file following exactly the scenario in RFC 7440.
 * This test uses window size and block size options.  There are lost packets.
 * Tests:
 *   * The client uses of non-default options
 *     (windowsize = 4 and blocksize = 12).
 *   * Test the scenario included in RFC 7440.
 *   * When a packet from the server is lost (client receives DATA packet with
 *     a too high block number), the client sends an ACK for the last package
 *     received in the correct sequence.
 *   * The client ignores duplicated packets (with block numbers it has
 *     already processed).
 *   * The data of the file ends exactly at a window size border (i.e.
 *     after the window a single empty DATA packet is sent by the server).
 *   * The test reads a file from the file system in chunks of 10 bytes.
 */
T_TEST_CASE_FIXTURE( read_file_rfc7440_scenario, &fixture_small_opt_size )
{
  tftp_test_context *ctx = T_fixture_context();
  int i;
  int bytes_read;
  uint16_t block_num = 0;
  size_t pos_in_file = 0;
  const char options[] =
    TFTP_OPTION_WINDOWSIZE"\0"
    RTEMS_XSTRING( SMALL_WINDOW_SIZE ) "\0"
    TFTP_OPTION_BLKSIZE "\0"
    RTEMS_XSTRING( SMALL_BLOCK_SIZE );

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    SMALL_BLOCK_SIZE,
    SMALL_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    options,
    sizeof( options ),
    true
  );
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  for ( i = 0; i < SMALL_WINDOW_SIZE; ++i ) {
    _Tftp_Add_interaction_recv_data(
      TFTP_FIRST_FD,
      FIRST_TIMEOUT_MILLISECONDS,
      SERV_PORT,
      tftpfs_ipv4_loopback,
      ++block_num,
      pos_in_file,
      SMALL_BLOCK_SIZE, /* Number of bytes transferred */
      get_file_content,
      true
    );
    pos_in_file += SMALL_BLOCK_SIZE;
  }
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    ++block_num,
    pos_in_file,
    SMALL_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += SMALL_BLOCK_SIZE;
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num + 2, /* Error: One packet from the server has been lost */
    pos_in_file,
    SMALL_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  for ( i = 0; i < SMALL_WINDOW_SIZE; ++i ) {
    _Tftp_Add_interaction_recv_data(
      TFTP_FIRST_FD,
      FIRST_TIMEOUT_MILLISECONDS,
      SERV_PORT,
      tftpfs_ipv4_loopback,
      ++block_num,
      pos_in_file,
      SMALL_BLOCK_SIZE, /* Number of bytes transferred */
      get_file_content,
      true
    );
    pos_in_file += SMALL_BLOCK_SIZE;
  }
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    ++block_num,
    pos_in_file,
    SMALL_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += SMALL_BLOCK_SIZE;
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num + 2, /* Error: One packet from the server has been lost */
    pos_in_file,
    SMALL_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num, /* The packet is assumed to be lost/does not reach the server */
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num + 3, /* Error: One packet from the server has been lost */
    pos_in_file,
    SMALL_BLOCK_SIZE, /* Number of bytes transferred */
    get_file_content,
    true
  );
  block_num = 9; /* The ACK for DATA packet 10 did not reach the server */
  pos_in_file = block_num * SMALL_BLOCK_SIZE;
  for ( i = 0; i < SMALL_WINDOW_SIZE; ++i ) {
    _Tftp_Add_interaction_recv_data(
      TFTP_FIRST_FD,
      FIRST_TIMEOUT_MILLISECONDS,
      SERV_PORT,
      tftpfs_ipv4_loopback,
      ++block_num,
      pos_in_file,
      SMALL_BLOCK_SIZE, /* Number of bytes transferred */
      get_file_content,
      true
    );
    pos_in_file += SMALL_BLOCK_SIZE;
  }
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    ++block_num,
    pos_in_file,
    0, /* Number of bytes transferred */
    get_file_content,
    true
  );
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    10, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( bytes_read, pos_in_file );
  T_no_more_interactions();
}

/*
 * Read a file using windowsize = 4.  All kinds of packet loss, out of order
 * packets and duplicated packets occur.
 * This test uses window size and block size options.  It is a stress test
 * for all kind of network problems which can appear during a transfer
 * of a file with window size larger than 1.
 * Tests:
 *   * The client uses non-default options (windowsize = 4 and blocksize = 12).
 *   * The first DATA packet of a window is lost.
 *   * The last DATA packet of a window is lost.
 *   * The middle DATA packets of a window is lost.
 *   * The first two DATA packets of a window are received in reverse order.
 *   * The middle two DATA packets of a window are received in reverse order.
 *   * The last two DATA packets of a window are received in reverse order.
 *   * The a DATA packet is received duplicated.
 *   * The an old DATA packet is received duplicated.
 *   * The a very old DATA packet is received duplicated.
 *   * The normal ACK of a window is not received by the server.
 *   * The server repeats a whole window (ensure the client sends an ACK
 *     packet despite of the repetition).
 *   * An ACK for an out-of-order packet is not received by the server.
 *   * Windows with errors appear consecutively without
 *     error free windows in between.
 *   * After the reception of the first two DATA packets of a window,
 *     a timeout occurs and the client must send an ACK.
 *   * File transfer ends exactly with the last packet of a window
 *     (the second last packet is full and the last one is empty).
 *   * The test reads a file from the file system in chunks 100 bytes.
 */
T_TEST_CASE_FIXTURE( read_file_windowsize_trouble, &fixture_small_opt_size )
{
  tftp_test_context *ctx = T_fixture_context();
  int i;
  int bytes_read;
  uint16_t block_num = 0;
  size_t pos_in_file = 0;
  int timeout = FIRST_TIMEOUT_MILLISECONDS;
  const char options[] =
    TFTP_OPTION_WINDOWSIZE"\0"
    RTEMS_XSTRING( SMALL_WINDOW_SIZE ) "\0"
    TFTP_OPTION_BLKSIZE "\0"
    RTEMS_XSTRING( SMALL_BLOCK_SIZE );
  /*
   * A positive number is the number of a DATA packet received.
   * A negative number is the number of an ACK packet send.
   * A zero indicates a timeout when waiting for a DATA packet.
   * Each line corresponds to a window.
   */
  int16_t pkg_sequence[] = {
    1, 1, 2, 3, 2, 1, 3, 4, -4, /* Duplicated DATA packets */
    6, -4, 7, 8, /* DATA packet 5 lost */
    6, 5, 7, -5, 4, 8, /* DATA packet 5 and 6 received in revers order;
                          reception of an very old packet: 4 */
    7, 6, 8, -6, /* DATA packet 6 and 7 received in revers order;
                    DATA packet 9 not send or lost */
    6, 7, 8, 9, -9, /* ACK packet 6 was not received by server */
    10, 11, 12, 0, -12, /* DATA packet 13 lost */
    13, 16, -13, /* DATA packets 14, 15 lost */
    12, 13, 14, 15, -15, 16, 17, /* Reception of duplicated old packets 12 and
                                    13 */
    16, 17, 18, 19, -19, /* Normal sequence; ACK 19 not receive by server */
    16, 17, 18, 19, -19, /* Normal sequence repeated;
                            ACK 19 not receive by server */
    16, 19, -19, 18, 17, /* Sequence repeated but DATA packet 17 and 18
                            received after 19 and in revers order */
    20, -20, 21, 22, 23, /* ACK 20 because the client assumes the server
                            did not get ACK 19 and restarted with 17 */
    21, 22, 23, 24, -24, /* Normal sequence */
    25, 27, -25, 26, 28, -26, /* The middle data packets 26, 27 are exchanged;
                                 the client assumes DATA 26 being the start
                                 of the next window and sends an ACK 26
                                 upon reception of out-of-sequence DATA 28;
                                 assume ACK 26 not received by server */
    26, 27, 29, -27, 28, /* The last data packets are exchanged;
                            ACK 27 not received by server */
    26, 27, 28, 29, -29, /* Normal sequence repeated from ACK 25 */
    30, 31, 0, -31, /* The last two data packets are lost (timeout) */
    32, 33, 34, /* Normal sequence; the last DATA packet (here missing) will
                   contain no data and ends the transfer at a window
                   boundary; a final ACK (here missing too) follows */
  };

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    SMALL_BLOCK_SIZE,
    SMALL_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    options,
    sizeof( options ),
    true
  );
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  for ( i = 0; i < RTEMS_ARRAY_SIZE( pkg_sequence ); ++i ) {
    if ( pkg_sequence[i] == 0 ) {
      block_num = pkg_sequence[i];
      _Tftp_Add_interaction_recv_nothing(
        TFTP_FIRST_FD, /* Timeout: No packet received within timeout period */
        timeout
      );
      timeout = TIMEOUT_MILLISECONDS;
    } else if ( pkg_sequence[i] > 0 ) {
      block_num = pkg_sequence[i];
      _Tftp_Add_interaction_recv_data(
        TFTP_FIRST_FD,
        timeout,
        SERV_PORT,
        tftpfs_ipv4_loopback,
        block_num,
        ( block_num - 1 ) * SMALL_BLOCK_SIZE,
        SMALL_BLOCK_SIZE, /* Number of bytes transferred */
        get_file_content,
        pkg_sequence[i] > 0 /* pkg_sequence[i] == 0 means timeout */
      );
      timeout = FIRST_TIMEOUT_MILLISECONDS;
      pos_in_file = block_num * SMALL_BLOCK_SIZE;
    } else {
      block_num = -pkg_sequence[i];
      _Tftp_Add_interaction_send_ack(
        TFTP_FIRST_FD,
        block_num,
        SERV_PORT,
        tftpfs_ipv4_loopback,
        true
      );
    }
  }
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    timeout,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    ++block_num,
    pos_in_file,
    0, /* Number of bytes transferred */
    get_file_content,
    true
  );
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    100, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( bytes_read, pos_in_file );
  T_no_more_interactions();
}

/*
 * Write a file to the server using an option to increase block size.
 * The file is 2 DATA packet and 1 byte long.  No timeouts, packet loss, ...
 * Tests:
 *   * The client uses only the blksize option in WRQ and OACK.
 *   * The client uses a block size which is larger than the default size.
 *   * The server can change the block size value in the OACK.
 *   * The option name in the OACK can be upper or lower case.
 *   * First and second DATA packets are full.
 *   * The second DATA packet is not full and signals the end of the transfer.
 *   * The client handles an empty DATA packet correctly as
 *     end of file indicator.
 *   * The test writes the file to the file system in chunks of 333 bytes.
 */
T_TEST_CASE_FIXTURE( write_simple_file_large_blocks, &fixture_large_blocksize )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_written;
  size_t pos_in_file = 0;
  uint16_t block_num = 1;
  uint16_t block_size = 211;
  const char options[] = "BLKsiZe" "\0" "211";

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    LARGE_BLOCK_SIZE,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    options,
    sizeof( options ),
    true
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    block_size,
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  pos_in_file += block_size;
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    block_size,
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  pos_in_file += block_size;
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    1, /* Data bytes in this block */
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  pos_in_file += 1;
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_written = write_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    pos_in_file, /* Size of file */
    333, /* Bytes written per call to write() */
    &ctx->fd0
  );
  T_eq_int( bytes_written, pos_in_file );
  T_no_more_interactions();
}

/*
 * Write a file to the server using default options.
 * The file is 23 data packet long.  No timeouts, packet loss, ...
 * Tests:
 *   * The client uses the default options
 *     (windowsize = 8 and blocksize = 1456).
 *   * The server sends the options in the same order the client
 *     did send them.
 *   * The 24th data packet signals the end of file transfer.
 *   * Client sends an empty data packet as end of file indicator.
 *   * The client handles files correctly which end exactly at
 *     a window border.
 */
T_TEST_CASE_FIXTURE(
  write_simple_file_default_options,
  &fixture_default_options
)
{
  tftp_test_context *ctx = T_fixture_context();
  int i;
  int bytes_written;
  size_t pos_in_file = 0;
  uint16_t block_num = 1;
  const char options[] =
    TFTP_OPTION_BLKSIZE "\0"
    RTEMS_XSTRING( TFTP_DEFAULT_BLOCK_SIZE ) "\0"
    TFTP_OPTION_WINDOWSIZE "\0"
    RTEMS_XSTRING( TFTP_DEFAULT_WINDOW_SIZE );

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    TFTP_DEFAULT_BLOCK_SIZE,
    TFTP_DEFAULT_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    options,
    sizeof( options ),
    true
  );
  for ( i = 0; i < 23; ++i ) {
    _Tftp_Add_interaction_send_data(
      TFTP_FIRST_FD,
      block_num++,
      pos_in_file,
      TFTP_DEFAULT_BLOCK_SIZE,
      get_file_content,
      SERV_PORT,
      tftpfs_ipv4_loopback,
      true
    );
    pos_in_file += TFTP_DEFAULT_BLOCK_SIZE;
    if ( i % 8 == 7 ) {
      _Tftp_Add_interaction_recv_ack(
        TFTP_FIRST_FD,
        FIRST_TIMEOUT_MILLISECONDS,
        SERV_PORT,
        tftpfs_ipv4_loopback,
        block_num - 1,
        true
      );
    } else {
      _Tftp_Add_interaction_recv_nothing(
        TFTP_FIRST_FD,
        DO_NOT_WAIT_FOR_ANY_TIMEOUT
      );
    }
  }
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    0,
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_written = write_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    pos_in_file, /* Size of file */
    333, /* Bytes written per call to write() */
    &ctx->fd0
  );
  T_eq_int( bytes_written, pos_in_file );
  T_no_more_interactions();
}

/*
 * Write a file following exactly the scenario from RFC 7440.
 * This test uses window size and block size options.  There are lost packets.
 * Tests:
 *   * The client uses non-default options
 *     (windowsize = 4 and blocksize = 12).
 *   * Test the scenario included in RFC 7440.
 *   * The server sends the options in the inverse order the client
 *     did send them.
 *   * The data of the file ends exactly at a window size border (i.e.
 *     after the last window the server sends a single empty DATA packet).
 *   * The test writes a file to the file system in chunks of 10 bytes.
 */
T_TEST_CASE_FIXTURE( write_file_rfc7440_scenario, &fixture_small_opt_size )
{
  tftp_test_context *ctx = T_fixture_context();
  int i;
  int bytes_written;
  size_t pos_in_file = 0;
  uint16_t block_num = 1;
  const char options[] =
    TFTP_OPTION_WINDOWSIZE"\0"
    RTEMS_XSTRING( SMALL_WINDOW_SIZE ) "\0"
    TFTP_OPTION_BLKSIZE "\0"
    RTEMS_XSTRING( SMALL_BLOCK_SIZE );

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    SMALL_BLOCK_SIZE,
    SMALL_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    options,
    sizeof( options ),
    true
  );
  for ( i = 0; i < 6; ++i ) {
    _Tftp_Add_interaction_send_data(
      TFTP_FIRST_FD,
      block_num++,
      pos_in_file,
      SMALL_BLOCK_SIZE,
      get_file_content,
      SERV_PORT,
      tftpfs_ipv4_loopback,
      true
    );
    pos_in_file += SMALL_BLOCK_SIZE;
    if ( i % 4 == 3 ) {
      _Tftp_Add_interaction_recv_ack(
        TFTP_FIRST_FD,
        FIRST_TIMEOUT_MILLISECONDS,
        SERV_PORT,
        tftpfs_ipv4_loopback,
        block_num - 1,
        true
      );
    } else {
      _Tftp_Add_interaction_recv_nothing(
        TFTP_FIRST_FD,
        DO_NOT_WAIT_FOR_ANY_TIMEOUT
      );
    }
  }
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    SMALL_BLOCK_SIZE,
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    DO_NOT_WAIT_FOR_ANY_TIMEOUT,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    5,
    true
  );
  block_num = 6;
  pos_in_file = (block_num - 1) * SMALL_BLOCK_SIZE;
  for ( i = 0; i < 7; ++i ) {
    _Tftp_Add_interaction_send_data(
      TFTP_FIRST_FD,
      block_num++,
      pos_in_file,
      SMALL_BLOCK_SIZE,
      get_file_content,
      SERV_PORT,
      tftpfs_ipv4_loopback,
      true
    );
    pos_in_file += SMALL_BLOCK_SIZE;
    if ( i % 4 == 3 ) {
      _Tftp_Add_interaction_recv_ack(
        TFTP_FIRST_FD,
        FIRST_TIMEOUT_MILLISECONDS,
        SERV_PORT,
        tftpfs_ipv4_loopback,
        block_num - 1,
        true
      );
    } else {
      _Tftp_Add_interaction_recv_nothing(
        TFTP_FIRST_FD,
        DO_NOT_WAIT_FOR_ANY_TIMEOUT
      );
    }
  }
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    SMALL_BLOCK_SIZE,
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_nothing(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS
  );
  block_num = 10;
  pos_in_file = (block_num - 1) * SMALL_BLOCK_SIZE;
  for ( i = 0; i < 4; ++i ) {
    _Tftp_Add_interaction_send_data(
      TFTP_FIRST_FD,
      block_num++,
      pos_in_file,
      SMALL_BLOCK_SIZE,
      get_file_content,
      SERV_PORT,
      tftpfs_ipv4_loopback,
      true
    );
    pos_in_file += SMALL_BLOCK_SIZE;
    if ( i % 4 == 3 ) {
      _Tftp_Add_interaction_recv_ack(
        TFTP_FIRST_FD,
        FIRST_TIMEOUT_MILLISECONDS,
        SERV_PORT,
        tftpfs_ipv4_loopback,
        block_num - 1,
        true
      );
    } else {
      _Tftp_Add_interaction_recv_nothing(
        TFTP_FIRST_FD,
        DO_NOT_WAIT_FOR_ANY_TIMEOUT
      );
    }
  }
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    0,
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_written = write_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    pos_in_file, /* Size of file */
    10, /* Bytes written per call to write() */
    &ctx->fd0
  );
  T_eq_int( bytes_written, pos_in_file );
  T_no_more_interactions();
}

/*
 * Write a file using windowsize = 4.  All kinds of packet loss, out of
 * order packets and duplicated ACK packets appear.
 * This test uses window size and block size options.  It is a stress test
 * for all kind of network problems which can appear during a transfer
 * of a file with window size larger than 1.
 * Tests:
 *   * The server repeats a whole window (timeout).
 *   * The client receives a duplicated ACK packet (directly in sequence).
 *   * The client receives a duplicated ACK (after sending a window).
 *   * The client receives an ACK with a block number which is not
 *     the end of the current window.
 *   * The client receives an very old ACK in the middle of a window.
 *   * The client receives an very old ACK at the end of a window.
 *   * The client receives an ACK for a not yet send DATA packet in the
 *     middle of a window (should be ignored or cause a error).
 *   * The client receives an ACK for a not yet send DATA packet at the
 *     end of a window (should be ignored or cause a error).
 *   * The client receives an ACK after sending a full window.
 *   * The client receives an ACK before all DATA packets of a
 *     window have been sent.
 *   * The client must repeat the first window completely (timeout).
 *   * The client must repeat the first window partially.
 *   * The client must repeat the last window completely (timeout).
 *   * The client must repeat the last window partially.
 *   * Windows with errors in between appear consecutively, without
 *     error free windows in between.
 *   * The test writes a file to the file system in chunks of block size.
 */
T_TEST_CASE_FIXTURE( write_file_windowsize_trouble, &fixture_small_opt_size )
{
  tftp_test_context *ctx = T_fixture_context();
  int i;
  int bytes_written;
  size_t pos_in_file = 0;
  uint16_t block_num = 1;
  int timeout = FIRST_TIMEOUT_MILLISECONDS;
  const char options[] =
    TFTP_OPTION_BLKSIZE "\0"
    RTEMS_XSTRING( SMALL_BLOCK_SIZE ) "\0"
    TFTP_OPTION_WINDOWSIZE"\0"
    RTEMS_XSTRING( SMALL_WINDOW_SIZE );
  /*
   * A positive number is the number of an ACK packet received
   *   at the end of window.
   * A 9999 indicates a timeout when waiting for an ACK packet.
   * A zero indicates no ACK packet is received when checking for it.
   * A positive number >= 10000 is the number+10000 of an ACK packet received
   *   while only checking for a packet.
   * A negative number is the number of a DATA packet send
   *   at the end of a window.
   * A negative number <= -10000 is the number-10000 of an *empty* DATA
   *   packet send.
   * Each line corresponds to a window.
   */
  int16_t pkg_sequence[] = {
    -1, 0, -2, 0, -3, 0, -4, 9999, /* First window, trigger full repeat */
    -1, 0, -2, 0, -3, 0, -4, 2, /* ACK at end of window;
                                   first window must be partially repeated */
    -3, 0, -4, 0, -5, 0, -6, 6, /* Normal sequence */
    -7, 0, -8, 0, -9, 0, -10, 6, /* Duplicate ACK */
    -7, 10006, /* Duplicate ACK; ACK before sending all packets of a window */
    -7, 0, -8, 10008, /* ACK before sending all packets of a window;
                         ACK is not at the window end */
    -9, 10007, 0, -10, 10013, 0, -11, 0, -12, 12, /* Reception of very old ACK;
                                                    Reception of future ACK (The
                                                    wrong "future" ACK must be
                                                    beyond the block size)
                                                    in the middle of a window */
    -13, 0, -14, 0, -15, 0, -16, 11, 17, 16, /* Reception of very old ACK;
                                                Reception of future ACK at the
                                                end of a window */
    -17, 0, -18, 0, -10019, 9999, /* Last window timeout, trigger full repeat */
    -17, 0, -18, 0, -10019, 16, /* Last window, duplicated ACK */
    -17, 0, -18, 10017, /* Last window, ACK before sending all packets */
    -18, 0, -10019, 19 /* Last window partially repeated */
  };

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    SMALL_BLOCK_SIZE,
    SMALL_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    options,
    sizeof( options ),
    true
  );
  for ( i = 0; i < RTEMS_ARRAY_SIZE( pkg_sequence ); ++i ) {
    if ( pkg_sequence[i] == 0 ) {
      _Tftp_Add_interaction_recv_nothing(
        TFTP_FIRST_FD,
        DO_NOT_WAIT_FOR_ANY_TIMEOUT
      );
      timeout = FIRST_TIMEOUT_MILLISECONDS;
    } else if ( pkg_sequence[i] == 9999 ) {
      _Tftp_Add_interaction_recv_nothing(
        TFTP_FIRST_FD,
        timeout
      );
      timeout = FIRST_TIMEOUT_MILLISECONDS;
    } else if ( pkg_sequence[i] >= 10000 ) {
      block_num = pkg_sequence[i] - 10000;
      _Tftp_Add_interaction_recv_ack(
        TFTP_FIRST_FD,
        DO_NOT_WAIT_FOR_ANY_TIMEOUT,
        SERV_PORT,
        tftpfs_ipv4_loopback,
        block_num++,
        true
      );
      timeout = FIRST_TIMEOUT_MILLISECONDS;
    } else if ( pkg_sequence[i] > 0 ) {
      block_num = pkg_sequence[i];
      _Tftp_Add_interaction_recv_ack(
        TFTP_FIRST_FD,
        timeout,
        SERV_PORT,
        tftpfs_ipv4_loopback,
        block_num++,
        true
      );
      timeout = FIRST_TIMEOUT_MILLISECONDS;
    } else if ( pkg_sequence[i] <= -10000 ) {
      block_num = -pkg_sequence[i] - 10000;
      pos_in_file = (block_num - 1) * SMALL_BLOCK_SIZE;
      _Tftp_Add_interaction_send_data(
        TFTP_FIRST_FD,
        block_num++,
        pos_in_file,
        0, /* Number of bytes transferred */
        get_file_content,
        SERV_PORT,
        tftpfs_ipv4_loopback,
        true
      );
      timeout = FIRST_TIMEOUT_MILLISECONDS;
    } else {
      block_num = -pkg_sequence[i];
      pos_in_file = (block_num - 1) * SMALL_BLOCK_SIZE;
      _Tftp_Add_interaction_send_data(
        TFTP_FIRST_FD,
        block_num++,
        pos_in_file,
        SMALL_BLOCK_SIZE,
        get_file_content,
        SERV_PORT,
        tftpfs_ipv4_loopback,
        true
      );
      timeout = FIRST_TIMEOUT_MILLISECONDS;
      pos_in_file += SMALL_BLOCK_SIZE;
    }
  }
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_written = write_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    pos_in_file, /* Size of file */
    SMALL_BLOCK_SIZE, /* Bytes written per call to write() */
    &ctx->fd0
  );
  T_eq_int( bytes_written, pos_in_file );
  T_no_more_interactions();
}

/*
 * Write a file to the server where the server sends an OACK without options.
 * The file is half a data packet long.  No timeouts, packet loss, ...
 * Tests:
 *   * The client processes an OACK without any options in it correctly.
 *   * The client uses the RFC1350 block size because the server does
 *     not agree to the options send.
 *   * The first data packet is half full and signals the end of the transfer.
 *   * The test reads a file from the file system in chunks of double block size.
 */
T_TEST_CASE_FIXTURE( write_tiny_file_OACK_no_options, &fixture_large_blocksize )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_written;
  size_t pos_in_file = 0;
  uint16_t block_num = 1;
  const char options[] = {};

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    LARGE_BLOCK_SIZE,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    options,
    sizeof( options ),
    true
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE / 2,
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE / 2;
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_written = write_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    pos_in_file, /* Size of file */
    2 * TFTP_RFC1350_BLOCK_SIZE, /* Bytes written per call to write() */
    &ctx->fd0
  );
  T_eq_int( bytes_written, pos_in_file );
  T_no_more_interactions();
}

/*
 * Read a file and when the server responses with an ERROR to options
 * fallback to no options.
 * The file is one byte long.  No timeouts, packet loss, ...
 * Tests:
 *   * The client uses windowsize and blksize option in the first RRQ.
 *   * Upon reception of an ERROR packet from the server, the client
 *     re-tries opening the session using an RRQ without options.
 *   * The server accepts the RRQ without options by sending a DATA packet.
 *   * The second RRQ is sent to the TFTP server port 69 and not to the
 *     port from which the first ERROR packet came from.
 *   * The first data packet contains a single byte and signals the
 *     end of the transfer.
 */
T_TEST_CASE_FIXTURE( read_file_fallback_to_no_options,
  &fixture_default_options )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  uint16_t block_num = 1;
  size_t pos_in_file = 0;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    TFTP_DEFAULT_BLOCK_SIZE,
    TFTP_DEFAULT_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_error(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    TFTP_ERROR_CODE_OPTION_NEGO,
    "Don't like options",
    true
  );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_data(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num,
    pos_in_file,
    1, /* Number of bytes transferred */
    get_file_content,
    true
  );
  pos_in_file += 1;
  _Tftp_Add_interaction_send_ack(
    TFTP_FIRST_FD,
    block_num++,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    TFTP_DEFAULT_BLOCK_SIZE / 2, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( bytes_read, pos_in_file );
  T_no_more_interactions();
}

/*
 * Read a file from the server but the server responds with
 * an ERROR to all RRQ with and without options.
 * The file is one byte long.  No timeouts, packet loss, ...
 * Tests:
 *   * The client uses windowsize and blksize option in the first RRQ.
 *   * Upon reception of an ERROR packet from the server, the client
 *     re-tries opening a session using an RRQ without options.
 *   * The second RRQ is sent to the TFTP server port 69 and not to the
 *     port from which the first ERROR packet came from.
 *   * The server does not accept the RRQ without options
 *     and responds again with an ERROR packet.
 *   * The client ends all attempts to create a connection after
 *     receiving an ERROR packet to an RRQ without options.
 *   * The client signals the error to the user.
 */
T_TEST_CASE_FIXTURE( read_file_useless_fallback_to_no_options,
  &fixture_default_options )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    TFTP_DEFAULT_BLOCK_SIZE,
    TFTP_DEFAULT_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_error(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    TFTP_ERROR_CODE_ILLEGAL,
    "Don't like options",
    true
  );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_error(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    TFTP_ERROR_CODE_ILLEGAL,
    "Go away",
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    TFTP_DEFAULT_BLOCK_SIZE / 2, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( errno, EINVAL );
  T_eq_int( bytes_read, 0 );
  T_no_more_interactions();
}

/*
 * Write a file to the server and the server responses with an ACK packet.
 * The file is half a data packet long.  No timeouts, packet loss, ...
 * Tests:
 *   * The client uses windowsize and blksize option in the WRQ.
 *   * The client uses the RFC1350 option values for the data transfer
 *     because the server responded with an ACK packet.
 *   * The whole package sequence behaves as if only RFC1350 was used.
 *   * The first data packet is half filled and signals the end of the
 *     transfer.
 */
T_TEST_CASE_FIXTURE( write_file_ACK_instead_of_OACK, &fixture_default_options )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_written;
  size_t pos_in_file = 0;
  uint16_t block_num = 0;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    TFTP_DEFAULT_BLOCK_SIZE,
    TFTP_DEFAULT_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE / 2,
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE / 2;
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_written = write_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    pos_in_file, /* Size of file */
    2 * TFTP_RFC1350_BLOCK_SIZE, /* Bytes written per call to write() */
    &ctx->fd0
  );
  T_eq_int( bytes_written, pos_in_file );
  T_no_more_interactions();
}

/*
 * Write a file and when the server responses with an ERROR to options
 * fallback to no options.
 * The file is half a data packet long.  No timeouts, packet loss, ...
 * Tests:
 *   * The client uses windowsize and blksize options in the first WRQ.
 *   * Upon reception of an ERROR from the server, the client re-tries
 *     opening a session using a WRQ without options.
 *   * The second WRQ is sent to the TFTP server port 69 and not to the
 *     port from which the first ERROR packet came from.
 *   * The server accepts the WRQ without options by sending an ACK packet.
 *   * The first data packet is half filled and signals the end of the transfer.
 */
T_TEST_CASE_FIXTURE( write_file_fallback_to_no_options,
  &fixture_default_options )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_written;
  size_t pos_in_file = 0;
  uint16_t block_num = 0;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    TFTP_DEFAULT_BLOCK_SIZE,
    TFTP_DEFAULT_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_error(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    TFTP_ERROR_CODE_ILLEGAL,
    "Don't like options",
    true
  );
  _Tftp_Add_interaction_send_wrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    NO_BLOCK_SIZE_OPTION,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_send_data(
    TFTP_FIRST_FD,
    block_num,
    pos_in_file,
    TFTP_RFC1350_BLOCK_SIZE / 2,
    get_file_content,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  pos_in_file += TFTP_RFC1350_BLOCK_SIZE / 2;
  _Tftp_Add_interaction_recv_ack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    block_num++,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_written = write_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    pos_in_file, /* Size of file */
    2 * TFTP_RFC1350_BLOCK_SIZE, /* Bytes written per call to write() */
    &ctx->fd0
  );
  T_eq_int( bytes_written, pos_in_file );
  T_no_more_interactions();
}

/*
 * The server sends a malformed OACK: The final 0 byte is missing.
 */
T_TEST_CASE_FIXTURE( OACK_without_null, &fixture_default_options )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  const char options[] = { 'b', 'l', 'k', 's', 'i', 'z', 'e', '\0', '1', '2' };

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    TFTP_DEFAULT_BLOCK_SIZE,
    TFTP_DEFAULT_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    options,
    sizeof( options ),
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_OPTION_NEGO,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    LARGE_BLOCK_SIZE, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( errno, EPROTO );
  T_eq_int( bytes_read, 0 );
  T_no_more_interactions();
}

/*
 * Server sends a malformed OACK packet: The value of the option is missing.
 */
T_TEST_CASE_FIXTURE( OACK_without_option_value, &fixture_default_options )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  const char options[] = TFTP_OPTION_BLKSIZE;

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    TFTP_DEFAULT_BLOCK_SIZE,
    TFTP_DEFAULT_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    options,
    sizeof( options ),
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_OPTION_NEGO,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    LARGE_BLOCK_SIZE, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( errno, EPROTO );
  T_eq_int( bytes_read, 0 );
  T_no_more_interactions();
}

/*
 * Server sends a malformed OACK packet: The option is unknown.
 */
T_TEST_CASE_FIXTURE( OACK_with_unknown_option, &fixture_default_options )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  const char options[] =
    "shoesize" "\0"
    RTEMS_XSTRING( TFTP_DEFAULT_BLOCK_SIZE );

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    TFTP_DEFAULT_BLOCK_SIZE,
    TFTP_DEFAULT_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    options,
    sizeof( options ),
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_OPTION_NEGO,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    LARGE_BLOCK_SIZE, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( errno, EPROTO );
  T_eq_int( bytes_read, 0 );
  T_no_more_interactions();
}

/*
 * Server sends a malformed OACK packet: The value of the option is
 * not a number.
 */
T_TEST_CASE_FIXTURE( OACK_malformed_option_value, &fixture_default_options )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  const char options[] = TFTP_OPTION_BLKSIZE "\0" "abc";

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    TFTP_DEFAULT_BLOCK_SIZE,
    TFTP_DEFAULT_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    options,
    sizeof( options ),
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_OPTION_NEGO,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    LARGE_BLOCK_SIZE, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( errno, EPROTO );
  T_eq_int( bytes_read, 0 );
  T_no_more_interactions();
}

/*
 * Server sends a malformed OACK packet: The value of the option is empty.
 */
T_TEST_CASE_FIXTURE( OACK_with_empty_option_value, &fixture_default_options )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  const char options[] = TFTP_OPTION_BLKSIZE "\0";

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    TFTP_DEFAULT_BLOCK_SIZE,
    TFTP_DEFAULT_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    options,
    sizeof( options ),
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_OPTION_NEGO,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    LARGE_BLOCK_SIZE, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( errno, EPROTO );
  T_eq_int( bytes_read, 0 );
  T_no_more_interactions();
}

/*
 * Server sends a malformed OACK packet: The option name is empty.
 */
T_TEST_CASE_FIXTURE( OACK_with_empty_option_name, &fixture_default_options )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  const char options[] = "\0" RTEMS_XSTRING( TFTP_DEFAULT_BLOCK_SIZE );

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    TFTP_DEFAULT_BLOCK_SIZE,
    TFTP_DEFAULT_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    options,
    sizeof( options ),
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_OPTION_NEGO,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    LARGE_BLOCK_SIZE, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( errno, EPROTO );
  T_eq_int( bytes_read, 0 );
  T_no_more_interactions();
}

/*
 * Server sends a malformed OACK packet: The block size option value
 * is too small.
 */
T_TEST_CASE_FIXTURE( OACK_blocksize_too_small, &fixture_default_options )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  const char options[] = TFTP_OPTION_BLKSIZE "\0" "7";

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    TFTP_DEFAULT_BLOCK_SIZE,
    TFTP_DEFAULT_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    options,
    sizeof( options ),
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_OPTION_NEGO,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    LARGE_BLOCK_SIZE, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( errno, EPROTO );
  T_eq_int( bytes_read, 0 );
  T_no_more_interactions();
}

/*
 * Server sends a malformed OACK packet: The block size option value
 * is too large.
 */
T_TEST_CASE_FIXTURE( OACK_blocksize_too_large, &fixture_default_options )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  const char options[] = TFTP_OPTION_BLKSIZE "\0" "1457";

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    TFTP_DEFAULT_BLOCK_SIZE,
    TFTP_DEFAULT_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    options,
    sizeof( options ),
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_OPTION_NEGO,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    LARGE_BLOCK_SIZE, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( errno, EPROTO );
  T_eq_int( bytes_read, 0 );
  T_no_more_interactions();
}

/*
 * Server sends a malformed OACK packet: The windowsize option value
 * is too small.
 */
T_TEST_CASE_FIXTURE( OACK_windowsize_too_small, &fixture_default_options )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  const char options[] = TFTP_OPTION_WINDOWSIZE "\0" "0";

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    TFTP_DEFAULT_BLOCK_SIZE,
    TFTP_DEFAULT_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    options,
    sizeof( options ),
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_OPTION_NEGO,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    LARGE_BLOCK_SIZE, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( errno, EPROTO );
  T_eq_int( bytes_read, 0 );
  T_no_more_interactions();
}

/*
 * Server sends a malformed OACK packet: The windowsize option is too large.
 */
T_TEST_CASE_FIXTURE( OACK_windowsize_too_large, &fixture_default_options )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  const char options[] = TFTP_OPTION_WINDOWSIZE "\0" "9";

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    TFTP_DEFAULT_BLOCK_SIZE,
    TFTP_DEFAULT_WINDOW_SIZE,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    options,
    sizeof( options ),
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_OPTION_NEGO,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    LARGE_BLOCK_SIZE, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( errno, EPROTO );
  T_eq_int( bytes_read, 0 );
  T_no_more_interactions();
}

/*
 * Server sends a malformed OACK packet: There is a known but surplus option.
 */
T_TEST_CASE_FIXTURE( OACK_with_surplus_option, &fixture_large_blocksize )
{
  tftp_test_context *ctx = T_fixture_context();
  int bytes_read;
  const char options[] =
    TFTP_OPTION_BLKSIZE "\0"
    RTEMS_XSTRING( LARGE_BLOCK_SIZE ) "\0"
    TFTP_OPTION_WINDOWSIZE "\0"
    "1";

  /* T_set_verbosity( T_VERBOSE ); */
  _Tftp_Add_interaction_socket( AF_INET, SOCK_DGRAM, 0, TFTP_FIRST_FD );
  _Tftp_Add_interaction_send_rrq(
    TFTP_FIRST_FD,
    tftpfs_file,
    TFTP_STD_PORT,
    tftpfs_ipv4_loopback,
    LARGE_BLOCK_SIZE,
    NO_WINDOW_SIZE_OPTION,
    true
  );
  _Tftp_Add_interaction_recv_oack(
    TFTP_FIRST_FD,
    FIRST_TIMEOUT_MILLISECONDS,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    options,
    sizeof( options ),
    true
  );
  _Tftp_Add_interaction_send_error(
    TFTP_FIRST_FD,
    TFTP_ERROR_CODE_OPTION_NEGO,
    SERV_PORT,
    tftpfs_ipv4_loopback,
    true
  );
  _Tftp_Add_interaction_close( TFTP_FIRST_FD, 0 );

  bytes_read = read_tftp_file(
    create_tftpfs_path( tftpfs_ipv4_loopback, tftpfs_file ),
    LARGE_BLOCK_SIZE, /* Bytes read per call to read() */
    SIZE_MAX,
    &ctx->fd0
  );
  T_eq_int( errno, EPROTO );
  T_eq_int( bytes_read, 0 );
  T_no_more_interactions();
}

const char rtems_test_name[] = "TFTPFS";

static void Init( rtems_task_argument argument )
{
  rtems_test_run( argument, TEST_STATE );
}

/*
 * RTEMS configuration for tftp
 */

#define CONFIGURE_FILESYSTEM_TFTPFS
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 4

/*
 * Simple RTEMS configuration
 */

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
