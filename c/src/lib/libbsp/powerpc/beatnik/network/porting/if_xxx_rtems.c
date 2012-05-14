#include <rtemscompat.h>

/* Template for driver task, setup and attach routines. To be instantiated
 * by defining the relevant symbols in header files.
 */

/* Copyright: Till Straumann <strauman@slac.stanford.edu>, 2005;
 * License:   see LICENSE file.
 */

#include <rtems/irq.h>

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include <sys/cdefs.h>

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/socket.h>
#include <sys/sockio.h>

#include <net/if.h>
#include <net/if_arp.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>

#include <net/if_media.h>

#ifdef IF_REG_HEADER
#include IF_REG_HEADER
#endif
#ifdef IF_VAR_HEADER
#include IF_VAR_HEADER
#endif

#include <rtemscompat1.h>

#define EX_EVENT RTEMS_EVENT_1
#undef  IRQ_AT_8259

NETDEV_DECL = { /*[0]:*/{ /* softc: */ { /* arpcom: */{ /* ac_if: */ { 0 }}}}};

static void net_daemon(void *arg);

#ifdef HAVE_LIBBSPEXT
#include <bsp/bspExt.h>
static void the_net_isr(void *);
#else
static void noop(const rtems_irq_connect_data *unused) {}
static int  noop1(const rtems_irq_connect_data *unused) { return 0;}
#if ISMINVERSION(4,6,99)
static void the_net_isr(rtems_irq_hdl_param);
#else
static void the_net_isr();
#if NETDRIVER_SLOTS > 1
#error only one instance supported (stupid IRQ API)
#else
static struct NET_SOFTC *thesc;
#endif
#endif
#endif

#if defined(NETDRIVER_PCI)
/* Public setup routine for PCI devices;
 * TODO: currently doesn't work for subsystem vendor/id , i.e. 
 *       devices behind a standard PCI interface...
 */
int
NET_EMBEMB(rtems_,NETDRIVER_PREFIX,_pci_setup)(int inst);
#endif

static unsigned
NET_EMBEMB(,NETDRIVER_PREFIX,_net_driver_ticks_per_sec) = 0;

/* other drivers may already have this created */
extern unsigned net_driver_ticks_per_sec
__attribute__((weak, alias(NET_STRSTR(NETDRIVER_PREFIX)"_net_driver_ticks_per_sec") ));

#ifdef DEBUG_MODULAR
net_drv_tbl_t * volatile METHODSPTR = 0;
#endif


