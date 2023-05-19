/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2015, 2017 embedded brains GmbH & Co. KG
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
 * @param[in] intr The FDT interrupt number cells.
 * @param[in] icells The FDT interrupt cell count.
 *
 * @return The interrupt vector of the FDT interrupt number.
 */
uint32_t bsp_fdt_map_intr(const uint32_t *intr, size_t icells);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_SHARED_FDT_H */
