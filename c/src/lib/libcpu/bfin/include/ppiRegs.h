/*  Blackfin Parallel Peripheral Interface Registers
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _ppiRegs_h_
#define _ppiRegs_h_


/* register addresses */

#define PPI_CONTROL_OFFSET                            0x0000
#define PPI_STATUS_OFFSET                             0x0004
#define PPI_COUNT_OFFSET                              0x0008
#define PPI_DELAY_OFFSET                              0x000c
#define PPI_FRAME_OFFSET                              0x0010


/* register fields */

#define PPI_CONTROL_POLS                              0x8000
#define PPI_CONTROL_POLC                              0x4000
#define PPI_CONTROL_DLEN_MASK                         0x3800
#define PPI_CONTROL_DLEN_8                            0x0000
#define PPI_CONTROL_DLEN_10                           0x0800
#define PPI_CONTROL_DLEN_11                           0x1000
#define PPI_CONTROL_DLEN_12                           0x1800
#define PPI_CONTROL_DLEN_13                           0x2000
#define PPI_CONTROL_DLEN_14                           0x2800
#define PPI_CONTROL_DLEN_15                           0x3000
#define PPI_CONTROL_DLEN_16                           0x3800
#define PPI_CONTROL_SKIP_EO                           0x0400
#define PPI_CONTROL_SKIP_EN                           0x0200
#define PPI_CONTROL_PACK_EN                           0x0080
#define PPI_CONTROL_FLD_SEL                           0x0040
#define PPI_CONTROL_PORT_CFG_MASK                     0x0030
#define PPI_CONTROL_PORT_CFG_SHIFT                         4
#define PPI_CONTROL_XFR_TYPE_MASK                     0x000c
#define PPI_CONTROL_XFR_TYPE_SHIFT                         2
#define PPI_CONTROL_PORT_DIR                          0x0002
#define PPI_CONTROL_PORT_EN                           0x0001

#define PPI_STATUS_ERR_NCOR                           0x8000
#define PPI_STATUS_ERR_DET                            0x4000
#define PPI_STATUS_UNDR                               0x2000
#define PPI_STATUS_OVR                                0x1000
#define PPI_STATUS_FT_ERR                             0x0800
#define PPI_STATUS_FLD                                0x0400
#define PPI_STATUS_LT_ERR_UNDR                        0x0200
#define PPI_STATUS_LT_ERR_OVR                         0x0100


#endif /* _ppiRegs_h_ */

