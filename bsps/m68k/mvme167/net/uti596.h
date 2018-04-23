/* uti596.h: Contains the defines and structures used by the uti596 driver */

/*
 * EII: March 11: Created v. 0.0
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
  int	rx_packets;							/* total packets received */
  int	tx_packets;							/* total packets transmitted */
  int	rx_errors;							/* bad packets received	*/
  int	tx_errors;							/* packet transmit problems	*/
  int	rx_dropped;							/* no space in buffers */
  int	tx_dropped;
  int   tx_retries_exceeded;  /* excessive retries */
  int	multicast;							/* multicast packets received	*/
  int	collisions;

  /* detailed rx_errors: */
  int	rx_length_errors;
  int	rx_over_errors;					/* receiver ring buff overflow	*/
  int	rx_crc_errors;					/* recved pkt with crc error	*/
  int	rx_frame_errors;				/* recv'd frame alignment error */
  int	rx_fifo_errors;					/* recv'r fifo overrun		*/
  int	rx_missed_errors;				/* receiver missed packet	*/

  /* detailed tx_errors */
  int	tx_aborted_errors;
  int	tx_carrier_errors;
  int	tx_fifo_errors;
  int	tx_heartbeat_errors;
  int	tx_window_errors;

  /* NIC reset errors */
  int   nic_reset_count;      /* The number of times uti596reset() has been called. */
};

#define CMD_EOL						0x8000		/* The last command of the list, stop. */
#define CMD_SUSP					0x4000		/* Suspend after doing cmd. 					 */
#define CMD_INTR					0x2000		/* Interrupt after doing cmd. 				 */

#define CMD_FLEX					0x0008		/* Enable flexible memory model   */

#define SCB_STAT_CX    		0x8000  	/* Cmd completes with 'I' bit set */
#define SCB_STAT_FR    		0x4000  	/* Frame Received                 */
#define SCB_STAT_CNA   		0x2000  	/* Cmd unit Not Active            */
#define SCB_STAT_RNR   		0x1000  	/* Receiver Not Ready             */

#define SCB_CUS_SUSPENDED 0x0100
#define SCB_CUS_ACTIVE    0x0200

#define STAT_C						0x8000		/* Set to 1 after execution              */
#define STAT_B						0x4000		/* 1 : Cmd being executed, 0 : Cmd done. */
#define STAT_OK						0x2000		/* 1: Command executed ok 0 : Error      */
#define STAT_A    				0x1000  	/* command has been aborted              */

#define STAT_S11       		0x0800
#define STAT_S10        	0x0400
#define STAT_S9         	0x0200
#define STAT_S8         	0x0100
#define STAT_S7         	0x0080
#define STAT_S6         	0x0040
#define STAT_S5         	0x0020
#define STAT_MAX_COLLS  	0x000F

#define RBD_STAT_P      	0x4000  	/* prefetch */
#define RBD_STAT_F      	0x4000  	/* used */

#define	CUC_START					0x0100
#define	CUC_RESUME				0x0200
#define	CUC_SUSPEND    		0x0300
#define	CUC_ABORT					0x0400
#define	RX_START					0x0010
#define	RX_RESUME					0x0020
#define	RX_SUSPEND				0x0030
#define	RX_ABORT					0x0040

#define RU_SUSPENDED    	0x0010
#define RU_NO_RESOURCES 	0x0020
#define RU_READY        	0x0040

#define I596_NULL ( ( void * ) 0xffffffff)
#define UTI_596_END_OF_FRAME 0x8000

struct i596_tbd;  /* necessary forward declaration */

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
 * 82596 Dump Command Result
 */
typedef volatile struct i596_dump_result {
  unsigned char bf;
  unsigned char config_bytes[11];
  unsigned char reserved1[2];
  unsigned char ia_bytes[6];
  unsigned short last_tx_status;
  unsigned short tx_crc_byte01;
  unsigned short tx_crc_byte23;
  unsigned short rx_crc_byte01;
  unsigned short rx_crc_byte23;
  unsigned short rx_temp_mem01;
  unsigned short rx_temp_mem23;
  unsigned short rx_temp_mem45;
  unsigned short last_rx_status;
  unsigned short hash_reg01;
  unsigned short hash_reg23;
  unsigned short hash_reg45;
  unsigned short hash_reg67;
  unsigned short slot_time_counter;
  unsigned short wait_time_counter;
  unsigned short rx_frame_length;
  unsigned long reserved2;
  unsigned long cb_in3;
  unsigned long cb_in2;
  unsigned long cb_in1;
  unsigned long la_cb_addr;
  unsigned long rdb_pointer;
  unsigned long int_memory;
  unsigned long rfd_size;
  unsigned long tbd_pointer;
  unsigned long base_addr;
  unsigned long ru_temp_reg;
  unsigned long tcb_count;
  unsigned long next_rb_size;
  unsigned long next_rb_addr;
  unsigned long curr_rb_size;
  unsigned long la_rbd_addr;
  unsigned long next_rbd_addr;
  unsigned long curr_rbd_addr;
  unsigned long curr_rb_count;
  unsigned long next_fd_addr;
  unsigned long curr_fd_add;
  unsigned long temp_cu_reg;
  unsigned long next_tb_count;
  unsigned long buffer_addr;
  unsigned long la_tbd_addr;
  unsigned long next_tbd_addr;
  unsigned long cb_command;
  unsigned long next_cb_addr;
  unsigned long curr_cb_addr;
  unsigned long scb_cmd_word;
  unsigned long scb_pointer;
  unsigned long cb_stat_word;
  unsigned long mm_lfsr;
  unsigned char micro_machine_bit_array[28];
  unsigned char cu_port[16];
  unsigned long mm_alu;
  unsigned long reserved3;
  unsigned long mm_temp_a_rr;
  unsigned long mm_temp_a;
  unsigned long tx_dma_b_cnt;
  unsigned long mm_input_port_addr_reg;
  unsigned long tx_dma_addr;
  unsigned long mm_port_reg1;
  unsigned long rx_dma_b_cnt;
  unsigned long mm_port_reg2;
  unsigned long rx_dma_addr;
  unsigned long reserved4;
  unsigned long bus_t_timers;
  unsigned long diu_cntrl_reg;
  unsigned long reserved5;
  unsigned long sysbus;
  unsigned long biu_cntrl_reg;
  unsigned long mm_disp_reg;
  unsigned long mm_status_reg;
  unsigned short dump_status;
} i596_dump_result;

