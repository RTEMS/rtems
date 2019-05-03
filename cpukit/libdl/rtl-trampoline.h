/*
 *  COPYRIGHT (c) 2019 Chris Johns <chrisj@rtems.org>
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
 * @brief RTEMS Run-Time Linker Object File Trampoline Relocations Table.
 *
 * Cache relocation records that could result in a trampoline. The unresolved
 * symbol table holds variable the symbol name (length string) and the object
 * file's relocation records that reference the unresolved symbol. The
 * trampoline cache is an extension to this table to reuse the code and memory
 * and support trampolines.
 *
 * Some architectures require trampolines or veneers to extend the range of
 * some instructions. The compiler generates small optimized instructions
 * assuming most destinations are within the range of the instruction. The
 * instructions are smaller in size and can have a number of encodings with
 * different ranges. If a relocation record points to a symbol that is out of
 * range for the instruction a trampoline is used to extend the instruction's
 * range. A trampoline is a small fragment of architecture specific
 * instructions located within the range of the relocation record instruction
 * that can reach the entire address range. The trampoline's execution is
 * transparent to the execution of the object file.
 *
 * An object file that needs a trampoline has a table allocated close to the
 * text section. It has to be close to ensure the largest possible object file
 * can be spported. The number of slots in a table depends on:
 *
 *  # Location of the code
 *  # The type of relocation records in the object file
 *  # The instruction encoding the relocation record points too
 *  # The landing address of the instruction
 *
 * The allocation of the text segment and the trampoline table have to happen
 * with the allocator lock being locked and held to make sure no other
 * allocations happen inbetween the text section allocation and the trampoline
 * table. Holding an allocator lock limits what the link editor can do when
 * when the default heap allocator is being used. If calls any operating
 * system services including the file system use the same allocator a deadlock
 * will occur. This creates a conflict between performing the allocations
 * together and reading the instructions while holding the allocator lock.
 *
 * The trampoline cache holds the parsed relocation records that could result
 * in a trampoline. These records can be exaimined after the allocation of the
 * text segment to determine how many relocation record target's are out of
 * range. The minimum range for a specific type of relocation record has to be
 * used as the instructions cannot be loaded.
 */

#if !defined (_RTEMS_RTL_TRAMPOLINE_H_)
#define _RTEMS_RTL_TRAMPOLINE_H_

#include <rtems/rtl/rtl-unresolved.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Add a relocation to the list of trampolinr relocations.
 *
 * @param obj The object table the relocation record is for.
 * @param flags Format specific flags.
 * @param sect The target section number the relocation references.
 * @param symvalue The symbol's value.
 * @param rel The format specific relocation data.
 * @retval true The relocation has been added.
 * @retval false The relocation could not be added.
 */
bool rtems_rtl_trampoline_add (rtems_rtl_obj*        obj,
                               const uint16_t        flags,
                               const uint16_t        sect,
                               const rtems_rtl_word  symvalue,
                               const rtems_rtl_word* rel);

/**
 * Remove the relocation records for an object file.
 *
 * @param obj The object table the symbols are for.
 */
void rtems_rtl_trampoline_remove (rtems_rtl_obj* obj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
