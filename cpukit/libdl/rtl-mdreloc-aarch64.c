/*
 * Taken from NetBSD and stripped of the relocations not needed on RTEMS.
 */

/* $NetBSD: mdreloc.c,v 1.14 2020/06/16 21:01:30 joerg Exp $ */

/*-
 * Copyright (c) 2014 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Matt Thomas of 3am Software Foundry.
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

/*-
 * Copyright (c) 2014-2015 The FreeBSD Foundation
 * All rights reserved.
 *
 * Portions of this software were developed by Andrew Turner
 * under sponsorship from the FreeBSD Foundation.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#ifndef lint
__RCSID("$NetBSD: mdreloc.c,v 1.14 2020/06/16 21:01:30 joerg Exp $");
#endif /* not lint */

#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/endian.h>

#include <rtems/rtl/rtl.h>
#include "rtl-elf.h"
#include "rtl-error.h"
#include <rtems/rtl/rtl-trace.h>
#include <rtems/score/tls.h>

#include "rtl-unwind.h"
#include "rtl-unwind-dw2.h"

struct tls_data {
  size_t    td_tlsindex;
  Elf_Addr  td_tlsoffs;
};

rtems_rtl_elf_rel_status
rtems_rtl_elf_reloc_rela (
  rtems_rtl_obj*            obj,
  const Elf_Rela*           rela,
  const rtems_rtl_obj_sect* sect,
  const char*               symname,
  const Elf_Byte            syminfo,
  const Elf_Word            symvalue,
  const bool                parsing
);

#define __BITS(hi,lo) ((~((~(Elf_Addr)0)<<((hi)+1)))&((~(Elf_Addr)0)<<(lo)))
#define WIDTHMASK(w) (0xffffffffffffffffUL >> (64 - (w)))

static inline bool
checkoverflow(Elf_Addr addr, int bitwidth, Elf_Addr targetaddr,
    const char *bitscale, void *where, Elf64_Addr off)
{
  const Elf_Addr mask = ~__BITS(bitwidth - 1, 0);

  if (((addr & mask) != 0) && ((addr & mask) != mask)) {
    printf("kobj_reloc: Relocation 0x%" PRIxPTR " too far from %p"
        " (base+0x%jx) for %dbit%s\n",
        (uintptr_t)targetaddr, where, off, bitwidth, bitscale);
    return true;
  }

  return false;
}

static inline bool
checkalign(Elf_Addr addr, int alignbyte, void *where, Elf64_Addr off)
{
  if ((addr & (alignbyte - 1)) != 0) {
    printf("kobj_reloc: Relocation 0x%" PRIxPTR " unaligned at %p"
        " (base+0x%jx). must be aligned %d\n",
        (uintptr_t)addr, where, off, alignbyte);
    return true;
  }
  return false;
}

/*
 * Set to 1 to allow untested relocations. If you tested one and it
 * works or you fixed the relocation please remove the guard.
 */
#define ALLOW_UNTESTED_RELOCS 1

static void*
set_veneer(void* tramopline, Elf_Addr target)
{
  /*
   * http://shell-storm.org/online/Online-Assembler-and-Disassembler/
   *
   * ldr x9, #8
   * br x9
   *
   */
  uint64_t* tramp = (uint64_t*) tramopline;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  *tramp++ = 0xd61f012058000049;
#else
  *tramp++ = 0xd61f012058000049; /* not tested */
#endif
  *tramp++ = (uint64_t) target;
  return tramp;
}

static size_t
get_veneer_size(int type)
{
  (void) type;
  return 16;
}

uint32_t rtems_rtl_obj_tramp_alignment (const rtems_rtl_obj* obj)
{
  (void) obj;
  return sizeof(uint64_t);
}

size_t
rtems_rtl_elf_relocate_tramp_max_size (void)
{
  return 16;
}

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
  return true;
}

