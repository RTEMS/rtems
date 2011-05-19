/**
 * @file
 *
 * @ingroup powerpc_shared
 *
 * @brief General purpose assembler macros, linker command file support and
 * some inline functions for direct register access.
 */

/*
 * Copyright (c) 2008, 2010, 2011
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * access function for Device Control Registers inspired by "ppc405common.h"
 * from Michael Hamel ADInstruments May 2008
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

/**
 * @defgroup powerpc_shared Shared PowerPC Code
 */

#ifndef __LIBCPU_POWERPC_UTILITY_H
#define __LIBCPU_POWERPC_UTILITY_H

#if !defined(ASM)
  #include <rtems.h>
#endif

#include <rtems/score/cpu.h>
#include <rtems/powerpc/registers.h>
#include <rtems/powerpc/powerpc.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(ASM)

#include <rtems/bspIo.h>
#include <rtems/system.h>

#include <libcpu/cpuIdent.h>

#define LINKER_SYMBOL(sym) extern char sym [];

/**
 * @brief Read one byte from @a src.
 */
static inline uint8_t ppc_read_byte(const volatile void *src)
{
  uint8_t value;

  __asm__ volatile (
    "lbz %0, 0(%1)"
    : "=r" (value)
    : "b" (src)
  );

  return value;
}

/**
 * @brief Read one half word from @a src.
 */
static inline uint16_t ppc_read_half_word(const volatile void *src)
{
  uint16_t value;

  __asm__ volatile (
    "lhz %0, 0(%1)"
    : "=r" (value)
    : "b" (src)
  );

  return value;
}

/**
 * @brief Read one word from @a src.
 */
static inline uint32_t ppc_read_word(const volatile void *src)
{
  uint32_t value;

  __asm__ volatile (
    "lwz %0, 0(%1)"
    : "=r" (value)
    : "b" (src)
  );

  return value;
}

/**
 * @brief Write one byte @a value to @a dest.
 */
static inline void ppc_write_byte(uint8_t value, volatile void *dest)
{
  __asm__ volatile (
    "stb %0, 0(%1)"
    :
    : "r" (value), "b" (dest)
  );
}

/**
 * @brief Write one half word @a value to @a dest.
 */
static inline void ppc_write_half_word(uint16_t value, volatile void *dest)
{
  __asm__ volatile (
    "sth %0, 0(%1)"
    :
    : "r" (value), "b" (dest)
  );
}

/**
 * @brief Write one word @a value to @a dest.
 */
static inline void ppc_write_word(uint32_t value, volatile void *dest)
{
  __asm__ volatile (
    "stw %0, 0(%1)" :
    : "r" (value), "b" (dest)
  );
}


static inline void *ppc_stack_pointer(void)
{
  void *sp;

  __asm__ volatile (
    "mr %0, 1"
    : "=r" (sp)
  );

  return sp;
}

static inline void ppc_set_stack_pointer(void *sp)
{
  __asm__ volatile (
    "mr 1, %0"
    :
    : "r" (sp)
  );
}

static inline void *ppc_link_register(void)
{
  void *lr;

  __asm__ volatile (
    "mflr %0"
    : "=r" (lr)
  );

  return lr;
}

static inline void ppc_set_link_register(void *lr)
{
  __asm__ volatile (
    "mtlr %0"
    :
    : "r" (lr)
  );
}

static inline uint32_t ppc_machine_state_register(void)
{
  uint32_t msr;

  __asm__ volatile (
    "mfmsr %0"
    : "=r" (msr)
  );

  return msr;
}

static inline void ppc_set_machine_state_register(uint32_t msr)
{
  __asm__ volatile (
    "mtmsr %0"
    :
    : "r" (msr)
  );
}

static inline void ppc_synchronize_data(void)
{
  RTEMS_COMPILER_MEMORY_BARRIER();

  __asm__ volatile ("sync");
}

static inline void ppc_synchronize_instructions(void)
{
  RTEMS_COMPILER_MEMORY_BARRIER();

  __asm__ volatile ("isync");
}

static inline void ppc_enforce_in_order_execution_of_io(void)
{
  RTEMS_COMPILER_MEMORY_BARRIER();

  __asm__ volatile ("eieio");
}

static inline void ppc_data_cache_block_flush(void *addr)
{
  __asm__ volatile (
    "dcbf 0, %0"
    :
    : "r" (addr)
    : "memory"
  );
}

static inline void ppc_data_cache_block_flush_2(
  void *base,
  uintptr_t offset
)
{
  __asm__ volatile (
    "dcbf %0, %1"
    :
    : "b" (base), "r" (offset)
    : "memory"
  );
}

