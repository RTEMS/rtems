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
 */

#ifndef BSP_POWERPC_PCI_H
#define BSP_POWERPC_PCI_H

#include <rtems/pci.h>
#include <stdio.h>

struct _pin_routes
{
      int pin, int_name[4];
};
struct _int_map
{
      int bus, slot, opts;
      struct _pin_routes pin_route[5];
};

/* If there's a conflict between a name in the routing table and
 * what's already set on the device, reprogram the device setting
 * to reflect int_name[0] for the routing table entry
 */
#define PCI_FIXUP_OPT_OVERRIDE_NAME	(1<<0)

void FixupPCI( const struct _int_map *, int (*swizzler)(int,int) );

/* FIXME: This probably belongs into rtems/pci.h */
extern unsigned char pci_bus_count();

/* FIXME: This also is generic and could go into rtems/pci.h */

/* Scan pci config space and run a user callback on each
 * device present; the user callback may return 0 to
 * continue the scan or a value > 0 to abort the scan.
 * Return values < 0 are reserved and must not be used.
 *
 * RETURNS: a (opaque) handle pointing to the bus/slot/fn-triple
 *          just after where the scan was aborted by a callback
 *          returning 1 (see above) or NULL if all devices were
 *          scanned.
 *          The handle may be passed to this routine to resume the
 *          scan continuing with the device after the one causing the
 *          abort.
 *          Pass a NULL 'handle' argument to start scanning from
 *          the beginning (bus/slot/fn = 0/0/0).
 */
typedef void *BSP_PciScanHandle;
typedef int (*BSP_PciScannerCb)(int bus, int slot, int fun, void *uarg);

BSP_PciScanHandle
BSP_pciScan(BSP_PciScanHandle handle, BSP_PciScannerCb cb, void *uarg);

/* Dump basic config. space info to a file. The argument may
 * be NULL in which case 'stdout' is used.
 * NOTE: the C-library must be functional before you can use
 *       this routine.
 */
void
BSP_pciConfigDump(FILE *fp);

#endif /* BSP_POWERPC_PCI_H */
