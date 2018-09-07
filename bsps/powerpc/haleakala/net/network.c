/*
 *  network.c
 *  RTEMS_490
 *
 *  Created by Michael Hamel on 18/11/08.
 *
 * This code is for the PPC405EX, 405EXr on the Haleakala board with an 
 * 88E1111 PHY. 
 * Its has some adaptations for the 405GP, and 405GPr (untested).
 *
 * It should handle dual interfaces correctly, but has not been tested.
 *
 */

#include <machine/rtems-bsd-kernel-space.h>

#include <bsp.h>
#include <stdio.h>
#include <errno.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/rtems_mii_ioctl.h>
#include <rtems/score/assert.h>

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>

#include <net/if.h>

#include <netinet/in.h>

#include <netinet/if_ether.h>
#include <bsp/irq.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <ppc4xx/ppc405gp.h>
#include <ppc4xx/ppc405ex.h>

#define qDebug     /* General printf debugging */
/* #define qMultiDebug */  /* Debugging for the multicast hardware filter */

/*---------------------------- Hardware definitions -------------------------- */

/* PHY addresses for Kilauea & Haleakala; defined by hardware */
enum {
  kPHY0 = 1,
  kPHY1 = 2,
  kMaxEMACs = 2
};

enum {
  kMaxRxBuffers = 256,
  kNXmtDescriptors = 256,  /* May as well use all of them */
  kNoXmtBusy = 666    /* Arbitrary flag value outside 0..kNXmtDescriptors */
};


/*----------------------- Local variables for the driver -------------------------- */

typedef struct MALDescriptor {
  uint16_t ctrlBits;
  uint16_t adrDataSize;  /* 4 bits of high address, 12 bits of length */
  uint8_t* ptr;
} MALDescriptor;

typedef struct EMACLocals {
    struct arpcom  arpcom;

  /* Pointer to memory-mapped hardware */
  volatile EthernetRegisters_GP*  EMAC;
  
    /* Transmit and receive task references */
    rtems_id    rxDaemonTid;
  rtems_id    txDaemonTid;
  int        nRxBuffers;
  int        xmtFreeIndex;
  int        xmtBusyIndex;
  MALDescriptor*  xmtDescTable;
  MALDescriptor*  rcvDescTable;
  
  struct mbuf* rxMBufs[kMaxRxBuffers];
  struct mbuf* txMBufs[kNXmtDescriptors];
  
  int      phyAddr,  /* PHY address */
        phyState,  /* Last link state */
        phyOUI,    /* Cached PHY type info */
        phyModel,
        phyRev;

  /* Statistics */
  uint32_t   rxInterrupts;
  uint32_t   rxOverrun;
  uint32_t   rxRunt;
  uint32_t   rxBadCRC;
  uint32_t   rxNonOctet;
  uint32_t   rxGiant;
  
  uint32_t  txInterrupts;
  
  uint32_t  txLostCarrier;
  uint32_t  txDeferred;
  uint32_t  txOneCollision;
  uint32_t  txMultiCollision;
  uint32_t  txTooManyCollision;
  uint32_t  txLateCollision;
  uint32_t  txUnderrun;
  uint32_t  txPoorSignal;
} EMACLocals;


EMACLocals gEmacs[kMaxEMACs];

int ppc405_emac_phy_adapt(EMACLocals* ep);

/*----------------------------------- Globals --------------------------------------*/

/* 
   Pointers to the buffer descriptor tables used by the MAL. Tricky because they are both
   read and written to by the MAL, which is unaware of the CPU data cache. As four 8-byte 
   descriptors fit into a single cache line this makes managing them in cached memory difficult. 
   Best solution is to label them as uncached using the MMU. This code assumes an appropriate 
   sized block stating at _enet_bdesc_base has been reserved by linkcmds and has been set up
   with uncached MMU attrributes in bspstart.c */

LINKER_SYMBOL(_enet_bdesc_start);    /* start of buffer descriptor space, from linkcmds */
LINKER_SYMBOL(_enet_bdesc_end);      /* top limit, from linkcmds */

static MALDescriptor* gTx0Descs = NULL;
static MALDescriptor* gRx0Descs = NULL;
static MALDescriptor* gTx1Descs = NULL;  
static MALDescriptor* gRx1Descs = NULL;

/*------------------------------------------------------------*/


/*
 * RTEMS event used by interrupt handler to signal driver tasks.
 * This must not be any of the events used by the network task synchronization.
 */
#define INTERRUPT_EVENT      RTEMS_EVENT_1

/*
 * RTEMS event used to start transmit daemon.
 * This must not be the same as INTERRUPT_EVENT.
 */
#define START_TRANSMIT_EVENT  RTEMS_EVENT_2

#define _sync    __asm__ volatile ("sync\n"::)

#define  kCacheLineMask  (PPC_CACHE_ALIGNMENT - 1)


/*----------------------- IRQ handler glue -----------------------*/

static void InstallIRQHandler(rtems_irq_number id,
                rtems_irq_hdl handler,
                rtems_irq_enable turnOn,
                rtems_irq_disable turnOff)
{
  rtems_irq_connect_data params;
  
  params.name = id;
  params.hdl = handler;
  params.on = turnOn;
  params.off = turnOff;
  params.isOn = NULL;
  params.handle = NULL;
  if (! BSP_install_rtems_irq_handler(&params))
    rtems_panic ("Can't install interrupt handler");
}

static void
NoAction(const rtems_irq_connect_data* unused)
{
  /* printf("NoAction %d\n",unused->name); */
}


/*------------------------ PHY interface -------------------------- */
/* This code recognises and works with the 88E1111 only. Other PHYs 
   will require appropriate adaptations to this code */
   
enum {
  kPHYControl = 0,
   kPHYReset = 0x8000,
  kPHYStatus = 1,
   kPHYLinkStatus = 0x0004,
  kPHYID1 = 2,
  kPHYID2 = 3,
  kPHYAutoNegExp = 6,
  kPHY1000BaseTCtl = 9,
  kPHYExtStatus = 15,
  
  /* 88E1111 identification */
  kMarvellOUI = 0x5043,
  k88E1111Part = 0x0C,