static inline void ppc_data_cache_block_invalidate(void *addr)
{
  __asm__ volatile (
    "dcbi 0, %0"
    :
    : "r" (addr)
    : "memory"
  );
}

static inline void ppc_data_cache_block_invalidate_2(
  void *base,
  uintptr_t offset
)
{
  __asm__ volatile (
    "dcbi %0, %1"
    :
    : "b" (base), "r" (offset)
    : "memory"
  );
}

static inline void ppc_data_cache_block_store(const void *addr)
{
  __asm__ volatile (
    "dcbst 0, %0"
    :
    : "r" (addr)
  );
}

static inline void ppc_data_cache_block_store_2(
  const void *base,
  uintptr_t offset
)
{
  __asm__ volatile (
    "dcbst %0, %1"
    :
    : "b" (base), "r" (offset)
  );
}

static inline void ppc_data_cache_block_touch(const void *addr)
{
  __asm__ volatile (
    "dcbt 0, %0"
    :
    : "r" (addr)
  );
}

static inline void ppc_data_cache_block_touch_2(
  const void *base,
  uintptr_t offset
)
{
  __asm__ volatile (
    "dcbt %0, %1"
    :
    : "b" (base), "r" (offset)
  );
}

static inline void ppc_data_cache_block_touch_for_store(const void *addr)
{
  __asm__ volatile (
    "dcbtst 0, %0"
    :
    : "r" (addr)
  );
}

static inline void ppc_data_cache_block_touch_for_store_2(
  const void *base,
  uintptr_t offset
)
{
  __asm__ volatile (
    "dcbtst %0, %1"
    :
    : "b" (base), "r" (offset)
  );
}

static inline void ppc_data_cache_block_clear_to_zero(void *addr)
{
  __asm__ volatile (
    "dcbz 0, %0"
    :
    : "r" (addr)
    : "memory"
  );
}

static inline void ppc_data_cache_block_clear_to_zero_2(
  void *base,
  uintptr_t offset
)
{
  __asm__ volatile (
    "dcbz %0, %1"
    :
    : "b" (base), "r" (offset)
    : "memory"
  );
}

static inline void ppc_instruction_cache_block_invalidate(void *addr)
{
  __asm__ volatile (
    "icbi 0, %0"
    :
    : "r" (addr)
  );
}

static inline void ppc_instruction_cache_block_invalidate_2(
  void *base,
  uintptr_t offset
)
{
  __asm__ volatile (
    "icbi %0, %1"
    :
    : "b" (base), "r" (offset)
  );
}

/**
 * @brief Enables external exceptions.
 *
 * You can use this function to enable the external exceptions and restore the
 * machine state with ppc_external_exceptions_disable() later.
 */
static inline uint32_t ppc_external_exceptions_enable(void)
{
  uint32_t current_msr;
  uint32_t new_msr;

  RTEMS_COMPILER_MEMORY_BARRIER();

  __asm__ volatile (
    "mfmsr %0;"
    "ori %1, %0, 0x8000;"
    "mtmsr %1"
    : "=r" (current_msr), "=r" (new_msr)
  );

  return current_msr;
}

/**
 * @brief Restores machine state.
 *
 * @see ppc_external_exceptions_enable()
 */
static inline void ppc_external_exceptions_disable(uint32_t msr)
{
  ppc_set_machine_state_register(msr);

  RTEMS_COMPILER_MEMORY_BARRIER();
}

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */

#if defined(mpx8xx) || defined(mpc860) || defined(mpc821)
/* Wonderful bookE doesn't have mftb/mftbu; they only
 * define the TBRU/TBRL SPRs so we use these. Luckily,
 * we run in supervisory mode so that should work on
 * all CPUs. In user mode we'd have a problem...
 * 2007/11/30, T.S.
 *
 * OTOH, PSIM currently lacks support for reading
 * SPRs 268/269. You need GDB patch sim/2376 to avoid
 * a crash...
 * OTOH, the MPC8xx do not allow to read the timebase registers via mfspr.
 * we NEED a mftb to access the time base.
 * 2009/10/30 Th. D.
 */
#define CPU_Get_timebase_low( _value ) \
    __asm__ volatile( "mftb  %0" : "=r" (_value) )
#else
#define CPU_Get_timebase_low( _value ) \
    __asm__ volatile( "mfspr %0,268" : "=r" (_value) )
#endif

/* Must be provided for rtems_bsp_delay to work */
extern     uint32_t bsp_clicks_per_usec;

