/*
 *  include/asm-ppc/spr.h -- Access to special purpose registers.
 *
 *  Copyright (C) 1998 Gabriel Paubert, paubert@iram.es
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License.  See the file COPYING in the main directory of this archive
 *  for more details.
 */


#ifndef _PPC_SPR_H
#define _PPC_SPR_H

#include <libcpu/cpu.h> 

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
