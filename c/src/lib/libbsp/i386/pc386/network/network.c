/*
 * RTEMS/KA9Q driver for WD8003 Ethernet Controller
 *
 *
 *  $Id$
 */
#include <bsp.h>
#include <wd80x3.h>
#include <rtems/error.h>
#include <ka9q/rtems_ka9q.h>
#include <ka9q/global.h>
#include <ka9q/enet.h>
#include <ka9q/iface.h>
#include <ka9q/netuser.h>
#include <ka9q/trace.h>
#include <ka9q/commands.h>
#include <ka9q/domain.h>
#include <irq.h>

#define	ET_MINLEN 60		/* minimum message length */

/*
 * Number of SCCs supported by this driver
 */
#define NSCCDRIVER	1

/*
 * Default number of buffer descriptors set aside for this driver.
 * The number of transmit buffer descriptors has to be quite large
 * since a single frame often uses four or more buffer descriptors.
 */

#define RX_BUF_COUNT     15
#define TX_BUF_COUNT     4
#define TX_BD_PER_BUF    4

/*
 * RTEMS event used by interrupt handler to signal daemons.
 * This must *not* be the same event used by the KA9Q task synchronization.
 */
#define INTERRUPT_EVENT	RTEMS_EVENT_1

/*
 * Receive buffer size -- Allow for a full ethernet packet plus a pointer
 */
#define RBUF_SIZE	(1520 + sizeof (struct iface *))

/*
 * Hardware-specific storage
 */
typedef struct  {
  rtems_irq_connect_data	irqInfo;
  struct mbuf			**rxMbuf;
  struct mbuf			**txMbuf;
  unsigned int 			port;
  unsigned char			*base;
  unsigned long			bpar;
  int				rxBdCount;
  int				txBdCount;
  int				txBdHead;
  int				txBdTail;
  int				txBdActiveCount;
  struct iface			*iface;
  rtems_id			txWaitTid;
  
  /*
   * Statistics
   */
  unsigned long			rxInterrupts;
  unsigned long			rxNotFirst;
  unsigned long			rxNotLast;
  unsigned long			rxGiant;
  unsigned long			rxNonOctet;
  unsigned long			rxRunt;
  unsigned long			rxBadCRC;
  unsigned long			rxOverrun;
  unsigned long			rxCollision;
  
  unsigned long			txInterrupts;
  unsigned long			txDeferred;
  unsigned long			txHeartbeat;
  unsigned long			txLateCollision;
  unsigned long			txRetryLimit;
  unsigned long			txUnderrun;
  unsigned long			txLostCarrier;
  unsigned long			txRawWait;
}wd80x3EnetDriver;

#define RO 0x10

#define SHATOT (8*1024)		/* size of shared memory */
#define SHAPAGE 256		/* shared memory information */
#define MAXSIZ 	1536		/*(MAXBUF - MESSH_SZ)*/
#define OUTPAGE ((SHATOT-(MAXSIZ+SHAPAGE-1))/SHAPAGE)

static unsigned long loopc;

static wd80x3EnetDriver wd8003EnetDriver[NSCCDRIVER];

/*
 * WD8003 interrupt handler. The code as it is cleraly showes that
 * only one driver is connected. In order to change this a table
 * making the correspondance between the current irq number and
 * the corresponding wd8003EnetDriver structure could be used...
 */
