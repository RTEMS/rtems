/*	$NetBSD: if_gfe.c,v 1.13.8.1 2005/04/29 11:28:56 kent Exp $	*/

/*
 * Copyright (c) 2002 Allegro Networks, Inc., Wasabi Systems, Inc.
 * All rights reserved.
 *
 * Copyright 2004: Enable hardware cache snooping. Kate Feng <feng1@bnl.gov>
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
 *      This product includes software developed for the NetBSD Project by
 *      Allegro Networks, Inc., and Wasabi Systems, Inc.
 * 4. The name of Allegro Networks, Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 * 5. The name of Wasabi Systems, Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ALLEGRO NETWORKS, INC. AND
 * WASABI SYSTEMS, INC. ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL EITHER ALLEGRO NETWORKS, INC. OR WASABI SYSTEMS, INC.
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * if_gfe.c -- GT ethernet MAC driver
 */

/* Enable hardware cache snooping;
 * Copyright Shuchen K. Feng <feng1@bnl.gov>, 2004
 */

#ifdef __rtems__
#include <rtemscompat.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#endif

#include <sys/cdefs.h>
#ifndef __rtems__
__KERNEL_RCSID(0, "$NetBSD: if_gfe.c,v 1.13.8.1 2005/04/29 11:28:56 kent Exp $");

#include "opt_inet.h"
#include "bpfilter.h"
#endif

#include <sys/param.h>
#include <sys/types.h>
#ifndef __rtems__
#include <sys/inttypes.h>
#include <sys/queue.h>
#endif

#ifndef __rtems__
#include <uvm/uvm_extern.h>

#include <sys/callout.h>
#include <sys/device.h>
#endif
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/mbuf.h>
#include <sys/socket.h>

#ifndef __rtems__
#include <machine/bus.h>
#endif

#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_media.h>
#ifndef __rtems__
#include <net/if_ether.h>
#else
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <rtems/rtems_mii_ioctl.h>
#endif

#ifdef INET
#include <netinet/in.h>
#ifndef __rtems__
#include <netinet/if_inarp.h>
#endif
#endif
#if NBPFILTER > 0
#include <net/bpf.h>
#endif

#ifndef __rtems__
#include <dev/mii/miivar.h>

#include <dev/marvell/gtintrreg.h>
#include <dev/marvell/gtethreg.h>

#include <dev/marvell/gtvar.h>
#include <dev/marvell/if_gfevar.h>
#else
#include	<bsp/gtintrreg.h>
#include	<bsp/gtreg.h>
#include	"gtethreg.h"

#include	"gtvar.h"
#include	"if_gfevar.h"
#include 	<rtemscompat1.h>
#define		ether_sprintf ether_sprintf_macro
#endif

