/*
 * Checksum routine for Internet Protocol family headers.
 *
 * This routine is very heavily used in the network
 * code and should be modified for each CPU to be as fast as possible.
 *
 * This implementation is the PowerPC version.
 */

#include <stdio.h>                /* for puts */

#undef	ADDCARRY
#define ADDCARRY(x)     if ((x) > 0xffff) (x) -= 0xffff
#define REDUCE          {sum = (sum & 0xffff) + (sum >> 16); ADDCARRY(sum);}

/*
 * Thanks to gcc we don't have to guess
 * which registers contain sum & w.
 */

#define LDTMP(n) tmp = *((u_int *)((u_char *)w + n))

#define ADD(n)	\
  LDTMP(n);  \
  __asm__ volatile("addc %0,%0,%2" : "=r" (sum) : "0" (sum), "r" (tmp))

#define ADDC(n) \
  LDTMP(n);  \
  __asm__ volatile("adde  %0,%0,%2" : "=r" (sum) : "0" (sum), "r" (tmp))

#define MOP \
  tmp = 0; \
  __asm__ volatile("adde  %0,%0,%2" : "=r" (sum) : "0" (sum), "r" (tmp))

#define LOAD(n)	junk = (u_char) *((volatile u_char *) w + n)


int
in_cksum(
	struct mbuf *m,
	int len
)
{
	u_char junk;
	register u_short *w;
	register unsigned sum = 0;
	register unsigned tmp;
	register int mlen = 0;
	int byte_swapped = 0;
	union { char	c[2]; u_short	s; } su;

	for (;m && len; m = m->m_next) {
		if (m->m_len == 0)
			continue;
		w = mtod(m, u_short *);
		if (mlen == -1) {
			/*
			 * The first byte of this mbuf is the continuation
			 * of a word spanning between this mbuf and the
			 * last mbuf.
			 */

			/* su.c[0] is already saved when scanning previous
			 * mbuf.  sum was REDUCEd when we found mlen == -1
			 */
			su.c[1] = *(u_char *)w;
			sum += su.s;
			w = (u_short *)((char *)w + 1);
			mlen = m->m_len - 1;
			len--;
		} else
			mlen = m->m_len;
		if (len < mlen)
			mlen = len;
		len -= mlen;
		/*
		 * Force to long boundary so we do longword aligned
		 * memory operations
		 */
		if (3 & (int) w) {
			REDUCE;
			if ((1 & (int) w) && (mlen > 0)) {
				sum <<= 8;
				su.c[0] = *(char *)w;
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
		 * Do as much of the checksum as possible 32 bits at at time.
		 * In fact, this loop is unrolled to keep overhead from
		 * branches small.
		 */
		while (mlen >= 32) {
			/*
			 * Add with carry 16 words and fold in the last
			 * carry by adding a 0 with carry.
			 *
			 * The early ADD(16) and the LOAD(32) are intended
			 * to help  get the data into the cache.
			 */
			ADD(16);
			ADDC(0);
			ADDC(4);
			ADDC(8);
			ADDC(12);
			LOAD(32);
			ADDC(20);
			ADDC(24);
			ADDC(28);
			MOP;
			w += 16;
			mlen -= 32;
		}
		if (mlen >= 16) {
			ADD(0);
			ADDC(4);
			ADDC(8);
			ADDC(12);
			MOP;
			w += 8;
			mlen -= 16;
		}
		if (mlen >= 8) {
			ADD(0);
			ADDC(4);
			MOP;
			w += 4;
			mlen -= 8;
		}
		if (mlen == 0 && byte_swapped == 0)
			continue;       /* worth 1% maybe ?? */
		REDUCE;
		while ((mlen -= 2) >= 0) {
			sum += *w++;
		}
		if (byte_swapped) {
			sum <<= 8;
			byte_swapped = 0;
			if (mlen == -1) {
				su.c[1] = *(char *)w;
				sum += su.s;
				mlen = 0;
			} else
				mlen = -1;
		} else if (mlen == -1)
			/*
			 * This mbuf has odd number of bytes.
			 * There could be a word split betwen
			 * this mbuf and the next mbuf.
			 * Save the last byte (to prepend to next mbuf).
			 */
			su.c[0] = *(char *)w;
	}

	if (len)
		puts("cksum: out of data");
	if (mlen == -1) {
		/* The last mbuf has odd # of bytes. Follow the
		   standard (the odd byte is shifted left by 8 bits) */
		su.c[1] = 0;
		sum += su.s;
	}
	REDUCE;
	return (~sum & 0xffff);
}
