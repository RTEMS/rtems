/*
 * RTEMS driver for TULIP based Ethernet Controller
 *
 *  $Header$
 */



/* make sure we can identify the platform (is __i386 valid?) */
#if !defined(__PPC) && !defined(__i386)
#   error "unknown platform:  should be __i386 or __PPC"
#endif


#include <bsp.h>
#if defined(i386)
#include <pcibios.h>
#endif

/* #include <stdlib.h> */
#include <stdio.h>
#include <stdarg.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>

#include <rtems/score/cpu.h>

#include <sys/param.h>
#include <sys/mbuf.h>

#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
 
#if defined(i386)
#include <irq.h>
#endif

#ifdef malloc
#undef malloc
#endif
#ifdef free
#undef free 
#endif

#define PCI_VENDOR_ID_DEC 0x1011
#define PCI_DEVICE_ID_DEC_TULIP_FAST 0x0009

#define IO_MASK  0x3
#define MEM_MASK  0xF

/* command and status registers, 32-bit access, only if MEMORY-ACCESS */
#define CSR0  0x00  /* bus mode register */
#define CSR1  0x02  /* transmit poll demand */
#define CSR2  0x04  /* receive poll demand */
#define CSR3  0x06  /* receive list base address */
#define CSR4  0x08  /* transmit list base address */
#define CSR5  0x0A  /* status register */
#define CSR6  0x0C  /* operation mode register */
#define CSR7  0x0E  /* interrupt mask register */
#define CSR8  0x10  /* missed frame counter */
#define CSR9  0x12  /* Ethernet ROM register */
#define CSR10 0x14  /* reserved */
#define CSR11 0x16  /* full-duplex register */
#define CSR12 0x18  /* SIA status register */
#define CSR13 0x1A
#define CSR14 0x1C
#define CSR15 0x1E  /* SIA general register */

#define DEC_REGISTER_SIZE    0x100   /* to reserve virtual memory */

#define RESET_CHIP   0x00000001
#if defined(__PPC)
#define CSR0_MODE    0x0020E002   
#define CSR6_INIT    0x0224c000   
#else
#define CSR0_MODE    0x01a08000   
#define CSR6_INIT    0x020c0000   
#endif
#define ROM_ADDRESS  0x00004800
#define CSR6_TX      0x00002000   
#define CSR6_TXRX    0x00002002   
#define IT_SETUP     0x0001a3ef   
#define CLEAR_IT     0xFFFFFFFF   
#define NO_IT        0x00000000   

#define SETUP_PACKET      0x08000000
#define END_OF_RING       0x02000000
#define CHAINED_ADDRESS   0x01000000
#define OWNED_BY_DEC21140 0x80000000
#define OWNED_BY_HOST     0x00000000
#define LAST_SEGMENT      0x40000000
#define FIRST_SEGMENT     0x20000000

#define NRXBUFS 8   /* number of receive buffers */
#define NTXBUFS 32  /* number of transmit buffers */

/* message descriptor entry */
struct MD {
    /* used by hardware */
    volatile unsigned32 status;
    volatile unsigned32 counts;
    unsigned32 buf1, buf2;  
    /* used by software */
    struct mbuf *m;
    struct MD *next;
};

static inline void write_descr_status(volatile struct MD *m, unsigned32 status) {
    st_le32(&(m->status), status);
}

static inline unsigned32 read_descr_status(volatile struct MD *m) {
    return ld_le32(&(m->status));
}

static inline void write_descr_counts(volatile struct MD *m, unsigned32 counts) {
    st_le32(&(m->counts), counts);
}

static inline unsigned32 read_descr_counts(volatile struct MD *m) {
    return ld_le32(&(m->counts));
}

static inline void set_chain_address(volatile struct MD *m, void *addr) {
    st_le32(&(m->buf2), LOCAL_TO_PCI(addr));
}

static inline void set_buffer_address(volatile struct MD *m, void *addr) {
    st_le32(&(m->buf1), LOCAL_TO_PCI(addr));
}

static inline void *get_buffer_address(volatile struct MD *m) {
    return (void *)INVERSE_LOCAL_TO_PCI(ld_le32(&(m->buf1)));
}

/*
 * Number of WDs supported by this driver
 */
#define NDECDRIVER  1

/*
 * Receive buffer size -- Allow for a full ethernet packet including CRC
 */
#define RBUF_SIZE   1520