  /* 88E1111 register addresses */
  k8PHYSpecStatus = 17,
    k8PHYSpeedShift = 14,
    k8PHYDuplex    = 0x2000,
    k8PHYResolved  = 0x0800,
    k8PHYLinkUp    = 0x0400,
  k8IntStatus = 19,
  k8IntEnable = 18,
   k8AutoNegComplete = 0x0800,
   k8LinkStateChanged = 0x0400,
  k8ExtCtlReg = 20,
    k8RcvTimingDelay = 0x0080,
    k8XmtTimingDelay = 0x0002,
    k8XmtEnable      = 0x0001,
  k8LEDCtlReg = 24,
  k8ExtStatusReg = 27,
};


static uint16_t ReadPHY(EMACLocals* ep, uint8_t reg)
{
  int n = 0;
  uint32_t t;
  
  reg &= 0x1F;
  
  /* 405EX-specific! */
  while ((ep->EMAC->STAcontrol & keSTARun) != 0)
    { ; }
  ep->EMAC->STAcontrol = keSTADirectRd + (ep->phyAddr<<5) + reg;
  ep->EMAC->STAcontrol |= keSTARun;
  /* Wait for the read to complete, should take ~25usec */
  do {
    t = ep->EMAC->STAcontrol;
    if (++n > 200000) 
      rtems_panic("PHY read timed out");
  } while ((t & keSTARun) != 0);
  
  if (t & kSTAErr) 
    rtems_panic("PHY read failed");
  return t >> 16;
}

static void WritePHY(EMACLocals* ep, uint8_t reg, uint16_t value)
{

  reg &= 0x1F;
  
  /* 405EX-specific */
  while ((ep->EMAC->STAcontrol & keSTARun) != 0)
    { ; }
  ep->EMAC->STAcontrol = (value<<16) | keSTADirectWr | (ep->phyAddr<<5) | reg;
  ep->EMAC->STAcontrol |= keSTARun;
}

static void ResetPHY(EMACLocals* ep)
{
  int n;
  
  n = ReadPHY(ep, kPHYControl);
  n |= kPHYReset;
  WritePHY(ep, kPHYControl, n);
  do {
    rtems_task_wake_after( (rtems_bsdnet_ticks_per_second/20) + 1 );
    n = ReadPHY(ep, kPHYControl);
  } while ((n & kPHYReset)!=0);
}

enum {
  kELinkUp = 0x80,
  kELinkFullDuplex = 0x40,
  kELinkSpeed10 = 0,
  kELinkSpeed100 = 1,
  kELinkSpeed1000 = 2,
  kELinkSpeedMask = 3
};

static int GetPHYLinkState(EMACLocals* ep)
/* Return link state (up/speed/duplex) as a set of flags */
{
  int state, result;
  
  /* if (ep->phyOUI==kMarvellOUI) */
  result = 0;
  state = ReadPHY(ep,k8PHYSpecStatus);
  if ((state & k8PHYLinkUp) && (state & k8PHYResolved)) {
    result |= kELinkUp;
    if (state & k8PHYDuplex) result |= kELinkFullDuplex;
    result |= ((state >> k8PHYSpeedShift) & 3);
  }
  return result;
}

/*---------------------- PHY setup ------------------------*/


static void InitPHY(EMACLocals* ep)
{
  int id,id2,n;
  
  id = ReadPHY(ep,kPHYID1);
  id2 = ReadPHY(ep,kPHYID2);
  ep->phyOUI = (id<<6) + (id2>>10);
  ep->phyModel = (id2>>4) & 0x1F;
  ep->phyRev = id2 & 0xF;
  
  #ifdef qDebug
    printf("PHY %d maker $%X model %d revision %d\n",ep->phyAddr,ep->phyOUI,ep->phyModel,ep->phyRev); 
  #endif
  
  /* Test for PHYs that we understand; insert new PHY types initialisation here */
  if (ep->phyOUI == kMarvellOUI || ep->phyModel == k88E1111Part) {
    /* 88E111-specific: Enable RxTx timing control, enable transmitter */
    n = ReadPHY(ep, k8ExtCtlReg);
    n |= k8RcvTimingDelay + k8XmtTimingDelay + k8XmtEnable;    
    WritePHY(ep, k8ExtCtlReg, n);
    
    /* Set LED mode; Haleakala has LINK10 and TX LEDs only. Set up to do 100/1000 and link up/active*/
    WritePHY(ep, k8LEDCtlReg, 0x4109);
    
    /* Need to do a reset after fiddling with registers*/
    ResetPHY(ep);
  } else
    rtems_panic("Unknown PHY type");
}


/*--------------------- Interrupt handlers for the MAL ----------------------------- */

static void
MALTXDone_handler(rtems_irq_hdl_param param)
{
  int n;
  
  n = PPC_DEVICE_CONTROL_REGISTER(MAL0_TXEOBISR);
  if (n & kMALChannel0) {
    gEmacs[0].txInterrupts++;
    rtems_event_send (gEmacs[0].txDaemonTid, INTERRUPT_EVENT);
  }
  if (n & kMALChannel1) {
    gEmacs[1].txInterrupts++;
    rtems_event_send (gEmacs[1].txDaemonTid, INTERRUPT_EVENT);
  }
  PPC_SET_DEVICE_CONTROL_REGISTER(MAL0_TXEOBISR,n);
}

static void
MALRXDone_handler (rtems_irq_hdl_param param)
{
  int n;
  
  n = PPC_DEVICE_CONTROL_REGISTER(MAL0_RXEOBISR);
  if (n & kMALChannel0) {
    gEmacs[0].rxInterrupts++;
    rtems_event_send (gEmacs[0].rxDaemonTid, INTERRUPT_EVENT);
  }
  if (n & kMALChannel1) {
    gEmacs[1].rxInterrupts++;
    rtems_event_send (gEmacs[1].rxDaemonTid, INTERRUPT_EVENT);
  }
  PPC_SET_DEVICE_CONTROL_REGISTER(MAL0_RXEOBISR,n);  /* Write back to clear interrupt */
}

/* These handlers are useful for debugging, but we don't actually need to process these interrupts */

