/* Interrupt driver + dispatcher for the discovery host controller */

/* Author: T. Straumann, 2005-2007
 *
 * Acknowledgements:
 * Valuable information was obtained from the following drivers
 *   netbsd: (C) Allegro Networks Inc; Wasabi Systems Inc.
 *   linux:  (C) MontaVista, Software, Inc; Chris Zankel, Mark A. Greer.
 *   rtems:  (C) Brookhaven National Laboratory; K. Feng
 * but this implementation is original work by the author.
 */

/* 
 * Authorship
 * ----------
 * This software ('beatnik' RTEMS BSP for MVME6100 and MVME5500) was
 *     created by Till Straumann <strauman@slac.stanford.edu>, 2005-2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 * 
 * Acknowledgement of sponsorship
 * ------------------------------
 * The 'beatnik' BSP was produced by
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
#include <rtems.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/gtreg.h>
#include <bsp/gtintrreg.h>
#include <rtems/bspIo.h>
#include <bsp/vectors.h>
#include <libcpu/byteorder.h>
#include <libcpu/spr.h>

/* dont change the order (main_lo, main_hi, gpp) which
 * matches the interrupt numbers!
 */
#define MAIN_LO_IDX	0
#define MAIN_HI_IDX	1
#define GPP_IDX		2
#define NUM_INTR_REGS 3


#define SYNC() __asm__ volatile("sync")

/* How many times should the ISR dispatcher check for
 * pending interrupts until it decides that something's
 * fishy (i.e., a user ISR fails to clear the interrupt
 * source)
 */
#define MAX_SPIN_LOOPS 100

/* If FASTER is defined, a few obscure I/O statements found in the linux
 * driver are removed
 */
#define	FASTER

/* Array helper */
#define NumberOf(arr) (sizeof(arr)/sizeof((arr)[0]))


/* MVME6100 specific; re-define watchdog NMI pin to be a normal output
 * so we have a way to raise an interrupt in software (GPP[26] is wired to
 * GPP[6] on the MVME6100).
 */
#define	MVME6100_IRQ_DEBUG 4

#define GPP_WIRED_OUT_BIT_6100 26	/* CAVEAT: this is bit 26 on the 6100 */
#define GPP_WIRED_OUT_BIT_5500 24	/* CAVEAT: this is bit 24 on the 5500 */
#define GPP_WIRED_IN_BIT   6

/* Ored mask of debugging features to enable */
#define IRQ_DEBUG_BASIC			1
/* This is _very_ lowlevel */
#define IRQ_DEBUG_DISPATCHER	2
/* Record maximal dispatching latency */
#define IRQ_DEBUG_MAXLAT		8	/* PPC only */

#define IRQ_DEBUG (0 /*|(IRQ_DEBUG_BASIC)*/|(MVME6100_IRQ_DEBUG)|(IRQ_DEBUG_MAXLAT))

/**********
 * Typedefs
 **********/

/* Set of the three relevant cause registers */
typedef volatile unsigned IrqMask[NUM_INTR_REGS];

#define REGP(x) ((volatile uint32_t *)(x))

/* Information we keep about the PIC         */
typedef struct _Mv64x60PicRec {
							/* base address as seen from CPU  */
	uintptr_t	 			reg_base;

							/* addresses of 'cause' registers */
	volatile uint32_t		*causes[NUM_INTR_REGS];

							/* addresses of 'mask'  registers */
	volatile uint32_t		*masks[NUM_INTR_REGS];

							/* masks for all priorities. If an
							 * interrupt source has priority X,
							 * its corresponding bit is set
							 * (enabled) in mcache[i] for all
							 * i < X and cleared for i >= X
							 */
	volatile IrqMask		mcache[BSP_IRQ_MAX_PRIO+1];

							/* Priority we're executing at.
							 * Thread-level is priority 0,
							 * ISRs range from 1..MAX_PRIO
							 */
	volatile rtems_irq_prio	current_priority;
} Mv64x60PicRec, *Mv64x60Pic;

/**********
 * Globals
 **********/


/* Copy of the configuration */
static rtems_irq_global_settings	theConfig;
/* PIC description           */
static Mv64x60PicRec				thePic;

#if	(IRQ_DEBUG) & MVME6100_IRQ_DEBUG
static unsigned long				gpp_out_bit = 0;
#endif

#if	(IRQ_DEBUG) & IRQ_DEBUG_MAXLAT
unsigned long						discovery_pic_max_dispatching_latency = 0;
#ifdef __PPC__
static inline unsigned long mftb(void)
{
unsigned long rval;
	asm volatile("mftb %0":"=r"(rval));
	return rval;
}
#else
#define mftb()	0
#endif
#endif