#define ET_MINLEN 60        /* minimum message length */

/*
 * RTEMS event used by interrupt handler to signal driver tasks.
 * This must not be any of the events used by the network task synchronization.
 */
#define INTERRUPT_EVENT RTEMS_EVENT_1

/*
 * RTEMS event used to start transmit daemon.
 * This must not be the same as INTERRUPT_EVENT.
 */
#define START_TRANSMIT_EVENT    RTEMS_EVENT_2

#if (MCLBYTES < RBUF_SIZE)
# error "Driver must have MCLBYTES > RBUF_SIZE"
#endif

/*
 * Per-device data
 */
struct dec21140_softc {
    struct arpcom           arpcom;
#if defined(__PPC)
    int         irqInfo;
#else
    rtems_irq_connect_data  irqInfo;
#endif
    struct MD   *MDbase;
    char        *bufferBase;
    int         acceptBroadcast;
    rtems_id    rxDaemonTid;
    rtems_id    txDaemonTid;

    struct MD   *TxMD;
    struct MD   *SentTxMD;
    int         PendingTxCount;
    int         TxSuspended;

    unsigned32  port;
    unsigned32  *base;

    /*
    * Statistics
    */
    unsigned32  rxInterrupts;
    unsigned32  rxNotFirst;
    unsigned32  rxNotLast;
    unsigned32  rxGiant;
    unsigned32  rxNonOctet;
    unsigned32  rxRunt;
    unsigned32  rxBadCRC;
    unsigned32  rxOverrun;
    unsigned32  rxCollision;

    unsigned32  txInterrupts;
    unsigned32  txDeferred;
    unsigned32  txHeartbeat;
    unsigned32  txLateCollision;
    unsigned32  txRetryLimit;
    unsigned32  txUnderrun;
    unsigned32  txLostCarrier;
    unsigned32  txRawWait;
};

static struct dec21140_softc dec21140_softc[NDECDRIVER];
static rtems_interval ticks_per_second;

/* ================================================================ */

static inline void write_csr(unsigned32 *base, int csr, unsigned32 value) {
    synchronize_io();
    st_le32(base + csr, value);
}

static inline unsigned32 read_csr(unsigned32 *base, int csr) {
    synchronize_io();
    return ld_le32(base + csr);
}




/* ================================================================ */

/*
 * DEC21140 interrupt handler
 */
static rtems_isr dec21140Enet_interrupt_handler (rtems_vector_number v) {
    unsigned32 *tbase;
    unsigned32 status;
    struct dec21140_softc *sc;

#if NDECDRIVER == 1
    sc = &dec21140_softc[0];
#else
#   error "need to find dec21140_softc[i] based on vector number"
#endif
    tbase = sc->base ;

    /*
     * Read status
     */
    status = read_csr(tbase, CSR5);
    write_csr(tbase, CSR5, status);  /* clear the bits we've read */

    /*
     * severe error?
     */
    if (status & 0x0000230a){
        printk("FATAL ERROR in network driver:  CSR5=0x%08x\n", status);
    }
    /*
     * Frame received?
     */
    if (status & 0x000000c0){
        sc->rxInterrupts++;
        rtems_event_send (sc->rxDaemonTid, INTERRUPT_EVENT);
    }

    /*
     * Frame transmitted or transmit error?
     */
    if (status & 0x00000025) {
        if (status & 0x00000004) {
            sc->TxSuspended = 1;
        }
        sc->txInterrupts++;
        rtems_event_send (sc->txDaemonTid, INTERRUPT_EVENT);
    }
}

#if defined(__i386)
static void nopOn(const rtems_irq_connect_data* notUsed) {
    /*
     * code should be moved from dec21140Enet_initialize_hardware
     * to this location
     */
}

static int dec21140IsOn(const rtems_irq_connect_data* irq) {
    return BSP_irq_enabled_at_i8259s (irq->name);
}
#endif


/*
 * Initialize the ethernet hardware
 */
