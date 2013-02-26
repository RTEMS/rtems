/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
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

#include <string.h>
#include <utf8proc/utf8proc.h>

static void
test_utf8proc_errmsg ( void )
{
  int          error;
  size_t       string_length;
  const char*  err_msg;

  for ( error = 0; error >= UTF8PROC_ERROR_INVALIDOPTS - 1; --error ) {
    err_msg = utf8proc_errmsg ( error );
    rtems_test_assert ( err_msg != NULL );

    string_length = strlen (err_msg );
    rtems_test_assert ( string_length > 0 );
  }
}

static void
test_utf8proc_version ( void )
{
  const char*  version;
  size_t       string_length;

  version = utf8proc_version ( );
  rtems_test_assert ( version != NULL );

  string_length = strlen ( version );
  rtems_test_assert ( string_length > 0 );

  rtems_test_assert (0 == strcmp ( version, "1.1.5" ) );
}

static void
test_utf8proc_iterate ( void )
{
  char         utf8_str_simple[]    = "The quick brown.fox";
  uint8_t     *utf8_str_simple_ptr  = (uint8_t*)(&utf8_str_simple[0]);
  size_t       length_simple_string = strlen ( utf8_str_simple );
  int32_t      unicode_char;
  unsigned int index;
  ssize_t      bytes_read;

  for (index = 0; index < length_simple_string; ++index) {
    bytes_read = utf8proc_iterate (
      &utf8_str_simple_ptr[index],
      length_simple_string - index,
      &unicode_char );
    rtems_test_assert ( bytes_read == 1 );
    rtems_test_assert ( (uint8_t)unicode_char == utf8_str_simple_ptr[index]);
  }
}

static void
test_utf8proc_encode_char ( void )
{
  uint8_t utf8_str[4];
  int32_t unicode_char;
  ssize_t bytes_written;

  for ( unicode_char = 0; unicode_char < 128; ++unicode_char ) {
    bytes_written = utf8proc_encode_char ( unicode_char, utf8_str );

    rtems_test_assert ( bytes_written == 1 );
    rtems_test_assert ( utf8_str[0]   == (uint8_t)unicode_char );
  }
}

static void
test_utf8proc_get_property ( void )
{
  int32_t                    unicode_char;
  const utf8proc_property_t* properties;

  for ( unicode_char = 0x0000; unicode_char <= 0x10FFFF; ++unicode_char ) {
    properties = utf8proc_get_property ( unicode_char );
    rtems_test_assert ( NULL != properties );
  }
}

static void
test_utf8proc_decompose_char ( void )
{
  int32_t unicode_char;
  int32_t unicode_char_decomposed[4];
  ssize_t chars_written;

  for ( unicode_char = 0x0000; unicode_char <= 0x10FFFF; ++unicode_char ) {
    chars_written = utf8proc_decompose_char (
      unicode_char,
      unicode_char_decomposed,
      sizeof ( unicode_char_decomposed ) / sizeof ( unicode_char_decomposed[0] ),
      UTF8PROC_STABLE | UTF8PROC_DECOMPOSE,
      0);
    if ( unicode_char < 0x80 ) {
      rtems_test_assert ( chars_written == 1 );
      rtems_test_assert ( unicode_char_decomposed[0] == unicode_char);
    }
    else
     rtems_test_assert ( chars_written > 0 );
  }
}

static void
test_utf8proc_decompose ( void )
{
  char         string_simple[]    = "The quick brown.fox";
  uint8_t     *string_simple_utf8 = (uint8_t*)(&string_simple[0]);
  int32_t      string_decomposed[sizeof ( string_simple ) * 4];
  ssize_t      chars_written;
  unsigned int index;

  memset (&string_decomposed[0], 0, sizeof ( string_decomposed ) );

  chars_written = utf8proc_decompose (
    string_simple_utf8,
    sizeof ( string_simple ),
    &string_decomposed[0],
    sizeof ( string_decomposed ),
    UTF8PROC_NULLTERM | UTF8PROC_STABLE | UTF8PROC_DECOMPOSE );
  rtems_test_assert ( chars_written == strlen ( string_simple ) );
  /* Our source string contains only very simple characters. Thus the above
   * decomposition should result in exactly the same string
   */
  for ( index = 0; index < sizeof ( string_simple ); ++index ) {
    rtems_test_assert ( string_simple_utf8[index] == (uint8_t)string_decomposed[index] );
  }
}

