/*  Blackfin BF537 Definitions
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _bf537_h_
#define _bf537_h_

/* register (or register block) addresses */

#define SIC_BASE_ADDRESS                          0xffc00100
#define WDOG_BASE_ADDRESS                         0xffc00200
#define RTC_BASE_ADDRESS                          0xffc00300
#define UART0_BASE_ADDRESS                        0xffc00400
#define SPI_BASE_ADDRESS                          0xffc00500
#define TIMER_BASE_ADDRESS                        0xffc00600
#define TIMER_CHANNELS                                     8
#define TIMER_PITCH                                     0x10
#define TIMER0_BASE_ADDRESS                       0xffc00600
#define TIMER1_BASE_ADDRESS                       0xffc00610
#define TIMER2_BASE_ADDRESS                       0xffc00620
#define TIMER3_BASE_ADDRESS                       0xffc00630
#define TIMER4_BASE_ADDRESS                       0xffc00640
#define TIMER5_BASE_ADDRESS                       0xffc00650
#define TIMER6_BASE_ADDRESS                       0xffc00660
#define TIMER7_BASE_ADDRESS                       0xffc00670
#define TIMER_ENABLE                              0xffc00680
#define TIMER_DISABLE                             0xffc00684
#define TIMER_STATUS                              0xffc00688
#define PORTFIO_BASE_ADDRESS                      0xffc00700
#define SPORT0_BASE_ADDRESS                       0xffc00800
#define SPORT1_BASE_ADDRESS                       0xffc00900
#define EBIU_BASE_ADDRESS                         0xffc00a00
#define DMA_TC_PER                                0xffc00b0c
#define DMA_TC_CNT                                0xffc00b10
#define DMA_BASE_ADDRESS                          0xffc00c00
#define DMA_CHANNELS                                      12
#define DMA_PITCH                                       0x40
#define DMA0_BASE_ADDRESS                         0xffc00c00
#define DMA1_BASE_ADDRESS                         0xffc00c40
#define DMA2_BASE_ADDRESS                         0xffc00c80
#define DMA3_BASE_ADDRESS                         0xffc00cc0
#define DMA4_BASE_ADDRESS                         0xffc00d00
#define DMA5_BASE_ADDRESS                         0xffc00d40
#define DMA6_BASE_ADDRESS                         0xffc00d80
#define DMA7_BASE_ADDRESS                         0xffc00dc0
#define DMA8_BASE_ADDRESS                         0xffc00e00
#define DMA9_BASE_ADDRESS                         0xffc00e40
#define DMA10_BASE_ADDRESS                        0xffc00e80
#define DMA11_BASE_ADDRESS                        0xffc00ec0
#define MDMA_BASE_ADDRESS                         0xffc00f00
#define MDMA_CHANNELS                                      2
#define MDMA_D_S                                        0x40
#define MDMA_PITCH                                      0x80
#define MDMA0D_BASE_ADDRESS                       0xffc00f00
#define MDMA0S_BASE_ADDRESS                       0xffc00f40
#define MDMA1D_BASE_ADDRESS                       0xffc00f80
#define MDMA1S_BASE_ADDRESS                       0xffc00fc0
#define PPI_BASE_ADDRESS                          0xffc01000
#define TWI_BASE_ADDRESS                          0xffc01400
#define PORTGIO_BASE_ADDRESS                      0xffc01500
#define PORTHIO_BASE_ADDRESS                      0xffc01700
#define UART1_BASE_ADDRESS                        0xffc02000
#define CAN_BASE_ADDRESS                          0xffc02a00
#define CAN_AM_BASE_ADDRESS                       0xffc02b00
#define CAN_MB_BASE_ADDRESS                       0xffc02c00
#define EMAC_BASE_ADDRESS                         0xffc03000
#define PORTF_FER                                 0xffc03200
#define PORTG_FER                                 0xffc03204
#define PORTH_FER                                 0xffc03208
#define PORT_MUX                                  0xffc0320c
#define HMDMA0_BASE_ADDRESS                       0xffc03300
#define HMDMA1_BASE_ADDRESS                       0xffc03340


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

#define TIMER_ENABLE_TIMEN7                           0x0080
#define TIMER_ENABLE_TIMEN6                           0x0040
#define TIMER_ENABLE_TIMEN5                           0x0020
#define TIMER_ENABLE_TIMEN4                           0x0010
#define TIMER_ENABLE_TIMEN3                           0x0008
#define TIMER_ENABLE_TIMEN2                           0x0004
#define TIMER_ENABLE_TIMEN1                           0x0002
#define TIMER_ENABLE_TIMEN0                           0x0001

#define TIMER_DISABLE_TIMDIS7                         0x0080
#define TIMER_DISABLE_TIMDIS6                         0x0040
#define TIMER_DISABLE_TIMDIS5                         0x0020
#define TIMER_DISABLE_TIMDIS4                         0x0010
#define TIMER_DISABLE_TIMDIS3                         0x0008
#define TIMER_DISABLE_TIMDIS2                         0x0004
#define TIMER_DISABLE_TIMDIS1                         0x0002
#define TIMER_DISABLE_TIMDIS0                         0x0001

