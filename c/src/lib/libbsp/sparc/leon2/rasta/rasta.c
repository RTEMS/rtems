/*
 *  GR-RASTA-IO PCI board driver
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/bspIo.h>
#include <pci.h>
#include <rasta.h>
#include <ambapp.h>
#include <grlib.h>
#include <grcan_rasta.h>
#include <grspw_rasta.h>
#include <b1553brm_rasta.h>
#include <apbuart_rasta.h>

#include <string.h>

/* If RASTA_SRAM is defined SRAM will be used, else SDRAM */
/*#define RASTA_SRAM 1*/

#define RASTA_IRQ  5

/* Offset from 0x80000000 (dual bus version) */
#define AHB1_IOAREA_BASE_ADDR 0x80100000
#define APB2_OFFSET    0x200000
#define IRQ_OFFSET     0x200500
#define GRHCAN_OFFSET  0x201000
#define BRM_OFFSET     0x100000
#define SPW_OFFSET     0xa00
#define UART_OFFSET    0x200200
#define GPIO0_OFF      0x200600
#define GPIO1_OFF      0x200700

/* #define DEBUG 1 */

#ifdef DEBUG
#define DBG(x...) printk(x)
#else
#define DBG(x...)
#endif

static int bus, dev, fun;

struct irqmp_regs *irq = NULL;
LEON_Register_Map *regs = (LEON_Register_Map *)0x80000000;

struct gpio_reg *gpio0, *gpio1;

/* static rtems_isr pci_interrupt_handler (rtems_vector_number v) { */

/*     volatile unsigned int *pci_int = (volatile unsigned int *) 0x80000168; */
/*     volatile unsigned int *pci_mem = (volatile unsigned int *) 0xb0400000; */

/*     if (*pci_int & 0x20) { */

/*         *pci_int = 0x20; */

/*         *pci_mem = 0; */

/*         printk("pci died\n"); */

/*     } */

/* } */

void *uart0_int_arg, *uart1_int_arg;
void *spw0_int_arg, *spw1_int_arg, *spw2_int_arg;
void *grcan_int_arg;
void *brm_int_arg;

void (*uart0_int_handler)(int irq, void *arg) = NULL;
void (*uart1_int_handler)(int irq, void *arg) = NULL;
void (*spw0_int_handler)(int irq, void *arg) = NULL;
void (*spw1_int_handler)(int irq, void *arg) = NULL;
void (*spw2_int_handler)(int irq, void *arg) = NULL;
void (*grcan_int_handler)(int irq, void *arg) = NULL;
void (*brm_int_handler)(int irq, void *arg) = NULL;

static rtems_isr rasta_interrupt_handler (rtems_vector_number v)
{
    unsigned int status;

    status = irq->ipend;

    if ( (status & GRCAN_IRQ) && grcan_int_handler ) {
      grcan_int_handler(GRCAN_IRQNO,grcan_int_arg);
    }

    if (status & SPW_IRQ) {
      if ( (status & SPW0_IRQ) && spw0_int_handler ){
        spw0_int_handler(SPW0_IRQNO,spw0_int_arg);
      }

      if ( (status & SPW1_IRQ) && spw1_int_handler ){
        spw1_int_handler(SPW1_IRQNO,spw1_int_arg);
      }

      if ( (status & SPW2_IRQ) && spw2_int_handler ){
        spw2_int_handler(SPW2_IRQNO,spw2_int_arg);
      }
    }
    if ((status & BRM_IRQ) && brm_int_handler ){
        brm_int_handler(BRM_IRQNO,brm_int_arg);
    }
    if ( (status & UART0_IRQ) && uart0_int_handler ) {
      uart0_int_handler(UART0_IRQNO,uart0_int_arg);
    }
    if ( (status & UART1_IRQ) && uart1_int_handler) {
      uart1_int_handler(UART1_IRQNO,uart1_int_arg);
    }

    DBG("RASTA-IRQ: 0x%x\n",status);
    irq->iclear = status;

}

