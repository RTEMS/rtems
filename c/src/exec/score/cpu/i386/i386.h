/*  i386.h
 *
 *  This include file contains information pertaining to the Intel
 *  i386 processor.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __i386_h
#define __i386_h

#ifdef __cplusplus
extern "C" {
#endif

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
 *    i386_fp    (i386 DX or SX w/o i387)
 *    i486dx
 *    i486sx
 *    pentium
 *
 *  Floating point is the only feature which currently varies.  Eventually
 *  the i486-plus level instruction for endian swapping should be added
 *  to this feature list.
 */

#if defined(i386_fp)

#define CPU_MODEL_NAME  "i386 with i387"
#define I386_HAS_FPU 1

#elif defined(i386_nofp)

#define CPU_MODEL_NAME  "i386 w/o i387"
#define I386_HAS_FPU 0

#elif defined(i486dx)

#define CPU_MODEL_NAME  "i486dx"
#define I386_HAS_FPU 1

#elif defined(i486sx)

#define CPU_MODEL_NAME  "i486sx"
#define I386_HAS_FPU 0

#elif defined(pentium)

#define CPU_MODEL_NAME  "Pentium"
#define I386_HAS_FPU 1

#else

#error "Unsupported CPU Model"

#endif

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "Intel i386"

#ifndef ASM

/*
 *  Structure which makes it easier to deal with LxDT and SxDT instructions.
 */
 
typedef struct {
  unsigned short limit;
  unsigned short physical_address[ 2 ];
} i386_DTR_load_save_format;
 
/* See Chapter 5 - Memory Management in i386 manual */
 
typedef struct {
  unsigned short limit_0_15;
  unsigned short base_0_15;
  unsigned char  base_16_23;
  unsigned char  type_dt_dpl_p;
  unsigned char  limit_16_19_granularity;
  unsigned char  base_24_31;
}   i386_GDT_slot;
 
/* See Chapter 9 - Exceptions and Interrupts in i386 manual
 *
 *  NOTE: This is the IDT entry for interrupt gates ONLY.
 */
 
typedef struct {
  unsigned short offset_0_15;
  unsigned short segment_selector;
  unsigned char  reserved;
  unsigned char  p_dpl;
  unsigned short offset_16_31;
}   i386_IDT_slot;

/*
 *  Interrupt Level Macros
 */

#define i386_disable_interrupts( _level ) \
  { \
    _level = 0;   /* avoids warnings */ \
    asm volatile ( "pushf ; \
                    cli ; \
                    pop %0" \
                    : "=r" ((_level)) : "0" ((_level)) \
    ); \
  }

#define i386_enable_interrupts( _level )  \
  { \
    asm volatile ( "push %0 ; \
                    popf" \
                    : "=r" ((_level)) : "0" ((_level)) \
    ); \
  }

#define i386_flash_interrupts( _level ) \
  { \
    asm volatile ( "push %0 ; \
                    popf ; \
                    cli" \
                    : "=r" ((_level)) : "0" ((_level)) \
    ); \
  }

#define i386_get_interrupt_level( _level ) \
  do { \
    register unsigned32 _eflags = 0; \
    \
    asm volatile ( "pushf ; \
                    pop %0" \
                    : "=r" ((_eflags)) : "0" ((_eflags)) \
    ); \
    \
    _level = (_eflags & 0x0200) ? 0 : 1; \
  } while (0)

/*
 *  The following routine swaps the endian format of an unsigned int.
 *  It must be static so it can be referenced indirectly.
 */

static inline unsigned int i386_swap_U32(
  unsigned int value
)
{
  asm volatile( "rorw  $8,%%ax;"
                "rorl  $16,%0;"
                "rorw  $8,%%ax" : "=a" (value) : "0" (value) );
  return( value );
}

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
 *  IO Port Access Routines
 */

#define i386_outport_byte( _port, _value ) \
   { register unsigned short __port  = _port; \
     register unsigned char  __value = _value; \
     \
     asm volatile ( "outb %0,%1" : "=a" (__value), "=d" (__port) \
                                 : "0"   (__value), "1"  (__port) \
                  ); \
   }

#define i386_outport_word( _port, _value ) \
   { register unsigned short __port  = _port; \
     register unsigned short __value = _value; \
     \
     asm volatile ( "outw %0,%1" : "=a" (__value), "=d" (__port) \
                                 : "0"   (__value), "1"  (__port) \
                  ); \
   }

#define i386_outport_long( _port, _value ) \
   { register unsigned short __port  = _port; \
     register unsigned int  __value = _value; \
     \
     asm volatile ( "outl %0,%1" : "=a" (__value), "=d" (__port) \
                                 : "0"   (__value), "1"  (__port) \
                  ); \
   }

#define i386_inport_byte( _port, _value ) \
   { register unsigned short __port  = _port; \
     register unsigned char  __value = 0; \
     \
     asm volatile ( "inb %1,%0" : "=a" (__value), "=d" (__port) \
                                : "0"   (__value), "1"  (__port) \
                  ); \
     _value = __value; \
   }

#define i386_inport_word( _port, _value ) \
   { register unsigned short __port  = _port; \
     register unsigned short __value = 0; \
     \
     asm volatile ( "inw %1,%0" : "=a" (__value), "=d" (__port) \
                                : "0"   (__value), "1"  (__port) \
                  ); \
     _value = __value; \
   }

#define i386_inport_long( _port, _value ) \
   { register unsigned short __port  = _port; \
     register unsigned int  __value = 0; \
     \
     asm volatile ( "inl %1,%0" : "=a" (__value), "=d" (__port) \
                                : "0"   (__value), "1"  (__port) \
                  ); \
     _value = __value; \
   }

