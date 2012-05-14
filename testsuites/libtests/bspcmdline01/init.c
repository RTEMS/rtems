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
#include <rtems/bspcmdline.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
void test_errors(void);
void test_search(bool null_expected, const char *cmdline, const char *param);

extern const char *bsp_boot_cmdline;

void test_errors(void)
{
  const char *p;
  char        result[32];

  bsp_boot_cmdline = NULL;

  puts( "rtems_bsp_cmdline_get_param - name=NULL - returns NULL" );
  p = rtems_bsp_cmdline_get_param( NULL, result, 32 );
  rtems_test_assert( p == NULL );

  puts( "rtems_bsp_cmdline_get_param - result=NULL - returns NULL" );
  p = rtems_bsp_cmdline_get_param( "name", NULL, 32 );
  rtems_test_assert( p == NULL );

  puts( "rtems_bsp_cmdline_get_param - length=0 - returns NULL" );
  p = rtems_bsp_cmdline_get_param( "name", result, 0 );
  rtems_test_assert( p == NULL );

  puts( "rtems_bsp_cmdline_get_param_raw - name=NULL - returns NULL" );
  p = rtems_bsp_cmdline_get_param_raw( NULL );
  rtems_test_assert( p == NULL );

  bsp_boot_cmdline = NULL;

  puts( "rtems_bsp_cmdline_get_param - bsp_boot_cmdline=NULL - returns NULL" );
  p = rtems_bsp_cmdline_get_param( "name", result, 5 );
  rtems_test_assert( p == NULL );

  puts(
    "rtems_bsp_cmdline_get_param_raw - bsp_boot_cmdline=NULL - returns NULL" );
  p = rtems_bsp_cmdline_get_param_raw( "name" );
  rtems_test_assert( p == NULL );
  
  bsp_boot_cmdline = "edit";
  puts (
    "rtems_bsp_cmdline_get_param - bsp_boot_cmdline = edit name = "
      "edit -no error" );
  p = rtems_bsp_cmdline_get_param("edit", result, 5);
  rtems_test_assert( p != NULL );

  bsp_boot_cmdline = "joel=123456789";
  puts( "rtems_bsp_cmdline_get_param - too short buffer" );
  p = rtems_bsp_cmdline_get_param("joel", result, 5);
  rtems_test_assert( p != NULL );

  bsp_boot_cmdline = "--arg1=X`";
  puts( "rtems_bsp_cmdline_get_param_rhs - short match" );
  p = rtems_bsp_cmdline_get_param_rhs("arg", result, 10);
  rtems_test_assert( p == NULL );
}

void test_search(
  bool        null_expected,
  const char *cmdline,
  const char *param
)
{
  const char *p;
  char        value[80];
  size_t      length;

  bsp_boot_cmdline = cmdline;

  printf(
    "\n"
    "Testing for param=(%s)%s\n"
    "  Command Line : (%s)\n",
    param,
    ((null_expected) ? " - Expect NULL" : ""),
    cmdline
  );

  printf( "rtems_bsp_cmdline_get_param_raw(%s)\n", param );
  p = rtems_bsp_cmdline_get_param_raw( param );
  if ( null_expected ) {
    if ( p )
      puts( "ERROR - rtems_bsp_cmdline_get_param_raw did not return NULL" );
    else
      printf( "rtems_bsp_cmdline_get_param_raw(%s) returned NULL\n", param );
    rtems_test_assert( !p );
  } else {
    if ( p )
      printf( "rtems_bsp_cmdline_get_param_raw(%s) returned (%s)\n", param, p );
    else
      printf( "rtems_bsp_cmdline_get_param_raw(%s) returned NULL\n", param );

    rtems_test_assert( p );
  }

  printf( "rtems_bsp_cmdline_get_param_rhs(%s)\n", param );
  length = sizeof(value);
  p = rtems_bsp_cmdline_get_param_rhs( param, value, length );
  if ( null_expected ) {
    if ( p )
      puts( "ERROR - rtems_bsp_cmdline_get_param_rhs did not return NULL" );
    else
      printf( "rtems_bsp_cmdline_get_param_rhs(%s) returned NULL\n", param );
    rtems_test_assert( !p );
  } else {
    if ( !p )
      puts( "ERROR - rtems_bsp_cmdline_get_param_rhs returned NULL" );
    rtems_test_assert( p );
    printf(
      "rtems_bsp_cmdline_get_param_rhs(%s) returned (%s) value=(%s)\n",
      param,
      ((*p == '\0') ? "ZERO_LENGTH_STRING" : p ),
      ((*value == '\0') ? "ZERO_LENGTH_STRING" : value )
    );
  }

}

rtems_task Init(
  rtems_task_argument ignored
)
{
  const char *bspcmdline;

  puts( "\n\n*** TEST OF PARSING BOOT COMMAND STRING ***" );

  bspcmdline = rtems_bsp_cmdline_get();
  if ( bspcmdline ) {
    printf(
      "BSP has a boot command line:\n"
      "%s\n",
      bspcmdline
    );
  } else {
    puts( "BSP does not have a boot command line" );
  }

  puts( "\nTest Parameter Error Conditions" );
  test_errors();

  test_search( false, "--arg=", "--arg" );
  test_search( true, "--arg=", "-g" );
  test_search( false, "--ip=192.168.1.151 --name=fred", "-name" );
  test_search( false, "--ip=192.168.1.151 --name=fred", "-ip" );
  test_search(
    false,
    "--ip=192.168.1.151 --name=\"joel and michele\" --cpu=fast",
    "-name"
    );

  puts( "*** END OF OF PARSING BOOT COMMAND STRING ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS         1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
