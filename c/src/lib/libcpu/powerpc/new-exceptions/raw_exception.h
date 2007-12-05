/*
 * raw_execption.h
 *
 *	    This file contains implementation of C function to
 *          Instantiate 60x ppc primary exception entries.
 *	    More detailed information can be found on motorola
 *	    site and more precisely in the following book :
 *
 *		MPC750 
 *		Risc Microporcessor User's Manual
 *		Mtorola REF : MPC750UM/AD 8/97
 *
 * Copyright (C) 1999  Eric Valette (valette@crf.canon.fr)
 *                     Canon Centre Recherche France.
 *
 * Enhanced by Jay Kulpinski <jskulpin@eng01.gdds.com>
 * to support 603, 603e, 604, 604e exceptions
 *
 * moved to "libcpu/powerpc/new-exceptions and consolidated
 * by Thomas Doerfler <Thomas.Doerfler@embedded-brains.de>
 * to be common for all PPCs with new excpetions
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#ifndef _LIBCPU_RAW_EXCEPTION_H
#define _LIBCPU_RAW_EXCEPTION_H

#include <rtems/powerpc/powerpc.h>

/* For now, many BSPs still rely on <cpu_flavor> being defined
 * but that should be phased out.
 * The BSP support for exceptions and interrupts under 'bspsupp'
 * is designed to be #ifdef <flavor> FREE.
 * BSPs using 'bspsupp' should work with __ppc_generic
 */

#ifndef __ppc_generic

/*
 * find out, whether we want to (re)enable the MMU in the assembly code
 * FIXME: move this to a better location
 */
#if (defined(ppc403) || defined(ppc405))
#define PPC_USE_MMU 0
#else
#define PPC_USE_MMU 1
#endif

/*
 * Exception Vectors and offsets as defined in the MCP750 manual
 * used by most PPCs
 */

#define	ASM_RESET_VECTOR 		0x01
#define	ASM_RESET_VECTOR_OFFSET 	(ASM_RESET_VECTOR << 8)

#define	ASM_MACH_VECTOR			0x02
#define	ASM_MACH_VECTOR_OFFSET 		(ASM_MACH_VECTOR  << 8)

#define	ASM_PROT_VECTOR			0x03
#define	ASM_PROT_VECTOR_OFFSET 		(ASM_PROT_VECTOR  << 8)

#define	ASM_ISI_VECTOR 			0x04
#define	ASM_ISI_VECTOR_OFFSET 		(ASM_ISI_VECTOR   << 8)

#define	ASM_EXT_VECTOR 			0x05
#define	ASM_EXT_VECTOR_OFFSET 		(ASM_EXT_VECTOR   << 8)

#define	ASM_ALIGN_VECTOR 		0x06
#define	ASM_ALIGN_VECTOR_OFFSET 	(ASM_ALIGN_VECTOR << 8)

#define	ASM_PROG_VECTOR			0x07
#define	ASM_PROG_VECTOR_OFFSET 		(ASM_PROG_VECTOR  << 8)

#define	ASM_FLOAT_VECTOR		0x08
#define	ASM_FLOAT_VECTOR_OFFSET		(ASM_FLOAT_VECTOR << 8)

#define	ASM_DEC_VECTOR			0x09
#define	ASM_DEC_VECTOR_OFFSET		(ASM_DEC_VECTOR   << 8)

/* Bummer: Altivec unavailable doesn't fit into this scheme... (0xf20).
 *    We'd like to avoid reserved vectors but OTOH we don't want to use
 *    just an available high number because tables (and copies) are of
 *    size LAST_VALID_EXC.
 *    So until there is a CPU that uses 0xA we'll just use that :-(
 */
#define ASM_60X_VEC_VECTOR			0x0A
#define ASM_60X_VEC_VECTOR_OFFSET		(0xf20)

#define	ASM_SYS_VECTOR			0x0C
#define	ASM_SYS_VECTOR_OFFSET		(ASM_SYS_VECTOR   << 8)

#define	ASM_TRACE_VECTOR		0x0D
#define	ASM_TRACE_VECTOR_OFFSET		(ASM_TRACE_VECTOR << 8)

#if defined(ppc405)
     /*
      * vectors for PPC405
      */
#define	ASM_BOOKE_CRIT_VECTOR                 ASM_RESET_VECTOR
#define	ASM_BOOKE_CRIT_VECTOR_OFFSET          (ASM_BOOKE_CRIT_VECTOR << 8)	

