/* Author: T. Straumann <strauman@slac.stanford.edu>; see ../../LICENSE */
#include "rtemscompat.h"
#include "gtethreg.h"

#include <bsp/early_enet_link_status.h>
#include <bsp/if_gfe_pub.h>
#include <bsp/irq.h>

/* from if_gfe.c */
#define	GE_READ(sc, reg) \
	bus_space_read_4((sc)->sc_gt_memt, (sc)->sc_memh, ETH__ ## reg)
#define	GE_WRITE(sc, reg, v) \
	bus_space_write_4((sc)->sc_gt_memt, (sc)->sc_memh, ETH__ ## reg, (v))

#define	GT_READ(sc, reg) \
	bus_space_read_4((sc)->sc_gt_memt, (sc)->sc_gt_memh, reg)
#define	GT_WRITE(sc, reg, v) \
	bus_space_write_4((sc)->sc_gt_memt, (sc)->sc_gt_memh, reg, (v))

#include "if_xxx_rtems.c"

#include <bsp.h>
#include <libcpu/io.h>

int
NET_EMBEMB(rtems_,NETDRIVER_PREFIX,_setup)(
	int					unit,
	char 				*ea,
	uint32_t			base_addr)
{
struct NET_SOFTC *sc;

	if ( !ea ) {
		fprintf(stderr,"Station address argument missing\n");
		return 0;
	}

	if ( !(sc=net_drv_check_unit(unit)) ) {
		fprintf(stderr,"Bad unit number -- (not enought driver slots?)\n");
		return 0;
	}

	unit--;

#ifdef DEBUG_MODULAR
	if ( !METHODSPTR ) {
		fprintf(stderr,"Methods not set -- module not loaded?\n");
		return 0;
	}
#endif

	if ( !base_addr ) {
#ifdef BSP_MV64x60_BASE
		base_addr = BSP_MV64x60_BASE;
#else
		fprintf(stderr,"Missing GT64260 base address\n");
		return 0;
#endif
	}
	sc->sc_gt_memh = base_addr;
	/* must set this as well to indicate that the device is set up */
	sc->NET_SOFTC_BHANDLE_FIELD = base_addr + 0x2400 + (unit<<10);
	sc->sc_macno   = unit;
	memcpy( sc->arpcom.ac_enaddr, ea, ETHER_ADDR_LEN);

	if ( 0 == METHODSPTR->n_probe(&THEDEVS[unit]) ) {
		printf(NETDRIVER": Unit %i set up\n", unit + 1);
		sc->irq_no = BSP_IRQ_ETH0 + unit;
		return 1;
	}
	return 0;
}

static int
gfe_early_init(int idx)
{
struct gfe_softc	*sc;
uint32_t			d;

	if ( idx < 0 || idx >= NETDRIVER_SLOTS )
		return -1;

	sc = device_get_softc(&the_gfe_devs[idx]);
	d  = bus_space_read_4(sc->sc_gt_memt, sc->sc_gt_memh, ETH_EPAR);

	sc->sc_phyaddr      = ETH_EPAR_PhyAD_GET(d, sc->sc_macno);
	sc->sc_dev.dv_xname = NETDRIVER;
	return 0;
}

static int
gfe_early_read_phy(int idx, unsigned reg)
{
uint32_t rval;
struct gfe_softc	*sc;

	if ( idx < 0 || idx >= NETDRIVER_SLOTS )
		return -1;

	sc = device_get_softc(&the_gfe_devs[idx]);

	if ( gfe_mii_read( 0, sc, reg, &rval) )
		return -1;
	return rval & 0xffff;
}


static int
gfe_early_write_phy(int idx, unsigned reg, unsigned val)
{
struct gfe_softc	*sc;

	if ( idx < 0 || idx >= NETDRIVER_SLOTS )
		return -1;

	sc = device_get_softc(&the_gfe_devs[idx]);

	return gfe_mii_write( 0, sc, reg, val);
}

rtems_bsdnet_early_link_check_ops
rtems_gfe_early_link_check_ops = {
	init:		gfe_early_init,
	read_phy:	gfe_early_read_phy,
	write_phy:	gfe_early_write_phy,
	name:		NETDRIVER,
	num_slots:	NETDRIVER_SLOTS
};