static void
MALErr_handler (rtems_irq_hdl_param param)
{
  uint32_t errCause;
  
  errCause = PPC_DEVICE_CONTROL_REGISTER(MAL0_ESR);
  /* Clear the error */
  PPC_SET_DEVICE_CONTROL_REGISTER(MAL0_ESR,errCause);
}
  
static void
EMAC0Err_handler (rtems_irq_hdl_param param)
{
  uint32_t errCause;
  
  errCause = gEmacs[0].EMAC->intStatus;
  /* Clear error by writing back */
  gEmacs[0].EMAC->intStatus = errCause;
}

static void
EMAC1Err_handler (rtems_irq_hdl_param param)
{
  uint32_t errCause;
  
  errCause = gEmacs[1].EMAC->intStatus;
  /* Clear error by writing back */
  gEmacs[1].EMAC->intStatus = errCause;
}


/*--------------------- Low-level hardware initialisation ----------------------------- */



static void
mal_initialise(void)
{  
  uint32_t bdescbase;
  int nBytes, ntables;
  
  /*------------------- Initialise the MAL for both channels ---------------------- */
      
  PPC_SET_DEVICE_CONTROL_REGISTER(MAL0_CFG,kMALReset);
  PPC_SET_DEVICE_CONTROL_REGISTER(MAL0_TXCARR, kMALChannel0 | kMALChannel1);
  PPC_SET_DEVICE_CONTROL_REGISTER(MAL0_RXCARR, kMALChannel0 | kMALChannel1);

  /* Acquire MAL interrupts */
  InstallIRQHandler(BSP_UIC_MALTXEOB, MALTXDone_handler, NoAction, NoAction);
  InstallIRQHandler(BSP_UIC_MALRXEOB, MALRXDone_handler, NoAction, NoAction);
  InstallIRQHandler(BSP_UIC_MALSERR, MALErr_handler, NoAction, NoAction);
    
  /* Set up the buffer descriptor tables */
  bdescbase = (uint32_t)(_enet_bdesc_start);
  nBytes = sizeof(MALDescriptor) * 256;
  ntables = 4;
  if (get_ppc_cpu_type() != PPC_405EX) {
    /* The 405GP/GPr requires table bases to be 4K-aligned and can use two tx channels on one EMAC */
    nBytes = (nBytes + 0x0FFF) & ~0x0FFF;
    bdescbase = (bdescbase + 0x0FFF) & ~0x0FFF;
    ntables = 3;
  }
  
  /* printf("Buffer descriptors at $%X..$%X, code from $%X\n",bdescbase, bdescbase + nBytes*ntables - 1,(uint32_t)&_text_start); */
  
  /* Check that we have been given enough space and the buffers don't run past the enet_bdesc_end address */
  if (bdescbase + nBytes*ntables > (uint32_t)_enet_bdesc_end)
    rtems_panic("Ethernet descriptor space insufficient!");
    
  gTx0Descs = (MALDescriptor*)bdescbase;
  gTx1Descs = (MALDescriptor*)(bdescbase + nBytes);
  gRx0Descs = (MALDescriptor*)(bdescbase + nBytes*2);
  /* Clear the buffer descriptor tables */
  memset(gTx0Descs, 0, sizeof(MALDescriptor)*256);
  memset(gTx1Descs, 0, sizeof(MALDescriptor)*256);
  memset(gRx0Descs, 0, sizeof(MALDescriptor)*256);
  if (get_ppc_cpu_type() == PPC_405EX) {
    gRx1Descs = (MALDescriptor*)(bdescbase + nBytes*3);
    memset(gRx1Descs, 0, sizeof(MALDescriptor)*256);
  }
  
  /* Set up the MAL registers */
  PPC_SET_DEVICE_CONTROL_REGISTER(MAL0_TXCTP0R,gTx0Descs);
  PPC_SET_DEVICE_CONTROL_REGISTER(MAL0_TXCTP1R,gTx1Descs);
  PPC_SET_DEVICE_CONTROL_REGISTER(MAL0_RXCTP0R,gRx0Descs);
  PPC_SET_DEVICE_CONTROL_REGISTER(MAL0_RCBS0, (MCLBYTES-16)>>4);    /* The hardware writes directly to the mbuf clusters, so it can write MCLBYTES */
  if (get_ppc_cpu_type() == PPC_405EX) {
    PPC_SET_DEVICE_CONTROL_REGISTER(MAL0_CFG,kMALMedHiPriority + keMALRdMaxBurst32 + keMALWrMedHiPriority + keMALWrMaxBurst32 +
                       kMALLocksOPB + kMALLocksErrs + kMALCanBurst);
    PPC_SET_DEVICE_CONTROL_REGISTER(MAL0_RXCTP1R,gRx1Descs);
    PPC_SET_DEVICE_CONTROL_REGISTER(MAL0_RCBS1, (MCLBYTES-16)>>4);
    PPC_SET_DEVICE_CONTROL_REGISTER(MAL0_IER,0xF7);    /* Enable all MAL interrupts */
  } else {
    PPC_SET_DEVICE_CONTROL_REGISTER(MAL0_CFG,kMALMedHiPriority + kMALLocksOPB + kMALLocksErrs + kMALCanBurst + kMALLatency8);
    PPC_SET_DEVICE_CONTROL_REGISTER(MAL0_IER,0x1F);    /* Enable all MAL interrupts */
  }
}



#ifdef qDebug
static void printaddr(uint8_t* enetaddr)
{
  printf("%02X.%02X.%02X.%02X.%02X.%02X",enetaddr[0],enetaddr[1],enetaddr[2],enetaddr[3],enetaddr[4],enetaddr[5]);
}
#endif

static bool gMALInited = FALSE;
  
