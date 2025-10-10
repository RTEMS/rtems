/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @ingroup
 *
 *  @brief
 */

/*
 * Copyright (C) 2014 Chris Johns <chrisj@rtems.org>.
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
#include <inttypes.h>
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
                             const Elf_Shdr*      shdr)
{
  (void) obj;
  (void) shdr;

  return 0;
}

uint32_t
rtems_rtl_elf_arch_parse_section (const rtems_rtl_obj* obj,
                                  int                  section,
                                  const char*          name,
                                  const Elf_Shdr*      shdr,
                                  const uint32_t       flags)
{
  (void) obj;
  (void) section;
  (void) name;
  (void) shdr;
  return flags;
}

bool
rtems_rtl_elf_arch_section_alloc (const rtems_rtl_obj* obj,
                                  rtems_rtl_obj_sect*  sect)
{
  (void) obj;
  (void) sect;
  return false;
}

bool
rtems_rtl_elf_arch_section_free (const rtems_rtl_obj* obj,
                                  rtems_rtl_obj_sect*  sect)
{
  (void) obj;
  (void) sect;
  return false;
}

bool
rtems_rtl_elf_rel_resolve_sym (Elf_Word type)
{
  (void) type;

  return true;
}

uint32_t rtems_rtl_obj_tramp_alignment (const rtems_rtl_obj* obj)
{
  (void) obj;
  return sizeof(uint32_t);
}

size_t
rtems_rtl_elf_relocate_tramp_max_size (void)
{
  /*
   * Disable by returning 0.
   */
  return 0;
}

rtems_rtl_elf_rel_status
rtems_rtl_elf_relocate_rela_tramp (rtems_rtl_obj*            obj,
                                   const Elf_Rela*           rela,
                                   const rtems_rtl_obj_sect* sect,
                                   const char*               symname,
                                   const Elf_Byte            syminfo,
                                   const Elf_Word            symvalue)
{
  (void) obj;
  (void) rela;
  (void) sect;
  (void) symname;
  (void) syminfo;
  (void) symvalue;
  rtems_rtl_set_error (EINVAL, "rela type record not supported");
  return rtems_rtl_elf_rel_failure;
}

rtems_rtl_elf_rel_status
rtems_rtl_elf_relocate_rela (rtems_rtl_obj*            obj,
                             const Elf_Rela*           rela,
                             const rtems_rtl_obj_sect* sect,
                             const char*               symname,
                             const Elf_Byte            syminfo,
                             const Elf_Word            symvalue)
{
  (void) obj;
  (void) rela;
  (void) sect;
  (void) symname;
  (void) syminfo;
  (void) symvalue;
  rtems_rtl_set_error (EINVAL, "rela type record not supported");
  return rtems_rtl_elf_rel_failure;
}

rtems_rtl_elf_rel_status
rtems_rtl_elf_relocate_rel_tramp (rtems_rtl_obj*            obj,
                                  const Elf_Rel*            rel,
                                  const rtems_rtl_obj_sect* sect,
                                  const char*               symname,
                                  const Elf_Byte            syminfo,
                                  const Elf_Word            symvalue)
{
  (void) obj;
  (void) rel;
  (void) sect;
  (void) symname;
  (void) syminfo;
  (void) symvalue;
  return rtems_rtl_elf_rel_no_error;
}

#define RTEMS_RTL_MIPS_HI16_MAX (128)

static struct {
  Elf_Addr* where_hi16;
  Elf_Addr  ahl;
} mips_hi16_list[RTEMS_RTL_MIPS_HI16_MAX];
static size_t mips_hi16_list_cnt;

/*
 * 1. _gp_disp symbol are not considered in this file.
 * 2. There is a local/external column;
 * local corresponds to (STB_LOCAL & STT_SECTION) and
 * all others are external. Because if the type of a
 * symbol is STT_SECTION, it must be STB_LOCAL. Thus
 * just consider symtype here.
 */
