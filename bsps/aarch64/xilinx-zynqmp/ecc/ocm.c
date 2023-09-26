/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64XilinxZynqMP
 *
 * @brief This source file contains the implementation of OCM ECC support.
 */

/*
 * Copyright (C) 2023 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
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
#include <bsp/ecc_priv.h>
#include <bsp/irq.h>
#include <bsp/utility.h>
#include <libcpu/mmu-vmsav8-64.h>

static uintptr_t ocm_base = 0xFF960000;

/* ECC Control */
#define OCM_ECC_CTRL 0x14
/* 0 -> single error, 1 -> continuous errors */
#define OCM_ECC_CTRL_FI_MODE BSP_BIT32(2)
/* When bit is set, detection occurs without correction */
#define OCM_ECC_CTRL_DET_ONLY BSP_BIT32(1)
/* Enable ECC, should only be modified at system boot */
#define OCM_ECC_CTRL_ECC_ON_OFF BSP_BIT32(0)

/* Interrupt Enable */
#define OCM_IE 0xc
#define OCM_IE_UE_RMW BSP_BIT32(10)
#define OCM_IE_UE BSP_BIT32(7)
#define OCM_IE_CE BSP_BIT32(6)

/* Error Response Control */
#define OCM_ERR_CTRL 0x0
#define OCM_ERR_CTRL_UE_RES BSP_BIT32(3)

/*
 * Fault Injection Data, four registers comprising 16 bytes of a data word
 *
 * Bits set to 1 toggle the corresponding bits of the next written word during a
 * fault injection.
 */
#define OCM_FI_D0 0x4c
#define OCM_FI_D1 0x50
#define OCM_FI_D2 0x54
#define OCM_FI_D3 0x58

/* Fault Injection Syndrome */
#define OCM_FI_SY 0x5c
#define OCM_FI_SY_DATA(val) BSP_FLD32(val, 0, 15)
#define OCM_FI_SY_DATA_GET(reg) BSP_FLD32GET(reg, 0, 15)
#define OCM_FI_SY_DATA_SET(reg, val) BSP_FLD32SET(reg, val, 0, 15)

/* Fault Injection Counter */
#define OCM_FI_CNTR 0x74
#define OCM_FI_CNTR_COUNT(val) BSP_FLD32(val, 0, 23)
#define OCM_FI_CNTR_COUNT_GET(reg) BSP_FLD32GET(reg, 0, 23)
#define OCM_FI_CNTR_COUNT_SET(reg, val) BSP_FLD32SET(reg, val, 0, 23)

/* Interrupt Status */
#define OCM_IS 0x4
#define OCM_IS_UE_RMW BSP_BIT32(10)
#define OCM_IS_UE BSP_BIT32(7)
#define OCM_IS_CE BSP_BIT32(6)

/* Interrupt Mask */
#define OCM_IM 0x8
#define OCM_IM_UE_RMW BSP_BIT32(10)
#define OCM_IM_UE BSP_BIT32(7)
#define OCM_IM_CE BSP_BIT32(6)

void zynqmp_ocm_inject_fault( void )
{
  volatile uint32_t *fi_d0 = (uint32_t*)(ocm_base + OCM_FI_D0);
  volatile uint32_t *fi_cnt = (uint32_t*)(ocm_base + OCM_FI_CNTR);
  volatile uint64_t *ocm_top = (uint64_t*)0xFFFFFFF0U;
  volatile uint32_t *ecc_ctrl = (uint32_t*)(ocm_base + OCM_ECC_CTRL);
  uint64_t ocm_tmp = *ocm_top;

  /* Configure OCM to throw constant errors */
  *ecc_ctrl |= OCM_ECC_CTRL_FI_MODE;

  /* Inject a single bit error */
  *fi_d0 = 1;

  /* Configure the clock count after which errors will begin */
  *fi_cnt = 0;

  /* Insert a memory barrier to ensure that fault injection is active */
  _AARCH64_Data_synchronization_barrier();

  /* trigger fault with a write of data that was already at the given address */
  *ocm_top = 0;

  /* Insert a memory barrier to prevent optimization */
  _AARCH64_Data_synchronization_barrier();

  /* Perform read to force reporting of the error */
  *ocm_top;

  /* Disable constant fault mode */
  *ecc_ctrl &= ~(OCM_ECC_CTRL_FI_MODE);

  /* Insert a memory barrier to ensure the mode has changed */
  _AARCH64_Data_synchronization_barrier();

  /* Reset to original value now that constant errors are disabled */
  *ocm_top = ocm_tmp;
}

