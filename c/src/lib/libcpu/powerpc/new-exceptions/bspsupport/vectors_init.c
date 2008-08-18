/*
 * vectors_init.c Exception hanlding initialisation (and generic handler).
 *
 *  This include file describe the data structure and the functions implemented
 *  by rtems to handle exceptions.
 *
 *  Copyright (C) 1999 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/error.h>

#include <libcpu/raw_exception.h>
#include <libcpu/spr.h>
#include <libcpu/cpuIdent.h>

#include "vectors.h"
#include "ppc_exc_bspsupp.h"

static rtems_raw_except_global_settings exception_config;
static rtems_raw_except_connect_data    exception_table[LAST_VALID_EXC + 1];

uint32_t ppc_exc_cache_wb_check = 1;

#if 0
typedef struct ppc_exc_connect_data_ {
	rtems_raw_except_connect_data	raw;
	ppc_exc_handler_t				c_hdl;			
} ppc_exc_connect_data;
#endif

exception_handler_t globalExceptHdl;

/* T. Straumann: provide a stack trace
 * <strauman@slac.stanford.edu>, 6/26/2001
 */
typedef struct LRFrameRec_ {
	struct LRFrameRec_ *frameLink;
	unsigned long *lr;
} LRFrameRec, *LRFrame;

#define STACK_CLAMP 50	/* in case we have a corrupted bottom */

SPR_RW(SPRG1)
SPR_RW(SPRG2)
SPR_RO(LR)
SPR_RO(DAR)
#define DEAR_BOOKE 61
#define DEAR_405   0x3d5
SPR_RO(DEAR_BOOKE)
SPR_RO(DEAR_405)

uint32_t ppc_exc_get_DAR_dflt(void)
{
	if ( ppc_cpu_is_60x() )
		return _read_DAR();
	else switch ( ppc_cpu_is_bookE() ) {
		default: break;
		case PPC_BOOKE_STD:
		case PPC_BOOKE_E500:
			return _read_DEAR_BOOKE();
		case PPC_BOOKE_405:
			return _read_DEAR_405();
	}
	return 0xdeadbeef;
}

uint32_t (*ppc_exc_get_DAR)(void) = ppc_exc_get_DAR_dflt;

void
BSP_printStackTrace(BSP_Exception_frame* excPtr)
{
LRFrame	f;
int		i;
LRFrame	sp;
void	*lr;

	printk("Stack Trace: \n  ");
	if (excPtr) {
		printk("IP: 0x%08x, ",excPtr->EXC_SRR0);
		sp=(LRFrame)excPtr->GPR1;
		lr=(void*)excPtr->EXC_LR;
	} else {
		/* there's no macro for this */
		__asm__ __volatile__("mr %0, 1":"=r"(sp));
		lr=(LRFrame)_read_LR();
	}
	printk("LR: 0x%08x\n",lr);
	for (f=(LRFrame)sp, i=0; f->frameLink && i<STACK_CLAMP; f=f->frameLink) {
		printk("--^ 0x%08x", (long)(f->frameLink->lr));
		if (!(++i%5))
			printk("\n");
	}
	if (i>=STACK_CLAMP) {
		printk("Too many stack frames (stack possibly corrupted), giving up...\n");
	} else {
		if (i%5)
			printk("\n");
	}
}

