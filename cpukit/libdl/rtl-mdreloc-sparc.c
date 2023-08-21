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

#include <inttypes.h>
#include <stdio.h>

#include <rtems/rtl/rtl.h>
#include "rtl-elf.h"
#include "rtl-error.h"
#include <rtems/rtl/rtl-trace.h>
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
  _RF_A|_RF_P|        _RF_SZ(32) | _RF_RS(2),    /* WPLT30 */
  _RF_SZ(32) |                     _RF_RS(0),    /* COPY */
  _RF_S|_RF_A|        _RF_SZ(32) | _RF_RS(0),    /* GLOB_DAT */
  _RF_SZ(32) |                     _RF_RS(0),    /* JMP_SLOT */
  _RF_A|  _RF_B|      _RF_SZ(32) | _RF_RS(0),    /* RELATIVE */
  _RF_S|_RF_A|_RF_U|  _RF_SZ(32) | _RF_RS(0),    /* UA_32 */

  /* TLS and 64 bit relocs not listed here... */
};
#define RELOC_TARGET_FLAGS_SIZE \
  (sizeof(reloc_target_flags) / sizeof(reloc_target_flags[0]))

#define RTLD_DEBUG_RELOC
#ifdef RTLD_DEBUG_RELOC
static const char *reloc_names[] = {
  "NONE", "RELOC_8", "RELOC_16", "RELOC_32", "DISP_8",
  "DISP_16", "DISP_32", "WDISP_30", "WDISP_22", "HI22",
  "22", "13", "LO10", "GOT10", "GOT13",
  "GOT22", "PC10", "PC22", "WPLT30", "COPY",
  "GLOB_DAT", "JMP_SLOT", "RELATIVE", "UA_32",

  /* not used with 32bit userland, besides a few of the TLS ones */
  "PLT32",
  "HIPLT22", "LOPLT10", "LOPLT10", "PCPLT22", "PCPLT32",
  "10", "11", "64", "OLO10", "HH22",
  "HM10", "LM22", "PC_HH22", "PC_HM10", "PC_LM22",
  "WDISP16", "WDISP19", "GLOB_JMP", "7", "5", "6",
  "DISP64", "PLT64", "HIX22", "LOX10", "H44", "M44",
  "L44", "REGISTER", "UA64", "UA16",
  "TLS_GD_HI22", "TLS_GD_LO10", "TLS_GD_ADD", "TLS_GD_CALL",
  "TLS_LDM_HI22", "TLS_LDM_LO10", "TLS_LDM_ADD", "TLS_LDM_CALL",
  "TLS_LDO_HIX22", "TLS_LDO_LOX10", "TLS_LDO_ADD", "TLS_IE_HI22",
  "TLS_IE_LO10", "TLS_IE_LD", "TLS_IE_LDX", "TLS_IE_ADD", "TLS_LE_HIX22",
  "TLS_LE_LOX10", "TLS_DTPMOD32", "TLS_DTPMOD64", "TLS_DTPOFF32",
  "TLS_DTPOFF64", "TLS_TPOFF32", "TLS_TPOFF64",
};
#endif

#define RELOC_RESOLVE_SYMBOL(t)         ((reloc_target_flags[t] & _RF_S) != 0)
#define RELOC_PC_RELATIVE(t)            ((reloc_target_flags[t] & _RF_P) != 0)
#define RELOC_BASE_RELATIVE(t)          ((reloc_target_flags[t] & _RF_B) != 0)
#define RELOC_UNALIGNED(t)              ((reloc_target_flags[t] & _RF_U) != 0)
#define RELOC_USE_ADDEND(t)             ((reloc_target_flags[t] & _RF_A) != 0)
#define RELOC_TARGET_SIZE(t)            ((reloc_target_flags[t] >> 8) & 0xff)
#define RELOC_VALUE_RIGHTSHIFT(t)       (reloc_target_flags[t] & 0xff)
#define RELOC_TLS(t)                    (t >= R_TYPE(TLS_GD_HI22))

static const int reloc_target_bitmask[] = {
#define _BM(x)  (~(-(1ULL << (x))))
  0,                            /* NONE */
  _BM(8), _BM(16), _BM(32),     /* RELOC_8, _16, _32 */
  _BM(8), _BM(16), _BM(32),     /* DISP8, DISP16, DISP32 */
  _BM(30), _BM(22),             /* WDISP30, WDISP22 */
  _BM(22), _BM(22),             /* HI22, _22 */
  _BM(13), _BM(10),             /* RELOC_13, _LO10 */
  _BM(10), _BM(13), _BM(22),    /* GOT10, GOT13, GOT22 */
  _BM(10), _BM(22),             /* _PC10, _PC22 */
  _BM(30), 0,                   /* _WPLT30, _COPY */
  -1, -1, -1,                   /* _GLOB_DAT, JMP_SLOT, _RELATIVE */
  _BM(32)                       /* _UA32 */
#undef _BM
};
#define RELOC_VALUE_BITMASK(t)  (reloc_target_bitmask[t])

