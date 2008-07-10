/**
 * @file
 *
 * @ingroup powerpc_shared
 *
 * @brief General purpose assembler macros, linker command file support and
 * some inline functions for direct register access.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

/**
 * @defgroup powerpc_shared Shared PowerPC Code
 */

#ifndef LIBCPU_POWERPC_UTILITY_H
#define LIBCPU_POWERPC_UTILITY_H

#include <rtems/powerpc/registers.h>

#ifdef ASM

#include <rtems/asm.h>

.macro LA reg, addr
	lis \reg, (\addr)@h
	ori \reg, \reg, (\addr)@l
.endm

.macro LWI reg, value
	lis \reg, (\value)@h
	ori \reg, \reg, (\value)@l
.endm

.macro LW reg, addr
	lis \reg, \addr@ha
	lwz \reg, \addr@l(\reg)
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
 * Disables all asynchronous exeptions (interrupts) which may cause a context
 * switch.
 */
.macro INTERRUPT_DISABLE level, mask
	mfmsr	\level
	mfspr	\mask, sprg0
	andc	\mask, \level, \mask
	mtmsr	\mask
.endm

/*
 * Restore previous machine state.
 */
.macro INTERRUPT_ENABLE level
	mtmsr	\level
.endm

#define LINKER_SYMBOL( sym) .extern sym

#else /* ASM */

#include <stdint.h>

#include <rtems/bspIo.h>
#include <rtems/system.h>
#include <rtems/score/cpu.h>

#include <libcpu/cpuIdent.h>

#define LINKER_SYMBOL( sym) extern char sym []

/**
 * @brief Read one byte from @a src.
 */
static inline uint8_t ppc_read_byte( const volatile void *src)
{
	uint8_t value;

	asm volatile (
		"lbz %0, 0(%1)"
		: "=r" (value)
		: "r" (src)
	);

	return value;
}

/**
 * @brief Read one half word from @a src.
 */
static inline uint16_t ppc_read_half_word( const volatile void *src)
{
	uint16_t value;

	asm volatile (
		"lhz %0, 0(%1)"
		: "=r" (value)
		: "r" (src)
	);

	return value;
}

/**
 * @brief Read one word from @a src.
 */
static inline uint32_t ppc_read_word( const volatile void *src)
{
	uint32_t value;

	asm volatile (
		"lwz %0, 0(%1)"
		: "=r" (value)
		: "r" (src)
	);

	return value;
}

/**
 * @brief Write one byte @a value to @a dest.
 */
static inline void ppc_write_byte( uint8_t value, volatile void *dest)
{
	asm volatile (
		"stb %0, 0(%1)"
		:
		: "r" (value), "r" (dest)
	);
}

/**
 * @brief Write one half word @a value to @a dest.
 */
static inline void ppc_write_half_word( uint16_t value, volatile void *dest)
{
	asm volatile (
		"sth %0, 0(%1)"
		:
		: "r" (value), "r" (dest)
	);
}

/**
 * @brief Write one word @a value to @a dest.
 */
static inline void ppc_write_word( uint32_t value, volatile void *dest)
{
	asm volatile (
		"stw %0, 0(%1)" :
		: "r" (value), "r" (dest)
	);
}

static inline void *ppc_stack_pointer()
{
	void *sp;

	asm volatile (
		"mr %0, 1"
		: "=r" (sp)
	);

	return sp;
}

static inline void ppc_set_stack_pointer( void *sp)
{
	asm volatile (
		"mr 1, %0"
		:
		: "r" (sp)
	);
}

static inline void *ppc_link_register()
{
	void *lr;

	asm volatile (
		"mflr %0"
		: "=r" (lr)
	);

	return lr;
}

static inline void ppc_set_link_register( void *lr)
{
	asm volatile (
		"mtlr %0"
		:
		: "r" (lr)
	);
}

static inline uint32_t ppc_machine_state_register()
{
	uint32_t msr;

	asm volatile (
		"mfmsr %0"
		: "=r" (msr)
	);

	return msr;
}

static inline void ppc_set_machine_state_register( uint32_t msr)
{
	asm volatile (
		"mtmsr %0"
		:
		: "r" (msr)
	);
}

/**
 * @brief Enables external exceptions.
 * 
 * You can use this function to enable the external exceptions and restore the
 * machine state with ppc_external_exceptions_disable() later.
 */