/**********
 * Functions
 **********/

/* Debugging helper routines */
static void pregs(volatile uint32_t **p)
{
int i;
	for (i=NUM_INTR_REGS-1; i>=0; i--) {
		printk(" 0x%08x", ld_le32(p[i]));
		printk( i ? " --":"\n");
	}
}

static void pmsks(volatile IrqMask p)
{
int i;
	for (i=NUM_INTR_REGS-1; i>=0; i--) {
		printk(" 0x%08x", p[i]);
		printk( i ? " --":"\n");
	}
}

void discovery_dump_picregs(void)
{
		printk("       ..GPP_IRQ. -- ..MAIN_HI. -- ..MAIN_LO.\n");
		printk("Cause:"); pregs(thePic.causes);
		printk("Mask: "); pregs(thePic.masks);
}

/* Small inline helpers      */

/* return 0 if this PIC is not 'responsible' for a given irq number
 * we also 'ignore' the GPP summary bits - these must always remain
 * enabled.
 */
static inline int
validIrqNo(rtems_irq_number irq)
{
	return
		   irq >= BSP_PCI_IRQ_LOWEST_OFFSET
		&& irq <= BSP_PCI_IRQ_MAX_OFFSET
		&& ! (IMH_GPP_SUM & (1<<(irq-32)));
}

/* return 0 if a given priority is outside the valid range          */
static inline int
validPri(rtems_irq_prio pri)
{
	/* silence compiler warning about limited range of type;
	 * hope it never changes...
	 */
	return /* pri>=0 && */ pri <=BSP_IRQ_MAX_PRIO;
}

/* Return position of the most significant bit that is set in 'x'  */
static inline int
__ilog2(unsigned x)
{
	asm volatile("cntlzw %0, %0":"=&r"(x):"0"(x));
	return 31-x;
}

/* Convert irq number to cause register index
 * (array of handles in the PicRec).
 * ASSUMES: 'irq' within valid range.
 */
static inline unsigned
irqDiv32(unsigned irq)
{
	return (irq-BSP_PCI_IRQ_LOWEST_OFFSET)>>5;
}

/* Convert irq number to cause/mask bit number.
 * ASSUMES: 'irq' within valid range.
 */

static inline unsigned
irqMod32(unsigned irq)
{
	return (irq-BSP_PCI_IRQ_LOWEST_OFFSET)&31;
}

/* NON-ATOMICALLY set/clear bits in a MV64x60 register
 *
 * register contents at offset 'off' are ANDed with
 * complement of the 'clr' mask and ORed with 'set' mask:
 *
 *   *off = (*off & ~clr) | set
 * 
 * ASSUMES: executed from IRQ-disabled section
 */
static inline void
gt_bitmod(unsigned off, unsigned set, unsigned clr)
{
	st_le32(REGP(thePic.reg_base + off),
			(ld_le32(REGP(thePic.reg_base+off)) & ~clr) | set);
}

static inline unsigned
gt_read(unsigned off)
{
	return ld_le32(REGP(thePic.reg_base + off));
}

static inline void
gt_write(unsigned off, unsigned val)
{
	st_le32(REGP(thePic.reg_base + off), val);
}

/* Enable interrupt number 'irq' at the PIC.
 *
 * Checks for valid arguments but has no way of
 * communicating violation; prints to console
 * if illegal arguments are given.
 *
 * This routine may be called from ISR level.
 *
 * Algorithm: set corresponding bit in masks
 *            for all priorities lower than the
 *            target irq's priority and push
 *            mask for the currently executing
 *            priority out to the PIC.
 */

