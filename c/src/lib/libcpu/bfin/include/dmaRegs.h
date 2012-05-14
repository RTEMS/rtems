/*  Blackfin DMA Registers
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _dmaRegs_h_
#define _dmaRegs_h_


/* register addresses */

#define DMA_NEXT_DESC_PTR_OFFSET                      0x0000
#define DMA_START_ADDR_OFFSET                         0x0004
#define DMA_CONFIG_OFFSET                             0x0008
#define DMA_X_COUNT_OFFSET                            0x0010
#define DMA_X_MODIFY_OFFSET                           0x0014
#define DMA_Y_COUNT_OFFSET                            0x0018
#define DMA_Y_MODIFY_OFFSET                           0x001c
#define DMA_CURR_DESC_PTR_OFFSET                      0x0020
#define DMA_CURR_ADDR_OFFSET                          0x0024
#define DMA_IRQ_STATUS_OFFSET                         0x0028
#define DMA_PERIPHERAL_MAP_OFFSET                     0x002c
#define DMA_CURR_X_COUNT_OFFSET                       0x0030
#define DMA_CURR_Y_COUNT_OFFSET                       0x0038

#define HMDMA_CONTROL_OFFSET                          0x0000
#define HMDMA_ECINIT_OFFSET                           0x0004
#define HMDMA_BCINIT_OFFSET                           0x0008
#define HMDMA_ECURGENT_OFFSET                         0x000c
#define HMDMA_ECOVERFLOW_OFFSET                       0x0010
#define HMDMA_ECOUNT_OFFSET                           0x0014
#define HMDMA_BCOUNT_OFFSET                           0x0018


/* register fields */

#define DMA_CONFIG_FLOW_MASK                          0x7000
#define DMA_CONFIG_FLOW_STOP                          0x0000
#define DMA_CONFIG_FLOW_AUTOBUFFER                    0x1000
#define DMA_CONFIG_FLOW_DESC_ARRAY                    0x4000
#define DMA_CONFIG_FLOW_DESC_SMALL                    0x6000
#define DMA_CONFIG_FLOW_DESC_LARGE                    0x7000
#define DMA_CONFIG_NDSIZE_MASK                        0x0f00
#define DMA_CONFIG_NDSIZE_SHIFT                            8
#define DMA_CONFIG_DI_EN                              0x0080
#define DMA_CONFIG_DI_SEL                             0x0040
#define DMA_CONFIG_SYNC                               0x0020
#define DMA_CONFIG_DMA2D                              0x0010
#define DMA_CONFIG_WDSIZE_MASK                        0x000c
#define DMA_CONFIG_WDSIZE_8                           0x0000
#define DMA_CONFIG_WDSIZE_16                          0x0004
#define DMA_CONFIG_WDSIZE_32                          0x0008
#define DMA_CONFIG_WNR                                0x0002
#define DMA_CONFIG_DMAEN                              0x0001

#define DMA_IRQ_STATUS_DMA_RUN                        0x0008
#define DMA_IRQ_STATUS_DFETCH                         0x0004
#define DMA_IRQ_STATUS_DMA_ERR                        0x0002
#define DMA_IRQ_STATUS_DMA_DONE                       0x0001

#define DMA_PERIPHERAL_MAP_PMAP_MASK                  0xf000
#define DMA_PERIPHERAL_MAP_PMAP_PPI                   0x0000
#define DMA_PERIPHERAL_MAP_PMAP_ETHRX                 0x1000
#define DMA_PERIPHERAL_MAP_PMAP_ETHTX                 0x2000
#define DMA_PERIPHERAL_MAP_PMAP_SPORT0RX              0x3000
#define DMA_PERIPHERAL_MAP_PMAP_SPORT0TX              0x4000
#define DMA_PERIPHERAL_MAP_PMAP_SPORT1RX              0x5000
#define DMA_PERIPHERAL_MAP_PMAP_SPORT1TX              0x6000
#define DMA_PERIPHERAL_MAP_PMAP_SPI                   0x7000
#define DMA_PERIPHERAL_MAP_PMAP_UART0RX               0x8000
#define DMA_PERIPHERAL_MAP_PMAP_UART0TX               0x9000
#define DMA_PERIPHERAL_MAP_PMAP_UART1RX               0xa000
#define DMA_PERIPHERAL_MAP_PMAP_UART1TX               0xb000
#define DMA_PERIPHERAL_MAP_CTYPE                      0x0040

#define HMDMA_CONTROL_BDI                             0x8000
#define HMDMA_CONTROL_OI                              0x4000
#define HMDMA_CONTROL_PS                              0x2000
#define HMDMA_CONTROL_RBC                             0x1000
#define HMDMA_CONTROL_DRQ_MASK                        0x0300
#define HMDMA_CONTROL_DRQ_NONE                        0x0000
#define HMDMA_CONTROL_DRQ_SINGLE                      0x0100
#define HMDMA_CONTROL_DRQ_MULTIPLE                    0x0200
#define HMDMA_CONTROL_DRQ_URGENT_MULTIPLE             0x0300
#define HMDMA_CONTROL_MBDI                            0x0040
#define HMDMA_CONTROL_BDIE                            0x0020
#define HMDMA_CONTROL_OIE                             0x0010
#define HMDMA_CONTROL_UTE                             0x0008
#define HMDMA_CONTROL_REP                             0x0002
#define HMDMA_CONTROL_HMDMAEN                         0x0001

#endif /* _dmaRegs_h_ */