rtems_rtl_elf_rel_status
rtems_rtl_elf_reloc_rela (rtems_rtl_obj*            obj,
                          const Elf_Rela*           rela,
                          const rtems_rtl_obj_sect* sect,
                          const char*               symname,
                          const Elf_Byte            syminfo,
                          const Elf_Word            symvalue,
                          const bool                parsing)
{
  Elf64_Addr *where;
  Elf32_Addr *where32;
  Elf_Addr    off = rela->r_offset;
  Elf_Addr    target;
  Elf_Addr    raddr;
  uint32_t   *insn, immhi, immlo, shift;

  where = (Elf_Addr *)(sect->base + rela->r_offset);
  where32 = (void *)where;

  insn = (uint32_t *)where;

  /*
   * S - the address of the symbol
   * A - the addend of the reolcation
   * P - the address of the place being relocated (derived from r_offset)
   * Page(expr) - the page address of the expression expr, defined as (expr & ~0xFFF).
   * TPREL(expr) - the thread pointer offset of the expression expr
   */
  switch (ELF_R_TYPE(rela->r_info)) {
    case R_TYPE(NONE):
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC)) {
        printf ("rtl: NONE %p in %s\n", where, rtems_rtl_obj_oname (obj));
      }
      break;

    case R_TYPE(ABS64): /* word S + A */
    case R_TYPE(GLOB_DAT): /* word S + A */
      if (!parsing) {
        target = (Elf_Addr)symvalue + rela->r_addend;

        if (*where != target)
          *where = target;

        if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
          printf ("rtl: reloc 64/GLOB_DAT in %s --> %p in %s\n",
                  sect->name, (void *)(uintptr_t)*where,
                  rtems_rtl_obj_oname (obj));
      }
      break;

    /*
     * If S is a normal symbol, resolves to the difference between the static
     * link address of S and the execution address of S. If S is the null symbol
     * (ELF symbol index 0), resolves to the difference between the static link
     * address of P and the execution address of P.
     */
    case R_TYPE(RELATIVE):  /* Delta(S) + A */
      if (!parsing) {
        *where = (Elf_Addr)(uintptr_t)(sect->base + rela->r_addend);
        if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
          printf ("rtl: reloc RELATIVE in %s --> %p in %s\n",
                  sect->name, (void *)(uintptr_t)*where,
                  rtems_rtl_obj_oname (obj));
      }
      break;

    case R_TYPE(COPY):
      /*
       * These are deferred until all other relocations have
       * been done.  All we do here is make sure that the
       * COPY relocation is not in a shared library.  They
       * are allowed only in executable files.
       */
      printf("rtl: reloc COPY (please report)\n");
      break;

    case R_AARCH64_ADD_TPREL_HI12: /* TPREL(S + A) */
    case R_AARCH64_ADD_TPREL_LO12:
    case R_AARCH64_ADD_TPREL_LO12_NC:
      uint32_t of_check = 0;
      switch (ELF_R_TYPE(rela->r_info)) {
        case R_AARCH64_ADD_TPREL_LO12:
	  of_check = 212;
	/* fallthrough */
        case R_AARCH64_ADD_TPREL_LO12_NC:
          shift = 0;
          break;
        case R_AARCH64_ADD_TPREL_HI12:
	  of_check = 224;
          shift = 12;
          break;
        default:
          printf("illegal rtype: %" PRIu64 "\n", ELF_R_TYPE(rela->r_info));
          break;
      }

      if (!parsing) {
        target = (Elf_Addr)symvalue;
        target >>= shift;
	target &= WIDTHMASK(12);
        if (of_check && target >= of_check) {
          return rtems_rtl_elf_rel_failure;
        }
        *insn = htole32(
            (le32toh(*insn) & ~__BITS(21,10)) | (target << 10));
      }
      break;

    case R_AARCH64_ADD_ABS_LO12_NC: /* S + A */
    case R_AARCH64_LDST8_ABS_LO12_NC:
    case R_AARCH_LDST16_ABS_LO12_NC:
    case R_AARCH_LDST32_ABS_LO12_NC:
    case R_AARCH_LDST64_ABS_LO12_NC:
    case R_AARCH64_LDST128_ABS_LO12_NC:
      switch (ELF_R_TYPE(rela->r_info)) {
        case R_AARCH64_ADD_ABS_LO12_NC:
        case R_AARCH64_LDST8_ABS_LO12_NC:
          shift = 0;
          break;
        case R_AARCH_LDST16_ABS_LO12_NC:
          shift = 1;
          break;
        case R_AARCH_LDST32_ABS_LO12_NC:
          shift = 2;
          break;
        case R_AARCH_LDST64_ABS_LO12_NC:
          shift = 3;
          break;
        case R_AARCH64_LDST128_ABS_LO12_NC:
          shift = 4;
          break;
        default:
          printf("illegal rtype: %" PRIu64 "\n", ELF_R_TYPE(rela->r_info));
          break;
      }

      /*
       * S + A
       *  e.g.) add  x0,x0,#:lo12:<sym>+<addend>
       *        ldrb w0,[x0,#:lo12:<sym>+<addend>]
       *        ldrh w0,[x0,#:lo12:<sym>+<addend>]
       *        ldr  w0,[x0,#:lo12:<sym>+<addend>]
       *        ldr  x0,[x0,#:lo12:<sym>+<addend>]
       */
      if (!parsing) {
        target = (Elf_Addr)symvalue + rela->r_addend;
        if (checkalign(target, 1 << shift, where, off)) {
          printf ("rtl: reloc checkalign failed in %s --> %p in %s\n",
                  sect->name, (void *)(uintptr_t)*where,
                  rtems_rtl_obj_oname (obj));
          printf("ELF_R_TYPE is : %" PRIu64 "\n", ELF_R_TYPE(rela->r_info));
          break;
        }
        target &= WIDTHMASK(12);
        target >>= shift;
        *insn = htole32(
            (le32toh(*insn) & ~__BITS(21,10)) | (target << 10));
      }
      break;

    case R_AARCH64_ADR_PREL_PG_HI21:
      /*
       * Page(S + A) - Page(P)
       *  e.g.) adrp x0,<sym>+<addend>
       */
      if (!parsing) {
        target = (Elf_Addr)symvalue + rela->r_addend;
        target = target >> 12;
        raddr = target << 12;
        target -= (uintptr_t)where >> 12;

        if (checkoverflow(target, 21, raddr, " x 4k", where, off)) {
          printf("]] %d\n", __LINE__);
          return rtems_rtl_elf_rel_failure;
        }

        immlo = target & WIDTHMASK(2);
        immhi = (target >> 2) & WIDTHMASK(19);
        *insn = htole32((le32toh(*insn) &
            ~(__BITS(30,29) | __BITS(23,5))) |
            (immlo << 29) | (immhi << 5));
      }
      break;

    case R_AARCH_JUMP26:
    case R_AARCH_CALL26:
      /*
       * S + A - P
       *  e.g.) b <sym>+<addend>
       *        bl <sym>+<addend>
       */
      if (parsing && sect->base == 0) {
        if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
          printf ("rtl: JUMP26/PC26/CALL tramp cache\n");
        return rtems_rtl_elf_rel_tramp_cache;
      }

      raddr = (Elf_Addr)symvalue + rela->r_addend;
      target = raddr - (uintptr_t)where;
      if (checkalign(target, 4, where, off)) {
        return rtems_rtl_elf_rel_failure;
      }

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf (
          "rtl: JUMP26/PC26/CALL: insn=%p where=%p target=%p raddr=%p parsing=%d\n",
          insn, (void*) where, (void*)(uintptr_t) target, (void*)(uintptr_t) raddr,
          parsing
        );

      target = (intptr_t)target >> 2;

      if (((Elf_Sword)target > 0x1FFFFFF) || ((Elf_Sword)target < -0x2000000)) {
        Elf_Word tramp_addr;
        size_t   tramp_size = get_veneer_size(ELF_R_TYPE(rela->r_info));

        if (parsing) {
          if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
            printf ("rtl: JUMP26/PC26/CALL tramp add\n");
          return rtems_rtl_elf_rel_tramp_add;
        }

        if (!rtems_rtl_obj_has_tramp_space (obj, tramp_size)) {
          rtems_rtl_set_error (EINVAL,
                               "%s: CALL/JUMP26: overflow: no tramp memory",
                               sect->name);
          return rtems_rtl_elf_rel_failure;
        }

        tramp_addr = ((Elf_Addr)(uintptr_t) obj->tramp_brk) | (symvalue & 1);
        obj->tramp_brk = set_veneer(obj->tramp_brk, symvalue);
        target = tramp_addr + rela->r_addend - (uintptr_t)where;
        target = (uintptr_t)target >> 2;
      }

      if (checkoverflow(target, 26, raddr, " word", where, off)) {
        return rtems_rtl_elf_rel_failure;
      }

      if (!parsing) {
        target &= WIDTHMASK(26);
        *insn = htole32((le32toh(*insn) & ~__BITS(25,0)) | target);
      }

      break;

    case R_AARCH64_PREL32:
      /*
       * S + A - P
       *  e.g.) 1: .word <sym>+<addend>-1b
       */
      if (!parsing) {
        raddr = (Elf_Addr)symvalue + rela->r_addend;
        target = raddr - (uintptr_t)where;
        if (checkoverflow(target, 32, raddr, "", where, off)) {
          printf("]] %d\n", __LINE__);
          return rtems_rtl_elf_rel_failure;
        }
        *where32 = target;
      }
      break;

    case R_TYPE(TLSDESC):
      printf ("rtl: reloc TLSDESC in %s --> %p in %s\n",
              sect->name, (void *)(uintptr_t)*where,
              rtems_rtl_obj_oname (obj));
      break;

    case R_TLS_TYPE(TLS_DTPREL):
      printf ("rtl: reloc TLS_DTPREL in %s --> %p in %s\n",
              sect->name, (void *)(uintptr_t)*where,
              rtems_rtl_obj_oname (obj));
      break;
    case R_TLS_TYPE(TLS_DTPMOD):
      printf ("rtl: reloc TLS_DTPMOD in %s --> %p in %s\n",
              sect->name, (void *)(uintptr_t)*where,
              rtems_rtl_obj_oname (obj));
      break;

    case R_TLS_TYPE(TLS_TPREL):
      printf ("rtl: reloc TLS_TPREL in %s --> %p in %s\n",
              sect->name, (void *)(uintptr_t)*where,
              rtems_rtl_obj_oname (obj));
      break;

    default:
      printf ("rtl: Unsupported relocation type (%" PRIu64
              ") in %s --> %p in %s\n",
              ELF_R_TYPE(rela->r_info), sect->name, (void *)where,
              rtems_rtl_obj_oname (obj));
      return rtems_rtl_elf_rel_failure;
  }

  return rtems_rtl_elf_rel_no_error;
}

