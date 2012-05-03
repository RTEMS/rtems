/*
 * cpu.c  - This file contains implementation of C function to
 *          instantiate IDT entries. More detailled information can be found
 *	    on Intel site and more precisely in the following book :
 *
 *		Pentium Processor family
 *		Developper's Manual
 *
 *		Volume 3 : Architecture and Programming Manual
 *
 * Copyright (C) 1998  Eric Valette (valette@crf.canon.fr)
 *                     Canon Centre Recherche France.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <libcpu/cpu.h>
#include <bsp/irq.h>

static rtems_raw_irq_connect_data* 	raw_irq_table;
static rtems_raw_irq_connect_data  	default_raw_irq_entry;
static interrupt_gate_descriptor   	default_idt_entry;
static rtems_raw_irq_global_settings* 	local_settings;

void create_interrupt_gate_descriptor (interrupt_gate_descriptor* idtEntry,
				       rtems_raw_irq_hdl hdl)
{
    idtEntry->low_offsets_bits	= (((unsigned) hdl) & 0xffff);
    idtEntry->segment_selector	= i386_get_cs();
    idtEntry->fixed_value_bits	= 0;
    idtEntry->gate_type		= 0xe;
    idtEntry->privilege		= 0;
    idtEntry->present		= 1;
    idtEntry->high_offsets_bits	= ((((unsigned) hdl) >> 16) & 0xffff);
}

rtems_raw_irq_hdl get_hdl_from_vector(rtems_vector_offset index)
{
    uint32_t                    hdl;
    interrupt_gate_descriptor* 	idt_entry_tbl;
    unsigned                    limit;

    i386_get_info_from_IDTR (&idt_entry_tbl, &limit);

    /* Convert limit into number of entries */
    limit = (limit + 1) / sizeof(interrupt_gate_descriptor);

    if(index >= limit) {
        return 0;
    }

    hdl = (idt_entry_tbl[index].low_offsets_bits |
          (idt_entry_tbl[index].high_offsets_bits << 16));
    return (rtems_raw_irq_hdl) hdl;
}

int i386_set_idt_entry  (const rtems_raw_irq_connect_data* irq)
{
    interrupt_gate_descriptor* 	idt_entry_tbl;
    unsigned			limit;
    rtems_interrupt_level       level;

    i386_get_info_from_IDTR (&idt_entry_tbl, &limit);

    /* Convert limit into number of entries */
    limit = (limit + 1) / sizeof(interrupt_gate_descriptor);

    if (irq->idtIndex >= limit) {
      return 0;
    }
    /*
     * Check if default handler is actually connected. If not issue an error.
     * You must first get the current handler via i386_get_current_idt_entry
     * and then disconnect it using i386_delete_idt_entry.
     * RATIONALE : to always have the same transition by forcing the user
     * to get the previous handler before accepting to disconnect.
     */
    if (get_hdl_from_vector(irq->idtIndex) != default_raw_irq_entry.hdl) {
      return 0;
    }

    rtems_interrupt_disable(level);

    raw_irq_table [irq->idtIndex] = *irq;
    create_interrupt_gate_descriptor (&idt_entry_tbl[irq->idtIndex], irq->hdl);
    if (irq->on)
      irq->on(irq);

    rtems_interrupt_enable(level);
    return 1;
}

void _CPU_ISR_install_vector (uint32_t vector,
			      proc_ptr hdl,
			      proc_ptr * oldHdl)
{
    interrupt_gate_descriptor* 	idt_entry_tbl;
    unsigned			limit;
    interrupt_gate_descriptor	new;
    rtems_interrupt_level       level;

    i386_get_info_from_IDTR (&idt_entry_tbl, &limit);

    /* Convert limit into number of entries */
    limit = (limit + 1) / sizeof(interrupt_gate_descriptor);

    if (vector >= limit) {
      return;
    }
    rtems_interrupt_disable(level);
    * ((unsigned int *) oldHdl) = idt_entry_tbl[vector].low_offsets_bits |
	(idt_entry_tbl[vector].high_offsets_bits << 16);

    create_interrupt_gate_descriptor(&new,  hdl);
    idt_entry_tbl[vector] = new;

    rtems_interrupt_enable(level);
}

int i386_get_current_idt_entry (rtems_raw_irq_connect_data* irq)
{
    interrupt_gate_descriptor* 	idt_entry_tbl;
    unsigned			limit;

    i386_get_info_from_IDTR (&idt_entry_tbl, &limit);

    /* Convert limit into number of entries */
    limit = (limit + 1) / sizeof(interrupt_gate_descriptor);

    if (irq->idtIndex >= limit) {
      return 0;
    }
    raw_irq_table [irq->idtIndex].hdl = get_hdl_from_vector(irq->idtIndex);

    *irq = raw_irq_table [irq->idtIndex];

    return 1;
}