#define	GE_READ(sc, reg) \
	bus_space_read_4((sc)->sc_gt_memt, (sc)->sc_memh, ETH__ ## reg)
#define	GE_WRITE(sc, reg, v) \
	bus_space_write_4((sc)->sc_gt_memt, (sc)->sc_memh, ETH__ ## reg, (v))

#define	GT_READ(sc, reg) \
	bus_space_read_4((sc)->sc_gt_memt, (sc)->sc_gt_memh, reg)
#define	GT_WRITE(sc, reg, v) \
	bus_space_write_4((sc)->sc_gt_memt, (sc)->sc_gt_memh, reg, (v))

#define	GE_DEBUG
#if 0
#define	GE_NOHASH
#define	GE_NORX
#endif

#ifdef GE_DEBUG
#define	GE_DPRINTF(sc, a)	do \
				  if ((sc)->sc_ec.ec_if.if_flags & IFF_DEBUG) \
				    printf a; \
				while (0)
#define	GE_FUNC_ENTER(sc, func)	GE_DPRINTF(sc, ("[" func))
#define	GE_FUNC_EXIT(sc, str)	GE_DPRINTF(sc, (str "]"))
#else
#define	GE_DPRINTF(sc, a)	do { } while (0)
#define	GE_FUNC_ENTER(sc, func)	do { } while (0)
#define	GE_FUNC_EXIT(sc, str)	do { } while (0)
#endif
enum gfe_whack_op {
	GE_WHACK_START,		GE_WHACK_RESTART,
	GE_WHACK_CHANGE,	GE_WHACK_STOP
};

enum gfe_hash_op {
	GE_HASH_ADD,		GE_HASH_REMOVE,
};


#if 1
#define	htogt32(a)		htobe32(a)
#define	gt32toh(a)		be32toh(a)
#else
#define	htogt32(a)		htole32(a)
#define	gt32toh(a)		le32toh(a)
#endif

#ifdef __rtems__
#define htobe32 htonl
#define be32toh ntohl
#endif

#define GE_RXDSYNC(sc, rxq, n, ops) \
	bus_dmamap_sync((sc)->sc_dmat, (rxq)->rxq_desc_mem.gdm_map, \
	    (n) * sizeof((rxq)->rxq_descs[0]), sizeof((rxq)->rxq_descs[0]), \
	    (ops))
#define	GE_RXDPRESYNC(sc, rxq, n) \
	GE_RXDSYNC(sc, rxq, n, BUS_DMASYNC_PREREAD|BUS_DMASYNC_PREWRITE)
#define	GE_RXDPOSTSYNC(sc, rxq, n) \
	GE_RXDSYNC(sc, rxq, n, BUS_DMASYNC_POSTREAD|BUS_DMASYNC_POSTWRITE)

#define GE_TXDSYNC(sc, txq, n, ops) \
	bus_dmamap_sync((sc)->sc_dmat, (txq)->txq_desc_mem.gdm_map, \
	    (n) * sizeof((txq)->txq_descs[0]), sizeof((txq)->txq_descs[0]), \
	    (ops))
#define	GE_TXDPRESYNC(sc, txq, n) \
	GE_TXDSYNC(sc, txq, n, BUS_DMASYNC_PREREAD|BUS_DMASYNC_PREWRITE)
#define	GE_TXDPOSTSYNC(sc, txq, n) \
	GE_TXDSYNC(sc, txq, n, BUS_DMASYNC_POSTREAD|BUS_DMASYNC_POSTWRITE)

#define	STATIC

#ifndef __rtems__
STATIC int gfe_match (struct device *, struct cfdata *, void *);
STATIC void gfe_attach (struct device *, struct device *, void *);
#else
STATIC int gfe_probe (device_t);
STATIC int gfe_attach (device_t);
STATIC void gfe_init (void*);
#endif

STATIC int gfe_dmamem_alloc(struct gfe_softc *, struct gfe_dmamem *, int,
	size_t, int);
STATIC void gfe_dmamem_free(struct gfe_softc *, struct gfe_dmamem *);

#ifndef __rtems__
STATIC int gfe_ifioctl (struct ifnet *, u_long, caddr_t);
#else
STATIC int gfe_ifioctl (struct ifnet *, ioctl_command_t, caddr_t);
#endif
STATIC void gfe_ifstart (struct ifnet *);
STATIC void gfe_ifwatchdog (struct ifnet *);

#ifndef __rtems__
STATIC int gfe_mii_mediachange (struct ifnet *);
STATIC void gfe_mii_mediastatus (struct ifnet *, struct ifmediareq *);
STATIC int gfe_mii_read (struct device *, int, int);
STATIC void gfe_mii_write (struct device *, int, int, int);
STATIC void gfe_mii_statchg (struct device *);
#endif

STATIC void gfe_tick(void *arg);

STATIC void gfe_tx_restart(void *);
STATIC int gfe_tx_enqueue(struct gfe_softc *, enum gfe_txprio);
STATIC uint32_t gfe_tx_done(struct gfe_softc *, enum gfe_txprio, uint32_t);
STATIC void gfe_tx_cleanup(struct gfe_softc *, enum gfe_txprio, int);
STATIC int gfe_tx_txqalloc(struct gfe_softc *, enum gfe_txprio);
STATIC int gfe_tx_start(struct gfe_softc *, enum gfe_txprio);
STATIC void gfe_tx_stop(struct gfe_softc *, enum gfe_whack_op);

STATIC void gfe_rx_cleanup(struct gfe_softc *, enum gfe_rxprio);
STATIC void gfe_rx_get(struct gfe_softc *, enum gfe_rxprio);
STATIC int gfe_rx_prime(struct gfe_softc *);
STATIC uint32_t gfe_rx_process(struct gfe_softc *, uint32_t, uint32_t);
STATIC int gfe_rx_rxqalloc(struct gfe_softc *, enum gfe_rxprio);
STATIC int gfe_rx_rxqinit(struct gfe_softc *, enum gfe_rxprio);
STATIC void gfe_rx_stop(struct gfe_softc *, enum gfe_whack_op);

STATIC int gfe_intr(void *);

STATIC int gfe_whack(struct gfe_softc *, enum gfe_whack_op);

STATIC int gfe_hash_compute(struct gfe_softc *, const uint8_t [ETHER_ADDR_LEN]);
STATIC int gfe_hash_entry_op(struct gfe_softc *, enum gfe_hash_op,
	enum gfe_rxprio, const uint8_t [ETHER_ADDR_LEN]);
#ifndef __rtems__
STATIC int gfe_hash_multichg(struct ethercom *, const struct ether_multi *,
	u_long);
#endif
STATIC int gfe_hash_fill(struct gfe_softc *);
STATIC int gfe_hash_alloc(struct gfe_softc *);

#ifndef __rtems__
/* Linkup to the rest of the kernel */
CFATTACH_DECL(gfe, sizeof(struct gfe_softc),
    gfe_match, gfe_attach, NULL, NULL);
#else
net_drv_tbl_t METHODS = {
    n_probe  : gfe_probe,
    n_attach : gfe_attach,
    n_detach : 0,
    n_intr   : (void (*)(void*))gfe_intr,
};

int
gfe_mii_read(int phy, void *arg, unsigned reg, uint32_t *pval);
int
gfe_mii_write(int phy, void *arg, unsigned reg, uint32_t val);

struct rtems_mdio_info
gfe_mdio_access = {
	mdio_r:	gfe_mii_read,
	mdio_w:	gfe_mii_write,
	has_gmii: 0
};

#endif

extern struct cfdriver gfe_cd;

#ifndef __rtems__
int
gfe_match(struct device *parent, struct cfdata *cf, void *aux)
{
	struct gt_softc *gt = (struct gt_softc *) parent;
	struct gt_attach_args *ga = aux;
	uint8_t enaddr[6];

	if (!GT_ETHEROK(gt, ga, &gfe_cd))
		return 0;

	if (gtget_macaddr(gt, ga->ga_unit, enaddr) < 0)
		return 0;

	if (enaddr[0] == 0 && enaddr[1] == 0 && enaddr[2] == 0 &&
	    enaddr[3] == 0 && enaddr[4] == 0 && enaddr[5] == 0)
		return 0;

	return 1;
}
#else
int
gfe_probe(device_t dev)
{
	switch ( BSP_getDiscoveryVersion(0) ) {
		case GT_64260_A:
		case GT_64260_B:
			return 0;
		default:
			break;
	}
	return -1;
}

void
gfe_init(void *arg)
{
struct gfe_softc *sc = arg;
	if ( sc->sc_ec.ec_if.if_flags & IFF_RUNNING )
		gfe_whack(sc, GE_WHACK_RESTART);
	else
		gfe_whack(sc, GE_WHACK_START);
}
#endif

/*
 * Attach this instance, and then all the sub-devices
 */
#ifndef __rtems__
void
gfe_attach(struct device *parent, struct device *self, void *aux)
#else
int
gfe_attach(device_t dev)
#endif
{
#ifndef __rtems__
	struct gt_attach_args * const ga = aux;
	struct gt_softc * const gt = (struct gt_softc *) parent;
	struct gfe_softc * const sc = (struct gfe_softc *) self;
#else
	struct gfe_softc * const sc = device_get_softc(dev);
#endif
	struct ifnet * const ifp = &sc->sc_ec.ec_if;
	uint32_t data;
	uint8_t enaddr[6];
	int phyaddr;
	uint32_t sdcr;
	int error;
#ifdef __rtems__
	SPRINTFVARDECL;
#endif

#ifndef __rtems__
	GT_ETHERFOUND(gt, ga);

	sc->sc_gt_memt = ga->ga_memt;
	sc->sc_gt_memh = ga->ga_memh;
	sc->sc_dmat = ga->ga_dmat;
	sc->sc_macno = ga->ga_unit;

	if (bus_space_subregion(sc->sc_gt_memt, sc->sc_gt_memh,
		    ETH_BASE(sc->sc_macno), ETH_SIZE, &sc->sc_memh)) {
		aprint_error(": failed to map registers\n");
	}
	
	callout_init(&sc->sc_co);
#else
	/* sc_macno, irq_no and sc_gt_memh must be filled in by 'setup' */

	/* make ring sizes even numbers so that we have always multiple
	 * cache lines (paranoia)
	 */
	if ( (sc->num_rxdesc = dev->d_ifconfig->rbuf_count) & 1 )
		sc->num_rxdesc++;
	if ( 0 == sc->num_rxdesc )
		sc->num_rxdesc = 64;

	if ( (sc->num_txdesc = dev->d_ifconfig->xbuf_count) & 1 )
		sc->num_txdesc++;
	if ( 0 == sc->num_txdesc )
		sc->num_txdesc = 256;

	/* Enable hardware cache snooping;
         * Copyright Shuchen K. Feng <feng1@bnl.gov>, 2004
         */
	/* regs are eth0: 0xf200/0xf204, eth1 0xf220/0xf224, eth2: 0xf240/0xf244 */
	{
	uint32_t v;
	v = GT_READ(sc, ETH_ACTL_0_LO + (sc->sc_macno<<5));
	v |= RxBSnoopEn|TxBSnoopEn|RxDSnoopEn|TxDSnoopEn;
	GT_WRITE(sc, ETH_ACTL_0_LO + (sc->sc_macno<<5), v);

	v = GT_READ(sc, ETH_ACTL_0_HI + (sc->sc_macno<<5));
	v |= HashSnoopEn;
	GT_WRITE(sc, ETH_ACTL_0_HI + (sc->sc_macno<<5), v);
	}

#endif

	data = bus_space_read_4(sc->sc_gt_memt, sc->sc_gt_memh, ETH_EPAR);
#ifdef __rtems__
	sc->sc_phyaddr =
#endif
	phyaddr = ETH_EPAR_PhyAD_GET(data, sc->sc_macno);

#ifndef __rtems__
	gtget_macaddr(gt, sc->sc_macno, enaddr);
#else
	memset( enaddr, 0, ETHER_ADDR_LEN );
	if ( !memcmp(enaddr, sc->arpcom.ac_enaddr, ETHER_ADDR_LEN) ) {
		aprint_error(": MAC address not set (pass to rtems_gfe_setup())\n");
		return -1;
	}
	/* mac address needs to be provided by 'setup' */
	memcpy(enaddr, sc->arpcom.ac_enaddr, ETHER_ADDR_LEN);
#endif

	sc->sc_pcr = GE_READ(sc, EPCR);
	sc->sc_pcxr = GE_READ(sc, EPCXR);
	sc->sc_intrmask = GE_READ(sc, EIMR) | ETH_IR_MIIPhySTC;

	aprint_normal(": address %s", ether_sprintf(enaddr));

#if defined(DEBUG)
	aprint_normal(", pcr %#x, pcxr %#x", sc->sc_pcr, sc->sc_pcxr);
#endif

	sc->sc_pcxr &= ~ETH_EPCXR_PRIOrx_Override;
#ifndef __rtems__
	if (sc->sc_dev.dv_cfdata->cf_flags & 1) {
		aprint_normal(", phy %d (rmii)", phyaddr);
		sc->sc_pcxr |= ETH_EPCXR_RMIIEn;
	} else
#endif
	{
		aprint_normal(", phy %d (mii)", phyaddr);
		sc->sc_pcxr &= ~ETH_EPCXR_RMIIEn;
	}
#ifndef __rtems__
	if (sc->sc_dev.dv_cfdata->cf_flags & 2)
		sc->sc_flags |= GE_NOFREE;
#endif
	sc->sc_pcxr &= ~(3 << 14);
	sc->sc_pcxr |= (ETH_EPCXR_MFL_1536 << 14);

	if (sc->sc_pcr & ETH_EPCR_EN) {
		int tries = 1000;
		/*
		 * Abort transmitter and receiver and wait for them to quiese
		 */
		GE_WRITE(sc, ESDCMR, ETH_ESDCMR_AR|ETH_ESDCMR_AT);
		do {
			delay(100);
		} while (tries-- > 0 && (GE_READ(sc, ESDCMR) & (ETH_ESDCMR_AR|ETH_ESDCMR_AT)));
	}

	sc->sc_pcr &= ~(ETH_EPCR_EN | ETH_EPCR_RBM | ETH_EPCR_PM | ETH_EPCR_PBF);

#if defined(DEBUG)
	aprint_normal(", pcr %#x, pcxr %#x", sc->sc_pcr, sc->sc_pcxr);
#endif

	/*
	 * Now turn off the GT.  If it didn't quiese, too ***ing bad.
	 */
	GE_WRITE(sc, EPCR, sc->sc_pcr);
#ifndef __rtems__
	GE_WRITE(sc, EIMR, sc->sc_intrmask);
#else
	GE_WRITE(sc, EICR, 0);
	GE_WRITE(sc, EIMR, 0);
#endif
	sdcr = GE_READ(sc, ESDCR);
	ETH_ESDCR_BSZ_SET(sdcr, ETH_ESDCR_BSZ_4);
	sdcr |= ETH_ESDCR_RIFB;
	GE_WRITE(sc, ESDCR, sdcr);
	sc->sc_max_frame_length = 1536;

	aprint_normal("\n");
#ifndef __rtems__
	sc->sc_mii.mii_ifp = ifp;
	sc->sc_mii.mii_readreg = gfe_mii_read;
	sc->sc_mii.mii_writereg = gfe_mii_write;
	sc->sc_mii.mii_statchg = gfe_mii_statchg;

	ifmedia_init(&sc->sc_mii.mii_media, 0, gfe_mii_mediachange,
		gfe_mii_mediastatus);

	mii_attach(&sc->sc_dev, &sc->sc_mii, 0xffffffff, phyaddr,
		MII_OFFSET_ANY, MIIF_NOISOLATE);
	if (LIST_FIRST(&sc->sc_mii.mii_phys) == NULL) {
		ifmedia_add(&sc->sc_mii.mii_media, IFM_ETHER|IFM_NONE, 0, NULL);
		ifmedia_set(&sc->sc_mii.mii_media, IFM_ETHER|IFM_NONE);
	} else {
		ifmedia_set(&sc->sc_mii.mii_media, IFM_ETHER|IFM_AUTO);
	}

	strcpy(ifp->if_xname, sc->sc_dev.dv_xname);
#else
	if_initname(ifp, device_get_name(dev), device_get_unit(dev));
	ifp->if_mtu   = ETHERMTU;
	ifp->if_output = ether_output;
	ifp->if_init   = gfe_init;
	ifp->if_snd.ifq_maxlen = GE_TXDESC_MAX - 1;
	ifp->if_baudrate = 10000000;
#endif
	ifp->if_softc = sc;
	/* ifp->if_mowner = &sc->sc_mowner; */
	ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX | IFF_MULTICAST;
#if 0
	ifp->if_flags |= IFF_DEBUG;
#endif
	ifp->if_ioctl = gfe_ifioctl;
	ifp->if_start = gfe_ifstart;
	ifp->if_watchdog = gfe_ifwatchdog;

	if (sc->sc_flags & GE_NOFREE) {
		error = gfe_rx_rxqalloc(sc, GE_RXPRIO_HI);
		if (!error)
			error = gfe_rx_rxqalloc(sc, GE_RXPRIO_MEDHI);
		if (!error)
			error = gfe_rx_rxqalloc(sc, GE_RXPRIO_MEDLO);
		if (!error)
			error = gfe_rx_rxqalloc(sc, GE_RXPRIO_LO);
		if (!error)
			error = gfe_tx_txqalloc(sc, GE_TXPRIO_HI);
		if (!error)
			error = gfe_hash_alloc(sc);
		if (error)
			aprint_error(
			    "%s: failed to allocate resources: %d\n",
			    ifp->if_xname, error);
	}

	if_attach(ifp);
#ifndef __rtems__
	ether_ifattach(ifp, enaddr);
#else
	ether_ifattach(ifp);
#endif
#if NBPFILTER > 0
	bpfattach(ifp, DLT_EN10MB, sizeof(struct ether_header));
#endif
#if NRND > 0
	rnd_attach_source(&sc->sc_rnd_source, self->dv_xname, RND_TYPE_NET, 0);
#endif
#ifndef __rtems__
	intr_establish(IRQ_ETH0 + sc->sc_macno, IST_LEVEL, IPL_NET,
	    gfe_intr, sc);
#else
	return 0;
#endif
}

int
gfe_dmamem_alloc(struct gfe_softc *sc, struct gfe_dmamem *gdm, int maxsegs,
	size_t size, int flags)
{
	int error = 0;
	GE_FUNC_ENTER(sc, "gfe_dmamem_alloc");

	KASSERT(gdm->gdm_kva == NULL);
	gdm->gdm_size = size;
	gdm->gdm_maxsegs = maxsegs;

#ifndef __rtems__
	error = bus_dmamem_alloc(sc->sc_dmat, gdm->gdm_size, PAGE_SIZE,
	    gdm->gdm_size, gdm->gdm_segs, gdm->gdm_maxsegs, &gdm->gdm_nsegs,
	    BUS_DMA_NOWAIT);
	if (error)
		goto fail;

	error = bus_dmamem_map(sc->sc_dmat, gdm->gdm_segs, gdm->gdm_nsegs,
	    gdm->gdm_size, &gdm->gdm_kva, flags | BUS_DMA_NOWAIT);
	if (error)
		goto fail;

	error = bus_dmamap_create(sc->sc_dmat, gdm->gdm_size, gdm->gdm_nsegs,
	    gdm->gdm_size, 0, BUS_DMA_ALLOCNOW|BUS_DMA_NOWAIT, &gdm->gdm_map);
	if (error)
		goto fail;

	error = bus_dmamap_load(sc->sc_dmat, gdm->gdm_map, gdm->gdm_kva,
	    gdm->gdm_size, NULL, BUS_DMA_NOWAIT);
	if (error)
		goto fail;
#else
	gdm->gdm_segs[0].ds_len   = size;

	/* FIXME: probably we can relax the alignment */
	if ( ! ( gdm->gdm_unaligned_buf = malloc( size + PAGE_SIZE - 1, M_DEVBUF, M_NOWAIT ) ) )
		goto fail;

	gdm->gdm_map   = gdm;
	gdm->gdm_nsegs = 1;
	gdm->gdm_kva   = (caddr_t)(gdm->gdm_segs[0].ds_addr = _DO_ALIGN(gdm->gdm_unaligned_buf, PAGE_SIZE));
#endif

	/* invalidate from cache */
	bus_dmamap_sync(sc->sc_dmat, gdm->gdm_map, 0, gdm->gdm_size,
	    BUS_DMASYNC_PREREAD);
fail:
	if (error) {
		gfe_dmamem_free(sc, gdm);
		GE_DPRINTF(sc, (":err=%d", error));
	}
	GE_DPRINTF(sc, (":kva=%p/%#x,map=%p,nsegs=%d,pa=%" PRIx32 "/%" PRIx32,
	    gdm->gdm_kva, gdm->gdm_size, gdm->gdm_map, gdm->gdm_map->dm_nsegs,
	    gdm->gdm_map->dm_segs->ds_addr, gdm->gdm_map->dm_segs->ds_len));
	GE_FUNC_EXIT(sc, "");
	return error;
}

void
gfe_dmamem_free(struct gfe_softc *sc, struct gfe_dmamem *gdm)
{
	GE_FUNC_ENTER(sc, "gfe_dmamem_free");
#ifndef __rtems__
	if (gdm->gdm_map)
		bus_dmamap_destroy(sc->sc_dmat, gdm->gdm_map);
	if (gdm->gdm_kva)
		bus_dmamem_unmap(sc->sc_dmat, gdm->gdm_kva, gdm->gdm_size);
	if (gdm->gdm_nsegs > 0)
		bus_dmamem_free(sc->sc_dmat, gdm->gdm_segs, gdm->gdm_nsegs);
#else
	if (gdm->gdm_nsegs > 0)
		free(gdm->gdm_unaligned_buf, M_DEVBUF);
#endif
	gdm->gdm_map = NULL;
	gdm->gdm_kva = NULL;
	gdm->gdm_nsegs = 0;
	GE_FUNC_EXIT(sc, "");
}

#ifndef __rtems__
int
gfe_ifioctl(struct ifnet *ifp, u_long cmd, caddr_t data)
#else
int
gfe_ifioctl(struct ifnet *ifp, ioctl_command_t cmd, caddr_t data)
#endif
{
	struct gfe_softc * const sc = ifp->if_softc;
	struct ifreq *ifr = (struct ifreq *) data;
#ifndef __rtems__
	struct ifaddr *ifa = (struct ifaddr *) data;
#endif
	int s, error = 0;

	GE_FUNC_ENTER(sc, "gfe_ifioctl");
	s = splnet();

	switch (cmd) {
#ifndef __rtems__
	case SIOCSIFADDR:
		ifp->if_flags |= IFF_UP;
		switch (ifa->ifa_addr->sa_family) {
#ifdef INET
		case AF_INET:
			error = gfe_whack(sc, GE_WHACK_START);
			if (error == 0)
				arp_ifinit(ifp, ifa);
			break;
#endif
		default:
			error = gfe_whack(sc, GE_WHACK_START);
			break;
		}
		break;
#endif

	case SIOCSIFFLAGS:
		if ((sc->sc_ec.ec_if.if_flags & IFF_PROMISC) == 0)
			sc->sc_pcr &= ~ETH_EPCR_PM;
		else
			sc->sc_pcr |=  ETH_EPCR_PM;
		switch (ifp->if_flags & (IFF_UP|IFF_RUNNING)) {
		case IFF_UP|IFF_RUNNING:/* active->active, update */
			error = gfe_whack(sc, GE_WHACK_CHANGE);
			break;
		case IFF_RUNNING:	/* not up, so we stop */
			error = gfe_whack(sc, GE_WHACK_STOP);
			break;
		case IFF_UP:		/* not running, so we start */
			error = gfe_whack(sc, GE_WHACK_START);
			break;
		case 0:			/* idle->idle: do nothing */
			break;
		}
		break;

	case SIOCADDMULTI:
	case SIOCDELMULTI:
		error = (cmd == SIOCADDMULTI)
		    ? ether_addmulti(ifr, &sc->sc_ec)
		    : ether_delmulti(ifr, &sc->sc_ec);
		if (error == ENETRESET) {
			if (ifp->if_flags & IFF_RUNNING)
#if !defined(__rtems__)
				error = gfe_whack(sc, GE_WHACK_CHANGE);
#else
			/* doing GE_WHACK_CHANGE seems wrong - that
			 * doesn't do anything to the hash table.
			 * Therefore we perform a stop/start sequence.
			 */
			{
				error = gfe_whack(sc, GE_WHACK_STOP);
				if ( error )
					break;
				error = gfe_whack(sc, GE_WHACK_START);
			}
#endif
			else
				error = 0;
		}
		break;

	case SIOCSIFMTU:
		if (ifr->ifr_mtu > ETHERMTU || ifr->ifr_mtu < ETHERMIN) {
			error = EINVAL;
			break;
		}
		ifp->if_mtu = ifr->ifr_mtu;
		break;

	case SIOCSIFMEDIA:
	case SIOCGIFMEDIA:
#ifndef __rtems__
		error = ifmedia_ioctl(ifp, ifr, &sc->sc_mii.mii_media, cmd);
#else
		error = rtems_mii_ioctl(&gfe_mdio_access, sc, cmd, &ifr->ifr_media);
#endif
		break;

	default:
#ifndef __rtems__
	error = EINVAL;
#else
	error = ether_ioctl(ifp, cmd, data);
#endif
		break;
	}
	splx(s);
	GE_FUNC_EXIT(sc, "");
	return error;
}

void
gfe_ifstart(struct ifnet *ifp)
{
	struct gfe_softc * const sc = ifp->if_softc;
	struct mbuf *m;

	GE_FUNC_ENTER(sc, "gfe_ifstart");

	if ((ifp->if_flags & IFF_RUNNING) == 0) {
		GE_FUNC_EXIT(sc, "$");
		return;
	}

	for (;;) {
		IF_DEQUEUE(&ifp->if_snd, m);
		if (m == NULL) {
			ifp->if_flags &= ~IFF_OACTIVE;
			GE_FUNC_EXIT(sc, "");
			return;
		}

		/*
		 * No space in the pending queue?  try later.
		 */
		if (IF_QFULL(&sc->sc_txq[GE_TXPRIO_HI].txq_pendq))
			break;

		/*
		 * Try to enqueue a mbuf to the device. If that fails, we
		 * can always try to map the next mbuf.
		 */
		IF_ENQUEUE(&sc->sc_txq[GE_TXPRIO_HI].txq_pendq, m);
		GE_DPRINTF(sc, (">"));
#ifndef GE_NOTX
		(void) gfe_tx_enqueue(sc, GE_TXPRIO_HI);
#endif
	}

	/*
	 * Attempt to queue the mbuf for send failed.
	 */
	IF_PREPEND(&ifp->if_snd, m);
	ifp->if_flags |= IFF_OACTIVE;
	GE_FUNC_EXIT(sc, "%%");
}

void
gfe_ifwatchdog(struct ifnet *ifp)
{
	struct gfe_softc * const sc = ifp->if_softc;
	struct gfe_txqueue * const txq = &sc->sc_txq[GE_TXPRIO_HI];

	GE_FUNC_ENTER(sc, "gfe_ifwatchdog");
	printf("%s: device timeout", sc->sc_dev.dv_xname);
	if (ifp->if_flags & IFF_RUNNING) {
		uint32_t curtxdnum = (bus_space_read_4(sc->sc_gt_memt, sc->sc_gt_memh, txq->txq_ectdp) - txq->txq_desc_busaddr) / sizeof(txq->txq_descs[0]);
		GE_TXDPOSTSYNC(sc, txq, txq->txq_fi);
		GE_TXDPOSTSYNC(sc, txq, curtxdnum);
		printf(" (fi=%d(%#" PRIx32 "),lo=%d,cur=%" PRIx32 "(%#" PRIx32 "),icm=%#" PRIx32 ") ",
		    txq->txq_fi, txq->txq_descs[txq->txq_fi].ed_cmdsts,
		    txq->txq_lo, curtxdnum, txq->txq_descs[curtxdnum].ed_cmdsts,
		    GE_READ(sc, EICR));
		GE_TXDPRESYNC(sc, txq, txq->txq_fi);
		GE_TXDPRESYNC(sc, txq, curtxdnum);
	}
	printf("\n");
	ifp->if_oerrors++;
	(void) gfe_whack(sc, GE_WHACK_RESTART);
	GE_FUNC_EXIT(sc, "");
}

#ifdef __rtems__
static struct mbuf *
gfe_newbuf(struct mbuf *m)
{
	if ( !m ) {
		MGETHDR(m, M_DONTWAIT, MT_DATA);
		if ( !m )
			return 0;
		MCLGET(m, M_DONTWAIT);
		if ( !(M_EXT & m->m_flags) ) {
			m_freem(m);
			return 0;
		}
	} else {
		m->m_data = m->m_ext.ext_buf;
	}
	m->m_len = m->m_pkthdr.len = MCLBYTES;
#if 0
	m_adj(m, 2); /* so payload is 16-byte aligned */
#endif
	return m;
}
#endif

int
gfe_rx_rxqalloc(struct gfe_softc *sc, enum gfe_rxprio rxprio)
{
	struct gfe_rxqueue * const rxq = &sc->sc_rxq[rxprio];
	int error;

	GE_FUNC_ENTER(sc, "gfe_rx_rxqalloc");
	GE_DPRINTF(sc, ("(%d)", rxprio));

	error = gfe_dmamem_alloc(sc, &rxq->rxq_desc_mem, 1,
	    GE_RXDESC_MEMSIZE, BUS_DMA_NOCACHE);
	if (error) {
		GE_FUNC_EXIT(sc, "!!");
		return error;
	}

#ifndef __rtems__
	error = gfe_dmamem_alloc(sc, &rxq->rxq_buf_mem, GE_RXBUF_NSEGS,
	    GE_RXBUF_MEMSIZE, 0);
#else
	if ( ! (rxq->rxq_bufs = malloc( sizeof(*rxq->rxq_bufs) * GE_RXDESC_MAX, M_DEVBUF, M_NOWAIT ) ) ) {
		error = -1;
	} else {
		int i;
		for ( i = 0; i<GE_RXDESC_MAX; i++ ) {
			if ( !(rxq->rxq_bufs[i] = gfe_newbuf(0)) ) {
				fprintf(stderr,"gfe: Not enough mbuf clusters to initialize RX ring!\n");
				while (--i >=0 ) {
					m_freem(rxq->rxq_bufs[i]);
				}
				free(rxq->rxq_bufs, M_DEVBUF);
				rxq->rxq_bufs = 0;
				error = -1;
				break;
			}
		}
	}
#endif
	if (error) {
		GE_FUNC_EXIT(sc, "!!!");
		return error;
	}
	GE_FUNC_EXIT(sc, "");
	return error;
}

int
gfe_rx_rxqinit(struct gfe_softc *sc, enum gfe_rxprio rxprio)
{
	struct gfe_rxqueue * const rxq = &sc->sc_rxq[rxprio];
	volatile struct gt_eth_desc *rxd;
#ifndef __rtems__
	const bus_dma_segment_t *ds;
#endif
	int idx;
	bus_addr_t nxtaddr;
#ifndef __rtems__
	bus_size_t boff;
#endif

	GE_FUNC_ENTER(sc, "gfe_rx_rxqinit");
	GE_DPRINTF(sc, ("(%d)", rxprio));

	if ((sc->sc_flags & GE_NOFREE) == 0) {
		int error = gfe_rx_rxqalloc(sc, rxprio);
		if (error) {
			GE_FUNC_EXIT(sc, "!");
			return error;
		}
	} else {
		KASSERT(rxq->rxq_desc_mem.gdm_kva != NULL);
#ifndef __rtems__
		KASSERT(rxq->rxq_buf_mem.gdm_kva != NULL);
#else
		KASSERT(rxq->rxq_bufs != NULL);
#endif
	}

	memset(rxq->rxq_desc_mem.gdm_kva, 0, GE_RXDESC_MEMSIZE);

	rxq->rxq_descs =
	    (volatile struct gt_eth_desc *) rxq->rxq_desc_mem.gdm_kva;
	rxq->rxq_desc_busaddr = rxq->rxq_desc_mem.gdm_map->dm_segs[0].ds_addr;
#ifndef __rtems__
	rxq->rxq_bufs = (struct gfe_rxbuf *) rxq->rxq_buf_mem.gdm_kva;
#endif
	rxq->rxq_fi = 0;
	rxq->rxq_active = GE_RXDESC_MAX;
	for (idx = 0, rxd = rxq->rxq_descs,
#ifndef __rtems__
		boff = 0, ds = rxq->rxq_buf_mem.gdm_map->dm_segs,
#endif
		nxtaddr = rxq->rxq_desc_busaddr + sizeof(*rxd);
	     idx < GE_RXDESC_MAX;
	     idx++, rxd++, nxtaddr += sizeof(*rxd)) {
#ifndef __rtems__
		rxd->ed_lencnt = htogt32(GE_RXBUF_SIZE << 16);
#else
		rxd->ed_lencnt = htogt32(MCLBYTES << 16);
#endif
		rxd->ed_cmdsts = htogt32(RX_CMD_F|RX_CMD_L|RX_CMD_O|RX_CMD_EI);
#ifndef __rtems__
		rxd->ed_bufptr = htogt32(ds->ds_addr + boff);
#else
		rxd->ed_bufptr = htogt32(mtod(rxq->rxq_bufs[idx], uint32_t));
#endif
		/*
		 * update the nxtptr to point to the next txd.
		 */
		if (idx == GE_RXDESC_MAX - 1)
			nxtaddr = rxq->rxq_desc_busaddr;
		rxd->ed_nxtptr = htogt32(nxtaddr);
#ifndef __rtems__
		boff += GE_RXBUF_SIZE;
		if (boff == ds->ds_len) {
			ds++;
			boff = 0;
		}
#endif
	}
	bus_dmamap_sync(sc->sc_dmat, rxq->rxq_desc_mem.gdm_map, 0,
			rxq->rxq_desc_mem.gdm_map->dm_mapsize,
			BUS_DMASYNC_PREREAD|BUS_DMASYNC_PREWRITE);
#ifndef __rtems__
	bus_dmamap_sync(sc->sc_dmat, rxq->rxq_buf_mem.gdm_map, 0,
			rxq->rxq_buf_mem.gdm_map->dm_mapsize,
			BUS_DMASYNC_PREREAD);
#else
	/* FIXME: we leave this call in here so compilation fails
	 *        if bus_dmamap_sync() is ever fleshed-out to implement
	 *        software cache coherency...
	 */
	bus_dmamap_sync(sc->sc_dmat, rxq->rxq_buf_mem.gdm_map, 0,
			rxq->rxq_buf_mem.gdm_map->dm_mapsize,
			BUS_DMASYNC_PREREAD);
#endif

	rxq->rxq_intrbits = ETH_IR_RxBuffer|ETH_IR_RxError;
	switch (rxprio) {
	case GE_RXPRIO_HI:
		rxq->rxq_intrbits |= ETH_IR_RxBuffer_3|ETH_IR_RxError_3;
		rxq->rxq_efrdp = ETH_EFRDP3(sc->sc_macno);
		rxq->rxq_ecrdp = ETH_ECRDP3(sc->sc_macno);
		break;
	case GE_RXPRIO_MEDHI:
		rxq->rxq_intrbits |= ETH_IR_RxBuffer_2|ETH_IR_RxError_2;
		rxq->rxq_efrdp = ETH_EFRDP2(sc->sc_macno);
		rxq->rxq_ecrdp = ETH_ECRDP2(sc->sc_macno);
		break;
	case GE_RXPRIO_MEDLO:
		rxq->rxq_intrbits |= ETH_IR_RxBuffer_1|ETH_IR_RxError_1;
		rxq->rxq_efrdp = ETH_EFRDP1(sc->sc_macno);
		rxq->rxq_ecrdp = ETH_ECRDP1(sc->sc_macno);
		break;
	case GE_RXPRIO_LO:
		rxq->rxq_intrbits |= ETH_IR_RxBuffer_0|ETH_IR_RxError_0;
		rxq->rxq_efrdp = ETH_EFRDP0(sc->sc_macno);
		rxq->rxq_ecrdp = ETH_ECRDP0(sc->sc_macno);
		break;
	}
	GE_FUNC_EXIT(sc, "");
	return 0;
}

void
gfe_rx_get(struct gfe_softc *sc, enum gfe_rxprio rxprio)
{
	struct ifnet * const ifp = &sc->sc_ec.ec_if;
	struct gfe_rxqueue * const rxq = &sc->sc_rxq[rxprio];
#ifndef __rtems__
	struct mbuf *m = rxq->rxq_curpkt;
#else
	struct mbuf *m;
#endif

	GE_FUNC_ENTER(sc, "gfe_rx_get");
	GE_DPRINTF(sc, ("(%d)", rxprio));

	while (rxq->rxq_active > 0) {
		volatile struct gt_eth_desc *rxd = &rxq->rxq_descs[rxq->rxq_fi];
#ifndef __rtems__
		struct gfe_rxbuf *rxb = &rxq->rxq_bufs[rxq->rxq_fi];
#else
		struct mbuf		**rxb = &rxq->rxq_bufs[rxq->rxq_fi];
#endif
		const struct ether_header *eh;
		unsigned int cmdsts;
		size_t buflen;

		GE_RXDPOSTSYNC(sc, rxq, rxq->rxq_fi);
		cmdsts = gt32toh(rxd->ed_cmdsts);
		GE_DPRINTF(sc, (":%d=%#x", rxq->rxq_fi, cmdsts));
		rxq->rxq_cmdsts = cmdsts;
		/*
		 * Sometimes the GE "forgets" to reset the ownership bit.
		 * But if the length has been rewritten, the packet is ours
		 * so pretend the O bit is set.
		 */
		buflen = gt32toh(rxd->ed_lencnt) & 0xffff;
		if ((cmdsts & RX_CMD_O) && buflen == 0) {
			GE_RXDPRESYNC(sc, rxq, rxq->rxq_fi);
			break;
		}

		/*
		 * If this is not a single buffer packet with no errors
		 * or for some reason it's bigger than our frame size,
		 * ignore it and go to the next packet.
		 */
		if ((cmdsts & (RX_CMD_F|RX_CMD_L|RX_STS_ES)) !=
			    (RX_CMD_F|RX_CMD_L) ||
		    buflen > sc->sc_max_frame_length) {
			GE_DPRINTF(sc, ("!"));
			--rxq->rxq_active;
			ifp->if_ipackets++;
			ifp->if_ierrors++;
			
			*rxb = gfe_newbuf(*rxb);
			goto give_it_back;
		}

		/* CRC is included with the packet; trim it off. */
		buflen -= ETHER_CRC_LEN;

#ifndef __rtems__
		if (m == NULL) {
			MGETHDR(m, M_DONTWAIT, MT_DATA);
			if (m == NULL) {
				GE_DPRINTF(sc, ("?"));
				break;
			}
		}
		if ((m->m_flags & M_EXT) == 0 && buflen > MHLEN - 2) {
			MCLGET(m, M_DONTWAIT);
			if ((m->m_flags & M_EXT) == 0) {
				GE_DPRINTF(sc, ("?"));
				break;
			}
		}
		m->m_data += 2;
		m->m_len = 0;
		m->m_pkthdr.len = 0;
		m->m_pkthdr.rcvif = ifp;
#else
		if ( ! (m=gfe_newbuf(0)) ) {
			/* recycle old buffer */
			*rxb = gfe_newbuf(*rxb);
			goto give_it_back;
		}
		/* swap mbufs */
		{
		struct mbuf *tmp = *rxb;
		*rxb = m;
		m    = tmp;
		rxd->ed_bufptr = htogt32(mtod(*rxb, uint32_t));
		}
#endif
		rxq->rxq_cmdsts = cmdsts;
		--rxq->rxq_active;

#ifdef __rtems__
		/* FIXME: we leave this call in here so compilation fails
		 *        if bus_dmamap_sync() is ever fleshed-out to implement
		 *        software cache coherency...
		 */
		bus_dmamap_sync(sc->sc_dmat, rxq->rxq_buf_mem.gdm_map,
		    rxq->rxq_fi * sizeof(*rxb), buflen, BUS_DMASYNC_POSTREAD);
#else
		bus_dmamap_sync(sc->sc_dmat, rxq->rxq_buf_mem.gdm_map,
		    rxq->rxq_fi * sizeof(*rxb), buflen, BUS_DMASYNC_POSTREAD);

		KASSERT(m->m_len == 0 && m->m_pkthdr.len == 0);
		memcpy(m->m_data + m->m_len, rxb->rb_data, buflen);
#endif

		m->m_len = buflen;
		m->m_pkthdr.len = buflen;

		ifp->if_ipackets++;
#if NBPFILTER > 0
		if (ifp->if_bpf != NULL)
			bpf_mtap(ifp->if_bpf, m);
#endif

		eh = (const struct ether_header *) m->m_data;
		if ((ifp->if_flags & IFF_PROMISC) ||
		    (rxq->rxq_cmdsts & RX_STS_M) == 0 ||
		    (rxq->rxq_cmdsts & RX_STS_HE) ||
		    (eh->ether_dhost[0] & 1) != 0 ||
		    memcmp(eh->ether_dhost,
#ifndef __rtems__
				LLADDR(ifp->if_sadl),
#else
				sc->sc_ec.ac_enaddr,
#endif
			ETHER_ADDR_LEN) == 0) {
#ifndef __rtems__
			(*ifp->if_input)(ifp, m);
			m = NULL;
#else
			DO_ETHER_INPUT_SKIPPING_ETHER_HEADER(ifp,m);
#endif
			GE_DPRINTF(sc, (">"));
		} else {
#ifndef __rtems__
			m->m_len = 0;
			m->m_pkthdr.len = 0;
#else
			m_freem(m);
#endif
			GE_DPRINTF(sc, ("+"));
		}
		rxq->rxq_cmdsts = 0;

	   give_it_back:
		rxd->ed_lencnt &= ~0xffff;	/* zero out length */
		rxd->ed_cmdsts = htogt32(RX_CMD_F|RX_CMD_L|RX_CMD_O|RX_CMD_EI);
#if 0
		GE_DPRINTF(sc, ("([%d]->%08lx.%08lx.%08lx.%08lx)",
		    rxq->rxq_fi,
		    ((unsigned long *)rxd)[0], ((unsigned long *)rxd)[1],
		    ((unsigned long *)rxd)[2], ((unsigned long *)rxd)[3]));
#endif
		GE_RXDPRESYNC(sc, rxq, rxq->rxq_fi);
		if (++rxq->rxq_fi == GE_RXDESC_MAX)
			rxq->rxq_fi = 0;
		rxq->rxq_active++;
	}
#ifndef __rtems__
	rxq->rxq_curpkt = m;
#endif
	GE_FUNC_EXIT(sc, "");
}

uint32_t
gfe_rx_process(struct gfe_softc *sc, uint32_t cause, uint32_t intrmask)
{
	struct ifnet * const ifp = &sc->sc_ec.ec_if;
	struct gfe_rxqueue *rxq;
	uint32_t rxbits;
#define	RXPRIO_DECODER	0xffffaa50
	GE_FUNC_ENTER(sc, "gfe_rx_process");

	rxbits = ETH_IR_RxBuffer_GET(cause);
	while (rxbits) {
		enum gfe_rxprio rxprio = (RXPRIO_DECODER >> (rxbits * 2)) & 3;
		GE_DPRINTF(sc, ("%1" PRIx32, rxbits));
		rxbits &= ~(1 << rxprio);
		gfe_rx_get(sc, rxprio);
	}

	rxbits = ETH_IR_RxError_GET(cause);
	while (rxbits) {
		enum gfe_rxprio rxprio = (RXPRIO_DECODER >> (rxbits * 2)) & 3;
		uint32_t masks[(GE_RXDESC_MAX + 31) / 32];
		int idx;
		rxbits &= ~(1 << rxprio);
		rxq = &sc->sc_rxq[rxprio];
		sc->sc_idlemask |= (rxq->rxq_intrbits & ETH_IR_RxBits);
		intrmask &= ~(rxq->rxq_intrbits & ETH_IR_RxBits);
		if ((sc->sc_tickflags & GE_TICK_RX_RESTART) == 0) {
			sc->sc_tickflags |= GE_TICK_RX_RESTART;
			callout_reset(&sc->sc_co, 1, gfe_tick, sc);
		}
		ifp->if_ierrors++;
		GE_DPRINTF(sc, ("%s: rx queue %d filled at %u\n",
		    sc->sc_dev.dv_xname, rxprio, rxq->rxq_fi));
		memset(masks, 0, sizeof(masks));
		bus_dmamap_sync(sc->sc_dmat, rxq->rxq_desc_mem.gdm_map,
		    0, rxq->rxq_desc_mem.gdm_size,
		    BUS_DMASYNC_POSTREAD|BUS_DMASYNC_POSTWRITE);
		for (idx = 0; idx < GE_RXDESC_MAX; idx++) {
			volatile struct gt_eth_desc *rxd = &rxq->rxq_descs[idx];

			if (RX_CMD_O & gt32toh(rxd->ed_cmdsts))
				masks[idx/32] |= 1 << (idx & 31);
		}
		bus_dmamap_sync(sc->sc_dmat, rxq->rxq_desc_mem.gdm_map,
		    0, rxq->rxq_desc_mem.gdm_size,
		    BUS_DMASYNC_PREREAD|BUS_DMASYNC_PREWRITE);
#if defined(DEBUG)
		printf("%s: rx queue %d filled at %u=%#x(%#x/%#x)\n",
		    sc->sc_dev.dv_xname, rxprio, rxq->rxq_fi,
		    rxq->rxq_cmdsts, masks[0], masks[1]);
#endif
	}
	if ((intrmask & ETH_IR_RxBits) == 0)
		intrmask &= ~(ETH_IR_RxBuffer|ETH_IR_RxError);

	GE_FUNC_EXIT(sc, "");
	return intrmask;
}

int
gfe_rx_prime(struct gfe_softc *sc)
{
	struct gfe_rxqueue *rxq;
	int error;

	GE_FUNC_ENTER(sc, "gfe_rx_prime");

	error = gfe_rx_rxqinit(sc, GE_RXPRIO_HI);
	if (error)
		goto bail;
	rxq = &sc->sc_rxq[GE_RXPRIO_HI];
	if ((sc->sc_flags & GE_RXACTIVE) == 0) {
		GE_WRITE(sc, EFRDP3, rxq->rxq_desc_busaddr);
		GE_WRITE(sc, ECRDP3, rxq->rxq_desc_busaddr);
	}
	sc->sc_intrmask |= rxq->rxq_intrbits;

	error = gfe_rx_rxqinit(sc, GE_RXPRIO_MEDHI);
	if (error)
		goto bail;
	if ((sc->sc_flags & GE_RXACTIVE) == 0) {
		rxq = &sc->sc_rxq[GE_RXPRIO_MEDHI];
		GE_WRITE(sc, EFRDP2, rxq->rxq_desc_busaddr);
		GE_WRITE(sc, ECRDP2, rxq->rxq_desc_busaddr);
		sc->sc_intrmask |= rxq->rxq_intrbits;
	}

	error = gfe_rx_rxqinit(sc, GE_RXPRIO_MEDLO);
	if (error)
		goto bail;
	if ((sc->sc_flags & GE_RXACTIVE) == 0) {
		rxq = &sc->sc_rxq[GE_RXPRIO_MEDLO];
		GE_WRITE(sc, EFRDP1, rxq->rxq_desc_busaddr);
		GE_WRITE(sc, ECRDP1, rxq->rxq_desc_busaddr);
		sc->sc_intrmask |= rxq->rxq_intrbits;
	}

	error = gfe_rx_rxqinit(sc, GE_RXPRIO_LO);
	if (error)
		goto bail;
	if ((sc->sc_flags & GE_RXACTIVE) == 0) {
		rxq = &sc->sc_rxq[GE_RXPRIO_LO];
		GE_WRITE(sc, EFRDP0, rxq->rxq_desc_busaddr);
		GE_WRITE(sc, ECRDP0, rxq->rxq_desc_busaddr);
		sc->sc_intrmask |= rxq->rxq_intrbits;
	}

  bail:
	GE_FUNC_EXIT(sc, "");
	return error;
}

void
gfe_rx_cleanup(struct gfe_softc *sc, enum gfe_rxprio rxprio)
{
	struct gfe_rxqueue *rxq = &sc->sc_rxq[rxprio];
	GE_FUNC_ENTER(sc, "gfe_rx_cleanup");
	if (rxq == NULL) {
		GE_FUNC_EXIT(sc, "");
		return;
	}

#ifndef __rtems__
	if (rxq->rxq_curpkt)
		m_freem(rxq->rxq_curpkt);
#endif
	if ((sc->sc_flags & GE_NOFREE) == 0) {
		gfe_dmamem_free(sc, &rxq->rxq_desc_mem);
#ifndef __rtems__
		gfe_dmamem_free(sc, &rxq->rxq_buf_mem);
#else
		if ( rxq->rxq_bufs ) {
			int i;
			for ( i=0; i<GE_RXDESC_MAX; i++ ) {
				if ( rxq->rxq_bufs[i] ) {
					m_freem(rxq->rxq_bufs[i]);
				}
			}
			free(rxq->rxq_bufs, M_DEVBUF);
		}
#endif
	}
	GE_FUNC_EXIT(sc, "");
}

void
gfe_rx_stop(struct gfe_softc *sc, enum gfe_whack_op op)
{
	GE_FUNC_ENTER(sc, "gfe_rx_stop");
	sc->sc_flags &= ~GE_RXACTIVE;
	sc->sc_idlemask &= ~(ETH_IR_RxBits|ETH_IR_RxBuffer|ETH_IR_RxError);
	sc->sc_intrmask &= ~(ETH_IR_RxBits|ETH_IR_RxBuffer|ETH_IR_RxError);
	GE_WRITE(sc, EIMR, sc->sc_intrmask);
	GE_WRITE(sc, ESDCMR, ETH_ESDCMR_AR);
	do {
		delay(10);
	} while (GE_READ(sc, ESDCMR) & ETH_ESDCMR_AR);
	gfe_rx_cleanup(sc, GE_RXPRIO_HI);
	gfe_rx_cleanup(sc, GE_RXPRIO_MEDHI);
	gfe_rx_cleanup(sc, GE_RXPRIO_MEDLO);
	gfe_rx_cleanup(sc, GE_RXPRIO_LO);
	GE_FUNC_EXIT(sc, "");
}

void
gfe_tick(void *arg)
{
	struct gfe_softc * const sc = arg;
	uint32_t intrmask;
	unsigned int tickflags;
	int s;

	GE_FUNC_ENTER(sc, "gfe_tick");

	s = splnet();

	tickflags = sc->sc_tickflags;
	sc->sc_tickflags = 0;
	intrmask = sc->sc_intrmask;
	if (tickflags & GE_TICK_TX_IFSTART)
		gfe_ifstart(&sc->sc_ec.ec_if);
	if (tickflags & GE_TICK_RX_RESTART) {
		intrmask |= sc->sc_idlemask;
		if (sc->sc_idlemask & (ETH_IR_RxBuffer_3|ETH_IR_RxError_3)) {
			struct gfe_rxqueue *rxq = &sc->sc_rxq[GE_RXPRIO_HI];
			rxq->rxq_fi = 0;
			GE_WRITE(sc, EFRDP3, rxq->rxq_desc_busaddr);
			GE_WRITE(sc, ECRDP3, rxq->rxq_desc_busaddr);
		}
		if (sc->sc_idlemask & (ETH_IR_RxBuffer_2|ETH_IR_RxError_2)) {
			struct gfe_rxqueue *rxq = &sc->sc_rxq[GE_RXPRIO_MEDHI];
			rxq->rxq_fi = 0;
			GE_WRITE(sc, EFRDP2, rxq->rxq_desc_busaddr);
			GE_WRITE(sc, ECRDP2, rxq->rxq_desc_busaddr);
		}
		if (sc->sc_idlemask & (ETH_IR_RxBuffer_1|ETH_IR_RxError_1)) {
			struct gfe_rxqueue *rxq = &sc->sc_rxq[GE_RXPRIO_MEDLO];
			rxq->rxq_fi = 0;
			GE_WRITE(sc, EFRDP1, rxq->rxq_desc_busaddr);
			GE_WRITE(sc, ECRDP1, rxq->rxq_desc_busaddr);
		}
		if (sc->sc_idlemask & (ETH_IR_RxBuffer_0|ETH_IR_RxError_0)) {
			struct gfe_rxqueue *rxq = &sc->sc_rxq[GE_RXPRIO_LO];
			rxq->rxq_fi = 0;
			GE_WRITE(sc, EFRDP0, rxq->rxq_desc_busaddr);
			GE_WRITE(sc, ECRDP0, rxq->rxq_desc_busaddr);
		}
		sc->sc_idlemask = 0;
	}
	if (intrmask != sc->sc_intrmask) {
		sc->sc_intrmask = intrmask;
		GE_WRITE(sc, EIMR, sc->sc_intrmask);
	}
	gfe_intr(sc);
	splx(s);

	GE_FUNC_EXIT(sc, "");
}

static int
gfe_free_slots(struct gfe_softc *sc, struct gfe_txqueue *const txq)
{
	struct ifnet * const ifp = &sc->sc_ec.ec_if;
#ifndef	__rtems__
	const int dcache_line_size = curcpu()->ci_ci.dcache_line_size;
#endif
	int							 got = 0;
	int							  fi = txq->txq_fi;
	volatile struct gt_eth_desc *txd = &txq->txq_descs[fi];
	uint32_t cmdsts;
#ifndef __rtems__
	size_t pktlen;
#endif

	GE_FUNC_ENTER(sc, "gfe_free_slots");

#ifdef __rtems__
	do {
#endif
		GE_TXDPOSTSYNC(sc, txq, fi);
		if ((cmdsts = gt32toh(txd->ed_cmdsts)) & TX_CMD_O) {
			int nextin;

			if (txq->txq_nactive == 1) {
				GE_TXDPRESYNC(sc, txq, fi);
				GE_FUNC_EXIT(sc, "");
				return -1;
			}
			/*
			 * Sometimes the Discovery forgets to update the
			 * ownership bit in the descriptor.  See if we own the
			 * descriptor after it (since we know we've turned
			 * that to the Discovery and if we own it now then the
			 * Discovery gave it back).  If we do, we know the
			 * Discovery gave back this one but forgot to mark it
			 * as ours.
			 */
			nextin = fi + 1;
			if (nextin == GE_TXDESC_MAX)
				nextin = 0;
			GE_TXDPOSTSYNC(sc, txq, nextin);
			if (gt32toh(txq->txq_descs[nextin].ed_cmdsts) & TX_CMD_O) {
				GE_TXDPRESYNC(sc, txq, fi);
				GE_TXDPRESYNC(sc, txq, nextin);
				GE_FUNC_EXIT(sc, "");
				return -1;
			}
#ifdef DEBUG
			printf("%s: gfe_free_slots: transmitter resynced at %d\n",
					sc->sc_dev.dv_xname, fi);
#endif
		}
		got++;
#ifdef __rtems__
		txd++;
		fi++;
	} while ( ! ( TX_CMD_LAST & cmdsts ) );

	{ struct mbuf *m;
		IF_DEQUEUE(&txq->txq_sentq, m);
		m_freem(m);
	}
#endif
#if 0
	GE_DPRINTF(sc, ("([%d]<-%08lx.%08lx.%08lx.%08lx)",
				txq->txq_lo,
				((unsigned long *)txd)[0], ((unsigned long *)txd)[1],
				((unsigned long *)txd)[2], ((unsigned long *)txd)[3]));
#endif
	GE_DPRINTF(sc, ("(%d)", fi));
	txq->txq_fi = fi;
	if ( txq->txq_fi >= GE_TXDESC_MAX)
		txq->txq_fi -= GE_TXDESC_MAX;
#ifndef __rtems__
	txq->txq_inptr = gt32toh(txd->ed_bufptr) - txq->txq_buf_busaddr;
	pktlen = (gt32toh(txd->ed_lencnt) >> 16) & 0xffff;
	bus_dmamap_sync(sc->sc_dmat, txq->txq_buf_mem.gdm_map,
			txq->txq_inptr, pktlen, BUS_DMASYNC_POSTWRITE);
	txq->txq_inptr += roundup(pktlen, dcache_line_size);
#endif

	/* statistics */
	ifp->if_opackets++;
#ifdef __rtems__
	/* FIXME: should we check errors on every fragment? */
#endif
	if (cmdsts & TX_STS_ES)
		ifp->if_oerrors++;

	/* txd->ed_bufptr = 0; */

	txq->txq_nactive -= got;

	GE_FUNC_EXIT(sc, "");

	return got;
}

#ifndef __rtems__
int
gfe_tx_enqueue(struct gfe_softc *sc, enum gfe_txprio txprio)
{
#ifndef	__rtems__
	const int dcache_line_size = curcpu()->ci_ci.dcache_line_size;
#else
#ifndef PPC_CACHE_ALIGNMENT
#error	"Unknown cache alignment for your CPU"
#endif
	const int dcache_line_size = PPC_CACHE_ALIGNMENT;
#endif
	struct ifnet * const ifp = &sc->sc_ec.ec_if;
	struct gfe_txqueue * const txq = &sc->sc_txq[txprio];
	volatile struct gt_eth_desc * const txd = &txq->txq_descs[txq->txq_lo];
	uint32_t intrmask = sc->sc_intrmask;
	size_t buflen;
	struct mbuf *m;

	GE_FUNC_ENTER(sc, "gfe_tx_enqueue");

	/*
	 * Anything in the pending queue to enqueue?  if not, punt. Likewise
	 * if the txq is not yet created.
	 * otherwise grab its dmamap.
	 */
	if (txq == NULL || (m = txq->txq_pendq.ifq_head) == NULL) {
		GE_FUNC_EXIT(sc, "-");
		return 0;
	}

	/*
	 * Have we [over]consumed our limit of descriptors?
	 * Do we have enough free descriptors?
	 */
	if (GE_TXDESC_MAX == txq->txq_nactive + 2) {
		if ( gfe_free_slots(sc, txq) <= 0 )
			return 0;
	}

	buflen = roundup(m->m_pkthdr.len, dcache_line_size);

	/*
	 * If this packet would wrap around the end of the buffer, reset back
	 * to the beginning.
	 */
	if (txq->txq_outptr + buflen > GE_TXBUF_SIZE) {
		txq->txq_ei_gapcount += GE_TXBUF_SIZE - txq->txq_outptr;
		txq->txq_outptr = 0;
	}

	/*
	 * Make sure the output packet doesn't run over the beginning of
	 * what we've already given the GT.
	 */
	if (txq->txq_nactive > 0 && txq->txq_outptr <= txq->txq_inptr &&
	    txq->txq_outptr + buflen > txq->txq_inptr) {
		intrmask |= txq->txq_intrbits &
		    (ETH_IR_TxBufferHigh|ETH_IR_TxBufferLow);
		if (sc->sc_intrmask != intrmask) {
			sc->sc_intrmask = intrmask;
			GE_WRITE(sc, EIMR, sc->sc_intrmask);
		}
		GE_FUNC_EXIT(sc, "#");
		return 0;
	}

	/*
	 * The end-of-list descriptor we put on last time is the starting point
	 * for this packet.  The GT is supposed to terminate list processing on
	 * a NULL nxtptr but that currently is broken so a CPU-owned descriptor
	 * must terminate the list.
	 */
	intrmask = sc->sc_intrmask;

	m_copydata(m, 0, m->m_pkthdr.len,
	    txq->txq_buf_mem.gdm_kva + txq->txq_outptr);
	bus_dmamap_sync(sc->sc_dmat, txq->txq_buf_mem.gdm_map,
	    txq->txq_outptr, buflen, BUS_DMASYNC_PREWRITE);
	txd->ed_bufptr = htogt32(txq->txq_buf_busaddr + txq->txq_outptr);
	txd->ed_lencnt = htogt32(m->m_pkthdr.len << 16);
	GE_TXDPRESYNC(sc, txq, txq->txq_lo);

	/*
	 * Request a buffer interrupt every 2/3 of the way thru the transmit
	 * buffer.
	 */
	txq->txq_ei_gapcount += buflen;
	if (txq->txq_ei_gapcount > 2 * GE_TXBUF_SIZE / 3) {
		txd->ed_cmdsts = htogt32(TX_CMD_FIRST|TX_CMD_LAST|TX_CMD_EI);
		txq->txq_ei_gapcount = 0;
	} else {
		txd->ed_cmdsts = htogt32(TX_CMD_FIRST|TX_CMD_LAST);
	}
#if 0
	GE_DPRINTF(sc, ("([%d]->%08lx.%08lx.%08lx.%08lx)", txq->txq_lo,
	    ((unsigned long *)txd)[0], ((unsigned long *)txd)[1],
	    ((unsigned long *)txd)[2], ((unsigned long *)txd)[3]));
#endif
	GE_TXDPRESYNC(sc, txq, txq->txq_lo);

	txq->txq_outptr += buflen;
	/*
	 * Tell the SDMA engine to "Fetch!"
	 */
	GE_WRITE(sc, ESDCMR,
		 txq->txq_esdcmrbits & (ETH_ESDCMR_TXDH|ETH_ESDCMR_TXDL));

	GE_DPRINTF(sc, ("(%d)", txq->txq_lo));

	/*
	 * Update the last out appropriately.
	 */
	txq->txq_nactive++;
	if (++txq->txq_lo == GE_TXDESC_MAX)
		txq->txq_lo = 0;

	/*
	 * Move mbuf from the pending queue to the snd queue.
	 */
	IF_DEQUEUE(&txq->txq_pendq, m);
#if NBPFILTER > 0
	if (ifp->if_bpf != NULL)
		bpf_mtap(ifp->if_bpf, m);
#endif
	m_freem(m);
	ifp->if_flags &= ~IFF_OACTIVE;

	/*
	 * Since we have put an item into the packet queue, we now want
	 * an interrupt when the transmit queue finishes processing the
	 * list.  But only update the mask if needs changing.
	 */
	intrmask |= txq->txq_intrbits & (ETH_IR_TxEndHigh|ETH_IR_TxEndLow);
	if (sc->sc_intrmask != intrmask) {
		sc->sc_intrmask = intrmask;
		GE_WRITE(sc, EIMR, sc->sc_intrmask);
	}
	if (ifp->if_timer == 0)
		ifp->if_timer = 5;
	GE_FUNC_EXIT(sc, "*");
	return 1;
}

#else

#ifdef __PPC__
static inline void membarrier(void)
{
	asm volatile("sync":::"memory");
}
#else
#error "memory synchronization for your CPU not implemented"
#endif


void
gfe_assign_desc(volatile struct gt_eth_desc *const d, struct mbuf *m, uint32_t flags)
{
	d->ed_cmdsts = htogt32(flags | TX_CMD_GC | TX_CMD_P);
	d->ed_bufptr = htogt32(mtod(m, uint32_t));
	d->ed_lencnt = htogt32(m->m_len << 16);
}

int
gfe_tx_enqueue(struct gfe_softc *sc, enum gfe_txprio txprio)
{
	struct ifnet * const ifp = &sc->sc_ec.ec_if;
	struct gfe_txqueue * const txq = &sc->sc_txq[txprio];
	volatile struct gt_eth_desc * const txd = &txq->txq_descs[txq->txq_lo];
#define NEXT_TXD(d)	((d)+1 < &txq->txq_descs[GE_TXDESC_MAX] ? (d)+1 : txq->txq_descs)
	volatile struct gt_eth_desc *l,*d;
	uint32_t intrmask = sc->sc_intrmask;
	struct mbuf *m_head,*m,*m1;
	int									avail, used;

	GE_FUNC_ENTER(sc, "gfe_tx_enqueue");

	/*
	 * Anything in the pending queue to enqueue?  if not, punt. Likewise
	 * if the txq is not yet created.
	 * otherwise grab its dmamap.
	 */
	if (txq == NULL || (m_head = txq->txq_pendq.ifq_head) == NULL) {
		GE_FUNC_EXIT(sc, "-");
		return 0;
	}

	/* find 1st mbuf with actual data; m_head is not NULL at this point */
	for ( m1=m_head; 0 == m1->m_len; ) {
		if ( ! (m1=m1->m_next) ) {
			/* nothing to send */
			IF_DEQUEUE(&txq->txq_pendq, m_head);
			m_freem(m_head);
			return 0;
		}
	}

	avail = GE_TXDESC_MAX - 1 - txq->txq_nactive;

	if ( avail < 1 && (avail += gfe_free_slots(sc, txq)) < 1 )
		return 0;

	avail--;

	l = txd;
	d = NEXT_TXD(txd);

	for ( m=m1->m_next, used = 1; m; m=m->m_next ) {
		if ( 0 == m->m_len )
			continue; /* skip empty mbufs */

		if ( avail < 1 && (avail += gfe_free_slots(sc, txq)) < 1 ) {
			/* not enough descriptors; cleanup */
			for ( l = NEXT_TXD(txd); l!=d; l = NEXT_TXD(l) ) {
				l->ed_cmdsts = 0;
				avail++;
			}
			avail++;
			if ( used >= GE_TXDESC_MAX-1 )
				panic("mbuf chain (#%i) longer than TX ring (#%i); configuration error!",
					used, GE_TXDESC_MAX-1);
			return 0;
		}
		used++;
		avail--;

		/* fill this slot */
		gfe_assign_desc(d, m, TX_CMD_O);

		bus_dmamap_sync(sc->sc_dmat, /* TODO */,
	   		 mtod(m, uint32_t), m->m_len, BUS_DMASYNC_PREWRITE);

		l = d;
		d = NEXT_TXD(d);

		GE_TXDPRESYNC(sc, txq, l - txq->txq_descs);
	}

	/* fill first slot */
	gfe_assign_desc(txd, m1, TX_CMD_F);

	bus_dmamap_sync(sc->sc_dmat, /* TODO */,
	   		 mtod(m1, uint32_t), m1->m_len, BUS_DMASYNC_PREWRITE);
	
	/* tag last slot; this covers where 1st = last */
	l->ed_cmdsts	|= htonl(TX_CMD_L | TX_CMD_EI);

	GE_TXDPRESYNC(sc, txq, l - txq->txq_descs);

	/*
	 * The end-of-list descriptor we put on last time is the starting point
	 * for this packet.  The GT is supposed to terminate list processing on
	 * a NULL nxtptr but that currently is broken so a CPU-owned descriptor
	 * must terminate the list.
	 */
	d = NEXT_TXD(l);

	out_be32((unsigned int*)&d->ed_cmdsts,0);

	GE_TXDPRESYNC(sc, txq, d - txq->txq_descs);

	membarrier();

	/* turn over the whole chain by flipping the ownership of the first desc */
	txd->ed_cmdsts |= htonl(TX_CMD_O);

	GE_TXDPRESYNC(sc, txq, txq->txq_lo);


	intrmask = sc->sc_intrmask;

#if 0
	GE_DPRINTF(sc, ("([%d]->%08lx.%08lx.%08lx.%08lx)", txq->txq_lo,
	    ((unsigned long *)txd)[0], ((unsigned long *)txd)[1],
	    ((unsigned long *)txd)[2], ((unsigned long *)txd)[3]));
#endif

	membarrier();

	/*
	 * Tell the SDMA engine to "Fetch!"
	 */
	GE_WRITE(sc, ESDCMR,
		 txq->txq_esdcmrbits & (ETH_ESDCMR_TXDH|ETH_ESDCMR_TXDL));

	GE_DPRINTF(sc, ("(%d)", txq->txq_lo));

	/*
	 * Update the last out appropriately.
	 */
	txq->txq_nactive += used;
	txq->txq_lo      += used;
	if ( txq->txq_lo >= GE_TXDESC_MAX )
		txq->txq_lo -= GE_TXDESC_MAX;

	/*
	 * Move mbuf from the pending queue to the snd queue.
	 */
	IF_DEQUEUE(&txq->txq_pendq, m_head);

	IF_ENQUEUE(&txq->txq_sentq, m_head);

#if NBPFILTER > 0
	if (ifp->if_bpf != NULL)
		bpf_mtap(ifp->if_bpf, m_head);
#endif
	ifp->if_flags &= ~IFF_OACTIVE;

	/*
	 * Since we have put an item into the packet queue, we now want
	 * an interrupt when the transmit queue finishes processing the
	 * list.  But only update the mask if needs changing.
	 */
	intrmask |= txq->txq_intrbits & (ETH_IR_TxEndHigh|ETH_IR_TxEndLow);
	if (sc->sc_intrmask != intrmask) {
		sc->sc_intrmask = intrmask;
		GE_WRITE(sc, EIMR, sc->sc_intrmask);
	}
	if (ifp->if_timer == 0)
		ifp->if_timer = 5;
	GE_FUNC_EXIT(sc, "*");
	return 1;
}
#endif

uint32_t
gfe_tx_done(struct gfe_softc *sc, enum gfe_txprio txprio, uint32_t intrmask)
{
	struct gfe_txqueue * const txq = &sc->sc_txq[txprio];
	struct ifnet * const ifp = &sc->sc_ec.ec_if;

	GE_FUNC_ENTER(sc, "gfe_tx_done");

	if (txq == NULL) {
		GE_FUNC_EXIT(sc, "");
		return intrmask;
	}

	while (txq->txq_nactive > 0) {
		if ( gfe_free_slots(sc, txq) < 0 )
			return intrmask;
		ifp->if_timer = 5;
	}
	if (txq->txq_nactive != 0)
		panic("%s: transmit fifo%d empty but active count (%d) not 0!",
		    sc->sc_dev.dv_xname, txprio, txq->txq_nactive);
	ifp->if_timer = 0;
	intrmask &= ~(txq->txq_intrbits & (ETH_IR_TxEndHigh|ETH_IR_TxEndLow));
	intrmask &= ~(txq->txq_intrbits & (ETH_IR_TxBufferHigh|ETH_IR_TxBufferLow));
	GE_FUNC_EXIT(sc, "");
	return intrmask;
}

int
gfe_tx_txqalloc(struct gfe_softc *sc, enum gfe_txprio txprio)
{
	struct gfe_txqueue * const txq = &sc->sc_txq[txprio];
	int error;

	GE_FUNC_ENTER(sc, "gfe_tx_txqalloc");

	error = gfe_dmamem_alloc(sc, &txq->txq_desc_mem, 1,
	    GE_TXDESC_MEMSIZE, BUS_DMA_NOCACHE);
	if (error) {
		GE_FUNC_EXIT(sc, "");
		return error;
	}
#ifndef __rtems__
	error = gfe_dmamem_alloc(sc, &txq->txq_buf_mem, 1, GE_TXBUF_SIZE, 0);
	if (error) {
		gfe_dmamem_free(sc, &txq->txq_desc_mem);
		GE_FUNC_EXIT(sc, "");
		return error;
	}
#endif
	GE_FUNC_EXIT(sc, "");
	return 0;
}

int
gfe_tx_start(struct gfe_softc *sc, enum gfe_txprio txprio)
{
	struct gfe_txqueue * const txq = &sc->sc_txq[txprio];
	volatile struct gt_eth_desc *txd;
	unsigned int i;
	bus_addr_t addr;

	GE_FUNC_ENTER(sc, "gfe_tx_start");

	sc->sc_intrmask &= ~(ETH_IR_TxEndHigh|ETH_IR_TxBufferHigh|
			     ETH_IR_TxEndLow |ETH_IR_TxBufferLow);

	if (sc->sc_flags & GE_NOFREE) {
		KASSERT(txq->txq_desc_mem.gdm_kva != NULL);
#ifndef __rtems__
		KASSERT(txq->txq_buf_mem.gdm_kva != NULL);
#endif
	} else {
		int error = gfe_tx_txqalloc(sc, txprio);
		if (error) {
			GE_FUNC_EXIT(sc, "!");
			return error;
		}
	}

	txq->txq_descs =
	    (volatile struct gt_eth_desc *) txq->txq_desc_mem.gdm_kva;
	txq->txq_desc_busaddr = txq->txq_desc_mem.gdm_map->dm_segs[0].ds_addr;
#ifndef __rtems__
	txq->txq_buf_busaddr = txq->txq_buf_mem.gdm_map->dm_segs[0].ds_addr;
#else
	/* never used */
	memset(&txq->txq_pendq,0,sizeof(txq->txq_pendq));
	memset(&txq->txq_sentq,0,sizeof(txq->txq_sentq));
	txq->txq_sentq.ifq_maxlen = 100000;
#endif

	txq->txq_pendq.ifq_maxlen = 10;
#ifndef __rtems__
	txq->txq_ei_gapcount = 0;
#endif
	txq->txq_nactive = 0;
	txq->txq_fi = 0;
	txq->txq_lo = 0;
#ifndef __rtems__
	txq->txq_ei_gapcount = 0;
	txq->txq_inptr = GE_TXBUF_SIZE;
	txq->txq_outptr = 0;
#endif
	for (i = 0, txd = txq->txq_descs,
	     addr = txq->txq_desc_busaddr + sizeof(*txd);
			i < GE_TXDESC_MAX - 1;
			i++, txd++, addr += sizeof(*txd)) {
		/*
		 * update the nxtptr to point to the next txd.
		 */
		txd->ed_cmdsts = 0;
		txd->ed_nxtptr = htogt32(addr);
	}
	txq->txq_descs[GE_TXDESC_MAX-1].ed_nxtptr =
	    htogt32(txq->txq_desc_busaddr);
	bus_dmamap_sync(sc->sc_dmat, txq->txq_desc_mem.gdm_map, 0,
	    GE_TXDESC_MEMSIZE, BUS_DMASYNC_PREREAD|BUS_DMASYNC_PREWRITE);

	switch (txprio) {
	case GE_TXPRIO_HI:
		txq->txq_intrbits = ETH_IR_TxEndHigh|ETH_IR_TxBufferHigh;
		txq->txq_esdcmrbits = ETH_ESDCMR_TXDH;
		txq->txq_epsrbits = ETH_EPSR_TxHigh;
		txq->txq_ectdp = ETH_ECTDP1(sc->sc_macno);
		GE_WRITE(sc, ECTDP1, txq->txq_desc_busaddr);
		break;

	case GE_TXPRIO_LO:
		txq->txq_intrbits = ETH_IR_TxEndLow|ETH_IR_TxBufferLow;
		txq->txq_esdcmrbits = ETH_ESDCMR_TXDL;
		txq->txq_epsrbits = ETH_EPSR_TxLow;
		txq->txq_ectdp = ETH_ECTDP0(sc->sc_macno);
		GE_WRITE(sc, ECTDP0, txq->txq_desc_busaddr);
		break;

	case GE_TXPRIO_NONE:
		break;
	}
#if 0
	GE_DPRINTF(sc, ("(ectdp=%#x", txq->txq_ectdp));
	gt_write(sc->sc_dev.dv_parent, txq->txq_ectdp, txq->txq_desc_busaddr);
	GE_DPRINTF(sc, (")"));
#endif

	/*
	 * If we are restarting, there may be packets in the pending queue
	 * waiting to be enqueued.  Try enqueuing packets from both priority
	 * queues until the pending queue is empty or there no room for them
	 * on the device.
	 */
	while (gfe_tx_enqueue(sc, txprio))
		continue;

	GE_FUNC_EXIT(sc, "");
	return 0;
}

void
gfe_tx_cleanup(struct gfe_softc *sc, enum gfe_txprio txprio, int flush)
{
	struct gfe_txqueue * const txq = &sc->sc_txq[txprio];

	GE_FUNC_ENTER(sc, "gfe_tx_cleanup");
	if (txq == NULL) {
		GE_FUNC_EXIT(sc, "");
		return;
	}

	if (!flush) {
		GE_FUNC_EXIT(sc, "");
		return;
	}

#ifdef __rtems__
	/* reclaim mbufs that were never sent */
	{
	struct mbuf *m;
		while ( txq->txq_sentq.ifq_head ) {
			IF_DEQUEUE(&txq->txq_sentq, m);
			m_freem(m);
		}
	}
#endif

	if ((sc->sc_flags & GE_NOFREE) == 0) {
		gfe_dmamem_free(sc, &txq->txq_desc_mem);
#ifndef __rtems__
		gfe_dmamem_free(sc, &txq->txq_buf_mem);
#endif
	}
	GE_FUNC_EXIT(sc, "-F");
}

void
gfe_tx_stop(struct gfe_softc *sc, enum gfe_whack_op op)
{
	GE_FUNC_ENTER(sc, "gfe_tx_stop");

	GE_WRITE(sc, ESDCMR, ETH_ESDCMR_STDH|ETH_ESDCMR_STDL);

	sc->sc_intrmask = gfe_tx_done(sc, GE_TXPRIO_HI, sc->sc_intrmask);
	sc->sc_intrmask = gfe_tx_done(sc, GE_TXPRIO_LO, sc->sc_intrmask);
	sc->sc_intrmask &= ~(ETH_IR_TxEndHigh|ETH_IR_TxBufferHigh|
			     ETH_IR_TxEndLow |ETH_IR_TxBufferLow);

	gfe_tx_cleanup(sc, GE_TXPRIO_HI, op == GE_WHACK_STOP);
	gfe_tx_cleanup(sc, GE_TXPRIO_LO, op == GE_WHACK_STOP);

	sc->sc_ec.ec_if.if_timer = 0;
	GE_FUNC_EXIT(sc, "");
}

int
gfe_intr(void *arg)
{
	struct gfe_softc * const sc = arg;
	uint32_t cause;
	uint32_t intrmask = sc->sc_intrmask;
	int claim = 0;
	int cnt;

	GE_FUNC_ENTER(sc, "gfe_intr");

	for (cnt = 0; cnt < 4; cnt++) {
		if (sc->sc_intrmask != intrmask) {
			sc->sc_intrmask = intrmask;
			GE_WRITE(sc, EIMR, sc->sc_intrmask);
		}
		cause = GE_READ(sc, EICR);
		cause &= sc->sc_intrmask;
		GE_DPRINTF(sc, (".%#" PRIx32, cause));
		if (cause == 0)
			break;

		claim = 1;

		GE_WRITE(sc, EICR, ~cause);
#ifndef GE_NORX
		if (cause & (ETH_IR_RxBuffer|ETH_IR_RxError))
			intrmask = gfe_rx_process(sc, cause, intrmask);
#endif

#ifndef GE_NOTX
		if (cause & (ETH_IR_TxBufferHigh|ETH_IR_TxEndHigh))
			intrmask = gfe_tx_done(sc, GE_TXPRIO_HI, intrmask);
		if (cause & (ETH_IR_TxBufferLow|ETH_IR_TxEndLow))
			intrmask = gfe_tx_done(sc, GE_TXPRIO_LO, intrmask);
#endif
		if (cause & ETH_IR_MIIPhySTC) {
			sc->sc_flags |= GE_PHYSTSCHG;
			/* intrmask &= ~ETH_IR_MIIPhySTC; */
		}
	}

	while (gfe_tx_enqueue(sc, GE_TXPRIO_HI))
		continue;
	while (gfe_tx_enqueue(sc, GE_TXPRIO_LO))
		continue;

	GE_FUNC_EXIT(sc, "");
	return claim;
}

#ifndef __rtems__
int
gfe_mii_mediachange (struct ifnet *ifp)
{
	struct gfe_softc *sc = ifp->if_softc;

	if (ifp->if_flags & IFF_UP)
		mii_mediachg(&sc->sc_mii);

	return (0);
}
void
gfe_mii_mediastatus (struct ifnet *ifp, struct ifmediareq *ifmr)
{
	struct gfe_softc *sc = ifp->if_softc;

	if (sc->sc_flags & GE_PHYSTSCHG) {
		sc->sc_flags &= ~GE_PHYSTSCHG;
		mii_pollstat(&sc->sc_mii);
	}
	ifmr->ifm_status = sc->sc_mii.mii_media_status;
	ifmr->ifm_active = sc->sc_mii.mii_media_active;
}

int
gfe_mii_read (struct device *self, int phy, int reg)
{
	return gt_mii_read(self, self->dv_parent, phy, reg);
}

void
gfe_mii_write (struct device *self, int phy, int reg, int value)
{
	gt_mii_write(self, self->dv_parent, phy, reg, value);
}

void
gfe_mii_statchg (struct device *self)
{
	/* struct gfe_softc *sc = (struct gfe_softc *) self; */
	/* do nothing? */
}

#else
int
gfe_mii_read(int phy, void *arg, unsigned reg, uint32_t *pval)
{
	struct gfe_softc *sc = arg;
	uint32_t data;
	int count = 10000;

	if ( 0 != phy )
		return -1; /* invalid index */

	phy = sc->sc_phyaddr;

	do {
		DELAY(10);
		data = GT_READ(sc, ETH_ESMIR);
	} while ((data & ETH_ESMIR_Busy) && count-- > 0);

	if (count == 0) {
		fprintf(stderr,"%s: mii read for phy %d reg %d busied out\n",
			sc->sc_dev.dv_xname, phy, reg);
		*pval = ETH_ESMIR_Value_GET(data);
		return -1;
	}

	GT_WRITE(sc, ETH_ESMIR, ETH_ESMIR_READ(phy, reg));

	count = 10000;
	do {
		DELAY(10);
		data = GT_READ(sc, ETH_ESMIR);
	} while ((data & ETH_ESMIR_ReadValid) == 0 && count-- > 0);

	if (count == 0)
		printf("%s: mii read for phy %d reg %d timed out\n",
			sc->sc_dev.dv_xname, phy, reg);
#if defined(GTMIIDEBUG)
	printf("%s: mii_read(%d, %d): %#x data %#x\n",
		sc->sc_dev.dv_xname, phy, reg, 
		data, ETH_ESMIR_Value_GET(data));
#endif
	*pval = ETH_ESMIR_Value_GET(data);
	return 0;
}

int
gfe_mii_write(int phy, void *arg, unsigned reg, uint32_t value)
{
	struct gfe_softc *sc = arg;
	uint32_t data;
	int count = 10000;

	if ( 0 != phy )
		return -1; /* invalid index */

	phy = sc->sc_phyaddr;

	do {
		DELAY(10);
		data = GT_READ(sc, ETH_ESMIR);
	} while ((data & ETH_ESMIR_Busy) && count-- > 0);

	if (count == 0) {
		fprintf(stderr, "%s: mii write for phy %d reg %d busied out (busy)\n",
			sc->sc_dev.dv_xname, phy, reg);
		return -1;
	}

	GT_WRITE(sc, ETH_ESMIR,
		 ETH_ESMIR_WRITE(phy, reg, value));

	count = 10000;
	do {
		DELAY(10);
		data = GT_READ(sc, ETH_ESMIR);
	} while ((data & ETH_ESMIR_Busy) && count-- > 0);

	if (count == 0)
		printf("%s: mii write for phy %d reg %d timed out\n",
			sc->sc_dev.dv_xname, phy, reg);
#if defined(GTMIIDEBUG)
	printf("%s: mii_write(%d, %d, %#x)\n", 
		sc->sc_dev.dv_xname, phy, reg, value);
#endif
	return 0;
}

#endif
int
gfe_whack(struct gfe_softc *sc, enum gfe_whack_op op)
{
	int error = 0;
	GE_FUNC_ENTER(sc, "gfe_whack");

	switch (op) {
	case GE_WHACK_RESTART:
#ifndef GE_NOTX
		gfe_tx_stop(sc, op);
#endif
		/* sc->sc_ec.ec_if.if_flags &= ~IFF_RUNNING; */
		/* FALLTHROUGH */
	case GE_WHACK_START:
#ifndef GE_NOHASH
		if (error == 0 && sc->sc_hashtable == NULL) {
			error = gfe_hash_alloc(sc);
			if (error)
				break;
		}
		if (op != GE_WHACK_RESTART)
			gfe_hash_fill(sc);
#endif
#ifndef GE_NORX
		if (op != GE_WHACK_RESTART) {
			error = gfe_rx_prime(sc);
			if (error)
				break;
		}
#endif
#ifndef GE_NOTX
		error = gfe_tx_start(sc, GE_TXPRIO_HI);
		if (error)
			break;
#endif
		sc->sc_ec.ec_if.if_flags |= IFF_RUNNING;
		GE_WRITE(sc, EPCR, sc->sc_pcr | ETH_EPCR_EN);
		GE_WRITE(sc, EPCXR, sc->sc_pcxr);
		GE_WRITE(sc, EICR, 0);
		GE_WRITE(sc, EIMR, sc->sc_intrmask);
#ifndef GE_NOHASH
		GE_WRITE(sc, EHTPR, sc->sc_hash_mem.gdm_map->dm_segs->ds_addr);
#endif
#ifndef GE_NORX
		GE_WRITE(sc, ESDCMR, ETH_ESDCMR_ERD);
		sc->sc_flags |= GE_RXACTIVE;
#endif
		/* FALLTHROUGH */
	case GE_WHACK_CHANGE:
		GE_DPRINTF(sc, ("(pcr=%#" PRIx32 ",imr=%#" PRIx32 ")",
		    GE_READ(sc, EPCR), GE_READ(sc, EIMR)));
		GE_WRITE(sc, EPCR, sc->sc_pcr | ETH_EPCR_EN);
		GE_WRITE(sc, EIMR, sc->sc_intrmask);
		gfe_ifstart(&sc->sc_ec.ec_if);
		GE_DPRINTF(sc, ("(ectdp0=%#" PRIx32 ", ectdp1=%#" PRIx32 ")",
		    GE_READ(sc, ECTDP0), GE_READ(sc, ECTDP1)));
		GE_FUNC_EXIT(sc, "");
		return error;
	case GE_WHACK_STOP:
		break;
	}

#ifdef GE_DEBUG
	if (error)
		GE_DPRINTF(sc, (" failed: %d\n", error));
#endif
	GE_WRITE(sc, EPCR, sc->sc_pcr);
	GE_WRITE(sc, EIMR, 0);
	sc->sc_ec.ec_if.if_flags &= ~IFF_RUNNING;
#ifndef GE_NOTX
	gfe_tx_stop(sc, GE_WHACK_STOP);
#endif
#ifndef GE_NORX
	gfe_rx_stop(sc, GE_WHACK_STOP);
#endif
#ifndef GE_NOHASH
	if ((sc->sc_flags & GE_NOFREE) == 0) {
		gfe_dmamem_free(sc, &sc->sc_hash_mem);
		sc->sc_hashtable = NULL;
	}
#endif

	GE_FUNC_EXIT(sc, "");
	return error;
}

int
gfe_hash_compute(struct gfe_softc *sc, const uint8_t eaddr[ETHER_ADDR_LEN])
{
	uint32_t w0, add0, add1;
	uint32_t result;
#ifdef __rtems__
	SPRINTFVARDECL;
#endif

	GE_FUNC_ENTER(sc, "gfe_hash_compute");
	add0 = ((uint32_t) eaddr[5] <<  0) |
	       ((uint32_t) eaddr[4] <<  8) |
	       ((uint32_t) eaddr[3] << 16);

	add0 = ((add0 & 0x00f0f0f0) >> 4) | ((add0 & 0x000f0f0f) << 4);
	add0 = ((add0 & 0x00cccccc) >> 2) | ((add0 & 0x00333333) << 2);
	add0 = ((add0 & 0x00aaaaaa) >> 1) | ((add0 & 0x00555555) << 1);

	add1 = ((uint32_t) eaddr[2] <<  0) |
	       ((uint32_t) eaddr[1] <<  8) |
	       ((uint32_t) eaddr[0] << 16);

	add1 = ((add1 & 0x00f0f0f0) >> 4) | ((add1 & 0x000f0f0f) << 4);
	add1 = ((add1 & 0x00cccccc) >> 2) | ((add1 & 0x00333333) << 2);
	add1 = ((add1 & 0x00aaaaaa) >> 1) | ((add1 & 0x00555555) << 1);

	GE_DPRINTF(sc, ("%s=", ether_sprintf(eaddr)));
	/*
	 * hashResult is the 15 bits Hash entry address.
	 * ethernetADD is a 48 bit number, which is derived from the Ethernet
	 *	MAC address, by nibble swapping in every byte (i.e MAC address
	 *	of 0x123456789abc translates to ethernetADD of 0x21436587a9cb).
	 */

	if ((sc->sc_pcr & ETH_EPCR_HM) == 0) {
		/*
		 * hashResult[14:0] = hashFunc0(ethernetADD[47:0])
		 *
		 * hashFunc0 calculates the hashResult in the following manner:
		 *   hashResult[ 8:0] = ethernetADD[14:8,1,0]
		 *		XOR ethernetADD[23:15] XOR ethernetADD[32:24]
		 */
		result = (add0 & 3) | ((add0 >> 6) & ~3);
		result ^= (add0 >> 15) ^ (add1 >>  0);
		result &= 0x1ff;
		/*
		 *   hashResult[14:9] = ethernetADD[7:2]
		 */
		result |= (add0 & ~3) << 7;	/* excess bits will be masked */
		GE_DPRINTF(sc, ("0(%#"PRIx32")", result & 0x7fff));
	} else {
#define	TRIBITFLIP	073516240	/* yes its in octal */
		/*
		 * hashResult[14:0] = hashFunc1(ethernetADD[47:0])
		 *
		 * hashFunc1 calculates the hashResult in the following manner:
		 *   hashResult[08:00] = ethernetADD[06:14]
		 *		XOR ethernetADD[15:23] XOR ethernetADD[24:32]
		 */
		w0 = ((add0 >> 6) ^ (add0 >> 15) ^ (add1)) & 0x1ff;
		/*
		 * Now bitswap those 9 bits
		 */
		result = 0;
		result |= ((TRIBITFLIP >> (((w0 >> 0) & 7) * 3)) & 7) << 6;
		result |= ((TRIBITFLIP >> (((w0 >> 3) & 7) * 3)) & 7) << 3;
		result |= ((TRIBITFLIP >> (((w0 >> 6) & 7) * 3)) & 7) << 0;

		/*
		 *   hashResult[14:09] = ethernetADD[00:05]
		 */
		result |= ((TRIBITFLIP >> (((add0 >> 0) & 7) * 3)) & 7) << 12;
		result |= ((TRIBITFLIP >> (((add0 >> 3) & 7) * 3)) & 7) << 9;
		GE_DPRINTF(sc, ("1(%#"PRIx32")", result));
	}
	GE_FUNC_EXIT(sc, "");
	return result & ((sc->sc_pcr & ETH_EPCR_HS_512) ? 0x7ff : 0x7fff);
}

int
gfe_hash_entry_op(struct gfe_softc *sc, enum gfe_hash_op op,
	enum gfe_rxprio prio, const uint8_t eaddr[ETHER_ADDR_LEN])
{
	uint64_t he;
	uint64_t *maybe_he_p = NULL;
	int limit;
	int hash;
	int maybe_hash = 0;

	GE_FUNC_ENTER(sc, "gfe_hash_entry_op");

	hash = gfe_hash_compute(sc, eaddr);

	if (sc->sc_hashtable == NULL) {
		panic("%s:%d: hashtable == NULL!", sc->sc_dev.dv_xname,
			__LINE__);
	}

	/*
	 * Assume we are going to insert so create the hash entry we
	 * are going to insert.  We also use it to match entries we
	 * will be removing.
	 */
	he = ((uint64_t) eaddr[5] << 43) |
	     ((uint64_t) eaddr[4] << 35) |
	     ((uint64_t) eaddr[3] << 27) |
	     ((uint64_t) eaddr[2] << 19) |
	     ((uint64_t) eaddr[1] << 11) |
	     ((uint64_t) eaddr[0] <<  3) |
	     HSH_PRIO_INS(prio) | HSH_V | HSH_R;

	/*
	 * The GT will search upto 12 entries for a hit, so we must mimic that.
	 */
	hash &= sc->sc_hashmask / sizeof(he);
	for (limit = HSH_LIMIT; limit > 0 ; --limit) {
		/*
		 * Does the GT wrap at the end, stop at the, or overrun the
		 * end?  Assume it wraps for now.  Stash a copy of the
		 * current hash entry.
		 */
		uint64_t *he_p = &sc->sc_hashtable[hash];
		uint64_t thishe = *he_p;

		/*
		 * If the hash entry isn't valid, that break the chain.  And
		 * this entry a good candidate for reuse.
		 */
		if ((thishe & HSH_V) == 0) {
			maybe_he_p = he_p;
			break;
		}

		/*
		 * If the hash entry has the same address we are looking for
		 * then ...  if we are removing and the skip bit is set, its
		 * already been removed.  if are adding and the skip bit is
		 * clear, then its already added.  In either return EBUSY
		 * indicating the op has already been done.  Otherwise flip
		 * the skip bit and return 0.
		 */
		if (((he ^ thishe) & HSH_ADDR_MASK) == 0) {
			if (((op == GE_HASH_REMOVE) && (thishe & HSH_S)) ||
			    ((op == GE_HASH_ADD) && (thishe & HSH_S) == 0))
				return EBUSY;
			*he_p = thishe ^ HSH_S;
			bus_dmamap_sync(sc->sc_dmat, sc->sc_hash_mem.gdm_map,
			    hash * sizeof(he), sizeof(he),
			    BUS_DMASYNC_PREWRITE);
			GE_FUNC_EXIT(sc, "^");
			return 0;
		}

		/*
		 * If we haven't found a slot for the entry and this entry
		 * is currently being skipped, return this entry.
		 */
		if (maybe_he_p == NULL && (thishe & HSH_S)) {
			maybe_he_p = he_p;
			maybe_hash = hash;
		}

		hash = (hash + 1) & (sc->sc_hashmask / sizeof(he));
	}

	/*
	 * If we got here, then there was no entry to remove.
	 */
	if (op == GE_HASH_REMOVE) {
		GE_FUNC_EXIT(sc, "?");
		return ENOENT;
	}

	/*
	 * If we couldn't find a slot, return an error.
	 */
	if (maybe_he_p == NULL) {
		GE_FUNC_EXIT(sc, "!");
		return ENOSPC;
	}

	/* Update the entry.
	 */
	*maybe_he_p = he;
	bus_dmamap_sync(sc->sc_dmat, sc->sc_hash_mem.gdm_map,
	    maybe_hash * sizeof(he), sizeof(he), BUS_DMASYNC_PREWRITE);
	GE_FUNC_EXIT(sc, "+");
	return 0;
}

#ifndef __rtems__
int
gfe_hash_multichg(struct ethercom *ec, const struct ether_multi *enm, u_long cmd)
{
	struct gfe_softc * const sc = ec->ec_if.if_softc;
	int error;
	enum gfe_hash_op op;
	enum gfe_rxprio prio;
#ifdef __rtems__
	SPRINTFVARDECL;
#endif

	GE_FUNC_ENTER(sc, "hash_multichg");
	/*
	 * Is this a wildcard entry?  If so and its being removed, recompute.
	 */
	if (memcmp(enm->enm_addrlo, enm->enm_addrhi, ETHER_ADDR_LEN) != 0) {
		if (cmd == SIOCDELMULTI) {
			GE_FUNC_EXIT(sc, "");
			return ENETRESET;
		}

		/*
		 * Switch in
		 */
		sc->sc_flags |= GE_ALLMULTI;
		if ((sc->sc_pcr & ETH_EPCR_PM) == 0) {
			sc->sc_pcr |= ETH_EPCR_PM;
			GE_WRITE(sc, EPCR, sc->sc_pcr);
			GE_FUNC_EXIT(sc, "");
			return 0;
		}
		GE_FUNC_EXIT(sc, "");
		return ENETRESET;
	}

	prio = GE_RXPRIO_MEDLO;
	op = (cmd == SIOCDELMULTI ? GE_HASH_REMOVE : GE_HASH_ADD);

	if (sc->sc_hashtable == NULL) {
		GE_FUNC_EXIT(sc, "");
		return 0;
	}

	error = gfe_hash_entry_op(sc, op, prio, enm->enm_addrlo);
	if (error == EBUSY) {
		printf("%s: multichg: tried to %s %s again\n",
		       sc->sc_dev.dv_xname,
		       cmd == SIOCDELMULTI ? "remove" : "add",
		       ether_sprintf(enm->enm_addrlo));
		GE_FUNC_EXIT(sc, "");
		return 0;
	}

	if (error == ENOENT) {
		printf("%s: multichg: failed to remove %s: not in table\n",
		       sc->sc_dev.dv_xname,
		       ether_sprintf(enm->enm_addrlo));
		GE_FUNC_EXIT(sc, "");
		return 0;
	}

	if (error == ENOSPC) {
		printf("%s: multichg: failed to add %s: no space; regenerating table\n",
		       sc->sc_dev.dv_xname,
		       ether_sprintf(enm->enm_addrlo));
		GE_FUNC_EXIT(sc, "");
		return ENETRESET;
	}
	GE_DPRINTF(sc, ("%s: multichg: %s: %s succeeded\n",
	       sc->sc_dev.dv_xname,
	       cmd == SIOCDELMULTI ? "remove" : "add",
	       ether_sprintf(enm->enm_addrlo)));
	GE_FUNC_EXIT(sc, "");
	return 0;
}
#endif

int
gfe_hash_fill(struct gfe_softc *sc)
{
	struct ether_multistep step;
	struct ether_multi *enm;
	int error;

	GE_FUNC_ENTER(sc, "gfe_hash_fill");

#ifndef __rtems__
	error = gfe_hash_entry_op(sc, GE_HASH_ADD, GE_RXPRIO_HI,
	    LLADDR(sc->sc_ec.ec_if.if_sadl));
#else
	error = gfe_hash_entry_op(sc, GE_HASH_ADD, GE_RXPRIO_HI, sc->sc_ec.ac_enaddr);
#endif
	if (error) {
		GE_FUNC_EXIT(sc, "!");
		return error;
	}

	sc->sc_flags &= ~GE_ALLMULTI;
	if ((sc->sc_ec.ec_if.if_flags & IFF_PROMISC) == 0)
		sc->sc_pcr &= ~ETH_EPCR_PM;
	else
		sc->sc_pcr |=  ETH_EPCR_PM;
	ETHER_FIRST_MULTI(step, &sc->sc_ec, enm);
	while (enm != NULL) {
		if (memcmp(enm->enm_addrlo, enm->enm_addrhi, ETHER_ADDR_LEN)) {
			sc->sc_flags |= GE_ALLMULTI;
			sc->sc_pcr |= ETH_EPCR_PM;
		} else {
			error = gfe_hash_entry_op(sc, GE_HASH_ADD,
			    GE_RXPRIO_MEDLO, enm->enm_addrlo);
			if (error == ENOSPC)
				break;
		}
		ETHER_NEXT_MULTI(step, enm);
	}

	GE_FUNC_EXIT(sc, "");
	return error;
}

int
gfe_hash_alloc(struct gfe_softc *sc)
{
	int error;
	GE_FUNC_ENTER(sc, "gfe_hash_alloc");
	sc->sc_hashmask = (sc->sc_pcr & ETH_EPCR_HS_512 ? 16 : 256)*1024 - 1;
	error = gfe_dmamem_alloc(sc, &sc->sc_hash_mem, 1, sc->sc_hashmask + 1,
	    BUS_DMA_NOCACHE);
	if (error) {
		printf("%s: failed to allocate %d bytes for hash table: %d\n",
		    sc->sc_dev.dv_xname, sc->sc_hashmask + 1, error);
		GE_FUNC_EXIT(sc, "");
		return error;
	}
	sc->sc_hashtable = (uint64_t *) sc->sc_hash_mem.gdm_kva;
	memset(sc->sc_hashtable, 0, sc->sc_hashmask + 1);
	bus_dmamap_sync(sc->sc_dmat, sc->sc_hash_mem.gdm_map,
	    0, sc->sc_hashmask + 1, BUS_DMASYNC_PREWRITE);
	GE_FUNC_EXIT(sc, "");
	return 0;
}
