/**
 * @file rtems/score/m68k.h
 */

/*
 *  This include file contains information pertaining to the Motorola
 *  m68xxx processor family.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_M68K_H
#define _RTEMS_SCORE_M68K_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This section contains the information required to build
 *  RTEMS for a particular member of the Motorola MC68xxx
 *  family.  It does this by setting variables to indicate
 *  which implementation dependent features are present in
 *  a particular member of the family.
 *
 *  Currently recognized:
 *     -m68000
 *     -m68000 -msoft-float
 *     -m68020
 *     -m68020 -msoft-float
 *     -m68030
 *     -m68040 -msoft-float
 *     -m68040
 *     -m68040 -msoft-float
 *     -m68060
 *     -m68060 -msoft-float
 *     -m68302        (no FP) (deprecated, use -m68000)
 *     -m68332        (no FP) (deprecated, use -mcpu32)
 *     -mcpu32        (no FP)
 *     -m5200         (no FP)
 *     -m528x         (no FP, ISA A+)
 *
 *  As of gcc 2.8.1 and egcs 1.1, there is no distinction made between
 *  the CPU32 and CPU32+.  The option -mcpu32 generates code which can
 *  be run on either core.  RTEMS distinguishes between these two cores
 *  because they have different alignment rules which impact performance.
 *  If you are using a CPU32+, then the symbol RTEMS__mcpu32p__ should
 *  be defined in your custom file (see make/custom/gen68360.cfg for an
 *  example of how to do this.  If gcc ever distinguishes between these
 *  two cores, then RTEMS__mcpu32p__ usage will be replaced with the
 *  appropriate compiler defined predefine.
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
 *
 *  M68K_HAS_EXTB_L is used to enable/disable usage of the extb.l instruction
 *  which is not available for 68000 or 68ec000 cores (68000, 68001, 68008,
 *  68010, 68302, 68306, 68307).  This instruction is available on the 68020
 *  up and the cpu32 based models.
 *
 *  M68K_HAS_MISALIGNED is non-zero if the CPU allows byte-misaligned
 *  data access (68020, 68030, 68040, 68060, CPU32+).
 *
 *  NOTE:
 *    Eventually it would be nice to evaluate doing a lot of this section
 *    by having each model specify which core it uses and then go from there.
 */

/*
 * Handle the Coldfire family based on the instruction set.
 */
#if defined(__mcoldfire__)

# define CPU_NAME "Motorola ColdFire"

# if defined(__mcfisaa__)
/* Motorola ColdFire ISA A */
# define CPU_MODEL_NAME         "mcfisaa"
# define M68K_HAS_VBR             1
# define M68K_HAS_BFFFO           0
# define M68K_HAS_SEPARATE_STACKS 0
# define M68K_HAS_PREINDEXING     0
# define M68K_HAS_EXTB_L          1
# define M68K_HAS_MISALIGNED      1

# elif defined(__mcfisaaplus__)
/* Motorola ColdFire ISA A+ */
# define CPU_MODEL_NAME         "mcfisaaplus"
# define M68K_HAS_VBR             1
# define M68K_HAS_BFFFO           0
# define M68K_HAS_SEPARATE_STACKS 0
# define M68K_HAS_PREINDEXING     0
# define M68K_HAS_EXTB_L          1
# define M68K_HAS_MISALIGNED      1

# elif defined(__mcfisab__)
/* Motorola ColdFire ISA B */
# define CPU_MODEL_NAME         "mcfisab"
# define M68K_HAS_VBR             1
# define M68K_HAS_BFFFO           0
# define M68K_HAS_SEPARATE_STACKS 0
# define M68K_HAS_PREINDEXING     0
# define M68K_HAS_EXTB_L          1
# define M68K_HAS_MISALIGNED      1

# else
# error "Unsupported Coldfire ISA -- Please notify RTEMS"
# endif

/*
 * Assume the FPU support is independent. I think it is just the ISA B
 * instruction set.
 */
# if defined (__mcffpu__)
# define M68K_HAS_FPU            1
  /*
   * td: can we be sure that all CFs with FPU also have an EMAC?
   */
# define M68K_HAS_EMAC           1
# define M68K_HAS_FPSP_PACKAGE   0
# else
# define M68K_HAS_FPU            0
# define M68K_HAS_FPSP_PACKAGE   0
# endif

