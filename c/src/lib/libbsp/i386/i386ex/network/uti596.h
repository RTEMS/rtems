
/* uti596.h: Contains the defines and structures used by the uti596 driver */

/*
 * EII: March 11: Created v. 0.0
 *      Jan 12/98 Added STAT bits, s11-=s5 and max_colls.
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
  int	rx_packets;			/* total packets received	*/
  int	tx_packets;			/* total packets transmitted	*/
  int	rx_errors;			/* bad packets received		*/
  int	tx_errors;			/* packet transmit problems	*/
  int	rx_dropped;			/* no space in buffers          */
  int	tx_dropped;			/*                              */
  int   tx_retries_exceeded;              /* excessive retries            */
  int	multicast;			/* multicast packets received	*/
  int	collisions;

  /* detailed rx_errors: */
  int	rx_length_errors;
  int	rx_over_errors;			/* receiver ring buff overflow	*/
  int	rx_crc_errors;			/* recved pkt with crc error	*/
  int	rx_frame_errors;		/* recv'd frame alignment error */
  int	rx_fifo_errors;			/* recv'r fifo overrun		*/
  int	rx_missed_errors;		/* receiver missed packet	*/

  /* detailed tx_errors */
  int	tx_aborted_errors;
  int	tx_carrier_errors;
  int	tx_fifo_errors;
  int	tx_heartbeat_errors;
  int	tx_window_errors;
};



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


#define UTI596_MUTEX   1


#define CMD_EOL		0x8000	/* The last command of the list, stop. */
#define CMD_SUSP	0x4000	/* Suspend after doing cmd. */
#define CMD_INTR	0x2000	/* Interrupt after doing cmd. */

#define CMD_FLEX	0x0008	/* Enable flexible memory model */

#define SCB_STAT_CX     0x8000  /* Cmd completes with 'I' bit set */
#define SCB_STAT_FR     0x4000  /* Frame Received                 */
#define SCB_STAT_CNA    0x2000  /* Cmd unit Not Active            */
#define SCB_STAT_RNR    0x1000  /* Receiver Not Ready             */

#define STAT_C		0x8000	/* Set to 1 after execution              */
#define STAT_B		0x4000	/* 1 : Cmd being executed, 0 : Cmd done. */
#define STAT_OK		0x2000	/* 1: Command executed ok 0 : Error      */
#define STAT_A          0x1000  /* command has been aborted              */

#define STAT_S11        0x0800
#define STAT_S10        0x0400
#define STAT_S9         0x0200
#define STAT_S8         0x0100
#define STAT_S7         0x0080
#define STAT_S6         0x0040
#define STAT_S5         0x0020
#define STAT_MAX_COLLS  0x000F


#define RBD_STAT_P      0x4000  /* prefetch */
#define RBD_STAT_F      0x4000  /* used */

#define	 CUC_START	0x0100
#define	 CUC_RESUME	0x0200
#define	 CUC_SUSPEND    0x0300
#define	 CUC_ABORT	0x0400
#define	 RX_START	0x0010
#define	 RX_RESUME	0x0020
#define	 RX_SUSPEND	0x0030
#define	 RX_ABORT	0x0040

#define  RU_SUSPENDED    0x001
#define  RU_NO_RESOURCES 0x0020
#define  RU_READY        0x0040

        

#define IO_ADDR         0x360
#define PORT_ADDR       IO_ADDR
#define CHAN_ATTN       PORT_ADDR + 4
#define NIC_ADDR        PORT_ADDR + 8

struct i596_cmd {
    volatile unsigned short status;
    volatile unsigned short command;
    struct i596_cmd *next;
};

#define I596_NULL ( ( void * ) 0xffffffff)
#define UTI_596_END_OF_FRAME		0x8000
#define SIZE_MASK	0x3fff

/*
 * Transmit buffer Descriptor
 */

struct i596_tbd {
    unsigned short size;
    unsigned short pad;
    struct i596_tbd *next;
    char *data; 
};

/*
 * Receive buffer Descriptor
 */

struct i596_rbd {
    unsigned short count;
    unsigned short offset;
    struct i596_rbd *next;
    char           *data; 
    unsigned short size;
    unsigned short pad;
};

/*
 * Transmit Command Structure
 */
struct tx_cmd {
    struct i596_cmd cmd;
    struct i596_tbd *pTbd;
    unsigned short size;
    unsigned short pad;
} ;


/*
 * Receive Frame Descriptor
 */
struct i596_rfd {
    volatile unsigned short stat;
    volatile unsigned short cmd;
    struct i596_rfd *next;
    struct i596_rbd *pRbd; 
    unsigned short count;
    unsigned short size;
    char data [1532 ];    
} ;


struct i596_dump {
  struct i596_cmd cmd;
  char * pData;
};

struct i596_set_add {
  struct i596_cmd cmd;
  char   data[8];
};

struct i596_configure {
  struct i596_cmd cmd;
  char   data[16];
};



#define RX_RING_SIZE 8

/*
 * System Control Block
 */
struct i596_scb {
    volatile unsigned short status;
    volatile unsigned short command;
    struct i596_cmd *pCmd;
    struct i596_rfd *pRfd;
    volatile unsigned long crc_err;
    volatile unsigned long align_err;
    volatile unsigned long resource_err;
    volatile unsigned long over_err;
    volatile unsigned long rcvdt_err;
    volatile unsigned long short_err;
    volatile unsigned short t_on;
    volatile unsigned short t_off;
};


/* 
 * Intermediate System Control Block
 */
struct i596_iscp {
    volatile unsigned long stat;
    struct i596_scb *scb;
} ;
/*
 * System Control Parameters
 */
struct i596_scp {
    unsigned long sysbus;
    unsigned long pad;
    struct i596_iscp *iscp;
} ;

struct uti596_softc {
  struct arpcom                 arpcom;
  rtems_irq_connect_data	irqInfo;
  struct i596_scp              *pScp;
  struct i596_iscp              iscp;
  struct i596_scb               scb;
  struct i596_set_add           set_add;
  struct i596_configure         set_conf;
  struct i596_cmd               tdr;
  unsigned long                 stat;
  struct tx_cmd                *pTxCmd;
  struct i596_tbd              *pTbd;

  int                   ioAddr;

  struct i596_rfd     *pBeginRFA;
  struct i596_rfd     *pEndRFA;
  struct i596_rfd     *pLastUnkRFD;
  struct i596_rbd     *pLastUnkRBD;
  struct i596_rfd     *pEndSavedQueue;
  struct i596_cmd     *pCmdHead;
  struct i596_cmd     *pCmdTail;  /* unneeded, as chaining not used, but implemented */

  rtems_id		rxDaemonTid;
  rtems_id		txDaemonTid;

  struct enet_statistics stats;
  int                  started;
  unsigned long        rxInterrupts;
  unsigned long        txInterrupts;
  volatile int         cmdOk;
  int                  resetDone;
  unsigned long	       txRawWait;
  struct i596_rfd     *pInboundFrameQueue;
  short int            rxBdCount;
  short int            txBdCount;
  short int            countRFD;
  short int            savedCount;
  struct i596_rfd     *pSavedRfdQueue;
  rtems_name           semaphore_name;
  rtems_id             semaphore_id;
  char                 zeroes[64];
  unsigned long        rawsndcnt;
} ;
#endif
