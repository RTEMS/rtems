/*  network.c: An 82596 ethernet driver for rtems-bsd.
 */

#define KERNEL

/*
 *  Selectively define to debug the network driver. If you define any of these
 *  you must run with polled console I/O.
 */

/*
#define DBG_ADD_CMD
#define DBG_WAIT
#define DBG_SEND
#define DBG_MEM
#define DBG_SELFTEST_CMD
#define DBG_DUMP_CMD
#define DBG_RESET
#define DBG_ATTACH
#define DBG_START
#define DBG_INIT
#define DBG_STOP
#define DBG_RX
#define DBG_ISR
#define DBG_IOCTL
#define DBG_STAT
#define DBG_PACKETS
*/

#define IGNORE_SPURIOUS_IRQ
#define IGNORE_NO_RFA
#define IGNORE_MULTIPLE_RF

/*
 * Default number of buffer descriptors and buffer sizes.
 */
#define RX_BUF_COUNT   15
#define TX_BUF_COUNT   4
#define TX_BD_PER_BUF  4

#define RBUF_SIZE  1520

#define UTI_596_ETH_MIN_SIZE  60

#define INET_ADDR_MAX_BUF_SIZE (sizeof "255.255.255.255")

/*
 * RTEMS events
 */
#define INTERRUPT_EVENT         RTEMS_EVENT_1
#define START_TRANSMIT_EVENT    RTEMS_EVENT_2
#define NIC_RESET_EVENT         RTEMS_EVENT_3

#include <bsp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>
#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/types.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>

#include "uti596.h"

/* If we are running interrupt driven I/O no debug output is printed */
#if CD2401_IO_MODE == 0
   #define printk(arglist) do { printk arglist; printk("\r"); } while (0);
#else
   #define printk(arglist)
#endif

#define UTI_596_ASSERT( condition, str )  if (!( condition ) ) { printk((str)) }

/* Types of PORT commands */
#define UTI596_RESET_PORT_FUNCTION	  	0
#define UTI596_SELFTEST_PORT_FUNCTION 	1
#define UTI596_SCP_PORT_FUNCTION      	2
#define UTI596_DUMP_PORT_FUNCTION     	3

/* Types of waiting for commands */
#define UTI596_NO_WAIT									0
#define UTI596_WAIT_FOR_CU_ACCEPT				1
#define UTI596_WAIT_FOR_INITIALIZATION	2
#define UTI596_WAIT_FOR_STAT_C					3

/* Device dependent data structure */
static uti596_softc_ uti596_softc;

/* Globals */
int count_rx = 0;
static int scbStatus;
static rtems_status_code sc;
static i596_cmd *pIsrCmd;
static i596_rfd *pIsrRfd;

/*
 * Initial 596 configuration
 */
char uti596initSetup[] = {
  0x0E,   /* Byte 0: length, prefetch off ( no RBD's ) */
  0xC8,   /* Byte 1: fifo to 8, monitor off */
  0x40,   /* Byte 2: don't save bad frames ( was save= 80, use intel's 40 )*/
  0x2E,   /* Byte 3: No source address insertion, 8 byte preamble */
  0x00,   /* Byte 4: priority and backoff defaults */
  0x60,   /* Byte 5: interframe spacing */
  0x00,   /* Byte 6: slot time LSB */
  0xf2,   /* Byte 7: slot time and retries */
  0x0C,   /* Byte 8: not promisc, enable bcast, tx no crs, crc inserted 32bit, 802.3 framing */
  0x08,   /* Byte 9: collision detect */
  0x40,   /* Byte 10: minimum frame length */
  0xfb,   /* Byte 11: tried C8 same as byte 1 in bits 6-7, else ignored*/
  0x00,   /* Byte 12: disable full duplex */
  0x3f    /* Byte 13: no multi IA, backoff enabled */
};

/* Local Routines */

static unsigned long word_swap ( unsigned long );
static void * malloc_16byte_aligned ( void **, void ** adjusted_pointer, size_t );
RTEMS_INLINE_ROUTINE void uti596_writePortFunction ( volatile void *, unsigned long );
/* currently unused by RTEMS */
#if 0
RTEMS_INLINE_ROUTINE void uti596_portReset( void );
static unsigned long uti596_portSelfTest( i596_selftest * );
static int uti596_portDump ( i596_dump_result * );
static void uti596_CU_dump ( i596_dump_result * );
#endif
static int uti596_wait ( uti596_softc_ *, uint8_t);
static int uti596_issueCA ( uti596_softc_ *, uint8_t);
static void uti596_addCmd ( i596_cmd * );
static void uti596_addPolledCmd ( i596_cmd * );
static void uti596_dump_scb ( void );
static int uti596_setScpAndScb ( uti596_softc_ * );
static int uti596_diagnose ( void );
static int uti596_configure ( uti596_softc_ * );
static int uti596_IAsetup ( uti596_softc_ * );
static int uti596_initTBD ( uti596_softc_ * );
static int uti596_initRFA ( int );
static void uti596_initMem ( uti596_softc_ * );
static void uti596_initialize ( uti596_softc_ * );
static void uti596_initialize_hardware ( uti596_softc_ * );
static void uti596_reset_hardware ( uti596_softc_ *);
static void uti596_reset ( void );
static void uti596_clearListStatus ( i596_rfd * );
static i596_rfd * uti596_dequeue ( i596_rfd ** );
static void uti596_append ( i596_rfd ** , i596_rfd * );
static void uti596_supplyFD ( i596_rfd * );
static void send_packet ( struct ifnet *, struct mbuf * );

/* Required RTEMS network driver functions and tasks (plus reset daemon) */

static void uti596_start ( struct ifnet * );
void uti596_init ( void * );
void uti596_stop ( uti596_softc_ * );
void uti596_txDaemon ( void * );
void uti596_rxDaemon ( void * );
void uti596_resetDaemon( void * );
rtems_isr uti596_DynamicInterruptHandler ( rtems_vector_number );
static int uti596_ioctl ( struct ifnet *, u_long, caddr_t );
void uti596_stats ( uti596_softc_ * );

#ifdef DBG_PACKETS
static void dumpQ( void );
static void show_buffers( void );
static void show_queues( void );
static void print_eth  ( unsigned char * );
static void print_hdr  ( unsigned char * );
static void print_pkt  ( unsigned char * );
static void print_echo ( unsigned char * );
#endif

/*
 *  word_swap
 *
 *  Return a 32 bit value, swapping the upper and lower words first.
 *
 *  Input parameters:
 *    val - 32 bit value to swap
 *
 *  Output parameters: NONE
 *
 *  Return value:
 *    Input value with upper and lower 16-bit words swapped
 */
static unsigned long word_swap(
  unsigned long val
)
{
  return (((val >> 16)&(0x0000ffff)) | ((val << 16)&(0xffff0000)));
}

/*
 *  malloc_16byte_aligned
 *
 *  Allocate a block of a least nbytes aligned on a 16-byte boundary.
 *  Clients are responsible to store both the real address and the adjusted
 *  address. The real address must be used to free the block.
 *
 *  Input parameters:
 *    real_pointer - pointer to a void * pointer in which to store the starting
 *                   address of the block. Required for free.
 *    adjusted_pointer - pointer to a void * pointer in which to store the
 *                       starting address of the block rounded up to the next
 *                       16 byte boundary.
 *    nbytes - number of bytes of storage requested
 *
 *  Output parameters:
 *    real_pointer - starting address of the block.
 *    adjusted_pointer - starting address of the block rounded up to the next
 *                       16 byte boundary.
 *
 *  Return value:
 *    starting address of the block rounded up to the next 16 byte boundary.
 *    NULL if no storage was allocated.
 */
static void * malloc_16byte_aligned(
  void ** real_pointer,
  void ** adjusted_pointer,
  size_t nbytes
)
{
  *real_pointer = malloc( nbytes + 0xF, 0, M_NOWAIT );
  *adjusted_pointer = (void *)(((unsigned long)*real_pointer + 0xF ) & 0xFFFFFFF0 );
  return *adjusted_pointer;
}

/*
 *  uti596_scp_alloc
 *
 *  Allocate a new scp, possibly freeing a previously allocated one.
 *
 *  Input parameters:
 *    sc - pointer to the global uti596_softc in which to store pointers
 *         to the newly allocated block.
 *
 *  Output parameters: NONE
 *
 *  Return value:
 *    Pointer to the newly allocated, 16-byte aligned scp.
 */
static i596_scp * uti596_scp_alloc(
  uti596_softc_ * sc
)
{
  if( sc->base_scp != NULL ) {
    #ifdef DBG_MEM
    printk(("uti596_scp_alloc: Already have an SCP at %p\n", sc->base_scp))
    #endif
    return sc->pScp;
  }

  /* allocate enough memory for the Scp block to be aligned on 16 byte boundary */
  malloc_16byte_aligned( (void *)&(sc->base_scp), (void *)&(sc->pScp), sizeof( i596_scp ) );

  #ifdef DBG_MEM
  printk(("uti596_scp_alloc: Scp base address is %p\n", sc->base_scp))
  printk(("uti596_scp_alloc: Scp aligned address is : %p\n",sc->pScp))
  #endif

  return sc->pScp;
}

/*
 *  uti596_writePortFunction
 *
 *  Write the command into the PORT.
 *
 *  Input parameters:
 *    addr - 16-byte aligned address to write into the PORT.
 *    cmd - 4-bit cmd to write into the PORT
 *
 *  Output parameters: NONE
 *
 *  Return value: NONE
 *
 *  The Motorola manual swapped the high and low registers.
 */
RTEMS_INLINE_ROUTINE void uti596_writePortFunction(
  volatile void * addr,
  unsigned long cmd
)
{
  i82596->port_lower = (unsigned short)(((unsigned long)addr & 0xFFF0) | cmd);
  i82596->port_upper = (unsigned short)(((unsigned long)addr >> 16 ) & 0xFFFF);
}

/*
 *  uti596_portReset
 *
 *  Issue a port Reset to the uti596
 *
 *  Input parameters: NONE
 *
 *  Output parameters: NONE
 *
 *  Return value: NONE
 */
RTEMS_INLINE_ROUTINE void uti596_portReset( void )
{
  uti596_writePortFunction( NULL, UTI596_RESET_PORT_FUNCTION );
}

/* currently unused by RTEMS */
#if 0
/*
 *  uti596_portSelfTest
 *
 *  Perform a self-test. Wait for up to 1 second for the test to
 *  complete. Normally, the test should complete in a very short time,
 *  so busy waiting is not  an issue.
 *
 *  Input parameters:
 *    stp - pointer to a 16-byte aligned uti596_selftest structure.
 *
 *  Output parameters: NONE
 *
 *  Return value:
 *    32-bit result field if successful, -1 otherwise.
 */
static unsigned long uti596_portSelfTest(
  i596_selftest * stp
)
{
  rtems_interval ticks_per_second, start_ticks, end_ticks;

  stp->results = 0xFFFFFFFF;
  uti596_writePortFunction( stp, UTI596_SELFTEST_PORT_FUNCTION );

  rtems_clock_get(RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticks_per_second);
	rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &start_ticks);
	end_ticks = start_ticks + ticks_per_second;

  do {
    if( stp->results != 0xFFFFFFFF )
      break;
		else
			rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &start_ticks);
	} while (start_ticks <= end_ticks);

  if (start_ticks > end_ticks ) {
    #ifdef DBG_SELFTEST_CMD
    printk(("uti596_selftest: Timed out\n" ))
		#endif
		return -1;
  }
  else {
    #ifdef DBG_SELFTEST_CMD
    printk(("uti596_selftest: Succeeded with signature = 0x%08x, result = 0x%08x\n",
      			 stp->signature,
             stp->results))
		#endif
		return stp->results;
	}
}
#endif

/* currently unused by RTEMS */
#if 0
/*
 *  uti596_portDump
 *
 *  Perform a dump Wait for up to 1 second for the test to
 *  complete. Normally, the test should complete in a very short time,
 *  so busy waiting is not an issue.
 *
 *  Input parameters:
 *    dp - pointer to a 16-byte aligned uti596_dump structure.
 *
 *  Output parameters: NONE
 *
 *  Return value:
 *    16-bit dump_status field if successful, -1 otherwise.
 */
static int uti596_portDump(
  i596_dump_result * dp
)
{
  rtems_interval ticks_per_second, start_ticks, end_ticks;

  dp->dump_status = 0;
  uti596_writePortFunction( dp, UTI596_DUMP_PORT_FUNCTION );

  rtems_clock_get(RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticks_per_second);
	rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &start_ticks);
	end_ticks = start_ticks + ticks_per_second;

  do {
    if( dp->dump_status != 0xA006 )
      break;
		else
			rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &start_ticks);
	} while (start_ticks <= end_ticks);

  if (start_ticks > end_ticks ) {
    #ifdef DBG_DUMP_CMD
    printk(("uti596_dump: Timed out with dump at 0x%08x\n", (unsigned long)dp ))
		#endif
		return -1;
  }
  else {
    #ifdef DBG_DUMP_CMD
    printk(("uti596_dump: Succeeded with dump at = 0x%08x\n", (unsigned long)dp ))
		#endif
		return dp->dump_status;
	}
}
#endif

