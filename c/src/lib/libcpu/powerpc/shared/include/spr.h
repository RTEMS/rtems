/*
 *  spr.h -- Access to special purpose registers.
 *
 *  Copyright (C) 1998 Gabriel Paubert, paubert@iram.es
 *
 *  Modified to compile in RTEMS development environment
 *  by Eric Valette
 *
 *  Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */


#ifndef _LIBCPU_SPR_H
#define _LIBCPU_SPR_H

#include <rtems/powerpc/registers.h>

#define __MFSPR(reg, val) \
	__asm__ __volatile__("mfspr %0,"#reg : "=r" (val))

#define __MTSPR(val, reg) \
	__asm__ __volatile__("mtspr "#reg",%0" : : "r" (val))


#define SPR_RW(reg) \
static inline unsigned long _read_##reg(void) \
{\
	unsigned long val;\
	__MFSPR(reg, val);\
	return val;\
}\
static inline void _write_##reg(unsigned long val)\
{\
	__MTSPR(val,reg);\
	return;\
}

#define SPR_RO(reg) \
static inline unsigned long _read_##reg(void) \
{\
	unsigned long val;\
	__MFSPR(reg,val);\
	return val;\
}

static inline unsigned long _read_MSR(void)
{
	unsigned long val;
	asm volatile("mfmsr %0" : "=r" (val));
	return val;
}

static inline void _write_MSR(unsigned long val)
{
	asm volatile("mtmsr %0" : : "r" (val));
	return;
}

static inline unsigned long _read_SR(void * va)
{
	unsigned long val;
	asm volatile("mfsrin %0,%1" : "=r" (val): "r" (va));
	return val;
}

static inline void _write_SR(unsigned long val, void * va)
{
	asm volatile("mtsrin %0,%1" : : "r"(val), "r" (va): "memory");
	return;
}


#endif
