/*  addrconv.v
 *
 *  No address range conversion is required.
 *
 *  Input parameters:
 *    addr    - address to convert
 *
 *  Output parameters:
 *    returns - converted address
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <bsp.h>
#include <shm.h>

void *Shm_Convert_address(
  void *addr
)
{
  return ( addr );
}
