/**
 * @file
 *
 * @ingroup ppc_exc
 *
 * @brief PowerPC Exceptions implementation.
 */

/*
 * Copyright (C) 2007 Till Straumann <strauman@slac.stanford.edu>
 *
 * Copyright (C) 2009 embedded brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#include <string.h>

#include <bsp/vectors.h>

/* Offset into minimal prolog where vector number is hardcoded */
#define PPC_EXC_PROLOG_VEC_OFFSET 2

/* Symbols are defined by the linker */
extern const char ppc_exc_min_prolog_size [];
extern const char ppc_exc_tgpr_clr_prolog_size [];

/* Special prologue for handling register shadowing on 603-style CPUs */
extern const uint32_t ppc_exc_tgpr_clr_prolog [];

/*
 * Classic prologue which determines the vector dynamically from the offset
 * address. This must only be used for classic, synchronous exceptions with a
 * vector offset aligned on a 256-byte boundary.
 */
extern const uint32_t ppc_exc_min_prolog_auto [];

extern const uint32_t ppc_exc_min_prolog_auto_packed [];

/* Minimal prologue templates */
extern const uint32_t ppc_exc_min_prolog_async_tmpl_std [];
extern const uint32_t ppc_exc_min_prolog_sync_tmpl_std [];
extern const uint32_t ppc_exc_min_prolog_async_tmpl_p405_crit [];
extern const uint32_t ppc_exc_min_prolog_sync_tmpl_p405_crit [];
extern const uint32_t ppc_exc_min_prolog_async_tmpl_bookE_crit [];
extern const uint32_t ppc_exc_min_prolog_sync_tmpl_bookE_crit [];
extern const uint32_t ppc_exc_min_prolog_sync_tmpl_e500_mchk [];
extern const uint32_t ppc_exc_min_prolog_async_tmpl_e500_mchk [];
extern const uint32_t ppc_exc_min_prolog_tmpl_naked [];

static const uint32_t *const ppc_exc_prologue_templates [] = {
  [PPC_EXC_CLASSIC] = ppc_exc_min_prolog_sync_tmpl_std,
  [PPC_EXC_CLASSIC_ASYNC] = ppc_exc_min_prolog_async_tmpl_std,
  [PPC_EXC_405_CRITICAL] = ppc_exc_min_prolog_sync_tmpl_p405_crit,
  [PPC_EXC_405_CRITICAL_ASYNC] = ppc_exc_min_prolog_async_tmpl_p405_crit,
  [PPC_EXC_BOOKE_CRITICAL] = ppc_exc_min_prolog_sync_tmpl_bookE_crit,
  [PPC_EXC_BOOKE_CRITICAL_ASYNC] = ppc_exc_min_prolog_async_tmpl_bookE_crit,
  [PPC_EXC_E500_MACHCHK] = ppc_exc_min_prolog_sync_tmpl_e500_mchk,
  [PPC_EXC_E500_MACHCHK_ASYNC] = ppc_exc_min_prolog_async_tmpl_e500_mchk,
  [PPC_EXC_NAKED] = ppc_exc_min_prolog_tmpl_naked
};

rtems_status_code ppc_exc_make_prologue(
  unsigned vector,
  ppc_exc_category category,
  uint32_t *prologue,
  size_t *prologue_size
)
{
  const uint32_t *prologue_template = NULL;
  size_t prologue_template_size = 0;
  uintptr_t vector_address = (uintptr_t) ppc_exc_vector_address(vector);
  bool fixup_vector = false;

  if (!ppc_exc_is_valid_category(category)) {
    return RTEMS_INVALID_NUMBER;
  }

  if (
    ppc_cpu_has_shadowed_gprs()
      && (vector == ASM_60X_IMISS_VECTOR
        || vector == ASM_60X_DLMISS_VECTOR
        || vector == ASM_60X_DSMISS_VECTOR)
  ) {
    prologue_template = ppc_exc_tgpr_clr_prolog;
    prologue_template_size = (size_t) ppc_exc_tgpr_clr_prolog_size;
  } else if (
    category == PPC_EXC_CLASSIC
      && ((vector_address & 0xffU) == 0
        || (ppc_cpu_has_ivpr_and_ivor() && (vector_address & 0xfU) == 0))
  ) {
    if (ppc_cpu_has_ivpr_and_ivor()) {
      prologue_template = ppc_exc_min_prolog_auto_packed;
    } else {
      prologue_template = ppc_exc_min_prolog_auto;
    }
    prologue_template_size = (size_t) ppc_exc_min_prolog_size;
  } else {
    prologue_template = ppc_exc_prologue_templates [category];
    prologue_template_size = (size_t) ppc_exc_min_prolog_size;
    fixup_vector = true;
  }

  if (prologue_template_size <= *prologue_size) {
    *prologue_size = prologue_template_size;

    memcpy(prologue, prologue_template, prologue_template_size);

    if (fixup_vector) {
      if (vector <= 0x7fffU) {
        prologue [PPC_EXC_PROLOG_VEC_OFFSET] =
          (prologue [PPC_EXC_PROLOG_VEC_OFFSET] & 0xffff8000U)
            | (vector & 0x7fffU);
      } else {
        return RTEMS_INVALID_ID;
      }
    }
  } else {
    return RTEMS_INVALID_SIZE;
  }

  return RTEMS_SUCCESSFUL;
}
