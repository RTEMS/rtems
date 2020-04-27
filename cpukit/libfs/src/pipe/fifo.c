/**
 * @file
 *
 * @ingroup FIFO_PIPE
 *
 * @brief FIFO/Pipe Support
 */

/*
 * Author: Wei Shen <cquark@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/param.h>
#include <sys/filio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <rtems.h>
#include <rtems/libio_.h>
#include <rtems/pipe.h>

#define LIBIO_ACCMODE(_iop) (rtems_libio_iop_flags(_iop) & LIBIO_FLAGS_READ_WRITE)
#define LIBIO_NODELAY(_iop) rtems_libio_iop_is_no_delay(_iop)

static rtems_mutex pipe_mutex = RTEMS_MUTEX_INITIALIZER("Pipes");


#define PIPE_EMPTY(_pipe) (_pipe->Length == 0)
#define PIPE_FULL(_pipe)  (_pipe->Length == _pipe->Size)
#define PIPE_SPACE(_pipe) (_pipe->Size - _pipe->Length)
#define PIPE_WSTART(_pipe) ((_pipe->Start + _pipe->Length) % _pipe->Size)

#define PIPE_LOCK(_pipe) rtems_mutex_lock(&(_pipe)->Mutex)

#define PIPE_UNLOCK(_pipe) rtems_mutex_unlock(&(_pipe)->Mutex)

#define PIPE_READWAIT(_pipe)  \
  rtems_condition_variable_wait(&(_pipe)->readBarrier, &(_pipe)->Mutex)

#define PIPE_WRITEWAIT(_pipe)  \
  rtems_condition_variable_wait(&(_pipe)->writeBarrier, &(_pipe)->Mutex)

#define PIPE_WAKEUPREADERS(_pipe) \
  rtems_condition_variable_broadcast(&(_pipe)->readBarrier)

#define PIPE_WAKEUPWRITERS(_pipe) \
  rtems_condition_variable_broadcast(&(_pipe)->writeBarrier)

/*
 * Alloc pipe control structure, buffer, and resources.
 * Called with pipe_semaphore held.
 */
static int pipe_alloc(
  pipe_control_t **pipep
)
{
  static char c = 'a';
  pipe_control_t *pipe;
  int err = -ENOMEM;

  pipe = malloc(sizeof(pipe_control_t));
  if (pipe == NULL)
    return err;
  memset(pipe, 0, sizeof(pipe_control_t));

  pipe->Size = PIPE_BUF;
  pipe->Buffer = malloc(pipe->Size);
  if (pipe->Buffer == NULL) {
    free(pipe);
    return -ENOMEM;
  }

  rtems_condition_variable_init(&pipe->readBarrier, "Pipe Read");
  rtems_condition_variable_init(&pipe->writeBarrier, "Pipe Write");
  rtems_mutex_init(&pipe->Mutex, "Pipe");

  *pipep = pipe;
  if (c ++ == 'z')
    c = 'a';
  return 0;
}

/* Called with pipe_semaphore held. */
static inline void pipe_free(
  pipe_control_t *pipe
)
{
  rtems_condition_variable_destroy(&pipe->readBarrier);
  rtems_condition_variable_destroy(&pipe->writeBarrier);
  rtems_mutex_destroy(&pipe->Mutex);
  free(pipe->Buffer);
  free(pipe);
}

static void pipe_lock(void)
{
  rtems_mutex_lock(&pipe_mutex);
}

static void pipe_unlock(void)
{
  rtems_mutex_unlock(&pipe_mutex);
}

/*
 * If called with *pipep = NULL, pipe_new will call pipe_alloc to allocate a
 * pipe control structure and set *pipep to its address.
 * pipe is locked, when pipe_new returns with no error.
 */