static void wd8003Enet_interrupt_handler ()
{
  unsigned int tport, nowTicks, bootTicks;
  unsigned char status, status2;

  struct iface *iface = (struct iface *)(wd8003EnetDriver[0].iface);
  wd80x3EnetDriver *dp = (wd80x3EnetDriver *)&wd8003EnetDriver[0];
  struct mbuf *bp;
  unsigned int i2;
  unsigned int len;
  unsigned char start, next, current;
  char *shp, *temp;

  tport = wd8003EnetDriver[0].port ;

  /*
   * Drop chips interrupt
   */
  outport_byte(tport+IMR, 0x00);

  /*
   * Read status
   */
  inport_byte(tport+ISR, status);

  /*
   * Ring overwrite
   */

  if (status & MSK_OVW){
    outport_byte(tport+CMDR, MSK_STP + MSK_RD2);	/* stop 8390 */
    rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &bootTicks );
    while(nowTicks < bootTicks+loopc)               /* 2ms delay */
      rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &nowTicks );
    outport_byte(tport+RBCR0, 0);			/* clear byte count */
    outport_byte(tport+RBCR1, 0);
    inport_byte(tport+ISR, status2);
    status |= (status2 & (MSK_PTX+MSK_TXE)) ;	/* TX status */
    outport_byte(tport+TCR, MSK_LOOP);		/* loopback mode */
    outport_byte(tport+CMDR, MSK_STA + MSK_RD2);	/* start */
  }
  
  /*
   * Frame received?
   */       
  while (status & (MSK_PRX+MSK_RXE)) {
    outport_byte(tport+ISR, status & (MSK_PRX+MSK_RXE));
    inport_byte(tport+BNRY, start);
    start += 1;
    shp = dp->base + 1 + (SHAPAGE * start);
    next = *shp++;
    len = *((short *)shp)++ - 4;
    if (start >= OUTPAGE || next >= OUTPAGE)
      break;
    bp = ambufw (RBUF_SIZE);
    bp->data += sizeof (struct iface *);
    temp = bp->data;
    bp->cnt = len;

    if ((i2 = (OUTPAGE - start) * SHAPAGE - 4) < len){
      memcpy(temp, shp, i2);
      len -= i2;
      temp += i2;
      shp = dp->base;
    }
    memcpy(temp, shp, len);

    net_route (iface, &bp);
    outport_byte(tport+BNRY, next-1);
    outport_byte(tport+CMDR, MSK_PG1 + MSK_RD2);
    inport_byte(tport+CURR, current);
    outport_byte(tport+CMDR, MSK_PG0 + MSK_RD2); 
    if (current == next)
      break;
  }

  /*
   * Ring overwrite
   */
  if (status & MSK_OVW)	{     
    outport_byte(tport+ISR, MSK_OVW);	/* reset IR */
    outport_byte(tport+TCR, 0);		/* out of loopback */
    if ((status & (MSK_PTX+MSK_TXE)) == 0)
      outport_byte(tport+CMDR, MSK_TXP + MSK_RD2);	/* resend */
  }

  /*
   * Enable chip interrupts
   */
  outport_byte(tport+IMR, 0x15);
}

static void nopOn(const rtems_irq_connect_data* notUsed)
{
  /*
   * code should be moved from wd8003Enet_initialize_hardware
   * to this location
   */
}

static int wdIsOn(const rtems_irq_connect_data* irq)
{
  return pc386_irq_enabled_at_i8259s (irq->name);
}

/*
 * Initialize the ethernet hardware
 */
