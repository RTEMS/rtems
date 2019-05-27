/*
 * Gaisler Research ethernet MAC driver
 * adapted from Opencores driver by Marko Isomaki
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *
 *  2008-12-10, Converted to driver manager and added support for
 *              multiple GRETH cores. <daniel@gaisler.com>
 *  2007-09-07, Ported GBIT support from 4.6.5
 */

#include <machine/rtems-bsd-kernel-space.h>

#include <rtems.h>
#define CPU_U32_FIX
#include <bsp.h>

#ifdef GRETH_SUPPORTED

#include <inttypes.h>
#include <errno.h>
#include <rtems/bspIo.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>

#include <grlib/greth.h>
#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>
#include <grlib/ambapp.h>

#include <sys/param.h>
#include <sys/mbuf.h>

#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>

#ifdef malloc
#undef malloc
#endif
#ifdef free
#undef free
#endif

#include <grlib/grlib_impl.h>

#if defined(__m68k__)
extern m68k_isr_entry set_vector( rtems_isr_entry, rtems_vector_number, int );
#else
extern rtems_isr_entry set_vector( rtems_isr_entry, rtems_vector_number, int );
#endif


/* #define GRETH_DEBUG */

#ifdef GRETH_DEBUG
#define DBG(args...) printk(args)
#else
#define DBG(args...)
#endif

/* #define GRETH_DEBUG_MII */

#ifdef GRETH_DEBUG_MII
#define MIIDBG(args...) printk(args)
#else
#define MIIDBG(args...)
#endif

#ifdef CPU_U32_FIX
extern void ipalign(struct mbuf *m);
#endif

/* Used when reading from memory written by GRETH DMA unit */
#ifndef GRETH_MEM_LOAD
#define GRETH_MEM_LOAD(addr) (*(volatile unsigned int *)(addr))
#endif

/*
 * Number of OCs supported by this driver
 */
#define NOCDRIVER	1

/*
 * Receive buffer size -- Allow for a full ethernet packet including CRC
 */
#define RBUF_SIZE 1518

#define	ET_MINLEN 64		/* minimum message length */

/*
 * RTEMS event used by interrupt handler to signal driver tasks.
 * This must not be any of the events used by the network task synchronization.
 */
#define INTERRUPT_EVENT	RTEMS_EVENT_1

/*
 * RTEMS event used to start transmit daemon.
 * This must not be the same as INTERRUPT_EVENT.
 */
#define START_TRANSMIT_EVENT	RTEMS_EVENT_2

 /* event to send when tx buffers become available */
#define GRETH_TX_WAIT_EVENT  RTEMS_EVENT_3

#if (MCLBYTES < RBUF_SIZE)
# error "Driver must have MCLBYTES > RBUF_SIZE"
#endif

/* 4s Autonegotiation Timeout */
#ifndef GRETH_AUTONEGO_TIMEOUT_MS
#define GRETH_AUTONEGO_TIMEOUT_MS 4000
#endif
const struct timespec greth_tan = {
   GRETH_AUTONEGO_TIMEOUT_MS/1000,
   (GRETH_AUTONEGO_TIMEOUT_MS % 1000) * 1000000
};

/* For optimizing the autonegotiation time */
#define GRETH_AUTONEGO_PRINT_TIME

/* Ethernet buffer descriptor */

typedef struct _greth_rxtxdesc {
   volatile uint32_t ctrl; /* Length and status */
   uint32_t *addr;         /* Buffer pointer */
} greth_rxtxdesc;


/*
 * Per-device data
 */
struct greth_softc
{

   struct arpcom arpcom;
   struct drvmgr_dev *dev;		/* Driver manager device */
   char devName[32];

   greth_regs *regs;
   int minor;
   int phyaddr;  /* PHY Address configured by user (or -1 to autodetect) */
   unsigned int edcl_dis;
   int greth_rst;

   int acceptBroadcast;
   rtems_id daemonTid;
   
   unsigned int tx_ptr;
   unsigned int tx_dptr;
   unsigned int tx_cnt;
   unsigned int rx_ptr;
   unsigned int txbufs;
   unsigned int rxbufs;
   greth_rxtxdesc *txdesc;
   greth_rxtxdesc *rxdesc;
   unsigned int txdesc_remote;
   unsigned int rxdesc_remote;
   struct mbuf **rxmbuf;
   struct mbuf **txmbuf;
   rtems_vector_number vector;
   
   /* TX descriptor interrupt generation */
   int tx_int_gen;
   int tx_int_gen_cur;
   struct mbuf *next_tx_mbuf;
   int max_fragsize;
   
   /*Status*/
   struct phy_device_info phydev;
   int phy_read_access;
   int phy_write_access;
   int fd;
   int sp;
   int gb;
   int gbit_mac;
   int auto_neg;
   unsigned int advmodes; /* advertise ethernet speed modes. 0 = all modes. */
   struct timespec auto_neg_time;
   int mc_available;

   /*
    * Statistics
    */
   unsigned long rxInterrupts;
   
   unsigned long rxPackets;
   unsigned long rxLengthError;
   unsigned long rxNonOctet;
   unsigned long rxBadCRC;
   unsigned long rxOverrun;
   
   unsigned long txInterrupts;
   
   unsigned long txDeferred;
   unsigned long txHeartbeat;
   unsigned long txLateCollision;
   unsigned long txRetryLimit;
   unsigned long txUnderrun;

   /* Spin-lock ISR protection */
   SPIN_DECLARE(devlock);
};

int greth_process_tx_gbit(struct greth_softc *sc);
int greth_process_tx(struct greth_softc *sc);

static char *almalloc(int sz, int alignment)
{
        char *tmp;
        tmp = grlib_calloc(1, sz + (alignment-1));
        tmp = (char *) (((int)tmp+alignment) & ~(alignment -1));
        return(tmp);
}

/* GRETH interrupt handler */

static void greth_interrupt (void *arg)
{
        uint32_t status;
        uint32_t ctrl;
        rtems_event_set events = 0;
        struct greth_softc *greth = arg;
        SPIN_ISR_IRQFLAGS(flags);

        /* read and clear interrupt cause */
        status = greth->regs->status;
        greth->regs->status = status;

        SPIN_LOCK(&greth->devlock, flags);
        ctrl = greth->regs->ctrl;

        /* Frame received? */
        if ((ctrl & GRETH_CTRL_RXIRQ) && (status & (GRETH_STATUS_RXERR | GRETH_STATUS_RXIRQ)))
        {
                greth->rxInterrupts++;
                /* Stop RX-Error and RX-Packet interrupts */
                ctrl &= ~GRETH_CTRL_RXIRQ;
                events |= INTERRUPT_EVENT;
        }

        if ( (ctrl & GRETH_CTRL_TXIRQ) && (status & (GRETH_STATUS_TXERR | GRETH_STATUS_TXIRQ)) )
        {
                greth->txInterrupts++;
                ctrl &= ~GRETH_CTRL_TXIRQ;
                events |= GRETH_TX_WAIT_EVENT;
        }

        /* Clear interrupt sources */
        greth->regs->ctrl = ctrl;
        SPIN_UNLOCK(&greth->devlock, flags);

        /* Send the event(s) */
        if ( events )
            rtems_bsdnet_event_send(greth->daemonTid, events);
}

