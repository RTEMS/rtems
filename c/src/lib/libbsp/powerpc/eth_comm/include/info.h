/* info.h - Defines board info block structure and macros for
 *          handling elements of struct for ethernet comm board
 *
 * Written by Jay Monkman 7/21/98
 * Copyright Frasca International, Inc 1998
 *
 *  $Id$
 */

#ifndef __info_h__
#define __info_h__

typedef struct BoardInfoBlock_ {
  uint16_t         size;         /* size of info block in bytes */
  uint8_t          eth_id[6];    /* ethernet id of ethernet interface */
  uint32_t         cpu_spd;      /* cpu speed in Hz */
  uint32_t         flash_size;   /* size of flash memory in bytes */
  uint32_t         ram_size;     /* size of ram in bytes */
  uint32_t         version;      /* version of firmare (x.y format) */
  uint32_t         if429;        /* mask for arinc429 interface */
  uint32_t         ifcsdb;       /* mask for csdb interface */
  uint16_t         if232;        /* mask for rs232 interface */
  uint8_t          ifcan;        /* mask for canbus interface */
  uint8_t          if568;        /* mask for arinc568 interface */
  uint8_t          fpn[16];      /* Frasca part number in ASCII */
  uint16_t         rev;          /* Board revision */
  uint32_t         ip_num;       /* Board IP number */

} boardinfo_t;

#define IFACE_ARINC429_TX0 0x00000001;
#define IFACE_ARINC429_RX0 0x00000002;
#define IFACE_ARINC429_TX1 0x00000004;
#define IFACE_ARINC429_RX1 0x00000008;
#define IFACE_ARINC429_TX2 0x00000010;
#define IFACE_ARINC429_RX2 0x00000020;
#define IFACE_ARINC429_TX3 0x00000040;
#define IFACE_ARINC429_RX3 0x00000080;
#define IFACE_ARINC429_TX4 0x00000100;
#define IFACE_ARINC429_RX4 0x00000200;
#define IFACE_ARINC429_TX5 0x00000400;
#define IFACE_ARINC429_RX5 0x00000800;
#define IFACE_ARINC429_TX6 0x00001000;
#define IFACE_ARINC429_RX6 0x00002000;
#define IFACE_ARINC429_TX7 0x00004000;
#define IFACE_ARINC429_RX7 0x00008000;

#define IFACE_ARINC568_TX0 0x0001;
#define IFACE_ARINC568_RX0 0x0002;
#define IFACE_ARINC568_TX1 0x0004;
#define IFACE_ARINC568_RX1 0x0008;

#define IFACE_CSDB_TX0 0x00000001;
#define IFACE_CSDB_RX0 0x00000002;
#define IFACE_CSDB_TX1 0x00000004;
#define IFACE_CSDB_RX1 0x00000008;
#define IFACE_CSDB_TX2 0x00000010;
#define IFACE_CSDB_RX2 0x00000020;
#define IFACE_CSDB_TX3 0x00000040;
#define IFACE_CSDB_RX3 0x00000080;
#define IFACE_CSDB_TX4 0x00000100;
#define IFACE_CSDB_RX4 0x00000200;
#define IFACE_CSDB_TX5 0x00000400;
#define IFACE_CSDB_RX5 0x00000800;
#define IFACE_CSDB_TX6 0x00001000;
#define IFACE_CSDB_RX6 0x00002000;
#define IFACE_CSDB_TX7 0x00004000;
#define IFACE_CSDB_RX7 0x00008000;
#define IFACE_CSDB_TX8 0x00010000;
#define IFACE_CSDB_RX8 0x00020000;

#define IFACE_CAN_TX0 0x0001;
#define IFACE_CAN_RX0 0x0002;
#define IFACE_CAN_TX1 0x0004;
#define IFACE_CAN_RX1 0x0008;
#define IFACE_CAN_TX2 0x0010;
#define IFACE_CAN_RX2 0x0020;

#define IFACE_RS232_TX0 0x0001;
#define IFACE_RS232_RX0 0x0002;
#define IFACE_RS232_TX1 0x0004;
#define IFACE_RS232_RX1 0x0008;
#define IFACE_RS232_TX2 0x0010;
#define IFACE_RS232_RX2 0x0020;
#define IFACE_RS232_TX3 0x0040;
#define IFACE_RS232_RX3 0x0080;
#define IFACE_RS232_TX4 0x0100;
#define IFACE_RS232_RX4 0x0200;



#endif /* __info_h__*/
