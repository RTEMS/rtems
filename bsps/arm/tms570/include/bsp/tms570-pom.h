/**
 * @file
 * @ingroup RTEMSBSPsARMTMS570
 * @brief Parameter Overlay Module (POM) header file
 */

/*
 * Copyright (c) 2014 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_TMS570_POM_H
#define LIBBSP_ARM_TMS570_POM_H

#include <stdint.h>
#include <bsp/tms570.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define TMS570_POM_REGIONS 32
#define TMS570_POM_GLBCTRL_ENABLE 0x000000a0a

/* Specification of memory size used for field REGSIZE of tms570_pom_region_t */
#define TMS570_POM_REGSIZE_DISABLED 0x0
#define TMS570_POM_REGSIZE_64B      0x1
#define TMS570_POM_REGSIZE_128B     0x2
#define TMS570_POM_REGSIZE_256B     0x3
#define TMS570_POM_REGSIZE_512B     0x4
#define TMS570_POM_REGSIZE_1KB      0x5
#define TMS570_POM_REGSIZE_2KB      0x6
#define TMS570_POM_REGSIZE_4KB      0x7
#define TMS570_POM_REGSIZE_8KB      0x8
#define TMS570_POM_REGSIZE_16KB     0x9
#define TMS570_POM_REGSIZE_32KB     0xa
#define TMS570_POM_REGSIZE_64KB     0xb
#define TMS570_POM_REGSIZE_128KB    0xc
#define TMS570_POM_REGSIZE_256KB    0xd

#define TMS570_POM_REGADDRMASK    ((1<<23)-1)

void tms570_initialize_and_clear(void);
void tms570_pom_remap(void);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_TMS570_POM_H */
