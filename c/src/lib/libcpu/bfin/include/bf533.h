/*  Blackfin BF533 Definitions
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _bf533_h_
#define _bf533_h_

/* register (or register block) addresses */

#define SIC_BASE_ADDRESS                          0xffc00100
#define WDOG_BASE_ADDRESS                         0xffc00200
#define RTC_BASE_ADDRESS                          0xffc00300
#define UART0_BASE_ADDRESS                        0xffc00400
#define SPI_BASE_ADDRESS                          0xffc00500
#define TIMER_BASE_ADDRESS                        0xffc00600
#define TIMER_CHANNELS                                     3
#define TIMER_PITCH                                     0x10
#define TIMER0_BASE_ADDRESS                       0xffc00600
#define TIMER1_BASE_ADDRESS                       0xffc00610
#define TIMER2_BASE_ADDRESS                       0xffc00620
#define TIMER_ENABLE                              0xffc00640
#define TIMER_DISABLE                             0xffc00644
#define TIMER_STATUS                              0xffc00648
#define PORTFIO_BASE_ADDRESS                      0xffc00700
#define SPORT0_BASE_ADDRESS                       0xffc00800
#define SPORT1_BASE_ADDRESS                       0xffc00900
#define EBIU_BASE_ADDRESS                         0xffc00a00
#define DMA_TC_PER                                0xffc00b0c
#define DMA_TC_CNT                                0xffc00b10
#define DMA_BASE_ADDRESS                          0xffc00c00
#define DMA_CHANNELS                                       8
#define DMA_PITCH                                       0x40
#define DMA0_BASE_ADDRESS                         0xffc00c00
#define DMA1_BASE_ADDRESS                         0xffc00c40
#define DMA2_BASE_ADDRESS                         0xffc00c80
#define DMA3_BASE_ADDRESS                         0xffc00cc0
#define DMA4_BASE_ADDRESS                         0xffc00d00
#define DMA5_BASE_ADDRESS                         0xffc00d40
#define DMA6_BASE_ADDRESS                         0xffc00d80
#define DMA7_BASE_ADDRESS                         0xffc00dc0
#define MDMA_BASE_ADDRESS                         0xffc00e00
#define MDMA_CHANNELS                                      2
#define MDMA_D_S                                        0x40
#define MDMA_PITCH                                      0x80
#define MDMA0D_BASE_ADDRESS                       0xffc00e00
#define MDMA0S_BASE_ADDRESS                       0xffc00e40
#define MDMA1D_BASE_ADDRESS                       0xffc00e80
#define MDMA1S_BASE_ADDRESS                       0xffc00ec0
#define PPI_BASE_ADDRESS                          0xffc01000


/* register fields */

#define DMA_TC_PER_MDMA_ROUND_ROBIN_PERIOD_MASK       0xf800
#define DMA_TC_PER_MDMA_ROUND_ROBIN_PERIOD_SHIFT          11
#define DMA_TC_PER_DAB_TRAFFIC_PERIOD_MASK            0x0700
#define DMA_TC_PER_DAB_TRAFFIC_PERIOD_SHIFT                8
#define DMA_TC_PER_DEB_TRAFFIC_PERIOD_MASK            0x00f0
#define DMA_TC_PER_DEB_TRAFFIC_PERIOD_SHIFT                4
#define DMA_TC_PER_DCB_TRAFFIC_PERIOD_MASK            0x000f
#define DMA_TC_PER_DCB_TRAFFIC_PERIOD_SHIFT                0

#define DMA_TC_CNT_MDMA_ROUND_ROBIN_COUNT_MASK        0xf800
#define DMA_TC_CNT_MDMA_ROUND_ROBIN_COUNT_SHIFT           11
#define DMA_TC_CNT_DAB_TRAFFIC_COUNT_MASK             0x0700
#define DMA_TC_CNT_DAB_TRAFFIC_COUNT_SHIFT                 8
#define DMA_TC_CNT_DEB_TRAFFIC_COUNT_MASK             0x00f0
#define DMA_TC_CNT_DEB_TRAFFIC_COUNT_SHIFT                 4
#define DMA_TC_CNT_DCB_TRAFFIC_COUNT_MASK             0x000f
#define DMA_TC_CNT_DCB_TRAFFIC_COUNT_SHIFT                 0

#define TIMER_ENABLE_TIMEN2                           0x0004
#define TIMER_ENABLE_TIMEN1                           0x0002
#define TIMER_ENABLE_TIMEN0                           0x0001

#define TIMER_DISABLE_TIMDIS2                         0x0004
#define TIMER_DISABLE_TIMDIS1                         0x0002
#define TIMER_DISABLE_TIMDIS0                         0x0001

#define TIMER_STATUS_TRUN2                        0x00004000
#define TIMER_STATUS_TRUN1                        0x00002000
#define TIMER_STATUS_TRUN0                        0x00001000
#define TIMER_STATUS_TOVF_ERR2                    0x00000040
#define TIMER_STATUS_TOVF_ERR1                    0x00000020
#define TIMER_STATUS_TOVF_ERR0                    0x00000010
#define TIMER_STATUS_TIMIL2                       0x00000004
#define TIMER_STATUS_TIMIL1                       0x00000002
#define TIMER_STATUS_TIMIL0                       0x00000001

/* Core Event Controller vectors */

#define CEC_EMULATION_VECTOR                               0
#define CEC_RESET_VECTOR                                   1
#define CEC_NMI_VECTOR                                     2
#define CEC_EXCEPTIONS_VECTOR                              3
#define CEC_HARDWARE_ERROR_VECTOR                          5
#define CEC_CORE_TIMER_VECTOR                              6
#define CEC_INTERRUPT_BASE_VECTOR                          7
#define CEC_INTERRUPT_COUNT                                9


/* System Interrupt Controller vectors */

#define SIC_IAR_COUNT                                      3

#define SIC_PLL_WAKEUP_VECTOR                              0
#define SIC_DMA_ERROR_VECTOR                               1
#define SIC_PPI_ERROR_VECTOR                               2
#define SIC_SPORT0_ERROR_VECTOR                            3
#define SIC_SPORT1_ERROR_VECTOR                            4
#define SIC_SPI_ERROR_VECTOR                               5
#define SIC_UART0_ERROR_VECTOR                             6
#define SIC_RTC_VECTOR                                     7
#define SIC_DMA0_PPI_VECTOR                                8
#define SIC_DMA1_SPORT0_RX_VECTOR                          9
#define SIC_DMA2_SPORT0_TX_VECTOR                         10
#define SIC_DMA3_SPORT1_RX_VECTOR                         11
#define SIC_DMA4_SPORT1_TX_VECTOR                         12
#define SIC_DMA5_SPI_VECTOR                               13
#define SIC_DMA6_UART0_RX_VECTOR                          14
#define SIC_DMA7_UART0_TX_VECTOR                          15
#define SIC_TIMER0_VECTOR                                 16
#define SIC_TIMER1_VECTOR                                 17
#define SIC_TIMER2_VECTOR                                 18
#define SIC_MDMA0_VECTOR                                  21
#define SIC_MDMA1_VECTOR                                  22
#define SIC_WATCHDOG_VECTOR                               23

#endif /* _bf533_h_ */