void rasta_interrrupt_register(void *handler, int irqno, void *arg)
{
  DBG("RASTA: Registering irq %d\n",irqno);
  if ( irqno == UART0_IRQNO ){
    DBG("RASTA: Registering uart0 handler: 0x%x, arg: 0x%x\n",handler,arg);
    uart0_int_handler = handler;
    uart0_int_arg = arg;

    /* unmask interrupt source */
    irq->iclear = UART0_IRQ;
    irq->mask[0] |= UART0_IRQ;
  }

  if ( irqno == UART1_IRQNO ){
    DBG("RASTA: Registering uart1 handler: 0x%x, arg: 0x%x\n",handler,arg);
    uart1_int_handler = handler;
    uart1_int_arg = arg;

    /* unmask interrupt source */
    irq->iclear = UART1_IRQ;
    irq->mask[0] |= UART1_IRQ;
  }

  if ( irqno == SPW0_IRQNO ){
    DBG("RASTA: Registering spw0 handler: 0x%x, arg: 0x%x\n",handler,arg);
    spw0_int_handler = handler;
    spw0_int_arg = arg;

    /* unmask interrupt source */
    irq->iclear = SPW0_IRQ;
    irq->mask[0] |= SPW0_IRQ;
  }

  if ( irqno == SPW1_IRQNO ){
    DBG("RASTA: Registering spw1 handler: 0x%x, arg: 0x%x\n",handler,arg);
    spw1_int_handler = handler;
    spw1_int_arg = arg;

    /* unmask interrupt source */
    irq->iclear = SPW1_IRQ;
    irq->mask[0] |= SPW1_IRQ;
  }

  if ( irqno == SPW2_IRQNO ){
    DBG("RASTA: Registering spw2 handler: 0x%x, arg: 0x%x\n",handler,arg);
    spw2_int_handler = handler;
    spw2_int_arg = arg;

    /* unmask interrupt source */
    irq->iclear = SPW2_IRQ;
    irq->mask[0] |= SPW2_IRQ;
  }

  if ( irqno == GRCAN_IRQNO ){
    DBG("RASTA: Registering GRCAN handler: 0x%x, arg: 0x%x\n",handler,arg);
    grcan_int_handler = handler;
    grcan_int_arg = arg;

    /* unmask interrupt source */
    irq->iclear = GRCAN_IRQ;
    irq->mask[0] |= GRCAN_IRQ;
  }

  if ( irqno == BRM_IRQNO ){
    DBG("RASTA: Registering BRM handler: 0x%x, arg: 0x%x\n",handler,arg);
    brm_int_handler = handler;
    brm_int_arg = arg;

    /* unmask interrupt source */
    irq->iclear = BRM_IRQ;
    irq->mask[0] |= BRM_IRQ;
  }
}


int rasta_get_gpio(struct ambapp_bus *abus, int index, struct gpio_reg **regs,
                   int *irq)
{
  struct ambapp_apb_info dev;
  int cores;

  if ( !abus )
    return -1;

  /* Scan PnP info for GPIO port number 'index' */
  cores = ambapp_find_apbslv_next(abus, VENDOR_GAISLER, GAISLER_GPIO, &dev,
                                  index);
  if ( cores < 1 )
    return -1;

  if ( regs )
    *regs = (struct gpio_reg *)dev.start;

  if ( irq )
    *irq = dev.irq;

  return 0;
}

/* AMBA Plug&Play information */
static struct ambapp_bus abus;
static struct ambapp_mmap amba_maps[3];

