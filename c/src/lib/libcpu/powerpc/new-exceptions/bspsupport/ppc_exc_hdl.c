/* PowerPC exception handling middleware; consult README for more
 * information.
 *
 * Author: Till Straumann <strauman@slac.stanford.edu>, 2007
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp/vectors.h>

#include <rtems/score/apiext.h>
#include <rtems/score/threaddispatch.h>

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

uint32_t ppc_exc_vector_register_std  = 0;
uint32_t ppc_exc_vector_register_crit = 0;
uint32_t ppc_exc_vector_register_mchk = 0;

#ifndef PPC_EXC_CONFIG_BOOKE_ONLY

/* MSR bits to enable once critical status info is saved and the stack
 * is switched; must be set depending on CPU type
 *
 * Default is set here for classic PPC CPUs with a MMU
 * but is overridden from vectors_init.c
 */
uint32_t ppc_exc_msr_bits = MSR_IR | MSR_DR | MSR_RI;

#endif /* PPC_EXC_CONFIG_BOOKE_ONLY */

int ppc_exc_handler_default(BSP_Exception_frame *f, unsigned int vector)
{
  return -1;
}

#ifndef PPC_EXC_CONFIG_USE_FIXED_HANDLER

exception_handler_t globalExceptHdl = C_exception_handler;

/* Table of C-handlers */
ppc_exc_handler_t ppc_exc_handler_table [LAST_VALID_EXC + 1] = {
  [0 ... LAST_VALID_EXC] = ppc_exc_handler_default
};

#endif /* PPC_EXC_CONFIG_USE_FIXED_HANDLER */

ppc_exc_handler_t ppc_exc_get_handler(unsigned vector)
{
  if (
    vector <= LAST_VALID_EXC
      && ppc_exc_handler_table [vector] != ppc_exc_handler_default
  ) {
    return ppc_exc_handler_table [vector];
  } else {
    return NULL;
  }
}

rtems_status_code ppc_exc_set_handler(unsigned vector, ppc_exc_handler_t handler)
{
  if (vector <= LAST_VALID_EXC) {
    if (handler == NULL) {
      handler = ppc_exc_handler_default;
    }

    if (ppc_exc_handler_table [vector] != handler) {
#ifndef PPC_EXC_CONFIG_USE_FIXED_HANDLER
      ppc_exc_handler_table [vector] = handler;
#else /* PPC_EXC_CONFIG_USE_FIXED_HANDLER */
      return RTEMS_RESOURCE_IN_USE;
#endif /* PPC_EXC_CONFIG_USE_FIXED_HANDLER */
    }

    return RTEMS_SUCCESSFUL;
  } else {
    return RTEMS_INVALID_ID;
  }
}

void ppc_exc_wrapup(BSP_Exception_frame *frame)
{
  /* dispatch_disable level is decremented from assembly code.  */
  if ( _Thread_Dispatch_necessary ) {
    /* FIXME: I believe it should be OK to re-enable
     *        interrupts around the execution of _Thread_Dispatch();
     */
    _Thread_Dispatch();
  }
}
