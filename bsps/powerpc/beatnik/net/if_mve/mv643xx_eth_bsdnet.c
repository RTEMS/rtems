/* RTEMS driver for the mv643xx gigabit ethernet chip */

/* Acknowledgement:
 *
 * Valuable information for developing this driver was obtained
 * from the linux open-source driver mv643xx_eth.c which was written
 * by the following people and organizations:
 *
 * Matthew Dharm <mdharm@momenco.com>
 * rabeeh@galileo.co.il
 * PMC-Sierra, Inc., Manish Lachwani
 * Ralf Baechle <ralf@linux-mips.org>
 * MontaVista Software, Inc., Dale Farnsworth <dale@farnsworth.org>
 * Steven J. Hill <sjhill1@rockwellcollins.com>/<sjhill@realitydiluted.com>
 *
 * Note however, that in spite of the identical name of this file
 * (and some of the symbols used herein) this file provides a
 * new implementation and is the original work by the author.
 */

/* 
 * Authorship
 * ----------
 * This software (mv643xx ethernet driver for RTEMS) was
 *     created by Till Straumann <strauman@slac.stanford.edu>, 2005-2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 * 
 * Acknowledgement of sponsorship
 * ------------------------------
 * The 'mv643xx ethernet driver for RTEMS' was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 * 
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 * 
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 * 
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.  
 * 
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 * 
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */ 

/*
 * NOTE: Some register (e.g., the SMI register) are SHARED among the
 *       three devices. Concurrent access protection is provided by
 *       the global networking semaphore.
 *       If other drivers are running on a subset of IFs then proper
 *       locking of all shared registers must be implemented!
 *
 *       Some things I learned about this hardware can be found
 *       further down...
 */

#ifndef KERNEL
#define KERNEL
#endif
#ifndef _KERNEL
#define _KERNEL
#endif

#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/error.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/gtreg.h>
#include <libcpu/byteorder.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <inttypes.h>

#include <bsp/mv643xx_eth.h>

#include <sys/param.h>
#include <sys/proc.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <dev/mii/mii.h>
#include <net/if_var.h>
#include <net/if_arp.h>
#include <net/if_media.h>

/* Not so nice; would be more elegant not to depend on C library but the
 * RTEMS-specific ioctl for dumping statistics needs stdio anyways.
 */

/*#define NDEBUG effectively removes all assertions
 * If defining NDEBUG, MAKE SURE assert() EXPRESSION HAVE NO SIDE_EFFECTS!!
 * This driver DOES have side-effects, so DONT DEFINE NDEBUG
 * Performance-critical assertions are removed by undefining MVETH_TESTING.
 */

#undef NDEBUG
#include <rtems/rtems_bsdnet.h>
#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>

#include <rtems/rtems_mii_ioctl.h>

#include <bsp/early_enet_link_status.h>
#include <bsp/if_mve_pub.h>

/* CONFIGURABLE PARAMETERS */

/* Compile-time debugging features */

/* Enable paranoia assertions and checks; reduce # of descriptors to minimum for stressing   */
#undef  MVETH_TESTING

/* Enable debugging messages and some support routines  (dump rings etc.)                    */      
#undef  MVETH_DEBUG

/* Ring sizes */

#ifdef MVETH_TESTING

/* hard and small defaults */
#undef  MV643XX_RX_RING_SIZE
#define MV643XX_RX_RING_SIZE	2
#undef  MV643XX_TX_RING_SIZE
#define MV643XX_TX_RING_SIZE	4

#else /* MVETH_TESTING */

/* Define default ring sizes, allow override from bsp.h, Makefile,... and from ifcfg->rbuf_count/xbuf_count */

#ifndef MV643XX_RX_RING_SIZE
#define MV643XX_RX_RING_SIZE	40	/* attached buffers are always 2k clusters, i.e., this
									 * driver - with a configured ring size of 40 - constantly
									 * locks 80k of cluster memory - your app config better
									 * provides enough space!
									 */
#endif

#ifndef MV643XX_TX_RING_SIZE
/* NOTE: tx ring size MUST be > max. # of fragments / mbufs in a chain;
 *       in 'TESTING' mode, special code is compiled in to repackage
 *		 chains that are longer than the ring size. Normally, this is
 *		 disabled for sake of speed.
 *		 I observed chains of >17 entries regularly!
 *
 *       Also, TX_NUM_TAG_SLOTS (1) must be left empty as a marker, hence
 *       the ring size must be > max. #frags + 1.
 */
