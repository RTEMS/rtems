/**
 * @file rtems/score/sparc64.h
 */

/*
 *  This include file contains information pertaining to the SPARC 
 *  processor family.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  This file is based on the SPARC sparc.h file. Modifications are made 
 *  to support the SPARC64 processor.
 *    COPYRIGHT (c) 2010. Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SPARC_H
#define _RTEMS_SCORE_SPARC_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the "sparc" family.  It does
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
 *        1 - has scan instruction  (not currently implemented)
 * 
 *    + SPARC_NUMBER_OF_REGISTER_WINDOWS
 *        8 is the most common number supported by SPARC implementations.
 *        SPARC_PSR_CWP_MASK is derived from this value.
 */
 
/*
 *  Some higher end SPARCs have a bitscan instructions. It would
 *  be nice to take advantage of them.  Right now, there is no
 *  port to a CPU model with this feature and no (untested) code
 *  that is based on this feature flag.
 */

#define SPARC_HAS_BITSCAN                0

/*
 *  This should be OK until a port to a higher end SPARC processor
 *  is made that has more than 8 register windows.  If this cannot
 *  be determined based on multilib settings (v7/v8/v9), then the
 *  cpu_asm.S code that depends on this will have to move to libcpu.
 *
 *  SPARC v9 supports from 3 to 32 register windows.
 *  N_REG_WINDOWS = 8 on UltraSPARC T1 (impl. dep. #2-V8).
 */

#define SPARC_NUMBER_OF_REGISTER_WINDOWS 8
 
/*
 *  This should be determined based on some soft float derived 
 *  cpp predefine but gcc does not currently give us that information.
 */


#if defined(_SOFT_FLOAT)
#define SPARC_HAS_FPU 0
#else
#define SPARC_HAS_FPU 1
#endif

#if SPARC_HAS_FPU
#define CPU_MODEL_NAME "w/FPU"
#else
#define CPU_MODEL_NAME "w/soft-float"
#endif

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "SPARC"

/*
 *  Miscellaneous constants
 */

/* 
 * The PSR is deprecated and deleted.
 *
 * The following registers represent fields of the PSR:
 * 	PIL - Processor Interrupt Level register
 * 	CWP - Current Window Pointer register
 * 	VER - Version register
 * 	CCR - Condition Codes Register
 * 	PSTATE - Processor State register
 */

/*
 *  PSTATE masks and starting bit positions
 *
 *  NOTE: Reserved bits are ignored.
 */

#define SPARC_PSTATE_AG_MASK   0x00000001   /* bit  0 */
#define SPARC_PSTATE_IE_MASK   0x00000002   /* bit  1 */
#define SPARC_PSTATE_PRIV_MASK 0x00000004   /* bit  2 */
#define SPARC_PSTATE_AM_MASK   0x00000008   /* bit  3 */
#define SPARC_PSTATE_PEF_MASK  0x00000010   /* bit  4 */
#define SPARC_PSTATE_MM_MASK   0x00000040   /* bit  6 */
#define SPARC_PSTATE_TLE_MASK  0x00000100   /* bit  8 */
#define SPARC_PSTATE_CLE_MASK  0x00000200   /* bit  9 */

#define SPARC_PSTATE_AG_BIT_POSITION   0   /* bit  0 */
#define SPARC_PSTATE_IE_BIT_POSITION   1   /* bit  1 */
#define SPARC_PSTATE_PRIV_BIT_POSITION 2   /* bit  2 */
#define SPARC_PSTATE_AM_BIT_POSITION   3   /* bit  3 */
#define SPARC_PSTATE_PEF_BIT_POSITION  4   /* bit  4 */
#define SPARC_PSTATE_MM_BIT_POSITION   6   /* bit  6 */
#define SPARC_PSTATE_TLE_BIT_POSITION  8   /* bit  8 */
#define SPARC_PSTATE_CLE_BIT_POSITION  9   /* bit  9 */

#define SPARC_FPRS_FEF_MASK     0x0100 /* bit 2 */
#define SPARC_FPRS_FEF_BIT_POSITION 2      /* bit 2 */

#define SPARC_TSTATE_IE_MASK  0x00000200  /* bit 9 */

#define SPARC_SOFTINT_TM_MASK 0x00000001    /* bit 0 */
#define SPARC_SOFTINT_SM_MASK 0x00010000    /* bit 16 */
#define SPARC_SOFTINT_TM_BIT_POSITION    1       /* bit 0 */
#define SPARC_SOFTINT_SM_BIT_POSITION    17      /* bit 16 */

#define STACK_BIAS (2047)

#ifdef ASM

/* 
 * To enable the FPU we need to set both PSTATE.pef and FPRS.fef
 */

#define sparc64_enable_FPU(rtmp1) \
	rdpr %pstate, rtmp1; \
	or rtmp1, SPARC_PSTATE_PEF_MASK, rtmp1; \
	wrpr %g0, rtmp1, %pstate; \
	rd %fprs, rtmp1; \
	or rtmp1, SPARC_FPRS_FEF_MASK, rtmp1; \
	wr %g0, rtmp1, %fprs


#endif

#ifndef ASM

/*
 *  Standard nop
 */

#define nop() \
  do { \
    __asm__ volatile ( "nop" ); \
  } while ( 0 )

