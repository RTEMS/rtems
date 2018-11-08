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
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/score/cpu.h>
#include <bsp/irq.h>
#include <bsp/tblsizes.h>

/*
 * This locking is not enough if IDT is changed at runtime
 * and entry can be changed for vector which is enabled
 * at change time. But such use is broken anyway.
 * Protect code only against concurrent changes.
 * Even that is probably unnecessary if different
 * entries are changed concurrently.
 */
RTEMS_INTERRUPT_LOCK_DEFINE( static, rtems_idt_access_lock, "rtems_idt_access_lock" );

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
    rtems_interrupt_lock_context lock_context;

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

    rtems_interrupt_lock_acquire(&rtems_idt_access_lock, &lock_context);

    raw_irq_table [irq->idtIndex] = *irq;
    create_interrupt_gate_descriptor (&idt_entry_tbl[irq->idtIndex], irq->hdl);
    if (irq->on)
      irq->on(irq);

    rtems_interrupt_lock_release(&rtems_idt_access_lock, &lock_context);
    return 1;
}

void _CPU_ISR_install_vector (uint32_t vector,
			      CPU_ISR_handler hdl,
			      CPU_ISR_handler * oldHdl)
{
    interrupt_gate_descriptor* 	idt_entry_tbl;
    unsigned			limit;
    interrupt_gate_descriptor	new;
    rtems_interrupt_lock_context lock_context;

    i386_get_info_from_IDTR (&idt_entry_tbl, &limit);

    /* Convert limit into number of entries */
    limit = (limit + 1) / sizeof(interrupt_gate_descriptor);

    if (vector >= limit) {
      return;
    }
    rtems_interrupt_lock_acquire(&rtems_idt_access_lock, &lock_context);
    *oldHdl = (CPU_ISR_handler) (idt_entry_tbl[vector].low_offsets_bits |
	(idt_entry_tbl[vector].high_offsets_bits << 16));

    create_interrupt_gate_descriptor(&new,  hdl);
    idt_entry_tbl[vector] = new;

    rtems_interrupt_lock_release(&rtems_idt_access_lock, &lock_context);
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
    rtems_interrupt_lock_context lock_context;

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
    rtems_interrupt_lock_acquire(&rtems_idt_access_lock, &lock_context);

    idt_entry_tbl[irq->idtIndex] = default_idt_entry;

    if (irq->off)
      irq->off(irq);

    raw_irq_table[irq->idtIndex] = default_raw_irq_entry;
    raw_irq_table[irq->idtIndex].idtIndex = irq->idtIndex;

    rtems_interrupt_lock_release(&rtems_idt_access_lock, &lock_context);

    return 1;
}

/*
 * Caution this function assumes the IDTR has been already set.
 */
int i386_init_idt (rtems_raw_irq_global_settings* config)
{
    unsigned			limit;
    unsigned 			i;
    rtems_interrupt_lock_context lock_context;
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

    rtems_interrupt_lock_acquire(&rtems_idt_access_lock, &lock_context);

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
    rtems_interrupt_lock_release(&rtems_idt_access_lock, &lock_context);

    return 1;
}

int i386_get_idt_config (rtems_raw_irq_global_settings** config)
{
  *config = local_settings;
  return 1;
}

