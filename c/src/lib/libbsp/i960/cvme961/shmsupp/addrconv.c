/*  Shm_Convert_address
 *
 *  This routine takes into account the peculiar short VME address
 *  of the CVME961 board.  The CVME961 maps short address space
 *  0xffffxxxx to 0xb400xxxx.
 *
 *  Input parameters:
 *    address - address to convert
 *
 *  Output parameters:
 *    returns - converted address
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
#include <bsp.h>
#include "shm.h"

void *Shm_Convert_address(
  void *address
)
{
  rtems_unsigned32 workaddr = (rtems_unsigned32) address;

  if ( workaddr >= 0xffff0000 )
    workaddr = (workaddr & 0xffff) | 0xb4000000;
  return ( (rtems_unsigned32 *)workaddr );
}