/*
 *  Get and set the pstate
 */

#define sparc64_get_pstate( _pstate ) \
  do { \
     (_pstate) = 0; \
     __asm__ volatile( "rdpr %%pstate, %0" :  "=r" (_pstate) : "0" (_pstate) ); \
  } while ( 0 )

#define sparc64_set_pstate( _pstate ) \
  do { \
    __asm__ volatile ( \
      "wrpr  %%g0, %0, %%pstate " : "=r" ((_pstate)) : "0" ((_pstate)) ); \
  } while ( 0 )

/*
 * Get and set the PIL
 */

#define sparc64_get_pil( _pil ) \
  do { \
     (_pil) = 0; \
     __asm__ volatile( "rdpr %%pil, %0" :  "=r" (_pil) : "0" (_pil) ); \
  } while ( 0 )

#define sparc64_set_pil( _pil ) \
  do { \
    __asm__ volatile ( "wrpr  %%g0, %0, %%pil " : "=r" ((_pil)) : "0" ((_pil)) ); \
  } while ( 0 )


/*
 *  Get and set the TBA
 */

#define sparc64_get_tba( _tba ) \
  do { \
     (_tba) = 0; /* to avoid unitialized warnings */ \
     __asm__ volatile( "rdpr %%tba, %0" :  "=r" (_tba) : "0" (_tba) ); \
  } while ( 0 )

#define sparc64_set_tba( _tba ) \
  do { \
     __asm__ volatile( "wrpr %%g0, %0, %%tba" :  "=r" (_tba) : "0" (_tba) ); \
  } while ( 0 )

/*
 *  Get and set the TL (trap level)
 */

#define sparc64_get_tl( _tl ) \
  do { \
     (_tl) = 0; /* to avoid unitialized warnings */ \
     __asm__ volatile( "rdpr %%tl, %0" :  "=r" (_tl) : "0" (_tl) ); \
  } while ( 0 )

#define sparc64_set_tl( _tl ) \
  do { \
     __asm__ volatile( "wrpr %%g0, %0, %%tl" :  "=r" (_tl) : "0" (_tl) ); \
  } while ( 0 )


/*
 * read the stick register
 *
 * Note: 
 * stick asr=24, mnemonic=stick
 * Note: stick does not appear to be a valid ASR for US3, although it is 
 * implemented in US3i.
 */
#define sparc64_read_stick( _stick ) \
  do { \
    (_stick) = 0; \
    __asm__ volatile( "rd %%stick, %0" : "=r" (_stick) : "0" (_stick) );  \
  } while ( 0 )

/*
 * write the stick_cmpr register 
 *
 * Note: 
 * stick_cmpr asr=25, mnemonic=stick_cmpr
 * Note: stick_cmpr does not appear to be a valid ASR for US3, although it is 
 * implemented in US3i.
 */
#define sparc64_write_stick_cmpr( _stick_cmpr ) \
  do { \
    __asm__ volatile( "wr %%g0, %0, %%stick_cmpr" :  "=r" (_stick_cmpr)  \
                                              :  "0" (_stick_cmpr) ); \
  } while ( 0 )

/*
 * read the Tick register
 */
#define sparc64_read_tick( _tick ) \
  do { \
    (_tick) = 0; \
    __asm__ volatile( "rd %%tick, %0" : "=r" (_tick) : "0" (_tick) ); \
  } while ( 0 )

/*
 * write the tick_cmpr register
 */
#define sparc64_write_tick_cmpr( _tick_cmpr ) \
  do { \
    __asm__ volatile( "wr %%g0, %0, %%tick_cmpr" :  "=r" (_tick_cmpr)  \
                                             :  "0" (_tick_cmpr) ); \
  } while ( 0 )

/* 
 * Clear the softint register.
 *
 * sun4u and sun4v: softint_clr asr = 21, with mnemonic clear_softint
 */
#define sparc64_clear_interrupt_bits( _bit_mask ) \
  do { \
  __asm__ volatile( "wr %%g0, %0, %%clear_softint" : "=r" (_bit_mask) \
                                               : "0" (_bit_mask)); \
  } while ( 0 )

/************* DEPRECATED ****************/
/* Note: Although the y register is deprecated, gcc still uses it */
/*
 *  Get and set the Y
 */
 
#define sparc_get_y( _y ) \
  do { \
    __asm__ volatile( "rd %%y, %0" :  "=r" (_y) : "0" (_y) ); \
  } while ( 0 )
 
#define sparc_set_y( _y ) \
  do { \
    __asm__ volatile( "wr %0, %%y" :  "=r" (_y) : "0" (_y) ); \
  } while ( 0 )

/************* /DEPRECATED ****************/

/*
 *  Manipulate the interrupt level in the pstate 
 */

uint32_t sparc_disable_interrupts(void);
void sparc_enable_interrupts(uint32_t);
  
#define sparc_flash_interrupts( _level ) \
  do { \
    register uint32_t   _ignored = 0; \
    \
    sparc_enable_interrupts( (_level) ); \
    _ignored = sparc_disable_interrupts(); \
  } while ( 0 )

#define sparc64_get_interrupt_level( _level ) \
  do { \
    _level = 0; \
    sparc64_get_pil( _level ); \
  } while ( 0 )

#endif /* !ASM */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_SPARC_H */
