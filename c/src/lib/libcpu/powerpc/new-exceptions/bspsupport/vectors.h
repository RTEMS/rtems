/**
 * @file
 *
 * @ingroup ppc_exc
 * @ingroup ppc_exc_frame
 *
 * @brief PowerPC Exceptions API.
 */

/*
 * Copyright (C) 1999 Eric Valette (valette@crf.canon.fr)
 *                    Canon Centre Recherche France.
 *
 * Copyright (C) 2007 Till Straumann <strauman@slac.stanford.edu>
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
 * Derived from file "libcpu/powerpc/new-exceptions/raw_exception.h".
 * Derived from file "libcpu/powerpc/new-exceptions/bspsupport/ppc_exc_bspsupp.h".
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

/* DO NOT INTRODUCE #ifdef <cpu_flavor> in this file */

#ifndef LIBCPU_VECTORS_H
#define LIBCPU_VECTORS_H

#include <bspopts.h>

#include <libcpu/powerpc-utility.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ppc_exc PowerPC Exceptions
 *
 * @brief XXX
 *
 * @{
 */

#define ASM_RESET_VECTOR                     0x01
#define ASM_MACH_VECTOR                      0x02
#define ASM_PROT_VECTOR                      0x03
#define ASM_ISI_VECTOR                       0x04
#define ASM_EXT_VECTOR                       0x05
#define ASM_ALIGN_VECTOR                     0x06
#define ASM_PROG_VECTOR                      0x07
#define ASM_FLOAT_VECTOR                     0x08
#define ASM_DEC_VECTOR                       0x09
#define ASM_SYS_VECTOR                       0x0C
#define ASM_TRACE_VECTOR                     0x0D

#define ASM_PPC405_APU_UNAVAIL_VECTOR        ASM_60X_VEC_ASSIST_VECTOR

#define ASM_8XX_FLOATASSIST_VECTOR           0x0E
#define ASM_8XX_SOFTEMUL_VECTOR              0x10
#define ASM_8XX_ITLBMISS_VECTOR              0x11
#define ASM_8XX_DTLBMISS_VECTOR              0x12
#define ASM_8XX_ITLBERROR_VECTOR             0x13
#define ASM_8XX_DTLBERROR_VECTOR             0x14
#define ASM_8XX_DBREAK_VECTOR                0x1C
#define ASM_8XX_IBREAK_VECTOR                0x1D
#define ASM_8XX_PERIFBREAK_VECTOR            0x1E
#define ASM_8XX_DEVPORT_VECTOR               0x1F

#define ASM_5XX_FLOATASSIST_VECTOR           0x0E
#define ASM_5XX_SOFTEMUL_VECTOR              0x10
#define ASM_5XX_IPROT_VECTOR                 0x13
#define ASM_5XX_DPROT_VECTOR                 0x14
#define ASM_5XX_DBREAK_VECTOR                0x1C
#define ASM_5XX_IBREAK_VECTOR                0x1D
#define ASM_5XX_MEBREAK_VECTOR               0x1E
#define ASM_5XX_NMEBREAK_VECTOR              0x1F

#define ASM_60X_VEC_VECTOR                   0x0A
#define ASM_60X_PERFMON_VECTOR               0x0F
#define ASM_60X_IMISS_VECTOR                 0x10
#define ASM_60X_DLMISS_VECTOR                0x11
#define ASM_60X_DSMISS_VECTOR                0x12
#define ASM_60X_ADDR_VECTOR                  0x13
#define ASM_60X_SYSMGMT_VECTOR               0x14
#define ASM_60X_VEC_ASSIST_VECTOR            0x16
#define ASM_60X_ITM_VECTOR                   0x17

/* Book E */
#define ASM_BOOKE_CRIT_VECTOR                0x01
/* We could use the std. decrementer vector # on bookE, too,
 * but the bookE decrementer has slightly different semantics
 * so we use a different vector (which happens to be
 * the PIT vector on the 405 which is like the booke decrementer)
 */
#define ASM_BOOKE_DEC_VECTOR                 0x10
#define ASM_BOOKE_ITLBMISS_VECTOR            0x11
#define ASM_BOOKE_DTLBMISS_VECTOR            0x12
#define ASM_BOOKE_FIT_VECTOR                 0x13
#define ASM_BOOKE_WDOG_VECTOR                0x14
#define ASM_BOOKE_APU_VECTOR                 0x18
#define ASM_BOOKE_DEBUG_VECTOR               ASM_TRACE_VECTOR

/* e200 and e500 */
#define ASM_E500_SPE_UNAVAILABLE_VECTOR      ASM_60X_VEC_VECTOR
#define ASM_E500_EMB_FP_DATA_VECTOR          0x19
#define ASM_E500_EMB_FP_ROUND_VECTOR         0x1A
#define ASM_E500_PERFMON_VECTOR              ASM_60X_PERFMON_VECTOR

/* e300 */
#define ASM_E300_CRIT_VECTOR                 0x0A
#define ASM_E300_PERFMON_VECTOR              ASM_60X_PERFMON_VECTOR
#define ASM_E300_IMISS_VECTOR                ASM_60X_IMISS_VECTOR  /* Special case: Shadowed GPRs */
#define ASM_E300_DLMISS_VECTOR               ASM_60X_DLMISS_VECTOR /* Special case: Shadowed GPRs */
#define ASM_E300_DSMISS_VECTOR               ASM_60X_DSMISS_VECTOR /* Special case: Shadowed GPRs */
#define ASM_E300_ADDR_VECTOR                 ASM_60X_ADDR_VECTOR
#define ASM_E300_SYSMGMT_VECTOR              ASM_60X_SYSMGMT_VECTOR

/*
 * If you change that number make sure to adjust the wrapper code in ppc_exc.S
 * and that ppc_exc_handler_table will be correctly initialized.
 */
#define LAST_VALID_EXC                       0x1F

/* DO NOT USE -- this symbol is DEPRECATED
 * (only used by libbsp/shared/vectors/vectors.S
 * which should not be used by new BSPs).
 */
#define ASM_60X_VEC_VECTOR_OFFSET            0xf20

#define ASM_PPC405_FIT_VECTOR_OFFSET         0x1010
#define ASM_PPC405_WDOG_VECTOR_OFFSET        0x1020
#define ASM_PPC405_TRACE_VECTOR_OFFSET       0x2000

/** @} */

#ifndef __SPE__
  #define PPC_EXC_GPR_OFFSET(gpr) ((gpr) * PPC_GPR_SIZE + 36)
  #define PPC_EXC_VECTOR_PROLOGUE_OFFSET PPC_EXC_GPR_OFFSET(4)
  #define PPC_EXC_MINIMAL_FRAME_SIZE 96
  #define PPC_EXC_FRAME_SIZE 176
#else
  #define PPC_EXC_SPEFSCR_OFFSET 36
  #define PPC_EXC_ACC_OFFSET 40
  #define PPC_EXC_GPR_OFFSET(gpr) ((gpr) * PPC_GPR_SIZE + 48)
  #define PPC_EXC_VECTOR_PROLOGUE_OFFSET (PPC_EXC_GPR_OFFSET(4) + 4)
  #define PPC_EXC_MINIMAL_FRAME_SIZE 160
  #define PPC_EXC_FRAME_SIZE 320
#endif

/**
 * @defgroup ppc_exc_frame PowerPC Exception Frame
 *
 * @brief XXX
 *
 * @{
 */

/*
 * The callee (high level exception code written in C)
 * will store the Link Registers (return address) at entry r1 + 4 !!!.
 * So let room for it!!!.
 */
#define LINK_REGISTER_CALLEE_UPDATE_ROOM 4

#define SRR0_FRAME_OFFSET 8
#define SRR1_FRAME_OFFSET 12
#define EXCEPTION_NUMBER_OFFSET 16
#define EXC_CR_OFFSET 20
#define EXC_CTR_OFFSET 24
#define EXC_XER_OFFSET 28
#define EXC_LR_OFFSET 32
#define GPR0_OFFSET PPC_EXC_GPR_OFFSET(0)
#define GPR1_OFFSET PPC_EXC_GPR_OFFSET(1)
#define GPR2_OFFSET PPC_EXC_GPR_OFFSET(2)
#define GPR3_OFFSET PPC_EXC_GPR_OFFSET(3)
#define GPR4_OFFSET PPC_EXC_GPR_OFFSET(4)
#define GPR5_OFFSET PPC_EXC_GPR_OFFSET(5)
#define GPR6_OFFSET PPC_EXC_GPR_OFFSET(6)
#define GPR7_OFFSET PPC_EXC_GPR_OFFSET(7)
#define GPR8_OFFSET PPC_EXC_GPR_OFFSET(8)
#define GPR9_OFFSET PPC_EXC_GPR_OFFSET(9)
#define GPR10_OFFSET PPC_EXC_GPR_OFFSET(10)
#define GPR11_OFFSET PPC_EXC_GPR_OFFSET(11)
#define GPR12_OFFSET PPC_EXC_GPR_OFFSET(12)
#define GPR13_OFFSET PPC_EXC_GPR_OFFSET(13)
#define GPR14_OFFSET PPC_EXC_GPR_OFFSET(14)
#define GPR15_OFFSET PPC_EXC_GPR_OFFSET(15)
#define GPR16_OFFSET PPC_EXC_GPR_OFFSET(16)
#define GPR17_OFFSET PPC_EXC_GPR_OFFSET(17)
#define GPR18_OFFSET PPC_EXC_GPR_OFFSET(18)
#define GPR19_OFFSET PPC_EXC_GPR_OFFSET(19)
#define GPR20_OFFSET PPC_EXC_GPR_OFFSET(20)
#define GPR21_OFFSET PPC_EXC_GPR_OFFSET(21)
#define GPR22_OFFSET PPC_EXC_GPR_OFFSET(22)
#define GPR23_OFFSET PPC_EXC_GPR_OFFSET(23)
#define GPR24_OFFSET PPC_EXC_GPR_OFFSET(24)
#define GPR25_OFFSET PPC_EXC_GPR_OFFSET(25)
#define GPR26_OFFSET PPC_EXC_GPR_OFFSET(26)
#define GPR27_OFFSET PPC_EXC_GPR_OFFSET(27)
#define GPR28_OFFSET PPC_EXC_GPR_OFFSET(28)
#define GPR29_OFFSET PPC_EXC_GPR_OFFSET(29)
#define GPR30_OFFSET PPC_EXC_GPR_OFFSET(30)
#define GPR31_OFFSET PPC_EXC_GPR_OFFSET(31)

#define EXC_GENERIC_SIZE PPC_EXC_FRAME_SIZE

#ifdef __ALTIVEC__
#define EXC_VEC_OFFSET EXC_GENERIC_SIZE
#ifndef PPC_CACHE_ALIGNMENT
#error "Missing include file!"
#endif
/*   20 volatile registers
 * + cache-aligned area for vcsr, vrsave
 * + area for alignment
 */
#define EXC_VEC_SIZE   (16*20 + 2*PPC_CACHE_ALIGNMENT)
#else
#define EXC_VEC_SIZE   (0)
#endif

/* Exception stack frame -> BSP_Exception_frame */
#define FRAME_LINK_SPACE 8

/*
 * maintain the EABI requested 8 bytes aligment
 * As SVR4 ABI requires 16, make it 16 (as some
 * exception may need more registers to be processed...)
 */
#define EXCEPTION_FRAME_END (EXC_GENERIC_SIZE + EXC_VEC_SIZE)

/** @} */

#ifndef ASM

/**
 * @ingroup ppc_exc_frame
 *
 * @{
 */

typedef CPU_Exception_frame BSP_Exception_frame;

/** @} */

/**
 * @ingroup ppc_exc
 *
 * @{
 */

/**
 * @brief Global exception handler type.
 */
typedef void (*exception_handler_t)(BSP_Exception_frame*);

/**
 * @brief Default global exception handler.
 */
void C_exception_handler(BSP_Exception_frame* excPtr);

void BSP_printStackTrace(const BSP_Exception_frame *excPtr);

/**
 * @brief Exception categories.
 *
 * Exceptions of different categories use different SRR registers to save the
 * machine state and do different things in the prologue and epilogue.
 *
 * For now, the CPU descriptions assume this fits into 8 bits.
 */
typedef enum {
  PPC_EXC_INVALID = 0,
  PPC_EXC_ASYNC = 1,
  PPC_EXC_CLASSIC = 2,
  PPC_EXC_CLASSIC_ASYNC = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  PPC_EXC_405_CRITICAL = 4,
  PPC_EXC_405_CRITICAL_ASYNC = PPC_EXC_405_CRITICAL | PPC_EXC_ASYNC,
  PPC_EXC_BOOKE_CRITICAL = 6,
  PPC_EXC_BOOKE_CRITICAL_ASYNC = PPC_EXC_BOOKE_CRITICAL | PPC_EXC_ASYNC,
  PPC_EXC_E500_MACHCHK  = 8,
  PPC_EXC_E500_MACHCHK_ASYNC = PPC_EXC_E500_MACHCHK | PPC_EXC_ASYNC,
  PPC_EXC_NAKED = 10
} ppc_exc_category;

/**
 * @brief Categorie set type.
 */
typedef uint8_t ppc_exc_categories [LAST_VALID_EXC + 1];

static inline bool ppc_exc_is_valid_category(ppc_exc_category category)
{
  return (unsigned) category <= (unsigned) PPC_EXC_NAKED;
}

/**
 * @brief Returns the entry address of the vector.
 *
 * @param[in] vector The vector number.
 * @param[in] vector_base The vector table base address.
 */
void *ppc_exc_vector_address(unsigned vector, void *vector_base);

/**
 * @brief Returns the category set for a CPU of type @a cpu, or @c NULL if
 * there is no category set available for this CPU.
 */
const ppc_exc_categories *ppc_exc_categories_for_cpu(ppc_cpu_id_t cpu);

/**
 * @brief Returns the category set for the current CPU, or @c NULL if there is
 * no category set available for this CPU.
 */
static inline const ppc_exc_categories *ppc_exc_current_categories(void)
{
  return ppc_exc_categories_for_cpu(ppc_cpu_current());
}

/**
 * @brief Returns the category for the vector @a vector using the category set
 * @a categories.
 */
ppc_exc_category ppc_exc_category_for_vector(
  const ppc_exc_categories *categories,
  unsigned vector
);

/**
 * @brief Makes a minimal prologue for the vector @a vector with the category
 * @a category.
 *
 * The minimal prologue will be copied to @a prologue.  Not more than
 * @a prologue_size bytes will be copied.  Returns the actual minimal prologue
 * size in bytes in @a prologue_size.
 *
 * @retval RTEMS_SUCCESSFUL Minimal prologue successfully made.
 * @retval RTEMS_INVALID_ID Invalid vector number.
 * @retval RTEMS_INVALID_NUMBER Invalid category.
 * @retval RTEMS_INVALID_SIZE Prologue size to small.
 */
rtems_status_code ppc_exc_make_prologue(
  unsigned vector,
  void *vector_base,
  ppc_exc_category category,
  uint32_t *prologue,
  size_t *prologue_size
);

/**
 * @brief Initializes the exception handling.
 *
 * @see ppc_exc_initialize().
 */
void ppc_exc_initialize_with_vector_base(
  uint32_t interrupt_disable_mask,
  uintptr_t interrupt_stack_begin,
  uintptr_t interrupt_stack_size,
  void *vector_base
);

/**
 * @brief Initializes the exception handling.
 *
 * If the initialization fails, then this is a fatal error.  The fatal error
 * source is RTEMS_FATAL_SOURCE_BSP_GENERIC and the fatal error code is
 * BSP_GENERIC_FATAL_EXCEPTION_INITIALIZATION.
 *
 * Possible error reasons are
 * - no category set available for the current CPU,
 * - the register r13 does not point to the small data area anchor required by
 *   SVR4/EABI, or
 * - the minimal prologue creation failed.
 */
static inline void ppc_exc_initialize(
  uint32_t interrupt_disable_mask,
  uintptr_t interrupt_stack_begin,
  uintptr_t interrupt_stack_size
)
{
  ppc_exc_initialize_with_vector_base(
    interrupt_disable_mask,
    interrupt_stack_begin,
    interrupt_stack_size,
    NULL
  );
}

/**
 * @brief High-level exception handler type.
 *
 * @retval 0 The exception was handled and normal execution may resume.
 * @retval -1 Reject the exception resulting in a call of the global exception
 * handler.
 * @retval other Reserved, do not use.
 */
typedef int (*ppc_exc_handler_t)(BSP_Exception_frame *f, unsigned vector);

/**
 * @brief Default high-level exception handler.
 *
 * @retval -1 Always.
 */
int ppc_exc_handler_default(BSP_Exception_frame *f, unsigned int vector);

#ifndef PPC_EXC_CONFIG_BOOKE_ONLY

/**
 * @brief Bits for MSR update.
 *
 * Bits in MSR that are enabled during execution of exception handlers / ISRs
 * (on classic PPC these are DR/IR/RI [default], on bookE-style CPUs they should
 * be set to 0 during initialization)
 *
 * By default, the setting of these bits that is in effect when exception
 * handling is initialized is used.
 */
extern uint32_t ppc_exc_msr_bits;

#endif /* PPC_EXC_CONFIG_BOOKE_ONLY */

/**
 * @brief Cache write back check flag.
 *
 * (See README under CAVEATS). During initialization
 * a check is performed to assert that write-back
 * caching is enabled for memory accesses. If a BSP
 * runs entirely without any caching then it should
 * set this variable to zero prior to initializing
 * exceptions in order to skip the test.
 * NOTE: The code does NOT support mapping memory
 *       with cache-attributes other than write-back
 *       (unless the entire cache is physically disabled)
 */
extern uint32_t ppc_exc_cache_wb_check;

#ifndef PPC_EXC_CONFIG_USE_FIXED_HANDLER
  /**
   * @brief High-level exception handler table.
   */
  extern ppc_exc_handler_t ppc_exc_handler_table [LAST_VALID_EXC + 1];

  /**
   * @brief Global exception handler.
   */
  extern exception_handler_t globalExceptHdl;
#else /* PPC_EXC_CONFIG_USE_FIXED_HANDLER */
  /**
   * @brief High-level exception handler table.
   */
  extern const ppc_exc_handler_t ppc_exc_handler_table [LAST_VALID_EXC + 1];

  /**
   * @brief Interrupt dispatch routine provided by BSP.
   */
  void bsp_interrupt_dispatch(void);
#endif /* PPC_EXC_CONFIG_USE_FIXED_HANDLER */

/**
 * @brief Set high-level exception handler.
 *
 * Hook C exception handlers.
 *  - handlers for asynchronous exceptions run on the ISR stack
 *    with thread-dispatching disabled.
 *  - handlers for synchronous exceptions run on the task stack
 *    with thread-dispatching enabled.
 *
 * If a particular slot is NULL then the traditional 'globalExcHdl' is used.
 *
 * ppc_exc_set_handler() registers a handler (returning 0 on success,
 * -1 if the vector argument is too big).
 *
 * It is legal to set a NULL handler. This leads to the globalExcHdl
 * being called if an exception for 'vector' occurs.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ID Invalid vector number.
 * @retval RTEMS_RESOURCE_IN_USE Handler table is read-only and handler does
 * not match.
 */
rtems_status_code ppc_exc_set_handler(unsigned vector, ppc_exc_handler_t hdl);

/**
 * @brief Returns the currently active high-level exception handler.
 */
ppc_exc_handler_t ppc_exc_get_handler(unsigned vector);

/**
 * @brief Function for DAR access.
 *
 * CPU support may store the address of a function here
 * that can be used by the default exception handler to
 * obtain fault-address info which is helpful. Unfortunately,
 * the SPR holding this information is not uniform
 * across PPC families so we need assistance from
 * CPU support
 */
extern uint32_t (*ppc_exc_get_DAR)(void);

void
ppc_exc_wrapup(BSP_Exception_frame *f);

/**
 * @brief Standard aligment handler.
 *
 * @retval 0 Performed a dcbz instruction.
 * @retval -1 Otherwise.
 */
int ppc_exc_alignment_handler(BSP_Exception_frame *frame, unsigned excNum);

/** @} */

/*
 * Compatibility with pc386
 */
typedef exception_handler_t cpuExcHandlerType;

#endif /* ASM */

#ifdef __cplusplus
}
#endif

#endif /* LIBCPU_VECTORS_H */
