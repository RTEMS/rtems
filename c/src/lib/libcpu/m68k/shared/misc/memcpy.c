/*
 *  C library memcpy routine
 *
 *  This routine has code to optimize performance on the CPU32+
 *  and another version for other 68k machines.
 *
 *  It could be optimized more for machines with MOVE16 instructions.
 *
 *  The routine is placed in this source directory to ensure that it
 *  is picked up by all applications.
 *
 *  W. Eric Norum
 *  Saskatchewan Accelerator Laboratory
 *  University of Saskatchewan
 *  Saskatoon, Saskatchewan, CANADA
 *  eric@skatter.usask.ca
 */

#include <string.h>
#include <rtems/score/m68k.h>

#if defined(__mcpu32__)
#define COPYSETUP(n)	n--
#define COPY(to,from,n,size) \
	asm volatile ("1:\n" \
	     "\tmove." size " (%0)+,(%1)+\n" \
	     "\tdbf %2,1b\n"  \
	     "\tsub.l #0x10000,%2\n" \
	     "\tbpl.b 1b\n" : \
		"=a" (from), "=a" (to), "=d" (n) :\
		 "0" (from),  "1" (to), "2" (n) : \
		 "cc", "memory")
#else
#define COPYSETUP(n)
#define COPY(to,from,n,size) \
	asm volatile ("1:\n" \
	     "\tmove." size " (%0)+,(%1)+\n" \
	     "\tsubq.l #1,%2\n\tbne.b 1b\n" : \
		"=a" (from), "=a" (to), "=d" (n) :\
		 "0" (from),  "1" (to), "2" (n) : \
		 "cc", "memory")
#endif

/* gcc doesn't know that cpu32+ is better than cpu32 :( */
#if defined(__mcpu32p__)
#undef M68k_HAS_MISALIGNED
#define M68k_HAS_MISALIGNED 1
#endif

void *
memcpy(void *s1, const void *s2, size_t n)
{
	char *p1 = s1;
	const char *p2 = s2;

	if (n) {
		if (n < 16) {
			COPYSETUP (n);
			COPY (p1, p2, n, "b");
		}
		else {
			int nbyte;
			int nl;
			nbyte = (int)p1 & 0x3;
			if (nbyte) {
				nbyte = 4 - nbyte;
				n -= nbyte;
				COPYSETUP (nbyte);
				COPY (p1, p2, nbyte, "b");
			}
#if (M68K_HAS_MISALIGNED == 0)
			/*
			 * Take care of machines that can't
			 * handle misaligned references.
			 */
			if ((int)p2 & 0x1) {
				COPYSETUP (n);
				COPY (p1, p2, n, "b");
				return s1;
			}
#endif
			nl = (unsigned int)n >> 2;
			COPYSETUP (nl);
			COPY (p1, p2, nl, "l");
			nbyte = (int)n & 0x3;
			if (nbyte) {
				COPYSETUP (nbyte);
				COPY (p1, p2, nbyte, "b");
			}
		}
	}
	return s1;
}