void
BSP_enable_irq_at_pic(rtems_irq_number irq)
{
unsigned		   i,j;
unsigned long	   flags;
volatile uint32_t *p;
uint32_t 		   v,m;

	if ( !validIrqNo(irq) ) {
/* API change - must silently ignore...
		printk("BSP_enable_irq_at_pic: Invalid argument (irq #%i)\n",irq);
 */
		return;
	}

#if (IRQ_DEBUG) & IRQ_DEBUG_BASIC
	printk("IRQ: Enable #%i;",irq);
#endif

	if ( (i=irqDiv32(irq)) > NUM_INTR_REGS ) {
		/* This is probably a more serious error; don't ignore silently */
		printk("BSP_enable_irq_at_pic: illegal argument\n");
		return;
	}
	/* compute register pointer and bit mask */
	p = thePic.masks[i];
	m = 1<<irqMod32(irq);
		
	rtems_interrupt_disable(flags);
	{
		/* access table from protected section to be thread-safe */
		rtems_irq_prio	pri = theConfig.irqPrioTbl[irq];
		for ( j=0; j<pri; j++ ) {
			thePic.mcache[j][i] |= m;
		}
		st_le32(p, (v=thePic.mcache[thePic.current_priority][i]));
		/* linux driver reads back GPP mask; maybe it's wise to do the same */
		(void)ld_le32(thePic.masks[GPP_IDX]);
	}
	SYNC();
	rtems_interrupt_enable(flags);

#if (IRQ_DEBUG) & IRQ_DEBUG_BASIC
	printk(" Mask[%i]: 0x%08x -> 0x%08x\n",i,v,ld_le32(p));

#endif
}

/* Disable interrupt number 'irq' at the PIC.
 *
 * Checks for valid arguments but has no way of
 * communicating violation; prints to console
 * if illegal arguments are given.
 *
 * This routine may be called from ISR level.
 *
 * Algorithm: clear corresponding bit in masks
 *            for all priorities and push the
 *            mask for the currently executing
 *            priority out to the PIC.
 */

int
BSP_disable_irq_at_pic(rtems_irq_number irq)
{
unsigned		   i,j;
unsigned long	   flags;
volatile uint32_t *p;
uint32_t           v,m;
int                rval;

	if ( !validIrqNo(irq) ) {
/* API change - must silently ignore...
		printk("BSP_disable_irq_at_pic: Invalid argument (irq #%i)\n",irq);
 */
		return -1;
	}

#if (IRQ_DEBUG) & IRQ_DEBUG_BASIC
	printk("IRQ: Disable #%i;",irq);
#endif

	if ( (i=irqDiv32(irq)) > NUM_INTR_REGS ) {
		/* This is probably a more serious error; don't ignore silently */
		printk("BSP_enable_irq_at_pic: illegal argument\n");
		return -1;
	}

	/* compute register pointer and bit mask */
	p = thePic.masks[i];
	m = (1<<irqMod32(irq));

	rtems_interrupt_disable(flags);
	{
		rval = thePic.mcache[thePic.current_priority][i] & m;
		for (j=0; j<=BSP_IRQ_MAX_PRIO; j++)
			thePic.mcache[j][i] &= ~m;
		st_le32(p, (v=thePic.mcache[thePic.current_priority][i]));
		/* linux driver reads back GPP mask; maybe it's wise to do the same */
		(void)ld_le32(thePic.masks[GPP_IDX]);
	}
	SYNC();
	rtems_interrupt_enable(flags);

#if (IRQ_DEBUG) & IRQ_DEBUG_BASIC
	printk(" Mask[%i]: 0x%08x -> 0x%08x\n",i,v,ld_le32(p));
#endif

	return rval ? 1 : 0;
}

int
BSP_irq_is_enabled_at_pic(rtems_irq_number irq)
{
unsigned i;
	if ( !validIrqNo(irq) ) {
		printk("BSP_irq_is_enabled_at_pic: Invalid argument (irq #%i)\n",irq);
		return -1;
	}

	if ( (i=irqDiv32(irq)) > NUM_INTR_REGS ) {
		printk("BSP_enable_irq_at_pic: illegal argument\n");
		return -1;
	}
	return ld_le32(thePic.masks[i]) & (1<<irqMod32(irq)) ? 1 : 0;
}


/* Change priority of interrupt number 'irq' to 'pri'
 *
 * RETURNS: 0 on success, nonzero on failure (illegal args)
 *
 * NOTE: This routine must not be called from ISR level.
 *
 * Algorithm: Set bit corresponding to 'irq' in the masks for
 *            all priorities < pri and clear in all masks
 *            for priorities >=pri
 */