#define TIMER_STATUS_TRUN7                        0x80000000
#define TIMER_STATUS_TRUN6                        0x40000000
#define TIMER_STATUS_TRUN5                        0x20000000
#define TIMER_STATUS_TRUN4                        0x10000000
#define TIMER_STATUS_TOVF_ERR7                    0x00800000
#define TIMER_STATUS_TOVF_ERR6                    0x00400000
#define TIMER_STATUS_TOVF_ERR5                    0x00200000
#define TIMER_STATUS_TOVF_ERR4                    0x00100000
#define TIMER_STATUS_TIMIL7                       0x00080000
#define TIMER_STATUS_TIMIL6                       0x00040000
#define TIMER_STATUS_TIMIL5                       0x00020000
#define TIMER_STATUS_TIMIL4                       0x00010000
#define TIMER_STATUS_TRUN3                        0x00008000
#define TIMER_STATUS_TRUN2                        0x00004000
#define TIMER_STATUS_TRUN1                        0x00002000
#define TIMER_STATUS_TRUN0                        0x00001000
#define TIMER_STATUS_TOVF_ERR3                    0x00000080
#define TIMER_STATUS_TOVF_ERR2                    0x00000040
#define TIMER_STATUS_TOVF_ERR1                    0x00000020
#define TIMER_STATUS_TOVF_ERR0                    0x00000010
#define TIMER_STATUS_TIMIL3                       0x00000008
#define TIMER_STATUS_TIMIL2                       0x00000004
#define TIMER_STATUS_TIMIL1                       0x00000002
#define TIMER_STATUS_TIMIL0                       0x00000001

#define PORT_MUX_PGTE                                 0x0800
#define PORT_MUX_PGRE                                 0x0400
#define PORT_MUX_PGSE                                 0x0200
#define PORT_MUX_PFFE                                 0x0100
#define PORT_MUX_PFS4E                                0x0080
#define PORT_MUX_PFS5E                                0x0040
#define PORT_MUX_PFS6E                                0x0020
#define PORT_MUX_PFTE                                 0x0010
#define PORT_MUX_PFDE                                 0x0008
#define PORT_MUX_PJCE_MASK                            0x0006
#define PORT_MUX_PJCE_DR0SEC_DTOSEC                   0x0000
#define PORT_MUX_PJCE_CANRX_CANTX                     0x0002
#define PORT_MUX_PJCE_SPISSEL7                        0x0004
#define PORT_MUX_PJSE                                 0x0001


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

#define SIC_IAR_COUNT                                      4

#define SIC_PLL_WAKEUP_VECTOR                              0
#define SIC_DMA_ERROR_VECTOR                               1
#define SIC_DMAR0_BLOCK_DONE_VECTOR                        1
#define SIC_DMAR1_BLOCK_DONE_VECTOR                        1
#define SIC_DMAR0_OVERFLOW_VECTOR                          1
#define SIC_DMAR1_OVERFLOW_VECTOR                          1
#define SIC_CAN_ERROR_VECTOR                               2
#define SIC_MAC_ERROR_VECTOR                               2
#define SIC_SPORT0_ERROR_VECTOR                            2
#define SIC_SPORT1_ERROR_VECTOR                            2
#define SIC_PPI_ERROR_VECTOR                               2
#define SIC_SPI_ERROR_VECTOR                               2
#define SIC_UART0_ERROR_VECTOR                             2
#define SIC_UART1_ERROR_VECTOR                             2
#define SIC_RTC_VECTOR                                     3
#define SIC_DMA0_PPI_VECTOR                                4
#define SIC_DMA3_SPORT0_RX_VECTOR                          5
#define SIC_DMA4_SPORT0_TX_VECTOR                          6
#define SIC_DMA5_SPORT1_RX_VECTOR                          7
#define SIC_DMA5_SPORT1_TX_VECTOR                          8
#define SIC_TWI_VECTOR                                     9
#define SIC_DMA7_SPI_VECTOR                               10
#define SIC_DMA8_UART0_RX_VECTOR                          11
#define SIC_DMA9_UART0_TX_VECTOR                          12
#define SIC_DMA10_UART1_RX_VECTOR                         13
#define SIC_DMA11_UART1_TX_VECTOR                         14
#define SIC_CAN_RX_VECTOR                                 15
#define SIC_CAN_TX_VECTOR                                 16
#define SIC_DMA1_MAC_RX_VECTOR                            17
#define SIC_PORTH_IRQ_A_VECTOR                            17
#define SIC_DMA2_MAC_TX_VECTOR                            18
#define SIC_PORTH_IRQ_B_VECTOR                            18
#define SIC_TIMER0_VECTOR                                 19
#define SIC_TIMER1_VECTOR                                 20
#define SIC_TIMER2_VECTOR                                 21
#define SIC_TIMER3_VECTOR                                 22
#define SIC_TIMER4_VECTOR                                 23
#define SIC_TIMER5_VECTOR                                 24
#define SIC_TIMER6_VECTOR                                 25
#define SIC_TIMER7_VECTOR                                 26
#define SIC_PORTF_IRQ_A_VECTOR                            27
#define SIC_PORTG_IRQ_A_VECTOR                            27
#define SIC_PORTG_IRQ_B_VECTOR                            28
#define SIC_MDMA0_VECTOR                                  29
#define SIC_MDMA1_VECTOR                                  30
#define SIC_WATCHDOG_VECTOR                               31
#define SIC_PORTF_IRQ_B_VECTOR                            31


#endif /* _bf537_h_ */