/* Correctable Error First Failing Address */
#define OCM_CE_FFA 0x1c
#define OCM_CE_FFA_ADDR(val) BSP_FLD32(val, 0, 17)
#define OCM_CE_FFA_ADDR_GET(reg) BSP_FLD32GET(reg, 0, 17)
#define OCM_CE_FFA_ADDR_SET(reg, val) BSP_FLD32SET(reg, val, 0, 17)

/* Correctable Error First Failing Data, four registers comprising 16 bytes */
#define OCM_CE_FFD0 0x20
#define OCM_CE_FFD1 0x24
#define OCM_CE_FFD2 0x28
#define OCM_CE_FFD3 0x2c

/* Correctable Error First Failing ECC */
#define OCM_CE_FFE 0x1c
#define OCM_CE_FFE_SYNDROME(val) BSP_FLD32(val, 0, 15)
#define OCM_CE_FFE_SYNDROME_GET(reg) BSP_FLD32GET(reg, 0, 15)
#define OCM_CE_FFE_SYNDROME_SET(reg, val) BSP_FLD32SET(reg, val, 0, 15)

/* Uncorrectable Error First Failing Address */
#define OCM_UE_FFA 0x34
#define OCM_UE_FFA_ADDR(val) BSP_FLD32(val, 0, 17)
#define OCM_UE_FFA_ADDR_GET(reg) BSP_FLD32GET(reg, 0, 17)
#define OCM_UE_FFA_ADDR_SET(reg, val) BSP_FLD32SET(reg, val, 0, 17)

/* Uncorrectable Error First Failing Data, four registers comprising 16 bytes */
#define OCM_UE_FFD0 0x38
#define OCM_UE_FFD1 0x3c
#define OCM_UE_FFD2 0x40
#define OCM_UE_FFD3 0x44

/* Uncorrectable Error First Failing ECC */
#define OCM_UE_FFE 0x48
#define OCM_UE_FFE_SYNDROME(val) BSP_FLD32(val, 0, 15)
#define OCM_UE_FFE_SYNDROME_GET(reg) BSP_FLD32GET(reg, 0, 15)
#define OCM_UE_FFE_SYNDROME_SET(reg, val) BSP_FLD32SET(reg, val, 0, 15)

/* Read/Modify/Write Uncorrectable Error First Failing Address */
#define OCM_RMW_UE_FFA 0x70
#define OCM_RMW_UE_FFA_ADDR(val) BSP_FLD32(val, 0, 17)
#define OCM_RMW_UE_FFA_ADDR_GET(reg) BSP_FLD32GET(reg, 0, 17)
#define OCM_RMW_UE_FFA_ADDR_SET(reg, val) BSP_FLD32SET(reg, val, 0, 17)

static void ocm_handle_rmw( void )
{
  volatile uint32_t *rmw_ffa = (uint32_t*)(ocm_base + OCM_RMW_UE_FFA);
  OCM_Error_Info info;

  info.type = OCM_UNCORRECTABLE_RMW;
  info.offset = OCM_RMW_UE_FFA_ADDR_GET(*rmw_ffa);
  zynqmp_invoke_ecc_handler(OCM_RAM, &info);
}

static void ocm_handle_ce( void )
{
  volatile uint32_t *ce_ffa = (uint32_t*)(ocm_base + OCM_CE_FFA);
  volatile uint32_t *ce_ffe = (uint32_t*)(ocm_base + OCM_CE_FFA);
  volatile uint32_t *ce_ffd0 = (uint32_t*)(ocm_base + OCM_CE_FFD0);
  volatile uint32_t *ce_ffd1 = (uint32_t*)(ocm_base + OCM_CE_FFD1);
  volatile uint32_t *ce_ffd2 = (uint32_t*)(ocm_base + OCM_CE_FFD2);
  volatile uint32_t *ce_ffd3 = (uint32_t*)(ocm_base + OCM_CE_FFD3);
  OCM_Error_Info info;

  info.type = OCM_CORRECTABLE;
  info.offset = OCM_CE_FFA_ADDR_GET(*ce_ffa);
  info.data0 = *ce_ffd0;
  info.data1 = *ce_ffd1;
  info.data2 = *ce_ffd2;
  info.data3 = *ce_ffd3;
  info.syndrome = OCM_CE_FFE_SYNDROME_GET(*ce_ffe);
  zynqmp_invoke_ecc_handler(OCM_RAM, &info);
}