#define MV643XX_TX_RING_SIZE	200	/* these are smaller fragments and not occupied when
									 * the driver is idle.
									 */
#endif

#endif /* MVETH_TESTING */

#define DRVNAME			"mve"
#define MAX_NUM_SLOTS   3

#ifdef NDEBUG
#error "Driver uses assert() statements with side-effects; MUST NOT define NDEBUG"
#endif

#ifdef MVETH_DEBUG
#define STATIC
#else
#define STATIC static
#endif

/* macros for ring alignment; proper alignment is a hardware req; . */

/* Software cache management */

#ifndef __PPC__
#error	"Dont' know how to deal with cache on this CPU architecture"
#endif

/* Ring entries are 32 bytes; coherency-critical chunks are 16 -> software coherency
 * management works for cache line sizes of 16 and 32 bytes only. If the line size
 * is bigger, the descriptors could be padded...
 */
#if	PPC_CACHE_ALIGMENT != 16 && PPC_CACHE_ALIGNMENT != 32
#error "Cache line size must be 16 or 32"
#else
#define RING_ALIGNMENT				PPC_CACHE_ALIGNMENT
#define RX_BUF_ALIGNMENT			PPC_CACHE_ALIGNMENT
#endif

/* HELPER MACROS */

/* Align base to alignment 'a' */
#define MV643XX_ALIGN(b, a)	((((uint32_t)(b)) + (a)-1) & (~((a)-1)))

/* The chip puts the ethernet header at offset 2 into the buffer so
 * that the payload is aligned
 */
#define ETH_RX_OFFSET								2
#define ETH_CRC_LEN									4	/* strip FCS at end of packet */

/* TYPE DEFINITIONS */

/* just to make the purpose explicit; vars of this
 * type may need CPU-dependent address translation,
 * endian conversion etc.
 */
typedef uint32_t Dma_addr_t;

/* stuff needed for bsdnet support */
struct mveth_bsdsupp {
	int				oif_flags;					/* old / cached if_flags */
	int             tx_ring_size;
	int             rx_ring_size;
};

struct mveth_softc {
	struct arpcom			arpcom;
	struct mveth_bsdsupp	bsd;
	struct mveth_private	*pvt;
};

typedef struct BsdMveIter {
	MveEthBufIter it;
	struct mbuf  *m;
	struct mbuf  *h;
} BsdMveIter;

/* GLOBAL VARIABLES */
#ifdef MVETH_DEBUG_TX_DUMP
int mveth_tx_dump = 0;
#endif

/* register access protection mutex */
STATIC rtems_id mveth_mtx = 0;

#define REGLOCK()	do { \
		if ( RTEMS_SUCCESSFUL != rtems_semaphore_obtain(mveth_mtx, RTEMS_WAIT, RTEMS_NO_TIMEOUT) ) \
			rtems_panic(DRVNAME": unable to lock register protection mutex"); \
		} while (0)
#define REGUNLOCK()	rtems_semaphore_release(mveth_mtx)

/* THE array of driver/bsdnet structs */

STATIC
struct mveth_softc theMvEths[MV643XXETH_NUM_DRIVER_SLOTS]
= {{{{0}},}}
;

/* daemon task id */
STATIC rtems_id	mveth_tid = 0;

/* MII Ioctl Interface */

/* mdio / mii interface wrappers for rtems_mii_ioctl API */

static int mveth_mdio_r(int phy, void *uarg, unsigned reg, uint32_t *pval)
{
	if ( phy > 1 )
		return -1;

	*pval = BSP_mve_mii_read(uarg, reg);
	return 0;
}

static int mveth_mdio_w(int phy, void *uarg, unsigned reg, uint32_t val)
{
	if ( phy > 1 )
		return -1;
	BSP_mve_mii_write(uarg, reg, val);
	return 0;
}

static struct rtems_mdio_info mveth_mdio = {
	mdio_r:	  mveth_mdio_r,
	mdio_w:	  mveth_mdio_w,
	has_gmii: 1,
};

