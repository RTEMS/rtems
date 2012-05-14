/*  Shm_Get_packet
 *
 *  This routine is the shared memory locked queue MPCI driver
 *  routine used to obtain an empty message packet.
 *
 *  Input parameters:
 *    packet             - address of pointer to packet
 *
 *  Output parameters:
 *    *(cpb->get_packet) - address of allocated packet
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include "shm_driver.h"

rtems_mpci_entry Shm_Get_packet(
  rtems_packet_prefix **packet
)
{
  Shm_Envelope_control *ecb;

  ecb = Shm_Allocate_envelope();
  if ( !ecb )
    rtems_fatal_error_occurred ( SHM_NO_FREE_PKTS );
  *packet = Shm_Envelope_control_to_packet_prefix_pointer( ecb );
}
