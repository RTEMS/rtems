/* PCI configuration space access */

/* 
 * Acknowledgements:
 * Valuable information was obtained from the following drivers
 *   netbsd: (C) Allegro Networks Inc; Wasabi Systems Inc.
 *   linux:  (C) MontaVista, Software, Inc; Chris Zankel, Mark A. Greer.
 *   rtems:  (C) Brookhaven National Laboratory; K. Feng
 */

/*
 * Original file header of libbsp/shared/pci.c where this file is based upon.
 *
 *  Copyright (C) 1999 valette@crf.canon.fr
 *
 *  This code is heavily inspired by the public specification of STREAM V2
 *  that can be found at :
 *
 *      <http://www.chorus.com/Documentation/index.html> by following
 *  the STREAM API Specification Document link.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Till Straumann, <strauman@slac.stanford.edu>, 1/2002
 *   - separated bridge detection code out of this file
 */

#include <rtems.h>
#include <bsp.h>
#include <libcpu/io.h>
#include <bsp/pci.h>
#include <rtems/bspIo.h>
#include <stdint.h>

/* set to max so we get early access to hose 0 */
unsigned	BSP_pci_hose1_bus_base = (unsigned)-1;

#define MV64x60_PCI0_CONFIG_ADDR	(BSP_MV64x60_BASE + 0xcf8)
#define MV64x60_PCI0_CONFIG_DATA	(BSP_MV64x60_BASE + 0xcfc)
#define MV64x60_PCI1_CONFIG_ADDR	(BSP_MV64x60_BASE + 0xc78)
#define MV64x60_PCI1_CONFIG_DATA	(BSP_MV64x60_BASE + 0xc7c)

#define PCI_BUS2HOSE(bus) (bus<BSP_pci_hose1_bus_base?0:1)

void detect_host_bridge(void)
{

}

typedef struct {
	volatile unsigned char *pci_config_addr;
	volatile unsigned char *pci_config_data;
} PciHoseCfg;

static PciHoseCfg hoses[2] = {
	{
		pci_config_addr:	(volatile unsigned char *)(MV64x60_PCI0_CONFIG_ADDR),
		pci_config_data:	(volatile unsigned char *)(MV64x60_PCI0_CONFIG_DATA),
	},
	{
		pci_config_addr:	(volatile unsigned char *)(MV64x60_PCI1_CONFIG_ADDR),
		pci_config_data:	(volatile unsigned char *)(MV64x60_PCI1_CONFIG_DATA),
	}
};

#define pci hoses[hose]

#define HOSE_PREAMBLE \
	uint8_t hose; \
		if (bus < BSP_pci_hose1_bus_base) { \
			hose = 0; \
		} else { \
			hose = 1; \
			bus -= BSP_pci_hose1_bus_base; \
		}


/* Sigh; we have to copy those out from the shared area... */
static int
indirect_pci_read_config_byte(unsigned char bus, unsigned char slot,
			      unsigned char function, 
			      unsigned char offset, uint8_t *val) {
HOSE_PREAMBLE;
	out_be32((volatile unsigned *) pci.pci_config_addr, 
		 0x80|(bus<<8)|(PCI_DEVFN(slot,function)<<16)|((offset&~3)<<24));
	*val = in_8(pci.pci_config_data + (offset&3));
	return PCIBIOS_SUCCESSFUL;
}

static int
indirect_pci_read_config_word(unsigned char bus, unsigned char slot,
			      unsigned char function, 
			      unsigned char offset, uint16_t *val) {
HOSE_PREAMBLE;
	*val = 0xffff; 
	if (offset&1) return PCIBIOS_BAD_REGISTER_NUMBER;
	out_be32((unsigned int*) pci.pci_config_addr, 
		 0x80|(bus<<8)|(PCI_DEVFN(slot,function)<<16)|((offset&~3)<<24));
	*val = in_le16((volatile unsigned short *)(pci.pci_config_data + (offset&3)));
	return PCIBIOS_SUCCESSFUL;
}

static int
indirect_pci_read_config_dword(unsigned char bus, unsigned char slot,
			      unsigned char function, 
			      unsigned char offset, uint32_t *val) {
HOSE_PREAMBLE;
	*val = 0xffffffff; 
	if (offset&3) return PCIBIOS_BAD_REGISTER_NUMBER;
	out_be32((unsigned int*) pci.pci_config_addr, 
		 0x80|(bus<<8)|(PCI_DEVFN(slot,function)<<16)|(offset<<24));
	*val = in_le32((volatile unsigned *)pci.pci_config_data);
	return PCIBIOS_SUCCESSFUL;
}

static int
indirect_pci_write_config_byte(unsigned char bus, unsigned char slot,
			       unsigned char function, 
			       unsigned char offset, uint8_t val) {
HOSE_PREAMBLE;
	out_be32((unsigned int*) pci.pci_config_addr, 
		 0x80|(bus<<8)|(PCI_DEVFN(slot,function)<<16)|((offset&~3)<<24));
	out_8(pci.pci_config_data + (offset&3), val);
	return PCIBIOS_SUCCESSFUL;
}

static int
indirect_pci_write_config_word(unsigned char bus, unsigned char slot,
			       unsigned char function, 
			       unsigned char offset, uint16_t val) {
HOSE_PREAMBLE;
	if (offset&1) return PCIBIOS_BAD_REGISTER_NUMBER;
	out_be32((unsigned int*) pci.pci_config_addr, 
		 0x80|(bus<<8)|(PCI_DEVFN(slot,function)<<16)|((offset&~3)<<24));
	out_le16((volatile unsigned short *)(pci.pci_config_data + (offset&3)), val);
	return PCIBIOS_SUCCESSFUL;
}