static struct mveth_private *
mve_setup_bsd(
	int		 unit,
	rtems_id tid,
	void     (*isr)(void *isr_arg),
	void     *isr_arg,
	void (*cleanup_txbuf)(void *user_buf, void *closure, int error_on_tx_occurred), 
	void *cleanup_txbuf_arg,
	void *(*alloc_rxbuf)(int *p_size, uintptr_t *p_data_addr),
	void (*consume_rxbuf)(void *user_buf, void *closure, int len),
	void *consume_rxbuf_arg,
	int		rx_ring_size,
	int		tx_ring_size,
	int		irq_mask
)
{
struct ifnet         *ifp;
struct mveth_private *mp;

	if ( unit <= 0 || unit > MV643XXETH_NUM_DRIVER_SLOTS ) {
		printk(DRVNAME": Bad unit number %i; must be 1..%i\n", unit, MV643XXETH_NUM_DRIVER_SLOTS);
		return 0;
	}

	ifp = &theMvEths[unit-1].arpcom.ac_if;

	if ( ifp->if_init ) {
		if ( ifp->if_init ) {
			printk(DRVNAME": instance %i already attached.\n", unit);
			return 0;
		}
	}

	mp  = theMvEths[unit-1].pvt;

	if ( ! (mp = BSP_mve_create(
				unit,
				tid,
				isr, isr_arg,
				cleanup_txbuf, cleanup_txbuf_arg,
				alloc_rxbuf,
				consume_rxbuf, consume_rxbuf_arg,
				rx_ring_size, tx_ring_size,
				irq_mask)) ) {
		return 0;
	}
	theMvEths[unit-1].pvt = mp;

	/* lazy init of mutex (non thread-safe! - we assume 1st initialization is single-threaded) */
	if ( ! mveth_mtx ) {
		rtems_status_code sc;
		sc = rtems_semaphore_create(
				rtems_build_name('m','v','e','X'),
				1,
				RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY | RTEMS_DEFAULT_ATTRIBUTES,
				0,
				&mveth_mtx);
		if ( RTEMS_SUCCESSFUL != sc ) {
			rtems_error(sc,DRVNAME": creating mutex\n");
			rtems_panic("unable to proceed\n");
		}
	}

	theMvEths[unit-1].bsd.tx_ring_size = tx_ring_size;
	theMvEths[unit-1].bsd.rx_ring_size = rx_ring_size;

	/* mark as used */
	ifp->if_init = (void*)(-1);

	return mp;
}

struct mveth_private *
BSP_mve_setup(
	int		 unit,
	rtems_id tid,
	void (*cleanup_txbuf)(void *user_buf, void *closure, int error_on_tx_occurred), 
	void *cleanup_txbuf_arg,
	void *(*alloc_rxbuf)(int *p_size, uintptr_t *p_data_addr),
	void (*consume_rxbuf)(void *user_buf, void *closure, int len),
	void *consume_rxbuf_arg,
	int		rx_ring_size,
	int		tx_ring_size,
	int		irq_mask
)
{
	if ( irq_mask && 0 == tid ) {
		printk(DRVNAME": must supply a TID if irq_msk not zero\n");
		return 0;	
	}

	return mve_setup_bsd(
				unit,
				tid,
				0, 0,
				cleanup_txbuf, cleanup_txbuf_arg,
				alloc_rxbuf,
				consume_rxbuf, consume_rxbuf_arg,
				rx_ring_size, tx_ring_size,
				irq_mask);
}

struct mveth_private *
BSP_mve_setup_1(
	int		 unit,
	void     (*isr)(void *isr_arg),
	void     *isr_arg,
	void (*cleanup_txbuf)(void *user_buf, void *closure, int error_on_tx_occurred), 
	void *cleanup_txbuf_arg,
	void *(*alloc_rxbuf)(int *p_size, uintptr_t *p_data_addr),
	void (*consume_rxbuf)(void *user_buf, void *closure, int len),
	void *consume_rxbuf_arg,
	int		rx_ring_size,
	int		tx_ring_size,
	int		irq_mask
)
{
	if ( irq_mask && 0 == isr ) {
		printk(DRVNAME": must supply an ISR if irq_msk not zero\n");
		return 0;	
	}

	return mve_setup_bsd(
				unit,
				0,
				isr, isr_arg,
				cleanup_txbuf, cleanup_txbuf_arg,
				alloc_rxbuf,
				consume_rxbuf, consume_rxbuf_arg,
				rx_ring_size, tx_ring_size,
				irq_mask);
}

/* allocate a new cluster and copy an existing chain there;
 * old chain is released...
 */
