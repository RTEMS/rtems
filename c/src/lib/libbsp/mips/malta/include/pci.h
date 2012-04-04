/**
 *  @file
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

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

#ifndef BSP_PCI_H
#define BSP_PCI_H

#include <rtems/pci.h>
#include <bsp.h>
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
struct pcibridge
{
  int bus;
  int slot;
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
void BSP_pciConfigDump(FILE *fp);

int indirect_pci_read_config_byte(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint8_t       *val
);

int indirect_pci_read_config_word(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint16_t      *val
);

int indirect_pci_read_config_dword(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint32_t *val
);

int indirect_pci_write_config_byte(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint8_t       val
);

int indirect_pci_write_config_word(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint16_t      val
);

int indirect_pci_write_config_dword(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint32_t      val
);

/* Can these be moved to the rtems pci.h? */
int FindPCIbridge( int mybus, struct pcibridge *pb );
void pci_list_devices( void );

const pci_config_access_functions pci_indirect_functions;

void     pci_out_le32( uint32_t base, uint32_t addr, uint32_t val);
void     pci_out_32( uint32_t base, uint32_t addr, uint32_t val);
uint8_t  pci_in_8    ( uint32_t base, uint32_t addr );
int16_t  pci_in_le16 ( uint32_t base, uint32_t addr );
uint32_t pci_in_le32 ( uint32_t base, uint32_t addr );
int16_t  pci_in_16 ( uint32_t base, uint32_t addr );
uint32_t pci_in_32 ( uint32_t base, uint32_t addr );
void     pci_out_8   ( uint32_t base, uint32_t addr, uint8_t val );
void     pci_out_le16( uint32_t base, uint32_t addr, uint16_t val );
void     pci_out_16( uint32_t base, uint32_t addr, uint16_t val );
void     pci_out_32  ( uint32_t base, uint32_t addr, uint32_t val);

#define out_32(_addr, _val)   pci_out_32(BSP_PCI_BASE_ADDRESS, _addr, _val)
#define out_le32(_addr, _val) pci_out_le32(BSP_PCI_BASE_ADDRESS, _addr, _val)
#define out_32(_addr, _val)   pci_out_32(BSP_PCI_BASE_ADDRESS, _addr, _val)
#define in_8(_addr)           pci_in_8( BSP_PCI_BASE_ADDRESS, _addr )
#define in_le16(_addr)        pci_in_le16( BSP_PCI_BASE_ADDRESS, _addr )
#define in_le32(_addr)        pci_in_le32( BSP_PCI_BASE_ADDRESS, _addr )
#define in_16(_addr)          pci_in_16( BSP_PCI_BASE_ADDRESS, _addr )
#define in_32(_addr)          pci_in_32( BSP_PCI_BASE_ADDRESS, _addr )
#define out_8(_addr,_val)     pci_out_8( BSP_PCI_BASE_ADDRESS, _addr, _val )
#define out_le16(_addr,_val)  pci_out_le16( BSP_PCI_BASE_ADDRESS, _addr, _val )
#define out_16(_addr,_val)    pci_out_16( BSP_PCI_BASE_ADDRESS, _addr, _val )

#endif /* BSP_PCI_H */
