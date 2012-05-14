/*
 *       RTEMS NETWORK DRIVER FOR NATIONAL DP83932 `SONIC'
 *         SYSTEMS-ORIENTED NETWORK INTERFACE CONTROLLER
 *
 *                REUSABLE CHIP DRIVER CONFIGURATION
 *
 * References:
 *
 *  1) DP83932C-20/25/33 MHz SONIC(TM) Systems-Oriented Network Interface
 *     Controller data sheet.  TL/F/10492, RRD-B30M105, National Semiconductor,
 *     1995.
 *
 *  2) Software Driver Programmer's Guide for the DP83932 SONIC(TM),
 *     Application Note 746, Wesley Lee and Mike Lui, TL/F/11140,
 *     RRD-B30M75, National Semiconductor, March, 1991.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _SONIC_DP83932_
#define _SONIC_DP83932_


 /*
  * Debug levels
  *
  */

#define SONIC_DEBUG_NONE                0x0000
#define SONIC_DEBUG_ALL                 0xFFFF
#define SONIC_DEBUG_PRINT_REGISTERS     0x0001
#define SONIC_DEBUG_MEMORY              0x0002
#define SONIC_DEBUG_MEMORY_ALLOCATE     0x0004
#define SONIC_DEBUG_MEMORY_DESCRIPTORS  0x0008
#define SONIC_DEBUG_FRAGMENTS           0x0008
#define SONIC_DEBUG_CAM                 0x0010
#define SONIC_DEBUG_DESCRIPTORS         0x0020
#define SONIC_DEBUG_ERRORS              0x0040
#define SONIC_DEBUG_DUMP_TX_MBUFS       0x0080
#define SONIC_DEBUG_DUMP_RX_MBUFS       0x0100

#define SONIC_DEBUG_DUMP_MBUFS \
      (SONIC_DEBUG_DUMP_TX_MBUFS|SONIC_DEBUG_DUMP_RX_MBUFS)

#define SONIC_DEBUG  (SONIC_DEBUG_NONE)
/*
#define SONIC_DEBUG  (SONIC_DEBUG_ERRORS | SONIC_DEBUG_PRINT_REGISTERS |\
      SONIC_DEBUG_DESCRIPTORS)
*/

/*
  ((SONIC_DEBUG_ALL) & ~(SONIC_DEBUG_PRINT_REGISTERS|SONIC_DEBUG_DUMP_MBUFS))
  ((SONIC_DEBUG_ALL) & ~(SONIC_DEBUG_DUMP_MBUFS))
*/

#if (SONIC_DEBUG & SONIC_DEBUG_PRINT_REGISTERS)
extern char SONIC_Reg_name[64][6];
#endif


/*
 *  Configuration Information
 */

typedef void (*sonic_write_register_t)(
  void       *base,
  uint32_t    regno,
  uint32_t    value
);

typedef uint32_t   (*sonic_read_register_t)(
  void       *base,
  uint32_t    regno
);

typedef struct {
  void                   *base_address;
  uint32_t                vector;
  uint32_t                dcr_value;
  uint32_t                dc2_value;
  uint32_t                tda_count;
  uint32_t                rda_count;
  sonic_write_register_t  write_register;
  sonic_read_register_t   read_register;
} sonic_configuration_t;

/*
 ******************************************************************
 *                                                                *
 *                        Device Registers                        *
 *                                                                *
 ******************************************************************
 */
