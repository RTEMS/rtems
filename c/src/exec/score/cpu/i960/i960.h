/*  i960.h
 *
 *  This include file contains information pertaining to the Intel
 *  i960 processor family.
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

#ifndef __i960_h
#define __i960_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  The following define the CPU Family and Model within the family
 *
 *  NOTE: The string "REPLACE_THIS_WITH_THE_CPU_MODEL" is replaced
 *        with the name of the appropriate macro for this target CPU.
 */
 
#ifdef i960
#undef i960
#endif
#define i960

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
 *  RTEMS for a particular member of the Intel i960
 *  family.  It does this by setting variables to indicate
 *  which implementation dependent features are present
 *  in a particular member of the family.
 *
 *  NOTE: For now i960 is really the i960ca.  eventually need
 *        to put in at least support for FPU.
 */

#if defined(i960ca)

#define RTEMS_MODEL_NAME  "i960ca"
#define I960_HAS_FPU 0

#else

#error "Unsupported CPU Model"

#endif

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "Intel i960"

#ifndef ASM

/*
 * XXX    should have an ifdef here and have stuff for the other
 * XXX    family members...
 */
 
#if defined(__i960CA__) || defined(__i960_CA__) || defined(__i960CA)
 
/* i960CA control structures */
 
/* Intel i960CA Control Table */
 
typedef struct {
                            /* Control Group 0 */
  unsigned int ipb0;              /* IP breakpoint 0 */
  unsigned int ipb1;              /* IP breakpoint 1 */
  unsigned int dab0;              /* data address breakpoint 0 */
  unsigned int dab1;              /* data address breakpoint 1 */
                            /* Control Group 1 */
  unsigned int imap0;             /* interrupt map 0 */
  unsigned int imap1;             /* interrupt map 1 */
  unsigned int imap2;             /* interrupt map 2 */
  unsigned int icon;              /* interrupt control */
                            /* Control Group 2 */
  unsigned int mcon0;             /* memory region 0 configuration */
  unsigned int mcon1;             /* memory region 1 configuration */
  unsigned int mcon2;             /* memory region 2 configuration */
  unsigned int mcon3;             /* memory region 3 configuration */
                            /* Control Group 3 */
  unsigned int mcon4;             /* memory region 4 configuration */
  unsigned int mcon5;             /* memory region 5 configuration */
  unsigned int mcon6;             /* memory region 6 configuration */
  unsigned int mcon7;             /* memory region 7 configuration */
                            /* Control Group 4 */
  unsigned int mcon8;             /* memory region 8 configuration */
  unsigned int mcon9;             /* memory region 9 configuration */
  unsigned int mcon10;            /* memory region 10 configuration */
  unsigned int mcon11;            /* memory region 11 configuration */
                            /* Control Group 5 */
  unsigned int mcon12;            /* memory region 12 configuration */
  unsigned int mcon13;            /* memory region 13 configuration */
  unsigned int mcon14;            /* memory region 14 configuration */
  unsigned int mcon15;            /* memory region 15 configuration */
                            /* Control Group 6 */
  unsigned int bpcon;             /* breakpoint control */
  unsigned int tc;                /* trace control */
  unsigned int bcon;              /* bus configuration control */
  unsigned int reserved;          /* reserved */
}   i960ca_control_table;
 
/* Intel i960CA Processor Control Block */
 
typedef struct {
  unsigned int    *fault_tbl;     /* fault table base address */
  i960ca_control_table
                  *control_tbl;   /* control table base address */
  unsigned int     initial_ac;    /* AC register initial value */
  unsigned int     fault_config;  /* fault configuration word */
  void           **intr_tbl;      /* interrupt table base address */
  void            *sys_proc_tbl;  /* system procedure table
                                     base address */
  unsigned int     reserved;      /* reserved */
  unsigned int    *intr_stack;    /* interrupt stack pointer */
  unsigned int     ins_cache_cfg; /* instruction cache
                                     configuration word */
  unsigned int     reg_cache_cfg; /* register cache configuration word */
}   i960ca_PRCB;
 
#endif

/*
 *  Interrupt Level Routines
 */

#define i960_disable_interrupts( oldlevel ) \
  { (oldlevel) = 0x1f0000; \
    asm volatile ( "modpc   0,%1,%1" \
                       : "=d" ((oldlevel)) \
                       : "0"  ((oldlevel)) ); \
  }

#define i960_enable_interrupts( oldlevel ) \
  { unsigned int _mask = 0x1f0000; \
    asm volatile ( "modpc   0,%0,%1" \
                       : "=d" (_mask), "=d" ((oldlevel)) \
                       : "0"  (_mask), "1"  ((oldlevel)) ); \
  }

