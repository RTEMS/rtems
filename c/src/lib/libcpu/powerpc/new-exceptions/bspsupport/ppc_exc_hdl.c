/* PowerPC exception handling middleware; consult README for more
 * information.
 *
 * Author: Till Straumann <strauman@slac.stanford.edu>, 2007
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/score/apiext.h>

#include <bsp/vectors.h>

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

/* MSR bits to enable once critical status info is saved and the stack
 * is switched; must be set depending on CPU type
 *
 * Default is set here for classic PPC CPUs with a MMU
 * but is overridden from vectors_init.c
 */
uint32_t ppc_exc_msr_bits = MSR_IR | MSR_DR | MSR_RI;

static int ppc_exc_handler_default(BSP_Exception_frame *f, unsigned int vector)
{
  return -1;
}

/* Table of C-handlers */
ppc_exc_handler_t ppc_exc_handler_table [LAST_VALID_EXC + 1] = {
  [0 ... LAST_VALID_EXC] = ppc_exc_handler_default
};

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
      ppc_exc_handler_table [vector] = ppc_exc_handler_default;
    } else {
      ppc_exc_handler_table [vector] = handler;
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
