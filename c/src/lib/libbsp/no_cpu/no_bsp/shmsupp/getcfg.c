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

/*
 *  configured if currently polling of interrupt driven
 */

#define INTERRUPT 0        /* XXX: */
#define POLLING   1        /* XXX: fix me -- is polling ONLY!!! */

shm_config_table BSP_shm_cfgtbl;

void Shm_Get_configuration(
  uint32_t           localnode,
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
