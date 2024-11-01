/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUARMPMSAv7
 *
 * @brief This header file provides the API to manage an Arm PMSAv7 based
 *   Memory Protection Unit (MPU).
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_SCORE_ARMV7_PMSA_H
#define _RTEMS_SCORE_ARMV7_PMSA_H

#include <rtems/score/aarch32-system-registers.h>
#include <rtems/score/assert.h>
#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Allow users of this header file to optionally place the inline functions
 * into a non-standard section.
 */
#ifndef ARMV7_PMSA_TEXT_SECTION
#define ARMV7_PMSA_TEXT_SECTION
#endif

/**
 * @defgroup RTEMSScoreCPUARMPMSAv7 PMSAv7 Support
 *
 * @ingroup RTEMSScoreCPUARM
 *
 * @brief This group provides support functions to manage an Arm PMSAv7
 *   (Protected Memory System Architecture) based Memory Protection Unit (MPU).
 *
 * @{
 */

/* RGNR, MPU Region Number Register */

#define ARMV7_RGNR_REGION_SHIFT 0
#define ARMV7_RGNR_REGION_MASK 0xffU
#define ARMV7_RGNR_REGION_GET(_reg) \
  (((_reg)&ARMV7_RGNR_REGION_MASK) >> ARMV7_RGNR_REGION_SHIFT)
#define ARMV7_RGNR_REGION_SET(_reg, _val) \
  (((_reg) & ~ARMV7_RGNR_REGION_MASK) |   \
   (((_val) << ARMV7_RGNR_REGION_SHIFT) & ARMV7_RGNR_REGION_MASK))
#define ARMV7_RGNR_REGION(_val) \
  (((_val) << ARMV7_RGNR_REGION_SHIFT) & ARMV7_RGNR_REGION_MASK)

static inline uint32_t _ARMV7_Read_rgnr(void) {
  uint32_t value;

  __asm__ volatile("mrc p15, 0, %0, c6, c2, 0" : "=&r"(value) : : "memory");

  return value;
}

static inline void _ARMV7_Write_rgnr(uint32_t value) {
  __asm__ volatile("mcr p15, 0, %0, c6, c2, 0" : : "r"(value) : "memory");
}

/* DRBAR, Data Region Base Address Register */

static inline uint32_t _ARMV7_Read_drbar(void) {
  uint32_t value;

  __asm__ volatile("mrc p15, 0, %0, c6, c1, 0" : "=&r"(value) : : "memory");

  return value;
}

static inline void _ARMV7_Write_drbar(uint32_t value) {
  __asm__ volatile("mcr p15, 0, %0, c6, c1, 0" : : "r"(value) : "memory");
}

/* IRBAR, Instruction Region Base Address Register */

static inline uint32_t _ARMV7_Read_irbar(void) {
  uint32_t value;

  __asm__ volatile("mrc p15, 0, %0, c6, c1, 1" : "=&r"(value) : : "memory");

  return value;
}

static inline void _ARMV7_Write_irbar(uint32_t value) {
  __asm__ volatile("mcr p15, 0, %0, c6, c1, 1" : : "r"(value) : "memory");
}

#define ARMV7_RSR_EN 0x1U

#define ARMV7_RSR_RSIZE_SHIFT 1
#define ARMV7_RSR_RSIZE_MASK 0x3eU
#define ARMV7_RSR_RSIZE_GET(_reg) \
  (((_reg)&ARMV7_RSR_RSIZE_MASK) >> ARMV7_RSR_RSIZE_SHIFT)
#define ARMV7_RSR_RSIZE_SET(_reg, _val) \
  (((_reg) & ~ARMV7_RSR_RSIZE_MASK) |   \
   (((_val) << ARMV7_RSR_RSIZE_SHIFT) & ARMV7_RSR_RSIZE_MASK))
#define ARMV7_RSR_RSIZE(_val) \
  (((_val) << ARMV7_RSR_RSIZE_SHIFT) & ARMV7_RSR_RSIZE_MASK)

#define ARMV7_RSR_SD_SHIFT 8
#define ARMV7_RSR_SD_MASK 0xff00U
#define ARMV7_RSR_SD_GET(_reg) \
  (((_reg)&ARMV7_RSR_SD_MASK) >> ARMV7_RSR_SD_SHIFT)
#define ARMV7_RSR_SD_SET(_reg, _val) \
  (((_reg) & ~ARMV7_RSR_SD_MASK) |   \
   (((_val) << ARMV7_RSR_SD_SHIFT) & ARMV7_RSR_SD_MASK))
#define ARMV7_RSR_SD(_val) (((_val) << ARMV7_RSR_SD_SHIFT) & ARMV7_RSR_SD_MASK)

/* DRSR, Data Region Size and Enable Register */

static inline uint32_t _ARMV7_Read_drsr(void) {
  uint32_t value;

  __asm__ volatile("mrc p15, 0, %0, c6, c1, 2" : "=&r"(value) : : "memory");

  return value;
}

