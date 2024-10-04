#ifndef LIBBSP_OR1k_SHARED_LINKER_SYMBOLS_H
#define LIBBSP_OR1k_SHARED_LINKER_SYMBOLS_H

/**
 * @file
 *
 * @ingroup RTEMSBSPsLinkerSymbolsOr1k
 *
 * @brief This header file provides interfaces to OpenRISC 1000 specific linker
 *   symbols and sections.
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSBSPsLinkerSymbolsOr1k OpenRISC 1000 Linker Symbols
 *
 * @ingroup RTEMSBSPsLinkerSymbols
 *
 * @brief This group provides support for OpenRISC 1000 specific linker symbols
 *   and sections.
 *
 * @{
 */

#ifndef ASM
  #define LINKER_SYMBOL(sym) extern char sym [];
#else
  #define LINKER_SYMBOL(sym) .extern sym
#endif

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

#endif /* LIBBSP_OR1K_SHARED_LINKER_SYMBOLS_H */