#define rtems_bsp_delay( _microseconds ) \
  do { \
    uint32_t   start, ticks, now; \
    CPU_Get_timebase_low( start ) ; \
    ticks = (_microseconds) * bsp_clicks_per_usec; \
    do \
      CPU_Get_timebase_low( now ) ; \
    while (now - start < ticks); \
  } while (0)

#define rtems_bsp_delay_in_bus_cycles( _cycles ) \
  do { \
    uint32_t   start, now; \
    CPU_Get_timebase_low( start ); \
    do \
      CPU_Get_timebase_low( now ); \
    while (now - start < (_cycles)); \
  } while (0)

/*
 *  Routines to access the decrementer register
 */

#define PPC_Set_decrementer( _clicks ) \
  do { \
    __asm__ volatile( "mtdec %0" : : "r" ((_clicks)) ); \
  } while (0)

#define PPC_Get_decrementer( _clicks ) \
    __asm__ volatile( "mfdec  %0" : "=r" (_clicks) )

/*
 *  Routines to access the time base register
 */

static inline uint64_t PPC_Get_timebase_register( void )
{
  uint32_t tbr_low;
  uint32_t tbr_high;
  uint32_t tbr_high_old;
  uint64_t tbr;

  do {
#if defined(mpx8xx) || defined(mpc860) || defined(mpc821)
/* See comment above (CPU_Get_timebase_low) */
    __asm__ volatile( "mftbu %0" : "=r" (tbr_high_old));
    __asm__ volatile( "mftb  %0" : "=r" (tbr_low));
    __asm__ volatile( "mftbu %0" : "=r" (tbr_high));
#else
    __asm__ volatile( "mfspr %0, 269" : "=r" (tbr_high_old));
    __asm__ volatile( "mfspr %0, 268" : "=r" (tbr_low));
    __asm__ volatile( "mfspr %0, 269" : "=r" (tbr_high));
#endif
  } while ( tbr_high_old != tbr_high );

  tbr = tbr_high;
  tbr <<= 32;
  tbr |= tbr_low;
  return tbr;
}

static inline  void PPC_Set_timebase_register (uint64_t tbr)
{
  uint32_t tbr_low;
  uint32_t tbr_high;

  tbr_low = (uint32_t) tbr;
  tbr_high = (uint32_t) (tbr >> 32);
  __asm__ volatile( "mtspr 284, %0" : : "r" (tbr_low));
  __asm__ volatile( "mtspr 285, %0" : : "r" (tbr_high));

}

static inline uint32_t ppc_decrementer_register(void)
{
  uint32_t dec;

  PPC_Get_decrementer(dec);

  return dec;
}

static inline void ppc_set_decrementer_register(uint32_t dec)
{
  PPC_Set_decrementer(dec);
}

/**
 * @brief Preprocessor magic for stringification of @a x.
 */
#define PPC_STRINGOF(x) #x

/**
 * @brief Returns the value of the Special Purpose Register with number @a spr.
 *
 * @note This macro uses a GNU C extension.
 */
#define PPC_SPECIAL_PURPOSE_REGISTER(spr) \
  ({ \
    uint32_t val; \
    __asm__ volatile (\
      "mfspr %0, " PPC_STRINGOF(spr) \
      : "=r" (val) \
    ); \
    val;\
  } )

/**
 * @brief Sets the Special Purpose Register with number @a spr to the value in
 * @a val.
 */
#define PPC_SET_SPECIAL_PURPOSE_REGISTER(spr, val) \
  do { \
    __asm__ volatile (\
      "mtspr " PPC_STRINGOF(spr) ", %0" \
      : \
      : "r" (val) \
    ); \
  } while (0)

/**
 * @brief Sets in the Special Purpose Register with number @a spr all bits
 * which are set in @a bits.
 *
 * Interrupts are disabled throughout this operation.
 */
#define PPC_SET_SPECIAL_PURPOSE_REGISTER_BITS(spr, bits) \
  do { \
    rtems_interrupt_level level; \
    uint32_t val; \
    uint32_t mybits = bits; \
    rtems_interrupt_disable(level); \
    val = PPC_SPECIAL_PURPOSE_REGISTER(spr); \
    val |= mybits; \
    PPC_SET_SPECIAL_PURPOSE_REGISTER(spr, val); \
    rtems_interrupt_enable(level); \
  } while (0)

/**
 * @brief Sets in the Special Purpose Register with number @a spr all bits
 * which are set in @a bits.  The previous register value will be masked with
 * @a mask.
 *
 * Interrupts are disabled throughout this operation.
 */