int i386_delete_idt_entry (const rtems_raw_irq_connect_data* irq)
{
    interrupt_gate_descriptor* 	idt_entry_tbl;
    unsigned			limit;
    rtems_interrupt_level       level;

    i386_get_info_from_IDTR (&idt_entry_tbl, &limit);

    /* Convert limit into number of entries */
    limit = (limit + 1) / sizeof(interrupt_gate_descriptor);

    if (irq->idtIndex >= limit) {
      return 0;
    }
    /*
     * Check if handler passed is actually connected. If not issue an error.
     * You must first get the current handler via i386_get_current_idt_entry
     * and then disconnect it using i386_delete_idt_entry.
     * RATIONALE : to always have the same transition by forcing the user
     * to get the previous handler before accepting to disconnect.
     */
    if (get_hdl_from_vector(irq->idtIndex) != irq->hdl){
      return 0;
    }
    rtems_interrupt_disable(level);

    idt_entry_tbl[irq->idtIndex] = default_idt_entry;

    if (irq->off)
      irq->off(irq);

    raw_irq_table[irq->idtIndex] = default_raw_irq_entry;
    raw_irq_table[irq->idtIndex].idtIndex = irq->idtIndex;

    rtems_interrupt_enable(level);

    return 1;
}

/*
 * Caution this function assumes the IDTR has been already set.
 */
int i386_init_idt (rtems_raw_irq_global_settings* config)
{
    unsigned			limit;
    unsigned 			i;
    rtems_interrupt_level       level;
    interrupt_gate_descriptor*	idt_entry_tbl;

    i386_get_info_from_IDTR (&idt_entry_tbl, &limit);

    /* Convert limit into number of entries */
    limit = (limit + 1) / sizeof(interrupt_gate_descriptor);

    if (config->idtSize != limit) {
      return 0;
    }
    /*
     * store various accelarators
     */
    raw_irq_table 		= config->rawIrqHdlTbl;
    local_settings 		= config;
    default_raw_irq_entry 	= config->defaultRawEntry;

    rtems_interrupt_disable(level);

    create_interrupt_gate_descriptor (&default_idt_entry, default_raw_irq_entry.hdl);

    for (i=0; i < limit; i++) {
      interrupt_gate_descriptor new;
      create_interrupt_gate_descriptor (&new, raw_irq_table[i].hdl);
      idt_entry_tbl[i] = new;
      if (raw_irq_table[i].hdl != default_raw_irq_entry.hdl) {
	raw_irq_table[i].on(&raw_irq_table[i]);
      }
      else {
	raw_irq_table[i].off(&raw_irq_table[i]);
      }
    }
    rtems_interrupt_enable(level);

    return 1;
}

int i386_get_idt_config (rtems_raw_irq_global_settings** config)
{
  *config = local_settings;
  return 1;
}

/*
 * Caution this function assumes the GDTR has been already set.
 */
int i386_set_gdt_entry (unsigned short segment_selector, unsigned base,
			unsigned limit)
{
    unsigned 			gdt_limit;
    unsigned short              tmp_segment = 0;
    unsigned int                limit_adjusted;
    segment_descriptors* 	gdt_entry_tbl;

    i386_get_info_from_GDTR (&gdt_entry_tbl, &gdt_limit);

    if (segment_selector > limit) {
      return 0;
    }
    /*
     * set up limit first
     */
    limit_adjusted = limit;
    if ( limit > 4095 ) {
      gdt_entry_tbl[segment_selector].granularity = 1;
      limit_adjusted /= 4096;
    }
    gdt_entry_tbl[segment_selector].limit_15_0  = limit_adjusted & 0xffff;
    gdt_entry_tbl[segment_selector].limit_19_16 = (limit_adjusted >> 16) & 0xf;
    /*
     * set up base
     */
    gdt_entry_tbl[segment_selector].base_address_15_0  = base & 0xffff;
    gdt_entry_tbl[segment_selector].base_address_23_16 = (base >> 16) & 0xff;
    gdt_entry_tbl[segment_selector].base_address_31_24 = (base >> 24) & 0xff;
    /*
     * set up descriptor type (this may well becomes a parameter if needed)
     */
    gdt_entry_tbl[segment_selector].type 		= 2;   	/* Data R/W */
    gdt_entry_tbl[segment_selector].descriptor_type 	= 1;	/* Code or Data */
    gdt_entry_tbl[segment_selector].privilege 		= 0; 	/* ring 0 */
    gdt_entry_tbl[segment_selector].present 		= 1; 	/* not present */

    /*
     * Now, reload all segment registers so the limit takes effect.
     */

    __asm__ volatile( "movw %%ds,%0 ; movw %0,%%ds\n\t"
                  "movw %%es,%0 ; movw %0,%%es\n\t"
                  "movw %%fs,%0 ; movw %0,%%fs\n\t"
                  "movw %%gs,%0 ; movw %0,%%gs\n\t"
                  "movw %%ss,%0 ; movw %0,%%ss"
                   : "=r" (tmp_segment)
                   : "0"  (tmp_segment)
		  );

    return 1;
}
