/*
 *  Exercise Printk
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <tmacros.h>

rtems_task Init(
  rtems_task_argument argument
)
{
  void                   *p1;
  bool                    retbool;
  Heap_Information_block  info;

  printk( "\n\n*** TEST PRINTK ***\n" );

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


  printk( "*** END OF TEST PRINTK ***" );
  rtems_test_exit( 0 );
}

/* configuration information */

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS           1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

