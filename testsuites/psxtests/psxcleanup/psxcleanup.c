/*
 *
 *  This is a simple real-time applications XXX.
 *
 *  Other POSIX facilities such as XXX, condition, .. is also used
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
#include <pthread.h>  /* thread facilities */
#include <signal.h>   /* signal facilities */
#include <unistd.h>   /* sleep facilities */
#include <sched.h>    /* schedule facilities */
#include <time.h>     /* time facilities */
#include <stdio.h>    /* console facilities */
#include "tmacros.h"

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

  /* do nothing */
  puts("do nothing cleanup pop for write");
  pthread_cleanup_pop(0);

  /* Thread has write lock. */
  puts("cleanup pop for write");
  pthread_cleanup_pop(1);
  return NULL;
}

/*
 *  main entry point to the test
 */

void *POSIX_Init(
  void *argument
)
{
  pthread_attr_t attr;              /* task attributes */
  int            status;
  lock_t         l;

  puts( "\n\n*** POSIX CLEANUP TEST ***" );

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

  /*************** ERROR CASES  ***************/
  puts("Call pthread_cleanup_push with NULL handler");
  pthread_cleanup_push(NULL, NULL);

  puts("Call pthread_cleanup_pop with no push");
  pthread_cleanup_pop(1);

  /*************** END OF TEST *****************/
  puts( "*** END OF POSIX CLEANUP TEST ***\n" );
  rtems_test_exit(0);
}

