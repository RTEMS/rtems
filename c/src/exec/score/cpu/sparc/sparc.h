/*  sparc.h
 *
 *  This include file contains information pertaining to the Motorola
 *  SPARC processor family.
 *
 *  $Id$
 */

#ifndef _INCLUDE_SPARC_h
#define _INCLUDE_SPARC_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  The following define the CPU Family and Model within the family
 *
 *  NOTE: The string "REPLACE_THIS_WITH_THE_CPU_MODEL" is replaced
 *        with the name of the appropriate macro for this target CPU.
 */
 
#ifdef sparc
#undef sparc
#endif
#define sparc

#ifdef REPLACE_THIS_WITH_THE_CPU_MODEL
#undef REPLACE_THIS_WITH_THE_CPU_MODEL
#endif
#define REPLACE_THIS_WITH_THE_CPU_MODEL
 
#ifdef REPLACE_THIS_WITH_THE_BSP
#undef REPLACE_THIS_WITH_THE_BSP
#endif
#define REPLACE_THIS_WITH_THE_BSP

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the "sparc"
 *  family when executing in protected mode.  It does
 *  this by setting variables to indicate which implementation
 *  dependent features are present in a particular member
 *  of the family.
 *
 *  Currently recognized feature flags:
 *
 *    + SPARC_HAS_FPU 
 *        0 - no HW FPU
 *        1 - has HW FPU (assumed to be compatible w/90C602)
 *
 *    + SPARC_HAS_BITSCAN 
 *        0 - does not have scan instructions
 *        1 - has scan instruction  (no support implemented)
 * 
 */
 
#if defined(erc32)
 
#define CPU_MODEL_NAME  "erc32"
#define SPARC_HAS_FPU     1
#define SPARC_HAS_BITSCAN 0
 
#else
 
#error "Unsupported CPU Model"
 
#endif

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "SPARC"

/*
 *  Standard nop
 */

#define nop() \
  do { \
    asm volatile ( "nop" ); \
  } while ( 0 )

/*
 *  Some macros to aid in accessing special registers.
 */

#define sparc_get_psr( _psr ) \
  do { \
     (_psr) = 0; \
     asm volatile( "rd %%psr, %0" :  "=r" (_psr) : "0" (_psr) ); \
  } while ( 0 )

#define sparc_set_psr( _psr ) \
  do { \
    asm volatile ( "wr   %%g0,%0,%%psr " : "=r" ((_psr)) : "0" ((_psr)) ); \
    nop(); nop(); nop(); \
  } while ( 0 )

#define sparc_get_tbr( _tbr ) \
  do { \
     asm volatile( "rd %%tbr, %0" :  "=r" (_tbr) : "0" (_tbr) ); \
  } while ( 0 )

#define sparc_set_tbr( _tbr ) \
  do { \
  } while ( 0 )

#define sparc_get_wim( _wim ) \
  do { \
     asm volatile( "rd %%wim, %0" :  "=r" (_wim) : "0" (_wim) ); \
  } while ( 0 )

#define sparc_set_wim( _wim ) \
  do { \
  } while ( 0 )

/*
 *  Manipulate the interrupt level in the psr 
 *
 */

#define SPARC_PIL_MASK  0x00000F00

#define sparc_disable_interrupts( _level ) \
  do { register unsigned int _mask = SPARC_PIL_MASK; \
    (_level) = 0; \
    \
    asm volatile ( "rd   %%psr,%0 ; \
                    wr   %0,%1,%%psr " \
                    : "=r" ((_level)), "=r" (_mask) \
                    : "0" ((_level)), "1" (_mask) \
    ); \
    nop(); nop(); nop(); \
  } while ( 0 )
 
#define sparc_enable_interrupts( _level ) \
  do { unsigned int _tmp; \
    sparc_get_psr( _tmp ); \
    _tmp &= ~SPARC_PIL_MASK; \
    _tmp |= (_level) & SPARC_PIL_MASK; \
    sparc_set_psr( _tmp ); \
  } while ( 0 ) 
  
 
#define sparc_flash_interrupts( _level ) \
  do { \
      register unsigned32 _ignored = 0; \
      sparc_enable_interrupts( (_level) ); \
      sparc_disable_interrupts( _ignored ); \
  } while ( 0 )

#define sparc_set_interrupt_level( _new_level ) \
  do { register unsigned32 _new_psr_level = 0; \
    \
    sparc_get_psr( _new_psr_level ); \
    _new_psr_level &= ~SPARC_PIL_MASK; \
    _new_psr_level |= (((_new_level) << 8) & SPARC_PIL_MASK); \
    sparc_set_psr( _new_psr_level ); \
  } while ( 0 )

#define sparc_get_interrupt_level( _level ) \
  do { \
    register unsigned32 _psr_level = 0; \
    \
    sparc_get_psr( _psr_level ); \
    (_level) = (_psr_level & SPARC_PIL_MASK) >> 8; \
  } while ( 0 )

#ifdef __cplusplus
}
#endif

#endif /* ! _INCLUDE_SPARC_h */
/* end of include file */
