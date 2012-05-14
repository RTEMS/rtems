/*
 *  ctermid() - POSIX 1003.1b 4.7.1 - Generate Terminal Pathname
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#if defined(RTEMS_NEWLIB)

#include <stdio.h>
#include <string.h>

static char *ctermid_name = "/dev/console";

char *ctermid(
  char *s
)
{
  if ( !s )
    return ctermid_name;

  /*
   *  We have no way of knowing the length of the user provided buffer.
   *  It may not be large enough but there is no way to know that. :(
   *  So this is a potential buffer owerrun that we can do nothing about.
   */
  strcpy( s, ctermid_name );
  return s;
}

#endif