static void ocm_handle_ue( void )
{
  volatile uint32_t *ue_ffa = (uint32_t*)(ocm_base + OCM_UE_FFA);
  volatile uint32_t *ue_ffe = (uint32_t*)(ocm_base + OCM_UE_FFA);
  volatile uint32_t *ue_ffd0 = (uint32_t*)(ocm_base + OCM_UE_FFD0);
  volatile uint32_t *ue_ffd1 = (uint32_t*)(ocm_base + OCM_UE_FFD1);
  volatile uint32_t *ue_ffd2 = (uint32_t*)(ocm_base + OCM_UE_FFD2);
  volatile uint32_t *ue_ffd3 = (uint32_t*)(ocm_base + OCM_UE_FFD3);
  OCM_Error_Info info;

  info.type = OCM_UNCORRECTABLE;
  info.offset = OCM_UE_FFA_ADDR_GET(*ue_ffa);
  info.data0 = *ue_ffd0;
  info.data1 = *ue_ffd1;
  info.data2 = *ue_ffd2;
  info.data3 = *ue_ffd3;
  info.syndrome = OCM_UE_FFE_SYNDROME_GET(*ue_ffe);
  zynqmp_invoke_ecc_handler(OCM_RAM, &info);
}

static void ocm_handler(void *arg)
{
  volatile uint32_t *ocm_is = (uint32_t*)(ocm_base + OCM_IS);
  uint32_t ocm_is_value = *ocm_is;
  (void) arg;

  /* Check and clear each error type after handling */
  if ((ocm_is_value & OCM_IS_UE_RMW) != 0) {
    ocm_handle_rmw();
    *ocm_is = OCM_IS_UE_RMW;
  }

  if ((ocm_is_value & OCM_IS_CE) != 0) {
    ocm_handle_ce();
    *ocm_is = OCM_IS_CE;
  }

  if ((ocm_is_value & OCM_IS_UE) != 0) {
    ocm_handle_ue();
    *ocm_is = OCM_IS_UE;
  }
}

static rtems_interrupt_entry zynqmp_ocm_ecc_entry;

rtems_status_code zynqmp_configure_ocm_ecc( void )
{
  volatile uint32_t *err_ctrl = (uint32_t*)(ocm_base + OCM_ERR_CTRL);
  volatile uint32_t *ecc_ctrl = (uint32_t*)(ocm_base + OCM_ECC_CTRL);
  volatile uint32_t *int_enable = (uint32_t*)(ocm_base + OCM_IE);
  rtems_status_code sc;

  rtems_interrupt_entry_initialize(
    &zynqmp_ocm_ecc_entry,
    ocm_handler,
    NULL,
    "OCM RAM ECC"
  );

  sc = rtems_interrupt_entry_install(
    ZYNQMP_IRQ_OCM,
    RTEMS_INTERRUPT_SHARED,
    &zynqmp_ocm_ecc_entry
  );

  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  if ((*ecc_ctrl & OCM_ECC_CTRL_ECC_ON_OFF) == 0) {
    /*
     * ECC is not enabled and should already have been by BOOT.bin. Enabling it
     * now could corrupt existing data in the OCM.
     */
    return RTEMS_NOT_CONFIGURED;
  }

  /*
   * OCM_ERR_CTRL.UE_RES forces generation of a synchronous external abort
   * instead of using interrupts to signal the fault
   */
  *err_ctrl &= ~(OCM_ERR_CTRL_UE_RES);

  /* Ensure ECC_CTRL is in the right state */
  *ecc_ctrl &= ~(OCM_ECC_CTRL_DET_ONLY);

  /* enable correctable and uncorrectable error interrupts */
  *int_enable = OCM_IE_CE | OCM_IE_UE | OCM_IE_UE_RMW;

  return RTEMS_SUCCESSFUL;
}
