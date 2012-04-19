/*  Gaisler wrapper to OpenCores CAN, driver interface
 *
 *  COPYRIGHT (c) 2007.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Author: Daniel Hellström, Gaisler Research AB, www.gaisler.com
 */


#ifndef __OCCAN_H__
#define __OCCAN_H__

#include <ambapp.h>

#ifdef __cplusplus
extern "C" {
#endif

/* CAN MESSAGE */
typedef struct {
	char extended; /* 1= Extended Frame (29-bit id), 0= STD Frame (11-bit id) */
	char rtr; /* RTR - Remote Transmission Request */
	char sshot; /* single shot */
	unsigned char len;
	unsigned char data[8];
	unsigned int id;
} CANMsg;

typedef struct {
	/* tx/rx stats */
	unsigned int rx_msgs;
	unsigned int tx_msgs;

	/* Error Interrupt counters */
	unsigned int err_warn;
	unsigned int err_dovr;
	unsigned int err_errp;
	unsigned int err_arb;
	unsigned int err_bus;

	/**** BUS ERRORS (err_arb) ****/

	/* ALC 4-0 */
	unsigned int err_arb_bitnum[32]; /* At what bit arbitration is lost */

	/******************************/

	/**** BUS ERRORS (err_bus) ****/

	/* ECC 7-6 */
	unsigned int err_bus_bit; /* Bit error */
	unsigned int err_bus_form; /* Form Error */
	unsigned int err_bus_stuff; /* Stuff Error */
	unsigned int err_bus_other; /* Other Error */

	/* ECC 5 */
	unsigned int err_bus_rx; /* Errors during Reception */
	unsigned int err_bus_tx; /* Errors during Transmission */

	/* ECC 4:0 */
	unsigned int err_bus_segs[32]; /* Segment (Where in frame error occured)
	                                * See OCCAN_SEG_* defines for indexes
	                                */

	/******************************/


	/* total number of interrupts */
	unsigned int ints;

	/* software monitoring hw errors */
	unsigned int tx_buf_error;

  /* Software fifo overrun */
  unsigned int rx_sw_dovr;

} occan_stats;

/* indexes into occan_stats.err_bus_segs[index] */
#define OCCAN_SEG_ID28 0x02 /* ID field bit 28:21 */
#define OCCAN_SEG_ID20 0x06 /* ID field bit 20:18 */
#define OCCAN_SEG_ID17 0x07 /* ID field bit 17:13 */
#define OCCAN_SEG_ID12 0x0f /* ID field bit 12:5 */
#define OCCAN_SEG_ID4 0x0e  /* ID field bit 4:0 */

#define OCCAN_SEG_START 0x03 /* Start of Frame */
#define OCCAN_SEG_SRTR 0x04  /* Bit SRTR */
#define OCCAN_SEG_IDE 0x05   /* Bit IDE */
#define OCCAN_SEG_RTR 0x0c   /* Bit RTR */
#define OCCAN_SEG_RSV0 0x09  /* Reserved bit 0 */
#define OCCAN_SEG_RSV1 0x0d  /* Reserved bit 1 */

#define OCCAN_SEG_DLEN 0x0b    /* Data Length code */
#define OCCAN_SEG_DFIELD 0x0a  /* Data Field */

#define OCCAN_SEG_CRC_SEQ 0x08    /* CRC Sequence */
#define OCCAN_SEG_CRC_DELIM 0x18  /* CRC Delimiter */

#define OCCAN_SEG_ACK_SLOT 0x19   /* Acknowledge slot */
#define OCCAN_SEG_ACK_DELIM 0x1b  /* Acknowledge delimiter */
#define OCCAN_SEG_EOF 0x1a        /* End Of Frame */
#define OCCAN_SEG_INTERMISSION 0x12 /* Intermission */
#define OCCAN_SEG_ACT_ERR 0x11    /* Active error flag */
#define OCCAN_SEG_PASS_ERR 0x16   /* Passive error flag */
#define OCCAN_SEG_DOMINANT 0x13   /* Tolerate dominant bits */
#define OCCAN_SEG_EDELIM 0x17     /* Error delimiter */
#define OCCAN_SEG_OVERLOAD 0x1c   /* overload flag */


#define CANMSG_OPT_RTR 0x40 			/* RTR Frame */
#define CANMSG_OPT_EXTENDED 0x80  /* Exteneded frame */
#define CANMSG_OPT_SSHOT 0x01     /* Single Shot, no retry */

#define OCCAN_IOC_START 1
#define OCCAN_IOC_STOP  2

#define OCCAN_IOC_GET_CONF 3
#define OCCAN_IOC_GET_STATS 4
#define OCCAN_IOC_GET_STATUS 5

#define OCCAN_IOC_SET_SPEED 6
#define OCCAN_IOC_SPEED_AUTO 7
#define OCCAN_IOC_SET_LINK 8
#define OCCAN_IOC_SET_FILTER 9
#define OCCAN_IOC_SET_BLK_MODE 10
#define OCCAN_IOC_SET_BUFLEN 11
#define OCCAN_IOC_SET_BTRS 12


struct occan_afilter {
	unsigned char code[4];
	unsigned char mask[4];
	int single_mode;
};

#define OCCAN_STATUS_RESET 0x01
#define OCCAN_STATUS_OVERRUN 0x02
#define OCCAN_STATUS_WARN 0x04
#define OCCAN_STATUS_ERR_PASSIVE 0x08
#define OCCAN_STATUS_ERR_BUSOFF 0x10
#define OCCAN_STATUS_QUEUE_ERROR 0x80

#define OCCAN_BLK_MODE_RX 0x1
#define OCCAN_BLK_MODE_TX 0x2

int occan_register(struct ambapp_bus *bus);


#define OCCAN_SPEED_500K 500000
#define OCCAN_SPEED_250K 250000
#define OCCAN_SPEED_125K 125000
#define OCCAN_SPEED_75K  75000
#define OCCAN_SPEED_50K  50000
#define OCCAN_SPEED_25K  25000
#define OCCAN_SPEED_10K  10000

#ifdef __cplusplus
}
#endif

#endif
