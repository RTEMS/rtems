/*
 * Copyright (c) 2011 Sebastian Huber.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_LM3S69XX_UART_H
#define LIBBSP_ARM_LM3S69XX_UART_H

#include <libchip/serial.h>
#include <bsp/utility.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
#define UARTDR_OE BSP_BIT32(11)
#define UARTDR_BE BSP_BIT32(10)
#define UARTDR_PE BSP_BIT32(9)
#define UARTDR_FE BSP_BIT32(8)
#define UARTDR_DATA(val) BSP_FLD32(val, 0, 7)
  uint32_t dr;

  uint32_t rsr_ecr;
  uint32_t reserved [4];

#define UARTFR_TXFE BSP_BIT32(7)
#define UARTFR_RXFF BSP_BIT32(6)
#define UARTFR_TXFF BSP_BIT32(5)
#define UARTFR_RXFE BSP_BIT32(4)
#define UARTFR_BUSY BSP_BIT32(3)
  uint32_t fr;
  uint32_t ilpr;
  uint32_t ibrd;
  uint32_t fbrd;

#define UARTLCRH_SPS BSP_BIT32(7)
#define UARTLCRH_WLEN(val) BSP_FLD32(val, 5, 6)
#define UARTLCRH_FEN BSP_BIT32(4)
#define UARTLCRH_STP2 BSP_BIT32(3)
#define UARTLCRH_EPS BSP_BIT32(2)
#define UARTLCRH_PEN BSP_BIT32(1)
#define UARTLCRH_BRK BSP_BIT32(0)
  uint32_t lcrh;

#define UARTCTL_RXE BSP_BIT32(9)
#define UARTCTL_TXE BSP_BIT32(8)
#define UARTCTL_LBE BSP_BIT32(7)
#define UARTCTL_SIRLP BSP_BIT32(2)
#define UARTCTL_SIREN BSP_BIT32(1)
#define UARTCTL_UARTEN BSP_BIT32(0)
  uint32_t ctl;

#define UARTIFLS_RXIFLSEL(val) BSP_FLD32(val, 3, 5)
#define UARTIFLS_TXIFLSEL(val) BSP_FLD32(val, 0, 2)
  uint32_t ifls;

#define UARTI_OE BSP_BIT32(10)
#define UARTI_BE BSP_BIT32(9)
#define UARTI_PE BSP_BIT32(8)
#define UARTI_FE BSP_BIT32(7)
#define UARTI_RT BSP_BIT32(6)
#define UARTI_TX BSP_BIT32(5)
#define UARTI_RX BSP_BIT32(4)
  uint32_t im;
  uint32_t ris;
  uint32_t mis;
  uint32_t icr;
} lm3s69xx_uart;

extern console_fns lm3s69xx_uart_fns;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LM3S69XX_UART_H */