static uint32_t read_mii(struct greth_softc *sc, uint32_t phy_addr, uint32_t reg_addr)
{
    sc->phy_read_access++;
    while (sc->regs->mdio_ctrl & GRETH_MDIO_BUSY) {}
    sc->regs->mdio_ctrl = (phy_addr << 11) | (reg_addr << 6) | GRETH_MDIO_READ;
    while (sc->regs->mdio_ctrl & GRETH_MDIO_BUSY) {}
    if (!(sc->regs->mdio_ctrl & GRETH_MDIO_LINKFAIL)) {
        MIIDBG("greth%d: mii read[%d] OK to %" PRIx32 ".%" PRIx32
               " (0x%08" PRIx32 ",0x%08" PRIx32 ")\n",
               sc->minor, sc->phy_read_access, phy_addr, reg_addr,
               sc->regs->ctrl, sc->regs->mdio_ctrl);
        return((sc->regs->mdio_ctrl >> 16) & 0xFFFF);
    } else {
        printf("greth%d: mii read[%d] failed to %" PRIx32 ".%" PRIx32
               " (0x%08" PRIx32 ",0x%08" PRIx32 ")\n",
               sc->minor, sc->phy_read_access, phy_addr, reg_addr,
               sc->regs->ctrl, sc->regs->mdio_ctrl);
        return (0xffff);
    }
}

static void write_mii(struct greth_softc *sc, uint32_t phy_addr, uint32_t reg_addr, uint32_t data)
{
    sc->phy_write_access++;
    while (sc->regs->mdio_ctrl & GRETH_MDIO_BUSY) {}
    sc->regs->mdio_ctrl =
     ((data & 0xFFFF) << 16) | (phy_addr << 11) | (reg_addr << 6) | GRETH_MDIO_WRITE;
    while (sc->regs->mdio_ctrl & GRETH_MDIO_BUSY) {}
    if (!(sc->regs->mdio_ctrl & GRETH_MDIO_LINKFAIL)) {
        MIIDBG("greth%d: mii write[%d] OK to  to %" PRIx32 ".%" PRIx32
               "(0x%08" PRIx32 ",0x%08" PRIx32 ")\n",
               sc->minor, sc->phy_write_access, phy_addr, reg_addr,
               sc->regs->ctrl, sc->regs->mdio_ctrl);
    } else {
        printf("greth%d: mii write[%d] failed to to %" PRIx32 ".%" PRIx32
               " (0x%08" PRIx32 ",0x%08" PRIx32 ")\n",
               sc->minor, sc->phy_write_access, phy_addr, reg_addr,
               sc->regs->ctrl, sc->regs->mdio_ctrl);
    }
}

static void print_init_info(struct greth_softc *sc) 
{
    printf("greth: driver attached\n");
    if ( sc->auto_neg == -1 ){
        printf("Auto negotiation timed out. Selecting default config\n");
    }
    printf("**** PHY ****\n");
    printf("Vendor: %x   Device: %x   Revision: %d\n",sc->phydev.vendor, sc->phydev.device, sc->phydev.rev);
    printf("Current Operating Mode: ");
    if (sc->gb) {
        printf("1000 Mbit ");
    } else if (sc->sp) {
        printf("100 Mbit ");
    } else {
        printf("10 Mbit ");
    }
    if (sc->fd) {
        printf("Full Duplex\n");
    } else {
        printf("Half Duplex\n");
    }
#ifdef GRETH_AUTONEGO_PRINT_TIME
    if ( sc->auto_neg ) {
        printf("Autonegotiation Time: %" PRIdMAX "ms\n",
               (intmax_t)sc->auto_neg_time.tv_sec * 1000 +
               sc->auto_neg_time.tv_nsec / 1000000);
    }
#endif
}

/*
 * Generates the hash words based on CRCs of the enabled MAC addresses that are
 * allowed to be received. The allowed MAC addresses are maintained in a linked
 * "multi-cast" list available in the arpcom structure.
 *
 * Returns the number of MAC addresses that were processed (in the list)
 */
static int
greth_mac_filter_calc(struct arpcom *ac, uint32_t *msb, uint32_t *lsb)
{
    struct ether_multistep step;
    struct ether_multi *enm;
    int cnt = 0;
    uint32_t crc, htindex, ht[2] = {0, 0};

    /* Go through the Ethernet Multicast addresses one by one and add their
     * CRC contribution to the MAC filter.
     */
    ETHER_FIRST_MULTI(step, ac, enm);
    while (enm) {
        crc = ether_crc32_be((uint8_t *)enm->enm_addrlo, 6);
        htindex = crc & 0x3f;
        ht[htindex >> 5] |= (1 << (htindex & 0x1F));
        cnt++;
        ETHER_NEXT_MULTI(step, enm);
    }

    if (cnt > 0) {
        *msb = ht[1];
        *lsb = ht[0];
    }

    return cnt;
}

/*
 * Initialize the ethernet hardware
 */
static int greth_mac_filter_set(struct greth_softc *sc)
{
    struct ifnet *ifp = &sc->arpcom.ac_if;
    uint32_t hash_msb, hash_lsb, ctrl;
    SPIN_IRQFLAGS(flags);

    hash_msb = 0;
    hash_lsb = 0;
    ctrl = 0;
    if (ifp->if_flags & IFF_PROMISC) {
        /* No need to enable multi-cast when promiscous mode accepts all */
        ctrl |= GRETH_CTRL_PRO;
    } else if(!sc->mc_available) {
        return EINVAL; /* no hardware support for multicast filtering. */
    } else if (ifp->if_flags & IFF_ALLMULTI) {
        /* We should accept all multicast addresses */
        ctrl |= GRETH_CTRL_MCE;
        hash_msb = 0xFFFFFFFF;
        hash_lsb = 0xFFFFFFFF;
    } else if (greth_mac_filter_calc(&sc->arpcom, &hash_msb, &hash_lsb) > 0) {
        /* Generate hash for MAC filtering out multicast addresses */
        ctrl |= GRETH_CTRL_MCE;
    } else {
        /* Multicast list is empty .. disable multicast */
    }
    SPIN_LOCK_IRQ(&sc->devlock, flags);
    sc->regs->ht_msb = hash_msb;
    sc->regs->ht_lsb = hash_lsb;
    sc->regs->ctrl = (sc->regs->ctrl & ~(GRETH_CTRL_PRO | GRETH_CTRL_MCE)) |
                     ctrl;
    SPIN_UNLOCK_IRQ(&sc->devlock, flags);

    return 0;
}

/*
 * Initialize the ethernet hardware
 */
