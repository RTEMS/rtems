/*
 *  Exercise putk, printk, and getchark
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/io.h>

/*
 * Undefined the RTEMS_PRINTFLIKE and make it nothing. The test code
 * contained in the file is suppose to be wrong.
 */
#undef RTEMS_PRINTFLIKE
#define RTEMS_PRINTFLIKE(_a, _b)

#include <rtems/bspIo.h>
#include <tmacros.h>

const char rtems_test_name[] = "SPPRINTK";

static int test_getchar(void)
{
  return 0x35;
}

static void do_getchark(void)
{
  int                                sc;
  BSP_polling_getchar_function_type  poll_char;

  poll_char = BSP_poll_char;

  BSP_poll_char = NULL;

  putk( "getchark - NULL getchar method - return -1" );
  sc = getchark();
  rtems_test_assert( sc == -1 );

  putk( "getchark - test getchar method - returns 0x35" );
  BSP_poll_char = test_getchar;
  sc = getchark();
  rtems_test_assert( sc == 0x35 );

  BSP_poll_char = poll_char;
}

static void do_putk(void)
{
  putk( "This is a test of putk" );
}

static void do_printk(void)
{
  long lm = 2147483647L;
  unsigned long ulm = 4294967295UL;
  long long llm = 9223372036854775807LL;
  long long ullm = 18446744073709551615ULL;

  printk( "bad format                   -- %%q in parentheses (%q)\n" );

  printk( "bad format                   -- %%lq in parentheses (%lq)\n" );

  printk( "%%o of 16                     -- %o\n", 16 );
  printk( "%%i of 16                     -- %i\n", 16 );
  printk( "%%d of 16                     -- %d\n", 16 );
  printk( "'%%-3d' of 16                 -- '%-3d'\n", 16 );
  printk( "'%%3d' of 16                  -- '%3d'\n", 16 );
  printk( "%%u of 16                     -- %u\n", 16 );
  printk( "%%X of 16                     -- %X\n", 16 );
  printk( "%%x of 16                     -- %x\n", 16 );
  printk( "%%p of 0x1234                 -- %p\n", (void *)0x1234 );

  /* long */
  printk( "%%lo of 2147483647            -- %lo\n", lm );
  printk( "%%li of 2147483647            -- %li\n", lm );
  printk( "%%lu of 2147483647            -- %lu\n", lm );
  printk( "%%lx of 2147483647            -- %lx\n", lm );
  printk( "%%lo of -2147483648           -- %lo\n", -lm - 1L );
  printk( "%%li of -2147483648           -- %li\n", -lm - 1L );
  printk( "%%lx of -2147483648           -- %lx\n", -lm - 1L );
  printk( "%%lo of 4294967295            -- %lo\n", ulm );
  printk( "%%lu of 4294967295            -- %lu\n", ulm );
  printk( "%%lx of 4294967295            -- %lx\n", ulm );

  /* long long */
  printk( "%%llo of 9223372036854775807  -- %llo\n", llm );
  printk( "%%lli of 9223372036854775807  -- %lli\n", llm );
  printk( "%%llu of 9223372036854775807  -- %llu\n", llm );
  printk( "%%llx of 9223372036854775807  -- %llx\n", llm );
  printk( "%%llo of -9223372036854775808 -- %llo\n", -llm - 1LL );
  printk( "%%lli of -9223372036854775808 -- %lli\n", -llm - 1LL );
  printk( "%%llx of -9223372036854775808 -- %llx\n", -llm - 1LL );
  printk( "%%llo of 18446744073709551615 -- %llo\n", ullm );
  printk( "%%llu of 18446744073709551615 -- %llu\n", ullm );
  printk( "%%llx of 18446744073709551615 -- %llx\n", ullm );

  /* negative numbers */
  printk( "%%d of -16                    -- %d\n", -16 );
  printk( "%%d of -16                    -- %-3d\n", -16 );
  printk( "%%u of -16                    -- %u\n", -16 );

  /* string formats */
  printk( "%%s of Mary Had a Little Lamb -- '%s'\n",
          "Mary Had a Little Lamb" );
  printk( "%%s of NULL                   -- '%s'\n", NULL );
  printk( "%%12s of joel                 -- '%20s'\n", "joel" );
  printk( "%%4s of joel                  -- '%4s'\n", "joel" );
  printk( "%%-12s of joel                -- '%-20s'\n", "joel" );
  printk( "%%-4s of joel                 -- '%-4s'\n", "joel" );
  printk( "%%c of X                      -- '%c'\n", 'X' );
  printk( "%%hhu of X                    -- %hhu\n", 'X' );
}

typedef struct {
  char buf[128];
  size_t i;
} test_context;