/*
 *  uti596_wait
 *
 *  Wait for a certain condition.
 *
 *  Input parameters:
 *    sc - pointer to the uti596_softc struct
 *    wait_type - UTI596_NO_WAIT
 *    						UTI596_WAIT
 *                UTI596_WAIT_FOR_CU_ACCEPT
 *                UTI596_WAIT_FOR_INITIALIZATION
 *                UTI596_WAIT_FOR_STAT_C
 *
 *  Output parameters: NONE
 *
 *  Return value:
 *    0 if successful, -1 otherwise.
 */
static int uti596_wait(
  uti596_softc_ *sc,
  uint8_t   waitType
)
{
  rtems_interval ticks_per_second, start_ticks, end_ticks;

  rtems_clock_get(RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticks_per_second);
	rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &start_ticks);
	end_ticks = start_ticks + ticks_per_second;

  switch( waitType ) {

    case UTI596_NO_WAIT:
      return 0;

    case UTI596_WAIT_FOR_CU_ACCEPT:
		  do {
			  if (sc->scb.command == 0)
				  break;
			  else
				  rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &start_ticks);

		  } while (start_ticks <= end_ticks);

		  if( (sc->scb.command != 0) || (start_ticks > end_ticks) ) {
			  printf("i82596 timed out with status %x, cmd %x.\n",
               sc->scb.status,  sc->scb.command);
        return -1;
      }
      else
        return 0;

    case UTI596_WAIT_FOR_INITIALIZATION:
		  do {
		    if( !sc->iscp.busy )
		      break;
				else
					rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &start_ticks);
			} while (start_ticks <= end_ticks);

		  if (start_ticks > end_ticks ) {
		    #ifdef DBG_WAIT
		    printk(("uti596_setScpAndScb: Timed out\n"  ))
				#endif
				return -1;
		  }
		  else {
		    #ifdef DBG_WAIT
		    printk(("uti596_setScpAndScb: Succeeded\n" ))
				#endif
				return 0;
			}

	 case UTI596_WAIT_FOR_STAT_C:
		 do {
		   if( *sc->pCurrent_command_status & STAT_C )
		      break;
			 else
					rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &start_ticks);
			} while (start_ticks <= end_ticks);

		  if (start_ticks > end_ticks ) {
		    #ifdef DBG_WAIT
		    printk(("uti596_initMem: timed out - STAT_C not obtained\n"  ))
				#endif
				return -1;
		  }
		  else {
		    #ifdef DBG_WAIT
		    printk(("uti596_initMem: STAT_C obtained OK\n" ))
				#endif
				return 0;
			}
	 }
	 return -1;
}

/*
 *  uti596_issueCA
 *
 *  Issue a Channel Attention command. Possibly wait for the
 *  command to start or complete.
 *
 *  Input parameters:
 *    sc - pointer to the uti596_softc
 *    wait_type - UTI596_NO_WAIT
 *                UTI596_WAIT_BEGIN
 *                UTI596_WAIT_COMPLETION
 *
 *  Output parameters: NONE
 *
 *  Return value:
 *    0 if successful, -1 otherwise.
 */
static int uti596_issueCA(
  uti596_softc_ *sc,
  uint8_t   waitType
)
{
  /* Issue Channel Attention */
  i82596->chan_attn = 0x00000000;

  return (uti596_wait ( sc, waitType ));
}

/*
 *  uti596_addCmd
 *
 *  Add a uti596_cmd onto the end of the CBL command chain,
 *  	or to the start if the chain is empty.
 *
 *  Input parameters:
 *  	pCmd - a pointer to the command to be added.
 *
 *  Output parameters: NONE
 *
 *  Return value: NONE
 */
static void uti596_addCmd(
  i596_cmd *pCmd
)
{
  ISR_Level level;

	#ifdef DBG_ADD_CMD
  printk(("uti596_addCmd: Adding command 0x%x\n", pCmd -> command ))
	#endif

  /* Mark command as last in list, to return an interrupt */
  pCmd->command |= (CMD_EOL | CMD_INTR );
  pCmd->status = 0;
  pCmd->next = I596_NULL;

  _ISR_Disable(level);

  if (uti596_softc.pCmdHead == I596_NULL) {
    uti596_softc.pCmdHead = uti596_softc.pCmdTail = uti596_softc.scb.pCmd = pCmd;
    uti596_softc.scb.cmd_pointer = word_swap ((unsigned long)pCmd);

		uti596_wait ( &uti596_softc, UTI596_WAIT_FOR_CU_ACCEPT );
		uti596_softc.scb.command = CUC_START;
  	uti596_issueCA ( &uti596_softc, UTI596_NO_WAIT );

  	_ISR_Enable(level);
  }
  else {
    uti596_softc.pCmdTail->next = (i596_cmd *) word_swap ((unsigned long)pCmd);
    uti596_softc.pCmdTail = pCmd;
    _ISR_Enable(level);
	}

	#ifdef DBG_ADD_CMD
  printk(("uti596_addCmd: Scb status & command 0x%x 0x%x\n",
           uti596_softc.scb.status,
           uti596_softc.scb.command ))
	#endif
}

/*
 *  uti596_addPolledCmd
 *
 *  Add a single uti596_cmd to the end of the command block list
 *  for processing, send a CU_START and wait for its acceptance
 *
 *  Input parameters:
 *  	sc - a pointer to the uti596_softc struct
 *
 *  Output parameters: NONE
 *
 *  Return value: NONE
 */
void uti596_addPolledCmd(
  i596_cmd *pCmd
)
{

	#ifdef DBG_ADD_CMD
  printk(("uti596_addPolledCmd: Adding command 0x%x\n", pCmd -> command ))
	#endif

	pCmd->status = 0;
  pCmd->command |=  CMD_EOL ; /* only command in list*/
  pCmd->next = I596_NULL;

  uti596_wait ( &uti596_softc, UTI596_WAIT_FOR_CU_ACCEPT );

  uti596_softc.pCmdHead = uti596_softc.pCmdTail = uti596_softc.scb.pCmd = pCmd;
  uti596_softc.scb.cmd_pointer = word_swap((unsigned long)pCmd);
  uti596_softc.scb.command = CUC_START;
  uti596_issueCA ( &uti596_softc, UTI596_WAIT_FOR_CU_ACCEPT );

  uti596_softc.pCmdHead = uti596_softc.pCmdTail = uti596_softc.scb.pCmd = I596_NULL;
  uti596_softc.scb.cmd_pointer = (unsigned long) I596_NULL;

	#ifdef DBG_ADD_CMD
  printk(("uti596_addPolledCmd: Scb status & command 0x%x 0x%x\n",
           uti596_softc.scb.status,
           uti596_softc.scb.command ))
	#endif
}

/* currently unused by RTEMS */
#if 0
/*
 *  uti596_CU_dump
 *
 *  Dump the LANC 82596 registers
 *  	The outcome is the same as the portDump() but executed
 *  	via the CU instead of via a PORT access.
 *
 *  Input parameters:
 *  	drp - a pointer to a i596_dump_result structure.
 *
 *  Output parameters: NONE
 *
 *  Return value: NONE
 */
static void uti596_CU_dump ( i596_dump_result * drp)
{
  i596_dump dumpCmd;

  dumpCmd.cmd.command = CmdDump;
  dumpCmd.cmd.next    = I596_NULL;
  dumpCmd.pData       = (char *) drp;
  uti596_softc.cmdOk  = 0;
  uti596_addCmd ( (i596_cmd *) &dumpCmd );

}
#endif

/*
 *  uti596_dump_scb
 *
 *  Dump the system control block
 *  This function expands to nothing when using interrupt driven I/O
 *
 *  Input parameters: NONE
 *
 *  Output parameters: NONE
 *
 *  Return value: NONE
 */
static void uti596_dump_scb ( void )
{
  printk(("status 0x%x\n",uti596_softc.scb.status))
  printk(("command 0x%x\n",uti596_softc.scb.command))
  printk(("cmd 0x%x\n",(int)uti596_softc.scb.pCmd))
  printk(("rfd 0x%x\n",(int)uti596_softc.scb.pRfd))
  printk(("crc_err 0x%x\n",uti596_softc.scb.crc_err))
  printk(("align_err 0x%x\n",uti596_softc.scb.align_err))
  printk(("resource_err 0x%x\n",uti596_softc.scb.resource_err ))
  printk(("over_err 0x%x\n",uti596_softc.scb.over_err))
  printk(("rcvdt_err 0x%x\n",uti596_softc.scb.rcvdt_err))
  printk(("short_err 0x%x\n",uti596_softc.scb.short_err))
  printk(("t_on 0x%x\n",uti596_softc.scb.t_on))
  printk(("t_off 0x%x\n",uti596_softc.scb.t_off))
}

/*
 *  uti596_setScpAndScb
 *
 *  Issue the first channel attention after reset and wait for the busy
 *  field to clear in the iscp.
 *
 *  Input parameters:
 *    sc - pointer to the global uti596_softc
 *
 *  Output parameters: NONE
 *
 *  Return value:
 *    0 if successful, -1 otherwise.
 */
static int uti596_setScpAndScb(
  uti596_softc_ * sc
)
{
  /* set the busy flag in the iscp */
  sc->iscp.busy = 1;

  /* the command block list (CBL) is empty */
  sc->scb.command = 0;
  sc->scb.cmd_pointer = (unsigned long) I596_NULL;	/* all 1's */
  sc->pCmdHead = sc->scb.pCmd = I596_NULL;			    /* all 1's */

  uti596_writePortFunction( sc->pScp, UTI596_SCP_PORT_FUNCTION );

  /* Issue CA: pass the scb address to the 596 */
  return ( uti596_issueCA ( sc, UTI596_WAIT_FOR_INITIALIZATION ) );
}

/*
 *  uti596_diagnose
 *
 *  Send a diagnose command to the CU
 *
 *  Input parameters: NONE
 *
 *  Output parameters: NONE
 *
 *  Return value:
 *  	0 if successful, -1 otherwise
 */
static int uti596_diagnose( void )
{
  i596_cmd diagnose;

  diagnose.command = CmdDiagnose;
  diagnose.status = 0;
  uti596_softc.pCurrent_command_status = (unsigned short *)&diagnose.status;
  uti596_addPolledCmd(&diagnose);
  return (uti596_wait ( &uti596_softc, UTI596_WAIT_FOR_STAT_C ));

	#ifdef DBG_INIT
  printk(("Status diagnostic: 0xa000 is a success ... 0x%2.2x\n", diagnose.status))
	#endif
}

/*
 *  uti596_configure
 *
 *  Send the CU a configure command with the desired
 *  configuration structure
 *
 *  Input parameters:
 *  	sc - a pointer to the uti596_softc struct
 *
 *  Output parameters: NONE
 *
 *  Return value:
 *  	0 if successful, -1 otherwise
 */
static int uti596_configure (
	uti596_softc_ * sc
)
{
  sc->set_conf.cmd.command = CmdConfigure;
  memcpy ( (void *)sc->set_conf.data, uti596initSetup, 14);
  uti596_addPolledCmd( (i596_cmd *) &sc->set_conf);

  /* Poll for successful command completion */
  sc->pCurrent_command_status = (unsigned short *)&(sc->set_conf.cmd.status);
  return ( uti596_wait ( sc, UTI596_WAIT_FOR_STAT_C ) );
}

/*
 *  uti596_IAsetup
 *
 *  Send the CU an Individual Address setup command with
 *  the ethernet hardware address
 *
 *  Input parameters:
 *  	sc - a pointer to the uti596_softc struct
 *
 *  Output parameters: NONE
 *
 *  Return value:
 *  	0 if successful, -1 otherwise
 */
static int uti596_IAsetup (
	uti596_softc_ * sc
)
{
	int i;

  sc->set_add.cmd.command = CmdSASetup;
  for ( i=0; i<6; i++) {
    sc->set_add.data[i]=sc->arpcom.ac_enaddr[i];
  }
  sc->cmdOk = 0;
  uti596_addPolledCmd((i596_cmd *)&sc->set_add);

  /* Poll for successful command completion */
  sc->pCurrent_command_status = (unsigned short *)&(sc->set_add.cmd.status);
  return ( uti596_wait ( sc, UTI596_WAIT_FOR_STAT_C ) );
}

/*
 *  uti596_initTBD
 *
 *  Initialize transmit buffer descriptors
 *    dynamically allocate mem for the number of tbd's required
 *
 *  Input parameters:
 *  	sc - a pointer to the uti596_softc struct
 *
 *  Output parameters: NONE
 *
 *  Return value:
 *  	0 if successful, -1 otherwise
 */