static void
greth_initialize_hardware (struct greth_softc *sc)
{
    struct mbuf *m;
    int i;
    int phyaddr;
    int phyctrl;
    int phystatus;
    int tmp1;
    int tmp2;
    struct timespec tstart, tnow;
    greth_regs *regs;
    unsigned int advmodes, speed;

    regs = sc->regs;

    /* Reset the controller.  */
    sc->rxInterrupts = 0;
    sc->rxPackets = 0;

    if (sc->greth_rst) {
        /* Reset ON */
        regs->ctrl = GRETH_CTRL_RST | GRETH_CTRL_DD | GRETH_CTRL_ED;
        for (i = 0; i<100 && (regs->ctrl & GRETH_CTRL_RST); i++)
            ;
        speed = 0; /* probe mode below */
    } else {
        /* inherit EDCL mode for now */
        speed = sc->regs->ctrl & (GRETH_CTRL_GB|GRETH_CTRL_SP|GRETH_CTRL_FULLD);
    }
    /* Reset OFF and RX/TX DMA OFF. SW do PHY Init */
    regs->ctrl = GRETH_CTRL_DD | GRETH_CTRL_ED | speed;

    /* Check if mac is gbit capable*/
    sc->gbit_mac = (regs->ctrl >> 27) & 1;

    /* Get the phy address which assumed to have been set
       correctly with the reset value in hardware*/
    if ( sc->phyaddr == -1 ) {
        phyaddr = (regs->mdio_ctrl >> 11) & 0x1F;
    } else {
        phyaddr = sc->phyaddr;
    }
    sc->phy_read_access = 0;
    sc->phy_write_access = 0;

    /* As I understand the PHY comes back to a good default state after
     * Power-down or Reset, so we do both just in case. Power-down bit should
     * be cleared.
     * Wait for old reset (if asserted by boot loader) to complete, otherwise
     * power-down instruction might not have any effect.
     */
    while (read_mii(sc, phyaddr, 0) & 0x8000) {}
    write_mii(sc, phyaddr, 0, 0x0800); /* Power-down */
    write_mii(sc, phyaddr, 0, 0x0000); /* Power-Up */
    write_mii(sc, phyaddr, 0, 0x8000); /* Reset */

    /* We wait about 30ms */
    rtems_task_wake_after(rtems_clock_get_ticks_per_second()/32);

    /* Wait for reset to complete and get default values */
    while ((phyctrl = read_mii(sc, phyaddr, 0)) & 0x8000) {}

    /* Set up PHY advertising modes for auto-negotiation */
    advmodes = sc->advmodes;
    if (advmodes == 0)
        advmodes = GRETH_ADV_ALL;
    if (!sc->gbit_mac)
        advmodes &= ~(GRETH_ADV_1000_FD | GRETH_ADV_1000_HD);

    /* Enable/Disable GBit auto-neg advetisement so that the link partner
     * know that we have/haven't GBit capability. The MAC may not support
     * Gbit even though PHY does...
     */
    phystatus = read_mii(sc, phyaddr, 1);
    if (phystatus & 0x0100) {
        tmp1 = read_mii(sc, phyaddr, 9);
        tmp1 &= ~0x300;
        if (advmodes & GRETH_ADV_1000_FD)
            tmp1 |= 0x200;
        if (advmodes & GRETH_ADV_1000_HD)
            tmp1 |= 0x100;
        write_mii(sc, phyaddr, 9, tmp1);
    }

    /* Optionally limit the 10/100 modes as configured by user */
    tmp1 = read_mii(sc, phyaddr, 4);
    tmp1 &= ~0x1e0;
    if (advmodes & GRETH_ADV_100_FD)
        tmp1 |= 0x100;
    if (advmodes & GRETH_ADV_100_HD)
        tmp1 |= 0x080;
    if (advmodes & GRETH_ADV_10_FD)
        tmp1 |= 0x040;
    if (advmodes & GRETH_ADV_10_HD)
        tmp1 |= 0x020;
    write_mii(sc, phyaddr, 4, tmp1);

    /* If autonegotiation implemented we start it */
    if (phystatus & 0x0008) {
        write_mii(sc, phyaddr, 0, phyctrl | 0x1200);
        phyctrl = read_mii(sc, phyaddr, 0);
    }

    /* Check if PHY is autoneg capable and then determine operating mode, 
       otherwise force it to 10 Mbit halfduplex */
    sc->gb = 0;
    sc->fd = 0;
    sc->sp = 0;
    sc->auto_neg = 0;
    timespecclear(&sc->auto_neg_time);
    if ((phyctrl >> 12) & 1) {
            /*wait for auto negotiation to complete*/
            sc->auto_neg = 1;
            if (rtems_clock_get_uptime(&tstart) != RTEMS_SUCCESSFUL)
                    printk("rtems_clock_get_uptime failed\n");
            while (!(((phystatus = read_mii(sc, phyaddr, 1)) >> 5) & 1)) {
                    if (rtems_clock_get_uptime(&tnow) != RTEMS_SUCCESSFUL)
                            printk("rtems_clock_get_uptime failed\n");
                    timespecsub(&tnow, &tstart, &sc->auto_neg_time);
                    if (timespeccmp(&sc->auto_neg_time, &greth_tan, >)) {
                            sc->auto_neg = -1; /* Failed */
                            tmp1 = read_mii(sc, phyaddr, 0);
                            sc->gb = ((phyctrl >> 6) & 1) && !((phyctrl >> 13) & 1);
                            sc->sp = !((phyctrl >> 6) & 1) && ((phyctrl >> 13) & 1);
                            sc->fd = (phyctrl >> 8) & 1;
                            goto auto_neg_done;
                    }
                    /* Wait about 30ms, time is PHY dependent */
                    rtems_task_wake_after(rtems_clock_get_ticks_per_second()/32);
            }
            sc->phydev.adv = read_mii(sc, phyaddr, 4);
            sc->phydev.part = read_mii(sc, phyaddr, 5);
            if ((phystatus >> 8) & 1) {
                    sc->phydev.extadv = read_mii(sc, phyaddr, 9);
                    sc->phydev.extpart = read_mii(sc, phyaddr, 10);
                       if ( (sc->phydev.extadv & GRETH_MII_EXTADV_1000HD) &&
                            (sc->phydev.extpart & GRETH_MII_EXTPRT_1000HD)) {
                               sc->gb = 1;
                               sc->fd = 0;
                       }
                       if ( (sc->phydev.extadv & GRETH_MII_EXTADV_1000FD) &&
                            (sc->phydev.extpart & GRETH_MII_EXTPRT_1000FD)) {
                               sc->gb = 1;
                               sc->fd = 1;
                       }
            }
            if ((sc->gb == 0) || ((sc->gb == 1) && (sc->gbit_mac == 0))) {
                    if ( (sc->phydev.adv & GRETH_MII_100TXFD) &&
                         (sc->phydev.part & GRETH_MII_100TXFD)) {
                            sc->sp = 1;
                            sc->fd = 1;
                    } else if ( (sc->phydev.adv & GRETH_MII_100TXHD) &&
                                (sc->phydev.part & GRETH_MII_100TXHD)) {
                            sc->sp = 1;
                            sc->fd = 0;
                    } else if ( (sc->phydev.adv & GRETH_MII_10FD) &&
                                (sc->phydev.part & GRETH_MII_10FD)) {
                            sc->fd = 1;
                    }
            }
    }
auto_neg_done:
    sc->phydev.vendor = 0;
    sc->phydev.device = 0;
    sc->phydev.rev = 0;
    phystatus = read_mii(sc, phyaddr, 1);

    /* Read out PHY info if extended registers are available */
    if (phystatus & 1) {  
            tmp1 = read_mii(sc, phyaddr, 2);
            tmp2 = read_mii(sc, phyaddr, 3);

            sc->phydev.vendor = (tmp1 << 6) | ((tmp2 >> 10) & 0x3F);
            sc->phydev.rev = tmp2 & 0xF;
            sc->phydev.device = (tmp2 >> 4) & 0x3F;
    }

    /* Force to 10 mbit half duplex if the 10/100 MAC is used with a 1000 PHY */
    if (((sc->gb) && !(sc->gbit_mac)) || !((phyctrl >> 12) & 1)) {
        write_mii(sc, phyaddr, 0, sc->sp << 13);

        /* check if marvell 88EE1111 PHY. Needs special reset handling */
        if ((phystatus & 1) && (sc->phydev.vendor == 0x005043) &&
            (sc->phydev.device == 0x0C))
            write_mii(sc, phyaddr, 0, 0x8000);

        sc->gb = 0;
        sc->sp = 0;
        sc->fd = 0;
    }
    while ((read_mii(sc, phyaddr, 0)) & 0x8000) {}

    if (sc->greth_rst) {
        /* Reset ON */
        regs->ctrl = GRETH_CTRL_RST | GRETH_CTRL_DD | GRETH_CTRL_ED;
        for (i = 0; i < 100 && (regs->ctrl & GRETH_CTRL_RST); i++)
            ;
    }
    /* Reset OFF. Set mode matching PHY settings. */
    speed = (sc->gb << 8) | (sc->sp << 7) | (sc->fd << 4);
    regs->ctrl = GRETH_CTRL_DD | sc->edcl_dis | speed;

    /* Initialize rx/tx descriptor table pointers. Due to alignment we 
     * always allocate maximum table size.
     */
    sc->txdesc = (greth_rxtxdesc *) almalloc(0x800, 0x400);
    sc->rxdesc = (greth_rxtxdesc *) &sc->txdesc[128];
    sc->tx_ptr = 0;
    sc->tx_dptr = 0;
    sc->tx_cnt = 0;
    sc->rx_ptr = 0;

    /* Translate the Descriptor DMA table base address into an address that
     * the GRETH core can understand
     */
    drvmgr_translate_check(
        sc->dev,
        CPUMEM_TO_DMA,
        (void *)sc->txdesc,
        (void **)&sc->txdesc_remote,
        0x800);
    sc->rxdesc_remote = sc->txdesc_remote + 0x400;
    regs->txdesc = (int) sc->txdesc_remote;
    regs->rxdesc = (int) sc->rxdesc_remote;

    sc->rxmbuf = grlib_calloc(sc->rxbufs, sizeof(*sc->rxmbuf));
    sc->txmbuf = grlib_calloc(sc->txbufs, sizeof(*sc->txmbuf));

    for (i = 0; i < sc->txbufs; i++)
      {
        sc->txdesc[i].ctrl = 0;
        if (!(sc->gbit_mac)) {
            drvmgr_translate_check(
                sc->dev, 
                CPUMEM_TO_DMA,
                (void *)grlib_malloc(GRETH_MAXBUF_LEN),
                (void **)&sc->txdesc[i].addr,
                GRETH_MAXBUF_LEN);
        }
#ifdef GRETH_DEBUG
              /* printf("TXBUF: %08x\n", (int) sc->txdesc[i].addr); */
#endif
      }
    for (i = 0; i < sc->rxbufs; i++)
      {
         MGETHDR (m, M_WAIT, MT_DATA);
          MCLGET (m, M_WAIT);
          if (sc->gbit_mac)
                  m->m_data += 2;
	  m->m_pkthdr.rcvif = &sc->arpcom.ac_if;
          sc->rxmbuf[i] = m;
          drvmgr_translate_check(
            sc->dev,
            CPUMEM_TO_DMA,
            (void *)mtod(m, uint32_t *),
            (void **)&sc->rxdesc[i].addr,
            GRETH_MAXBUF_LEN);
          sc->rxdesc[i].ctrl = GRETH_RXD_ENABLE | GRETH_RXD_IRQ;
#ifdef GRETH_DEBUG
/* 	  printf("RXBUF: %08x\n", (int) sc->rxdesc[i].addr); */
#endif
      }
    sc->rxdesc[sc->rxbufs - 1].ctrl |= GRETH_RXD_WRAP;

    /* set ethernet address.  */
    regs->mac_addr_msb = 
      sc->arpcom.ac_enaddr[0] << 8 | sc->arpcom.ac_enaddr[1];
    regs->mac_addr_lsb = 
      sc->arpcom.ac_enaddr[2] << 24 | sc->arpcom.ac_enaddr[3] << 16 |
      sc->arpcom.ac_enaddr[4] << 8 | sc->arpcom.ac_enaddr[5];

    if ( sc->rxbufs < 10 ) {
        sc->tx_int_gen = sc->tx_int_gen_cur = 1;
    }else{
        sc->tx_int_gen = sc->tx_int_gen_cur = sc->txbufs/2;
    }
    sc->next_tx_mbuf = NULL;

    if ( !sc->gbit_mac )
        sc->max_fragsize = 1;

    /* clear all pending interrupts */
    regs->status = 0xffffffff;

    /* install interrupt handler */
    drvmgr_interrupt_register(sc->dev, 0, "greth", greth_interrupt, sc);

    regs->ctrl |= GRETH_CTRL_RXEN | GRETH_CTRL_RXIRQ;

    print_init_info(sc);
}