#define PPCBUG_HW_ADDR_STORAGE 0x1f2c
static void dec21140Enet_initialize_hardware (struct dec21140_softc *sc) {
    rtems_status_code st;
    unsigned32 *tbase;
    int i;
    char *cp, *setup_frm, *eaddrs; 
    unsigned char *buffer;
    struct MD *rmd;


    tbase = sc->base;

    /*
     * WARNING : First write in CSR6
     *           Then Reset the chip ( 1 in CSR0)
     */

    write_csr(tbase, CSR6, CSR6_INIT);  
    write_csr(tbase, CSR0, RESET_CHIP);  
    delay_in_bus_cycles(200);

    /*
     * Init CSR0
     */
    write_csr(tbase, CSR0, CSR0_MODE);  

    read_nvram(sc->arpcom.ac_enaddr, PPCBUG_HW_ADDR_STORAGE, 6);
 
#ifdef DEC_DEBUG
    printk("DC21140 %x:%x:%x:%x:%x:%x IRQ %d IO %x M %x .........\n",
            sc->arpcom.ac_enaddr[0], sc->arpcom.ac_enaddr[1],
            sc->arpcom.ac_enaddr[2], sc->arpcom.ac_enaddr[3],
            sc->arpcom.ac_enaddr[4], sc->arpcom.ac_enaddr[5],
            sc->irqInfo, sc->port, sc->base);
#endif
  
    /*
     * Init RX ring
     */

#if defined(__i386)
    cp = (char *)malloc((NRXBUFS + NTXBUFS) * (sizeof(struct MD) + RBUF_SIZE) + 
                        PG_SIZE);
    sc->bufferBase = cp;
    cp += (PG_SIZE - (int)cp) & MASK_OFFSET ;
    if (_CPU_is_paging_enabled()) {
        _CPU_change_memory_mapping_attribute
                (NULL, cp,
                (NRXBUFS + NTXBUFS) * (sizeof(struct MD) + RBUF_SIZE),
                PTE_CACHE_DISABLE | PTE_WRITABLE);
    }
#endif
#if defined(__PPC)
    cp = (char *)malloc((NRXBUFS + NTXBUFS)*(sizeof(struct MD)+ RBUF_SIZE) + 
                        4096);
#endif
    rmd = (struct MD *)cp;
    sc->MDbase = rmd;
    buffer = cp + ((NRXBUFS + NTXBUFS)*sizeof(struct MD));

    write_csr(tbase, CSR3, LOCAL_TO_PCI(sc->MDbase));
    for (i = 0 ; i < NRXBUFS; i++){
        struct mbuf *m;
        
        /* allocate an mbuf for each receive descriptor */
        MGETHDR (m, M_WAIT, MT_DATA);
        MCLGET (m, M_WAIT);
        m->m_pkthdr.rcvif = &sc->arpcom.ac_if;
        rmd->m = m;

        set_buffer_address(rmd, mtod(m, void *));
        if (i == NRXBUFS - 1) {
            write_descr_counts(rmd, END_OF_RING | RBUF_SIZE);
            rmd->next = sc->MDbase;
        } else {
            write_descr_counts(rmd, CHAINED_ADDRESS | RBUF_SIZE);
            set_chain_address(rmd, rmd + 1);
            rmd->next = rmd + 1;
        }
        write_descr_status(rmd, OWNED_BY_DEC21140);
        rmd++;
    }

    /*
     * Init TX ring
     */
    write_csr(tbase, CSR4, LOCAL_TO_PCI(rmd));
    for (i = 0 ; i < NTXBUFS; i++){
        set_buffer_address(rmd + i, buffer + NRXBUFS * RBUF_SIZE + i * RBUF_SIZE);
        write_descr_counts(rmd + i, FIRST_SEGMENT | LAST_SEGMENT | CHAINED_ADDRESS);
        if (i == NTXBUFS - 1) {
            set_chain_address(rmd + i, rmd);
            (rmd + i)->next = rmd;
        } else {
            set_chain_address(rmd + i, rmd + i + 1);
            (rmd + i)->next = rmd + i + 1;
        }
        write_descr_status(rmd + i, OWNED_BY_HOST);
    }

#if defined(__i386)
    sc->irqInfo.hdl = (rtems_irq_hdl)dec21140Enet_interrupt_handler;
    sc->irqInfo.on  = nopOn;
    sc->irqInfo.off = nopOn;
    sc->irqInfo.isOn = dec21140IsOn;  
    st = BSP_install_rtems_irq_handler (&sc->irqInfo);
    if (!st) {
        rtems_panic ("Can't attach DEC21140 interrupt handler for irq %d\n",
                        sc->irqInfo.name);
    }
#endif
#if defined(__PPC)
    {
        rtems_isr_entry old_handler;

        st = bsp_interrupt_catch(dec21140Enet_interrupt_handler, 
                                 IRQ_ETHERNET, &old_handler);
        if (st != RTEMS_SUCCESSFUL) {
            rtems_panic("Can't attach DEC21140 interrupt handler\n");
        }
        bsp_interrupt_enable(IRQ_ETHERNET, PRIORITY_ISA_INT);
    }
#endif

    /* no interrupts for now */
    write_csr(tbase, CSR7, NO_IT);

    /*
     * Build setup frame
     */
    setup_frm = get_buffer_address(rmd);
    eaddrs = (char *)(sc->arpcom.ac_enaddr);
    /* Fill the buffer with our physical address. */
    for (i = 1; i < 16; i++) {
        *setup_frm++ = eaddrs[0];
        *setup_frm++ = eaddrs[1];
        setup_frm += 2;
        *setup_frm++ = eaddrs[2];
        *setup_frm++ = eaddrs[3];
        setup_frm += 2;
        *setup_frm++ = eaddrs[4];
        *setup_frm++ = eaddrs[5];
        setup_frm += 2;
    }
    /* Add the broadcast address when doing perfect filtering */
    memset((char*)setup_frm, 0xff, 12);
    write_descr_counts(rmd, SETUP_PACKET | CHAINED_ADDRESS | 192);
    write_descr_status(rmd, OWNED_BY_DEC21140);
    /*
     * Start TX for setup frame
     */
    write_csr(tbase, CSR6, CSR6_INIT | CSR6_TX);

    write_csr(tbase, CSR1, 1);
    while (read_descr_status(rmd) & OWNED_BY_DEC21140);
    sc->SentTxMD = sc->TxMD = rmd + 1;
    sc->PendingTxCount = 0;
    sc->TxSuspended = 1;
    /*
     * Set up interrupts
     */
    write_csr(tbase, CSR5, IT_SETUP);
    write_csr(tbase, CSR7, IT_SETUP); 

    /*
     * Enable RX and TX
     */
    write_csr(tbase, CSR6, CSR6_INIT | CSR6_TXRX);

}

