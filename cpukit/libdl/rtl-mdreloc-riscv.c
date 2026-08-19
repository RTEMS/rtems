/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplLibDL
 *
 * @brief This source file contains the RISC-V relocation support of the
 *   dynamic linker.
 */

/*-
 * Copyright (c) 2019 Hesham Almatary
 *
 * This software was developed by SRI International and the University of
 * Cambridge Computer Laboratory (Department of Computer Science and
 * Technology) under DARPA contract HR0011-18-C-0016 ("ECATS"), as part of the
 * DARPA SSITH research programme.
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

/* Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <sys/cdefs.h>

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "rtl-elf.h"
#include "rtl-error.h"
#include "rtl-unwind-dw2.h"
#include "rtl-unwind.h"
#include <rtems/rtl/rtl-trace.h>
#include <rtems/rtl/rtl.h>

uint32_t rtems_rtl_elf_section_flags(const rtems_rtl_obj* obj,
                                     const Elf_Shdr* shdr) {
  (void)obj;
  (void)shdr;

  return 0;
}

uint32_t rtems_rtl_elf_arch_parse_section(const rtems_rtl_obj* obj, int section,
                                          const char* name,
                                          const Elf_Shdr* shdr,
                                          const uint32_t flags) {
  (void)obj;
  (void)section;
  (void)name;
  (void)shdr;
  return flags;
}

bool rtems_rtl_elf_arch_section_alloc(const rtems_rtl_obj* obj,
                                      rtems_rtl_obj_sect* sect) {
  (void)obj;
  (void)sect;
  return false;
}

bool rtems_rtl_elf_arch_section_free(const rtems_rtl_obj* obj,
                                     rtems_rtl_obj_sect* sect) {
  (void)obj;
  (void)sect;
  return false;
}

bool rtems_rtl_elf_rel_resolve_sym(Elf_Word type) {
  (void)type;

  return true;
}

uint32_t rtems_rtl_obj_tramp_alignment(const rtems_rtl_obj* obj) {
  (void)obj;
  return sizeof(uint32_t);
}

size_t rtems_rtl_elf_relocate_tramp_max_size(void) {
  /*
   * Disable by returning 0.
   */
  return 0;
}

rtems_rtl_elf_rel_status rtems_rtl_elf_relocate_rel_tramp(
    rtems_rtl_obj* obj, const Elf_Rel* rel, const rtems_rtl_obj_sect* sect,
    const char* symname, const Elf_Byte syminfo, const Elf_Word symvalue) {
  (void)obj;
  (void)rel;
  (void)sect;
  (void)symname;
  (void)syminfo;
  (void)symvalue;
  return rtems_rtl_elf_rel_no_error;
}

// Extract bits V[Begin:End], where range is inclusive, and Begin must be < 63.
static uint32_t extractBits(uint64_t v, uint32_t begin, uint32_t end) {
  return (v & ((1ULL << (begin + 1)) - 1)) >> end;
}

/*
 * The relocated locations are not always naturally aligned. A compressed
 * instruction can move a 32-bit instruction to a 2 byte boundary and the
 * exception and unwind tables are packed. Access the memory byte wise.
 */

static void write16le(void* loc, uint16_t val) {
  uint8_t* p = (uint8_t*)loc;

  p[0] = (uint8_t)val;
  p[1] = (uint8_t)(val >> 8);
}

static void write32le(void* loc, uint32_t val) {
  uint8_t* p = (uint8_t*)loc;
  int b;

  for (b = 0; b < 4; ++b) {
    p[b] = (uint8_t)(val >> (b * 8));
  }
}

static void write64le(void* loc, uint64_t val) {
  uint8_t* p = (uint8_t*)loc;
  int b;

  for (b = 0; b < 8; ++b) {
    p[b] = (uint8_t)(val >> (b * 8));
  }
}

