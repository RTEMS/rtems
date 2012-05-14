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
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
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
extern const uint32_t ppc_exc_min_prolog_async_tmpl_normal [];

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

static bool ppc_exc_create_branch_op(
  unsigned vector,
  uint32_t *prologue,
  size_t prologue_size
)
{
  static const uintptr_t BRANCH_OP_CODE = 18 << 26;
/*  static const uintptr_t BRANCH_OP_LINK = 0x1; */
  static const uintptr_t BRANCH_OP_ABS = 0x2;
  static const uintptr_t BRANCH_OP_MSK = 0x3ffffff;
  size_t branch_op_index = prologue_size / 4 - 1;
  uintptr_t vector_address = (uintptr_t) ppc_exc_vector_address(vector);
  uintptr_t branch_op_address = vector_address + 4 * branch_op_index;

  /* This value may have BRANCH_OP_LINK set */
  uintptr_t target_address = prologue [branch_op_index];

  uintptr_t branch_target_address = target_address - branch_op_address;

  /*
   * We prefer to use a relative branch.  This has the benefit that custom
   * minimal prologues in a read-only area are relocatable.
   */
  if ((branch_target_address & ~BRANCH_OP_MSK) != 0) {
    /* Target to far for relative branch (PC ± 32M) */
    if (target_address >= 0xfe000001 || target_address < 0x01fffffd) {
      /* Can use an absolute branch */
      branch_target_address = (target_address | BRANCH_OP_ABS) & BRANCH_OP_MSK;
    } else {
      return false;
    }
  }

  prologue [branch_op_index] = BRANCH_OP_CODE | branch_target_address;

  return true;
}

rtems_status_code ppc_exc_make_prologue(
  unsigned vector,
  ppc_exc_category category,
  uint32_t *prologue,
  size_t *prologue_size
)
{
  const uint32_t *prologue_template = NULL;
  size_t prologue_template_size = 0;
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
      && ppc_cpu_is_bookE() != PPC_BOOKE_STD
      && ppc_cpu_is_bookE() != PPC_BOOKE_E500
  ) {
    prologue_template = ppc_exc_min_prolog_auto;
    prologue_template_size = (size_t) ppc_exc_min_prolog_size;
  } else if (
    category == PPC_EXC_CLASSIC_ASYNC
      && ppc_cpu_is_bookE() == PPC_BOOKE_E500
      && (ppc_interrupt_get_disable_mask() & MSR_CE) == 0
  ) {
    prologue_template = ppc_exc_min_prolog_async_tmpl_normal;
    prologue_template_size = (size_t) ppc_exc_min_prolog_size;
    fixup_vector = true;
  } else {
    prologue_template = ppc_exc_prologue_templates [category];
    prologue_template_size = (size_t) ppc_exc_min_prolog_size;
    fixup_vector = true;
  }

  if (prologue_template_size <= *prologue_size) {
    *prologue_size = prologue_template_size;

    memcpy(prologue, prologue_template, prologue_template_size);

    if (!ppc_exc_create_branch_op(vector, prologue, prologue_template_size)) {
      return RTEMS_INVALID_ADDRESS;
    }

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
