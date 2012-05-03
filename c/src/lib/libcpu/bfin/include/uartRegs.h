/*  Blackfin UART Registers
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _uartRegs_h_
#define _uartRegs_h_

/* register addresses */

#define UART_RBR_OFFSET                               0x0000
#define UART_THR_OFFSET                               0x0000
#define UART_DLL_OFFSET                               0x0000
#define UART_IER_OFFSET                               0x0004
#define UART_DLH_OFFSET                               0x0004
#define UART_IIR_OFFSET                               0x0008
#define UART_LCR_OFFSET                               0x000c
#define UART_MCR_OFFSET                               0x0010
#define UART_LSR_OFFSET                               0x0014
#define UART_SCR_OFFSET                               0x001c
#define UART_GCTL_OFFSET                              0x0024


/* register fields */

#define UART_LCR_DLAB                                   0x80
#define UART_LCR_SB                                     0x40
#define UART_LCR_STP                                    0x20
#define UART_LCR_EPS                                    0x10
#define UART_LCR_PEN                                    0x08
#define UART_LCR_STB                                    0x04
#define UART_LCR_WLS_MASK                               0x03
#define UART_LCR_WLS_5                                  0x00
#define UART_LCR_WLS_6                                  0x01
#define UART_LCR_WLS_7                                  0x02
#define UART_LCR_WLS_8                                  0x03

#define UART_MCR_LOOP                                   0x10

#define UART_LSR_TEMT                                   0x40
#define UART_LSR_THRE                                   0x20
#define UART_LSR_BI                                     0x10
#define UART_LSR_FE                                     0x08
#define UART_LSR_PE                                     0x04
#define UART_LSR_OE                                     0x02
#define UART_LSR_DR                                     0x01

#define UART_IER_ELSI                                   0x04
#define UART_IER_ETBEI                                  0x02
#define UART_IER_ERBFI                                  0x01

#define UART_IIR_STATUS_MASK                            0x06
#define UART_IIR_STATUS_THRE                            0x02
#define UART_IIR_STATUS_RDR                             0x04
#define UART_IIR_STATUS_LS                              0x06
#define UART_IIR_NINT                                   0x01

#define UART_GCTL_FFE                                   0x20
#define UART_GCTL_FPE                                   0x10
#define UART_GCTL_RPOLC                                 0x08
#define UART_GCTL_TPOLC                                 0x04
#define UART_GCTL_IREN                                  0x02
#define UART_GCTL_UCEN                                  0x01

#endif /* _uartRegs_h_ */

