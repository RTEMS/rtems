/*
 * Copyright 2010, Alin Rus <alin.codejunkie@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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

/* forward declarations to avoid warnings */
struct aiocb *create_aiocb(int fd);
void free_aiocb(struct aiocb *aiocbp);

struct aiocb *
create_aiocb (int fd)
{
  struct aiocb *aiocbp;

  aiocbp = malloc (sizeof (struct aiocb));
  memset (aiocbp, 0, sizeof (struct aiocb));
  aiocbp->aio_buf = malloc (BUFSIZE * sizeof (char));
  aiocbp->aio_nbytes = BUFSIZE;
  aiocbp->aio_offset = 0;
  aiocbp->aio_reqprio = 0;
  aiocbp->aio_fildes = fd;

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
  int result, fd;
  struct aiocb *aiocbp;
  int status;

  rtems_aio_init ();

  status = mkdir ("/tmp", S_IRWXU);
  rtems_test_assert (!status);
  
  fd = open ("/tmp/aio_fildes", O_RDWR|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO);
  rtems_test_assert ( fd != -1);

  puts ("\n\n*** POSIX AIO TEST 01 ***");

  puts (" Init: EBADF TESTS ");

  aiocbp = create_aiocb (WRONG_FD);
  status = aio_write (aiocbp);
  rtems_test_assert (status == -1);

  /* Bad file descriptor */
  puts ("Init: aio_write() - EBADF ");

  result = aio_return (aiocbp);
  rtems_test_assert (result == -1);
  status = aio_error (aiocbp);
  rtems_test_assert (status == EBADF);

  status = aio_read (aiocbp);
  rtems_test_assert (status == -1);

  /* Bad file descriptor */
  puts ("Init: aio_read() - EBADF ");

  result = aio_return (aiocbp);
  rtems_test_assert (result == -1);
  status = aio_error (aiocbp);
  rtems_test_assert (status == EBADF);

  status = aio_cancel (WRONG_FD, NULL);
  rtems_test_assert (status == -1);

  /* Bad file descriptor */
  puts ("Init: aio_cancel() - EBADF ");
  
  result = aio_return (aiocbp);
  rtems_test_assert (result == -1);
  status = aio_error (aiocbp);
  rtems_test_assert (status == EBADF);

  status = aio_fsync (O_SYNC, aiocbp);
  rtems_test_assert (status == -1);
  
  /* Bad file descriptor */
  puts ("Init: aio_fsync() - EBADF ");

  result = aio_return (aiocbp);
  rtems_test_assert (result == -1);
  status = aio_error (aiocbp);
  rtems_test_assert (status == EBADF);
  
  free_aiocb (aiocbp);

  /* Invalid offset */
  puts ("Init: aio_write() - EINVAL [aio_offset]");

  aiocbp = create_aiocb (fd);
  aiocbp->aio_offset = -1;
  status = aio_write (aiocbp);
  rtems_test_assert (status == -1);

  result = aio_return (aiocbp);
  rtems_test_assert (result == -1);
  status = aio_error (aiocbp);
  rtems_test_assert (status == EINVAL);

    /* Invalid offset */
  puts ("Init: aio_read() - EINVAL [aio_offset]");

  status = aio_read (aiocbp);
  rtems_test_assert (status == -1);

  result = aio_return (aiocbp);
  rtems_test_assert (result == -1);
  status = aio_error (aiocbp);
  rtems_test_assert (status == EINVAL);

  free_aiocb (aiocbp);

  /* Invalid request priority */
  puts ("Init: aio_write() - EINVAL [aio_reqprio]");

  aiocbp = create_aiocb (fd);
  aiocbp->aio_reqprio = AIO_PRIO_DELTA_MAX + 1;
  status = aio_write (aiocbp);
  rtems_test_assert (status == -1);

  result = aio_return (aiocbp);
  rtems_test_assert (result == -1);
  status = aio_error (aiocbp);
  rtems_test_assert (status == EINVAL);

   /* Invalid request priority */
  puts ("Init: aio_read() - EINVAL [aio_reqprio]");

  status = aio_read (aiocbp);
  rtems_test_assert (status == -1);

  result = aio_return (aiocbp);
  rtems_test_assert (result == -1);
  status = aio_error (aiocbp);
  rtems_test_assert (status == EINVAL);

  /* Invalid request aio_cancel */
  puts ("Init: aio_cancel() - EINVAL ");

  status = aio_cancel (WRONG_FD, aiocbp);
  rtems_test_assert (status == -1);

  result = aio_return (aiocbp);
  rtems_test_assert (result == -1);
  status = aio_error (aiocbp);
  rtems_test_assert (status == EINVAL);

  /* Invalid operation to aio_fsync */
  puts ("Init: aio_fsync() - EINVAL ");
  status = aio_fsync (-1, aiocbp);
  rtems_test_assert (status == -1);

  result = aio_return (aiocbp);
  rtems_test_assert (result == -1);
  status = aio_error (aiocbp);
  rtems_test_assert (status == EINVAL);

  free_aiocb (aiocbp);

  puts ("*** END OF POSIX AIO TEST 01 ***");

  close (fd);
  rtems_test_exit (0);

  return NULL;

}
