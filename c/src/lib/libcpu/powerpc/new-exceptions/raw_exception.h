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

/* DO NOT INTRODUCE #ifdef <cpu_flavor> in this file */

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
/* We could use the std. decrementer vector # on bookE, too,
 * but the bookE decrementer has slightly different semantics
 * so we use a different vector (which happens to be
 * the PIT vector on the 405 which is like the booke decrementer)
 */
#define	ASM_BOOKE_DEC_VECTOR	             0x10
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

/* DO NOT USE -- this symbol is DEPRECATED
 * (only used by libbsp/shared/vectors/vectors.S
 * which should not be used by new BSPs).
 */
#define ASM_60X_VEC_VECTOR_OFFSET			0xf20

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
 * Exceptions of different categories use different SRR registers
 * to save machine state (:-()
 *
 * For now, the CPU descriptions assume this fits into 8 bits.
 */
typedef enum {
	PPC_EXC_INVALID        = 0,
	PPC_EXC_CLASSIC        = 1,
	PPC_EXC_405_CRITICAL   = 2,
	PPC_EXC_BOOKE_CRITICAL = 3,
	PPC_EXC_E500_MACHCHK   = 4,
	PPC_EXC_ASYNC          = 0x80,
} ppc_raw_exception_category;

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
 * and returns category.
 */
extern ppc_raw_exception_category ppc_vector_is_valid(rtems_vector vector);

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
