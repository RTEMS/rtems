/* info.h - Defines board info block structure and macros for
 *          handling elements of struct for ethernet comm board
 *
 * Written by Jay Monkman 7/21/98
 * Copyright Frasca International, Inc 1998
 */

#ifndef __info_h__
#define __info_h__

typedef struct BoardInfoBlock_ {
  rtems_unsigned16       size;         /* size of info block in bytes */
  rtems_unsigned8        eth_id[6];    /* ethernet id of ethernet interface */
  rtems_unsigned32       cpu_spd;      /* cpu speed in Hz */
  rtems_unsigned32       flash_size;   /* size of flash memory in bytes */
  rtems_unsigned32       ram_size;     /* size of ram in bytes */
  rtems_unsigned32       version;      /* version of firmare (x.y format) */
  rtems_unsigned32       if429;        /* mask for arinc429 interface */
  rtems_unsigned32       ifcsdb;       /* mask for csdb interface */
  rtems_unsigned16       if232;        /* mask for rs232 interface */
  rtems_unsigned8        ifcan;        /* mask for canbus interface */
  rtems_unsigned8        if568;        /* mask for arinc568 interface */
  rtems_unsigned8        fpn[16];      /* Frasca part number in ASCII */
  rtems_unsigned16       rev;          /* Board revision */
  rtems_unsigned32       ip_num;       /* Board IP number */
  
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
