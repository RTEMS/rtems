/*
 *  openpic.c -- OpenPIC Interrupt Handling
 *
 *  Copyright (C) 1997 Geert Uytterhoeven
 *
 *  Modified to compile in RTEMS development environment
 *  by Eric Valette
 *
 *  Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 * $Id$
 */

/*
 *  Note: Interprocessor Interrupt (IPI) and Timer support is incomplete
 */

#include <rtems/bspIo.h>
#include <bsp/openpic.h>
#include <bsp/pci.h>
#include <bsp/consoleIo.h>
#include <libcpu/io.h>
#include <libcpu/byteorder.h>
#include <bsp.h>

#define NULL 0
#define REGISTER_DEBUG
#undef REGISTER_DEBUG


volatile struct OpenPIC *OpenPIC = NULL;
unsigned int OpenPIC_NumInitSenses  = 0;
unsigned char *OpenPIC_InitSenses  = NULL;

static unsigned int NumProcessors;
static unsigned int NumSources;


    /*
     *  Accesses to the current processor's registers
     */

#define THIS_CPU		Processor[cpu]
#define CHECK_THIS_CPU		check_arg_cpu(cpu)


    /*
     *  Sanity checks
     */

#if 1
#define check_arg_ipi(ipi) \
    if (ipi < 0 || ipi >= OPENPIC_NUM_IPI) \
	printk("openpic.c:%d: illegal ipi %d\n", __LINE__, ipi);
#define check_arg_timer(timer) \
    if (timer < 0 || timer >= OPENPIC_NUM_TIMERS) \
	printk("openpic.c:%d: illegal timer %d\n", __LINE__, timer);
#define check_arg_vec(vec) \
    if (vec < 0 || vec >= OPENPIC_NUM_VECTORS) \
	printk("openpic.c:%d: illegal vector %d\n", __LINE__, vec);
#define check_arg_pri(pri) \
    if (pri < 0 || pri >= OPENPIC_NUM_PRI) \
	printk("openpic.c:%d: illegal priority %d\n", __LINE__, pri);
#define check_arg_irq(irq) \
    if (irq < 0 || irq >= NumSources) \
	printk("openpic.c:%d: illegal irq %d from %p,[%p],[[%p]]\n", \
	       __LINE__, irq, __builtin_return_address(0), \
	       __builtin_return_address(1), __builtin_return_address(2) \
	       );
#define check_arg_cpu(cpu) \
    if (cpu < 0 || cpu >= NumProcessors) \
	printk("openpic.c:%d: illegal cpu %d\n", __LINE__, cpu);
#else
#define check_arg_ipi(ipi)	do {} while (0)
#define check_arg_timer(timer)	do {} while (0)
#define check_arg_vec(vec)	do {} while (0)
#define check_arg_pri(pri)	do {} while (0)
#define check_arg_irq(irq)	do {} while (0)
#define check_arg_cpu(cpu)	do {} while (0)
#endif



    /*
     *  I/O functions
     */

static inline unsigned int openpic_read(volatile unsigned int *addr)
{
    unsigned int val;

    val = ld_le32(addr);
#ifdef REGISTER_DEBUG
    printk("openpic_read(0x%08x) = 0x%08x\n", (unsigned int)addr, val);
#endif
    return val;
}

static inline void openpic_write(volatile unsigned int *addr, unsigned int val)
{
#ifdef REGISTER_DEBUG
    printk("openpic_write(0x%08x, 0x%08x)\n", (unsigned int)addr, val);
#endif
    out_le32(addr, val);
}


static inline unsigned int openpic_readfield(volatile unsigned int *addr, unsigned int mask)
{
    unsigned int val = openpic_read(addr);
    return val & mask;
}

inline void openpic_writefield(volatile unsigned int *addr, unsigned int mask,
				      unsigned int field)
{
    unsigned int val = openpic_read(addr);
    openpic_write(addr, (val & ~mask) | (field & mask));
}

static inline void openpic_clearfield(volatile unsigned int *addr, unsigned int mask)
{
    openpic_writefield(addr, mask, 0);
}

