#ifndef KERNEL
#define KERNEL
#endif

#include <rtems.h>
#include <rtems/rtems_bsdnet_internal.h>
#include <bsp.h>
#include <sys/mbuf.h>

#include "mv64340_eth_ll.h"

#include <string.h>
#include <assert.h>

#include <netinet/in.h>
#include <stdio.h>

#define RX_SPACING 1
#define TX_SPACING 1

#define RX_RING_SIZE (MV64340_RX_QUEUE_SIZE*RX_SPACING)
#define TX_RING_SIZE (MV64340_TX_QUEUE_SIZE*TX_SPACING)


struct eth_rx_desc rx_ring[RX_RING_SIZE] __attribute__((aligned(32)));
struct eth_rx_desc rx_ring[RX_RING_SIZE] = {{0},};

struct eth_tx_desc tx_ring[TX_RING_SIZE] __attribute__((aligned(32)));
struct eth_tx_desc tx_ring[TX_RING_SIZE] = {{0},};

/* packet buffers */
char rx_buf[MV64340_RX_QUEUE_SIZE][2048] __attribute__((aligned(8)));
char rx_buf[MV64340_RX_QUEUE_SIZE][2048];

char tx_buf[MV64340_RX_QUEUE_SIZE][2048] __attribute__((aligned(8)));
char tx_buf[MV64340_RX_QUEUE_SIZE][2048];

char BcHeader[22] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* dst */
	0x00, 0x01, 0xaf, 0x13, 0xb5, 0x3e, /* src */
	00, 00, /* len */
	0xAA,	/* dsap */
	0xAA,	/* ssap */
	0x03,	/* ctrl */
	0x08, 0x00, 0x56,	/* snap_org [stanford] */
	0x80, 0x5b,			/* snap_type (stanford kernel) */
};

struct mv64340_private mveth = {
	port_num: 0,
	port_mac_addr: {0x00,0x01,0xAF,0x13,0xB5,0x3C},
	/* port_config .. tx_resource_err are set by port_init */
	0
};

struct pkt_info p0,p1;

static inline void rx_stopq(int port)
{
	MV_WRITE(MV64340_ETH_RECEIVE_QUEUE_COMMAND_REG(port), 0x0000ff00);
}

static inline void tx_stopq(int port)
{
	MV_WRITE(MV64340_ETH_TRANSMIT_QUEUE_COMMAND_REG(port), 0x0000ff00);
}

#define MV64360_ENET2MEM_SNOOP_NONE 0x0000
#define MV64360_ENET2MEM_SNOOP_WT	0x1000
#define MV64360_ENET2MEM_SNOOP_WB	0x2000

#if 0
int
mveth_init(struct mv64340_private *mp)
{
int i;
	mp->p_rx_desc_area = rx_ring;
	mp->p_tx_desc_area = tx_ring;

	rx_stopq(mp->port_num);
	tx_stopq(mp->port_num);

	/* MotLoad has cache snooping disabled on the ENET2MEM windows.
	 * Some comments in (linux) indicate that there are errata
	 * which cause problems which is a real bummer.
	 * We try it anyways...
	 */
	{
	unsigned long disbl, bar;
	disbl = MV_READ(MV64340_ETH_BASE_ADDR_ENABLE_REG);
	/* disable all 6 windows */
	MV_WRITE(MV64340_ETH_BASE_ADDR_ENABLE_REG, 0x3f);
	/* set WB snooping */
	for ( i=0; i<6*8; i+=8 ) {
		if ( (bar = MV_READ(MV64340_ETH_BAR_0 + i)) && MV_READ(MV64340_ETH_SIZE_REG_0 + i) ) {
			MV_WRITE(MV64340_ETH_BAR_0 + i, bar | MV64360_ENET2MEM_SNOOP_WB);
			/* read back to flush fifo [linux comment] */
			(void)MV_READ(MV64340_ETH_BAR_0 + i);
		}
	}
	/* restore/re-enable */
	MV_WRITE(MV64340_ETH_BASE_ADDR_ENABLE_REG, disbl);
	}

	eth_port_init(mp);

	sleep(1);

	mveth_init_tx_desc_ring(mp);
	mveth_init_rx_desc_ring(mp);
#if 0
	for ( i = 0; i<MV64340_RX_QUEUE_SIZE; i++ ) {
		p0.byte_cnt 	= sizeof(rx_buf[0]);
		p0.buf_ptr  	= (dma_addr_t)rx_buf[i];
		p0.return_info  = (void*)i;
		/* other fields are not used by ll driver */
		assert ( ETH_OK == eth_rx_return_buff(mp,&p0) );
	}
	memset(&p0, 0, sizeof(p0));
#endif

	return eth_port_start(mp);
}
#endif

void
mveth_stop(struct mv64340_private *mp)
{
extern void mveth_stop_hw();
	rtems_bsdnet_semaphore_obtain();
		mveth_stop_hw(mp);
	rtems_bsdnet_semaphore_release();
}

extern int mveth_send_mbuf();
extern int mveth_swipe_tx();

