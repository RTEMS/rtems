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
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>
#include <shm_driver.h>

void *Shm_Convert_address(
  void *addr
)
{
  return ( addr );
}
