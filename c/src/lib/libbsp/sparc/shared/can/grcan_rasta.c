#include <rasta.h>

/* PCI frequency */
#define SYS_FREQ_HZ 30000000

/*#define USE_AT697_RAM              1      */

/* memarea_to_hw(x)
 *
 * x: address in AT697 address space
 *
 * returns the address in the RASTA address space that can be used to access x with dma.
 *
*/
#ifdef USE_AT697_RAM
static inline unsigned int memarea_to_hw(unsigned int addr) {
    return ((addr & 0x0fffffff) | RASTA_PCI_BASE);
}
#else
static inline unsigned int memarea_to_hw(unsigned int addr) {
    return ((addr & 0x0fffffff) | RASTA_LOCAL_SRAM);
}
#endif

#define MEMAREA_TO_HW(x) memarea_to_hw(x)

#define IRQ_CLEAR_PENDING(irqno)
#define IRQ_UNMASK(irqno)
#define IRQ_MASK(irqno)

#define IRQ_GLOBAL_PREPARE(level) rtems_interrupt_level level
#define IRQ_GLOBAL_DISABLE(level) rtems_interrupt_disable(level)
#define IRQ_GLOBAL_ENABLE(level) rtems_interrupt_enable(level)

#define GRCAN_REG_INT(handler,irqno,arg) \
  if ( grcan_rasta_int_reg ) \
    grcan_rasta_int_reg(handler,irqno,arg);

void (*grcan_rasta_int_reg)(void *handler, int irq, void *arg) = 0;

#define GRCAN_PREFIX(name) grcan_rasta##name

/* We provide our own handler */
#define GRCAN_DONT_DECLARE_IRQ_HANDLER

#define GRCAN_REG_BYPASS_CACHE
#define GRCAN_DMA_BYPASS_CACHE

#define GRCAN_MAX_CORES 1

/* Custom Statically allocated memory */
#undef STATICALLY_ALLOCATED_TX_BUFFER
#undef STATICALLY_ALLOCATED_RX_BUFFER

#define STATIC_TX_BUF_SIZE 4096
#define STATIC_RX_BUF_SIZE 4096
#define TX_BUF_SIZE 4096
#define RX_BUF_SIZE 4096

#define STATIC_TX_BUF_ADDR(core) \
	((unsigned int *)\
  (grcan_rasta_rambase+(core)*(STATIC_TX_BUF_SIZE+STATIC_RX_BUF_SIZE)))

#define STATIC_RX_BUF_ADDR(core) \
  ((unsigned int *) \
	(grcan_rasta_rambase+(core)*(STATIC_TX_BUF_SIZE+STATIC_RX_BUF_SIZE)+STATIC_RX_BUF_SIZE))


#define GRCAN_DEVNAME "/dev/grcan0"
#define GRCAN_DEVNAME_NO(devstr,no) ((devstr)[10]='0'+(no))

void grcan_rasta_interrupt_handler(int irq, void *pDev);

unsigned int grcan_rasta_rambase;

#include "grcan.c"


int grcan_rasta_ram_register(struct ambapp_bus *abus, int rambase)
{
  grcan_rasta_rambase = rambase;

  return GRCAN_PREFIX(_register)(abus);
}
#if 0
static void grcan_rasta_interrupt_handler(int v)
{
  /* We know there is always only one GRCAN core in a RASTA chip... */
  grcan_interrupt(&grcans[0]);
  /*
	struct grcan_priv *pDev = arg;
	grcan_interrupt(pDev);
  */
}
#endif
void GRCAN_PREFIX(_interrupt_handler)(int irq, void *pDev)
{
  grcan_interrupt(pDev);
}