uint32_t
rtems_rtl_elf_section_flags (const rtems_rtl_obj* obj,
                             const Elf_Shdr*      shdr)
{
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
  bool r = false;
  if (type < RELOC_TARGET_FLAGS_SIZE) {
    r = RELOC_RESOLVE_SYMBOL (type) ? true : false;
  }
  switch (type) {
    case R_TYPE(TLS_DTPOFF32):
    case R_TYPE(TLS_LE_HIX22):
    case R_TYPE(TLS_LE_LOX10):
      r = true;
      break;
  default:
    break;
  }
  return r;
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
  return rtems_rtl_elf_rel_no_error;
}

rtems_rtl_elf_rel_status
rtems_rtl_elf_relocate_rela (rtems_rtl_obj*            obj,
                             const Elf_Rela*           rela,
                             const rtems_rtl_obj_sect* sect,
                             const char*               symname,
                             const Elf_Byte            syminfo,
                             const Elf_Word            symvalue)
{
  Elf_Addr *where;
  Elf_Word type, value, mask;
  Elf_Addr tmp = 0;

  where = (Elf_Addr *) (sect->base + rela->r_offset);

  type = ELF_R_TYPE(rela->r_info);
  if (type == R_TYPE(NONE))
    return rtems_rtl_elf_rel_no_error;

  /* We do JMP_SLOTs in _rtld_bind() below */
  if (type == R_TYPE(JMP_SLOT))
    return rtems_rtl_elf_rel_no_error;

  /* COPY relocs are also handled elsewhere */
  if (type == R_TYPE(COPY))
    return rtems_rtl_elf_rel_no_error;

  /*
   * We use the fact that relocation types are an `enum'
   * Note: R_SPARC_6 is currently numerically largest.
   */
  if (type > R_TYPE(TLS_TPOFF64)) {
    if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
      printf("rtl: invalid reloc type: %d\n", (int) type);
    return rtems_rtl_elf_rel_failure;
  }

  value = rela->r_addend;

  /*
   * Handle TLS relocations here, they are different.
   */
  if (RELOC_TLS(type)) {
    switch (type) {
    case R_TYPE(TLS_DTPMOD32):
      #if NETBSD_CODE__NOT_USED
      *where = (Elf_Addr)defobj->tlsindex;
      #endif

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf("rtl: reloc: TLS_DTPMOD32 %s in %s --> %p\n",
               symname, rtems_rtl_obj_oname (obj), (void *)*where);
      break;

    case R_TYPE(TLS_DTPOFF32):
      *where = (Elf_Addr)(symvalue + rela->r_addend);

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf("rtl: reloc: TLS_DTPOFF32 %s in %s --> %p\n",
               symname, rtems_rtl_obj_oname (obj), (void *)*where);
      break;

    case R_TYPE(TLS_TPOFF32):
      #if NETBSD_CODE__NOT_USED
      if (!defobj->tls_static &&
          _rtld_tls_offset_allocate(__UNCONST(defobj)))
        return ;

      *where = (Elf_Addr)(def->st_value -
                          defobj->tlsoffset + rela->r_addend);
     #endif
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf("rtl: reloc: TLS_TPOFF32 %s in %s --> %p\n",
               symname, rtems_rtl_obj_oname (obj), (void *)*where);
      return rtems_rtl_elf_rel_failure;

    case R_TYPE(TLS_LE_HIX22):
      *where |= ((symvalue + rela->r_addend) ^ 0xffffffff) >> 10;
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf("rtl: reloc: R_SPARC_TLS_LE_HIX22 %s in %s --> %p\n",
               symname, rtems_rtl_obj_oname (obj), (void *)*where);
      break;

    case R_TYPE(TLS_LE_LOX10):
      *where |= ((symvalue + rela->r_addend) & 0x3ff) | 0x1c00;
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf("rtl: reloc: R_SPARC_TLS_LE_LOX10 %s in %s --> %p\n",
               symname, rtems_rtl_obj_oname (obj), (void *)*where);
      break;

    default:
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf("rtl: reloc: unknown TLS relo: %d for %s in %s --> %p\n",
               type, symname, rtems_rtl_obj_oname (obj), (void *)*where);
      return rtems_rtl_elf_rel_failure;
    }
    return rtems_rtl_elf_rel_no_error;
  }

  /*
   * If it is no TLS relocation (handled above), we can not
   * deal with it if it is beyond R_SPARC_6.
   */
  if (type > R_TYPE(6))
    return rtems_rtl_elf_rel_failure;

  /*
   * Handle relative relocs here, as an optimization.
   */
  if (type == R_TYPE (RELATIVE)) {
    *where += (Elf_Addr)(sect->base + value);
    if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
      printf ("rtl: reloc relative in %s --> %p",
              rtems_rtl_obj_oname (obj), (void *)*where);
    return rtems_rtl_elf_rel_no_error;
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
        printf("rtl: reloc base_rel(%s): where=%p, *where 0x%" PRIu32 ", "
               "addend=0x%" PRIu32 ", base %p\n",
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

  return rtems_rtl_elf_rel_no_error;
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
  rtems_rtl_set_error (EINVAL, "rel type record not supported");
  return rtems_rtl_elf_rel_failure;
}

rtems_rtl_elf_rel_status
rtems_rtl_elf_relocate_rel (rtems_rtl_obj*            obj,
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
  printf ("rtl: rel type record not supported; please report\n");
  return rtems_rtl_elf_rel_failure;
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
