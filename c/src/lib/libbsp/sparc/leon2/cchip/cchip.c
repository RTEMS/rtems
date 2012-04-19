/*
 *  GR-701 (Companion Chip) PCI board driver
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <rtems/bspIo.h>
#include <rtems.h>
#include <string.h>

#include <rtems.h>
#include <leon.h>
#include <ambapp.h>
#include <pci.h>

#include <b1553brm_pci.h>
#include <occan_pci.h>
#include <grspw_pci.h>
#include <apbuart_pci.h>

#include <cchip.h>

/*
#define DEBUG
#define DEBUG_IRQS
*/
#define BOARD_INFO
/*#define PRINT_SPURIOUS*/

/* AT697 Register MAP */
static LEON_Register_Map *regs = (LEON_Register_Map *)0x80000000;

/* initializes interrupt management for companionship board */
void cchip1_irq_init(void);

/* register interrupt handler (called from drivers) */
void cchip1_set_isr(void *handler, int irqno, void *arg);

#define READ_REG(address) _READ_REG((unsigned int)address)
static __inline__ unsigned int _READ_REG(unsigned int addr) {
	unsigned int tmp;
	__asm__ ("lda [%1]1, %0 "
	  : "=r"(tmp)
	  : "r"(addr)
	);
	return tmp;
}

/* PCI bride reg layout on AMBA side */
typedef struct {
	unsigned int bar0;
	unsigned int bar1;
	unsigned int bar2;
	unsigned int bar3;
	unsigned int bar4;/* 0x10 */

	unsigned int unused[4*3-1];

	unsigned int ambabars[1]; /* 0x40 */
} amba_bridge_regs;

/* PCI bride reg layout on PCI side */
typedef struct {
	unsigned int bar0;
	unsigned int bar1;
	unsigned int bar2;
	unsigned int bar3;
	unsigned int bar4; /* 0x10 */

	unsigned int ilevel;
	unsigned int ipend;
	unsigned int iforce;
	unsigned int istatus;
	unsigned int iclear;
	unsigned int imask;
} pci_bridge_regs;

typedef struct {
	pci_bridge_regs  *pcib;
	amba_bridge_regs *ambab;

	/* AT697 PCI */
	unsigned int bars[5];
	int bus, dev, fun;

	/* AMBA bus */
	struct ambapp_bus amba_bus;
	struct ambapp_mmap amba_maps[2];

	/* FT AHB SRAM */
	int ftsram_size; /* kb */
	unsigned int ftsram_start;
	unsigned int ftsram_end;

} cchip1;

cchip1 cc1;