static inline void _ARMV7_Write_drsr(uint32_t value) {
  __asm__ volatile("mcr p15, 0, %0, c6, c1, 2" : : "r"(value) : "memory");
}

/* IRSR, Instruction Region Size and Enable Register */

static inline uint32_t _ARMV7_Read_irsr(void) {
  uint32_t value;

  __asm__ volatile("mrc p15, 0, %0, c6, c1, 3" : "=&r"(value) : : "memory");

  return value;
}

static inline void _ARMV7_Write_irsr(uint32_t value) {
  __asm__ volatile("mcr p15, 0, %0, c6, c1, 3" : : "r"(value) : "memory");
}

#define ARMV7_RACR_B 0x1U
#define ARMV7_RACR_C 0x2U
#define ARMV7_RACR_S 0x4U
#define ARMV7_RACR_TEX_0 0x8U
#define ARMV7_RACR_TEX_1 0x10U
#define ARMV7_RACR_TEX_2 0x20U
#define ARMV7_RACR_AP_0 0x100U
#define ARMV7_RACR_AP_1 0x200U
#define ARMV7_RACR_AP_2 0x400U
#define ARMV7_RACR_XN 0x1000U

/* DRACR, Data Region Access Control Register */

static inline uint32_t _ARMV7_Read_dracr(void) {
  uint32_t value;

  __asm__ volatile("mrc p15, 0, %0, c6, c1, 4" : "=&r"(value) : : "memory");

  return value;
}

static inline void _ARMV7_Write_dracr(uint32_t value) {
  __asm__ volatile("mcr p15, 0, %0, c6, c1, 4" : : "r"(value) : "memory");
}

/* IRACR, Instruction Region Access Control Register */

static inline uint32_t _ARMV7_Read_iracr(void) {
  uint32_t value;

  __asm__ volatile("mrc p15, 0, %0, c6, c1, 5" : "=&r"(value) : : "memory");

  return value;
}

static inline void _ARMV7_Write_iracr(uint32_t value) {
  __asm__ volatile("mcr p15, 0, %0, c6, c1, 5" : : "r"(value) : "memory");
}

#define ARMV7_PMSA_READ_ONLY_CACHED                                   \
  (ARMV7_RACR_TEX_0 | ARMV7_RACR_C | ARMV7_RACR_B | ARMV7_RACR_AP_0 | \
   ARMV7_RACR_AP_2)

#define ARMV7_PMSA_READ_ONLY_UNCACHED \
  (ARMV7_RACR_TEX_0 | ARMV7_RACR_AP_0 | ARMV7_RACR_AP_2)

#define ARMV7_PMSA_READ_WRITE_CACHED \
  (ARMV7_RACR_TEX_0 | ARMV7_RACR_C | ARMV7_RACR_B | ARMV7_RACR_AP_0)

#define ARMV7_PMSA_READ_WRITE_UNCACHED (ARMV7_RACR_TEX_0 | ARMV7_RACR_AP_0)

#define ARMV7_PMSA_READ_WRITE_SHARED \
  (ARMV7_RACR_TEX_0 | ARMV7_RACR_S | ARMV7_RACR_AP_0)

#define ARMV7_PMSA_SHAREABLE_DEVICE (ARMV7_RACR_B | ARMV7_RACR_AP_0)

#define ARMV7_PMSA_NON_SHAREABLE_DEVICE (ARMV7_RACR_TEX_1 | ARMV7_RACR_AP_0)

/**
 * @brief The region definition is used to initialize the Memory Protection
 *   Unit (MPU).
 *
 * A region is empty if the size is zero.
 */
typedef struct ARMV7_PMSA_Region {
  /**
   * @brief This member defines the begin address of the region.
   */
  uint32_t begin;

  /**
   * @brief This member defines the size in bytes of the region.
   */
  uint32_t size;

  /**
   * @brief This member defines the attributes of the region.
   */
  uint32_t attributes;
} ARMV7_PMSA_Region;

ARMV7_PMSA_TEXT_SECTION static inline uint32_t _ARMV7_PMSA_Power2(uint32_t x) {
  return 32 - __builtin_clz(x - 1U);
}

ARMV7_PMSA_TEXT_SECTION static inline uint32_t _ARMV7_PMSA_Ceil2(uint32_t x) {
  return 1U << _ARMV7_PMSA_Power2(x);
}

ARMV7_PMSA_TEXT_SECTION static inline uint32_t _ARMV7_PMSA_Down2(uint32_t x,
                                                                 uint32_t y) {
  uint32_t m = y - 1U;
  return x & ~m;
}

ARMV7_PMSA_TEXT_SECTION static inline uint32_t _ARMV7_PMSA_Up2(uint32_t x,
                                                               uint32_t y) {
  uint32_t m = y - 1U;
  return (x + m) & ~m;
}