int
BSP_irq_set_priority(rtems_irq_number irq, rtems_irq_prio pri)
{
unsigned long	   flags;
volatile uint32_t *p;
uint32_t 		   v,m;
unsigned           i,j;

	if ( thePic.current_priority > 0 ) {
		printk("BSP_irq_set_priority: must not be called from ISR level\n");
		return -1;
	}

	if ( !validPri(pri) ) {
		printk("BSP_irq_set_priority: invalid argument (pri #%i)\n",pri);		
		return -1;
	}

	if ( BSP_DECREMENTER != irq ) {
		if ( !validIrqNo(irq) ) {
			printk("BSP_irq_set_priority: invalid argument (irq #%i)\n",irq);		
			return -1;
		}

		if ( (i=irqDiv32(irq)) > NUM_INTR_REGS ) {
			printk("BSP_irq_set_priority: illegal argument (irq #%i not PCI?)\n", irq);
			return -1;
		}
	}

#if (IRQ_DEBUG) & IRQ_DEBUG_BASIC
	printk("IRQ: Set Priority #%i -> %i;",irq,pri);
#endif

	if ( BSP_DECREMENTER == irq ) {
		theConfig.irqPrioTbl[irq] = pri;
		return 0;
	}

	/* compute register pointer and bit mask */
	p = thePic.masks[i];
	m = 1<<irqMod32(irq);
	
	rtems_interrupt_disable(flags);
	{
		for (j=0; j<=BSP_IRQ_MAX_PRIO; j++) {
			if ( j<pri )
				thePic.mcache[j][i] |= m;
			else
				thePic.mcache[j][i] &= ~m;
		}
		theConfig.irqPrioTbl[irq] = pri;
		st_le32(p, (v=thePic.mcache[thePic.current_priority][i]));
		/* linux driver reads back GPP mask; maybe it's wise to do the same */
		(void)ld_le32(thePic.masks[GPP_IDX]);
	}
	SYNC();
	rtems_interrupt_enable(flags);

#if (IRQ_DEBUG) & IRQ_DEBUG_BASIC
	printk(" Mask[%i]: 0x%08x -> 0x%08x\n",i,v,ld_le32(p));
#endif

	return 0;
}

/* Initialize the PIC; routine needed by BSP framework
 *
 * RETURNS: NONZERO on SUCCESS, 0 on error!
 */
int
BSP_setup_the_pic(rtems_irq_global_settings* config)
{
int i;
   /*
    * Store copy of configuration
    */
	theConfig				= *config;

	/* check config */
	if ( theConfig.irqNb <= BSP_PCI_IRQ_MAX_OFFSET ) {
		printk("BSP_setup_the_pic: FATAL ERROR: configured IRQ table too small???\n");
		return 0;
	} 

	for ( i=0; i<theConfig.irqNb; i++ ) {
		if ( !validPri(theConfig.irqPrioTbl[i]) ) {
			printk("BSP_setup_the_pic: invalid priority (%i) for irg #%i; setting to 1\n", theConfig.irqPrioTbl[i], i);
			theConfig.irqPrioTbl[i]=1;
		}
	}

	/* TODO: Detect; Switch wired-out bit;  */
	thePic.reg_base = BSP_MV64x60_BASE;

	thePic.current_priority = 0;

#if	(IRQ_DEBUG) & MVME6100_IRQ_DEBUG
#endif

	switch ( BSP_getDiscoveryVersion(/* assert */ 1) ) {
		case MV_64360:
			thePic.causes[MAIN_LO_IDX] = REGP(thePic.reg_base + ICR_360_MIC_LO);
			thePic.causes[MAIN_HI_IDX] = REGP(thePic.reg_base + ICR_360_MIC_HI);
			thePic.masks[MAIN_LO_IDX]  = REGP(thePic.reg_base + ICR_360_C0IM_LO);
			thePic.masks[MAIN_HI_IDX]  = REGP(thePic.reg_base + ICR_360_C0IM_HI);
		break;

		case GT_64260_A:
		case GT_64260_B:
			thePic.causes[MAIN_LO_IDX] = REGP(thePic.reg_base + ICR_260_MIC_LO);
			thePic.causes[MAIN_HI_IDX] = REGP(thePic.reg_base + ICR_260_MIC_HI);
			thePic.masks[MAIN_LO_IDX]  = REGP(thePic.reg_base + ICR_260_CIM_LO);
			thePic.masks[MAIN_HI_IDX]  = REGP(thePic.reg_base + ICR_260_CIM_HI);
		break;

		default:
			BSP_panic("Unable to initialize interrupt controller; unknown chip");
		break;
	}

	thePic.causes[GPP_IDX]     = REGP(thePic.reg_base + GT_GPP_Interrupt_Cause);
	thePic.masks[GPP_IDX]      = REGP(thePic.reg_base + GT_GPP_Interrupt_Mask);

	/* Initialize mask cache */
	for ( i=0; i<=BSP_IRQ_MAX_PRIO; i++ ) {
		thePic.mcache[i][MAIN_LO_IDX] = 0;
		/* Always enable the summary bits. Otherwise, GPP interrupts dont
		 * make it 'through' to the GPP cause
		 */
		thePic.mcache[i][MAIN_HI_IDX] = IMH_GPP_SUM;
		thePic.mcache[i][GPP_IDX]     = 0;
	}

	/* mask and clear everything */
	for ( i=0; i<NUM_INTR_REGS; i++ ) {
		st_le32(thePic.causes[i], 0);
		st_le32(thePic.masks[i], 0);
	}

	/* make sure GPP Irqs are level sensitive */
	gt_bitmod(
		GT_CommUnitArb_Ctrl,							/* reg */
		GT_CommUnitArb_Ctrl_GPP_Ints_Level_Sensitive,	/* set */
		0);												/* clr */

	/* enable summaries */
	st_le32(thePic.masks[MAIN_LO_IDX], thePic.mcache[thePic.current_priority][MAIN_LO_IDX]);
	st_le32(thePic.masks[MAIN_HI_IDX], thePic.mcache[thePic.current_priority][MAIN_HI_IDX]);
	st_le32(thePic.masks[GPP_IDX    ], thePic.mcache[thePic.current_priority][GPP_IDX    ]);

	/* believe the interrupts are all level sensitive (which is good); we leave all the
	 * inputs configured they way the are by MotLoad...
	 */
	
	/* Finally, enable all interrupts for which the configuration table has already
	 * a handler installed.
	 */
	for ( i=BSP_PCI_IRQ_LOWEST_OFFSET; i<=BSP_PCI_IRQ_MAX_OFFSET; i++ ) {
		if ( theConfig.irqHdlTbl[i].hdl != theConfig.defaultEntry.hdl ) {
			BSP_enable_irq_at_pic(i);
		}
	}

	return 1;
}

