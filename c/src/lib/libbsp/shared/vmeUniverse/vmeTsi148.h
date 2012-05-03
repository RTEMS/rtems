#ifndef VME_TSI148_DRIVER_H
#define VME_TSI148_DRIVER_H

/* Driver for the Tundra Tsi148 pci-vme bridge */

/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2005-2007,
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
#include <bsp/vme_am_defs.h>


/* NOTE: A64 currently not implemented */

/* These can be ored with the AM */

/* NOTE: unlike the universe, the tsi148 doesn't allow for disabling posted writes ! */

#define VME_MODE_PREFETCH_ENABLE			VME_AM_IS_MEMORY
#define	_LD_VME_MODE_PREFETCHSZ				24
#define VME_MODE_PREFETCH_SIZE(x)			(((x)&3)<<_LD_VME_MODE_PREFETCHSZ)

/* These bits can be or'ed with the address-modifier when calling
 * the 'XlateAddr' routine below to further qualify the
 * search criteria.
 */
#define VME_MODE_MATCH_MASK					(3<<30)
#define VME_MODE_EXACT_MATCH				(2<<30)	/* all bits must match */
#define VME_MODE_AS_MATCH					(1<<30)	/* only A16/24/32 must match */

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile uint32_t BERegister; /* emphasize contents are big endian */

/*
 * Scan the PCI busses for the Nth (N=='instance') Tsi148 VME bridge.
 *
 * RETURNS:
 *    contents of the IRQ_LINE PCI config register on Success,
 *    the base address of the Tsi148 register block is stored in
 *    *pbase.
 *    -1 on error (no Tsi found, error accessing PCI config space).
 *
 * SIDE_EFFECTS: PCI busmaster and response to memory addresses is enabled.
 */
int
vmeTsi148FindPciBase(int instance, BERegister **pbase);

/* Initialize driver for Nth Tsi148 device found.
 * This routine does not change any registers but
 * just scans the PCI bus for Tsi bridges and initializes
 * a driver slot.
 *
 * RETURNS: 0 on success, nonzero on error (or if no Tsi148
 *          device is found).
 */
int
vmeTsi148InitInstance(unsigned instance);

/* Initialize driver with 1st Tsi148 bridge found
 * RETURNS: (see vmeTsi148InitInstance()).
 */
int
vmeTsi148Init(void);

/* Setup the tsi148 chip, i.e. disable most of its
 * mappings, reset interrupts etc.
 */
void
vmeTsi148ResetXX(BERegister *base);

/* Setup the tsi148 connected to the first driver slot */
void
vmeTsi148Reset(void);

/* Pull VME SYSRESET line */
void
vmeTsi148ResetBusXX(BERegister *base);

/* Pull VME SYSRESET line of the 1st controller */
void
vmeTsi148ResetBus(void);

/* NOTE: all non-'XX' versions of driver entry points which
 *       have an associated 'XX' entry point operate on the
 *       device connected to the 1st driver slot.
 */

/* configure a outbound port
 *
 *   port:	    port number 0..7
 *
 *   address_space: vxWorks compliant addressing mode identifier
 *                  (see vme.h). The most important are:
 *                    0x0d - A32, Sup, Data
 *                    0x3d - A24, Sup, Data
 *                    0x2d - A16, Sup, Data
 *                  additionally, the value 0 is accepted; it will
 *                  disable this port.
 *   vme_address:   address on the vme_bus of this port.
 *   local_address: address on the pci_bus of this port.
 *   length:        size of this port.
 *
 *   NOTE: the addresses and length parameters must meet certain alignment
 *         requirements (see Tsi148 documentation).
 *
 *   RETURNS: 0 on success, -1 on failure. Error messages printed to stderr.
 */

int
vmeTsi148OutboundPortCfgXX(
	BERegister		*base,
	unsigned long	port,
	unsigned long	address_space,
	unsigned long	vme_address,
	unsigned long	pci_address,
	unsigned long	length);

int
vmeTsi148OutboundPortCfg(
	unsigned long	port,
	unsigned long	address_space,
	unsigned long	vme_address,
	unsigned long	pci_address,
	unsigned long	length);


/* configure a VME inbound (PCI master) port */
int
vmeTsi148InboundPortCfgXX(
	BERegister		*base,
	unsigned long	port,
	unsigned long	address_space,
	unsigned long	vme_address,
	unsigned long	pci_address,
	unsigned long	length);