/*
 * Tiny RTEMS support. Small stack and limited priorities.
 *
 * These CPUs have very limited on-CPU memory which cannot
 * be expanded. We have to be gentle with them or nothing
 * will every run.
 */
# if (defined(__mcf_cpu_52221) || \
      defined(__mcf_cpu_52223) || \
      defined(__mcf_cpu_52230) || \
      defined(__mcf_cpu_52231) || \
      defined(__mcf_cpu_52232) || \
      defined(__mcf_cpu_52233) || \
      defined(__mcf_cpu_52234) || \
      defined(__mcf_cpu_52235) || \
      defined(__mcf_cpu_52225) || \
      defined(__mcf_cpu_52235))
  #define M68K_CPU_STACK_MINIMUM_SIZE 1024
  /* Define the lowest priority. Based from 0 to this is 16 levels. */
  #define M68K_CPU_PRIORITY_MAXIMUM   15
# else
  #define M68K_CPU_STACK_MINIMUM_SIZE 4096
  /* Use the default number of priorities */
  #define M68K_CPU_PRIORITY_MAXIMUM   255
# endif

#else

/*
 *  Figure out all CPU Model Feature Flags based upon compiler
 *  predefines.   Notice the only exception to this is that
 *  gcc does not distinguish between CPU32 and CPU32+.  This
 *  feature selection logic is setup such that if RTEMS__mcpu32p__
 *  is defined, then CPU32+ rules are used.  Otherwise, the safe
 *  but less efficient CPU32 rules are used for the CPU32+.
 */

# define CPU_NAME "Motorola MC68xxx"

/*
 * One stack size fits all 68000 processors.
 */
# define M68K_CPU_STACK_MINIMUM_SIZE 4096

# if (defined(__mc68020__) && !defined(__mcpu32__))

# define CPU_MODEL_NAME          "m68020"
# define M68K_HAS_VBR             1
# define M68K_HAS_SEPARATE_STACKS 1
# define M68K_HAS_BFFFO           1
# define M68K_HAS_PREINDEXING     1
# define M68K_HAS_EXTB_L          1
# define M68K_HAS_MISALIGNED      1
#  if defined (__HAVE_68881__)
#  define M68K_HAS_FPU            1
#  define M68K_HAS_FPSP_PACKAGE   0
#  else
#  define M68K_HAS_FPU            0
#  define M68K_HAS_FPSP_PACKAGE   0
#  endif

# elif defined(__mc68030__)

# define CPU_MODEL_NAME          "m68030"
# define M68K_HAS_VBR             1
# define M68K_HAS_SEPARATE_STACKS 1
# define M68K_HAS_BFFFO           1
# define M68K_HAS_PREINDEXING     1
# define M68K_HAS_EXTB_L          1
# define M68K_HAS_MISALIGNED      1
#  if defined (__HAVE_68881__)
#  define M68K_HAS_FPU            1
#  define M68K_HAS_FPSP_PACKAGE   0
#  else
#  define M68K_HAS_FPU            0
#  define M68K_HAS_FPSP_PACKAGE   0
#  endif

# elif defined(__mc68040__)

# define CPU_MODEL_NAME          "m68040"
# define M68K_HAS_VBR             1
# define M68K_HAS_SEPARATE_STACKS 1
# define M68K_HAS_BFFFO           1
# define M68K_HAS_PREINDEXING     1
# define M68K_HAS_EXTB_L          1
# define M68K_HAS_MISALIGNED      1
#  if defined (__HAVE_68881__)
#  define M68K_HAS_FPU            1
#  define M68K_HAS_FPSP_PACKAGE   1
#  else
#  define M68K_HAS_FPU            0
#  define M68K_HAS_FPSP_PACKAGE   0
#  endif

# elif defined(__mc68060__)

# define CPU_MODEL_NAME          "m68060"
# define M68K_HAS_VBR             1
# define M68K_HAS_SEPARATE_STACKS 0
# define M68K_HAS_BFFFO           1
# define M68K_HAS_PREINDEXING     1
# define M68K_HAS_EXTB_L          1
# define M68K_HAS_MISALIGNED      1
#  if defined (__HAVE_68881__)
#  define M68K_HAS_FPU            1
#  define M68K_HAS_FPSP_PACKAGE   0
#  else
#  define M68K_HAS_FPU            0
#  define M68K_HAS_FPSP_PACKAGE   0
#  endif