void C_exception_handler(BSP_Exception_frame* excPtr)
{
static int          nest        = 0;

  int               recoverable = 0;
  rtems_id          id          = 0;
  int               synch;
  unsigned          n;
  rtems_status_code sc;

  /* Catch recursion */
  nest++;

  if ( nest > 2 ) {
	/* maybe printk() or dereferencing excPtr caused an exception;
	 * die silently...
	 */
	while (1)
		;
  }

  synch  = (int)excPtr->_EXC_number >= 0 ;
  n      = excPtr->_EXC_number & 0x7fff; 

  printk("Exception handler called for exception %d (0x%x)\n", n, n);
  printk("\t Next PC or Address of fault = %08x\n", excPtr->EXC_SRR0);
  printk("\t Saved MSR = %08x\n", excPtr->EXC_SRR1);

  if ( nest > 1 ) {
    printk("Recursion in the exception handler detected; I'll spin now...\n");
	while ( 1 )
		;
  }

  /* Try to find out more about the context where this happened */
  printk("\t Context: ");
  if ( rtems_interrupt_is_in_progress() ) {
	printk("ISR");
  } else if ( !_Thread_Executing ) {
	printk("Initialization (_Thread_Executing not available yet)");
  } else {
	if ( RTEMS_SUCCESSFUL != (sc=rtems_task_ident(RTEMS_SELF, RTEMS_LOCAL, &id)) ) {
		printk("Unable to determine faulting task; rtems_task_ident() returned %u", sc);
		id = 0;
	} else {
		printk("Task ID 0x%08x", id);
	}
  }
  printk("\n");

  /* Dump registers */

  printk("\t R0  = %08x", excPtr->GPR0);
  if ( synch ) {
    printk(" R1  = %08x", excPtr->GPR1);
    printk(" R2  = %08x", excPtr->GPR2);
  } else {
    printk("               ");
    printk("               ");
  }
  printk(" R3  = %08x\n", excPtr->GPR3);
  printk("\t R4  = %08x", excPtr->GPR4);
  printk(" R5  = %08x",   excPtr->GPR5);
  printk(" R6  = %08x",   excPtr->GPR6);
  printk(" R7  = %08x\n", excPtr->GPR7);
  printk("\t R8  = %08x", excPtr->GPR8);
  printk(" R9  = %08x",   excPtr->GPR9);
  printk(" R10 = %08x",   excPtr->GPR10);
  printk(" R11 = %08x\n", excPtr->GPR11);
  printk("\t R12 = %08x", excPtr->GPR12);
  if ( synch ) {
	  printk(" R13 = %08x",   excPtr->GPR13);
	  printk(" R14 = %08x",   excPtr->GPR14);
	  printk(" R15 = %08x\n", excPtr->GPR15);
	  printk("\t R16 = %08x", excPtr->GPR16);
	  printk(" R17 = %08x",   excPtr->GPR17);
	  printk(" R18 = %08x",   excPtr->GPR18);
	  printk(" R19 = %08x\n", excPtr->GPR19);
	  printk("\t R20 = %08x", excPtr->GPR20);
	  printk(" R21 = %08x",   excPtr->GPR21);
	  printk(" R22 = %08x",   excPtr->GPR22);
	  printk(" R23 = %08x\n", excPtr->GPR23);
	  printk("\t R24 = %08x", excPtr->GPR24);
	  printk(" R25 = %08x",   excPtr->GPR25);
	  printk(" R26 = %08x",   excPtr->GPR26);
	  printk(" R27 = %08x\n", excPtr->GPR27);
	  printk("\t R28 = %08x", excPtr->GPR28);
	  printk(" R29 = %08x",   excPtr->GPR29);
	  printk(" R30 = %08x",   excPtr->GPR30);
	  printk(" R31 = %08x\n", excPtr->GPR31);
  } else {
      printk("\n");
  }
  printk("\t CR  = %08x\n", excPtr->EXC_CR);
  printk("\t CTR = %08x\n", excPtr->EXC_CTR);
  printk("\t XER = %08x\n", excPtr->EXC_XER);
  printk("\t LR  = %08x\n", excPtr->EXC_LR);

  /* Would be great to print DAR but unfortunately,
   * that is not portable across different CPUs.
   * AFAIK on classic PPC DAR is SPR 19, on the
   * 405 we have DEAR = SPR 0x3d5 and booE says
   * DEAR = SPR 61 :-(
   */
  if ( ppc_exc_get_DAR ) {
  	printk("\t DAR = %08x\n",  ppc_exc_get_DAR());
  }

  BSP_printStackTrace(excPtr);

  if (excPtr->_EXC_number == ASM_DEC_VECTOR)
       recoverable = 1;
  if (excPtr->_EXC_number == ASM_SYS_VECTOR)
#ifdef TEST_RAW_EXCEPTION_CODE
    recoverable = 1;
#else
    recoverable = 0;
#endif
  if (!recoverable) {
    if ( id ) {
		printk("Suspending faulting task (0x%08x)\n", id);
		/* Unnest here because rtems_task_suspend() never returns */
		nest--;
		rtems_task_suspend(id);
	} else {
    	printk("unrecoverable exception!!! Push reset button\n");
    	while(1);
	}
  } else {
  	nest--;
  }
}

/***********************************************************
 * dummy functions for on/off/isOn calls
 * these functions just do nothing fulfill the semantic
 * requirements to enable/disable a certain exception
 */
void exception_nop_enable(const rtems_raw_except_connect_data* ptr)
{
}

int exception_always_enabled(const rtems_raw_except_connect_data* ptr)
{
  return 1;
}

/* Raw exception framework wants to keep a pointer to
 * the prologue so we must keep the ones we generate
 * from templates around...
 */
#define NUM_PROLOG	8	/* just a reasonable limit */
static int                  n_prolog = 0;
static ppc_exc_min_prolog_t prologues[NUM_PROLOG];