int
vmeTsi148InboundPortCfg(
	unsigned long	port,
	unsigned long	address_space,
	unsigned long	vme_address,
	unsigned long	pci_address,
	unsigned long	length);

/* Translate an address through the bridge
 *
 * vmeTsi248XlateAddr(0,0,as,addr,&result)
 * yields a VME a address that reflects
 * a local memory location as seen from the VME bus through the
 * tsi148 VME inbound port.
 *
 * Likewise does vmeTsi148XlateAddr(1,0,as,addr,&result)
 * translate a VME bus addr (backwards, through the VME outbound
 * port) to the PCI side of the bridge.
 *
 * A valid address space modifier must be specified.
 * If VME_MODE_EXACT_MATCH is set, all the mode bits must
 * match the requested mode. If VME_MODE_EXACT_MATCH is not
 * set in the mode word, only the basic mode (address-space,
 * sup/usr and pgm/data) is compared.
 *
 * The 'reverse' parameter may be used to find a reverse
 * mapping, i.e. the pci address in a outbound window can be
 * found if the respective vme address is known etc.
 *
 * RETURNS: translated address in *pbusAdrs / *plocalAdrs
 *
 *          0:  success
 *          -1: address/modifier not found in any bridge port
 *          -2: invalid modifier
 */

int
vmeTsi148XlateAddrXX(
	BERegister *base,	/* TSI 148 base address */
	int outbound, 		/* look in the outbound windows */
	int reverse,		/* reverse mapping; for outbound ports: map local to VME */
	unsigned long as,	/* address space */
	unsigned long aIn,	/* address to look up */
	unsigned long *paOut/* where to put result */
	);

int
vmeTsi148XlateAddr(
	int outbound, 		/* look in the outbound windows */
	int reverse,		/* reverse mapping; for outbound: map local to VME */
	unsigned long as,	/* address space */
	unsigned long aIn,	/* address to look up */
	unsigned long *paOut/* where to put result */
	);


/* avoid pulling stdio.h into this header.
 * Applications that want a declaration of the
 * following routines should
 *  #include <stdio.h>
 *  #define _VME_TSI148_DECLARE_SHOW_ROUTINES
 *  #include <vmeTsi148.h>
 */
#ifdef _VME_TSI148_DECLARE_SHOW_ROUTINES

/* Print the current configuration of all outbound ports to
 * f (stdout if NULL)
 */

void
vmeTsi148OutboundPortsShowXX(BERegister *base, FILE *f);

void
vmeTsi148OutboundPortsShow(FILE *f);

/* Print the current configuration of all inbound ports to
 * f (stdout if NULL)
 */

void
vmeTsi148InboundPortsShowXX(BERegister *base, FILE *f);

void
vmeTsi148InboundPortsShow(FILE *f);

#endif


/* Disable all in- or out-bound ports, respectively */
void
vmeTsi148DisableAllInboundPortsXX(BERegister *base);

void
vmeTsi148DisableAllInboundPorts(void);

void
vmeTsi148DisableAllOutboundPortsXX(BERegister *base);

void
vmeTsi148DisableAllOutboundPorts(void);

#	define TSI_VEAT_VES			(1<<31)
#	define TSI_VEAT_VEOF		(1<<30)
#	define TSI_VEAT_VESCL		(1<<29)
#	define TSI_VEAT_2eOT		(1<<21)
#	define TSI_VEAT_2eST		(1<<20)
#	define TSI_VEAT_BERR		(1<<19)
#	define TSI_VEAT_LWORD		(1<<18)
#	define TSI_VEAT_WRITE		(1<<17)
#	define TSI_VEAT_IACK		(1<<16)
#	define TSI_VEAT_DS1			(1<<15)
#	define TSI_VEAT_DS0			(1<<14)
#	define TSI_VEAT_AM(v)		(((v)>>8)&63)
#	define TSI_VEAT_XAM(v)		((v)&255)

/* Check and clear the error (AKA 'exception') register.
 * Note that the Tsi148 does *not* propagate VME bus errors of any kind to
 * the PCI status register and hence this routine (or registering an ISR
 * to the TSI_VERR_INT_VEC) is the only means for detecting a bus error.
 *
 * RETURNS:
 *   0 if no error has occurred since this routine was last called.
 *     Contents of the 'VEAT' register (bit definitions as above)
 *     otherwise.
 *   If a non-NULL 'paddr'  argument is provided then the lower 32-bit
 *   of the error address is stored in *paddr (only if return value is
 *   non-zero).
 *
 * SIDE EFFECTS: this routine clears the error attribute register, allowing
 *               for future errors to be latched.
 */