int rasta_register(void)
{
    unsigned int bar0, bar1, data;

    unsigned int *page0 = NULL;
    unsigned int *apb_base = NULL;
    int found=0;


    DBG("Searching for RASTA board ...");

    /* Search PCI vendor/device id. */
    if (BSP_pciFindDevice(0x1AC8, 0x0010, 0, &bus, &dev, &fun) == 0) {
      found = 1;
    }

    /* Search old PCI vendor/device id. */
    if ( (!found) && (BSP_pciFindDevice(0x16E3, 0x0210, 0, &bus, &dev, &fun) == 0) ) {
      found = 1;
    }

    /* Did we find a RASTA board? */
    if ( !found )
      return -1;

    DBG(" found it (dev/fun: %d/%d).\n", dev, fun);

    pci_read_config_dword(bus, dev, fun, 0x10, &bar0);
    pci_read_config_dword(bus, dev, fun, 0x14, &bar1);

    page0 = (unsigned int *)(bar0 + 0x400000);
    *page0 = 0x80000000;                  /* Point PAGE0 to start of APB       */

    apb_base = (unsigned int *)(bar0+APB2_OFFSET);

/*  apb_base[0] = 0x000002ff;
    apb_base[1] = 0x8a205260;
    apb_base[2] = 0x00184000; */

    /* Configure memory controller */
#ifdef RASTA_SRAM
    apb_base[0] = 0x000002ff;
    apb_base[1] = 0x00001260;
    apb_base[2] = 0x000e8000;
#else
    apb_base[0] = 0x000002ff;
    apb_base[1] = 0x82206000;
    apb_base[2] = 0x000e8000;
#endif
    /* Set up rasta irq controller */
    irq = (struct irqmp_regs *) (bar0+IRQ_OFFSET);
    irq->iclear = 0xffff;
    irq->ilevel = 0;
    irq->mask[0] = 0xffff & ~(UART0_IRQ|UART1_IRQ|SPW0_IRQ|SPW1_IRQ|SPW2_IRQ|GRCAN_IRQ|BRM_IRQ);

    /* Configure AT697 ioport bit 7 to input pci irq */
    regs->PIO_Direction &= ~(1<<7);
    regs->PIO_Interrupt  |= (0x87<<8);    /* level sensitive */

    apb_base[0x100] |= 0x40000000;        /* Set GRPCI mmap 0x4 */
    apb_base[0x104] =  0x40000000;        /* 0xA0000000;  Point PAGE1 to RAM */


    /* set parity error response */
    pci_read_config_dword(bus, dev, fun, 0x4, &data);
    pci_write_config_dword(bus, dev, fun, 0x4, data|0x40);


    pci_master_enable(bus, dev, fun);

    /* install PCI interrupt vector */
    /*    set_vector(pci_interrupt_handler,14+0x10, 1); */


    /* install interrupt vector */
    set_vector(rasta_interrupt_handler, RASTA_IRQ+0x10, 1);

    /* Scan AMBA Plug&Play */

    /* AMBA MAP bar0 (in CPU) ==> 0x80000000(remote amba address) */
    amba_maps[0].size = 0x10000000;
    amba_maps[0].local_adr = bar0;
    amba_maps[0].remote_adr = 0x80000000;

    /* AMBA MAP bar1 (in CPU) ==> 0x40000000(remote amba address) */
    amba_maps[1].size = 0x10000000;
    amba_maps[1].local_adr = bar1;
    amba_maps[1].remote_adr = 0x40000000;

    /* Mark end of table */
    amba_maps[2].size=0;
    amba_maps[2].local_adr = 0;
    amba_maps[2].remote_adr = 0;

    memset(&abus,0,sizeof(abus));

    /* Start AMBA PnP scan at first AHB bus */
    ambapp_scan(&abus, bar0 + (AHB1_IOAREA_BASE_ADDR & ~0xf0000000), NULL,
                &amba_maps[0]);

    printk("Registering RASTA GRCAN driver\n\r");

    /*grhcan_register(bar0 + GRHCAN_OFFSET, bar1);*/
    grcan_rasta_int_reg=rasta_interrrupt_register;
    if ( grcan_rasta_ram_register(&abus,bar1+0x20000) ){
      printk("Failed to register RASTA GRCAN driver\n\r");
      return -1;
    }

    printk("Registering RASTA BRM driver\n\r");

    /*brm_register(bar0   +  BRM_OFFSET, bar1);*/
    /* register the BRM RASTA driver, use 128k on RASTA SRAM... */
    b1553brm_rasta_int_reg=rasta_interrrupt_register;
	  if ( b1553brm_rasta_register(&abus,2,0,3,bar1,0x40000000) ){
      printk("Failed to register BRM RASTA driver\n");
      return -1;
    }

    /* provide the spacewire driver with AMBA Plug&Play
     * info so that it can find the GRSPW cores.
     */
    grspw_rasta_int_reg=rasta_interrrupt_register;
    if ( grspw_rasta_register(&abus,bar1) ){
      printk("Failed to register RASTA GRSPW driver\n\r");
      return -1;
    }

    /* provide the spacewire driver with AMBA Plug&Play
     * info so that it can find the GRSPW cores.
     */
    apbuart_rasta_int_reg=rasta_interrrupt_register;
    if ( apbuart_rasta_register(&abus) ){
      printk("Failed to register RASTA APBUART driver\n\r");
      return -1;
    }

    /* Find GPIO0 address */
    if ( rasta_get_gpio(&abus,0,&gpio0,NULL) ){
      printk("Failed to get address for RASTA GPIO0\n\r");
      return -1;
    }

    /* Find GPIO1 address */
    if ( rasta_get_gpio(&abus,1,&gpio1,NULL) ){
      printk("Failed to get address for RASTA GPIO1\n\r");
      return -1;
    }

    /* Successfully registered the RASTA board */
    return 0;
}