static test_context test_instance;

static void clear( test_context *ctx )
{
  ctx->i = 0;
  memset( ctx->buf, 0, sizeof( ctx->buf ) );
}

static void put_char( int c, void *arg )
{
  test_context *ctx;

  ctx = arg;

  if ( ctx->i < sizeof( ctx->buf ) ) {
    ctx->buf[ ctx->i ] = (char) c;
    ++ctx->i;
  }
}

static test_context test_instance;

static void test_io_printf( test_context *ctx )
{
  int i;
  intmax_t j;
  long long ll;
  long l;
  size_t z;
  ptrdiff_t t;

  clear( ctx );
  i = 123;
  _IO_Printf( put_char, ctx, "%i", i );
  rtems_test_assert( strcmp( ctx->buf, "123" ) == 0 );

  clear( ctx );
  j = 456;
  _IO_Printf( put_char, ctx, "%ji", j );
  rtems_test_assert( strcmp( ctx->buf, "456" ) == 0 );

  clear( ctx );
  ll = 789;
  _IO_Printf( put_char, ctx, "%lli", ll );
  rtems_test_assert( strcmp( ctx->buf, "789" ) == 0 );

  clear( ctx );
  l = 101112;
  _IO_Printf( put_char, ctx, "%li", l );
  rtems_test_assert( strcmp( ctx->buf, "101112" ) == 0 );

  clear( ctx );
  z = 131415;
  _IO_Printf( put_char, ctx, "%zi", z );
  rtems_test_assert( strcmp( ctx->buf, "131415" ) == 0 );

  clear( ctx );
  t = 161718;
  _IO_Printf( put_char, ctx, "%ti", t );
  rtems_test_assert( strcmp( ctx->buf, "161718" ) == 0 );
}

static void test_io_base64( test_context *ctx )
{
  unsigned char buf[] = "abcdefghi";
  int n;

  clear( ctx );
  n = _IO_Base64( put_char, ctx, buf, 9, "\n", 0 );
  rtems_test_assert( n == 14 );
  rtems_test_assert( strcmp( ctx->buf, "YWJj\nZGVm\nZ2hp" ) == 0 );

  clear( ctx );
  n = _IO_Base64( put_char, ctx, buf, 8, "\n", 4 );
  rtems_test_assert( n == 14 );
  rtems_test_assert( strcmp( ctx->buf, "YWJj\nZGVm\nZ2g=" ) == 0 );

  clear( ctx );
  n = _IO_Base64( put_char, ctx, buf, 7, "\n", 4 );
  rtems_test_assert( n == 14 );
  rtems_test_assert( strcmp( ctx->buf, "YWJj\nZGVm\nZw==" ) == 0 );

  clear( ctx );
  n = _IO_Base64( put_char, ctx, buf, 6, "\n", 4 );
  rtems_test_assert( n == 9 );
  rtems_test_assert( strcmp( ctx->buf, "YWJj\nZGVm" ) == 0 );

  clear( ctx );
  n = _IO_Base64( put_char, ctx, buf, 5, "\n", 4 );
  rtems_test_assert( n == 9 );
  rtems_test_assert( strcmp( ctx->buf, "YWJj\nZGU=" ) == 0 );

  clear( ctx );
  n = _IO_Base64( put_char, ctx, buf, 4, "\n", 4 );
  rtems_test_assert( n == 9 );
  rtems_test_assert( strcmp( ctx->buf, "YWJj\nZA==" ) == 0 );

  clear( ctx );
  n = _IO_Base64( put_char, ctx, buf, 3, "\n", 4 );
  rtems_test_assert( n == 4 );
  rtems_test_assert( strcmp( ctx->buf, "YWJj" ) == 0 );

  clear( ctx );
  n = _IO_Base64( put_char, ctx, buf, 2, "\n", 4 );
  rtems_test_assert( n == 4 );
  rtems_test_assert( strcmp( ctx->buf, "YWI=" ) == 0 );

  clear( ctx );
  n = _IO_Base64( put_char, ctx, buf, 1, "\n", 4 );
  rtems_test_assert( n == 4 );
  rtems_test_assert( strcmp( ctx->buf, "YQ==" ) == 0 );

  clear( ctx );
  n = _IO_Base64( put_char, ctx, buf, 0, "\n", 4 );
  rtems_test_assert( n == 0 );
}

static rtems_task Init(
  rtems_task_argument argument
)
{
  TEST_BEGIN();

  do_putk();
  putk("");

  do_printk();
  putk("");

  do_getchark();
  test_io_printf(&test_instance);
  test_io_base64(&test_instance);

  TEST_END();
  rtems_test_exit( 0 );
}

/* configuration information */

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS           1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