unsigned long
vmeTsi148ClearVMEBusErrorsXX(BERegister *base, uint32_t *paddr);

unsigned long
vmeTsi148ClearVMEBusErrors(uint32_t *paddr);

/* Map internal register block to VME.
 *
 * This routine is intended for BSP implementors. The registers must be
 * accessible from VME so that the interrupt handler can flush the
 * bridge FIFO (see below).
 *
 *            vme_base: VME address where the TSI registers (4k) can be mapped.
 *                      This VME address must fall into a range covered by
 *                      any pre-configured outbound window.
 *       address_space: The desired VME address space.
 *                      (all of SUP/USR/PGM/DATA are always accepted).
 *
 * See NOTES [vmeTsi148InstallIrqMgrAlt()] below for further information.
 *
 * RETURNS: 0 on success, nonzero on error. It is not possible (and results
 *          in a non-zero return code) to change the CRG VME address after
 *          initializing the interrupt manager as it uses the CRG.
 */
int
vmeTsi148MapCRGXX(BERegister *base, uint32_t vme_base, uint32_t address_space);

int
vmeTsi148MapCRG(uint32_t vme_base, uint32_t address_space);


/* VME Interrupt Handler functionality */

/* we dont use the current RTEMS/BSP interrupt API for the
 * following reasons:
 *
 *    - RTEMS/BSP API does not pass an argument to the ISR :-( :-(
 *    - no separate vector space for VME vectors. Some vectors would
 *      have to overlap with existing PCI/ISA vectors.
 *    - RTEMS/BSP API allocates a structure for every possible vector
 *    - the irq_on(), irq_off() functions add more bloat than helping.
 *      They are (currently) only used by the framework to disable
 *      interrupts at the device level before removing a handler
 *      and to enable interrupts after installing a handler.
 *      These operations may as well be done by the driver itself.
 *
 * Hence, we maintain our own (VME) handler table and hook our PCI
 * handler into the standard RTEMS/BSP environment. Our handler then
 * dispatches VME interrupts.
 */

typedef void (*VmeTsi148ISR) (void *usrArg, unsigned long vector);

/* install a handler for a VME vector
 * RETURNS 0 on success, nonzero on failure.
 */
int
vmeTsi148InstallISR(unsigned long vector, VmeTsi148ISR handler, void *usrArg);

/* remove a handler for a VME vector. The vector and usrArg parameters
 * must match the respective parameters used when installing the handler.
 * RETURNS 0 on success, nonzero on failure.
 */
int
vmeTsi148RemoveISR(unsigned long vector, VmeTsi148ISR handler, void *usrArg);

/* query for the currently installed ISR and usr parameter at a given vector
 * RETURNS: ISR or 0 (vector too big or no ISR installed)
 */
VmeTsi148ISR
vmeTsi148ISRGet(unsigned long vector, void **parg);

/* utility routines to enable/disable a VME IRQ level
 *
 * To enable/disable the internal interrupt sources (special vectors above)
 * pass a vector argument > 255.
 *
 * RETURNS 0 on success, nonzero on failure
 */
int
vmeTsi148IntEnable(unsigned int level);

int
vmeTsi148IntDisable(unsigned int level);

/* Check if an interrupt level or internal source is enabled:
 *
 * 'level': VME level 1..7 or internal special vector > 255
 *
 * RETURNS: value > 0 if interrupt is currently enabled,
 *          zero      if interrupt is currently disabled,
 *          -1        on error (invalid argument).
 */

int
vmeTsi148IntIsEnabled(unsigned int level);

/* Set IACK width (1,2, or 4 bytes) for a given interrupt level.
 *
 * 'width' arg may be 0,1,2 or 4. If zero, the currently active
 * value is returned but not modified.
 *
 * RETURNS: old width or -1 if invalid argument.
 */

int
vmeTsi148SetIackWidth(int level, int width);