int discovery_pic_max_loops = 0;


/* Change the priority level we're executing at and mask all interrupts of
 * the same and lower priorities
 *
 * RETURNS old priority;
 */

static inline  rtems_irq_prio
change_executing_prio_level(rtems_irq_prio pri)
{
register rtems_irq_prio rval = thePic.current_priority;
	thePic.current_priority = pri;
	st_le32(thePic.masks[MAIN_LO_IDX], thePic.mcache[pri][MAIN_LO_IDX]);
	st_le32(thePic.masks[MAIN_HI_IDX], thePic.mcache[pri][MAIN_HI_IDX]);
	st_le32(thePic.masks[GPP_IDX    ], thePic.mcache[pri][GPP_IDX    ]);
	/* this DOES seem to be necessary */
	(void)ld_le32(thePic.masks[GPP_IDX]);
	return rval;
}

/* Scan the three cause register and find the pending interrupt with
 * the highest priority.
 *
 * Two facts make this quite efficient
 *   a) the PPC has an opcode for finding the number of leading zero-bits
 *      in a register (__ilog2()).
 *   b) as we proceed we mask all sources of equal or lower priorites; they won't be
 *      seen while scanning:
 *
 *   maxpri = 0;
 *   bits   = in_le32(cause);
 *   while ( bits &= mask[maxpri] ) {
 *      irq_no = __ilog2(bits);
 *     	maxpri = priority[irq_no]; 
 *   }
 *   
 *   a)  __ilog() is 1-2 machine instructions
 *   b) while loop is only executed as many times as interrupts of different
 *      priorities are pending at the same time (and only if lower-priority
 *      ones are found first; otherwise, the iteration terminates quicker).
 *
 *   ==> highest priority source is found quickly. It takes at most
 *
 *		BSP_IRQ_MAX_PRIO * ( ~3 reg-only instructions + 2 memory access )
 *      + 2 reg-only instructions + 1 I/O + 1 memory access.
 *
 *       
 */

static unsigned mlc, mhc, gpc;

static int decrementerPending = 0;
#if (IRQ_DEBUG) & IRQ_DEBUG_DISPATCHER
int        decrementerIrqs    = 0;
#endif

