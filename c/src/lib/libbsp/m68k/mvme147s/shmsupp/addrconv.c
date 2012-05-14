/*  Shm_Convert_address
 *
 *  This MVME147 has a "normal" view of the VME address space.
 *  No address range conversion is required.
 *
 *  Input parameters:
 *    address - address to convert
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
 *  MVME147 port for TNI - Telecom Bretagne
 *  by Dominique LE CAMPION (Dominique.LECAMPION@enst-bretagne.fr)
 *  June 1996
 */

#include <rtems.h>
#include <bsp.h>
#include <shm_driver.h>

void *Shm_Convert_address(
  void *address
)
{
  return ( address );
}