#define PPC_SET_SPECIAL_PURPOSE_REGISTER_BITS_MASKED(spr, bits, mask) \
  do { \
    rtems_interrupt_level level; \
    uint32_t val; \
    uint32_t mybits = bits; \
    uint32_t mymask = mask; \
    rtems_interrupt_disable(level); \
    val = PPC_SPECIAL_PURPOSE_REGISTER(spr); \
    val &= ~mymask; \
    val |= mybits; \
    PPC_SET_SPECIAL_PURPOSE_REGISTER(spr, val); \
    rtems_interrupt_enable(level); \
  } while (0)

/**
 * @brief Clears in the Special Purpose Register with number @a spr all bits
 * which are set in @a bits.
 *
 * Interrupts are disabled throughout this operation.
 */
#define PPC_CLEAR_SPECIAL_PURPOSE_REGISTER_BITS(spr, bits) \
  do { \
    rtems_interrupt_level level; \
    uint32_t val; \
    uint32_t mybits = bits; \
    rtems_interrupt_disable(level); \
    val = PPC_SPECIAL_PURPOSE_REGISTER(spr); \
    val &= ~mybits; \
    PPC_SET_SPECIAL_PURPOSE_REGISTER(spr, val); \
    rtems_interrupt_enable(level); \
  } while (0)

/**
 * @brief Returns the value of the Device Control Register with number @a dcr.
 *
 * The PowerPC 4XX family has Device Control Registers.
 *
 * @note This macro uses a GNU C extension.
 */
#define PPC_DEVICE_CONTROL_REGISTER(dcr) \
  ({ \
    uint32_t val; \
    __asm__ volatile (\
      "mfdcr %0, " PPC_STRINGOF(dcr) \
      : "=r" (val) \
    ); \
    val;\
  } )

/**
 * @brief Sets the Device Control Register with number @a dcr to the value in
 * @a val.
 *
 * The PowerPC 4XX family has Device Control Registers.
 */
#define PPC_SET_DEVICE_CONTROL_REGISTER(dcr, val) \
  do { \
    __asm__ volatile (\
      "mtdcr " PPC_STRINGOF(dcr) ", %0" \
      : \
      : "r" (val) \
    ); \
  } while (0)

/**
 * @brief Sets in the Device Control Register with number @a dcr all bits
 * which are set in @a bits.
 *
 * Interrupts are disabled throughout this operation.
 */
#define PPC_SET_DEVICE_CONTROL_REGISTER_BITS(dcr, bits) \
  do { \
    rtems_interrupt_level level; \
    uint32_t val; \
    uint32_t mybits = bits; \
    rtems_interrupt_disable(level); \
    val = PPC_DEVICE_CONTROL_REGISTER(dcr); \
    val |= mybits; \
    PPC_SET_DEVICE_CONTROL_REGISTER(dcr, val); \
    rtems_interrupt_enable(level); \
  } while (0)

/**
 * @brief Sets in the Device Control Register with number @a dcr all bits
 * which are set in @a bits.  The previous register value will be masked with
 * @a mask.
 *
 * Interrupts are disabled throughout this operation.
 */
#define PPC_SET_DEVICE_CONTROL_REGISTER_BITS_MASKED(dcr, bits, mask) \
  do { \
    rtems_interrupt_level level; \
    uint32_t val; \
    uint32_t mybits = bits; \
    uint32_t mymask = mask; \
    rtems_interrupt_disable(level); \
    val = PPC_DEVICE_CONTROL_REGISTER(dcr); \
    val &= ~mymask; \
    val |= mybits; \
    PPC_SET_DEVICE_CONTROL_REGISTER(dcr, val); \
    rtems_interrupt_enable(level); \
  } while (0)

/**
 * @brief Clears in the Device Control Register with number @a dcr all bits
 * which are set in @a bits.
 *
 * Interrupts are disabled throughout this operation.
 */
#define PPC_CLEAR_DEVICE_CONTROL_REGISTER_BITS(dcr, bits) \
  do { \
    rtems_interrupt_level level; \
    uint32_t val; \
    uint32_t mybits = bits; \
    rtems_interrupt_disable(level); \
    val = PPC_DEVICE_CONTROL_REGISTER(dcr); \
    val &= ~mybits; \
    PPC_SET_DEVICE_CONTROL_REGISTER(dcr, val); \
    rtems_interrupt_enable(level); \
  } while (0)

static inline uint32_t ppc_time_base(void)
{
  uint32_t val;

  CPU_Get_timebase_low(val);

  return val;
}

static inline void ppc_set_time_base(uint32_t val)
{
  PPC_SET_SPECIAL_PURPOSE_REGISTER(TBWL, val);
}

static inline uint32_t ppc_time_base_upper(void)
{
  return PPC_SPECIAL_PURPOSE_REGISTER(TBRU);
}

