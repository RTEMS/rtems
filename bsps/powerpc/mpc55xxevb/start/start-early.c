/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMPC55XX
 *
 * @brief Early initialization code.
 */

/*
 * Copyright (C) 2008, 2012 embedded brains GmbH & Co. KG
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

#include <bsp/mpc55xx-config.h>
#include <bsp/linker-symbols.h>

/* This function is defined in start.S */
BSP_START_TEXT_SECTION void mpc55xx_start_load_section(
  void *dst,
  const void *src,
  size_t n
);

static BSP_START_TEXT_SECTION void mpc55xx_start_mmu(void)
{
  #ifdef MPC55XX_BOOTFLAGS
    /* If the low bit of bootflag 0 is clear don't change the MMU.  */
    bool do_mmu_config = (mpc55xx_bootflag_0 [0] & 1) != 0;
  #else
    bool do_mmu_config = true;
  #endif

  if (do_mmu_config) {
    mpc55xx_start_mmu_apply_config(
      &mpc55xx_start_config_mmu [0],
      mpc55xx_start_config_mmu_count [0]
    );
  }
}

static BSP_START_TEXT_SECTION void mpc55xx_start_internal_ram(void)
{
  #ifdef MPC55XX_NEEDS_LOW_LEVEL_INIT
    /* Initialize internal SRAM to zero (ECC) */
    bsp_start_zero(
      (char *) bsp_ram_start + MPC55XX_EARLY_STACK_SIZE,
      (size_t) bsp_ram_size - MPC55XX_EARLY_STACK_SIZE
    );
    #ifdef MPC55XX_HAS_SECOND_INTERNAL_RAM_AREA
      bsp_start_zero(&bsp_ram_1_start [0], (size_t) bsp_ram_1_size);
    #endif
  #else
    bsp_start_zero(
      bsp_section_sbss_begin,
      (size_t) bsp_section_sbss_size
    );
    bsp_start_zero(
      bsp_section_bss_begin,
      (size_t) bsp_section_bss_size
    );
  #endif
}

static BSP_START_TEXT_SECTION void mpc55xx_start_load_nocache_section(void)
{
  mpc55xx_start_load_section(
    bsp_section_nocache_begin,
    bsp_section_nocache_load_begin,
    (size_t) bsp_section_nocache_size
  );
  rtems_cache_flush_multiple_data_lines(
    bsp_section_nocache_begin,
    (size_t) bsp_section_nocache_size
  );
}

static BSP_START_TEXT_SECTION void mpc55xx_start_mode_change(void)
{
  #ifdef MPC55XX_NEEDS_LOW_LEVEL_INIT
    #ifdef MPC55XX_HAS_MODE_CONTROL
      uint32_t mctl_key1 = 0x5af0;
      uint32_t mctl_key2 = 0xa50f;
      int i = 0;

      /* Clear any pending RGM status */
      RGM.FES.R = 0xffff;
      RGM.DES.R = 0xffff;

      /* Make sure XOSC and PLLs are on in RUN0 state */
      ME.DRUN_MC.R = 0x001f0074;
      ME.RUN_MC [0].R = 0x001f0074;

      /*
       * Make sure all peripherals are active in DRUN and RUN0 state.
       *
       * FIXME: This might be optimized to reduce power consumtion.
       */
      for (i = 0; i < 8; ++i) {
        ME_RUN_PC_32B_tag run_pc = { .R = ME.RUN_PC [i].R };

        run_pc.B.DRUN = 1;
        run_pc.B.RUN0 = 1;

        ME.RUN_PC [i].R = run_pc.R;
      }

      /* Switch to RUN0 state */
      ME.MCTL.R = 0x40000000 | mctl_key1;
      ME.MCTL.R = 0x40000000 | mctl_key2;

      while (ME.GS.B.S_MTRANS) {
        /* Wait for mode switch to be completed */
      }
    #endif
  #endif
}

static BSP_START_TEXT_SECTION void mpc55xx_start_siu(void)
{
  size_t i = 0;

  for (i = 0; i < mpc55xx_start_config_siu_pcr_count [0]; ++i) {
     const mpc55xx_siu_pcr_config *e = &mpc55xx_start_config_siu_pcr [i];
     int j = e->index;
     int n = j + e->count;
     uint8_t gpdo = e->output;
     uint16_t pcr = e->pcr.R;

     while (j < n) {
       SIU.GPDO [j].R = gpdo;
       SIU.PCR [j].R = pcr;
       ++j;
     }
  }
}

static BSP_START_TEXT_SECTION void mpc55xx_start_ebi_chip_select(void)
{
  #ifdef MPC55XX_HAS_EBI
    size_t i = 0;

    for (i = 0; i < mpc55xx_start_config_ebi_cs_count [0]; ++i) {
      EBI.CS [i] = mpc55xx_start_config_ebi_cs [i];
    }

    for (i = 0; i < mpc55xx_start_config_ebi_cal_cs_count [0]; ++i) {
      EBI.CAL_CS [i] = mpc55xx_start_config_ebi_cal_cs [i];
    }
  #endif
}

static BSP_START_TEXT_SECTION void mpc55xx_start_ebi(void)
{
  #ifdef MPC55XX_HAS_EBI
    size_t i = 0;

    for (i = 0; i < mpc55xx_start_config_ebi_count [0]; ++i) {
      SIU.ECCR.B.EBDF = mpc55xx_start_config_ebi [i].siu_eccr_ebdf;
      EBI.MCR.R = mpc55xx_start_config_ebi [i].ebi_mcr.R;
    }
  #endif
}

#ifdef MPC55XX_NEEDS_LOW_LEVEL_INIT
static BSP_START_TEXT_SECTION bool
mpc55xx_start_is_in_internal_ram(const void *addr)
{
  return (size_t) addr - (size_t) bsp_ram_start < (size_t) bsp_ram_size;
}
#endif

static BSP_START_TEXT_SECTION void mpc55xx_start_clear_bss(void)
{
  #ifdef MPC55XX_NEEDS_LOW_LEVEL_INIT
    if (!mpc55xx_start_is_in_internal_ram(bsp_section_sbss_begin)) {
      bsp_start_zero(
        bsp_section_sbss_begin,
        (size_t) bsp_section_sbss_size
      );
    }

    if (!mpc55xx_start_is_in_internal_ram(bsp_section_bss_begin)) {
      bsp_start_zero(
        bsp_section_bss_begin,
        (size_t) bsp_section_bss_size
      );
    }
  #endif
}

BSP_START_TEXT_SECTION void mpc55xx_start_early(void)
{
  mpc55xx_start_watchdog();
  mpc55xx_start_clock();
  mpc55xx_start_flash();
  #if defined(BSP_DATA_CACHE_ENABLED) || defined(BSP_INSTRUCTION_CACHE_ENABLED)
    mpc55xx_start_cache();
  #endif
  mpc55xx_start_internal_ram();
  mpc55xx_start_load_nocache_section();
  mpc55xx_start_mmu();
  mpc55xx_start_mode_change();
  mpc55xx_start_siu();
  mpc55xx_start_ebi_chip_select();
  mpc55xx_start_ebi();
  mpc55xx_start_clear_bss();
}