#ifdef CPU_U32_FIX

/*
 * Routine to align the received packet so that the ip header
 * is on a 32-bit boundary. Necessary for cpu's that do not
 * allow unaligned loads and stores and when the 32-bit DMA
 * mode is used.
 *
 * Transfers are done on word basis to avoid possibly slow byte
 * and half-word writes.
 */

void ipalign(struct mbuf *m)
{
  unsigned int *first, *last, data;
  unsigned int tmp = 0;

  if ((((int) m->m_data) & 2) && (m->m_len)) {
    last = (unsigned int *) ((((int) m->m_data) + m->m_len + 8) & ~3);
    first = (unsigned int *) (((int) m->m_data) & ~3);
		/* tmp = *first << 16; */
		asm volatile (" lda [%1] 1, %0\n" : "=r"(tmp) : "r"(first) );
		tmp = tmp << 16;
    first++;
    do {
			/* When snooping is not available the LDA instruction must be used
			 * to avoid the cache to return an illegal value.
			 ** Load with forced cache miss
			 * data = *first; 
			 */
      asm volatile (" lda [%1] 1, %0\n" : "=r"(data) : "r"(first) );
      *first = tmp | (data >> 16);
      tmp = data << 16;
      first++;
    } while (first <= last);

    m->m_data = (caddr_t)(((int) m->m_data) + 2);
  }
}
#endif

