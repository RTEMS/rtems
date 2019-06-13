/*
 * Copyright (c) 2016 embedded brains GmbH Huber.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/start.h>
#include <bsp/pin-config.h>
#include <bsp/atsam-clock-config.h>

#include <chip.h>
#include <include/board_lowlevel.h>
#include <include/board_memories.h>

#define SIZE_0K 0
#define SIZE_32K (32 * 1024)
#define SIZE_64K (64 * 1024)
#define SIZE_128K (128 * 1024)

#define ITCMCR_SZ_0K 0x0
#define ITCMCR_SZ_32K 0x6
#define ITCMCR_SZ_64K 0x7
#define ITCMCR_SZ_128K 0x8

static BSP_START_TEXT_SECTION void efc_send_command(uint32_t eefc)
{
  EFC->EEFC_FCR = eefc | EEFC_FCR_FKEY_PASSWD;
}

static BSP_START_TEXT_SECTION void tcm_enable(void)
{
  SCB->ITCMCR |= SCB_ITCMCR_EN_Msk;
  SCB->DTCMCR |= SCB_DTCMCR_EN_Msk;
}

static BSP_START_TEXT_SECTION void tcm_disable(void)
{
  SCB->ITCMCR &= ~SCB_ITCMCR_EN_Msk;
  SCB->DTCMCR &= ~SCB_DTCMCR_EN_Msk;
}

static BSP_START_TEXT_SECTION bool tcm_setup_and_check_if_do_efc_config(
  uintptr_t tcm_size,
  uint32_t itcmcr_sz
)
{
  if (tcm_size == SIZE_0K && itcmcr_sz == ITCMCR_SZ_0K) {
    tcm_disable();
    return false;
  } else if (tcm_size == SIZE_32K && itcmcr_sz == ITCMCR_SZ_32K) {
    tcm_enable();
    return false;
  } else if (tcm_size == SIZE_64K && itcmcr_sz == ITCMCR_SZ_64K) {
    tcm_enable();
    return false;
  } else if (tcm_size == SIZE_128K && itcmcr_sz == ITCMCR_SZ_128K) {
    tcm_enable();
    return false;
  } else {
    return true;
  }
}

static bool ATSAM_START_SRAM_SECTION sdram_settings_unchanged(void)
{
  return (
    (SDRAMC->SDRAMC_CR == BOARD_Sdram_Config.sdramc_cr) &&
    (SDRAMC->SDRAMC_TR == BOARD_Sdram_Config.sdramc_tr) &&
    (SDRAMC->SDRAMC_MDR == BOARD_Sdram_Config.sdramc_mdr) &&
    (SDRAMC->SDRAMC_CFR1 == BOARD_Sdram_Config.sdramc_cfr1)
  );
}

static void ATSAM_START_SRAM_SECTION setup_CPU_and_SDRAM(void)
{
  SystemInit();
  if (!PMC_IsPeriphEnabled(ID_SDRAMC) || !sdram_settings_unchanged()) {
    BOARD_ConfigureSdram();
  }
}

static void configure_tcm(void)
{
  uintptr_t tcm_size;
  uint32_t itcmcr_sz;

  tcm_size = (uintptr_t) atsam_memory_itcm_size;
  itcmcr_sz = (SCB->ITCMCR & SCB_ITCMCR_SZ_Msk) >> SCB_ITCMCR_SZ_Pos;

  if (tcm_setup_and_check_if_do_efc_config(tcm_size, itcmcr_sz)) {
    if (tcm_size == SIZE_128K) {
      efc_send_command(EEFC_FCR_FCMD_SGPB | EEFC_FCR_FARG(7));
      efc_send_command(EEFC_FCR_FCMD_SGPB | EEFC_FCR_FARG(8));
      tcm_enable();
    } else if (tcm_size == SIZE_64K) {
      efc_send_command(EEFC_FCR_FCMD_CGPB | EEFC_FCR_FARG(7));
      efc_send_command(EEFC_FCR_FCMD_SGPB | EEFC_FCR_FARG(8));
      tcm_enable();
    } else if (tcm_size == SIZE_32K) {
      efc_send_command(EEFC_FCR_FCMD_SGPB | EEFC_FCR_FARG(7));
      efc_send_command(EEFC_FCR_FCMD_CGPB | EEFC_FCR_FARG(8));
      tcm_enable();
    } else {
      efc_send_command(EEFC_FCR_FCMD_CGPB | EEFC_FCR_FARG(7));
      efc_send_command(EEFC_FCR_FCMD_CGPB | EEFC_FCR_FARG(8));
      tcm_disable();
    }
  }
}

void BSP_START_TEXT_SECTION bsp_start_hook_0(void)
{
  system_init_flash(BOARD_MCK);

  PIO_Configure(&atsam_pin_config[0], atsam_pin_config_count);
  MATRIX->CCFG_SYSIO = atsam_matrix_ccfg_sysio;

  configure_tcm();
#if ATSAM_CHANGE_CLOCK_FROM_SRAM != 0
  /* Early copy of .fast_text section for CPU and SDRAM setup. */
  bsp_start_memcpy_libc(
    bsp_section_fast_text_begin,
    bsp_section_fast_text_load_begin,
    (size_t) bsp_section_fast_text_size
  );
#endif
  setup_CPU_and_SDRAM();

  if ((SCB->CCR & SCB_CCR_IC_Msk) == 0) {
    SCB_EnableICache();
  }

  if ((SCB->CCR & SCB_CCR_DC_Msk) == 0) {
    SCB_EnableDCache();
  }

  _SetupMemoryRegion();
}

void BSP_START_TEXT_SECTION bsp_start_hook_1(void)
{
  bsp_start_copy_sections_compact();
  SCB_CleanDCache();
  SCB_InvalidateICache();
  bsp_start_clear_bss();
}
