/*  i386.h
 *
 *  This include file contains information pertaining to the Intel
 *  i386 processor.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
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

#if defined(i386_fp)

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