static void
greth_Daemon (void *arg)
{
    struct ether_header *eh;
    struct greth_softc *dp = (struct greth_softc *) arg;
    struct ifnet *ifp = &dp->arpcom.ac_if;
    struct mbuf *m;
    unsigned int len, len_status, bad;
    rtems_event_set events;
    SPIN_IRQFLAGS(flags);
    int first;
    int tmp;
    unsigned int addr;

    for (;;)
      {
        rtems_bsdnet_event_receive (INTERRUPT_EVENT | GRETH_TX_WAIT_EVENT,
                                    RTEMS_WAIT | RTEMS_EVENT_ANY,
                                    RTEMS_NO_TIMEOUT, &events);
        
        if ( events & GRETH_TX_WAIT_EVENT ){
            /* TX interrupt.
             * We only end up here when all TX descriptors has been used,
             * and 
             */
            if ( dp->gbit_mac )
                greth_process_tx_gbit(dp);
            else
                greth_process_tx(dp);
            
            /* If we didn't get a RX interrupt we don't process it */
            if ( (events & INTERRUPT_EVENT) == 0 )
                continue;
        }
        
        
#ifdef GRETH_ETH_DEBUG
    printf ("r\n");
#endif
    first=1;
    /* Scan for Received packets */
again:
    while (!((len_status =
		    GRETH_MEM_LOAD(&dp->rxdesc[dp->rx_ptr].ctrl)) & GRETH_RXD_ENABLE))
	    {
                    bad = 0;
                    if (len_status & GRETH_RXD_TOOLONG)
                    {
                            dp->rxLengthError++;
                            bad = 1;
                    }
                    if (len_status & GRETH_RXD_DRIBBLE)
                    {
                            dp->rxNonOctet++;
                            bad = 1;
                    }
                    if (len_status & GRETH_RXD_CRCERR)
                    {
                            dp->rxBadCRC++;
                            bad = 1;
                    }
                    if (len_status & GRETH_RXD_OVERRUN)
                    {
                            dp->rxOverrun++;
                            bad = 1;
                    }
                    if (len_status & GRETH_RXD_LENERR)
                    {
                            dp->rxLengthError++;
                            bad = 1;
                    }
                    if (!bad)
                    {
                            /* pass on the packet in the receive buffer */
                            len = len_status & 0x7FF;
                            m = dp->rxmbuf[dp->rx_ptr];
#ifdef GRETH_DEBUG
                            int i;
                            printf("RX: 0x%08x, Len: %d : ", (int) m->m_data, len);
                            for (i=0; i<len; i++)
                                    printf("%x%x", (m->m_data[i] >> 4) & 0x0ff, m->m_data[i] & 0x0ff);
                            printf("\n");
#endif
                            m->m_len = m->m_pkthdr.len =
                                    len - sizeof (struct ether_header);

                            eh = mtod (m, struct ether_header *);

                            m->m_data += sizeof (struct ether_header);
#ifdef CPU_U32_FIX
                            if(!dp->gbit_mac) {
                                    /* OVERRIDE CACHED ETHERNET HEADER FOR NON-SNOOPING SYSTEMS */
                                    addr = (unsigned int)eh;
                                    asm volatile (" lda [%1] 1, %0\n" : "=r"(tmp) : "r"(addr) );
                                    addr+=4;
                                    asm volatile (" lda [%1] 1, %0\n" : "=r"(tmp) : "r"(addr) );
                                    addr+=4;
                                    asm volatile (" lda [%1] 1, %0\n" : "=r"(tmp) : "r"(addr) );
                                    addr+=4;
                                    asm volatile (" lda [%1] 1, %0\n" : "=r"(tmp) : "r"(addr) );

                                    ipalign(m);	/* Align packet on 32-bit boundary */
                            }
#endif
/*
                            if(!(dp->gbit_mac) && !CPU_SPARC_HAS_SNOOPING) {
                                    rtems_cache_invalidate_entire_data();
                            }
*/
                            ether_input (ifp, eh, m);
                            MGETHDR (m, M_WAIT, MT_DATA);
                            MCLGET (m, M_WAIT);
                            if (dp->gbit_mac)
                                    m->m_data += 2;
                            dp->rxmbuf[dp->rx_ptr] = m;
                            m->m_pkthdr.rcvif = ifp;
                            drvmgr_translate_check(
                                dp->dev,
                                CPUMEM_TO_DMA,
                                (void *)mtod (m, uint32_t *),
                                (void **)&dp->rxdesc[dp->rx_ptr].addr,
                                GRETH_MAXBUF_LEN);
                            dp->rxPackets++;
                    }
                    if (dp->rx_ptr == dp->rxbufs - 1) {
                            dp->rxdesc[dp->rx_ptr].ctrl = GRETH_RXD_ENABLE | GRETH_RXD_IRQ | GRETH_RXD_WRAP;
                    } else {
                            dp->rxdesc[dp->rx_ptr].ctrl = GRETH_RXD_ENABLE | GRETH_RXD_IRQ;
                    }
                    SPIN_LOCK_IRQ(&dp->devlock, flags);
                    dp->regs->ctrl |= GRETH_CTRL_RXEN;
                    SPIN_UNLOCK_IRQ(&dp->devlock, flags);
                    dp->rx_ptr = (dp->rx_ptr + 1) % dp->rxbufs;
            }

        /* Always scan twice to avoid deadlock */
        if ( first ){
            first=0;
            SPIN_LOCK_IRQ(&dp->devlock, flags);
            dp->regs->ctrl |= GRETH_CTRL_RXIRQ;
            SPIN_UNLOCK_IRQ(&dp->devlock, flags);
            goto again;
        }

      }
}

static int
sendpacket (struct ifnet *ifp, struct mbuf *m)
{
    struct greth_softc *dp = ifp->if_softc;
    unsigned char *temp;
    struct mbuf *n;
    unsigned int len;
    SPIN_IRQFLAGS(flags);

    /*
     * Is there a free descriptor available?
     */
    if (GRETH_MEM_LOAD(&dp->txdesc[dp->tx_ptr].ctrl) & GRETH_TXD_ENABLE){
            /* No. */
            return 1;
    }
    
    /* Remember head of chain */
    n = m;

    len = 0;
    temp = (unsigned char *) GRETH_MEM_LOAD(&dp->txdesc[dp->tx_ptr].addr);
    drvmgr_translate(dp->dev, CPUMEM_FROM_DMA, (void *)temp, (void **)&temp);
#ifdef GRETH_DEBUG
    printf("TXD: 0x%08x : BUF: 0x%08x\n", (int) m->m_data, (int) temp);
#endif
    for (;;)
    {
#ifdef GRETH_DEBUG
            int i;
            printf("MBUF: 0x%08x : ", (int) m->m_data);
            for (i=0;i<m->m_len;i++)
                    printf("%x%x", (m->m_data[i] >> 4) & 0x0ff, m->m_data[i] & 0x0ff);
            printf("\n");
#endif
            len += m->m_len;
            if (len <= RBUF_SIZE)
                    memcpy ((void *) temp, (char *) m->m_data, m->m_len);
            temp += m->m_len;
            if ((m = m->m_next) == NULL)
                    break;
    }

    m_freem (n);

    /* don't send long packets */

    if (len <= GRETH_MAXBUF_LEN) {
            if (dp->tx_ptr < dp->txbufs-1) {
                    dp->txdesc[dp->tx_ptr].ctrl = GRETH_TXD_IRQ |
                                                  GRETH_TXD_ENABLE | len;
            } else {
                    dp->txdesc[dp->tx_ptr].ctrl = GRETH_TXD_IRQ |
                            GRETH_TXD_WRAP | GRETH_TXD_ENABLE | len;
            }
            dp->tx_ptr = (dp->tx_ptr + 1) % dp->txbufs;
            SPIN_LOCK_IRQ(&dp->devlock, flags);
            dp->regs->ctrl = dp->regs->ctrl | GRETH_CTRL_TXEN;
            SPIN_UNLOCK_IRQ(&dp->devlock, flags);
    }

    return 0;
}