static void
wd8003Enet_initialize_hardware (wd80x3EnetDriver *dp, int broadcastFlag)
{
  int  i1, ultra;
  char cc1, cc2;
  unsigned char  temp;
  rtems_status_code sc;
  unsigned int tport;

  tport = dp->port;

  /* address from board ROM */
  inport_byte(tport+0x04, temp);
  outport_byte(tport+0x04, temp & 0x7f);

  for (i1=cc2=0; i1<8; i1++) {
    inport_byte(tport + ADDROM + i1, cc1);
    cc2 += cc1;
    if (i1 < 6)
      dp->iface->hwaddr[i1] = cc1;
  }
  
  inport_byte(tport+0x04, temp); 
  outport_byte(tport+0x04, temp | 0x80);	/* alternate registers */
  outport_byte(tport+W83CREG, MSK_RESET);	/* reset board, set buffer */
  outport_byte(tport+W83CREG, 0);
  outport_byte(tport+W83CREG, MSK_ENASH + (int)((dp->bpar>>13)&0x3f));

  outport_byte(tport+CMDR, MSK_PG0 + MSK_RD2);
  cc1 = MSK_BMS + MSK_FT10; /* configure 8 or 16 bits */

  inport_byte(tport+0x07, temp) ; 

  ultra = ((temp & 0xf0) == 0x20 || (temp & 0xf0) == 0x40);
  if (ultra)
    cc1 = MSK_WTS + MSK_BMS + MSK_FT10;
  outport_byte(tport+DCR, cc1);
  outport_byte(tport+RBCR0, 0);
  outport_byte(tport+RBCR1, 0);
  outport_byte(tport+RCR, MSK_MON);	       	/* disable the rxer */
  outport_byte(tport+TCR, 0);			/* normal operation */
  outport_byte(tport+PSTOP, OUTPAGE);		/* init PSTOP */
  outport_byte(tport+PSTART, 0);	       	/* init PSTART */
  outport_byte(tport+BNRY, -1);			/* init BNRY */
  outport_byte(tport+ISR, -1);			/* clear IR's */
  outport_byte(tport+IMR, 0x15);	       	/* 0x17 enable interrupt */

  outport_byte(tport+CMDR, MSK_PG1 + MSK_RD2);

  for (i1=0; i1<6; i1++)			/* initial physical addr */
    outport_byte(tport+PAR+i1, dp->iface->hwaddr[i1]);

  for (i1=0; i1<MARsize; i1++)			/* clear multicast */
    outport_byte(tport+MAR+i1, 0);
  outport_byte(tport+CURR, 0);			/* init current packet */

  outport_byte(tport+CMDR, MSK_PG0 + MSK_RD2);
  outport_byte(tport+CMDR, MSK_STA + MSK_RD2);	/* put 8390 on line */
  outport_byte(tport+RCR, MSK_AB);		/* MSK_AB accept broadcast */
  
  if (ultra) {
    inport_byte(tport+0x0c, temp);
    outport_byte(tport+0x0c, temp | 0x80);
    outport_byte(tport+0x05, 0x80);
    outport_byte(tport+0x06, 0x01);
  }
  
  /*
   * Set up interrupts
   */
  dp->irqInfo.hdl = wd8003Enet_interrupt_handler;
  dp->irqInfo.on  = nopOn;
  dp->irqInfo.off = nopOn;
  dp->irqInfo.isOn = wdIsOn;
  
  sc = pc386_install_rtems_irq_handler (&dp->irqInfo);
  if (!sc)
    rtems_panic ("Can't attach WD interrupt handler for irq %d\n",
		  dp->irqInfo.name);
}


/*
 * Send raw packet (caller provides header).
 * This code runs in the context of the interface transmit
 * task or in the context of the network task.
 */
static int
wd8003Enet_raw (struct iface *iface, struct mbuf **bpp)
{
  wd80x3EnetDriver *dp = &wd8003EnetDriver[iface->dev];
  struct mbuf *bp;
  unsigned int len, tport;
  char *shp;

  tport = dp->port;
  
  /*
   * Fill in some logging data
   */
  iface->rawsndcnt++;
  iface->lastsent = secclock ();
  dump (iface, IF_TRACE_OUT, *bpp);
  
  /*
   * It would not do to have two tasks active in the transmit
   * loop at the same time.
   * The blocking is simple-minded since the odds of two tasks
   * simultaneously attempting to use this code are low.  The only
   * way that two tasks can try to run here is:
   *	1) Task A enters this code and ends up having to
   *	   wait for a transmit buffer descriptor.
   *	2) Task B  gains control and tries to transmit a packet.
   * The RTEMS/KA9Q scheduling semaphore ensures that there
   * are no race conditions associated with manipulating the
   * txWaitTid variable.
   */

  if (dp->txWaitTid) {
    dp->txRawWait++;
    while (dp->txWaitTid)
      rtems_ka9q_ppause (10);
  }
  
  if (dp->txWaitTid == 0)		
    rtems_task_ident (0, 0, &dp->txWaitTid);

  bp = *bpp;
  len = 0;
  shp = dp->base + (SHAPAGE * OUTPAGE);

  /*rtems_interrupt_disable(level);*/
  
  for (;;){
    len += bp->cnt;
    memcpy(shp, (char *)bp->data, bp->cnt);
    shp += bp->cnt ;
    if ((bp = bp->next) == NULL)
      break;
  }

  free_p(bpp);

  if (len < ET_MINLEN) len = ET_MINLEN;
  outport_byte(tport+TBCR0, len);
  outport_byte(tport+TBCR1, (len >> 8) );
  outport_byte(tport+TPSR, OUTPAGE);
  outport_byte(tport+CMDR, MSK_TXP + MSK_RD2);

  /*
   * Show that we've finished with the packet
   */
  dp->txWaitTid = 0;
  return 0;
  
}


