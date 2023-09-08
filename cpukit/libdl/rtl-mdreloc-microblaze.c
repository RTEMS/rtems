/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2023 On-Line Applications Research Corporation (OAR)
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

#include <sys/cdefs.h>

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <rtems/rtl/rtl.h>
#include "rtl-elf.h"
#include "rtl-error.h"
#include <rtems/rtl/rtl-trace.h>
#include "rtl-unwind.h"
#include "rtl-unwind-dw2.h"

uint32_t
rtems_rtl_elf_section_flags (const rtems_rtl_obj* obj,
                             const Elf_Shdr*      shdr) {
  (void) obj;
  (void) shdr;
  return 0;
}

uint32_t
rtems_rtl_elf_arch_parse_section (const rtems_rtl_obj* obj,
                                  int                  section,
                                  const char*          name,
                                  const Elf_Shdr*      shdr,
                                  const uint32_t       flags) {
  (void) obj;
  (void) section;
  (void) name;
  (void) shdr;
  return flags;
}

bool
rtems_rtl_elf_arch_section_alloc (const rtems_rtl_obj* obj,
                                  rtems_rtl_obj_sect*  sect) {
  (void) obj;
  (void) sect;
  return false;
}

bool
rtems_rtl_elf_arch_section_free (const rtems_rtl_obj* obj,
                                 rtems_rtl_obj_sect*  sect) {
  (void) obj;
  (void) sect;
  return false;
}

bool
rtems_rtl_elf_rel_resolve_sym (Elf_Word type) {
  return type != 0;
}

uint32_t rtems_rtl_obj_tramp_alignment (const rtems_rtl_obj* obj)
{
  (void) obj;
  return sizeof(uint32_t);
}

size_t
rtems_rtl_elf_relocate_tramp_max_size (void) {
  /*
   * Disable by returning 0.
   */
  return 0;
}

rtems_rtl_elf_rel_status
rtems_rtl_elf_relocate_rel_tramp (rtems_rtl_obj*            obj,
                                  const Elf_Rel*            rel,
                                  const rtems_rtl_obj_sect* sect,
                                  const char*               symname,
                                  const Elf_Byte            syminfo,
                                  const Elf_Word            symvalue) {
  (void) obj;
  (void) rel;
  (void) sect;
  (void) symname;
  (void) syminfo;
  (void) symvalue;
  return rtems_rtl_elf_rel_no_error;
}

static void write16le(void *loc, uint16_t val) {
  *((uint16_t *) loc) = val;
}

static void write32le(void *loc, uint32_t val) {
  *((uint32_t *) loc) = val;
}

static uint16_t read16le(void *loc) {
  return *((uint16_t *) loc);
}

static uint32_t read32le(void *loc) {
  return *((uint32_t *) loc);
}

