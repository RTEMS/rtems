/*
 *  COPYRIGHT (c) 2012 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems_rtl
 *
 * @brief RTEMS Run-Time Linker ELF Headers
 */

#if !defined (_RTEMS_RTL_ELF_H_)
#define _RTEMS_RTL_ELF_H_

#include "rtl-fwd.h"
#include "rtl-obj-fwd.h"
#include "rtl-sym.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 ** Imported NetBSD ELF Specifics Start.
 **/

/*
 * Always 32bit for RTEMS at the moment. Do not add '()'. Leave plain.
 */
#define ELFSIZE 32

/*
 * Define _STANDALONE then remove after.
 */
#define _STANDALONE 1

#include <sys/cdefs.h>
#include <sys/exec_elf.h>

#undef _STANDALONE

/**
 ** Imported NetBSD ELF Specifics End.
 **/

/**
 * Maximum string length. This a read buffering limit rather than a
 * specific ELF length. I hope this is ok as I am concerned about
 * some C++ symbol lengths.
 */
#define RTEMS_RTL_ELF_STRING_MAX (256)

/**
 * Architecture specific handler to check is a relocation record's type is
 * required to resolve a symbol.
 *
 * @param type The type field in the relocation record.
 * @retval true The relocation record require symbol resolution.
 * @retval false The relocation record does not require symbol resolution.
 */
bool rtems_rtl_elf_rel_resolve_sym (Elf_Word type);

/**
 * Architecture specific relocation handler compiled in for a specific
 * architecture by the build system. The handler applies the relocation
 * to the target.
 *
 * @param obj The object file being relocated.
 * @param rel The ELF relocation record.
 * @param sect The section of the object file the relocation is for.
 * @param symname The symbol's name.
 * @param syminfo The ELF symbol info field.
 * @param symvalue If a symbol is referenced, this is the symbols value.
 * @retval bool The relocation has been applied.
 * @retval bool The relocation could not be applied.
 */
bool rtems_rtl_elf_relocate_rel (const rtems_rtl_obj_t*      obj,
                                 const Elf_Rel*              rel,
                                 const rtems_rtl_obj_sect_t* sect,
                                 const char*                 symname,
                                 const Elf_Byte              syminfo,
                                 const Elf_Word              symvalue);

/**
 * Architecture specific relocation handler compiled in for a specific
 * architecture by the build system. The handler applies the relocation
 * to the target.
 *
 * @param obj The object file being relocated.
 * @param rela The ELF addend relocation record.
 * @param sect The section of the object file the relocation is for.
 * @param symname The symbol's name.
 * @param syminfo The ELF symbol info field.
 * @param symvalue If a symbol is referenced, this is the symbols value.
 * @retval bool The relocation has been applied.
 * @retval bool The relocation could not be applied.
 */
bool rtems_rtl_elf_relocate_rela (const rtems_rtl_obj_t*      obj,
                                  const Elf_Rela*             rela,
                                  const rtems_rtl_obj_sect_t* sect,
                                  const char*                 symname,
                                  const Elf_Byte              syminfo,
                                  const Elf_Word              symvalue);

/**
 * Find the symbol. The symbol is passed as an ELF type symbol with the name
 * and the value returned is the absolute address of the symbol.
 *
 * If the symbol type is STT_NOTYPE the symbol references a global symbol. The
 * gobal symbol table is searched to find it and that value returned. If the
 * symbol is local to the object module the section for the symbol is located
 * and it's base added to the symbol's value giving an absolute location.
 *
 * @param obj The object the symbol is being resolved for.
 * @param sym The ELF type symbol.
 * @param symname The sym's name read from the symbol string table.
 * @param value Return the value of the symbol. Only valid if the return value
 *              is true.
 * @retval true The symbol resolved.
 * @retval false The symbol could not be result. The RTL error is set.
 */
bool rtems_rtl_elf_find_symbol (rtems_rtl_obj_t* obj,
                                const Elf_Sym*   sym,
                                const char*      symname,
                                Elf_Word*        value);

/**
 * The ELF format check handler.
 *
 * @param obj The object being checked.
 * @param fd The file descriptor.
 */
bool rtems_rtl_elf_file_check (rtems_rtl_obj_t* obj, int fd);

/**
 * The ELF file details handler.
 *
 * @param obj Load the details of the obj.
 */
bool rtems_rtl_elf_load_details (rtems_rtl_obj_t* obj);

/**
 * The ELF format load handler.
 *
 * @param obj The object to load.
 * @param fd The file descriptor.
 */
bool rtems_rtl_elf_file_load (rtems_rtl_obj_t* obj, int fd);

/**
 * The ELF format signature handler.
 *
 * @return rtems_rtl_loader_format_t* The format's signature.
 */
rtems_rtl_loader_format_t* rtems_rtl_elf_file_sig (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
