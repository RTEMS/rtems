/*  Shm_isr_mvme147()
 *
 *  NOTE: This routine is not used when in polling mode.  Either
 *        this routine OR Shm_clockisr is used in a particular system.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  MVME147 port for TNI - Telecom Bretagne
 *  by Dominique LE CAMPION (Dominique.LECAMPION@enst-bretagne.fr)
 *  June 1996
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>
#include <shm.h>

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