static void
test_utf8proc_reencode ( void )
{
  char         string_simple[]    = "The quick brown.fox";
  uint8_t     *string_simple_utf8 = (uint8_t*)(&string_simple[0]);
  int32_t      string_decomposed[sizeof ( string_simple ) * 4];
  uint8_t     *string_reencoded   = (uint8_t*)(&string_decomposed[0]);
  ssize_t      chars_written;
  unsigned int index;

  memset (&string_decomposed[0], 0, sizeof ( string_decomposed ) );

  chars_written = utf8proc_decompose (
    string_simple_utf8,
    sizeof ( string_simple ),
    &string_decomposed[0],
    sizeof ( string_decomposed ),
    UTF8PROC_NULLTERM | UTF8PROC_STABLE | UTF8PROC_DECOMPOSE );
  rtems_test_assert ( chars_written == strlen ( string_simple ) );

  chars_written = utf8proc_reencode (
    &string_decomposed[0],
    chars_written,
    UTF8PROC_NULLTERM | UTF8PROC_STABLE | UTF8PROC_DECOMPOSE );
  rtems_test_assert ( chars_written == strlen ( string_simple ) );
  /* Our source string contains only very simple characters. Thus the above
   * decomposition should result in exactly the same string
   */
  for ( index = 0; index < sizeof ( string_simple ); ++index ) {
    rtems_test_assert ( string_simple_utf8[index] == string_reencoded[index] );
  }
}

static void
test_utf8proc_map ( void )
{
  char         string_simple[]    = "The quick brown.fox";
  uint8_t     *string_simple_utf8 = (uint8_t*)(&string_simple[0]);
  uint8_t     *dest               = NULL;
  ssize_t      chars_written;
  unsigned int index;

  chars_written = utf8proc_map(
    string_simple_utf8,
    sizeof ( string_simple ),
    &dest,
    UTF8PROC_NULLTERM | UTF8PROC_STABLE | UTF8PROC_DECOMPOSE );
  rtems_test_assert ( chars_written == strlen ( string_simple ) );
  rtems_test_assert ( dest != NULL);

  /* Our source string contains only very simple characters. Thus the above
   * decomposition should result in exactly the same string
   */
  for ( index = 0; index < chars_written; ++index ) {
    rtems_test_assert ( string_simple_utf8[index] == dest[index] );
  }
  free ( dest );
}

typedef uint8_t* (*normalization_method)(const uint8_t* str);

static void
test_utf8proc_normalize ( const normalization_method test_sample )
{
  char         string_simple[]    = "The quick brown.fox";
  uint8_t     *string_simple_utf8 = (uint8_t*)(&string_simple[0]);
  uint8_t     *dest               = NULL;
  unsigned int index;

  dest = test_sample ( string_simple_utf8 );
  rtems_test_assert ( dest != NULL);

  /* Our source string contains only very simple characters. Thus the above
   * decomposition should result in exactly the same string
   */
  for ( index = 0; index < sizeof ( string_simple ); ++index ) {
    rtems_test_assert ( string_simple_utf8[index] == dest[index] );
  }
  free ( dest );
}

static void test ( void )
{
  test_utf8proc_errmsg         ( );
  test_utf8proc_version        ( );
  test_utf8proc_iterate        ( );
  test_utf8proc_encode_char    ( );
  test_utf8proc_get_property   ( );
  test_utf8proc_decompose_char ( );
  test_utf8proc_decompose      ( );
  test_utf8proc_reencode       ( );
  test_utf8proc_map            ( );
  test_utf8proc_normalize      ( utf8proc_NFD  );
  test_utf8proc_normalize      ( utf8proc_NFC  );
  test_utf8proc_normalize      ( utf8proc_NFKD );
  test_utf8proc_normalize      ( utf8proc_NFKC );
}

static void Init ( rtems_task_argument arg )
{
  puts ( "\n\n*** TEST utf8proc01 ***" );

  test ( );

  puts ( "*** END OF TEST utf8proc01 ***" );

  rtems_test_exit ( 0 );
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
