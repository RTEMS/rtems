/**
 * @file
 *
 * @brief Information Required to Build RTEMS for a Particular Member
 * of the SPARC Family
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
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SPARC_H
#define _RTEMS_SCORE_SPARC_H

#include <rtems/score/basedefs.h>

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
 * Some higher end SPARCs have a bitscan instructions. It would
 * be nice to take advantage of them.  Right now, there is no
 * port to a CPU model with this feature and no (untested) code
 * that is based on this feature flag.
 */
#define SPARC_HAS_BITSCAN                0

/**
 * This should be OK until a port to a higher end SPARC processor
 * is made that has more than 8 register windows.  If this cannot
 * be determined based on multilib settings (v7/v8/v9), then the
 * cpu_asm.S code that depends on this will have to move to libcpu.
 */
#define SPARC_NUMBER_OF_REGISTER_WINDOWS 8

/**
 * See GRLIB-TN-0009: "LEON3FT Stale Cache Entry After Store with
 * Data Tag Parity Error"
 */
#if defined(__FIX_LEON3FT_B2BST)
  #define SPARC_LEON3FT_B2BST_NOP nop
#else
  #define SPARC_LEON3FT_B2BST_NOP
#endif

/**
 * This macro indicates whether this multilib variation has hardware
 * floating point or not.  We use the gcc cpp predefine _SOFT_FLOAT
 * to determine that.
 */
#if defined(_SOFT_FLOAT)
  #define SPARC_HAS_FPU 0
#else
  #define SPARC_HAS_FPU 1
#endif

/**
 * This macro contains a string describing the multilib variant being
 * build.
 */
#if SPARC_HAS_FPU
  #define CPU_MODEL_NAME "w/FPU"
#else
  #define CPU_MODEL_NAME "w/soft-float"
#endif

/**
 * Define the name of the CPU family.
 */
#define CPU_NAME "SPARC"

/*
 *  Miscellaneous constants
 */

/**
 * PSR masks and starting bit positions
 *
 * NOTE: Reserved bits are ignored.
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

#define LEON3_ASR17_PROCESSOR_INDEX_SHIFT 28

/* SPARC Software Trap number definitions */
#define SPARC_SWTRAP_SYSCALL 0
#define SPARC_SWTRAP_IRQDIS 9
#define SPARC_SWTRAP_IRQEN 10
#if SPARC_HAS_FPU == 1
#define SPARC_SWTRAP_IRQDIS_FP 11
#endif

/**
 * @brief This is the bit step in a vector number to indicate it is being
 *   installed as a synchronous trap.
 */
#define SPARC_SYNCHRONOUS_TRAP_BIT_MASK 0x100

/**
 * @brief Maps the real hardware vector number to the associated asynchronous
 *   trap number.
 *
 * @param _vector is the real hardware vector number to map.
 *
 * @return Returns the asynchronous trap number associated with the real
 *   hardware vector number.
 */
#define SPARC_ASYNCHRONOUS_TRAP( _vector ) ( _vector )

/**
 * @brief Maps the real hardware vector number to the associated synchronous
 *   trap number.
 *
 * @param _vector is the real hardware vector number to map.
 *
 * @return Returns the synchronous trap number associated with the
 *   real hardware vector number.
 */
#define SPARC_SYNCHRONOUS_TRAP( _vector ) ( ( _vector ) + 256 )

/**
 * @brief Maps the synchronous or asynchronous trap number to the associated
 *   real hardware vector number.
 *
 * @param _trap is the synchronous or asynchronous trap number to map.
 *
 * @return Returns the real hardware vector number associated with the
 *   synchronous or asynchronous trap number.
 */
#define SPARC_REAL_TRAP_NUMBER( _trap ) ( ( _trap ) % 256 )

/**
 * @brief Checks if the real hardware vector number, synchronous trap number,
 *   or asynchronous trap number is an interrupt trap.
 *
 * Interrupt traps are defined by Table 7-1 "Exception and Interrupt Request
 * Priority and tt Values" in "The SPARC Architecture Manual: Version 8".
 *
 * @param _trap is the real hardware vector number, synchronous trap number, or
 *   asynchronous trap number to check.
 *
 * @return Returns true, if the real hardware vector number, synchronous trap
 *   number, or asynchronous trap number is an interrupt trap, otherwise false.
 */
#define SPARC_IS_INTERRUPT_TRAP( _trap ) \
  ( SPARC_REAL_TRAP_NUMBER( _trap ) >= 0x11 && \
    SPARC_REAL_TRAP_NUMBER( _trap ) <= 0x1f )

