/*  canbus.h
 *
 *  This include file contains all canbus IO definitions
 *
 *  Written by Jay Monkman (jmonkman@frasca.com)
 *
 *  COPYRIGHT (c) 1998
 *  Frasca International, Inc.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: 
 */

#ifndef __CANBUS_H_
#define __CANBUS_H_

#include <rtems.h>

typedef struct i82527_msg_t_ {
  rtems_unsigned8  ctrl0 __attribute__ ((packed));    /* Control 0 register */
  rtems_unsigned8  ctrl1 __attribute__ ((packed));    /* Control 1 register */
  rtems_unsigned32 arb __attribute__ ((packed));      /* Arbitration reg */
  rtems_unsigned8  cfg __attribute__ ((packed));      /* Message config reg */
  rtems_unsigned8  data[8] __attribute__ ((packed));  /* Actual message */
} i82527_msg_t;

typedef struct i82527_t_ {
  rtems_unsigned8  ctrl __attribute__ ((packed));    /* Control register */
  rtems_unsigned8  status __attribute__ ((packed));  /* Status register */
  rtems_unsigned8  cir __attribute__ ((packed));     /* CPU interface reg */
  rtems_unsigned8  _res0 __attribute__ ((packed));
  rtems_unsigned16 hsr __attribute__ ((packed));     /* High speed read */
  rtems_unsigned16 gms __attribute__ ((packed));     /* Global Mask - std */
  rtems_unsigned32 gml __attribute__ ((packed));     /* Global Mask - long */
  rtems_unsigned32 mlm __attribute__ ((packed));     /* Mask last message  */
  i82527_msg_t     msg1 __attribute__ ((packed));    /* Message 1 */
  rtems_unsigned8  clkout __attribute__ ((packed));  /* CLKOUT register */
  i82527_msg_t     msg2 __attribute__ ((packed));    /* Message 2 */
  rtems_unsigned8  bcr __attribute__ ((packed));     /* Bus config register */
  i82527_msg_t     msg3 __attribute__ ((packed));    /* Message 3 */
  rtems_unsigned8  btr0 __attribute__ ((packed));    /* Bit timing reg 0 */
  i82527_msg_t     msg4 __attribute__ ((packed));    /* Message 4 */
  rtems_unsigned8  btr1 __attribute__ ((packed));    /* Bit timing reg 1 */
  i82527_msg_t     msg5 __attribute__ ((packed));    /* Message 5 */
  rtems_unsigned8  intr __attribute__ ((packed));    /* Interrupt register */
  i82527_msg_t     msg6 __attribute__ ((packed));    /* Message 6 */
  rtems_unsigned8  _res1 __attribute__ ((packed));
  i82527_msg_t     msg7 __attribute__ ((packed));    /* Message 7 */
  rtems_unsigned8  _res2 __attribute__ ((packed));
  i82527_msg_t     msg8 __attribute__ ((packed));    /* Message 8 */
  rtems_unsigned8  _res3 __attribute__ ((packed));
  i82527_msg_t     msg9 __attribute__ ((packed));    /* Message 9 */
  rtems_unsigned8  p1conf __attribute__ ((packed));  /* Port 1 config */
  i82527_msg_t     msg10 __attribute__ ((packed));   /* Message 10 */
  rtems_unsigned8  p2conf __attribute__ ((packed));  /* Port 2 config */
  i82527_msg_t     msg11 __attribute__ ((packed));   /* Message 11 */
  rtems_unsigned8  p1in __attribute__ ((packed));    /* Port 1 in */
  i82527_msg_t     msg12 __attribute__ ((packed));   /* Message 12 */
  rtems_unsigned8  p2in __attribute__ ((packed));    /* Port 2 in */
  i82527_msg_t     msg13 __attribute__ ((packed));   /* Message 13 */
  rtems_unsigned8  p1out __attribute__ ((packed));   /* Port 1 out */
  i82527_msg_t     msg14 __attribute__ ((packed));   /* Message 14 */
  rtems_unsigned8  p2out __attribute__ ((packed));   /* Port 2 out */
  i82527_msg_t     msg15 __attribute__ ((packed));   /* Message 15 */
  rtems_unsigned8  sra __attribute__ ((packed));     /* Serial reset address */
} i82527_t;

