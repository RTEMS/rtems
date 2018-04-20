/**
 *  @file
 *
 *  Template for Shared Memory Driver Interrupt Support
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <shm_driver.h>

rtems_isr Shm_isr_nobsp(rtems_vector_number ignored)
{
  /*
   *  If this routine has to do anything other than the mpisr.c
   *  found in the generic driver, then copy the contents of the generic
   *  mpisr.c and augment it to satisfy this particular board.  Typically,
   *  you need to have a board specific mpisr.c when the interrupt
   *  must be cleared.
   *
   *  If the generic mpisr.c satisifies your requirements, then
   *  remove this routine from your target's shmsupp/mpisr.c file.
   *  Then simply install the generic Shm_isr in the Shm_setvec
   *  routine below.
   */
}

/* 
 *  This driver routine sets the SHM interrupt vector to point to the
 *  driver's SHM interrupt service routine.
 */

void Shm_setvec( void )
{
  /* XXX: FIX ME!!! */
}
