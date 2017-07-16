/*
 * Copyright (c) 2016 Chris Johns <chrisj@rtems.org>.
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

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include <rtems/rtems-debugger.h>
#include <rtems/debugger/rtems-debugger-server.h>
#include <rtems/debugger/rtems-debugger-remote.h>

static rtems_debugger_remote* remotes[4];

int
rtems_debugger_remote_register(rtems_debugger_remote* remote)
{
  size_t r;
  if (remote->begin == NULL || remote->end == NULL ||
      remote->connect == NULL || remote->disconnect == NULL ||
      remote->isconnected == NULL ||
      remote->read == NULL || remote->write == NULL) {
    errno = EINVAL;
    return -1;
  }
  for (r = 0; r < RTEMS_DEBUGGER_NUMOF(remotes); ++r) {
    if (remotes[r] == NULL) {
      remotes[r] = remote;
      return 0;
    }
  }
  errno = ENOSPC;
  return -1;
}

rtems_debugger_remote*
rtems_debugger_remote_find(const char* name)
{
  size_t r;
  for (r = 0; r < RTEMS_DEBUGGER_NUMOF(remotes); ++r) {
    if (remotes[r] != NULL) {
      if (strcasecmp(name, remotes[r]->name) == 0)
        return remotes[r];
    }
  }
  return NULL;
}
