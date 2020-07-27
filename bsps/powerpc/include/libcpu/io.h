/*
 * io.h
 *
 *	    This file contains inline implementation of function to
 *          deal with IO.
 *
 * It is a stripped down version of linux ppc file...
 *
 * Copyright (C) 1999  Eric Valette (valette@crf.canon.fr)
 *                     Canon Centre Recherche France.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */
#ifndef _LIBCPU_IO_H
#define _LIBCPU_IO_H


#define PREP_ISA_IO_BASE 	0x80000000
#define PREP_ISA_MEM_BASE 	0xc0000000
#define PREP_PCI_DRAM_OFFSET 	0x80000000

#define CHRP_ISA_IO_BASE	0xfe000000
#define CHRP_ISA_MEM_BASE	0xfd000000
#define CHRP_PCI_DRAM_OFFSET	0x00000000

/* _IO_BASE, _ISA_MEM_BASE, PCI_DRAM_OFFSET are now defined by bsp.h */

#ifndef ASM

#include <bsp.h>		/* for _IO_BASE & friends */
#include <stdint.h>

/* NOTE: The use of these macros is DISCOURAGED.
 *       you should consider e.g. using in_xxx / out_xxx
 *       with a device specific base address that is
 *       defined by the BSP. This makes drivers easier
 *       to port.
 */
#define inb(port)		in_8((uint8_t *)((port)+_IO_BASE))
#define outb(val, port)		out_8((uint8_t *)((port)+_IO_BASE), (val))
#define inw(port)		in_le16((uint16_t *)((port)+_IO_BASE))
#define outw(val, port)		out_le16((uint16_t *)((port)+_IO_BASE), (val))
#define inl(port)		in_le32((uint32_t *)((port)+_IO_BASE))
#define outl(val, port)		out_le32((uint32_t *)((port)+_IO_BASE), (val))

/*
 * Enforce In-order Execution of I/O:
 * Acts as a barrier to ensure all previous I/O accesses have
 * completed before any further ones are issued.
 */
static inline void io_eieio(void)
{
	__asm__ __volatile__ ("eieio");
}


/* Enforce in-order execution of data I/O.
 * No distinction between read/write on PPC; use eieio for all three.
 */
#define iobarrier_rw() io_eieio()
#define iobarrier_r()  io_eieio()
#define iobarrier_w()  io_eieio()

/*
 * 8, 16 and 32 bit, big and little endian I/O operations, with barrier.
 */
static inline uint8_t in_8(const volatile uint8_t *addr)
{
	uint8_t ret;

	__asm__ __volatile__("lbz%U1%X1 %0,%1; eieio" : "=r" (ret) : "m" (*addr));
	return ret;
}

static inline void out_8(volatile uint8_t *addr, uint8_t val)
{
	__asm__ __volatile__("stb%U0%X0 %1,%0; eieio" : "=m" (*addr) : "r" (val));
}

static inline uint16_t in_le16(const volatile uint16_t *addr)
{
	uint16_t ret;

	__asm__ __volatile__("lhbrx %0,0,%1; eieio" : "=r" (ret) :
			      "r" (addr), "m" (*addr));
	return ret;
}

static inline uint16_t in_be16(const volatile uint16_t *addr)
{
	uint16_t ret;

	__asm__ __volatile__("lhz%U1%X1 %0,%1; eieio" : "=r" (ret) : "m" (*addr));
	return ret;
}

static inline void out_le16(volatile uint16_t *addr, uint16_t val)
{
	__asm__ __volatile__("sthbrx %1,0,%2; eieio" : "=m" (*addr) :
			      "r" (val), "r" (addr));
}

static inline void out_be16(volatile uint16_t *addr, uint16_t val)
{
	__asm__ __volatile__("sth%U0%X0 %1,%0; eieio" : "=m" (*addr) : "r" (val));
}

static inline uint32_t in_le32(const volatile uint32_t *addr)
{
	uint32_t ret;

	__asm__ __volatile__("lwbrx %0,0,%1; eieio" : "=r" (ret) :
			     "r" (addr), "m" (*addr));
	return ret;
}

static inline uint32_t in_be32(const volatile uint32_t *addr)
{
	uint32_t ret;

	__asm__ __volatile__("lwz%U1%X1 %0,%1; eieio" : "=r" (ret) : "m" (*addr));
	return ret;
}

static inline void out_le32(volatile uint32_t *addr, uint32_t val)
{
	__asm__ __volatile__("stwbrx %1,0,%2; eieio" : "=m" (*addr) :
			     "r" (val), "r" (addr));
}

static inline void out_be32(volatile uint32_t *addr, uint32_t val)
{
	__asm__ __volatile__("stw%U0%X0 %1,%0; eieio" : "=m" (*addr) : "r" (val));
}

#endif /* ASM */
#endif /* _LIBCPU_IO_H */
