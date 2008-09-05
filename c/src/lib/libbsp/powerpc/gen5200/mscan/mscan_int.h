/*===============================================================*\
| Project: RTEMS generic MPC5200 BSP                              |
+-----------------------------------------------------------------+
| Partially based on the code references which are named below.   |
| Adaptions, modifications, enhancements and any recent parts of  |
| the code are:                                                   |
|                    Copyright (c) 2005                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file has to be included by the m driver                |
\*===============================================================*/
#ifndef __MSCAN_INT_H__
#define __MSCAN_INT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <bsp/mscan-base.h>

#include "../mscan/mscan.h"

#define MSCAN_RX_BUFF_NUM        4
#define MSCAN_TX_BUFF_NUM        3

#define MSCAN_NON_INITIALIZED_MODE   0
#define MSCAN_INITIALIZED_MODE       1
#define MSCAN_INIT_NORMAL_MODE       2
#define MSCAN_NORMAL_MODE            4
#define MSCAN_SLEEP_MODE             8

#define MSCAN_RX_BUFF_NOACTIVE   (0 << 4)
#define MSCAN_RX_BUFF_EMPTY      (1 << 6)
#define MSCAN_RX_BUFF_FULL       (1 << 5)
#define MSCAN_RX_BUFF_OVERRUN    ((MSCAN_RX_BUFF_EMPTY) | (MSCAN_RX_BUFF_FULL))
#define MSCAN_RX_BUFF_BUSY       (1 << 4)

#define MSCAN_MBUFF_MASK         0x07

#define MSCAN_TX_BUFF0           (1 << 0)
#define MSCAN_TX_BUFF1           (1 << 1)
#define MSCAN_TX_BUFF2           (1 << 2)

#define MSCAN_IDE                (1 << 0)
#define MSCAN_RTR                (1 << 1)
#define MSCAN_READ_RXBUFF_0      (1 << 2)
#define MSCAN_READ_RXBUFF_1      (1 << 2)
#define MSCAN_READ_RXBUFF_2      (1 << 2)
#define MSCAN_READ_RXBUFF_3      (1 << 2)

#define MSCAN_STATE_OK           0
#define MSCAN_STATE_ERR          1
#define MSCAN_STATE_WRN		     2
#define MSCAN_STATE_BUSOFF       3

#define TX_MBUF_SEL(buf_no)      (1 << (buf_no))
#define TX_DATA_LEN(len)         ((len) & 0x0F)

#define TX_MBUF_EMPTY(val)       (1 << (val))

#define ID_RTR                   (1 << 4)

#define SET_IDR0(u16)            ((uint8_t)((u16) >> 3))
#define SET_IDR1(u16)            (MSCAN_MESS_ID_HAS_RTR(u16) ? ((uint8_t)(((u16) & 0x0007) << 5))|((uint8_t)(ID_RTR)) : ((uint8_t)(((u16) & 0x0007) << 5)))

#define SET_IDR2(u16)            SET_IDR0(u16)
#define SET_IDR3(u16)            SET_IDR1(u16)

#define SET_IDR4(u16)            SET_IDR0(u16)
#define SET_IDR5(u16)            SET_IDR1(u16)

#define SET_IDR6(u16)            SET_IDR0(u16)
#define SET_IDR7(u16)            SET_IDR1(u16)

#define GET_IDR0(u16)            ((uint16_t) ((u16) << 3))
#define GET_IDR1(u16)            ((((u16)&(ID_RTR))==(ID_RTR)) ? (uint16_t) ((((u16) >> 5)&0x0007)|MSCAN_MESS_ID_RTR) : (uint16_t)(((u16) >> 5)&0x0007))

#define GET_IDR2(u16)            GET_IDR0(u16)
#define GET_IDR3(u16)            GET_IDR1(u16)

#define GET_IDR4(u16)            GET_IDR0(u16)
#define GET_IDR5(u16)            GET_IDR1(u16)

#define GET_IDR6(u16)            GET_IDR0(u16)
#define GET_IDR7(u16)            GET_IDR1(u16)

