#include <libcpu/io.h>
#include <libcpu/spr.h>

#include <bsp.h>
#include <bsp/pci.h>

#include <rtems/bspIo.h>

#define PCI_ERR_BITS        0xf900
#define PCI_STATUS_OK(x)    (!((x)&PCI_ERR_BITS))

/* For now, just clear errors in the PCI status reg.
 *
 * Returns: (for diagnostic purposes)
 *          original settings (i.e. before applying the clearing
 *          sequence) or the error bits or 0 if there were no errors.
 *
 */

unsigned short
(*_BSP_clear_vmebridge_errors)(int) = 0;

unsigned long
_BSP_clear_hostbridge_errors(int enableMCP, int quiet)
{
unsigned long   rval;
unsigned short  pcistat;
int             count;

    if (enableMCP)
	return -1; /* exceptions not supported / MCP not wired */

    /* read error status for info return */
    pci_read_config_word(0,0,0,PCI_STATUS,&pcistat);
    rval = pcistat;

    count=10;
    do {
        /* clear error reporting registers */

        /* clear PCI status register */
        pci_write_config_word(0,0,0,PCI_STATUS, PCI_ERR_BITS);

        /* read  new status */
        pci_read_config_word(0,0,0,PCI_STATUS, &pcistat);

    } while ( ! PCI_STATUS_OK(pcistat) && count-- );

    if ( !PCI_STATUS_OK(rval) && !quiet) {
        printk("Cleared PCI errors: pci_stat was 0x%04x\n", rval);
    }
    if ( !PCI_STATUS_OK(pcistat) ) {
        printk("Unable to clear PCI errors: still 0x%04x after 10 attempts\n", pcistat);
    }

	rval &= PCI_ERR_BITS;

	/* Some VME bridges (Tsi148) don't propagate VME bus errors to PCI status reg. */
	if ( _BSP_clear_vmebridge_errors )
		rval |= _BSP_clear_vmebridge_errors(quiet)<<16;

    return rval;
}