ARMV7_PMSA_TEXT_SECTION static inline uint32_t
_ARMV7_PMSA_Get_region(uint32_t b, uint32_t s, uint32_t* rs) {
  uint32_t e = b + s;
  uint32_t s2 = _ARMV7_PMSA_Ceil2(s);
  uint32_t b2 = _ARMV7_PMSA_Down2(b, s2);
  uint32_t e2 = _ARMV7_PMSA_Up2(e, s2);
  uint32_t s3 = _ARMV7_PMSA_Ceil2(e2 - b2);
  *rs = s3;
  return _ARMV7_PMSA_Down2(b2, s3);
}

ARMV7_PMSA_TEXT_SECTION static inline void _ARMV7_PMSA_Write_region(
    uint32_t index,
    uint32_t b,
    uint32_t s,
    uint32_t rb,
    uint32_t rs,
    uint32_t attr) {
  int rp = _ARMV7_PMSA_Power2(rs);
  int shift = rp - 3;
  uint32_t u = (b - rb) >> shift;
  uint32_t v = (_ARMV7_PMSA_Up2(b + s, 1U << shift) - rb) >> shift;
  uint32_t sub = ((0xffU >> (8 - v)) >> u) << u;

  _ARMV7_Write_rgnr(ARMV7_RGNR_REGION(index));
  _ARM_Instruction_synchronization_barrier();
  _ARMV7_Write_drbar(rb);
  _ARMV7_Write_dracr(attr);
  _ARMV7_Write_drsr(ARMV7_RSR_SD(~sub) | ARMV7_RSR_RSIZE(rp - 1) |
                    ARMV7_RSR_EN);
  _ARM_Data_synchronization_barrier();
  _ARM_Instruction_synchronization_barrier();
}

ARMV7_PMSA_TEXT_SECTION static inline uint32_t
_ARMV7_PMSA_Add_regions(uint32_t index, uint32_t b, uint32_t s, uint32_t attr) {
  uint32_t rs;
  uint32_t rb = _ARMV7_PMSA_Get_region(b, s, &rs);
  uint32_t e = b + s;

  if (e - rb > rs) {
    uint32_t re = rb + rs;
    uint32_t bl = b;
    uint32_t sl = re - b;

    rb = _ARMV7_PMSA_Get_region(bl, sl, &rs);
    _ARMV7_PMSA_Write_region(index, bl, sl, rb, rs, attr);
    ++index;

    s = e - re;
    b = re;
    rb = _ARMV7_PMSA_Get_region(b, s, &rs);
  }

  _ARMV7_PMSA_Write_region(index, b, s, rb, rs, attr);
  return index + 1;
}

ARMV7_PMSA_TEXT_SECTION static inline uint32_t _ARMV7_PMSA_Get_max_regions(
  void
)
{
  uint32_t region_count = _AArch32_Read_mpuir();

  region_count &= AARCH32_MPUIR_REGION_MASK;
  region_count >>= AARCH32_MPUIR_REGION_SHIFT;

  return region_count;
}

ARMV7_PMSA_TEXT_SECTION static inline uint32_t _ARMV7_PMSA_Find_region(
    uintptr_t address,
    uint32_t index) {
  uint32_t region_count = _ARMV7_PMSA_Get_max_regions();

  while (index < region_count) {
    _ARMV7_Write_rgnr(ARMV7_RGNR_REGION(index));
    _ARM_Instruction_synchronization_barrier();

    uint32_t rsr = _ARMV7_Read_drsr();

    if ((rsr & ARMV7_RSR_EN) != 0) {
      uint32_t rbar = _ARMV7_Read_drbar();
      uint32_t offset = (address & ~UINT32_C(0x3)) - rbar;
      uint32_t region_power = ARMV7_RSR_RSIZE_GET(rsr) + 1;
      uint32_t region_size = UINT32_C(1) << region_power;

      if (offset < region_size) {
        if (region_size < 256) {
          return index;
        }

        uint32_t sub = offset >> (region_power - 3);

        if ((ARMV7_RSR_SD_GET(rsr) & (UINT32_C(1) << sub)) == 0) {
          return index;
        }
      }
    }

    ++index;
  }

  return UINT32_MAX;
}

ARMV7_PMSA_TEXT_SECTION
static inline bool _ARMV7_PMSA_Is_region_enabled(uint32_t index)
{
  _Assert(index < _ARMV7_PMSA_Get_max_regions());

  _ARMV7_Write_rgnr(ARMV7_RGNR_REGION(index));
  _ARM_Instruction_synchronization_barrier();

  return (_ARMV7_Read_drsr() & ARMV7_RSR_EN) != 0;
}

ARMV7_PMSA_TEXT_SECTION
static inline uint32_t _ARMV7_PMSA_Find_available_region(void)
{
  uint32_t region_count = _ARMV7_PMSA_Get_max_regions();
  uint32_t index;

  for (index = 0; index < region_count; index++) {
    if (!_ARMV7_PMSA_Is_region_enabled(index)) {
      return index;
    }
  }

  return UINT32_MAX;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_ARMV7_PMSA_H */
