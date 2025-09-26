/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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

const char rtems_test_name[] = "PSXMSGQ 3";

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
  (void) timer;
  (void) arg;

  int msg = 4;

  tsr_fired = true;

  tsr_status = mq_send( Queue, (const char *)&msg, sizeof(int), 1 );
  tsr_errno = errno;
}

void *POSIX_Init(
  void *argument
)
{
  (void) argument;

  struct mq_attr     attr;
  int                status;
  rtems_id           timer;
  rtems_status_code  rc;

  TEST_BEGIN();

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
  if ( (tsr_status != -1) || (tsr_errno != EAGAIN) ) {
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

  TEST_END();
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
