/*
 *  GDB Support Routines for the Mongoose-V
 *
 *  COPYRIGHT (c) 1989-2002.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems.h>
#include <rtems/bspIo.h>


char getDebugChar (void)
{
  return 0;
}

void putDebugChar (char c)
{
  /* big hack */
  printk( "%c" );
}

