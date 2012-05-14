/**
 * @file
 *
 * @ingroup ppc_exc
 *
 * @brief PowerPC Exceptions implementation.
 */

/*
 * Copyright (C) 1999 Eric Valette (valette@crf.canon.fr)
 *                    Canon Centre Recherche France.
 *
 * Copyright (C) 2009 embedded brains GmbH.
 *
 * Enhanced by Jay Kulpinski <jskulpin@eng01.gdds.com>
 * to support 603, 603e, 604, 604e exceptions
 *
 * Moved to "libcpu/powerpc/new-exceptions" and consolidated
 * by Thomas Doerfler <Thomas.Doerfler@embedded-brains.de>
 * to be common for all PPCs with new exceptions.
 *
 * Derived from file "libcpu/powerpc/new-exceptions/raw_exception.c".
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>

#include <bsp/vectors.h>

bool bsp_exceptions_in_RAM = true;

uint32_t ppc_exc_vector_base = 0;

/*
 * XXX: These values are choosen to directly generate the vector offsets for an
 * e200z1 which has hard wired IVORs (IVOR0=0x00, IVOR1=0x10, IVOR2=0x20, ...).
 */
static const uint8_t ivor_values [] = {
  [ASM_BOOKE_CRIT_VECTOR] = 0,
  [ASM_MACH_VECTOR] = 1,
  [ASM_PROT_VECTOR] = 2,
  [ASM_ISI_VECTOR] = 3,
  [ASM_EXT_VECTOR] = 4,
  [ASM_ALIGN_VECTOR] = 5,
  [ASM_PROG_VECTOR] = 6,
  [ASM_FLOAT_VECTOR] = 7,
  [ASM_SYS_VECTOR] = 8,
  [ASM_BOOKE_APU_VECTOR] = 9,
  [ASM_BOOKE_DEC_VECTOR] = 10,
  [ASM_BOOKE_FIT_VECTOR] = 11,
  [ASM_BOOKE_WDOG_VECTOR] = 12,
  [ASM_BOOKE_DTLBMISS_VECTOR] = 13,
  [ASM_BOOKE_ITLBMISS_VECTOR] = 14,
  [ASM_BOOKE_DEBUG_VECTOR] = 15,
  [ASM_E500_SPE_UNAVAILABLE_VECTOR] = 16,
  [ASM_E500_EMB_FP_DATA_VECTOR] = 17,
  [ASM_E500_EMB_FP_ROUND_VECTOR] = 18,
  [ASM_E500_PERFMON_VECTOR] = 19
};

void *ppc_exc_vector_address(unsigned vector)
{
  uintptr_t vector_base = 0xfff00000;
  uintptr_t vector_offset = vector << 8;

  if (ppc_cpu_has_altivec()) {
    if (vector == ASM_60X_VEC_VECTOR) {
      vector_offset = ASM_60X_VEC_VECTOR_OFFSET;
    }
  }

  if (ppc_cpu_is(PPC_405)) {
    switch (vector) {
      case ASM_BOOKE_FIT_VECTOR:
        vector_offset = ASM_PPC405_FIT_VECTOR_OFFSET;
        break;
      case ASM_BOOKE_WDOG_VECTOR:
        vector_offset = ASM_PPC405_WDOG_VECTOR_OFFSET;
        break;
      case ASM_TRACE_VECTOR:
        vector_offset = ASM_PPC405_TRACE_VECTOR_OFFSET;
        break;
      case ASM_PPC405_APU_UNAVAIL_VECTOR:
        vector_offset = ASM_60X_VEC_VECTOR_OFFSET;
      default:
        break;
    }
  }

  if (
    ppc_cpu_is_bookE() == PPC_BOOKE_STD
      || ppc_cpu_is_bookE() == PPC_BOOKE_E500
  ) {
    if (vector < sizeof(ivor_values) / sizeof(ivor_values [0])) {
      vector_offset = ((uintptr_t) ivor_values [vector]) << 4;
    } else {
      vector_offset = 0;
    }
  }

  if (bsp_exceptions_in_RAM) {
    vector_base = ppc_exc_vector_base;
  }

  return (void *) (vector_base + vector_offset);
}