static inline unsigned 
find_highest_priority_pending_irq(rtems_irq_prio *ppri)
{
register int			rval = -1;
register rtems_irq_prio *pt  = theConfig.irqPrioTbl + BSP_PCI_IRQ_LOWEST_OFFSET;
register rtems_irq_prio pmax = *ppri;
register unsigned		cse,ocse;

#if (IRQ_DEBUG) & IRQ_DEBUG_DISPATCHER
	discovery_dump_picregs();
#endif

	if ( decrementerPending ) {
/* Don't flood 
#if (IRQ_DEBUG) & IRQ_DEBUG_DISPATCHER
		printk("Decrementer IRQ pending\n");
#endif
*/
		if ( theConfig.irqPrioTbl[BSP_DECREMENTER] > pmax ) {
			pmax = theConfig.irqPrioTbl[BSP_DECREMENTER];
			rval = BSP_DECREMENTER;
		}
	}

	mlc = cse = ld_le32(thePic.causes[MAIN_LO_IDX]);
#if (IRQ_DEBUG) & IRQ_DEBUG_DISPATCHER
	printk("MAIN_LO; cse: 0x%08x, msk 0x%08x\n", cse ,thePic.mcache[pmax][MAIN_LO_IDX]);
#endif
	while ( cse &= thePic.mcache[pmax][MAIN_LO_IDX] ) {
		rval = __ilog2(cse);
		pmax = pt[rval];
#if (IRQ_DEBUG) & IRQ_DEBUG_DISPATCHER
		printk("Max pri IRQ now %i\n",rval);
#endif
	}
	mhc = cse = ocse = ld_le32(thePic.causes[MAIN_HI_IDX]);
#if (IRQ_DEBUG) & IRQ_DEBUG_DISPATCHER
	printk("MAIN_HI; cse: 0x%08x, msk 0x%08x\n", cse, thePic.mcache[pmax][MAIN_HI_IDX]);
#endif
	/* don't look at the GPP summary; only check for 'real' MAIN_HI sources */
	cse &= ~IMH_GPP_SUM;
	while ( cse &= thePic.mcache[pmax][MAIN_HI_IDX] ) {
		rval = __ilog2(cse) + 32;
		pmax = pt[rval];
#if (IRQ_DEBUG) & IRQ_DEBUG_DISPATCHER
		printk("Max pri IRQ now %i\n",rval);
#endif
	}
	gpc = cse = ld_le32(thePic.causes[GPP_IDX    ]);
	/* if there were GPP ints, scan the GPP cause now */
	if ( ocse & IMH_GPP_SUM ) {
#if (IRQ_DEBUG) & IRQ_DEBUG_DISPATCHER
		printk("GPP; cse: 0x%08x, msk 0x%08x\n", cse, thePic.mcache[pmax][GPP_IDX    ]);
#endif
		cse &= thePic.mcache[pmax][GPP_IDX    ];
		ocse = cse;
		while ( cse ) {
			rval = __ilog2(cse) + 64;
			pmax = pt[rval];
			cse &= thePic.mcache[pmax][GPP_IDX    ];
#if (IRQ_DEBUG) & IRQ_DEBUG_DISPATCHER
			printk("Max pri IRQ now %i\n",rval);
#endif
		}
#ifndef FASTER
		/* this doesn't seem to be necessary -- however, the linux people do it... */
		out_le32(thePic.causes[GPP_IDX], ~ocse);
#endif
	}
#ifndef FASTER
	/* this doesn't seem to be necessary -- however, the linux people do it... */
	(void)in_le32(thePic.causes[GPP_IDX]);
#endif

	*ppri = pmax;

	if ( BSP_DECREMENTER == rval ) {
#if (IRQ_DEBUG) & IRQ_DEBUG_DISPATCHER
		decrementerIrqs++;
#endif
		decrementerPending = 0;
	}

	return rval;
}

