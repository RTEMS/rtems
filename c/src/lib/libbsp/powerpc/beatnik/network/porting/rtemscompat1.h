#ifndef RTEMS_COMPAT1_BSD_NET_H
#define RTEMS_COMPAT1_BSD_NET_H

/* BSD -> RTEMS conversion wrappers; stuff that must be defined
 *        after most BSD headers are included.
 */

#include <netinet/in.h>
#include <netinet/if_ether.h>

/* Copyright: Till Straumann <strauman@slac.stanford.edu>, 2005;
 * License:   see LICENSE file.
 */

typedef struct _netdev_t {
	struct NET_SOFTC d_softc;	/* MUST BE FIRST FIELD */
	char			 *d_name;
	char			 *d_desc;
	int              d_unit;
	int				 flags;
	/* pointer to ifconfig only valid during excution of
	 * the n_attach/n_detach methods (see below)
	 */
	struct rtems_bsdnet_ifconfig *d_ifconfig;
} netdev_t;

#define THEDEVS	NET_EMBEMB(the_,NETDRIVER_PREFIX,_devs)
#define NETDEV_DECL netdev_t THEDEVS[NETDRIVER_SLOTS]

extern NETDEV_DECL;

typedef struct _net_drv_tbl {
	int (*n_probe)(device_t);
	int (*n_attach)(device_t);
	int (*n_detach)(device_t);
	void (*n_intr)(void *);
} net_drv_tbl_t;

static inline netdev_t *
net_dev_get(struct rtems_bsdnet_ifconfig *config)
{
	int     unitNo;
	char *unitName;

	unitNo = rtems_bsdnet_parse_driver_name(config, &unitName);
	if ( unitNo < 0 )
		return 0;

	if ( unitNo <=0 || unitNo > NETDRIVER_SLOTS ) {
		device_printf(dev, "Bad "NETDRIVER" unit number.\n");
		return 0;
	}

	if ( THEDEVS[unitNo-1].d_unit && THEDEVS[unitNo-1].d_unit != unitNo ) {
		device_printf(dev, "Unit # mismatch !!??\n");
		return 0;
	}

	THEDEVS[unitNo-1].d_unit = unitNo;
	THEDEVS[unitNo-1].d_name = unitName;
	THEDEVS[unitNo-1].d_ifconfig = config;
	
	return &THEDEVS[unitNo - 1];
}

/* kludge; that's why softc needs to be first */
static inline netdev_t *
softc_get_device(struct NET_SOFTC *sc)
{
	return (netdev_t *)sc;
}

static inline struct NET_SOFTC *
device_get_softc(netdev_t *dev)
{ return &dev->d_softc; }

static inline int
device_get_unit(netdev_t *dev)
{ return dev->d_unit; }

static inline char *
device_get_name(netdev_t *dev)
{ return dev->d_name; }

static inline void
if_initname(struct ifnet *ifp, char *name, int unit)
{
	ifp->if_name = name;
	ifp->if_unit = unit;
}

static inline void
device_set_desc(netdev_t *dev, char *str)
{
	dev->d_desc = str;
}

static inline void
device_set_desc_copy(netdev_t *dev, char *str)
{
	dev->d_desc = strdup(str);
}


static inline int
device_is_attached(netdev_t *dev)
{
	return dev->d_softc.arpcom.ac_if.if_addrlist && dev->d_softc.arpcom.ac_if.if_init;
}

#ifdef NETDRIVER_PCI
#include NETDRIVER_PCI
#include <pcireg.h>

static inline unsigned
pci_read_config(device_t dev, unsigned addr, unsigned width)
{
rtemscompat_32_t d;
unsigned short   s;
unsigned char    b;
struct NET_SOFTC *sc = device_get_softc(dev);
	switch (width) {
		case 1:	pci_read_config_byte(sc->b, sc->d, sc->f, addr, &b);
				return b;
		case 2:	pci_read_config_word(sc->b, sc->d, sc->f, addr, &s);
				return s;
		case 4:	pci_read_config_dword(sc->b, sc->d, sc->f, addr, &d);
				return d;
		default:
				break;
	}
	return 0xdeadbeef;
}

static inline void
pci_write_config(device_t dev, unsigned addr, unsigned width, unsigned val)
{
struct NET_SOFTC *sc = device_get_softc(dev);
	switch (width) {
		case 1:	pci_write_config_byte(sc->b, sc->d, sc->f, addr, val);
				return ;
		case 2:	pci_write_config_word(sc->b, sc->d, sc->f, addr, val);
				return ;
		case 4:	pci_write_config_dword(sc->b, sc->d, sc->f, addr, val);
				return ;
		default:
				break;
	}
}


static inline unsigned short
pci_get_vendor(device_t dev)
{
	return pci_read_config(dev, PCIR_VENDOR, 2);
}

static inline unsigned short
pci_get_device(device_t dev)
{
	return pci_read_config(dev, PCIR_DEVICE, 2);
}

static inline unsigned short
pci_get_subvendor(device_t dev)
{
	return pci_read_config(dev, PCIR_SUBVEND_0, 2);
}

static inline unsigned short
pci_get_subdevice(device_t dev)
{
	return pci_read_config(dev, PCIR_SUBDEV_0, 2);
}


static inline void
pci_enable_busmaster(device_t dev)
{
	pci_write_config(
		dev,
		PCIR_COMMAND,
		2,
		pci_read_config(dev, PCIR_COMMAND, 2)
		| PCIM_CMD_BUSMASTEREN);
}

#define mtx_init(a,b,c,d) 	do {} while(0)
#define mtx_initialized(ma) (1)
#define mtx_destroy(ma)   	do {} while(0)
#define mtx_lock(a) 		do {} while(0)
#define mtx_unlock(a) 		do {} while(0)
#define mtx_assert(a,b)		do {} while(0)

#define callout_handle_init(x) do {} while (0)
#define untimeout(a...) do {} while (0)

#if !ISMINVERSION(4,6,99)
#define pci_bus_count BusCountPCI
#endif

#endif

/* Ugly hack to allow unloading/reloading the driver core.
 * Needed because rtems' bsdnet release doesn't implement
 * if_detach(). Therefore, we bring the interface down but
 * keep the device record alive...
 */
static inline void
__ether_ifdetach(struct ifnet *ifp)
{
		ifp->if_flags = 0;
		ifp->if_ioctl = 0;
		ifp->if_start = 0;
		ifp->if_watchdog = 0;
		ifp->if_init  = 0;
}

#endif
