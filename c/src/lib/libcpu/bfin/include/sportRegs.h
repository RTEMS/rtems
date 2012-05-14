/*  Blackfin SPORT Registers
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _sportRegs_h_
#define _sportRegs_h_


/* register addresses */

#define SPORT_TCR1_OFFSET                             0x0000
#define SPORT_TCR2_OFFSET                             0x0004
#define SPORT_TCLKDIV_OFFSET                          0x0008
#define SPORT_TFSDIV_OFFSET                           0x000c
#define SPORT_TX_OFFSET                               0x0010
#define SPORT_RX_OFFSET                               0x0018
#define SPORT_RCR1_OFFSET                             0x0020
#define SPORT_RCR2_OFFSET                             0x0024
#define SPORT_RCLKDIV_OFFSET                          0x0028
#define SPORT_RFSDIV_OFFSET                           0x002c
#define SPORT_STAT_OFFSET                             0x0030
#define SPORT_CHNL_OFFSET                             0x0034
#define SPORT_MCMC1_OFFSET                            0x0038
#define SPORT_MCMC2_OFFSET                            0x003c
#define SPORT_MTCS0_OFFSET                            0x0040
#define SPORT_MTCS1_OFFSET                            0x0044
#define SPORT_MTCS2_OFFSET                            0x0048
#define SPORT_MTCS3_OFFSET                            0x004c
#define SPORT_MRCS0_OFFSET                            0x0050
#define SPORT_MRCS1_OFFSET                            0x0054
#define SPORT_MRCS2_OFFSET                            0x0058
#define SPORT_MRCS3_OFFSET                            0x005c


/* register fields */

#define SPORT_TCR1_TCKFE                              0x4000
#define SPORT_TCR1_LATFS                              0x2000
#define SPORT_TCR1_LTFS                               0x1000
#define SPORT_TCR1_DITFS                              0x0800
#define SPORT_TCR1_TFSR                               0x0400
#define SPORT_TCR1_ITFS                               0x0200
#define SPORT_TCR1_TLSBIT                             0x0010
#define SPORT_TCR1_TDTYPE_MASK                        0x000c
#define SPORT_TCR1_TDTYPE_NORMAL                      0x0000
#define SPORT_TCR1_TDTYPE_ULAW                        0x0008
#define SPORT_TCR1_TDTYPE_ALAW                        0x000c
#define SPORT_TCR1_ITCLK                              0x0002
#define SPORT_TCR1_TSPEN                              0x0001

#define SPORT_TCR2_TRFST                              0x0400
#define SPORT_TCR2_TSFSE                              0x0200
#define SPORT_TCR2_TXSE                               0x0100
#define SPORT_TCR2_SLEN_MASK                          0x001f
#define SPORT_TCR2_SLEN_SHIFT                              0

#define SPORT_RCR1_RCKFE                              0x4000
#define SPORT_RCR1_LARFS                              0x2000
#define SPORT_RCR1_LRFS                               0x1000
#define SPORT_RCR1_RFSR                               0x0400
#define SPORT_RCR1_IRFS                               0x0200
#define SPORT_RCR1_RLSBIT                             0x0010
#define SPORT_RCR1_RDTYPE_MASK                        0x000c
#define SPORT_RCR1_RDTYPE_ZEROFILL                    0x0000
#define SPORT_RCR1_RDTYPE_SIGNEXTEND                  0x0004
#define SPORT_RCR1_RDTYPE_ULAW                        0x0008
#define SPORT_RCR1_RDTYPE_ALAW                        0x000c
#define SPORT_RCR1_IRCLK                              0x0002
#define SPORT_RCR1_RSPEN                              0x0001

#define SPORT_RCR2_RRFST                              0x0400
#define SPORT_RCR2_RSFSE                              0x0200
#define SPORT_RCR2_RXSE                               0x0100
#define SPORT_RCR2_SLEN_MASK                          0x001f
#define SPORT_RCR2_SLEN_SHIFT                              0

#define SPORT_STAT_TXHRE                              0x0040
#define SPORT_STAT_TOVF                               0x0020
#define SPORT_STAT_TUVF                               0x0010
#define SPORT_STAT_TXF                                0x0008
#define SPORT_STAT_ROVF                               0x0004
#define SPORT_STAT_RUVF                               0x0002
#define SPORT_STAT_RXNE                               0x0001

#define SPORT_CHNL_CHNL_MASK                          0x03ff
#define SPORT_CHNL_CHNL_SHIFT                              0

#define SPORT_MCMC1_WSIZE_MASK                        0xf000
#define SPORT_MCMC1_WSIZE_SHIFT                           12
#define SPORT_MCMC1_WOFF_MASK                         0x03ff
#define SPORT_MCMC1_WOFF_SHIFT                             0

#define SPORT_MCMC2_MFD_MASK                          0xf000
#define SPORT_MCMC2_MFD_SHIFT                             12
#define SPORT_MCMC2_FSDR                              0x0080
#define SPORT_MCMC2_MCMEN                             0x0010
#define SPORT_MCMC2_MCDRXPE                           0x0008
#define SPORT_MCMC2_MCDTXPE                           0x0004
#define SPORT_MCMC2_MCCRM_MASK                        0x0003
#define SPORT_MCMC2_MCCRM_BYPASS                      0x0000
#define SPORT_MCMC2_MCCRM_2_4                         0x0002
#define SPORT_MCMC2_MCCRM_8_16                        0x0003


#endif /* _sportRegs_h_ */

