/*
 * cpu.h
 *
 * This file contains some powerpc MSR and registers access definitions.
 *
 * It is a stripped down version of linux ppc processor.h file...
 *
 * Copyright (C) 1999  Eric Valette (valette@crf.canon.fr)
 *                     Canon Centre Recherche France.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __PPC_CPU_H
#define __PPC_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

/* Bit encodings for Machine State Register (MSR) */
#define MSR_POW		(1<<18)		/* Enable Power Management */
#define MSR_TGPR	(1<<17)		/* TLB Update registers in use */
#define MSR_ILE		(1<<16)		/* Interrupt Little-Endian enable */
#define MSR_EE		(1<<15)		/* External Interrupt enable */
#define MSR_PR		(1<<14)		/* Supervisor/User privilege */
#define MSR_FP		(1<<13)		/* Floating Point enable */
#define MSR_ME		(1<<12)		/* Machine Check enable */
#define MSR_FE0		(1<<11)		/* Floating Exception mode 0 */
#define MSR_SE		(1<<10)		/* Single Step */
#define MSR_BE		(1<<9)		/* Branch Trace */
#define MSR_FE1		(1<<8)		/* Floating Exception mode 1 */
#define MSR_IP		(1<<6)		/* Exception prefix 0x000/0xFFF */
#define MSR_IR		(1<<5)		/* Instruction MMU enable */
#define MSR_DR		(1<<4)		/* Data MMU enable */
#define MSR_RI		(1<<1)		/* Recoverable Exception */
#define MSR_LE		(1<<0)		/* Little-Endian enable */

#define MSR_		MSR_ME|MSR_RI
#define MSR_KERNEL      MSR_|MSR_IR|MSR_DR
#define MSR_USER	MSR_KERNEL|MSR_PR|MSR_EE

/* Bit encodings for Hardware Implementation Register (HID0)
   on PowerPC 603, 604, etc. processors (not 601). */
#define HID0_EMCP	(1<<31)		/* Enable Machine Check pin */
#define HID0_EBA	(1<<29)		/* Enable Bus Address Parity */
#define HID0_EBD	(1<<28)		/* Enable Bus Data Parity */
#define HID0_SBCLK	(1<<27)
#define HID0_EICE	(1<<26)
#define HID0_ECLK	(1<<25)
#define HID0_PAR	(1<<24)
#define HID0_DOZE	(1<<23)
#define HID0_NAP	(1<<22)
#define HID0_SLEEP	(1<<21)
#define HID0_DPM	(1<<20)
#define HID0_ICE	(1<<15)		/* Instruction Cache Enable */
#define HID0_DCE	(1<<14)		/* Data Cache Enable */
#define HID0_ILOCK	(1<<13)		/* Instruction Cache Lock */
#define HID0_DLOCK	(1<<12)		/* Data Cache Lock */
#define HID0_ICFI	(1<<11)		/* Instruction Cache Flash Invalidate */
#define HID0_DCI	(1<<10)		/* Data Cache Invalidate */
#define HID0_SIED	(1<<7)		/* Serial Instruction Execution [Disable] */
#define HID0_BHTE	(1<<2)		/* Branch History Table Enable */
#define HID0_BTCD	(1<<1)		/* Branch target cache disable */

/* fpscr settings */
#define FPSCR_FX        (1<<31)
#define FPSCR_FEX       (1<<30)

#define _MACH_prep     1
#define _MACH_Pmac     2 /* pmac or pmac clone (non-chrp) */
#define _MACH_chrp     4 /* chrp machine */
#define _MACH_mbx      8 /* Motorola MBX board */
#define _MACH_apus    16 /* amiga with phase5 powerup */
#define _MACH_fads    32 /* Motorola FADS board */

/* see residual.h for these */
#define _PREP_Motorola 0x01  /* motorola prep */
#define _PREP_Firm     0x02  /* firmworks prep */
#define _PREP_IBM      0x00  /* ibm prep */
#define _PREP_Bull     0x03  /* bull prep */