static struct mbuf *
repackage_chain(struct mbuf *m_head)
{
struct mbuf *m;
	MGETHDR(m, M_DONTWAIT, MT_DATA);

	if ( !m ) {
		goto bail;
	}

	MCLGET(m, M_DONTWAIT);

	if ( !(M_EXT & m->m_flags) ) {
		m_freem(m);
		m = 0;
		goto bail;
	}

	m_copydata(m_head, 0, MCLBYTES, mtod(m, caddr_t));
	m->m_pkthdr.len = m->m_len = m_head->m_pkthdr.len;

bail:
	m_freem(m_head);
	return m;
}

static MveEthBufIter *fillIter(struct mbuf *m, BsdMveIter *it)
{
	if ( (it->m = (void*)m) ) {
		it->it.data = mtod(m, void*);
		it->it.len  = m->m_len;
		it->it.uptr = m->m_next ? it->h : 0;
		return (MveEthBufIter*)it;
	}
	return 0;
}

static MveEthBufIter *nextBuf(MveEthBufIter *arg)
{
BsdMveIter *it = (BsdMveIter*)arg;
	return fillIter( it->m->m_next, it );
}

int
BSP_mve_send_buf(struct mveth_private *mp, void *m_head, void *data_p, int len)
{
int						rval;
register struct mbuf	*m1;
BsdMveIter              iter;

/* Only way to get here is when we discover that the mbuf chain
 * is too long for the tx ring
 */
startover:

	rval = 0;

#ifdef MVETH_TESTING 
	assert(m_head);
#endif

	if ( ! (m1 = m_head) )
		return 0;

	/* find first mbuf with actual data */
	while ( 0 == m1->m_len ) {
		if ( ! (m1 = m1->m_next) ) {
			/* end reached and still no data to send ?? */
			m_freem(m_head);
			return 0;
		}
	}

#ifdef MVETH_DEBUG_TX_DUMP
	if ( (mveth_tx_dump & (1<<mp->port_num)) ) {
		int ll,kk;
		struct mbuf *m;
		for ( kk=0, m=m_head; m; m=m->m_next) {
			for ( ll=0; ll<m->m_len; ll++ ) {
				printf("%02X ",*(mtod(m,char*) + ll));
				if ( ((++kk)&0xf) == 0 )
					printf("\n");
			}
		}
		printf("\n");
	}
#endif

	fillIter( m_head, &iter );
	iter.h = m_head;

	rval = BSP_mve_send_buf_chain( mp, nextBuf, &iter.it );

	if ( -2 == rval ) {
		if ( ! (m_head = repackage_chain( m_head )) ) {
			/* no cluster available */
			/* No access to this counter, unfortunately			
			mp->stats.odrops++;
			 */
			return 0;
		}
		goto startover;
	}

	return rval;
}

int
BSP_mve_media_ioctl(struct mveth_private *mp, int cmd, int *parg)
{
int rval;
	/* alias cmd == 0,1 */
	switch ( cmd ) {
		case 0: cmd = SIOCGIFMEDIA;
			break;
		case 1: cmd = SIOCSIFMEDIA;
		case SIOCGIFMEDIA:
		case SIOCSIFMEDIA:
			break;
		default: return -1;
	}
	REGLOCK();
	rval = rtems_mii_ioctl(&mveth_mdio, mp, cmd, parg);
	REGUNLOCK();
	return rval;
}

/* Translate IFFLAGS to low-level driver representation */
static int
xlateMediaFlags(int media)
{
int lowLevelFlags = 0;

	if ( IFM_LINK_OK & media ) {
		lowLevelFlags |= MV643XX_MEDIA_LINK;

		if ( IFM_FDX & media ) {
			lowLevelFlags |= MV643XX_MEDIA_FD;
		}

		switch ( IFM_SUBTYPE(media) ) {
			default: /* treat as 10 */
				lowLevelFlags |= MV643XX_MEDIA_10;
				break;
			case IFM_100_TX:
				lowLevelFlags |= MV643XX_MEDIA_100;
				break;
			case IFM_1000_T:
				lowLevelFlags |= MV643XX_MEDIA_1000;
				break;
		}
	}
	return lowLevelFlags;
}

int
BSP_mve_ack_link_chg(struct mveth_private *mp, int *pmedia)
{
int media = IFM_MAKEWORD(0,0,0,0);

	if ( 0 == BSP_mve_media_ioctl(mp, SIOCGIFMEDIA, &media)) {
		if ( IFM_LINK_OK & media ) {
			int serport_cfg = xlateMediaFlags( media );

			BSP_mve_update_serial_port(mp, serport_cfg);
		}
		if ( pmedia )
			*pmedia = media;
		return 0;
	}
	return -1;
}

