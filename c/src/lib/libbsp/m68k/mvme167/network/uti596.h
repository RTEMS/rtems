

/* uti596.h: Contains the defines and structures used by the uti596 driver */

/*
 * EII: March 11: Created v. 0.0
 *      Jan 12/98 Added STAT bits, s11-=s5 and max_colls.
 *
 *  $Id$
 */

#ifndef UTI596_H
#define UTI596_H
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>

#include <net/if.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>

/* Ethernet statistics */

struct enet_statistics{
  int   rx_packets;                                                     /* total packets received */
  int   tx_packets;                                                     /* total packets transmitted */
  int   rx_errors;                                                      /* bad packets received */
  int   tx_errors;                                                      /* packet transmit problems     */
  int   rx_dropped;                                                     /* no space in buffers */
  int   tx_dropped;
  int   tx_retries_exceeded;  /* excessive retries */
  int   multicast;                                                      /* multicast packets received   */
  int   collisions;

  /* detailed rx_errors: */
  int   rx_length_errors;
  int   rx_over_errors;                                 /* receiver ring buff overflow  */
  int   rx_crc_errors;                                  /* recved pkt with crc error    */
  int   rx_frame_errors;                                /* recv'd frame alignment error */
  int   rx_fifo_errors;                                 /* recv'r fifo overrun          */
  int   rx_missed_errors;                               /* receiver missed packet       */

  /* detailed tx_errors */
  int   tx_aborted_errors;
  int   tx_carrier_errors;
  int   tx_fifo_errors;
  int   tx_heartbeat_errors;
  int   tx_window_errors;

  /* NIC reset errors */
  int   nic_reset_count;      /* The number of times uti596reset() has been called. */
};

#define UTI596_MUTEX   1


#define CMD_EOL                                         0x8000          /* The last command of the list, stop. */
#define CMD_SUSP                                        0x4000          /* Suspend after doing cmd.                                      */
#define CMD_INTR                                        0x2000          /* Interrupt after doing cmd.                            */

#define CMD_FLEX                                        0x0008          /* Enable flexible memory model */

#define SCB_STAT_CX             0x8000          /* Cmd completes with 'I' bit set */
#define SCB_STAT_FR             0x4000          /* Frame Received                 */
#define SCB_STAT_CNA            0x2000          /* Cmd unit Not Active            */
#define SCB_STAT_RNR            0x1000          /* Receiver Not Ready             */

#define SCB_CUS_SUSPENDED 0x0100
#define SCB_CUS_ACTIVE    0x0200


#define STAT_C                                          0x8000          /* Set to 1 after execution              */
#define STAT_B                                          0x4000          /* 1 : Cmd being executed, 0 : Cmd done. */
#define STAT_OK                                         0x2000          /* 1: Command executed ok 0 : Error      */
#define STAT_A                                  0x1000          /* command has been aborted              */

#define STAT_S11                0x0800
#define STAT_S10                0x0400
#define STAT_S9                 0x0200
#define STAT_S8                 0x0100
#define STAT_S7                 0x0080
#define STAT_S6                 0x0040
#define STAT_S5                 0x0020
#define STAT_MAX_COLLS          0x000F



#define RBD_STAT_P              0x4000          /* prefetch */
#define RBD_STAT_F              0x4000          /* used */

#define CUC_START                                       0x0100
#define CUC_RESUME                              0x0200
#define CUC_SUSPEND             0x0300
#define CUC_ABORT                                       0x0400
#define RX_START                                        0x0010
#define RX_RESUME                                       0x0020
#define RX_SUSPEND                              0x0030
#define RX_ABORT                                        0x0040

#define RU_SUSPENDED            0x0010
#define RU_NO_RESOURCES         0x0020
#define RU_READY                0x0040


#define IO_ADDR                 0x360
#define PORT_ADDR               IO_ADDR
#define CHAN_ATTN               PORT_ADDR + 4
#define NIC_ADDR                PORT_ADDR + 8

#define I596_NULL ( ( void * ) 0xffffffff)
#define UTI_596_END_OF_FRAME 0x8000
#define SIZE_MASK       0x3fff

struct i596_tbd;

enum commands {
  CmdNOp           = 0, 
  CmdSASetup       = 1, 
  CmdConfigure     = 2, 
  CmdMulticastList = 3,
  CmdTx            = 4, 
  CmdTDR           = 5, 
  CmdDump          = 6, 
  CmdDiagnose      = 7
};

/* 
 * Action commands
 *   (big endian, linear mode)
 */
