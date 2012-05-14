/*
 * fifo.c: POSIX FIFO/pipe for RTEMS
 *
 * Author: Wei Shen <cquark@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */


#if HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef RTEMS_POSIX_API
#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#endif

#include <errno.h>
#include <stdlib.h>

#include <rtems.h>
#include <rtems/libio_.h>

#include "pipe.h"


#define MIN(a, b) ((a) < (b)? (a): (b))

#define LIBIO_ACCMODE(_iop) ((_iop)->flags & LIBIO_FLAGS_READ_WRITE)
#define LIBIO_NODELAY(_iop) ((_iop)->flags & LIBIO_FLAGS_NO_DELAY)

static rtems_id pipe_semaphore = RTEMS_ID_NONE;


#define PIPE_EMPTY(_pipe) (_pipe->Length == 0)
#define PIPE_FULL(_pipe)  (_pipe->Length == _pipe->Size)
#define PIPE_SPACE(_pipe) (_pipe->Size - _pipe->Length)
#define PIPE_WSTART(_pipe) ((_pipe->Start + _pipe->Length) % _pipe->Size)

#define PIPE_LOCK(_pipe)  \
  ( rtems_semaphore_obtain(_pipe->Semaphore, RTEMS_WAIT, RTEMS_NO_TIMEOUT)  \
   == RTEMS_SUCCESSFUL )

#define PIPE_UNLOCK(_pipe)  rtems_semaphore_release(_pipe->Semaphore)

#define PIPE_READWAIT(_pipe)  \
  ( rtems_barrier_wait(_pipe->readBarrier, RTEMS_NO_TIMEOUT)  \
   == RTEMS_SUCCESSFUL)

#define PIPE_WRITEWAIT(_pipe)  \
  ( rtems_barrier_wait(_pipe->writeBarrier, RTEMS_NO_TIMEOUT)  \
   == RTEMS_SUCCESSFUL)

#define PIPE_WAKEUPREADERS(_pipe) \
  do {uint32_t n; rtems_barrier_release(_pipe->readBarrier, &n); } while(0)

#define PIPE_WAKEUPWRITERS(_pipe) \
  do {uint32_t n; rtems_barrier_release(_pipe->writeBarrier, &n); } while(0)


#ifdef RTEMS_POSIX_API
#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__

#include <rtems/rtems/barrier.h>
#include <rtems/score/thread.h>

/* Set barriers to be interruptible by signals. */
static void pipe_interruptible(pipe_control_t *pipe)
{
  Objects_Locations location;

  _Barrier_Get(pipe->readBarrier, &location)->Barrier.Wait_queue.state
    |= STATES_INTERRUPTIBLE_BY_SIGNAL;
  _Thread_Enable_dispatch();
  _Barrier_Get(pipe->writeBarrier, &location)->Barrier.Wait_queue.state
    |= STATES_INTERRUPTIBLE_BY_SIGNAL;
  _Thread_Enable_dispatch();
}
#endif

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
  if (! pipe->Buffer)
    goto err_buf;

  err = -ENOMEM;

  if (rtems_barrier_create(
        rtems_build_name ('P', 'I', 'r', c),
        RTEMS_BARRIER_MANUAL_RELEASE, 0,
        &pipe->readBarrier) != RTEMS_SUCCESSFUL)
    goto err_rbar;
  if (rtems_barrier_create(
        rtems_build_name ('P', 'I', 'w', c),
        RTEMS_BARRIER_MANUAL_RELEASE, 0,
        &pipe->writeBarrier) != RTEMS_SUCCESSFUL)
    goto err_wbar;
  if (rtems_semaphore_create(
        rtems_build_name ('P', 'I', 's', c), 1,
        RTEMS_BINARY_SEMAPHORE | RTEMS_FIFO,
        RTEMS_NO_PRIORITY, &pipe->Semaphore) != RTEMS_SUCCESSFUL)
    goto err_sem;

#ifdef RTEMS_POSIX_API
  pipe_interruptible(pipe);
#endif

  *pipep = pipe;
  if (c ++ == 'z')
    c = 'a';
  return 0;

err_sem:
  rtems_barrier_delete(pipe->writeBarrier);
err_wbar:
  rtems_barrier_delete(pipe->readBarrier);
err_rbar:
  free(pipe->Buffer);
err_buf:
  free(pipe);
  return err;
}

/* Called with pipe_semaphore held. */
static inline void pipe_free(
  pipe_control_t *pipe
)
{
  rtems_barrier_delete(pipe->readBarrier);
  rtems_barrier_delete(pipe->writeBarrier);
  rtems_semaphore_delete(pipe->Semaphore);
  free(pipe->Buffer);
  free(pipe);
}