static inline uint32_t ppc_external_exceptions_enable()
{
	uint32_t current_msr;
	uint32_t new_msr;

	RTEMS_COMPILER_MEMORY_BARRIER();

	asm volatile (
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
static inline void ppc_external_exceptions_disable( uint32_t msr)
{
	ppc_set_machine_state_register( msr);

	RTEMS_COMPILER_MEMORY_BARRIER();
}

static inline uint32_t ppc_decrementer_register()
{
	uint32_t dec;

	PPC_Get_decrementer( dec);

	return dec;
}

static inline void ppc_set_decrementer_register( uint32_t dec)
{
	PPC_Set_decrementer( dec);
}

/* Do not use the following macros.  Use the inline functions instead. */

#define PPC_INTERNAL_MACRO_RETURN_SPECIAL_PURPOSE_REGISTER( spr) \
	uint32_t val; \
	asm volatile ( \
		"mfspr %0, " #spr \
		: "=r" (val) \
	); \
	return val;

#define PPC_INTERNAL_MACRO_RETURN_SPECIAL_PURPOSE_REGISTER_EXPAND( spr) \
	PPC_INTERNAL_MACRO_RETURN_SPECIAL_PURPOSE_REGISTER( spr)

#define PPC_INTERNAL_MACRO_SET_SPECIAL_PURPOSE_REGISTER( spr, val) \
	asm volatile ( \
		"mtspr " #spr ", %0" \
		: \
		: "r" (val) \
	);

#define PPC_INTERNAL_MACRO_SET_SPECIAL_PURPOSE_REGISTER_EXPAND( spr, val) \
	PPC_INTERNAL_MACRO_SET_SPECIAL_PURPOSE_REGISTER( spr, val)

static inline uint32_t ppc_special_purpose_register_0()
{
	PPC_INTERNAL_MACRO_RETURN_SPECIAL_PURPOSE_REGISTER_EXPAND( SPRG0);
}

static inline void ppc_set_special_purpose_register_0( uint32_t val)
{
	PPC_INTERNAL_MACRO_SET_SPECIAL_PURPOSE_REGISTER_EXPAND( SPRG0, val);
}

static inline uint32_t ppc_special_purpose_register_1()
{
	PPC_INTERNAL_MACRO_RETURN_SPECIAL_PURPOSE_REGISTER_EXPAND( SPRG1);
}

static inline void ppc_set_special_purpose_register_1( uint32_t val)
{
	PPC_INTERNAL_MACRO_SET_SPECIAL_PURPOSE_REGISTER_EXPAND( SPRG1, val);
}

static inline uint32_t ppc_special_purpose_register_2()
{
	PPC_INTERNAL_MACRO_RETURN_SPECIAL_PURPOSE_REGISTER_EXPAND( SPRG2);
}

static inline void ppc_set_special_purpose_register_2( uint32_t val)
{
	PPC_INTERNAL_MACRO_SET_SPECIAL_PURPOSE_REGISTER_EXPAND( SPRG2, val);
}

static inline uint32_t ppc_special_purpose_register_3()
{
	PPC_INTERNAL_MACRO_RETURN_SPECIAL_PURPOSE_REGISTER_EXPAND( SPRG3);
}

static inline void ppc_set_special_purpose_register_3( uint32_t val)
{
	PPC_INTERNAL_MACRO_SET_SPECIAL_PURPOSE_REGISTER_EXPAND( SPRG3, val);
}

static inline uint32_t ppc_special_purpose_register_4()
{
	PPC_INTERNAL_MACRO_RETURN_SPECIAL_PURPOSE_REGISTER_EXPAND( SPRG4);
}

static inline void ppc_set_special_purpose_register_4( uint32_t val)
{
	PPC_INTERNAL_MACRO_SET_SPECIAL_PURPOSE_REGISTER_EXPAND( SPRG4, val);
}

static inline uint32_t ppc_special_purpose_register_5()
{
	PPC_INTERNAL_MACRO_RETURN_SPECIAL_PURPOSE_REGISTER_EXPAND( SPRG5);
}

static inline void ppc_set_special_purpose_register_5( uint32_t val)
{
	PPC_INTERNAL_MACRO_SET_SPECIAL_PURPOSE_REGISTER_EXPAND( SPRG5, val);
}

static inline uint32_t ppc_special_purpose_register_6()
{
	PPC_INTERNAL_MACRO_RETURN_SPECIAL_PURPOSE_REGISTER_EXPAND( SPRG6);
}

static inline void ppc_set_special_purpose_register_6( uint32_t val)
{
	PPC_INTERNAL_MACRO_SET_SPECIAL_PURPOSE_REGISTER_EXPAND( SPRG6, val);
}

static inline uint32_t ppc_special_purpose_register_7()
{
	PPC_INTERNAL_MACRO_RETURN_SPECIAL_PURPOSE_REGISTER_EXPAND( SPRG7);
}

static inline void ppc_set_special_purpose_register_7( uint32_t val)
{
	PPC_INTERNAL_MACRO_SET_SPECIAL_PURPOSE_REGISTER_EXPAND( SPRG7, val);
}

static inline uint32_t ppc_user_special_purpose_register_0()
{
	PPC_INTERNAL_MACRO_RETURN_SPECIAL_PURPOSE_REGISTER_EXPAND( USPRG0);
}

static inline void ppc_set_user_special_purpose_register_0( uint32_t val)
{
	PPC_INTERNAL_MACRO_SET_SPECIAL_PURPOSE_REGISTER_EXPAND( USPRG0, val);
}

static inline uint32_t ppc_timer_control_register()
{
	PPC_INTERNAL_MACRO_RETURN_SPECIAL_PURPOSE_REGISTER_EXPAND( BOOKE_TCR);
}

static inline void ppc_set_timer_control_register( uint32_t val)
{
	PPC_INTERNAL_MACRO_SET_SPECIAL_PURPOSE_REGISTER_EXPAND( BOOKE_TCR, val);
}

static inline uint32_t ppc_timer_status_register()
{
	PPC_INTERNAL_MACRO_RETURN_SPECIAL_PURPOSE_REGISTER_EXPAND( BOOKE_TSR);
}

static inline void ppc_set_timer_status_register( uint32_t val)
{
	PPC_INTERNAL_MACRO_SET_SPECIAL_PURPOSE_REGISTER_EXPAND( BOOKE_TSR, val);
}

static inline uint32_t ppc_decrementer_auto_reload_register()
{
	PPC_INTERNAL_MACRO_RETURN_SPECIAL_PURPOSE_REGISTER_EXPAND( BOOKE_DECAR);
}

static inline void ppc_set_decrementer_auto_reload_register( uint32_t val)
{
	PPC_INTERNAL_MACRO_SET_SPECIAL_PURPOSE_REGISTER_EXPAND( BOOKE_DECAR, val);
}

static inline uint32_t ppc_hardware_implementation_dependent_register_0()
{
	PPC_INTERNAL_MACRO_RETURN_SPECIAL_PURPOSE_REGISTER_EXPAND( HID0);
}

static inline void ppc_set_hardware_implementation_dependent_register_0( uint32_t val)
{
	PPC_INTERNAL_MACRO_SET_SPECIAL_PURPOSE_REGISTER_EXPAND( HID0, val);
}

static inline uint32_t ppc_hardware_implementation_dependent_register_1()
{
	PPC_INTERNAL_MACRO_RETURN_SPECIAL_PURPOSE_REGISTER_EXPAND( HID1);
}

static inline void ppc_set_hardware_implementation_dependent_register_1( uint32_t val)
{
	PPC_INTERNAL_MACRO_SET_SPECIAL_PURPOSE_REGISTER_EXPAND( HID1, val);
}

static inline uint32_t ppc_time_base()
{
	uint32_t val;

	CPU_Get_timebase_low( val);

	return val;
}

static inline void ppc_set_time_base( uint32_t val)
{
	PPC_INTERNAL_MACRO_SET_SPECIAL_PURPOSE_REGISTER_EXPAND( TBWL, val);
}

static inline uint32_t ppc_time_base_upper()
{
	PPC_INTERNAL_MACRO_RETURN_SPECIAL_PURPOSE_REGISTER_EXPAND( TBRU);
}

static inline void ppc_set_time_base_upper( uint32_t val)
{
	PPC_INTERNAL_MACRO_SET_SPECIAL_PURPOSE_REGISTER_EXPAND( TBWU, val);
}

static inline uint64_t ppc_time_base_64()
{
	return PPC_Get_timebase_register();
}

static inline void ppc_set_time_base_64( uint64_t val)
{
	PPC_Set_timebase_register( val);
}

#endif /* ASM */

#endif /* LIBCPU_POWERPC_UTILITY_H */
