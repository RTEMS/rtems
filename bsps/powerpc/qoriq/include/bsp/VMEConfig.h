/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCQorIQ
 *
 * @brief This header file provides the interfaces used by VME bus device
 *   drivers.
 *
 * Note that for the MVME2500, you need the PCIe support from libbsd for this to
 * work.
 */

/*
 * Copyright (C) 2023 embedded brains GmbH & Co. KG
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

#ifndef RTEMS_BSP_VME_CONFIG_H
#define RTEMS_BSP_VME_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define _VME_DRIVER_TSI148

/*
 * Base address of the PCI that is used for the VME bridge. Value is set in
 * libbsd during device discovery.
 */
extern uintptr_t bsp_vme_pcie_base_address;

#define PCI_MEM_BASE 0
#define PCI_DRAM_OFFSET 0

/*
 * NOTE: shared vmeconfig.c uses hardcoded window lengths that match this layout
 *
 * The memory length of the PCIe controllers on the P2020 processor is
 * 0x20000000. The Tsi148 registers are mapped at the bsp_vme_pcie_base_address
 * with a size of 0x1000. Therefore the VME windows are arranged a bit different
 * then on other BSPs.
 */
#define _VME_A32_WIN0_ON_PCI   (bsp_vme_pcie_base_address + 0x10000000)
#define _VME_A24_ON_PCI        (bsp_vme_pcie_base_address + 0x03000000)
#define _VME_A16_ON_PCI        (bsp_vme_pcie_base_address + 0x02000000)
#define _VME_CSR_ON_PCI        (bsp_vme_pcie_base_address + 0x01000000)

/* FIXME: Make this a BSP config option */
#define _VME_A32_WIN0_ON_VME 0x20000000

/*
 * FIXME: The fixed QORIQ_IRQ_EXT_0 is valid for the MVME2500 board. In theory
 * there should be some possibility to get that information from the device tree
 * or from PCI config space. But I didn't find it anywhere.
 */
#define BSP_VME_INSTALL_IRQ_MGR(err) \
  do { \
    err = qoriq_pic_set_sense_and_polarity(\
      QORIQ_IRQ_EXT_0, \
      QORIQ_EIRQ_TRIGGER_LEVEL_LOW, \
      NULL \
    ); \
    if (err == 0) { \
      err = vmeTsi148InstallIrqMgrAlt(0, 0, QORIQ_IRQ_EXT_0, -1); \
    } \
  } while (0)

/* Add prototypes that are in all VMEConfig.h files */
extern int BSP_VMEInit(void);
extern int BSP_VMEIrqMgrInstall(void);
extern unsigned short (*_BSP_clear_vmebridge_errors)(int);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RTEMS_BSP_VME_CONFIG_H */
