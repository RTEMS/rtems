/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup bsp_linker
 *
 * @brief Symbols defined in linker command base file.
 */

/*
 * Copyright (C) 2008, 2013 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_M68K_SHARED_LINKER_SYMBOLS_H
#define LIBBSP_M68K_SHARED_LINKER_SYMBOLS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSBSPsM68kSharedLinker Linker Support
 *
 * @ingroup RTEMSBSPsM68kShared
 *
 * @brief Linker support.
 *
 * @{
 */

#ifndef ASM
  #define LINKER_SYMBOL(sym) extern char sym [];
#else
  #define LINKER_SYMBOL(sym) .extern sym
#endif

LINKER_SYMBOL(bsp_vector0_begin)
LINKER_SYMBOL(bsp_vector0_end)
LINKER_SYMBOL(bsp_vector0_size)

LINKER_SYMBOL(bsp_vector1_begin)
LINKER_SYMBOL(bsp_vector1_end)
LINKER_SYMBOL(bsp_vector1_size)

LINKER_SYMBOL(bsp_section_text_begin)
LINKER_SYMBOL(bsp_section_text_end)
LINKER_SYMBOL(bsp_section_text_size)
LINKER_SYMBOL(bsp_section_text_load_begin)
LINKER_SYMBOL(bsp_section_text_load_end)

LINKER_SYMBOL(bsp_section_data_begin)
LINKER_SYMBOL(bsp_section_data_end)
LINKER_SYMBOL(bsp_section_data_size)
LINKER_SYMBOL(bsp_section_data_load_begin)
LINKER_SYMBOL(bsp_section_data_load_end)

LINKER_SYMBOL(bsp_section_bss_begin)
LINKER_SYMBOL(bsp_section_bss_end)
LINKER_SYMBOL(bsp_section_bss_size)

LINKER_SYMBOL(bsp_section_work_begin)
LINKER_SYMBOL(bsp_section_work_end)
LINKER_SYMBOL(bsp_section_work_size)

LINKER_SYMBOL(bsp_initstack_begin)
LINKER_SYMBOL(bsp_initstack_end)
LINKER_SYMBOL(bsp_initstack_size)

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_M68K_SHARED_LINKER_SYMBOLS_H */
