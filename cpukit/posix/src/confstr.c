/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *  @brief Implementation for confstr method
 */

/*
 * Copyright (C) 2020 Eshan Dhawan
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


#include <sys/cdefs.h>
#include <sys/param.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

/* Many programming environment flags have same values
* this block is added to raise error if the flags value change
*/

#if (_CS_POSIX_V6_ILP32_OFF32_CFLAGS != _CS_POSIX_V7_ILP32_OFF32_CFLAGS)
#error "_CS_POSIX_V6_ILP32_OFF32_CFLAGS and _CS_POSIX_V7_ILP32_OFF32_CFLAGS flag values not equal"
#endif

#if (_CS_POSIX_V6_ILP32_OFF32_LDFLAGS != _CS_POSIX_V7_ILP32_OFF32_LDFLAGS)
#error "_CS_POSIX_V6_ILP32_OFF32_LDFLAGS and _CS_POSIX_V7_ILP32_OFF32_LDFLAGS flag values not equal"
#endif

#if (_CS_POSIX_V6_ILP32_OFF32_LIBS!= _CS_POSIX_V7_ILP32_OFF32_LIBS)
#error "_CS_POSIX_V6_ILP32_OFF32_LIBS and _CS_POSIX_V7_ILP32_OFF32_LIBS flag values not equal"
#endif

#if (_CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS != _CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS)
#error "_CS_POSIX_V6_LPBIG_OFFBIG_CFLAG and _CS_POSIX_V7_LPBIG_OFFBIG_CFLAG flag values not equal"
#endif

#if (_CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS != _CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS)
#error "_CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS and _CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS flag values not equal"
#endif

#if (_CS_POSIX_V7_LPBIG_OFFBIG_LIBS != _CS_POSIX_V7_LPBIG_OFFBIG_LIBS)
#error "_CS_POSIX_V6_LPBIG_OFFBIG_LIBS and _CS_POSIX_V7_LPBIG_OFFBIG_LIBS flag values not equal"
#endif

#if (_CS_POSIX_V6_ILP32_OFFBIG_CFLAGS != _CS_POSIX_V7_ILP32_OFFBIG_CFLAGS)
#error "_CS_POSIX_V6_ILP32_OFFBIG_CFLAGS and _CS_POSIX_V7_ILP32_OFFBIG_CFLAGS flag values not equal"
#endif

#if (_CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS != _CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS)
#error "_CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS and _CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS flag values not equal"
#endif

#if (_CS_POSIX_V7_ILP32_OFFBIG_LIBS != _CS_POSIX_V7_ILP32_OFFBIG_LIBS)
#error "_CS_POSIX_V6_ILP32_OFFBIG_LIBS and _CS_POSIX_V7_ILP32_OFFBIG_LIBS flag values not equal"
#endif

#if (_CS_POSIX_V6_LP64_OFF64_CFLAGS != _CS_POSIX_V7_LP64_OFF64_CFLAGS)
#error "_CS_POSIX_V6_LP64_OFF64_CFLAGS and _CS_POSIX_V7_LP64_OFF64_CFLAGS flag values not equal"
#endif

#if (_CS_POSIX_V6_LP64_OFF64_LDFLAGS != _CS_POSIX_V7_LP64_OFF64_LDFLAGS)
#error "_CS_POSIX_V6_LP64_OFF64_LDFLAGS and _CS_POSIX_V7_LP64_OFF64_LDFLAGS flag values not equal"
#endif

#if (_CS_POSIX_V7_LP64_OFF64_LIBS != _CS_POSIX_V7_LP64_OFF64_LIBS)
#error "_CS_POSIX_V6_LP64_OFF64_LIBS and _CS_POSIX_V7_LP64_OFF64_LIBS flag values not equal"
#endif

#if (_CS_V6_ENV != _CS_V7_ENV)
#error "_CS_V6_ENV and _CS_V7_ENV flag values not equal"
#endif

size_t confstr(int name, char *buf, size_t len){
  const char *p;
  const char UPE[] = "unsupported programming environment";

  switch (name) {
    case _CS_PATH:
      errno = EINVAL;
      return (0);
    case _CS_POSIX_V7_ILP32_OFF32_CFLAGS:
    case _CS_POSIX_V7_ILP32_OFF32_LDFLAGS:
    case _CS_POSIX_V7_ILP32_OFF32_LIBS:
    case _CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS:
    case _CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS:
    case _CS_POSIX_V7_LPBIG_OFFBIG_LIBS:
    case _CS_POSIX_V7_ILP32_OFFBIG_CFLAGS:
    case _CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS:
    case _CS_POSIX_V7_ILP32_OFFBIG_LIBS:
    case _CS_POSIX_V7_LP64_OFF64_CFLAGS:
    case _CS_POSIX_V7_LP64_OFF64_LDFLAGS:
    case _CS_POSIX_V7_LP64_OFF64_LIBS:
    case _CS_V7_ENV:
      p = UPE;
      if (len != 0 && buf != NULL){
        strlcpy(buf, p, len);
      }
      return (strlen(p) + 1);
    default:
      errno = EINVAL;
      return (0);
  }
}