static void
ppc405_emac_initialize_hardware(EMACLocals* ep)
{

  int  n,mfr;
  int  unitnum = ep->arpcom.ac_if.if_unit;
  
  if (get_ppc_cpu_type() == PPC_405EX) {
    /* PPC405EX: configure the RGMII bridge and clocks */
    RGMIIRegisters* rgmp = (RGMIIRegisters*)RGMIIAddress;
    rgmp->FER = 0x00080055;  /* Both EMACs RGMII */
    rgmp->SSR = 0x00000044;  /* Both EMACs 1000Mbps */
    /* Configure the TX clock to be external */
    mfsdr(SDR0_MFR,mfr);
    mfr &= ~0x0C000000;    /* Switches both PHYs */
    mtsdr(SDR0_MFR,mfr);
  }
  
  /* Reset the EMAC */
  n = 0;
  ep->EMAC->mode0 = kEMACSoftRst;              
  while ((ep->EMAC->mode0 & kEMACSoftRst) != 0)
    n++;    /* Wait for it to complete */
  
  /* Set up so we can talk to the PHY */
  ep->EMAC->mode1 = keEMACIPHYAddr4 |  keEMACOPB100MHz;
  
  /* Initialise the PHY  */
  InitPHY(ep);
  
  /* Initialise the MAL (once only) */
  if ( ! gMALInited) {
    mal_initialise();
    gMALInited = TRUE;
  }

  /* Set up IRQ handlers and enable the MAL channels for this port */
  if (unitnum==0) {
    ep->xmtDescTable = gTx0Descs;
    ep->rcvDescTable = gRx0Descs;
    InstallIRQHandler(BSP_UIC_EMAC0, EMAC0Err_handler, NoAction, NoAction);
    PPC_SET_DEVICE_CONTROL_REGISTER(MAL0_TXCASR,kMALChannel0);
    PPC_SET_DEVICE_CONTROL_REGISTER(MAL0_RXCASR,kMALChannel0);
  } else {
    ep->xmtDescTable = gTx1Descs;
    ep->rcvDescTable = gRx1Descs;
    InstallIRQHandler(BSP_UIC_EMAC1, EMAC1Err_handler, NoAction, NoAction);
    PPC_SET_DEVICE_CONTROL_REGISTER(MAL0_TXCASR,kMALChannel1);
    PPC_SET_DEVICE_CONTROL_REGISTER(MAL0_RXCASR,kMALChannel1);
  }
  
  /* The rest of the EMAC initialisation is done in emac_phy_adapt
     when we know what the characteristics of the link are */
}


/* EMAC second stage initialisation; talks to the PHY to find out how to do it.
   Resets the EMAC if the PHY parameters need to be changed */
  
int ppc405_emac_phy_adapt(EMACLocals* ep)
{
  int linkState = GetPHYLinkState(ep);
  int spd;
  
  if ((linkState & kELinkUp) && (linkState != ep->phyState)) {
    /* Reset the EMAC and set registers according to PHY state */
    int i,n = 0;
    uint32_t mode, rmode;
    
    ep->EMAC->mode0 = kEMACSoftRst;              
    while ((ep->EMAC->mode0 & kEMACSoftRst) != 0)
      n++;    /* Wait for it to complete */
    spd = linkState & kELinkSpeedMask;
    mode = (spd<<22) | kgEMACTx0Multi;
    if (get_ppc_cpu_type() == PPC_405EX) 
      mode |= (keEMAC16KRxFIFO | keEMAC16KTxFIFO | keEMACIPHYAddr4 | keEMACOPB100MHz );
    else
      mode |= (kgEMAC4KRxFIFO | kgEMAC2KTxFIFO);
    if (linkState & kELinkFullDuplex)
      mode |= kEMACFullDuplex + kEMACDoFlowControl;
    if ( (linkState & kELinkFullDuplex) || (spd > kELinkSpeed10) )
      mode |= kEMACIgnoreSQE;
  
    
    if (spd==kELinkSpeed1000) {
      /* Gigabit, so we support jumbo frames. Take appropriate measures: adjust the if_mtu */
      /* Note that we do this here because changing it later doesn't work very well : see
         the SIOCSIFMTU discussion below */
      struct ifnet* ifp = &ep->arpcom.ac_if;
      ifp->if_mtu = ETHERMTU_JUMBO;
      mode |= keEMACJumbo;
    }
    
    
    ep->phyState = linkState;
    ep->EMAC->mode1 = mode;
    
    /* Install 48-bit hardware address that we have been given */
    ep->EMAC->addrHi = (ep->arpcom.ac_enaddr[0]<<8) + ep->arpcom.ac_enaddr[1];
    ep->EMAC->addrLo = (ep->arpcom.ac_enaddr[2]<<24) + (ep->arpcom.ac_enaddr[3]<<16)
            + (ep->arpcom.ac_enaddr[4]<<8) + (ep->arpcom.ac_enaddr[5] );
    
    /* Set receive mode appropriately */
    rmode = kEMACStripPadding + kEMACStripFCS + kEMACBrcastRcv;
    
    if (ep->arpcom.ac_if.if_flags & IFF_PROMISC) rmode |= kEMACPromiscRcv;
                        else rmode |= kEMACIndivRcv;
    if (get_ppc_cpu_type() == PPC_405EX)
      rmode |= keEMACRxFIFOAFMax;  
    if ((ep->arpcom.ac_if.if_flags & IFF_ALLMULTI) != 0) 
      rmode |= kEMACPromMultRcv;
    else if ((ep->arpcom.ac_if.if_flags & IFF_MULTICAST) != 0)
      rmode |= kEMACMultcastRcv;
    
    ep->EMAC->rcvMode = rmode;

    if (get_ppc_cpu_type() == PPC_405EX) 
      for (i=0; i<8; i++)
        ep->EMAC->e_groupHash[i] = 0;
    else 
      for (i=0; i<4; i++)
        ep->EMAC->g_groupHash[i] = 0;

    if (get_ppc_cpu_type() == PPC_405EX) {
      /* Rcv low watermark, must be < mode1 Rcv FIFO size and > MAL burst length (default 64x4 bytes), 16-byte units
         High watermark must be > low and < RcvFIFO size */
      ep->EMAC->rcvWatermarks = (16<<22) + (768<<6);
      /* Xmt low request must be >= 17 FIFO entries, Xmt urgent must be > low */
      ep->EMAC->xmtMode1 = (17<<27) + (68<<14);      /* TLR = 17, TUR = 68 */
      /* Xmt partial packet request threshold */
      ep->EMAC->xmtReqThreshold = ((1000>>2)-1) << 24;  /* TRTR[TRT] = 1000 FIFO entries */
    } else {
      ep->EMAC->rcvWatermarks = (15<<24) + (32<<8);
      ep->EMAC->xmtReqThreshold = ((1448>>6)-1) << 26;  /* TRT = 1024b */
      ep->EMAC->xmtMode1 = 0x40200000;          /* TLR = 8w=32b, TUR=32w=128b */
    }
      
    ep->EMAC->IPGap = 8;
    
    /* Want EMAC interrupts for error logging & statistics */
    ep->EMAC->intEnable = kEMACIOverrun + kEMACIPause + kEMACIBadPkt + kEMACIRuntPkt + kEMACIShortEvt
              + kEMACIAlignErr + kEMACIBadFCS + kEMACIOverSize + kEMACILLCRange + kEMACISQEErr
              + kEMACITxErr;
            
    /* Start it running */
    ep->EMAC->mode0 = kEMACRxEnable + kEMACTxEnable;    
    return 0;
  } else
    return -1;
}


