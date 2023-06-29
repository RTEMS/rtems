/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2023 Reflex Aerospace GmbH
 *
 * Written by Philip Kirkpatrick <p.kirkpatrick@reflexaerospace.com>
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

#include <xil_mpu.h>
#include <xil_cache.h>
#include <xreg_cortexr5.h>

static BSP_START_TEXT_SECTION void zynqmp_configure_mpu_sections(void);

/*
 * Make weak and let the user override.
 */
BSP_START_TEXT_SECTION void zynqmp_setup_mpu_and_cache(void) __attribute__ ((weak));

BSP_START_TEXT_SECTION void zynqmp_setup_mpu_and_cache(void)
{
  zynqmp_configure_mpu_sections();
  Xil_EnableMPU();
  Xil_DCacheEnable();
  Xil_ICacheEnable();
}

/*
 * Setup MPU sections.
 *
 * The MPU on the ZynqMP RPU only supports 16 regions.
 * Regions must align on boundaries equal to the size of the region
 * Regions may overlap or be nested with the later definition taking precedence
 *
 * Note: LWIP for Zynq requires an available region in xemacpsif_dma.c:init_dma()
 *       this is used for the BD memory.
 *
 * The following code attempts to implement the section map from Init_MPU() in
 * https://github.com/Xilinx/embeddedsw/blob/master/lib/bsp/standalone/src/arm/cortexr5/platform/ZynqMP/mpu.c
 * and from ARMV7_CP15_START_DEFAULT_SECTIONS in bsps/arm/include/bsp/arm-cp15-start.h
 * Due to the limitation on number of regions, some compromises have been made.
 *  - Merges device memories instead of configuring each one independently
 *  - For DRAM, assumes a baseline of `Normal write-back Cacheable` `Full Access`
 *    then uses precedence to set no-cache and RO sections
 *
 * Reference:
 * https://docs.xilinx.com/r/en-US/ug1085-zynq-ultrascale-trm/System-Address-Map-Interconnects
 * https://developer.arm.com/documentation/ddi0460/c/Memory-Protection-Unit
 *
 *|                 | Memory Range            | Attributes of MPURegion     |
 *|-----------------|-------------------------|-----------------------------|
 *| DDR             | 0x00000000 - 0x7FFFFFFF | Normal write-back Cacheable |
 *|  -rodata        |                         |  + PRIV_RO_USER_RO          |
 *|  -nocache       |                         | Normal non-cacheable        |
 *|  -nocachenoload |                         | Normal non-cacheable        |
 *| PL              | 0x80000000 - 0xBFFFFFFF | Strongly Ordered            |
 *| Devices         | 0xC0000000 - 0xFFFFFFFF | Device Memory               |
 *| -QSPI           | 0xC0000000 - 0xDFFFFFFF |                             |
 *| -PCIe           | 0xE0000000 - 0xEFFFFFFF |                             |
 *| -Reserved       | 0xF0000000 - 0xF7FFFFFF |                             |
 *| -STM_CORESIGHT  | 0xF8000000 - 0xF8FFFFFF |                             |
 *| -RPU_R5_GIC     | 0xF9000000 - 0xF90FFFFF |                             |
 *| -Reserved       | 0xF9100000 - 0xFCFFFFFF |                             |
 *| -FPS            | 0xFD000000 - 0xFDFFFFFF |                             |
 *| -LPS            | 0xFE000000 - 0xFFFFFFFF | (1)                         |
 *| OCM             | 0xFFFC0000 - 0xFFFFFFFF | Normal write-back Cacheable |
 *
 * Note 1: Actual range for LPS goes to 0xFFBFFFFF,  to use less sections go to
 *         0xFFFFFFFF and use precedence to configure OCM
 */

static BSP_START_TEXT_SECTION void zynqmp_configure_mpu_sections(void)
{
  u32 addr;
  u64 size;
  u32 attrib;

  // Configure baseline DDR memory 0x00000000 - 0x7FFFFFFF
  addr = 0x00000000U;
  size = 0x80000000U;
  attrib = NORM_NSHARED_WB_WA | PRIV_RW_USER_RW;
  Xil_SetMPURegion(addr, size, attrib);

  // Configure PL interfaces 0x80000000 - 0xBFFFFFFF
  addr = 0x80000000U;
  size = 0x40000000U;
  attrib = STRONG_ORDERD_SHARED | PRIV_RW_USER_RW;
  Xil_SetMPURegion(addr, size, attrib);

  // Configure devices 0xC0000000 - 0xFFFFFFFF
  addr = 0xC0000000U;
  size = 0x40000000U;
  attrib = DEVICE_NONSHARED | PRIV_RW_USER_RW;
  Xil_SetMPURegion(addr, size, attrib);

  // Configure OCM 0xFFFC0000 - 0xFFFFFFFF
  addr = 0xFFFC0000U;
  size = 0x00040000U;
  attrib = NORM_NSHARED_WB_WA | PRIV_RW_USER_RW;
  Xil_SetMPURegion(addr, size, attrib);

  // Add RO region for RO section
  addr = (u32) bsp_section_rodata_begin;
  size = bsp_section_rodata_end - bsp_section_rodata_begin;
  attrib = NORM_NSHARED_WB_WA | PRIV_RO_USER_RO;
  Xil_SetMPURegion(addr, size, attrib);

  // Add no cache region for no cache section
  addr = (u32) bsp_section_nocache_begin;
  size = bsp_section_nocache_end - bsp_section_nocache_begin;
  attrib = NORM_SHARED_NCACHE | PRIV_RW_USER_RW;
  Xil_SetMPURegion(addr, size, attrib);

  // Add no cache region for no cache no load section
  addr = (u32) bsp_section_nocachenoload_begin;
  size = bsp_section_nocachenoload_end - bsp_section_nocachenoload_begin;
  attrib = NORM_SHARED_NCACHE | PRIV_RW_USER_RW;
  Xil_SetMPURegion(addr, size, attrib);
}
