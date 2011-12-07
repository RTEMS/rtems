/**
 * @file src/sup_fs_is_separator.c
 */

/*
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/libio_.h>

/*
 *  rtems_filesystem_is_separator
 *
 *  Function to determine if a character is a path name separator.
 *  This was originally a macro in libio_.h
 *
 *  NOTE:  This function handles MS-DOS and UNIX style names.
 */

int rtems_filesystem_is_separator(char ch)
{
  return ((ch == '/') || (ch == '\\') || (ch == '\0'));
}