#define SONIC_REG_CR      0x00 /* Command */
#define SONIC_REG_DCR     0x01 /* Data configuration */
#define SONIC_REG_RCR     0x02 /* Receive control */
#define SONIC_REG_TCR     0x03 /* Transmit control */
#define SONIC_REG_IMR     0x04 /* Interrupt mask */
#define SONIC_REG_ISR     0x05 /* Interrupt status */
#define SONIC_REG_UTDA    0x06 /* Upper transmit descriptor address */
#define SONIC_REG_CTDA    0x07 /* Current transmit descriptor address */
#define SONIC_REG_URDA    0x0D /* Upper receive descriptor address */
#define SONIC_REG_CRDA    0x0E /* Current receive descriptor address */
#define SONIC_REG_EOBC    0x13 /* End of buffer word count */
#define SONIC_REG_URRA    0x14 /* Upper receive resource */
#define SONIC_REG_RSA     0x15 /* Resource start address */
#define SONIC_REG_REA     0x16 /* Resource end address */
#define SONIC_REG_RRP     0x17 /* Resouce read pointer */
#define SONIC_REG_RWP     0x18 /* Resouce write pointer */
#define SONIC_REG_CEP     0x21 /* CAM entry pointer */
#define SONIC_REG_CAP2    0x22 /* CAM address port 2 */
#define SONIC_REG_CAP1    0x23 /* CAM address port 1 */
#define SONIC_REG_CAP0    0x24 /* CAM address port 0 */
#define SONIC_REG_CE      0x25 /* CAM enable */
#define SONIC_REG_CDP     0x26 /* CAM descriptor pointer */
#define SONIC_REG_CDC     0x27 /* CAM descriptor count */
#define SONIC_REG_SR      0x28 /* Silicon revision */
#define SONIC_REG_WT0     0x29 /* Watchdog timer 0 */
#define SONIC_REG_WT1     0x2A /* Watchdog timer 1 */
#define SONIC_REG_RSC     0x2B /* Receive sequence counter */
#define SONIC_REG_CRCT    0x2C /* CRC error tally */
#define SONIC_REG_FAET    0x2D /* FAE tally */
#define SONIC_REG_MPT     0x2E /* Missed packet tally */
#define SONIC_REG_MDT     0x2F /* TX Maximum Deferral */
#define SONIC_REG_DCR2    0x3F /* Data configuration 2 */

/*
 * Command register
 */
#define CR_LCAM         0x0200
#define CR_RRRA         0x0100
#define CR_RST          0x0080
#define CR_ST           0x0020
#define CR_STP          0x0010
#define CR_RXEN         0x0008
#define CR_RXDIS        0x0004
#define CR_TXP          0x0002
#define CR_HTX          0x0001

/*
 * Data configuration register
 */
#define DCR_EXBUS       0x8000
#define DCR_LBR         0x2000
#define DCR_PO1         0x1000
#define DCR_PO0         0x0800
#define DCR_SBUS        0x0400
#define DCR_USR1        0x0200
#define DCR_USR0        0x0100
#define DCR_WC1         0x0080
#define DCR_WC0         0x0040
#define DCR_DW          0x0020
#define DCR_BMS         0x0010
#define DCR_RFT1        0x0008
#define DCR_RFT0        0x0004
#define DCR_TFT1        0x0002
#define DCR_TFT0        0x0001

/* data configuration register aliases */
#define DCR_SYNC        DCR_SBUS  /* synchronous (memory cycle 2 clocks) */
#define DCR_ASYNC       0         /* asynchronous (memory cycle 3 clocks) */

#define DCR_WAIT0       0                 /* 0 wait states added */
#define DCR_WAIT1       DCR_WC0           /* 1 wait state added */
#define DCR_WAIT2       DCR_WC1           /* 2 wait states added */
#define DCR_WAIT3       (DCR_WC1|DCR_WC0) /* 3 wait states added */

#define DCR_DW16        0       /* use 16-bit DMA accesses */
#define DCR_DW32        DCR_DW  /* use 32-bit DMA accesses */

#define DCR_DMAEF       0       /* DMA until TX/RX FIFO has emptied/filled */
#define DCR_DMABLOCK    DCR_BMS /* DMA until RX/TX threshold crossed */

#define DCR_RFT4        0               /* receive threshold 4 bytes */
#define DCR_RFT8        DCR_RFT0        /* receive threshold 8 bytes */
#define DCR_RFT16       DCR_RFT1        /* receive threshold 16 bytes */
#define DCR_RFT24       (DCR_RFT1|DCR_RFT0) /* receive threshold 24 bytes */

#define DCR_TFT8        0               /* transmit threshold 8 bytes */
#define DCR_TFT16       DCR_TFT0        /* transmit threshold 16 bytes */
#define DCR_TFT24       DCR_TFT1        /* transmit threshold 24 bytes */
#define DCR_TFT28       (DCR_TFT1|DCR_TFT0) /* transmit threshold 28 bytes */

/*
 * Receive control register
 */
#define RCR_ERR         0x8000
#define RCR_RNT         0x4000
#define RCR_BRD         0x2000
#define RCR_PRO         0x1000
#define RCR_AMC         0x0800
#define RCR_LB1         0x0400
#define RCR_LB0         0x0200
#define RCR_MC          0x0100
#define RCR_BC          0x0080
#define RCR_LPKT        0x0040
#define RCR_CRS         0x0020
#define RCR_COL         0x0010
#define RCR_CRCR        0x0008
#define RCR_FAER        0x0004
#define RCR_LBK         0x0002
#define RCR_PRX         0x0001

