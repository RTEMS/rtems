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

#include <tmacros.h>
#include <rtems/bspIo.h>

const char rtems_test_name[] = "SPPRINTK";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
int test_getchar(void);
void do_getchark(void);
void do_putk(void);
void do_printk(void);

int test_getchar(void)
{
  return 0x35;
}

void do_getchark(void)
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

void do_putk(void)
{
  putk( "This is a test of putk" );
}

void do_printk(void)
{
  long lm = 2147483647L;
  unsigned long ulm = 4294967295UL;
  long long llm = 9223372036854775807LL;
  long long ullm = 18446744073709551615ULL;

  printk( "bad format -- %%q in parentheses (%q)\n" );

  printk( "bad format -- %%lq in parentheses (%lq)\n", 0x1234 );

  printk( "%%O octal upper case 16 -- %O\n", 16 );
  printk( "%%o octal lower case of 16 -- %O\n", 16 );
  printk( "%%I of 16 -- %I\n", 16 );
  printk( "%%i of 16 -- %i\n", 16 );
  printk( "%%D of 16 -- %D\n", 16 );
  printk( "%%d of 16 -- %d\n", 16 );
  printk( "%%-3d of 16 -- %-3d\n", 16 );
  printk( "%%U of 16 -- %U\n", 16 );
  printk( "%%u of 16 -- %u\n", 16 );
  printk( "%%X of 16 -- %X\n", 16 );
  printk( "%%x of 16 -- %x\n", 16 );
  printk( "%%p of 0x1234 -- %p\n", (void *)0x1234 );

  /* long */
  printk( "%%lo of 2147483647 -- %lo\n", lm );
  printk( "%%li of 2147483647 -- %li\n", lm );
  printk( "%%lu of 2147483647 -- %lu\n", lm );
  printk( "%%lx of 2147483647 -- %lx\n", lm );
  printk( "%%lo of -2147483648 -- %lo\n", -lm - 1L );
  printk( "%%li of -2147483648 -- %li\n", -lm - 1L );
  printk( "%%lx of -2147483648 -- %lx\n", -lm - 1L );
  printk( "%%lo of 4294967295 -- %lo\n", ulm );
  printk( "%%lu of 4294967295 -- %lu\n", ulm );
  printk( "%%lx of 4294967295 -- %lx\n", ulm );

  /* long long */
  printk( "%%llo of 9223372036854775807 -- %llo\n", llm );
  printk( "%%lli of 9223372036854775807 -- %lli\n", llm );
  printk( "%%llu of 9223372036854775807 -- %llu\n", llm );
  printk( "%%llx of 9223372036854775807 -- %llx\n", llm );
  printk( "%%llo of -9223372036854775808 -- %llo\n", -llm - 1LL );
  printk( "%%lli of -9223372036854775808 -- %lli\n", -llm - 1LL );
  printk( "%%llx of -9223372036854775808 -- %llx\n", -llm - 1LL );
  printk( "%%llo of 18446744073709551615 -- %llo\n", ullm );
  printk( "%%llu of 18446744073709551615 -- %llu\n", ullm );
  printk( "%%llx of 18446744073709551615 -- %llx\n", ullm );

  /* negative numbers */
  printk( "%%d of -16 -- %d\n", -16 );
  printk( "%%d of -16 -- %-3d\n", -16 );
  printk( "%%u of -16 -- %u\n", -16 );

  /* string formats */
  printk( "%%s of Mary Had a Little Lamb -- (%s)\n",
          "Mary Had a Little Lamb" );
  printk( "%%s of NULL -- (%s)\n", NULL );
  printk( "%%12s of joel -- (%20s)\n", "joel" );
  printk( "%%4s of joel -- (%4s)\n", "joel" );
  printk( "%%-12s of joel -- (%-20s)\n", "joel" );
  printk( "%%-4s of joel -- (%-4s)\n", "joel" );
  printk( "%%c of X -- (%c)\n", 'X' );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_test_begink();

  do_putk();
  putk("");

  do_printk();
  putk("");

  do_getchark();

  rtems_test_endk();
  rtems_test_exit( 0 );
}

/* configuration information */

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS           1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

