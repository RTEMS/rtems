/*  m68k.h
 *
 *  This include file contains information pertaining to the Motorola
 *  m68xxx processor family.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __M68k_h
#define __M68k_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  The following define the CPU Family and Model within the family
 *
 *  NOTE: The string "REPLACE_THIS_WITH_THE_CPU_MODEL" is replaced
 *        with the name of the appropriate macro for this target CPU.
 */
 
#ifdef m68k
#undef m68k
#endif
#define m68k

#ifdef REPLACE_THIS_WITH_THE_CPU_MODEL
#undef REPLACE_THIS_WITH_THE_CPU_MODEL
#endif
#define REPLACE_THIS_WITH_THE_CPU_MODEL

#ifdef REPLACE_THIS_WITH_THE_BSP
#undef REPLACE_THIS_WITH_THE_BSP
#endif
#define REPLACE_THIS_WITH_THE_BSP

/*
 *  This section contains the information required to build
 *  RTEMS for a particular member of the Motorola MC68xxx
 *  family.  It does this by setting variables to indicate
 *  which implementation dependent features are present in
 *  a particular member of the family.
 *
 *  Currently recognized:
 *     m68000        (no FP)
 *     m68020        (implies FP)
 *     m68020_nofp   (no FP)
 *     m68030        (implies FP)
 *     m68040        (implies FP)
 *     m68lc040      (no FP)
 *     m68ec040      (no FP)
 *
 *  Primary difference (for RTEMS) between m68040, m680lc040, and 
 *  m68ec040 is the presence or abscense of the FPU.
 *
 *  Here is some information on the 040 variants (courtesy of Doug McBride,
 *  mcbride@rodin.colorado.edu):
 *
 *    "The 68040 is a superset of the 68EC040 and the 68LC040.  The
 *    68EC040 and 68LC040 do not have FPU's.  The 68LC040 and the
 *    68EC040 have renamed the DLE pin as JS0 which must be tied to
 *    Gnd or Vcc. The 68EC040 has renamed the MDIS pin as JS1.  The
 *    68EC040 has access control units instead of memory management units.
 *    The 68EC040 should not have the PFLUSH or PTEST instructions executed
 *    (cause an indeterminate result).  The 68EC040 and 68LC040 do not
 *    implement the DLE or multiplexed bus modes.  The 68EC040 does not
 *    implement the output buffer impedance selection mode of operation."
 */
 
#if defined(m68000)
 
#define RTEMS_MODEL_NAME         "m68000"
#define M68K_HAS_VBR             0
#define M68K_HAS_SEPARATE_STACKS 0
#define M68K_HAS_FPU             0
#define M68K_HAS_BFFFO           0
#define M68K_HAS_PREINDEXING     0

#elif defined(m68020)
 
#define RTEMS_MODEL_NAME         "m68020"
#define M68K_HAS_VBR             1
#define M68K_HAS_SEPARATE_STACKS 1
#define M68K_HAS_FPU             1
#define M68K_HAS_BFFFO           1
#define M68K_HAS_PREINDEXING     1
 
#elif defined(m68020_nofp)
 
#define RTEMS_MODEL_NAME         "m68020 w/o fp"
#define M68K_HAS_VBR             1
#define M68K_HAS_SEPARATE_STACKS 1
#define M68K_HAS_FPU             0
#define M68K_HAS_BFFFO           1
#define M68K_HAS_PREINDEXING     1
 
#elif defined(m68030)
 
#define RTEMS_MODEL_NAME         "m68030"
#define M68K_HAS_VBR             1
#define M68K_HAS_SEPARATE_STACKS 1
#define M68K_HAS_FPU             1
#define M68K_HAS_BFFFO           1
#define M68K_HAS_PREINDEXING     1
 
#elif defined(m68040)

#define RTEMS_MODEL_NAME         "m68040"
#define M68K_HAS_VBR             1
#define M68K_HAS_SEPARATE_STACKS 1
#define M68K_HAS_FPU             1
#define M68K_HAS_BFFFO           1
#define M68K_HAS_PREINDEXING     1
 
#elif defined(m68lc040)

#define RTEMS_MODEL_NAME         "m68lc040"
#define M68K_HAS_VBR             1
#define M68K_HAS_SEPARATE_STACKS 1
#define M68K_HAS_FPU             0
#define M68K_HAS_BFFFO           1
#define M68K_HAS_PREINDEXING     1
 
