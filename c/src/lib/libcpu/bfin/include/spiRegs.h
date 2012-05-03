/*  Blackfin SPI Registers
 *
 *  Copyright (c) 2010 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _spiRegs_h_
#define _spiRegs_h_


/* register addresses */

#define SPI_CTL_OFFSET                                0x0000
#define SPI_FLG_OFFSET                                0x0004
#define SPI_STAT_OFFSET                               0x0008
#define SPI_TDBR_OFFSET                               0x000c
#define SPI_RDBR_OFFSET                               0x0010
#define SPI_BAUD_OFFSET                               0x0014
#define SPI_SHADOW_OFFSET                             0x0018


/* register fields */

#define SPI_CTL_SPE                                   0x4000
#define SPI_CTL_WOM                                   0x2000
#define SPI_CTL_MSTR                                  0x1000
#define SPI_CTL_CPOL                                  0x0800
#define SPI_CTL_CPHA                                  0x0400
#define SPI_CTL_LSBF                                  0x0200
#define SPI_CTL_SIZE                                  0x0100
#define SPI_CTL_EMISO                                 0x0020
#define SPI_CTL_PSSE                                  0x0010
#define SPI_CTL_GM                                    0x0008
#define SPI_CTL_SZ                                    0x0004
#define SPI_CTL_TIMOD_MASK                            0x0003
#define SPI_CTL_TIMOD_RDBR                            0x0000
#define SPI_CTL_TIMOD_TDBR                            0x0001
#define SPI_CTL_TIMOD_DMA_RDBR                        0x0002
#define SPI_CTL_TIMOD_DMA_TDBR                        0x0003

#define SPI_FLG_FLG7                                  0x8000
#define SPI_FLG_FLG6                                  0x4000
#define SPI_FLG_FLG5                                  0x2000
#define SPI_FLG_FLG4                                  0x1000
#define SPI_FLG_FLG3                                  0x0800
#define SPI_FLG_FLG2                                  0x0400
#define SPI_FLG_FLG1                                  0x0200
#define SPI_FLG_FLS7                                  0x0080
#define SPI_FLG_FLS6                                  0x0040
#define SPI_FLG_FLS5                                  0x0020
#define SPI_FLG_FLS4                                  0x0010
#define SPI_FLG_FLS3                                  0x0008
#define SPI_FLG_FLS2                                  0x0004
#define SPI_FLG_FLS1                                  0x0002

#define SPI_STAT_TXCOL                                0x0040
#define SPI_STAT_RXS                                  0x0020
#define SPI_STAT_RBSY                                 0x0010
#define SPI_STAT_TXS                                  0x0008
#define SPI_STAT_TXE                                  0x0004
#define SPI_STAT_MODF                                 0x0002
#define SPI_STAT_SPIF                                 0x0001


#endif /* _spiRegs_h_ */

