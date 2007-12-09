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

#ifndef PPC_EXC_SHARED_H
#define PPC_EXC_SHARED_H

#ifdef __cplusplus
extern "C" {
#endif

/********* C-Exception Handlers *********************/

/* API to be used by middleware,                    */
/* BSP and application code (if necessary           */

/****************************************************/

typedef int (*ppc_exc_handler_t)(BSP_Exception_frame *f, unsigned int vector);

/*
 * Bits in MSR that are enabled during execution of exception handlers / ISRs 
 * (on classic PPC these are DR/IR/RI [default], on bookE-style CPUs they should
 * be set to 0 during initialization)
 *
 * By default, the setting of these bits that is in effect when exception
 * handling is initialized is used.
 */
extern uint32_t ppc_exc_msr_bits;

/*
 * Set of MSR bits required to disable all 
 * asynchronous exceptions (depends on CPU type;
 * must be set during initialization).
 * Interrupt are disabled by writing the
 * one's complement of this mask to msr:
 *  msr &= ~ppc_exc_msr_irq_mask;
 */
extern uint32_t ppc_exc_msr_irq_mask;

/*
 * Hook C exception handlers.
 *  - handlers for asynchronous exceptions run on the ISR stack
 *    with thread-dispatching disabled.
 *  - handlers for synchronous exceptions run on the task stack
 *    with thread-dispatching enabled.
 *
 * If a particular slot is NULL then the traditional 'globalExcHdl' is used.
 *
 * ppc_exc_set_handler() registers a handler (returning 0 on success,
 * -1 if the vector argument is too big).
 *
 * It is legal to set a NULL handler. This leads to the globalExcHdl
 * being called if an exception for 'vector' occurs.
 */
int
ppc_exc_set_handler(unsigned vector, ppc_exc_handler_t hdl);

/* ppc_exc_get_handler() retrieves the currently active handler.
 */
ppc_exc_handler_t
ppc_exc_get_handler(unsigned vector);

/********* Low-level Exception Handlers *************/

/* This API is used by middleware code              */

/****************************************************/

typedef uint32_t ppc_exc_min_prolog_t[4];

/* Templates are ppc_raw_except_func BUT they must be exactly 16 bytes */
typedef rtems_raw_except_func ppc_exc_min_prolog_template_t;

/* 
 * Expand a prolog template into 'buf' using vector 'vec'
 */
void
ppc_exc_min_prolog_expand(ppc_exc_min_prolog_t buf, ppc_exc_min_prolog_template_t templ, uint16_t vec);

extern unsigned ppc_exc_min_prolog_size[];
/* Symbols are defined by the linker; declare as an array so
 * that gcc doesn't attempt to emit a relocation looking for
 * it in the SDA section
 */
extern unsigned ppc_exc_tgpr_clr_prolog_size[];

/* Templates for ppc_exc_min_prolog_expand() which fills-in the vector information */
extern void ppc_exc_min_prolog_async_tmpl_std();
extern void ppc_exc_min_prolog_sync_tmpl_std();
extern void ppc_exc_min_prolog_async_tmpl_p405_crit();
extern void ppc_exc_min_prolog_sync_tmpl_p405_crit();
extern void ppc_exc_min_prolog_async_tmpl_bookE_crit();
extern void ppc_exc_min_prolog_sync_tmpl_bookE_crit();
extern void ppc_exc_min_prolog_sync_tmpl_e500_mchk();
extern void ppc_exc_min_prolog_async_tmpl_e500_mchk();

/* Special prologue for handling register shadowing on 603-style CPUs */
extern void ppc_exc_tgpr_clr_prolog();
/* Classic prologue which determines the vector dynamically from
 * the offset address. This must only be used for classic, synchronous
 * exceptions with a vector offset aligned on a 256-byte boundary.
 */
extern void ppc_exc_min_prolog_auto();


/* CPU support may store the address of a function here
 * that can be used by the default exception handler to
 * obtain fault-address info which is helpful. Unfortunately,
 * the SPR holding this information is not uniform
 * across PPC families so we need assistance from
 * CPU support
 */
extern uint32_t (*ppc_exc_get_DAR)();

#ifdef __cplusplus
};
#endif

#endif
