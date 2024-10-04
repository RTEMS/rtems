#ifndef LIBBSP_RISCV_SHARED_LINKER_SYMBOLS_H
#define LIBBSP_RISCV_SHARED_LINKER_SYMBOLS_H

/**
 * @file
 *
 * @ingroup RTEMSBSPsLinkerSymbolsRISCV
 *
 * @brief This header file provides interfaces to RISC-V specific linker
 *   symbols and sections.
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSBSPsLinkerSymbolsRISCV RISC-V Linker Symbols
 *
 * @ingroup RTEMSBSPsLinkerSymbols
 *
 * @brief This group provides support for RISC-V specific linker symbols and
 *   sections.
 *
 * @{
 */

#ifndef ASM
#define LINKER_SYMBOL(sym) extern char sym [];
#else
#define LINKER_SYMBOL(sym) .extern sym
#endif

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_RISCV_SHARED_LINKER_SYMBOLS_H */