/*
 *  Descriptor Table helper routines
 */


#define i386_get_GDTR( _gdtr_address ) \
  { \
    void *_gdtr = (_gdtr_address); \
    \
    asm volatile( "sgdt   (%0)" : "=r" (_gdtr) : "0" (_gdtr) ); \
  }

#define i386_get_GDT_slot( _gdtr_base, _segment, _slot_address ) \
  { \
    register unsigned int   _gdt_slot  = (_gdtr_base) + (_segment); \
    register volatile void *_slot      = (_slot_address); \
    register unsigned int   _temporary = 0; \
    \
    asm volatile( "movl %%gs:(%0),%1 ; \
                   movl %1,(%2) ; \
                   movl %%gs:4(%0),%1 ; \
                   movl %1,4(%2)"  \
                     : "=r" (_gdt_slot), "=r" (_temporary), "=r" (_slot) \
                     : "0"  (_gdt_slot), "1"  (_temporary), "2"  (_slot) \
                );  \
  }

#define i386_set_GDT_slot( _gdtr_base, _segment, _slot_address ) \
  { \
    register unsigned int   _gdt_slot  = (_gdtr_base) + (_segment); \
    register volatile void *_slot      = (_slot_address); \
    register unsigned int   _temporary = 0; \
    \
    asm volatile( "movl (%2),%1 ; \
                   movl %1,%%gs:(%0) ; \
                   movl 4(%2),%1 ; \
                   movl %1,%%gs:4(%0) \
                  " \
                     : "=r" (_gdt_slot), "=r" (_temporary), "=r" (_slot) \
                     : "0"  (_gdt_slot), "1"  (_temporary), "2"  (_slot) \
                );  \
  }

static inline void i386_set_segment(
  unsigned short segment,
  unsigned int  base,
  unsigned int  limit
)
{
  i386_DTR_load_save_format         gdtr;
  volatile i386_GDT_slot            Gdt_slot;
  volatile i386_GDT_slot           *gdt_slot = &Gdt_slot;
  unsigned short                    tmp_segment = 0;
  unsigned int                      limit_adjusted;

  /* load physical address of the GDT */

  i386_get_GDTR( &gdtr );

  gdt_slot->type_dt_dpl_p  = 0x92;             /* present, dpl=0,      */
                                               /* application=1,       */
                                               /* type=data read/write */
  gdt_slot->limit_16_19_granularity = 0x40;    /* 32 bit segment       */

  limit_adjusted = limit;
  if ( limit > 4095 ) {
    gdt_slot->limit_16_19_granularity |= 0x80; /* set granularity bit */
    limit_adjusted /= 4096;
  }

  gdt_slot->limit_16_19_granularity |= (limit_adjusted >> 16) & 0xff;
  gdt_slot->limit_0_15               = limit_adjusted & 0xffff;

  gdt_slot->base_0_15  = base & 0xffff;
  gdt_slot->base_16_23 = (base >> 16) & 0xff;
  gdt_slot->base_24_31 = (base >> 24);

  i386_set_GDT_slot(
    gdtr.physical_address[0] + (gdtr.physical_address[1] << 16),
    segment,
    gdt_slot
  );

  /* Now, reload all segment registers so the limit takes effect. */

  asm volatile( "movw %%ds,%0 ; movw %0,%%ds
                 movw %%es,%0 ; movw %0,%%es
                 movw %%fs,%0 ; movw %0,%%fs
                 movw %%gs,%0 ; movw %0,%%gs
                 movw %%ss,%0 ; movw %0,%%ss"
                   : "=r" (tmp_segment)
                   : "0"  (tmp_segment)
              );

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

/*
 *  i386_Install_idt
 *
 *  This routine installs an IDT entry.
 */

void i386_Install_idt(
  unsigned int   source_offset,
  unsigned short destination_segment,
  unsigned int   destination_offset
);

/*
 *  "Simpler" names for a lot of the things defined in this file
 */

/* segment access routines */
 
#define get_cs()   i386_get_cs()
#define get_ds()   i386_get_ds()
#define get_es()   i386_get_es()
#define get_ss()   i386_get_ss()
#define get_fs()   i386_get_fs()
#define get_gs()   i386_get_gs()
 
#define CPU_swap_u32( _value )  i386_swap_U32( _value )
 
/* i80x86 I/O instructions */
 
#define outport_byte( _port, _value ) i386_outport_byte( _port, _value )
#define outport_word( _port, _value ) i386_outport_word( _port, _value )
#define outport_long( _port, _value ) i386_outport_long( _port, _value )
#define inport_byte( _port, _value )  i386_inport_byte( _port, _value )
#define inport_word( _port, _value )  i386_inport_word( _port, _value )
#define inport_long( _port, _value )  i386_inport_long( _port, _value )
 
/* complicated static inline functions */
 
#define get_GDTR( _gdtr_address ) \
        i386_get_GDTR( _gdtr_address )
 
#define get_GDT_slot( _gdtr_base, _segment, _slot_address ) \
        i386_get_GDT_slot( _gdtr_base, _segment, _slot_address )
 
#define set_GDT_slot( _gdtr_base, _segment, _slot_address ) \
        i386_set_GDT_slot( _gdtr_base, _segment, _slot_address )
 
#define set_segment( _segment, _base, _limit ) \
  i386_set_segment( _segment, _base, _limit )
 

#ifdef __cplusplus
}
#endif

#endif /* !ASM */

#endif
/* end of include file */