static rtems_rtl_elf_rel_status
rtems_rtl_elf_reloc_rela (rtems_rtl_obj*            obj,
                          const Elf_Rela*           rela,
                          const rtems_rtl_obj_sect* sect,
                          const char*               symname,
                          const Elf_Byte            syminfo,
                          const Elf_Word            symvalue,
                          const bool                parsing) {
  Elf_Word *where;
  Elf_Word addend = (Elf_Word)rela->r_addend;
  Elf_Addr target;

  where = (Elf_Addr *)(sect->base + rela->r_offset);

  /* Target value */
  target = (Elf_Addr)symvalue + addend;
  /* Final PCREL value */
  Elf_Word pcrel_val = target - ((Elf_Word)where);

  if (parsing) {
    return rtems_rtl_elf_rel_no_error;
  }

  switch (ELF_R_TYPE(rela->r_info)) {
  case R_TYPE(NONE):
    break;

  case R_TYPE(64):
    /* Set the lower 16 bits of where to the upper 16 bits of target */
    write16le(where,
              (read16le(where) & 0xFFFF0000) | (target >> 16));
    /* Set the lower 16 bits of where + 4 to the lower 16 bits of target */
    write16le(where + 1,
              (read16le(where + 1) & 0xFFFF0000) | (target & 0xFFFF));
    break;

  case R_TYPE(32):
    {
      uintptr_t addr = (uintptr_t)where;
      if ((uintptr_t)where & 3) {
        /* `where` is not aligned to a 4-byte boundary. */
        uintptr_t addr_down = addr & ~3;
        uintptr_t addr_up = (addr + 3) & ~3;

        uint32_t value_down = read32le((void*)addr_down);
        uint32_t value_up = read32le((void*)addr_up);

        /*
         * Calculate how many bytes `where` is offset from the previous 4-byte
         * boundary.
         */
        unsigned offset = addr & 3;

        /*
         * Combine `target` with `value_down` and `value_up` to form the new
         * values to write.
         */
        uint32_t new_value_down = (value_down & ((1 << (offset * 8)) - 1)) |
          (target << (offset * 8));
        uint32_t new_value_up = (value_up & ~((1 << (offset * 8)) - 1)) |
          (target >> ((4 - offset) * 8));

        write32le((void*)addr_down, new_value_down);
        write32le((void*)addr_up, new_value_up);
      } else {
        write32le(where, target);
      }
    }
    break;

  case R_TYPE(32_PCREL):
    write32le(where, pcrel_val);

    if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
      printf ("rtl: R_MICROBLAZE_32_PCREL %p @ %p in %s\n",
              (void *) * (where), where, rtems_rtl_obj_oname (obj));
    break;

  case R_TYPE(64_PCREL):
    /*
     * Compensate for the fact that the PC is 4 bytes ahead of the instruction
     */
    pcrel_val -= 4;
    /* Set the lower 16 bits of where to the upper 16 bits of target */
    write16le(where,
              (read16le(where) & 0xFFFF0000) | (pcrel_val >> 16));
    /* Set the lower 16 bits of where + 4 to the lower 16 bits of target */
    write16le(where + 1,
              (read16le(where + 1) & 0xFFFF0000) | (pcrel_val & 0xFFFF));

    if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
      printf ("rtl: R_MICROBLAZE_64_PCREL %p @ %p in %s\n",
              (void *) * (where), where, rtems_rtl_obj_oname (obj));
    break;

  case R_TYPE(32_PCREL_LO):
    write16le(where, (read16le(where) & 0xFFFF0000) | (pcrel_val & 0xFFFF));
    break;

  default:
    rtems_rtl_set_error (EINVAL,
                         "%s: Unsupported relocation type %d "
                         "in non-PLT relocations",
                         sect->name, (uint32_t) ELF_R_TYPE(rela->r_info));
    return rtems_rtl_elf_rel_failure;
  }

  return rtems_rtl_elf_rel_no_error;
}

rtems_rtl_elf_rel_status
rtems_rtl_elf_relocate_rela (rtems_rtl_obj*            obj,
                             const Elf_Rela*           rela,
                             const rtems_rtl_obj_sect* sect,
                             const char*               symname,
                             const Elf_Byte            syminfo,
                             const Elf_Word            symvalue) {
  return rtems_rtl_elf_reloc_rela (obj,
                                   rela,
                                   sect,
                                   symname,
                                   syminfo,
                                   symvalue,
                                   false);
}

rtems_rtl_elf_rel_status
rtems_rtl_elf_relocate_rela_tramp (rtems_rtl_obj*            obj,
                                   const Elf_Rela*           rela,
                                   const rtems_rtl_obj_sect* sect,
                                   const char*               symname,
                                   const Elf_Byte            syminfo,
                                   const Elf_Word            symvalue) {
  return rtems_rtl_elf_reloc_rela (obj,
                                   rela,
                                   sect,
                                   symname,
                                   syminfo,
                                   symvalue,
                                   true);
}

rtems_rtl_elf_rel_status
rtems_rtl_elf_relocate_rel (rtems_rtl_obj*            obj,
                            const Elf_Rel*            rel,
                            const rtems_rtl_obj_sect* sect,
                            const char*               symname,
                            const Elf_Byte            syminfo,
                            const Elf_Word            symvalue) {
  rtems_rtl_set_error (EINVAL, "rel type record not supported");
  return rtems_rtl_elf_rel_failure;
}

bool
rtems_rtl_elf_unwind_parse (const rtems_rtl_obj* obj,
                            const char*          name,
                            uint32_t             flags) {
  return rtems_rtl_elf_unwind_dw2_parse (obj, name, flags);
}

bool
rtems_rtl_elf_unwind_register (rtems_rtl_obj* obj) {
  return rtems_rtl_elf_unwind_dw2_register (obj);
}

bool
rtems_rtl_elf_unwind_deregister (rtems_rtl_obj* obj) {
  return rtems_rtl_elf_unwind_dw2_deregister (obj);
}
