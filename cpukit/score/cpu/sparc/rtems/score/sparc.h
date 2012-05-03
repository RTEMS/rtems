/**
 * @file rtems/score/sparc.h
 *
 * This file contains the information required to build
 * RTEMS for a particular member of the SPARC family.  It does
 * this by setting variables to indicate which implementation
 * dependent features are present in a particular member
 * of the family.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
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

/**
 *  Some higher end SPARCs have a bitscan instructions. It would
 *  be nice to take advantage of them.  Right now, there is no
 *  port to a CPU model with this feature and no (untested) code
 *  that is based on this feature flag.
 */
#define SPARC_HAS_BITSCAN                0

/**
 *  This should be OK until a port to a higher end SPARC processor
 *  is made that has more than 8 register windows.  If this cannot
 *  be determined based on multilib settings (v7/v8/v9), then the
 *  cpu_asm.S code that depends on this will have to move to libcpu.
 */
#define SPARC_NUMBER_OF_REGISTER_WINDOWS 8

/**
 *  This macro indicates whether this multilib variation has hardware
 *  floating point or not.  We use the gcc cpp predefine _SOFT_FLOAT
 *  to determine that.
 */
#if defined(_SOFT_FLOAT)
  #define SPARC_HAS_FPU 0
#else
  #define SPARC_HAS_FPU 1
#endif

/**
 *  This macro contains a string describing the multilib variant being
 *  build.
 */
#if SPARC_HAS_FPU
  #define CPU_MODEL_NAME "w/FPU"
#else
  #define CPU_MODEL_NAME "w/soft-float"
#endif

/**
 *  Define the name of the CPU family.
 */
#define CPU_NAME "SPARC"

/*
 *  Miscellaneous constants
 */

/**
 *  PSR masks and starting bit positions
 *
 *  @note Reserved bits are ignored.
 */
#if (SPARC_NUMBER_OF_REGISTER_WINDOWS == 8)
  #define SPARC_PSR_CWP_MASK               0x07   /* bits  0 -  4 */
#elif (SPARC_NUMBER_OF_REGISTER_WINDOWS == 16)
  #define SPARC_PSR_CWP_MASK               0x0F   /* bits  0 -  4 */
#elif (SPARC_NUMBER_OF_REGISTER_WINDOWS == 32)
  #define SPARC_PSR_CWP_MASK               0x1F   /* bits  0 -  4 */
#else
  #error "Unsupported number of register windows for this cpu"
#endif

/** This constant is a mask for the ET bits in the PSR. */
#define SPARC_PSR_ET_MASK   0x00000020   /* bit   5 */
/** This constant is a mask for the PS bits in the PSR. */
#define SPARC_PSR_PS_MASK   0x00000040   /* bit   6 */
/** This constant is a mask for the S bits in the PSR. */
#define SPARC_PSR_S_MASK    0x00000080   /* bit   7 */
/** This constant is a mask for the PIL bits in the PSR. */
#define SPARC_PSR_PIL_MASK  0x00000F00   /* bits  8 - 11 */
/** This constant is a mask for the EF bits in the PSR. */
#define SPARC_PSR_EF_MASK   0x00001000   /* bit  12 */
/** This constant is a mask for the EC bits in the PSR. */
#define SPARC_PSR_EC_MASK   0x00002000   /* bit  13 */
/** This constant is a mask for the ICC bits in the PSR. */
#define SPARC_PSR_ICC_MASK  0x00F00000   /* bits 20 - 23 */
/** This constant is a mask for the VER bits in the PSR. */
#define SPARC_PSR_VER_MASK  0x0F000000   /* bits 24 - 27 */
/** This constant is a mask for the IMPL bits in the PSR. */
#define SPARC_PSR_IMPL_MASK 0xF0000000   /* bits 28 - 31 */

/** This constant is the starting bit position of the CWP in the PSR. */
#define SPARC_PSR_CWP_BIT_POSITION   0   /* bits  0 -  4 */
/** This constant is the starting bit position of the ET in the PSR. */
#define SPARC_PSR_ET_BIT_POSITION    5   /* bit   5 */
/** This constant is the starting bit position of the PS in the PSR. */
#define SPARC_PSR_PS_BIT_POSITION    6   /* bit   6 */
/** This constant is the starting bit position of the S in the PSR. */
#define SPARC_PSR_S_BIT_POSITION     7   /* bit   7 */
/** This constant is the starting bit position of the PIL in the PSR. */
#define SPARC_PSR_PIL_BIT_POSITION   8   /* bits  8 - 11 */
/** This constant is the starting bit position of the EF in the PSR. */
#define SPARC_PSR_EF_BIT_POSITION   12   /* bit  12 */
/** This constant is the starting bit position of the EC in the PSR. */
#define SPARC_PSR_EC_BIT_POSITION   13   /* bit  13 */
/** This constant is the starting bit position of the ICC in the PSR. */
#define SPARC_PSR_ICC_BIT_POSITION  20   /* bits 20 - 23 */
/** This constant is the starting bit position of the VER in the PSR. */
#define SPARC_PSR_VER_BIT_POSITION  24   /* bits 24 - 27 */
/** This constant is the starting bit position of the IMPL in the PSR. */
#define SPARC_PSR_IMPL_BIT_POSITION 28   /* bits 28 - 31 */