static void dec21140_rxDaemon (void *arg) {
    unsigned32 *tbase;
    struct dec21140_softc *sc = arg;
    struct ifnet *ifp = &sc->arpcom.ac_if;
    struct MD *rmd;
    unsigned32 len;
    unsigned32 rx_status;
    rtems_event_set events;

    tbase = sc->base;
    rmd = sc->MDbase;

    for (;;){

        rtems_bsdnet_event_receive (INTERRUPT_EVENT,
                                    RTEMS_WAIT | RTEMS_EVENT_ANY,
                                    RTEMS_NO_TIMEOUT,
                                    &events);
        while (((rx_status = read_descr_status(rmd)) & OWNED_BY_DEC21140) == 0) {
            struct ether_header *eh;
            struct mbuf *m = rmd->m;
            
            /* 
             *  packet is good if Error Summary = 0 and First Descriptor = 1
             *  and Last Descriptor = 1
             */
            if ((rx_status & 0x00008300) == 0x00000300) {
                /* pass on the packet in the mbuf */
                len = (rx_status >> 16) & 0x7ff;
                m->m_len = m->m_pkthdr.len = len - sizeof(struct ether_header);
                eh = mtod (m, struct ether_header *);
                m->m_data += sizeof(struct ether_header);
                ether_input (ifp, eh, m);

                /* get a new mbuf for the 21140 */
                MGETHDR (m, M_WAIT, MT_DATA);
                MCLGET (m, M_WAIT);
                m->m_pkthdr.rcvif = ifp;
                rmd->m = m;
                set_buffer_address(rmd, mtod(m, void *));
            } else {
                if ((rx_status & (1 << 9)) == 0) {
                    sc->rxNotFirst++;
                }
                if ((rx_status & (1 << 8)) == 0) {
                    sc->rxNotLast++;
                }
                if (rx_status & (1 << 7)) {
                    sc->rxGiant++;
                }
                if (rx_status & (1 << 2)) {
                    sc->rxNonOctet++;
                }
                if (rx_status & (1 << 11)) {
                    sc->rxRunt++;
                }
                if (rx_status & (1 << 1)) {
                    sc->rxBadCRC++;
                }
                if (rx_status & (1 << 14)) {
                    sc->rxOverrun++;
                }
                if (rx_status & (1 << 6)) {
                    sc->rxCollision++;
                }
            }
            
            /* give the descriptor back to the 21140 */
            write_descr_status(rmd, OWNED_BY_DEC21140);

            /* check for more ready descriptors */
            rmd = rmd->next;
        }
    } 
}