# elif defined(__mc68302__)

# define CPU_MODEL_NAME          "m68302"
# define M68K_HAS_VBR             0
# define M68K_HAS_SEPARATE_STACKS 0
# define M68K_HAS_BFFFO           0
# define M68K_HAS_PREINDEXING     0
# define M68K_HAS_EXTB_L          0
# define M68K_HAS_MISALIGNED      0
# define M68K_HAS_FPU             0
# define M68K_HAS_FPSP_PACKAGE    0

  /* gcc and egcs do not distinguish between CPU32 and CPU32+ */
# elif defined(RTEMS__mcpu32p__)

# define CPU_MODEL_NAME          "mcpu32+"
# define M68K_HAS_VBR             1
# define M68K_HAS_SEPARATE_STACKS 0
# define M68K_HAS_BFFFO           0
# define M68K_HAS_PREINDEXING     1
# define M68K_HAS_EXTB_L          1
# define M68K_HAS_MISALIGNED      1
# define M68K_HAS_FPU             0
# define M68K_HAS_FPSP_PACKAGE    0

# elif defined(__mcpu32__)

# define CPU_MODEL_NAME          "mcpu32"
# define M68K_HAS_VBR             1
# define M68K_HAS_SEPARATE_STACKS 0
# define M68K_HAS_BFFFO           0
# define M68K_HAS_PREINDEXING     1
# define M68K_HAS_EXTB_L          1
# define M68K_HAS_MISALIGNED      0
# define M68K_HAS_FPU             0
# define M68K_HAS_FPSP_PACKAGE    0

# elif defined(__mc68000__)

# define CPU_MODEL_NAME          "m68000"
# define M68K_HAS_VBR             0
# define M68K_HAS_SEPARATE_STACKS 0
# define M68K_HAS_BFFFO           0
# define M68K_HAS_PREINDEXING     0
# define M68K_HAS_EXTB_L          0
# define M68K_HAS_MISALIGNED      0
#  if defined (__HAVE_68881__)
#  define M68K_HAS_FPU            1
#  define M68K_HAS_FPSP_PACKAGE   0
#  else
#  define M68K_HAS_FPU            0
#  define M68K_HAS_FPSP_PACKAGE   0
#  endif

# else

# error "Unsupported 68000 CPU model -- are you sure you're running a 68k compiler?"

# endif

/*
 * No Tiny RTEMS support on the standard 68000 family.
 */
# define M68K_CPU_STACK_MINIMUM_SIZE 4096
# define M68K_CPU_PRIORITY_MAXIMUM   255

#endif

/*
 * OBSOLETE: Backward compatibility only - Don't use.
 * Use __mcoldfire__ instead.
 */
#if defined(__mcoldfire__)
#define M68K_COLDFIRE_ARCH  1
#else
#define M68K_COLDFIRE_ARCH	0
#endif

#ifndef ASM

#if ( defined(__mcoldfire__) )
#define m68k_disable_interrupts( _level ) \
   do { register uint32_t   _tmpsr = 0x0700; \
        __asm__ volatile ( "move.w %%sr,%0\n\t" \
 		       "or.l   %0,%1\n\t" \
 		       "move.w %1,%%sr" \
 		       : "=d" (_level), "=d"(_tmpsr) : "1"(_tmpsr) \
               : "cc" ); \
   } while( 0 )
#else
#define m68k_disable_interrupts( _level ) \
  __asm__ volatile ( "move.w  %%sr,%0\n\t" \
                 "or.w    #0x0700,%%sr" \
                    : "=d" (_level) \
                    : : "cc" )
#endif

#define m68k_enable_interrupts( _level ) \
  __asm__ volatile ( "move.w  %0,%%sr " : : "d" (_level) : "cc");

#if ( defined(__mcoldfire__) )
#define m68k_flash_interrupts( _level ) \
   do { register uint32_t   _tmpsr = 0x0700; \
	asm volatile ( "move.w %2,%%sr\n\t" \
		       "or.l   %2,%1\n\t" \
		       "move.w %1,%%sr" \
		       : "=d"(_tmpsr) : "0"(_tmpsr), "d"(_level) \
               : "cc"); \
   } while( 0 )
#else
#define m68k_flash_interrupts( _level ) \
  __asm__ volatile ( "move.w  %0,%%sr\n\t" \
                 "or.w    #0x0700,%%sr" \
                    : : "d" (_level) \
                    : "cc" )