/* Change the routing of IRQ 'level' to 'pin'.
 * If the BSP connects more than one of the four
 * physical interrupt lines from the tsi148 to
 * the board's PIC then you may change the physical
 * line a given 'level' is using. By default,
 * all 7 VME levels use the first wire (pin==0) and
 * all internal sources use the (optional) second
 * wire (pin==1) [The driver doesn't support more than
 * four wires].
 * This feature is useful if you want to make use of
 * different hardware priorities of the PIC. Let's
 * say you want to give IRQ level 7 the highest priority.
 * You could then give 'pin 0' a higher priority (at the
 * PIC) and 'pin 1' a lower priority and issue.
 *
 *   for ( i=1; i<7; i++ ) vmeTsi148IntRoute(i, 1);
 *
 * PARAMETERS:
 *    'level' : VME interrupt level '1..7' or one of
 *              the internal sources. Pass the internal
 *              source's vector number (>=256).
 *    'pin'   : a value of 0 routes the requested IRQ to
 *              the first line registered with the manager,
 *              a value of 1 routes it to the second wire
 *              etc.
 *
 * RETURNS: 0 on success, nonzero on error (invalid arguments)
 *
 * NOTES:	- DONT change the tsi148 'map' registers
 *            directly. The driver caches routing internally.
 *          - support for the extra wires (beyond wire #0) is
 *            board dependent. If the board only provides
 *            a single physical wire from the tsi148 to
 *            the PIC then the feature might not be available.
 */
int
vmeTsi148IntRoute(unsigned int level, unsigned int pin);

/* Raise a VME Interrupt at 'level' and respond with 'vector' to a
 * handler on the VME bus. (The handler could be a different board
 * or the tsi148 itself.
 *
 * Note that you could install a interrupt handler at TSI_VME_SW_IACK_INT_VEC
 * to be notified of an IACK cycle having completed.
 *
 * This routine is mainly FOR TESTING.
 *
 * NOTES:
 *   - the VICR register is modified.
 *   - NO MUTUAL EXCLUSION PROTECTION (reads VICR, modifies then writes back).
 *     If several users need access to VICR it is their responsibility to serialize access.
 *
 * Arguments:
 *  'level':  interrupt level, 1..7
 *  'vector': vector number (0..255) that the tsi148 puts on the bus in response to
 *            an IACK cycle.
 *
 * RETURNS:
 *        0:  Success
 *       -1:  Invalid argument (level not 1..7, vector >= 256)
 *       -2:  Interrupt 'level' already asserted (maybe nobody handles it).
 *            You can manually clear it be setting the IRQC bit in
 *            VICR. Make sure really nobody responds to avoid spurious
 *            interrupts (consult tsi148 docs).
 */

int
vmeTsi148IntRaiseXX(BERegister *base, int level, unsigned vector);

int
vmeTsi148IntRaise(int level, unsigned vector);

/* Loopback test of the VME interrupt subsystem.
 *  - installs ISRs on 'vector' and on TSI_VME_SW_IACK_INT_VEC
 *  - asserts VME interrupt 'level'
 *  - waits for both interrupts: 'ordinary' VME interrupt of 'level' and
 *    IACK completion interrupt ('special' vector TSI_VME_SW_IACK_INT_VEC).
 *
 * NOTES:
 *  - make sure no other handler responds to 'level'.
 *  - make sure no ISR is installed on both vectors yet.
 *  - ISRs installed by this routine are removed after completion.
 *  - no concurrent access protection of all involved resources
 *    (levels, vectors and registers  [see vmeTsi148IntRaise()])
 *    is implemented.
 *  - this routine is intended for TESTING (when implementing new BSPs etc.).
 *  - one RTEMS message queue is temporarily used (created/deleted).
 *
 * RETURNS:
 *                 0: Success.
 *                -1: Invalid arguments.
 *                 1: Test failed (outstanding interrupts).
 * rtems_status_code: Failed RTEMS directive.
 */

int
vmeTsi148IntLoopbackTst(int level, unsigned vector);

/* use these special vectors to connect a handler to the
 * tsi148 specific interrupts (such as "DMA done", SW or
 * error irqs etc.)
 * NOTE: The wrapper clears all status LINT bits (except
 * for regular VME irqs). Also note that it is the user's
 * responsibility to enable the necessary interrupts in
 * LINT_EN
 *
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * DO NOT CHANGE THE ORDER OF THESE VECTORS - THE DRIVER
 * DEPENDS ON IT
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 * Deliberately, these vectors match the universe driver's
 */
/* 256 no VOWN interrupt */
#define TSI_DMA_INT_VEC			257
#define TSI_LERR_INT_VEC		258
#define TSI_VERR_INT_VEC		259
/* 260 is reserved       */
#define TSI_VME_SW_IACK_INT_VEC	261
/* 262 no PCI SW IRQ     */
#define TSI_SYSFAIL_INT_VEC		263
#define TSI_ACFAIL_INT_VEC		264
#define TSI_MBOX0_INT_VEC		265
#define TSI_MBOX1_INT_VEC		266
#define TSI_MBOX2_INT_VEC		267
#define TSI_MBOX3_INT_VEC		268
#define TSI_LM0_INT_VEC			269
#define TSI_LM1_INT_VEC			270
#define TSI_LM2_INT_VEC			271
#define TSI_LM3_INT_VEC			272

