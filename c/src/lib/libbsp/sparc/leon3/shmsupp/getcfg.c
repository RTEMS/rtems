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

/* multiprocessor communications interface (MPCI) table */


extern rtems_mpci_entry Shm_Get_packet(
  rtems_packet_prefix **
);

rtems_mpci_entry Shm_Initialization( void );

extern rtems_mpci_entry Shm_Receive_packet(
  rtems_packet_prefix **
);

extern rtems_mpci_entry Shm_Return_packet(
  rtems_packet_prefix *
);

extern rtems_mpci_entry Shm_Send_packet(
  uint32_t,
  rtems_packet_prefix *
);


/* rtems_mpci_table MPCI_table  = { */
/*   100000,                     /\* default timeout value in ticks *\/ */
/*   MAX_PACKET_SIZE,            /\* maximum packet size *\/ */
/*   Shm_Initialization,         /\* initialization procedure   *\/ */
/*   Shm_Get_packet,             /\* get packet procedure       *\/ */
/*   Shm_Return_packet,          /\* return packet procedure    *\/ */
/*   Shm_Send_packet,            /\* packet send procedure      *\/ */
/*   Shm_Receive_packet          /\* packet receive procedure   *\/ */
/* }; */


/*
 *  configured if currently polling of interrupt driven
 */

#define INTERRUPT 0        /* XXX: */
#define POLLING   1        /* XXX: fix me -- is polling ONLY!!! */

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
    1 << LEON3_MP_IRQ,      /* USER OVERRIDABLE */
    4,
  },
};

void Shm_Get_configuration(
  uint32_t   localnode,
  shm_config_table **shmcfg
)
{
  extern rtems_configuration_table Configuration;
  int i;
  unsigned int tmp;

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
    BSP_shm_cfgtbl.Intr.value = 1 << LEON3_MP_IRQ; /* Use default MP-IRQ */
  BSP_shm_cfgtbl.Intr.length  = 4;

  if (LEON3_Cpu_Index == 0) {
    tmp = 0;
    for (i = 1;
         i < (Configuration.User_multiprocessing_table)->maximum_nodes; i++)
      tmp |= (1 << i);
    LEON3_IrqCtrl_Regs->mpstat = tmp;
  }

  *shmcfg = &BSP_shm_cfgtbl;
}