/* BSDNET SUPPORT/GLUE ROUTINES */

static void
mveth_set_filters(struct ifnet *ifp);

STATIC void
mveth_stop(struct mveth_softc *sc)
{
	BSP_mve_stop_hw(sc->pvt);
	sc->arpcom.ac_if.if_timer = 0;
}

/* allocate a mbuf for RX with a properly aligned data buffer
 * RETURNS 0 if allocation fails
 */
static void *
alloc_mbuf_rx(int *psz, uintptr_t *paddr)
{
struct mbuf		*m;
unsigned long	l,o;

	MGETHDR(m, M_DONTWAIT, MT_DATA);
	if ( !m )
		return 0;
	MCLGET(m, M_DONTWAIT);
	if ( ! (m->m_flags & M_EXT) ) {
		m_freem(m);
		return 0;
	}

	o = mtod(m, unsigned long);
	l = MV643XX_ALIGN(o, RX_BUF_ALIGNMENT) - o;

	/* align start of buffer */
	m->m_data += l;

	/* reduced length */
	l = MCLBYTES - l;

	m->m_len   = m->m_pkthdr.len = l;
	*psz       = m->m_len;
	*paddr     = mtod(m, uintptr_t); 

	return (void*) m;
}

static void consume_rx_mbuf(void *buf, void *arg, int len)
{
struct ifnet *ifp = arg;
struct mbuf    *m = buf;

	if ( len <= 0 ) {
		ifp->if_iqdrops++;
		if ( len < 0 ) {
			ifp->if_ierrors++;
		}
		if ( m )
			m_freem(m);
	} else {
		struct ether_header *eh;

			eh			= (struct ether_header *)(mtod(m, unsigned long) + ETH_RX_OFFSET);
			m->m_len	= m->m_pkthdr.len = len - sizeof(struct ether_header) - ETH_RX_OFFSET - ETH_CRC_LEN;
			m->m_data  += sizeof(struct ether_header) + ETH_RX_OFFSET;
			m->m_pkthdr.rcvif = ifp;

			ifp->if_ipackets++;
			ifp->if_ibytes  += m->m_pkthdr.len;
			
			if (0) {
				/* Low-level debugging */
				int i;
				for (i=0; i<13; i++) {
					printf("%02X:",((char*)eh)[i]);
				}
				printf("%02X\n",((char*)eh)[i]);
				for (i=0; i<m->m_len; i++) {
					if ( !(i&15) )
						printf("\n");
					printf("0x%02x ",mtod(m,char*)[i]);
				}
				printf("\n");
			}

			if (0) {
				/* Low-level debugging/testing without bsd stack */
				m_freem(m);
			} else {
				/* send buffer upwards */
				ether_input(ifp, eh, m);
			}
	}
}

static void release_tx_mbuf(void *buf, void *arg, int err)
{
struct ifnet *ifp = arg;
struct mbuf  *mb  = buf;

	if ( err ) {
		ifp->if_oerrors++;
	} else {
		ifp->if_opackets++;
	}
	ifp->if_obytes += mb->m_pkthdr.len;
	m_freem(mb);
}

/* BSDNET DRIVER CALLBACKS */

static void
mveth_init(void *arg)
{
struct mveth_softc	*sc  = arg;
struct ifnet		*ifp = &sc->arpcom.ac_if;
int                 media;
int                 lowLevelMediaStatus;

	media = IFM_MAKEWORD(0, 0, 0, 0);
	if ( 0 == BSP_mve_media_ioctl(sc->pvt, SIOCGIFMEDIA, &media) ) {
	    if ( (IFM_LINK_OK & media) ) {
			ifp->if_flags &= ~IFF_OACTIVE;
		} else {
			ifp->if_flags |= IFF_OACTIVE;
		}
	}

	lowLevelMediaStatus = xlateMediaFlags( media );

	BSP_mve_init_hw(sc->pvt, ifp->if_flags & IFF_PROMISC, sc->arpcom.ac_enaddr, lowLevelMediaStatus);


	/* if promiscuous then there is no need to change */
	if ( ! (ifp->if_flags & IFF_PROMISC) )
		mveth_set_filters(ifp);

	ifp->if_flags |= IFF_RUNNING;
	sc->arpcom.ac_if.if_timer = 0;
}

