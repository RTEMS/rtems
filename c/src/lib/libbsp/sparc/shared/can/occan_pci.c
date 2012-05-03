/* PCI cannot do byte accesses to addresses aligned byte wise
 * Use alternative reg map.
 */
#define OCCAN_WORD_REGS

/* Set registered device name */
#define OCCAN_DEVNAME "/dev/occanpci0"
#define OCCAN_DEVNAME_NO(devstr,no) ((devstr)[13]='0'+(no))

/* Any non-static function will begin with */
#define OCCAN_PREFIX(name) occanpci##name

/* do nothing, assume that the interrupt handler is called
 * setup externally calling b1553_interrupt_handler.
 */
#define OCCAN_REG_INT(handler,irq,arg) \
 if ( occan_pci_int_reg ) \
   occan_pci_int_reg(handler,irq,arg);

void (*occan_pci_int_reg)(void *handler, int irq, void *arg) = 0;

void occanpci_interrupt_handler(int irq, void *arg);

/* AMBA Bus is clocked using the PCI clock (33.3MHz) */
#define SYS_FREQ_HZ 33333333

/* Enable two redundant channels */
#define REDUNDANT_CHANNELS 2

#define OCCAN_SET_CHANNEL(priv,channel) occanpci_set_channel(priv,channel)

#include "occan.c"

/* Define method that sets redundant channel
 * The channel select register:
 *  0x00 = byte regs
 *  0x40 = channel select
 *  0x80 = word regs
 */
static void inline occanpci_set_channel(occan_priv *priv, int channel){
	unsigned int *chan_sel = (unsigned int *)(((unsigned int)priv->regs & ~0xff)+0x40);
	if ( channel == 0 )
	  *chan_sel = 0;
	else
		*chan_sel = 0xffffffff;
}

int occan_pci_register(struct ambapp_bus *bus)
{
	/* Setup configuration */

	/* Register the driver */
	return OCCAN_PREFIX(_register)(bus);
}


/* Call this from PCI interrupt handler
 * irq = the irq number of the HW device local to that IRQMP controller
 *
 */
void occanpci_interrupt_handler(int irq, void *arg){
	occan_interrupt(arg);
}