static int uti596_initTBD ( uti596_softc_ * sc )
{
	int i;
  i596_tbd *pTbd, *pPrev;

  /* Set up a transmit command with a tbd ready */
  sc->pLastUnkRFD = I596_NULL;
  sc->pTxCmd = (i596_tx *) calloc (1,sizeof (struct i596_tx) );
  sc->pTbd = (i596_tbd *) calloc (1,sizeof (struct i596_tbd) );
  if ((sc->pTxCmd == NULL) || (sc->pTbd == NULL)) {
  	return -1;
	}
  sc->pTxCmd->pTbd = (i596_tbd *) word_swap ((unsigned long) sc->pTbd);
  sc->pTxCmd->cmd.command = CMD_FLEX|CmdTx;
  sc->pTxCmd->pad = 0;
  sc->pTxCmd->count = 0; /* all bytes are in list of TBD's */

  pPrev = pTbd = sc->pTbd;

  /* Allocate a linked list of tbd's each with it's 'next' field written
   * with upper and lower words swapped (for big endian), and mark the end.
   */
  for ( i=0; i<sc->txBdCount; i++) {
    if ( (pTbd = (i596_tbd *) calloc (1,sizeof (struct i596_tbd) )) == NULL ) {
      return -1;
    }
    pPrev->next = (i596_tbd *) word_swap ((unsigned long) pTbd);
    pPrev = pTbd;
  }
  pTbd->next = I596_NULL;
  return 0;
}

/*
 *  uti596_initRFA
 *
 *  Initialize the Receive Frame Area
 *    dynamically allocate mem for the number of rfd's required
 *
 *  Input parameters:
 *  	sc - a pointer to the uti596_softc struct
 *
 *  Output parameters: NONE
 *
 *  Return value:
 *  	# of buffer descriptors successfully allocated
 */
static int uti596_initRFA( int num )
{
  i596_rfd *pRfd;
  int i = 0;

	#ifdef DBG_INIT
  printk(("uti596_initRFA: begins\n   Requested frame descriptors ... %d.\n", num))
	#endif

  /*
   * Create the first RFD in the RFA
   */
  pRfd = (i596_rfd *) calloc (1, sizeof (struct i596_rfd));
  if ( !pRfd ) {
    printk(("Can't allocate first buffer.\n"))
    return 0;
  }
  else {
    uti596_softc.countRFD = 1;
    uti596_softc.pBeginRFA = uti596_softc.pEndRFA = pRfd;
  }

  /* Create remaining RFDs */
  for (i = 1; i < num; i++) {
    pRfd = (i596_rfd *) calloc (1, sizeof (struct i596_rfd) );
    if ( pRfd != NULL ) {
      uti596_softc.countRFD++;                          /* update count */
      uti596_softc.pEndRFA->next =
        (i596_rfd *) word_swap ((unsigned long) pRfd);  /* write the link */
      uti596_softc.pEndRFA = pRfd;   										/* move the end */
    }
    else {
      printk(("Can't allocate all buffers: only %d allocated\n", i))
      break;
    }
  } /* end for */

  uti596_softc.pEndRFA->next = I596_NULL;
  UTI_596_ASSERT(uti596_softc.countRFD == num,"INIT:WRONG RFD COUNT\n" )

  #ifdef DBG_INIT
  printk (("uti596_initRFA: Head of RFA is buffer %p \n\
            uti596_initRFA: End of RFA is buffer %p \n",
            uti596_softc.pBeginRFA, uti596_softc.pEndRFA ))
	#endif

  /* Walk and initialize the RFD's */
  for ( pRfd = uti596_softc.pBeginRFA;
        pRfd != I596_NULL;
        pRfd = (i596_rfd *) word_swap ((unsigned long)pRfd->next) )
  {
    pRfd->cmd   = 0x0000;
    pRfd->stat  = 0x0000;
    pRfd->pRbd  = I596_NULL;
    pRfd->count = 0;  		/* number of bytes in buffer: usually less than size */
    pRfd->size   = 1532;  /* was 1532;  buffer size ( All RBD ) */
  } /* end for */

  /* mark the last RFD as the last one in the RDL */
  uti596_softc.pEndRFA -> cmd = CMD_EOL;
  uti596_softc.pSavedRfdQueue =
    uti596_softc.pEndSavedQueue = I596_NULL;   /* initially empty */

  uti596_softc.savedCount = 0;
  uti596_softc.nop.cmd.command = CmdNOp; /* initialize the nop command */

  return (i); /* the number of allocated buffers */
}

/*
 *  uti596_initMem
 *
 *  Initialize the 82596 memory structures for Tx and Rx
 *    dynamically allocate mem for the number of tbd's required
 *
 *  Input parameters:
 *  	sc - a pointer to the uti596_softc struct
 *
 *  Output parameters: NONE
 *
 *  Return value: NONE
 */
void uti596_initMem(
  uti596_softc_ * sc
)
{
  int i;

  #ifdef DBG_INIT
  printk(("uti596_initMem: begins\n"))
  #endif

  sc->resetDone = 0;

  /*
   * Set up receive frame area (RFA)
   */
  i = uti596_initRFA( sc->rxBdCount );
  if ( i < sc->rxBdCount ) {
    printk(("init_rfd: only able to allocate %d receive frame descriptors\n", i))
  }

  /*
   * Write the SCB with a pointer to the receive frame area
   * and keep a pointer for our use.
   */
  sc->scb.rfd_pointer = word_swap((unsigned long)sc->pBeginRFA);
  sc->scb.pRfd =  sc->pBeginRFA;

  /*
   * Diagnose the health of the board
   */
  uti596_diagnose();

  /*
   * Configure the 82596
   */
  uti596_configure( sc );

  /*
   * Set up the Individual (hardware) Address
   */
  uti596_IAsetup ( sc );

	/*
	 * Initialize the transmit buffer descriptors
	 */
  uti596_initTBD( sc );

  /* Padding used to fill short tx frames */
  memset ( &sc->zeroes, 0, 64);

  /* now need ISR  */
  sc->resetDone = 1;
}

/*
 *  uti596_initialize
 *
 *  Reset the 82596 and initialize it with a new SCP.
 *
 *  Input parameters:
 *    sc - pointer to the uti596_softc
 *
 *  Output parameters: NONE
 *
 *  Return value: NONE
 */
void uti596_initialize(
  uti596_softc_ *sc
)
{
  /* Reset the device. Stops it from doing whatever it might be doing.  */
  uti596_portReset();

  /* Get a new System Configuration Pointer */
  uti596_scp_alloc( sc );

  /* write the SYSBUS: interrupt pin active high, LOCK disabled,
   * internal triggering, linear mode
   */
  sc->pScp->sysbus = 0x44;

  /* provide the iscp to the scp, keep a pointer for our use */
  sc->pScp->iscp_pointer = word_swap((unsigned long)&sc->iscp);
  sc->pScp->iscp = &sc->iscp;

  /* provide the scb to the iscp, keep a pointer for our use */
  sc->iscp.scb_pointer = word_swap((unsigned long)&sc->scb);
  sc->iscp.scb = &sc->scb;

  #ifdef DBG_INIT
  printk(("uti596_initialize: Starting i82596.\n"))
  #endif

  /* Set up the 82596 */
  uti596_setScpAndScb( sc );

  /* clear the scb command word */
  sc->scb.command = 0;
}

/*
 *  uti596_initialize_hardware
 *
 *  Reset the 82596 and initialize it with a new SCP. Enable bus snooping.
 *  Install the interrupt handlers.
 *
 *  Input parameters:
 *    sc - pointer to the uti596_softc
 *
 *  Output parameters: NONE
 *
 *  Return value: NONE
 */
void uti596_initialize_hardware(
  uti596_softc_ *sc
)
{
  rtems_isr_entry dummy;

  printk(("uti596_initialize_hardware: begins\n"))

  /* Get the PCCChip2 to assert bus snooping signals on behalf of the i82596 */
  pccchip2->LANC_berr_ctl	= 0x40;

  uti596_initialize( sc );

  /*
   * Configure interrupt control in PCCchip2
   */
  pccchip2->LANC_error		= 0xff;		/* clear status register */
  pccchip2->LANC_int_ctl	= 0x5d;		/* lvl 5, enabled, edge-sensitive rising */
  pccchip2->LANC_berr_ctl	= 0x5d;		/* bus error: lvl 5, enabled, snoop control
  								 									 * will supply dirty data and leave dirty data
  								 									 * on read access and sink any data on write
  								 									 */
  /*
   * Install the interrupt handler
   * calls rtems_interrupt_catch
   */
  dummy = (rtems_isr_entry) set_vector( uti596_DynamicInterruptHandler, 0x57, 1 );

  /* Initialize the 82596 memory */
  uti596_initMem(sc);

  #ifdef DBG_INIT
  printk(("uti596_initialize_hardware: After attach, status of board = 0x%x\n", sc->scb.status ))
  #endif
}

/*
 *  uti596_reset_hardware
 *
 *  Reset the 82596 and initialize it with an SCP.
 *
 *  Input parameters:
 *    sc - pointer to the uti596_softc
 *
 *  Output parameters: NONE
 *
 *  Return value: NONE
 */
void uti596_reset_hardware(
  uti596_softc_ *sc
)
{
  rtems_status_code status_code;
  i596_cmd *pCmd;

  pCmd = sc->pCmdHead;  /* This is a tx command for sure (99.99999%)  */

  /* the command block list (CBL) is empty */
  sc->scb.cmd_pointer = (unsigned long) I596_NULL;	/* all 1's */
  sc->pCmdHead = sc->scb.pCmd = I596_NULL;			    /* all 1's */

  #ifdef DBG_RESET
  printk(("uti596_reset_hardware\n"))
  #endif
  uti596_initialize( sc );

  /*
   * Wake the transmitter if needed.
   */
  if ( sc->txDaemonTid && pCmd != I596_NULL ) {
    printk(("****RESET: wakes transmitter!\n"))
    status_code = rtems_event_send (sc->txDaemonTid,
                           INTERRUPT_EVENT);

    if ( status_code != RTEMS_SUCCESSFUL ) {
      printk(("****ERROR:Could NOT send event to tid 0x%x : %s\n",
             sc->txDaemonTid, rtems_status_text (status_code) ))
    }
  }

  #ifdef DBG_RESET
  printk(("uti596_reset_hardware: After reset_hardware, status of board = 0x%x\n", sc->scb.status ))
  #endif
}

/*
 *  uti596_clearListStatus
 *
 *  Clear the stat fields for all RFDs
 *
 *  Input parameters:
 *  	pRfd - a pointer to the head of the RFA
 *
 *  Output parameters: NONE
 *
 *  Return value: NONE
 */
void uti596_clearListStatus(
  i596_rfd *pRfd
)
{
  while ( pRfd != I596_NULL ) {
    pRfd -> stat = 0;
    pRfd = (i596_rfd *) word_swap((unsigned long)pRfd-> next);
  }
}

/*
 *  uti596_reset
 *
 *  Reset the 82596 and reconfigure
 *
 *  Input parameters: NONE
 *
 *  Output parameters: NONE
 *
 *  Return value: NONE
 */
void uti596_reset( void )
{
  uti596_softc_ *sc = &uti596_softc;

	#ifdef DBG_RESET
  printk(("uti596_reset: begin\n"))
	#endif

	/* Wait for the CU to be available, then
	 * reset the ethernet hardware. Must re-config.
 	 */
  sc->resetDone = 0;
	uti596_wait ( sc, UTI596_WAIT_FOR_CU_ACCEPT );
  uti596_reset_hardware ( &uti596_softc );

	#ifdef DBG_RESET
  uti596_diagnose();
	#endif

  /*
   * Configure the 82596
   */
  uti596_configure( sc );

  /*
   * Set up the Individual (hardware) Address
   */
  uti596_IAsetup ( sc );

  sc->pCmdHead = sc->pCmdTail = sc->scb.pCmd = I596_NULL;

  /* restore the RFA */

  if ( sc->pLastUnkRFD != I596_NULL ) {
    sc-> pEndRFA =  sc->pLastUnkRFD; /* The end position can be updated */
    sc-> pLastUnkRFD = I596_NULL;
  }

  sc->pEndRFA->next = (i596_rfd*)word_swap((uint32_t)sc->pSavedRfdQueue);
  if ( sc->pSavedRfdQueue != I596_NULL ) {
    sc->pEndRFA = sc->pEndSavedQueue;
    sc->pSavedRfdQueue = sc->pEndSavedQueue = I596_NULL;
    sc -> countRFD = sc->rxBdCount ;
  }

  /* Re-address the head of the RFA in the SCB */
  sc->scb.pRfd =  sc->pBeginRFA;
  sc->scb.rfd_pointer = word_swap((unsigned long)sc->pBeginRFA);

	/* Clear the status of all RFDs */
  uti596_clearListStatus( sc->pBeginRFA );

  printk(("uti596_reset: Starting NIC\n"))

  /* Start the receiver */
  sc->scb.command = RX_START;
  sc->started = 1;               /* assume that the start is accepted */
  sc->resetDone = 1;
  uti596_issueCA ( sc, UTI596_WAIT_FOR_CU_ACCEPT );

  UTI_596_ASSERT(sc->pCmdHead == I596_NULL, "Reset: CMD not cleared\n")

	#ifdef DBG_RESET
  printk(("uti596_reset: completed\n"))
	#endif
}

