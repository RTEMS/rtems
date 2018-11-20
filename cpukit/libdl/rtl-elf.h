/*
 *  COPYRIGHT (c) 2012-2018 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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

#include <rtems/rtl/rtl-fwd.h>
#include <rtems/rtl/rtl-obj-fwd.h>
#include <rtems/rtl/rtl-sym.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 ** Imported NetBSD ELF Specifics Start.
 **/

/*
 * Do not add '()'. Leave plain.
 */
#if defined(__powerpc64__) || defined(__arch64__)
#define ELFSIZE 64
#else
#define ELFSIZE 32
#endif

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
 * Architecture specific handler to translate unknown section flags to RTL
 * section flags.
 *
 * @param obj The object file being relocated.
 * @param shdr The ELF section header.
 * @retval 0 Unknown or unsupported flags.
 * @retval uint32_t RTL object file flags.
 */
uint32_t rtems_rtl_elf_section_flags (const rtems_rtl_obj* obj,
                                      const Elf_Shdr*      shdr);

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
bool rtems_rtl_elf_relocate_rel (const rtems_rtl_obj*      obj,
                                 const Elf_Rel*            rel,
                                 const rtems_rtl_obj_sect* sect,
                                 const char*               symname,
                                 const Elf_Byte            syminfo,
                                 const Elf_Word            symvalue);

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
bool rtems_rtl_elf_relocate_rela (const rtems_rtl_obj*      obj,
                                  const Elf_Rela*           rela,
                                  const rtems_rtl_obj_sect* sect,
                                  const char*               symname,
                                  const Elf_Byte            syminfo,
                                  const Elf_Word            symvalue);

/**
 * The ELF format check handler.
 *
 * @param obj The object being checked.
 * @param fd The file descriptor.
 */
bool rtems_rtl_elf_file_check (rtems_rtl_obj* obj, int fd);

/**
 * The ELF format load handler.
 *
 * @param obj The object to load.
 * @param fd The file descriptor.
 */
bool rtems_rtl_elf_file_load (rtems_rtl_obj* obj, int fd);

/**
 * The ELF format unload handler.
 *
 * @param obj The object to unload.
 */
bool rtems_rtl_elf_file_unload (rtems_rtl_obj* obj);

/**
 * The ELF format signature handler.
 *
 * @return rtems_rtl_loader_format* The format's signature.
 */
rtems_rtl_loader_format* rtems_rtl_elf_file_sig (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
