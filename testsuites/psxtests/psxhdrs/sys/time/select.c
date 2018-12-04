/**
 *  @file
 *  @brief select() API Conformance Test
 */

/*
 *  COPYRIGHT (c) <2018>.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Permission to use, copy, modify, and/or distribute this software
 *  for any purpose with or without fee is hereby granted.
 * 
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 *  WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
 *  BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
 *  OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 *  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 *  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/time.h>

int test(void);

int test(void)
{
  int callback;
  int nfds = 0;
  struct timeval tv;
  time_t tv_sec = 0;
  suseconds_t tv_usec = 0;
  fd_set readfds, writefds, errorfds;

  tv.tv_sec = tv_sec;
  tv.tv_usec = tv_usec;
  FD_ZERO(&readfds);
  FD_ZERO(&writefds);
  FD_ZERO(&errorfds);
  callback = select(nfds, &readfds, &writefds, &errorfds, &tv);
  return callback;
}