static ppc_exc_min_prolog_template_t prolog_templates[][2] = { 
	[ PPC_EXC_CLASSIC ] =
		{
		ppc_exc_min_prolog_sync_tmpl_std,
		ppc_exc_min_prolog_async_tmpl_std,
		},
	[ PPC_EXC_405_CRITICAL ] =
		{
		ppc_exc_min_prolog_sync_tmpl_p405_crit,
		ppc_exc_min_prolog_async_tmpl_p405_crit,
		},
	[ PPC_EXC_BOOKE_CRITICAL ] =
		{
		ppc_exc_min_prolog_sync_tmpl_bookE_crit,
		ppc_exc_min_prolog_async_tmpl_bookE_crit,
		},
	[ PPC_EXC_E500_MACHCHK ] =
		{
		ppc_exc_min_prolog_sync_tmpl_e500_mchk,
		ppc_exc_min_prolog_async_tmpl_e500_mchk,
		},
};

static rtems_raw_except_func
make_prologue(int vector, ppc_raw_exception_category cat)
{
int                           async = (cat & PPC_EXC_ASYNC) ? 1 : 0 ;
ppc_exc_min_prolog_template_t tmpl;

	cat &= ~PPC_EXC_ASYNC;

	if ( n_prolog >= NUM_PROLOG ) {
		rtems_panic("Not enough exception prologue slots; increase NUM_PROLOG (%s)\n",__FILE__);
	}

	if ( ! (tmpl = prolog_templates[cat][async]) ) {
		rtems_panic("No exception prologue template for category 0x%02x found\n", cat);
	}

  	ppc_exc_min_prolog_expand(prologues[n_prolog], tmpl, vector);

	return (rtems_raw_except_func)prologues[n_prolog++];
}

void ppc_exc_table_init(
	rtems_raw_except_connect_data    *exception_table,
	int                               nEntries)
{
unsigned i,v;
ppc_raw_exception_category cat;
uintptr_t vaddr;

	/*
	 * Initialize pointer used by low level execption handling
	 */
	globalExceptHdl 				= C_exception_handler;
	/*
	 * Put  default_exception_vector_code_prolog at relevant exception
	 * code entry addresses
	 */
	exception_config.exceptSize 			= nEntries;
	exception_config.rawExceptHdlTbl 		= exception_table;
	exception_config.defaultRawEntry.exceptIndex	= 0;
	exception_config.defaultRawEntry.hdl.vector	= 0;

	if (ppc_cpu_has_ivpr_and_ivor()) {
		/* Use packed version with 16-byte boundaries for CPUs with IVPR and IVOR registers */
		exception_config.defaultRawEntry.hdl.raw_hdl = ppc_exc_min_prolog_auto_packed;
	} else {
		/* Note that the 'auto' handler cannot be used for everything; in particular,
		 * it assumes classic exceptions with a vector offset aligned on a 256-byte
		 * boundary.
		 */
		exception_config.defaultRawEntry.hdl.raw_hdl = ppc_exc_min_prolog_auto;
	}

	/*
	 * Note that the cast of an array address to an unsigned
	 * is not a bug as it is defined by a .set directly in asm...
	 */
	exception_config.defaultRawEntry.hdl.raw_hdl_size = (unsigned)ppc_exc_min_prolog_size;

	for (i=0; i < exception_config.exceptSize; i++) {

		if ( PPC_EXC_INVALID == (cat = ppc_vector_is_valid ((v=exception_table[i].hdl.vector))) ) {
			continue;
		}

		exception_table[i].exceptIndex	= i;
		exception_table[v].hdl.raw_hdl_size = (unsigned)ppc_exc_min_prolog_size;

		/* special cases */
		if ( ppc_cpu_has_shadowed_gprs()
				&& (   ASM_60X_IMISS_VECTOR  == v
					|| ASM_60X_DLMISS_VECTOR == v
					|| ASM_60X_DSMISS_VECTOR == v ) ) {
			exception_table[i].hdl.raw_hdl	  = ppc_exc_tgpr_clr_prolog;
			exception_table[i].hdl.raw_hdl_size = (unsigned)ppc_exc_tgpr_clr_prolog_size;
		} else {

			vaddr = (uintptr_t)ppc_get_vector_addr( v );

			/*
			 * default prolog can handle classic, synchronous exceptions
			 * with a vector offset aligned on a 256-byte boundary.
			 */
			if (cat == PPC_EXC_CLASSIC && ((vaddr & 0xff) == 0 || (ppc_cpu_has_ivpr_and_ivor() && (vaddr & 0xf) == 0))) {
				exception_table[i].hdl.raw_hdl_size = exception_config.defaultRawEntry.hdl.raw_hdl_size;
				exception_table[i].hdl.raw_hdl	    = exception_config.defaultRawEntry.hdl.raw_hdl;
			} else {
				exception_table[i].hdl.raw_hdl_size = (unsigned)ppc_exc_min_prolog_size;
				exception_table[i].hdl.raw_hdl      = make_prologue( v, cat ); 
			}

		}
		exception_table[i].on		= exception_nop_enable;
		exception_table[i].off		= exception_nop_enable;
		exception_table[i].isOn		= exception_always_enabled;
	}
	if (!ppc_init_exceptions(&exception_config)) {
		BSP_panic("Exception handling initialization failed\n");
	}
#ifdef RTEMS_DEBUG
	else {
		printk("Exception handling initialization done\n");
	}
#endif
}


