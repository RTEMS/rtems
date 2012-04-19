/*
 *  Driver interface for APBUART
 *
 *  COPYRIGHT (c) 2007.
 *  Gaisler Research
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#ifndef __APBUART_H__
#define __APBUART_H__

#include <ambapp.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  unsigned int hw_dovr;
  unsigned int hw_parity;
  unsigned int hw_frame;
  unsigned int sw_dovr;
  unsigned int rx_cnt;
  unsigned int tx_cnt;
} apbuart_stats;

#define APBUART_START    0
#define APBUART_STOP     1
#define APBUART_SET_RXFIFO_LEN 2
#define APBUART_SET_TXFIFO_LEN 3
#define APBUART_SET_BAUDRATE   4
#define APBUART_SET_SCALER     5
#define APBUART_SET_BLOCKING   6
#define APBUART_SET_ASCII_MODE 7


#define APBUART_GET_STATS 16
#define APBUART_CLR_STATS 17

#define APBUART_BLK_RX 0x1
#define APBUART_BLK_TX 0x2
#define APBUART_BLK_FLUSH 0x4


#define APBUART_CTRL_RE 0x1
#define APBUART_CTRL_TE 0x2
#define APBUART_CTRL_RI 0x4
#define APBUART_CTRL_TI 0x8
#define APBUART_CTRL_PS 0x10
#define APBUART_CTRL_PE 0x20
#define APBUART_CTRL_FL 0x40
#define APBUART_CTRL_LB 0x80
#define APBUART_CTRL_EC 0x100
#define APBUART_CTRL_TF 0x200
#define APBUART_CTRL_RF 0x400

#define APBUART_STATUS_DR 0x1
#define APBUART_STATUS_TS 0x2
#define APBUART_STATUS_TE 0x4
#define APBUART_STATUS_BR 0x8
#define APBUART_STATUS_OV 0x10
#define APBUART_STATUS_PE 0x20
#define APBUART_STATUS_FE 0x40
#define APBUART_STATUS_TH 0x80
#define APBUART_STATUS_RH 0x100
#define APBUART_STATUS_TF 0x200
#define APBUART_STATUS_RF 0x400

/* Register APBUART driver
 * bus =  pointer to AMBA bus description used to search for APBUART(s).
 *        (&ambapp_plb for LEON3), (LEON2: see amba_scan)
 */
int apbuart_register (struct ambapp_bus *bus);

#ifdef __cplusplus
}
#endif

#endif /* __APBUART_H__ */