#define SET_IDMR0(u16)           ((uint8_t)((u16) >> 3))
#define SET_IDMR1(u16)           (MSCAN_MESS_ID_HAS_RTR(u16) ? ((uint8_t) (((((u16) & 0x0007) << 5)|((uint8_t)(ID_RTR)))|0x0007)) : ((uint8_t)((((u16) & 0x0007) << 5))|0x0007))

#define SET_IDMR2(u16)           SET_IDMR0(u16)
#define SET_IDMR3(u16)           SET_IDMR1(u16)

#define SET_IDMR4(u16)           SET_IDMR0(u16)
#define SET_IDMR5(u16)           SET_IDMR1(u16)

#define SET_IDMR6(u16)           SET_IDMR0(u16)
#define SET_IDMR7(u16)           SET_IDMR1(u16)

#define GET_IDMR0(u16)           ((uint16_t)((u16) << 3))
#define GET_IDMR1(u16)           ((((u16)&(ID_RTR))==(ID_RTR)) ? (uint16_t) ((((u16) >> 5)&0x0007)|MSCAN_MESS_ID_RTR) : (uint16_t)(((u16) >> 5)&0x0007))

#define GET_IDMR2(u16)           GET_IDMR0(u16)
#define GET_IDMR3(u16)           GET_IDMR1(u16)

#define GET_IDMR4(u16)           GET_IDMR0(u16)
#define GET_IDMR5(u16)           GET_IDMR1(u16)

#define GET_IDMR6(u16)           GET_IDMR0(u16)
#define GET_IDMR7(u16)           GET_IDMR1(u16)

#define NO_OF_MSCAN_RX_BUFF 	 20
#define MSCAN_MESSAGE_SIZE(size) (((size)%CPU_ALIGNMENT) ? (((size) + CPU_ALIGNMENT)-((size) + CPU_ALIGNMENT)%CPU_ALIGNMENT) : (size))

#define TX_BUFFER_0              0
#define TX_BUFFER_1              1
#define TX_BUFFER_2              2

#define RX_BUFFER_0              0
#define RX_BUFFER_1              1
#define RX_BUFFER_2              2
#define RX_BUFFER_3              3

#define NO_OF_MSCAN_TX_BUFF      20
#define RING_BUFFER_EMPTY(rbuff) ((((rbuff)->head) == ((rbuff)->tail)) ? TRUE : FALSE)
#define RING_BUFFER_FULL(rbuff)  ((((rbuff)->head) == ((rbuff)->tail)) ? TRUE : FALSE)


typedef struct _mscan_handle
  {
  uint8_t mscan_channel;
  void (*toucan_callback)(int16_t);
  } mscan_handle;

struct ring_buf
    {
    struct can_message * volatile buf_ptr;
    struct can_message * volatile head_ptr;
    struct can_message * volatile tail_ptr;
    };

struct mpc5200_rx_cntrl
  {
  struct can_message can_rx_message[MSCAN_RX_BUFF_NUM];
  };

struct mscan_channel_info
  {
  mscan *regs;
  uint32_t   int_rx_err;
  rtems_id   rx_qid;
  uint32_t   rx_qname;
  rtems_id   tx_rb_sid;
  uint32_t   tx_rb_sname;
  uint8_t    id_extended;
  uint8_t    mode;
  uint8_t    tx_buf_no;
  struct ring_buf tx_ring_buf;
  };

extern void CanInterrupt_A(int16_t);
extern void CanInterrupt_B(int16_t);

/*MSCAN driver internal functions */
void mscan_hardware_initialize(rtems_device_major_number, uint32_t, void *);
void mpc5200_mscan_wait_sync(mscan *);
void mpc5200_mscan_perform_init_mode_settings(mscan *);
void mpc5200_mscan_perform_normal_mode_settings(mscan *);
rtems_status_code mpc5200_mscan_set_mode(rtems_device_minor_number, uint8_t);
rtems_status_code mscan_channel_initialize(rtems_device_major_number, rtems_device_minor_number);

#ifdef __cplusplus
}
#endif

#endif /* __MSCAN_H__ */
