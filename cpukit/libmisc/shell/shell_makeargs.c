/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @brief Split string into argc/argv style argument list
 */

/*
 * COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
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

#include <string.h>
#include <ctype.h>
#include <rtems/shell.h>

int rtems_shell_make_args(
  char  *commandLine,
  int   *argc_p,
  char **argv_p,
  int    max_args
)
{
  int   argc;
  char *ch;
  int   status = 0;
 
  argc = 0;
  ch = commandLine;

  while ( *ch ) {

    while ( isspace((unsigned char)*ch) ) ch++;

    if ( *ch == '\0' )
      break;

    if ( *ch == '"' ) {
      argv_p[ argc ] = ++ch;
      while ( ( *ch != '\0' ) && ( *ch != '"' ) ) ch++;
    } else {
      argv_p[ argc ] = ch;
      while ( ( *ch != '\0' ) && ( !isspace((unsigned char)*ch) ) ) ch++;
    }

    argc++;

    if ( *ch == '\0' )
      break;

    *ch++ = '\0';

    if ( argc == (max_args-1) ) {
        status = -1;
        break;
    }


  }
  argv_p[ argc ] = NULL;
  *argc_p = argc;
  return status;
}