int
NET_EMBEMB(rtems_,NETDRIVER_PREFIX,_attach)
	(struct rtems_bsdnet_ifconfig *config, int attaching)
{
	int error     = 0;
	device_t dev = net_dev_get(config);
	struct	NET_SOFTC *sc;
	struct  ifnet     *ifp;
#ifndef HAVE_LIBBSPEXT
	rtems_irq_connect_data irq_data = {
				0,
				the_net_isr,
#if ISMINVERSION(4,6,99)
				0,
#endif
				noop,
				noop,
				noop1 };
#endif

	if ( !dev )
		return 1;

	if ( !dev->d_softc.NET_SOFTC_BHANDLE_FIELD ) {
#if defined(NETDRIVER_PCI)
		device_printf(dev,NETDRIVER" unit not configured; executing setup...");
		/* setup should really be performed prior to attaching.
		 * Wipe the device; setup and re-obtain the device...
		 */
		memset(dev,0,sizeof(*dev));
		error = NET_EMBEMB(rtems_,NETDRIVER_PREFIX,_pci_setup)(-1);
		/* re-obtain the device */
		dev   = net_dev_get(config);
		if ( !dev ) {
			printk("Unable to re-assign device structure???\n");
			return 1;
		}
		if (error <= 0) {
			device_printf(dev,NETDRIVER" FAILED; unable to attach interface, sorry\n");
			return 1;
		}
		device_printf(dev,"success\n");
#else
		device_printf(dev,NETDRIVER" unit not configured; use 'rtems_"NETDRIVER"_setup()'\n");
		return 1;
#endif
	}

	if ( !net_driver_ticks_per_sec )
		rtems_clock_get( RTEMS_CLOCK_GET_TICKS_PER_SECOND, &net_driver_ticks_per_sec );

	sc  = device_get_softc( dev );
	ifp = &sc->arpcom.ac_if;

#ifdef DEBUG_MODULAR
	if (!METHODSPTR) {
		device_printf(dev,NETDRIVER": method pointer not set\n");
		return -1;
	}
#endif

	if ( attaching ) {
		if ( ifp->if_init ) {
			device_printf(dev,NETDRIVER" Driver already attached.\n");
			return -1;
		}
		if ( config->hardware_address ) {
			/* use configured MAC address */
			memcpy(sc->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);
		} else {
#ifdef NET_READ_MAC_ADDR
			NET_READ_MAC_ADDR(sc);
#endif
		}
		if ( METHODSPTR->n_attach(dev) ) {
			device_printf(dev,NETDRIVER"_attach() failed\n");
			return -1;
		}
	} else {
		if ( !ifp->if_init ) {
			device_printf(dev,NETDRIVER" Driver not attached.\n");
			return -1;
		}
		if ( METHODSPTR->n_detach ) {
			if ( METHODSPTR->n_detach(dev) ) {
				device_printf(dev,NETDRIVER"_detach() failed\n");
				return -1;
			}
		} else {
			device_printf(dev,NETDRIVER"_detach() not implemented\n");
			return -1;
		}
	}


	if ( !sc->tid )
		sc->tid = rtems_bsdnet_newproc(NETDRIVER"d", 4096, net_daemon, sc);

	if (attaching) {
#ifdef DEBUG
		printf("Installing IRQ # %i\n",sc->irq_no);
#endif
#ifdef HAVE_LIBBSPEXT
		if ( bspExtInstallSharedISR(sc->irq_no, the_net_isr, sc, 0) )
#else
		/* BSP dependent :-( */
		irq_data.name   = sc->irq_no;
#if ISMINVERSION(4,6,99)
		irq_data.handle = (rtems_irq_hdl_param)sc;
#else
		thesc = sc;
#endif
		if ( ! BSP_install_rtems_irq_handler( &irq_data ) )
#endif
		{
			fprintf(stderr,NETDRIVER": unable to install ISR\n");
			error = -1;
		}
	} else {
		if ( sc->irq_no ) {
#ifdef DEBUG
		printf("Removing IRQ # %i\n",sc->irq_no);
#endif
#ifdef HAVE_LIBBSPEXT
		if (bspExtRemoveSharedISR(sc->irq_no, the_net_isr, sc))
#else
		/* BSP dependent :-( */
		irq_data.name   = sc->irq_no;
#if ISMINVERSION(4,6,99)
		irq_data.handle = (rtems_irq_hdl_param)sc;
#endif
		if ( ! BSP_remove_rtems_irq_handler( &irq_data ) )
#endif
		{
			fprintf(stderr,NETDRIVER": unable to uninstall ISR\n");
			error = -1;
		}
		}
	}
	return error;
}

static void
the_net_isr(
#ifdef HAVE_LIBBSPEXT
void *thesc
#elif ISMINVERSION(4,6,99)
rtems_irq_hdl_param thesc
#endif
)
{
struct NET_SOFTC *sc = thesc;

	/* disable interrupts */
	NET_DISABLE_IRQS(sc);

	rtems_event_send( sc->tid, EX_EVENT );
}

static void net_daemon(void *arg)
{
struct NET_SOFTC *sc = arg;
rtems_event_set evs;

	for (;;) {
		rtems_bsdnet_event_receive(
				EX_EVENT,
				RTEMS_WAIT | RTEMS_EVENT_ANY,
				RTEMS_NO_TIMEOUT,
				&evs);

		METHODSPTR->n_intr(sc);

		/* re-enable interrupts */
		NET_ENABLE_IRQS(sc);
	}
}

static struct NET_SOFTC *
net_drv_check_unit(int unit)
{
	unit--;
	if ( unit < 0 || unit >= NETDRIVER_SLOTS ) {
		fprintf(stderr,"Invalid unit # %i (not in %i..%i)\n", unit+1, 1, NETDRIVER_SLOTS);
		return 0;
	}

	if ( THEDEVS[unit].d_name ) {
		fprintf(stderr,"Unit %i already set up\n", unit+1);
		return 0;
	}

	memset( &THEDEVS[unit], 0, sizeof(THEDEVS[0]) );

	return &THEDEVS[unit].d_softc;
}

