/*
 *  ctermid() - POSIX 1003.1b 4.7.1 - Generate Terminal Pathname
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
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

  strcpy( s, ctermid_name );
  return s;
}

#endif
