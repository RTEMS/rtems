/**
 * @file
 *
 * @ingroup lpc32xx_emc
 *
 * @brief EMC support implementation.
 */

/*
 * Copyright (c) 2010
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp/emc.h>

#include <bsp.h>
#include <bsp/mmu.h>

static volatile lpc_emc *const emc = &lpc32xx.emc;

static volatile lpc32xx_emc_ahb *const emc_ahb = &lpc32xx.emc_ahb [0];

static void dynamic_init(const lpc32xx_emc_dynamic_config *cfg)
{
  uint32_t chip_begin = LPC32XX_BASE_EMC_DYCS_0;
  uint32_t dynamiccontrol = (cfg->control | EMC_DYN_CTRL_CE | EMC_DYN_CTRL_CS)
    & ~EMC_DYN_CTRL_I_MASK;
  size_t i = 0;

  LPC32XX_SDRAMCLK_CTRL = cfg->sdramclk_ctrl;

  emc->dynamicreadconfig = cfg->readconfig;

  /* Timings */
  emc->dynamictrp = cfg->trp;
  emc->dynamictras = cfg->tras;
  emc->dynamictsrex = cfg->tsrex;
  emc->dynamictwr = cfg->twr;
  emc->dynamictrc = cfg->trc;
  emc->dynamictrfc = cfg->trfc;
  emc->dynamictxsr = cfg->txsr;
  emc->dynamictrrd = cfg->trrd;
  emc->dynamictmrd = cfg->tmrd;
  emc->dynamictcdlr = cfg->tcdlr;
  for (i = 0; i < EMC_DYN_CHIP_COUNT; ++i) {
    if (cfg->chip [i].size != 0) {
      emc->dynamic [i].config = cfg->chip [i].config;
      emc->dynamic [i].rascas = cfg->chip [i].rascas;
    }
  }

  /* NOP period */
  emc->dynamiccontrol = dynamiccontrol | EMC_DYN_CTRL_I_NOP;
  lpc32xx_micro_seconds_delay(cfg->nop_time_in_us);

  /* Precharge */
  emc->dynamiccontrol = dynamiccontrol | EMC_DYN_CTRL_I_PALL;
  emc->dynamicrefresh = 1;
  /* FIXME: Why a delay, why this value? */
  lpc32xx_micro_seconds_delay(10);

  /* Refresh timing */
  emc->dynamicrefresh = cfg->refresh;
  /* FIXME: Why a delay, why this value? */
  lpc32xx_micro_seconds_delay(16);

  /* Set modes */
  for (i = 0; i < EMC_DYN_CHIP_COUNT; ++i) {
    if (cfg->chip [i].size != 0) {
      lpc32xx_set_translation_table_entries(
        (void *) chip_begin,
        (void *) (chip_begin + cfg->chip [i].size),
        LPC32XX_MMU_READ_WRITE
      );
      emc->dynamiccontrol = dynamiccontrol | EMC_DYN_CTRL_I_MODE;
      *(volatile uint32_t *)(chip_begin + cfg->chip [i].mode);
      emc->dynamiccontrol = dynamiccontrol | EMC_DYN_CTRL_I_MODE;
      *(volatile uint32_t *)(chip_begin + cfg->chip [i].extmode);
    }
    chip_begin += 0x20000000;
  }

  emc->dynamiccontrol = cfg->control;
}

void lpc32xx_emc_init(const lpc32xx_emc_dynamic_config *dyn_cfg)
{
  /* Enable buffers in AHB ports */
  emc_ahb [0].control = EMC_AHB_PORT_BUFF_EN;
  emc_ahb [3].control = EMC_AHB_PORT_BUFF_EN;
  emc_ahb [4].control = EMC_AHB_PORT_BUFF_EN;

  /* Set AHB port timeouts */
  emc_ahb [0].timeout = EMC_AHB_TIMEOUT(32);
  emc_ahb [3].timeout = EMC_AHB_TIMEOUT(32);
  emc_ahb [4].timeout = EMC_AHB_TIMEOUT(32);

  /* Enable EMC */
  emc->control = EMC_CTRL_E,
  emc->config = 0;

  dynamic_init(dyn_cfg);
}
