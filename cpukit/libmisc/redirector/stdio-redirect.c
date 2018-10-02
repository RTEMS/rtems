/*
 * Copyright (C) 2014 Chris Johns (chrisj@rtems.org)
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution.
 *
 * This software with is provided ``as is'' and with NO WARRANTY.
 */

/*
 * RTEMS std redirector.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>

#include <rtems/thread.h>
#include <rtems/error.h>
#include <rtems/stdio-redirect.h>

#define RTEMS_STDIO_REDIRECT_RUNNING  (1 << 0)
#define RTEMS_STDIO_REDIRECT_FINISHED (1 << 1)

static void
rtems_stdio_redirect_lock(rtems_stdio_redirect* sr)
{
  rtems_mutex_lock(&sr->lock);
}

static void
rtems_stdio_redirect_unlock(rtems_stdio_redirect* sr)
{
  rtems_mutex_unlock(&sr->lock);
}

static void
rtems_stdio_redirect_write (rtems_stdio_redirect* sr, const char* buf, ssize_t len)
{
  rtems_stdio_redirect_lock(sr);

  if (sr->buffer)
  {
    if (len >= sr->buffer_size)
    {
      ssize_t offset = len - sr->buffer_size;
      memcpy (sr->buffer, buf + offset, sr->buffer_size);
      sr->in  = 0;
      sr->full = true;
    }
    else
    {
      if ((sr->in + len) > sr->buffer_size)
      {
        ssize_t bytes = sr->buffer_size - sr->in;
        memcpy (sr->buffer + sr->in, buf, bytes);
        buf += bytes;
        len -= bytes;
        sr->in = 0;
        sr->full = true;
      }
      else
      {
        memcpy (sr->buffer + sr->in, buf, len);
        sr->in += len;
      }
    }
  }

  if (sr->handler)
    sr->handler(buf, len);

  rtems_stdio_redirect_unlock(sr);
}

static rtems_task
rtems_stdio_redirect_reader(rtems_task_argument arg)
{
  rtems_stdio_redirect* sr = (rtems_stdio_redirect*) arg;

  while (sr->state & RTEMS_STDIO_REDIRECT_RUNNING)
  {
    ssize_t r = read (sr->pipe[0], sr->input, sr->input_size);

    if (r <= 0)
      break;

    if (sr->echo)
      write (sr->fd_dup, sr->input, r);

    rtems_stdio_redirect_write (sr, sr->input, r);
  }

  sr->state |= RTEMS_STDIO_REDIRECT_FINISHED;

  rtems_task_exit();
}

rtems_stdio_redirect*
rtems_stdio_redirect_open(int                          fd,
                          rtems_task_priority          priority,
                          size_t                       stack_size,
                          ssize_t                      input_size,
                          ssize_t                      buffer_size,
                          bool                         echo,
                          rtems_stdio_redirect_handler handler)
{
  rtems_stdio_redirect* sr;
  rtems_name            name;
  rtems_status_code     sc;

  sr = malloc(sizeof(*sr));
  if (!sr)
  {
    fprintf(stderr, "error: stdio-redirect: no memory\n");
    return NULL;
  }

  memset(sr, 0, sizeof(*sr));

  sr->input_size = input_size;
  sr->input = malloc(input_size);
  if (!sr->input)
  {
    fprintf(stderr, "error: stdio-redirect: no memory for input\n");
    free(sr);
    return NULL;
  }

  if (buffer_size)
  {
    sr->buffer_size = buffer_size;
    sr->buffer = malloc(buffer_size);
    if (!sr->buffer)
    {
      fprintf(stderr, "error: stdio-redirect: no memory for buffer\n");
      free(sr->input);
      free(sr);
      return NULL;
    }
  }

  sr->fd = fd;

  sr->fd_dup = dup(fd);
  if (sr->fd_dup < 0)
  {
    fprintf(stderr, "error: stdio-redirect: dup: %s\n", strerror(errno));
    free(sr->buffer);
    free(sr->input);
    free(sr);
    return NULL;
  }

  if (pipe(sr->pipe) < 0)
  {
    fprintf(stderr, "error: stdio-redirect: pipe create: %s\n", strerror(errno));
    free(sr->buffer);
    free(sr->input);
    free(sr);
    return NULL;

  }

  sr->echo = echo;
  sr->handler = handler;

  rtems_mutex_init(&sr->lock, "stdio-redirect");

  name = rtems_build_name ('S', 'R', '0' + (fd / 10), '0' + (fd % 10));
  sc = rtems_task_create (name,
                          priority,
                          stack_size,
                          RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_NO_ASR,
                          RTEMS_LOCAL | RTEMS_NO_FLOATING_POINT,
                          &sr->reader);
  if (sc != RTEMS_SUCCESSFUL)
  {
    fprintf(stderr, "error: stdio-redirect: reader create: %s\n", rtems_status_text(sc));
    rtems_mutex_destroy(&sr->lock);
    free(sr->buffer);
    free(sr->input);
    free(sr);
    return NULL;
  }

  sr->state |= RTEMS_STDIO_REDIRECT_RUNNING;

  if (dup2 (sr->pipe[1], fd) < 0)
  {
    fprintf(stderr, "error: stdio-redirect: dup2: %s\n", strerror(errno));
    free(sr->buffer);
    free(sr->input);
    free(sr);
    return NULL;
  }

  sc = rtems_task_start (sr->reader,
                         rtems_stdio_redirect_reader,
                         (rtems_task_argument) sr);

  if (sc != RTEMS_SUCCESSFUL)
  {
    fprintf(stderr, "error: stdio-redirect: reader start: %s\n", rtems_status_text(sc));
    rtems_task_delete(sr->reader);
    rtems_mutex_destroy(&sr->lock);
    free(sr->buffer);
    free(sr->input);
    free(sr);
    return NULL;
  }

  return sr;
}

void
rtems_stdio_redirect_close(rtems_stdio_redirect* sr)
{
  rtems_stdio_redirect_lock(sr);

  sr->state &= ~RTEMS_STDIO_REDIRECT_RUNNING;
  close(sr->pipe[0]);

  rtems_stdio_redirect_unlock(sr);

  while (sr->state & RTEMS_STDIO_REDIRECT_FINISHED)
  {
    usleep(250UL * 1000000UL);
  }

  rtems_stdio_redirect_lock(sr);

  dup2(sr->fd, sr->fd_dup);

  free(sr->buffer);
  free(sr->input);

  rtems_stdio_redirect_unlock(sr);

  rtems_mutex_destroy(&sr->lock);

  free(sr);
}

ssize_t
rtems_stdio_redirect_read(rtems_stdio_redirect* sr,
                          char*                 buffer,
                          ssize_t               length)
{
  ssize_t written = 0;

  rtems_stdio_redirect_lock(sr);

  if (sr->buffer)
  {
    ssize_t rem = 0;

    if (sr->full)
    {
      if (length < sr->buffer_size)
      {
        if (length > sr->in)
        {
          rem = length - sr->in;
          memcpy (buffer, sr->buffer + sr->buffer_size - rem, rem);
        }

        memcpy (buffer + rem, sr->buffer, sr->in);
        written = length;
      }
      else
      {
        rem = sr->buffer_size - sr->in;
        memcpy (buffer, sr->buffer + sr->in, rem);
        memcpy (buffer + rem, sr->buffer, sr->in);
        written = sr->buffer_size;
      }

      sr->full = false;
    }
    else if (length < sr->in)
    {
      rem = sr->in - length;
      memcpy (buffer, sr->buffer + rem, length);
      written = length;
    }
    else
    {
      memcpy (buffer, sr->buffer, sr->in);
      written = sr->in;
    }
  }

  rtems_stdio_redirect_unlock(sr);

  return written;
}