static void reap_sent_descriptors(struct dec21140_softc *sc) {
    struct MD *descr = sc->SentTxMD;
    unsigned32 *tbase = sc->base;
    unsigned32 tx_status = 0;
    struct mbuf *m, *n;

    while (sc->PendingTxCount > 0 && 
          ((tx_status = read_descr_status(descr)) & OWNED_BY_DEC21140) == 0 ) {
        for (m = descr->m; m; m = n) {
            MFREE(m, n);
        }
        if (read_descr_counts(descr) & LAST_SEGMENT) {
            if (tx_status & (1 << 0)) {
                sc->txDeferred++;
            }
            if (tx_status & (1 << 7)) {
                sc->txHeartbeat++;
            }
            if (tx_status & (1 << 9)) {
                sc->txLateCollision++;
            }
            if (tx_status & (1 << 8)) {
                sc->txRetryLimit++;
            }
            if (tx_status & (1 << 1)) {
                sc->txUnderrun++;
                write_csr(tbase, CSR1, 0x1); /* restart transmitter */
                /* this shouldn't happen - descriptor should still be
                   owned by 21140 */
                printk("ethernet chip underrun error\n");
            }
            if (tx_status & (1 << 10)) {
                sc->txLostCarrier++;
            }
        }
        descr = descr->next;
        sc->PendingTxCount--;
    }
    sc->SentTxMD = descr;
    /* check for Underflow and restart transmission */
    if ((tx_status & 0x80000002) == 0x80000002) {
        sc->txUnderrun++;
        write_csr(tbase, CSR1, 0x1); /* restart transmitter */
        printk("ethernet chip underrun error\n");
    }
}

static void sendpacket (volatile struct ifnet *ifp, struct mbuf *m) {
    struct mbuf *mbuf = m;
    struct dec21140_softc *sc = ifp->if_softc;
    struct MD *descr, *first, *last;
    unsigned32 *tbase = sc->base;
    int count = 0;

    first = last = descr = sc->TxMD;
    reap_sent_descriptors(sc);
    while (mbuf) {
        if (mbuf->m_len) {
            /* if no descriptor is available, wait for interrupt */
            while ((sc->PendingTxCount + count) == NTXBUFS) {
                rtems_event_set events;
                rtems_status_code result;

                if (sc->PendingTxCount == 0) {
                    printk("ERROR: too many segments to transmit in mbuf\n");
                }
                result = rtems_bsdnet_event_receive(INTERRUPT_EVENT,
                                                   RTEMS_WAIT | RTEMS_EVENT_ANY,
                                                   20 * ticks_per_second,
                                                   &events);
                if (result == RTEMS_TIMEOUT) {
                    printk("still waiting for tx descriptor in sendpacket\n");
                }
                reap_sent_descriptors(sc);
                if (sc->TxSuspended) {
                    sc->TxSuspended = 0;
                    write_csr(tbase, CSR1, 0x1);
                }
            }
            set_buffer_address(descr, mtod (mbuf, void *));
            write_descr_counts(descr, CHAINED_ADDRESS | mbuf->m_len);
            last = descr;
            if (descr != first) {
                write_descr_status(descr, OWNED_BY_DEC21140);
            }
            descr->m = 0;
            count++;
            descr = descr->next;
        }
        mbuf = mbuf->m_next;
    }
    write_descr_counts(first, read_descr_counts(first) | FIRST_SEGMENT);
    write_descr_counts(last, read_descr_counts(last) | LAST_SEGMENT);
    last->m = m;
    sc->TxMD = descr;
    synchronize_io();
    write_descr_status(first, OWNED_BY_DEC21140);
    sc->PendingTxCount += count;
    if (sc->TxSuspended) {
        sc->TxSuspended = 0;
        synchronize_io();
        write_csr(tbase, CSR1, 0x1);
    }
}

/*
 * Driver transmit daemon
 */
