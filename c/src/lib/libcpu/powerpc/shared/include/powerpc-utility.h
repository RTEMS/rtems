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
 * access function for Device Control Registers inspired by "ppc405common.h"
 * from Michael Hamel ADInstruments May 2008
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

#ifndef ASM

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

/**
 * @brief Preprocessor magic for stringification of @a x.
 */
#define PPC_STRINGOF( x) #x

/**
 * @brief Returns the value of the Special Purpose Register with number @a spr.
 *
 * @note This macro uses a GNU C extension.
 */
#define PPC_SPECIAL_PURPOSE_REGISTER( spr) \
	( { \
		uint32_t val; \
		asm volatile ( \
			"mfspr %0, " PPC_STRINGOF( spr) \
			: "=r" (val) \
		); \
		val;\
	} )

/**
 * @brief Sets the Special Purpose Register with number @a spr to the value in
 * @a val.
 */
#define PPC_SET_SPECIAL_PURPOSE_REGISTER( spr, val) \
	do { \
		asm volatile ( \
			"mtspr " PPC_STRINGOF( spr) ", %0" \
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
#define PPC_SET_SPECIAL_PURPOSE_REGISTER_BITS( spr, bits) \
	do { \
		rtems_interrupt_level level; \
		uint32_t val; \
		uint32_t mybits = bits; \
		rtems_interrupt_disable( level); \
		val = PPC_SPECIAL_PURPOSE_REGISTER( spr); \
		val |= mybits; \
		PPC_SET_SPECIAL_PURPOSE_REGISTER( spr, val); \
		rtems_interrupt_enable( level); \
	} while (0)

/**
 * @brief Sets in the Special Purpose Register with number @a spr all bits
 * which are set in @a bits.  The previous register value will be masked with
 * @a mask.
 *
 * Interrupts are disabled throughout this operation.
 */
#define PPC_SET_SPECIAL_PURPOSE_REGISTER_BITS_MASKED( spr, bits, mask) \
	do { \
		rtems_interrupt_level level; \
		uint32_t val; \
		uint32_t mybits = bits; \
		uint32_t mymask = mask; \
		rtems_interrupt_disable( level); \
		val = PPC_SPECIAL_PURPOSE_REGISTER( spr); \
		val &= ~mymask; \
		val |= mybits; \
		PPC_SET_SPECIAL_PURPOSE_REGISTER( spr, val); \
		rtems_interrupt_enable( level); \
	} while (0)

/**
 * @brief Clears in the Special Purpose Register with number @a spr all bits
 * which are set in @a bits.
 *
 * Interrupts are disabled throughout this operation.
 */
#define PPC_CLEAR_SPECIAL_PURPOSE_REGISTER_BITS( spr, bits) \
	do { \
		rtems_interrupt_level level; \
		uint32_t val; \
		uint32_t mybits = bits; \
		rtems_interrupt_disable( level); \
		val = PPC_SPECIAL_PURPOSE_REGISTER( spr); \
		val &= ~mybits; \
		PPC_SET_SPECIAL_PURPOSE_REGISTER( spr, val); \
		rtems_interrupt_enable( level); \
	} while (0)

/**
 * @brief Returns the value of the Device Control Register with number @a dcr.
 *
 * The PowerPC 4XX family has Device Control Registers.
 *
 * @note This macro uses a GNU C extension.
 */
#define PPC_DEVICE_CONTROL_REGISTER( dcr) \
	( { \
		uint32_t val; \
		asm volatile ( \
			"mfdcr %0, " PPC_STRINGOF( dcr) \
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
#define PPC_SET_DEVICE_CONTROL_REGISTER( dcr, val) \
	do { \
		asm volatile ( \
			"mtdcr " PPC_STRINGOF( dcr) ", %0" \
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
#define PPC_SET_DEVICE_CONTROL_REGISTER_BITS( dcr, bits) \
	do { \
		rtems_interrupt_level level; \
		uint32_t val; \
		uint32_t mybits = bits; \
		rtems_interrupt_disable( level); \
		val = PPC_DEVICE_CONTROL_REGISTER( dcr); \
		val |= mybits; \
		PPC_SET_DEVICE_CONTROL_REGISTER( dcr, val); \
		rtems_interrupt_enable( level); \
	} while (0)

/**
 * @brief Sets in the Device Control Register with number @a dcr all bits
 * which are set in @a bits.  The previous register value will be masked with
 * @a mask.
 *
 * Interrupts are disabled throughout this operation.
 */
#define PPC_SET_DEVICE_CONTROL_REGISTER_BITS_MASKED( dcr, bits, mask) \
	do { \
		rtems_interrupt_level level; \
		uint32_t val; \
		uint32_t mybits = bits; \
		uint32_t mymask = mask; \
		rtems_interrupt_disable( level); \
		val = PPC_DEVICE_CONTROL_REGISTER( dcr); \
		val &= ~mymask; \
		val |= mybits; \
		PPC_SET_DEVICE_CONTROL_REGISTER( dcr, val); \
		rtems_interrupt_enable( level); \
	} while (0)

/**
 * @brief Clears in the Device Control Register with number @a dcr all bits
 * which are set in @a bits.
 *
 * Interrupts are disabled throughout this operation.
 */
#define PPC_CLEAR_DEVICE_CONTROL_REGISTER_BITS( dcr, bits) \
	do { \
		rtems_interrupt_level level; \
		uint32_t val; \
		uint32_t mybits = bits; \
		rtems_interrupt_disable( level); \
		val = PPC_DEVICE_CONTROL_REGISTER( dcr); \
		val &= ~mybits; \
		PPC_SET_DEVICE_CONTROL_REGISTER( dcr, val); \
		rtems_interrupt_enable( level); \
	} while (0)

static inline uint32_t ppc_time_base()
{
	uint32_t val;

	CPU_Get_timebase_low( val);

	return val;
}

static inline void ppc_set_time_base( uint32_t val)
{
	PPC_SET_SPECIAL_PURPOSE_REGISTER( TBWL, val);
}

static inline uint32_t ppc_time_base_upper()
{
	return PPC_SPECIAL_PURPOSE_REGISTER( TBRU);
}

static inline void ppc_set_time_base_upper( uint32_t val)
{
	PPC_SET_SPECIAL_PURPOSE_REGISTER( TBWU, val);
}

static inline uint64_t ppc_time_base_64()
{
	return PPC_Get_timebase_register();
}

static inline void ppc_set_time_base_64( uint64_t val)
{
	PPC_Set_timebase_register( val);
}

#else /* ASM */

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

#define LINKER_SYMBOL( sym) .extern sym

#endif /* ASM */

#endif /* LIBCPU_POWERPC_UTILITY_H */
