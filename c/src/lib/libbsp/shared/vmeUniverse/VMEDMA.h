#ifndef BSP_VME_DMA_H
#define BSP_VME_DMA_H

/* Public interface of DMA routines */

/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2006, 2007
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * This software was produced by
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

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/* NOTE: Access to DMA Channels is *not* protected / thread-safe.
 *       It is the responsability of the user to provide appropriate
 *       locking/serialization.
 */

/* Simple abstraction of DMA controller setup / bus utilization:  */

/* Since VME is the bottleneck, the settings for PCI are always
 * chosen aggressively.
 */


/* Optimize for throughput; accept longer latencies:
 * Choose a large block size (1k) and immediately re-request
 * the bus at block boundaries.
 */
#define BSP_VMEDMA_OPT_THROUGHPUT	1
/* Optimize for latency, accept throughput penalty:
 * Choose a small block size (32b) and immediately re-request
 * the bus at block boundaries.
 */
#define BSP_VMEDMA_OPT_LOWLATENCY	2

/* Optimize for bus sharing with other devices:
 * Choose relatively small block size (128) and back off for 64us
 * at each block boundary.
 */
#define BSP_VMEDMA_OPT_SHAREDBUS	3

/* Choose bridge default/reset configuration:
 * (see manual)
 */
#define BSP_VMEDMA_OPT_DEFAULT		4

/* Provide custom configuration pass pointer to array
 * with as many 32-bit words the particular bridge chip
 * expects.
 */
#define BSP_VMEDMA_OPT_CUSTOM		5

/* VME Transfer modes */

/* Bitwise OR of the VME address-modifier/transfer-type
 * with driver specific (no standard AM code for 2eVME and
 * 2eSST defined) and optional special flags (see below)
 */

/* Additional qualifiers: */

/* Don't increment VME address */
#define BSP_VMEDMA_MODE_NOINC_VME	(1<<20)
/* Don't increment PCI address */
#define BSP_VMEDMA_MODE_NOINC_PCI	(1<<21)

/* Direction */
#define BSP_VMEDMA_MODE_PCI2VME		(1<<31)

typedef void *BSP_VMEDmaListDescriptor;

/* Program the device for the selected mode;
 *
 *  'bus_mode': one of the ...VMEDMA_OPT... choices
 *              listed above.
 * 'xfer_mode': VME address-modifier optionally ORed with
 *              ...VMEDMA_MODE... bits listed above.
 *    'custom': (only used if bus_mode is VMEDMA_OPT_CUSTOM)
 *              pointer to a list of setup parameters (chip-driver
 *              specific).
 *
 * RETURNS: 0 on success, nonzero on error (mode or channel
 *          unsupported).
 *
 * NOTES:   The setup is preserved across multiple DMA transfers.
 *          It is the responsibility of the driver to reprogram
 *          the setup if the hardware does not preserve it.
 *          However - in linked list mode, some fields may be
 *          read from the list descriptors.
 *
 *          Usually this routine must be used even in linked-list
 *          mode to program the 'bus_mode'.
 *
 *          Direction of transfer is specified by a bit in the
 *          'xfer_mode' (BSP_VMEDMA_MODE_PCI2VME).
 */
int
BSP_VMEDmaSetup(int channel, uint32_t bus_mode, uint32_t xfer_mode, void *custom_setup);

/* Start direct (not linked-list) transfer.
 *
 * RETURNS: 0 on success, nonzero on failure
 */
int
BSP_VMEDmaStart(int channel, uint32_t pci_addr, uint32_t vme_addr, uint32_t n_bytes);

/* Transfer status/result */
#define BSP_VMEDMA_STATUS_OK		0
/* Unsupported channel */
#define BSP_VMEDMA_STATUS_UNSUP		(-1)
/* Bus error on VME */
#define BSP_VMEDMA_STATUS_BERR_VME	1
/* Bus error on PCI */
#define BSP_VMEDMA_STATUS_BERR_PCI	2
/* Channel busy        */
#define BSP_VMEDMA_STATUS_BUSY		3
/* Setup/programming error */
#define BSP_VMEDMA_STATUS_PERR		4
/* Other/unspecified error */
#define BSP_VMEDMA_STATUS_OERR		5

