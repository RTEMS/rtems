/*  void Shm_get_config( localnode, &shmcfg )
 *
 *  This routine initializes, if necessary, and returns a pointer
 *  to the Shared Memory Configuration Table for the UNIX
 *  simulator.
 *
 *  INPUT PARAMETERS:
 *    localnode - local node number
 *    shmcfg    - address of pointer to SHM Config Table
 *
 *  OUTPUT PARAMETERS:
 *    *shmcfg   - pointer to SHM Config Table
 *
 *  NOTES:  This driver is capable of supporting a practically unlimited
 *          number of nodes.
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

#include <bsp.h>
#include <shm.h>

shm_config_table BSP_shm_cfgtbl;

int              semid;

void Shm_Cause_interrupt_unix(
  rtems_unsigned32 node
);

void Shm_Get_configuration(
  rtems_unsigned32   localnode,
  shm_config_table **shmcfg
)
{
  _CPU_SHM_Init(
    Shm_Maximum_nodes,
    Shm_Is_master_node(),
    (void **)&BSP_shm_cfgtbl.base,
    (unsigned32 *)&BSP_shm_cfgtbl.length
  );

  BSP_shm_cfgtbl.format       = SHM_BIG;

  BSP_shm_cfgtbl.cause_intr   = Shm_Cause_interrupt_unix;

#ifdef NEUTRAL_BIG
  BSP_shm_cfgtbl.convert      = NULL_CONVERT;
#else
  BSP_shm_cfgtbl.convert      = CPU_swap_u32;
#endif

  if ( _CPU_SHM_Get_vector() ) {
    BSP_shm_cfgtbl.poll_intr    = INTR_MODE;
    BSP_shm_cfgtbl.Intr.address = (vol_u32 *) _CPU_Get_pid(); /* process id */
    BSP_shm_cfgtbl.Intr.value   = _CPU_SHM_Get_vector();  /* signal to send */
    BSP_shm_cfgtbl.Intr.length  = LONG;
  } else {
    BSP_shm_cfgtbl.poll_intr    = POLLED_MODE;
    BSP_shm_cfgtbl.Intr.address = NO_INTERRUPT;
    BSP_shm_cfgtbl.Intr.value   = NO_INTERRUPT;
    BSP_shm_cfgtbl.Intr.length  = NO_INTERRUPT;
  }

  *shmcfg = &BSP_shm_cfgtbl;
}