static int
sendpacket_gbit (struct ifnet *ifp, struct mbuf *m)
{
        struct greth_softc *dp = ifp->if_softc;
        unsigned int len;
        
        unsigned int ctrl;
        int frags;
        struct mbuf *mtmp;
        int int_en;
        SPIN_IRQFLAGS(flags);

        len = 0;
#ifdef GRETH_DEBUG
        printf("TXD: 0x%08x\n", (int) m->m_data);
#endif
        /* Get number of fragments too see if we have enough
         * resources.
         */
        frags=1;
        mtmp=m;
        while(mtmp->m_next){
            frags++;
            mtmp = mtmp->m_next;
        }

        if ( frags > dp->max_fragsize ) 
            dp->max_fragsize = frags;
        
        if ( frags > dp->txbufs ){
            printf("GRETH: MBUF-chain cannot be sent. Increase descriptor count.\n");
            return -1;
        }
        
        if ( frags > (dp->txbufs-dp->tx_cnt) ){
            /* Return number of fragments */
            return frags;
        }
        
        
        /* Enable interrupt from descriptor every tx_int_gen
         * descriptor. Typically every 16 descriptor. This
         * is only to reduce the number of interrupts during
         * heavy load.
         */
        dp->tx_int_gen_cur-=frags;
        if ( dp->tx_int_gen_cur <= 0 ){
            dp->tx_int_gen_cur = dp->tx_int_gen;
            int_en = GRETH_TXD_IRQ;
        }else{
            int_en = 0;
        }
        
        /* At this stage we know that enough descriptors are available */
        for (;;)
        {
                
#ifdef GRETH_DEBUG
            int i;
            printf("MBUF: 0x%08x, Len: %d : ", (int) m->m_data, m->m_len);
            for (i=0; i<m->m_len; i++)
                printf("%x%x", (m->m_data[i] >> 4) & 0x0ff, m->m_data[i] & 0x0ff);
            printf("\n");
#endif
            len += m->m_len;
            drvmgr_translate_check(
                dp->dev,
                CPUMEM_TO_DMA,
                (void *)(uint32_t *)m->m_data,
                (void **)&dp->txdesc[dp->tx_ptr].addr,
                m->m_len);

            /* Wrap around? */
            if (dp->tx_ptr < dp->txbufs-1) {
                ctrl = GRETH_TXD_ENABLE;
            }else{
                ctrl = GRETH_TXD_ENABLE | GRETH_TXD_WRAP;
            }

            /* Enable Descriptor */  
            if ((m->m_next) == NULL) {
                dp->txdesc[dp->tx_ptr].ctrl = ctrl | int_en | m->m_len;
                break;
            }else{
                dp->txdesc[dp->tx_ptr].ctrl = GRETH_TXD_MORE | ctrl | int_en | m->m_len;
            }

            /* Next */
            dp->txmbuf[dp->tx_ptr] = m;
            dp->tx_ptr = (dp->tx_ptr + 1) % dp->txbufs;
            dp->tx_cnt++;
            m = m->m_next;
        }
        dp->txmbuf[dp->tx_ptr] = m;
        dp->tx_ptr = (dp->tx_ptr + 1) % dp->txbufs;
        dp->tx_cnt++;
      
        /* Tell Hardware about newly enabled descriptor */
        SPIN_LOCK_IRQ(&dp->devlock, flags);
        dp->regs->ctrl = dp->regs->ctrl | GRETH_CTRL_TXEN;
        SPIN_UNLOCK_IRQ(&dp->devlock, flags);

        return 0;
}

int greth_process_tx_gbit(struct greth_softc *sc)
{
    struct ifnet *ifp = &sc->arpcom.ac_if;
    struct mbuf *m;
    SPIN_IRQFLAGS(flags);
    int first=1;

    /*
     * Send packets till queue is empty
     */
    for (;;){
        /* Reap Sent packets */
        while((sc->tx_cnt > 0) && !(GRETH_MEM_LOAD(&sc->txdesc[sc->tx_dptr].ctrl) & GRETH_TXD_ENABLE)) {
            m_free(sc->txmbuf[sc->tx_dptr]);
            sc->tx_dptr = (sc->tx_dptr + 1) % sc->txbufs;
            sc->tx_cnt--;
        }
        
        if ( sc->next_tx_mbuf ){
            /* Get packet we tried but faild to transmit last time */
            m = sc->next_tx_mbuf;
            sc->next_tx_mbuf = NULL; /* Mark packet taken */
        }else{
            /*
             * Get the next mbuf chain to transmit from Stack.
             */
            IF_DEQUEUE (&ifp->if_snd, m);
            if (!m){
                /* Hardware has sent all schedule packets, this
                 * makes the stack enter at greth_start next time
                 * a packet is to be sent.
                 */
                ifp->if_flags &= ~IFF_OACTIVE;
                break;
            }
        }

        /* Are there free descriptors available? */
        /* Try to send packet, if it a negative number is returned. */
        if ( (sc->tx_cnt >= sc->txbufs) || sendpacket_gbit(ifp, m) ){
            /* Not enough resources */
             
            /* Since we have taken the mbuf out of the "send chain"
             * we must remember to use that next time we come back.
             * or else we have dropped a packet.
             */
            sc->next_tx_mbuf = m;
            
            /* Not enough resources, enable interrupt for transmissions
             * this way we will be informed when more TX-descriptors are 
             * available.
             */
            if ( first ){
                first = 0;
                SPIN_LOCK_IRQ(&sc->devlock, flags);
                ifp->if_flags |= IFF_OACTIVE;
                sc->regs->ctrl |= GRETH_CTRL_TXIRQ;
                SPIN_UNLOCK_IRQ(&sc->devlock, flags);
                
                /* We must check again to be sure that we didn't 
                 * miss an interrupt (if a packet was sent just before
                 * enabling interrupts)
                 */
                continue;
            }

            return -1;
        }else{
            /* Sent Ok, proceed to process more packets if available */
        }
    }
    return 0;
}

int greth_process_tx(struct greth_softc *sc)
{
    struct ifnet *ifp = &sc->arpcom.ac_if;
    struct mbuf *m;
    SPIN_IRQFLAGS(flags);
    int first=1;

    /*
     * Send packets till queue is empty
     */
    for (;;){
        if ( sc->next_tx_mbuf ){
            /* Get packet we tried but failed to transmit last time */
            m = sc->next_tx_mbuf;
            sc->next_tx_mbuf = NULL; /* Mark packet taken */
        }else{
            /*
             * Get the next mbuf chain to transmit from Stack.
             */
            IF_DEQUEUE (&ifp->if_snd, m);
            if (!m){
                /* Hardware has sent all schedule packets, this
                 * makes the stack enter at greth_start next time
                 * a packet is to be sent.
                 */
                ifp->if_flags &= ~IFF_OACTIVE;
                break;
            }
        }

        /* Try to send packet, failed if it a non-zero number is returned. */
        if ( sendpacket(ifp, m) ){
            /* Not enough resources */
             
            /* Since we have taken the mbuf out of the "send chain"
             * we must remember to use that next time we come back.
             * or else we have dropped a packet.
             */
            sc->next_tx_mbuf = m;
            
            /* Not enough resources, enable interrupt for transmissions
             * this way we will be informed when more TX-descriptors are 
             * available.
             */
            if ( first ){
                first = 0;
                SPIN_LOCK_IRQ(&sc->devlock, flags);
                ifp->if_flags |= IFF_OACTIVE;
                sc->regs->ctrl |= GRETH_CTRL_TXIRQ;
                SPIN_UNLOCK_IRQ(&sc->devlock, flags);

                /* We must check again to be sure that we didn't 
                 * miss an interrupt (if a packet was sent just before
                 * enabling interrupts)
                 */
                continue;
            }

            return -1;
        }else{
            /* Sent Ok, proceed to process more packets if available */
        }
    }
    return 0;
}