/*
 * Shut down the interface
 * FIXME: This is a pretty simple-minded routine.  It doesn't worry
 * about cleaning up mbufs, shutting down daemons, etc.
 */
static int
wd8003Enet_stop (struct iface *iface)
{
  unsigned int tport;
  unsigned char temp;
  /*
   * Stop the transmitter
   */
  tport=wd8003EnetDriver[0].port ;
  inport_byte(tport+0x04,temp);
  outport_byte(tport+0x04, temp & 0x7f);
  outport_byte(tport + CMDR, MSK_STP + MSK_RD2);
  return 0;
}

/*
 * Show interface statistics
 */
static void
wd8003Enet_show (struct iface *iface)
{
  printf ("      Rx Interrupts:%-8lu", wd8003EnetDriver[0].rxInterrupts);
  printf ("       Not First:%-8lu", wd8003EnetDriver[0].rxNotFirst);
  printf ("        Not Last:%-8lu\n", wd8003EnetDriver[0].rxNotLast);
  printf ("              Giant:%-8lu", wd8003EnetDriver[0].rxGiant);
  printf ("            Runt:%-8lu", wd8003EnetDriver[0].rxRunt);
  printf ("       Non-octet:%-8lu\n", wd8003EnetDriver[0].rxNonOctet);
  printf ("            Bad CRC:%-8lu", wd8003EnetDriver[0].rxBadCRC);
  printf ("         Overrun:%-8lu", wd8003EnetDriver[0].rxOverrun);
  printf ("       Collision:%-8lu\n", wd8003EnetDriver[0].rxCollision);
  printf ("      Tx Interrupts:%-8lu", wd8003EnetDriver[0].txInterrupts);
  printf ("        Deferred:%-8lu", wd8003EnetDriver[0].txDeferred);
  printf (" Missed Hearbeat:%-8lu\n", wd8003EnetDriver[0].txHeartbeat);
  printf ("         No Carrier:%-8lu", wd8003EnetDriver[0].txLostCarrier);
  printf ("Retransmit Limit:%-8lu", wd8003EnetDriver[0].txRetryLimit);
  printf ("  Late Collision:%-8lu\n", wd8003EnetDriver[0].txLateCollision);
  printf ("           Underrun:%-8lu", wd8003EnetDriver[0].txUnderrun);
  printf (" Raw output wait:%-8lu\n", wd8003EnetDriver[0].txRawWait);
}

/*
 * Attach an WD8003 driver to the system
 * This is the only `extern' function in the driver.
 *
 * argv[0]: interface label, e.g., "rtems"
 * The remainder of the arguemnts are key/value pairs:
 * mtu ##                  --  maximum transmission unit, default 1500
 * broadcast y/n           -- accept or ignore broadcast packets, default yes
 * rbuf ##                 -- Set number of receive buffer descriptors
 * rbuf ##                 -- Set number of transmit buffer descriptors
 * ip ###.###.###.###      -- IP address
 * ether ##:##:##:##:##:## -- Ethernet address
 * irno                    -- Set controller irq
 * port			   -- Set io port
 * bpar                    -- Set RAM address
 */
