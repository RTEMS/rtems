/*
 * cpu.h  - This file contains definitions for data structure related 
 *          to Intel system programming. More information can be found
 *	    on Intel site and more precisely in the following book :
 *
 *		Pentium Processor familly
 *		Developper's Manual
 *
 *		Volume 3 : Architecture and Programming Manual
 *
 * Copyright (C) 1998  Eric Valette (valette@crf.canon.fr)
 *                     Canon Centre Recherche France.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 * $Id$
 */

#ifndef _LIBCPU_i386_CPU_H
#define _LIBCPU_i386_CPU_H

#include <libcpu/registers.h>


#ifndef ASM

/*
 *  Interrupt Level Macros
 */

#define i386_disable_interrupts( _level ) \
  { \
    asm volatile ( "pushf ; \
                    cli ; \
                    pop %0" \
                   : "=rm" ((_level)) \
    ); \
  }

#define i386_enable_interrupts( _level )  \
  { \
    asm volatile ( "push %0 ; \
                    popf" \
                    : : "rm" ((_level)) : "cc" \
    ); \
  }

#define i386_flash_interrupts( _level ) \
  { \
    asm volatile ( "push %0 ; \
                    popf ; \
                    cli" \
                    : : "rm" ((_level)) : "cc" \
    ); \
  }

#define i386_get_interrupt_level( _level ) \
  do { \
    register unsigned32 _eflags; \
    \
    asm volatile ( "pushf ; \
                    pop %0" \
                    : "=rm" ((_eflags)) \
    ); \
    \
    _level = (_eflags & EFLAGS_INTR_ENABLE) ? 0 : 1; \
  } while (0)

#define _CPU_ISR_Disable( _level ) i386_disable_interrupts( _level )
#define _CPU_ISR_Enable( _level ) i386_enable_interrupts( _level )
     
/*
 *  Segment Access Routines
 *
 *  NOTE:  Unfortunately, these are still static inlines even when the
 *         "macro" implementation of the generic code is used.
 */