/* these are arbitrary */
#define _CHRP_Motorola 0x04  /* motorola chrp, the cobra */
#define _CHRP_IBM     0x05   /* IBM chrp, the longtrail and longtrail 2 */

#define _GLOBAL(n)\
	.globl n;\
n:

#define	TBRU	269	/* Time base Upper/Lower (Reading) */
#define	TBRL	268
#define TBWU	284	/* Time base Upper/Lower (Writing) */
#define TBWL	285
#define	XER	1
#define LR	8
#define CTR	9
#define HID0	1008	/* Hardware Implementation */
#define PVR	287	/* Processor Version */
#define IBAT0U	528	/* Instruction BAT #0 Upper/Lower */
#define IBAT0L	529
#define IBAT1U	530	/* Instruction BAT #1 Upper/Lower */
#define IBAT1L	531
#define IBAT2U	532	/* Instruction BAT #2 Upper/Lower */
#define IBAT2L	533
#define IBAT3U	534	/* Instruction BAT #3 Upper/Lower */
#define IBAT3L	535
#define DBAT0U	536	/* Data BAT #0 Upper/Lower */
#define DBAT0L	537
#define DBAT1U	538	/* Data BAT #1 Upper/Lower */
#define DBAT1L	539
#define DBAT2U	540	/* Data BAT #2 Upper/Lower */
#define DBAT2L	541
#define DBAT3U	542	/* Data BAT #3 Upper/Lower */
#define DBAT3L	543
#define DMISS	976	/* TLB Lookup/Refresh registers */
#define DCMP	977
#define HASH1	978
#define HASH2	979
#define IMISS	980
#define ICMP	981
#define RPA	982
#define SDR1	25	/* MMU hash base register */
#define DAR	19	/* Data Address Register */
#define SPR0	272	/* Supervisor Private Registers */
#define SPRG0   272
#define SPR1	273
#define SPRG1   273
#define SPR2	274
#define SPRG2   274
#define SPR3	275
#define SPRG3   275
#define DSISR	18
#define SRR0	26	/* Saved Registers (exception) */
#define SRR1	27
#define IABR	1010	/* Instruction Address Breakpoint */
#define DEC	22	/* Decrementer */
#define EAR	282	/* External Address Register */
#define L2CR	1017    /* PPC 750 L2 control register */

#define THRM1	1020
#define THRM2	1021
#define THRM3	1022
#define THRM1_TIN 0x1
#define THRM1_TIV 0x2
#define THRM1_THRES (0x7f<<2)
#define THRM1_TID (1<<29)
#define THRM1_TIE (1<<30)
#define THRM1_V   (1<<31)
#define THRM3_E   (1<<31)

/* Segment Registers */
#define SR0	0
#define SR1	1
#define SR2	2
#define SR3	3
#define SR4	4
#define SR5	5
#define SR6	6
#define SR7	7
#define SR8	8
#define SR9	9
#define SR10	10
#define SR11	11
#define SR12	12
#define SR13	13
#define SR14	14
#define SR15	15

#ifndef ASM
typedef enum {
  PPC_601 	= 0x1,
  PPC_603 	= 0x3,
  PPC_604	= 0x4,
  PPC_603e	= 0x6,
  PPC_603ev	= 0x7,
  PPC_750	= 0x8,
  PPC_604e	= 0x9,
  PPC_604r	= 0xA,
  PPC_620	= 0x16,
  PPC_860	= 0x50,
  PPC_821	= PPC_860,
  PPC_UNKNOWN	= 0xff
} ppc_cpu_id_t;

typedef unsigned short ppc_cpu_revision_t;

extern ppc_cpu_id_t get_ppc_cpu_type();
extern ppc_cpu_id_t current_ppc_cpu;
extern ppc_cpu_revision_t get_ppc_cpu_revision();
extern ppc_cpu_revision_t current_ppc_revision;
/*
 *  Routines to access the time base register
 */

