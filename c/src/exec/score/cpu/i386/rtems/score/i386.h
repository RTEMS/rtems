/*  i386.h
 *
 *  This include file contains information pertaining to the Intel
 *  i386 processor.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
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
 *    i386_nofp  (i386 DX or SX w/o i387)
 *    i486dx
 *    i486sx
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
 *
 */

#if defined(rtems_multilib)
/*
 *  Figure out all CPU Model Feature Flags based upon compiler 
 *  predefines. 
 */

#define CPU_MODEL_NAME  "rtems_multilib"
#define I386_HAS_FPU   0
#define I386_HAS_BSWAP 0

#elif defined(i386_fp)

#define CPU_MODEL_NAME  "i386 with i387"
#define I386_HAS_BSWAP 0

#elif defined(i386_nofp)

#define CPU_MODEL_NAME  "i386 w/o i387"
#define I386_HAS_FPU   0
#define I386_HAS_BSWAP 0

#elif defined(i486dx)

#define CPU_MODEL_NAME  "i486dx"

#elif defined(i486sx)

#define CPU_MODEL_NAME  "i486sx"
#define I386_HAS_FPU 0

#elif defined(pentium)

#define CPU_MODEL_NAME  "Pentium"

#elif defined(pentiumpro)

#define CPU_MODEL_NAME  "Pentium Pro"

#else

#error "Unsupported CPU Model"

#endif

/*
 *  Set default values for CPU model feature flags
 *
 *  NOTE: These settings are chosen to reflect most of the family members.
 */

#ifndef I386_HAS_FPU
#define I386_HAS_FPU 1
#endif

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

static inline unsigned int i386_swap_U32(
  unsigned int value
)
{
  unsigned long lout;

#if (I386_HAS_BSWAP == 0)
  asm volatile( "rorw  $8,%%ax;"
                "rorl  $16,%0;"
                "rorw  $8,%%ax" : "=a" (lout) : "0" (value) );
#else
    __asm__ volatile( "bswap %0" : "=r"  (lout) : "0"   (value));
#endif
  return( lout );
}

static inline unsigned int i386_swap_U16(
  unsigned int value
)
{
    unsigned short      sout;

    __asm__ volatile( "rorw $8,%0" : "=r"  (sout) : "0"   (value));
    return (sout);
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
 * Disable the entire cache
 */
void _CPU_disable_cache() {
  cr0 regCr0;

  regCr0.i = i386_get_cr0();
  regCr0.cr0.page_level_cache_disable = 1;
  regCr0.cr0.no_write_through = 1;
  i386_set_cr0( regCr0.i );
  rtems_flush_entire_data_cache();
}

/*
 * Enable the entire cache
 */
static inline void _CPU_enable_cache() {
  cr0 regCr0;

  regCr0.i = i386_get_cr0();
  regCr0.cr0.page_level_cache_disable = 0;
  regCr0.cr0.no_write_through = 0;
  i386_set_cr0( regCr0.i );
  /*rtems_flush_entire_data_cache();*/
}

/*
 * CACHE MANAGER: The following functions are CPU-specific.
 * They provide the basic implementation for the rtems_* cache
 * management routines. If a given function has no meaning for the CPU,
 * it does nothing by default.
 * 
 * FIXME: Definitions for I386_CACHE_ALIGNMENT are missing above for
 *        each CPU. The routines below should be implemented per CPU,
 *        to accomodate the capabilities of each.
 */

/* FIXME: I don't belong here. */
#define I386_CACHE_ALIGNMENT 16

#if defined(I386_CACHE_ALIGNMENT)
#define _CPU_DATA_CACHE_ALIGNMENT I386_CACHE_ALIGNMENT
#define _CPU_INST_CACHE_ALIGNEMNT I386_CACHE_ALIGNMENT

static inline void _CPU_flush_1_data_cache_line (const void * d_addr) {}
static inline void _CPU_invalidate_1_data_cache_line (const void * d_addr) {}
static inline void _CPU_freeze_data_cache (void) {}
static inline void _CPU_unfreeze_data_cache (void) {}
static inline void _CPU_invalidate_1_inst_cache_line const void * d_addr() {}
static inline void _CPU_freeze_inst_cache (void) {}
static inline void _CPU_unfreeze_inst_cache (void) {}

static inline void _CPU_flush_entire_data_cache (
  const void * d_addr )
{
  asm ("wbinvd");
}
static inline void _CPU_invalidate_entire_data_cache (
  const void * d_addr )
{
  asm ("invd");
}

static inline void _CPU_enable_data_cache (
	void )
{
	_CPU_enable_cache();
}

static inline void _CPU_disable_data_cache (
	void )
{
	_CPU_disable_cache();
}

static inline void _CPU_invalidate_entire_inst_cache (
  const void * i_addr )
{
  asm ("invd");
}

static inline void _CPU_enable_inst_cache (
	void )
{
	_CPU_enable_cache();
}

static inline void _CPU_disable_inst_cache (
	void )
{
	_CPU_disable_cache();
}
#endif


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
#define CPU_swap_u16( _value )  i386_swap_U16( _value )
 
/* i80x86 I/O instructions */
 
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
/* end of include file */