static inline void ppc_set_time_base_upper(uint32_t val)
{
  PPC_SET_SPECIAL_PURPOSE_REGISTER(TBWU, val);
}

static inline uint64_t ppc_time_base_64(void)
{
  return PPC_Get_timebase_register();
}

static inline void ppc_set_time_base_64(uint64_t val)
{
  PPC_Set_timebase_register(val);
}

static inline uint32_t ppc_alternate_time_base(void)
{
  return PPC_SPECIAL_PURPOSE_REGISTER(FSL_EIS_ATBL);
}

static inline uint32_t ppc_alternate_time_base_upper(void)
{
  return PPC_SPECIAL_PURPOSE_REGISTER(FSL_EIS_ATBU);
}

static inline uint64_t ppc_alternate_time_base_64(void)
{
  uint32_t atbl;
  uint32_t atbu_0;
  uint32_t atbu_1;

  do {
    atbu_0 = ppc_alternate_time_base_upper();
    atbl = ppc_alternate_time_base();
    atbu_1 = ppc_alternate_time_base_upper();
  } while (atbu_0 != atbu_1);

  return (((uint64_t) atbu_1) << 32) | ((uint64_t) atbl);
}

static inline uint32_t ppc_processor_id(void)
{
  return PPC_SPECIAL_PURPOSE_REGISTER(BOOKE_PIR);
}

static inline void ppc_set_processor_id(uint32_t val)
{
  PPC_SET_SPECIAL_PURPOSE_REGISTER(BOOKE_PIR, val);
}

static inline uint32_t ppc_fsl_system_version(void)
{
  return PPC_SPECIAL_PURPOSE_REGISTER(FSL_EIS_SVR);
}

static inline uint32_t ppc_fsl_system_version_cid(uint32_t svr)
{
  return (svr >> 28) & 0xf;
}

static inline uint32_t ppc_fsl_system_version_sid(uint32_t svr)
{
  return (svr >> 16) & 0xfff;
}

static inline uint32_t ppc_fsl_system_version_proc(uint32_t svr)
{
  return (svr >> 12) & 0xf;
}

static inline uint32_t ppc_fsl_system_version_mfg(uint32_t svr)
{
  return (svr >> 8) & 0xf;
}

static inline uint32_t ppc_fsl_system_version_mjrev(uint32_t svr)
{
  return (svr >> 4) & 0xf;
}

static inline uint32_t ppc_fsl_system_version_mnrev(uint32_t svr)
{
  return (svr >> 0) & 0xf;
}

void ppc_code_copy(void *dest, const void *src, size_t n);

#endif /* ifndef ASM */

#if defined(ASM)
#include <rtems/asm.h>

.macro LA reg, addr
	lis	\reg, (\addr)@h
	ori	\reg, \reg, (\addr)@l
.endm

.macro LWI reg, value
	lis \reg, (\value)@h
	ori	\reg, \reg, (\value)@l
.endm

.macro LW reg, addr
	lis	\reg, \addr@ha
	lwz	\reg, \addr@l(\reg)
.endm

/*
 * Tests the bits in reg1 against the bits set in mask.  A match is indicated
 * by EQ = 0 in CR0.  A mismatch is indicated by EQ = 1 in CR0.  The register
 * reg2 is used to load the mask.
 */
.macro	TSTBITS	reg1, reg2, mask
	LWI 	\reg2, \mask
	and	\reg1, \reg1, \reg2
	cmplw	\reg1, \reg2
.endm

.macro 	SETBITS reg1, reg2, mask
	LWI 	\reg2, \mask
	or	\reg1, \reg1, \reg2
.endm

.macro 	CLRBITS reg1, reg2, mask
	LWI 	\reg2, \mask
	andc	\reg1, \reg1, \reg2
.endm

.macro GLOBAL_FUNCTION name
	.global \name
	.type \name, @function
\name:
.endm

/*
 * Obtain interrupt mask
 */
.macro GET_INTERRUPT_MASK mask
	mfspr	\mask, sprg0
.endm

/*
 * Disables all asynchronous exeptions (interrupts) which may cause a context
 * switch.
 */
.macro INTERRUPT_DISABLE level, mask
	mfmsr	\level
	GET_INTERRUPT_MASK mask=\mask
	andc	\mask, \level, \mask
	mtmsr	\mask
.endm

/*
 * Restore previous machine state.
 */
.macro INTERRUPT_ENABLE level
	mtmsr	\level
.endm

#define LINKER_SYMBOL(sym) .extern sym

#endif /* ASM */

#ifdef __cplusplus
}
#endif

#endif /* __LIBCPU_POWERPC_UTILITY_H */