/* bsdnet driver entry to start transmission */
static void
mveth_start(struct ifnet *ifp)
{
struct mveth_softc	*sc = ifp->if_softc;
struct mbuf			*m  = 0;

	while ( ifp->if_snd.ifq_head ) {
		IF_DEQUEUE( &ifp->if_snd, m );
		if ( BSP_mve_send_buf(sc->pvt, m, 0, 0) < 0 ) {
			IF_PREPEND( &ifp->if_snd, m);
			ifp->if_flags |= IFF_OACTIVE;
			break;
		}
		/* need to do this really only once
		 * but it's cheaper this way.
		 */
		ifp->if_timer = 2*IFNET_SLOWHZ;
	}
}

/* bsdnet driver entry; */
static void
mveth_watchdog(struct ifnet *ifp)
{
struct mveth_softc	*sc = ifp->if_softc;

	ifp->if_oerrors++;
	printk(DRVNAME"%i: watchdog timeout; resetting\n", ifp->if_unit);

	mveth_init(sc);
	mveth_start(ifp);
}

static void
mveth_set_filters(struct ifnet *ifp)
{
struct mveth_softc  *sc = ifp->if_softc;

	BSP_mve_promisc_set( sc->pvt, !!(ifp->if_flags & IFF_PROMISC));

	if ( ifp->if_flags & (IFF_PROMISC | IFF_ALLMULTI) ) {
		BSP_mve_mcast_filter_accept_all(sc->pvt);
	} else {
		struct ether_multi     *enm;
		struct ether_multistep step;

		BSP_mve_mcast_filter_clear( sc->pvt );
		
		ETHER_FIRST_MULTI(step, (struct arpcom *)ifp, enm);

		while ( enm ) {
			if ( memcmp(enm->enm_addrlo, enm->enm_addrhi, ETHER_ADDR_LEN) )
				assert( !"Should never get here; IFF_ALLMULTI should be set!" );

			BSP_mve_mcast_filter_accept_add(sc->pvt, enm->enm_addrlo);

			ETHER_NEXT_MULTI(step, enm);
		}
	}
}

/* bsdnet driver ioctl entry */
static int
mveth_ioctl(struct ifnet *ifp, ioctl_command_t cmd, caddr_t data)
{
struct mveth_softc	*sc   = ifp->if_softc;
struct ifreq		*ifr  = (struct ifreq *)data;
int					error = 0;
int					f;

	switch ( cmd ) {
  		case SIOCSIFFLAGS:
			f = ifp->if_flags;
			if ( f & IFF_UP ) {
				if ( ! ( f & IFF_RUNNING ) ) {
					mveth_init(sc);
				} else {
					if ( (f & IFF_PROMISC) != (sc->bsd.oif_flags & IFF_PROMISC) ) {
						mveth_set_filters(ifp);
					}
					/* FIXME: other flag changes are ignored/unimplemented */
				}
			} else {
				if ( f & IFF_RUNNING ) {
					mveth_stop(sc);
					ifp->if_flags  &= ~(IFF_RUNNING | IFF_OACTIVE);
				}
			}
			sc->bsd.oif_flags = ifp->if_flags;
		break;

  		case SIOCGIFMEDIA:
  		case SIOCSIFMEDIA:
			error = BSP_mve_media_ioctl(sc->pvt, cmd, &ifr->ifr_media);
		break;
 
		case SIOCADDMULTI:
		case SIOCDELMULTI:
			error = (cmd == SIOCADDMULTI)
		    		? ether_addmulti(ifr, &sc->arpcom)
				    : ether_delmulti(ifr, &sc->arpcom);

			if (error == ENETRESET) {
				if (ifp->if_flags & IFF_RUNNING) {
					mveth_set_filters(ifp);
				}
				error = 0;
			}
		break;


		break;

 		case SIO_RTEMS_SHOW_STATS:
			BSP_mve_dump_stats(sc->pvt, stdout);
		break;

		default:
			error = ether_ioctl(ifp, cmd, data);
		break;
	}

	return error;
}

/* DRIVER TASK */

static void mveth_isr(void *arg)
{
struct mveth_softc	*sc = (struct mveth_softc*) arg;
int                 idx = (sc - &theMvEths[0]);
	BSP_mve_disable_irqs( sc->pvt );
#ifdef MVETH_DEBUG
	printk(DRVNAME": bsdnet isr; posting event %i to 0x%08x\n", idx, BSP_mve_get_tid( sc->pvt ));
#endif
	rtems_bsdnet_event_send( BSP_mve_get_tid( sc->pvt ), (1<<idx) );
}