#if 0 /* TODO: should this be cleaned up ? */
#define _IRQ_DEBUG IRQ_DEBUG_DISPATCHER
static inline unsigned 
ffind_highest_priority_pending_irq(rtems_irq_prio *ppri)
{
register int			rval = -1;
register rtems_irq_prio *pt  = theConfig.irqPrioTbl + BSP_PCI_IRQ_LOWEST_OFFSET;
register rtems_irq_prio pmax = *ppri;
register unsigned		cse,ocse;

#if (_IRQ_DEBUG) & IRQ_DEBUG_DISPATCHER
	discovery_dump_picregs();
#endif

	cse = in_le32(thePic.causes[MAIN_LO_IDX]);
#if (_IRQ_DEBUG) & IRQ_DEBUG_DISPATCHER
	printk("MAIN_LO; cse: 0x%08x, msk 0x%08x\n", cse ,thePic.mcache[pmax][MAIN_LO_IDX]);
#endif
	while ( cse &= thePic.mcache[pmax][MAIN_LO_IDX] ) {
		rval = __ilog2(cse);
		pmax = pt[rval];
#if (_IRQ_DEBUG) & IRQ_DEBUG_DISPATCHER
		printk("Max pri IRQ now %i\n",rval);
#endif
	}
	cse = ocse = in_le32(thePic.causes[MAIN_HI_IDX]);
#if (_IRQ_DEBUG) & IRQ_DEBUG_DISPATCHER
	printk("MAIN_HI; cse: 0x%08x, msk 0x%08x\n", cse, thePic.mcache[pmax][MAIN_HI_IDX]);
#endif
	/* don't look at the GPP summary; only check for 'real' MAIN_HI sources */
	cse &= ~IMH_GPP_SUM;
	while ( cse &= thePic.mcache[pmax][MAIN_HI_IDX] ) {
		rval = __ilog2(cse) + 32;
		pmax = pt[rval];
#if (_IRQ_DEBUG) & IRQ_DEBUG_DISPATCHER
		printk("Max pri IRQ now %i\n",rval);
#endif
	}
	/* if there were GPP ints, scan the GPP cause now */
	if ( ocse & IMH_GPP_SUM ) {
		cse = in_le32(thePic.causes[GPP_IDX    ]);
#if (_IRQ_DEBUG) & IRQ_DEBUG_DISPATCHER
		printk("GPP; cse: 0x%08x, msk 0x%08x\n", cse, thePic.mcache[pmax][GPP_IDX    ]);
#endif
		cse &= thePic.mcache[pmax][GPP_IDX    ];
		ocse = cse;
		while ( cse ) {
			rval = __ilog2(cse) + 64;
			pmax = pt[rval];
			cse &= thePic.mcache[pmax][GPP_IDX    ];
#if (_IRQ_DEBUG) & IRQ_DEBUG_DISPATCHER
			printk("Max pri IRQ now %i\n",rval);
#endif
		}
		/* this doesn't seem to be necessary -- however, the linux people do it... */
		out_le32(thePic.causes[GPP_IDX], ~ocse);
	}
	/* this doesn't seem to be necessary -- however, the linux people do it... */
	(void)in_le32(thePic.causes[GPP_IDX]);

	*ppri = pmax;
	return rval;
}
#endif


/* Here's our dispatcher; the BSP framework uses the same one for EE and decrementer
 * exceptions...
 */
int C_dispatch_irq_handler (BSP_Exception_frame *frame, unsigned int excNum)
{
register int             irq;
int				         loop, last_irq;
rtems_irq_prio           pri;
#if	(IRQ_DEBUG) & IRQ_DEBUG_MAXLAT
unsigned long	         diff;
#endif

#if	(IRQ_DEBUG) & IRQ_DEBUG_MAXLAT
	diff = mftb();
#endif

	if (excNum == ASM_DEC_VECTOR) {
		decrementerPending = 1;
	}

	/* Tradeoff: EITHER we loop as long as interrupts are pending
	 *           incurring the overhead of one extra run of the 'find_pending_irq' routine.
	 *           OR we do rely on the handler just being invoked again if multiple
	 *			 interrupts are pending.
	 *
	 *           The first solution gives better worst-case behavior
	 *			 the second slightly better average performance.
	 *			 --> we go for the first solution. This also enables us to catch
	 *			 runaway interrupts, i.e., bad drivers that don't clear interrupts
	 *			 at the device. Can be very handy during driver development...
	 */
	for ( loop=0, last_irq=-1, pri = thePic.current_priority;
		  (irq=find_highest_priority_pending_irq(&pri)) >=0;
		  loop++, last_irq = irq ) {

		/* raise priority level and remember current one */
		pri = change_executing_prio_level(pri);

		SYNC();

#if	(IRQ_DEBUG) & IRQ_DEBUG_MAXLAT
		if ( 0 == loop ) {
			diff = mftb()-diff;
			if ( diff > discovery_pic_max_dispatching_latency )
				discovery_pic_max_dispatching_latency = diff;
		}
#endif

#if (IRQ_DEBUG) & IRQ_DEBUG_DISPATCHER
		if ( BSP_DECREMENTER == irq ) {
			printk("IRQ: dispatching DECREMENTER\n");
		} else {
		int idx = irqDiv32(irq);
			printk("IRQ: dispatching #%i; causes[%i]=0x%08x\n", irq, idx, ld_le32(thePic.causes[idx]));
		}
#endif

		bsp_irq_dispatch_list( theConfig.irqHdlTbl, irq, theConfig.defaultEntry.hdl );

		/* restore executing priority level */
		(void)change_executing_prio_level(pri);

		if ( (loop > MAX_SPIN_LOOPS) && (last_irq == irq) ) {
			/* try to catch run-away interrupts without disabling a 'legal' one;
			 * this should never happen with the decrementer (and
			 * BSP_disable_irq_at_pic(BSP_DECREMENTER) would fail)
			 */
			printk("Runaway IRQ #%i; disabling\n", irq);
			BSP_disable_irq_at_pic(irq);
			loop = 0;
		}
	}

	if (!loop) {
		if ( decrementerPending && pri >= theConfig.irqPrioTbl[BSP_DECREMENTER] ) {
			/* we cannot mask the decrementer interrupt so it is possible that it
			 * gets delivered even though it has a lower priority than what we're
			 * currently executing at.
			 * In this case, we ignore the zero loop count and return;
			 * the interrupted instance of C_dispatch_irq_handler() will eventually
			 * lower the executing priority and catch the 'decrementerPending' flag
			 * we just set.
			 */
		} else {
			printk("Discovery: Spurious interrupt; causes were gpp: 0x%x, mhc: 0x%x, mlc: 0x%x\n", gpc, mhc, mlc);
			printk("Current priority level %i, decrementerPending %i\n", pri, decrementerPending);
			{
				rtems_irq_prio p=pri;	
				printk("PIC register dump:\n");
				discovery_dump_picregs();
				printk("Current Priority: %i, found %i\n",pri,find_highest_priority_pending_irq(&p));
				discovery_dump_picregs();
				for (p=0; p<=BSP_IRQ_MAX_PRIO; p++) {
					printk("M[%i] :",p);pmsks(thePic.mcache[p]);
				}
			}
		}
	}
	else if (loop>discovery_pic_max_loops)
		discovery_pic_max_loops = loop;

	return 0;
}


