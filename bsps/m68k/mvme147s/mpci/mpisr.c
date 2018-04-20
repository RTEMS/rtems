/**
 * @file
 *
 *  NOTE: This routine is not used when in polling mode.  Either
 *        this routine OR Shm_clockisr is used in a particular system.
 */

/*
 *  COPYRIGHT (c) 1989-1999, 2016.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  MVME147 port for TNI - Telecom Bretagne
 *  by Dominique LE CAMPION (Dominique.LECAMPION@enst-bretagne.fr)
 *  June 1996
 */

#include <rtems.h>
#include <bsp.h>
#include <shm_driver.h>

static rtems_isr Shm_isr_mvme147(rtems_vector_number vector)
{
  (void) vector;
  Shm_Interrupt_count += 1;
  rtems_multiprocessing_announce();
  vme_gcsr->global_1 = 1; /* clear SIGLP intr */
}

/*
 *  This driver routine sets the SHM interrupt vector to point to the
 *  driver's SHM interrupt service routine.
 */
void Shm_setvec(void)
{
  /* may need to disable intr */
  set_vector( Shm_isr_mvme147, VME_SIGLP_VECTOR, 1 );
}
