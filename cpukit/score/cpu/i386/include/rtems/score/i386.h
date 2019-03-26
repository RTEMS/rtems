/**
 * @file
 *
 * @brief Intel I386 CPU Dependent Source
 * 
 * @addtogroup RTEMSScoreCPUi386
 *
 * This include file contains information pertaining to the Intel
 * i386 processor.
 */

/*
 *  COPYRIGHT (c) 1989-2016.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (C) 1998  Eric Valette (valette@crf.canon.fr)
 *                     Canon Centre Recherche France.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_I386_H
#define _RTEMS_SCORE_I386_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/interrupts.h>
#include <rtems/score/registers.h>

/*
 *  This section contains the information required to build
 *  RTEMS for a particular member of the Intel i386
 *  family when executing in protected mode.  It does
 *  this by setting variables to indicate which implementation
 *  dependent features are present in a particular member
 *  of the family.
 *
 *  Currently recognized:
 *    i386_fp    (i386 DX or SX w/i387)
 *    i486dx
 *    pentium
 *    pentiumpro
 *
 *  CPU Model Feature Flags:
 *
 *  I386_HAS_BSWAP:  Defined to "1" if the instruction for endian swapping
 *                   (bswap) should be used.  This instruction appears to
 *                   be present in all i486's and above.
 *
 *  I386_HAS_FPU:    Defined to "1" if the CPU has an FPU.
 *                   As of at least gcc 4.7, i386 soft-float was obsoleted.
 *                   Thus this is always set to "1".
 */
#define I386_HAS_FPU 1

#if defined(__pentiumpro__)

  #define CPU_MODEL_NAME  "Pentium Pro"

#elif defined(__i586__)

  #if defined(__pentium__)
    #define CPU_MODEL_NAME "Pentium"
  #elif defined(__k6__)
    #define CPU_MODEL_NAME "K6"
  #else
    #define CPU_MODEL_NAME "i586"
  #endif

#elif defined(__i486__)

  #define CPU_MODEL_NAME  "i486dx"

#elif defined(__i386__)

  #define I386_HAS_BSWAP  0
  #define CPU_MODEL_NAME  "i386 with i387"

#else
  #error "Unknown CPU Model"
#endif

/*
 *  Set default values for CPU model feature flags
 *
 *  NOTE: These settings are chosen to reflect most of the family members.
 */
#ifndef I386_HAS_BSWAP
#define I386_HAS_BSWAP 1
#endif

/*
 *  Define the name of the CPU family.
 */
#define CPU_NAME "Intel i386"

#ifndef ASM

/*
 *  The following routine swaps the endian format of an unsigned int.
 *  It must be static so it can be referenced indirectly.
 */

static inline uint32_t i386_swap_u32(
  uint32_t value
)
{
  uint32_t lout;

#if (I386_HAS_BSWAP == 0)
  __asm__ volatile( "rorw  $8,%%ax;"
                "rorl  $16,%0;"
                "rorw  $8,%%ax" : "=a" (lout) : "0" (value) );
#else
    __asm__ volatile( "bswap %0" : "=r"  (lout) : "0"   (value));
#endif
  return( lout );
}
#define CPU_swap_u32( _value )  i386_swap_u32( _value )

static inline uint16_t i386_swap_u16(
  uint16_t value
)
{
  unsigned short      sout;

  __asm__ volatile( "rorw $8,%0" : "=r"  (sout) : "0"   (value));
  return (sout);
}
#define CPU_swap_u16( _value )  i386_swap_u16( _value )

/*
 * Added for pagination management
 */
