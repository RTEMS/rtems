/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup rtems_rtl
 *
 * @brief RTEMS Run-Time Linker Unwind Support.
 */

/*
 *  COPYRIGHT (c) 2016, 2018 Chris Johns <chrisj@rtems.org>
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
