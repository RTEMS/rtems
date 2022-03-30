/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup utsname Service
 *
 * @brief Get System Name
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
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

#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>
#include <inttypes.h>

#include <rtems.h>
#include <rtems/seterr.h>

/*
 *  4.4.1 Get System Name, P1003.1b-1993, p. 90
 */

int uname(
  struct utsname *name
)
{
  /*  XXX: Here is what Solaris returns...
          sysname = SunOS
          nodename = node_name
          release = 5.3
          version = Generic_101318-12
          machine = sun4m
  */

  if ( !name )
    rtems_set_errno_and_return_minus_one( EFAULT );

  strncpy( name->sysname, "RTEMS", sizeof(name->sysname) );

  snprintf(
    name->nodename,
    sizeof(name->nodename),
    "Node %" PRId16, rtems_object_get_local_node()
  );

  strncpy( name->release, RTEMS_VERSION, sizeof(name->release) );

  strncpy( name->version, "", sizeof(name->version) );

  snprintf( name->machine, sizeof(name->machine), "%s/%s", CPU_NAME, CPU_MODEL_NAME );

  return 0;
}