/* Retrieve status of last transfer.
 *
 * RETURNS: 0 if the transfer was successful,
 *          nonzero on error (e.g., one of the
 *          values defined above).
 *
 *    NOTE: Driver is allowed to pass other,
 *          device specific codes
 */

uint32_t
BSP_VMEDmaStatus(int channel);

/*
 * Hook a callback (executed from ISR context) to DMA interrupt and
 * enable it.
 * If called with NULL callback then an existing callback is removed
 * and the interrupt disabled.
 *
 * RETURNS: 0 on success, nonzero on failure (IRQ in use, unsupported
 *          channel).
 */
typedef void (*BSP_VMEDmaIRQCallback)(void *usr_arg);

int
BSP_VMEDmaInstallISR(int channel, BSP_VMEDmaIRQCallback cb, void *usr_arg);

/*
 * DMA List operations.
 *
 * Note that the list is totally unprotected, i.e., the user is
 * responsible for maintaining coherency against concurrent
 * access by multiple threads or hardware.
 * We assume the user builds/updates a list, hands it over to
 * the hardware (list start command) and leaves it alone until
 * the DMA controller is done with it.
 */

/* Modify a list entry. If the list element pointer is NULL
 * then a new list element is allocated.
 * Only the fields with its corresponding bit set in the mask
 * argument are touched.
 *
 * RETURNS: 'd' or newly allocated descriptor or NULL (no memory,
 *          or invalid setup).
 */
#define BSP_VMEDMA_MSK_ATTR		(1<<0)
#define BSP_VMEDMA_MSK_PCIA		(1<<1)
#define BSP_VMEDMA_MSK_VMEA		(1<<2)
#define BSP_VMEDMA_MSK_BCNT		(1<<3)
#define BSP_VMEDMA_MSK_ALL		(0xf)
BSP_VMEDmaListDescriptor
BSP_VMEDmaListDescriptorSetup(
		BSP_VMEDmaListDescriptor d,
		uint32_t                 attr_mask,
		uint32_t				 xfer_mode,
		uint32_t                 pci_addr,
		uint32_t                 vme_addr,
		uint32_t                 n_bytes);

/* De-allocate a list descriptor previously obtained by
 * BSP_VMEDmaListDescriptorSetup(0,...);
 *
 * RETURNS: 0 on success, nonzero on failure (d currently on a list)
 */
int
BSP_VMEDmaListDescriptorDestroy(BSP_VMEDmaListDescriptor d);

/* Traverse a list of descriptors and destroy all elements */
int
BSP_VMEDmaListDestroy(BSP_VMEDmaListDescriptor anchor);

/* Enqueue a list descriptor 'd' after 'tail'
 *
 * If 'tail' is NULL then 'd' is removed from
 * the list it is currently on.
 *
 * RETURNS: 0 on success, nonzero if 'd' is already
 *          on a list (enqueue) or if it is not currently
 *          on a list (dequeue).
 *
 * NOTE: it is obviously the user's responsibility to update
 *       list queue/tail pointers when changing the
 *       structure of the list.
 */
int
BSP_VMEDmaListDescriptorEnq(
		BSP_VMEDmaListDescriptor tail,
		BSP_VMEDmaListDescriptor d);

/* Obtain next and previous descriptors */
BSP_VMEDmaListDescriptor
BSP_VMEDmaListDescriptorNext(BSP_VMEDmaListDescriptor d);

BSP_VMEDmaListDescriptor
BSP_VMEDmaListDescriptorPrev(BSP_VMEDmaListDescriptor d);

/* Set and get a 'usrData' pointer in the descriptor */
void
BSP_VMEDmaListDescriptorSetUsr(BSP_VMEDmaListDescriptor d, void *usrData);

void *
BSP_VMEDmaListDescriptorGetUsr(BSP_VMEDmaListDescriptor d);

/* Refresh an entire list. Some DMA controllers modify certain
 * fields (e.g., byte count) and this command restores the original
 * setup.
 */

int
BSP_VMEDmaListRefresh(BSP_VMEDmaListDescriptor anchor);

/* Start linked-list operation.
 *
 * RETURNS: 0 on success, nonzero on failure
 */
int
BSP_VMEDmaListStart(int channel, BSP_VMEDmaListDescriptor list);

#ifdef __cplusplus
}
#endif

#endif
