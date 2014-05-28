/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
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

#include <bsp.h>
#include <bsp/start.h>
#include <bsp/arm-cp15-start.h>
#include <bsp/arm-a9mpcore-start.h>
#include <bsp/linker-symbols.h>
#include <alt_address_space.h>
#include "socal/socal.h"
#include "socal/alt_sdr.h"
#include "socal/hps.h"
#include "../include/arm-cache-l1.h"

#ifdef RTEMS_SMP
  #define MMU_DATA_READ_WRITE ARMV7_MMU_DATA_READ_WRITE_SHAREABLE
#else
  #define MMU_DATA_READ_WRITE ARMV7_MMU_DATA_READ_WRITE_CACHED
#endif

/* 1 MB reset default value for address filtering start */
#define BSPSTART_L2_CACHE_ADDR_FILTERING_START_RESET 0x100000

#ifndef BSPSTARTHOOKS_MIN
#define BSPSTARTHOOKS_MIN( a, b ) ( ( a ) < ( b ) ? ( a ) : ( b ) )
#endif

LINKER_SYMBOL( bsp_section_nocache_size );
LINKER_SYMBOL( bsp_section_nocache_end );
LINKER_SYMBOL( bsp_section_nocache_begin );

BSP_START_DATA_SECTION static const arm_cp15_start_section_config
  altcycv_mmu_config_table[] = {
  ARMV7_CP15_START_DEFAULT_SECTIONS,
  {
    .begin = (uint32_t) bsp_section_nocache_begin,
    .end   = (uint32_t) bsp_section_nocache_end,

    .flags = ARMV7_MMU_DATA_READ_WRITE | ARM_MMU_SECT_TEX_0
  }, { /* Periphery area */
    .begin = 0xFC000000U,
    .end   = 0x00000000U,
    .flags = ARMV7_MMU_DEVICE
  }
};

BSP_START_TEXT_SECTION static void setup_mmu_and_cache( const uint32_t CPU_ID )
{
  uint32_t       ctrl  = arm_cp15_get_control();
  const uint32_t CORES = BSPSTARTHOOKS_MIN(
    (uintptr_t) bsp_processor_count,
    rtems_configuration_get_maximum_processors() );

  /* We expect the L1 caches and program flow prediction to be off */
  assert( ( ctrl & ARM_CP15_CTRL_I ) == 0 );
  assert( ( ctrl & ARM_CP15_CTRL_C ) == 0 );
  assert( ( ctrl & ARM_CP15_CTRL_Z ) == 0 );

  ctrl = arm_cp15_start_setup_mmu_and_cache(
    ARM_CP15_CTRL_A | ARM_CP15_CTRL_M,
    ARM_CP15_CTRL_AFE
    );

  if( CPU_ID == 0 ) {
    arm_cp15_start_setup_translation_table(
      (uint32_t *) bsp_translation_table_base,
      ARM_MMU_DEFAULT_CLIENT_DOMAIN,
      &altcycv_mmu_config_table[0],
      RTEMS_ARRAY_SIZE( altcycv_mmu_config_table )
    );
  } else {
    /* FIXME: Sharing the translation table between processors is brittle */
    arm_cp15_set_translation_table_base((uint32_t *) bsp_translation_table_base);
  }

  /* Enable MMU */
  ctrl |= ARM_CP15_CTRL_M;

  arm_cp15_set_control( ctrl );
  
  if( CPU_ID == (CORES - 1) ) {
    /* Enable all cache levels for the last core */
    rtems_cache_enable_instruction();
    rtems_cache_enable_data();
  } else {
    /* Enable only L1 cache */
    arm_cache_l1_enable_data();
    arm_cache_l1_enable_instruction();
  }

  /* Enable flow control prediction aka. branch prediction */

/* TODO: With the current network stack 06-Feb2014 in_checksum()
 * becomes a severe performance bottle neck with branch prediction enabled
   ctrl |= ARM_CP15_CTRL_Z;
   arm_cp15_set_control(ctrl);*/
}

BSP_START_TEXT_SECTION void bsp_start_hook_0( void )
{
  uint32_t ctrl;
  volatile a9mpcore_scu *scu    = (volatile a9mpcore_scu *) BSP_ARM_A9MPCORE_SCU_BASE;
  uint32_t               cpu_id = arm_cortex_a9_get_multiprocessor_cpu_id();
  const uint32_t         CORES  = BSPSTARTHOOKS_MIN(
    (uintptr_t) bsp_processor_count,
    rtems_configuration_get_maximum_processors() );

  assert( cpu_id < CORES );

  if( cpu_id < CORES ) {
    if( cpu_id == 0 ) {
      ctrl = arm_cp15_mmu_disable( 32 );

      ctrl &= ~( ARM_CP15_CTRL_I | ARM_CP15_CTRL_C | ARM_CP15_CTRL_Z );
      arm_cp15_set_control( ctrl );

      /* Enable Snoop Control Unit (SCU) */
      arm_a9mpcore_start_scu_enable( scu );
    }

#ifdef RTEMS_SMP
    /* Enable cache coherency support for this processor */
    uint32_t actlr = arm_cp15_get_auxiliary_control();
    actlr |= ARM_CORTEX_A9_ACTL_SMP | ARM_CORTEX_A9_ACTL_FW;
    arm_cp15_set_auxiliary_control(actlr);
#endif

    if (cpu_id == 0) {
      arm_a9mpcore_start_scu_invalidate(scu, cpu_id, 0xF);
    }

    setup_mmu_and_cache( cpu_id );

#ifdef RTEMS_SMP
    if (cpu_id != 0) {
      arm_a9mpcore_start_set_vector_base();

      arm_gic_irq_initialize_secondary_cpu();

      arm_cp15_set_domain_access_control(
        ARM_CP15_DAC_DOMAIN(ARM_MMU_DEFAULT_CLIENT_DOMAIN, ARM_CP15_DAC_CLIENT)
      );
      _SMP_Start_multitasking_on_secondary_processor();
    }
#endif
  } else {
    /* FIXME: Shutdown processor */
    while (1) {
      __asm__ volatile ("wfi");
    }
  }
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

  bsp_start_clear_bss();
}