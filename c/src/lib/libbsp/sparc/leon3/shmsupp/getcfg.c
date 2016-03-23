/**
 *  @file
 *
 *  LEON3 Shared Memory Driver Support - Configuration
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

/* Let user override this configuration by declaring this a weak variable */
shm_config_table BSP_shm_cfgtbl __attribute__((weak)) =
{
  (vol_u32 *)0x40000000,    /* USER OVERRIDABLE */
  0x00001000,               /* USER OVERRIDABLE */
  SHM_BIG,
  NULL_CONVERT,
  INTR_MODE,
  Shm_Cause_interrupt,
  {
    NULL,
    0,                      /* USER OVERRIDABLE - Uses default MP-IRQ if 0 */
    4,
  },
};

void Shm_Get_configuration(
  uint32_t   localnode,
  shm_config_table **shmcfg
)
{
  int i;
  unsigned int tmp;
  rtems_multiprocessing_table *mptable;

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

  BSP_shm_cfgtbl.poll_intr    = INTR_MODE;
  BSP_shm_cfgtbl.Intr.address =
     (vol_u32 *) &(LEON3_IrqCtrl_Regs->force[LEON3_Cpu_Index]);
  if (BSP_shm_cfgtbl.Intr.value == 0)
    BSP_shm_cfgtbl.Intr.value = 1 << LEON3_mp_irq; /* Use default MP-IRQ */
  BSP_shm_cfgtbl.Intr.length  = 4;

  if (LEON3_Cpu_Index == 0) {
    tmp = 0;
    mptable = rtems_configuration_get_user_multiprocessing_table();
    for (i = 1; i < mptable->maximum_nodes; i++)
      tmp |= (1 << i);
    LEON3_IrqCtrl_Regs->mpstat = tmp;
  }

  *shmcfg = &BSP_shm_cfgtbl;
}