void ppc_exc_initialize(
	uint32_t interrupt_disable_mask,
	uint32_t interrupt_stack_start,
	uint32_t interrupt_stack_size
)
{
	int i;
	int n = sizeof(exception_table)/sizeof(exception_table[0]);

	uint32_t interrupt_stack_end = 0;
	uint32_t interrupt_stack_pointer = 0;
	uint32_t *p = NULL;
	uint32_t r13, sda_base;

	/* Assembly code needs SDA_BASE in r13 (SVR4 or EABI). Make sure
	 * early init code put it there.
	 */
	asm volatile(
		"	lis %0,     _SDA_BASE_@h  \n"
		"	ori %0, %0, _SDA_BASE_@l  \n"
		"	mr  %1, 13                \n"
	:"=r"(sda_base),"=r"(r13)
	);

	if ( sda_base != r13 ) {
		printk("ppc_exc_initialize(): INTERNAL ERROR\n");
		printk("  your BSP seems to not have loaded _SDA_BASE_\n");
		printk("  into R13 as required by SVR4/EABI. Check early init code...\n");
		printk("  _SDA_BASE_: 0x%08x, R13: 0x%08x\n", sda_base, r13);
		while (1)
			;
	}

	/* Interrupt stack end and pointer */
	interrupt_stack_end = interrupt_stack_start + interrupt_stack_size;

	interrupt_stack_pointer  = interrupt_stack_end - PPC_MINIMUM_STACK_FRAME_SIZE;

	/* Ensure proper interrupt stack alignment */
	interrupt_stack_pointer &= ~(CPU_STACK_ALIGNMENT - 1);

	/* Tag interrupt stack bottom */
	p = (uint32_t *) interrupt_stack_pointer;
	*p = 0;

	/* Move interrupt stack values to special purpose registers */
	_write_SPRG1( interrupt_stack_pointer);
	_write_SPRG2( interrupt_stack_start);

	/* Interrupt disable mask */
	ppc_interrupt_set_disable_mask( interrupt_disable_mask);

	/* Use current MMU / RI settings when running C exception handlers */
	ppc_exc_msr_bits = _read_MSR() & ( MSR_DR | MSR_IR | MSR_RI );

	for ( i=0; i<n; i++ )
		exception_table[i].hdl.vector = i;
	ppc_exc_table_init(exception_table, n);

	/* If we are on a classic PPC with MSR_DR enabled then
	 * assert that the mapping for at least this task's
	 * stack is write-back-caching enabled (see README/CAVEATS)
	 * Do this only if the cache is physically enabled.
	 * Since it is not easy to figure that out in a
	 * generic way we need help from the BSP: BSPs
	 * which run entirely w/o the cache may set
	 * ppc_exc_cache_wb_check to zero prior to calling
	 * this routine.
	 *
	 * We run this check only after exception handling is
	 * initialized so that we have some chance to get
	 * information printed if it fails.
	 *
	 * Note that it is unsafe to ignore this issue; if
	 * the check fails, do NOT disable it unless caches
	 * are always physically disabled.
	 */
	if ( ppc_exc_cache_wb_check && (MSR_DR & ppc_exc_msr_bits) ) {
		/* The size of 63 assumes cache lines are at most 32 bytes */
		uint8_t   dummy[63];
		uintptr_t p = (uintptr_t)dummy;
		/* If the dcbz instruction raises an alignment exception
		 * then the stack is mapped as write-thru or caching-disabled.
		 * The low-level code is not capable of dealing with this
		 * ATM.
		 */
		p = (p + 31) & ~31;
		asm volatile("dcbz 0, %0"::"b"(p));
		/* If we make it thru here then things seem to be OK */
	}

}
