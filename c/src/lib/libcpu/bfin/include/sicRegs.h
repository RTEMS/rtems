/*  Blackfin System Interrupt Controller Registers
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _sicRegs_h_
#define _sicRegs_h_

/* register addresses */

#define SIC_IMASK                (SIC_BASE_ADDRESS + 0x000c)
#define SIC_IAR_BASE_ADDRESS     (SIC_BASE_ADDRESS + 0x0010)
#define SIC_IAR_COUNT                                      4
#define SIC_IAR_PITCH                                   0x04 
#define SIC_IAR0                 (SIC_BASE_ADDRESS + 0x0010)
#define SIC_IAR1                 (SIC_BASE_ADDRESS + 0x0014)
#define SIC_IAR2                 (SIC_BASE_ADDRESS + 0x0018)
#define SIC_IAR3                 (SIC_BASE_ADDRESS + 0x001c)
#define SIC_ISR                  (SIC_BASE_ADDRESS + 0x0020)
#define SIC_IWR                  (SIC_BASE_ADDRESS + 0x0024)


/* register fields */



#endif /* _sicRegs_h_ */

