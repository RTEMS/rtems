/*  Shm_isr_mvme136()
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
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>
#include <shm.h>

rtems_isr Shm_isr_mvme136()
{
  Shm_Interrupt_count += 1;
  rtems_multiprocessing_announce();
  (*(volatile rtems_unsigned8 *)0xfffb006b) = 0; /* clear MPCSR intr */
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
  set_vector( Shm_isr_mvme136, 75, 1 );
}
