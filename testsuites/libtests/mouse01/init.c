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

#include <bsp.h>

#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <rtems/mw_uid.h>
#include "termios_testdriver_intr.h"
#include "tmacros.h"

#define UID_MESSAGE_COUNT 10

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
bool enqueue_next_action(
  const unsigned char *actions,
  size_t               max,
  size_t               to_enqueue
);
void open_it(void);
void register_it(void);
void printf_uid_message(struct MW_UID_MESSAGE *uid);
void receive_uid_message(void);
void close_it(void);

extern const char *Mouse_Type_Long;
extern const char *Mouse_Type_Short;
extern const unsigned char Mouse_Actions[];
extern const size_t Mouse_Actions_Size;
extern const size_t Mouse_Actions_Per_Iteration;

int Mouse_Index = 0;

bool enqueue_next_action(
  const unsigned char *actions,
  size_t               max,
  size_t               to_enqueue
)
{
  if ( (Mouse_Index + to_enqueue) > max )
    return false;

  termios_test_driver_set_rx_enqueue_now( true );
  
  termios_test_driver_set_rx( &actions[Mouse_Index], to_enqueue );
  Mouse_Index += to_enqueue;

  return true;
}

int Test_fd;

void open_it(void)
{
  /* open the file */
  puts( "open(/dev/mouse) - OK " );
  Test_fd = open( "/dev/mouse", O_RDONLY );
  rtems_test_assert( Test_fd != -1 );
}

void register_it(void)
{
  int                rc;
  char               name[5] = "mous";

  printf( "uid_open_queue() - mouse queue\n" );
  rc = uid_open_queue( name, 0, UID_MESSAGE_COUNT );
  rtems_test_assert( rc == 0 );

  printf( "uid_register_device() - OK\n");
  rc = uid_register_device( Test_fd, name );
  rtems_test_assert( rc == 0 );
}

void printf_uid_message(
  struct MW_UID_MESSAGE *uid
)
{
  uid_print_message_with_plugin(
    stdout,
    (rtems_printk_plugin_t)fprintf,
    uid
  );
}

void receive_uid_message(void)
{
  int                    rc;
  struct MW_UID_MESSAGE  uid;

  rc = uid_read_message( &uid, 0 );
  if ( rc != sizeof(struct MW_UID_MESSAGE) )
    return;
  printf_uid_message( &uid );
}

void close_it(void)
{
  int rc;

  printf( "uid_unregister_device() - OK\n" );
  rc = uid_unregister_device( Test_fd );
  rtems_test_assert( rc == 0 );

  puts( "close(/dev/mouse) - OK " );
  rc = close( Test_fd );
  rtems_test_assert( rc == 0 );
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  bool more_data;

  printf( "\n\n*** %s TEST ***\n", Mouse_Type_Long );

  open_it();
  register_it(); 
  do {
    more_data = enqueue_next_action(
      Mouse_Actions,
      Mouse_Actions_Size, 
      Mouse_Actions_Per_Iteration
    );
    receive_uid_message();
  } while (more_data);
  close_it(); 
  printf( "*** END OF %s TEST ***\n", Mouse_Type_Long );
  rtems_test_exit( 0 );
}

/* configuration information */

#include <rtems/serial_mouse.h>
#include "termios_testdriver_intr.h"

/* NOTICE: the clock driver is explicitly disabled */
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_EXTRA_DRIVERS \
    TERMIOS_TEST_DRIVER_TABLE_ENTRY, \
    SERIAL_MOUSE_DRIVER_TABLE_ENTRY

/* one for the console and one for the test port */
#define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 2

/* we need to be able to open the test device and mouse */
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 5
#define CONFIGURE_MAXIMUM_TASKS  1
#define CONFIGURE_MAXIMUM_TIMERS 2
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES 1

#define CONFIGURE_MESSAGE_BUFFER_MEMORY \
  CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( \
    UID_MESSAGE_COUNT, \
    sizeof(struct MW_UID_MESSAGE) \
  )

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_INIT

#include <rtems/confdefs.h>

/* end of file */