static void dec21140_txDaemon (void *arg) {
    struct dec21140_softc *sc = (struct dec21140_softc *)arg;
    volatile struct ifnet *ifp = &sc->arpcom.ac_if;
    unsigned32 *tbase = sc->base;
    struct mbuf *m;
    rtems_event_set events;

    for (;;) {
        /*
         * Wait for packet
         */

        rtems_bsdnet_event_receive (START_TRANSMIT_EVENT, 
                                    RTEMS_EVENT_ANY | RTEMS_WAIT, 
                                    RTEMS_NO_TIMEOUT, &events);

        /*
         * Send packets till queue is empty
         */
        for (;;) {
            /*
             * Get the next mbuf chain to transmit.
             */
            IF_DEQUEUE(&ifp->if_snd, m);
            if (!m) {
                break;
            }
            sendpacket (ifp, m);
        }
        ifp->if_flags &= ~IFF_OACTIVE;
    }
}   


static void dec21140_start (struct ifnet *ifp) {
    struct dec21140_softc *sc = ifp->if_softc;

    ifp->if_flags |= IFF_OACTIVE;
    rtems_event_send (sc->txDaemonTid, START_TRANSMIT_EVENT);
}

/*
 * Initialize and start the device
 */
static void dec21140_init (void *arg) {
    struct dec21140_softc *sc = arg;
    struct ifnet *ifp = &sc->arpcom.ac_if;

    if (sc->txDaemonTid == 0) {

        /*
         * Set up DEC21140 hardware
         */
        dec21140Enet_initialize_hardware (sc);

        /*
         * Start driver tasks
         */
#if NDECDRIVER == 1
        sc->rxDaemonTid = rtems_bsdnet_newproc ("DCrx", 4096,
                                                dec21140_rxDaemon, sc);
        sc->txDaemonTid = rtems_bsdnet_newproc ("DCtx", 4096,
                                                dec21140_txDaemon, sc);
#else
#   error "need to fix task IDs"
#endif
    }

    /*
    * Tell the world that we're running.
    */
    ifp->if_flags |= IFF_RUNNING;

}

/*
 * Stop the device
 */
static void dec21140_stop (struct dec21140_softc *sc) {
    unsigned32 *tbase;
    struct ifnet *ifp = &sc->arpcom.ac_if;

    ifp->if_flags &= ~IFF_RUNNING;

    /*
     * Stop the transmitter
     */
    tbase = sc->base;
    write_csr(tbase, CSR7, NO_IT);
    write_csr(tbase, CSR6, CSR6_INIT);
    free(sc->bufferBase);
}


/*
 * Show interface statistics
 */
static void dec21140_stats (struct dec21140_softc *sc) {
    printf ("      Rx Interrupts:%-8u", sc->rxInterrupts);
    printf ("       Not First:%-8u", sc->rxNotFirst);
    printf ("        Not Last:%-8u\n", sc->rxNotLast);
    printf ("              Giant:%-8u", sc->rxGiant);
    printf ("            Runt:%-8u", sc->rxRunt);
    printf ("       Non-octet:%-8u\n", sc->rxNonOctet);
    printf ("            Bad CRC:%-8u", sc->rxBadCRC);
    printf ("         Overrun:%-8u", sc->rxOverrun);
    printf ("       Collision:%-8u\n", sc->rxCollision);

    printf ("      Tx Interrupts:%-8u", sc->txInterrupts);
    printf ("        Deferred:%-8u", sc->txDeferred);
    printf (" Missed Hearbeat:%-8u\n", sc->txHeartbeat);
    printf ("         No Carrier:%-8u", sc->txLostCarrier);
    printf ("Retransmit Limit:%-8u", sc->txRetryLimit);
    printf ("  Late Collision:%-8u\n", sc->txLateCollision);
    printf ("           Underrun:%-8u", sc->txUnderrun);
    printf (" Raw output wait:%-8u\n", sc->txRawWait);
}

/*
 * Driver ioctl handler
 */
static int dec21140_ioctl (struct ifnet *ifp, int command, caddr_t data) {
    struct dec21140_softc *sc = ifp->if_softc;
    int error = 0;

    switch (command) {
    case SIOCGIFADDR:
    case SIOCSIFADDR:
        ether_ioctl (ifp, command, data);
        break;

    case SIOCSIFFLAGS:
        switch (ifp->if_flags & (IFF_UP | IFF_RUNNING)) {
        case IFF_RUNNING:
            dec21140_stop (sc);
            break;

        case IFF_UP:
            dec21140_init (sc);
            break;

        case IFF_UP | IFF_RUNNING:
            dec21140_stop (sc);
            dec21140_init (sc);
            break;

        default:
            break;
        }
        break;

    case SIO_RTEMS_SHOW_STATS:
        dec21140_stats (sc);
        break;
        
    /*
     * FIXME: All sorts of multicast commands need to be added here!
     */
    default:
        error = EINVAL;
        break;
    }
    return error;
}

