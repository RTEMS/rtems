/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsLinkerSymbolsPowerPC
 *
 * @brief This header file provides interfaces to PowerPC-specific linker
 *   symbols and sections.
 */

/*
 * Copyright (C) 2010, 2016 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_POWERPC_SHARED_LINKER_SYMBOLS_H
#define LIBBSP_POWERPC_SHARED_LINKER_SYMBOLS_H

#include <rtems/score/basedefs.h>

#include <libcpu/powerpc-utility.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSBSPsLinkerSymbolsPowerPC PowerPC Linker Symbols and Sections
 *
 * @ingroup RTEMSBSPsLinkerSymbols
 *
 * @brief This group provides support for PowerPC-specific linker symbols and
 *   sections.
 *
 * @{
 */

LINKER_SYMBOL(bsp_section_sdata_begin)
LINKER_SYMBOL(bsp_section_sdata_end)
LINKER_SYMBOL(bsp_section_sdata_size)

LINKER_SYMBOL(bsp_section_sbss_begin)
LINKER_SYMBOL(bsp_section_sbss_end)
LINKER_SYMBOL(bsp_section_sbss_size)

LINKER_SYMBOL(bsp_section_sdata_libdl_begin)
LINKER_SYMBOL(bsp_section_sdata_libdl_end)
LINKER_SYMBOL(bsp_section_sdata_libdl_size)

LINKER_SYMBOL(bsp_section_nvram_begin)
LINKER_SYMBOL(bsp_section_nvram_end)
LINKER_SYMBOL(bsp_section_nvram_size)

#define BSP_NVRAM_SECTION RTEMS_SECTION(".bsp_nvram")

#define BSP_NVRAM_SUBSECTION(subsection) \
  RTEMS_SECTION(".bsp_nvram." # subsection)

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_POWERPC_SHARED_LINKER_SYMBOLS_H */
