/* irq_init.c
 *
 *  This file contains the implementation of rtems initialization
 *  related to interrupt handling.
 *
 *  Copyright (c) 2009 embedded brains GmbH
 *  CopyRight (C) 1998 valette@crf.canon.fr
 *
 *  COPYRIGHT (c) 2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/bspIo.h>

#include <libcpu/cpu.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>

/*
 * rtems prologue generated in irq_asm.S
 */
extern void rtems_irq_prologue_0(void);
extern void rtems_irq_prologue_1(void);
extern void rtems_irq_prologue_2(void);
extern void rtems_irq_prologue_3(void);
extern void rtems_irq_prologue_4(void);
extern void rtems_irq_prologue_5(void);
extern void rtems_irq_prologue_6(void);
extern void rtems_irq_prologue_7(void);
extern void rtems_irq_prologue_8(void);
extern void rtems_irq_prologue_9(void);
extern void rtems_irq_prologue_10(void);
extern void rtems_irq_prologue_11(void);
extern void rtems_irq_prologue_12(void);
extern void rtems_irq_prologue_13(void);
extern void rtems_irq_prologue_14(void);
extern void rtems_irq_prologue_15(void);
extern void rtems_irq_prologue_16(void);
/*
 * default vectors
 */
extern void default_raw_idt_handler(void);

/*
 * default raw on/off function
 */
static void raw_nop_func(const struct __rtems_raw_irq_connect_data__ *unused)
{
}

/*
 * default raw isOn function
 */
static int raw_not_connected(
  const struct __rtems_raw_irq_connect_data__ *unused
)
{
  return 0;
}

static rtems_raw_irq_connect_data 	idtHdl[IDT_SIZE];

static rtems_raw_irq_hdl rtemsIrq[BSP_IRQ_LINES_NUMBER] = {
  rtems_irq_prologue_0,
  rtems_irq_prologue_1,
  rtems_irq_prologue_2,
  rtems_irq_prologue_3,
  rtems_irq_prologue_4,
  rtems_irq_prologue_5,
  rtems_irq_prologue_6,
  rtems_irq_prologue_7,
  rtems_irq_prologue_8,
  rtems_irq_prologue_9,
  rtems_irq_prologue_10,
  rtems_irq_prologue_11,
  rtems_irq_prologue_12,
  rtems_irq_prologue_13,
  rtems_irq_prologue_14,
  rtems_irq_prologue_15,
  rtems_irq_prologue_16,
};

static rtems_raw_irq_connect_data 	defaultRawIrq = {
  0,                       /* vectorIdex */
  default_raw_idt_handler, /* hdl */
  raw_nop_func,            /* on */
  raw_nop_func,            /* off */
  raw_not_connected        /* isOn */
};

static interrupt_gate_descriptor	idtEntry;

static rtems_raw_irq_global_settings raw_initial_config;

void raw_idt_notify(void)
{
  printk("raw_idt_notify has been called \n");
}

void  rtems_irq_mngt_init(void)
{
    int 			i;
    interrupt_gate_descriptor* 	idt_entry_tbl;
    unsigned int                limit;
    rtems_interrupt_level       level;

    i386_get_info_from_IDTR(&idt_entry_tbl, &limit);

    /* Convert into number of entries */
    limit = (limit + 1)/sizeof(interrupt_gate_descriptor);

    if(limit != IDT_SIZE) {
       printk("IDT table size mismatch !!! System locked\n");
       while(1);
    }

    rtems_interrupt_disable(level);

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
      create_interrupt_gate_descriptor(&idtEntry, rtemsIrq[i]);
      idt_entry_tbl[i + BSP_ASM_IRQ_VECTOR_BASE] = idtEntry;
    }
    /*
     * At this point we have completed the initialization of IDT
     * with raw handlers.  We must now initialize the higher level
     * interrupt management.
     */

    /*
     * Init initial Interrupt management config
     */
    if (bsp_interrupt_initialize() != RTEMS_SUCCESSFUL) {
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
