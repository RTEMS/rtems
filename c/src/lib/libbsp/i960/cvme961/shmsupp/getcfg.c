/*  void Shm_Get_configuration( localnode, &shmcfg )
 *
 *  This routine initializes, if necessary, and returns a pointer
 *  to the Shared Memory Configuration Table for the Cyclone CVME961.
 *
 *  INPUT PARAMETERS:
 *    localnode - local node number
 *    shmcfg    - address of pointer to SHM Config Table
 *
 *  OUTPUT PARAMETERS:
 *    *shmcfg   - pointer to SHM Config Table
 *
 *  NOTES:  CVME961 target system has onboard dual-ported memory.  This
 *          file uses the USE_ONBOARD_RAM macro to determine if this
 *          RAM is to be used as the SHM.  If so (i.e. USE_ONBOARD_RAM
 *          is set to 1), it is assumed that the master node's dual
 *          ported memory will be used and that it is configured
 *          correctly.  The node owning the memory CANNOT access it
 *          using a local address.  The "if" insures that the MASTER
 *          node uses a local address to access the dual-ported memory.
 *
 *          The interprocessor interrupt used on the CVME961 is generated
 *          by the VIC068.   The ICMS capablities of the VIC068 are used
 *          to generate interprocessor interrupts for up to eight nodes.
 *
 *           The following table illustrates the configuration limitations:
 *
 *                                   BUS     MAX
 *                          MODE    ENDIAN  NODES
 *                        ========= ====== =======
 *                         POLLED   LITTLE  2+
 *                        INTERRUPT LITTLE  2-8
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
#include "shm_driver.h"

#define USE_ONBOARD_RAM 0             /* use onboard (1) or VME RAM   */
                                      /*   for SHM communications     */

#define INTERRUPT 1                   /* CVME961 target supports both */
#define POLLING   0                   /* polling and interrupt modes  */


shm_config_table BSP_shm_cfgtbl;

void Shm_Get_configuration(
  rtems_unsigned32   localnode,
  shm_config_table **shmcfg
)
{
#if ( USE_ONBOARD_RAM == 1 )
   if ( Shm_RTEMS_MP_Configuration->node == MASTER )
     BSP_shm_cfgtbl.base   = (rtems_unsigned32 *)0x00300000;
   else
     BSP_shm_cfgtbl.base   = (rtems_unsigned32 *)0x10300000;
#else
   BSP_shm_cfgtbl.base     = (rtems_unsigned32 *)0x20000000;
#endif

   BSP_shm_cfgtbl.length       = 1 * MEGABYTE;
   BSP_shm_cfgtbl.format       = SHM_LITTLE;

   BSP_shm_cfgtbl.cause_intr   = Shm_Cause_interrupt;

#ifdef NEUTRAL_BIG
   BSP_shm_cfgtbl.convert      = (void *)CPU_swap_u32;
#else
   BSP_shm_cfgtbl.convert      = NULL_CONVERT;
#endif

#if (POLLING==1)
   BSP_shm_cfgtbl.poll_intr    = POLLED_MODE;
   BSP_shm_cfgtbl.Intr.address = NO_INTERRUPT;
   BSP_shm_cfgtbl.Intr.value   = NO_INTERRUPT;
   BSP_shm_cfgtbl.Intr.length  = NO_INTERRUPT;
#else
   BSP_shm_cfgtbl.poll_intr    = INTR_MODE;
   BSP_shm_cfgtbl.Intr.address =
        (rtems_unsigned32 *) (0xffff0021|((localnode-1) << 12));
                                                          /* use ICMS0 */
   BSP_shm_cfgtbl.Intr.value   = 1;
   BSP_shm_cfgtbl.Intr.length  = BYTE;
#endif

   *shmcfg = &BSP_shm_cfgtbl;

}
