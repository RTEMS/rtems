/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * RTEMS generic MPC5200 BSP
 *
 * This file has to be included by application when using mscan.
 */

/*
 * Copyright (c) 2005 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __MSCAN_H__
#define __MSCAN_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MSCAN_A_DEV_NAME         "/dev/mscana"
#define MSCAN_B_DEV_NAME         "/dev/mscanb"
#define MSCAN_0_DEV_NAME         "/dev/mscan0"
#define MSCAN_1_DEV_NAME         "/dev/mscan1"
#define MSCAN_A                  0
#define MSCAN_B                  1

#define MSCAN_MAX_DATA_BYTES     8

#define MSCAN_MESS_ID_RTR        (1 << 15)
#define MSCAN_MESS_ID_RTR_MASK   (1 << 15)
#define MSCAN_MESS_ID_ID_MASK    ((1 << 11)-1)
#define MSCAN_MESS_ID_HAS_RTR(id) (((id)&MSCAN_MESS_ID_RTR_MASK)==MSCAN_MESS_ID_RTR)

#define MSCAN_SET_RX_ID          1
#define MSCAN_GET_RX_ID          2
#define MSCAN_SET_RX_ID_MASK     3
#define MSCAN_GET_RX_ID_MASK     4
#define MSCAN_SET_TX_ID          5
#define MSCAN_GET_TX_ID          6
#define TOUCAN_MSCAN_INIT        7
#define MSCAN_SET_BAUDRATE       8
#define SET_TX_BUF_NO            9

struct can_message
  {
    /* uint16_t mess_len; */
  uint16_t mess_id;
  uint16_t mess_time_stamp;
  uint8_t  mess_data[MSCAN_MAX_DATA_BYTES];
  uint8_t  mess_len;
  uint8_t  mess_rtr;
  uint32_t toucan_tx_idx;
  };

struct mscan_rx_parms
  {
  struct can_message *rx_mess;
  uint32_t rx_timeout;
  uint8_t  rx_flags;
  };

struct mscan_tx_parms
  {
  struct can_message *tx_mess;
  uint32_t tx_idx;
  };

struct mscan_ctrl_parms
  {
    uint32_t ctrl_id;
    uint32_t ctrl_id_mask;
    uint8_t  ctrl_reg_no;
    uint8_t  ctrl_tx_buf_no;
    uint32_t ctrl_can_bitrate;
    void (*toucan_cb_fnc)(int16_t);
  };


rtems_device_driver mscan_initialize( rtems_device_major_number,
				                      rtems_device_minor_number,
				                      void *
				                    );

rtems_device_driver mscan_open( rtems_device_major_number,
				                rtems_device_minor_number,
				                void *
				              );

rtems_device_driver mscan_close( rtems_device_major_number,
				                 rtems_device_minor_number,
                				 void *
                			   );

rtems_device_driver mscan_read( rtems_device_major_number,
				                rtems_device_minor_number,
				                void *
				              );

rtems_device_driver mscan_write( rtems_device_major_number,
				                 rtems_device_minor_number,
				                 void *
				               );

rtems_device_driver mscan_control( rtems_device_major_number,
				                   rtems_device_minor_number,
				                   void *
				                 );


#define MSCAN_DRIVER_TABLE_ENTRY \
  { mscan_initialize, mscan_open, mscan_close, \
    mscan_read, mscan_write, mscan_control }

#ifdef __cplusplus
}
#endif

#endif /* __MSCAN_H__ */
