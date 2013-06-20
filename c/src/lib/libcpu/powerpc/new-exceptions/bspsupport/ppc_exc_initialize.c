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
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>

#include <bsp/vectors.h>
#include <bsp/generic-fatal.h>

#define PPC_EXC_ASSERT_OFFSET(field, off) \
  RTEMS_STATIC_ASSERT( \
    offsetof(CPU_Exception_frame, field) + FRAME_LINK_SPACE == off, \
    CPU_Exception_frame_offset_ ## field \
  )

#define PPC_EXC_ASSERT_CANONIC_OFFSET(field) \
  PPC_EXC_ASSERT_OFFSET(field, field ## _OFFSET)

PPC_EXC_ASSERT_OFFSET(EXC_SRR0, SRR0_FRAME_OFFSET);
PPC_EXC_ASSERT_OFFSET(EXC_SRR1, SRR1_FRAME_OFFSET);
PPC_EXC_ASSERT_OFFSET(_EXC_number, EXCEPTION_NUMBER_OFFSET);
PPC_EXC_ASSERT_CANONIC_OFFSET(EXC_CR);
PPC_EXC_ASSERT_CANONIC_OFFSET(EXC_CTR);
PPC_EXC_ASSERT_CANONIC_OFFSET(EXC_XER);
PPC_EXC_ASSERT_CANONIC_OFFSET(EXC_LR);
#ifdef __SPE__
  PPC_EXC_ASSERT_OFFSET(EXC_SPEFSCR, PPC_EXC_SPEFSCR_OFFSET);
  PPC_EXC_ASSERT_OFFSET(EXC_ACC, PPC_EXC_ACC_OFFSET);
#endif
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR0);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR1);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR2);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR3);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR4);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR5);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR6);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR7);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR8);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR9);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR10);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR11);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR12);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR13);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR14);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR15);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR16);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR17);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR18);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR19);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR20);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR21);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR22);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR23);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR24);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR25);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR26);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR27);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR28);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR29);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR30);
PPC_EXC_ASSERT_CANONIC_OFFSET(GPR31);

RTEMS_STATIC_ASSERT(
  PPC_EXC_MINIMAL_FRAME_SIZE % CPU_STACK_ALIGNMENT == 0,
  PPC_EXC_MINIMAL_FRAME_SIZE
);

RTEMS_STATIC_ASSERT(
  PPC_EXC_FRAME_SIZE % CPU_STACK_ALIGNMENT == 0,
  PPC_EXC_FRAME_SIZE
);

RTEMS_STATIC_ASSERT(
  sizeof(CPU_Exception_frame) + FRAME_LINK_SPACE <= PPC_EXC_FRAME_SIZE,
  CPU_Exception_frame
);

uint32_t ppc_exc_cache_wb_check = 1;

#define MTIVPR(prefix) __asm__ volatile ("mtivpr %0" : : "r" (prefix))
#define MTIVOR(x, vec) __asm__ volatile ("mtivor"#x" %0" : : "r" (vec))

static void ppc_exc_initialize_booke(void *vector_base)
{
  /* Interupt vector prefix register */
  MTIVPR((uint32_t) vector_base);

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
  bsp_generic_fatal(BSP_GENERIC_FATAL_EXCEPTION_INITIALIZATION);
}

void ppc_exc_initialize_with_vector_base(
  uint32_t interrupt_disable_mask,
  uintptr_t interrupt_stack_begin,
  uintptr_t interrupt_stack_size,
  void *vector_base
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

  /* Ensure proper interrupt stack alignment */
  interrupt_stack_pointer &= ~((uintptr_t) CPU_STACK_ALIGNMENT - 1);

  /* Tag interrupt stack bottom */
  *(uint32_t *) interrupt_stack_pointer = 0;

  /* Move interrupt stack values to special purpose registers */
  PPC_SET_SPECIAL_PURPOSE_REGISTER(SPRG1, interrupt_stack_pointer);
  PPC_SET_SPECIAL_PURPOSE_REGISTER(SPRG2, interrupt_stack_begin);

  ppc_interrupt_set_disable_mask(interrupt_disable_mask);

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
