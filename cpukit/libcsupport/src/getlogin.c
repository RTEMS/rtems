/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Get User Name
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

#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#include <rtems/seterr.h>
#include <rtems/userenv.h>

#include <unistd.h>
#include <pwd.h>

/**
 *  4.2.4 Get User Name, P1003.1b-1993, p. 87
 *
 *  @note P1003.1c/D10, p. 49 adds getlogin_r().
 */
char *getlogin( void )
{
  (void) getlogin_r( _POSIX_types_Getlogin_buffer, LOGIN_NAME_MAX );
  return _POSIX_types_Getlogin_buffer;
}

/**
 *  4.2.4 Get User Name, P1003.1b-1993, p. 87
 *
 *  @note P1003.1c/D10, p. 49 adds getlogin_r().
 */
int getlogin_r(
  char   *name,
  size_t  namesize
)
{
  struct passwd *pw;
  char          *pname;

  if ( !name ) 
    return EFAULT;

  if ( namesize < LOGIN_NAME_MAX )
    return ERANGE;

  /* Set the pointer to a default name */
  pname = "";

  pw = getpwuid(getuid());
  if ( pw )
   pname = pw->pw_name;

  strncpy( name, pname, LOGIN_NAME_MAX );
  return 0;
}
