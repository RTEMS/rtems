/**
 * @file
 *
 * @ingroup RTEMSBSPsARMCycV
 */

/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#define ARM_CP15_TEXT_SECTION BSP_START_TEXT_SECTION

#include <bsp.h>
#include <bsp/start.h>
#include <bsp/arm-cp15-start.h>
#include <bsp/arm-a9mpcore-start.h>
#include <bsp/linker-symbols.h>
#include <bsp/alt_address_space.h>
#include <bsp/socal/socal.h>
#include <bsp/socal/alt_sdr.h>
#include <bsp/socal/hps.h>

/* 1 MB reset default value for address filtering start */
#define BSPSTART_L2_CACHE_ADDR_FILTERING_START_RESET 0x100000

BSP_START_TEXT_SECTION void bsp_start_hook_0( void )
{
  arm_cp15_instruction_cache_invalidate();
  arm_cp15_data_cache_invalidate_all_levels();
  arm_a9mpcore_start_hook_0();
}

BSP_START_TEXT_SECTION static void setup_mmu_and_cache(void)
{
  uint32_t ctrl = arm_cp15_start_setup_mmu_and_cache(
    ARM_CP15_CTRL_A | ARM_CP15_CTRL_M,
    ARM_CP15_CTRL_AFE | ARM_CP15_CTRL_Z
  );

  arm_cp15_start_setup_translation_table_and_enable_mmu_and_cache(
    ctrl,
    (uint32_t *) bsp_translation_table_base,
    ARM_MMU_DEFAULT_CLIENT_DOMAIN,
    &arm_cp15_start_mmu_config_table[0],
    arm_cp15_start_mmu_config_table_size
  );
}

BSP_START_TEXT_SECTION void bsp_start_hook_1( void )
{
  uint32_t addr_filt_start;
  uint32_t addr_filt_end;

  /* Disable ECC. Preloader respectively UBoot enable ECC.
     But they do run without interrupts. Our BSP will enable interrupts
     and get spurious ECC error interrupts. Thus we disasable ECC
     until we either know about a better handling or Altera has modified
     it's SDRAM settings to not create possibly false ECC errors */
  uint32_t ctlcfg = alt_read_word( ALT_SDR_CTL_CTLCFG_ADDR );
  ctlcfg &= ALT_SDR_CTL_CTLCFG_ECCEN_CLR_MSK;
  alt_write_word( ALT_SDR_CTL_CTLCFG_ADDR, ctlcfg );

  /* Perform L3 remap register programming first by setting the desired new MPU
     address space 0 mapping. Assume BOOTROM in order to be able to boot the
     second core. */
  alt_addr_space_remap(
    ALT_ADDR_SPACE_MPU_ZERO_AT_BOOTROM,
    ALT_ADDR_SPACE_NONMPU_ZERO_AT_SDRAM,
    ALT_ADDR_SPACE_H2F_ACCESSIBLE,
    ALT_ADDR_SPACE_LWH2F_ACCESSIBLE );

  /* Next, adjust the L2 cache address filtering range. Set the start address
   * to the default reset value and retain the existing end address
   * configuration. */
  alt_l2_addr_filter_cfg_get( &addr_filt_start, &addr_filt_end );

  if ( addr_filt_start != BSPSTART_L2_CACHE_ADDR_FILTERING_START_RESET ) {
    alt_l2_addr_filter_cfg_set( BSPSTART_L2_CACHE_ADDR_FILTERING_START_RESET,
                                addr_filt_end );
  }

  arm_a9mpcore_start_hook_1();
  bsp_start_copy_sections();
  setup_mmu_and_cache();
#ifndef RTEMS_SMP
  /* Enable unified L2 cache */
  rtems_cache_enable_data();
#endif
  bsp_start_clear_bss();
}