/**
 * @brief Maps the interrupt trap number to the associated interrupt source
 *   number.
 *
 * Interrupt trap numbers are real hardware vector numbers, synchronous trap
 * numbers, and asynchronous trap numbers for which SPARC_IS_INTERRUPT_TRAP()
 * returns true.
 *
 * @param _trap is the real hardware vector number, synchronous trap number, or
 *   asynchronous trap number to map.
 *
 * @return Returns the interrupt source number associated with the interrupt
 *   trap number.
 */
#define SPARC_INTERRUPT_TRAP_TO_SOURCE( _trap ) \
  ( SPARC_REAL_TRAP_NUMBER( _trap ) - 0x10 )

/**
 * @brief Maps the interrupt source number to the associated asynchronous trap
 *   number.
 *
 * @param _source is the interrupt source number to map.
 *
 * @return Returns the asynchronous trap number associated with the interrupt
 *   source number.
 */
#define SPARC_INTERRUPT_SOURCE_TO_TRAP( _source ) \
  ( SPARC_ASYNCHRONOUS_TRAP( _source ) + 0x10 )

#ifndef ASM

/**
 * This macro is a standard nop instruction.
 */
#define nop() \
  do { \
    __asm__ volatile ( "nop" ); \
  } while ( 0 )

/**
 * @brief Macro to obtain the PSR.
 *
 * This macro returns the current contents of the PSR register in @a _psr.
 */
#if defined(RTEMS_PARAVIRT)

uint32_t _SPARC_Get_PSR( void );

#define sparc_get_psr( _psr ) \
  (_psr) = _SPARC_Get_PSR()

#else /* RTEMS_PARAVIRT */

#define sparc_get_psr( _psr ) \
  do { \
     (_psr) = 0; \
     __asm__ volatile( "rd %%psr, %0" :  "=r" (_psr) : "0" (_psr) ); \
  } while ( 0 )

#endif /* RTEMS_PARAVIRT */

/**
 * @brief Macro to set the PSR.
 *
 * This macro sets the PSR register to the value in @a _psr.
 */
#if defined(RTEMS_PARAVIRT)

void _SPARC_Set_PSR( uint32_t new_psr );

#define sparc_set_psr( _psr ) \
  _SPARC_Set_PSR( _psr )

#else /* RTEMS_PARAVIRT */

#define sparc_set_psr( _psr ) \
  do { \
    __asm__ volatile ( "mov  %0, %%psr " : "=r" ((_psr)) : "0" ((_psr)) ); \
    nop(); \
    nop(); \
    nop(); \
  } while ( 0 )

#endif /* RTEMS_PARAVIRT */

/**
 * @brief Macro to obtain the TBR.
 *
 * This macro returns the current contents of the TBR register in @a _tbr.
 */
#if defined(RTEMS_PARAVIRT)

uint32_t _SPARC_Get_TBR( void );

#define sparc_get_tbr( _tbr ) \
  (_tbr) = _SPARC_Get_TBR()

#else /* RTEMS_PARAVIRT */

#define sparc_get_tbr( _tbr ) \
  do { \
     (_tbr) = 0; /* to avoid unitialized warnings */ \
     __asm__ volatile( "rd %%tbr, %0" :  "=r" (_tbr) : "0" (_tbr) ); \
  } while ( 0 )

#endif /* RTEMS_PARAVIRT */

/**
 * @brief Macro to set the TBR.
 *
 * This macro sets the TBR register to the value in @a _tbr.
 */
#if defined(RTEMS_PARAVIRT)

void _SPARC_Set_TBR( uint32_t new_tbr );

#define sparc_set_tbr( _tbr ) \
  _SPARC_Set_TBR((_tbr))

#else /* RTEMS_PARAVIRT */

#define sparc_set_tbr( _tbr ) \
  do { \
     __asm__ volatile( "wr %0, 0, %%tbr" :  "=r" (_tbr) : "0" (_tbr) ); \
  } while ( 0 )

#endif /* RTEMS_PARAVIRT */

/**
 * @brief Macro to obtain the WIM.
 *
 * This macro returns the current contents of the WIM field in @a _wim.
 */
#define sparc_get_wim( _wim ) \
  do { \
    __asm__ volatile( "rd %%wim, %0" :  "=r" (_wim) : "0" (_wim) ); \
  } while ( 0 )

/**
 * @brief Macro to set the WIM.
 *
 * This macro sets the WIM field to the value in @a _wim.
 */
