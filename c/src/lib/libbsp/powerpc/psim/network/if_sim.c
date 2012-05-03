/* Trivial driver for PSIM's emulated ethernet device 'hw_ethtap'.
 *
 * NOTE: At the time of this writing (2009/1) 'hw_ethtap' requires
 *       a patched version of PSIM -- the vanilla version does not
 *       implement this device.
 *       Also, support for this device is currently only available
 *       on a linux host.
 *
 * Author/Copyright: Till Straumann <Till.Straumann@TU-Berlin.de>
 *
 * LICENSE
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#include <bsp.h>
#include <rtems.h>
#include <bsp/irq.h>
#include <psim.h>
#include <libcpu/io.h>
#include <inttypes.h>


#ifndef KERNEL
#define KERNEL
#endif
#ifndef _KERNEL
#define _KERNEL
#endif

#include <rtems/rtems_bsdnet.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>

#include <stdio.h>

#define IFSIM_SLOTS 1

#define DRVNAME "if_sim"

#define IFSIM_TX_BUF_REG 1
#define IFSIM_TX_STA_REG 2

#define IFSIM_RX_BUF_REG 4
#define IFSIM_RX_STA_REG 5

#define IFSIM_RX_CNT_MSK  0xffff
#define IFSIM_RX_STA_DONE (1<<31)

#define IFSIM_RX_ERR_NOSPC	(1<<20)  /* buffer too small   */
#define IFSIM_RX_ERR_DMA    (1<<21)  /* DMA error          */
#define IFSIM_RX_ERR_RD     (1<<23)  /* file read error    */

#define IFSIM_TX_STA_LST  (1<<16)
#define IFSIM_TX_STA_DONE (1<<31)

#define IFSIM_IEN_REG    6
#define IFSIM_IRQ_REG    7

#define IFSIM_RX_IRQ     (1<<0)

#define IFSIM_MACA0_REG 8
#define IFSIM_MACA1_REG 9

#define IFSIM_CSR_REG  10
#define IFSIM_CSR_PROM    (1<<0)
/* Enable CRC generation/checking */
#define IFSIM_CSR_CRCEN   (1<<1)


#define RX_BUF_ALIGNMENT 1
#define ETH_RX_OFFSET    0

/*
 * Align 'p' up to a multiple of 'a' which must be
 * a power of two. Result is cast to (uintptr_t)
 */
#define ALIGNTO(p,a)    ((((uintptr_t)(p)) + (a) - 1) & ~((a)-1))


typedef volatile unsigned reg_t;

struct ifsim_private {
	reg_t *base;
	void  *rbuf;
	unsigned rx_cserrs;
	unsigned rx_err_nospc;
	unsigned rx_err_dma;
	unsigned rx_err_rd;
	unsigned rx_nobufs;
	unsigned rx_irqs;
};

struct ifsim_softc {
	struct arpcom          arpcom;
	struct ifsim_private   pvt;
};

struct ifsim_softc theIfSims[IFSIM_SLOTS] = {{{{0}}} };

rtems_id           ifsim_tid = 0;

__inline__ uint32_t
ifsim_in(struct ifsim_softc *sc, unsigned regno)
{
	return in_be32( sc->pvt.base + regno );
}

__inline__ void
ifsim_out(struct ifsim_softc *sc, unsigned regno, uint32_t v)
{
	out_be32(sc->pvt.base + regno, v);
}

static void *
alloc_mbuf_rx(int *psz, uintptr_t *paddr)
{
struct mbuf     *m;
unsigned long   l,o;

    MGETHDR(m, M_DONTWAIT, MT_DATA);
    if ( !m )
        return 0;
    MCLGET(m, M_DONTWAIT);
    if ( ! (m->m_flags & M_EXT) ) {
        m_freem(m);
        return 0;
    }

    o = mtod(m, unsigned long);
    l = ALIGNTO(o, RX_BUF_ALIGNMENT) - o;

    /* align start of buffer */
    m->m_data += l;

    /* reduced length */
    l = MCLBYTES - l;

    m->m_len   = m->m_pkthdr.len = l;
    *psz       = m->m_len;
    *paddr     = mtod(m, unsigned long);

    return (void*) m;
}

