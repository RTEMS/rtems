/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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

  tcm_size = (uintptr_t) atsam_memory_dtcm_size;
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
