/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Get Supplementary IDs
 *
 *  @ingroup libcsupport
 */

/*
 * COPYRIGHT (C) 1989-1999 On-Line Applications Research Corporation (OAR).
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>

#include <rtems/seterr.h>

/**
 *  4.2.3 Get Supplementary IDs, P1003.1b-1993, p. 86
 */
int getgroups(
  int    gidsetsize,
  gid_t  grouplist[]
)
{
  int rv;
  struct passwd pwd;
  struct passwd *pwd_res;
  char buf[256];
  gid_t gid;
  const char *user;
  struct group *grp;

  rv = getpwuid_r(getuid(), &pwd, &buf[0], sizeof(buf), &pwd_res);
  if (rv != 0) {
    return rv;
  }

  gid = pwd.pw_gid;
  user = pwd.pw_name;

  setgrent();

  while ((grp = getgrent()) != NULL) {
    char **mem = &grp->gr_mem[0];

    if (grp->gr_gid == gid) {
      continue;
    }

    while (*mem != NULL) {
      if (strcmp(*mem, user) == 0) {
        if (rv < gidsetsize) {
          grouplist[rv] = grp->gr_gid;
        }

        ++rv;

        break;
      }

      ++mem;
    }
  }

  endgrent();

  if (gidsetsize == 0 || rv <= gidsetsize) {
    return rv;
  } else {
    rtems_set_errno_and_return_minus_one(EINVAL);
  }
}
