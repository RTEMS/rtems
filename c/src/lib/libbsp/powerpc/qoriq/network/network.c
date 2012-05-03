/**
 * @file
 *
 * @ingroup QorIQ
 *
 * @brief Network configuration.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#define __INSIDE_RTEMS_BSD_TCPIP_STACK__ 1
#define __BSD_VISIBLE 1

#include <assert.h>
#include <string.h>

#include <rtems/rtems_bsdnet.h>
#include <rtems/rtems_bsdnet_internal.h>

#include <libcpu/powerpc-utility.h>

#include <bsp.h>
#include <bsp/tsec.h>
#include <bsp/u-boot.h>
#include <bsp/qoriq.h>

int BSP_tsec_attach(
  struct rtems_bsdnet_ifconfig *config,
  int attaching
)
{
  char *unit_name = NULL;
  int unit_number = rtems_bsdnet_parse_driver_name(config, &unit_name);
  tsec_config tsec_cfg;
  bool has_groups = false;

  memset(&tsec_cfg, 0, sizeof(tsec_cfg));
  config->drv_ctrl = &tsec_cfg;

  if (unit_number <= 0 || unit_number > TSEC_COUNT) {
    return 0;
  }

  switch (ppc_fsl_system_version_sid(ppc_fsl_system_version())) {
    /* P1010 and P1020 */
    case 0x0ec:
    case 0x0e4:
    case 0x0ed:
    case 0x0e5:
      has_groups = true;
      break;
  }

  if (config->hardware_address == NULL) {
    #ifdef HAS_UBOOT
      switch (unit_number) {
        case 1:
          config->hardware_address = bsp_uboot_board_info.bi_enetaddr;
          break;
        case 2:
          config->hardware_address = bsp_uboot_board_info.bi_enet1addr;
          break;
        case 3:
          config->hardware_address = bsp_uboot_board_info.bi_enet2addr;
          break;
        default:
	  assert(0);
          break;
      }
    #else
      assert(0);
    #endif
  }

  switch (unit_number) {
    case 1:
      if (has_groups) {
        tsec_cfg.reg_ptr = &qoriq.tsec_1_group_0;
      } else {
        tsec_cfg.reg_ptr = &qoriq.tsec_1;
      }
      tsec_cfg.mdio_ptr = &qoriq.tsec_1;
      tsec_cfg.irq_num_tx = QORIQ_IRQ_ETSEC_TX_1;
      tsec_cfg.irq_num_rx = QORIQ_IRQ_ETSEC_RX_1;
      tsec_cfg.irq_num_err = QORIQ_IRQ_ETSEC_ER_1;
      tsec_cfg.phy_default = QORIQ_ETSEC_1_PHY_ADDR;
      break;
    case 2:
      if (has_groups) {
        tsec_cfg.reg_ptr = &qoriq.tsec_2_group_0;
      } else {
        tsec_cfg.reg_ptr = &qoriq.tsec_2;
      }
      tsec_cfg.mdio_ptr = &qoriq.tsec_1;
      tsec_cfg.irq_num_tx = QORIQ_IRQ_ETSEC_TX_2;
      tsec_cfg.irq_num_rx = QORIQ_IRQ_ETSEC_RX_2;
      tsec_cfg.irq_num_err = QORIQ_IRQ_ETSEC_ER_2;
      tsec_cfg.phy_default = QORIQ_ETSEC_2_PHY_ADDR;
      break;
    case 3:
      if (has_groups) {
        tsec_cfg.reg_ptr = &qoriq.tsec_3_group_0;
      } else {
        tsec_cfg.reg_ptr = &qoriq.tsec_3;
      }
      tsec_cfg.mdio_ptr = &qoriq.tsec_1;
      tsec_cfg.irq_num_tx = QORIQ_IRQ_ETSEC_TX_3;
      tsec_cfg.irq_num_rx = QORIQ_IRQ_ETSEC_RX_3;
      tsec_cfg.irq_num_err = QORIQ_IRQ_ETSEC_ER_3;
      tsec_cfg.phy_default = QORIQ_ETSEC_3_PHY_ADDR;
      break;
    default:
      assert(0);
      break;
  }

  tsec_cfg.unit_number = unit_number;
  tsec_cfg.unit_name = unit_name;

  return tsec_driver_attach_detach(config, attaching);
}