static void
ppc405_emac_disable(EMACLocals* ep)
/* Disable the EMAC channels so we stop running and processing interrupts */
{
  ep->EMAC->mode0 = 0;
}

static void
ppc405_emac_startxmt(EMACLocals* ep)
/* Start the transmitter: set TMR0[GNP] */
{
  ep->EMAC->xmtMode0 = kEMACNewPacket0 + 7;    /* *** TFAE value for EX */
}

static void ppc405_emac_watchdog(struct ifnet* ifp)
/* Called if a transmit stalls or when the interface is down. Check the PHY
   until we get a valid link */
{
  EMACLocals* ep = ifp->if_softc;
  
  if (ppc405_emac_phy_adapt(ep)==0) {
    ep->arpcom.ac_if.if_flags |= IFF_RUNNING;
    ifp->if_timer = 0;    /* No longer needed */
  } else
    ifp->if_timer = 1;    /* reschedule, once a second */
}



/*----------------------- The transmit daemon/task -------------------------- */


static void
FreeTxDescriptors(EMACLocals* ep)
/* Make descriptors and mbufs at xmtBusyIndex available for re-use if the packet that they */
/* point at has all gone */
{
  uint16_t scan, status;
  
  if (ep->xmtBusyIndex != kNoXmtBusy) {
    scan = ep->xmtBusyIndex;
    while (TRUE) {
      /* Scan forward through the descriptors */
      status = ep->xmtDescTable[scan].ctrlBits;
      if (++scan >= kNXmtDescriptors)
        scan = 0;
      /* If we find a ready (i.e not-yet-sent) descriptor, stop */
      if ((status & kMALTxReady) != 0)
        break; 
      /* If we find a last descriptor, we can free all the buffers up to and including it */
      if ((status & kMALLast) != 0) {
        /* End of packet and it has been sent or abandoned; advance xmtBusyIndex to  */
        /* the next buffer and free buffers. */
        if ((status & kEMACErrMask) != 0) {
          /* Transmit error of some kind */
          
          if ((status & kEMACDeferred) != 0)
            ep->txDeferred++;
          if ((status & kEMACLostCarrier) != 0)
            ep->txLostCarrier++;  /* *** Perhaps more serious reaction needed... */
          
          if ((status & kEMACLateColl) != 0)
            ep->txLateCollision++;
          if ((status & kEMACOneColl) != 0)
            ep->txOneCollision++;
          if ((status & kEMACMultColl) != 0)
            ep->txMultiCollision++;
          if ((status & kEMACCollFail) != 0)
            ep->txTooManyCollision++;
          
          if ((status & kEMACSQEFail) != 0)
            ep->txPoorSignal++;
          if ((status & kEMACUnderrun) != 0)
            ep->txUnderrun++;
        }
        while (ep->xmtBusyIndex != scan) {
          m_free(ep->txMBufs[ep->xmtBusyIndex]);
          if (++ep->xmtBusyIndex >= kNXmtDescriptors) ep->xmtBusyIndex = 0;
        }
        if (ep->xmtBusyIndex == ep->xmtFreeIndex) {
          /* Nothing is busy */
          ep->xmtBusyIndex = kNoXmtBusy;
          break;
        }
      }
    }
  }
}


static void
SendPacket (EMACLocals* ep, struct ifnet *ifp, struct mbuf *m)
/* Given a chain of mbufs, set up a transmit description and fire it off */
{
  int nAdded, index, lastidx = -1, totalbytes;
  uint16_t status;
  struct mbuf* lastAdded;
  
  nAdded = 0;
  totalbytes = 0;
  lastAdded = NULL;
  index = ep->xmtFreeIndex;
  
  /* Go through the chain of mbufs setting up descriptors for each */
  while (m != NULL) {
          
    if (m->m_len == 0) {
      /* Can be empty: dispose and unlink from chain */
      m = m_free(m);
      if (lastAdded!=NULL) lastAdded->m_next = m;
    } else {
      /* Make sure the mbuf has been written to memory */
      rtems_cache_flush_multiple_data_lines(mtod (m, void *), m->m_len);
      /* If there are no descriptors available wait until there are */
      while (index == ep->xmtBusyIndex) {
        rtems_event_set events;
        ifp->if_timer = 2;
        /* Then check for free descriptors, followed by: */
        rtems_bsdnet_event_receive (INTERRUPT_EVENT, RTEMS_WAIT | RTEMS_EVENT_ANY, RTEMS_NO_TIMEOUT, &events);
        FreeTxDescriptors(ep);
      }
        
      /* Fill in a descriptor for this mbuf and record it */
      ep->txMBufs[index] = m;
      ep->xmtDescTable[index].ptr = mtod (m, void *);
      ep->xmtDescTable[index].adrDataSize = m->m_len;
      /* Fill in ctrlBits as we go but don't mark the first one as ready yet */
      status = kEMACGenFCS + kEMACGenPad + kEMACRepSrcAddr;
      if (nAdded > 0)
        status |= kMALTxReady;
      if (index==kNXmtDescriptors-1)
        status |= kMALWrap;
      ep->xmtDescTable[index].ctrlBits = status;
      lastidx = index;
          
      totalbytes += m->m_len;
      lastAdded = m;
      m = m->m_next;
      nAdded++;
      
      index += 1;
      if (index==kNXmtDescriptors)
        index = 0;
      
      if (nAdded==kNXmtDescriptors) 
        rtems_fatal_error_occurred(RTEMS_INTERNAL_ERROR);    /* This is impossible, of course... */
    }
  }

  _Assert( lastidx != -1 );

  if (nAdded > 0) {
    /* Done and we added some buffers */
    /* Label the last buffer and ask for an interrupt */
    ep->xmtDescTable[lastidx].ctrlBits |= kMALLast + kMALInterrupt;
    /* Finally set the ready bit on the first buffer */
    ep->xmtDescTable[ep->xmtFreeIndex].ctrlBits |= kMALTxReady;
    /* Make sure this has been written */
    _sync;
    if (ep->xmtBusyIndex == kNoXmtBusy)
      ep->xmtBusyIndex = ep->xmtFreeIndex;
    ep->xmtFreeIndex = index;
    /* Poke the EMAC to get it started (which may not be needed if its already running */
    ppc405_emac_startxmt(ep);
    ifp->if_timer = 2;
  }
}