static int pipe_lock(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (pipe_semaphore == RTEMS_ID_NONE) {
    rtems_libio_lock();

    if (pipe_semaphore == RTEMS_ID_NONE) {
      sc = rtems_semaphore_create(
        rtems_build_name('P', 'I', 'P', 'E'),
        1,
        RTEMS_BINARY_SEMAPHORE | RTEMS_INHERIT_PRIORITY | RTEMS_PRIORITY,
        RTEMS_NO_PRIORITY,
        &pipe_semaphore
      );
    }

    rtems_libio_unlock();
  }

  if (sc == RTEMS_SUCCESSFUL) {
    sc = rtems_semaphore_obtain(pipe_semaphore, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  }

  if (sc == RTEMS_SUCCESSFUL) {
    return 0;
  } else {
    return -ENOMEM;
  }
}

static void pipe_unlock(void)
{
#ifdef RTEMS_DEBUG
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc =
#endif
   rtems_semaphore_release(pipe_semaphore);
  #ifdef RTEMS_DEBUG
    if (sc != RTEMS_SUCCESSFUL) {
      rtems_fatal_error_occurred(0xdeadbeef);
    }
  #endif
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

  err = pipe_lock();
  if (err)
    return err;

  pipe = *pipep;
  if (pipe == NULL) {
    err = pipe_alloc(&pipe);
    if (err)
      goto out;
  }

  if (! PIPE_LOCK(pipe))
    err = -EINTR;

  if (*pipep == NULL) {
    if (err)
      pipe_free(pipe);
    else
      *pipep = pipe;
  }

out:
  pipe_unlock();
  return err;
}

/*
 * Interface to file system close.
 *
 * *pipep points to pipe control structure. When the last user releases pipe,
 * it will be set to NULL.
 */
void pipe_release(
  pipe_control_t **pipep,
  rtems_libio_t *iop
)
{
  pipe_control_t *pipe = *pipep;
  uint32_t mode;

  #if defined(RTEMS_DEBUG)
    /* WARN pipe not freed and pipep not set to NULL! */
    if (pipe_lock())
      rtems_fatal_error_occurred(0xdeadbeef);

    /* WARN pipe not released! */
    if (!PIPE_LOCK(pipe))
      rtems_fatal_error_occurred(0xdeadbeef);
  #endif

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
  iop->flags &= ~LIBIO_FLAGS_OPEN;
  if(iop->pathinfo.ops->unlink_h(&iop->pathinfo))
    return;
#endif

}

/*
 * Interface to file system open.
 *
 * *pipep points to pipe control structure. If called with *pipep = NULL,
 * fifo_open will try allocating and initializing a control structure. If the
 * call succeeds, *pipep will be set to address of new control structure.
 */
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
          PIPE_UNLOCK(pipe);
          if (! PIPE_READWAIT(pipe))
            goto out_error;
          if (! PIPE_LOCK(pipe))
            goto out_error;
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
          PIPE_UNLOCK(pipe);
          if (! PIPE_WRITEWAIT(pipe))
            goto out_error;
          if (! PIPE_LOCK(pipe))
            goto out_error;
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

/*
 * Interface to file system read.
 */
ssize_t pipe_read(
  pipe_control_t *pipe,
  void           *buffer,
  size_t          count,
  rtems_libio_t  *iop
)
{
  int chunk, chunk1, read = 0, ret = 0;

  if (! PIPE_LOCK(pipe))
    return -EINTR;

  while (read < count) {
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
      PIPE_UNLOCK(pipe);
      if (! PIPE_READWAIT(pipe))
        ret = -EINTR;
      if (! PIPE_LOCK(pipe)) {
        /* WARN waitingReaders not restored! */
        ret = -EINTR;
        goto out_nolock;
      }
      pipe->waitingReaders --;
      if (ret != 0)
        goto out_locked;
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
  }

out_locked:
  PIPE_UNLOCK(pipe);

out_nolock:
  if (read > 0)
    return read;
  return ret;
}

/*
 * Interface to file system write.
 */
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

  if (! PIPE_LOCK(pipe))
    return -EINTR;

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
      PIPE_UNLOCK(pipe);
      if (! PIPE_WRITEWAIT(pipe))
        ret = -EINTR;
      if (! PIPE_LOCK(pipe)) {
        /* WARN waitingWriters not restored! */
        ret = -EINTR;
        goto out_nolock;
      }
      pipe->waitingWriters --;
      if (ret != 0)
        goto out_locked;

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

out_nolock:
#ifdef RTEMS_POSIX_API
  /* Signal SIGPIPE */
  if (ret == -EPIPE)
    kill(getpid(), SIGPIPE);
#endif

  if (written > 0)
    return written;
  return ret;
}

/*
 * Interface to file system ioctl.
 */
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

    if (! PIPE_LOCK(pipe))
      return -EINTR;

    /* Return length of pipe */
    *(unsigned int *)buffer = pipe->Length;
    PIPE_UNLOCK(pipe);
    return 0;
  }

  return -EINVAL;
}
