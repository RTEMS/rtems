/*  void Shm_Get_configuration( localnode, &shmcfg )
 *
 *  This routine initializes, if necessary, and returns a pointer
 *  to the Shared Memory Configuration Table for the HP PA-RISC
 *  simulator.
 *
 *  INPUT PARAMETERS:
 *    localnode - local node number
 *    shmcfg    - address of pointer to SHM Config Table
 *
 *  OUTPUT PARAMETERS:
 *    *shmcfg   - pointer to SHM Config Table
 *
 *  NOTES:  The MP interrupt used is the Runway bus' ability to directly
 *          address the control registers of up to four CPUs and cause
 *          interrupts on them.
 *
 *          The following table illustrates the configuration limitations:
 *
 *                                   BUS     MAX
 *                          MODE    ENDIAN  NODES
 *                        ========= ====== =======
 *                         POLLED    BIG    2+
 *                        INTERRUPT  BIG    2..4 (on Runway)
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

#include <bsp.h>

#include <shm_driver.h>

#define INTERRUPT 0        /* can be interrupt or polling */
#define POLLING   1

#define HPPA_RUNWAY_PROC_HPA_BASE  ((void *) 0xFFFA0000)

/* given a processor number, where is its HPA? */
#define HPPA_RUNWAY_HPA(cpu)   \
  ((rtems_unsigned32) (HPPA_RUNWAY_PROC_HPA_BASE + ((cpu) * 0x2000)))

#define HPPA_RUNWAY_REG_IO_EIR_OFFSET   0x000

shm_config_table BSP_shm_cfgtbl;

extern void Shm_Cause_interrupt_pxfl( rtems_unsigned32 node );

void Shm_Get_configuration(
  rtems_unsigned32   localnode,
  shm_config_table **shmcfg
)
{
   BSP_shm_cfgtbl.base         = (vol_u32 *) 0x44000000;
   BSP_shm_cfgtbl.length       = 16 * KILOBYTE;
   BSP_shm_cfgtbl.format       = SHM_BIG;

   BSP_shm_cfgtbl.cause_intr   = Shm_Cause_interrupt_pxfl;

#ifdef NEUTRAL_BIG
   BSP_shm_cfgtbl.convert      = NULL_CONVERT;
#else
   BSP_shm_cfgtbl.convert      = CPU_swap_u32;
#endif

#if ( POLLING == 1 )
   BSP_shm_cfgtbl.poll_intr    = POLLED_MODE;
   BSP_shm_cfgtbl.Intr.address = NO_INTERRUPT;
   BSP_shm_cfgtbl.Intr.value   = NO_INTERRUPT;
   BSP_shm_cfgtbl.Intr.length  = NO_INTERRUPT;
#else
   BSP_shm_cfgtbl.poll_intr    = INTR_MODE;
   BSP_shm_cfgtbl.Intr.address =
        (vol_u32 *) (HPPA_RUNWAY_HPA( localnode - 1) +
                     HPPA_RUNWAY_REG_IO_EIR_OFFSET);
   BSP_shm_cfgtbl.Intr.value   = HPPA_INTERRUPT_EXTERNAL_MPCI;
   BSP_shm_cfgtbl.Intr.length  = LONG;
#endif

   *shmcfg = &BSP_shm_cfgtbl;
}