/*
 *  uti596_dequeue
 *
 *  Remove an RFD from the received fram queue
 *
 *  Input parameters:
 *  	ppQ - a pointer to a i596_rfd pointer
 *
 *  Output parameters: NONE
 *
 *  Return value:
 *  	pRfd - a pointer to the dequeued RFD
 */
i596_rfd * uti596_dequeue(
  i596_rfd ** ppQ
)
{
  ISR_Level level;
  i596_rfd * pRfd;

  _ISR_Disable(level);

  /* invalid address, or empty queue or emptied queue */
  if( ppQ == NULL || *ppQ == NULL || *ppQ == I596_NULL) {
    _ISR_Enable(level);
     return I596_NULL;
  }

	/*
	 * Point to the dequeued buffer, then
	 * adjust the queue pointer and detach the buffer
	 */
  pRfd = *ppQ;
  *ppQ = (i596_rfd *) word_swap ((unsigned long) pRfd->next);
  pRfd->next = I596_NULL;  /* unlink the rfd being returned */

  _ISR_Enable(level);
  return pRfd;
}

/*
 *  uti596_append
 *
 *  Remove an RFD buffer from the RFA and tack it on to
 *  	the received frame queue for processing.
 *
 *  Input parameters:
 *  	 ppQ - a pointer to the queue pointer
 *  	pRfd - a pointer to the buffer to be returned
 *
 *  Output parameters: NONE
 *
 *  Return value: NONE
 */

void uti596_append(
  i596_rfd ** ppQ,
  i596_rfd * pRfd
)
{

  i596_rfd *p;

  if ( pRfd != NULL && pRfd != I596_NULL) {
    pRfd -> next = I596_NULL;
    pRfd -> cmd |= CMD_EOL;    /* set EL bit */

    if ( *ppQ == NULL || *ppQ == I596_NULL ) {
      /* empty list */
      *ppQ = pRfd;
    }
    else {
      /* walk to the end of the list */
      for ( p=*ppQ;
            p->next != I596_NULL;
            p=(i596_rfd *) word_swap ((unsigned long)p->next) );

      /* append the rfd */
      p->cmd &= ~CMD_EOL;  /* Clear EL bit at end */
      p->next = (i596_rfd *) word_swap ((unsigned long)pRfd);
    }
  }
  else {
    printk(("Illegal attempt to append: %p\n", pRfd))
  }
}

/*
 *  uti596_supplyFD
 *
 *  Return a buffer (RFD) to the receive frame area (RFA).
 *  Call with interrupts disabled.
 *
 *  Input parameters:
 *  	pRfd - a pointer to the buffer to be returned
 *
 *  Output parameters: NONE
 *
 *  Return value: NONE
 */
void uti596_supplyFD (
  i596_rfd * pRfd
)
{
 i596_rfd *pLastRfd;

 UTI_596_ASSERT(pRfd != I596_NULL, "Supplying NULL RFD!\n")

 pRfd -> cmd  = CMD_EOL;
 pRfd -> pRbd = I596_NULL;
 pRfd -> next = I596_NULL;
 pRfd -> stat = 0x0000;      /* clear STAT_C and STAT_B bits */

 /*
  * Check if the list is empty:
  */
 if ( uti596_softc.pBeginRFA == I596_NULL ) {

 	 /* Start a list with one entry */
   uti596_softc.pBeginRFA = uti596_softc.pEndRFA = pRfd;
   UTI_596_ASSERT(uti596_softc.countRFD == 0, "Null begin, but non-zero count\n")
   if ( uti596_softc.pLastUnkRFD != I596_NULL ) {
     printk(("LastUnkRFD is NOT NULL!!\n"))
   }
   uti596_softc.countRFD = 1;
   return;
 }

 /*
  * Check if the last RFD is used/read by the 596.
  */
 pLastRfd = uti596_softc.pEndRFA;

 /* C = complete, B = busy (prefetched) */
 if ( pLastRfd != I596_NULL && ! (pLastRfd -> stat & ( STAT_C | STAT_B ) )) {

   /*
    * Not yet too late to add it
    */
   pLastRfd -> next = (i596_rfd *) word_swap ((unsigned long)pRfd);
   pLastRfd -> cmd &= ~CMD_EOL;  /* RESET_EL : reset EL bit to 0  */
   uti596_softc.countRFD++;  /* Lets assume we add it successfully
                                If not, the RFD may be used, and may
                                decrement countRFD < 0 !! */
   /*
    * Check if the last RFD was used while appending.
    */
   if ( pLastRfd -> stat & ( STAT_C | STAT_B ) ) { /* completed or was prefetched */
     /*
      * Either the EL bit of the last rfd has been read by the 82596,
      * and it will stop after reception,( true when RESET_EL not reached ) or
      * the EL bit was NOT read by the 82596 and it will use the linked
      * RFD for the next reception. ( true when RESET_EL was reached )
      * So, it is unknown whether or not the linked rfd will be used.
      * Therefore, the end of list CANNOT be updated.
      */
     UTI_596_ASSERT ( uti596_softc.pLastUnkRFD == I596_NULL, "Too many Unk RFD's\n" )
     uti596_softc.pLastUnkRFD = pRfd;
     return;
   }
   else {
     /*
      * The RFD being added was not touched by the 82596
      */
     if (uti596_softc.pLastUnkRFD != I596_NULL ) {

       uti596_append((i596_rfd **)&uti596_softc.pSavedRfdQueue, pRfd); /* Only here! saved Q */
       uti596_softc.pEndSavedQueue = pRfd;
       uti596_softc.savedCount++;
       uti596_softc.countRFD--;

     }
     else {

       uti596_softc.pEndRFA = pRfd;   /* the RFA has been extended */

       if ( ( uti596_softc.scb.status & SCB_STAT_RNR ||
              uti596_softc.scb.status & RU_NO_RESOURCES ) &&
              uti596_softc.countRFD > 1 ) {

         /* Ensure that beginRFA is not EOL */
         uti596_softc.pBeginRFA -> cmd &= ~CMD_EOL;

         UTI_596_ASSERT(uti596_softc.pEndRFA -> next == I596_NULL, "supply: List buggered\n")
         UTI_596_ASSERT(uti596_softc.pEndRFA -> cmd & CMD_EOL, "supply: No EOL at end.\n")
         UTI_596_ASSERT(uti596_softc.scb.command == 0, "Supply: scb command must be zero\n")

				 #ifdef DBG_MEM
         printk(("uti596_supplyFD: starting receiver"))
				 #endif

         /* start the receiver */
         UTI_596_ASSERT(uti596_softc.pBeginRFA != I596_NULL, "rx start w/ NULL begin! \n")
         uti596_softc.scb.pRfd = uti596_softc.pBeginRFA;
         uti596_softc.scb.rfd_pointer = word_swap ((unsigned long) uti596_softc.pBeginRFA);

         /* Don't ack RNR! The receiver should be stopped in this case */
         uti596_softc.scb.command = RX_START | SCB_STAT_RNR;

         UTI_596_ASSERT( !(uti596_softc.scb.status & SCB_STAT_FR),"FRAME RECEIVED INT COMING!\n")

         /* send CA signal */
         uti596_issueCA ( &uti596_softc, UTI596_NO_WAIT );
       }
     }
     return;
   }
 }
 else {
   /*
    * too late , pLastRfd in use ( or NULL ),
    * in either case, EL bit has been read, and RNR condition will occur
    */
   uti596_append( (i596_rfd **)&uti596_softc.pSavedRfdQueue, pRfd); /* save it for RNR */

   uti596_softc.pEndSavedQueue = pRfd;  /* reset end of saved queue */
   uti596_softc.savedCount++;

   return;
 }
}

/*
 *  send_packet
 *
 *  Send a raw ethernet packet, add a
 *  	transmit command to the CBL
 *
 *  Input parameters:
 *  	ifp - a pointer to the ifnet structure
 *  	  m	-	a pointer to the mbuf being sent
 *
 *  Output parameters: NONE
 *
 *  Return value: NONE
 */
void send_packet(
  struct ifnet *ifp, struct mbuf *m
)
{
  i596_tbd *pPrev = I596_NULL;
  i596_tbd *pRemainingTbdList;
  i596_tbd *pTbd;
  struct mbuf *n, *input_m = m;
  uti596_softc_ *sc = ifp->if_softc;
  struct mbuf *l = NULL;
  unsigned int length = 0;
  rtems_status_code status;
  int bd_count = 0;
  rtems_event_set events;

 /*
  * For all mbufs in the chain,
  *  fill a transmit buffer descriptor for each
  */
  pTbd = (i596_tbd*) word_swap ((unsigned long)sc->pTxCmd->pTbd);

  do {
    if (m->m_len) {
      /*
       * Fill in the buffer descriptor
       */
      length    += m->m_len;
      pTbd->data = (char *) word_swap ((unsigned long) mtod (m, void *));
      pTbd->size = m->m_len;
      pPrev      = pTbd;
      pTbd       = (i596_tbd *) word_swap ((unsigned long) pTbd->next);
      l          = m;
      m          = m->m_next;
    }
    else {
      /*
       * Just toss empty mbufs
       */
      MFREE (m, n);
      m = n;
      if (l != NULL)
        l->m_next = m;
    }
  } while( m != NULL && ++bd_count < 16 );

  if ( length < UTI_596_ETH_MIN_SIZE ) {
    pTbd->data = (char *) word_swap ((unsigned long) sc->zeroes); /* add padding to pTbd */
    pTbd->size = UTI_596_ETH_MIN_SIZE - length; /* zeroes have no effect on the CRC */
  }
  else   /* Don't use pTbd in the send routine */
    pTbd = pPrev;

  /*  Disconnect the packet from the list of Tbd's  */
  pRemainingTbdList = (i596_tbd *) word_swap ((unsigned long)pTbd->next);
  pTbd->next  = I596_NULL;
  pTbd->size |= UTI_596_END_OF_FRAME;

  sc->rawsndcnt++;

	#ifdef DBG_SEND
  printk(("send_packet: sending packet\n"))
	#endif

  /* Sending Zero length packet: shouldn't happen */
  if (pTbd->size <= 0) return;

	#ifdef DBG_PACKETS
  printk     (("\nsend_packet: Transmitter adds packet\n"))
  print_hdr  ( sc->pTxCmd->pTbd->data ); /* print the first part */
  print_pkt  ( sc->pTxCmd->pTbd->next->data ); /* print the first part */
  print_echo (sc->pTxCmd->pTbd->data);
	#endif

  /* add the command to the output command queue */
  uti596_addCmd ( (i596_cmd *) sc->pTxCmd );

  /* sleep until the command has been processed or Timeout encountered. */
  status= rtems_bsdnet_event_receive (INTERRUPT_EVENT,
                                      RTEMS_WAIT|RTEMS_EVENT_ANY,
                                      RTEMS_NO_TIMEOUT,
                                      &events);

  if ( status != RTEMS_SUCCESSFUL ) {
    printk(("Could not sleep %s\n", rtems_status_text(status)))
  }

	#ifdef DBG_SEND
  printk(("send_packet: RAW - wake\n"))
	#endif

  sc->txInterrupts++;

  if ( sc->pTxCmd -> cmd.status & STAT_OK ) {
    sc->stats.tx_packets++;
  }
  else {

    printk(("*** send_packet: Driver Error 0x%x\n", sc->pTxCmd -> cmd.status ))

    sc->stats.tx_errors++;
    if ( sc->pTxCmd->cmd.status  & 0x0020 )
      sc->stats.tx_retries_exceeded++;
    if (!(sc->pTxCmd->cmd.status & 0x0040))
      sc->stats.tx_heartbeat_errors++;
    if ( sc->pTxCmd->cmd.status  & 0x0400 )
      sc->stats.tx_carrier_errors++;
    if ( sc->pTxCmd->cmd.status  & 0x0800 )
      sc->stats.collisions++;
    if ( sc->pTxCmd->cmd.status  & 0x1000 )
      sc->stats.tx_aborted_errors++;
  } /* end if stat_ok */

  /*
   * Restore the transmitted buffer descriptor chain.
   */
  pTbd -> next = (i596_tbd *) word_swap ((unsigned long)pRemainingTbdList);

  /*
   * Free the mbufs used by the sender.
   */
  m = input_m;
  while ( m != NULL ) {
    MFREE(m,n);
    m = n;
  }
}

/***********************************************************************
 *  Function:   uti596_attach
 *
 *  Description:
 *              Configure the driver, and connect to the network stack
 *
 *  Algorithm:
 *
 *              Check parameters in the ifconfig structure, and
 *              set driver parameters accordingly.
 *              Initialize required rx and tx buffers.
 *              Link driver data structure onto device list.
 *              Return 1 on successful completion.
 *
 ***********************************************************************/