/* Daemon task does all the 'interrupt' work */
static void mveth_daemon(void *arg)
{
struct mveth_softc	*sc;
struct ifnet		*ifp;
rtems_event_set		evs;
int                 avail;

#ifdef MVETH_DEBUG
	sleep(1);
	printk(DRVNAME": bsdnet mveth_daemon started\n");
#endif

	for (;;) {
		rtems_bsdnet_event_receive( 7, RTEMS_WAIT | RTEMS_EVENT_ANY, RTEMS_NO_TIMEOUT, &evs );
#ifdef MVETH_DEBUG
		printk(DRVNAME": bsdnet mveth_daemon event received 0x%x\n", evs);
#endif
		evs &= 7;
		for ( sc = theMvEths; evs; evs>>=1, sc++ ) {
			if ( (evs & 1) ) {
				register uint32_t x;

				ifp = &sc->arpcom.ac_if;

				if ( !(ifp->if_flags & IFF_UP) ) {
					mveth_stop(sc);
					ifp->if_flags &= ~(IFF_UP|IFF_RUNNING);
					continue;
				}

				if ( !(ifp->if_flags & IFF_RUNNING) ) {
					/* event could have been pending at the time hw was stopped;
					 * just ignore...
					 */
					continue;
				}

				x = BSP_mve_ack_irqs(sc->pvt);

				if ( MV643XX_ETH_EXT_IRQ_LINK_CHG & x ) {
					/* phy status changed */
					int media;

					if ( 0 == BSP_mve_ack_link_chg(sc->pvt, &media) ) {
						if ( IFM_LINK_OK & media ) {
							ifp->if_flags &= ~IFF_OACTIVE;
							mveth_start(ifp);
						} else {
							/* stop sending */
							ifp->if_flags |= IFF_OACTIVE;
						}
					}
				}
				/* free tx chain */
				if ( (MV643XX_ETH_EXT_IRQ_TX_DONE & x) && (avail = BSP_mve_swipe_tx(sc->pvt)) > 0 ) {
					ifp->if_flags &= ~IFF_OACTIVE;
					if ( sc->bsd.tx_ring_size == avail )
						ifp->if_timer = 0;
					mveth_start(ifp);
				}
				if ( (MV643XX_ETH_IRQ_RX_DONE & x) )
					BSP_mve_swipe_rx(sc->pvt);

				BSP_mve_enable_irqs(sc->pvt);
			}
		}
	}
}

