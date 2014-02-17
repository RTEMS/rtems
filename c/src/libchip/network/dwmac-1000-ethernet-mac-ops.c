/**
 * @file
 *
 * @brief Operations for the dwmac 1000 ethernet mac
 *
 * DWMAC_1000_ETHERNET_MAC_OPS will be accessible in the API header and can be
 * passed to the configuration data if handling a DWMAC 1000 driver
 */

/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#include "dwmac-common.h"

extern const dwmac_common_dma_ops  dwmac_dma_ops_1000;
extern const dwmac_common_core_ops dwmac_core_ops_1000;

const dwmac_ethernet_mac_ops       DWMAC_1000_ETHERNET_MAC_OPS =
  DWMAC_ETHERNET_MAC_OPS_INITIALIZER(
    &dwmac_core_ops_1000,
    &dwmac_dma_ops_1000
    );