#define i960_flash_interrupts( oldlevel ) \
  { unsigned int _mask = 0x1f0000; \
    asm volatile ( "modpc   0,%0,%1 ; \
                    mov     %0,%1 ; \
                    modpc   0,%0,%1"  \
                       : "=d" (_mask), "=d" ((oldlevel)) \
                       : "0"  (_mask), "1"  ((oldlevel)) ); \
  }

#define i960_get_interrupt_level( _level ) \
  { \
    i960_disable_interrupts( _level ); \
    i960_enable_interrupts( _level ); \
    (_level) = ((_level) & 0x1f0000) >> 16; \
  } while ( 0 )

#define i960_atomic_modify( mask, addr, prev ) \
 { register unsigned int  _mask = (mask); \
   register unsigned int *_addr = (unsigned int *)(addr); \
   asm volatile( "atmod  %0,%1,%1" \
                  : "=d" (_addr), "=d" (_mask) \
                  : "0"  (_addr), "1"  (_mask) ); \
   (prev) = _mask; \
 }


#define atomic_modify( _mask, _address, _previous ) \
  i960_atomic_modify( _mask, _address, _previous )

#define i960_enable_tracing() \
 { register unsigned int _pc = 0x1; \
   asm volatile( "modpc 0,%0,%0" : "=d" (_pc) : "0" (_pc) ); \
 }

#define i960_unmask_intr( xint ) \
 { register unsigned int _mask= (1<<(xint)); \
   asm volatile( "or sf1,%0,sf1" : "=d" (_mask) : "0" (_mask) ); \
 }

#define i960_mask_intr( xint ) \
 { register unsigned int _mask= (1<<(xint)); \
   asm volatile( "andnot %0,sf1,sf1" : "=d" (_mask) : "0" (_mask) ); \
 }

#define i960_clear_intr( xint ) \
 { register unsigned int _xint=(xint); \
asm volatile( "loop_til_cleared: clrbit %0,sf0,sf0 ; \
                  bbs    %0,sf0, loop_til_cleared" \
                  : "=d" (_xint) : "0" (_xint) ); \
 }

#define i960_reload_ctl_group( group ) \
 { register int _cmd = ((group)|0x400) ; \
   asm volatile( "sysctl %0,%0,%0" : "=d" (_cmd) : "0" (_cmd) ); \
 }

#define i960_cause_intr( intr ) \
 { register int _intr = (intr); \
   asm volatile( "sysctl %0,%0,%0" : "=d" (_intr) : "0" (_intr) ); \
 }

#define i960_soft_reset( prcb ) \
 { register i960ca_PRCB *_prcb = (prcb); \
   register unsigned int         *_next=0; \
   register unsigned int          _cmd  = 0x30000; \
   asm volatile( "lda    next,%1; \
                  sysctl %0,%1,%2; \
            next: mov    g0,g0" \
                  : "=d" (_cmd), "=d" (_next), "=d" (_prcb) \
                  : "0"  (_cmd), "1"  (_next), "2"  (_prcb) ); \
 }

static inline unsigned int i960_pend_intrs()
{ register unsigned int _intr=0;
  asm volatile( "mov sf0,%0" : "=d" (_intr) : "0" (_intr) );
  return ( _intr );
}

static inline unsigned int i960_mask_intrs()
{ register unsigned int _intr=0;
  asm volatile( "mov sf1,%0" : "=d" (_intr) : "0" (_intr) );
  return( _intr );
}

static inline unsigned int i960_get_fp()
{ register unsigned int _fp=0;
  asm volatile( "mov fp,%0" : "=d" (_fp) : "0" (_fp) );
  return ( _fp );
}

/*
 *  The following routine swaps the endian format of an unsigned int.
 *  It must be static because it is referenced indirectly.
 *
 *  This version is based on code presented in Vol. 4, No. 4 of
 *  Insight 960.  It is certainly something you wouldn't think
 *  of on your own.
 */

static inline unsigned int CPU_swap_u32(
  unsigned int value
)
{
  register unsigned int to_swap = value;
  register unsigned int temp    = 0xFF00FF00;
  register unsigned int swapped = 0;

                                            /*  to_swap      swapped  */
  asm volatile ( "rotate  16,%0,%2 ;"       /* 0x12345678  0x56781234 */
                 "modify  %1,%0,%2 ;"       /* 0x12345678  0x12785634 */
                 "rotate  8,%2,%2"          /* 0x12345678  0x78563412 */
                 : "=r" (to_swap), "=r" (temp), "=r" (swapped)
                 : "0" (to_swap), "1" (temp), "2" (swapped)
               );
  return( swapped );
}

#ifdef __cplusplus
}
#endif

#endif /* !ASM */

#endif
/* end of include file */