static int
get_rxbuf(struct ifsim_softc *sc)
{
int         sz;
uintptr_t   addr;
void        *nbuf;

	nbuf = alloc_mbuf_rx(&sz, &addr);

	if ( nbuf ) {
		sc->pvt.rbuf = nbuf;
		ifsim_out(sc, IFSIM_RX_BUF_REG, addr);
		ifsim_out(sc, IFSIM_RX_STA_REG, sz);
		return 0;
	}
	return -1;
}

/* set/clear promiscuous mode */
static void
ifsim_upd_promisc(struct ifsim_softc *sc)
{
uint32_t ncsr, csr;

	ncsr = csr = ifsim_in(sc, IFSIM_CSR_REG);

	if ( sc->arpcom.ac_if.if_flags & IFF_PROMISC )
		ncsr |=  IFSIM_CSR_PROM;
	else
		ncsr &= ~IFSIM_CSR_PROM;

	if ( ncsr != csr )
		ifsim_out(sc, IFSIM_CSR_REG, ncsr);
}

static void
ifsim_init(void *arg)
{
struct ifsim_softc *sc = arg;
struct ifnet       *ifp = &sc->arpcom.ac_if;

	if ( 0 == get_rxbuf(sc) ) {
		ifsim_upd_promisc(sc);
		ifp->if_flags |= IFF_RUNNING;
	}
}

static void
ifsim_start(struct ifnet *ifp)
{
struct ifsim_softc *sc = ifp->if_softc;
struct mbuf        *m, *mh, *m1;

	while ( ifp->if_snd.ifq_head ) {
		IF_DEQUEUE( &ifp->if_snd, mh );
		for ( m=mh, m1 = m->m_next ; m1 ; m1 = m1->m_next ) {
			ifsim_out(sc, IFSIM_TX_BUF_REG, mtod(m, uint32_t));
			ifsim_out(sc, IFSIM_TX_STA_REG, m->m_len);
			/* dummy-busywait; the emulated hardware DMAs our
			 * data away 'immediately' i.e., this loop is
			 * never executed
			 */
			while ( ! (IFSIM_TX_STA_DONE & ifsim_in(sc, IFSIM_TX_STA_REG)) )
				/* Loop */;
			m = m1;
		}
		ifsim_out(sc, IFSIM_TX_BUF_REG, mtod(m, uint32_t));
		ifsim_out(sc, IFSIM_TX_STA_REG, m->m_len | IFSIM_TX_STA_LST);

		/* dummy-busywait; the emulated hardware DMAs our
		 * data away 'immediately' i.e., this loop is
		 * never executed
		 */
		while ( ! (IFSIM_TX_STA_DONE & ifsim_in(sc, IFSIM_TX_STA_REG)) )
			/* Loop */;

		m_freem(mh);
	}
}

static int
ifsim_ioctl(struct ifnet *ifp, ioctl_command_t cmd, caddr_t data)
{
struct ifsim_softc *sc  = ifp->if_softc;
int                rval = 0;
int                f;

	switch (cmd) {

		case SIOCSIFFLAGS:
			f = ifp->if_flags;
			if ( f & IFF_UP ) {
				if ( ! (f & IFF_RUNNING) ) {
					ifsim_init(sc);
				} else {
					ifsim_upd_promisc(sc);
				}
				/* FIXME: handle other flags here */
			} else {
				if ( f & IFF_RUNNING ) {
					printk("WARNING: bringing "DRVNAME" down not really implemented\n");
					ifp->if_flags &= ~(IFF_RUNNING | IFF_OACTIVE);
				}
			}

		break;

		case SIO_RTEMS_SHOW_STATS:
			printf("RX bad chksum  : %u\n", sc->pvt.rx_cserrs);
			printf("RX no space    : %u\n", sc->pvt.rx_err_nospc);
			printf("RX bad DMA     : %u\n", sc->pvt.rx_err_dma);
			printf("RX read errors : %u\n", sc->pvt.rx_err_rd);
			printf("rx_nobufs      : %u\n", sc->pvt.rx_nobufs);
			printf("rx_irqs        : %u\n", sc->pvt.rx_irqs);
		break;

		default:
			rval = ether_ioctl(ifp, cmd, data);
		break;
	}

	return rval;
}