static void
greth_start (struct ifnet *ifp)
{
    struct greth_softc *sc = ifp->if_softc;
    
    if ( ifp->if_flags & IFF_OACTIVE )
            return;
    
    if ( sc->gbit_mac ){
        /* No use trying to handle this if we are waiting on GRETH
         * to send the previously scheduled packets.
         */
        
        greth_process_tx_gbit(sc);
    }else{
        greth_process_tx(sc);
    }
    
}

/*
 * Initialize and start the device
 */
static void
greth_init (void *arg)
{
    struct greth_softc *sc = arg;
    struct ifnet *ifp = &sc->arpcom.ac_if;
    char name[4] = {'E', 'T', 'H', '0'};

    if (sc->daemonTid == 0)
      {
          /*
           * Start driver tasks
           */
          name[3] += sc->minor;
          sc->daemonTid = rtems_bsdnet_newproc (name, 4096,
                                                greth_Daemon, sc);

          /*
           * Set up GRETH hardware
           */
          greth_initialize_hardware (sc);
      }

    /*
     * Setup promiscous/multi-cast MAC address filters if user enabled it
     */
    greth_mac_filter_set(sc);

    /*
     * Tell the world that we're running.
     */
    ifp->if_flags |= IFF_RUNNING;
}

/*
 * Stop the device
 */
static void
greth_stop (struct greth_softc *sc)
{
    struct ifnet *ifp = &sc->arpcom.ac_if;
    SPIN_IRQFLAGS(flags);
    unsigned int speed;

    SPIN_LOCK_IRQ(&sc->devlock, flags);
    ifp->if_flags &= ~IFF_RUNNING;

    speed = sc->regs->ctrl & (GRETH_CTRL_GB | GRETH_CTRL_SP | GRETH_CTRL_FULLD);

    /* RX/TX OFF */
    sc->regs->ctrl = GRETH_CTRL_DD | GRETH_CTRL_ED | speed;
    /* Reset ON */
    if (sc->greth_rst)
        sc->regs->ctrl = GRETH_CTRL_RST | GRETH_CTRL_DD | GRETH_CTRL_ED | speed;
    /* Reset OFF and restore link settings previously detected if any */
    sc->regs->ctrl = GRETH_CTRL_DD | sc->edcl_dis | speed;
    SPIN_UNLOCK_IRQ(&sc->devlock, flags);

    sc->next_tx_mbuf = NULL;
}


/*
 * Show interface statistics
 */
static void
greth_stats (struct greth_softc *sc)
{
  printf ("      Rx Interrupts:%-8lu", sc->rxInterrupts);
  printf ("      Rx Packets:%-8lu", sc->rxPackets);
  printf ("          Length:%-8lu", sc->rxLengthError);
  printf ("       Non-octet:%-8lu\n", sc->rxNonOctet);
  printf ("            Bad CRC:%-8lu", sc->rxBadCRC);
  printf ("         Overrun:%-8lu", sc->rxOverrun);
  printf ("      Tx Interrupts:%-8lu", sc->txInterrupts);
  printf ("      Maximal Frags:%-8d", sc->max_fragsize);
  printf ("      GBIT MAC:%-8d", sc->gbit_mac);
}

/*
 * Driver ioctl handler
 */
static int
greth_ioctl (struct ifnet *ifp, ioctl_command_t command, caddr_t data)
{
    struct greth_softc *sc = ifp->if_softc;
    int error = 0;
    struct ifreq *ifr;

    switch (command)
      {
      case SIOCGIFADDR:
      case SIOCSIFADDR:
	  ether_ioctl (ifp, command, data);
	  break;

      case SIOCSIFFLAGS:
	  switch (ifp->if_flags & (IFF_UP | IFF_RUNNING))
	    {
	    case IFF_RUNNING:
		greth_stop (sc);
                break;

	    case IFF_UP:
		greth_init (sc);
		break;

	    case IFF_UP | IFF_RUNNING:
		greth_stop (sc);
		greth_init (sc);
		break;
       default:
		break;
	    }
	  break;

      case SIO_RTEMS_SHOW_STATS:
	  greth_stats (sc);
	  break;

	  /*
	   * Multicast commands: Enabling/disabling filtering of MAC addresses
	   */
      case SIOCADDMULTI:
      case SIOCDELMULTI:
      ifr = (struct ifreq *)data;
      if (command == SIOCADDMULTI) {
        error = ether_addmulti(ifr, &sc->arpcom);
      } else {
        error = ether_delmulti(ifr, &sc->arpcom);
      }
      if (error == ENETRESET) {
        error = greth_mac_filter_set(sc);
      }
      break;

      default:
	  error = EINVAL;
	  break;
      }

    return error;
}

/*
 * Attach an GRETH driver to the system
 */
static int
greth_interface_driver_attach (
    struct rtems_bsdnet_ifconfig *config,
    int attach
    )
{
    struct greth_softc *sc;
    struct ifnet *ifp;
    int mtu;
    int unitNumber;
    char *unitName;
    
      /* parse driver name */
    if ((unitNumber = rtems_bsdnet_parse_driver_name (config, &unitName)) < 0)
	return 0;

    sc = config->drv_ctrl;
    ifp = &sc->arpcom.ac_if;
#ifdef GRETH_DEBUG
    printf("GRETH[%d]: %s, sc %p, dev %p on %s\n", unitNumber, config->ip_address, sc, sc->dev, sc->dev->parent->dev->name);
#endif
    if (config->hardware_address)
      {
	  memcpy (sc->arpcom.ac_enaddr, config->hardware_address,
		  ETHER_ADDR_LEN);
      }
    else
      {
	  memset (sc->arpcom.ac_enaddr, 0x08, ETHER_ADDR_LEN);
      }

    if (config->mtu)
	mtu = config->mtu;
    else
	mtu = ETHERMTU;

    sc->acceptBroadcast = !config->ignore_broadcast;

    /*
     * Set up network interface values
     */
    ifp->if_softc = sc;
    ifp->if_unit = unitNumber;
    ifp->if_name = unitName;
    ifp->if_mtu = mtu;
    ifp->if_init = greth_init;
    ifp->if_ioctl = greth_ioctl;
    ifp->if_start = greth_start;
    ifp->if_output = ether_output;
    ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX;
    if (sc->mc_available)
        ifp->if_flags |= IFF_MULTICAST;
    if (ifp->if_snd.ifq_maxlen == 0)
	ifp->if_snd.ifq_maxlen = ifqmaxlen;

    /*
     * Attach the interface
     */
    if_attach (ifp);
    ether_ifattach (ifp);

#ifdef GRETH_DEBUG
    printf ("GRETH : driver has been attached\n");
#endif
    return 1;
}