static inline void openpic_setfield(volatile unsigned int *addr, unsigned int mask)
{
    openpic_writefield(addr, mask, mask);
}


    /*
     *  Update a Vector/Priority register in a safe manner. The interrupt will
     *  be disabled.
     */

static void openpic_safe_writefield(volatile unsigned int *addr, unsigned int mask,
				    unsigned int field)
{
    openpic_setfield(addr, OPENPIC_MASK);
    /* wait until it's not in use */
    while (openpic_read(addr) & OPENPIC_ACTIVITY);
    openpic_writefield(addr, mask | OPENPIC_MASK, field | OPENPIC_MASK);
}


/* -------- Global Operations ---------------------------------------------- */


    /*
     *  Initialize the OpenPIC
     *
     * Add some kludge to use the Motorola Raven OpenPIC which does not
     * report vendor and device id, and gets the wrong number of interrupts.
     * (Motorola did a great job on that one!)
     */

void openpic_init(int main_pic)
{
    unsigned int t, i;
    unsigned int vendorid, devid, stepping, timerfreq;
    const char *version, *vendor, *device;

    if (!OpenPIC)
	BSP_panic("No OpenPIC found");

    t = openpic_read(&OpenPIC->Global.Feature_Reporting0);
    switch (t & OPENPIC_FEATURE_VERSION_MASK) {
	case 1:
	    version = "1.0";
	    break;
	case 2:
	    version = "1.2";
	    break;
	default:
	    version = "?";
	    break;
    }
    NumProcessors = ((t & OPENPIC_FEATURE_LAST_PROCESSOR_MASK) >>
		     OPENPIC_FEATURE_LAST_PROCESSOR_SHIFT) + 1;
    NumSources = ((t & OPENPIC_FEATURE_LAST_SOURCE_MASK) >>
		  OPENPIC_FEATURE_LAST_SOURCE_SHIFT) + 1;
    t = openpic_read(&OpenPIC->Global.Vendor_Identification);

    vendorid = t & OPENPIC_VENDOR_ID_VENDOR_ID_MASK;
    devid = (t & OPENPIC_VENDOR_ID_DEVICE_ID_MASK) >>
	    OPENPIC_VENDOR_ID_DEVICE_ID_SHIFT;
    stepping = (t & OPENPIC_VENDOR_ID_STEPPING_MASK) >>
	       OPENPIC_VENDOR_ID_STEPPING_SHIFT;

    /* Kludge for the Raven */
    pci_read_config_dword(0, 0, 0, 0, &t);
    if (t == PCI_VENDOR_ID_MOTOROLA + (PCI_DEVICE_ID_MOTOROLA_RAVEN<<16)) {
    	vendor = "Motorola";
	device = "Raven";
	NumSources += 1;
    } else {
	switch (vendorid) {
	    case OPENPIC_VENDOR_ID_APPLE:
	  	vendor = "Apple";
	    	break;
	    default:
	    	vendor = "Unknown";
	    break;
	}
	switch (devid) {
	    case OPENPIC_DEVICE_ID_APPLE_HYDRA:
	    	device = "Hydra";
		break;
	    default:
	        device = "Unknown";
		break;
	}
    }
    printk("OpenPIC Version %s (%d CPUs and %d IRQ sources) at %p\n", version,
	   NumProcessors, NumSources, OpenPIC);

    printk("OpenPIC Vendor %d (%s), Device %d (%s), Stepping %d\n", vendorid,
	   vendor, devid, device, stepping);

    timerfreq = openpic_read(&OpenPIC->Global.Timer_Frequency);
    printk("OpenPIC timer frequency is ");
    if (timerfreq)
	printk("%d Hz\n", timerfreq);
    else
	printk("not set\n");

    if ( main_pic )
    {
	    /* Initialize timer interrupts */
	    for (i = 0; i < OPENPIC_NUM_TIMERS; i++) {
		    /* Disabled, Priority 0 */
		    openpic_inittimer(i, 0, OPENPIC_VEC_TIMER+i);
		    /* No processor */
		    openpic_maptimer(i, 0);
	    }
	    
	    /* Initialize IPI interrupts */
	    for (i = 0; i < OPENPIC_NUM_IPI; i++) {
		    /* Disabled, Priority 0 */
		    openpic_initipi(i, 0, OPENPIC_VEC_IPI+i);
	    }
	    
	    /* Initialize external interrupts */
	    /* SIOint (8259 cascade) is special */
	    openpic_initirq(0, 8, OPENPIC_VEC_SOURCE, 1, 1);
	    /* Processor 0 */
	    openpic_mapirq(0, 1<<0);
	    for (i = 1; i < NumSources; i++) {
		    /* Enabled, Priority 8 */
		    openpic_initirq(i, 8, OPENPIC_VEC_SOURCE+i, 0,
				    i < OpenPIC_NumInitSenses ? OpenPIC_InitSenses[i] : 1);
		    /* Processor 0 */
		    openpic_mapirq(i, 1<<0);
	    }
	    
	    /* Initialize the spurious interrupt */
	    openpic_set_spurious(OPENPIC_VEC_SPURIOUS);
#if 0	    
	    if (request_irq(IRQ_8259_CASCADE, no_action, SA_INTERRUPT,
			    "82c59 cascade", NULL))
	      printk("Unable to get OpenPIC IRQ 0 for cascade\n");
#endif	    
	    openpic_set_priority(0, 0);
	    openpic_disable_8259_pass_through();
    }
}


    /*
     *  Reset the OpenPIC
     */

