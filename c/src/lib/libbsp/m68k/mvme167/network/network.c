/* uti596.c: An 82596 ethernet driver for rtems-bsd.
 *
 *  $Id$
 */

#define KERNEL

/* 
 *  Selectively define to debug the network driver. If you define any of these
 *  you must run with polled console I/O.
 */

#define DBG_596
#define DBG_596_RFA
#define DBG_START
#define DBG_INIT
#define DBG_INIT_2
/*#define DBG_INIT_3*/
#define DBG_RESET
#define DBG_VERSION
#define DBG_ISR
#define DBG_RAW_ISR
#define DBG_596_RFD
#define DBG_FR
#define DBG_RAW
#define DBG_ATTACH
#define DBG_POLLED_CMD
#define DBG_ADD
#define DBG_SUPPLY_FD


/*
 * Default number of buffer descriptors and buffer sizes.
 */
#define RX_BUF_COUNT     15
#define TX_BUF_COUNT     4
#define TX_BD_PER_BUF    4

#define RBUF_SIZE       1520

#define UTI_596_ETH_MIN_SIZE 60

/*
 * RTEMS events
 */
#define INTERRUPT_EVENT         RTEMS_EVENT_1
#define START_TRANSMIT_EVENT    RTEMS_EVENT_2
#define NIC_RESET_EVENT         RTEMS_EVENT_3


#include <bsp.h>
#include <stdio.h>

#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>
#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>

#include "uti596.h"
#include "netexterns.h"

#if CD2401_POLLED_IO == 1
	#define printk(arglist) printk arglist;
#else
	#define printk(arglist)
#endif

#define  UTI_596_ASSERT( condition, str )  if (!( condition ) ) { printk((str)) }

int	count_rx = 0;
i596_rfd *pISR_Rfd;

static uti596_softc_ uti596_softc;

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
        0x0E,   /* Byte 8: not promisc, disable bcast, tx no crs, crc inserted 32bit, 802.3 framing */
        0x08,   /* Byte 9: collision detect */
        0x40,   /* Byte 10: minimum frame length */
        0xfb,   /* Byte 11: tried C8 same as byte 1 in bits 6-7, else ignored*/
        0x00,   /* Byte 12: disable full duplex */
        0x3f    /* Byte 13: no multi IA, backoff enabled */
};


/* Required RTEMS network driver functions and tasks (reset daemon extra) */

static void uti596_start( struct ifnet * );
void uti596_init( void * );
void uti596_stop( uti596_softc_ * );
void uti596_txDaemon( void * );
void uti596_rxDaemon( void * );
void uti596_resetDaemon( void * );
rtems_isr uti596_DynamicInterruptHandler (rtems_vector_number );
static int uti596_ioctl( struct ifnet *, int, caddr_t );
void uti596_stats( uti596_softc_ * );

/* Local Routines */

void uti596_initialize_hardware( uti596_softc_ * );
void uti596_initMem( uti596_softc_ * );
int uti596_initRFA( int );
void uti596addPolledCmd( i596_cmd * );
void uti596Diagnose( int );
i596_rfd * uti596dequeue( i596_rfd ** );
void uti596reset( void );
void uti596_reset_hardware( uti596_softc_ *);
void uti596clearListStatus( i596_rfd * );
void send_packet( struct ifnet *, struct mbuf * );
void uti596addCmd( i596_cmd *pCmd );
void uti596supplyFD( i596_rfd * );
void uti596append( i596_rfd ** , i596_rfd * );

void printk_time( void );
void dump_scb( void );

#ifdef DBG_INIT_3
static void print_eth  ( unsigned char * );
static void print_hdr  ( unsigned char * );
static void print_pkt  ( unsigned char * );
static void print_echo ( unsigned char * );
#endif

int uti596dump( char * );
void dumpQ( void );
void show_buffers( void );
void show_queues( void );

/* Helper function for reading/writing big endian memory structures */
unsigned long word_swap(unsigned long);


/* Waits for the command word to clear.  The command word is cleared AFTER the interrupt is
 * generated. This allows the CPU to issue the next command
 */
#define  UTI_WAIT_COMMAND_ACCEPTED(duration,function)												\
{		rtems_interval ticks_per_second, start_ticks, end_ticks;								\
		rtems_clock_get(RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticks_per_second);		\
		rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &start_ticks);				\
		end_ticks = start_ticks + ticks_per_second;															\
		do {																																		\
			if (uti596_softc.scb.command == 0)																		\
				break;																															\
			else {																																\
				rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &start_ticks);		\
			}																																			\
		} while (start_ticks <= end_ticks);																			\
		if ((uti596_softc.scb.command != 0) || (start_ticks > end_ticks))			\
			printf("%s: i82596 timed out with status %x, cmd %x.\n", function, 		\
                  uti596_softc.scb.status,  uti596_softc.scb.command);		\
}

#define  UTI_WAIT_TICKS												\
{		rtems_interval ticks_per_second, start_ticks, end_ticks;								\
		rtems_clock_get(RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticks_per_second);		\
		rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &start_ticks);				\
		end_ticks = start_ticks + ticks_per_second;															\
		do {																																		\
				rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &start_ticks);		\
		} while (start_ticks <= end_ticks);																			\
}
/************************************************************************/


 /***********************************************************************
  *  Function:   uti596_attach
  *
  *  Description:
  *              Configure the driver, and connect to the network stack
  *
  *  Algorithm:
  *
  *              Check parameters in the ifconfig structure, and
  *                set driver parameters accordingly.
  *              initialize required rx and tx buffers
  *              link driver data structure onto device list
  *              return 1 on successful completion
  *
  ***********************************************************************/

