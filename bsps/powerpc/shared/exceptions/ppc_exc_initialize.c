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
 * Copyright (C) 2007 Till Straumann <strauman@slac.stanford.edu>
 *
 * Copyright (C) 2009-2012 embedded brains GmbH.
 *
 * Derived from file "libcpu/powerpc/new-exceptions/bspsupport/vectors_init.c".
 * Derived from file "libcpu/powerpc/new-exceptions/e500_raw_exc_init.c".
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>

#include <bsp/vectors.h>
#include <bsp/fatal.h>

uint32_t ppc_exc_cache_wb_check = 1;

#define MTIVPR(prefix) __asm__ volatile ("mtivpr %0" : : "r" (prefix))
#define MTIVOR(x, vec) __asm__ volatile ("mtivor"#x" %0" : : "r" (vec))

static void ppc_exc_initialize_booke(void *vector_base)
{
  /* Interrupt vector prefix register */
  MTIVPR((uintptr_t) vector_base);

  if (
    ppc_cpu_is_specific_e200(PPC_e200z0)
      || ppc_cpu_is_specific_e200(PPC_e200z1)
  ) {
    /*
     * These cores have hard wired IVOR registers.  An access will case a
     * program exception.
     */
    return;
  }

  /* Interupt vector offset registers */
  MTIVOR(0,  ppc_exc_vector_address(ASM_BOOKE_CRIT_VECTOR, vector_base));
  MTIVOR(1,  ppc_exc_vector_address(ASM_MACH_VECTOR, vector_base));
  MTIVOR(2,  ppc_exc_vector_address(ASM_PROT_VECTOR, vector_base));
  MTIVOR(3,  ppc_exc_vector_address(ASM_ISI_VECTOR, vector_base));
  MTIVOR(4,  ppc_exc_vector_address(ASM_EXT_VECTOR, vector_base));
  MTIVOR(5,  ppc_exc_vector_address(ASM_ALIGN_VECTOR, vector_base));
  MTIVOR(6,  ppc_exc_vector_address(ASM_PROG_VECTOR, vector_base));
  MTIVOR(7,  ppc_exc_vector_address(ASM_FLOAT_VECTOR, vector_base));
  MTIVOR(8,  ppc_exc_vector_address(ASM_SYS_VECTOR, vector_base));
  MTIVOR(9,  ppc_exc_vector_address(ASM_BOOKE_APU_VECTOR, vector_base));
  MTIVOR(10, ppc_exc_vector_address(ASM_BOOKE_DEC_VECTOR, vector_base));
  MTIVOR(11, ppc_exc_vector_address(ASM_BOOKE_FIT_VECTOR, vector_base));
  MTIVOR(12, ppc_exc_vector_address(ASM_BOOKE_WDOG_VECTOR, vector_base));
  MTIVOR(13, ppc_exc_vector_address(ASM_BOOKE_DTLBMISS_VECTOR, vector_base));
  MTIVOR(14, ppc_exc_vector_address(ASM_BOOKE_ITLBMISS_VECTOR, vector_base));
  MTIVOR(15, ppc_exc_vector_address(ASM_BOOKE_DEBUG_VECTOR, vector_base));
  if (ppc_cpu_is_e200() || ppc_cpu_is_e500()) {
    MTIVOR(32, ppc_exc_vector_address(ASM_E500_SPE_UNAVAILABLE_VECTOR, vector_base));
    MTIVOR(33, ppc_exc_vector_address(ASM_E500_EMB_FP_DATA_VECTOR, vector_base));
    MTIVOR(34, ppc_exc_vector_address(ASM_E500_EMB_FP_ROUND_VECTOR, vector_base));
  }
  if (ppc_cpu_is_specific_e200(PPC_e200z7) || ppc_cpu_is_e500()) {
    MTIVOR(35, ppc_exc_vector_address(ASM_E500_PERFMON_VECTOR, vector_base));
  }
}

