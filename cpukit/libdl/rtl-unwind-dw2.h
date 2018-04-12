/*
 *  COPYRIGHT (c) 2016, 2018 Chris Johns <chrisj@rtems.org>
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
 * @brief RTEMS Run-Time Linker Unwind DWARF  Support.
 */

#if !defined (_RTEMS_RTL_UNWIND_DW2_H_)
#define _RTEMS_RTL_UNWIND_DW2_H_

#include "rtl-elf.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if __SIZEOF_LONG__ >= __SIZEOF_POINTER__
  typedef long rtems_rtl_elf_unwind_dw2_sleb128;
  typedef unsigned long rtems_rtl_elf_unwind_dw2_uleb128;
#elif __SIZEOF_LONG_LONG__ >= __SIZEOF_POINTER__
  typedef long long rtems_rtl_elf_unwind_dw2_sleb128;
  typedef unsigned long long rtems_rtl_elf_unwind_dw2_uleb128;
#else
  #error No DW2 type available.
#endif

/**
 * Architecture specific handler to check if a section contains exception
 * handler data..
 *
 * @param obj The object file.
 * @param name The section's name.
 * @param uint32 flags The object file's flags.
 * @retval true The section contains unwind information.
 * @retval false The section does not contain unwind information.
 */
bool rtems_rtl_elf_unwind_dw2_parse (const rtems_rtl_obj* obj,
				     const char*          name,
				     uint32_t             flags);

/**
 * Architecture specific handler to add an object file's unwind information to
 * the base image.
 *
 * @param obj The object file.
 * @retval true The unwind information has been registered.
 * @retval false The unwind information could not be registered.
 */
bool rtems_rtl_elf_unwind_dw2_register (const rtems_rtl_obj* obj);

/**
 * Architecture specific handler to remove an object file's unwind information
 * from the base image.
 *
 * @param obj The object file.
 * @retval true The unwind information has been deregistered.
 * @retval false The unwind information could not be deregistered.
 */
bool rtems_rtl_elf_unwind_dw2_deregister (const rtems_rtl_obj* obj);

/**
 * Read signed and unsigned LEB128 values.
 */
const uint8_t* rtems_rtl_elf_unwind_dw2_read_uleb128 (const uint8_t*                    data,
						      rtems_rtl_elf_unwind_dw2_uleb128* val);
const uint8_t* rtems_rtl_elf_unwind_dw2_read_sleb128 (const uint8_t*                    data,
						      rtems_rtl_elf_unwind_dw2_sleb128* val);

bool rtems_rtl_elf_unwind_dw2_relocate (const Elf_Addr* where, Elf_Word value, Elf_Word mask);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
