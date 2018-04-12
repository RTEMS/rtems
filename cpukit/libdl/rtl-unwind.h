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
 * @brief RTEMS Run-Time Linker Unwind Support.
 */

#if !defined (_RTEMS_RTL_UNWIND_H_)
#define _RTEMS_RTL_UNWIND_H_

#include "rtl-elf.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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
bool rtems_rtl_elf_unwind_parse (const rtems_rtl_obj* obj,
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
bool rtems_rtl_elf_unwind_register (rtems_rtl_obj* obj);

/**
 * Architecture specific handler to remove an object file's unwind information
 * from the base image.
 *
 * @param obj The object file.
 * @retval true The unwind information has been deregistered.
 * @retval false The unwind information could not be deregistered.
 */
bool rtems_rtl_elf_unwind_deregister (rtems_rtl_obj* obj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