typedef volatile struct i596_cmd {
  volatile unsigned short status;
  volatile unsigned short command;
  volatile struct i596_cmd *next;
} i596_cmd;

typedef volatile struct i596_nop {
  i596_cmd cmd;
} i596_nop;

typedef volatile struct i596_set_add {
  i596_cmd cmd;
  char   data[8];
} i596_set_add;

typedef volatile struct i596_configure {
  i596_cmd cmd;
  char   data[16];
} i596_configure;

typedef volatile struct i596_tx {
    i596_cmd cmd;
    volatile struct i596_tbd *pTbd;
    unsigned short count;
    unsigned short pad;
    char data[6];
    unsigned short length;
} i596_tx;

typedef volatile struct i596_tdr {
  i596_cmd cmd;
  unsigned long data;
} i596_tdr;

typedef volatile struct i596_dump {
  i596_cmd cmd;
  char   *pData;
} i596_dump;

/*
 * Transmit buffer descriptor
 */
typedef volatile struct i596_tbd {
    unsigned short size;
    unsigned short pad;
    volatile struct i596_tbd *next;
    char *data; 
} i596_tbd;

/*
 * Receive buffer descriptor
 *   (flexible memory structure)
 */
typedef volatile struct i596_rbd {
    unsigned short count;
    unsigned short offset;
    volatile struct i596_rbd *next;
    char *data; 
    unsigned short size;
    unsigned short pad;
} i596_rbd;

/*
 * Receive Frame Descriptor
 */
typedef volatile struct i596_rfd {
    volatile unsigned short stat;
    volatile unsigned short cmd;
    volatile struct i596_rfd *next;
    i596_rbd *pRbd; 
    unsigned short count;
    unsigned short size;
    char data [1532];    
} i596_rfd;

#define RX_RING_SIZE 8

/*
 * System Control Block
 */
typedef volatile struct i596_scb {
    volatile unsigned short status;
    volatile unsigned short command;
    volatile unsigned long Cmd_val;
    volatile unsigned long Rfd_val;
    volatile unsigned long crc_err;
    volatile unsigned long align_err;
    volatile unsigned long resource_err;
    volatile unsigned long over_err;
    volatile unsigned long rcvdt_err;
    volatile unsigned long short_err;
    volatile unsigned short t_off;
    volatile unsigned short t_on;
    i596_cmd *pCmd;
    i596_rfd *pRfd;
} i596_scb;

/* 
 * Intermediate System Configuration Pointer
 */
typedef volatile struct i596_iscp {
    volatile unsigned long stat;
    volatile unsigned long scb_val;
    i596_scb *scb;
} i596_iscp;

/*
 * System Configuration Pointer
 */
typedef volatile struct i596_scp {
    unsigned long sysbus;
    unsigned long pad;
    unsigned long iscp_val;
    i596_iscp *iscp;
} i596_scp;

typedef volatile struct uti596_softc {
  struct arpcom          arpcom;
  i596_scp              *pScp;
  i596_iscp              iscp;
  i596_scb               scb;
  i596_set_add           set_add;
  i596_configure         set_conf;
  i596_tdr               tdr;
  i596_nop               nop;               
  unsigned long          stat;
  i596_tx               *pTxCmd;
  i596_tbd              *pTbd;

  int                   ioAddr;

  i596_rfd     *pBeginRFA;
  i596_rfd     *pEndRFA;
  i596_rfd     *pLastUnkRFD;
  i596_rbd     *pLastUnkRBD;
  i596_rfd     *pEndSavedQueue;
  i596_cmd     *pCmdHead;
  i596_cmd     *pCmdTail;  /* unneeded, as chaining not used, but implemented */

  rtems_id              rxDaemonTid;
  rtems_id              txDaemonTid;
  rtems_id              resetDaemonTid;

  struct enet_statistics stats;
  int                  started;
  unsigned long        rxInterrupts;
  unsigned long        txInterrupts;
  volatile int         cmdOk;
  int                  resetDone;
  unsigned long        txRawWait;
  i596_rfd            *pInboundFrameQueue;
  short int            rxBdCount;
  short int            txBdCount;
  short int            countRFD;
  short int            savedCount;
  i596_rfd            *pSavedRfdQueue;
  rtems_name           semaphore_name;
  rtems_id             semaphore_id;
  char                 zeroes[64];
  unsigned long        rawsndcnt;
  int                  nic_reset; /* flag is for requesting that ISR issue a reset quest */
} uti596_softc_;

#endif /* UTI596_H */

