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
#if defined(__powerpc64__) || defined(__arch64__) || (__riscv_xlen == 64)
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
 * ELF Relocation status codes.
 */
typedef enum rtems_rtl_elf_rel_status
{
  rtems_rtl_elf_rel_no_error,    /**< There is no error processing the record. */
  rtems_rtl_elf_rel_failure,     /**< There was a failure processing the record. */
  rtems_rtl_elf_rel_tramp_cache, /**< The reloc record may need a trampoliine. */
  rtems_rtl_elf_rel_tramp_add    /**< Add a trampoliine. */
} rtems_rtl_elf_rel_status;

/**
 * Relocation trampoline relocation data.
 */
typedef struct rtems_rtl_mdreloc_trmap
{
  bool   parsing;     /**< The reloc records are being parsed. */
  void*  tampolines;  /**< The trampoline memory. */
  size_t size;        /**< The trampoline size. */
} rtems_rtl_mdreloc_tramp;

/**
 * Maximum string length. This a read buffering limit rather than a
 * specific ELF length. I hope this is ok as I am concerned about
 * some C++ symbol lengths.
 */
#define RTEMS_RTL_ELF_STRING_MAX (256)

/**
 * Architecture specific handler to translate unknown section flags to RTL
 * section flags. If this function returns 0 an error is raised.
 *
 * @param obj The object file being relocated.
 * @param shdr The ELF section header.
 * @retval 0 Unknown or unsupported flags.
 * @retval uint32_t RTL object file flags.
 */
uint32_t rtems_rtl_elf_section_flags (const rtems_rtl_obj* obj,
                                      const Elf_Shdr*      shdr);

/**
 * Architecture specific handler to parse the section and add any flags that
 * may be need to handle the section.
 *
 * @param obj The object file being relocated.
 * @param seciton The section index.
 * @param name The name of the section
 * @param shdr The ELF section header.
 * @param flags The standard ELF parsed flags.
 * @retval uint32_t Extra RTL object file flags.
 */
uint32_t rtems_rtl_elf_arch_parse_section (const rtems_rtl_obj* obj,
                                           int                  section,
                                           const char*          name,
                                           const Elf_Shdr*      shdr,
                                           const uint32_t       flags);

/**
 * Architecture specific handler to allocate a section. Some sections are
 * specific to an architecture and need special allocators.
 *
 * @param obj The object file being relocated.
 * @param sect The section data.
 * @retval true The allocator was successful.
 */
bool rtems_rtl_elf_arch_section_alloc (const rtems_rtl_obj* obj,
                                       rtems_rtl_obj_sect* sect);

/**
 * Architecture specific handler to free a section. Some sections are
 * specific to an architecture and need special allocators.
 *
 * @param obj The object file being relocated.
 * @param sect The section data.
 * @retval true The allocator was successful.
 */
bool rtems_rtl_elf_arch_section_free (const rtems_rtl_obj* obj,
                                      rtems_rtl_obj_sect* sect);

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
 * Architecture specific relocation maximum trampoline size. A trampoline entry
 * of this size is allocated for each unresolved external.
 *
 * @return size_t The maximum size of a trampoline for this architecture.
 */
size_t rtems_rtl_elf_relocate_tramp_max_size (void);

/**
 * Architecture specific relocation trampoline handler compiled in for a
 * specific architecture by the build system. The handler determines if the
 * relocation record requires a trampoline.
 *
 * @param obj The object file being relocated.
 * @param rela The ELF relocation record.
 * @param sect The section of the object file the relocation is for.
 * @param symname The symbol's name.
 * @param syminfo The ELF symbol info field.
 * @param symvalue If a symbol is referenced, this is the symbols value.
 * @retval rtems_rtl_elf_rel_status The result of the trampoline parsing.
 */
rtems_rtl_elf_rel_status rtems_rtl_elf_relocate_rel_tramp (rtems_rtl_obj*            obj,
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
 * @retval rtems_rtl_elf_rel_status The result of the trampoline parsing.
 */
rtems_rtl_elf_rel_status  rtems_rtl_elf_relocate_rela_tramp (rtems_rtl_obj*            obj,
                                                             const Elf_Rela*           rela,
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
 * @param rel The ELF relocation record.
 * @param sect The section of the object file the relocation is for.
 * @param symname The symbol's name.
 * @param syminfo The ELF symbol info field.
 * @param symvalue If a symbol is referenced, this is the symbols value.
 * @retval rtems_rtl_elf_rel_status The result of the trampoline parsing.
 */
rtems_rtl_elf_rel_status rtems_rtl_elf_relocate_rel (rtems_rtl_obj*            obj,
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
 * @retval rtems_rtl_elf_rel_status The result of the trampoline parsing.
 */
rtems_rtl_elf_rel_status rtems_rtl_elf_relocate_rela (rtems_rtl_obj*            obj,
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
