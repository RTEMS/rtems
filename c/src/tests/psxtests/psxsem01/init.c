#define CONFIGURE_INIT
#include "system.h"
#include <sched.h>
#include <fcntl.h>
#include <time.h>

void *POSIX_Init(
  void *argument
)
{
  int             status;
  int             value;
  int             i;
  sem_t           sems[CONFIGURE_MAXIMUM_POSIX_SEMAPHORES];
  sem_t           sem2;
  sem_t           *n_sem1;
  sem_t           *n_sem2;
  sem_t           testsem;
  struct timespec waittime;

  puts( "\n\n*** POSIX SEMAPHORE MANAGER TEST 1 ***" );

  puts( "Init: sem_init - SUCCESSFUL" );
  for (i = 0; i < CONFIGURE_MAXIMUM_POSIX_SEMAPHORES; i++) {
    status = sem_init(&sems[i], 0, i);
    assert( status == 0 );
  }
  puts( "Init: sem_init - UNSUCCESSFUL (ENOSPC)" );
  status = sem_init(&sem2, 0, 1);
  assert( (status == -1) && (errno == ENOSPC) );
 
  puts( "Init: sem_init - UNSUCCESSFUL (ENOSYS -- pshared not supported)" );
  status = sem_init(&sem2, 1, 1);
  assert( (status == -1) && (errno == ENOSYS) );
  
  puts( "Init: sem_getvalue - SUCCESSFUL ");
  for (i = 0; i < CONFIGURE_MAXIMUM_POSIX_SEMAPHORES; i++) {
    status = sem_getvalue(&sems[i], &value);
    assert( (status == 0) && (value == i) );
  }
  puts( "Init: sem_getvalue - UNSUCCESSFUL ");
  status = sem_getvalue(&sem2, &value);
  assert( (status == -1) && (errno == EINVAL) );

  puts( "Init: sem_destroy - SUCCESSFUL" );
  status = sem_destroy(&sems[0]);
  assert( status == 0 );

  puts( "Init: sem_destroy - UNSUCCESSFUL (EINVAL)" );
  status = sem_destroy(&sem2);
  assert( (status == -1) && (errno == EINVAL) );

  puts( "Init: sem_wait - SUCCESSFUL" );
  status = sem_wait(&sems[1]);
  assert( status == 0);

  puts( "Init: sem_wait - UNSUCCESSFUL (EINVAL)" );
  status = sem_wait(&sem2);
  assert( (status == -1) && (errno == EINVAL) );

  puts( "Init: sem_post - SUCCESSFUL" );
  status = sem_post(&sems[1]);
  assert( status == 0 );

  puts( "Init: sem_wait - SUCCESSFUL (after a sem_post)" );
  status = sem_wait(&sems[1]);
  assert( status == 0 );

  puts( "Init: sem_trywait - SUCCESSFUL" );
  status = sem_trywait(&sems[2]);
  assert( status == 0 );

  puts( "Init: sem_trywait - UNSUCCESSFUL (EAGAIN)" );
  status = sem_trywait(&sems[1]);
  assert( (status == -1) && (errno == EAGAIN) );

  puts( "Init: sem_trywait - UNSUCCESSFUL (EINVAL)" );
  status = sem_trywait(&sem2);
  assert( (status == -1) && (errno == EINVAL) );

  puts( "Init: sem_timedwait - SUCCESSFUL" );
  waittime.tv_sec = 0;
  waittime.tv_nsec = 100;
  status = sem_timedwait(&sems[2], &waittime);
  assert( status == 0 );

  puts( "Init: sem_timedwait - UNSUCCESSFUL (ETIMEDOUT)" );
  status = sem_timedwait(&sems[1], &waittime);
  assert( (status == -1) && (errno == ETIMEDOUT) );

  puts( "Init: sem_timedwait - UNSUCCESSFUL (EINVAL)" );
  status = sem_timedwait(&sem2, &waittime);
  assert( (status == -1) && (errno == EINVAL) );

  puts( "Init: sem_post - UNSUCCESSFUL (EINVAL)" );
  status = sem_post(&sem2);
  assert( (status == -1) && (errno == EINVAL) );

  puts( "Init: sem_destroy - SUCCESSFUL" );
  for (i = 1; i < CONFIGURE_MAXIMUM_POSIX_SEMAPHORES; i++) {
    status = sem_destroy(&sems[i]);
    assert( status == 0);
  }

  /* Modes are currently unsupported */

  puts( "Init: sem_open - SUCCESSFUL" );
  n_sem1 = sem_open("sem1", O_CREAT, 00777, 1);
  assert( n_sem1 != SEM_FAILED );

  puts( "Init: sem_open - UNSUCCESSFUL (EEXIST)" );
  n_sem2 = sem_open("sem1", O_CREAT | O_EXCL, 00777, 1);
  assert( (n_sem2 == SEM_FAILED) && (errno == EEXIST) );

  puts( "Init: sem_close - SUCCESSFUL" );
  status = sem_close(n_sem1);
  assert( status == 0);

  /*
  puts( "Init: sem_close - UNSUCCESSFUL (EINVAL)" );
  status = sem_close(n_sem2);
  assert( (status == -1) && (errno == EINVAL) );

  puts( "Init: sem_unlink - SUCCESSFUL" );
  status = sem_unlink("sem1");
  assert( status == 0 );

  puts( "Init: sem_unlink - UNSUCCESSFUL (ENOENT)" );
  status = sem_unlink("sem2");
  assert( (status == -1) && (errno == ENOENT) );
  */

  /* Try adding in unlinking before closing... (can we still open?) */

  puts( "*** POSIX SEMAPHORE MANAGER TEST 1 COMPLETED ***" );
  exit(0);

  return NULL; /* just so the compiler thinks we returned something */
}