int
rtems_ka9q_driver_attach (int argc, char *argv[], void *p)
{
  struct iface *iface;
  wd80x3EnetDriver *dp;
  char *cp;
  int i;
  int argIndex;
  int broadcastFlag;
  char cbuf[30];
  
  /*
   * Find a free driver
   */
  for (i = 0 ; i < NSCCDRIVER ; i++) {
    if (wd8003EnetDriver[i].iface == NULL)
      break;
  }
  if (i >= NSCCDRIVER) {
    printf ("Too many SCC drivers.\n");
    return -1;
  }
  if (if_lookup (argv[0]) != NULL) {
    printf ("Interface %s already exists\n", argv[0]);
    return -1;
  }
  dp = &wd8003EnetDriver[i];
  
  /*
   * Create an inteface descriptor
   */
  iface = callocw (1, sizeof *iface);
  iface->name = strdup (argv[0]);
  
  /*
   * Set default values
   */
  broadcastFlag = 1;
  dp->txWaitTid = 0;
  dp->rxBdCount = RX_BUF_COUNT;
  dp->txBdCount = TX_BUF_COUNT * TX_BD_PER_BUF;
  dp->irqInfo.name = (rtems_irq_symbolic_name) 5;
  dp->port = 0x240;
  dp->base = (unsigned char*) 0xD0000;
  dp->bpar = 0xD0000;
  iface->mtu = 1500;
  iface->addr = Ip_addr;
  iface->hwaddr = mallocw (EADDR_LEN);
  memset (iface->hwaddr, 0x08, EADDR_LEN);
  
  /*
   * Parse arguments
   */
  for (argIndex = 1 ; argIndex < (argc - 1) ; argIndex++) {
    if (strcmp ("mtu", argv[argIndex]) == 0) {
      iface->mtu = atoi (argv[++argIndex]);
    }
    else if (strcmp ("broadcast", argv[argIndex]) == 0) {
      if (*argv[++argIndex] == 'n')
	broadcastFlag = 0;
    }
    else if (strcmp ("rbuf", argv[argIndex]) == 0) {
      dp->rxBdCount = atoi (argv[++argIndex]);
    }
    else if (strcmp ("tbuf", argv[argIndex]) == 0) {
      dp->txBdCount = atoi (argv[++argIndex]) * TX_BD_PER_BUF;
    }
    else if (strcmp ("ip", argv[argIndex]) == 0) {
      iface->addr = resolve (argv[++argIndex]);
    }
    else if (strcmp ("ether", argv[argIndex]) == 0) {
      argIndex++;
      gether (iface->hwaddr, argv[argIndex]);
    }
    else if (strcmp ("irno", argv[argIndex]) == 0) {
      dp->irqInfo.name = (rtems_irq_symbolic_name) atoi (argv[++argIndex]);
    }
    else if (strcmp ("port", argv[argIndex]) == 0) {
      sscanf(argv[++argIndex], "%x", &(dp->port));
    }
    else if (strcmp ("bpar", argv[argIndex]) == 0) {
      sscanf(argv[++argIndex], "%x", (unsigned *) &(dp->bpar));
      dp->base = (unsigned char *)(dp->bpar);
    }
    else {
      printf ("Argument %d (%s) is invalid.\n", argIndex, argv[argIndex]);
      return -1;
    }
  }
  printf ("Ethernet address: %s\n", pether (cbuf, iface->hwaddr));
  printf ("Internet address: %s\n", inet_ntoa(iface->addr));
  printf ("Irno: %X, port: %X, bpar: %X, base: %X\n",dp->irqInfo.name, dp->port,
	  (unsigned) dp->bpar, (unsigned) dp->base);
  fflush(stdout);
  /*	
   * Fill in remainder of interface configuration
   */	
  iface->dev = i;
  iface->raw = wd8003Enet_raw;
  iface->stop = wd8003Enet_stop;
  iface->show = wd8003Enet_show;
  dp->iface = iface;
  setencap (iface, "Ethernet");
  
  /*
   * Set up SCC hardware
   */
  wd8003Enet_initialize_hardware (dp, broadcastFlag);
  fflush(stdout);
  
  /*
   * Chain onto list of interfaces
   */
  iface->next = Ifaces;
  Ifaces = iface;
    
  /* calibrate a delay loop for 2 milliseconds */
  rtems_clock_get(RTEMS_CLOCK_GET_TICKS_PER_SECOND, &loopc );
  loopc /= 500;
  
  /*
   * Start I/O daemons
   */
  cp = if_name (iface, " tx");
  iface->txproc = newproc (cp, 1024, if_tx, iface->dev, iface, NULL, 0);
  free (cp);
  return 0;
}	







