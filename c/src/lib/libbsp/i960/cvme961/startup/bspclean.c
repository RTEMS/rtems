/*
 *  This routine is used to return control to the NINDY monitor
 *  and is automatically invoked at shutdown.
 *
 *  NOTES:  DOES NOT RETURN!!!
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems.h>
#include "bsp.h"

void bsp_cleanup( void )
{
  extern void start( void  );

  register volatile void *start_addr = 0;

  asm volatile( "mov   0,g0;"
                "fmark ;"
                "syncf ;"
                ".word    0xfeedface ; "
                "bx       (%0)" : "=r" (start_addr) : "0" (start_addr) );
 /*  The constant 0xfeedface is a magic word for break which
  *  is defined by NINDY.  The branch extended restarts the
  *  application if the user types "go".
  */
}