rtems_rtl_elf_rel_status
rtems_rtl_elf_relocate_rel (rtems_rtl_obj*            obj,
                            const Elf_Rel*            rel,
                            const rtems_rtl_obj_sect* sect,
                            const char*               symname,
                            const Elf_Byte            syminfo,
                            const Elf_Word            symvalue)
{
  (void) symname;

  Elf_Addr *where;
  Elf_Word tmp;
  Elf_Word addend = (Elf_Word)0;
  Elf_Word local = 0;
  Elf_Addr *where_hi16;
  Elf_Addr ahl;
  uint32_t t;

  where = (Elf_Addr *)(sect->base + rel->r_offset);
  addend = *where;

  if (syminfo == STT_SECTION)
    local = 1;

  switch (ELF_R_TYPE(rel->r_info)) {
    case R_TYPE(NONE):
      break;

    case R_TYPE(16):
      tmp = addend & 0xffff;
      if ((tmp & 0x8000) == 0x8000)
        tmp |= 0xffff0000; /* Sign extend */
      tmp = symvalue + (int)tmp;
      if ((tmp & 0xffff0000) != 0) {
        printf("R_MIPS_16 Overflow\n");
        return rtems_rtl_elf_rel_failure;
      }

      *where = (tmp & 0xffff) | (*where & 0xffff0000);

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: R_MIPS_16 %p @ %p in %s\n",
                (void *)*(where), where, rtems_rtl_obj_oname (obj));
      break;

    case R_TYPE(32):
      tmp = symvalue + addend;
      if (addend != tmp)
        *where = tmp;

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: R_MIPS_32 %p @ %p in %s\n",
                (void *)*(where), where, rtems_rtl_obj_oname (obj));
      break;

    case R_TYPE(26):

        addend &= 0x03ffffff;
        addend <<= 2;

      if (local == 1) { /* STB_LOCAL and STT_SECTION */
        tmp = symvalue + (((Elf_Addr)where & 0xf0000000) | addend);
        tmp >>= 2;

      } else { /* external */

        tmp = addend;

        if ((tmp & 0x08000000) == 0x08000000)
          tmp |= 0xf0000000; /* Sign extened */
        tmp = ((int)tmp + symvalue) >> 2;

      }

      *where &= ~0x03ffffff;
      *where |= tmp & 0x03ffffff;

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: R_MIPS_26 local=%" PRIu32 " @ %p in %s\n",
                local, (void *)*(where), rtems_rtl_obj_oname (obj));
      break;

    case R_TYPE(HI16):
      if (mips_hi16_list_cnt >= RTEMS_RTL_MIPS_HI16_MAX) {
        rtems_rtl_set_error (ENOMEM,
                             "%s: too many MIPS_HI16 relocs", sect->name);
        return false;
      }
      mips_hi16_list[mips_hi16_list_cnt].where_hi16 = where;
      mips_hi16_list[mips_hi16_list_cnt].ahl = addend << 16;
      ++mips_hi16_list_cnt;

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: R_MIPS_HI16 %p @ %p in %s\n",
                (void *)*(where), where, rtems_rtl_obj_oname (obj));
      break;

    case R_TYPE(LO16):
      t = (int16_t) addend;
      tmp = symvalue;
      if (tmp == 0) {
        rtems_rtl_set_error (EINVAL,
                             "%s: symvalue is 0 in MIPS_LO16", sect->name);
        return rtems_rtl_elf_rel_failure;
      }

      /* reloc low part */
      addend &= 0xffff0000;
      addend |= (uint16_t)(t + tmp);
      *where = addend;

      if (rtems_rtl_trace(RTEMS_RTL_TRACE_RELOC))
        printf("*where %x where %p\n", *where, where);

      /* reloc hi parts */
      while (mips_hi16_list_cnt != 0) {
        --mips_hi16_list_cnt;
        ahl = mips_hi16_list[mips_hi16_list_cnt].ahl;
        where_hi16 = mips_hi16_list[mips_hi16_list_cnt].where_hi16;
        addend = *(where_hi16);
        addend &= 0xffff0000;
        addend |= ((ahl + t + tmp) - (int16_t) (ahl + t + tmp)) >> 16;
        *(where_hi16) = addend;
        if (rtems_rtl_trace(RTEMS_RTL_TRACE_RELOC))
          printf("*where_hi %x where_hi %p ahl=%08x\n",
                 *(where_hi16), where_hi16, ahl);
      }

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: R_MIPS_LO16 %p @ %p in %s\n",
                (void *)*(where), where, rtems_rtl_obj_oname (obj));
      break;

    case R_TYPE(PC16):
      tmp = addend & 0xffff;
      if ((tmp & 0x8000) == 0x8000)
        tmp |= 0xffff0000; /* Sign extend */
      tmp = symvalue + ((int)tmp*4) - (Elf_Addr)where;
      tmp = (Elf_Sword)tmp >> 2;
      if (((Elf_Sword)tmp > 0x7fff) || ((Elf_Sword)tmp < -0x8000)) {
        printf("R_MIPS_PC16 Overflow\n");
        return rtems_rtl_elf_rel_failure;
      }

      *where = (tmp & 0xffff) | (*where & 0xffff0000);

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: R_MIPS_PC16 %p @ %p in %s\n",
                (void *)*(where), where, rtems_rtl_obj_oname (obj));

      break;

    default:
      printf ("rtl: reloc unknown: sym = %" PRIu32 ", type = %" PRIu32 ", offset = %p, "
              "contents = %p\n",
              ELF_R_SYM(rel->r_info), (uint32_t) ELF_R_TYPE(rel->r_info),
              (void *)rel->r_offset, (void *)*where);
      rtems_rtl_set_error (EINVAL,
                           "%s: Unsupported relocation type %" PRIu32
                           "in non-PLT relocations",
                           sect->name, (uint32_t) ELF_R_TYPE(rel->r_info));
      return rtems_rtl_elf_rel_failure;
  }

  return rtems_rtl_elf_rel_no_error;
}

bool
rtems_rtl_elf_unwind_parse (const rtems_rtl_obj* obj,
                            const char*          name,
                            uint32_t             flags)
{
  return rtems_rtl_elf_unwind_dw2_parse (obj, name, flags);
}

bool
rtems_rtl_elf_unwind_register (rtems_rtl_obj* obj)
{
  return rtems_rtl_elf_unwind_dw2_register (obj);
}

bool
rtems_rtl_elf_unwind_deregister (rtems_rtl_obj* obj)
{
  return rtems_rtl_elf_unwind_dw2_deregister (obj);
}
