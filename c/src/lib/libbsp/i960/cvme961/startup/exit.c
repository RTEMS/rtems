/*  exit
 *
 *  This routine is used to return control to the NINDY monitor
 *  and is automatically invoked by the STDIO exit() routine.
 *
 *  INPUT:
 *    status - exit status
 *
 *  OUTPUT: NONE
 *
 *  NOTES:  DOES NOT RETURN!!!
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

#include <rtems.h>
#include "bsp.h"

void _exit( )
{
  asm volatile( "mov   0,g0; \
                 fmark ; \
                 syncf ; \
                 .word    0xfeedface ; \
                 bx       start" : : );
 /*  The constant 0xfeedface is a magic word for break which
  *  is defined by NINDY.  The branch extended restarts the
  *  application if the user types "go".
  */
}
