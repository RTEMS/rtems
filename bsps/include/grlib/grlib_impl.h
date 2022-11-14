/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2017 Cobham Gaisler AB
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef GRLIB_IMPL_H
#define GRLIB_IMPL_H

#include <rtems/score/basedefs.h>
#include <rtems/malloc.h>

/*
 * Use interrupt lock primitives compatible with SMP defined in RTEMS 4.11.99
 * and higher.
 */
#if (((__RTEMS_MAJOR__ << 16) | (__RTEMS_MINOR__ << 8) | __RTEMS_REVISION__) >= 0x040b63)

#include <rtems/score/isrlock.h>

/* map via rtems_interrupt_lock_* API: */
#define SPIN_DECLARE(lock) RTEMS_INTERRUPT_LOCK_MEMBER(lock)
#define SPIN_INIT(lock, name) rtems_interrupt_lock_initialize(lock, name)
#define SPIN_LOCK(lock, level) rtems_interrupt_lock_acquire_isr(lock, &level)
#define SPIN_LOCK_IRQ(lock, level) rtems_interrupt_lock_acquire(lock, &level)
#define SPIN_UNLOCK(lock, level) rtems_interrupt_lock_release_isr(lock, &level)
#define SPIN_UNLOCK_IRQ(lock, level) rtems_interrupt_lock_release(lock, &level)
#define SPIN_IRQFLAGS(k) rtems_interrupt_lock_context k
#define SPIN_ISR_IRQFLAGS(k) SPIN_IRQFLAGS(k)
#define SPIN_FREE(lock) rtems_interrupt_lock_destroy(lock)

/* turn on/off local CPU's interrupt to ensure HW timing - not SMP safe. */
#define IRQ_LOCAL_DECLARE(_level) rtems_interrupt_level _level
#define IRQ_LOCAL_DISABLE(_level) rtems_interrupt_local_disable(_level)
#define IRQ_LOCAL_ENABLE(_level) rtems_interrupt_local_enable(_level)

#else

#ifdef RTEMS_SMP
#error SMP mode not compatible with these interrupt lock primitives
#endif

/* maintain single-core compatibility with older versions of RTEMS: */
#define SPIN_DECLARE(name)
#define SPIN_INIT(lock, name)
#define SPIN_LOCK(lock, level)
#define SPIN_LOCK_IRQ(lock, level) rtems_interrupt_disable(level)
#define SPIN_UNLOCK(lock, level)
#define SPIN_UNLOCK_IRQ(lock, level) rtems_interrupt_enable(level)
#define SPIN_IRQFLAGS(k) rtems_interrupt_level k
#define SPIN_ISR_IRQFLAGS(k)
#define SPIN_FREE(lock)

/* turn on/off local CPU's interrupt to ensure HW timing - not SMP safe. */
#define IRQ_LOCAL_DECLARE(_level) rtems_interrupt_level _level
#define IRQ_LOCAL_DISABLE(_level) rtems_interrupt_disable(_level)
#define IRQ_LOCAL_ENABLE(_level) rtems_interrupt_enable(_level)

#endif

#ifdef __cplusplus
extern "C" {
#endif

#if (((__RTEMS_MAJOR__ << 16) | (__RTEMS_MINOR__ << 8) | __RTEMS_REVISION__) >= 0x050000)

static inline void *grlib_malloc(size_t size)
{
 return rtems_malloc(size);
}

static inline void *grlib_calloc(size_t nelem, size_t elsize)
{
 return rtems_calloc(nelem, elsize);
}

#else

static inline void *grlib_malloc(size_t size)
{
 return malloc(size);
}

static inline void *grlib_calloc(size_t nelem, size_t elsize)
{
 return calloc(nelem, elsize);
}

#endif

#ifdef __sparc__

static inline unsigned char grlib_read_uncached8(unsigned int address)
{
       unsigned char tmp;
       __asm__ (" lduba [%1]1, %0 "
           : "=r"(tmp)
           : "r"(address)
       );
       return tmp;
}

static inline unsigned short grlib_read_uncached16(unsigned int addr) {
       unsigned short tmp;
       __asm__ (" lduha [%1]1, %0 "
         : "=r"(tmp)
         : "r"(addr)
       );
       return tmp;
}


static inline unsigned int grlib_read_uncached32(unsigned int address)
{
	unsigned int tmp;
	__asm__ (" lda [%1]1, %0 "
	        : "=r"(tmp)
	        : "r"(address)
	);
	return tmp;
}

static inline uint64_t grlib_read_uncached64(uint64_t *address)
{
	uint64_t tmp;
	__asm__ (" ldda [%1]1, %0 "
	        : "=r"(tmp)
	        : "r"(address)
	);
	return tmp;
}

#define GRLIB_DMA_IS_CACHE_COHERENT CPU_SPARC_HAS_SNOOPING

#else

static unsigned char __inline__ grlib_read_uncached8(unsigned int address)
{
	unsigned char tmp = (*(volatile unsigned char *)(address));
	return tmp;
}

static __inline__ unsigned short grlib_read_uncached16(unsigned int address) {
	unsigned short tmp = (*(volatile unsigned short *)(address));
	return tmp;
}

static inline unsigned int grlib_read_uncached32(unsigned int address)
{
	unsigned int tmp = (*(volatile unsigned int *)(address));
	return tmp;
}

#define GRLIB_DMA_IS_CACHE_COHERENT 1

#endif

#ifdef __cplusplus
}
#endif

#endif /* GRLIB_IMPL_H */