#define sparc_set_wim( _wim ) \
  do { \
    __asm__ volatile( "wr %0, %%wim" :  "=r" (_wim) : "0" (_wim) ); \
    nop(); \
    nop(); \
    nop(); \
  } while ( 0 )

/**
 * @brief Macro to obtain the Y register.
 *
 * This macro returns the current contents of the Y register in @a _y.
 */
#define sparc_get_y( _y ) \
  do { \
    __asm__ volatile( "rd %%y, %0" :  "=r" (_y) : "0" (_y) ); \
  } while ( 0 )

/**
 * @brief Macro to set the Y register.
 *
 * This macro sets the Y register to the value in @a _y.
 */
#define sparc_set_y( _y ) \
  do { \
    __asm__ volatile( "wr %0, %%y" :  "=r" (_y) : "0" (_y) ); \
  } while ( 0 )

/**
 * @brief SPARC disable processor interrupts.
 *
 * This method is invoked to disable all maskable interrupts.
 *
 * @return This method returns the entire PSR contents.
 */
static inline uint32_t sparc_disable_interrupts(void)
{
  register uint32_t psr __asm__("g1"); /* return value of trap handler */
  __asm__ volatile ( "ta %1\n\t" : "=r" (psr) : "i" (SPARC_SWTRAP_IRQDIS));
  return psr;
}

/**
 * @brief SPARC enable processor interrupts.
 *
 * This method is invoked to enable all maskable interrupts.
 *
 * @param[in] psr is the PSR returned by @ref sparc_disable_interrupts.
 */
static inline void sparc_enable_interrupts(uint32_t psr)
{
  register uint32_t _psr __asm__("g1") = psr; /* input to trap handler */

  /*
   * The trap instruction has a higher trap priority than the interrupts
   * according to "The SPARC Architecture Manual: Version 8", Table 7-1
   * "Exception and Interrupt Request Priority and tt Values".  Add a nop to
   * prevent a trap instruction right after the interrupt enable trap.
   */
  __asm__ volatile ( "ta %0\nnop\n" :: "i" (SPARC_SWTRAP_IRQEN), "r" (_psr));
}

/**
 * @brief SPARC exit through system call 1
 *
 * This method is invoked to go into system error halt. The optional
 * arguments can be given to hypervisor, hardware debugger, simulator or
 * similar.
 *
 * System error mode is entered when taking a trap when traps have been
 * disabled. What happens when error mode is entered depends on the motherboard.
 * In a typical development systems the CPU relingish control to the debugger,
 * simulator, hypervisor or similar. The following steps are taken:
 *
 * 1. Going into system error mode by Software Trap 0
 * 2. %g1=1 (syscall 1 - Exit)
 * 3. %g2=Primary exit code
 * 4. %g3=Secondary exit code. Dependends on %g2 exit type.
 *
 * This function never returns.
 *
 * @param[in] exitcode1 Primary exit code stored in CPU g2 register after exit
 * @param[in] exitcode2 Primary exit code stored in CPU g3 register after exit
 */
RTEMS_NO_RETURN void sparc_syscall_exit(
  uint32_t exitcode1,
  uint32_t exitcode2
);

/**
 * @brief SPARC flash processor interrupts.
 *
 * This method is invoked to temporarily enable all maskable interrupts.
 *
 * @param[in] _psr is the PSR returned by @ref sparc_disable_interrupts.
 */
#define sparc_flash_interrupts( _psr ) \
  do { \
    sparc_enable_interrupts( (_psr) ); \
    _psr = sparc_disable_interrupts(); \
  } while ( 0 )

/**
 * @brief SPARC obtain interrupt level.
 *
 * This method is invoked to obtain the current interrupt disable level.
 *
 * @param[in] _level is the PSR returned by @ref sparc_disable_interrupts.
 */
#define sparc_get_interrupt_level( _level ) \
  do { \
    uint32_t _psr_level = 0; \
    \
    sparc_get_psr( _psr_level ); \
    (_level) = \
      (_psr_level & SPARC_PSR_PIL_MASK) >> SPARC_PSR_PIL_BIT_POSITION; \
  } while ( 0 )

static inline uint32_t _LEON3_Get_current_processor( void )
{
  uint32_t asr17;

  __asm__ volatile (
    "rd %%asr17, %0"
    : "=&r" (asr17)
  );

  return asr17 >> LEON3_ASR17_PROCESSOR_INDEX_SHIFT;
}

#endif /* ASM */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_SPARC_H */
