/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsLinkerSymbolsArm
 *
 * @brief This header file provides interfaces to Arm-specific linker symbols
 *   and sections.
 */

/*
 * Copyright (C) 2008, 2016 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_ARM_SHARED_LINKER_SYMBOLS_H
#define LIBBSP_ARM_SHARED_LINKER_SYMBOLS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSBSPsLinkerSymbolsArm Arm Linker Symbols
 *
 * @ingroup RTEMSBSPsLinkerSymbols
 *
 * @brief This group provides support for Arm-specific linker symbols and
 *   sections.
 *
 * @{
 */

#ifndef ASM
  #define LINKER_SYMBOL(sym) extern char sym [];
#else
  #define LINKER_SYMBOL(sym) .extern sym
#endif

LINKER_SYMBOL(bsp_stack_fiq_size)
LINKER_SYMBOL(bsp_stack_abt_size)
LINKER_SYMBOL(bsp_stack_und_size)
LINKER_SYMBOL(bsp_stack_hyp_size)

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

#endif /* LIBBSP_ARM_SHARED_LINKER_SYMBOLS_H */