static int pipe_new(
  pipe_control_t **pipep
)
{
  pipe_control_t *pipe;
  int err = 0;

  _Assert( pipep );
  pipe_lock();

  pipe = *pipep;
  if (pipe == NULL) {
    err = pipe_alloc(&pipe);
    if (err) {
      pipe_unlock();
      return err;
    }
  }

  PIPE_LOCK(pipe);

  *pipep = pipe;
  pipe_unlock();
  return err;
}

void pipe_release(
  pipe_control_t **pipep,
  rtems_libio_t *iop
)
{
  pipe_control_t *pipe = *pipep;
  uint32_t mode;

  pipe_lock();
  PIPE_LOCK(pipe);

  mode = LIBIO_ACCMODE(iop);
  if (mode & LIBIO_FLAGS_READ)
     pipe->Readers --;
  if (mode & LIBIO_FLAGS_WRITE)
     pipe->Writers --;

  PIPE_UNLOCK(pipe);

  if (pipe->Readers == 0 && pipe->Writers == 0) {
#if 0
    /* To delete an anonymous pipe file when all users closed it */
    if (pipe->Anonymous)
      delfile = TRUE;
#endif
    pipe_free(pipe);
    *pipep = NULL;
  }
  else if (pipe->Readers == 0 && mode != LIBIO_FLAGS_WRITE)
    /* Notify waiting Writers that all their partners left */
    PIPE_WAKEUPWRITERS(pipe);
  else if (pipe->Writers == 0 && mode != LIBIO_FLAGS_READ)
    PIPE_WAKEUPREADERS(pipe);

  pipe_unlock();

#if 0
  if (! delfile)
    return;
  if (iop->pathinfo.ops->unlink_h == NULL)
    return;

  /* This is safe for IMFS, but how about other FSes? */
  rtems_libio_iop_flags_clear( iop, LIBIO_FLAGS_OPEN );
  if(iop->pathinfo.ops->unlink_h(&iop->pathinfo))
    return;
#endif

}

int fifo_open(
  pipe_control_t **pipep,
  rtems_libio_t *iop
)
{
  pipe_control_t *pipe;
  unsigned int prevCounter;
  int err;

  err = pipe_new(pipep);
  if (err)
    return err;
  pipe = *pipep;

  switch (LIBIO_ACCMODE(iop)) {
    case LIBIO_FLAGS_READ:
      pipe->readerCounter ++;
      if (pipe->Readers ++ == 0)
        PIPE_WAKEUPWRITERS(pipe);

      if (pipe->Writers == 0) {
        /* Not an error */
        if (LIBIO_NODELAY(iop))
          break;

        prevCounter = pipe->writerCounter;
        err = -EINTR;
        /* Wait until a writer opens the pipe */
        do {
          PIPE_READWAIT(pipe);
        } while (prevCounter == pipe->writerCounter);
      }
      break;

    case LIBIO_FLAGS_WRITE:
      pipe->writerCounter ++;

      if (pipe->Writers ++ == 0)
        PIPE_WAKEUPREADERS(pipe);

      if (pipe->Readers == 0 && LIBIO_NODELAY(iop)) {
	PIPE_UNLOCK(pipe);
        err = -ENXIO;
        goto out_error;
      }

      if (pipe->Readers == 0) {
        prevCounter = pipe->readerCounter;
        err = -EINTR;
        do {
          PIPE_WRITEWAIT(pipe);
        } while (prevCounter == pipe->readerCounter);
      }
      break;

    case LIBIO_FLAGS_READ_WRITE:
      pipe->readerCounter ++;
      if (pipe->Readers ++ == 0)
        PIPE_WAKEUPWRITERS(pipe);
      pipe->writerCounter ++;
      if (pipe->Writers ++ == 0)
        PIPE_WAKEUPREADERS(pipe);
      break;
  }

  PIPE_UNLOCK(pipe);
  return 0;

out_error:
  pipe_release(pipep, iop);
  return err;
}