rtems_rtl_elf_rel_status
rtems_rtl_elf_relocate_rela_tramp (rtems_rtl_obj*            obj,
                                   const Elf_Rela*           rela,
                                   const rtems_rtl_obj_sect* sect,
                                   const char*               symname,
                                   const Elf_Byte            syminfo,
                                   const Elf_Word            symvalue)
{
  return rtems_rtl_elf_reloc_rela (obj,
                                   rela,
                                   sect,
                                   symname,
                                   syminfo,
                                   symvalue,
                                   true);
}

rtems_rtl_elf_rel_status
rtems_rtl_elf_relocate_rela (rtems_rtl_obj*            obj,
                             const Elf_Rela*           rela,
                             const rtems_rtl_obj_sect* sect,
                             const char*               symname,
                             const Elf_Byte            syminfo,
                             const Elf_Word            symvalue)
{
  return rtems_rtl_elf_reloc_rela (obj,
                                   rela,
                                   sect,
                                   symname,
                                   syminfo,
                                   symvalue,
                                   false);
}

rtems_rtl_elf_rel_status
rtems_rtl_elf_relocate_rel_tramp (rtems_rtl_obj*            obj,
                                  const Elf_Rel*            rel,
                                  const rtems_rtl_obj_sect* sect,
                                  const char*               symname,
                                  const Elf_Byte            syminfo,
                                  const Elf_Word            symvalue)
{
  rtems_rtl_set_error (EINVAL, "rela type record not supported");
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
  rtems_rtl_set_error (EINVAL, "rela type record not supported");
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