#ifndef ASM

/**
 *  This macro is a standard nop instruction.
 */
#define nop() \
  do { \
    __asm__ volatile ( "nop" ); \
  } while ( 0 )

/**
 *  @brief Macro to Obtain the PSR
 *
 *  This macro returns the current contents of the PSR register in @a _psr.
 */
#define sparc_get_psr( _psr ) \
  do { \
     (_psr) = 0; \
     __asm__ volatile( "rd %%psr, %0" :  "=r" (_psr) : "0" (_psr) ); \
  } while ( 0 )

/**
 *  @brief Macro to Set the PSR
 *
 *  This macro sets the PSR register to the value in @a _psr.
 */
#define sparc_set_psr( _psr ) \
  do { \
    __asm__ volatile ( "mov  %0, %%psr " : "=r" ((_psr)) : "0" ((_psr)) ); \
    nop(); \
    nop(); \
    nop(); \
  } while ( 0 )

/**
 *  @brief Macro to Obtain the TBR
 *
 *  This macro returns the current contents of the TBR register in @a _tbr.
 */
#define sparc_get_tbr( _tbr ) \
  do { \
     (_tbr) = 0; /* to avoid unitialized warnings */ \
     __asm__ volatile( "rd %%tbr, %0" :  "=r" (_tbr) : "0" (_tbr) ); \
  } while ( 0 )

/**
 *  @brief Macro to Set the TBR
 *
 *  This macro sets the TBR register to the value in @a _tbr.
 */
#define sparc_set_tbr( _tbr ) \
  do { \
     __asm__ volatile( "wr %0, 0, %%tbr" :  "=r" (_tbr) : "0" (_tbr) ); \
  } while ( 0 )

/**
 *  @brief Macro to Obtain the WIM
 *
 *  This macro returns the current contents of the WIM field in @a _wim.
 */
#define sparc_get_wim( _wim ) \
  do { \
    __asm__ volatile( "rd %%wim, %0" :  "=r" (_wim) : "0" (_wim) ); \
  } while ( 0 )

/**
 *  @brief Macro to Set the WIM
 *
 *  This macro sets the WIM field to the value in @a _wim.
 */
#define sparc_set_wim( _wim ) \
  do { \
    __asm__ volatile( "wr %0, %%wim" :  "=r" (_wim) : "0" (_wim) ); \
    nop(); \
    nop(); \
    nop(); \
  } while ( 0 )

/**
 *  @brief Macro to Obtain the Y Register
 *
 *  This macro returns the current contents of the Y register in @a _y.
 */
#define sparc_get_y( _y ) \
  do { \
    __asm__ volatile( "rd %%y, %0" :  "=r" (_y) : "0" (_y) ); \
  } while ( 0 )

/**
 *  @brief Macro to Set the Y Register
 *
 *  This macro sets the Y register to the value in @a _y.
 */
#define sparc_set_y( _y ) \
  do { \
    __asm__ volatile( "wr %0, %%y" :  "=r" (_y) : "0" (_y) ); \
  } while ( 0 )

/**
 *  @brief SPARC Disable Processor Interrupts
 *
 *  This method is invoked to disable all maskable interrupts.
 *
 *  @return This method returns the entire PSR contents.
 */
uint32_t sparc_disable_interrupts(void);

/**
 *  @brief SPARC Enable Processor Interrupts
 *
 *  This method is invoked to enable all maskable interrupts.
 *
 *  @param[in] psr is the PSR returned by @ref sparc_disable_interrupts.
 */
void sparc_enable_interrupts(uint32_t psr);

/**
 *  @brief SPARC Flash Processor Interrupts
 *
 *  This method is invoked to temporarily enable all maskable interrupts.
 *
 *  @param[in] _psr is the PSR returned by @ref sparc_disable_interrupts.
 */
#define sparc_flash_interrupts( _psr ) \
  do { \
    sparc_enable_interrupts( (_psr) ); \
    _psr = sparc_disable_interrupts(); \
  } while ( 0 )

/**
 *  @brief SPARC Obtain Interrupt Level
 *
 *  This method is invoked to obtain the current interrupt disable level.
 *
 *  @param[in] _level is the PSR returned by @ref sparc_disable_interrupts.
 */
#define sparc_get_interrupt_level( _level ) \
  do { \
    register uint32_t   _psr_level = 0; \
    \
    sparc_get_psr( _psr_level ); \
    (_level) = \
      (_psr_level & SPARC_PSR_PIL_MASK) >> SPARC_PSR_PIL_BIT_POSITION; \
  } while ( 0 )

#endif

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_SPARC_H */
