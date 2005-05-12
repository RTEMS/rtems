/*
 *
 *	PCI defines and function prototypes
 *	Copyright 1994, Drew Eckhardt
 *	Copyright 1997, 1998 Martin Mares <mj@atrey.karlin.mff.cuni.cz>
 *
 *	For more information, please consult the following manuals (look at
 *	http://www.pcisig.com/ for how to get them):
 *
 *	PCI BIOS Specification
 *	PCI Local Bus Specification
 *	PCI to PCI Bridge Specification
 *	PCI System Design Guide
 *
 *      pci.h,v 1.2 2002/05/14 17:10:16 joel Exp
 *
 *      S. Kate Feng : Added support for Marvell and PLX. 3/5/2004.

 */

#ifndef RTEMS_PCI_H
#define RTEMS_PCI_H

#include <rtems/pci.h>

#define PCI_CAPABILITY_LIST_POINTER    0x34  /* <SKF> */

/* Device classes and subclasses */
#define PCI_CLASS_GT6426xAB_BRIDGE_PCI  0x0580    /* <SKF> */
          
/*
 * Vendor and card ID's: sort these numerically according to vendor
 * (and according to card ID within vendor). Send all updates to
 * <linux-pcisupport@cck.uni-kl.de>.
 */
#define PCI_VENDOR_ID_MARVELL           0x11ab   /* <SKF> */
#define PCI_DEVICE_ID_MARVELL_GT6426xAB 0x6430   /* <SKF> */

/* Note : The PLX Technology Inc. had the old VENDOR_ID.
 * See PCI_VENDOR_ID_PLX,  PCI_VENDOR_ID_PLX_9050, ..etc.
 */
#define PCI_VENDOR_ID_PLX2              0x3388   /* <SKF> */
#define PCI_DEVICE_ID_PLX2_PCI6154_HB2  0x26     /* <SKF> */

#define PCI_DEVICE_INTEL_82544EI_COPPER	0x1008	/* <SKF> */

struct _pin_routes
{
      int pin, int_name[4];
};
struct _int_map
{
      int bus, slot, opts;
      struct _pin_routes pin_route[5];
};

void FixupPCI( struct _int_map *, int (*swizzler)(int,int) );

#endif /* RTEMS_PCI_H */
