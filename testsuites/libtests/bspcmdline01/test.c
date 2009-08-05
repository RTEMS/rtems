/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <rtems/bspcmdline.h>

#define rtems_test_exit(_x) exit(_x)

extern const char *bsp_boot_cmdline;

void test_errors(void)
{
  const char *p;
  char        result[32];

  bsp_boot_cmdline = NULL;

  puts( "bsp_cmdline_get_param - name=NULL - returns NULL" );
  p = bsp_cmdline_get_param( NULL, result, 32 );
  assert( p == NULL );

  puts( "bsp_cmdline_get_param - result=NULL - returns NULL" );
  p = bsp_cmdline_get_param( "name", NULL, 32 );
  assert( p == NULL );

  puts( "bsp_cmdline_get_param - length=0 - returns NULL" );
  p = bsp_cmdline_get_param( "name", result, 0 );
  assert( p == NULL );

  puts( "bsp_cmdline_get_param_raw - name=NULL - returns NULL" );
  p = bsp_cmdline_get_param_raw( NULL );
  assert( p == NULL );

  bsp_boot_cmdline = NULL;
  
  puts( "bsp_cmdline_get_param - bsp_boot_cmdline=NULL - returns NULL" );
  p = bsp_cmdline_get_param( "name", result, 5 );
  assert( p == NULL );

  puts( "bsp_cmdline_get_param_raw - bsp_boot_cmdline=NULL - returns NULL" );
  p = bsp_cmdline_get_param_raw( "name" );
  assert( p == NULL );
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

  printf( "bsp_cmdline_get_param_raw(%s)\n", param );
  p = bsp_cmdline_get_param_raw( param );
  if ( null_expected ) {
    if ( p )
      puts( "ERROR - bsp_cmdline_get_param_raw did not return NULL" );
    else
      printf( "bsp_cmdline_get_param_raw(%s) returned NULL\n", param );
    assert( !p );
  } else {
    if ( p )
      printf( "bsp_cmdline_get_param_raw(%s) returned (%s)\n", param, p );
    else
      printf( "bsp_cmdline_get_param_raw(%s) returned NULL\n", param );
    
    assert( p );
  }


  printf( "bsp_cmdline_get_param_rhs(%s)\n", param );
  length = sizeof(value);
  p = bsp_cmdline_get_param_rhs( param, value, length );
  if ( null_expected ) {
    if ( p )
      puts( "ERROR - bsp_cmdline_get_param_rhs did not return NULL" );
    else
      printf( "bsp_cmdline_get_param_rhs(%s) returned NULL\n", param );
    assert( !p );
  } else {
    if ( !p )
      puts( "ERROR - bsp_cmdline_get_param_rhs returned NULL" );
    assert( p );
    printf(
      "bsp_cmdline_get_param_rhs(%s) returned (%s) value=(%s)\n",
      param,
      ((*p == '\0') ? "ZERO_LENGTH_STRING" : p ),
      ((*value == '\0') ? "ZERO_LENGTH_STRING" : value )
    );
  }

}

void test_cmdline(void)
{
    
  puts( "\n\n*** TEST OF PARSING BOOT COMMAND STRING ***" );

  puts( "Test Parameter Error Conditions" );
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
}
