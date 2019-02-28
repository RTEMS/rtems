/**
 *  @file
 *
 *  @ingroup arm_gic
 *
 *  @brief ARM GIC Register definitions
 */

/*
 * Copyright (c) 2013, 2019 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_ARM_SHARED_ARM_GIC_REGS_H
#define LIBBSP_ARM_SHARED_ARM_GIC_REGS_H

#include <bsp/utility.h>

typedef struct {
  uint32_t iccicr;
#define GIC_CPUIF_ICCICR_CBPR BSP_BIT32(4)
#define GIC_CPUIF_ICCICR_FIQ_EN BSP_BIT32(3)
#define GIC_CPUIF_ICCICR_ACK_CTL BSP_BIT32(2)
#define GIC_CPUIF_ICCICR_ENABLE_GRP_1 BSP_BIT32(1)
#define GIC_CPUIF_ICCICR_ENABLE BSP_BIT32(0)
  uint32_t iccpmr;
#define GIC_CPUIF_ICCPMR_PRIORITY(val) BSP_FLD32(val, 0, 7)
#define GIC_CPUIF_ICCPMR_PRIORITY_GET(reg) BSP_FLD32GET(reg, 0, 7)
#define GIC_CPUIF_ICCPMR_PRIORITY_SET(reg, val) BSP_FLD32SET(reg, val, 0, 7)
  uint32_t iccbpr;
#define GIC_CPUIF_ICCBPR_BINARY_POINT(val) BSP_FLD32(val, 0, 2)
#define GIC_CPUIF_ICCBPR_BINARY_POINT_GET(reg) BSP_FLD32GET(reg, 0, 2)
#define GIC_CPUIF_ICCBPR_BINARY_POINT_SET(reg, val) BSP_FLD32SET(reg, val, 0, 2)
  uint32_t icciar;
#define GIC_CPUIF_ICCIAR_CPUID(val) BSP_FLD32(val, 10, 12)
#define GIC_CPUIF_ICCIAR_CPUID_GET(reg) BSP_FLD32GET(reg, 10, 12)
#define GIC_CPUIF_ICCIAR_CPUID_SET(reg, val) BSP_FLD32SET(reg, val, 10, 12)
#define GIC_CPUIF_ICCIAR_ACKINTID(val) BSP_FLD32(val, 0, 9)
#define GIC_CPUIF_ICCIAR_ACKINTID_GET(reg) BSP_FLD32GET(reg, 0, 9)
#define GIC_CPUIF_ICCIAR_ACKINTID_SET(reg, val) BSP_FLD32SET(reg, val, 0, 9)
  uint32_t icceoir;
#define GIC_CPUIF_ICCEOIR_CPUID(val) BSP_FLD32(val, 10, 12)
#define GIC_CPUIF_ICCEOIR_CPUID_GET(reg) BSP_FLD32GET(reg, 10, 12)
#define GIC_CPUIF_ICCEOIR_CPUID_SET(reg, val) BSP_FLD32SET(reg, val, 10, 12)
#define GIC_CPUIF_ICCEOIR_EOIINTID(val) BSP_FLD32(val, 0, 9)
#define GIC_CPUIF_ICCEOIR_EOIINTID_GET(reg) BSP_FLD32GET(reg, 0, 9)
#define GIC_CPUIF_ICCEOIR_EOIINTID_SET(reg, val) BSP_FLD32SET(reg, val, 0, 9)
  uint32_t iccrpr;
#define GIC_CPUIF_ICCRPR_PRIORITY(val) BSP_FLD32(val, 0, 7)
#define GIC_CPUIF_ICCRPR_PRIORITY_GET(reg) BSP_FLD32GET(reg, 0, 7)
#define GIC_CPUIF_ICCRPR_PRIORITY_SET(reg, val) BSP_FLD32SET(reg, val, 0, 7)
  uint32_t icchpir;
#define GIC_CPUIF_ICCHPIR_CPUID(val) BSP_FLD32(val, 10, 12)
#define GIC_CPUIF_ICCHPIR_CPUID_GET(reg) BSP_FLD32GET(reg, 10, 12)
#define GIC_CPUIF_ICCHPIR_CPUID_SET(reg, val) BSP_FLD32SET(reg, val, 10, 12)
#define GIC_CPUIF_ICCHPIR_PENDINTID(val) BSP_FLD32(val, 0, 9)
#define GIC_CPUIF_ICCHPIR_PENDINTID_GET(reg) BSP_FLD32GET(reg, 0, 9)
#define GIC_CPUIF_ICCHPIR_PENDINTID_SET(reg, val) BSP_FLD32SET(reg, val, 0, 9)
  uint32_t iccabpr;
#define GIC_CPUIF_ICCABPR_BINARY_POINT(val) BSP_FLD32(val, 0, 2)
#define GIC_CPUIF_ICCABPR_BINARY_POINT_GET(reg) BSP_FLD32GET(reg, 0, 2)
#define GIC_CPUIF_ICCABPR_BINARY_POINT_SET(reg, val) BSP_FLD32SET(reg, val, 0, 2)
  uint32_t reserved_20[55];
  uint32_t icciidr;
#define GIC_CPUIF_ICCIIDR_PRODUCT_ID(val) BSP_FLD32(val, 24, 31)
#define GIC_CPUIF_ICCIIDR_PRODUCT_ID_GET(reg) BSP_FLD32GET(reg, 24, 31)
#define GIC_CPUIF_ICCIIDR_PRODUCT_ID_SET(reg, val) BSP_FLD32SET(reg, val, 24, 31)
#define GIC_CPUIF_ICCIIDR_ARCH_VERSION(val) BSP_FLD32(val, 16, 19)
#define GIC_CPUIF_ICCIIDR_ARCH_VERSION_GET(reg) BSP_FLD32GET(reg, 16, 19)
#define GIC_CPUIF_ICCIIDR_ARCH_VERSION_SET(reg, val) BSP_FLD32SET(reg, val, 16, 19)
#define GIC_CPUIF_ICCIIDR_REVISION(val) BSP_FLD32(val, 12, 15)
#define GIC_CPUIF_ICCIIDR_REVISION_GET(reg) BSP_FLD32GET(reg, 12, 15)
#define GIC_CPUIF_ICCIIDR_REVISION_SET(reg, val) BSP_FLD32SET(reg, val, 12, 15)
#define GIC_CPUIF_ICCIIDR_IMPLEMENTER(val) BSP_FLD32(val, 0, 11)
#define GIC_CPUIF_ICCIIDR_IMPLEMENTER_GET(reg) BSP_FLD32GET(reg, 0, 11)
#define GIC_CPUIF_ICCIIDR_IMPLEMENTER_SET(reg, val) BSP_FLD32SET(reg, val, 0, 11)
} gic_cpuif;

typedef struct {
  uint32_t icddcr;
#define GIC_DIST_ICDDCR_ENABLE_GRP_1 BSP_BIT32(1)
#define GIC_DIST_ICDDCR_ENABLE BSP_BIT32(0)
  uint32_t icdictr;
#define GIC_DIST_ICDICTR_LSPI(val) BSP_FLD32(val, 11, 15)
#define GIC_DIST_ICDICTR_LSPI_GET(reg) BSP_FLD32GET(reg, 11, 15)
#define GIC_DIST_ICDICTR_LSPI_SET(reg, val) BSP_FLD32SET(reg, val, 11, 15)
#define GIC_DIST_ICDICTR_SECURITY_EXTN BSP_BIT32(10)
#define GIC_DIST_ICDICTR_CPU_NUMBER(val) BSP_FLD32(val, 5, 7)
#define GIC_DIST_ICDICTR_CPU_NUMBER_GET(reg) BSP_FLD32GET(reg, 5, 7)
#define GIC_DIST_ICDICTR_CPU_NUMBER_SET(reg, val) BSP_FLD32SET(reg, val, 5, 7)
#define GIC_DIST_ICDICTR_IT_LINES_NUMBER(val) BSP_FLD32(val, 0, 4)
#define GIC_DIST_ICDICTR_IT_LINES_NUMBER_GET(reg) BSP_FLD32GET(reg, 0, 4)
#define GIC_DIST_ICDICTR_IT_LINES_NUMBER_SET(reg, val) BSP_FLD32SET(reg, val, 0, 4)
  uint32_t icdiidr;
#define GIC_DIST_ICDIIDR_PRODUCT_ID(val) BSP_FLD32(val, 24, 31)
#define GIC_DIST_ICDIIDR_PRODUCT_ID_GET(reg) BSP_FLD32GET(reg, 24, 31)
#define GIC_DIST_ICDIIDR_PRODUCT_ID_SET(reg, val) BSP_FLD32SET(reg, val, 24, 31)
#define GIC_DIST_ICDIIDR_VARIANT(val) BSP_FLD32(val, 16, 19)
#define GIC_DIST_ICDIIDR_VARIANT_GET(reg) BSP_FLD32GET(reg, 16, 19)
#define GIC_DIST_ICDIIDR_VARIANT_SET(reg, val) BSP_FLD32SET(reg, val, 16, 19)
#define GIC_DIST_ICDIIDR_REVISION(val) BSP_FLD32(val, 12, 15)
#define GIC_DIST_ICDIIDR_REVISION_GET(reg) BSP_FLD32GET(reg, 12, 15)
#define GIC_DIST_ICDIIDR_REVISION_SET(reg, val) BSP_FLD32SET(reg, val, 12, 15)
#define GIC_DIST_ICDIIDR_IMPLEMENTER(val) BSP_FLD32(val, 0, 11)
#define GIC_DIST_ICDIIDR_IMPLEMENTER_GET(reg) BSP_FLD32GET(reg, 0, 11)
#define GIC_DIST_ICDIIDR_IMPLEMENTER_SET(reg, val) BSP_FLD32SET(reg, val, 0, 11)
  uint32_t reserved_0c[29];
  uint32_t icdigr[32];
  uint32_t icdiser[32];
  uint32_t icdicer[32];
  uint32_t icdispr[32];
  uint32_t icdicpr[32];
  uint32_t icdabr[32];
  uint32_t reserved_380[32];
  uint8_t icdipr[256];
  uint32_t reserved_500[192];
  uint8_t icdiptr[256];
  uint32_t reserved_900[192];
  uint32_t icdicfr[64];
  uint32_t reserved_d00[128];
  uint32_t icdsgir;
#define GIC_DIST_ICDSGIR_TARGET_LIST_FILTER(val) BSP_FLD32(val, 24, 25)
#define GIC_DIST_ICDSGIR_TARGET_LIST_FILTER_GET(reg) BSP_FLD32GET(reg, 24, 25)
#define GIC_DIST_ICDSGIR_TARGET_LIST_FILTER_SET(reg, val) BSP_FLD32SET(reg, val, 24, 25)
#define GIC_DIST_ICDSGIR_CPU_TARGET_LIST(val) BSP_FLD32(val, 16, 23)
#define GIC_DIST_ICDSGIR_CPU_TARGET_LIST_GET(reg) BSP_FLD32GET(reg, 16, 23)
#define GIC_DIST_ICDSGIR_CPU_TARGET_LIST_SET(reg, val) BSP_FLD32SET(reg, val, 16, 23)
#define GIC_DIST_ICDSGIR_NSATT BSP_BIT32(15)
#define GIC_DIST_ICDSGIR_SGIINTID(val) BSP_FLD32(val, 0, 3)
#define GIC_DIST_ICDSGIR_SGIINTID_GET(reg) BSP_FLD32GET(reg, 0, 3)
#define GIC_DIST_ICDSGIR_SGIINTID_SET(reg, val) BSP_FLD32SET(reg, val, 0, 3)
} gic_dist;

#endif /* LIBBSP_ARM_SHARED_ARM_GIC_REGS_H */