static void ifsim_irq_on(const rtems_irq_connect_data *pd)
{
struct ifsim_softc *sc = pd->handle;
	ifsim_out(sc, IFSIM_IEN_REG, IFSIM_RX_IRQ);
}

static void ifsim_irq_off(const rtems_irq_connect_data *pd)
{
struct ifsim_softc *sc = pd->handle;
	ifsim_out(sc, IFSIM_IEN_REG, 0);
}

static int ifsim_irq_ison(const rtems_irq_connect_data *pd)
{
struct ifsim_softc *sc = pd->handle;
	return ifsim_in(sc, IFSIM_IEN_REG) & IFSIM_RX_IRQ ? 1 : 0;
}

static void
ifsim_isr(void *arg)
{
struct ifsim_softc *sc = arg;

	sc->pvt.rx_irqs++;
#ifdef IRQ_DEBUG
	printk("ISR happened\n");
#endif

	ifsim_out(sc, IFSIM_IEN_REG, 0);
	rtems_event_send(ifsim_tid, (1<<(sc-theIfSims)));
}

static void
ifsim_task(void *arg)
{
struct ifsim_softc *sc;
uint32_t           sta;
struct ifnet       *ifp;
unsigned           len;
rtems_event_set    evs;

	while (1) {

		rtems_bsdnet_event_receive(
			((1<<IFSIM_SLOTS)-1),
			RTEMS_WAIT | RTEMS_EVENT_ANY,
			RTEMS_NO_TIMEOUT,
			&evs);

		evs &= ((1<<IFSIM_SLOTS)-1);

#ifdef IRQ_DEBUG
		printk("Task got evs %u\n", evs);
#endif

		for ( sc = theIfSims; evs; evs>>=1, sc++ ) {

			if ( ! ( evs & 1 ) )
				continue;

			ifp = &sc->arpcom.ac_if;

			while ( ifsim_in(sc, IFSIM_IRQ_REG) & IFSIM_RX_IRQ ) {
				struct mbuf *m = sc->pvt.rbuf;

				sta = ifsim_in(sc, IFSIM_RX_STA_REG);


				if ( (sta & IFSIM_RX_STA_DONE) ) {

					if ( (ifp->if_flags & IFF_RUNNING) ) {
						if ( 0 == get_rxbuf(sc) ) {
							/* enqueue packet */
							struct ether_header *eh;
							uint32_t            crc_net, crc;
							int                 crc_len;

							crc_len = (IFSIM_CSR_CRCEN & ifsim_in(sc, IFSIM_CSR_REG)) ? sizeof(crc_net) : 0;

							len = (sta & IFSIM_RX_CNT_MSK) - crc_len;

							eh = (struct ether_header*)(mtod(m,unsigned long) + ETH_RX_OFFSET);

							m->m_len = m->m_pkthdr.len = len - sizeof(struct ether_header) - ETH_RX_OFFSET;
							m->m_data += sizeof(struct ether_header) + ETH_RX_OFFSET;
							m->m_pkthdr.rcvif = ifp;

#ifdef DEBUG_WO_BSDNET
							{
								int i;
								for ( i=0; i<len + crc_len; ) {
									printk("%02X ",((char*)eh)[i]);
									if ( 0 == (++i & 0xf) )
										fputc('\n',stdout);
								}
								if ( i & 0xf )
									fputc('\n', stdout);
								printk("*****\n");
							}
#endif

							if ( crc_len
								 && (memcpy(&crc_net, (char*)eh + len, crc_len),
							        (crc = (ether_crc32_le((uint8_t *)eh, len) ^ 0xffffffff)) != crc_net) ) {
								printk("CSUM: me 0x%08X, them 0x%08x\n", crc, crc_net);
								sc->pvt.rx_cserrs++;
							} else {

								ifp->if_ipackets++;
								ifp->if_ibytes  += len;

#ifdef DEBUG_WO_BSDNET
								m_freem(m);
#else
								ether_input(ifp, eh, m);
#endif

								m = 0;
							}

						} else {
							/* throw away and reuse buffer */
							sc->pvt.rx_nobufs++;
						}
					}
				} else {
					/* throw away and reuse buffer */
					if ( (sta & IFSIM_RX_ERR_NOSPC) )
						sc->pvt.rx_err_nospc++;
					if ( (sta & IFSIM_RX_ERR_DMA) )
						sc->pvt.rx_err_dma++;
					if ( (sta & IFSIM_RX_ERR_RD) )
						sc->pvt.rx_err_rd++;
				}

				if ( m ) {
					/* reuse */
					ifsim_out(sc, IFSIM_RX_STA_REG, m->m_pkthdr.len);
				}
			}
			/* re-enable interrupt */
			ifsim_out(sc, IFSIM_IEN_REG, IFSIM_RX_IRQ);
		}
	}
}