static void
ppc405_emac_txDaemon (void* param)
{
  EMACLocals* ep = param;
  struct ifnet *ifp = &ep->arpcom.ac_if;
  struct mbuf *m;
  rtems_event_set events;

  ep->xmtFreeIndex = 0;
  ep->xmtBusyIndex = kNoXmtBusy;
  while (TRUE) {
    /* Wait for someone wanting to transmit */
    rtems_bsdnet_event_receive (START_TRANSMIT_EVENT | INTERRUPT_EVENT,
                  RTEMS_EVENT_ANY | RTEMS_WAIT, 
                  RTEMS_NO_TIMEOUT,
                  &events);
    if (events & INTERRUPT_EVENT) 
      ifp->if_timer = 0;
    /* Grab packets and send until empty */
    /* Note that this doesn't (at the time of writing, RTEMS 4.9.1), ever get asked to send more than
       one header mbuf and one data mbuf cluster, regardless of the MTU. This is because sosend() in the FreeBSD
       stack only passes one mbuf at a time across to tcp_send, which immediately sends it */
    while (TRUE) {
      FreeTxDescriptors(ep);
      IF_DEQUEUE(&ifp->if_snd, m);
      if (m == NULL) 
        break;
      SendPacket (ep, ifp, m);
    }
    ifp->if_flags &= ~IFF_OACTIVE;
  }
}

/*----------------------- The receive daemon/task -------------------------- */

static void
MakeRxBuffer(EMACLocals* ep, int index)
{
  struct mbuf*   m;
  
  /* Allocate an mbuf, wait if necessary, label as dynamic data, start of record */
  MGETHDR (m, M_WAIT, MT_DATA);
  /* Allocate a cluster buffer to this mbuf, waiting if necessary */
  MCLGET (m, M_WAIT);
  /* Set up reference to the interface the packet will be received on */
  m->m_pkthdr.rcvif = &ep->arpcom.ac_if;
  ep->rxMBufs[index] = m;
  ep->rcvDescTable[index].ptr = mtod (m, uint8_t*);
  ep->rcvDescTable[index].adrDataSize = 0x0EEE;    /* Precaution */
  if (index==ep->nRxBuffers-1)
    ep->rcvDescTable[index].ctrlBits = kMALRxEmpty + kMALInterrupt + kMALWrap;
  else
    ep->rcvDescTable[index].ctrlBits = kMALRxEmpty + kMALInterrupt;
}



static void
ppc405_emac_rxDaemon (void* param)
{
  EMACLocals* ep = param;
  int     index,n,mdex;
  struct mbuf*   m;
  struct mbuf*    mstart = NULL;
  struct mbuf*    mlast = NULL;
  struct ifnet*  ifp;
  struct ether_header* eh = NULL;
  rtems_event_set events;
  
  /* Startup : allocate a bunch of receive buffers and point the descriptor table entries at them */
  ifp = &ep->arpcom.ac_if;
  index = 0;
  while (index < ep->nRxBuffers) {
    MakeRxBuffer(ep,index);
    index += 1;
  }
  index = 0;
  mdex = 0;
  
  /* Loop waiting for frames to arrive */
  while (TRUE) {
    rtems_bsdnet_event_receive (INTERRUPT_EVENT,
                      RTEMS_WAIT | RTEMS_EVENT_ANY,
                      RTEMS_NO_TIMEOUT,
                      &events);
    while ((ep->rcvDescTable[index].ctrlBits & kMALRxEmpty) == 0) {
      /* Got a frame */
      uint16_t flags = ep->rcvDescTable[index].ctrlBits;
      if ((flags & kEMACErrMask) != 0) {
        /* It has errors. Update statistics */
        if ((flags & kEMACOverrun) != 0)
          ep->rxOverrun++;
        if ((flags & kEMACRuntPkt) != 0)
          ep->rxRunt++;
        if ((flags & kEMACBadFCS) != 0)
          ep->rxBadCRC++;
        if ((flags & kEMACAlignErr) != 0)
          ep->rxNonOctet++;
        if ((flags & kEMACPktLong) != 0)
          ep->rxGiant++;
        /* and reset descriptor to empty */
        
        /* No need to get new mbufs, just reset */
        ep->rcvDescTable[index].adrDataSize = 0x0EEE;
        if (index==ep->nRxBuffers-1)
          ep->rcvDescTable[index].ctrlBits = kMALRxEmpty + kMALInterrupt + kMALWrap;
        else
          ep->rcvDescTable[index].ctrlBits = kMALRxEmpty + kMALInterrupt;
        
      } else {
        /* Seems to be OK. Invalidate cache over the size we received */
        n = ep->rcvDescTable[index].adrDataSize & 0x0FFF;
        m = ep->rxMBufs[index];
        rtems_cache_invalidate_multiple_data_lines(m->m_data, (n + kCacheLineMask) & ~kCacheLineMask);
        
        /* Consider copying small packets out of the cluster into m_pktdat to save clusters? */
        m->m_len = n;
          
        /* Jumbo packets will span multiple mbufs; chain them together and submit when we get the last one */
        if (flags & kMALRxFirst) {
          /* First mbuf in the packet */
          if (mstart!=NULL)
            rtems_panic("first, no last");
          
          /* Adjust the mbuf pointers to skip the header and set eh to point to it */
          m->m_len -= sizeof(struct ether_header);
          m->m_pkthdr.len = m->m_len;
          eh = mtod (m, struct ether_header *);
          m->m_data += sizeof(struct ether_header);
          mstart = m;
          mlast = m;
          mdex = index;
        } else {
          /* Chain onto mstart: add length to pkthdr.len */
          if (mstart == NULL)
            rtems_panic("last, no first");
          
          mstart->m_pkthdr.len += n;
          m->m_flags &= ~M_PKTHDR;
          mlast->m_next = m;
          mlast = m;
        }
        
        if (flags & kMALLast) {
          /* Last mbuf in the packet: pass base of the chain to a higher level */
          ether_input (ifp, eh, mstart);
          
          /* ether_input took the chain, set up new mbufs in the slots we used */
          mdex -= 1;
          do {
            if (++mdex==ep->nRxBuffers) mdex = 0;
            MakeRxBuffer(ep,mdex);
          } while (mdex != index);
          mstart = NULL;
          mlast = NULL;
          eh = NULL;
        }
      }
      index += 1;
      if (index == ep->nRxBuffers) index = 0;
    }
  }
}