int uti596_attach(
  struct rtems_bsdnet_ifconfig * pConfig
)
{
  uti596_softc_ *sc = &uti596_softc;    /* device dependent data structure */
  struct ifnet * ifp = &sc->arpcom.ac_if;			/* ifnet structure */

	int unitNumber;
	char *unitName;

#ifdef DBG_ATTACH
  printk(("uti596_attach: begins\n"))
#endif

  /* The NIC is not started yet */
  sc->started = 0;

  /* Indicate to ULCS that this is initialized */
  ifp->if_softc = sc;
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

  /* For now the ethernet address must be specified in the ifconfig structure,
   * else FIXME so it can be read in from BBRAM at $FFFC1F2C (6 bytes)
   * mvme167 manual p. 1-47
   */
  if ( pConfig->hardware_address) {
    memcpy (sc->arpcom.ac_enaddr, pConfig->hardware_address, ETHER_ADDR_LEN);
  }

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
  ifp->if_flags	= IFF_BROADCAST | IFF_SIMPLEX;
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
 *  					 send an event to the tx task
 *             set the if_flags
 *
 ***********************************************************************/
static void uti596_start(
  struct ifnet *ifp
)
{
  uti596_softc_ *sc = ifp->if_softc;
#ifdef DBG_INIT
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
  struct ifnet *ifp = &sc->arpcom.ac_if;

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
    sc->txDaemonTid = rtems_bsdnet_newproc ("UTtx", 2*4096, uti596_txDaemon, sc);
    sc->rxDaemonTid = rtems_bsdnet_newproc ("UTrx", 2*4096, uti596_rxDaemon, sc);
    sc->resetDaemonTid = rtems_bsdnet_newproc ("UTrt", 2*4096, uti596_resetDaemon, sc);

#ifdef DBG_INIT
    printk(("uti596_init: After attach, status of board = 0x%x\n", sc->scb.status ))
#endif
  }

  /*
   * Enable receiver
   */
#ifdef DBG_INIT
    printk(("uti596_init: enabling the reciever...\n" ))
#endif
  sc->scb.command = RX_START;
  i82596->chan_attn = 0x00000000;
  UTI_WAIT_COMMAND_ACCEPTED(4000,"uti596_init: RX_START");
  
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
 *
 *
 ***********************************************************************/

/* static */ void uti596_stop(
  uti596_softc_ *sc
)
{
	struct ifnet *ifp = &sc->arpcom.ac_if;

	ifp->if_flags &= ~IFF_RUNNING;
  sc->started = 0;

#ifdef DBG_596
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
  struct ifnet *ifp = &sc->arpcom.ac_if;
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
  struct ifnet *ifp = &sc->arpcom.ac_if;
  struct mbuf *m;

  i596_rfd *pRfd;
  ISR_Level level;
  int tid;
  rtems_event_set events;
  struct ether_header *eh;

  int frames = 0;
#ifdef DBG_INIT_3
	int i;
#endif

#ifdef DBG_596
  printk(("uti596_rxDaemon: begin\n"))
  printk(("&scb = %p, pRfd = %p\n", &sc->scb,sc->scb.pRfd))
#endif

  rtems_task_ident (0, 0, &tid);

#ifdef DBG_596
  printk(("uti596_rxDaemon: RX tid = 0x%x\n", tid))
#endif

    for(;;) {
      /*
       * Wait for packet.
       */
#ifdef DBG_596
      printk(("uti596_rxDaemon: Receiver sleeps\n"))
#endif

      rtems_bsdnet_event_receive (INTERRUPT_EVENT,
                                  RTEMS_WAIT|RTEMS_EVENT_ANY,
                                  RTEMS_NO_TIMEOUT,
                                  &events);

#ifdef DBG_596
      printk(("uti596_rxDaemon: Receiver wakes\n"))
#endif
      /*
       * While received frames are available. Note that the frame may be
       * a fragment, so it is NOT a complete packet.
       */
      pRfd = uti596dequeue( &sc->pInboundFrameQueue);
      while ( pRfd &&
              pRfd != I596_NULL &&
              pRfd -> stat & STAT_C )
        {

#ifdef DBG_INIT_3
          printk(("\nuti596_rxDaemon: Received packet:\n"))
          print_eth( pRfd->data);
#endif
          if ( pRfd->stat & STAT_OK) {					/*   a good frame */
            int pkt_len = pRfd->count & 0x3fff; /* the actual # of bytes received */

#ifdef DBG_596
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
            memcpy(m->m_data,
            			 pRfd->data,
                   pkt_len);

            m->m_len = m->m_pkthdr.len = pkt_len - sizeof(struct ether_header) - 4;

            /* move the header to an mbuf */
            eh = mtod (m, struct ether_header *);
            m->m_data += sizeof(struct ether_header);

#ifdef DBG_596
            printk(("uti596_rxDaemon: m->m_ext: %p pRfd -> data: %p\n",
                    m->m_ext, pRfd->data))
#endif
#ifdef DBG_INIT_3
            printk(("uti596_rxDaemon: mbuf contains:\n"))
            print_eth( (char *) (((int)m->m_data)-sizeof(struct ether_header)));
            for ( i = 0; i<20; i++) {
              printk(("."))
            }
#endif
            ether_input (ifp, eh, m);

          } /* end if STAT_OK */

          else {
            /*
             * A bad frame is present: Note that this could be the last RFD!
             */
#ifdef DBG_596
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

#ifdef DBG_SUPPLY_FD
          printk(("uti596_rxDaemon: Supply FD Starting\n"))
#endif
          _ISR_Disable(level);
          uti596supplyFD ( pRfd );   /* Return RFD to RFA. */
          _ISR_Enable(level);
#ifdef DBG_SUPPLY_FD
          printk(("uti596_rxDaemon: Supply FD Complete\n"))
#endif
          pRfd = uti596dequeue( &sc->pInboundFrameQueue); /* grab next frame */

        } /* end while */
    } /* end for(;;) */

#ifdef DBG_596
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
    uti596reset();
    rtems_bsdnet_semaphore_release ();
  }
}


 /***********************************************************************
  *  Function:   uti596_DynamicInterruptHandler
  *
  *  Description:
  *             This is the interrupt handler for the uti596 board
  *
  *  Algorithm:
  *
  ***********************************************************************/

/* static */ rtems_isr uti596_DynamicInterruptHandler(
  rtems_vector_number irq
)
{
#ifdef DEBUG_ISR
  printk(("uti596_DynamicInterruptHandler: begins"))
#endif

 UTI_WAIT_COMMAND_ACCEPTED(20000,"****ERROR:on ISR entry");

 scbStatus = uti596_softc.scb.status & 0xf000;

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
    i82596->chan_attn = 0x00000000;

    if( uti596_softc.resetDone ) {
      /* stack is attached */
      UTI_WAIT_COMMAND_ACCEPTED(20000,"****ERROR:ACK");
    }
    else {
      /* printk(("***INFO: ACK'd w/o processing. status = %x\n", scbStatus)) */
      return;
    }
  }
  else {
    printk(("\n***ERROR: Spurious interrupt. Resetting...\n"))
    uti596_softc.nic_reset = 1;
  }


  if ( (scbStatus & SCB_STAT_CX) && !(scbStatus & SCB_STAT_CNA) ){
    printk_time();
    printk(("\n*****ERROR: Command Complete, and CNA available: 0x%x\nResetting...", scbStatus))
    uti596_softc.nic_reset = 1;
    return;
  }

  if ( !(scbStatus & SCB_STAT_CX) && (scbStatus & SCB_STAT_CNA) ) {
    printk_time();
    printk(("\n*****ERROR: CNA, NO CX:0x%x\nResetting...",scbStatus))
    uti596_softc.nic_reset = 1;
    return;
  }

  if ( scbStatus & SCB_CUS_SUSPENDED ) {
    printk_time();
    printk(("\n*****ERROR: Command unit suspended!:0x%x\nResetting...",scbStatus))
    uti596_softc.nic_reset = 1;
    return;
  }

  if ( scbStatus & RU_SUSPENDED  ) {
    printk_time();
    printk(("\n*****ERROR: Receive unit suspended!:0x%x\nResetting...",scbStatus))
    uti596_softc.nic_reset = 1;
    return;
  }

  if ( scbStatus & SCB_STAT_RNR ) {
    printk_time();
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
  
#ifdef DBG_FR
    printk(("uti596_DynamicInterruptHandler: Frame received\n"))
#endif
    if ( uti596_softc.pBeginRFA == I596_NULL ||
         !( uti596_softc.pBeginRFA -> stat & STAT_C)) {
     dump_scb();
     uti596_softc.nic_reset = 1;
    }
    else {
      while ( uti596_softc.pBeginRFA != I596_NULL &&
           ( uti596_softc.pBeginRFA -> stat & STAT_C)) {

#ifdef DBG_ISR
        printk(("uti596_DynamicInterruptHandler: pBeginRFA != NULL\n"))
#endif
        count_rx ++;
        if ( count_rx > 1) {
          printk(("****WARNING: Received 2 frames on 1 interrupt \n"))
				}
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
        uti596append( &uti596_softc.pInboundFrameQueue , uti596_softc.pBeginRFA );

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
#ifdef DBG_RAW_ISR
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

#ifdef DBG_RAW_ISR
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

          /*      printk(("****INFO:Configure OK\n")) */
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
#ifdef DBG_RAW_ISR
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
        printk_time();
        printk(("****INFO: Request board reset ( tx )\n"))
        uti596_softc.nic_reset = 1;
        if ( uti596_softc.txDaemonTid) {
          /* Ensure that a transmitter is present */
          sc = rtems_event_send (uti596_softc.txDaemonTid,
                                 INTERRUPT_EVENT);
          if ( sc != RTEMS_SUCCESSFUL ) {
            printk(("****ERROR:Could NOT send event to tid 0x%x : %s\n",uti596_softc.txDaemonTid, rtems_status_text (sc) ))
					}
#ifdef DBG_RAW_ISR
          else {
            printk(("uti596_DynamicInterruptHandler: ****INFO:Tx wake: %#x\n",uti596_softc.txDaemonTid))
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
      printk(("uti596_DynamicInterruptHandler: INFO:RNR: status %#x \n", uti596_softc.scb.status ))
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
#ifdef DEBUG_RFA
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
          uti596append( &uti596_softc.pSavedRfdQueue, uti596_softc.pLastUnkRFD );
          uti596_softc.savedCount++;
          uti596_softc.pEndSavedQueue = uti596_softc.pLastUnkRFD;
          uti596_softc.countRFD--;                    /* It was not in the RFA */
         /*
          * The Begin pointer CAN advance this far. We must resynch the CPU side
          * with the chip.
          */
          if ( uti596_softc.pBeginRFA == uti596_softc.pLastUnkRFD ) {
#ifdef DEBUG_RFA
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
#ifdef DEBUG_RFA
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
#ifdef DEBUG_RFA
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
#ifdef DEBUG_ISR
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

#ifdef DBG_596_RFD
      printk(("uti596_DynamicInterruptHandler: The list starts here %p\n",uti596_softc.pBeginRFA ))
#endif

      if ( uti596_softc.countRFD > 1) {
        printk_time();
        printk(("****INFO: pBeginRFA -> stat = 0x%x\n",uti596_softc.pBeginRFA -> stat))
        printk(("****INFO: pBeginRFA -> cmd = 0x%x\n",uti596_softc.pBeginRFA -> cmd))
        uti596_softc.pBeginRFA -> stat = 0;
        UTI_596_ASSERT(uti596_softc.scb.command == 0, "****ERROR:scb command must be zero\n")
        uti596_softc.scb.pRfd = uti596_softc.pBeginRFA;
        uti596_softc.scb.Rfd_val = word_swap((unsigned long)uti596_softc.pBeginRFA);
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
 *  Algorithm:
 *
 ***********************************************************************/
 
static int uti596_ioctl(
  struct ifnet *ifp,
  int command, caddr_t data
)
{
  uti596_softc_ *sc = ifp->if_softc;
  int error = 0;

#ifdef DBG_INIT
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
          uti596_init (sc);
          break;

        case IFF_UP | IFF_RUNNING:
          printk(("IFF_UP and RUNNING\n"))
          uti596_stop (sc);
          uti596_init (sc);
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
  printf("CPU Reports:\n");
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

  printf("  NIC reports\n");

  dump_scb();
}


/***********************************************************************
 *************************** LOCAL ROUTINES ****************************
 ***********************************************************************/


/***********************************************************************
 *  Function:   void uti596_initialize_hardware
 *
 *  Description:
 ***********************************************************************/
void uti596_initialize_hardware(
  uti596_softc_ *sc
)
{
  int boguscnt = 1000;
  rtems_isr_entry dummy;

  printk(("uti596_initialize_hardware: begins\n"))

  pccchip2->LANC_berr_ctl	= 0x40;		 /* Enable snooping */
  
  /* reset the board  */
  i82596->port_lower = 0x0000; 	/* Each Port access must consist of two 16-bit writes */
  i82596->port_upper = 0x0000;

  /* allocate enough memory for the Scp block to be aligned on 16 bit boundary */
  uti596_softc.pScp = (i596_scp *) calloc(1,sizeof(struct i596_scp) + 0xf);
  					
#ifdef DBG_INIT
  printk(("uti596_initialize_hardware: Scp address initially %p\n", sc->pScp))
#endif
  
  /* align the block */
  sc->pScp = (i596_scp *)
    ((((int)uti596_softc.pScp) + 0xf) & 0xfffffff0);

#ifdef DBG_INIT
  printk(("uti596_initialize_hardware: change scp address to : %p\n",sc->pScp))
#endif

  /* change the scp address */
#ifdef DBG_INIT
  printk(("uti596_initialize_hardware: Change the SCP address\n"))
#endif

  /* reset the board  */
  i82596->port_lower = 0x0000;
  i82596->port_upper = 0x0000;
  
  /* supply the Scp address
   * Lower Command Word D31-D16; Upper Command Word D15-D0
   */
  i82596->port_lower = (((int)sc->pScp) & 0xfff0) | 0x0002;
  i82596->port_upper = (((int)sc->pScp) >> 16 ) & 0xffff;

  
  /* write the SYSBUS: interrupt pin active high, LOCK disabled,
   * internal triggering, linear mode
   */
  sc->pScp->sysbus = word_swap(0x00540000);
  
  /* provide the iscp to the scp, keep a pointer for our use */
  sc->pScp->iscp_val = word_swap((unsigned long)&sc->iscp);
  sc->pScp->iscp = &sc->iscp;

  /* provide the scb to the iscp, keep a pointer for our use */
  sc->iscp.scb_val = word_swap((unsigned long)&sc->scb);
  sc->iscp.scb = &sc->scb;

  /* set the busy flag in the iscp */
  sc->iscp.stat = word_swap(0x00000001);

  /* the command block list (CBL) is empty */
  sc->scb.Cmd_val = (unsigned long) I596_NULL;	/* all 1's */
  sc->pCmdHead = sc->scb.pCmd = I596_NULL;			/* all 1's */

#ifdef DBG_596
  printk(("uti596_initialize_hardware: Starting i82596.\n"))
#endif

  /* Issue CA: pass the scb address to the 596 */
  i82596->chan_attn = 0x00000000;

  UTI_WAIT_TICKS

  /* Wait for the 596 to read the SCB address and clear 'stat' */
  while (sc->iscp.stat) {
    if (--boguscnt == 0) {
        printk(("initialize_hardware: timed out with status %4.4lx\n",
               sc->iscp.stat ))
        break;
    }
  } /* end while */

  /* clear the scb command word */
  sc->scb.command = 0;
  
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

/***********************************************************************
 *  Function:   uti596_initMem
 *
 *  Description:
 *             creates the necessary descriptors for the
 *             uti596 board, hooks the interrupt, and starts the board.
 *             Assumes that interrupts are hooked.
 *
 *  Algorithm:
 *
 ***********************************************************************/

void uti596_initMem(
  uti596_softc_ * sc
)
{
    int i,count;
    i596_tbd *pTbd, *pPrev;

#ifdef DBG_INIT
  printk(("uti596_initMem: begins\n"))
#endif

    sc->resetDone = 0;

    /*
     * Set up receive frame area (RFA)
     */
#ifdef DBG_INIT
    printk(("uti596_initMem: Initializing the RFA...\n"))
#endif
    i = uti596_initRFA( sc->rxBdCount );
    if ( i < sc->rxBdCount ) {
      printk(("init_rfd: only able to allocate %d receive frame descriptors\n", i))
		}
		
		sc->scb.Rfd_val = word_swap((unsigned long)sc->pBeginRFA);
    sc->scb.pRfd =  sc->pBeginRFA;

    /*
     * Diagnose the health of the board
     *   send the CU a diagnostic command
     */
#ifdef DBG_INIT
    printk(("uti596_initMem: Running diagnostics...\n"))
#endif
    uti596Diagnose(1);

    /*
     * Configure the 82596
     *   send the CU a configure command with our initial setup
     */
#ifdef DBG_INIT
    printk(("uti596_initMem: Configuring...\n"))
#endif
    sc->set_conf.cmd.command = CmdConfigure;
    memcpy (sc->set_conf.data, uti596initSetup, 14);
    uti596addPolledCmd( (i596_cmd *) &sc->set_conf);

    /* Poll for successful command completion */
    count = 2000;
    while( !( sc->set_conf.cmd.status & STAT_C ) && --count ) {
      printk(("."))
    }

    if ( count ) {
      printk(("Configure OK, count = %d\n",count))
    }
    else {
      printk(("***Configure failed\n"))
		}
    /*
     * Set up the Internet Address
     *   send the CU an IA-setup command
     */
#ifdef DBG_INIT
    printk(("uti596_initMem: Setting Address...\n"))
#endif
    sc->set_add.cmd.command = CmdSASetup;
    for ( i=0; i<6; i++) {
      sc->set_add.data[i]=sc->arpcom.ac_enaddr[i];
#ifdef DBG_INIT
      printk(("uti596_initMem: copying byte %d: 0x%x\n", i, sc->set_add.data[i]))
#endif
    }
    sc->cmdOk = 0;
    uti596addPolledCmd((i596_cmd *)&sc->set_add);

		/* Poll for successful command completion */
    count = 2000;
    while( !(sc->set_add.cmd.status & STAT_C ) && --count) {
      printk(("."))
    }

    if ( count ) {
      printk(("Set Address OK, count= %d\n",count))
    }
    else {
      printk(("Set Address Failed\n"))
		}
#ifdef DBG_INIT
    printk(( "uti596_initMem: After initialization, status and command: 0x%x, 0x%x\n",
            sc->scb.status, sc->scb.status))
#endif

    /*
     *Initialize transmit buffer descriptors
     */
#ifdef DBG_INIT
    printk(( "uti596_initMem:Initializing TBDs...\n"))
#endif
    sc->pLastUnkRFD						= I596_NULL;
    sc->pTxCmd								= (i596_tx *) calloc (1,sizeof (struct i596_tx) );
    sc->pTbd									= (i596_tbd *) calloc (1,sizeof (struct i596_tbd) );
   	sc->pTxCmd->pTbd					= (i596_tbd *) word_swap ((unsigned long) sc->pTbd);
    sc->pTxCmd->cmd.command		= CMD_FLEX|CmdTx;
    sc->pTxCmd->pad						= 0;
    sc->pTxCmd->count					= 0; /* all bytes are in list of TBD's */

    pPrev = pTbd = sc->pTbd;

		/* Allocate a linked list of tbd's each with it's 'next' field written
		 * with upper and lower words swapped (for big endian), and mark the end.
		 */
    for ( i=0; i<sc->txBdCount; i++) {
      pTbd = (i596_tbd *) calloc (1,sizeof (struct i596_tbd) );
      pPrev->next = (i596_tbd *) word_swap ((unsigned long) pTbd);
      pPrev = pTbd;
		}
    pTbd->next = I596_NULL;

		/* Padding used to fill short tx frames */
    memset ( &sc->zeroes, 0, 64);

#ifdef DBG_596
    printk(( "uti596_initMem: After receiver start, status and command: 0x%x, 0x%x\n",
            sc->scb.status, sc->scb.status))
#endif

#ifdef DBG_596
    printk(("uti596_initMem allows ISR's\n"))
#endif
    sc->resetDone = 1; /* now need ISR  */
}

/***********************************************************************
 *  Function:   uti596initRFA(int num)
 *
 *  Description:
 *              attempts to allocate and initialize ( chain together )
 *              the requested number of FD's
 *
 *  Algorithm:
 *
 ***********************************************************************/

int uti596_initRFA( int num )
{
    i596_rfd *pRfd;
    int i = 0;

#ifdef DBG_596
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
    printk(("First Rfd allocated @: %p\n",
             uti596_softc.pBeginRFA))
    }

		/* Create remaining RFAs */
    for (i = 1; i < num; i++) {
      pRfd = (i596_rfd *) calloc (1, sizeof (struct i596_rfd) );
      if ( pRfd != NULL ) {
        uti596_softc.countRFD++;																/* update count */
        uti596_softc.pEndRFA->next = 
          (i596_rfd *) word_swap ((unsigned long) pRfd); /* write the link */
        uti596_softc.pEndRFA = pRfd; 														/* move the end */
#ifdef DBG_596_RFA
        printk(("uti596_initRFA: Allocated RFD @ %p\n", pRfd))
#endif
      }
      else {
        printk(("Can't allocate all buffers: only %d allocated\n", i))
        break;
      }
    } /* end for */

    uti596_softc.pEndRFA->next = I596_NULL;
    UTI_596_ASSERT(uti596_softc.countRFD == RX_BUF_COUNT,"INIT:WRONG RFD COUNT\n" )

#ifdef DBG_596_RFA
    printk (("uti596_initRFA: Head of RFA is buffer %p \n\
    					uti596_initRFA: End of RFA is buffer %p \n",
              uti596_softc.pBeginRFA,
              uti596_softc.pEndRFA ))
#endif
    /* Initialize the RFD's */
    for ( pRfd = uti596_softc.pBeginRFA;
          pRfd != I596_NULL;
          pRfd = (i596_rfd *) word_swap ((unsigned long)pRfd->next) ) {

      pRfd->cmd		= 0x0000;
      pRfd->stat  = 0x0000;
      pRfd->pRbd 	= I596_NULL;
      pRfd->count = 0;  /* number of bytes in buffer: usually less than size */
      pRfd->size 	= 1532;      /* was 1532;  buffer size ( All RBD ) */
      if ( pRfd -> data == NULL ) {
        printk(("Can't allocate the RFD data buffer\n"))
      }
    }

    /* mark the last RFD as the last one in the RDL */
    uti596_softc.pEndRFA -> cmd = CMD_EOL;

    uti596_softc.pSavedRfdQueue =
      uti596_softc.pEndSavedQueue = I596_NULL;   /* initially empty */

    uti596_softc.savedCount = 0;

    uti596_softc.nop.cmd.command = CmdNOp; /* initialize the nop command */

    return (i); /* the number of allocated buffers */
}

 /***********************************************************************
  *  Function:   uti596addPolledCmd
  *
  *  Description:
  *             This routine issues a single command then polls for it's
  *             completion.
  *
  *  Algorithm:
  *            Give the command to the driver. ( CUC_START is ALWAYS required )
  *            Poll for completion.
  *
  ***********************************************************************/

void uti596addPolledCmd(
  i596_cmd *pCmd
)
{

 #ifdef DBG_596
     printk(("uti596addPolledCmd: Adding command 0x%x\n", pCmd -> command ))
 #endif

#ifdef DBG_POLLED_CMD

     switch ( pCmd -> command & 0x7 ) {    /* check bottom 3 bits */
     case CmdConfigure:
       printk(("uti596addPolledCmd: Configure Command 0x%x\n", pCmd->command))
       break;
     case CmdSASetup:
       printk(("uti596addPolledCmd: Set CMDress Command 0x%x\n", pCmd->command))
       break;
     case CmdMulticastList:
       printk(("uti596addPolledCmd: Multi-cast list 0x%x\n", pCmd->command))
       break;
     case CmdNOp:
       printk(("uti596addPolledCmd: NO op 0x%x\n", pCmd->command))
       break;
     case CmdTDR:
       printk(("uti596addPolledCmd: TDR 0x%x\n", pCmd->command))
       break;
     case CmdDump:
       printk(("uti596addPolledCmd: Dump 0x%x\n", pCmd->command))
       break;
     case CmdDiagnose:
       printk(("uti596addPolledCmd: Diagnose 0x%x\n", pCmd->command))
       break;
     case CmdTx:
       break;
     default:
       printk(("PolledCMD: ****Unknown Command encountered 0x%x\n", pCmd->command))
       break;
     } /* end switch */

#endif

     pCmd->status = 0;
     pCmd->command |=  CMD_EOL ; /* only command in list*/

     pCmd->next = I596_NULL;

     UTI_WAIT_COMMAND_ACCEPTED(10000,"Add Polled command: wait prev");

     uti596_softc.pCmdHead = uti596_softc.pCmdTail = uti596_softc.scb.pCmd = pCmd;
     uti596_softc.scb.Cmd_val = word_swap((unsigned long)pCmd);
     uti596_softc.scb.command = CUC_START;
     i82596->chan_attn = 0x00000000;

     UTI_WAIT_COMMAND_ACCEPTED(10000,"Add Polled command: start");
     uti596_softc.pCmdHead = uti596_softc.pCmdTail = uti596_softc.scb.pCmd = I596_NULL;
		 uti596_softc.scb.Cmd_val = (unsigned long) I596_NULL;

#ifdef DBG_POLLED_CMD
     printk(("uti596addPolledCmd: Scb status & command 0x%x 0x%x\n",
            uti596_softc.scb.status,
            uti596_softc.scb.command ))
#endif
}


/***********************************************************************
 *  Function:   void uti596Diagnose
 *
 *  Description:
 *              
 ***********************************************************************/
void uti596Diagnose(
  int verbose
)
{
  i596_cmd diagnose;
  int count=10000;

  diagnose.command = CmdDiagnose;
  diagnose.status = 0;
  uti596addPolledCmd(&diagnose);
  while( !( diagnose.status & STAT_C ) && count ) {
    if(verbose) {
      printk(("."))
    }
    count --;
  }
  if(verbose) {
    printk(("Status diagnostic: 0xa000 is a success ... 0x%2.2x\n", diagnose.status))
  }
}

/***********************************************************************
 *  Function:   void uti596dequeue
 *
 *  Description:
 *              removes an RFD from the received frame queue,
 *
 *  Algorithm:
 *
 ***********************************************************************/
i596_rfd * uti596dequeue(
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

  pRfd = *ppQ;            												/* The dequeued buffer           */
  *ppQ = (i596_rfd *) \
          word_swap ((unsigned long) pRfd->next); /* advance the queue pointer     */
  pRfd->next = I596_NULL;													/* unlink the rfd being returned */

  _ISR_Enable(level);
  return pRfd;
}

/***********************************************************************
 *  Function:   void uti596reset
 *
 *  Description:
 ***********************************************************************/
void uti596reset( void )
{
   int i,count;
   uti596_softc_ *sc = &uti596_softc;
   /*   i596_rfd * pRfd; */

#ifdef DBG_RESET
     printk(("uti596reset: begin\n"))
#endif

  sc->resetDone = 0;
  UTI_WAIT_COMMAND_ACCEPTED(10000, "reset: wait for previous command complete");
  uti596_reset_hardware(&uti596_softc); /* reset the ethernet hardware. must re-config */

#ifdef DBG_RESET
  uti596Diagnose(1);
#endif

  sc->set_conf.cmd.command = CmdConfigure;
  memcpy (sc->set_conf.data, uti596initSetup, 14);
  uti596addPolledCmd( (i596_cmd *) &sc->set_conf);

  /* POLL */
  count = 2000;
  while( !( sc->set_conf.cmd.status & STAT_C ) && --count ) {
    printk(("."))
  }

  if ( count ) {
    printk(("Configure OK, count = %d\n",count))
  }
  else {
    printk(("reset: Configure failed\n"))
  }

  /*
   * Create the IA setup command
   */

#ifdef DBG_RESET
  printk(("uti596reset: Setting Address\n"))
#endif
  sc->set_add.cmd.command = CmdSASetup;
  for ( i=0; i<6; i++) {
    sc->set_add.data[i]=sc->arpcom.ac_enaddr[i];
  }
  sc->cmdOk = 0;
  uti596addPolledCmd((i596_cmd *)&sc->set_add);

  count = 2000;
  while( !(sc->set_add.cmd.status & STAT_C ) && --count) {
    printk(("."))
	}
  if ( count ) {
    printk(("Reset Set Address OK, count= %d\n",count))
  }
  else {
    printk(("Reset Set Address Failed\n"))
  }

  sc->pCmdHead = sc->pCmdTail = sc->scb.pCmd = I596_NULL;

#ifdef DBG_RESET
  printk(( "uti596reset: After reset, status and command: 0x%x, 0x%x\n",
          sc->scb.status, sc->scb.status))
#endif

  /* restore the RFA */

  /* dumpQ(); */

  if ( sc->pLastUnkRFD != I596_NULL ) {
    sc-> pEndRFA =  sc->pLastUnkRFD; /* The end position can be updated */
    sc-> pLastUnkRFD = I596_NULL;
  }

  sc->pEndRFA->next = sc->pSavedRfdQueue;
  if ( sc->pSavedRfdQueue != I596_NULL ) {
    sc->pEndRFA = sc->pEndSavedQueue;
    sc->pSavedRfdQueue = sc->pEndSavedQueue = I596_NULL;
    sc -> countRFD = sc->rxBdCount ;
  }

  sc->scb.pRfd =  sc->pBeginRFA; /* readdress the head of the RFA in the SCB */
  sc->scb.Rfd_val = word_swap((unsigned long)sc->pBeginRFA);

  uti596clearListStatus(sc->pBeginRFA );

  /*  dumpQ(); */

  printk(("Reset:Starting NIC\n"))
  sc->scb.command = RX_START;
  sc->started = 1;               /* we assume that the start works */
  sc->resetDone = 1;
  i82596->chan_attn = 0x00000000;
  UTI_WAIT_COMMAND_ACCEPTED(4000, "reset");
  printk(("Reset:Start complete \n"))
  UTI_596_ASSERT(sc->pCmdHead == I596_NULL, "Reset: CMD not cleared\n")

#ifdef DBG_RESET
  printk(("uti596reset: completed\n"))
#endif
}


/***********************************************************************
 *  Function:   void uti596_reset_hardware
 *
 *  Description:
 ***********************************************************************/
void uti596_reset_hardware(
  uti596_softc_ *sc
)
{
  int boguscnt = 1000;
  rtems_status_code status_code;
  i596_cmd *pCmd;


  printk(("uti596_reset_hardware\n"))
  pCmd = sc->pCmdHead;  /* This is a tx command for sure (99.99999%)  */

  /* reset the board  */
  i82596->port_lower = 0x0000;
  i82596->port_upper = 0x0000;

  if ( sc->pScp == NULL ) {
    printk(("Calloc scp\n"))
    uti596_softc.pScp = (i596_scp *) calloc(1,sizeof(struct i596_scp) + 0xf);
  }

#ifdef DBG_RESET
  printk(("uti596_reset_hardware: Scp address %p\n", sc->pScp))
#endif

  /* align the block */
  sc->pScp = (i596_scp *)
    ((((int)uti596_softc.pScp) + 0xf) & 0xfffffff0);

#ifdef DBG_RESET
  printk(("uti596_reset_hardware: change scp address to : %p\n",sc->pScp))
#endif

  /* change the scp address */
#ifdef DBG_RESET
  printk(("uti596_reset_hardware: Change the SCP address\n"))
#endif

  /* reset the board  */
  i82596->port_lower = 0x0000;
  i82596->port_upper = 0x0000;
  
  /* supply the Scp address
   * Lower Command Word D31-D16; Upper Command Word D15-D0
   */
  i82596->port_lower = (((int)sc->pScp) & 0xfff0) | 0x0002;
  i82596->port_upper = (((int)sc->pScp) >> 16 ) & 0xffff;

  /* write the SYSBUS: interrupt pin active high, LOCK disabled,
   * internal triggering, linear mode
   */
  sc->pScp->sysbus = word_swap(0x00540000);
  
  /* provide the iscp to the scp, keep a pointer for our use */
  sc->pScp->iscp_val = word_swap((unsigned long)&sc->iscp);
  sc->pScp->iscp = &sc->iscp;

  /* provide the scb to the iscp, keep a pointer for our use */
  sc->iscp.scb_val = word_swap((unsigned long)&sc->scb);
  sc->iscp.scb = &sc->scb;

  /* set the busy flag in the iscp */
  sc->iscp.stat = word_swap(0x00000001);

  /* the command block list (CBL) is empty */
  sc->scb.Cmd_val = (unsigned long) I596_NULL;	/* all 1's */
  sc->pCmdHead = sc->scb.pCmd = I596_NULL;			/* all 1's */

  /*
   * Wake the transmitter if needed.
   */
  if ( uti596_softc.txDaemonTid && pCmd != I596_NULL ) {
    printk(("****RESET: wakes transmitter!\n"))
    status_code = rtems_event_send (uti596_softc.txDaemonTid,
                           INTERRUPT_EVENT);

    if ( status_code != RTEMS_SUCCESSFUL ) {
      printk(("****ERROR:Could NOT send event to tid 0x%x : %s\n",
             uti596_softc.txDaemonTid, rtems_status_text (status_code) ))
    }
  }

#ifdef DBG_596
  printk(("uti596_reset_hardware: starting i82596.\n"))
#endif

  /* Pass the scb address to the 596 */
  i82596->chan_attn = 0x00000000;

  while (sc->iscp.stat)
    if (--boguscnt == 0)
      {
        printk(("reset_hardware: timed out with status %4.4lx\n",
               sc->iscp.stat ))
        break;
      }

  /* clear the command word */
  sc->scb.command = 0;

#ifdef DBG_RESET
  printk(("uti596_reset_hardware: After reset_hardware, status of board = 0x%x\n", sc->scb.status ))
#endif
}

 /***********************************************************************
  *  Function:   uti596clearListStatus
  *
  *  Description:
  *             Clear the stat fields for all rfd's
  *  Algorithm:
  *
  ***********************************************************************/

void uti596clearListStatus(
  i596_rfd *pRfd
)
{
  while ( pRfd != I596_NULL ) {
    pRfd -> stat = 0;
    pRfd = (i596_rfd *) word_swap((unsigned long)pRfd-> next);
  }
}

 /***********************************************************************
  *  Function:   send_packet
  *
  *  Description: Send a raw ethernet packet
  *
  *  Algorithm:
  *             increment some stats counters,
  *             create the transmit command,
  *             add the command to the CBL,
  *             wait for event
  *
  ***********************************************************************/

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

  /* This should never happen */
  if ( bd_count == 16 ) {
    printk(("TX ERROR:Too many mbufs in the packet!!!\n"))
    printk(("Must coalesce!\n"))
  }

  if ( length < UTI_596_ETH_MIN_SIZE ) {
    pTbd->data = (char *) word_swap ((unsigned long) sc->zeroes); /* add padding to pTbd */
    pTbd->size = UTI_596_ETH_MIN_SIZE - length; 									/* zeroes have no effect on the CRC */
  }
  else
    pTbd = pPrev; /* Don't use pTbd in the send routine */

  /*  Disconnect the packet from the list of Tbd's  */
  pRemainingTbdList = (i596_tbd *) word_swap ((unsigned long)pTbd->next);
  pTbd->next  = I596_NULL;
  pTbd->size |= UTI_596_END_OF_FRAME;

#ifdef DBG_RAW
  printk(("send_packet: RAW - Add cmd and sleep\n"))
#endif

  sc->rawsndcnt++;

#ifdef DBG_RAW
  printk(("send_packet: RAW - sending packet\n"))
#endif

  /* Sending Zero length packet: shouldn't happen */
  if (pTbd->size <= 0) return ;

#ifdef DBG_INIT_3
  printk(("\nsend_packet: Transmitter adds packet\n"))
  print_hdr    ( sc->pTxCmd->pTbd->data ); /* print the first part */
  print_pkt    ( sc->pTxCmd->pTbd->next->data ); /* print the first part */
  /*  print_echo(sc->pTxCmd->pTbd->data); */
#endif

  /* add the command to the output command queue */
  uti596addCmd ( (i596_cmd *) sc->pTxCmd );

  /* sleep until the command has been processed or Timeout encountered. */
  status= rtems_bsdnet_event_receive (INTERRUPT_EVENT,
                                      RTEMS_WAIT|RTEMS_EVENT_ANY,
                                      RTEMS_NO_TIMEOUT,
                                      &events);

  if ( status != RTEMS_SUCCESSFUL ) {
    printk(("Could not sleep %s\n", rtems_status_text(status)))
  }

#ifdef DBG_RAW
  printk(("send_packet: RAW - wake\n"))
#endif

  sc->txInterrupts++;

#ifdef DBG_INIT_3
  printk(("\nsend_packet: Transmitter issued packet\n"))
  print_hdr    ( sc->pTxCmd->pTbd -> data ); /* print the first part */
  print_pkt    ( sc->pTxCmd->pTbd ->next-> data ); /* print the first part */
#endif

  if ( sc->pTxCmd -> cmd.status & STAT_OK ) {
    sc->stats.tx_packets++;
  }
  else {
#ifdef DBG_RAW
      printk(("******send_packet: Driver Error 0x%x\n", sc->pTxCmd -> cmd.status ))
#endif
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
   * Restore the transmited buffer descriptor chain.
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
  *  Function:   uti596addCmd
  *
  *  Description:
  *             This routine adds a command onto the end of the
  *             command chain
  *
  *  Algorithm:
  *            Add the command to the end of an existing chain,
  *            or start the chain and issue a CUC_START
  *
  *
  ***********************************************************************/

/* static */ void uti596addCmd(
  i596_cmd *pCmd
)
{
     ISR_Level level;

 #ifdef DBG_596
     printk(("uti596addCmd: Adding command 0x%x\n", pCmd -> command ))
 #endif

#ifdef DBG_ADD

     switch ( pCmd -> command & 0x7 ){ /* check bottom 3 bits */
     case CmdConfigure:
       printk(("uti596addCmd: Configure Command 0x%x\n", pCmd->command))
       break;
     case CmdSASetup:
       printk(("uti596addCmd: Set Address Command 0x%x\n", pCmd->command))
       break;
     case CmdMulticastList:
       printk(("uti596addCmd: Multi-cast list 0x%x\n", pCmd->command))
       break;
     case CmdNOp:
       printk(("uti596addCmd: NO op 0x%x\n", pCmd->command))
       break;
     case CmdTDR:
       printk(("uti596addCmd: TDR 0x%x\n", pCmd->command))
       break;
     case CmdDump:
       printk(("uti596addCmd: Dump 0x%x\n", pCmd->command))
       break;
     case CmdDiagnose:
       printk(("uti596addCmd: Diagnose 0x%x\n", pCmd->command))
       break;
     case CmdTx:
       break;
     default:
       printk(("****Unknown Command encountered 0x%x\n", pCmd->command))
       break;
     } /* end switch */
#endif

     pCmd->status = 0;
     pCmd->command |= (CMD_EOL | CMD_INTR ); /* all commands last in list & return an interrupt */

     pCmd->next = I596_NULL;

     _ISR_Disable(level);
     if (uti596_softc.pCmdHead == I596_NULL)
       {
         uti596_softc.pCmdHead =
           uti596_softc.pCmdTail = 
           uti596_softc.scb.pCmd = pCmd;
         uti596_softc.scb.Cmd_val = word_swap ((unsigned long)pCmd);
#ifdef DBG_596
         printk(("uti596addCmd: First Cmd\n"))
#endif
         UTI_WAIT_COMMAND_ACCEPTED(10000,"add command"); /* wait for acceptance of previous command */
         uti596_softc.scb.command = CUC_START;
         i82596->chan_attn = 0x00000000;
     _ISR_Enable(level);
       }
     else
       {
#ifdef DBG_596
         printk(("uti596addCmd: Chained Cmd\n"))
#endif
         uti596_softc.pCmdTail->next = (i596_cmd *) word_swap ((unsigned long)pCmd);
         uti596_softc.pCmdTail = pCmd;
     _ISR_Enable(level);
       }

#ifdef DBG_596
         printk(("uti596addCmd: Scb status & command 0x%x 0x%x\n",
                uti596_softc.scb.status,
                uti596_softc.scb.command ))
#endif
}


/***********************************************************************
 *  Function:   uti596supplyFD
 *
 *  Description: returns a buffer to the receive frame pool.
 *               call this with Inetrrupts disabled!
 *
 *  Algorithm:
 *
 ***********************************************************************/
 
void uti596supplyFD(
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
   /* Init a list w/ one entry */
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

 if (    pLastRfd != I596_NULL &&
      ! (pLastRfd -> stat & ( STAT_C | STAT_B ) )) { /* C = complete, B = busy (prefetched) */

   /*
    * Not yet too late to add it
    */
   pLastRfd -> next = (i596_rfd *) word_swap ((unsigned long)pRfd);
   pLastRfd -> cmd &= ~CMD_EOL;  /* RESET_EL : reset EL bit to 0  */
   uti596_softc.countRFD++;  /* Lets assume we add it successfully
                                If not, the RFD may be used, and may decrement countRFD < 0 !!*/
   /*
    * Check if the last RFD was used while appending.
    */
   if ( pLastRfd -> stat & ( STAT_C | STAT_B ) ) { /* completed or was prefetched */
     /*
      * Either the EL bit of the last rfd has been read by the 82596,
      * and it will stop after reception,( true when RESET_EL not reached ) or
      * the EL bit was NOT read by the 82596 and it will use the linked
      * RFD for the next reception. ( true is RESET_EL was reached )
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

       uti596append(&uti596_softc.pSavedRfdQueue, pRfd); /* Only here! saved Q */
       uti596_softc.pEndSavedQueue = pRfd;
       uti596_softc.savedCount++;
       uti596_softc.countRFD--;

     }
     else {
       uti596_softc.pEndRFA = pRfd;           				/* the RFA has been extended */
       if ( ( uti596_softc.scb.status & SCB_STAT_RNR ||
              uti596_softc.scb.status & RU_NO_RESOURCES ) &&
              uti596_softc.countRFD > 1 ) {
         uti596_softc.pBeginRFA -> cmd &= ~CMD_EOL;		/* Ensure that beginRFA is not EOL */

         UTI_596_ASSERT(uti596_softc.pEndRFA -> next == I596_NULL, "supply: List buggered\n")
         UTI_596_ASSERT(uti596_softc.pEndRFA -> cmd & CMD_EOL, "supply: No EOL at end.\n")
         UTI_596_ASSERT(uti596_softc.scb.command == 0, "Supply: scb command must be zero\n")
#ifdef DBG_START
         printk(("uti596supplyFD: starting receiver"))
#endif
         /* start the receiver */
         UTI_596_ASSERT(uti596_softc.pBeginRFA != I596_NULL, "rx start w/ NULL begin! \n")
         uti596_softc.scb.pRfd = uti596_softc.pBeginRFA;
         uti596_softc.scb.Rfd_val = word_swap ((unsigned long) uti596_softc.pBeginRFA);
         uti596_softc.scb.command = RX_START | SCB_STAT_RNR;  /* Don't ack RNR! The receiver
         																											 * should be stopped in this case */
         UTI_596_ASSERT( !(uti596_softc.scb.status & SCB_STAT_FR),"FRAME RECEIVED INT COMING!\n")
	 i82596->chan_attn = 0x00000000;	/* send CA signal */
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
   uti596append( &uti596_softc.pSavedRfdQueue, pRfd); /* save it for RNR */

   uti596_softc.pEndSavedQueue = pRfd;                /* reset end of saved queue */
   uti596_softc.savedCount++;

   return;
 }
}

/***********************************************************************
 *  Function:   void uti596append
 *
 *  Description:
 *              adds an RFD to the end of the received frame queue,
 *              for processing by the rxproc.
 *              Also removes this RFD from the RFA
 *
 *  Algorithm:
 *
 ***********************************************************************/
void uti596append(
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
      p->cmd &= ~CMD_EOL; /* Clear EL bit at end */
      p->next = (i596_rfd *) word_swap ((unsigned long)pRfd);
    }
  }
  else {
    printk(("Illegal attempt to append: %p\n", pRfd))
  }
}

/***********************************************************************
 *  Function:   void printk_time
 *
 *  Description:
 ***********************************************************************/
void printk_time( void )
{
  rtems_time_of_day tm_struct;

  rtems_clock_get(RTEMS_CLOCK_GET_TOD, &tm_struct);
  printk(("Current time: %d:%d:%d \n",
         tm_struct.hour,
         tm_struct.minute,
         tm_struct.second))
}

/***********************************************************************
 *  Function:   void dump_scb
 *
 *  Description:
 ***********************************************************************/
void dump_scb( void )
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


#ifdef DBG_INIT_3

 /***********************************************************************
  ** 
  **    										Debugging Functions
  **
  ***********************************************************************/


 /***********************************************************************
  *  Function:   print_eth
  *
  *  Description:
  *              Print the contents of an ethernet packet header
  *              CANNOT BE CALLED FROM ISR
  *
  *  Algorithm:
  *            Print Destination, Src, and type of packet
  *
  ***********************************************************************/

/* static */ void print_eth(
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

  if ( add[12] == 0x08 && add[13] == 0x00 ) { /* an IP packet */
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
    /* printk(("********************IP Packet Data*******************\n"))
    length -=20;
    for ( i=0; i < length ; i++) {
      printk(("0x%2.2x ", add[34+i]))
    }
    printk(("\n"))

    printk(("ICMP checksum: %2.2x %2.2x\n", add[36], add[37]))
    printk(("ICMP identifier: %2.2x %2.2x\n", add[38], add[39]))
    printk(("ICMP sequence nbr: %2.2x %2.2x\n", add[40], add[41]))
    */
  }
}

 /***********************************************************************
  *  Function:   print_hdr
  *
  *  Description:
  *              Print the contents of an ethernet packet header
  *              CANNOT BE CALLED FROM ISR
  *
  *  Algorithm:
  *            Print Destination, Src, and type of packet
  *
  ***********************************************************************/

/* static */ void print_hdr(
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

 /***********************************************************************
  *  Function:   print_pkt
  *
  *  Description:
  *              Print the contents of an ethernet packet header
  *              CANNOT BE CALLED FROM ISR
  *
  *  Algorithm:
  *            Print Destination, Src, and type of packet
  *
  ***********************************************************************/

/* static */ void print_pkt(
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

 /***********************************************************************
  *  Function:   print_echo
  *
  *  Description:
  *              Print the contents of an ethernet packet header
  *              CANNOT BE CALLED FROM ISR
  *
  *  Algorithm:
  *            Prints only echo packets
  *
  ***********************************************************************/

/* static */ void print_echo(
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

/***********************************************************************
 *  Function:   uti596dump
 *
 *  Description: Dump 596 registers
 *
 *  Algorithm:
 ***********************************************************************/
/* static */ int uti596dump(
  char * pDumpArea
)
{
  i596_dump dumpCmd;
  int boguscnt = 25000000; /* over a second! */

#ifdef DBG_596
  printk(("uti596dump: begin\n"))
#endif

  dumpCmd.cmd.command = CmdDump;
  dumpCmd.cmd.next    = I596_NULL;
  dumpCmd.pData       = pDumpArea;
  uti596_softc.cmdOk = 0;
  uti596addCmd        ( (i596_cmd *)&dumpCmd);
  while (1) {
    if ( --boguscnt == 0) {
      printk(("Dump command was not processed within spin loop delay\n"))
      return 0;
    }
    else {
      if ( uti596_softc.cmdOk ) {
        return 1; /* successful completion */
      }
    }
  }
}

/***********************************************************************
 *  Function:   void dumpQ
 *
 *  Description:
 ***********************************************************************/
void dumpQ( void )
{
  i596_rfd *pRfd;

  printk(("savedQ:\n"))
  
  for( pRfd = uti596_softc.pSavedRfdQueue;
       pRfd != I596_NULL;
       pRfd = pRfd -> next) {
      printk(("pRfd: %p, stat: 0x%x cmd: 0x%x\n",pRfd,pRfd -> stat,pRfd -> cmd))
  }
      
  printk(("Inbound:\n"))
  
  for( pRfd = uti596_softc.pInboundFrameQueue;
       pRfd != I596_NULL;
       pRfd = pRfd -> next) {
    printk(("pRfd: %p, stat: 0x%x cmd: 0x%x\n",pRfd,pRfd -> stat,pRfd -> cmd))
  }
    
  printk(("Last Unk: %p\n", uti596_softc.pLastUnkRFD ))
  printk(("RFA:\n"))
  
  for( pRfd = uti596_softc.pBeginRFA;
       pRfd != I596_NULL;
       pRfd = pRfd -> next) {
    printk(("pRfd: %p, stat: 0x%x cmd: 0x%x\n",pRfd,pRfd -> stat,pRfd -> cmd))
  }
}


/***********************************************************************
 *  Function:   void show_buffers
 *
 *  Description:
 ***********************************************************************/
void show_buffers (void)
{
  i596_rfd *pRfd;

  printk(("82596 cmd: 0x%x, status: 0x%x RFA len: %d\n",
         uti596_softc.scb.command,
         uti596_softc.scb.status,
         uti596_softc.countRFD))

  printk(("\nRFA: \n"))
  
  for ( pRfd = uti596_softc.pBeginRFA;
        pRfd != I596_NULL;
        pRfd = pRfd->next) {
    printk(("Frame @ %p, status: %2.2x, cmd: %2.2x\n",
            pRfd, pRfd->stat, pRfd->cmd))
	}
  printk(("\nInbound: \n"))
  
  for ( pRfd = uti596_softc.pInboundFrameQueue;
        pRfd != I596_NULL;
        pRfd = pRfd->next) {
    printk(("Frame @ %p, status: %2.2x, cmd: %2.2x\n",
            pRfd, pRfd->stat, pRfd->cmd))
	}

  printk(("\nSaved: \n"))
  
  for ( pRfd = uti596_softc.pSavedRfdQueue;
        pRfd != I596_NULL;
        pRfd = pRfd->next) {
    printk(("Frame @ %p, status: %2.2x, cmd: %2.2x\n",
             pRfd, pRfd->stat, pRfd->cmd))
  }
           
  printk(("\nUnknown: %p\n",uti596_softc.pLastUnkRFD))
}

/***********************************************************************
 *  Function:   void show_queues
 *
 *  Description:
 ***********************************************************************/
void show_queues(void)
{
  i596_rfd *pRfd;

  printk(("CMD: 0x%x, Status: 0x%x\n",
         uti596_softc.scb.command,
         uti596_softc.scb.status))
  printk(("saved Q\n"))

  for ( pRfd = uti596_softc.pSavedRfdQueue;
        pRfd != I596_NULL &&
        pRfd != NULL;
        pRfd = pRfd->next) {
    printk(("0x%p\n", pRfd))
  }

  printk(("End saved Q 0x%p\n", uti596_softc.pEndSavedQueue))

  printk(("\nRFA:\n"))
  
  for ( pRfd = uti596_softc.pBeginRFA;
        pRfd != I596_NULL &&
        pRfd != NULL;
        pRfd = pRfd->next) {
    printk(("0x%p\n", pRfd))
  }

  printk(("uti596_softc.pEndRFA: %p\n",uti596_softc.pEndRFA))
}

 /***********************************************************************
  *  Function:   word_swap
  *
  *  Description:
  *              Return a 32 bit value, swapping the upper
  *							 and lower words first.
  *
  ***********************************************************************/
unsigned long word_swap (unsigned long val)
{
  return (((val >> 16)&(0x0000ffff)) | ((val << 16)&(0xffff0000)));
}