static void ppc_exc_fatal_error(void)
{
  bsp_fatal(PPC_FATAL_EXCEPTION_INITIALIZATION);
}

void ppc_exc_initialize_with_vector_base(
  uintptr_t interrupt_stack_begin,
  void *vector_base
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  const ppc_exc_categories *const categories = ppc_exc_current_categories();
  unsigned vector = 0;
  uint32_t sda_base = 0;
  uint32_t r13 = 0;

  if (categories == NULL) {
    ppc_exc_fatal_error();
  }

  /* Assembly code needs SDA_BASE in r13 (SVR4 or EABI). Make sure
   * early init code put it there.
   */
  __asm__ volatile (
    "lis %0, _SDA_BASE_@h\n"
    "ori %0, %0, _SDA_BASE_@l\n"
    "mr  %1, 13\n"
    : "=r" (sda_base), "=r"(r13)
  );

  if (sda_base != r13) {
    ppc_exc_fatal_error();
  }

  ppc_exc_initialize_interrupt_stack(interrupt_stack_begin);

#ifndef PPC_EXC_CONFIG_BOOKE_ONLY

  /* Use current MMU / RI settings when running C exception handlers */
  ppc_exc_msr_bits = ppc_machine_state_register() & (MSR_DR | MSR_IR | MSR_RI);

#ifdef __ALTIVEC__
  /* Need vector unit enabled to save/restore altivec context */
  ppc_exc_msr_bits |= MSR_VE;
#endif

#endif /* PPC_EXC_CONFIG_BOOKE_ONLY */

  if (ppc_cpu_is_bookE() == PPC_BOOKE_STD || ppc_cpu_is_bookE() == PPC_BOOKE_E500) {
    ppc_exc_initialize_booke(vector_base);
  }

  for (vector = 0; vector <= LAST_VALID_EXC; ++vector) {
    ppc_exc_category category = ppc_exc_category_for_vector(categories, vector);

    if (category != PPC_EXC_INVALID) {
      void *const vector_address = ppc_exc_vector_address(vector, vector_base);
      uint32_t prologue [16];
      size_t prologue_size = sizeof(prologue);

      sc = ppc_exc_make_prologue(
        vector,
        vector_base,
        category,
        prologue,
        &prologue_size
      );
      if (sc != RTEMS_SUCCESSFUL) {
        ppc_exc_fatal_error();
      }

      ppc_code_copy(vector_address, prologue, prologue_size);
    }
  }

#ifndef PPC_EXC_CONFIG_BOOKE_ONLY
  /* If we are on a classic PPC with MSR_DR enabled then
   * assert that the mapping for at least this task's
   * stack is write-back-caching enabled (see README/CAVEATS)
   * Do this only if the cache is physically enabled.
   * Since it is not easy to figure that out in a
   * generic way we need help from the BSP: BSPs
   * which run entirely w/o the cache may set
   * ppc_exc_cache_wb_check to zero prior to calling
   * this routine.
   *
   * We run this check only after exception handling is
   * initialized so that we have some chance to get
   * information printed if it fails.
   *
   * Note that it is unsafe to ignore this issue; if
   * the check fails, do NOT disable it unless caches
   * are always physically disabled.
   */
  if (ppc_exc_cache_wb_check && (MSR_DR & ppc_exc_msr_bits)) {
    /* The size of 63 assumes cache lines are at most 32 bytes */
    uint8_t dummy[63];
    uintptr_t p = (uintptr_t) dummy;
    /* If the dcbz instruction raises an alignment exception
     * then the stack is mapped as write-thru or caching-disabled.
     * The low-level code is not capable of dealing with this
     * ATM.
     */
    p = (p + 31U) & ~31U;
    __asm__ volatile ("dcbz 0, %0"::"b" (p));
    /* If we make it thru here then things seem to be OK */
  }
#endif /* PPC_EXC_CONFIG_BOOKE_ONLY */
}
