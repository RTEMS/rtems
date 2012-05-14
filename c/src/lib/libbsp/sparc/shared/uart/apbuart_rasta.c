#undef DEBUG

/* Set registered device name */
#define APBUART_DEVNAME "/dev/apburasta0"
#define APBUART_DEVNAME_NO(devstr,no) ((devstr)[14]='0'+(no))

/* Any non-static function will begin with */
#define APBUART_PREFIX(name) apbuartrasta##name

/* do nothing, assume that the interrupt handler is called
 * setup externally calling apbuartrasta_interrupt_handler.
 */
#define APBUART_REG_INT(handler,irq,arg) \
 if ( apbuart_rasta_int_reg ) \
   apbuart_rasta_int_reg(handler,irq,arg);

void (*apbuart_rasta_int_reg)(void *handler, int irq, void *arg) = 0;

void apbuartrasta_interrupt_handler(int irq, void *arg);

/* AMBA Bus is clocked using the RASTA internal clock (30MHz) */
#define SYS_FREQ_HZ 30000000

#include "apbuart.c"

int apbuart_rasta_register(struct ambapp_bus *bus)
{
	/* Setup configuration */

	/* Register the driver */
	return APBUART_PREFIX(_register)(bus);
}


/* Call this from RASTA interrupt handler
 * irq = the irq number of the HW device local to that IRQMP controller
 *
 */
void apbuartrasta_interrupt_handler(int irq, void *arg){
	apbuart_interrupt(arg);
}