/*
 * Transmit control register
 */
#define TCR_PINT        0x8000
#define TCR_POWC        0x4000
#define TCR_CRCI        0x2000
#define TCR_EXDIS       0x1000
#define TCR_EXD         0x0400
#define TCR_DEF         0x0200
#define TCR_NCRS        0x0100
#define TCR_CRSL        0x0080
#define TCR_EXC         0x0040
#define TCR_OWC         0x0020
#define TCR_PMB         0x0008
#define TCR_FU          0x0004
#define TCR_BCM         0x0002
#define TCR_PTX         0x0001

/*
 * Interrupt mask register
 */
#define IMR_BREN        0x4000
#define IMR_HBLEN       0x2000
#define IMR_LCDEN       0x1000
#define IMR_PINTEN      0x0800
#define IMR_PRXEN       0x0400
#define IMR_PTXEN       0x0200
#define IMR_TXEREN      0x0100
#define IMR_TCEN        0x0080
#define IMR_RDEEN       0x0040
#define IMR_RBEEN       0x0020
#define IMR_RBAEEN      0x0010
#define IMR_CRCEN       0x0008
#define IMR_FAEEN       0x0004
#define IMR_MPEN        0x0002
#define IMR_RFOEN       0x0001

/*
 * Interrupt status register
 */
#define ISR_BR          0x4000
#define ISR_HBL         0x2000
#define ISR_LCD         0x1000
#define ISR_PINT        0x0800
#define ISR_PKTRX       0x0400
#define ISR_TXDN        0x0200
#define ISR_TXER        0x0100
#define ISR_TC          0x0080
#define ISR_RDE         0x0040
#define ISR_RBE         0x0020
#define ISR_RBAE        0x0010
#define ISR_CRC         0x0008
#define ISR_FAE         0x0004
#define ISR_MP          0x0002
#define ISR_RFO         0x0001

/*
 * Data configuration register 2
 */
#define DCR2_EXPO3      0x8000
#define DCR2_EXPO2      0x4000
#define DCR2_EXPO1      0x2000
#define DCR2_EXPO0      0x1000
#define DCR2_HBDIS      0x0800
#define DCR2_PH         0x0010
#define DCR2_PCM        0x0004
#define DCR2_PCNM       0x0002
#define DCR2_RJCM       0x0001

/*
 *  Known values for the Silicon Revision Register.
 *  Note that DP83934 has revision 5 and seems to work.
 */

#define SONIC_REVISION_B   4
#define SONIC_REVISION_DP83934   5
#define SONIC_REVISION_C   6

/*
 ******************************************************************
 *                                                                *
 *                   Transmit Buffer Management                   *
 *                                                                *
 ******************************************************************
 */

/*
 * Transmit descriptor area entry.
 * There is one transmit descriptor for each packet to be transmitted.
 * Statically reserve space for up to MAXIMUM_FRAGS_PER_PACKET fragments
 * per descriptor.
 */
#define MAXIMUM_FRAGS_PER_DESCRIPTOR    6
struct TransmitDescriptor {
  uint32_t          status;
  uint32_t          pkt_config;
  uint32_t          pkt_size;
  uint32_t          frag_count;

  /*
   * Packet fragment pointers
   */
  struct TransmitDescriptorFragLink {
      uint32_t   frag_lsw;  /* LSW of fragment address */
#define                      frag_link frag_lsw
      uint32_t   frag_msw;  /* MSW of fragment address */
      uint32_t   frag_size;
  }                       frag[MAXIMUM_FRAGS_PER_DESCRIPTOR];

  /*
   * Space for link if all fragment pointers are used.
   */
  uint32_t          link_pad;

  /*
   * Extra RTEMS stuff
   */
  struct TransmitDescriptor       *next;  /* Circularly-linked list */
  struct mbuf                     *mbufp; /* First mbuf in packet */
  volatile uint32_t         *linkp; /* Pointer to un[xxx].link */
};
typedef struct TransmitDescriptor TransmitDescriptor_t;
typedef volatile TransmitDescriptor_t *TransmitDescriptorPointer_t;

/*
 * Transmit Configuration.
 * For standard Ethernet transmission, all bits in the transmit
 * configuration field are set to 0.
 */
#define TDA_CONFIG_PINT 0x8000
#define TDA_CONFIG_POWC 0x4000
#define TDA_CONFIG_CRCI 0x2000
#define TDA_CONFIG_EXDIS        0x1000

