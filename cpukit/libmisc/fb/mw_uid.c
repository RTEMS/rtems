/*
 *
 * Copyright (c) 2000 - Rosimildo da Silva
 *
 * MODULE DESCRIPTION:
 * This module implements the input devices interface used by MicroWindows
 * in an embedded system environment.  It uses the RTEMS message queue as
 * the repository for the messages posted by the devices registered.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <rtems.h>

#include <rtems/mw_uid.h>
#include <rtems/seterr.h>

static rtems_id  queue_id = 0;
static int       open_count = 0;

/*
#define MW_DEBUG_ON     1
*/

/* open a message queue with the kernel */
int uid_open_queue(
  const char *q_name,
  int         flags __attribute__((unused)),
  size_t      max_msgs
)
{
  rtems_status_code status;

  /*
   * For the first device calling this function we would create the queue.
   * It is assumed that this call is done at initialization, and no concerns
   * regarding multi-threading is taken in consideration here.
   */
  if ( open_count ) {
    open_count++;
    return 0;
  }

  status = rtems_message_queue_create(
    rtems_build_name( q_name[0], q_name[1], q_name[2], q_name[3] ),
    max_msgs,
    sizeof( struct MW_UID_MESSAGE ),
    RTEMS_FIFO | RTEMS_LOCAL,
    &queue_id
  );
  if ( status != RTEMS_SUCCESSFUL ) {
    #ifdef MW_DEBUG_ON
      printk( "UID_Queue: error creating queue: %d\n", status );
     #endif
    return -1;
  }
  #ifdef MW_DEBUG_ON
    printk( "UID_Queue: id=%X\n", queue_id );
  #endif
  open_count++;
  return 0;
}


/* close message queue */
int uid_close_queue( void )
{
  if ( open_count == 1 ) {
    rtems_message_queue_delete( queue_id );
    queue_id = 0;
  }
  open_count--;
  return 0;
}

/* reads for a message from the device */
int uid_read_message( struct MW_UID_MESSAGE *m, unsigned long timeout )
{
  rtems_status_code status;
  size_t            size = 0;
  unsigned long     micro_secs = timeout*1000;
  int               wait = (timeout != 0);

  status = rtems_message_queue_receive(
   queue_id,
   (void*)m,
   &size,
   wait ? RTEMS_WAIT : RTEMS_NO_WAIT,
   RTEMS_MICROSECONDS_TO_TICKS(micro_secs)
  );

  if( status == RTEMS_SUCCESSFUL ) {
     return size;
  } else if( ( status == RTEMS_UNSATISFIED ) || ( status == RTEMS_TIMEOUT ) ) {
     rtems_set_errno_and_return_minus_one( ETIMEDOUT );
  }
  /* Here we have one error condition */
  #ifdef MW_DEBUG_ON
    printk( "UID_Queue: error reading queue: %d\n", status );
  #endif
  return -1;
}

/*
 * add a message to the queue of events. This method can be used to
 * simulate hardware events, and it can be very handy during development
 * a new interface.
 */
int uid_send_message( struct MW_UID_MESSAGE *m )
{
  rtems_status_code status;
  status = rtems_message_queue_send(
    queue_id, ( void * )m, sizeof( struct MW_UID_MESSAGE ) );
  return (status == RTEMS_SUCCESSFUL) ? 0 : -1;
}

/*
 * register the device to insert events to the message
 * queue named as the value passed in q_name
 */
int uid_register_device( int fd, const char *q_name )
{
  return ioctl( fd, MW_UID_REGISTER_DEVICE, q_name );
}

/* tell this device to stop adding events to the queue */
int uid_unregister_device( int fd )
{
  return ioctl( fd, MW_UID_UNREGISTER_DEVICE, NULL );
}

/* set the keyboard */
int uid_set_kbd_mode( int fd, int mode, int *old_mode )
{
  if (ioctl( fd, MV_KDGKBMODE, old_mode) < 0) {
     return -1;
  }
  if (ioctl(fd, MV_KDSKBMODE, mode ) < 0 ) {
     return -1;
  }
  return 0;
}
