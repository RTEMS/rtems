#undef DEBUG

/* Set registered device name */
#define APBUART_DEVNAME "/dev/apbupci0"
#define APBUART_DEVNAME_NO(devstr,no) ((devstr)[12]='0'+(no))

/* Any non-static function will begin with */
#define APBUART_PREFIX(name) apbuartpci##name

/* do nothing, assume that the interrupt handler is called
 * setup externally calling apbuartpci_interrupt_handler.
 */
#define APBUART_REG_INT(handler,irq,arg) \
 if ( apbuart_pci_int_reg ) \
   apbuart_pci_int_reg(handler,irq,arg);

void (*apbuart_pci_int_reg)(void *handler, int irq, void *arg) = 0;

void apbuartpci_interrupt_handler(int irq, void *arg);

/* AMBA Bus is clocked using the PCI clock (33.3MHz) */
#define SYS_FREQ_HZ 33333333

#include "apbuart.c"

int apbuart_pci_register(struct ambapp_bus *bus)
{
	/* Setup configuration */

	/* Register the driver */
	return APBUART_PREFIX(_register)(bus);
}


/* Call this from PCI interrupt handler
 * irq = the irq number of the HW device local to that IRQMP controller
 *
 */
void apbuartpci_interrupt_handler(int irq, void *arg){
	apbuart_interrupt(arg);
}