struct rtems_bsdnet_ifconfig NET_EMBEMB(NETDRIVER_PREFIX,_dbg,_config) = {
	NETDRIVER"1",
	NET_EMBEMB(rtems_,NETDRIVER_PREFIX,_attach),
	0
};

#ifdef DEBUG
void
NET_EMBEMB(rtems_,NETDRIVER_PREFIX,_bringup)(char *ipaddr)
{
short flags;
struct sockaddr_in addr;
char *mask;


	if (!ipaddr) {
		printf("Need an ip[/mask] argument (dot notation)\n");
		return;
	}

	ipaddr = strdup(ipaddr);

	if ( (mask = strchr(ipaddr,'/')) ) {
		*mask++=0;
	} else {
		mask = "255.255.255.0";
	}

#if defined(NETDRIVER_PCI)
	/* this fails if already setup */
	NET_EMBEMB(rtems_,NETDRIVER_PREFIX,_pci_setup)(-1);
#endif
	rtems_bsdnet_attach(&NET_EMBEMB(NETDRIVER_PREFIX,_dbg,_config));

	flags = IFF_UP /*| IFF_PROMISC*/;
	if ( rtems_bsdnet_ifconfig(NETDRIVER"1",SIOCSIFFLAGS,&flags) < 0 ) {
		printf("Can't bring '"NETDRIVER"1' up\n");
		goto cleanup;
	}
	memset(&addr,0,sizeof(addr));
	addr.sin_len = sizeof(addr);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(mask);
	if ( rtems_bsdnet_ifconfig(NETDRIVER"1",SIOCSIFNETMASK,&addr) < 0 ) {
		printf("Unable to set netmask on '"NETDRIVER"1'\n");
		goto cleanup;
	}
	addr.sin_addr.s_addr = inet_addr(ipaddr);
	if ( rtems_bsdnet_ifconfig(NETDRIVER"1",SIOCSIFADDR,&addr) < 0 ) {
		printf("Unable to set address on '"NETDRIVER"1'\n");
		goto cleanup;
	}
cleanup:
	the_real_free (ipaddr);
}

int
NET_EMBEMB(rtems_,NETDRIVER_PREFIX,_bringdown)()
{
short flags;
	flags = 0;
	if ( rtems_bsdnet_ifconfig(NETDRIVER"1",SIOCSIFFLAGS,&flags) < 0 ) {
		printf("Can't bring '"NETDRIVER"1' down\n");
		return -1;
	}
	
	rtems_bsdnet_detach(&NET_EMBEMB(NETDRIVER_PREFIX,_dbg,_config));
	return 0;
}
#endif


#if defined(NETDRIVER_PCI) && !defined(NETDRIVER_OWN_SETUP)
/* Public setup routine for PCI devices;
 * TODO: currently doesn't work for subsystem vendor/id , i.e. 
 *       devices behind a standard PCI interface...
 * passing 'inst' > only sets-up the 'inst'th card; passing
 * 'inst' == 0 sets-up all matching cards.
 */
