/*  Shm_isr_mvme147()
 *
 *  NOTE: This routine is not used when in polling mode.  Either
 *        this routine OR Shm_clockisr is used in a particular system.
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

rtems_isr Shm_isr_mvme147()
{
  Shm_Interrupt_count += 1;
  rtems_multiprocessing_announce();
  vme_gcsr->global_1 = 1; /* clear SIGLP intr */
}

/*  void _Shm_setvec( )
 *
 *  This driver routine sets the SHM interrupt vector to point to the
 *  driver's SHM interrupt service routine.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters: NONE
 */

void Shm_setvec()
{
  /* may need to disable intr */
  set_vector( Shm_isr_mvme147, VME_SIGLP_VECTOR, 1 );
}