ssize_t pipe_read(
  pipe_control_t *pipe,
  void           *buffer,
  size_t          count,
  rtems_libio_t  *iop
)
{
  int chunk, chunk1, read = 0, ret = 0;

  PIPE_LOCK(pipe);

  while (PIPE_EMPTY(pipe)) {
    /* Not an error */
    if (pipe->Writers == 0)
      goto out_locked;

    if (LIBIO_NODELAY(iop)) {
      ret = -EAGAIN;
      goto out_locked;
    }

    /* Wait until pipe is no more empty or no writer exists */
    pipe->waitingReaders ++;
    PIPE_READWAIT(pipe);
    pipe->waitingReaders --;
  }

  /* Read chunk bytes */
  chunk = MIN(count - read,  pipe->Length);
  chunk1 = pipe->Size - pipe->Start;
  if (chunk > chunk1) {
    memcpy(buffer + read, pipe->Buffer + pipe->Start, chunk1);
    memcpy(buffer + read + chunk1, pipe->Buffer, chunk - chunk1);
  }
  else
    memcpy(buffer + read, pipe->Buffer + pipe->Start, chunk);

  pipe->Start += chunk;
  pipe->Start %= pipe->Size;
  pipe->Length -= chunk;
  /* For buffering optimization */
  if (PIPE_EMPTY(pipe))
    pipe->Start = 0;

  if (pipe->waitingWriters > 0)
    PIPE_WAKEUPWRITERS(pipe);
  read += chunk;

out_locked:
  PIPE_UNLOCK(pipe);

  if (read > 0)
    return read;
  return ret;
}

ssize_t pipe_write(
  pipe_control_t *pipe,
  const void     *buffer,
  size_t          count,
  rtems_libio_t  *iop
)
{
  int chunk, chunk1, written = 0, ret = 0;

  /* Write nothing */
  if (count == 0)
    return 0;

  PIPE_LOCK(pipe);

  if (pipe->Readers == 0) {
    ret = -EPIPE;
    goto out_locked;
  }

  /* Write of PIPE_BUF bytes or less shall not be interleaved */
  chunk = count <= pipe->Size ? count : 1;

  while (written < count) {
    while (PIPE_SPACE(pipe) < chunk) {
      if (LIBIO_NODELAY(iop)) {
        ret = -EAGAIN;
        goto out_locked;
      }

      /* Wait until there is chunk bytes space or no reader exists */
      pipe->waitingWriters ++;
      PIPE_WRITEWAIT(pipe);
      pipe->waitingWriters --;

      if (pipe->Readers == 0) {
        ret = -EPIPE;
        goto out_locked;
      }
    }

    chunk = MIN(count - written, PIPE_SPACE(pipe));
    chunk1 = pipe->Size - PIPE_WSTART(pipe);
    if (chunk > chunk1) {
      memcpy(pipe->Buffer + PIPE_WSTART(pipe), buffer + written, chunk1);
      memcpy(pipe->Buffer, buffer + written + chunk1, chunk - chunk1);
    }
    else
      memcpy(pipe->Buffer + PIPE_WSTART(pipe), buffer + written, chunk);

    pipe->Length += chunk;
    if (pipe->waitingReaders > 0)
      PIPE_WAKEUPREADERS(pipe);
    written += chunk;
    /* Write of more than PIPE_BUF bytes can be interleaved */
    chunk = 1;
  }

out_locked:
  PIPE_UNLOCK(pipe);

#ifdef RTEMS_POSIX_API
  /* Signal SIGPIPE */
  if (ret == -EPIPE)
    kill(getpid(), SIGPIPE);
#endif

  if (written > 0)
    return written;
  return ret;
}

int pipe_ioctl(
  pipe_control_t  *pipe,
  ioctl_command_t  cmd,
  void            *buffer,
  rtems_libio_t   *iop
)
{
  if (cmd == FIONREAD) {
    if (buffer == NULL)
      return -EFAULT;

    PIPE_LOCK(pipe);

    /* Return length of pipe */
    *(unsigned int *)buffer = pipe->Length;
    PIPE_UNLOCK(pipe);
    return 0;
  }

  return -EINVAL;
}
