/*
 * byteorder.h
 *
 *        This file contains inline implementation of function to
 *          deal with endian conversion.
 *
 * It is a stripped down version of linux ppc file...
 *
 * Copyright (C) 1999  Eric Valette (valette@crf.canon.fr)
 *                     Canon Centre Recherche France.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef _PPC_BYTEORDER_H
#define _PPC_BYTEORDER_H

#ifdef __GNUC__

extern __inline__ unsigned ld_le16(volatile unsigned short *addr)
{
	unsigned val;

	__asm__ __volatile__ ("lhbrx %0,0,%1" : "=r" (val) : "r" (addr), "m" (*addr));
	return val;
}

extern __inline__ void st_le16(volatile unsigned short *addr, unsigned val)
{
	__asm__ __volatile__ ("sthbrx %1,0,%2" : "=m" (*addr) : "r" (val), "r" (addr));
}

extern __inline__ unsigned ld_le32(volatile unsigned *addr)
{
	unsigned val;

	__asm__ __volatile__ ("lwbrx %0,0,%1" : "=r" (val) : "r" (addr), "m" (*addr));
	return val;
}

extern __inline__ void st_le32(volatile unsigned *addr, unsigned val)
{
	__asm__ __volatile__ ("stwbrx %1,0,%2" : "=m" (*addr) : "r" (val), "r" (addr));
}

/* alas, egcs sounds like it has a bug in this code that doesn't use the
   inline asm correctly, and can cause file corruption. Until I hear that
   it's fixed, I can live without the extra speed. I hope. */
#if !(__GNUC__ >= 2 && __GNUC_MINOR__ >= 90)
#if 0
#  define __arch_swab16(x) ld_le16(&x)
#  define __arch_swab32(x) ld_le32(&x)
#else
static __inline__ __const__ unsigned short ___arch__swab16(unsigned short value)
{
	unsigned int tmp;

	__asm__("rlwimi %0,%0,8,0xff0000"
	    : "=r" (tmp)
	    : "0" (value));
	return (tmp&0x00ffff00)>>8;
}

static __inline__ __const__ unsigned int ___arch__swab32(unsigned int value)
{
	unsigned int result;

	__asm__("rotlwi %0,%1,24\n\t"
	    "rlwimi %0,%1,8,0xff\n\t"
	    "rlwimi %0,%1,8,0xff0000"
	    : "=&r" (result)
	    : "r" (value));
	return result;
}
#define __arch__swab32(x) ___arch__swab32(x)
#define __arch__swab16(x) ___arch__swab16(x)
#endif /* 0 */

#endif

/* The same, but returns converted value from the location pointer by addr. */
#define __arch__swab16p(addr) ld_le16(addr)
#define __arch__swab32p(addr) ld_le32(addr)

/* The same, but do the conversion in situ, ie. put the value back to addr. */
#define __arch__swab16s(addr) st_le16(addr,*addr)
#define __arch__swab32s(addr) st_le32(addr,*addr)

#endif /* __GNUC__ */

#endif /* _PPC_BYTEORDER_H */