/*----------- Vectored routines called through the driver struct ------------------ */

static void ppc405_emac_init (void* p)
/* Initialise the hardware, create and start the transmit and receive tasks */
{
  char txName[] = "ETx0";
  char rxName[] = "ERx0";
  
  EMACLocals* ep = (EMACLocals*)p;
  if (ep->txDaemonTid == 0) {
    ppc405_emac_initialize_hardware(ep);
    rxName[3] += ep->phyAddr;
    ep->rxDaemonTid = rtems_bsdnet_newproc (rxName, 4096, ppc405_emac_rxDaemon, ep);
    txName[3] += ep->phyAddr;
    ep->txDaemonTid = rtems_bsdnet_newproc (txName, 4096, ppc405_emac_txDaemon, ep);
  }
  /* Only set IFF_RUNNING if the PHY is ready. If not set the watchdog timer running so we check it */
  if ( GetPHYLinkState(ep) & kELinkUp ) 
    ep->arpcom.ac_if.if_flags |= IFF_RUNNING;
  else
    ep->arpcom.ac_if.if_timer = 1;
}

static void ppc405_emac_start(struct ifnet *ifp)
/* Send a packet: send an event to the transmit task, waking it up */
{
  EMACLocals* ep = ifp->if_softc;
  rtems_event_send (ep->txDaemonTid, START_TRANSMIT_EVENT);
  ifp->if_flags |= IFF_OACTIVE;
}

static void ppc405_emac_stop (EMACLocals* ep)
{
  uint32_t mask;
  
  mask = 0x80000000 >> ep->arpcom.ac_if.if_unit;
  PPC_SET_DEVICE_CONTROL_REGISTER(MAL0_TXCARR,mask);
  PPC_SET_DEVICE_CONTROL_REGISTER(MAL0_RXCARR,mask);
  ppc405_emac_disable(ep);
  /* *** delete daemons, or do they exit themselves? */
  ep->arpcom.ac_if.if_flags &= ~IFF_RUNNING;
}

#ifdef qDebug
static void ppc405_emac_stats (EMACLocals* ep)
{

  printf ("  Rx Interrupts:%-8lu", ep->rxInterrupts);
  printf ("          Giant:%-8lu", ep->rxGiant);
  printf ("           Runt:%-8lu\n", ep->rxRunt);
  printf ("      Non-octet:%-8lu", ep->rxNonOctet);
  printf ("        Bad CRC:%-8lu", ep->rxBadCRC);
  printf ("        Overrun:%-8lu\n", ep->rxOverrun);

  printf ("    Tx Interrupts:%-8lu", ep->txInterrupts);
  printf ("    Long deferral:%-8lu", ep->txDeferred);
  printf ("       No Carrier:%-8lu\n", ep->txLostCarrier);
  printf ("   Late collision:%-8lu", ep->txLateCollision);
  printf ("    One collision:%-8lu", ep->txOneCollision);
  printf ("  Many collisions:%-8lu\n", ep->txMultiCollision);
  printf ("Excess collisions:%-8lu", ep->txTooManyCollision);
  printf ("         Underrun:%-8lu", ep->txUnderrun);
  printf ("      Poor signal:%-8lu\n", ep->txPoorSignal);
}
#endif

static int UpdateMulticast(EMACLocals* ep)
{
  /* Traverse list of multicast addresses and update hardware hash filter. This is just a work-reduction */
  /* step; the filter uses a hash of the hardware address and therefore doesn't catch all unwanted packets */
  /* We have to do other checks in software. */
  /* 405GP/GPr has 4x16-bit hash registers, 405EX/EXr has 8x32-bit */
  
  struct ether_multi* enm;
  struct ether_multistep step;
  uint32_t hash;
  
  #ifdef qMultiDebug
    printf("\nMulticast etheraddrs:\n");
  #endif
  
  ETHER_FIRST_MULTI(step, &ep->arpcom, enm);
  while (enm != NULL) {
    
    /* *** Doesn't implement ranges */
        
    hash = ether_crc32_be( (uint8_t*)&enm->enm_addrlo, sizeof(enm->enm_addrlo) );
    if (get_ppc_cpu_type() == PPC_405EX) {
      hash >>= 24;  /* Upper 8 bits, split 3/5 */
      /* This has been experimentally verified against the hardware */
      ep->EMAC->e_groupHash[7-(hash>>5)] |= (1 << (hash & 0x1F));
    } else {
      hash >>= 26;  /* Upper 6 bits, split 2/4 */
      /* This has not been checked */
      ep->EMAC->g_groupHash[3-(hash>>6)] |= (1 << (hash & 0xF));
    }
    
    #ifdef qMultiDebug
      printf("  ");
      printaddr(enm->enm_addrlo);
      printf(" = bit %d",hash);
      if (memcmp(&enm->enm_addrlo, &enm->enm_addrhi, 6) != 0) {
        printf(" - ");
        printaddr(enm->enm_addrhi);
        printf(" [not supported]");
      }
      printf("\n");
    #endif
    
    ETHER_NEXT_MULTI(step, enm);
  }
  #ifdef qMultiDebug
  {
    int i;
    printf(" Grouphash is ");
    for (i=0; i<8; i++)
      printf("%08X:",(int)ep->EMAC->e_groupHash[i]);
    printf("\n");
  }
  #endif
  return 0;
}