/* PUBLIC RTEMS BSDNET ATTACH FUNCTION */
int
rtems_mve_attach(struct rtems_bsdnet_ifconfig *ifcfg, int attaching)
{
char				*unitName;
int					unit,i,cfgUnits;
struct	mveth_softc *sc;
struct	ifnet		*ifp;

	unit = rtems_bsdnet_parse_driver_name(ifcfg, &unitName);
	if ( unit <= 0 || unit > MV643XXETH_NUM_DRIVER_SLOTS ) {
		printk(DRVNAME": Bad unit number %i; must be 1..%i\n", unit, MV643XXETH_NUM_DRIVER_SLOTS);
		return 1;
	}

	sc  = &theMvEths[unit-1];
	ifp = &sc->arpcom.ac_if;

	if ( attaching ) {
		if ( ifp->if_init ) {
			printk(DRVNAME": instance %i already attached.\n", unit);
			return -1;
		}

		for ( i=cfgUnits = 0; i<MV643XXETH_NUM_DRIVER_SLOTS; i++ ) {
			if ( theMvEths[i].arpcom.ac_if.if_init )
				cfgUnits++;
		}
		cfgUnits++; /* this new one */

		/* lazy init of TID should still be thread-safe because we are protected
		 * by the global networking semaphore..
		 */
		if ( !mveth_tid ) {
			/* newproc uses the 1st 4 chars of name string to build an rtems name */
			mveth_tid = rtems_bsdnet_newproc("MVEd", 4096, mveth_daemon, 0);
		}

#ifdef MVETH_DEBUG
		printk(DRVNAME": daemon created; id 0x%08x\n", mveth_tid);
#endif

		if ( 0 == ifcfg->rbuf_count ) {
			ifcfg->rbuf_count = MV643XX_RX_RING_SIZE;
		}

		if ( 0 == ifcfg->xbuf_count ) {
			ifcfg->xbuf_count = MV643XX_TX_RING_SIZE;
		}

		if ( !mve_setup_bsd( unit,
						     mveth_tid,
		                     mveth_isr, (void*)sc,
						     release_tx_mbuf, ifp,
						     alloc_mbuf_rx,
						     consume_rx_mbuf, ifp,
						     ifcfg->rbuf_count,
						     ifcfg->xbuf_count,
			                 BSP_MVE_IRQ_TX | BSP_MVE_IRQ_RX | BSP_MVE_IRQ_LINK) ) {
			return -1;
		}

		if ( nmbclusters < ifcfg->rbuf_count * cfgUnits + 60 /* arbitrary */ )  {
			printk(DRVNAME"%i: (mv643xx ethernet) Your application has not enough mbuf clusters\n", unit);
			printk(     "                         configured for this driver.\n");
			return -1;
		}

		if ( ifcfg->hardware_address ) {
			memcpy(sc->arpcom.ac_enaddr, ifcfg->hardware_address, ETHER_ADDR_LEN);
		} else {
			/* read back from hardware assuming that MotLoad already had set it up */
			BSP_mve_read_eaddr(sc->pvt, sc->arpcom.ac_enaddr);
		}

		ifp->if_softc			= sc;
		ifp->if_unit			= unit;
		ifp->if_name			= unitName;

		ifp->if_mtu				= ifcfg->mtu ? ifcfg->mtu : ETHERMTU;

		ifp->if_init			= mveth_init;
		ifp->if_ioctl			= mveth_ioctl;
		ifp->if_start			= mveth_start;
		ifp->if_output			= ether_output;
		/*
		 * While nonzero, the 'if->if_timer' is decremented
		 * (by the networking code) at a rate of IFNET_SLOWHZ (1hz) and 'if_watchdog'
		 * is called when it expires. 
		 * If either of those fields is 0 the feature is disabled.
		 */
		ifp->if_watchdog		= mveth_watchdog;
		ifp->if_timer			= 0;

		sc->bsd.oif_flags		= /* ... */
		ifp->if_flags			= IFF_BROADCAST | IFF_MULTICAST | IFF_SIMPLEX;

		/*
		 * if unset, this set to 10Mbps by ether_ifattach; seems to be unused by bsdnet stack;
		 * could be updated along with phy speed, though...
		ifp->if_baudrate		= 10000000;
		*/

		/* NOTE: ether_output drops packets if ifq_len >= ifq_maxlen
		 *       but this is the packet count, not the fragment count!
		ifp->if_snd.ifq_maxlen	= sc->pvt->buf_count;
		*/
		ifp->if_snd.ifq_maxlen	= ifqmaxlen;

		{
			if_attach(ifp);
			ether_ifattach(ifp);
		}

	} else {
		printk(DRVNAME": interface detaching not implemented\n");
		return -1;
	}

	return 0;
}

/* EARLY PHY ACCESS */
static int
mveth_early_init(int idx)
{
	if ( idx < 0 || idx >= MV643XXETH_NUM_DRIVER_SLOTS )
		return -1;

	return 0;
}

static int
mveth_early_read_phy(int idx, unsigned reg)
{
int rval;

	if ( idx < 0 || idx >= MV643XXETH_NUM_DRIVER_SLOTS )
		return -1;

	rval = BSP_mve_mii_read_early(idx, reg);
	return rval < 0 ? rval : rval & 0xffff;
}

static int
mveth_early_write_phy(int idx, unsigned reg, unsigned val)
{
	if ( idx < 0 || idx >= MV643XXETH_NUM_DRIVER_SLOTS )
		return -1;

	BSP_mve_mii_write_early(idx, reg, val);
	return 0;
}

rtems_bsdnet_early_link_check_ops
rtems_mve_early_link_check_ops = {
	init:		mveth_early_init,
	read_phy:	mveth_early_read_phy,
	write_phy:	mveth_early_write_phy,
	name:		DRVNAME,
	num_slots:	MAX_NUM_SLOTS
};

/* DEBUGGING */

#ifdef MVETH_DEBUG
/* Display/dump descriptor rings */

int
mveth_dring(struct mveth_softc *sc)
{
		/* can't just invalidate the descriptor - if it contains
		 * data that hasn't been flushed yet, we create an inconsistency...
		 */
		rtems_bsdnet_semaphore_obtain();

		BSP_mve_dring_nosync(sc->pvt);

		rtems_bsdnet_semaphore_release();
	return 0;
}

#endif