int
NET_EMBEMB(rtems_,NETDRIVER_PREFIX,_pci_setup)(int inst)
{
unsigned b,d,f,i,isio,unit;
rtemscompat_32_t base;
unsigned short cmd,id;
unsigned char  h;
struct NET_SOFTC *sc;
unsigned try[] = { PCI_BASE_ADDRESS_0, PCI_BASE_ADDRESS_1, 0 };

#ifdef DEBUG_MODULAR
	if ( !METHODSPTR ) {
		fprintf(stderr,NETDRIVER": Methods pointer not set\n");
		return -1;
	}
#endif

	/* 0 can be reached when looking for the desired instance */
	if ( 0 == inst )
		inst = -1;

#ifdef HAVE_LIBBSPEXT
	/* make sure it's initialized */
	bspExtInit();
#endif

	/* scan PCI for supported devices */
	for ( b=0, sc=0, unit=0; b<pci_bus_count(); b++ ) {
		for ( d=0; d<PCI_MAX_DEVICES; d++ ) {
			pci_read_config_word(b,d,0,PCI_VENDOR_ID,&id);
			if ( 0xffff == id )
					continue; /* empty slot */

			pci_read_config_byte(b,d,0,PCI_HEADER_TYPE,&h);
			h = h&0x80 ? PCI_MAX_FUNCTIONS : 1; /* multifunction device ? */

			for ( f=0;  f<h; f++ ) {
				if ( !sc && !(sc=net_drv_check_unit(unit+1))) {
					fprintf(stderr,"Not enough driver slots; stop looking for more devices...\n");
					return unit;
				}
				pci_read_config_word(b,d,f,PCI_VENDOR_ID,&id);
				if ( 0xffff == id )
					continue; /* empty slot */

				pci_read_config_word(b,d,f,PCI_CLASS_DEVICE,&id);
				if ( PCI_CLASS_NETWORK_ETHERNET != id )
					continue; /* only look at ethernet devices */
				
				sc->b = b;
				sc->d = d;
				sc->f = f;

				for ( i=0, base=0, isio=0; try[i]; i++ ) {
					pci_read_config_dword(b,d,f,try[i],&base);
					if ( base ) {
						if ( (isio = (PCI_BASE_ADDRESS_SPACE_IO == (base & PCI_BASE_ADDRESS_SPACE )) ) ) {
#ifdef NET_CHIP_PORT_IO
							base &= PCI_BASE_ADDRESS_IO_MASK;
							sc->NET_SOFTC_BHANDLE_FIELD = PCI_IO_2LOCAL(base,b);
#ifdef DEBUG
							printf("Found PCI I/O Base 0x%08x\n", (unsigned)base);
#endif
#else
							base = 0;
							continue;
#endif
						} else {
#ifdef NET_CHIP_MEM_IO
							base &= PCI_BASE_ADDRESS_MEM_MASK;
							sc->NET_SOFTC_BHANDLE_FIELD = PCI2LOCAL(base,b);
#ifdef DEBUG
							printf("Found PCI MEM Base 0x%08x\n", (unsigned)base);
#endif
#else
							base = 0;
							continue;
#endif
						}
					break;
					}
				}
				if ( !base ) {
#ifdef DEBUG
					fprintf(stderr, NETDRIVER": (warning) Neither PCI base address 0 nor 1 are configured; skipping bus %i, slot %i, fn %i...\n",b,d,f);	
#endif
					continue;
				}

				if ( 0 == METHODSPTR->n_probe(&THEDEVS[unit]) && (inst < 0 || !--inst) ) {
					pci_read_config_word(b,d,f,PCI_COMMAND,&cmd);
					pci_write_config_word(b,d,f,PCI_COMMAND,
						cmd | (isio ? PCI_COMMAND_IO : PCI_COMMAND_MEMORY) | PCI_COMMAND_MASTER );
					pci_read_config_byte(b,d,f,PCI_INTERRUPT_LINE,&sc->irq_no);
					printf(NETDRIVER": card found @PCI[%s] 0x%08x (local 0x%08x), IRQ %i\n",
						(isio ? "io" : "mem"), (unsigned)base, sc->NET_SOFTC_BHANDLE_FIELD, sc->irq_no);

					sc = 0; /* need to allocate a new slot */
					unit++;

					if ( 0 == inst ) {
						/* found desired instance */
						goto terminated;
					}
				}
			}
		}
	}

terminated:
	return unit;
}
#else

/* simple skeleton
int
NET_EMBEMB(rtems_,NETDRIVER_PREFIX,_setup)(
	int		unit,
	void	*base_addr,
	int		irq_no)
{
struct NET_SOFTC *sc;
	if ( !(sc=net_drv_check_unit(unit)) ) {
		fprintf(stderr,"Bad unit number -- (not enought driver slots?)\n");
		return 0;
	}
	sc->NET_SOFTC_BHANDLE_FIELD = base_addr;
	if ( 0 == METHODSPTR->n_probe(&THEDEVS[unit-1]) ) {
		sc->irq_no					= irq_no;
		printf(NETDRIVER": Unit %i set up\n", unit);
		return unit;
	}
	return 0;
}
*/

#endif