static int
indirect_pci_write_config_dword(unsigned char bus, unsigned char slot,
				unsigned char function, 
				unsigned char offset, uint32_t val) {
HOSE_PREAMBLE;
	if (offset&3) return PCIBIOS_BAD_REGISTER_NUMBER;
	out_be32((unsigned int*) pci.pci_config_addr, 
		 0x80|(bus<<8)|(PCI_DEVFN(slot,function)<<16)|(offset<<24));
	out_le32((volatile unsigned *)pci.pci_config_data, val);
	return PCIBIOS_SUCCESSFUL;
}

const pci_config_access_functions pci_hosed_indirect_functions = {
  	indirect_pci_read_config_byte,
  	indirect_pci_read_config_word,
  	indirect_pci_read_config_dword,
  	indirect_pci_write_config_byte,
  	indirect_pci_write_config_word,
  	indirect_pci_write_config_dword
};


extern unsigned char ucMaxPCIBus; /* importing this is ugly */

/* This is a very ugly hack. I don't want to change the shared
 * code to support multiple hoses so we hide everything under
 * the hood with horrible kludges for now. Sorry.
 */
void
BSP_pci_initialize(void)
{

#if 0	/* These values are already set up for the shared/pci.c code      */
{
extern pci_config_access_functions pci_indirect_functions;
        /* by means of the PCI_CONFIG_ADDR/PCI_CONFIG_DATA macros (bsp.h) */
	BSP_pci_configuration.pci_config_addr = hoses[0].pci_config_addr;
	BSP_pci_configuration.pci_config_data = hoses[0].pci_config_data;
	BSP_pci_configuration.pci_functions   = &pci_indirect_functions;
}
#endif
	/* initialize the first hose */
	/* scan hose 0 and sets the maximum bus number */
	pci_initialize();
	/* remember the boundary */
	BSP_pci_hose1_bus_base = pci_bus_count();
	/* so far, so good -- now comes the cludgy part: */
	/* hack/reset the bus count */
	ucMaxPCIBus = 0;
	/* scan hose 1 */
	BSP_pci_configuration.pci_config_addr = hoses[1].pci_config_addr;
	BSP_pci_configuration.pci_config_data = hoses[1].pci_config_data;
	pci_initialize();
	/* check for overflow of an unsigned char */
	if ( BSP_pci_hose1_bus_base + pci_bus_count() > 255 ) {
		BSP_panic("Too many PCI busses in the system");
	}
	/* readjust total number */
	ucMaxPCIBus+=BSP_pci_hose1_bus_base;

	/* install new access functions that can hide the hoses */
	BSP_pci_configuration.pci_config_addr = (volatile unsigned char *)0xdeadbeef;
	BSP_pci_configuration.pci_config_data = (volatile unsigned char *)0xdeadbeef;
	BSP_pci_configuration.pci_functions   = &pci_hosed_indirect_functions;
}

#define PCI_ERR_BITS		0xf900
#define PCI_STATUS_OK(x)	(!((x)&PCI_ERR_BITS))

/* For now, just clear errors in the PCI status reg.
 *
 * Returns: (for diagnostic purposes)
 *          original settings (i.e. before applying the clearing
 *          sequence) 
 *          (pci_status(hose_1)&0xff00) | ((pci_status(hose_2)>>8)&0xff)
 */

static unsigned long clear_hose_errors(int bus, int quiet)
{
unsigned long	rval;
uint16_t        pcistat;
int				count;
int				hose = PCI_BUS2HOSE(bus);

	/* read error status for info return */
	pci_read_config_word(bus,0,0,PCI_STATUS,&pcistat);
	rval = pcistat;

	count=10;
	do {
		/* clear error reporting registers */

		/* clear PCI status register */
		pci_write_config_word(bus,0,0,PCI_STATUS, PCI_ERR_BITS);

		/* read  new status */
		pci_read_config_word(bus,0,0,PCI_STATUS, &pcistat);

	} while ( ! PCI_STATUS_OK(pcistat) && count-- );

	if ( !PCI_STATUS_OK(rval) && !quiet) {
		printk("Cleared PCI errors at discovery (hose %i): pci_stat was 0x%04x\n", hose, rval);
	}
	if ( !PCI_STATUS_OK(pcistat) ) {
		printk("Unable to clear PCI errors at discovery (hose %i) still 0x%04x after 10 attempts\n",hose, pcistat);
	}
	return rval;
}

unsigned short
(*_BSP_clear_vmebridge_errors)(int) = 0;

unsigned long
_BSP_clear_hostbridge_errors(int enableMCP, int quiet)
{
unsigned long	rval;

	/* MCP is not connected */
	if ( enableMCP )
		return -1;

	rval  = (clear_hose_errors(0, quiet) & PCI_ERR_BITS)>>8;
	rval |= clear_hose_errors(BSP_pci_hose1_bus_base, quiet) & PCI_ERR_BITS;

	/* Tsi148 doesn't propagate VME bus errors to PCI status reg. */
	if ( _BSP_clear_vmebridge_errors )
		rval |= _BSP_clear_vmebridge_errors(quiet)<<16;

	return rval;
}
