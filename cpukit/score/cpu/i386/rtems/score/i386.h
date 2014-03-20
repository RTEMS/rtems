/**
 * @file
 *
 * @brief Intel I386 CPU Dependent Source
 *
 * This include file contains information pertaining to the Intel
 * i386 processor.
 */

/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
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

static inline uint16_t i386_swap_u16(
  uint16_t value
)
{
  unsigned short      sout;

  __asm__ volatile( "rorw $8,%0" : "=r"  (sout) : "0"   (value));
  return (sout);
}

/*
 * Added for pagination management
 */
static inline unsigned int i386_get_cr0(void)
{
  register unsigned int segment = 0;

  __asm__ volatile ( "movl %%cr0,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

static inline void i386_set_cr0(unsigned int segment)
{
  __asm__ volatile ( "movl %0,%%cr0" : "=r" (segment) : "0" (segment) );
}

static inline unsigned int i386_get_cr2(void)
{
  register unsigned int segment = 0;

  __asm__ volatile ( "movl %%cr2,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

static inline unsigned int i386_get_cr3(void)
{
  register unsigned int segment = 0;

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

#define CPU_swap_u32( _value )  i386_swap_u32( _value )
#define CPU_swap_u16( _value )  i386_swap_u16( _value )

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