static inline unsigned int i386_get_cr0(void)
{
  unsigned int segment = 0;

  __asm__ volatile ( "movl %%cr0,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

static inline void i386_set_cr0(unsigned int segment)
{
  __asm__ volatile ( "movl %0,%%cr0" : "=r" (segment) : "0" (segment) );
}

static inline unsigned int i386_get_cr2(void)
{
  unsigned int segment = 0;

  __asm__ volatile ( "movl %%cr2,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

static inline unsigned int i386_get_cr3(void)
{
  unsigned int segment = 0;

  __asm__ volatile ( "movl %%cr3,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

static inline void i386_set_cr3(unsigned int segment)
{
  __asm__ volatile ( "movl %0,%%cr3" : "=r" (segment) : "0" (segment) );
}

/* routines */

/*
 *  i386_Logical_to_physical
 *
 *  Converts logical address to physical address.
 */
void *i386_Logical_to_physical(
  unsigned short  segment,
  void           *address
);

/*
 *  i386_Physical_to_logical
 *
 *  Converts physical address to logical address.
 */
void *i386_Physical_to_logical(
  unsigned short  segment,
  void           *address
);

/**
 * @brief Converts real mode pointer {segment, offset} to physical address.
 *
 * i386_Real_to_physical
 *
 * @param[in] segment used with \p offset to compute physical address
 * @param[in] offset used with \p segment to compute physical address
 * @retval    physical address
 */
RTEMS_INLINE_ROUTINE void *i386_Real_to_physical(
    uint16_t segment,
    uint16_t offset)
{
    return (void *)(((uint32_t)segment<<4)+offset);
}

/**
 * @brief Retrieves real mode pointer elements {segmnet, offset} from
 * physical address.
 *
 * i386_Physical_to_real
 * Function returns the highest segment (base) address possible.
 * Example:    input   address - 0x4B3A2
 *             output  segment - 0x4B3A
 *                     offset  - 0x2
 *             input   address - 0x10F12E
 *             output  segment - 0xFFFF
 *                     offset  - 0xF13E
 *
 * @param[in]  address address to be converted, must be less than 0x10FFEF
 * @param[out] segment segment computed from \p address
 * @param[out] offset offset computed from \p address
 * @retval  0 address not convertible
 * @retval  1 segment and offset extracted
 */
int i386_Physical_to_real(
  void *address,
  uint16_t *segment,
  uint16_t *offset
);

/*
 *  Segment Access Routines
 *
 *  NOTE:  Unfortunately, these are still static inlines even when the
 *         "macro" implementation of the generic code is used.
 */

static __inline__ unsigned short i386_get_cs(void)
{
  unsigned short segment = 0;

  __asm__ volatile ( "movw %%cs,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

static __inline__ unsigned short i386_get_ds(void)
{
  unsigned short segment = 0;

  __asm__ volatile ( "movw %%ds,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

static __inline__ unsigned short i386_get_es(void)
{
  unsigned short segment = 0;

  __asm__ volatile ( "movw %%es,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

static __inline__ unsigned short i386_get_ss(void)
{
  unsigned short segment = 0;

  __asm__ volatile ( "movw %%ss,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

static __inline__ unsigned short i386_get_fs(void)
{
  unsigned short segment = 0;

  __asm__ volatile ( "movw %%fs,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

static __inline__ unsigned short i386_get_gs(void)
{
  unsigned short segment = 0;

  __asm__ volatile ( "movw %%gs,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

/*
 *  IO Port Access Routines
 */

#define i386_outport_byte( _port, _value ) \
do { unsigned short __port  = _port; \
     unsigned char  __value = _value; \
     \
     __asm__ volatile ( "outb %0,%1" : : "a" (__value), "d" (__port) ); \
   } while (0)

#define i386_outport_word( _port, _value ) \
do { unsigned short __port  = _port; \
     unsigned short __value = _value; \
     \
     __asm__ volatile ( "outw %0,%1" : : "a" (__value), "d" (__port) ); \
   } while (0)

#define i386_outport_long( _port, _value ) \
do { unsigned short __port  = _port; \
     unsigned int  __value = _value; \
     \
     __asm__ volatile ( "outl %0,%1" : : "a" (__value), "d" (__port) ); \
   } while (0)

#define i386_inport_byte( _port, _value ) \
do { unsigned short __port  = _port; \
     unsigned char  __value = 0; \
     \
     __asm__ volatile ( "inb %1,%0" : "=a" (__value) \
                                : "d"  (__port) \
                  ); \
     _value = __value; \
   } while (0)

#define i386_inport_word( _port, _value ) \
do { unsigned short __port  = _port; \
     unsigned short __value = 0; \
     \
     __asm__ volatile ( "inw %1,%0" : "=a" (__value) \
                                : "d"  (__port) \
                  ); \
     _value = __value; \
   } while (0)

#define i386_inport_long( _port, _value ) \
do { unsigned short __port  = _port; \
     unsigned int  __value = 0; \
     \
     __asm__ volatile ( "inl %1,%0" : "=a" (__value) \
                                : "d"  (__port) \
                  ); \
     _value = __value; \
   } while (0)

/*
 * Type definition for raw interrupts.
 */

typedef unsigned char  rtems_vector_offset;

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

#include <rtems/score/idtr.h>

/*
 * C callable function enabling to get handler currently connected to a vector
 *
 */
rtems_raw_irq_hdl get_hdl_from_vector(rtems_vector_offset);

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
/**
 * @brief describes one entry of Global/Local Descriptor Table
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
} RTEMS_PACKED segment_descriptors;

/*
 * C callable function enabling to get easilly usable info from
 * the actual value of GDT register.
 */
extern void i386_get_info_from_GDTR (segment_descriptors** table,
                                     uint16_t* limit);
/*
 * C callable function enabling to change the value of GDT register. Must be called
 * with interrupts masked at processor level!!!.
 */
extern void i386_set_GDTR (segment_descriptors*,
                           uint16_t limit);

/**
 * @brief Allows to set a GDT entry.
 *
 * Puts global descriptor \p sd to the global descriptor table on index
 * \p segment_selector_index
 *
 * @param[in] segment_selector_index index to GDT entry
 * @param[in] sd structure to be coppied to given \p segment_selector in GDT
 * @retval  0 FAILED out of GDT range or index is 0, which is not valid
 *                   index in GDT
 * @retval  1 SUCCESS
 */
extern uint32_t i386_raw_gdt_entry (uint16_t segment_selector_index,
                               segment_descriptors* sd);

/**
 * @brief fills \p sd with provided \p base in appropriate fields of \p sd
 *
 * @param[in] base 32-bit address to be set as descriptor's base
 * @param[out] sd descriptor being filled with \p base
 */
extern void i386_fill_segment_desc_base (uint32_t base,
                                         segment_descriptors* sd);

/**
 * @brief fills \p sd with provided \p limit in appropriate fields of \p sd
 *
 * sets granularity bit if necessary
 *
 * @param[in] limit 32-bit value representing number of limit bytes
 * @param[out] sd descriptor being filled with \p limit
 */
extern void i386_fill_segment_desc_limit (uint32_t limit,
                                          segment_descriptors* sd);

/*
 * C callable function enabling to set up one raw interrupt handler
 */
extern uint32_t i386_set_gdt_entry (uint16_t segment_selector,
                                    uint32_t base,
                                    uint32_t limit);

/**
 * @brief Returns next empty descriptor in GDT.
 *
 * Number of descriptors that can be returned depends on \a GDT_SIZE
 *
 * @retval  0 FAILED GDT is full
 * @retval  <1;65535> segment_selector number as index to GDT
 */
extern uint16_t i386_next_empty_gdt_entry (void);

/**
 * @brief Copies GDT entry at index \p segment_selector to structure
 * pointed to by \p struct_to_fill
 *
 * @param[in] segment_selector index to GDT table specifying descriptor to copy
 * @param[out] struct_to_fill pointer to memory where will be descriptor coppied
 * @retval  0 FAILED segment_selector out of GDT range
 * @retval  <1;65535> retrieved segment_selector
 */
extern uint16_t i386_cpy_gdt_entry (uint16_t segment_selector,
                                    segment_descriptors* struct_to_fill);

/**
 * @brief Returns pointer to GDT table at index given by \p segment_selector
 *
 * @param[in] sgmnt_selector index to GDT table for specifying descriptor to get
 * @retval  NULL FAILED segment_selector out of GDT range
 * @retval  pointer to GDT table at \p segment_selector
 */
extern segment_descriptors* i386_get_gdt_entry (uint16_t sgmnt_selector);

/**
 * @brief Extracts base address from GDT entry pointed to by \p gdt_entry
 *
 * @param[in]  gdt_entry pointer to entry from which base should be retrieved
 * @retval base address from GDT entry
*/
RTEMS_INLINE_ROUTINE void* i386_base_gdt_entry (segment_descriptors* gdt_entry)
{
    return (void*)(gdt_entry->base_address_15_0 |
            (gdt_entry->base_address_23_16<<16) |
            (gdt_entry->base_address_31_24<<24));
}

/**
 * @brief Extracts limit in bytes from GDT entry pointed to by \p gdt_entry
 *
 * @param[in]  gdt_entry pointer to entry from which limit should be retrieved
 * @retval limit value in bytes from GDT entry
 */
extern uint32_t i386_limit_gdt_entry (segment_descriptors* gdt_entry);

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

/* Simpler names for the i80x86 I/O instructions */
#define outport_byte( _port, _value ) i386_outport_byte( _port, _value )
#define outport_word( _port, _value ) i386_outport_word( _port, _value )
#define outport_long( _port, _value ) i386_outport_long( _port, _value )
#define inport_byte( _port, _value )  i386_inport_byte( _port, _value )
#define inport_word( _port, _value )  i386_inport_word( _port, _value )
#define inport_long( _port, _value )  i386_inport_long( _port, _value )

#ifdef __cplusplus
}
#endif

#endif /* !ASM */

#endif
