/*  getint
 *
 *  This routine reads and returns an integer.
 *  It assumes decimal.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:
 *    returns - number read
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <stdio.h>
#include <stdlib.h>

int getint( void );

int getint( void )
{
  char buff[16];

  gets(buff);
  return atoi(buff);
}