uint32_t i386_raw_gdt_entry (uint16_t segment_selector_index,
                             segment_descriptors* sd)
{
    uint16_t                gdt_limit;
    uint16_t                tmp_segment = 0;
    segment_descriptors*    gdt_entry_tbl;
    uint8_t                 present;

    i386_get_info_from_GDTR (&gdt_entry_tbl, &gdt_limit);

    if (segment_selector_index >= (gdt_limit+1)/8) {
      /* index to GDT table out of bounds */
      return 0;
    }
    if (segment_selector_index == 0) {
      /* index 0 is not usable */
      return 0;
    }

    /* put prepared descriptor into the GDT */
    present = sd->present;
    sd->present = 0;
    gdt_entry_tbl[segment_selector_index].present = 0;
    RTEMS_COMPILER_MEMORY_BARRIER();
    gdt_entry_tbl[segment_selector_index] = *sd;
    RTEMS_COMPILER_MEMORY_BARRIER();
    gdt_entry_tbl[segment_selector_index].present = present;
    sd->present = present;
    /*
     * Now, reload all segment registers so that the possible changes takes effect.
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

void i386_fill_segment_desc_base(uint32_t base,
                                 segment_descriptors* sd)
{
    sd->base_address_15_0  = base & 0xffff;
    sd->base_address_23_16 = (base >> 16) & 0xff;
    sd->base_address_31_24 = (base >> 24) & 0xff;
}

void i386_fill_segment_desc_limit(uint32_t limit,
                                  segment_descriptors* sd)
{
    sd->granularity = 0;
    if (limit > 65535) {
      sd->granularity = 1;
      limit /= 4096;
    }
    sd->limit_15_0  = limit & 0xffff;
    sd->limit_19_16 = (limit >> 16) & 0xf;
}

/*
 * Caution this function assumes the GDTR has been already set.
 */
uint32_t i386_set_gdt_entry (uint16_t segment_selector_index, uint32_t base,
                             uint32_t limit)
{
    segment_descriptors     gdt_entry;
    memset(&gdt_entry, 0, sizeof(gdt_entry));

    i386_fill_segment_desc_limit(limit, &gdt_entry);
    i386_fill_segment_desc_base(base, &gdt_entry);
    /*
     * set up descriptor type (this may well becomes a parameter if needed)
     */
    gdt_entry.type              = 2;    /* Data R/W */
    gdt_entry.descriptor_type   = 1;    /* Code or Data */
    gdt_entry.privilege         = 0;    /* ring 0 */
    gdt_entry.present           = 1;    /* not present */

    /*
     * Now, reload all segment registers so the limit takes effect.
     */
    return i386_raw_gdt_entry(segment_selector_index, &gdt_entry);
}

uint16_t i386_next_empty_gdt_entry ()
{
    uint16_t                gdt_limit;
    segment_descriptors*    gdt_entry_tbl;
    /* initial amount of filled descriptors */
    static uint16_t         segment_selector_index = NUM_SYSTEM_GDT_DESCRIPTORS - 1;

    segment_selector_index += 1;
    i386_get_info_from_GDTR (&gdt_entry_tbl, &gdt_limit);
    if (segment_selector_index >= (gdt_limit+1)/8) {
      return 0;
    }
    return segment_selector_index;
}

uint16_t i386_cpy_gdt_entry(uint16_t segment_selector_index,
                            segment_descriptors* struct_to_fill)
{
    uint16_t                gdt_limit;
    segment_descriptors*    gdt_entry_tbl;

    i386_get_info_from_GDTR (&gdt_entry_tbl, &gdt_limit);

    if (segment_selector_index >= (gdt_limit+1)/8) {
      return 0;
    }

    *struct_to_fill = gdt_entry_tbl[segment_selector_index];
    return segment_selector_index;
}

segment_descriptors* i386_get_gdt_entry(uint16_t segment_selector_index)
{
    uint16_t                gdt_limit;
    segment_descriptors*    gdt_entry_tbl;

    i386_get_info_from_GDTR (&gdt_entry_tbl, &gdt_limit);

    if (segment_selector_index >= (gdt_limit+1)/8) {
      return 0;
    }
    return &gdt_entry_tbl[segment_selector_index];
}

uint32_t i386_limit_gdt_entry(segment_descriptors* gdt_entry)
{
    uint32_t lim = (gdt_entry->limit_15_0 + (gdt_entry->limit_19_16<<16));
    if (gdt_entry->granularity) {
      return lim*4096+4095;
    }
    return lim;
}
