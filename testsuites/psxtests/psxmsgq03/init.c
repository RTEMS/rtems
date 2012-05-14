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

#define CONFIGURE_INIT
#include "system.h"

#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>

#include "test_support.h"

/* forward declarations to avoid warnings */
rtems_timer_service_routine mq_send_timer(rtems_id timer, void *arg);

mqd_t Queue;
volatile bool tsr_fired;
volatile int  tsr_status;
volatile int  tsr_errno;

rtems_timer_service_routine mq_send_timer(
  rtems_id  timer,
  void     *arg
)
{
  int msg = 4;

  tsr_fired = true;

  tsr_status = mq_send( Queue, (const char *)&msg, sizeof(int), 1 );
  tsr_errno = errno;
}

void *POSIX_Init(
  void *argument
)
{
  struct mq_attr     attr;
  int                status;
  rtems_id           timer;
  rtems_status_code  rc;

  puts( "\n\n*** POSIX MESSAGE QUEUE TEST 3 ***" );

  attr.mq_maxmsg  = 1;
  attr.mq_msgsize = sizeof(int);

  puts( "Init - Open message queue" );
  Queue = mq_open( "Qsend", O_CREAT | O_RDWR, 0x777, &attr );
  if ( Queue == (-1) ) {
    perror( "mq_open failed" );
  }
  rtems_test_assert( Queue != (-1) );

  puts( "Init - send to message queue" );
  status = mq_send( Queue, (const char *)&status, sizeof(int), 1 );
  if ( status == (-1) ) {
    perror( "mq_status failed" );
  }
  rtems_test_assert( status != (-1) );

  /*
   * Now create the timer we will send to a full queue from.
   */
  puts( "Init - Create Classic API Timer" );
  rc = rtems_timer_create( 1, &timer );
  directive_failed( rc, "rtems_timer_create" );

  puts( "Init - Fire After Classic API Timer" );
  tsr_fired = false;

  rc = rtems_timer_fire_after(
    timer,
    rtems_clock_get_ticks_per_second(),
    mq_send_timer,
    NULL
  );
  directive_failed( rc, "rtems_timer_fire_after" );

  sleep(2);

  if ( tsr_fired == false ) {
    puts( "ERROR -- TSR DID NOT FIRE" );
    rtems_test_exit( 0 );
  }
  if ( (tsr_status != -1) || (tsr_errno != ENOMEM) ) {
    puts( "ERROR -- TSR DID NOT RETURN CORRECT STATUS" );
    printf(
      "status=%d errno=%d --> %s\n",
      tsr_status,
      tsr_errno,
      strerror(tsr_errno)
    );
    rtems_test_exit( 0 );
  }

  puts( "Init - mq_send from ISR returned correct status" );

  puts( "*** END OF POSIX MESSAGE QUEUE TEST 3 ***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