#define	ASM_BOOKE_PIT_VECTOR	                0x10
#define	ASM_BOOKE_PIT_VECTOR_OFFSET           (ASM_BOOKE_PIT_VECTOR      << 8)

#define	ASM_BOOKE_ITLBMISS_VECTOR             0x11
#define	ASM_BOOKE_ITLBMISS_VECTOR_OFFSET      (ASM_BOOKE_ITLBMISS_VECTOR << 8)

#define	ASM_BOOKE_DTLBMISS_VECTOR             0x12
#define	ASM_BOOKE_DTLBMISS_VECTOR_OFFSET      (ASM_BOOKE_DTLBMISS_VECTOR << 8)

#define	ASM_BOOKE_FIT_VECTOR                  0x13
#define	ASM_BOOKE_FIT_VECTOR_OFFSET	        (0x1010)

#define	ASM_BOOKE_WDOG_VECTOR                 0x14
#define	ASM_BOOKE_WDOG_VECTOR_OFFSET	        (0x1020)

#define LAST_VALID_EXC                  ASM_BOOKE_WDOG_VECTOR

/*
 * bit mask of all exception vectors, that are handled
 * as "critical" exsceptions (using SRR2/SRR3/rfci)
 * this value will be evaluated in the default exception entry/exit
 * code to determine, whether to use SRR0/SRR1/rfi or SRR2/SRR3/rfci
 */
#define ASM_VECTORS_CRITICAL			\
  (( 1 << (31-ASM_BOOKE_CRIT_VECTOR))			\
   |(1 << (31-ASM_MACH_VECTOR))			\
   |(1 << (31-ASM_WDOG_VECTOR)))

#elif ( defined(mpc860) || defined(mpc821) )
     /*
      * vectors for MPC8xx
      */

/*
 * FIXME: even more vector names might get used in common,
 * but the names have diverged between different PPC families
 */
#define	ASM_8XX_FLOATASSIST_VECTOR	      0x0E
#define	ASM_8XX_FLOATASSIST_VECTOR_OFFSET (ASM_8XX_FLOATASSIST_VECTOR << 8)

#define	ASM_8XX_SOFTEMUL_VECTOR	      0x10
#define	ASM_8XX_SOFTEMUL_VECTOR_OFFSET    (ASM_8XX_SOFTEMUL_VECTOR << 8)

#define	ASM_8XX_ITLBMISS_VECTOR           0x11
#define	ASM_8XX_ITLBMISS_VECTOR_OFFSET    (ASM_8XX_ITLBMISS_VECTOR << 8)

#define	ASM_8XX_DTLBMISS_VECTOR           0x12
#define	ASM_8XX_DTLBMISS_VECTOR_OFFSET    (ASM_8XX_DTLBMISS_VECTOR << 8)

#define	ASM_8XX_ITLBERROR_VECTOR          0x13
#define	ASM_8XX_ITLBERROR_VECTOR_OFFSET   (ASM_8XX_ITLBERROR_VECTOR << 8)

#define	ASM_8XX_DTLBERROR_VECTOR          0x14
#define	ASM_8XX_DTLBERROR_VECTOR_OFFSET   (ASM_8XX_DTLBERROR_VECTOR << 8)

#define ASM_8XX_DBREAK_VECTOR             0x1C
#define ASM_8XX_DBREAK_VECTOR_OFFSET      (ASM_8XX_DBREAK_VECTOR << 8)

#define ASM_8XX_IBREAK_VECTOR             0x1D
#define ASM_8XX_IBREAK_VECTOR_OFFSET      (ASM_8XX_IBREAK_VECTOR << 8)

#define ASM_8XX_PERIFBREAK_VECTOR         0x1E
#define ASM_8XX_PERIFBREAK_VECTOR_OFFSET  (ASM_8XX_PERIFBREAK_VECTOR << 8)

#define ASM_8XX_DEVPORT_VECTOR            0x1F
#define ASM_8XX_DEVPORT_VECTOR_OFFSET     (ASM_8XX_DEVPORT_VECTOR_OFFSET << 8)

#define LAST_VALID_EXC		ASM_8XX_DEVPORT_VECTOR

#elif (defined(mpc555) || defined(mpc505))
     /*
      * vectorx for MPC5xx
      */
#define	ASM_5XX_FLOATASSIST_VECTOR	0x0E

#define	ASM_5XX_SOFTEMUL_VECTOR	0x10

#define	ASM_5XX_IPROT_VECTOR	0x13
#define	ASM_5XX_DPROT_VECTOR	0x14

