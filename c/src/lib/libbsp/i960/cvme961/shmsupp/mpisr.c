/*  Shm_isr_cvme961()
 *
 *  NOTE: This routine is not used when in polling mode.  Either
 *        this routine OR Shm_clockisr is used in a particular system.
 *
 *        There must be sufficient time after the IACK (read at
 *        0xb600000x) for the VIC068 to clear the interrupt request
 *        before the interrupt request is cleared from IPND (sf0).
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

#include <rtems.h>
#include <bsp.h>
#include "shm_driver.h"

rtems_isr Shm_isr_cvme961(
  rtems_vector_number vector
)
{
  rtems_unsigned32 vic_vector;

  /* enable_tracing(); */
  vic_vector = (*(volatile rtems_unsigned8 *)0xb6000007);
                                           /* reset intr by reading */
                                           /*   vector at IPL=3 */
  Shm_Interrupt_count += 1;
  rtems_multiprocessing_announce();
  (*(volatile rtems_unsigned8 *)0xa000005f) = 0; /* clear ICMS0 */
  i960_clear_intr( 6 );

}

/*  void _Shm_setvec( )
 *
 *  This driver routine sets the SHM interrupt vector to point to the
 *  driver's SHM interrupt service routine.
 *
 *  NOTE: See pp. 21-22, 36-39 of the CVME961 Manual for more info.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters: NONE
 */

void Shm_setvec()
{
  rtems_unsigned32 isrlevel;

  rtems_interrupt_disable( isrlevel );
                                        /* set SQSIO4 CTL REG for */
                                        /*   VME slave address */
    (*(rtems_unsigned8 *)0xc00000b0) =
      (Shm_RTEMS_MP_Configuration->node - 1) | 0x10;
    set_vector( Shm_isr_cvme961, 6, 1 );
                                        /* set ICMS Bector Base Register */
    (*(rtems_unsigned8 *)0xa0000053) = 0x60;  /* XINT6 vector is 0x62 */
                                        /* set  ICMS Intr Control Reg */
    (*(rtems_unsigned8 *)0xa0000047) = 0xeb;  /* ICMS0 enabled, IPL=0 */
    (*(rtems_unsigned8 *)0xa000005f) = 0;     /* clear ICMS0 */
  rtems_interrupt_enable( isrlevel );
}
