/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  system.h,v 1.13 2000/06/12 15:00:12 joel Exp
 */

#include <rtems.h>

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

/* global variables */


/* configuration information */

#include <bsp.h> /* for device driver prototypes */
#include <libchip/ata.h> /* for ata driver prototype */
#include <libchip/ide_ctrl.h> /* for general ide driver prototype */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_HAS_OWN_DEVICE_DRIVER_TABLE

#ifdef CONFIGURE_INIT
rtems_driver_address_table Device_drivers[] =
	{
	CONSOLE_DRIVER_TABLE_ENTRY
	,CLOCK_DRIVER_TABLE_ENTRY
#ifdef RTEMS_BSP_HAS_IDE_DRIVER
	,IDE_CONTROLLER_DRIVER_TABLE_ENTRY
	/* important: ATA driver must be after ide drivers */
	,ATA_DRIVER_TABLE_ENTRY 
#endif
	};

#include <rtems/bdbuf.h>
rtems_bdbuf_config rtems_bdbuf_configuration[] = {
  {512,128,NULL}
};
int rtems_bdbuf_configuration_size =( sizeof(rtems_bdbuf_configuration)
				     /sizeof(rtems_bdbuf_configuration[0]));
#endif


/*
 * XXX: these values are higher than needed...
 */
#define CONFIGURE_MAXIMUM_TASKS             20
#define CONFIGURE_MAXIMUM_SEMAPHORES        20
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES    20
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 20
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS         (3 * RTEMS_MINIMUM_STACK_SIZE)

#include <confdefs.h>

/*
 *  Handy macros and static inline functions
 */

/*
 *  Macro to hide the ugliness of printing the time.
 */

#define print_time(_s1, _tb, _s2) \
  do { \
    printf( "%s%02d:%02d:%02d   %02d/%02d/%04d%s", \
       _s1, (_tb)->hour, (_tb)->minute, (_tb)->second, \
       (_tb)->month, (_tb)->day, (_tb)->year, _s2 ); \
    fflush(stdout); \
  } while ( 0 )

/*
 *  Macro to print an task name that is composed of ASCII characters.
 *
 */

#define put_name( _name, _crlf ) \
  do { \
    rtems_unsigned32 c0, c1, c2, c3; \
    \
    c0 = ((_name) >> 24) & 0xff; \
    c1 = ((_name) >> 16) & 0xff; \
    c2 = ((_name) >> 8) & 0xff; \
    c3 = (_name) & 0xff; \
    putchar( (char)c0 ); \
    if ( c1 ) putchar( (char)c1 ); \
    if ( c2 ) putchar( (char)c2 ); \
    if ( c3 ) putchar( (char)c3 ); \
    if ( (_crlf) ) \
      putchar( '\n' ); \
  } while (0)

/*
 *  static inline routine to make obtaining ticks per second easier.
 */

static inline rtems_unsigned32 get_ticks_per_second( void )
{
  rtems_interval ticks_per_second;
  (void) rtems_clock_get( RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticks_per_second );  return ticks_per_second;
}


/*
 *  This allows us to view the "Test_task" instantiations as a set
 *  of numbered tasks by eliminating the number of application
 *  tasks created.
 *
 *  In reality, this is too complex for the purposes of this
 *  example.  It would have been easier to pass a task argument. :)
 *  But it shows how rtems_id's can sometimes be used.
 */

#define task_number( tid ) \
  ( rtems_get_index( tid ) - \
     rtems_configuration_get_rtems_api_configuration()->number_of_initialization_tasks )

/* end of include file */

