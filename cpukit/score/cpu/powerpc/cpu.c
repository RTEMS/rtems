/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/*
 * For now, this file is just a stub to work around
 * structural deficiencies of the powerpc port.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/cpu.h>

#define PPC_ASSERT_OFFSET(field, off) \
  RTEMS_STATIC_ASSERT( \
    offsetof(ppc_context, field) + PPC_DEFAULT_CACHE_LINE_SIZE \
      == PPC_CONTEXT_OFFSET_ ## off, \
    ppc_context_offset_ ## field \
  )

PPC_ASSERT_OFFSET(gpr1, GPR1);
PPC_ASSERT_OFFSET(msr, MSR);
PPC_ASSERT_OFFSET(lr, LR);
PPC_ASSERT_OFFSET(cr, CR);
PPC_ASSERT_OFFSET(gpr14, GPR14);
PPC_ASSERT_OFFSET(gpr15, GPR15);
PPC_ASSERT_OFFSET(gpr16, GPR16);
PPC_ASSERT_OFFSET(gpr17, GPR17);
PPC_ASSERT_OFFSET(gpr18, GPR18);
PPC_ASSERT_OFFSET(gpr19, GPR19);
PPC_ASSERT_OFFSET(gpr20, GPR20);
PPC_ASSERT_OFFSET(gpr21, GPR21);
PPC_ASSERT_OFFSET(gpr22, GPR22);
PPC_ASSERT_OFFSET(gpr23, GPR23);
PPC_ASSERT_OFFSET(gpr24, GPR24);
PPC_ASSERT_OFFSET(gpr25, GPR25);
PPC_ASSERT_OFFSET(gpr26, GPR26);
PPC_ASSERT_OFFSET(gpr27, GPR27);
PPC_ASSERT_OFFSET(gpr28, GPR28);
PPC_ASSERT_OFFSET(gpr29, GPR29);
PPC_ASSERT_OFFSET(gpr30, GPR30);
PPC_ASSERT_OFFSET(gpr31, GPR31);

RTEMS_STATIC_ASSERT(
  sizeof(Context_Control) % PPC_DEFAULT_CACHE_LINE_SIZE == 0,
  ppc_context_size
);