static inline unsigned short i386_get_cs()
{
  register unsigned short segment = 0;

  asm volatile ( "movw %%cs,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

static inline unsigned short i386_get_ds()
{
  register unsigned short segment = 0;

  asm volatile ( "movw %%ds,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

static inline unsigned short i386_get_es()
{
  register unsigned short segment = 0;

  asm volatile ( "movw %%es,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

static inline unsigned short i386_get_ss()
{
  register unsigned short segment = 0;

  asm volatile ( "movw %%ss,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

static inline unsigned short i386_get_fs()
{
  register unsigned short segment = 0;

  asm volatile ( "movw %%fs,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

static inline unsigned short i386_get_gs()
{
  register unsigned short segment = 0;

  asm volatile ( "movw %%gs,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

/*
 * Added for pagination management
 */

static inline unsigned int i386_get_cr0()
{
  register unsigned int segment = 0;

  asm volatile ( "movl %%cr0,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

static inline void i386_set_cr0(unsigned int segment)
{
  asm volatile ( "movl %0,%%cr0" : "=r" (segment) : "0" (segment) );
}

static inline unsigned int i386_get_cr2()
{
  register unsigned int segment = 0;

  asm volatile ( "movl %%cr2,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

static inline unsigned int i386_get_cr3()
{
  register unsigned int segment = 0;

  asm volatile ( "movl %%cr3,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

static inline void i386_set_cr3(unsigned int segment)
{
  asm volatile ( "movl %0,%%cr3" : "=r" (segment) : "0" (segment) );
}

/*
 *  IO Port Access Routines
 */

#define i386_outport_byte( _port, _value ) \
do { register unsigned short __port  = _port; \
     register unsigned char  __value = _value; \
     \
     asm volatile ( "outb %0,%1" : : "a" (__value), "d" (__port) ); \
   } while (0)

#define i386_outport_word( _port, _value ) \
do { register unsigned short __port  = _port; \
     register unsigned short __value = _value; \
     \
     asm volatile ( "outw %0,%1" : : "a" (__value), "d" (__port) ); \
   } while (0)

#define i386_outport_long( _port, _value ) \
do { register unsigned short __port  = _port; \
     register unsigned int  __value = _value; \
     \
     asm volatile ( "outl %0,%1" : : "a" (__value), "d" (__port) ); \
   } while (0)

#define i386_inport_byte( _port, _value ) \
do { register unsigned short __port  = _port; \
     register unsigned char  __value = 0; \
     \
     asm volatile ( "inb %1,%0" : "=a" (__value) \
                                : "d"  (__port) \
                  ); \
     _value = __value; \
   } while (0)

#define i386_inport_word( _port, _value ) \
do { register unsigned short __port  = _port; \
     register unsigned short __value = 0; \
     \
     asm volatile ( "inw %1,%0" : "=a" (__value) \
                                : "d"  (__port) \
                  ); \
     _value = __value; \
   } while (0)

#define i386_inport_long( _port, _value ) \
do { register unsigned short __port  = _port; \
     register unsigned int  __value = 0; \
     \
     asm volatile ( "inl %1,%0" : "=a" (__value) \
                                : "d"  (__port) \
                  ); \
     _value = __value; \
   } while (0)

/*
 * Type definition for raw interrupts. 
 */

typedef unsigned char  rtems_vector_offset;

struct 	__rtems_raw_irq_connect_data__;

typedef void (*rtems_raw_irq_hdl)		(void);
typedef void (*rtems_raw_irq_enable)		(const struct __rtems_raw_irq_connect_data__*);
typedef void (*rtems_raw_irq_disable)		(const struct __rtems_raw_irq_connect_data__*);
typedef int  (*rtems_raw_irq_is_enabled)	(const struct __rtems_raw_irq_connect_data__*);

typedef struct __rtems_raw_irq_connect_data__{
 /*
  * IDT vector offset (IRQ line + PC386_IRQ_VECTOR_BASE)
  */
  rtems_vector_offset		idtIndex;
  /*
   * IDT raw handler. See comment on handler properties below in function prototype.
   */
  rtems_raw_irq_hdl	   	hdl;
  /*
   * function for enabling raw interrupts. In order to be consistent
   * with the fact that the raw connexion can defined in the
   * libcpu library, this library should have no knowledge of
   * board specific hardware to manage interrupts and thus the
   * "on" routine must enable the irq both at device and PIC level.
   * 
   */
    rtems_raw_irq_enable	on;	
  /*
   * function for disabling raw interrupts. In order to be consistent
   * with the fact that the raw connexion can defined in the
   * libcpu library, this library should have no knowledge of
   * board specific hardware to manage interrupts and thus the
   * "on" routine must disable the irq both at device and PIC level.
   * 
   */
  rtems_raw_irq_disable		off;
  /*
   * function enabling to know what interrupt may currently occur
   */
  rtems_raw_irq_is_enabled	isOn;
}rtems_raw_irq_connect_data;

typedef struct {
  /*
   * size of all the table fields (*Tbl) described below.
   */
  unsigned int	 		idtSize;
  /*
   * Default handler used when disconnecting interrupts.
   */
  rtems_raw_irq_connect_data	defaultRawEntry;
  /*
   * Table containing initials/current value.
   */
  rtems_raw_irq_connect_data*	rawIrqHdlTbl;
}rtems_raw_irq_global_settings;

/*
 * See page 14.9 Figure 14-2.
 *
 */
typedef struct {
  unsigned int low_offsets_bits : 16;
  unsigned int segment_selector : 16;
  unsigned int fixed_value_bits : 8;
  unsigned int gate_type	: 5;
  unsigned int privilege	: 2;
  unsigned int present		: 1;
  unsigned int high_offsets_bits: 16;
}interrupt_gate_descriptor;


/*
 * C callable function enabling to create a interrupt_gate_descriptor
 */
void create_interrupt_gate_descriptor (interrupt_gate_descriptor*, rtems_raw_irq_hdl);

/*
 * C callable function enabling to get handler currently connected to a vector
 * 
 */
rtems_raw_irq_hdl get_hdl_from_vector(rtems_vector_offset);

/*
 * C callable function enabling to get easilly usable info from
 * the actual value of IDT register.
 */
extern void i386_get_info_from_IDTR (interrupt_gate_descriptor** table,
				unsigned* limit);
/*
 * C callable function enabling to change the value of IDT register. Must be called
 * with interrupts masked at processor level!!!.
 */
extern void i386_set_IDTR (interrupt_gate_descriptor* table,
		      unsigned limit);

/*
 * C callable function enabling to set up one raw idt entry
 */
extern int i386_set_idt_entry (const rtems_raw_irq_connect_data*);

/*
 * C callable function enabling to get one current raw idt entry
 */
extern int i386_get_current_idt_entry (rtems_raw_irq_connect_data*);

/*
 * C callable function enabling to remove one current raw idt entry
 */
extern int i386_delete_idt_entry (const rtems_raw_irq_connect_data*);

/*
 * C callable function enabling to init idt.
 *
 * CAUTION : this function assumes that the IDTR register
 * has been already set.
 */
extern int i386_init_idt (rtems_raw_irq_global_settings* config);

/*
 * C callable function enabling to get actual idt configuration
 */
extern int i386_get_idt_config (rtems_raw_irq_global_settings** config);


/*
 * See page 11.12 Figure 11-8.
 *
 */

typedef struct {
  unsigned int limit_15_0 		: 16;
  unsigned int base_address_15_0	: 16;
  unsigned int base_address_23_16	: 8;
  unsigned int type			: 4;
  unsigned int descriptor_type		: 1;
  unsigned int privilege		: 2;
  unsigned int present			: 1;
  unsigned int limit_19_16		: 4;
  unsigned int available		: 1;
  unsigned int fixed_value_bits		: 1;
  unsigned int operation_size		: 1;
  unsigned int granularity		: 1;
  unsigned int base_address_31_24	: 8;
}segment_descriptors;

/*
 * C callable function enabling to get easilly usable info from
 * the actual value of GDT register.
 */
extern void i386_get_info_from_GDTR (segment_descriptors** table,
				     unsigned* limit);
/*
 * C callable function enabling to change the value of GDT register. Must be called
 * with interrupts masked at processor level!!!.
 */
extern void i386_set_GDTR (segment_descriptors*,
			   unsigned limit);

/*
 * C callable function enabling to set up one raw interrupt handler
 */
extern int i386_set_gdt_entry (unsigned short segment_selector, unsigned base,
					     unsigned limit);

/*
 * See page 11.18 Figure 11-12.
 *
 */

typedef struct {
  unsigned int offset			: 12;
  unsigned int page			: 10;
  unsigned int directory 		: 10;
}la_bits;

typedef union {
  la_bits	bits;
  unsigned int	address;
}linear_address;


/*
 * See page 11.20 Figure 11-14.
 *
 */

typedef struct {
  unsigned int present	 		: 1;
  unsigned int writable			: 1;
  unsigned int user			: 1;
  unsigned int write_through		: 1;
  unsigned int cache_disable		: 1;
  unsigned int accessed			: 1;
  unsigned int reserved1		: 1;
  unsigned int page_size		: 1;
  unsigned int reserved2		: 1;
  unsigned int available		: 3;
  unsigned int page_frame_address	: 20;
}page_dir_bits;

typedef union {
  page_dir_bits	bits;
  unsigned int	dir_entry;
}page_dir_entry;

typedef struct {
  unsigned int present	 		: 1;
  unsigned int writable			: 1;
  unsigned int user			: 1;
  unsigned int write_through		: 1;
  unsigned int cache_disable		: 1;
  unsigned int accessed			: 1;
  unsigned int dirty			: 1;
  unsigned int reserved2		: 2;
  unsigned int available		: 3;
  unsigned int page_frame_address	: 20;
}page_table_bits;

typedef union {
  page_table_bits	bits;
  unsigned int		table_entry;
} page_table_entry;
 
/*
 * definitions related to page table entry
 */
#define PG_SIZE 0x1000
#define MASK_OFFSET 0xFFF
#define MAX_ENTRY (PG_SIZE/sizeof(page_dir_entry))
#define FOUR_MB       0x400000
#define MASK_FLAGS 0x1A

#define PTE_PRESENT  		0x01
#define PTE_WRITABLE 		0x02
#define PTE_USER		0x04
#define PTE_WRITE_THROUGH	0x08
#define PTE_CACHE_DISABLE	0x10

typedef struct {
  page_dir_entry pageDirEntry[MAX_ENTRY];
} page_directory;

typedef struct {
  page_table_entry pageTableEntry[MAX_ENTRY];
} page_table;

static inline void flush_cache()
{
  /* Would this be better as a macro? */
  asm ("wbinvd");  /* gcc did not like a volatile being on this */
}


/* C declaration for paging management */

extern int  	_CPU_is_cache_enabled();
extern int  	_CPU_is_paging_enabled();
extern int 	init_paging();
extern void 	_CPU_enable_paging();
extern void 	_CPU_disable_paging();
extern void 	_CPU_disable_cache();
extern void 	_CPU_enable_cache();
extern int 	_CPU_map_phys_address
                      (void **mappedAddress, void *physAddress,
		       int size, int flag); 
extern int 	_CPU_unmap_virt_address (void *mappedAddress, int size); 
extern int 	_CPU_change_memory_mapping_attribute
                         (void **newAddress, void *mappedAddress,
			  unsigned int size, unsigned int flag);
extern int  	_CPU_display_memory_attribute(); 

# endif /* ASM */

#endif