static uint16_t read16le(const void* loc) {
  const uint8_t* p = (const uint8_t*)loc;

  return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static uint32_t read32le(const void* loc) {
  const uint8_t* p = (const uint8_t*)loc;
  uint32_t val = 0;
  int b;

  for (b = 0; b < 4; ++b) {
    val |= (uint32_t)p[b] << (b * 8);
  }

  return val;
}

static uint64_t read64le(const void* loc) {
  const uint8_t* p = (const uint8_t*)loc;
  uint64_t val = 0;
  int b;

  for (b = 0; b < 8; ++b) {
    val |= (uint64_t)p[b] << (b * 8);
  }

  return val;
}

/*
 * A ULEB128 field is updated in place. The encoded length of the field does
 * not change, the linker pads the field when it is created.
 */

static uint64_t read_uleb128(const void* loc) {
  const uint8_t* p = (const uint8_t*)loc;
  uint64_t val = 0;
  unsigned int shift = 0;
  uint8_t byte;

  do {
    byte = *p++;
    val |= (uint64_t)(byte & 0x7f) << shift;
    shift += 7;
  } while ((byte & 0x80) != 0 && shift < 64);

  return val;
}

static void write_uleb128(void* loc, uint64_t val) {
  uint8_t* p = (uint8_t*)loc;

  while ((*p & 0x80) != 0) {
    *p++ = (uint8_t)(0x80 | (val & 0x7f));
    val >>= 7;
  }

  *p = (uint8_t)(val & 0x7f);
}

/*
 * The RISC-V ABI splits a PC relative address into the high 20 bits held by an
 * AUIPC instruction and the low 12 bits held by a following instruction. The
 * symbol of a R_RISCV_PCREL_LO12_I or R_RISCV_PCREL_LO12_S relocation is a
 * label on the AUIPC instruction and not the target of the address, so the
 * value of the AUIPC relocation is needed to relocate the low 12 bits.
 *
 * The relocation records of a section are processed in ascending offset order
 * and the loader is serialised by the run time linker lock, so a small cache
 * of the most recent AUIPC relocations covers the normal case.
 *
 * A high 20 bits relocation with an unresolved symbol is deferred by the run
 * time linker until the symbol is found in an object file loaded later. The
 * low 12 bits relocation is not deferred because its symbol is a local
 * label. Park such a relocation as a fix up and apply it when the AUIPC
 * instruction is relocated.
 */

#define RTEMS_RTL_RISCV_HI20_CACHE (8)
#define RTEMS_RTL_RISCV_LO12_FIXUPS (32)

typedef struct {
  const rtems_rtl_obj_sect* sect; /**< The section of the AUIPC. */
  Elf_Addr where;                 /**< The address of the AUIPC. */
  int32_t value;                  /**< The PC relative value of the AUIPC. */
} rtems_rtl_riscv_hi20;

typedef struct {
  const rtems_rtl_obj_sect* sect; /**< The section, NULL if entry is free. */
  Elf_Addr hi20;                  /**< The address of the AUIPC. */
  Elf_Addr where;                 /**< The address of the instruction. */
  bool store;                     /**< The instruction is a store. */
} rtems_rtl_riscv_lo12;

static rtems_rtl_riscv_hi20 riscv_hi20[RTEMS_RTL_RISCV_HI20_CACHE];
static size_t riscv_hi20_next;
static rtems_rtl_riscv_lo12 riscv_lo12[RTEMS_RTL_RISCV_LO12_FIXUPS];

static void riscv_write_hi20(void* where, int64_t value) {
  uint32_t hi20 = (uint32_t)(value + 0x800) & 0xFFFFF000;

  write32le(where, (read32le(where) & 0xFFF) | hi20);
}

static void riscv_write_lo12(void* where, int32_t value, bool store) {
  if (store) {
    uint32_t imm11_5 = extractBits((uint64_t)(int64_t)value, 11, 5) << 25;
    uint32_t imm4_0 = extractBits((uint64_t)(int64_t)value, 4, 0) << 7;
    write32le(where, (read32le(where) & 0x1FFF07F) | imm11_5 | imm4_0);
  } else {
    uint32_t imm11_0 = ((uint32_t)value & 0xFFF) << 20;
    write32le(where, (read32le(where) & 0xFFFFF) | imm11_0);
  }
}

static bool riscv_sect_contains(const rtems_rtl_obj_sect* sect, Elf_Addr addr) {
  Elf_Addr base = (Elf_Addr)(uintptr_t)sect->base;

  return addr >= base && addr < base + sect->size;
}

static void riscv_hi20_add(const rtems_rtl_obj_sect* sect, Elf_Addr where,
                           int32_t value) {
  size_t entry;

  riscv_hi20[riscv_hi20_next].sect = sect;
  riscv_hi20[riscv_hi20_next].where = where;
  riscv_hi20[riscv_hi20_next].value = value;
  riscv_hi20_next = (riscv_hi20_next + 1) % RTEMS_RTL_RISCV_HI20_CACHE;

  for (entry = 0; entry < RTEMS_RTL_RISCV_LO12_FIXUPS; ++entry) {
    rtems_rtl_riscv_lo12* fixup = &riscv_lo12[entry];
    if (fixup->sect == sect && fixup->hi20 == where &&
        riscv_sect_contains(sect, fixup->where)) {
      riscv_write_lo12((void*)(uintptr_t)fixup->where, value, fixup->store);
      fixup->sect = NULL;
    }
  }
}

static bool riscv_hi20_find(const rtems_rtl_obj_sect* sect, Elf_Addr where,
                            int32_t* value) {
  size_t entry;

  /*
   * Search the most recent entry first so a reused address finds the current
   * relocation and not a stale one.
   */
  for (entry = 1; entry <= RTEMS_RTL_RISCV_HI20_CACHE; ++entry) {
    const rtems_rtl_riscv_hi20* hi20 =
        &riscv_hi20[(riscv_hi20_next + RTEMS_RTL_RISCV_HI20_CACHE - entry) %
                    RTEMS_RTL_RISCV_HI20_CACHE];
    if (hi20->sect == sect && hi20->where == where) {
      *value = hi20->value;
      return true;
    }
  }

  return false;
}

static bool riscv_lo12_defer(const rtems_rtl_obj_sect* sect, Elf_Addr hi20,
                             Elf_Addr where, bool store) {
  size_t entry;

  for (entry = 0; entry < RTEMS_RTL_RISCV_LO12_FIXUPS; ++entry) {
    rtems_rtl_riscv_lo12* fixup = &riscv_lo12[entry];
    if (fixup->sect == NULL) {
      fixup->sect = sect;
      fixup->hi20 = hi20;
      fixup->where = where;
      fixup->store = store;
      return true;
    }
  }

  return false;
}

static rtems_rtl_elf_rel_status
rtems_rtl_elf_reloc_rela(rtems_rtl_obj* obj, const Elf_Rela* rela,
                         const rtems_rtl_obj_sect* sect, const char* symname,
                         const Elf_Byte syminfo, const Elf_Word symvalue,
                         const bool parsing) {
  (void)symname;
  (void)syminfo;

  uint8_t* where;
  int64_t value;
  int64_t pcrel;

  if (parsing) {
    return rtems_rtl_elf_rel_no_error;
  }

  where = (uint8_t*)sect->base + rela->r_offset;

  /*
   * The value is S + A, the address of the symbol plus the addend. A thread
   * local symbol resolves to the offset from the thread pointer. The PC
   * relative value is S + A - P where P is the address being relocated.
   */
  value = (int64_t)((uint64_t)symvalue + (uint64_t)rela->r_addend);
  pcrel = value - (int64_t)(uintptr_t)where;

  switch (ELF_R_TYPE(rela->r_info)) {
  case R_TYPE(NONE):
    break;

  /*
   * Linker relaxation hints. The loader does not relax and the compiler has
   * emitted a valid instruction sequence, so there is nothing to do.
   */
  case R_TYPE(RELAX):
  case R_TYPE(TPREL_ADD):
    break;

  case R_TYPE(RVC_BRANCH): {
    uint16_t insn = read16le(where) & 0xE383;
    uint16_t imm8 = extractBits(pcrel, 8, 8) << 12;
    uint16_t imm4_3 = extractBits(pcrel, 4, 3) << 10;
    uint16_t imm7_6 = extractBits(pcrel, 7, 6) << 5;
    uint16_t imm2_1 = extractBits(pcrel, 2, 1) << 3;
    uint16_t imm5 = extractBits(pcrel, 5, 5) << 2;
    insn |= imm8 | imm4_3 | imm7_6 | imm2_1 | imm5;

    write16le(where, insn);
  } break;

  case R_TYPE(RVC_JUMP): {
    uint16_t insn = read16le(where) & 0xE003;
    uint16_t imm11 = extractBits(pcrel, 11, 11) << 12;
    uint16_t imm4 = extractBits(pcrel, 4, 4) << 11;
    uint16_t imm9_8 = extractBits(pcrel, 9, 8) << 9;
    uint16_t imm10 = extractBits(pcrel, 10, 10) << 8;
    uint16_t imm6 = extractBits(pcrel, 6, 6) << 7;
    uint16_t imm7 = extractBits(pcrel, 7, 7) << 6;
    uint16_t imm3_1 = extractBits(pcrel, 3, 1) << 3;
    uint16_t imm5 = extractBits(pcrel, 5, 5) << 2;
    insn |= imm11 | imm4 | imm9_8 | imm10 | imm6 | imm7 | imm3_1 | imm5;

    write16le(where, insn);
  } break;

  case R_TYPE(RVC_LUI): {
    if (((value + 0x800) >> 12) == 0) {
      /* `c.lui rd, 0` is illegal, convert it to `c.li rd, 0` */
      write16le(where, (read16le(where) & 0x0F83) | 0x4000);
    } else {
      uint16_t imm17 = extractBits(value + 0x800, 17, 17) << 12;
      uint16_t imm16_12 = extractBits(value + 0x800, 16, 12) << 2;
      write16le(where, (read16le(where) & 0xEF83) | imm17 | imm16_12);
    }
  } break;

  case R_TYPE(JAL): {
    uint32_t insn = read32le(where) & 0xFFF;
    uint32_t imm20 = extractBits(pcrel, 20, 20) << 31;
    uint32_t imm10_1 = extractBits(pcrel, 10, 1) << 21;
    uint32_t imm11 = extractBits(pcrel, 11, 11) << 20;
    uint32_t imm19_12 = extractBits(pcrel, 19, 12) << 12;
    insn |= imm20 | imm10_1 | imm11 | imm19_12;

    write32le(where, insn);
  } break;

  case R_TYPE(BRANCH): {

    uint32_t insn = read32le(where) & 0x1FFF07F;
    uint32_t imm12 = extractBits(pcrel, 12, 12) << 31;
    uint32_t imm10_5 = extractBits(pcrel, 10, 5) << 25;
    uint32_t imm4_1 = extractBits(pcrel, 4, 1) << 8;
    uint32_t imm11 = extractBits(pcrel, 11, 11) << 7;
    insn |= imm12 | imm10_5 | imm4_1 | imm11;

    write32le(where, insn);
  } break;

  case R_TYPE(64):
    write64le(where, (uint64_t)value);
    break;
  case R_TYPE(32):
    write32le(where, (uint32_t)value);
    break;

  case R_TYPE(SET6):
    *where = (*where & 0xc0) | ((uint8_t)value & 0x3f);
    break;
  case R_TYPE(SET8):
    *where = (uint8_t)value;
    break;
  case R_TYPE(SET16):
    write16le(where, (uint16_t)value);
    break;
  case R_TYPE(SET32):
    write32le(where, (uint32_t)value);
    break;

  case R_TYPE(ADD8):
    *where = (uint8_t)(*where + (uint8_t)value);
    break;
  case R_TYPE(ADD16):
    write16le(where, (uint16_t)(read16le(where) + (uint16_t)value));
    break;
  case R_TYPE(ADD32):
    write32le(where, read32le(where) + (uint32_t)value);
    break;
  case R_TYPE(ADD64):
    write64le(where, read64le(where) + (uint64_t)value);
    break;

  case R_TYPE(SUB6):
    *where = (*where & 0xc0) | (((*where & 0x3f) - (uint8_t)value) & 0x3f);
    break;
  case R_TYPE(SUB8):
    *where = (uint8_t)(*where - (uint8_t)value);
    break;
  case R_TYPE(SUB16):
    write16le(where, (uint16_t)(read16le(where) - (uint16_t)value));
    break;
  case R_TYPE(SUB32):
    write32le(where, read32le(where) - (uint32_t)value);
    break;
  case R_TYPE(SUB64):
    write64le(where, read64le(where) - (uint64_t)value);
    break;

  case R_TYPE(SET_ULEB128):
    write_uleb128(where, (uint64_t)value);
    break;
  case R_TYPE(SUB_ULEB128):
    write_uleb128(where, read_uleb128(where) - (uint64_t)value);
    break;

  case R_TYPE(32_PCREL): {
    write32le(where, (uint32_t)pcrel);

    if (rtems_rtl_trace(RTEMS_RTL_TRACE_RELOC)) {
      printf("rtl: R_RISCV_32_PCREL %08" PRIx32 " @ %p in %s\n",
             read32le(where), where, rtems_rtl_obj_oname(obj));
    }

  } break;

  case R_TYPE(PCREL_HI20):
    riscv_hi20_add(sect, (Elf_Addr)(uintptr_t)where, (int32_t)pcrel);
    riscv_write_hi20(where, pcrel);
    break;

  case R_TYPE(GOT_HI20):
  case R_TYPE(HI20):
  case R_TYPE(TPREL_HI20):
    riscv_write_hi20(where, value);
    break;

  /*
   * The symbol of these relocations is a label on the AUIPC instruction of
   * the pair. The value is the low 12 bits of the value of the AUIPC
   * relocation.
   */
  case R_TYPE(PCREL_LO12_I):
  case R_TYPE(PCREL_LO12_S): {
    const bool store = ELF_R_TYPE(rela->r_info) == R_TYPE(PCREL_LO12_S);
    const Elf_Addr hi20 = (Elf_Addr)(uintptr_t)value;
    int32_t hi20_value;

    if (riscv_hi20_find(sect, hi20, &hi20_value)) {
      riscv_write_lo12(where, hi20_value, store);
    } else if (!riscv_lo12_defer(sect, hi20, (Elf_Addr)(uintptr_t)where,
                                 store)) {
      rtems_rtl_set_error(ENOMEM,
                          "%s: no fix up left for the PC relative low 12 "
                          "bits relocation",
                          sect->name);
      return rtems_rtl_elf_rel_failure;
    }
  } break;

  case R_TYPE(LO12_I):
  case R_TYPE(TPREL_LO12_I):
    riscv_write_lo12(where, (int32_t)value, false);
    break;

  case R_TYPE(LO12_S):
  case R_TYPE(TPREL_LO12_S):
    riscv_write_lo12(where, (int32_t)value, true);
    break;

  case R_TYPE(CALL_PLT):
  case R_TYPE(CALL):
    riscv_write_hi20(where, pcrel);
    riscv_write_lo12(where + 4, (int32_t)pcrel, false);
    break;

  default:
    rtems_rtl_set_error(EINVAL,
                        "%s: Unsupported relocation type %u "
                        "in non-PLT relocations",
                        sect->name, (uint32_t)ELF_R_TYPE(rela->r_info));
    return rtems_rtl_elf_rel_failure;
  }

  return rtems_rtl_elf_rel_no_error;
}

rtems_rtl_elf_rel_status
rtems_rtl_elf_relocate_rela(rtems_rtl_obj* obj, const Elf_Rela* rela,
                            const rtems_rtl_obj_sect* sect, const char* symname,
                            const Elf_Byte syminfo, const Elf_Word symvalue) {
  return rtems_rtl_elf_reloc_rela(obj, rela, sect, symname, syminfo, symvalue,
                                  false);
}

rtems_rtl_elf_rel_status rtems_rtl_elf_relocate_rela_tramp(
    rtems_rtl_obj* obj, const Elf_Rela* rela, const rtems_rtl_obj_sect* sect,
    const char* symname, const Elf_Byte syminfo, const Elf_Word symvalue) {
  return rtems_rtl_elf_reloc_rela(obj, rela, sect, symname, syminfo, symvalue,
                                  true);
}

rtems_rtl_elf_rel_status
rtems_rtl_elf_relocate_rel(rtems_rtl_obj* obj, const Elf_Rel* rel,
                           const rtems_rtl_obj_sect* sect, const char* symname,
                           const Elf_Byte syminfo, const Elf_Word symvalue) {
  (void)obj;
  (void)rel;
  (void)sect;
  (void)symname;
  (void)syminfo;
  (void)symvalue;

  rtems_rtl_set_error(EINVAL, "rel type record not supported");
  return rtems_rtl_elf_rel_failure;
}

bool rtems_rtl_elf_unwind_parse(const rtems_rtl_obj* obj, const char* name,
                                uint32_t flags) {
  return rtems_rtl_elf_unwind_dw2_parse(obj, name, flags);
}

bool rtems_rtl_elf_unwind_register(rtems_rtl_obj* obj) {
  return rtems_rtl_elf_unwind_dw2_register(obj);
}

bool rtems_rtl_elf_unwind_deregister(rtems_rtl_obj* obj) {
  return rtems_rtl_elf_unwind_dw2_deregister(obj);
}
