/**
 *@file bf52x.h
 *
 *@brief
 *  - This file provides the register address for the 52X model. The file is
 *  based on the 533 implementation with some addition to support 52X range of
 *  processors.
 *
 * Target:   TLL6527v1-0
 * Compiler:
 *
 * COPYRIGHT (c) 2010 by ECE Northeastern University.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license
 *
 * @author Rohan Kangralkar, ECE, Northeastern University
 *         (kangralkar.r@husky.neu.edu)
 *
 * LastChange:
 */

#ifndef _BF52X_H_
#define _BF52X_H_

/* register (or register block) addresses */

#define SIC_BASE_ADDRESS                          0xffc00100
#define WDOG_BASE_ADDRESS                         0xffc00200
#define RTC_BASE_ADDRESS                          0xffc00300
#define UART0_BASE_ADDRESS                        0xffc00400
#define UART1_BASE_ADDRESS                        0xffc02000
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
#define DMA8_BASE_ADDRESS                         0xffc00e00
#define DMA9_BASE_ADDRESS                         0xffc00e40
#define DMA10_BASE_ADDRESS                        0xffc00e80
#define DMA11_BASE_ADDRESS                        0xffc00ec0
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

#define SIC_IAR_COUNT                                      8

#endif /* _BF52X_H_ */

