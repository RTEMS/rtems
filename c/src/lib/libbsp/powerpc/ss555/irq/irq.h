/* irq.h
 *
 *  This include file describe the data structure and the functions implemented
 *  by rtems to write interrupt handlers.
 *
 *
 *  SS555 port sponsored by Defence Research and Development Canada - Suffield
 *  Copyright (C) 2004, Real-Time Systems Inc. (querbach@realtime.bc.ca)
 *
 *  Derived from libbsp/powerpc/mbx8xx/irq/irq.h:
 *
 *  CopyRight (C) 1999 valette@crf.canon.fr
 *
 *  This code is heavilly inspired by the public specification of STREAM V2
 *  that can be found at :
 *
 *      <http://www.chorus.com/Documentation/index.html> by following
 *  the STREAM API Specification Document link.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_POWERPC_SS555_IRQ_IRQ_H
#define LIBBSP_POWERPC_SS555_IRQ_IRQ_H

#include <libcpu/irq.h>

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  These are no longer prototyped anywhere else. This appears to be
 *  remnants of the IRQ code upgrade.
 *
 *  --joel 28 April 2010
 */
int CPU_install_rtems_irq_handler(const rtems_irq_connect_data* irq);
int CPU_get_current_rtems_irq_handler(rtems_irq_connect_data* irq);
int CPU_remove_rtems_irq_handler(const rtems_irq_connect_data* irq);
int CPU_rtems_irq_mngt_set(rtems_irq_global_settings* config);
int CPU_rtems_irq_mngt_get(rtems_irq_global_settings** config);

/*
 * The SS555 has no external interrupt controller chip, so use the standard
 * routines from the CPU-dependent code.
 */
#define BSP_install_rtems_irq_handler(ptr)	CPU_install_rtems_irq_handler(ptr)
#define BSP_get_current_rtems_irq_handler(ptr)	CPU_get_current_rtems_irq_handler(ptr)
#define BSP_remove_rtems_irq_handler(ptr) 	CPU_remove_rtems_irq_handler(ptr)
#define BSP_rtems_irq_mngt_set(config)		CPU_rtems_irq_mngt_set(config)
#define BSP_rtems_irq_mngt_get(config)		CPU_rtems_irq_mngt_get(config)
#define BSP_rtems_irq_mng_init(cpuId)		CPU_rtems_irq_mng_init(cpuId)

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif /* LIBBSP_POWERPC_SS555_IRQ_IRQ_H */
