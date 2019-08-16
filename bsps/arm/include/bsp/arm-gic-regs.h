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
  /* GICD_CTLR */
  uint32_t icddcr;
/* GICv3 only */
#define GIC_DIST_ICDDCR_RWP BSP_BIT32(31)
#define GIC_DIST_ICDDCR_E1NWF BSP_BIT32(7)
#define GIC_DIST_ICDDCR_DS BSP_BIT32(6)
#define GIC_DIST_ICDDCR_ARE_NS BSP_BIT32(5)
#define GIC_DIST_ICDDCR_ARE_S BSP_BIT32(4)
#define GIC_DIST_ICDDCR_ENABLE_GRP1S BSP_BIT32(2)
#define GIC_DIST_ICDDCR_ENABLE_GRP1NS BSP_BIT32(1)
#define GIC_DIST_ICDDCR_ENABLE_GRP0 BSP_BIT32(0)
/* GICv1/GICv2 */
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
  /* GICD_IGRPMODR GICv3 only, reserved in GICv1/GICv2 */
  uint32_t icdigmr[32];
  uint32_t reserved_d80[96];
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

/* GICv3 only */
typedef struct {
  /* GICR_CTLR */
  uint32_t icrrcr;
#define GIC_REDIST_ICRRCR_UWP BSP_BIT32(31)
#define GIC_REDIST_ICRRCR_DPG1S BSP_BIT32(26)
#define GIC_REDIST_ICRRCR_DPG1NS BSP_BIT32(25)
#define GIC_REDIST_ICRRCR_DPG0 BSP_BIT32(24)
#define GIC_REDIST_ICRRCR_RWP BSP_BIT32(4)
#define GIC_REDIST_ICRRCR_ENABLE_LPI BSP_BIT32(0)
  uint32_t icriidr;
  uint64_t icrtyper;
#define GIC_REDIST_ICRTYPER_AFFINITY_VALUE(val) BSP_FLD64(val, 32, 63)
#define GIC_REDIST_ICRTYPER_AFFINITY_VALUE_GET(reg) BSP_FLD64GET(reg, 32, 63)
#define GIC_REDIST_ICRTYPER_AFFINITY_VALUE_SET(reg, val) BSP_FLD64SET(reg, val, 32, 63)
#define GIC_REDIST_ICRTYPER_COMMON_LPI_AFFINITY(val) BSP_FLD64(val, 24, 25)
#define GIC_REDIST_ICRTYPER_COMMON_LPI_AFFINITY_GET(reg) BSP_FLD64GET(reg, 24, 25)
#define GIC_REDIST_ICRTYPER_COMMON_LPI_AFFINITY_SET(reg, val) BSP_FLD64SET(reg, val, 24, 25)
#define GIC_REDIST_ICRTYPER_CPU_NUMBER(val) BSP_FLD64(val, 8, 23)
#define GIC_REDIST_ICRTYPER_CPU_NUMBER_GET(reg) BSP_FLD64GET(reg, 8, 23)
#define GIC_REDIST_ICRTYPER_CPU_NUMBER_SET(reg, val) BSP_FLD64SET(reg, val, 8, 23)
#define GIC_REDIST_ICRTYPER_DPGS BSP_BIT64(5)
#define GIC_REDIST_ICRTYPER_LAST BSP_BIT64(4)
#define GIC_REDIST_ICRTYPER_DIRECT_LPI BSP_BIT64(3)
#define GIC_REDIST_ICRTYPER_VLPIS BSP_BIT64(1)
#define GIC_REDIST_ICRTYPER_PLPIS BSP_BIT64(0)
  uint32_t unused_10;
  uint32_t icrwaker;
#define GIC_REDIST_ICRWAKER_CHILDREN_ASLEEP BSP_BIT32(2)
#define GIC_REDIST_ICRWAKER_PROCESSOR_SLEEP BSP_BIT32(1)
} gic_redist;

/* GICv3 only */
typedef struct {
  uint32_t reserved_0_80[32];
  /* GICR_IGROUPR0 */
  uint32_t icspigrpr[32];
  /* GICR_ISENABLER0 */
  uint32_t icspiser[32];
  /* GICR_ICENABLER0 */
  uint32_t icspicer[32];
  /* GICR_ISPENDR0 */
  uint32_t icspispendr[32];
  /* GICR_ICPENDR0 */
  uint32_t icspicpendr[32];
  /* GICR_ISACTIVER0 */
  uint32_t icspisar[32];
  /* GICR_ICACTIVER0 */
  uint32_t icspicar[32];
  /* GICR_IPRIORITYR */
  uint8_t icspiprior[32];
  uint32_t reserved_420_bfc[504];
  /* GICR_ICFGR0 */
  uint32_t icspicfgr0;
  /* GICR_ICFGR1 */
  uint32_t icspicfgr1;
  uint32_t reserved_c08_cfc[62];
  /* GICR_IGRPMODR0 */
  uint32_t icspigrpmodr[64];
} gic_sgi_ppi;

#endif /* LIBBSP_ARM_SHARED_ARM_GIC_REGS_H */
