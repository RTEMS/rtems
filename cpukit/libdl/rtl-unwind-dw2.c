/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup rtems_rtld
 *
 * @brief RTEMS Run-Time Link Editor
 *
 * This is the RTL implementation.
 */

/*
 *  COPYRIGHT (c) 2012-2016, 2018 Chris Johns <chrisj@rtems.org>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdio.h>

#include <rtems/rtl/rtl.h>
#include "rtl-elf.h"
#include "rtl-error.h"
#include "rtl-unwind.h"
#include "rtl-unwind-dw2.h"

/*
 * These interfaces are not exported outside the GCC source.
 */
void __register_frame (void *begin);
void __deregister_frame (void *begin);

bool
rtems_rtl_elf_unwind_dw2_parse (const rtems_rtl_obj* obj,
                                const char*          name,
                                uint32_t             flags)
{
  return
    ((flags & RTEMS_RTL_OBJ_SECT_CONST) != 0) &&
    ((strcmp(name, ".eh_frame") == 0) ||
     (strncmp(name, ".gcc_except_table.", sizeof (".gcc_except_table.") - 1) == 0));
}

bool
rtems_rtl_elf_unwind_dw2_register (const rtems_rtl_obj* obj)
{
  rtems_rtl_obj_sect* sect = rtems_rtl_obj_find_section (obj, ".eh_frame");

  if (sect != NULL && sect->size > 0 && sect->base != NULL)
  {
    __register_frame (sect->base);
  }

  return true;
}

bool rtems_rtl_elf_unwind_dw2_deregister (const rtems_rtl_obj* obj)
{
  rtems_rtl_obj_sect* sect = rtems_rtl_obj_find_section (obj, ".eh_frame");

  if (sect != NULL && sect->size > 0 && sect->base != NULL)
  {
    __deregister_frame (sect->base);
  }

  return true;
}
