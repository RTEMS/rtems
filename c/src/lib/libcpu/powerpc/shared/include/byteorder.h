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
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _LIBCPU_BYTEORDER_H
#define _LIBCPU_BYTEORDER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__

extern __inline__ unsigned ld_le16(volatile uint16_t *addr)
{
	unsigned val;

	__asm__ __volatile__ ("lhbrx %0,0,%1" : "=r" (val) : "r" (addr), "m" (*addr));
	return val;
}

extern __inline__ void st_le16(volatile uint16_t *addr, unsigned val)
{
	__asm__ __volatile__ ("sthbrx %1,0,%2" : "=m" (*addr) : "r" (val), "r" (addr));
}

extern __inline__ unsigned ld_le32(volatile uint32_t *addr)
{
	unsigned val;

	__asm__ __volatile__ ("lwbrx %0,0,%1" : "=r" (val) : "r" (addr), "m" (*addr));
	return val;
}

extern __inline__ void st_le32(volatile uint32_t *addr, unsigned val)
{
	__asm__ __volatile__ ("stwbrx %1,0,%2" : "=m" (*addr) : "r" (val), "r" (addr));
}

#endif /* __GNUC__ */

#ifdef __cplusplus
}
#endif

#endif /* _LIBCPU_BYTEORDER_H */
