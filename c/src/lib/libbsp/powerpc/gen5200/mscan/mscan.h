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
| this file has to be included by application when using mscan    |
\*===============================================================*/
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