static int ppc405_emac_ioctl (struct ifnet *ifp, ioctl_command_t command, caddr_t data)
{
  int error = 0;
  EMACLocals* ep = ifp->if_softc;
  struct ifreq* reqP = (struct ifreq *) data;
    
  switch (command) {
  
    case SIOCSIFFLAGS:
      switch (ifp->if_flags & (IFF_UP | IFF_RUNNING)) {
        case IFF_RUNNING:
          ppc405_emac_stop(ep);
          break;
    
        case IFF_UP:
          ppc405_emac_init(ep);
          break;
    
        case IFF_UP | IFF_RUNNING:
          ppc405_emac_stop(ep);
          ppc405_emac_init(ep);
          break;
    
        default:
          break;
      }
      break;
  
    case SIOCADDMULTI: {
        error = ether_addmulti( reqP, &ep->arpcom);
        if (error==ENETRESET)
          error = UpdateMulticast(ep);
      } break;
      
    case SIOCDELMULTI:
      error = ether_delmulti( (struct ifreq *) data, &ep->arpcom);
      if (error==ENETRESET)
        error = UpdateMulticast(ep);
      break;
      
    case SIOCSIFMTU: {
        /* Note that this may not do what you want; setting the interface MTU doesn't touch the route MTUs,
           and new routes are sometimes made by cloning old ones. So this won't change the MTU to known hosts
           and may not change the MTU to new ones either... */
        int max;
        if ( get_ppc_cpu_type() == PPC_405EX && (ep->EMAC->mode1 & keEMACJumbo) != 0 )
          max = ETHER_MAX_LEN_JUMBO;
        else
          max = ETHER_MAX_LEN;
        if (reqP->ifr_mtu > max - ETHER_HDR_LEN - ETHER_CRC_LEN)
          error = EINVAL;
        else 
          ifp->if_mtu = reqP->ifr_mtu;
      } break;
      
    case SIO_RTEMS_SHOW_STATS:
      #ifdef qDebug
        ppc405_emac_stats(ep);
      #endif
      break;
      
    default:
      /* Not handled here, pass to generic */
      error = ether_ioctl(ifp,command,data);
      break;
  }
  
  #ifdef qDebug
  if (error != 0)
    printf("--- Ethernet ioctl %d failed %d\n",(int)command,error);
  #endif
  
  return error;
}

  
/*----------------------- External attach function -------------------------- 
 *  
 * This is the one function we are required to define in here: declared in bsp.h 
 * as RTEMS_BSP_NETWORK_DRIVER_ATTACH and called from rtems_bsdnet_attach 
 *
*/

int
rtems_emac_driver_attach(struct rtems_bsdnet_ifconfig* config, int attaching)
{
  int   unitNumber, nUnits;
  char*  unitName;
  struct  ifnet* ifp;
  EMACLocals* ep;
  
  if (attaching==0) {
    printk ("EMAC: driver cannot be detached.\n");
    return 0;
  }
  
  nUnits = 1;
  if (get_ppc_cpu_type()==PPC_405EX && get_ppc_cpu_revision() > 0x1474)
    nUnits = 2;  /* PPC405EX has two interfaces, EXr has one */
  
  unitNumber = rtems_bsdnet_parse_driver_name (config, &unitName);
    if (unitNumber < 0 || unitNumber > nUnits-1) {
    printk ("EMAC: bad unit number %d.\n",unitNumber);
    return 0;
  }
  
  ep = &gEmacs[unitNumber];
  
  if (get_ppc_cpu_type()==PPC_405EX) {
    if (unitNumber==0) ep->EMAC = (EthernetRegisters_EX*)EMAC0EXAddress;
            else ep->EMAC = (EthernetRegisters_GP*)EMAC1EXAddress;
  } else 
    ep->EMAC = (EthernetRegisters_GP*)EMAC0GPAddress;
    
  ifp = &ep->arpcom.ac_if;
  if (ifp->if_softc != NULL) {
    printk ("EMAC: driver already in use.\n");
    return 0;
  }
  ifp->if_softc = ep;
  
  if (config->hardware_address == 0)
    rtems_panic("No Ethernet MAC address specified!");
  memcpy (ep->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);
  
  ifp->if_name = unitName;
  ifp->if_unit = unitNumber;
  
  if (config->mtu != 0)
    ifp->if_mtu = config->mtu;
  else 
    ifp->if_mtu = ETHERMTU;    /* May be adjusted later by ppc405_emac_phy_adapt() */
    
  ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX | IFF_MULTICAST;
  if (ifp->if_snd.ifq_maxlen == 0)
    ifp->if_snd.ifq_maxlen = ifqmaxlen;
  ifp->if_init = &ppc405_emac_init;
  ifp->if_ioctl = ppc405_emac_ioctl;
  ifp->if_start = ppc405_emac_start;
  ifp->if_output = ether_output;
  ifp->if_watchdog = ppc405_emac_watchdog;
  ifp->if_timer   = 0;
  
  if (config->rbuf_count != 0) {
    if (config->rbuf_count > 256) ep->nRxBuffers = 256;
                 else ep->nRxBuffers = config->rbuf_count;
  } else
    ep->nRxBuffers = nmbclusters/2;
    
  ep->phyAddr = unitNumber+1;
  ep->phyState = 0;
  
  #ifdef qDebug
    printf("\n  Setting up EMAC %d of %d\n",unitNumber+1,nUnits);
    printf("  MAC address is ");
    printaddr(ep->arpcom.ac_enaddr);
    printf("  MHLEN = %d, MINCLSIZE = %d MCLBYTES = %d\n",MHLEN,MINCLSIZE,MCLBYTES); 
    printf("  ticks/sec = %d, usec/tick = %d\n", rtems_bsdnet_ticks_per_second, rtems_bsdnet_microseconds_per_tick);
   #endif
  
   if_attach (ifp);
  ether_ifattach (ifp);

  return 1;
}

