/*
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	from tahoe:	in_cksum.c	1.2	86/01/05
 *	from:		@(#)in_cksum.c	1.3 (Berkeley) 1/19/91
 *	from: Id: in_cksum.c,v 1.8 1995/12/03 18:35:19 bde Exp
 */

#ifndef _MACHINE_IN_CKSUM_H_
#define	_MACHINE_IN_CKSUM_H_	1

#include <sys/cdefs.h>
#include <netinet/ip.h> /* struct ip */

/*
 * It it useful to have an Internet checksum routine which is inlineable
 * and optimized specifically for the task of computing IP header checksums
 * in the normal case (where there are no options and the header length is
 * therefore always exactly five 32-bit words.
 */

/*
 *  Optimized version for the i386 family
 */

#if (defined(__GNUC__) && defined(__i386__))

static __inline u_int
in_cksum_hdr(const struct ip *ip)
{
	register u_int sum = ((const uint32_t*)ip)[0];
	register u_int tmp;

	__asm__ __volatile__(
				"	addl %2, %0    \n"
				"	adcl %3, %0    \n"
				"	adcl %4, %0    \n"
				"	adcl %5, %0    \n"
				"	adcl $0, %0    \n"
				"	movl %0, %1    \n"
				"	roll $16, %0   \n"
				"	addl %1, %0    \n"
				:"+&r"(sum),"=&r"(tmp)
				:"g"(((const uint32_t*)ip)[1]),
				 "g"(((const uint32_t*)ip)[2]),
				 "g"(((const uint32_t*)ip)[3]),
				 "g"(((const uint32_t*)ip)[4]),
				 "m"(*ip)
				:"cc"
	);
		    
	return (~sum) >>16;
}

static __inline void
in_cksum_update(struct ip *ip)
{
	int __tmpsum;
	__tmpsum = (int)ntohs(ip->ip_sum) + 256;
	ip->ip_sum = htons(__tmpsum + (__tmpsum >> 16));
}

/*
 *  Optimized version for the MC68xxx and Coldfire families
 */

#elif (defined(__GNUC__) && (defined(__mc68000__) || defined(__m68k__)))

static __inline__ u_int
in_cksum_hdr(const struct ip *ip)
{
	register u_int *ap = (u_int *)ip;
	register u_int sum = *ap++;
	register u_int tmp;
		    
	__asm__ __volatile__("addl  %2@+,%0\n\t"
	        "movel %2@+,%1\n\t"
	        "addxl %1,%0\n\t"
	        "movel %2@+,%1\n\t"
	        "addxl %1,%0\n\t"
	        "movel %2@,%1\n\t"
	        "addxl %1,%0\n\t"
	        "moveq #0,%1\n\t"
	        "addxl %1,%0\n" :
		"=d" (sum), "=d" (tmp), "=a" (ap) :
		"0" (sum), "2" (ap), "m"(*ip));
	sum = (sum & 0xffff) + (sum >> 16);
	if (sum > 0xffff)
		sum -= 0xffff;
	return ~sum & 0xffff;
}

/*
 *  Optimized version for the PowerPC family
 */

#elif (defined(__GNUC__) && (defined(__PPC__) || defined(__ppc__)))

static __inline u_int
in_cksum_hdr(const struct ip *ip)
{
register u_int sum, tmp;
	__asm__ __volatile__(
		"	lwz   %0,  0(%2) \n"
		"	lwz   %1,  4(%2) \n"
		"	addc  %0, %0, %1 \n"	/* generate carry (XER[CA]) */
		"	lwz   %1,  8(%2) \n"
		"	adde  %0, %0, %1 \n"	/* add + generate           */
		"	lwz   %1, 12(%2) \n"
		"	adde  %0, %0, %1 \n"
		"	lwz   %1, 16(%2) \n"
		"	adde  %0, %0, %1 \n"
		"	addze %0, %0     \n"	/* mop up XER[CA]           */
		"	rotlwi %1, %0,16 \n"	/* word-swapped copy in %1  */
		"	add   %0, %0, %1 \n"    /* see comment below        */
		"	not   %0, %0     \n"
		"	srwi  %0, %0, 16 \n"
		:"=&r"(sum),"=&r"(tmp):"b"(ip), "m"(*ip):"xer"
	);
	/* Note: if 'add' generates a carry out of the lower 16 bits
	 *       then this is automatically added to the upper 16 bits
	 *       where the correct result is found. (Stolen from linux.)
	 *        %0 : upper-word  lower-word
	 *     +  %1 : lower-word  upper-word
	 *     =       word-sum    word-sum
	 *                     ^+inter-word-carry
	 */
	return sum;
}

static __inline void
in_cksum_update(struct ip *ip)
{
        int __tmpsum;
        __tmpsum = (int)ntohs(ip->ip_sum) + 256;
        ip->ip_sum = htons(__tmpsum + (__tmpsum >> 16));
}

/*
 *  SPARC Version
 */

#elif (defined(__GNUC__) && defined(sparc))

static __inline u_int
in_cksum_hdr(const struct ip *ip)
{
   register u_int sum = 0;
   register u_int tmp_o2;
   register u_int tmp_o3;

   __asm__ __volatile__ (" \
     ld [%0], %1 ; \
     ld [%0+4], %2 ; \
     ld [%0+8], %3 ; \
     addcc %1, %2, %1 ; \
     ld [%0+12], %2 ; \
     addxcc %1, %3, %1 ; \
     ld [%0+16], %3 ; \
     addxcc %1, %2, %1 ; \
     addxcc %1, %3, %1 ; \
     set 0x0ffff, %3 ; \
     srl %1, 16, %2 ; \
     and %1, %3, %1 ; \
     addx %1, %2, %1 ; \
     srl %1, 16, %2 ; \
     add %1, %2, %1 ; \
     not %1 ; \
     and %1, %3, %1 ; \
    " : "=r" (ip), "=r" (sum), "=r" (tmp_o2), "=r" (tmp_o3)
      : "0" (ip), "1" (sum), "m"(*ip)
  );
  return sum;
}

#define	in_cksum_update(ip) \
	do { \
		int __tmpsum; \
		__tmpsum = (int)ntohs(ip->ip_sum) + 256; \
		ip->ip_sum = htons(__tmpsum + (__tmpsum >> 16)); \
	} while(0)
/*
 *  Here is the generic, portable, inefficient algorithm.
 */

#else
u_int in_cksum_hdr(const struct ip *);
#define	in_cksum_update(ip) \
	do { \
		int __tmpsum; \
		__tmpsum = (int)ntohs(ip->ip_sum) + 256; \
		ip->ip_sum = htons(__tmpsum + (__tmpsum >> 16)); \
	} while(0)

#endif

#endif /* _MACHINE_IN_CKSUM_H_ */