#if	(IRQ_DEBUG) & MVME6100_IRQ_DEBUG
void
discovery_pic_install_debug_irq(void)
{
	switch ( BSP_getBoardType() ) {
		case	MVME6100:	gpp_out_bit = GPP_WIRED_OUT_BIT_6100; break;
		case	MVME5500:	gpp_out_bit = GPP_WIRED_OUT_BIT_5500; break;
		default:
							gpp_out_bit = 0; break;
							break;
	}
	if ( gpp_out_bit ) {
		unsigned mppoff;
		switch (gpp_out_bit / 8) {
			default:	/* silence warning; this is never reached */
			case 0: mppoff = GT_MPP_Control0; break;
			case 1: mppoff = GT_MPP_Control1; break;
			case 2: mppoff = GT_MPP_Control2; break;
			case 3: mppoff = GT_MPP_Control3; break;
		}

		/* switch GPP pin allocated to watchdog (value 4) to
		 * GPP I/O (value 0 ??; have no doc, found out by experimenting)
		 */
		gt_bitmod(mppoff, 0, (0xf<<(4*(gpp_out_bit % 8))));

		/* make it an output */
		gt_bitmod(GT_GPP_IO_Control, (1<<gpp_out_bit), 0);

		/* don't invert levels */
		gt_bitmod(GT_GPP_Level_Control, 0, (1<<GPP_WIRED_IN_BIT) | (1<<gpp_out_bit));

		/* clear output */
		gt_bitmod(GT_GPP_Value, 0, 1<<gpp_out_bit);

		printk("GPP levelctl now 0x%08x\n", gt_read(GT_GPP_Level_Control));
		printk("GPP value    now 0x%08x\n", gt_read(GT_GPP_Value));
		printk("MPP ctl 0    now 0x%08x\n", gt_read(GT_MPP_Control0));
		printk("MPP ctl 1    now 0x%08x\n", gt_read(GT_MPP_Control1));
		printk("MPP ctl 2    now 0x%08x\n", gt_read(GT_MPP_Control2));
		printk("MPP ctl 3    now 0x%08x\n", gt_read(GT_MPP_Control3));

	}
}

/* Control the state of the external 'wire' that connects the
 * GPP_WIRED_OUT --> GPP_WIRED_IN pins
 */
void
discovery_pic_set_debug_irq(int on)
{
unsigned long flags, clr;
	if ( !gpp_out_bit ) {
		printk("discovery_pic_set_debug_irq(): unknown wire output\n");
		return;
	}
	if (on) {
		on  = 1<<gpp_out_bit;
		clr = 0;
	} else {
		clr = 1<<gpp_out_bit;
		on  = 0;
	}
	rtems_interrupt_disable(flags);
		gt_bitmod(GT_GPP_Value, on, clr);
	rtems_interrupt_enable(flags);
}
#endif

#if 0
/* Here's some code for testing */
#endif
