/*  Blackfin General Purpose Timer Registers
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _timerRegs_h_
#define _timerRegs_h_


/* register addresses */

#define TIMER_CONFIG_OFFSET                             0x00
#define TIMER_WIDTH_OFFSET                              0x04
#define TIMER_PERIOD_OFFSET                             0x08
#define TIMER_COUNTER_OFFSET                            0x0c


/* register fields */

#define TIMER_CONFIG_ERR_TYP_MASK                     0xc000
#define TIMER_CONFIG_ERR_TYP_NONE                     0x0000
#define TIMER_CONFIG_ERR_TYP_OVERFLOW                 0x4000
#define TIMER_CONFIG_ERR_TYP_PERIOD                   0x8000
#define TIMER_CONFIG_ERR_TYP_WIDTH                    0xc000
#define TIMER_CONFIG_EMU_RUN                          0x0200
#define TIMER_CONFIG_TOGGLE_HI                        0x0100
#define TIMER_CONFIG_CLK_SEL                          0x0080
#define TIMER_CONFIG_OUT_DIS                          0x0040
#define TIMER_CONFIG_TIN_SEL                          0x0020
#define TIMER_CONFIG_IRQ_ENA                          0x0010
#define TIMER_CONFIG_PERIOD_CNT                       0x0008
#define TIMER_CONFIG_PULSE_HI                         0x0004
#define TIMER_CONFIG_TMODE_MASK                       0x0003
#define TIMER_CONFIG_TMODE_RESET                      0x0000
#define TIMER_CONFIG_TMODE_PWM_OUT                    0x0001
#define TIMER_CONFIG_TMODE_WDTH_CAP                   0x0002
#define TIMER_CONFIG_TMODE_EXT_CLK                    0x0003


#endif /* _timerRegs_h_ */