/* New vectors; only on TSI148 */
#define TSI_VIES_INT_VEC		273
#define TSI_DMA1_INT_VEC		274

#define TSI_NUM_INT_VECS		275

#ifdef __INSIDE_RTEMS_BSP__

#include <stdarg.h>

/* the tsi148 interrupt handler is capable of routing all sorts of
 * (VME) interrupts to 4 different lines (some of) which may be hooked up
 * in a (board specific) way to a PIC.
 *
 * This driver initially supports at most two lines (i.e., if the user
 * doesn't re-route anything). By default, it routes the
 * 7 VME interrupts to the main line and optionally, it routes the 'special'
 * interrupts generated by the tsi148 itself (DMA done, SW irq etc.)
 * to a second line. If no second line is available, all IRQs are routed
 * to the main line.
 *
 * The routing of interrupts to the two lines can be modified (using
 * the vmeTsi148IntRoute() call - see above - i.e., to make use of
 * different hardware priorities and/or more physically available lines.
 *
 * Because the driver has no way to figure out which lines are actually
 * wired to the PIC, this information has to be provided when installing
 * the manager.
 *
 * Hence the manager sets up routing VME interrupts to 1 or 2 tsi148
 * OUTPUTS. However, it must also be told to which PIC INPUTS they
 * are wired.
 * Optionally, the first PIC input line can be read from PCI config space
 * but the second must be passed to this routine. Note that the info read
 * from PCI config space is wrong for some boards!
 *
 * PARAMETERS:
 *              flags:  VMETSI148_IRQ_MGR_FLAG_SHARED:
 *                      use the BSP_install_rtems_shared_irq_handler() instead
 *                      of BSP_install_rtems_irq_handler(). Use this if the PIC
 *                      line is used by other devices, too.
 *                      CAVEAT: shared interrupts need RTEMS workspace, i.e., the
 *                      VME interrupt manager can only be installed
 *                      *after workspace is initialized* if 'shared' is nonzero
 *                      (i.e., *not* from bspstart()).
 *
 *           tsi_pin_0: to which output pin (of the tsi148) should the 7
 *       				VME irq levels be routed.
 *
 *           pic_pin_0: specifies to which PIC input the 'main' output is
 *                      wired on your board. If passed a value < 0, the driver
 *                      reads this information from PCI config space ("IRQ line").
 *           ...      : up to three additional tsi_pin/pic_pin pairs can be
 *                      specified if your board provides more physical wires.
 *                      In any case must the varargs list be terminated by '-1'.
 *
 * RETURNS: 0 on success, -1 on failure.
 *
 * NOTES: The Tsi148 always does 'posted' writes through a FIFO buffer.
 *        This effectively makes VME write operations asynchronous
 *        which can have undesired side-effects.
 *        In particular, consider the case of an ISR clearing the
 *        interrupt condition by writing to a CSR. The write operation
 *        doesn't really do anything but goes into the FIFO and
 *        the user ISR returns. At this point, the interrupt manager
 *        may find the IRQ still pending, trying another IACK
 *        cycle. Because it is probable that at this time the FIFO
 *        has been flushed and the CSR-write operation been effective,
 *        the IACK then times out.
 *        Note that this phenomenon becomes more obvious as CPUs
 *        become faster.
 *
 *        To avoid this race condition and many VME drivers having
 *        to be re-written, a VME read (having the desired side-effect
 *        of flushing the write FIFO) must be issued between the
 *        user ISR returning and the interrupt manager checking for
 *        more pending interrupts.
 *
 *        Therefore, the BSP needs to map the Tsi148 register
 *        block to VME so that a read over VME can be effectuated.
 *        (In addition to being mapped to VME, the mapped address
 *        range must be accessible through an outbound window.)
 */

#define VMETSI148_IRQ_MGR_FLAG_SHARED	1
int
vmeTsi148InstallIrqMgrAlt(int shared, int tsi_pin0, int pic_pin0, ...);

int
vmeTsi148InstallIrqMgrVa(int shared, int tsi_pin0, int pic_pin0, va_list ap);
#endif

#ifdef __cplusplus
}
#endif

#endif
