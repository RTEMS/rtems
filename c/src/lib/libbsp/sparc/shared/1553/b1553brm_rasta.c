/* Select PCI driver */
#define B1553BRM_NO_AMBA
#define B1553BRM_PCI

#undef B1553BRM_MAXDEVS

/* Use only 16K memory */
#define DMA_MEM_128K

/* Malloced memory or
 * Card local memory
 */
#define B1553BRM_LOCAL_MEM

#define DONT_DEF_RAMON

/* memory must be aligned to a 128k boundary */
unsigned int brmrasta_memarea_address;
#define B1553BRM_LOCAL_MEM_ADR brmrasta_memarea_address

/* We have custom address tranlation for HW addresses */
#define B1553BRM_ADR_TO

/* No custom MEMAREA=>CPU used since BRM Core work with offsets
 * in it's descriptors.
 */
#undef B1553BRM_ADR_FROM

/* Set registered device name */
#define B1553BRM_DEVNAME "/dev/brmrasta0"
#define B1553BRM_DEVNAME_NO(devstr,no) ((devstr)[13]='0'+(no))

/* Any non-static function will begin with */
#define B1553BRM_PREFIX(name) b1553brmrasta##name

/* do nothing, assume that the interrupt handler is called
 * setup externally calling b1553_interrupt_handler.
 */
#define B1553BRM_REG_INT(handler,irq,arg) \
 if ( b1553brm_rasta_int_reg ) \
   b1553brm_rasta_int_reg(handler,irq,arg);


#ifdef B1553BRM_ADR_TO
/* Translate a address within the Memory Region (memarea) into an Hardware
 * device address. This address is put into hardware registers or descriptors
 * so that the hardware can access the Memory Region.
 * Example:
 * An local AMBA access at 0xe0000000 will translate into PCI address 0x40000000,
 * the PCI address 0x40000000 will translate into CPU-AMBA address 0x40000000.
 */
unsigned int brmrasta_hw_address;
static inline unsigned int memarea_to_hw(unsigned int addr) {
		/* don't translate? */
		if ( brmrasta_hw_address == 0xffffffff )
			return addr;
    return ((addr & 0x0fffffff) | brmrasta_hw_address);
}
#endif

/* not used since BRM Core work with offsets */
#ifdef B1553BRM_ADR_FROM
unsigned int brmrasta_cpu_access_address;
static inline unsigned int hw_to_cpu(unsigned int addr) {
		/* don't translate? */
		if ( brmrasta_cpu_address == 0xffffffff )
			return addr;
    return ((addr & 0x0fffffff) | brmrasta_cpu_address);
}
#endif

void (*b1553brm_rasta_int_reg)(void *handler, int irq, void *arg) = 0;

static void b1553brmrasta_interrupt_handler(int irq, void *arg);

#include "b1553brm.c"

/*
 *
 * memarea     = preallocated memory somewhere, pointer to start of memory.
 * hw_address  = how to translate a memarea address into an HW device AMBA address.
 */

int b1553brm_rasta_register(
 struct ambapp_bus *bus,
 unsigned int clksel,
 unsigned int clkdiv,
 unsigned int brm_freq,
 unsigned int memarea,
 unsigned int hw_address
 )
{
	/* Setup configuration */

	/* if zero the malloc will be used */
	brmrasta_memarea_address = memarea;

	brmrasta_hw_address = hw_address;

#ifdef B1553BRM_ADR_FROM
	brmrasta_cpu_address = memarea & 0xf0000000;
#endif

	/* Register the driver */
	return B1553BRM_PREFIX(_register)(bus,clksel,clkdiv,brm_freq);
}

/* Call this from RASTA interrupt handler
 * irq = the irq number of the HW device local to that IRQMP controller
 *
 */
static void b1553brmrasta_interrupt_handler(int irq, void *arg){
	brm_interrupt(arg);
}