static inline unsigned long long PPC_Get_timebase_register( void )
{
  unsigned long tbr_low;
  unsigned long tbr_high;
  unsigned long tbr_high_old;
  unsigned long long tbr;

  do {
    asm volatile( "mftbu %0" : "=r" (tbr_high_old));
    asm volatile( "mftb  %0" : "=r" (tbr_low));
    asm volatile( "mftbu %0" : "=r" (tbr_high));
  } while ( tbr_high_old != tbr_high );

  tbr = tbr_high;
  tbr <<= 32;
  tbr |= tbr_low;
  return tbr;
}

static inline  void PPC_Set_timebase_register (unsigned long long tbr)
{
  unsigned long tbr_low;
  unsigned long tbr_high;

  tbr_low = (tbr & 0xffffffff) ;
  tbr_high = (tbr >> 32) & 0xffffffff;
  asm volatile( "mtspr 284, %0" : : "r" (tbr_low));
  asm volatile( "mtspr 285, %0" : : "r" (tbr_high));
  
}
#endif

#define _CPU_MSR_GET( _msr_value ) \
  do { \
    _msr_value = 0; \
    asm volatile ("mfmsr %0" : "=&r" ((_msr_value)) : "0" ((_msr_value))); \
  } while (0)

#define _CPU_MSR_SET( _msr_value ) \
{ asm volatile ("mtmsr %0" : "=&r" ((_msr_value)) : "0" ((_msr_value))); }

#define _CPU_ISR_Disable( _isr_cookie ) \
  { register unsigned int _disable_mask = MSR_EE; \
    _isr_cookie = 0; \
    asm volatile ( \
	"mfmsr %0; andc %1,%0,%1; mtmsr %1" : \
	"=&r" ((_isr_cookie)), "=&r" ((_disable_mask)) : \
	"0" ((_isr_cookie)), "1" ((_disable_mask)) \
	); \
  }


/*
 *  Enable interrupts to the previous level (returned by _CPU_ISR_Disable).
 *  This indicates the end of an RTEMS critical section.  The parameter
 *  _isr_cookie is not modified.
 */

#define _CPU_ISR_Enable( _isr_cookie )  \
  { \
     asm volatile ( "mtmsr %0" : \
		   "=r" ((_isr_cookie)) : \
                   "0" ((_isr_cookie))); \
  }

/*
 *  This temporarily restores the interrupt to _isr_cookie before immediately
 *  disabling them again.  This is used to divide long RTEMS critical
 *  sections into two or more parts.  The parameter _isr_cookie is not
 *  modified.
 *
 *  NOTE:  The version being used is not very optimized but it does
 *         not trip a problem in gcc where the disable mask does not
 *         get loaded.  Check this for future (post 10/97 gcc versions.
 */

#define _CPU_ISR_Flash( _isr_cookie ) \
  { register unsigned int _disable_mask = MSR_EE; \
    asm volatile ( \
      "mtmsr %0; andc %1,%0,%1; mtmsr %1" : \
      "=r" ((_isr_cookie)), "=r" ((_disable_mask)) : \
      "0" ((_isr_cookie)), "1" ((_disable_mask)) \
    ); \
  }


/* end of ISR handler macros */

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */

#define CPU_Get_timebase_low( _value ) \
    asm volatile( "mftb  %0" : "=r" (_value) )

#define rtems_bsp_delay( _microseconds ) \
  do { \
    unsigned32 start, ticks, now; \
    CPU_Get_timebase_low( start ) ; \
    ticks = (_microseconds) * rtems_cpu_configuration_get_clicks_per_usec(); \
    do \
      CPU_Get_timebase_low( now ) ; \
    while (now - start < ticks); \
  } while (0)

#define rtems_bsp_delay_in_bus_cycles( _cycles ) \
  do { \
    unsigned32 start, now; \
    CPU_Get_timebase_low( start ); \
    do \
      CPU_Get_timebase_low( now ); \
    while (now - start < (_cycles)); \
  } while (0)

#define PPC_Set_decrementer( _clicks ) \
  do { \
    asm volatile( "mtdec %0" : "=r" ((_clicks)) : "r" ((_clicks)) ); \
  } while (0)


#ifdef __cplusplus
}
#endif

#endif /* __PPC_CPU_H */