#define I82527_CTRL_CCE         (1<<6)
#define I82527_CTRL_EIE         (1<<3)
#define I82527_CTRL_SIE         (1<<2)
#define I82527_CTRL_IE          (1<<1)
#define I82527_CTRL_INIT        (1)
#define I82527_STATUS_BOFF      (1<<7)
#define I82527_STATUS_WARN      (1<<6)
#define I82527_STATUS_WAKE      (1<<5)
#define I82527_STATUS_RXOK      (1<<4)
#define I82527_STATUS_TXOK      (1<<3)
#define I82527_STATUS_LEC       (7)
#define I82527_STATUS_LEC_NONE  0
#define I82527_STATUS_LEC_STUFF 1
#define I82527_STATUS_LEC_FORM  2
#define I82527_STATUS_LEC_ACK   3
#define I82527_STATUS_LEC_BIT1  4
#define I82527_STATUS_LEC_BIT0  5
#define I82527_STATUS_LEC_CRC   6
#define I82527_CIR_RSTSTAT      (1<<7)
#define I82527_CIR_DSC          (1<<6)
#define I82527_CIR_DMC          (1<<5)
#define I82527_CIR_PWD          (1<<4)
#define I82527_CIR_SLEEP        (1<<3)
#define I82527_CIR_MUX          (1<<2)
#define I82527_CIR_CEN          (1)
#define I82527_CLKOUT_SL1       (1<<5)
#define I82527_CLKOUT_SLO       (1<<4)
#define I82527_BCR_COBY         (1<<6)
#define I82527_BCR_POL          (1<<5)
#define I82527_DCT1             (1<<3)
#define I82527_DCR1             (1<<1)
#define I82527_DCR0             (1)
#define I82527_BTR1_SPL         (1<<7)
#define I82527_MSG_CTRL_MSGVAL     (2<<6)
#define I82527_MSG_CTRL_MSGVAL_NC  (3<<6) 
#define I82527_MSG_CTRL_MSGVAL_SET (2<<6) 
#define I82527_MSG_CTRL_MSGVAL_CLR (1<<6)
#define I82527_MSG_CTRL_TXIE       (2<<4)
#define I82527_MSG_CTRL_TXIE_NC    (3<<4)
#define I82527_MSG_CTRL_TXIE_SET   (2<<4)
#define I82527_MSG_CTRL_TXIE_CLR   (1<<4)
#define I82527_MSG_CTRL_RXIE       (2<<2)
#define I82527_MSG_CTRL_RXIE_NC    (3<<2)
#define I82527_MSG_CTRL_RXIE_SET   (2<<2)
#define I82527_MSG_CTRL_RXIE_CLR   (1<<2)
#define I82527_MSG_CTRL_INTPND     (2)
#define I82527_MSG_CTRL_INTPND_NC  (3)
#define I82527_MSG_CTRL_INTPND_SET (2)
#define I82527_MSG_CTRL_INTPND_CLR (1)
#define I82527_MSG_CTRL_RMTPND     (2<<6)
#define I82527_MSG_CTRL_RMTPND_NC  (3<<6)
#define I82527_MSG_CTRL_RMTPND_SET (2<<6)
#define I82527_MSG_CTRL_RMTPND_CLR (1<<6)
#define I82527_MSG_CTRL_TXRQ       (2<<4)
#define I82527_MSG_CTRL_TXRQ_NC    (3<<4)
#define I82527_MSG_CTRL_TXRQ_SET   (2<<4)
#define I82527_MSG_CTRL_TXRQ_CLR   (1<<4)
#define I82527_MSG_CTRL_MSGLST     (2<<2)
#define I82527_MSG_CTRL_MSGLST_NC  (3<<2)
#define I82527_MSG_CTRL_MSGLST_SET (2<<2)
#define I82527_MSG_CTRL_MSGLST_CLR (1<<2)
#define I82527_MSG_CTRL_CPUUPD     (2<<2)
#define I82527_MSG_CTRL_CPUUPD_NC  (3<<2)
#define I82527_MSG_CTRL_CPUUPD_SET (2<<2)
#define I82527_MSG_CTRL_CPUUPD_CLR (1<<2)
#define I82527_MSG_CTRL_NEWDAT     (2)
#define I82527_MSG_CTRL_NEWDAT_NC  (3)
#define I82527_MSG_CTRL_NEWDAT_SET (2)
#define I82527_MSG_CTRL_NEWDAT_CLR (1)
#define I82527_MSG_CFG_DIR          (1<<3)
#define I82527_MSG_CFG_XTD          (1<<2)

extern i82527_t canbus0;
extern i82527_t canbus1;
extern i82527_t canbus2;


rtems_device_driver canbus_initialize(rtems_device_major_number, 
				      rtems_device_minor_number, 
				      void *);
rtems_device_driver canbus_open(rtems_device_major_number, 
				rtems_device_minor_number, 
				void *);
rtems_device_driver canbus_close(rtems_device_major_number, 
				 rtems_device_minor_number, 
				 void *);
rtems_device_driver canbus_read(rtems_device_major_number, 
				rtems_device_minor_number, 
				void *);
rtems_device_driver canbus_write(rtems_device_major_number, 
				 rtems_device_minor_number, 
				 void *);
rtems_device_driver canbus_control(rtems_device_major_number, 
				   rtems_device_minor_number, 
				   void *);


#define CANBUS_DRIVER_TABLE_ENTRY \
  { canbus_initialize, canbus_open, canbus_close, \
    canbus_read, canbus_write, canbus_control }

#endif /* __CANBUS_H_ */
