#include <rtems.h>
#include <bsp.h>
#include <bsp/if_mve_pub.h>
#include <stdlib.h>
#include <stdio.h>

/* Demo for the mv64360 ethernet quirk:
 *
 * $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
 * $$ buffer segments < 8 bytes must be aligned $$
 * $$ to 8 bytes but larger segments are not    $$
 * $$ sensitive to alignment.                   $$
 * $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
 *
 * How to use:
 *
 *   Init MVE driver on (unused) unit 2:
 *
 *    mve = mvtst_init(2)
 *
 *    data = { 1,2,3,4,5,6,7,8,9,0xa,0xb, ... }
 *
 *   Alloc 2-element mbuf chain (1st holds an
 *   ethernet header which is > 8bytes so we can't
 *   test this with only 1 mbuf. The 2nd mbuf holds
 *   a small fragment of data).
 *
 *     mb  = mvtst_getbuf(mve)
 *
 *   Copy data into aligned area inside 2nd mbuf,
 *   (so that you can see if the chip starts using
 *   the aligned area rather than the unaligned
 *   buffer pointer). Point mbuf's data pointer
 *   at 'off'set from the aligned area:
 *
 *     mvtst_putbuf(mb, data, len, offset)
 *
 *   Send chain off:
 *
 *    BSP_mve_send_buf(mve, mb, 0, 0)
 *
 *   Watch raw data:
 *
 *    tcpdump -XX -vv -s0 ether host <my-ether-addr>
 *
 *   E.g, if offset = 1, len = 2 then we would like
 *   to see 
 *  
 *   GOOD:
 *           < 14 header bytes > 0x02, 0x03

 *   but if the chip starts DMA at aligned address
 *   we see instead
 *   BAD:
 *           < 14 header bytes > 0x01, 0x02
 */

static inline void *rmalloc(size_t l) { return malloc(l); }
static inline void  rfree(void *p) { return free(p); }

#define _KERNEL
#include <sys/mbuf.h>

static void
cleanup_buf(void *u_b, void *closure, int error)
{
rtems_bsdnet_semaphore_obtain();
	m_freem((struct mbuf*)u_b);
rtems_bsdnet_semaphore_release();
}

struct mbuf *mvtst_getbuf(struct mveth_private *mp)
{
struct mbuf *m,*n;

	if ( !mp ) {
		printf("need driver ptr arg\n");
		return 0;
	}
rtems_bsdnet_semaphore_obtain();
	MGETHDR(m, M_DONTWAIT, MT_DATA);
	MGET(n, M_DONTWAIT, MT_DATA);
	m->m_next = n;
rtems_bsdnet_semaphore_release();
	/* Ethernet header */
	memset( mtod(m, unsigned char*), 0xff, 6);
	BSP_mve_read_eaddr(mp, mtod(m, unsigned char*) + 6);
	/* Arbitrary; setting to IP but we don't bother
	 * to setup a real IP header. We just watch the
	 * raw packet contents...
	 */
	mtod(m, unsigned char*)[12] = 0x08;
	mtod(m, unsigned char*)[13] = 0x00;
	m->m_pkthdr.len = m->m_len = 14;
	n->m_len = 0;
	return m;
}

int
mvtst_putbuf(struct mbuf *m, void *data, int len, int off)
{
int i;
	if ( m ) {
		m->m_pkthdr.len += len;
	if ( ( m= m->m_next ) ) {
		m->m_len = len;
		memcpy(mtod(m, void*), data, 32);
		m->m_data += off;
		printf("m.dat: 0x%08x, m.data: 0x%08x\n", m->m_dat, m->m_data);
		for ( i=0; i< 16; i++ ) {
			printf(" %02x,",mtod(m, unsigned char*)[i]);
		}
		printf("\n");
	}
	}
	
	return 0;
}

static void *alloc_rxbuf(int *p_size, unsigned long *paddr)
{
	return *(void**)paddr = rmalloc((*p_size = 1800));
}

static void consume_buf(void *buf, void *closure, int len)
{
	rfree(buf);
}

void *
mvtst_init(int unit)
{
struct mveth_private *mp;
	mp = BSP_mve_setup(
		unit, 0,
		cleanup_buf, 0,
		alloc_rxbuf,
		consume_buf, 0,
		10, 10,
		0);
	if ( mp )
		BSP_mve_init_hw(mp, 0, 0);
	return mp;
}
