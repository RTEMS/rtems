/*  Blackfin Core Event Controller Registers
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _cecRegs_h_
#define _cecRegs_h_

/* register addresses */
#define CEC_EVT_BASE                              0xffe02000
#define CEC_EVT_COUNT                                     16
#define CEC_EVT_PITCH                                   0x04
#define CEC_EVT0                                  0xffe02000
#define CEC_EVT1                                  0xffe02004
#define CEC_EVT2                                  0xffe02008
#define CEC_EVT3                                  0xffe0200c
#define CEC_EVT4                                  0xffe02010
#define CEC_EVT5                                  0xffe02014
#define CEC_EVT6                                  0xffe02018
#define CEC_EVT7                                  0xffe0201c
#define CEC_EVT8                                  0xffe02020
#define CEC_EVT9                                  0xffe02024
#define CEC_EVT10                                 0xffe02028
#define CEC_EVT11                                 0xffe0202c
#define CEC_EVT12                                 0xffe02030
#define CEC_EVT13                                 0xffe02034
#define CEC_EVT14                                 0xffe02038
#define CEC_EVT15                                 0xffe0203c
#define CEC_IMASK                                 0xffe02104
#define CEC_IPEND                                 0xffe02108
#define CEC_ILAT                                  0xffe0210c
#define CEC_IPRIO                                 0xffe02110


/* register fields */

#define CEC_IPRIO_IPRIO_MARK_MASK                 0x0000000f
#define CEC_IPRIO_IPRIO_MARK_SHIFT                         0


#endif /* _cecRegs_h_ */

