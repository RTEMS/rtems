/*  Blackfin Two Wire Interface Registers
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _twiRegs_h_
#define _twiRegs_h_


/* register addresses */

#define TWI_CLKDIV_OFFSET                             0x0000
#define TWI_CONTROL_OFFSET                            0x0004
#define TWI_SLAVE_CTL_OFFSET                          0x0008
#define TWI_SLAVE_STAT_OFFSET                         0x000c
#define TWI_SLAVE_ADDR_OFFSET                         0x0010
#define TWI_MASTER_CTL_OFFSET                         0x0014
#define TWI_MASTER_STAT_OFFSET                        0x0018
#define TWI_MASTER_ADDR_OFFSET                        0x001c
#define TWI_INT_STAT_OFFSET                           0x0020
#define TWI_INT_MASK_OFFSET                           0x0024
#define TWI_FIFO_CTL_OFFSET                           0x0028
#define TWI_FIFO_STAT_OFFSET                          0x002c
#define TWI_XMT_DATA8_OFFSET                          0x0080
#define TWI_XMT_DATA16_OFFSET                         0x0084
#define TWI_RCV_DATA8_OFFSET                          0x0088
#define TWI_RCV_DATA16_OFFSET                         0x008c


/* register fields */

#define TWI_CLKDIV_CLKHI_MASK                         0xff00
#define TWI_CLKDIV_CLKHI_SHIFT                             8
#define TWI_CLKDIV_CLKLOW_MASK                        0x00ff
#define TWI_CLKDIV_CLKLOW_SHIFT                            0

#define TWI_CONTROL_SCCB                              0x0200
#define TWI_CONTROL_TWI_ENA                           0x0080
#define TWI_CONTROL_PRESCALE_MASK                     0x007f
#define TWI_CONTROL_PRESCALE_SHIFT                         0

#define TWI_SLAVE_CTL_GEN                             0x0010
#define TWI_SLAVE_CTL_NAK                             0x0008
#define TWI_SLAVE_CTL_STDVAL                          0x0004
#define TWI_SLAVE_CTL_SEN                             0x0001

#define TWI_SLAVE_STAT_GCALL                          0x0002
#define TWI_SLAVE_STAT_SDIR                           0x0001

#define TWI_SLAVE_ADDR_SADDR_MASK                     0x007f
#define TWI_SLAVE_ADDR_SADDR_SHIFT                         0

#define TWI_MASTER_CTL_SCLOVR                         0x8000
#define TWI_MASTER_CTL_SDAOVR                         0x4000
#define TWI_MASTER_CTL_DCNT_MASK                      0x3fc0
#define TWI_MASTER_CTL_DCNT_SHIFT                          6
#define TWI_MASTER_CTL_RSTART                         0x0020
#define TWI_MASTER_CTL_STOP                           0x0010
#define TWI_MASTER_CTL_FAST                           0x0008
#define TWI_MASTER_CTL_MDIR                           0x0004
#define TWI_MASTER_CTL_MEN                            0x0001

#define TWI_MASTER_STAT_BUSBUSY                       0x0100
#define TWI_MASTER_STAT_SCLSEN                        0x0080
#define TWI_MASTER_STAT_SDASEN                        0x0040
#define TWI_MASTER_STAT_BUFWRERR                      0x0020
#define TWI_MASTER_STAT_BUFRDERR                      0x0010
#define TWI_MASTER_STAT_DNAK                          0x0008
#define TWI_MASTER_STAT_ANAK                          0x0004
#define TWI_MASTER_STAT_LOSTARB                       0x0002
#define TWI_MASTER_STAT_MPROG                         0x0001

#define TWI_MASTER_ADDR_MADDR_MASK                    0x007f
#define TWI_MASTER_ADDR_MADDR_SHIFT                        0

#define TWI_INT_STAT_RCVSERV                          0x0080
#define TWI_INT_STAT_XMTSERV                          0x0040
#define TWI_INT_STAT_MERR                             0x0020
#define TWI_INT_STAT_MCOMP                            0x0010
#define TWI_INT_STAT_SOVF                             0x0008
#define TWI_INT_STAT_SERR                             0x0004
#define TWI_INT_STAT_SCOMP                            0x0002
#define TWI_INT_STAT_SINIT                            0x0001

#define TWI_INT_MASK_RCVSERVM                         0x0080
#define TWI_INT_MASK_XMTSERVM                         0x0040
#define TWI_INT_MASK_MERRM                            0x0020
#define TWI_INT_MASK_MCOMPM                           0x0010
#define TWI_INT_MASK_SOVFM                            0x0008
#define TWI_INT_MASK_SERRM                            0x0004
#define TWI_INT_MASK_SCOMPM                           0x0002
#define TWI_INT_MASK_SINITM                           0x0001

#define TWI_FIFO_CTL_RCVINTLEN                        0x0008
#define TWI_FIFO_CTL_XMTINTLEN                        0x0004
#define TWI_FIFO_CTL_RCVFLUSH                         0x0002
#define TWI_FIFO_CTL_XMTFLUSH                         0x0001

#define TWI_FIFO_STAT_RCVSTAT_MASK                    0x000c
#define TWI_FIFO_STAT_RCVSTAT_EMPTY                   0x0000
#define TWI_FIFO_STAT_RCVSTAT_SHIFT                        2
#define TWI_FIFO_STAT_XMTSTAT_MASK                    0x0003
#define TWI_FIFO_STAT_XMTSTAT_FULL                    0x0003
#define TWI_FIFO_STAT_XMTSTAT_SHIFT                        0

#define TWI_XMT_DATA8_XMTDATA8_MASK                   0x00ff
#define TWI_XMT_DATA8_XMTDATA8_SHIFT                       0

#define TWI_RCV_DATA8_RCVDATA8_MASK                   0x00ff
#define TWI_RCV_DATA8_RCVDATA8_SHIFT                       0


#endif /* _twiRegs_h_ */

