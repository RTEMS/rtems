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
 *  http://www.rtems.com/license/LICENSE.
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

/* NOTE: The use of these macros is DISCOURAGED.
 *       you should consider e.g. using in_xxx / out_xxx
 *       with a device specific base address that is
 *       defined by the BSP. This makes drivers easier
 *       to port.
 */
#define inb(port)		in_8((unsigned char *)((port)+_IO_BASE))
#define outb(val, port)		out_8((unsigned char *)((port)+_IO_BASE), (val))
#define inw(port)		in_le16((unsigned short *)((port)+_IO_BASE))
#define outw(val, port)		out_le16((unsigned short *)((port)+_IO_BASE), (val))
#define inl(port)		in_le32((unsigned *)((port)+_IO_BASE))
#define outl(val, port)		out_le32((unsigned *)((port)+_IO_BASE), (val))

/*
 * Enforce In-order Execution of I/O:
 * Acts as a barrier to ensure all previous I/O accesses have
 * completed before any further ones are issued.
 */
static inline void eieio(void)
{
	__asm__ __volatile__ ("eieio");
}


/* Enforce in-order execution of data I/O.
 * No distinction between read/write on PPC; use eieio for all three.
 */
#define iobarrier_rw() eieio()
#define iobarrier_r()  eieio()
#define iobarrier_w()  eieio()

/*
 * 8, 16 and 32 bit, big and little endian I/O operations, with barrier.
 */
static inline int in_8(volatile unsigned char *addr)
{
	int ret;

	__asm__ __volatile__("lbz%U1%X1 %0,%1; eieio" : "=r" (ret) : "m" (*addr));
	return ret;
}

static inline void out_8(volatile unsigned char *addr, int val)
{
	__asm__ __volatile__("stb%U0%X0 %1,%0; eieio" : "=m" (*addr) : "r" (val));
}

static inline int in_le16(volatile unsigned short *addr)
{
	int ret;

	__asm__ __volatile__("lhbrx %0,0,%1; eieio" : "=r" (ret) :
			      "r" (addr), "m" (*addr));
	return ret;
}

static inline int in_be16(volatile unsigned short *addr)
{
	int ret;

	__asm__ __volatile__("lhz%U1%X1 %0,%1; eieio" : "=r" (ret) : "m" (*addr));
	return ret;
}

static inline void out_le16(volatile unsigned short *addr, int val)
{
	__asm__ __volatile__("sthbrx %1,0,%2; eieio" : "=m" (*addr) :
			      "r" (val), "r" (addr));
}

static inline void out_be16(volatile unsigned short *addr, int val)
{
	__asm__ __volatile__("sth%U0%X0 %1,%0; eieio" : "=m" (*addr) : "r" (val));
}

static inline unsigned in_le32(volatile unsigned *addr)
{
	unsigned ret;

	__asm__ __volatile__("lwbrx %0,0,%1; eieio" : "=r" (ret) :
			     "r" (addr), "m" (*addr));
	return ret;
}

static inline unsigned in_be32(volatile unsigned *addr)
{
	unsigned ret;

	__asm__ __volatile__("lwz%U1%X1 %0,%1; eieio" : "=r" (ret) : "m" (*addr));
	return ret;
}

static inline void out_le32(volatile unsigned *addr, int val)
{
	__asm__ __volatile__("stwbrx %1,0,%2; eieio" : "=m" (*addr) :
			     "r" (val), "r" (addr));
}

static inline void out_be32(volatile unsigned *addr, int val)
{
	__asm__ __volatile__("stw%U0%X0 %1,%0; eieio" : "=m" (*addr) : "r" (val));
}

#endif /* ASM */
#endif /* _LIBCPU_IO_H */
