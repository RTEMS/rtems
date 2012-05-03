/*
 * Copyright (c) 1988, 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
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
 *	@(#)in_cksum.c	8.1 (Berkeley) 6/10/93
 */

#include <sys/param.h>
#include <sys/mbuf.h>

#if defined(__mcoldfire__)
# define IS_COLDFIRE 1
#else
# define IS_COLDFIRE 0
#endif

#define REDUCE { sum = (sum & 0xFFFF) + (sum >> 16); if (sum > 0xFFFF) sum -= 0xFFFF; }

/*
 * Motorola 68k version of Internet Protocol Checksum routine
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * August, 1998
 */
int
in_cksum( struct mbuf *m,
	int len )
{
	unsigned short *w;
	unsigned long sum = 0;
	int mlen = 0;
	int byte_swapped = 0;
	union {
		char	c[2];
		u_short	s;
	} s_util;

	for ( ; m && len ; m = m->m_next) {
		if (m->m_len == 0)
			continue;
		w = mtod(m, u_short *);
		if (mlen == -1) {
			/*
			 * The first byte of this mbuf is the continuation
			 * of a word spanning between this mbuf and the
			 * last mbuf.
			 *
			 * s_util.c[0] is already saved when scanning previous
			 * mbuf.
			 */
			s_util.c[1] = *(char *)w;
			sum += s_util.s;
			w = (u_short *)((char *)w + 1);
			mlen = m->m_len - 1;
			len--;
		} else
			mlen = m->m_len;
		if (len < mlen)
			mlen = len;
		len -= mlen;

		/*
		 * Force to longword boundary.
		 */
		if (3 & (int)w) {
			REDUCE;
			if ((1 & (int) w) && (mlen > 0)) {
				sum <<= 8;
				s_util.c[0] = *(u_char *)w;
				w = (u_short *)((char *)w + 1);
				mlen--;
				byte_swapped = 1;
			}
			if ((2 & (int) w) && (mlen >= 2)) {
				sum += *w++;
				mlen -= 2;
			}
		}

		/*
		 * Sum all the longwords in the buffer.
		 * See RFC 1071 -- Computing the Internet Checksum.
		 * It should work for all 68k family members.
		 */
		{
		unsigned long tcnt = mlen, t1;
		__asm__ volatile (
		"movel   %2,%3\n\t"
		"lsrl    #6,%2       | count/64 = # loop traversals\n\t"
		"andl    #0x3c,%3    | Then find fractions of a chunk\n\t"
		"negl    %3\n\t      | Each long uses 4 instruction bytes\n\t"
#if IS_COLDFIRE
		"addql   #1,%2       | Clear X (extended carry flag)\n\t"
		"subql   #1,%2       | \n\t"
#else
		"andi    #0xf,%%cc   | Clear X (extended carry flag)\n\t"
#endif
		"jmp     %%pc@(2f-.-2:b,%3)  | Jump into loop\n"
		"1:                  | Begin inner loop...\n\t"
		"movel   %1@+,%3     |  0: Fetch 32-bit word\n\t"
		"addxl   %3,%0       |    Add word + previous carry\n\t"
		"movel   %1@+,%3     |  1: Fetch 32-bit word\n\t"
		"addxl   %3,%0       |    Add word + previous carry\n\t"
		"movel   %1@+,%3     |  2: Fetch 32-bit word\n\t"
		"addxl   %3,%0       |    Add word + previous carry\n\t"
		"movel   %1@+,%3     |  3: Fetch 32-bit word\n\t"
		"addxl   %3,%0       |    Add word + previous carry\n\t"
		"movel   %1@+,%3     |  4: Fetch 32-bit word\n\t"
		"addxl   %3,%0       |    Add word + previous carry\n\t"
		"movel   %1@+,%3     |  5: Fetch 32-bit word\n\t"
		"addxl   %3,%0       |    Add word + previous carry\n\t"
		"movel   %1@+,%3     |  6: Fetch 32-bit word\n\t"
		"addxl   %3,%0       |    Add word + previous carry\n\t"
		"movel   %1@+,%3     |  7: Fetch 32-bit word\n\t"
		"addxl   %3,%0       |    Add word + previous carry\n\t"
		"movel   %1@+,%3     |  8: Fetch 32-bit word\n\t"
		"addxl   %3,%0       |    Add word + previous carry\n\t"
		"movel   %1@+,%3     |  9: Fetch 32-bit word\n\t"
		"addxl   %3,%0       |    Add word + previous carry\n\t"
		"movel   %1@+,%3     |  A: Fetch 32-bit word\n\t"
		"addxl   %3,%0       |    Add word + previous carry\n\t"
		"movel   %1@+,%3     |  B: Fetch 32-bit word\n\t"
		"addxl   %3,%0       |    Add word + previous carry\n\t"
		"movel   %1@+,%3     |  C: Fetch 32-bit word\n\t"
		"addxl   %3,%0       |    Add word + previous carry\n\t"
		"movel   %1@+,%3     |  D: Fetch 32-bit word\n\t"
		"addxl   %3,%0       |    Add word + previous carry\n\t"
		"movel   %1@+,%3     |  E: Fetch 32-bit word\n\t"
		"addxl   %3,%0       |    Add word + previous carry\n\t"
		"movel   %1@+,%3     |  F: Fetch 32-bit word\n\t"
		"addxl   %3,%0       |    Add word + previous carry\n"
		"2:                  |  End of unrolled loop\n\t"
#if IS_COLDFIRE
		"moveq   #0,%3       | Add in last carry\n\t"
		"addxl   %3,%0       |\n\t"
		"subql 	 #1,%2       | Update loop count\n\t"
		"bplb    1b          | Loop (with X clear) if not done\n\t"
		"movel   #0xffff,%2  | Get word mask\n\t"
		"movel   %0,%3       | Fold 32 bit sum to 16 bits\n\t"
		"swap    %3          |\n\t"
		"andl    %2,%0       | Mask to 16-bit sum\n\t"
		"andl    %2,%3       | Mask to 16-bit sum\n\t"
		"addl    %3,%0       |\n\t"
		"movel   %0,%3       | Add in last carry\n\t"
		"swap    %3          |\n\t"
		"addl    %3,%0       |\n\t"
		"andl    %2,%0       | Mask to 16-bit sum\n\t"
#else
		"dbf     %2,1b       | (NB- dbf doesn't affect X)\n\t"
		"movel   %0,%3       | Fold 32 bit sum to 16 bits\n\t"
		"swap    %3          | (NB- swap doesn't affect X)\n\t"
		"addxw   %3,%0       |\n\t"
		"moveq   #0,%3       | Add in last carry\n\t"
		"addxw   %3,%0       |\n\t"
		"andl    #0xffff,%0  | Mask to 16-bit sum\n"
#endif
			:
			"=d" (sum), "=a" (w), "=d" (tcnt) , "=d" (t1) :
			"0" (sum), "1" (w), "2" (tcnt) :
			"cc", "memory");
		}
		mlen &= 3;

		/*
		 * Soak up the last 1, 2 or 3 bytes
		 */
		while ((mlen -= 2) >= 0)
			sum += *w++;
		if (byte_swapped) {
			REDUCE;
			sum <<= 8;
			byte_swapped = 0;
			if (mlen == -1) {
				s_util.c[1] = *(char *)w;
				sum += s_util.s;
				mlen = 0;
			} else
				mlen = -1;
		} else if (mlen == -1)
			s_util.c[0] = *(char *)w;
	}
	if (len)
		sum = 0xDEAD;
	if (mlen == -1) {
		/* The last mbuf has odd # of bytes. Follow the
		   standard (the odd byte may be shifted left by 8 bits
		   or not as determined by endian-ness of the machine) */
		s_util.c[1] = 0;
		sum += s_util.s;
	}
	REDUCE;
	return (~sum & 0xffff);
}