#define ASM_5XX_DBREAK_VECTOR	0x1C
#define ASM_5XX_IBREAK_VECTOR	0x1D
#define ASM_5XX_MEBREAK_VECTOR	0x1E
#define ASM_5XX_NMEBREAK_VECTOR	0x1F

#define LAST_VALID_EXC		ASM_5XX_NMEBREAK_VECTOR

#else /* 60x style cpu types */
#define PPC_HAS_60X_VECTORS

#define	ASM_60X_PERFMON_VECTOR		0x0F
#define	ASM_60X_PERFMON_VECTOR_OFFSET	(ASM_60X_PERFMON_VECTOR << 8)

#define	ASM_60X_IMISS_VECTOR		0x10

#define	ASM_60X_DLMISS_VECTOR		0x11

#define	ASM_60X_DSMISS_VECTOR		0x12

#define	ASM_60X_ADDR_VECTOR			0x13
#define	ASM_60X_ADDR_VECTOR_OFFSET	        (ASM_60X_ADDR_VECTOR  << 8)

#define	ASM_60X_SYSMGMT_VECTOR		0x14
#define	ASM_60X_SYSMGMT_VECTOR_OFFSET	(ASM_60X_SYSMGMT_VECTOR << 8)

#define ASM_60X_VEC_ASSIST_VECTOR           0x16
#define ASM_60X_VEC_ASSIST_VECTOR_OFFSET    (ASM_60X_VEC_ASSIST_VECTOR << 8)

#define	ASM_60X_ITM_VECTOR                  0x17
#define	ASM_60X_ITM_VECTOR_OFFSET           (ASM_60X_ITM_VECTOR   << 8)

#define LAST_VALID_EXC                  ASM_60X_ITM_VECTOR

#endif

     /*
      * bits to be set in MSR in exception entry code
      */
#if                     ( PPC_HAS_RI) && ( PPC_USE_MMU)
#define PPC_MSR_EXC_BITS (PPC_MSR_RI   | PPC_MSR_DR | PPC_MSR_IR)
#elif                   ( PPC_HAS_RI) && (!PPC_USE_MMU)
#define PPC_MSR_EXC_BITS (PPC_MSR_RI)
#elif                   (!PPC_HAS_RI) && ( PPC_USE_MMU)
#define PPC_MSR_EXC_BITS (               PPC_MSR_DR | PPC_MSR_IR)
#else
#endif

#else  /* __ppc_generic */

#define	ASM_RESET_VECTOR 		             0x01
#define	ASM_MACH_VECTOR			             0x02
#define	ASM_PROT_VECTOR			             0x03
#define	ASM_ISI_VECTOR 			             0x04
#define	ASM_EXT_VECTOR 			             0x05
#define	ASM_ALIGN_VECTOR 		             0x06
#define	ASM_PROG_VECTOR			             0x07
#define	ASM_FLOAT_VECTOR		             0x08
#define	ASM_DEC_VECTOR			             0x09
#define ASM_60X_VEC_VECTOR			         0x0A
#define	ASM_SYS_VECTOR			             0x0C
#define	ASM_TRACE_VECTOR		             0x0D

#define	ASM_BOOKE_CRIT_VECTOR	             0x01
#define	ASM_BOOKE_PIT_VECTOR	             0x10
#define	ASM_BOOKE_ITLBMISS_VECTOR            0x11
#define	ASM_BOOKE_DTLBMISS_VECTOR            0x12
#define	ASM_BOOKE_FIT_VECTOR                 0x13
#define	ASM_BOOKE_WDOG_VECTOR                0x14

#define	ASM_8XX_FLOATASSIST_VECTOR	         0x0E
#define	ASM_8XX_SOFTEMUL_VECTOR	             0x10
#define	ASM_8XX_ITLBMISS_VECTOR              0x11
#define	ASM_8XX_DTLBMISS_VECTOR              0x12
#define	ASM_8XX_ITLBERROR_VECTOR             0x13
#define	ASM_8XX_DTLBERROR_VECTOR             0x14
#define ASM_8XX_DBREAK_VECTOR                0x1C
#define ASM_8XX_IBREAK_VECTOR                0x1D
#define ASM_8XX_PERIFBREAK_VECTOR            0x1E
#define ASM_8XX_DEVPORT_VECTOR               0x1F

#define	ASM_5XX_FLOATASSIST_VECTOR	         0x0E
#define	ASM_5XX_SOFTEMUL_VECTOR	             0x10
#define	ASM_5XX_IPROT_VECTOR	             0x13
#define	ASM_5XX_DPROT_VECTOR	             0x14
#define ASM_5XX_DBREAK_VECTOR	             0x1C
#define ASM_5XX_IBREAK_VECTOR	             0x1D
#define ASM_5XX_MEBREAK_VECTOR	             0x1E
#define ASM_5XX_NMEBREAK_VECTOR	             0x1F


