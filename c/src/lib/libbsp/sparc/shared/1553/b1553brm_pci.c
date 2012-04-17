/* Select PCI driver */
#define B1553BRM_NO_AMBA
#define B1553BRM_PCI

#undef B1553BRM_MAXDEVS

/* Use only 16K memory */
#define DMA_MEM_16K

/* Malloced memory or
 * Card local memory
 */
#define B1553BRM_LOCAL_MEM

#define DONT_DEF_RAMON

/* memory must be aligned to a 128k boundary */
unsigned int brmpci_memarea_address;
#define B1553BRM_LOCAL_MEM_ADR brmpci_memarea_address

/* We have custom address tranlation for HW addresses */
#define B1553BRM_ADR_TO

/* No custom MEMAREA=>CPU used since BRM Core work with offsets
 * in it's descriptors.
 */
#undef B1553BRM_ADR_FROM

/* Set registered device name */
#define B1553BRM_DEVNAME "/dev/brmpci0"
#define B1553BRM_DEVNAME_NO(devstr,no) ((devstr)[11]='0'+(no))

/* Any non-static function will begin with */
#define B1553BRM_PREFIX(name) b1553brmpci##name

/* do nothing, assume that the interrupt handler is called
 * setup externally calling b1553_interrupt_handler.
 */
#define B1553BRM_REG_INT(handler,irq,arg) \
 if ( b1553brm_pci_int_reg ) \
   b1553brm_pci_int_reg(handler,irq,arg);


#ifdef B1553BRM_ADR_TO
/* Translate a address within the Memory Region (memarea) into an Hardware
 * device address. This address is put into hardware registers or descriptors
 * so that the hardware can access the Memory Region.
 * Example:
 * An local AMBA access at 0xe0000000 will translate into PCI address 0x40000000,
 * the PCI address 0x40000000 will translate into CPU-AMBA address 0x40000000.
 */
unsigned int brmpci_hw_address;
static inline unsigned int memarea_to_hw(unsigned int addr) {
		/* don't translate? */
		if ( brmpci_hw_address == 0xffffffff )
			return addr;
    return ((addr & 0x000fffff) | brmpci_hw_address);
}
#endif

/* not used since BRM Core work with offsets */
#ifdef B1553BRM_ADR_FROM
unsigned int brmpci_cpu_access_address;
static inline unsigned int hw_to_cpu(unsigned int addr) {
		/* don't translate? */
		if ( brmpci_cpu_address == 0xffffffff )
			return addr;
    return ((addr & 0x0fffffff) | brmpci_cpu_address);
}
#endif

void (*b1553brm_pci_int_reg)(void *handler, int irq, void *arg) = 0;

static void b1553brmpci_interrupt_handler(int irq, void *arg);

#include "b1553brm.c"

/*
 *
 * memarea     = preallocated memory somewhere, pointer to start of memory.
 * hw_address  = how to translate a memarea address into an HW device AMBA address.
 */

int b1553brm_pci_register(
 struct ambapp_bus *bus,
 unsigned int clksel,
 unsigned int clkdiv,
 unsigned int brm_freq,
 unsigned int memarea,
 unsigned int hw_address
 )
{
	/* Setup configuration */

	/* if zero malloc will be used */
	brmpci_memarea_address = memarea;

	brmpci_hw_address = hw_address;

#ifdef B1553BRM_ADR_FROM
	brmpci_cpu_address = memarea & 0xf0000000;
#endif

	/* Register the driver */
	return B1553BRM_PREFIX(_register)(bus,clksel,clkdiv,brm_freq);
}

/* Call this from PCI interrupt handler
 * irq = the irq number of the HW device local to that IRQMP controller
 *
 */
static void b1553brmpci_interrupt_handler(int irq, void *arg){
	brm_interrupt(arg);
}

#if 0
int b1553brm_pci_interrupt_handler(int irqmask){
	int i;
	unsigned int mask=0;
	/* find minor */
	for(i=0; i<brm_cores; i++){
		if ( (1<<brms[i].irqno) & irqmask ){
			mask |= 1<<brms[i].irqno;
			brm_interrupt(&brms[i]);
			/* more interrupts to scan for? */
			if ( irqmask & ~mask )
				return mask; /* handled */
		}
	}
	return mask;
}
#endif