typedef volatile struct i596_selftest {
  unsigned long rom_signature;
  unsigned long results;
} i596_selftest;

/*
 * Action commands
 *   (big endian, linear mode)
 */
typedef volatile struct i596_cmd {
  unsigned short status;
  unsigned short command;
  volatile struct i596_cmd *next;
} i596_cmd;

typedef volatile struct i596_nop {
  i596_cmd cmd;
} i596_nop;

typedef volatile struct i596_set_add {
  i596_cmd cmd;
  char data[8];
} i596_set_add;

typedef volatile struct i596_configure {
  i596_cmd cmd;
  char data[16];
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
  char *pData;
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
	  unsigned short stat;
	  unsigned short cmd;
	  volatile struct i596_rfd *next;
	  i596_rbd *pRbd;
	  unsigned short count;
	  unsigned short size;
	  char data [1532];
} i596_rfd;

/*
 * System Control Block
 */
typedef volatile struct i596_scb {
	  unsigned short status;
	  unsigned short command;
	  unsigned long cmd_pointer;
	  unsigned long rfd_pointer;
	  unsigned long crc_err;
	  unsigned long align_err;
	  unsigned long resource_err;
	  unsigned long over_err;
	  unsigned long rcvdt_err;
	  unsigned long short_err;
	  unsigned short t_off;
	  unsigned short t_on;
	  i596_cmd *pCmd;
	  i596_rfd *pRfd;
} i596_scb;

/*
 * Intermediate System Configuration Pointer
 */
typedef volatile struct i596_iscp {
    uint8_t   null1;            			/* Always zero */
    uint8_t   busy;										/* Busy byte */
    unsigned short scb_offset;  			/* Not used in linear mode */
    unsigned long scb_pointer;      	/* Swapped pointer to scb */
    i596_scb *scb;										/* Real pointer to scb */
} i596_iscp;

/*
 * System Configuration Pointer
 */
typedef volatile struct i596_scp {
    unsigned long sysbus;							/* Only low 8 bits are used */
    unsigned long pad;								/* Must be zero */
    unsigned long iscp_pointer;       /* Swapped pointer to iscp */
    i596_iscp *iscp;									/* Real pointer to iscp */
} i596_scp;

/*
 * Device Dependent Data Structure
 */
typedef volatile struct uti596_softc {
  struct arpcom arpcom;
  i596_scp *pScp;											/* Block aligned on 16 byte boundary */
  i596_scp *base_scp;                 /* Unaligned block. Need for free() */
  i596_iscp iscp;
  i596_scb scb;
  i596_set_add set_add;
  i596_configure set_conf;
  i596_tdr tdr;
  i596_nop nop;
  i596_tx  *pTxCmd;
  i596_tbd *pTbd;

  i596_rfd *pBeginRFA;
  i596_rfd *pEndRFA;
  i596_rfd *pLastUnkRFD;
  i596_rbd *pLastUnkRBD;
  i596_rfd *pEndSavedQueue;
  i596_cmd *pCmdHead;
  i596_cmd *pCmdTail;  				/* unneeded, as chaining not used, but implemented */

  rtems_id rxDaemonTid;
  rtems_id txDaemonTid;
  rtems_id resetDaemonTid;

  struct enet_statistics stats;
  int started;
  unsigned long rxInterrupts;
  unsigned long txInterrupts;
  volatile int cmdOk;
  unsigned short * pCurrent_command_status;
  int resetDone;
  unsigned long txRawWait;
  i596_rfd *pInboundFrameQueue;
  short int rxBdCount;
  short int txBdCount;
  short int countRFD;
  short int savedCount;
  i596_rfd *pSavedRfdQueue;
  rtems_name semaphore_name;
  rtems_id semaphore_id;
  char zeroes[64];
  unsigned long rawsndcnt;
  int nic_reset;  /* flag for requesting that ISR issue a reset quest */
} uti596_softc_;

#endif /* UTI596_H */