#define	ASM_60X_PERFMON_VECTOR               0x0F
#define	ASM_60X_IMISS_VECTOR                 0x10
#define	ASM_60X_DLMISS_VECTOR                0x11
#define	ASM_60X_DSMISS_VECTOR                0x12
#define	ASM_60X_ADDR_VECTOR                  0x13
#define	ASM_60X_SYSMGMT_VECTOR               0x14
#define ASM_60X_VEC_ASSIST_VECTOR            0x16
#define	ASM_60X_ITM_VECTOR                   0x17

#define LAST_VALID_EXC                       0x1F

#endif /* __ppc_generic */


#ifndef ASM

/*
 * Type definition for raw exceptions. 
 */

typedef unsigned char  rtems_vector;
struct 	__rtems_raw_except_connect_data__;
typedef void 		(*rtems_raw_except_func)		(void);
typedef unsigned long 	rtems_raw_except_hdl_size;

typedef struct {
  rtems_vector			vector;
  rtems_raw_except_func		raw_hdl;
  rtems_raw_except_hdl_size	raw_hdl_size;
}rtems_raw_except_hdl;
  
typedef void (*rtems_raw_except_enable)		(const struct __rtems_raw_except_connect_data__*);
typedef void (*rtems_raw_except_disable)	(const struct __rtems_raw_except_connect_data__*);
typedef int  (*rtems_raw_except_is_enabled)	(const struct __rtems_raw_except_connect_data__*);

typedef struct __rtems_raw_except_connect_data__{
 /*
  * Exception vector (As defined in the manual)
  */
  rtems_vector			exceptIndex;
  /*
   * Exception raw handler. See comment on handler properties below in function prototype.
   */
  rtems_raw_except_hdl	   	hdl;
  /*
   * function for enabling raw exceptions. In order to be consistent
   * with the fact that the raw connexion can defined in the
   * libcpu library, this library should have no knowledge of
   * board specific hardware to manage exceptions and thus the
   * "on" routine must enable the except at processor level only.
   * 
   */
    rtems_raw_except_enable	on;	
  /*
   * function for disabling raw exceptions. In order to be consistent
   * with the fact that the raw connexion can defined in the
   * libcpu library, this library should have no knowledge of
   * board specific hardware to manage exceptions and thus the
   * "on" routine must disable the except both at device and PIC level.
   * 
   */
  rtems_raw_except_disable	off;
  /*
   * function enabling to know what exception may currently occur
   */
  rtems_raw_except_is_enabled	isOn;
}rtems_raw_except_connect_data;

typedef struct {
  /*
   * size of all the table fields (*Tbl) described below.
   */
  unsigned int	 			exceptSize;
  /*
   * Default handler used when disconnecting exceptions.
   */
  rtems_raw_except_connect_data		defaultRawEntry;
  /*
   * Table containing initials/current value.
   */
  rtems_raw_except_connect_data*	rawExceptHdlTbl;
}rtems_raw_except_global_settings;

/*
 * C callable function enabling to set up one raw idt entry
 */
extern int ppc_set_exception (const rtems_raw_except_connect_data*);

/*
 * C callable function enabling to get one current raw idt entry
 */
extern int ppc_get_current_exception (rtems_raw_except_connect_data*);

/*
 * C callable function enabling to remove one current raw idt entry
 */
extern int ppc_delete_exception (const rtems_raw_except_connect_data*);

/*
 * C callable function enabling to check if vector is valid
 */
extern int ppc_vector_is_valid(rtems_vector vector);

/*
 * Exception global init.
 */
extern int ppc_init_exceptions (rtems_raw_except_global_settings* config);
extern int ppc_get_exception_config (rtems_raw_except_global_settings** config);

void* ppc_get_vector_addr(rtems_vector vector);

int ppc_is_e500();
void e500_setup_raw_exceptions();

/* This variable is initialized to 'TRUE' by default;
 * BSPs which have their vectors in ROM should set it
 * to FALSE prior to initializing raw exceptions.
 *
 * I suspect the only candidate is the simulator.
 * After all, the value of this variable is used to
 * determine where to install the prologue code and
 * installing to ROM on anyting that's real ROM
 * will fail anyways.
 *
 * This should probably go away... (T.S. 2007/11/30)
 */
extern boolean bsp_exceptions_in_RAM;

# endif /* ASM */

#endif
