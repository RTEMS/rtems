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
 * Copyright (C) 2009 embedded brains GmbH.
 *
 * Derived from file "libcpu/powerpc/new-exceptions/bspsupport/vectors_init.c".
 * Derived from file "libcpu/powerpc/new-exceptions/e500_raw_exc_init.c".
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>

#include <bsp/vectors.h>

uint32_t ppc_exc_cache_wb_check = 1;

#define MTIVPR(prefix) __asm__ volatile ("mtivpr %0" : : "r" (prefix))
#define MTIVOR(x, vec) __asm__ volatile ("mtivor"#x" %0" : : "r" (vec))

static void ppc_exc_initialize_booke(void)
{
  /* Interupt vector prefix register */
  MTIVPR(ppc_exc_vector_base);

  if (ppc_cpu_is(PPC_e200z0) || ppc_cpu_is(PPC_e200z1)) {
    /*
     * These cores have hard wired IVOR registers.  An access will case a
     * program exception.
     */
    return;
  }

  /* Interupt vector offset registers */
  MTIVOR(0,  ppc_exc_vector_address(ASM_BOOKE_CRIT_VECTOR));
  MTIVOR(1,  ppc_exc_vector_address(ASM_MACH_VECTOR));
  MTIVOR(2,  ppc_exc_vector_address(ASM_PROT_VECTOR));
  MTIVOR(3,  ppc_exc_vector_address(ASM_ISI_VECTOR));
  MTIVOR(4,  ppc_exc_vector_address(ASM_EXT_VECTOR));
  MTIVOR(5,  ppc_exc_vector_address(ASM_ALIGN_VECTOR));
  MTIVOR(6,  ppc_exc_vector_address(ASM_PROG_VECTOR));
  MTIVOR(7,  ppc_exc_vector_address(ASM_FLOAT_VECTOR));
  MTIVOR(8,  ppc_exc_vector_address(ASM_SYS_VECTOR));
  MTIVOR(9,  ppc_exc_vector_address(ASM_BOOKE_APU_VECTOR));
  MTIVOR(10, ppc_exc_vector_address(ASM_BOOKE_DEC_VECTOR));
  MTIVOR(11, ppc_exc_vector_address(ASM_BOOKE_FIT_VECTOR));
  MTIVOR(12, ppc_exc_vector_address(ASM_BOOKE_WDOG_VECTOR));
  MTIVOR(13, ppc_exc_vector_address(ASM_BOOKE_DTLBMISS_VECTOR));
  MTIVOR(14, ppc_exc_vector_address(ASM_BOOKE_ITLBMISS_VECTOR));
  MTIVOR(15, ppc_exc_vector_address(ASM_BOOKE_DEBUG_VECTOR));
  if (ppc_cpu_is_e200() || ppc_cpu_is_e500()) {
    MTIVOR(32, ppc_exc_vector_address(ASM_E500_SPE_UNAVAILABLE_VECTOR));
    MTIVOR(33, ppc_exc_vector_address(ASM_E500_EMB_FP_DATA_VECTOR));
    MTIVOR(34, ppc_exc_vector_address(ASM_E500_EMB_FP_ROUND_VECTOR));
  }
  if (ppc_cpu_is_e500()) {
    MTIVOR(35, ppc_exc_vector_address(ASM_E500_PERFMON_VECTOR));
  }
}

rtems_status_code ppc_exc_initialize(
  uint32_t interrupt_disable_mask,
  uintptr_t interrupt_stack_begin,
  uintptr_t interrupt_stack_size
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  const ppc_exc_categories *const categories = ppc_exc_current_categories();
  uintptr_t const interrupt_stack_end = interrupt_stack_begin + interrupt_stack_size;
  uintptr_t interrupt_stack_pointer = interrupt_stack_end - PPC_MINIMUM_STACK_FRAME_SIZE;
  unsigned vector = 0;
  uint32_t sda_base = 0;
  uint32_t r13 = 0;

  if (categories == NULL) {
    return RTEMS_NOT_IMPLEMENTED;
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
    return RTEMS_NOT_CONFIGURED;
  }

  /* Ensure proper interrupt stack alignment */
  interrupt_stack_pointer &= ~((uintptr_t) CPU_STACK_ALIGNMENT - 1);

  /* Tag interrupt stack bottom */
  *(uint32_t *) interrupt_stack_pointer = 0;

  /* Move interrupt stack values to special purpose registers */
  PPC_SET_SPECIAL_PURPOSE_REGISTER(SPRG1, interrupt_stack_pointer);
  PPC_SET_SPECIAL_PURPOSE_REGISTER(SPRG2, interrupt_stack_begin);

  ppc_interrupt_set_disable_mask(interrupt_disable_mask);

  /* Use current MMU / RI settings when running C exception handlers */
  ppc_exc_msr_bits = ppc_machine_state_register() & (MSR_DR | MSR_IR | MSR_RI);

#ifdef __ALTIVEC__
  /* Need vector unit enabled to save/restore altivec context */
  ppc_exc_msr_bits |= MSR_VE;
#endif
 
  if (ppc_cpu_is_bookE() == PPC_BOOKE_STD || ppc_cpu_is_bookE() == PPC_BOOKE_E500) {
    ppc_exc_initialize_booke();
  }

  for (vector = 0; vector <= LAST_VALID_EXC; ++vector) {
    ppc_exc_category category = ppc_exc_category_for_vector(categories, vector);

    if (category != PPC_EXC_INVALID) {
      void *const vector_address = ppc_exc_vector_address(vector);
      uint32_t prologue [16];
      size_t prologue_size = sizeof(prologue);

      sc = ppc_exc_make_prologue(vector, category, prologue, &prologue_size);
      if (sc != RTEMS_SUCCESSFUL) {
        return RTEMS_INTERNAL_ERROR;
      }

      ppc_code_copy(vector_address, prologue, prologue_size);
    }
  }

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

  return RTEMS_SUCCESSFUL;
}
