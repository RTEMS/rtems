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
 * found in found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#include <rtems.h>

#include <bsp/vectors.h>

bool bsp_exceptions_in_RAM = true;

uint32_t ppc_exc_vector_base = 0;

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

  if (ppc_cpu_has_ivpr_and_ivor()) {
    /*
     * XXX: this directly matches the vector offsets in a e200z1, 
     * which has hardwired IVORs (IVOR0=0,IVOR1=0x10,IVOR2=0x20...) 
     */
    vector_offset >>= 4;
  }

  if (bsp_exceptions_in_RAM) {
    vector_base = ppc_exc_vector_base;
  }

  return (void *) (vector_base + vector_offset);
}
