/*
 * Taken from NetBSD and stripped of the relocations not needed on RTEMS.
 */

/*  $NetBSD: mdreloc.c,v 1.43 2010/01/13 20:17:22 christos Exp $  */

/*-
 * Copyright (c) 1999, 2002 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Paul Kranenburg and by Charles M. Hannum.
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
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>

#include <stdio.h>

#include <rtems/rtl/rtl.h>
#include "rtl-elf.h"
#include "rtl-error.h"
#include "rtl-trace.h"
#include "rtl-unwind.h"
#include "rtl-unwind-dw2.h"

/*
 * The following table holds for each relocation type:
 *  - the width in bits of the memory location the relocation
 *    applies to (not currently used)
 *  - the number of bits the relocation value must be shifted to the
 *    right (i.e. discard least significant bits) to fit into
 *    the appropriate field in the instruction word.
 *  - flags indicating whether
 *    * the relocation involves a symbol
 *    * the relocation is relative to the current position
 *    * the relocation is for a GOT entry
 *    * the relocation is relative to the load address
 *
 */
#define _RF_S     0x80000000           /* Resolve symbol */
#define _RF_A     0x40000000           /* Use addend */
#define _RF_P     0x20000000           /* Location relative */
#define _RF_G     0x10000000           /* GOT offset */
#define _RF_B     0x08000000           /* Load address relative */
#define _RF_U     0x04000000           /* Unaligned */
#define _RF_SZ(s) (((s) & 0xff) << 8)  /* memory target size */
#define _RF_RS(s) ( (s) & 0xff)        /* right shift */

static const uint32_t reloc_target_flags[] = {
  0,                                             /* NONE */
  _RF_S|_RF_A|        _RF_SZ(8)  | _RF_RS(0),    /* RELOC_8 */
  _RF_S|_RF_A|        _RF_SZ(16) | _RF_RS(0),    /* RELOC_16 */
  _RF_S|_RF_A|        _RF_SZ(32) | _RF_RS(0),    /* RELOC_32 */
  _RF_S|_RF_A|_RF_P|  _RF_SZ(8)  | _RF_RS(0),    /* DISP_8 */
  _RF_S|_RF_A|_RF_P|  _RF_SZ(16) | _RF_RS(0),    /* DISP_16 */
  _RF_S|_RF_A|_RF_P|  _RF_SZ(32) | _RF_RS(0),    /* DISP_32 */
  _RF_S|_RF_A|_RF_P|  _RF_SZ(32) | _RF_RS(2),    /* WDISP_30 */
  _RF_S|_RF_A|_RF_P|  _RF_SZ(32) | _RF_RS(2),    /* WDISP_22 */
  _RF_S|_RF_A|        _RF_SZ(32) | _RF_RS(10),   /* HI22 */
  _RF_S|_RF_A|        _RF_SZ(32) | _RF_RS(0),    /* 22 */
  _RF_S|_RF_A|        _RF_SZ(32) | _RF_RS(0),    /* 13 */
  _RF_S|_RF_A|        _RF_SZ(32) | _RF_RS(0),    /* LO10 */
  _RF_G|              _RF_SZ(32) | _RF_RS(0),    /* GOT10 */
  _RF_G|              _RF_SZ(32) | _RF_RS(0),    /* GOT13 */
  _RF_G|              _RF_SZ(32) | _RF_RS(10),   /* GOT22 */
  _RF_S|_RF_A|_RF_P|  _RF_SZ(32) | _RF_RS(0),    /* PC10 */
  _RF_S|_RF_A|_RF_P|  _RF_SZ(32) | _RF_RS(10),   /* PC22 */
        _RF_A|_RF_P|  _RF_SZ(32) | _RF_RS(2),    /* WPLT30 */
                      _RF_SZ(32) | _RF_RS(0),    /* COPY */
  _RF_S|_RF_A|        _RF_SZ(32) | _RF_RS(0),    /* GLOB_DAT */
                      _RF_SZ(32) | _RF_RS(0),    /* JMP_SLOT */
        _RF_A|  _RF_B|_RF_SZ(32) | _RF_RS(0),    /* RELATIVE */
  _RF_S|_RF_A|  _RF_U|_RF_SZ(32) | _RF_RS(0),    /* UA_32 */
};

#define NOT_CURRENTLY_USED_BUT_MAYBE_USEFUL
#ifdef NOT_CURRENTLY_USED_BUT_MAYBE_USEFUL
static const char *reloc_names[] = {
  "NONE", "RELOC_8", "RELOC_16", "RELOC_32", "DISP_8",
  "DISP_16", "DISP_32", "WDISP_30", "WDISP_22", "HI22",
  "22", "13", "LO10", "GOT10", "GOT13",
  "GOT22", "PC10", "PC22", "WPLT30", "COPY",
  "GLOB_DAT", "JMP_SLOT", "RELATIVE", "UA_32"
};
#endif

#define RELOC_RESOLVE_SYMBOL(t)    ((reloc_target_flags[t] & _RF_S) != 0)
#define RELOC_PC_RELATIVE(t)       ((reloc_target_flags[t] & _RF_P) != 0)
#define RELOC_BASE_RELATIVE(t)     ((reloc_target_flags[t] & _RF_B) != 0)
#define RELOC_UNALIGNED(t)         ((reloc_target_flags[t] & _RF_U) != 0)
#define RELOC_USE_ADDEND(t)        ((reloc_target_flags[t] & _RF_A) != 0)
#define RELOC_TARGET_SIZE(t)       ((reloc_target_flags[t] >> 8) & 0xff)
#define RELOC_VALUE_RIGHTSHIFT(t)  (reloc_target_flags[t] & 0xff)