int init_pcif(void){
	unsigned int com1;
	int i,bus,dev,fun;
	pci_bridge_regs *pcib;
	amba_bridge_regs *ambab;
	struct ambapp_bus *abus;

  if ( BSP_pciFindDevice(0x1AC8, 0x0701, 0, &bus, &dev, &fun) == 0 ) {
    ;
  }else if (BSP_pciFindDevice(0x16E3, 0x0210, 0, &bus, &dev, &fun) == 0) {
    ;
  } else {
    /* didn't find any Companionship board on the PCI bus. */
    return -1;
  }

  /* found Companionship PCI board, Set it up: */

	pci_read_config_dword(bus, dev, fun, 0x10, &cc1.bars[0]);
	pci_read_config_dword(bus, dev, fun, 0x14, &cc1.bars[1]);
	pci_read_config_dword(bus, dev, fun, 0x18, &cc1.bars[2]);
	pci_read_config_dword(bus, dev, fun, 0x1c, &cc1.bars[3]);
	pci_read_config_dword(bus, dev, fun, 0x20, &cc1.bars[4]);

#ifdef DEBUG
	for(i=0; i<5; i++){
		printk("PCI: BAR%d: 0x%x\n\r",i,cc1.bars[i]);
	}
#endif

	/* Set up PCI ==> AMBA */
	pcib = (void *)cc1.bars[0];
	pcib->bar0 = 0xfc000000;
/*	pcib->bar1 = 0xff000000;*/
#ifdef BOARD_INFO
	printk("Found CCHIP1 Board at 0x%lx\n\r",(unsigned int)pcib);
#endif

	/* AMBA MAP cc1.bars[1] (in CPU) ==> 0xf0000000(remote amba address) */
	cc1.amba_maps[0].size = 0x04000000;
	cc1.amba_maps[0].local_adr = cc1.bars[1];
	cc1.amba_maps[0].remote_adr = 0xfc000000;

	/* Mark end of table */
	cc1.amba_maps[1].size=0;
	cc1.amba_maps[1].local_adr = 0;
	cc1.amba_maps[1].remote_adr = 0;

	/* Enable I/O and Mem accesses */
	pci_read_config_dword(bus, dev, fun, 0x4, &com1);
	com1 |= 0x3;
	pci_write_config_dword(bus, dev, fun, 0x4, com1);
	pci_read_config_dword(bus, dev, fun, 0x4, &com1);

	/* Set up AMBA Masters ==> PCI */
	ambab = (void *)(cc1.bars[1]+0x400);
#ifdef DEBUG
	printk("AMBA: PCIBAR[%d]: 0x%x, 0x%x\n\r",0,ambab->bar0,pcib->bar0);
	printk("AMBA: PCIBAR[%d]: 0x%x, 0x%x\n\r",1,ambab->bar1,pcib->bar1);
	printk("AMBA: PCIBAR[%d]: 0x%x, 0x%x\n\r",2,ambab->bar2,pcib->bar2);
#endif
	ambab->ambabars[0] = 0x40000000; /* 0xe0000000(AMBA) ==> 0x40000000(PCI) ==> 0x40000000(AT697 AMBA) */

	/* Scan bus for AMBA devices */
	abus = &cc1.amba_bus;
	memset(abus,0,sizeof(*abus));
	ambapp_scan(abus, cc1.bars[1]+0x3f00000, NULL, &cc1.amba_maps[0]);

	/* Init all msters, max 16 */
	for(i=1; i<16; i++) {
		ambab->ambabars[i] = 0x40000000;
		if (READ_REG(&ambab->ambabars[i]) != 0x40000000)
			break;
	}

	/* Enable PCI Master */
   pci_read_config_dword(bus, dev, fun, 0x4, &com1);
   com1 |= 0x4;
   pci_write_config_dword(bus, dev, fun, 0x4, com1);
   pci_read_config_dword(bus, dev, fun, 0x4, &com1);

	cc1.pcib  = pcib;
	cc1.ambab = ambab;
	cc1.bus = bus;
	cc1.dev = dev;
	cc1.fun = fun;

	return 0;
}

#ifndef GAISLER_FTAHBRAM
 #define GAISLER_FTAHBRAM 0x50
#endif
int init_onboard_sram(void){
	struct ambapp_ahb_info ahb;
	struct ambapp_apb_info apb;
	unsigned int conf, size;

	/* Find SRAM controller
	 * 1. AHB slave interface
	 * 2. APB slave interface
	 */
	if ( ambapp_find_apbslv(&cc1.amba_bus, VENDOR_GAISLER, GAISLER_FTAHBRAM,
                                &apb) != 1 ){
		printk("On Board FT SRAM not found (APB)\n");
		return -1;
	}

	if ( ambapp_find_ahbslv(&cc1.amba_bus, VENDOR_GAISLER, GAISLER_FTAHBRAM,
                                &ahb) != 1 ){
		printk("On Board FT SRAM not found (AHB)\n");
		return -1;
	}

	/* We have found the controller.
	 * Get it going.
	 *
	 * Get size of SRAM
	 */
	conf = *(unsigned int *)apb.start;
	size = (conf >>10) & 0x7;

	/* 2^x kb */
	cc1.ftsram_size = 1<<size;
	cc1.ftsram_start = ahb.start[0];
	cc1.ftsram_end = size*1024 + cc1.ftsram_start;
#ifdef BOARD_INFO
	printk("Found FT AHB SRAM %dkb at 0x%lx\n",cc1.ftsram_size,cc1.ftsram_start);
#endif
	return 0;
}