int uti596_attach(
  struct rtems_bsdnet_ifconfig * pConfig,
  int attaching
)
{
  uti596_softc_ *sc = &uti596_softc;				/* device dependent data structure */
  struct ifnet * ifp = (struct ifnet *)&sc->arpcom.ac_if;       /* ifnet structure */
  unsigned char j1;    /* State of J1 jumpers */
  int unitNumber;
  char *unitName;
#if defined(mvme167)
  char *pAddr;
  int addr;
#endif

  #ifdef DBG_ATTACH
  printk(("uti596_attach: begins\n"))
  #endif

  /* The NIC is not started yet */
  sc->started = 0;

  /* Indicate to ULCS that this is initialized */
  ifp->if_softc = (void *)sc;
  sc->pScp = NULL;

  /* Parse driver name */
  if ((unitNumber = rtems_bsdnet_parse_driver_name (pConfig, &unitName)) < 0)
    return 0;

  ifp->if_name = unitName;
  ifp->if_unit = unitNumber;

  /* Assign mtu */
  if ( pConfig -> mtu )
    ifp->if_mtu = pConfig -> mtu;
  else
    ifp->if_mtu = ETHERMTU;

  /*
   * Check whether parameters should be obtained from NVRAM. If
   * yes, and if an IP address, netmask, or ethernet address are
   * provided in NVRAM, cheat, and stuff them into the ifconfig
   * structure, OVERRIDING and existing or NULL values.
   *
   * Warning: If values are provided in NVRAM, the ifconfig entries
   * must be NULL because buffer memory allocated to hold the
   * structure values is unrecoverable and would be lost here.
   */

  /* Read the J1 header */
  j1 = (unsigned char)(lcsr->vector_base & 0xFF);

#if defined(mvme167)
  if ( !(j1 & 0x10) ) {
  	/* Jumper J1-4 is on, configure from NVRAM */

    if ( (addr = nvram->ipaddr) ) {
      /* We have a non-zero entry, copy the value */
      if ( (pAddr = malloc ( INET_ADDR_MAX_BUF_SIZE, 0, M_NOWAIT )) )
        pConfig->ip_address = (char *)inet_ntop(AF_INET, &addr, pAddr, INET_ADDR_MAX_BUF_SIZE -1 );
      else
        rtems_panic("Can't allocate ip_address buffer!\n");
    }

    if ( (addr = nvram->netmask) ) {
      /* We have a non-zero entry, copy the value */
      if ( (pAddr = malloc ( INET_ADDR_MAX_BUF_SIZE, 0, M_NOWAIT )) )
        pConfig->ip_netmask = (char *)inet_ntop(AF_INET, &addr, pAddr, INET_ADDR_MAX_BUF_SIZE -1 );
      else
        rtems_panic("Can't allocate ip_netmask buffer!\n");
    }

    /* Ethernet address requires special handling -- it must be copied into
     * the arpcom struct. The following if construct serves only to give the
     * NVRAM parameter the highest priority if J1-4 indicates we are configuring
     * from NVRAM.
     *
     * If the ethernet address is specified in NVRAM, go ahead and copy it.
     * (ETHER_ADDR_LEN = 6 bytes).
     */
    if ( nvram->enaddr[0] || nvram->enaddr[1] || nvram->enaddr[2] ) {
      /* Anything in the first three bytes indicates a non-zero entry, copy value */
  	  memcpy ((void *)sc->arpcom.ac_enaddr, &nvram->enaddr, ETHER_ADDR_LEN);
    }
    else if ( pConfig->hardware_address) {
      /* There is no entry in NVRAM, but there is in the ifconfig struct, so use it. */
      memcpy ((void *)sc->arpcom.ac_enaddr, pConfig->hardware_address, ETHER_ADDR_LEN);
    }
    else {
      /* There is no ethernet address provided, so it will be read
       * from BBRAM at $FFFC1F2C by default. [mvme167 manual p. 1-47]
       */
      memcpy ((void *)sc->arpcom.ac_enaddr, (char *)0xFFFC1F2C, ETHER_ADDR_LEN);
    }
  }
  else if ( pConfig->hardware_address) {
    /* We are not configuring from NVRAM (J1-4 is off), and the ethernet address
     * is given in the ifconfig structure. Copy it.
     */
    memcpy ((void *)sc->arpcom.ac_enaddr, pConfig->hardware_address, ETHER_ADDR_LEN);
  }
  else
#endif
  {
    /* We are not configuring from NVRAM (J1-4 is off), and there is no ethernet
     * address provided in the ifconfig struct, so it will be read from BBRAM at
     * $FFFC1F2C by default. [mvme167 manual p. 1-47]
     */
    memcpy ((void *)sc->arpcom.ac_enaddr, (char *)0xFFFC1F2C, ETHER_ADDR_LEN);
  }

  /* Possibly override default acceptance of broadcast packets */
  if (pConfig->ignore_broadcast)
  	uti596initSetup[8] |= 0x02;

  /* Assign requested receive buffer descriptor count */
  if (pConfig->rbuf_count)
    sc->rxBdCount = pConfig->rbuf_count;
  else
    sc->rxBdCount = RX_BUF_COUNT;

  /* Assign requested tx buffer descriptor count */
  if (pConfig->xbuf_count)
    sc->txBdCount = pConfig->xbuf_count;
  else
    sc->txBdCount = TX_BUF_COUNT * TX_BD_PER_BUF;

  /* Set up fields in the ifnet structure*/
  ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX;
  ifp->if_snd.ifq_maxlen = ifqmaxlen;
  ifp->if_init = uti596_init;
  ifp->if_ioctl = uti596_ioctl;
  ifp->if_start = uti596_start;
  ifp->if_output = ether_output;

  /* uti596_softc housekeeping */
  sc->started = 1;
  sc->pInboundFrameQueue = I596_NULL;
  sc->scb.command = 0;

  /*
   * Attach the interface
   */
  if_attach (ifp);
  ether_ifattach (ifp);
  return 1;
}

/***********************************************************************
 *  Function:  uti596_start
 *
 *  Description:
 *             start the driver
 *
 *  Algorithm:
 *             send an event to the tx task
 *             set the if_flags
 *
 ***********************************************************************/
static void uti596_start(
  struct ifnet *ifp
)
{
  uti596_softc_ *sc = ifp->if_softc;

	#ifdef DBG_START
  printk(("uti596_start: begins\n"))
	#endif

  rtems_event_send (sc->txDaemonTid, START_TRANSMIT_EVENT);
  ifp->if_flags |= IFF_OACTIVE;
}

/***********************************************************************
 *  Function:  uti596_init
 *
 *  Description:
 *             driver initialization
 *
 *  Algorithm:
 *             initialize the 82596
 *             start driver tx and rx tasks, and reset task
 *             send the RX_START command the the RU
 *             set if_flags
 *
 *
 ***********************************************************************/
void uti596_init(
  void * arg
)
{
  uti596_softc_ *sc = arg;
  struct ifnet *ifp = (struct ifnet *)&sc->arpcom.ac_if;

  if (sc->txDaemonTid == 0) {

    /*
     * Initialize the 82596
     */
    #ifdef DBG_INIT
    printk(("uti596_init: begins\nuti596_init: initializing the 82596...\n"))
    #endif
    uti596_initialize_hardware(sc);

    /*
     * Start driver tasks
     */
    #ifdef DBG_INIT
    printk(("uti596_init: starting driver tasks...\n"))
    #endif
    sc->txDaemonTid = rtems_bsdnet_newproc ("UTtx", 2*4096, uti596_txDaemon, (void *)sc);
    sc->rxDaemonTid = rtems_bsdnet_newproc ("UTrx", 2*4096, uti596_rxDaemon, (void *)sc);
    sc->resetDaemonTid = rtems_bsdnet_newproc ("UTrt", 2*4096, uti596_resetDaemon, (void *)sc);

    #ifdef DBG_INIT
    printk(("uti596_init: After attach, status of board = 0x%x\n", sc->scb.status ))
    #endif
  }

  /*
   * In case the ISR discovers there are no resources it reclaims
   * them and restarts
   */
  sc->started = 1;

  /*
   * Enable receiver
   */
  #ifdef DBG_INIT
  printk(("uti596_init: enabling the reciever...\n" ))
  #endif
  sc->scb.command = RX_START;
  uti596_issueCA ( sc, UTI596_WAIT_FOR_CU_ACCEPT );

  /*
   * Tell the world that we're running.
   */
  ifp->if_flags |= IFF_RUNNING;
  #ifdef DBG_INIT
  printk(("uti596_init: completed.\n"))
  #endif
}

/***********************************************************************
 *  Function:   uti596stop
 *
 *  Description:
 *             stop the driver
 *
 *  Algorithm:
 *             mark driver as not started,
 *             mark transmitter as busy
 *             abort any transmissions/receptions
 *             clean-up all buffers ( RFD's et. al. )
 *
 *
 ***********************************************************************/

/* static */ void uti596_stop(
  uti596_softc_ *sc
)
{
  struct ifnet *ifp = (struct ifnet *)&sc->arpcom.ac_if;

  ifp->if_flags &= ~IFF_RUNNING;
  sc->started = 0;

  #ifdef DBG_STOP
  printk(("uti596stop: %s: Shutting down ethercard, status was %4.4x.\n",
           uti596_softc.arpcom.ac_if.if_name, uti596_softc.scb.status))
  #endif

  printk(("Stopping interface\n"))
  sc->scb.command = CUC_ABORT | RX_ABORT;
  i82596->chan_attn = 0x00000000;
}

/***********************************************************************
 *  Function:   void uti596_txDaemon
 *
 *  Description: Transmit task
 *
 *  Algorithm: Get mbufs to be transmitted, stuff into RFDs, send
 *
 ***********************************************************************/

void uti596_txDaemon(
  void *arg
)
{
  uti596_softc_ *sc = (uti596_softc_ *)arg;
  struct ifnet *ifp = (struct ifnet *)&sc->arpcom.ac_if;
  struct mbuf *m;
  rtems_event_set events;

  for (;;) {
   /*
    * Wait for packet from stack
    */
    rtems_bsdnet_event_receive (START_TRANSMIT_EVENT,
                                RTEMS_EVENT_ANY | RTEMS_WAIT,
                                RTEMS_NO_TIMEOUT, &events);

   /*
    * Send packets till queue is empty.
    * Ensure that irq is on before sending.
    */
    for (;;) {
     /* Get the next mbuf chain to transmit. */
      IF_DEQUEUE(&ifp->if_snd, m);
      if (!m)
        break;

      send_packet (ifp, m); /* blocks */
    }
    ifp->if_flags &= ~IFF_OACTIVE; /* no more to send, mark output inactive  */
  }
}

/***********************************************************************
 *  Function:   uti596_rxDaemon
 *
 *  Description: Receiver task
 *
 *  Algorithm: Extract the packet from an RFD, and place into an
 *             mbuf chain.  Place the mbuf chain in the network task
 *             queue. Assumes that the frame check sequence is removed
 *             by the 82596.
 *
 ***********************************************************************/

/* static */ void uti596_rxDaemon(
  void *arg
)
{
  uti596_softc_ *sc = (uti596_softc_ *)arg;
  struct ifnet *ifp = (struct ifnet *)&sc->arpcom.ac_if;
  struct mbuf *m;

  i596_rfd *pRfd;
  ISR_Level level;
  rtems_id tid;
  rtems_event_set events;
  struct ether_header *eh;

  int frames = 0;

	#ifdef DBG_RX
  printk(("uti596_rxDaemon: begin\n"))
  printk(("&scb = %p, pRfd = %p\n", &sc->scb,sc->scb.pRfd))
	#endif

  rtems_task_ident (0, 0, &tid);

  for(;;) {
    /*
     * Wait for packet.
     */
	#ifdef DBG_RX
     printk(("uti596_rxDaemon: Receiver sleeps\n"))
	#endif

     rtems_bsdnet_event_receive (INTERRUPT_EVENT,
                                 RTEMS_WAIT|RTEMS_EVENT_ANY,
                                 RTEMS_NO_TIMEOUT,
                                 &events);

		 #ifdef DBG_RX
     printk(("uti596_rxDaemon: Receiver wakes\n"))
		 #endif
     /*
      * While received frames are available. Note that the frame may be
      * a fragment, so it is NOT a complete packet.
      */
     pRfd = uti596_dequeue( (i596_rfd **)&sc->pInboundFrameQueue);
     while ( pRfd &&
             pRfd != I596_NULL &&
             pRfd -> stat & STAT_C )
     {

       if ( pRfd->stat & STAT_OK) {    				/* a good frame */
         int pkt_len = pRfd->count & 0x3fff;	/* the actual # of bytes received */

				 #ifdef DBG_RX
         printk(("uti596_rxDaemon: Good frame, @%p, data @%p length %d\n", pRfd, pRfd -> data , pkt_len))
				 #endif
         frames++;

         /*
          * Allocate an mbuf to give to the stack
          * The format of the data portion of the RFD is:
          * <ethernet header, payload>.
          * The FRAME CHECK SEQUENCE / CRC is stripped by the uti596.
          * This is to be optimized later.... should not have to memcopy!
          */
         MGETHDR(m, M_WAIT, MT_DATA);
         MCLGET(m, M_WAIT);

         m->m_pkthdr.rcvif = ifp;
         /* move everything into an mbuf */
         memcpy(m->m_data, (const char *)pRfd->data, pkt_len);
         m->m_len = m->m_pkthdr.len = pkt_len - sizeof(struct ether_header) - 4;

         /* move the header to an mbuf */
         eh = mtod (m, struct ether_header *);
         m->m_data += sizeof(struct ether_header);

				 #ifdef DBG_PACKETS
				 {
				 	 int i;
         	 printk(("uti596_rxDaemon: mbuf contains:\n"))
         	 print_eth( (char *) (((int)m->m_data)-sizeof(struct ether_header)));
         	 for ( i = 0; i<20; i++) {
 		     	   printk(("."))
	       	 }
         }
				 #endif

       	 ether_input (ifp, eh, m);

       } /* end if STAT_OK */

       else {
         /*
          * A bad frame is present: Note that this could be the last RFD!
          */
				 #ifdef DBG_RX
         printk(("uti596_rxDaemon: Bad frame\n"))
				 #endif
         /*
          * FIX ME: use the statistics from the SCB
          */
         sc->stats.rx_errors++;
         if ((sc->scb.pRfd->stat) & 0x0001)
           sc->stats.collisions++;
         if ((sc->scb.pRfd->stat) & 0x0080)
           sc->stats.rx_length_errors++;
         if ((sc->scb.pRfd->stat) & 0x0100)
           sc->stats.rx_over_errors++;
         if ((sc->scb.pRfd->stat) & 0x0200)
           sc->stats.rx_fifo_errors++;
         if ((sc->scb.pRfd->stat) & 0x0400)
           sc->stats.rx_frame_errors++;
         if ((sc->scb.pRfd->stat) & 0x0800)
           sc->stats.rx_crc_errors++;
         if ((sc->scb.pRfd->stat) & 0x1000)
           sc->stats.rx_length_errors++;
       }

       UTI_596_ASSERT(pRfd != I596_NULL, "Supplying NULL RFD\n")

       _ISR_Disable(level);
       uti596_supplyFD ( pRfd );   /* Return RFD to RFA. */
       _ISR_Enable(level);

       pRfd = uti596_dequeue( (i596_rfd **)&sc->pInboundFrameQueue); /* grab next frame */

     } /* end while */
  } /* end for() */

	#ifdef DBG_RX
  printk (("uti596_rxDaemon: frames ... %d\n", frames))
	#endif
}

