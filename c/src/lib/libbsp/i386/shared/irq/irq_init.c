/* irq_init.c
 *
 *  This file contains the implementation of rtems initialization
 *  related to interrupt handling.
 *
 *  CopyRight (C) 1998 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <libcpu/cpu.h>
#include <irq.h>
#include <bsp.h>
#include <rtems/bspIo.h>

/*
 * rtems prologue generated in irq_asm.S
 */
extern void rtems_irq_prologue_0();
extern void rtems_irq_prologue_1();
extern void rtems_irq_prologue_2();
extern void rtems_irq_prologue_3();
extern void rtems_irq_prologue_4();
extern void rtems_irq_prologue_5();
extern void rtems_irq_prologue_6();
extern void rtems_irq_prologue_7();
extern void rtems_irq_prologue_8();
extern void rtems_irq_prologue_9();
extern void rtems_irq_prologue_10();
extern void rtems_irq_prologue_11();
extern void rtems_irq_prologue_12();
extern void rtems_irq_prologue_13();
extern void rtems_irq_prologue_14();
extern void rtems_irq_prologue_15();
/*
 * default idt vector
 */
extern void default_raw_idt_handler();
/*
 * default on/off function
 */
static void nop_func(){}
/*
 * default isOn function
 */
static int not_connected() {return 0;}


static rtems_raw_irq_connect_data 	idtHdl[IDT_SIZE];

/*
 * Table used to store rtems managed interrupt handlers.
 * Borrow the table to store raw handler entries at the beginning.
 * The table will be reinitialized before the call to BSP_rtems_irq_mngt_set().
 */
static rtems_irq_connect_data     	rtemsIrq[BSP_IRQ_LINES_NUMBER] = {
  {0,(rtems_irq_hdl)rtems_irq_prologue_0},
  {0,(rtems_irq_hdl)rtems_irq_prologue_1},
  {0,(rtems_irq_hdl)rtems_irq_prologue_2},
  {0,(rtems_irq_hdl)rtems_irq_prologue_3},
  {0,(rtems_irq_hdl)rtems_irq_prologue_4},
  {0,(rtems_irq_hdl)rtems_irq_prologue_5},
  {0,(rtems_irq_hdl)rtems_irq_prologue_6},
  {0,(rtems_irq_hdl)rtems_irq_prologue_7},
  {0,(rtems_irq_hdl)rtems_irq_prologue_8},
  {0,(rtems_irq_hdl)rtems_irq_prologue_9},
  {0,(rtems_irq_hdl)rtems_irq_prologue_10},
  {0,(rtems_irq_hdl)rtems_irq_prologue_11},
  {0,(rtems_irq_hdl)rtems_irq_prologue_12},
  {0,(rtems_irq_hdl)rtems_irq_prologue_13},
  {0,(rtems_irq_hdl)rtems_irq_prologue_14},
  {0,(rtems_irq_hdl)rtems_irq_prologue_15}
};

static rtems_raw_irq_connect_data 	defaultRawIrq = {
  /* vectorIdex,	 hdl			      , on	, off 		, isOn */
  0, 			default_raw_idt_handler ,nop_func	, nop_func,	not_connected
};

static rtems_irq_connect_data     	defaultIrq = {
  /* vectorIdex,	 hdl		, on		, off		, isOn */
  0, 			 nop_func	, nop_func	, nop_func	, not_connected
};

static rtems_irq_prio irqPrioTable[BSP_IRQ_LINES_NUMBER]={
  /*
   * actual rpiorities for interrupt :
   *	0   means that only current interrupt is masked
   *	255 means all other interrupts are masked
   * The second entry has a priority of 255 because
   * it is the slave pic entry and is should always remain
   * unmasked.
   */
  0,0,
  255,
  0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};
    

    
static interrupt_gate_descriptor	idtEntry;

static rtems_irq_global_settings     initial_config;
static rtems_raw_irq_global_settings raw_initial_config;

void raw_idt_notify()
{
  printk("raw_idt_notify has been called \n");
}

void  rtems_irq_mngt_init()
{
    int 			i;
    interrupt_gate_descriptor* 	idt_entry_tbl;
    unsigned int                limit;
    unsigned int 		level;

    i386_get_info_from_IDTR(&idt_entry_tbl, &limit);

    /* Convert into number of entries */
    limit = (limit + 1)/sizeof(interrupt_gate_descriptor);

    if(limit != IDT_SIZE) {
       printk("IDT table size mismatch !!! System locked\n");
       while(1);
    }
    

    _CPU_ISR_Disable(level);

    /*
     * Init the complete IDT vector table with defaultRawIrq value
     */
    for (i = 0; i < IDT_SIZE ; i++) {
      idtHdl[i] 	 = defaultRawIrq;
      idtHdl[i].idtIndex = i;
    }

    raw_initial_config.idtSize = IDT_SIZE;
    raw_initial_config.defaultRawEntry = defaultRawIrq;
    raw_initial_config.rawIrqHdlTbl = idtHdl;
    
    if (!i386_init_idt (&raw_initial_config)) {
      /*
       * put something here that will show the failure...
       */
      printk("Unable to initialize IDT!!! System locked\n");
      while (1);
    }
    /*
     * Patch the entry that will be used by RTEMS for interrupt management
     * with RTEMS prologue.
     */
    for (i = 0; i < BSP_IRQ_LINES_NUMBER; i++) {
      create_interrupt_gate_descriptor(&idtEntry,(rtems_raw_irq_hdl) rtemsIrq[i].hdl);
      idt_entry_tbl[i + BSP_ASM_IRQ_VECTOR_BASE] = idtEntry;
    }
    /*
     * At this point we have completed the initialization of IDT
     * with raw handlers.  We must now initialize the higher level
     * interrupt management.
     */
    /*
     * re-init the rtemsIrq table
     */
    for (i = 0; i < BSP_IRQ_LINES_NUMBER; i++) {
      rtemsIrq[i]      = defaultIrq;
      rtemsIrq[i].name = i;
    }
    /*
     * Init initial Interrupt management config
     */
    initial_config.irqNb 	= BSP_IRQ_LINES_NUMBER;
    initial_config.defaultEntry = defaultIrq;
    initial_config.irqHdlTbl	= rtemsIrq;
    initial_config.irqBase	= BSP_ASM_IRQ_VECTOR_BASE;
    initial_config.irqPrioTbl	= irqPrioTable;

    if (!BSP_rtems_irq_mngt_set(&initial_config)) {
      /*
       * put something here that will show the failure...
       */
      printk("Unable to initialize RTEMS interrupt Management!!! System locked\n");
      while (1);
    }

    /*
     * #define DEBUG    
     */
#ifdef DEBUG
    {
      /*
       * following adresses should be the same
       */
      unsigned tmp;

      printk("idt_entry_tbl =  %x Interrupt_descriptor_table addr = %x\n",
	     idt_entry_tbl, &Interrupt_descriptor_table);
      tmp = (unsigned) get_hdl_from_vector (BSP_ASM_IRQ_VECTOR_BASE + BSP_PERIODIC_TIMER);
      printk("clock isr address from idt = %x should be %x\n",
	     tmp, (unsigned) rtems_irq_prologue_0);
    }
    printk("i8259s_cache = %x\n", * (unsigned short*) &i8259s_cache);
    BSP_wait_polled_input();
#endif    
}
