/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *
 *  This is a simple real-time applications XXX.
 *
 *  Other POSIX facilities such as XXX, condition, .. is also used
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
#include <pthread.h>  /* thread facilities */
#include <signal.h>   /* signal facilities */
#include <unistd.h>   /* sleep facilities */
#include <sched.h>    /* schedule facilities */
#include <time.h>     /* time facilities */
#include <stdio.h>    /* console facilities */
#include "tmacros.h"

const char rtems_test_name[] = "PSXCLEANUP";

#define NUMBER_THREADS 2
pthread_t ThreadIds[NUMBER_THREADS];

typedef struct {
  pthread_mutex_t lock;
  pthread_cond_t rcond;
  pthread_cond_t wcond;
  int lock_count; /* < 0 .. Held by writer. */
                  /* > 0 .. Held by lock_count readers. */
                  /* = 0 .. Held by nobody. */
  int waiting_writers; /* Count of waiting writers. */
} lock_t;

volatile bool reader_cleanup_ran;
volatile bool release_read_lock_ran;
volatile bool writer_cleanup_ran;

void waiting_reader_cleanup(void *arg);
void lock_for_read(void *arg);
void release_read_lock(void *arg);
void waiting_writer_cleanup(void *arg);
void lock_for_write(lock_t *l);
void release_write_lock(void *arg);
void initialize_lock_t(lock_t *l);
void *ReaderThread(void *arg);
void *WriterThread(void *arg);

void waiting_reader_cleanup(void *arg)
{
  lock_t *l;

  reader_cleanup_ran = TRUE;

  l = (lock_t *) arg;
  pthread_mutex_unlock(&l->lock);
}

void lock_for_read(void *arg)
{
  lock_t *l = arg;

  pthread_mutex_lock(&l->lock);
  pthread_cleanup_push(waiting_reader_cleanup, l);
  while ((l->lock_count < 0) && (l->waiting_writers != 0))
    pthread_cond_wait(&l->rcond, &l->lock);
  l->lock_count++;
  reader_cleanup_ran = FALSE;

  /*
   * Note the pthread_cleanup_pop executes
   * waiting_reader_cleanup.
   */
  pthread_cleanup_pop(1);

  if ( reader_cleanup_ran == FALSE ) {
    puts( "reader cleanup did not run" );
    rtems_test_exit(0);
  }
}

void release_read_lock(void *arg)
{
  lock_t *l = arg;

  release_read_lock_ran = TRUE;
  pthread_mutex_lock(&l->lock);
  if (--l->lock_count == 0)
    pthread_cond_signal(&l->wcond);
  pthread_mutex_unlock(&l->lock);
}

void waiting_writer_cleanup(void *arg)
{
  lock_t *l = arg;

  writer_cleanup_ran = TRUE;

  if ((--l->waiting_writers == 0) && (l->lock_count >= 0)) {
    /*
     * This only happens if we have been canceled.
     */
    pthread_cond_broadcast(&l->wcond);
  }
    pthread_mutex_unlock(&l->lock);
}

void lock_for_write(lock_t *l)
{
  pthread_mutex_lock(&l->lock);
  l->waiting_writers++;
  l->lock_count = -1;

  pthread_cleanup_push(waiting_writer_cleanup, l);

  while (l->lock_count != 0)
      pthread_cond_wait(&l->wcond, &l->lock);
  l->lock_count = -1;

  /*
   * Note the pthread_cleanup_pop executes
   * waiting_writer_cleanup.
   */
  writer_cleanup_ran = FALSE;
  pthread_cleanup_pop(1);

  if ( writer_cleanup_ran == FALSE ) {
    puts( "writer cleanup did not run" );
    rtems_test_exit(0);
  }
}

void release_write_lock(void *arg)
{
  lock_t *l = arg;

  writer_cleanup_ran = TRUE;

  /* pthread_mutex_lock(&l->lock); */
  l->lock_count = 0;
  if (l->waiting_writers == 0)
      pthread_cond_broadcast(&l->rcond);
  else
      pthread_cond_signal(&l->wcond);
  /* pthread_mutex_unlock(&l->lock); */
}


/*
 * This function is called to initialize the read/write lock.
 */
void initialize_lock_t(lock_t *l)
{
  pthread_mutexattr_t mutexattr;    /* mutex attributes */
  pthread_condattr_t  condattr;     /* condition attributes */

  if (pthread_mutexattr_init (&mutexattr) != 0) {
    perror ("Error in mutex attribute init\n");
  }
  if (pthread_mutex_init (&l->lock,&mutexattr) != 0) {
    perror ("Error in mutex init");
  }

  if (pthread_condattr_init (&condattr) != 0) {
     perror ("Error in condition attribute init\n");
  }
  if (pthread_cond_init (&l->wcond,&condattr) != 0) {
    perror ("Error in write condition init");
  }
  if (pthread_cond_init (&l->rcond,&condattr) != 0) {
    perror ("Error in read condition init");
  }

  l->lock_count = 0;
  l->waiting_writers = 0;
}

void *ReaderThread(void *arg)
{
  lock_t *l = arg;

  puts("Lock for read");
  lock_for_read(l);
  puts("cleanup push for read");
  pthread_cleanup_push(release_read_lock, &l->lock);

  /* Thread has read lock. */
  release_read_lock_ran = FALSE;
  puts("cleanup pop for read");
  pthread_cleanup_pop(1);

  if ( release_read_lock_ran == FALSE ) {
    puts( "release read lock did not run" );
    rtems_test_exit(0);
  }
  return NULL;
}

void *WriterThread(void *arg)
{
  lock_t *l = arg;

  puts("Lock for write");
  lock_for_write(l);
  puts("cleanup push for write");
  pthread_cleanup_push(release_write_lock, &l->lock);

  /* Thread has write lock. */
  release_write_lock(&l->lock);

  /* do nothing */
  puts("do nothing cleanup pop for write");
  pthread_cleanup_pop(0);
  return NULL;
}

/*
 *  main entry point to the test
 */

void *POSIX_Init(
  void *argument
)
{
  (void) argument;

  pthread_attr_t attr;              /* task attributes */
  int            status;
  lock_t         l;

  TEST_BEGIN();

  /*************** INITIALIZE  ***************/
  initialize_lock_t(&l);
  if (pthread_attr_init(&attr) != 0) {
    perror ("Error in attribute init\n");
  }

  /*************** CREATE THREADS  ***************/

  status = pthread_create(&ThreadIds[0], NULL, ReaderThread, &l);
  posix_service_failed( status, "pthread_create Reader" );

  sleep(1);

  status = pthread_create(&ThreadIds[1], NULL, WriterThread, &l);
  posix_service_failed( status, "pthread_create Writer" );

  sleep(1);

  /*************** END OF TEST *****************/
  TEST_END();
  rtems_test_exit(0);
}

