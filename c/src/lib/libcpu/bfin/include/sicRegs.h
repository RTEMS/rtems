/*  Blackfin System Interrupt Controller Registers
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _sicRegs_h_
#define _sicRegs_h_

/* register addresses */

#define SIC_IMASK                (SIC_BASE_ADDRESS + 0x000c)
#define SIC_IMASK_PITCH          (0x40)

#define SIC_ISR                  (SIC_BASE_ADDRESS + 0x0020)
#define SIC_ISR_PITCH            (0x40)

#define SIC_IAR_BASE_ADDRESS     (SIC_BASE_ADDRESS + 0x0010)
#define SIC_IAR_PITCH                                   0x04

#define SIC_IAR0                 (SIC_BASE_ADDRESS + 0x0010)
#if SIC_IAR_COUNT > 1
#define SIC_IAR1                 (SIC_BASE_ADDRESS + 0x0014)
#endif
#if SIC_IAR_COUNT > 2
#define SIC_IAR2                 (SIC_BASE_ADDRESS + 0x0018)
#endif
#if SIC_IAR_COUNT > 3
#define SIC_IAR3                 (SIC_BASE_ADDRESS + 0x001c)
#endif

#define SIC_IWR                  (SIC_BASE_ADDRESS + 0x0024)


/* register fields */



#endif /* _sicRegs_h_ */

