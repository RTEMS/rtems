/*
 * Copyright (c) 2015, 2017 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_SHARED_FDT_H
#define LIBBSP_SHARED_FDT_H

#include <bsp.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * BSPs that implement the FDT support functions must define
 * BSP_FDT_IS_SUPPORTED.
 */

/**
 * @brief Copies the specified source FDT to a dedicated global data area.
 *
 * The source FDT is usually provided by a bootloader and may be located in a
 * memory area that is used by the program.  The low-level initialization
 * should copy the FDT for later use.
 *
 * The copy can be accessed by bsp_fdt_get().
 *
 * @param[in] src The source FDT.
 */
void bsp_fdt_copy(const void *src);

/**
 * @brief Returns the FDT of the BSP.
 *
 * @return The FDT of the BSP.
 */
const void *bsp_fdt_get(void);

/**
 * @brief Maps the interrupt number of the FDT to the interrupt vector used by
 * the interrupt management.
 *
 * This function is used by the libbsd to implement the OFW_BUS_MAP_INTR bus
 * method.
 *
 * @param[in] intr The FDT interrupt number.
 *
 * @return The interrupt vector of the FDT interrupt number.
 */
uint32_t bsp_fdt_map_intr(uint32_t intr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_SHARED_FDT_H */