static const int reloc_target_bitmask[] = {
#define _BM(x)  (~(-(1ULL << (x))))
  0,        /* NONE */
  _BM(8),  _BM(16), _BM(32),  /* RELOC_8, _16, _32 */
  _BM(8),  _BM(16), _BM(32),  /* DISP8, DISP16, DISP32 */
  _BM(30), _BM(22),           /* WDISP30, WDISP22 */
  _BM(22), _BM(22),           /* HI22, _22 */
  _BM(13), _BM(10),           /* RELOC_13, _LO10 */
  _BM(10), _BM(13), _BM(22),  /* GOT10, GOT13, GOT22 */
  _BM(10), _BM(22),           /* _PC10, _PC22 */
  _BM(30), 0,                 /* _WPLT30, _COPY */
  -1, -1, -1,                 /* _GLOB_DAT, JMP_SLOT, _RELATIVE */
  _BM(32)                     /* _UA32 */
#undef _BM
};
#define RELOC_VALUE_BITMASK(t)  (reloc_target_bitmask[t])

uint32_t
rtems_rtl_elf_section_flags (const rtems_rtl_obj_t* obj,
                             const Elf_Shdr*        shdr)
{
  return 0;
}

bool
rtems_rtl_elf_rel_resolve_sym (Elf_Word type)
{
  return RELOC_RESOLVE_SYMBOL (type) ? true : false;
}

bool
rtems_rtl_elf_relocate_rela (const rtems_rtl_obj_t*      obj,
                             const Elf_Rela*             rela,
                             const rtems_rtl_obj_sect_t* sect,
                             const char*                 symname,
                             const Elf_Byte              syminfo,
                             const Elf_Word              symvalue)
{
  Elf_Addr *where;
  Elf_Word type, value, mask;
  Elf_Addr tmp = 0;

  where = (Elf_Addr *) (sect->base + rela->r_offset);

  type = ELF_R_TYPE(rela->r_info);
  if (type == R_TYPE(NONE))
    return true;

  /* We do JMP_SLOTs in _rtld_bind() below */
  if (type == R_TYPE(JMP_SLOT))
    return true;

  /* COPY relocs are also handled elsewhere */
  if (type == R_TYPE(COPY))
    return true;

  /*
   * We use the fact that relocation types are an `enum'
   * Note: R_SPARC_6 is currently numerically largest.
   */
  if (type > R_TYPE(6))
    return false;

  value = rela->r_addend;

  /*
   * Handle relative relocs here, as an optimization.
   */
  if (type == R_TYPE (RELATIVE)) {
    *where += (Elf_Addr)(sect->base + value);
    if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
      printf ("rtl: reloc relative in %s --> %p",
              rtems_rtl_obj_oname (obj), (void *)*where);
    return true;
  }

  if (RELOC_RESOLVE_SYMBOL (type)) {
    /* Add in the symbol's absolute address */
    value += symvalue;
  }

  if (RELOC_PC_RELATIVE (type)) {
    value -= (Elf_Word)where;
  }

  if (RELOC_BASE_RELATIVE (type)) {
    /*
     * Note that even though sparcs use `Elf_rela'
     * exclusively we still need the implicit memory addend
     * in relocations referring to GOT entries.
     * Undoubtedly, someone f*cked this up in the distant
     * past, and now we're stuck with it in the name of
     * compatibility for all eternity..
     *
     * In any case, the implicit and explicit should be
     * mutually exclusive. We provide a check for that
     * here.
     */
#define DIAGNOSTIC
#ifdef DIAGNOSTIC
    if (value != 0 && *where != 0) {
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf("rtl: reloc base_rel(%s): where=%p, *where 0x%lx, "
               "addend=0x%lx, base %p\n",
               rtems_rtl_obj_oname (obj),
               where, *where, rela->r_addend, sect->base);
    }
#endif
    value += (Elf_Word)(sect->base + *where);
  }

  mask = RELOC_VALUE_BITMASK (type);
  value >>= RELOC_VALUE_RIGHTSHIFT (type);
  value &= mask;

  if (RELOC_UNALIGNED(type)) {
    /*
     * Handle unaligned relocations.
     */
    char *ptr = (char*) where;
    int i, size = RELOC_TARGET_SIZE (type) / 8;

    /* Read it in one byte at a time. */
    for (i = size - 1; i >= 0; i--)
      tmp = (tmp << 8) | ptr[i];

    tmp &= ~mask;
    tmp |= value;

    /* Write it back out. */
    for (i = size - 1; i >= 0; i--, tmp >>= 8)
      ptr[i] = tmp & 0xff;
  } else {
    *where &= ~mask;
    *where |= value;
    tmp = *where;
  }

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
    printf ("rtl: %s %p @ %p in %s\n",
            reloc_names[ELF_R_TYPE(rela->r_info)],
            (void *)tmp, where, rtems_rtl_obj_oname (obj));

  return true;
}

bool
rtems_rtl_elf_relocate_rel (const rtems_rtl_obj_t*      obj,
                            const Elf_Rel*              rel,
                            const rtems_rtl_obj_sect_t* sect,
                            const char*                 symname,
                            const Elf_Byte              syminfo,
                            const Elf_Word              symvalue)
{
  printf ("rtl: rel type record not supported; please report\n");
  return false;
}

bool
rtems_rtl_elf_unwind_parse (const rtems_rtl_obj_t* obj,
                            const char*            name,
                            uint32_t               flags)
{
  return rtems_rtl_elf_unwind_dw2_parse (obj, name, flags);
}

bool
rtems_rtl_elf_unwind_register (rtems_rtl_obj_t* obj)
{
  return rtems_rtl_elf_unwind_dw2_register (obj);
}

bool
rtems_rtl_elf_unwind_deregister (rtems_rtl_obj_t* obj)
{
  return rtems_rtl_elf_unwind_dw2_deregister (obj);
}