/***********************************************************************
 *  Function:   void uti596_resetDaemon
 *
 *  Description:
 ***********************************************************************/
void uti596_resetDaemon(
  void *arg
)
{
  uti596_softc_ *sc = (uti596_softc_ *)arg;
  rtems_event_set events;
  rtems_time_of_day tm_struct;

  /* struct ifnet *ifp = &sc->arpcom.ac_if; */

  for (;;) {
   /* Wait for reset event from ISR */
    rtems_bsdnet_event_receive (NIC_RESET_EVENT,
                                RTEMS_EVENT_ANY | RTEMS_WAIT,
                                RTEMS_NO_TIMEOUT, &events);

    rtems_clock_get(RTEMS_CLOCK_GET_TOD, &tm_struct);
    printk(("reset daemon: Resetting NIC @ %d:%d:%d \n",
           tm_struct.hour, tm_struct.minute, tm_struct.second))

    sc->stats.nic_reset_count++;
    /* Reinitialize the LANC */
    rtems_bsdnet_semaphore_obtain ();
    uti596_reset();
    rtems_bsdnet_semaphore_release ();
  }
}

/***********************************************************************
 *  Function:   uti596_DynamicInterruptHandler
 *
 *  Description:
 *             This is the interrupt handler for the uti596 board
 *
 ***********************************************************************/

/* static */ rtems_isr uti596_DynamicInterruptHandler(
  rtems_vector_number irq
)
{
int fullStatus;

	#ifdef DBG_ISR
  printk(("uti596_DynamicInterruptHandler: begins"))
	#endif

 uti596_wait (&uti596_softc, UTI596_WAIT_FOR_CU_ACCEPT);

 scbStatus = (fullStatus = uti596_softc.scb.status) & 0xf000;

 if ( scbStatus ) {
   /* acknowledge interrupts */

   /* Write to the ICLR bit in the PCCchip2 control registers to clear
    * the INT status bit. Clearing INT here *before* sending the CA signal
    * to the 82596 should ensure that interrupts won't be lost.
    */
    pccchip2->LANC_int_ctl |=0x08;
    pccchip2->LANC_berr_ctl |=0x08;

    /* printk(("***INFO: ACK %x\n", scbStatus))*/

    /* Send the CA signal to acknowledge interrupt */
    uti596_softc.scb.command = scbStatus;
    uti596_issueCA ( &uti596_softc, UTI596_NO_WAIT );

    if( uti596_softc.resetDone ) {
      /* stack is attached */
      uti596_wait ( &uti596_softc, UTI596_WAIT_FOR_CU_ACCEPT );
    }
    else {
      printk(("***INFO: ACK'd w/o processing. status = %x\n", scbStatus))
      return;
    }
  }
  else {
#ifndef IGNORE_SPURIOUS_IRQ
    printk(("\n***ERROR: Spurious interrupt (full status 0x%x). Resetting...\n", fullStatus))
    uti596_softc.nic_reset = 1;
#endif
  }

  if ( (scbStatus & SCB_STAT_CX) && !(scbStatus & SCB_STAT_CNA) ) {
    printk(("\n*****ERROR: Command Complete, and CNA available: 0x%x\nResetting...", scbStatus))
    uti596_softc.nic_reset = 1;
    return;
  }

  if ( !(scbStatus & SCB_STAT_CX) && (scbStatus & SCB_STAT_CNA) ) {
    printk(("\n*****ERROR: CNA, NO CX:0x%x\nResetting...",scbStatus))
    uti596_softc.nic_reset = 1;
    return;
  }

  if ( scbStatus & SCB_CUS_SUSPENDED ) {
    printk(("\n*****ERROR: Command unit suspended!:0x%x\nResetting...",scbStatus))
    uti596_softc.nic_reset = 1;
    return;
  }

  if ( scbStatus & RU_SUSPENDED  ) {
    printk(("\n*****ERROR: Receive unit suspended!:0x%x\nResetting...",scbStatus))
    uti596_softc.nic_reset = 1;
    return;
  }

  if ( scbStatus & SCB_STAT_RNR ) {
    printk(("\n*****WARNING: RNR %x\n",scbStatus))
    if (uti596_softc.pBeginRFA != I596_NULL) {
        printk(("*****INFO: RFD cmd: %x status:%x\n", uti596_softc.pBeginRFA->cmd,
                                        uti596_softc.pBeginRFA->stat))
    }
    else {
        printk(("*****WARNING: RNR condition with NULL BeginRFA\n"))
    }
  }

 /*
  * Receive Unit Control
  *   a frame is received
  */
  if ( scbStatus & SCB_STAT_FR ) {
    uti596_softc.rxInterrupts++;

		#ifdef DBG_ISR
    printk(("uti596_DynamicInterruptHandler: Frame received\n"))
		#endif
    if ( uti596_softc.pBeginRFA == I596_NULL ||
       !( uti596_softc.pBeginRFA -> stat & STAT_C)) {
#ifndef IGNORE_NO_RFA
      uti596_dump_scb();
      uti596_softc.nic_reset = 1;
#endif
    }
    else {
      while ( uti596_softc.pBeginRFA != I596_NULL &&
           ( uti596_softc.pBeginRFA -> stat & STAT_C)) {

				#ifdef DBG_ISR
        printk(("uti596_DynamicInterruptHandler: pBeginRFA != NULL\n"))
				#endif
        count_rx ++;
#ifndef IGNORE_MULTIPLE_RF
        if ( count_rx > 1) {
          printk(("****WARNING: Received %i frames on 1 interrupt \n", count_rx))
		}
#endif
       /* Give Received Frame to the ULCS */
        uti596_softc.countRFD--;

        if ( uti596_softc.countRFD < 0 ) {
          printk(("ISR: Count < 0 !!! count == %d, beginRFA = %p\n",
                 uti596_softc.countRFD, uti596_softc.pBeginRFA))
                                }
        uti596_softc.stats.rx_packets++;
        /* the rfd next link is stored with upper and lower words swapped so read it that way */
        pIsrRfd = (i596_rfd *) word_swap ((unsigned long)uti596_softc.pBeginRFA->next);
        /* the append destroys the link */
        uti596_append( (i596_rfd **)&uti596_softc.pInboundFrameQueue , uti596_softc.pBeginRFA );

       /*
        * if we have just received the a frame in the last unknown RFD,
        * then it is certain that the RFA is empty.
        */
        if ( uti596_softc.pLastUnkRFD == uti596_softc.pBeginRFA ) {
          UTI_596_ASSERT(uti596_softc.pLastUnkRFD != I596_NULL,"****ERROR:LastUnk is NULL, begin ptr @ end!\n")
          uti596_softc.pEndRFA = uti596_softc.pLastUnkRFD = I596_NULL;
        }

				#ifdef DBG_ISR
        printk(("uti596_DynamicInterruptHandler: Wake %#x\n",uti596_softc.rxDaemonTid))
				#endif
        sc = rtems_event_send(uti596_softc.rxDaemonTid, INTERRUPT_EVENT);
        if ( sc != RTEMS_SUCCESSFUL ) {
          rtems_panic("Can't notify rxDaemon: %s\n",
                    rtems_status_text (sc));
        }
				#ifdef DBG_ISR
        else {
          printk(("uti596_DynamicInterruptHandler: Rx Wake: %#x\n",uti596_softc.rxDaemonTid))
        }
				#endif

        uti596_softc.pBeginRFA = pIsrRfd;
      } /* end while */
    } /* end if */

    if ( uti596_softc.pBeginRFA == I596_NULL ) {
      /* adjust the pEndRFA to reflect an empty list */
      if ( uti596_softc.pLastUnkRFD == I596_NULL && uti596_softc.countRFD != 0 ) {
        printk(("Last Unk is NULL, BeginRFA is null, and count == %d\n",
               uti596_softc.countRFD))
                        }
      uti596_softc.pEndRFA = I596_NULL;
      if ( uti596_softc.countRFD != 0 ) {
        printk(("****ERROR:Count is %d, but begin ptr is NULL\n",
               uti596_softc.countRFD ))
      }
    }
  } /* end if ( scbStatus & SCB_STAT_FR ) */

 /*
  * Command Unit Control
  *   a command is completed
  */
  if ( scbStatus & SCB_STAT_CX ) {
		#ifdef DBG_ISR
    printk(("uti596_DynamicInterruptHandler: CU\n"))
		#endif

    pIsrCmd = uti596_softc.pCmdHead;

   /* For ALL completed commands */
   if ( pIsrCmd !=  I596_NULL && pIsrCmd->status & STAT_C  ) {

			 #ifdef DBG_ISR
       printk(("uti596_DynamicInterruptHandler: pIsrCmd != NULL\n"))
			 #endif

      /* Adjust the command block list */
      uti596_softc.pCmdHead = (i596_cmd *) word_swap ((unsigned long)pIsrCmd->next);

     /*
      * If there are MORE commands to process,
      * the serialization in the raw routine has failed.
      * ( Perhaps AddCmd is bad? )
      */
      UTI_596_ASSERT(uti596_softc.pCmdHead == I596_NULL, "****ERROR: command serialization failed\n")

      /* What if the command did not complete OK? */
      switch ( pIsrCmd->command & 0x7) {
        case CmdConfigure:

          uti596_softc.cmdOk = 1;
          break;

        case CmdDump:
					#ifdef DBG_ISR
          printk(("uti596_DynamicInterruptHandler: dump!\n"))
					#endif
          uti596_softc.cmdOk = 1;
          break;

        case CmdDiagnose:
					#ifdef DBG_ISR
          printk(("uti596_DynamicInterruptHandler: diagnose!\n"))
					#endif
          uti596_softc.cmdOk = 1;
          break;

        case CmdSASetup:
          /* printk(("****INFO:Set address interrupt\n")) */
          if ( pIsrCmd -> status & STAT_OK ) {
            uti596_softc.cmdOk = 1;
          }
          else {
            printk(("****ERROR:SET ADD FAILED\n"))
                                        }
          break;

        case CmdTx:
          UTI_596_ASSERT(uti596_softc.txDaemonTid, "****ERROR:Null txDaemonTid\n")
					#ifdef DBG_ISR
          printk(("uti596_DynamicInterruptHandler: wake TX:0x%x\n",uti596_softc.txDaemonTid))
					#endif
          if ( uti596_softc.txDaemonTid ) {
            /* Ensure that the transmitter is present */
            sc = rtems_event_send (uti596_softc.txDaemonTid,
                                 INTERRUPT_EVENT);

            if ( sc != RTEMS_SUCCESSFUL ) {
              printk(("****ERROR:Could NOT send event to tid 0x%x : %s\n",
                     uti596_softc.txDaemonTid, rtems_status_text (sc) ))
            }
						#ifdef DBG_ISR
            else {
              printk(("****INFO:Tx wake: %#x\n",uti596_softc.txDaemonTid))
            }
						#endif
          }
          break;

        case CmdMulticastList:
          printk(("***ERROR:Multicast?!\n"))
          pIsrCmd->next = I596_NULL;
          break;

        case CmdTDR: {
                  unsigned long status = *( (unsigned long *)pIsrCmd)+1;
                  printk(("****ERROR:TDR?!\n"))

                  if (status & STAT_C) {
                    /* mark the TDR command successful */
                    uti596_softc.cmdOk = 1;
                  }
                  else {
                    if (status & 0x4000) {
                      printk(("****WARNING:Transceiver problem.\n"))
                    }
                    if (status & 0x2000) {
                      printk(("****WARNING:Termination problem.\n"))
                    }
                    if (status & 0x1000) {
                      printk(("****WARNING:Short circuit.\n"))
                      /* printk(("****INFO:Time %ld.\n", status & 0x07ff)) */
                    }
                  }
          }
          break;

        default: {
          /*
           * This should never be reached
           */
          printk(("CX but NO known command\n"))
        }
      } /* end switch */

      pIsrCmd = uti596_softc.pCmdHead; /* next command */
      if ( pIsrCmd != I596_NULL ) {
        printk(("****WARNING: more commands in list, but no start to NIC\n"))
      }
    } /* end if pIsrCmd != NULL && pIsrCmd->stat & STAT_C  */

    else {
      if ( pIsrCmd != I596_NULL ) {
        /* The command MAY be NULL from a RESET */
        /* Reset the ethernet card, and wake the transmitter (if necessary) */
        printk(("****INFO: Request board reset ( tx )\n"))
        uti596_softc.nic_reset = 1;
        if ( uti596_softc.txDaemonTid) {
          /* Ensure that a transmitter is present */
          sc = rtems_event_send (uti596_softc.txDaemonTid,
                                 INTERRUPT_EVENT);
          if ( sc != RTEMS_SUCCESSFUL ) {
            printk(("****ERROR:Could NOT send event to tid 0x%x : %s\n",
            				 uti596_softc.txDaemonTid, rtems_status_text (sc) ))
          }
					#ifdef DBG_ISR
          else {
            printk(("uti596_DynamicInterruptHandler: ****INFO:Tx wake: %#x\n",
            				 uti596_softc.txDaemonTid))
          }
					#endif
        }
      }
    }
  }  /* end if command complete */

 /*
  * If the receiver has stopped,
  * check if this is a No Resources scenario,
  * Try to add more RFD's ( no RBDs are used )
  */
  if ( uti596_softc.started ) {
    if ( scbStatus & SCB_STAT_RNR ) {
			#ifdef DBG_ISR
      printk(("uti596_DynamicInterruptHandler: INFO:RNR: status %#x \n",
      				uti596_softc.scb.status ))
			#endif
     /*
      * THE RECEIVER IS OFF!
      */
      if ( uti596_softc.pLastUnkRFD != I596_NULL  ) {
        /* We have an unknown RFD, it is not inbound */
        if ( uti596_softc.pLastUnkRFD -> stat & (STAT_C | STAT_B )) { /* in use */
          uti596_softc.pEndRFA = uti596_softc.pLastUnkRFD;            /* update end */
        }
        else {
         /*
          *  It is NOT in use, and since RNR, we know EL bit of pEndRFA was read!
          *  So, unlink it from the RFA and move it to the saved queue.
          *  But pBegin can equal LastUnk!
          */

          if ( uti596_softc.pEndRFA != I596_NULL ) {
            /* check added feb24. */
						#ifdef DBG_ISR
            if ((i596_rfd *)word_swap((unsigned long)uti596_softc.pEndRFA->next) != uti596_softc.pLastUnkRFD) {
              printk(("***ERROR:UNK: %p not end->next: %p, end: %p\n",
                     uti596_softc.pLastUnkRFD,
                     uti596_softc.pEndRFA -> next,
                     uti596_softc.pEndRFA))
              printk(("***INFO:countRFD now %d\n",
                     uti596_softc.countRFD))
              printk(("\n\n"))
            }
						#endif
            uti596_softc.pEndRFA -> next = I596_NULL;   /* added feb 16 */
          }
          uti596_append( (i596_rfd **)&uti596_softc.pSavedRfdQueue, uti596_softc.pLastUnkRFD );
          uti596_softc.savedCount++;
          uti596_softc.pEndSavedQueue = uti596_softc.pLastUnkRFD;
          uti596_softc.countRFD--;                    /* It was not in the RFA */
         /*
          * The Begin pointer CAN advance this far. We must resynch the CPU side
          * with the chip.
          */
          if ( uti596_softc.pBeginRFA == uti596_softc.pLastUnkRFD ) {
						#ifdef DBG_ISR
            if ( uti596_softc.countRFD != 0 ) {
              printk(("****INFO:About to set begin to NULL, with count == %d\n\n",
                     uti596_softc.countRFD ))
            }
						#endif
            uti596_softc.pBeginRFA = I596_NULL;
            UTI_596_ASSERT(uti596_softc.countRFD == 0, "****ERROR:Count must be zero here!\n")
          }
        }
        uti596_softc.pLastUnkRFD = I596_NULL;
      } /* end if exists UnkRFD */

     /*
      * Append the saved queue to  the RFA.
      * Any further RFD's being supplied will be added to
      * this new list.
      */
      if ( uti596_softc.pSavedRfdQueue != I596_NULL ) {
        /* entries to add */
        if ( uti596_softc.pBeginRFA == I596_NULL ) {
          /* add at beginning to list */
					#ifdef DBG_ISR
          if(uti596_softc.countRFD != 0) {
            printk(("****ERROR:Begin pointer is NULL, but count == %d\n",
                   uti596_softc.countRFD))
          }
					#endif
          uti596_softc.pBeginRFA      = uti596_softc.pSavedRfdQueue;
          uti596_softc.pEndRFA        = uti596_softc.pEndSavedQueue;
          uti596_softc.pSavedRfdQueue = uti596_softc.pEndSavedQueue = I596_NULL;  /* Reset the End */
        }
        else {
					#ifdef DBG_ISR
          if ( uti596_softc.countRFD <= 0) {
            printk(("****ERROR:Begin pointer is not NULL, but count == %d\n",
                   uti596_softc.countRFD))
          }
					#endif
          UTI_596_ASSERT( uti596_softc.pEndRFA != I596_NULL, "****WARNING: END RFA IS NULL\n")
          UTI_596_ASSERT( uti596_softc.pEndRFA->next == I596_NULL, "****ERROR:END RFA -> next must be NULL\n")

          uti596_softc.pEndRFA->next   = (i596_rfd *)word_swap((unsigned long)uti596_softc.pSavedRfdQueue);
          uti596_softc.pEndRFA->cmd   &= ~CMD_EOL;      /* clear the end of list */
          uti596_softc.pEndRFA         = uti596_softc.pEndSavedQueue;
          uti596_softc.pSavedRfdQueue  = uti596_softc.pEndSavedQueue = I596_NULL; /* Reset the End */
					#ifdef DBG_ISR
          printk(("uti596_DynamicInterruptHandler: count... %d, saved ... %d \n",
                 uti596_softc.countRFD,
                 uti596_softc.savedCount))
					#endif
        }
        /* printk(("Isr: countRFD = %d\n",uti596_softc.countRFD)) */
        uti596_softc.countRFD += uti596_softc.savedCount;
        /* printk(("Isr: after countRFD = %d\n",uti596_softc.countRFD)) */
        uti596_softc.savedCount = 0;
      }

			#ifdef DBG_ISR
      printk(("uti596_DynamicInterruptHandler: The list starts here %p\n",uti596_softc.pBeginRFA ))
			#endif

      if ( uti596_softc.countRFD > 1) {
        printk(("****INFO: pBeginRFA -> stat = 0x%x\n",uti596_softc.pBeginRFA -> stat))
        printk(("****INFO: pBeginRFA -> cmd = 0x%x\n",uti596_softc.pBeginRFA -> cmd))
        uti596_softc.pBeginRFA -> stat = 0;
        UTI_596_ASSERT(uti596_softc.scb.command == 0, "****ERROR:scb command must be zero\n")
        uti596_softc.scb.pRfd = uti596_softc.pBeginRFA;
        uti596_softc.scb.rfd_pointer = word_swap((unsigned long)uti596_softc.pBeginRFA);
        /* start RX here  */
        printk(("****INFO: ISR Starting receiver\n"))
        uti596_softc.scb.command = RX_START; /* should this also be CU start? */
        i82596->chan_attn = 0x00000000;
      }
    } /* end stat_rnr */
  } /* end if receiver started */

	#ifdef DBG_ISR
  printk(("uti596_DynamicInterruptHandler: X\n"))
	#endif
  count_rx=0;

 /* Do this last, to ensure that the reset is called at the right time. */
  if ( uti596_softc.nic_reset ) {
    uti596_softc.nic_reset = 0;
    sc = rtems_event_send(uti596_softc.resetDaemonTid, NIC_RESET_EVENT);
    if ( sc != RTEMS_SUCCESSFUL )
      rtems_panic ("Can't notify resetDaemon: %s\n", rtems_status_text (sc));
  }
  return;
}