int
rtems_ifsim_attach(struct rtems_bsdnet_ifconfig *ifcfg, int attaching)
{
char                   *unitName;
int                    unit;
struct ifsim_softc     *sc;
struct ifnet           *ifp;
uint32_t               v;
rtems_irq_connect_data ihdl;

	if ( !attaching )
		return -1;

	unit = rtems_bsdnet_parse_driver_name(ifcfg, &unitName);

	if ( unit <= 0 || unit > IFSIM_SLOTS ) {
		printk(DRVNAME": Bad unit number %i; must be 1..%i\n", unit, IFSIM_SLOTS);
		return 1;
	}

	sc  = &theIfSims[unit-1];
	ifp = &sc->arpcom.ac_if;

	memset(&sc->pvt, 0, sizeof(sc->pvt));

	sc->pvt.base = (reg_t*)ifcfg->port;

	if ( 0 == sc->pvt.base )
		sc->pvt.base = (reg_t*)PSIM.Ethtap;


	sc->pvt.rbuf = 0;

	if ( !ifsim_tid ) {
		ifsim_tid = rtems_bsdnet_newproc("IFSM", 10000, ifsim_task, 0);
	}

	ihdl.name   = ifcfg->irno;
	ihdl.hdl    = ifsim_isr;
	ihdl.handle = sc;
	ihdl.on     = ifsim_irq_on;
	ihdl.off    = ifsim_irq_off;
	ihdl.isOn   = ifsim_irq_ison;

	if ( ! BSP_install_rtems_irq_handler(&ihdl) ) {
		printk(DRVNAME"_attach(): installing ISR failed!\n");
		return -1;
	}

	if ( ifcfg->hardware_address ) {
		memcpy(sc->arpcom.ac_enaddr, ifcfg->hardware_address, ETHER_ADDR_LEN);
	} else {
		v = ifsim_in(sc, IFSIM_MACA0_REG);
		memcpy(sc->arpcom.ac_enaddr, &v, 4);
		v = ifsim_in(sc, IFSIM_MACA1_REG);
		memcpy(sc->arpcom.ac_enaddr+4, &v, 2);
	}

	ifp->if_softc     = sc;
	ifp->if_unit      = unit;
	ifp->if_name      = unitName;

	ifp->if_mtu       = ifcfg->mtu ? ifcfg->mtu : ETHERMTU;

	ifp->if_init      = ifsim_init;
	ifp->if_ioctl     = ifsim_ioctl;
	ifp->if_start     = ifsim_start;
	ifp->if_output    = ether_output;

	ifp->if_watchdog  = 0;
	ifp->if_timer     = 0;

	ifp->if_flags     = IFF_BROADCAST | IFF_SIMPLEX;

	ifp->if_snd.ifq_maxlen = ifqmaxlen;

	if_attach(ifp);
	ether_ifattach(ifp);

	return 0;
}
