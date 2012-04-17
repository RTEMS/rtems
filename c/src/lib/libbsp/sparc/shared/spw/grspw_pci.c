/* Select PCI driver */
#define GRSPW_PCI

#undef GRSPW_MAXDEVS
#undef DEBUG_SPACEWIRE_ONOFF

/* Only Malloced memory supported
 */
#undef GRSPW_LOCAL_MEM

/* memory must be aligned to a 128k boundary */
unsigned int grspwpci_memarea_address;
#define GRSPW_LOCAL_MEM_ADR grspwpci_memarea_address

/* We have custom address tranlation for HW addresses */
#define GRSPW_ADR_TO

/* MEMAREA=>CPU used when reading descriptor buffer pointers,
 * they need to be translated from adresses used by GRSPW HW
 * into CPU readable addresses.
 *
 * NOT NEEDED AS GRSPW DRIVER USES INDEXES TO GET DESCRIPTOR
 * DATA POINTER ADDRESSES.
 */
#undef GRSPW_ADR_FROM

/* Set registered device name */
#define GRSPW_DEVNAME "/dev/grspwpci0"
#define GRSPW_DEVNAME_NO(devstr,no) ((devstr)[13]='0'+(no))

/* Any non-static function will begin with */
#define GRSPW_PREFIX(name) grspwpci##name

/* do nothing, assume that the interrupt handler is called
 * setup externally calling b1553_interrupt_handler.
 */
#define GRSPW_REG_INT(handler,irq,arg) \
 if ( grspw_pci_int_reg ) \
   grspw_pci_int_reg(handler,irq,arg);

void (*grspw_pci_int_reg)(void *handler, int irq, void *arg) = 0;


#ifdef GRSPW_ADR_TO
/* Translate an address within the Memory Region (memarea) into an Hardware
 * device address. This address is put into hardware registers or descriptors
 * so that the hardware can access the Memory Region.
 * Example:
 * A local AMBA access at 0xe0000000 will translate into PCI address 0x40000000,
 * the PCI address 0x40000000 will translate into LEON-AMBA address 0x40000000.
 */
unsigned int grspwpci_hw_address;
static inline unsigned int memarea_to_hw(unsigned int addr) {
		/* don't translate? */
		if ( grspwpci_hw_address == 0xffffffff )
			return addr;
    return ((addr & 0x0fffffff) | grspwpci_hw_address);
}
#endif

/* not used since BRM Core work with offsets */
#ifdef GRSPW_ADR_FROM
unsigned int grspwpci_cpu_access_address;
static inline unsigned int hw_to_cpu(unsigned int addr) {
		/* don't translate? */
		if ( grspwpci_cpu_address == 0xffffffff )
			return addr;
    return ((addr & 0x0fffffff) | grspwpci_cpu_address);
}
#endif

int grspwpci_interrupt_handler(int irq, void *arg);
#include "grspw.c"

/*
 *
 * memarea     = preallocated memory somewhere, pointer to start of memory.
 * hw_address  = how to translate a memarea address into an HW device AMBA address.
 */

int grspw_pci_register(
 struct ambapp_bus *bus,
 unsigned int memarea,
 unsigned int hw_address
 )
{
	/* Setup configuration */

	/* if zero the malloc will be used */
	grspwpci_memarea_address = memarea;

	grspwpci_hw_address = hw_address;

#ifdef GRSPW_ADR_FROM
	grspwpci_cpu_address = memarea & 0xf0000000;
#endif

	/* Register the driver */
	return GRSPW_PREFIX(_register)(bus);
}

/* Call this from PCI interrupt handler
 * irq = the irq number of the HW device local to that IRQMP controller
 *
 */
int grspwpci_interrupt_handler(int irq, void *arg){
	grspw_interrupt( (GRSPW_DEV *)arg );
  return 0;
}

#if 0
int grspw_pci_interrupt_handler(int irqmask){
	int i;
	unsigned int mask=0;
	/* find minor */
	for(i=0; i<spw_cores; i++){
		if ( (1<<SPW_PARAM(i).irq) & irqmask ){
			mask |= 1<<SPW_PARAM(i).irq;
			grspw_interrupt(i);
			/* more interrupts to scan for? */
			if ( irqmask & ~mask )
				return mask; /* handled */
		}
	}
	return mask;
}
#endif