/*
 * Transmit status
 */
#define TDA_STATUS_COLLISION_MASK       0xF800
#define TDA_STATUS_COLLISION_SHIFT      11
#define TDA_STATUS_EXD          0x0400
#define TDA_STATUS_DEF          0x0200
#define TDA_STATUS_NCRS         0x0100
#define TDA_STATUS_CRSL         0x0080
#define TDA_STATUS_EXC          0x0040
#define TDA_STATUS_OWC          0x0020
#define TDA_STATUS_PMB          0x0008
#define TDA_STATUS_FU           0x0004
#define TDA_STATUS_BCM          0x0002
#define TDA_STATUS_PTX          0x0001

#define TDA_LINK_EOL      0x0001
#define TDA_LINK_EOL_MASK 0xFFFE



/*
 ******************************************************************
 *                                                                *
 *                    Receive Buffer Management                   *
 *                                                                *
 ******************************************************************
 */

/*
 * Receive resource area entry.
 * There is one receive resource entry for each receive buffer area (RBA).
 * This driver allows only one packet per receive buffer area, so one
 * receive resource entry corresponds to one correctly-received packet.
 */
struct ReceiveResource {
  uint32_t          buff_ptr_lsw;   /* LSW of RBA address */
  uint32_t          buff_ptr_msw;   /* MSW of RBA address */
  uint32_t          buff_wc_lsw;    /* LSW of RBA size (16-bit words) */
  uint32_t          buff_wc_msw;    /* MSW of RBA size (16-bit words) */
};
typedef struct ReceiveResource ReceiveResource_t;
typedef volatile ReceiveResource_t *ReceiveResourcePointer_t;

/*
 * Receive descriptor area entry.
 * There is one receive descriptor for each packet received.
 */
struct ReceiveDescriptor {
  uint32_t          status;
  uint32_t          byte_count;
  uint32_t          pkt_lsw;        /* LSW of packet address */
  uint32_t          pkt_msw;        /* MSW of packet address */
  uint32_t          seq_no;
  uint32_t          link;
  uint32_t          in_use;

  /*
   * Extra RTEMS stuff
   */
  volatile struct ReceiveDescriptor  *next;  /* Circularly-linked list */
  struct mbuf                        *mbufp; /* First mbuf in packet */
};
typedef struct ReceiveDescriptor ReceiveDescriptor_t;
typedef volatile ReceiveDescriptor_t *ReceiveDescriptorPointer_t;

typedef struct {
  uint32_t    cep;  /* CAM Entry Pointer */
  uint32_t    cap0; /* CAM Address Port 0 xx-xx-xx-xx-YY-YY */
  uint32_t    cap1; /* CAM Address Port 1 xx-xx-YY-YY-xxxx */
  uint32_t    cap2; /* CAM Address Port 2 YY-YY-xx-xx-xx-xx */
  uint32_t    ce;
} CamDescriptor_t;

typedef volatile CamDescriptor_t *CamDescriptorPointer_t;

/*
 * Receive status
 */
#define RDA_STATUS_ERR          0x8800
#define RDA_STATUS_RNT          0x4000
#define RDA_STATUS_BRD          0x2000
#define RDA_STATUS_PRO          0x1000
#define RDA_STATUS_AMC          0x0800
#define RDA_STATUS_LB1          0x0400
#define RDA_STATUS_LB0          0x0200
#define RDA_STATUS_MC           0x0100
#define RDA_STATUS_BC           0x0080
#define RDA_STATUS_LPKT         0x0040
#define RDA_STATUS_CRS          0x0020
#define RDA_STATUS_COL          0x0010
#define RDA_STATUS_CRCR         0x0008
#define RDA_STATUS_FAER         0x0004
#define RDA_STATUS_LBK          0x0002
#define RDA_STATUS_PRX          0x0001

#define RDA_LINK_EOL        0x0001
#define RDA_LINK_EOL_MASK   0xFFFE
#define RDA_IN_USE          0x0000  /* SONIC has finished with the packet */
                                    /*   and the driver can process it */
#define RDA_FREE            0xFFFF  /* SONIC can use it */

/*
 *  Attach routine
 */

int rtems_sonic_driver_attach (
  struct rtems_bsdnet_ifconfig *config,
  sonic_configuration_t *chip
);

#ifdef CPU_U32_FIX
void ipalign(struct mbuf *m);
#endif

#endif /* _SONIC_DP83932_ */