#elif defined(m68ec040)

#define RTEMS_MODEL_NAME         "m68ec040"
#define M68K_HAS_VBR             1
#define M68K_HAS_SEPARATE_STACKS 1
#define M68K_HAS_FPU             0
#define M68K_HAS_BFFFO           1
#define M68K_HAS_PREINDEXING     1

#else

#error "Unsupported CPU Model"

#endif

/*
 *  If defined, this causes some of the macros to initialize their
 *  variables to zero before doing inline assembly.  This gets rid
 *  of compile time warnings at the cost of a little execution time
 *  in some time critical routines.
 */

#define NO_UNINITIALIZED_WARNINGS

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "Motorola MC68xxx"

#ifndef ASM

#ifdef NO_UNINITIALIZED_WARNINGS
#define m68k_disable_interrupts( _level ) \
  { \
    (_level) = 0;  /* avoids warnings */ \
    asm volatile ( "movew   %%sr,%0 ; \
                    orw     #0x0700,%%sr" \
                    : "=d" ((_level)) : "0" ((_level)) \
    ); \
  }
#else
#define m68k_disable_interrupts( _level ) \
  { \
    asm volatile ( "movew   %%sr,%0 ; \
                    orw     #0x0700,%%sr" \
                    : "=d" ((_level)) : "0" ((_level)) \
    ); \
  }
#endif

#define m68k_enable_interrupts( _level ) \
  { \
    asm volatile ( "movew   %0,%%sr " \
                   : "=d" ((_level)) : "0" ((_level)) \
    ); \
  }

#define m68k_flash_interrupts( _level ) \
  { \
    asm volatile ( "movew   %0,%%sr ; \
                    orw     #0x0700,%%sr" \
                    : "=d" ((_level)) : "0" ((_level)) \
    ); \
  }

#define m68k_get_interrupt_level( _level ) \
  do { \
    register unsigned32 _tmpsr = 0; \
    \
    asm volatile( "movw  %%sr,%0" \
                   : "=d" (_tmpsr) : "0" (_tmpsr) \
    ); \
    \
    _level = (_tmpsr & 0x0700) >> 8; \
  } while (0)
    
#define m68k_set_interrupt_level( _newlevel ) \
  { \
    register unsigned32 _tmpsr = 0; \
    \
    asm volatile( "movw  %%sr,%0" \
                   : "=d" (_tmpsr) : "0" (_tmpsr) \
    ); \
    \
    _tmpsr = (_tmpsr & 0xf8ff) | ((_newlevel) << 8); \
    \
    asm volatile( "movw  %0,%%sr" \
                   : "=d" (_tmpsr) : "0" (_tmpsr) \
    ); \
  }

#if ( M68K_HAS_VBR == 1 )
#define m68k_get_vbr( vbr ) \
  { (vbr) = 0; \
    asm volatile ( "movec   %%vbr,%0 " \
                       : "=r" (vbr) : "0" (vbr) ); \
  }

#define m68k_set_vbr( vbr ) \
  { register m68k_isr *_vbr= (m68k_isr *)(vbr); \
    asm volatile ( "movec   %0,%%vbr " \
                       : "=a" (_vbr) : "0" (_vbr) ); \
  }
#else
#define m68k_get_vbr( _vbr ) _vbr = 0
#define m68k_set_vbr( _vbr )
#endif

/*
 *  The following routine swaps the endian format of an unsigned int.
 *  It must be static because it is referenced indirectly.
 */

static inline unsigned int m68k_swap_u32(
  unsigned int value
)
{
  unsigned int swapped = value;

  asm volatile( "rorw  #8,%0" : "=d" (swapped) : "0" (swapped) );
  asm volatile( "swap  %0"    : "=d" (swapped) : "0" (swapped) );
  asm volatile( "rorw  #8,%0" : "=d" (swapped) : "0" (swapped) );

  return( swapped );
}

/* XXX this is only valid for some m68k family members and should be fixed */

#define m68k_enable_caching() \
  { register unsigned32 _ctl=0x01; \
    asm volatile ( "movec   %0,%%cacr" \
                       : "=d" (_ctl) : "0" (_ctl) ); \
  }

#define CPU_swap_u32( value )  m68k_swap_u32( value )

#ifdef __cplusplus
}
#endif

#endif  /* !ASM */

#endif
/* end of include file */
