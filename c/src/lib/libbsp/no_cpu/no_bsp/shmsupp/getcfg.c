/*  void Shm_Get_configuration( localnode, &shmcfg )
 *
 *  This routine initializes, if necessary, and returns a pointer
 *  to the Shared Memory Configuration Table for the XXX target.
 *
 *  INPUT PARAMETERS:
 *    localnode - local node number
 *    shmcfg    - address of pointer to SHM Config Table
 *
 *  OUTPUT PARAMETERS:
 *    *shmcfg   - pointer to SHM Config Table
 *
XXX: FIX THE COMMENTS BELOW WHEN THE CPU IS KNOWN
 *  NOTES:  The XYZ does not have an interprocessor interrupt.
 *
 *          The following table illustrates the configuration limitations:
 *
 *                                   BUS     MAX
 *                          MODE    ENDIAN  NODES
 *                        ========= ====== =======
 *                         POLLED    BIG    2+
 *                        INTERRUPT **** NOT SUPPORTED ****
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

/*
 *  configured if currently polling of interrupt driven
 */

#define INTERRUPT 0        /* XXX: */
#define POLLING   1        /* XXX: fix me -- is polling ONLY!!! */


shm_config_table BSP_shm_cfgtbl;

void Shm_Get_configuration(
  rtems_unsigned32   localnode,
  shm_config_table **shmcfg
)
{
   BSP_shm_cfgtbl.base         = 0x0;
   BSP_shm_cfgtbl.length       = 1 * MEGABYTE;
   BSP_shm_cfgtbl.format       = SHM_BIG;

   /*
    *  Override cause_intr or shm_isr if your target has
    *  special requirements.
    */

   BSP_shm_cfgtbl.cause_intr   = Shm_Cause_interrupt;

#ifdef NEUTRAL_BIG
   BSP_shm_cfgtbl.convert      = NULL_CONVERT;
#else
   BSP_shm_cfgtbl.convert      = CPU_swap_u32;
#endif

   BSP_shm_cfgtbl.poll_intr    = POLLED_MODE;
   BSP_shm_cfgtbl.Intr.address = NO_INTERRUPT;
   BSP_shm_cfgtbl.Intr.value   = NO_INTERRUPT;
   BSP_shm_cfgtbl.Intr.length  = NO_INTERRUPT;

   *shmcfg = &BSP_shm_cfgtbl;
}