/*
 * Attach an DEC21140 driver to the system
 */
int rtems_dec21140_driver_attach (struct rtems_bsdnet_ifconfig *config) {
    struct dec21140_softc *sc;
    struct ifnet *ifp;
    int mtu;
    int i;
#if defined(__i386)
    int signature;
    int value;
    char interrupt;
    int diag;
    
    /*
     * Initialise PCI module
     */
    if (pcib_init() == PCIB_ERR_NOTPRESENT) {
        rtems_panic("PCI BIOS not found !!");
    }
    
    /*
     * First, find a DEC board
     */
    if ((diag = pcib_find_by_devid(PCI_VENDOR_ID_DEC,
                       PCI_DEVICE_ID_DEC_TULIP_FAST,
                       0,
                       &signature)) != PCIB_ERR_SUCCESS) {
        rtems_panic("DEC PCI board not found !! (%d)\n", diag);
    } else {
        printk("DEC PCI Device found\n");
    }
#endif 
#if defined(__PPC)
    int sig;

    /* search PCI bus for Tulip chip */
    sig = pci_find_by_devid(PCI_VENDOR_ID_DEC, PCI_DEVICE_ID_DEC_TULIP_FAST, 0);
    if (sig == PCI_NOT_FOUND) {
        printk("PCI scan failed:  DEC 21140 not found\n");
        return 0;
    }

#endif
    
    /*
     * Find a free driver
     */
    for (i = 0 ; i < NDECDRIVER ; i++) {
        sc = &dec21140_softc[i];
        ifp = &sc->arpcom.ac_if;
        if (ifp->if_softc == NULL) {
            break;
        }
    }
    if (i >= NDECDRIVER) {
        printk ("Too many DEC drivers.\n");
        return 0;
    }

    /*
     * Process options
     */

    sc->port = pci_conf_read32(sig, PCI_BASE_ADDRESS_0) & ~IO_MASK;
    sc->base = (void *)
           PCI_TO_LOCAL(pci_conf_read32(sig, PCI_BASE_ADDRESS_1) & ~MEM_MASK);
    sc->irqInfo = pci_conf_read8(sig, 60);
#if defined(__i386)
    pcib_conf_read32(signature, 16, &value);
    sc->port = value & ~IO_MASK;
        
    pcib_conf_read32(signature, 20, &value);
    if (_CPU_is_paging_enabled()) {
        _CPU_map_phys_address(&(sc->base),
                (void *)(value & ~MEM_MASK),
                DEC_REGISTER_SIZE ,
                PTE_CACHE_DISABLE | PTE_WRITABLE);
    } else {
        sc->base = (unsigned32 *)(value & ~MEM_MASK);
    }
    
    pcib_conf_read8(signature, 60, &interrupt);
    sc->irqInfo.name = (rtems_irq_symbolic_name)interrupt;
#endif
    
    if (config->hardware_address) {
        memcpy(sc->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);
    } else {
        memset (sc->arpcom.ac_enaddr, 0x08, ETHER_ADDR_LEN);
    }
    if (config->mtu) {
        mtu = config->mtu;
    } else {
        mtu = ETHERMTU;
    }

    sc->acceptBroadcast = !config->ignore_broadcast;

    /*
     * Set up network interface values
     */
    ifp->if_softc = sc;
    ifp->if_unit = i + 1;
    ifp->if_name = "dc";
    ifp->if_mtu = mtu;
    ifp->if_init = dec21140_init;
    ifp->if_ioctl = dec21140_ioctl;
    ifp->if_start = dec21140_start;
    ifp->if_output = ether_output;
    ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX;
    if (ifp->if_snd.ifq_maxlen == 0) {
        ifp->if_snd.ifq_maxlen = ifqmaxlen;
    }

    /*
     * Attach the interface
     */
    if_attach (ifp);
    ether_ifattach (ifp);

    /*
     * Determine clock rate for timeout calculation
     */
    rtems_clock_get(RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticks_per_second);

    return 1;
};

