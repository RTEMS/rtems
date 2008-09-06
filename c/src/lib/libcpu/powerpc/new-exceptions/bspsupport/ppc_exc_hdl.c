/* PowerPC exception handling middleware; consult README for more
 * information.
 *
 * Author: Till Straumann <strauman@slac.stanford.edu>, 2007
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include	<stdint.h>
#include	<string.h>

#include	<rtems.h>
#include	<rtems/score/cpu.h>
#include	<libcpu/raw_exception.h>
#include	<libcpu/spr.h>
#include	<rtems/score/apiext.h>

#include	"vectors.h"
#include	"ppc_exc_bspsupp.h"

/* offset into min-prolog where vector # is hardcoded */
#define		PPC_EXC_PROLOG_VEC_OFFSET	2

/* Provide temp. storage space for a few registers.
 * This is used by the assembly code prior to setting up
 * the stack.
 * One set is needed for each exception type with its
 * own SRR0/SRR1 pair since such exceptions may nest.
 *
 * NOTE: The assembly code needs these variables to
 *       be in the .sdata section and accesses them
 *       via R13.
 */
uint32_t ppc_exc_lock_std  = 0;
uint32_t ppc_exc_lock_crit = 0;
uint32_t ppc_exc_lock_mchk = 0;

uint32_t ppc_exc_vector_register_std     = 0;
uint32_t ppc_exc_vector_register_crit    = 0;
uint32_t ppc_exc_vector_register_mchk    = 0;

/* MSR bits to enable once critical status info is saved and the stack
 * is switched; must be set depending on CPU type
 *
 * Default is set here for classic PPC CPUs with a MMU
 * but is overridden from vectors_init.c
 */
uint32_t ppc_exc_msr_bits     = MSR_IR | MSR_DR | MSR_RI;

int ppc_exc_handler_default( BSP_Exception_frame *f, unsigned int vector)
{
	return 1;
}

/* Table of C-handlers */
ppc_exc_handler_t ppc_exc_handler_table [LAST_VALID_EXC + 1] = {
	[0 ... LAST_VALID_EXC] = ppc_exc_handler_default
};

ppc_exc_handler_t ppc_exc_get_handler( unsigned vector)
{
	ppc_exc_handler_t handler = NULL;
	if (vector > LAST_VALID_EXC) {
		return 0;
	}
	if (ppc_exc_handler_table [vector] != ppc_exc_handler_default) {
		handler = ppc_exc_handler_table [vector];
	}
	return handler;
}

int ppc_exc_set_handler( unsigned vector, ppc_exc_handler_t handler)
{
	if (vector > LAST_VALID_EXC) {
		return -1;
	}
	if (handler == NULL) {
		ppc_exc_handler_table [vector] = ppc_exc_handler_default;
	} else {
		ppc_exc_handler_table [vector] = handler;
	}
	return 0;
}

void
ppc_exc_wrapup( BSP_Exception_frame *f)
{
	/* dispatch_disable level is decremented from assembly code.  */
	if ( _Context_Switch_necessary ) {
		/* FIXME: I believe it should be OK to re-enable
		 *        interrupts around the execution of _Thread_Dispatch();
		 */
		_Thread_Dispatch();
	} else if ( _ISR_Signals_to_thread_executing ) {
		_ISR_Signals_to_thread_executing = 0;
		/*
		 * Process pending signals that have not already been
		 * processed by _Thread_Dispatch. This happens quite
		 * unfrequently : the ISR must have posted an action
		 * to the current running thread.
		 */
		if ( _Thread_Do_post_task_switch_extension ||
				_Thread_Executing->do_post_task_switch_extension ) {
			_Thread_Executing->do_post_task_switch_extension = false;
			_API_extensions_Run_postswitch();
		}
	}
}

void
ppc_exc_min_prolog_expand(ppc_exc_min_prolog_t buf, ppc_exc_min_prolog_template_t templ, uint16_t vec)
{
	memcpy(&buf[0], templ, sizeof(ppc_exc_min_prolog_t));
	/* fixup the vector */
	buf[PPC_EXC_PROLOG_VEC_OFFSET] = (buf[PPC_EXC_PROLOG_VEC_OFFSET] & 0xffff8000) | (vec & 0x7fff);
}

#undef TESTING
#ifdef TESTING

static void noop(const struct __rtems_raw_except_connect_data__*x) {}

rtems_raw_except_connect_data exc_conn = {
	exceptIndex: ASM_SYS_VECTOR,
	hdl        : {
					vector: ASM_SYS_VECTOR,
					raw_hdl: 0,
					raw_hdl_size: 0
	             },
	on         : noop,
	off        : noop,
	isOn       : 0	/* never used AFAIK */
};

void
ppc_exc_raise()
{
	asm volatile("li 3, 0xffffdead; sc");
}


int
exc_conn_do()
{
	exc_conn.hdl.raw_hdl      = ppc_exc_min_prolog_auto;
	exc_conn.hdl.raw_hdl_size = 16;
	return ppc_set_exception(&exc_conn);
}
#endif
