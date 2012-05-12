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
#include "test_support.h"

#include <stdio.h>
#include <rtems/assoc.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

const rtems_assoc_t assoc_table_null[] = 
  {
    { NULL       , 0 , 0  },
    { "zero"     , 1 , 8  },
    { "one"      , 2 , 4  },
    { "two"      , 4 , 2  },
    { "three"    , 8 , 1  },
    { NULL       , -1, -1 }
  };

const rtems_assoc_t assoc_table_default[] = 
  {
    { "(default)", 0 , 0  },
    { "zero"     , 1 , 8  },
    { "one"      , 2 , 4  },
    { "two"      , 4 , 2  },
    { "three"    , 8 , 1  },
    { NULL       , -1, -1 }
  };

const rtems_assoc_t assoc_table[] = 
  {
    { "zero" , 1 , 8  },
    { "one"  , 2 , 4  },
    { "two"  , 4 , 2  },
    { "three", 8 , 1  },
    { NULL   , -1, -1 }
  };

uint32_t local;
uint32_t remote;
const rtems_assoc_t *assoc_item;
char *name;

static void reset_name( void )
{
  memset( name, 0, 40 );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  name = malloc(40);
  puts( "\n\n*** TEST ASSOC ROUTINES - 1 ***" );

  puts( "Init - get local by name -- OK" );
  local = rtems_assoc_local_by_name( assoc_table, "zero" );
  rtems_test_assert( local == 1 );

  puts( "Init - get local by name -- expect 0" );
  local = rtems_assoc_local_by_name( assoc_table, "four" );
  rtems_test_assert( local == 0 );

  puts( "Init - get local by remote bitfield -- OK" );
  local = rtems_assoc_local_by_remote_bitfield( assoc_table, 1 );
  rtems_test_assert( local == 8 );

  puts( "Init - get local by remote bitfield -- expect 0" );
  local = rtems_assoc_local_by_remote_bitfield( assoc_table, 0 );
  rtems_test_assert( local == 0 );

  puts( "Init - get local by remote -- OK" );
  local = rtems_assoc_local_by_remote( assoc_table, 1 );
  rtems_test_assert( local == 8 );

  puts( "Init - get local by remote -- expect 0" );
  local = rtems_assoc_local_by_remote( assoc_table, 0 );
  rtems_test_assert( local == 0 );

  reset_name();
  puts( "Init - get name by local bitfield -- OK" );
  name = rtems_assoc_name_by_local_bitfield( assoc_table, 1, name );
  rtems_test_assert ( !strcmp( name, "zero" ) );

  reset_name();
  puts( "Init - get name by local bitfield -- OK" );
  name = rtems_assoc_name_by_local_bitfield( assoc_table, 3, name );
  rtems_test_assert ( !strcmp( name, "zero one" ) );

  reset_name();
  puts( "Init - get name by local bitfield -- expect\"\"" );
  name = rtems_assoc_name_by_local_bitfield( assoc_table, 0, name );
  rtems_test_assert ( !strcmp( name, "" ) );
  
  reset_name();
  puts( "Init - get name by local -- OK" );
  rtems_test_assert( !strcmp( rtems_assoc_name_by_local( assoc_table, 1 ), 
			      "zero" ) );
  
  reset_name();
  puts( "Init - get name by local -- using bad value" );
  puts( rtems_assoc_name_by_local( assoc_table, 0 ) );

  reset_name();
  puts( "Init - get name by remote bitfield -- OK" );
  name = 
    rtems_assoc_name_by_remote_bitfield( assoc_table, 1, name );
  rtems_test_assert ( !strcmp( name, "three" ) );

  reset_name();
  puts( "Init - get name by remote bitfield -- OK" );
  name = 
    rtems_assoc_name_by_remote_bitfield( assoc_table, 3, name );
  rtems_test_assert ( !strcmp( name, "three two" ) );

  reset_name();
  puts( "Init - get name by remote bitfield -- expect\"\"" );
  name = 
    rtems_assoc_name_by_remote_bitfield( assoc_table, 0, name );
  rtems_test_assert ( !strcmp( name, "" ) );
  
  reset_name();
  puts( "Init - get name by remote -- OK" );
  rtems_test_assert( !strcmp( rtems_assoc_name_by_remote( assoc_table, 1 ),
			      "three" ) );
  
  reset_name();
  puts( "Init - get name by remote -- using bad value" );
  puts( rtems_assoc_name_by_remote( assoc_table, 0 ) );

  puts( "Init - get ptr by local -- OK" );
  assoc_item = rtems_assoc_ptr_by_local( assoc_table, 1 );
  rtems_test_assert( assoc_item == assoc_table );

  puts( "Init - get ptr by local -- expect NULL" );
  assoc_item = rtems_assoc_ptr_by_local( assoc_table, 0 );
  rtems_test_assert( assoc_item == 0 );

  puts( "Init - get ptr by remote -- OK" );
  assoc_item = rtems_assoc_ptr_by_remote( assoc_table, 8 );
  rtems_test_assert( assoc_item == assoc_table );

  puts( "Init - get ptr by remote -- expect NULL" );
  assoc_item = rtems_assoc_ptr_by_remote( assoc_table, 0 );
  rtems_test_assert( assoc_item == 0 );

  puts( "Init - get ptr by name -- OK" );
  assoc_item = rtems_assoc_ptr_by_name( assoc_table, "zero" );
  rtems_test_assert( assoc_item == assoc_table );

  puts( "Init - get ptr by name -- expect NULL" );
  assoc_item = rtems_assoc_ptr_by_name( assoc_table, "six" );
  rtems_test_assert( assoc_item == 0 );

  puts( "Init - get remote by local bitfield -- OK" );
  remote = rtems_assoc_remote_by_local_bitfield( assoc_table, 1 );
  rtems_test_assert( remote == 8 );

  puts( "Init - get remote by local bitfield -- expect 0" );
  remote = rtems_assoc_remote_by_local_bitfield( assoc_table, 0 );
  rtems_test_assert( remote == 0 );

  puts( "Init - get remote by local -- OK" );
  remote = rtems_assoc_remote_by_local( assoc_table, 1 );
  rtems_test_assert( remote == 8 );

  puts( "Init - get remote by local -- expect 0" );
  remote = rtems_assoc_remote_by_local( assoc_table, 0 );
  rtems_test_assert( remote == 0 );

  puts( "Init - get remote by name -- OK" );
  remote = rtems_assoc_remote_by_name( assoc_table, "zero" );
  rtems_test_assert( remote == 8 );

  puts( "Init - get remote by name -- expect 0" );
  remote = rtems_assoc_remote_by_name( assoc_table, "six" );
  rtems_test_assert( remote == 0 );

  puts( "Init - get ptr by name -- expect (default)" );
  assoc_item = rtems_assoc_ptr_by_name( assoc_table_default, "six" );
  rtems_test_assert( assoc_item == assoc_table_default );

  puts( "Init - get ptr by local -- expect (default)" );
  assoc_item = rtems_assoc_ptr_by_local( assoc_table_default, 0 );
  rtems_test_assert( assoc_item == assoc_table_default );

  puts( "Init - get ptr by remote -- expect (default)" );
  assoc_item = rtems_assoc_ptr_by_remote( assoc_table_default, 0 );
  rtems_test_assert( assoc_item == assoc_table_default );

  puts( "Init - get ptr by name -- expect NULL" );
  assoc_item = rtems_assoc_ptr_by_name( assoc_table_null, "six" );
  rtems_test_assert( assoc_item == 0 );

  puts( "Init - get ptr by local -- expect NULL" );
  assoc_item = rtems_assoc_ptr_by_local( assoc_table_null, 0 );
  rtems_test_assert( assoc_item == 0 );

  puts( "Init - get ptr by remote -- expect NULL" );
  assoc_item = rtems_assoc_ptr_by_remote( assoc_table_null, 0 );
  rtems_test_assert( assoc_item == 0 );

  free( name );

  puts( "*** END OF TEST ASSOC ROUTINES - 1 ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