/***********************************************************************
 *  Function:  uti596_ioctl
 *
 *  Description:
 *             driver ioctl function
 *             handles SIOCGIFADDR, SIOCSIFADDR, SIOCSIFFLAGS
 *
 ***********************************************************************/

static int uti596_ioctl(
  struct ifnet *ifp,
  u_long command,
  caddr_t data
)
{
  uti596_softc_ *sc = ifp->if_softc;
  int error = 0;

	#ifdef DBG_IOCTL
  printk(("uti596_ioctl: begins\n", sc->pScp))
	#endif

  switch (command) {
    case SIOCGIFADDR:
    case SIOCSIFADDR:
      printk(("SIOCSIFADDR\n"))
      ether_ioctl (ifp, command, data);
      break;

    case SIOCSIFFLAGS:
      printk(("SIOCSIFFLAGS\n"))
      switch (ifp->if_flags & (IFF_UP | IFF_RUNNING)) {
        case IFF_RUNNING:
          printk(("IFF_RUNNING\n"))
          uti596_stop (sc);
          break;

        case IFF_UP:
          printk(("IFF_UP\n"))
          uti596_init ( (void *)sc);
          break;

        case IFF_UP | IFF_RUNNING:
          printk(("IFF_UP and RUNNING\n"))
          uti596_stop (sc);
          uti596_init ( (void *)sc);
          break;

        default:
          printk(("default\n"))
          break;
      }
      break;

    case SIO_RTEMS_SHOW_STATS:
      printk(("show stats\n"))
      uti596_stats (sc);
      break;

    /* FIXME: All sorts of multicast commands need to be added here! */
    default:
      printk(("default: EINVAL\n"))
      error = EINVAL;
      break;
  }

  return error;
}

/***********************************************************************
 *  Function:   uti596_stats
 *
 *  Description:
 *             print out the collected data
 *
 *  Algorithm:
 *            use printf
 *
 ***********************************************************************/

void uti596_stats(
  uti596_softc_ *sc
)
{
  printf ("CPU Reports:\n");
  printf ("  Tx raw send count:%-8lu",  sc->rawsndcnt);
  printf ("  Rx Interrupts:%-8lu",  sc->rxInterrupts);
  printf ("  Tx Interrupts:%-8lu\n",  sc->txInterrupts);
  printf ("  Rx Packets:%-8u",  sc->stats.rx_packets);
  printf ("  Tx Attempts:%-u\n",  sc->stats.tx_packets);

  printf ("  Rx Dropped:%-8u",  sc->stats.rx_dropped);
  printf ("  Rx IP Packets:%-8u",  sc->stats.rx_packets);
  printf ("  Tx Errors:%-8u\n",  sc->stats.tx_errors);
  printf ("  Tx aborted:%-8u",  sc->stats.tx_aborted_errors);
  printf ("  Tx Dropped:%-8u\n",  sc->stats.tx_dropped);
  printf ("  Tx IP packets:%-8u",  sc->stats.tx_packets);

  printf ("  Collisions Detected:%-8u\n",  sc->stats.collisions);
  printf ("  Tx Heartbeat Errors:%-8u",  sc->stats.tx_heartbeat_errors);
  printf ("  Tx Carrier Errors:%-8u\n",  sc->stats.tx_carrier_errors);
  printf ("  Tx Aborted Errors:%-8u",  sc->stats.tx_aborted_errors);
  printf ("  Rx Length Errors:%-8u\n",  sc->stats.rx_length_errors);
  printf ("  Rx Overrun Errors:%-8u",  sc->stats.rx_over_errors);
  printf ("  Rx Fifo Errors:%-8u\n",  sc->stats.rx_fifo_errors);
  printf ("  Rx Framing Errors:%-8u",  sc->stats.rx_frame_errors);
  printf ("  Rx crc errors:%-8u\n",  sc->stats.rx_crc_errors);

  printf ("  TX WAITS: %-8lu\n",  sc->txRawWait);

  printf ("  NIC resets: %-8u\n",  sc->stats.nic_reset_count);

  printf ("  NIC reports\n");

	#ifdef DBG_STAT
  uti596_dump_scb();
  #endif
}

/************************ PACKET DEBUG ROUTINES ************************/

#ifdef DBG_PACKETS

/*
 *  dumpQ
 *
 *  Dumps frame queues for debugging
 */