#endif

#define m68k_get_interrupt_level( _level ) \
  do { \
    register uint32_t   _tmpsr; \
    \
    __asm__ volatile( "move.w %%sr,%0" : "=d" (_tmpsr)); \
    _level = (_tmpsr & 0x0700) >> 8; \
  } while (0)

#define m68k_set_interrupt_level( _newlevel ) \
  do { \
    register uint32_t   _tmpsr; \
    \
    __asm__ volatile( "move.w  %%sr,%0" : "=d" (_tmpsr)); \
    _tmpsr = (_tmpsr & 0xf8ff) | ((_newlevel) << 8); \
    __asm__ volatile( "move.w  %0,%%sr" : : "d" (_tmpsr)); \
  } while (0)

#if ( M68K_HAS_VBR == 1 && !defined(__mcoldfire__) )
#define m68k_get_vbr( vbr ) \
  __asm__ volatile ( "movec   %%vbr,%0 " : "=r" (vbr))

#define m68k_set_vbr( vbr ) \
  __asm__ volatile ( "movec   %0,%%vbr " : : "r" (vbr))

#elif ( defined(__mcoldfire__) )
extern void*                     _VBR;
#define m68k_get_vbr( _vbr ) _vbr = &_VBR

#define m68k_set_vbr( _vbr ) \
  do { \
    __asm__ volatile ( "movec   %0,%%vbr " : : "r" (_vbr)); \
    _VBR = (void *)_vbr; \
  } while(0)

#else
#define m68k_get_vbr( _vbr ) _vbr = (void *)_VBR
#define m68k_set_vbr( _vbr )
#endif

/*
 *  Access Control Registers
 */
#define m68k_set_cacr(_cacr) __asm__ volatile ("movec %0,%%cacr" : : "d" (_cacr))
#define m68k_set_acr0(_acr0) __asm__ volatile ("movec %0,%%acr0" : : "d" (_acr0))
#define m68k_set_acr1(_acr1) __asm__ volatile ("movec %0,%%acr1" : : "d" (_acr1))

/*
 *  The following routine swaps the endian format of an unsigned int.
 *  It must be static because it is referenced indirectly.
 */
#if ( defined(__mcoldfire__) )

/* There are no rotate commands in Coldfire architecture. We will use
 * generic implementation of endian swapping for Coldfire.
 */
static inline uint32_t m68k_swap_u32(
  uint32_t value
  )
{
  uint32_t   byte1, byte2, byte3, byte4, swapped;

  byte4 = (value >> 24) & 0xff;
  byte3 = (value >> 16) & 0xff;
  byte2 = (value >> 8)  & 0xff;
  byte1 =  value        & 0xff;

  swapped = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
  return( swapped );
}

static inline uint16_t m68k_swap_u16(
  uint16_t value
)
{
  return (((value & 0xff) << 8) | ((value >> 8) & 0xff));
}

#else

static inline uint32_t m68k_swap_u32(
  uint32_t value
)
{
  uint32_t swapped = value;

  __asm__ volatile( "rorw  #8,%0" : "=d" (swapped) : "0" (swapped) );
  __asm__ volatile( "swap  %0"    : "=d" (swapped) : "0" (swapped) );
  __asm__ volatile( "rorw  #8,%0" : "=d" (swapped) : "0" (swapped) );

  return( swapped );
}

static inline uint16_t m68k_swap_u16(
  uint16_t value
)
{
  uint16_t swapped = value;

  __asm__ volatile( "rorw  #8,%0" : "=d" (swapped) : "0" (swapped) );

  return( swapped );
}
#endif

#define CPU_swap_u32( value )  m68k_swap_u32( value )
#define CPU_swap_u16( value )  m68k_swap_u16( value )


/*
 *  _CPU_virtual_to_physical
 *
 *  DESCRIPTION:
 *
 *	This function is used to map virtual addresses to physical
 *	addresses.
 *
 *	FIXME: ASSUMES THAT VIRTUAL ADDRESSES ARE THE SAME AS THE
 *	PHYSICAL ADDRESSES
 */
static inline void * _CPU_virtual_to_physical (
  const void * d_addr )
{
  return (void *) d_addr;
}


#endif  /* !ASM */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_M68K_H */