void openpic_reset(void)
{
    openpic_setfield(&OpenPIC->Global.Global_Configuration0,
    		       OPENPIC_CONFIG_RESET);
}


    /*
     *  Enable/disable 8259 Pass Through Mode
     */

void openpic_enable_8259_pass_through(void)
{
    openpic_clearfield(&OpenPIC->Global.Global_Configuration0,
    		       OPENPIC_CONFIG_8259_PASSTHROUGH_DISABLE);
}

void openpic_disable_8259_pass_through(void)
{
    openpic_setfield(&OpenPIC->Global.Global_Configuration0,
    		     OPENPIC_CONFIG_8259_PASSTHROUGH_DISABLE);
}


    /*
     *  Find out the current interrupt
     */

unsigned int openpic_irq(unsigned int cpu)
{
    unsigned int vec;

    check_arg_cpu(cpu);
    vec = openpic_readfield(&OpenPIC->THIS_CPU.Interrupt_Acknowledge,
			    OPENPIC_VECTOR_MASK);
    return vec;
}


    /*
     *  Signal end of interrupt (EOI) processing
     */

void openpic_eoi(unsigned int cpu)
{
    check_arg_cpu(cpu);
    openpic_write(&OpenPIC->THIS_CPU.EOI, 0);
}


    /*
     *  Get/set the current task priority
     */

unsigned int openpic_get_priority(unsigned int cpu)
{
    CHECK_THIS_CPU;
    return openpic_readfield(&OpenPIC->THIS_CPU.Current_Task_Priority,
			     OPENPIC_CURRENT_TASK_PRIORITY_MASK);
}

void openpic_set_priority(unsigned int cpu, unsigned int pri)
{
    CHECK_THIS_CPU;
    check_arg_pri(pri);
    openpic_writefield(&OpenPIC->THIS_CPU.Current_Task_Priority,
    		       OPENPIC_CURRENT_TASK_PRIORITY_MASK, pri);
}

    /*
     *  Get/set the spurious vector
     */

unsigned int openpic_get_spurious(void)
{
    return openpic_readfield(&OpenPIC->Global.Spurious_Vector,
    			     OPENPIC_VECTOR_MASK);
}

void openpic_set_spurious(unsigned int vec)
{
    check_arg_vec(vec);
    openpic_writefield(&OpenPIC->Global.Spurious_Vector, OPENPIC_VECTOR_MASK,
    		       vec);
}


    /*
     *  Initialize one or more CPUs
     */

void openpic_init_processor(unsigned int cpumask)
{
    openpic_write(&OpenPIC->Global.Processor_Initialization, cpumask);
}


/* -------- Interprocessor Interrupts -------------------------------------- */


    /*
     *  Initialize an interprocessor interrupt (and disable it)
     *
     *  ipi: OpenPIC interprocessor interrupt number
     *  pri: interrupt source priority
     *  vec: the vector it will produce
     */