int
mveth_tx(struct mv64340_private *mp, char *data, int len, int nbufs)
{
int rval = -1,l;
char        *p;
struct mbuf *m;
char 	*emsg = 0;

	rtems_bsdnet_semaphore_obtain();
	MGETHDR(m, M_WAIT, MT_DATA);
	if ( !m ) {
		emsg="Unable to allocate header\n";
		goto bail;
	}
	MCLGET(m, M_WAIT);
	if ( !(m->m_flags & M_EXT) ) {
		m_freem(m);
		emsg="Unable to allocate cluster\n";
		goto bail;
	}
	p = mtod(m, char *);
	l = 0;
	switch (nbufs) {
		case 3:
		default:
			emsg="nbufs arg must be 1..3\n";
			goto bail;

		case 1:
			l += sizeof(BcHeader);
			memcpy(p, &BcHeader, sizeof(BcHeader));
			p += sizeof(BcHeader);

		case 2:
			memcpy(p,data,len);
			l += len;
			m->m_len = m->m_pkthdr.len = l;
			if ( 2 == nbufs ) {
    			M_PREPEND(m, sizeof (BcHeader), M_WAIT);
				if (!m) {
					emsg = "Unable to prepend\n";
					goto bail;
				}
				p = mtod(m, char*);
				memcpy(p,&BcHeader,sizeof(BcHeader));
				l += sizeof(BcHeader);
			}
		break;
	}
	*(short*)(mtod(m, char*) + 12) = htons(l-14);
	rval = mveth_send_mbuf(mp,m);

bail:
	rtems_bsdnet_semaphore_release();
	if (emsg)
		printf(emsg);

#if 0
    /*
     * Add local net header.  If no space in first mbuf,
     * allocate another.
     */
    M_PREPEND(m, sizeof (struct ether_header), M_DONTWAIT);
    if (m == 0)
        senderr(ENOBUFS);
    eh = mtod(m, struct ether_header *);
    (void)memcpy(&eh->ether_type, &type,
        sizeof(eh->ether_type));
    (void)memcpy(eh->ether_dhost, edst, sizeof (edst));
    (void)memcpy(eh->ether_shost, ac->ac_enaddr,
        sizeof(eh->ether_shost));
#endif
	return rval;
}

int
mveth_protected(int (*p)(struct mv64340_private*), struct mv64340_private *mp)
{
int rval;
	rtems_bsdnet_semaphore_obtain();
		rval = p(mp);
	rtems_bsdnet_semaphore_release();
	return rval;
}

int
mveth_rx(struct mv64340_private *mp)
{
extern int mveth_swipe_rx();
	return mveth_protected(mveth_swipe_rx,mp);
}

int
mveth_reclaim(struct mv64340_private *mp)
{
extern int mveth_swipe_tx();
	return mveth_protected(mveth_swipe_tx,mp);
}


int preth(FILE *f, char *p)
{
int i;
	for (i=0; i<4; i++)
		fprintf(f,"%02X:",p[i]);
	fprintf(f,"%02X",p[i]);
	return 6;
}

char *errcode2str(st)
{
char *rval;
	switch(st) {
		case ETH_OK:
			rval = "OK";
		break;
		case ETH_ERROR:
			rval = "Fundamental error.";
		break;
		case ETH_RETRY:
			rval = "Could not process request. Try later.";
		break;
		case ETH_END_OF_JOB:
			rval = "Ring has nothing to process.";
		break;
		case ETH_QUEUE_FULL:
			rval = "Ring resource error.";
		break;
		case ETH_QUEUE_LAST_RESOURCE:
			rval = "Ring resources about to exhaust.";
		break;
		default:
			rval = "UNKNOWN"; break;
	}
	return rval;
}


#if 0
int
mveth_rx(struct mv64340_private *mp)
{
int st;
struct pkt_info p;
	if ( ETH_OK != (st=eth_port_receive(mp, &p)) ) {
		fprintf(stderr,"receive: %s\n", errcode2str(st));
		return -1;
	}
	printf("%i bytes received from ", p.byte_cnt);
	preth(stdout,(char*)p.buf_ptr+6);
	printf(" (desc. stat: 0x%08x)\n", p.cmd_sts);
		
	p.byte_cnt = sizeof(rx_buf[0]);
	p.buf_ptr -= RX_BUF_OFFSET;
	if ( ETH_OK != (st=eth_rx_return_buff(mp,&p) ) ) {
		fprintf(stderr,"returning buffer: %s\n", errcode2str(st));
		return -1;
	}
	return 0;
}
#endif

int
dring()
{
int i;
if (1) {
struct eth_rx_desc *pr;
printf("RX:\n");
	for (i=0, pr=rx_ring; i<RX_RING_SIZE; i+=RX_SPACING, pr+=RX_SPACING) {
		dcbi(pr);
		printf("cnt: 0x%04x, size: 0x%04x, stat: 0x%08x, next: 0x%08x, buf: 0x%08x\n",
			pr->byte_cnt, pr->buf_size, pr->cmd_sts, pr->next_desc_ptr, pr->buf_ptr);
	}
}
if (1) {
struct eth_tx_desc *pt;
printf("TX:\n");
	for (i=0, pt=tx_ring; i<TX_RING_SIZE; i+=TX_SPACING, pt+=TX_SPACING) {
		dcbi(pt);
		printf("cnt: 0x%04x, stat: 0x%08x, next: 0x%08x, buf: 0x%08x\n",
			pt->byte_cnt, pt->cmd_sts, pt->next_desc_ptr, pt->buf_ptr);
	}
}
	return 0;
}
