/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2014 Chris Johns <chrisj@rtems.org>.  All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * @brief Redirect an stdio file decriptor.
 *
 * The is a helper module of code design to redirect an stdio file
 * descriptor. You can optionally have the data buffered and/or you can provide
 * a handler function that is called when data arrives.
 *
 * The module uses standard POSIX calls to implement the redirection and if the
 * threading was POSIX based the code would be portable. Currently the code
 * uses RTEMS threads.
 *
 * Redirecting stderr and stdout is useful in embedded system because you can
 * transport the output off your device or create a web interface that can
 * display the output. This can be a very powerful diagnostic and support tool.
 *
 * The implementation does:
 *
 *  1. Duplicate the file descriptor (fd) to redirect using the dup call. The
 *     duplicated desciptor is used to echo the output out the existing path.
 *
 *  2. Create a pipe using the pipe call.
 *
 *  3. Duplicate the pipe's writer file descriptor to user's file
 *     descriptor. This results in any writes to the user's fd being written to
 *     the pipe.
 *
 *  4. Create a reader task that blocks on the pipe. It optionally calls a
 *     handler and if configured buffers the data.
 */

#if !defined( RTEMS_STDIO_REDIRECT_H )
#define RTEMS_STDIO_REDIRECT_H

#include <stdbool.h>
#include <rtems.h>
#include <rtems/thread.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Handler called whenever redirected data arrives.
 *
 * @param buffer The data.
 * @param length The amount of data in the buffer.
 */
typedef void ( *rtems_stdio_redirect_handler )(
  const char *buffer,
  ssize_t     length
);

/*
 * Redirector data.
 */
typedef struct {
  volatile uint32_t state;       /**< The state. */
  rtems_id          reader;      /**< The reader thread. */
  rtems_mutex       lock;        /**< Lock for this struct. */
  int               fd;          /**< The file descriptor to redirect. */
  int               fd_dup;      /**< Duplicated fd to write to. */
  int               pipe[ 2 ];   /**< The pipe to the reader thread. */
  char             *input;       /**< The input buffer the reader uses. */
  ssize_t           input_size;  /**< The input buffer size. */
  char             *buffer;      /**< Captured redirected data. */
  ssize_t           buffer_size; /**< Capture buffer size. */
  ssize_t           in;          /**< Buffer in index. */
  bool              full;        /**< The buffer is full. */
  bool              echo;        /**< Echo the data out the existing path. */
  rtems_stdio_redirect_handler handler; /**< Redirected data handler. */
} rtems_stdio_redirect;

/*
 * Open a redirector returning the handle to it.
 *
 * @param fd The file descriptor to redirect.
 * @param priority The priority of the reader thread.
 */
rtems_stdio_redirect *rtems_stdio_redirect_open(
  int                          fd,
  rtems_task_priority          priority,
  size_t                       stack_size,
  ssize_t                      input_size,
  ssize_t                      buffer_size,
  bool                         echo,
  rtems_stdio_redirect_handler handler
);

/*
 * Close the redirector.
 */
void rtems_stdio_redirect_close( rtems_stdio_redirect *sr );

/*
 * Get data from the capture buffer. Data read is removed from the buffer.
 *
 * @param sr The stdio redirection handle.
 * @param buffer The buffer data is written into.
 * @param length The size of the buffer.
 * @return ssize_t The amount of data written and -1 or an error.
 */
ssize_t rtems_stdio_redirect_read(
  rtems_stdio_redirect *sr,
  char                 *buffer,
  ssize_t               length
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
