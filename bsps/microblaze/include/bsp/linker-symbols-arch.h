/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsLinkerSymbolsMicroBlaze
 *
 * @brief This header file provides interfaces to MicroBlaze-specific linker
 *   symbols and sections.
 */

/*
 * Copyright (C) 2015 Hesham Almatary
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

#ifndef LIBBSP_MICROBLAZE_SHARED_LINKER_SYMBOLS_H
#define LIBBSP_MICROBLAZE_SHARED_LINKER_SYMBOLS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSBSPsLinkerSymbolsMicroBlaze MicroBlaze Linker Symbols
 *
 * @ingroup RTEMSBSPsLinkerSymbols
 *
 * @brief This group provides support for MicroBlaze-specific linker symbols
 *   and sections.
 *
 * @{
 */

#ifndef ASM
  #define LINKER_SYMBOL(sym) extern char sym [];
#else
  #define LINKER_SYMBOL(sym) .extern sym
#endif

LINKER_SYMBOL(bsp_section_vector_begin)
LINKER_SYMBOL(bsp_section_vector_end)
LINKER_SYMBOL(bsp_section_vector_size)

LINKER_SYMBOL(bsp_vector_table_begin)
LINKER_SYMBOL(bsp_vector_table_end)
LINKER_SYMBOL(bsp_vector_table_size)

LINKER_SYMBOL(bsp_start_vector_table_begin)
LINKER_SYMBOL(bsp_start_vector_table_end)
LINKER_SYMBOL(bsp_start_vector_table_size)

LINKER_SYMBOL(bsp_translation_table_base)
LINKER_SYMBOL(bsp_translation_table_end)

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_MICROBLAZE_SHARED_LINKER_SYMBOLS_H */
