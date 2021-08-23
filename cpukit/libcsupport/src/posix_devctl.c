/*
 * Copyright (c) 2016, 2020 Joel Sherrill <joel@rtems.org>.
 * All rights reserved.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define _POSIX_26_C_SOURCE

#include <devctl.h>
#include <sys/ioctl.h>
#include <rtems/seterr.h>

#include  <unistd.h>
#include  <fcntl.h>

int posix_devctl(
  int              fd,
  int              dcmd,
  void *__restrict dev_data_ptr,
  size_t           nbyte,
  int *__restrict  dev_info_ptr
)
{
  int rv = 0;

  /*
   * posix_devctl() is supposed to return an errno. errno needs to be
   * preserved in spite of calling methods (e.g., close, fcntl, and ioctl)
   * that set it.
   */
  int errno_copy = errno;

  /*
   * The POSIX 1003.26 standard allows for library implementations
   * that implement posix_devctl() using ioctl(). In this case,
   * the extra parameters are largely ignored.
   *
   * The FACE Technical Standard requires only that FIONBIO
   * be supported for sockets.
   *
   * This method appears to be rarely implemented and there are
   * no known required use cases for this method beyond those
   * from the FACE Technical Standard.
   */

  /*
   * POSIX 1003.26 mentions that nbyte must be non-negative but this
   * doesn't make sense because size_t is guaranteed to be unsigned.
   */

  /*
   * Since this is implemented on top of ioctl(), the device information
   * is not going to be passed down. Fill it in with zero so the behavior
   * is defined.
   */
  if (dev_info_ptr != NULL) {
    *dev_info_ptr = 0;
  }

  /*
   *
   */
  switch (dcmd) {

    /*
     * The FACE Technical Standard Edition 3.0 and newer requires the SOCKCLOSE
     * ioctl command. This is because the Security Profile does not include
     * close() and applications need a way to close sockets. Closing sockets is
     * a minimum requirement so using close() in the implementation meets that
     * requirement but also lets the application close other file types.
     */
    case SOCKCLOSE:
      if (close(fd) != 0) {
        rv    = errno;
        errno = errno_copy;

        return rv;
      }
      break;

    /*
     * The FACE Technical Standard Edition 3.0 and newer requires the
     * posix_devctl command to support the FIONBIO subcommand.
     */
    case FIONBIO: {
      int tmp_flag;
      int flag;

      if (nbyte != sizeof(int)) {
        return EINVAL;
      }

      tmp_flag = fcntl(fd, F_GETFL, 0);
      if (tmp_flag == -1) {
        rv = errno;
        errno = errno_copy;

        return rv;
      }

      flag = *(int *)dev_data_ptr;

      if (flag != 0) {
        tmp_flag |= O_NONBLOCK;
      } else {
        tmp_flag &= ~O_NONBLOCK;
      }

      (void) fcntl(fd, F_SETFL, tmp_flag);
      break;
    }

    default:
      if (ioctl(fd, dcmd, dev_data_ptr) != 0) {
        rv = errno;
        errno = errno_copy;

        return rv;
      }
      break;
  }

  errno = errno_copy;

  return rv;
}