/******************* Driver manager interface ***********************/

/* Driver prototypes */
int greth_register_io(rtems_device_major_number *m);
int greth_device_init(struct greth_softc *sc);
int network_interface_add(struct rtems_bsdnet_ifconfig *interface);

#ifdef GRETH_INFO_AVAIL
static int greth_info(
	struct drvmgr_dev *dev,
	void (*print_line)(void *p, char *str),
	void *p, int argc, char *argv[]);
#define GRETH_INFO_FUNC greth_info
#else
#define GRETH_INFO_FUNC NULL
#endif

int greth_init2(struct drvmgr_dev *dev);
int greth_init3(struct drvmgr_dev *dev);

struct drvmgr_drv_ops greth_ops = 
{
	.init	=
		{
			NULL,
			greth_init2,
			greth_init3,
			NULL
		},
	.remove = NULL,
	.info = GRETH_INFO_FUNC,
};

struct amba_dev_id greth_ids[] = 
{
	{VENDOR_GAISLER, GAISLER_ETHMAC},
	{0, 0}		/* Mark end of table */
};

struct amba_drv_info greth_drv_info =
{
	{
		DRVMGR_OBJ_DRV,			/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_AMBAPP_GAISLER_GRETH_ID,	/* Driver ID */
		"GRETH_DRV",			/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,		/* Bus Type */
		&greth_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		0,
	},
	&greth_ids[0]
};

void greth_register_drv (void)
{
	DBG("Registering GRETH driver\n");
	drvmgr_drv_register(&greth_drv_info.general);
}

int greth_init2(struct drvmgr_dev *dev)
{
	struct greth_softc *priv;

	DBG("GRETH[%d] on bus %s\n", dev->minor_drv, dev->parent->dev->name);
	priv = dev->priv = grlib_calloc(1, sizeof(*priv));
	if ( !priv )
		return DRVMGR_NOMEM;
	priv->dev = dev;

	/* This core will not find other cores, so we wait for init3() */

	return DRVMGR_OK;
}

int greth_init3(struct drvmgr_dev *dev)
{
    struct greth_softc *sc;
    struct rtems_bsdnet_ifconfig *ifp;
    rtems_status_code status;

    sc = dev->priv;
    sprintf(sc->devName, "gr_eth%d", (dev->minor_drv+1));

    /* Init GRETH device */
    if ( greth_device_init(sc) ) {
        printk("GRETH: Failed to init device\n");
        return DRVMGR_FAIL;
    }

    /* Initialize Spin-lock for GRSPW Device. This is to protect
     * CTRL and DMACTRL registers from ISR.
     */
    SPIN_INIT(&sc->devlock, sc->devName);

    /* Register GRETH device as an Network interface */
    ifp = grlib_calloc(1, sizeof(*ifp));

    ifp->name = sc->devName;
    ifp->drv_ctrl = sc;
    ifp->attach = greth_interface_driver_attach;

    status = network_interface_add(ifp);
    if (status != 0) {
        return DRVMGR_FAIL;
    }

    return DRVMGR_OK;
}

int greth_device_init(struct greth_softc *sc)
{
    struct amba_dev_info *ambadev;
    struct ambapp_core *pnpinfo;
    union drvmgr_key_value *value;
    unsigned int speed;

    /* Get device information from AMBA PnP information */
    ambadev = (struct amba_dev_info *)sc->dev->businfo;
    if ( ambadev == NULL ) {
        return -1;
    }
    pnpinfo = &ambadev->info;
    sc->regs = (greth_regs *)pnpinfo->apb_slv->start;
    sc->minor = sc->dev->minor_drv;
    sc->greth_rst = 1;

    /* Remember EDCL enabled/disable state before reset */
    sc->edcl_dis = sc->regs->ctrl & GRETH_CTRL_ED;

    /* Default is to inherit EDCL Disable bit from HW. User can force En/Dis */
    value = drvmgr_dev_key_get(sc->dev, "edclDis", DRVMGR_KT_INT);
    if ( value ) {
        /* Force EDCL mode. Has an effect later when GRETH+PHY is initialized */
        if (value->i > 0) {
            sc->edcl_dis = GRETH_CTRL_ED;
        } else {
            /* Default to avoid soft-reset the GRETH when EDCL is forced */
            sc->edcl_dis = 0;
            sc->greth_rst = 0;
	}
    }

    /* let user control soft-reset of GRETH (for debug) */
    value = drvmgr_dev_key_get(sc->dev, "soft-reset", DRVMGR_KT_INT);
    if ( value) {
        sc->greth_rst = value->i ? 1 : 0;
    }

    /* clear control register and reset NIC and keep current speed modes.
     * This should be done as quick as possible during startup, this is to
     * stop DMA transfers after a reboot.
     *
     * When EDCL is forced enabled reset is skipped, disabling RX/TX DMA is
     * is enough during debug.
     */
    speed = sc->regs->ctrl & (GRETH_CTRL_GB | GRETH_CTRL_SP | GRETH_CTRL_FULLD);
    sc->regs->ctrl = GRETH_CTRL_DD | GRETH_CTRL_ED | speed;
    if (sc->greth_rst)
        sc->regs->ctrl = GRETH_CTRL_RST | GRETH_CTRL_DD | GRETH_CTRL_ED | speed;
    sc->regs->ctrl = GRETH_CTRL_DD | sc->edcl_dis | speed;

    /* Configure driver by overriding default config with the bus resources 
     * configured by the user
     */
    sc->txbufs = 32;
    sc->rxbufs = 32;
    sc->phyaddr = -1;

    value = drvmgr_dev_key_get(sc->dev, "txDescs", DRVMGR_KT_INT);
    if ( value && (value->i <= 128) )
        sc->txbufs = value->i;

    value = drvmgr_dev_key_get(sc->dev, "rxDescs", DRVMGR_KT_INT);
    if ( value && (value->i <= 128) )
        sc->rxbufs = value->i;

    value = drvmgr_dev_key_get(sc->dev, "phyAdr", DRVMGR_KT_INT);
    if ( value && (value->i < 32) )
        sc->phyaddr = value->i;

    value = drvmgr_dev_key_get(sc->dev, "advModes", DRVMGR_KT_INT);
    if ( value )
        sc->advmodes = value->i;

    /* Check if multicast support is available */
    sc->mc_available = sc->regs->ctrl & GRETH_CTRL_MC;

    return 0;
}

#ifdef GRETH_INFO_AVAIL
static int greth_info(
	struct drvmgr_dev *dev,
	void (*print_line)(void *p, char *str),
	void *p, int argc, char *argv[])
{
	struct greth_softc *sc;
	char buf[64];

	if (dev->priv == NULL)
		return -DRVMGR_EINVAL;
	sc = dev->priv;

	sprintf(buf, "IFACE NAME:  %s", sc->devName);
	print_line(p, buf);
	sprintf(buf, "GBIT MAC:    %s", sc->gbit_mac ? "YES" : "NO");
	print_line(p, buf);

	return DRVMGR_OK;
}
#endif

#endif