void openpic_initipi(unsigned int ipi, unsigned int pri, unsigned int vec)
{
    check_arg_timer(ipi);
    check_arg_pri(pri);
    check_arg_vec(vec);
    openpic_safe_writefield(&OpenPIC->Global.IPI_Vector_Priority(ipi),
    			    OPENPIC_PRIORITY_MASK | OPENPIC_VECTOR_MASK,
    			    (pri << OPENPIC_PRIORITY_SHIFT) | vec);
}


    /*
     *  Send an IPI to one or more CPUs
     */

void openpic_cause_IPI(unsigned int cpu, unsigned int ipi, unsigned int cpumask)
{
    CHECK_THIS_CPU;
    check_arg_ipi(ipi);
    openpic_write(&OpenPIC->THIS_CPU.IPI_Dispatch(ipi), cpumask);
}


/* -------- Timer Interrupts ----------------------------------------------- */


    /*
     *  Initialize a timer interrupt (and disable it)
     *
     *  timer: OpenPIC timer number
     *  pri: interrupt source priority
     *  vec: the vector it will produce
     */

void openpic_inittimer(unsigned int timer, unsigned int pri, unsigned int vec)
{
    check_arg_timer(timer);
    check_arg_pri(pri);
    check_arg_vec(vec);
    openpic_safe_writefield(&OpenPIC->Global.Timer[timer].Vector_Priority,
    			    OPENPIC_PRIORITY_MASK | OPENPIC_VECTOR_MASK,
    			    (pri << OPENPIC_PRIORITY_SHIFT) | vec);
}


    /*
     *  Map a timer interrupt to one or more CPUs
     */

void openpic_maptimer(unsigned int timer, unsigned int cpumask)
{
    check_arg_timer(timer);
    openpic_write(&OpenPIC->Global.Timer[timer].Destination, cpumask);
}


/* -------- Interrupt Sources ---------------------------------------------- */


    /*
     *  Enable/disable an interrupt source
     */

void openpic_enable_irq(unsigned int irq)
{
    check_arg_irq(irq);
    openpic_clearfield(&OpenPIC->Source[irq].Vector_Priority, OPENPIC_MASK);
}

void openpic_disable_irq(unsigned int irq)
{
    check_arg_irq(irq);
    openpic_setfield(&OpenPIC->Source[irq].Vector_Priority, OPENPIC_MASK);
}


    /*
     *  Initialize an interrupt source (and disable it!)
     *
     *  irq: OpenPIC interrupt number
     *  pri: interrupt source priority
     *  vec: the vector it will produce
     *  pol: polarity (1 for positive, 0 for negative)
     *  sense: 1 for level, 0 for edge
     */

void openpic_initirq(unsigned int irq, unsigned int pri, unsigned int vec, int pol, int sense)
{
    check_arg_irq(irq);
    check_arg_pri(pri);
    check_arg_vec(vec);
    openpic_safe_writefield(&OpenPIC->Source[irq].Vector_Priority,
    			    OPENPIC_PRIORITY_MASK | OPENPIC_VECTOR_MASK |
    			    OPENPIC_SENSE_POLARITY | OPENPIC_SENSE_LEVEL,
    			    (pri << OPENPIC_PRIORITY_SHIFT) | vec |
			    (pol ? OPENPIC_SENSE_POLARITY : 0) |
			    (sense ? OPENPIC_SENSE_LEVEL : 0));
}


    /*
     *  Map an interrupt source to one or more CPUs
     */

void openpic_mapirq(unsigned int irq, unsigned int cpumask)
{
    check_arg_irq(irq);
    openpic_write(&OpenPIC->Source[irq].Destination, cpumask);
}


    /*
     *  Set the sense for an interrupt source (and disable it!)
     *
     *  sense: 1 for level, 0 for edge
     */

void openpic_set_sense(unsigned int irq, int sense)
{
    check_arg_irq(irq);
    openpic_safe_writefield(&OpenPIC->Source[irq].Vector_Priority,
    			    OPENPIC_SENSE_LEVEL,
			    (sense ? OPENPIC_SENSE_LEVEL : 0));
}