int cchip1_register(void){

	/* Init AT697 PCI Controller */
	init_pci();

	/* Find & init CChip board .
	 * Also scan AMBA Plug&Play info for us.
	 */
	if ( init_pcif() ){
		printk("Failed to initialize CCHIP board\n\r");
		return -1;
	}

	/* Set interrupt common board stuff */
	cchip1_irq_init();

	/* Find on board SRAM */
	if ( init_onboard_sram() ){
		printk("Failed to register On Board SRAM. It is needed by b1553BRM\n");
		return -1;
	}

	/* Register interrupt install functions */
	b1553brm_pci_int_reg = cchip1_set_isr;
	occan_pci_int_reg = cchip1_set_isr;
	grspw_pci_int_reg = cchip1_set_isr;
	apbuart_pci_int_reg = cchip1_set_isr;

	/* register the BRM PCI driver, use 16k FTSRAM... */
	if ( b1553brm_pci_register(&cc1.amba_bus,0,0,3,cc1.ftsram_start,0xffa00000) ){
		printk("Failed to register BRM PCI driver\n");
		return -1;
	}

	/* register the BRM PCI driver, no DMA memory... */
	if ( occan_pci_register(&cc1.amba_bus) ){
		printk("Failed to register OC_CAN PCI driver\n");
		return -1;
	}

	/* register the GRSPW PCI driver, use malloc... */
	if ( grspw_pci_register(&cc1.amba_bus,0,0xe0000000) ){
		printk("Failed to register GRSPW PCI driver\n");
		return -1;
	}

	/* register the APBUART PCI driver, no DMA memory */
	if ( apbuart_pci_register(&cc1.amba_bus) ){
		printk("Failed to register APBUART PCI driver\n");
		return -1;
	}

	return 0;
}

static rtems_isr cchip1_interrupt_dispatcher(rtems_vector_number v);
static unsigned int cchip1_spurious_cnt;

typedef struct {
	unsigned int (*handler)(int irqno, void *arg);
	void *arg;
} int_handler;

static int_handler int_handlers[16];

void cchip1_irq_init(void){

	/* Configure AT697 ioport bit 7 to input pci irq */
  regs->PIO_Direction &= ~(1<<7);
  regs->PIO_Interrupt  = 0x87;          /* level sensitive */

  /* Set up irq controller (mask all IRQs) */
	cc1.pcib->imask = 0x0000;
	cc1.pcib->ipend = 0;
	cc1.pcib->iclear = 0xffff;
	cc1.pcib->iforce = 0;
	cc1.pcib->ilevel = 0x0;

	memset(int_handlers,0,sizeof(int_handlers));

	/* Reset spurious counter */
	cchip1_spurious_cnt = 0;

	/* Register interrupt handler */
	set_vector(cchip1_interrupt_dispatcher,LEON_TRAP_TYPE(CCHIP_IRQ),1);
}

void cchip1_set_isr(void *handler, int irqno, void *arg){
	int_handlers[irqno].handler = handler;
	int_handlers[irqno].arg = arg;
#ifdef DEBUG
	printk("Registering IRQ %d to 0x%lx(%d,0x%lx)\n\r",irqno,(unsigned int)handler,irqno,(unsigned int)arg);
#endif
	cc1.pcib->imask |= 1<<irqno; /* Enable the registered IRQ */
}

static rtems_isr cchip1_interrupt_dispatcher(rtems_vector_number v){
	unsigned int pending = READ_REG(&cc1.pcib->ipend);
	unsigned int (*handler)(int irqno, void *arg);
	unsigned int clr = pending;
	int irq=1;

	if ( !pending ){
#ifdef PRINT_SPURIOUS
		printk("Spurious IRQ %d: %d\n",v,cchip1_spurious_cnt);
#endif
		cchip1_spurious_cnt++;
		return;
	}
#ifdef DEBUG_IRQS
  printk("CCIRQ: 0x%x\n",(unsigned int)pending);
#endif
	/* IRQ 0 doesn't exist */
	irq=1;
	pending = pending>>1;

	while ( pending ){
		if ( (pending & 1) && (handler=int_handlers[irq].handler) ){
			handler(irq,int_handlers[irq].arg);
		}
		irq++;
		pending = pending>>1;
	}

	cc1.pcib->iclear = clr;

	/*LEON_Clear_interrupt( brd->irq );*/
}
