/*  Shm_isr_nobsp()
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <shm_driver.h>

rtems_isr Shm_isr_nobsp( void )
{
  /*
   *  If this routine has to do anything other than the mpisr.c
   *  found in the generic driver, then copy the contents of the generic
   *  mpisr.c and augment it to satisfy this particular board.  Typically,
   *  you need to have a board specific mpisr.c when the interrupt
   *  must be cleared.
   *
   *  If the generic mpisr.c satisifies your requirements, then
   *  remove this routine from your target's shmsupp/mpisb.c file.
   *  Then simply install the generic Shm_isr in the Shm_setvec
   *  routine below.
   */
}

/*  Shm_setvec
 *
 *  This driver routine sets the SHM interrupt vector to point to the
 *  driver's SHM interrupt service routine.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters: NONE
 */


void Shm_setvec( void )
{
  LEON_Unmask_interrupt(LEON3_MP_IRQ);
  set_vector(Shm_isr, LEON_TRAP_TYPE(LEON3_MP_IRQ), 1);
}