static void dumpQ( void )
{
  i596_rfd *pRfd;

  printk(("savedQ:\n"))

  for( pRfd = uti596_softc.pSavedRfdQueue;
       pRfd != I596_NULL;
       pRfd = (i596_rfd*)word_swap((uint32_t)pRfd -> next)) {
      printk(("pRfd: %p, stat: 0x%x cmd: 0x%x\n",pRfd,pRfd -> stat,pRfd -> cmd))
  }

  printk(("Inbound:\n"))

  for( pRfd = uti596_softc.pInboundFrameQueue;
       pRfd != I596_NULL;
       pRfd = (i596_rfd*)word_swap((uint32_t)pRfd -> next)) {
    printk(("pRfd: %p, stat: 0x%x cmd: 0x%x\n",pRfd,pRfd -> stat,pRfd -> cmd))
  }

  printk(("Last Unk: %p\n", uti596_softc.pLastUnkRFD ))
  printk(("RFA:\n"))

  for( pRfd = uti596_softc.pBeginRFA;
       pRfd != I596_NULL;
       pRfd = (i596_rfd*)word_swap((uint32_t)pRfd -> next)) {
    printk(("pRfd: %p, stat: 0x%x cmd: 0x%x\n",pRfd,pRfd -> stat,pRfd -> cmd))
  }
}

/*
 *  show_buffers
 *
 *  Print out the RFA and frame queues
 */
static void show_buffers (void)
{
  i596_rfd *pRfd;

  printk(("82596 cmd: 0x%x, status: 0x%x RFA len: %d\n",
         uti596_softc.scb.command,
         uti596_softc.scb.status,
         uti596_softc.countRFD))

  printk(("\nRFA: \n"))

  for ( pRfd = uti596_softc.pBeginRFA;
        pRfd != I596_NULL;
        pRfd = (i596_rfd *)word_swap((uint32_t)pRfd->next) ) {
    printk(("Frame @ %p, status: %2.2x, cmd: %2.2x\n",
            pRfd, pRfd->stat, pRfd->cmd))
        }
  printk(("\nInbound: \n"))

  for ( pRfd = uti596_softc.pInboundFrameQueue;
        pRfd != I596_NULL;
        pRfd = (i596_rfd *)word_swap((uint32_t)pRfd->next) ) {
    printk(("Frame @ %p, status: %2.2x, cmd: %2.2x\n",
            pRfd, pRfd->stat, pRfd->cmd))
        }

  printk(("\nSaved: \n"))

  for ( pRfd = uti596_softc.pSavedRfdQueue;
        pRfd != I596_NULL;
        pRfd = (i596_rfd *)word_swap((uint32_t)pRfd->next) ) {
    printk(("Frame @ %p, status: %2.2x, cmd: %2.2x\n",
             pRfd, pRfd->stat, pRfd->cmd))
  }

  printk(("\nUnknown: %p\n",uti596_softc.pLastUnkRFD))
}

/*
 *  show_queues
 *
 *  Print out the saved frame queue and the RFA
 */
static void show_queues(void)
{
  i596_rfd *pRfd;

  printk(("CMD: 0x%x, Status: 0x%x\n",
         uti596_softc.scb.command,
         uti596_softc.scb.status))
  printk(("saved Q\n"))

  for ( pRfd = uti596_softc.pSavedRfdQueue;
        pRfd != I596_NULL &&
        pRfd != NULL;
        pRfd = (i596_rfd *)word_swap((uint32_t)pRfd->next) ) {
    printk(("0x%p\n", pRfd))
  }

  printk(("End saved Q 0x%p\n", uti596_softc.pEndSavedQueue))

  printk(("\nRFA:\n"))

  for ( pRfd = uti596_softc.pBeginRFA;
        pRfd != I596_NULL &&
        pRfd != NULL;
        pRfd = (i596_rfd *)word_swap((uint32_t)pRfd->next) ) {
    printk(("0x%p\n", pRfd))
  }

  printk(("uti596_softc.pEndRFA: %p\n",uti596_softc.pEndRFA))
}

/*
 *  print_eth
 *
 *  Print the contents of an ethernet packet
 *  CANNOT BE CALLED FROM ISR
 */
static void print_eth(
  unsigned char *add
)
{
  int i;
  short int length;

  printk (("Packet Location %p\n", add))
  printk (("Dest  "))

  for (i = 0; i < 6; i++) {
    printk ((" %2.2X", add[i]))
        }
  printk (("\n"))
  printk (("Source"))

  for (i = 6; i < 12; i++) {
        printk ((" %2.2X", add[i]))
  }

  printk (("\n"))
  printk (("frame type %2.2X%2.2X\n", add[12], add[13]))

  if ( add[12] == 0x08 && add[13] == 0x06 ) {
    /* an ARP */
    printk (("Hardware type : %2.2X%2.2X\n", add[14],add[15]))
    printk (("Protocol type : %2.2X%2.2X\n", add[16],add[17]))
    printk (("Hardware size : %2.2X\n", add[18]))
    printk (("Protocol size : %2.2X\n", add[19]))
    printk (("op            : %2.2X%2.2X\n", add[20],add[21]))
    printk (("Sender Enet addr: "))

    for ( i=0; i< 5 ; i++) {
      printk (("%x:", add[22 + i]))
                }
    printk (("%x\n", add[27]))
    printk (("Sender IP addr: "))

    for ( i=0; i< 3 ; i++) {
      printk (("%u.", add[28 + i]))
                }
    printk (("%u\n", add[31]))
    printk (("Target Enet addr: "))

    for ( i=0; i< 5 ; i++) {
      printk (( "%x:", add[32 + i]))
                }
    printk (("%x\n", add[37]))
    printk (("Target IP addr: "))

    for ( i=0; i< 3 ; i++) {
      printk (( "%u.", add[38 + i]))
    }
    printk (("%u\n", add[41]))
  }

  if ( add[12] == 0x08 && add[13] == 0x00 ) {
  	/* an IP packet */
    printk (("*********************IP HEADER******************\n"))
    printk (("IP version/IPhdr length: %2.2X TOS: %2.2X\n", add[14] , add[15]))
    printk (("IP total length: %2.2X %2.2X, decimal %d\n", add[16], add[17], length = (add[16]<<8 | add[17] )))
    printk (("IP identification: %2.2X %2.2X, 3-bit flags and offset %2.2X %2.2X\n",
            add[18],add[19], add[20], add[21]))
    printk (("IP TTL: %2.2X, protocol: %2.2X, checksum: %2.2X %2.2X \n",
             add[22],add[23],add[24],add[25]))
    printk (("IP packet type: %2.2X code %2.2X\n", add[34],add[35]))
    printk (("Source IP address: "))

    for ( i=0; i< 3 ; i++) {
      printk (("%u.", add[26 + i]))
    }
    printk (("%u\n", add[29]))
    printk (("Destination IP address: "))

    for ( i=0; i< 3 ; i++) {
      printk (("%u.", add[30 + i]))
    }
    printk (("%u\n", add[33]))
  }
}

/*
 *  print_hdr
 *
 *  Print the contents of an ethernet packet header
 *  CANNOT BE CALLED FROM ISR
 */
static  void print_hdr(
  unsigned char *add
)
{
  int i;

  printk (("print_hdr: begins\n"))
  printk (("Header Location %p\n", add))
  printk (("Dest  "))

  for (i = 0; i < 6; i++) {
    printk ((" %2.2X", add[i]))
        }
  printk (("\nSource"))

  for (i = 6; i < 12; i++) {
    printk ((" %2.2X", add[i]))
        }
  printk (("\nframe type %2.2X%2.2X\n", add[12], add[13]))
  printk (("print_hdr: completed"))
}

/*
 *  Function:   print_pkt
 *
 *  Print the contents of an ethernet packet & data
 *  CANNOT BE CALLED FROM ISR
 */
static void print_pkt(
  unsigned char *add
)
{
  int i;
  short int length;

  printk (("print_pkt: begins"))
  printk (("Data Location %p\n", add))

  if ( add[0] == 0x08 && add[1] == 0x06 ) {
    /* an ARP */
    printk (("Hardware type : %2.2X%2.2X\n", add[14],add[15]))
    printk (("Protocol type : %2.2X%2.2X\n", add[16],add[17]))
    printk (("Hardware size : %2.2X\n", add[18]))
    printk (("Protocol size : %2.2X\n", add[19]))
    printk (("op            : %2.2X%2.2X\n", add[20],add[21]))
    printk (("Sender Enet addr: "))

    for ( i=0; i< 5 ; i++) {
      printk (( "%x:", add[22 + i]))
                }
    printk (("%x\n", add[27]))
    printk (("Sender IP addr: "))

    for ( i=0; i< 3 ; i++) {
      printk (("%u.", add[28 + i]))
                }
    printk (("%u\n", add[31]))
    printk (("Target Enet addr: "))

    for ( i=0; i< 5 ; i++) {
      printk (( "%x:", add[32 + i]))
    }
    printk (("%x\n", add[37]))
    printk (("Target IP addr: "))

    for ( i=0; i< 3 ; i++) {
      printk (( "%u.", add[38 + i]))
    }
    printk (("%u\n", add[41]))
  }

  if ( add[0] == 0x08 && add[1] == 0x00 ) {
    /* an IP packet */
    printk (("*********************IP HEADER******************\n"))
    printk (("IP version/IPhdr length: %2.2X TOS: %2.2X\n", add[14] , add[15]))
    printk (("IP total length: %2.2X %2.2X, decimal %d\n", add[16], add[17], length = (add[16]<<8 | add[17] )))
    printk (("IP identification: %2.2X %2.2X, 3-bit flags and offset %2.2X %2.2X\n",
            add[18],add[19], add[20], add[21]))
    printk (("IP TTL: %2.2X, protocol: %2.2X, checksum: %2.2X %2.2X \n",
            add[22],add[23],add[24],add[25]))
    printk (("IP packet type: %2.2X code %2.2X\n", add[34],add[35]))
    printk (("Source IP address: "))

    for ( i=0; i< 3 ; i++) {
      printk(( "%u.", add[26 + i]))
                }
    printk(("%u\n", add[29]))
    printk(("Destination IP address: "))

    for ( i=0; i< 3 ; i++) {
      printk(( "%u.", add[30 + i]))
    }
    printk(("%u\n", add[33]))
    printk(("********************IP Packet Data*******************\n"))
    length -=20;

    for ( i=0; i < length ; i++) {
      printk(("0x%2.2x ", add[34+i]))
    }
    printk(("\n"))
    printk(("ICMP checksum: %2.2x %2.2x\n", add[36], add[37]))
    printk(("ICMP identifier: %2.2x %2.2x\n", add[38], add[39]))
    printk(("ICMP sequence nbr: %2.2x %2.2x\n", add[40], add[41]))
    printk(("print_pkt: completed"))
  }
}

/*
 *  print_echo
 *
 *  Print the contents of an echo packet
 *  CANNOT BE CALLED FROM ISR
 */
static void print_echo(
  unsigned char *add
)
{
  int i;
  short int length;

  printk (("print_echo: begins"))

  if ( add[12] == 0x08 && add[13] == 0x00 ) {
    /* an IP packet */
    printk (("Packet Location %p\n", add))
    printk (("Dest  "))

    for (i = 0; i < 6; i++) {
      printk ((" %2.2X", add[i]))
                }
    printk (("\n"))
    printk (("Source"))

    for (i = 6; i < 12; i++) {
      printk ((" %2.2X", add[i]))
                }
    printk (("\n"))
    printk (("frame type %2.2X%2.2X\n", add[12], add[13]))

    printk (("*********************IP HEADER******************\n"))
    printk (("IP version/IPhdr length: %2.2X TOS: %2.2X\n", add[14] , add[15]))
    printk (("IP total length: %2.2X %2.2X, decimal %d\n", add[16], add[17], length = (add[16]<<8 | add[17] )))
    printk (("IP identification: %2.2X %2.2X, 3-bit flags and offset %2.2X %2.2X\n",
            add[18],add[19], add[20], add[21]))
    printk (("IP TTL: %2.2X, protocol: %2.2X, checksum: %2.2X %2.2X \n",
            add[22],add[23],add[24],add[25]))
    printk (("IP packet type: %2.2X code %2.2X\n", add[34],add[35]))
    printk (("Source IP address: "))

    for ( i=0; i< 3 ; i++) {
      printk (("%u.", add[26 + i]))
                }
    printk (("%u\n", add[29]))
    printk (("Destination IP address: "))

    for ( i=0; i< 3 ; i++) {
      printk (("%u.", add[30 + i]))
    }
    printk (("%u\n", add[33]))
    printk(("********************IP Packet Data*******************\n"))
    length -=20;

    for ( i=0; i < length ; i++) {
      printk(("0x%2.2x ", add[34+i]))
                }
    printk(("\n"))
    printk(("ICMP checksum: %2.2x %2.2x\n", add[36], add[37]))
    printk(("ICMP identifier: %2.2x %2.2x\n", add[38], add[39]))
    printk(("ICMP sequence nbr: %2.2x %2.2x\n", add[40], add[41]))
    printk(("print_echo: completed"))
  }
}

#endif
