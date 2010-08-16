/*
 * Copyright 2010, Alin Rus <alin.codejunkie@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#define CONFIGURE_INIT
#include "system.h"
#include <rtems.h>
#include "tmacros.h"
#include <rtems/posix/aio_misc.h>
#include <aio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sched.h>
#include <fcntl.h>

#define BUFSIZE 512
#define WRONG_FD 404

struct aiocb *
create_aiocb (void)
{
  struct aiocb *aiocbp;

  aiocbp = malloc (sizeof (struct aiocb));
  memset (aiocbp, 0, sizeof (struct aiocb));
  aiocbp->aio_buf = malloc (BUFSIZE * sizeof (char));
  aiocbp->aio_nbytes = BUFSIZE;
  aiocbp->aio_offset = 0;
  aiocbp->aio_reqprio = 0;
  aiocbp->aio_fildes = open ("aio_fildes", O_RDWR | O_CREAT);

  return aiocbp;
}

void
free_aiocb (struct aiocb *aiocbp)
{
  free ((char*) aiocbp->aio_buf);
  free (aiocbp);
}

void *
POSIX_Init (void *argument)
{
  int result, policy;
  struct aiocb *aiocbp;
  rtems_status_code status;
  struct sched_param param;

  puts ("\n\n*** POSIX AIO TEST 02 ***");

  puts ("\n*** POSIX aio_read() test ***");

  /* Request canceled */
  puts ("Init: aio_read - ECANCELED");

  aiocbp = create_aiocb ();
  aio_read (aiocbp);
  aio_cancel (aiocbp->aio_fildes, aiocbp);
  result = aio_return (aiocbp);
  rtems_test_assert (result != -1);
  status = aio_error (aiocbp);
  rtems_test_assert (status != ECANCELED);
  free_aiocb (aiocbp);

  /* Successfull added request to queue */
  puts ("Init: aio_read - SUCCESSFUL");
  aiocbp = create_aiocb ();
  aiocbp->aio_fildes = WRONG_FD;
  status = aio_read (aiocbp);
  rtems_test_assert (!status);

  pthread_getschedparam (pthread_self (), &policy, &param);
  policy = SCHED_RR;
  param.sched_priority = 30;
  pthread_setschedparam (pthread_self (), policy, &param);
  sleep (1);

  while (aio_error (aiocbp) == EINPROGRESS);

  /* Bad file descriptor */
  puts ("Init: aio_read() - EBADF ");

  result = aio_return (aiocbp);
  rtems_test_assert (result != -1);
  status = aio_error (aiocbp);
  rtems_test_assert (status != EBADF);
  free_aiocb (aiocbp);

  /* Invalid offset */
  puts ("Init: aio_read() - EINVAL [aio_offset]");

  aiocbp = create_aiocb ();
  aiocbp->aio_offset = -1;
  aio_read (aiocbp);
  sleep (1);

  while (aio_error (aiocbp) == EINPROGRESS);

  result = aio_return (aiocbp);
  rtems_test_assert (result != -1);
  status = aio_error (aiocbp);
  rtems_test_assert (status != EINVAL);
  free_aiocb (aiocbp);

  /* Invalid request priority */
  puts ("Init: aio_read() - EINVAL [aio_reqprio]");

  aiocbp = create_aiocb ();
  aiocbp->aio_reqprio = AIO_PRIO_DELTA_MAX + 1;
  aio_read (aiocbp);
  sleep (1);

  while (aio_error (aiocbp) == EINPROGRESS);

  result = aio_return (aiocbp);
  rtems_test_assert (result != -1);
  status = aio_error (aiocbp);
  rtems_test_assert (status != EINVAL);
  free_aiocb (aiocbp);

  /* aio_nbytes > 0 && aio_nbytes + aio_offset > max offset of aio_fildes */
  puts ("Init: aio_read() - OVERFLOW");
  aiocbp = create_aiocb ();
  aiocbp->aio_nbytes = 10;
  aiocbp->aio_offset = lseek (aiocbp->aio_fildes, 0, SEEK_END);
  aio_read (aiocbp);
  sleep (1);

  while (aio_error (aiocbp) == EINPROGRESS);

  result = aio_return (aiocbp);
  rtems_test_assert (result != -1);
  status = aio_error (aiocbp);
  rtems_test_assert (status != EFBIG);
  free_aiocb (aiocbp);

  puts ("*** END OF POSIX AIO TEST 01 ***");

  rtems_test_exit (0);

  return NULL;

}
