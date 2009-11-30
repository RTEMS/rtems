/*  ictrl.h
 *
 *  This file contains definitions and declarations for the
 *  PowerPC 403 CPU built-in external interrupt controller
 *
 *
 *  Author: Thomas Doerfler <td@imd.m.isar.de>
 *
 *  COPYRIGHT (c) 1998 by IMD, Puchheim, Germany
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of IMD not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      IMD makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Modifications for PPC405GP by Dennis Ehlin
 *
 */


#ifndef _ICTRL_H
#define _ICTRL_H

#include <rtems.h>
#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/powerpc/powerpc.h>
#ifdef __cplusplus
extern "C" {
#endif

/*
 *  definitions for second level IRQ handler support
 *  External Interrupts via EXTERNAL/EISR
 */
#define PPC_IRQ_EXT_BASE        (PPC_IRQ_LAST+1)

/* mask for external interrupt status in EXIER/EXISR register */
/* note: critical interrupt is in these registers aswell */
#ifndef ppc405
#define PPC_EXI_MASK           0x0FFFFFFF
#else /* ppc405 */
#define PPC_EXI_MASK           0xFFFFFFFF
#endif /* ppc405 */

#ifndef ppc405
#define PPC_IRQ_EXT_SPIR        (PPC_IRQ_EXT_BASE+4)
#define PPC_IRQ_EXT_SPIT        (PPC_IRQ_EXT_BASE+5)
#else /* ppc405 */
#define PPC_IRQ_EXT_UART0       (PPC_IRQ_EXT_BASE+0)
#define PPC_IRQ_EXT_UART1       (PPC_IRQ_EXT_BASE+1)
#endif /* ppc405 */
#define PPC_IRQ_EXT_JTAGR       (PPC_IRQ_EXT_BASE+6)
#define PPC_IRQ_EXT_JTAGT       (PPC_IRQ_EXT_BASE+7)
#define PPC_IRQ_EXT_DMA0        (PPC_IRQ_EXT_BASE+8)
#define PPC_IRQ_EXT_DMA1        (PPC_IRQ_EXT_BASE+9)
#define PPC_IRQ_EXT_DMA2        (PPC_IRQ_EXT_BASE+10)
#define PPC_IRQ_EXT_DMA3        (PPC_IRQ_EXT_BASE+11)
#define PPC_IRQ_EXT_0           (PPC_IRQ_EXT_BASE+27)
#define PPC_IRQ_EXT_1           (PPC_IRQ_EXT_BASE+28)
#define PPC_IRQ_EXT_2           (PPC_IRQ_EXT_BASE+29)
#define PPC_IRQ_EXT_3           (PPC_IRQ_EXT_BASE+30)
#define PPC_IRQ_EXT_4           (PPC_IRQ_EXT_BASE+31)

#define PPC_IRQ_EXT_MAX         (32)

#define VEC_TO_EXMSK(v)         (0x80000000 >> (v))

/*
 *
 * install a user vector for one of the external interrupt sources
 *
 */
rtems_status_code
ictrl_set_vector(rtems_isr_entry   new_handler,
		 uint32_t          vector,
		 rtems_isr_entry   *old_handler
);
/*
 * activate the interrupt controller
 */
rtems_status_code
ictrl_init(void);

#ifdef __cplusplus
}
#endif

#endif /* _ICTRL_H */
/* end of include file */
