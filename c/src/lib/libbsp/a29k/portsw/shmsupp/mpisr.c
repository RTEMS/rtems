/*  Shm_isr_nobsp()
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

#include <rtems.h>
#include <bsp.h>
#include <shm.h>

#ifndef lint
static char _sccsid[] = "@(#)mpisr.c 04/08/96     1.1\n";
#endif

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
  /* XXX: FIX ME!!! */
}
