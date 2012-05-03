#ifndef _SMC91111_H_
#define _SMC91111_H_

#include <libchip/smc91111exp.h>
#include <rtems/bspIo.h>

#define LAN91CXX_TCR         0x00
#define LAN91CXX_EPH_STATUS  0x01
#define LAN91CXX_RCR         0x02
#define LAN91CXX_COUNTER     0x03
#define LAN91CXX_MIR         0x04
#define LAN91CXX_MCR         0x05 /* Other than 91C111*/
#define LAN91CXX_RPCR        0x05 /* 91C111 only*/
#define LAN91CXX_RESERVED_0  0x06
#define LAN91CXX_BS          0x07
#define LAN91CXX_CONFIG      0x08
#define LAN91CXX_BASE_REG    0x09
#define LAN91CXX_IA01        0x0a
#define LAN91CXX_IA23        0x0b
#define LAN91CXX_IA45        0x0c
#define LAN91CXX_GENERAL     0x0d /* 91C96 - was "RESERVED_1" for others*/
#define LAN91CXX_CONTROL     0x0e
#define LAN91CXX_BS2         0x0f
#define LAN91CXX_MMU_COMMAND 0x10
#define LAN91CXX_PNR         0x11
#define LAN91CXX_FIFO_PORTS  0x12
#define LAN91CXX_POINTER     0x13
#define LAN91CXX_DATA_HIGH   0x14
#define LAN91CXX_DATA        0x15
#define LAN91CXX_INTERRUPT   0x16
#define LAN91CXX_BS3         0x17
#define LAN91CXX_MT01        0x18
#define LAN91CXX_MT23        0x19
#define LAN91CXX_MT45        0x1a
#define LAN91CXX_MT67        0x1b
#define LAN91CXX_MGMT        0x1c
#define LAN91CXX_REVISION    0x1d
#define LAN91CXX_ERCV        0x1e
#define LAN91CXX_BS4         0x1f

#define LAN91CXX_RCR_SOFT_RST   0x8000    /* soft reset*/
#define LAN91CXX_RCR_FILT_CAR   0x4000    /* filter carrier*/
#define LAN91CXX_RCR_ABORT_ENB  0x2000    /* abort on collision*/
#define LAN91CXX_RCR_STRIP_CRC  0x0200    /* strip CRC*/
#define LAN91CXX_RCR_RXEN       0x0100    /* enable RX*/
#define LAN91CXX_RCR_ALMUL      0x0004    /* receive all muticasts*/
#define LAN91CXX_RCR_PRMS       0x0002    /* promiscuous*/
#define LAN91CXX_RCR_RX_ABORT   0x0001    /* set when abort due to long frame*/

#define LAN91CXX_TCR_SWFDUP     0x8000    /* Switched Full Duplex mode*/
#define LAN91CXX_TCR_ETEN_TYPE  0x4000    /* ETEN type (91C96) 0 <=> like a 91C94*/
#define LAN91CXX_TCR_EPH_LOOP   0x2000    /* loopback mode*/
#define LAN91CXX_TCR_STP_SQET   0x1000    /* Stop transmission on SQET error*/
#define LAN91CXX_TCR_FDUPLX     0x0800    /* full duplex*/
#define LAN91CXX_TCR_MON_CSN    0x0400    /* monitor carrier during tx (91C96)*/
#define LAN91CXX_TCR_NOCRC      0x0100    /* does not append CRC to frames*/
#define LAN91CXX_TCR_PAD_EN     0x0080    /* pads frames with 00 to min length*/
#define LAN91CXX_TCR_FORCOL     0x0004    /* force collision*/
#define LAN91CXX_TCR_LLOOP      0x0002    /* local loopback (91C96)*/
#define LAN91CXX_TCR_TXENA      0x0001    /* enable*/

#define LAN91CXX_POINTER_RCV        0x8000
#define LAN91CXX_POINTER_AUTO_INCR  0x4000
#define LAN91CXX_POINTER_READ       0x2000
#define LAN91CXX_POINTER_ETEN       0x1000
#define LAN91CXX_POINTER_NOT_EMPTY  0x0800


#define LAN91CXX_INTERRUPT_TX_IDLE_M      0x8000 /* (91C96)*/
#define LAN91CXX_INTERRUPT_ERCV_INT_M     0x4000
#define LAN91CXX_INTERRUPT_EPH_INT_M      0x2000
#define LAN91CXX_INTERRUPT_RX_OVRN_INT_M  0x1000
#define LAN91CXX_INTERRUPT_ALLOC_INT_M    0x0800
#define LAN91CXX_INTERRUPT_TX_EMPTY_INT_M 0x0400
#define LAN91CXX_INTERRUPT_TX_INT_M       0x0200
#define LAN91CXX_INTERRUPT_RCV_INT_M      0x0100
#define LAN91CXX_INTERRUPT_TX_IDLE        0x0080 /* (91C96)*/
#define LAN91CXX_INTERRUPT_ERCV_INT       0x0040 /* also ack*/
#define LAN91CXX_INTERRUPT_EPH_INT        0x0020
#define LAN91CXX_INTERRUPT_RX_OVRN_INT    0x0010 /* also ack*/
#define LAN91CXX_INTERRUPT_ALLOC_INT      0x0008
#define LAN91CXX_INTERRUPT_TX_EMPTY_INT   0x0004 /* also ack*/
#define LAN91CXX_INTERRUPT_TX_INT         0x0002 /* also ack*/
#define LAN91CXX_INTERRUPT_RCV_INT        0x0001

#define LAN91CXX_INTERRUPT_TX_SET         0x0006 /* TX_EMPTY + TX*/
#define LAN91CXX_INTERRUPT_TX_SET_ACK     0x0004 /* TX_EMPTY and not plain TX*/
#define LAN91CXX_INTERRUPT_TX_FIFO_ACK    0x0002 /* TX alone*/
#define LAN91CXX_INTERRUPT_TX_SET_M       0x0600 /* TX_EMPTY + TX*/

#define LAN91CXX_CONTROL_RCV_BAD       0x4000
#define LAN91CXX_CONTROL_AUTO_RELEASE  0x0800
#define LAN91CXX_CONTROL_LE_ENABLE     0x0080
#define LAN91CXX_CONTROL_CR_ENABLE     0x0040
#define LAN91CXX_CONTROL_TE_ENABLE     0x0020

/* These are for setting the MAC address in the 91C96 serial EEPROM*/
#define LAN91CXX_CONTROL_EEPROM_SELECT 0x0004
#define LAN91CXX_CONTROL_RELOAD        0x0002
#define LAN91CXX_CONTROL_STORE         0x0001
#define LAN91CXX_CONTROL_EEPROM_BUSY   0x0003
#define LAN91CXX_ESA_EEPROM_OFFSET     0x0020

#define LAN91CXX_STATUS_TX_UNRN        0x8000
#define LAN91CXX_STATUS_LINK_OK        0x4000
#define LAN91CXX_STATUS_CTR_ROL        0x1000
#define LAN91CXX_STATUS_EXC_DEF        0x0800
#define LAN91CXX_STATUS_LOST_CARR      0x0400
#define LAN91CXX_STATUS_LATCOL         0x0200
#define LAN91CXX_STATUS_WAKEUP         0x0100
#define LAN91CXX_STATUS_TX_DEFR        0x0080
#define LAN91CXX_STATUS_LTX_BRD        0x0040
#define LAN91CXX_STATUS_SQET           0x0020
#define LAN91CXX_STATUS_16COL          0x0010
#define LAN91CXX_STATUS_LTX_MULT       0x0008
#define LAN91CXX_STATUS_MUL_COL        0x0004
#define LAN91CXX_STATUS_SNGL_COL       0x0002
#define LAN91CXX_STATUS_TX_SUC         0x0001

#define LAN91CXX_MMU_COMMAND_BUSY      0x0001

#define LAN91CXX_MMU_noop              0x0000
#define LAN91CXX_MMU_alloc_for_tx      0x0020
#define LAN91CXX_MMU_reset_mmu         0x0040
#define LAN91CXX_MMU_rem_rx_frame      0x0060
#define LAN91CXX_MMU_rem_tx_frame      0x0070 /* (91C96) only when TX stopped*/
#define LAN91CXX_MMU_remrel_rx_frame   0x0080
#define LAN91CXX_MMU_rel_packet        0x00a0
#define LAN91CXX_MMU_enq_packet        0x00c0
#define LAN91CXX_MMU_reset_tx_fifo     0x00e0

#define LAN91CXX_CONTROLBYTE_CRC       0x1000
#define LAN91CXX_CONTROLBYTE_ODD       0x2000
#define LAN91CXX_CONTROLBYTE_RX        0x4000

#define LAN91CXX_RX_STATUS_ALIGNERR    0x8000
#define LAN91CXX_RX_STATUS_BCAST       0x4000
#define LAN91CXX_RX_STATUS_BADCRC      0x2000
#define LAN91CXX_RX_STATUS_ODDFRM      0x1000
#define LAN91CXX_RX_STATUS_TOOLONG     0x0800
#define LAN91CXX_RX_STATUS_TOOSHORT    0x0400
#define LAN91CXX_RX_STATUS_HASHVALMASK 0x007e /* MASK*/
#define LAN91CXX_RX_STATUS_MCAST       0x0001
#define LAN91CXX_RX_STATUS_BAD     \
    (LAN91CXX_RX_STATUS_ALIGNERR | \
     LAN91CXX_RX_STATUS_BADCRC   | \
     LAN91CXX_RX_STATUS_TOOLONG  | \
     LAN91CXX_RX_STATUS_TOOSHORT)

#define LAN91CXX_RX_STATUS_IS_ODD(__cpd,__stat) ((__stat) & LAN91CXX_RX_STATUS_ODDFRM)
#define LAN91CXX_CONTROLBYTE_IS_ODD(__cpd,__val) ((__val) & LAN91CXX_CONTROLBYTE_ODD)

/* Attribute memory registers in PCMCIA mode*/
#define LAN91CXX_ECOR                  0x8000
#define LAN91CXX_ECOR_RESET            (1<<7)
#define LAN91CXX_ECOR_LEVIRQ           (1<<6)
#define LAN91CXX_ECOR_ATTWR            (1<<2)
#define LAN91CXX_ECOR_ENABLE           (1<<0)

#define LAN91CXX_ECSR                  0x8002
#define LAN91CXX_ECSR_IOIS8            (1<<5)
#define LAN91CXX_ECSR_PWRDWN           (1<<2)
#define LAN91CXX_ECSR_INTR             (1<<1)

/* These are for manipulating the MII interface*/
#define LAN91CXX_MGMT_MDO              0x0001
#define LAN91CXX_MGMT_MDI              0x0002
#define LAN91CXX_MGMT_MCLK             0x0004
#define LAN91CXX_MGMT_MDOE             0x0008

/* Internal PHY registers (91c111)*/
#define LAN91CXX_PHY_CTRL              0
#define LAN91CXX_PHY_STAT              1
#define LAN91CXX_PHY_ID1               2
#define LAN91CXX_PHY_ID2               3
#define LAN91CXX_PHY_AUTO_AD           4
#define LAN91CXX_PHY_AUTO_CAP          5
#define LAN91CXX_PHY_CONFIG1          16
#define LAN91CXX_PHY_CONFIG2          17
#define LAN91CXX_PHY_STATUS_OUT       18
#define LAN91CXX_PHY_MASK             19

/* PHY control bits*/
#define LAN91CXX_PHY_CTRL_COLTST      (1 << 7)
#define LAN91CXX_PHY_CTRL_DPLX        (1 << 8)
#define LAN91CXX_PHY_CTRL_ANEG_RST    (1 << 9)
#define LAN91CXX_PHY_CTRL_MII_DIS     (1 << 10)
#define LAN91CXX_PHY_CTRL_PDN         (1 << 11)
#define LAN91CXX_PHY_CTRL_ANEG_EN     (1 << 12)
#define LAN91CXX_PHY_CTRL_SPEED       (1 << 13)
#define LAN91CXX_PHY_CTRL_LPBK        (1 << 14)
#define LAN91CXX_PHY_CTRL_RST         (1 << 15)

/* PHY Configuration Register 1 */
#define PHY_CFG1_LNKDIS		0x8000	/* 1=Rx Link Detect Function disabled */
#define PHY_CFG1_XMTDIS		0x4000	/* 1=TP Transmitter Disabled */
#define PHY_CFG1_XMTPDN		0x2000	/* 1=TP Transmitter Powered Down */
#define PHY_CFG1_BYPSCR		0x0400	/* 1=Bypass scrambler/descrambler */
#define PHY_CFG1_UNSCDS		0x0200	/* 1=Unscramble Idle Reception Disable */
#define PHY_CFG1_EQLZR		0x0100	/* 1=Rx Equalizer Disabled */
#define PHY_CFG1_CABLE		0x0080	/* 1=STP(150ohm), 0=UTP(100ohm) */
#define PHY_CFG1_RLVL0		0x0040	/* 1=Rx Squelch level reduced by 4.5db */
#define PHY_CFG1_TLVL_SHIFT	2	/* Transmit Output Level Adjust */
#define PHY_CFG1_TLVL_MASK	0x003C
#define PHY_CFG1_TRF_MASK	0x0003	/* Transmitter Rise/Fall time */

/* PHY Configuration Register 2 */
#define PHY_CFG2_REG		0x11
#define PHY_CFG2_APOLDIS	0x0020	/* 1=Auto Polarity Correction disabled */
#define PHY_CFG2_JABDIS		0x0010	/* 1=Jabber disabled */
#define PHY_CFG2_MREG		0x0008	/* 1=Multiple register access (MII mgt) */
#define PHY_CFG2_INTMDIO	0x0004	/* 1=Interrupt signaled with MDIO pulseo */

/* PHY Status Output (and Interrupt status) Register */
#define PHY_INT_REG		0x12	/* Status Output (Interrupt Status) */
#define PHY_INT_INT		0x8000	/* 1=bits have changed since last read */
#define	PHY_INT_LNKFAIL		0x4000	/* 1=Link Not detected */
#define PHY_INT_LOSSSYNC	0x2000	/* 1=Descrambler has lost sync */
#define PHY_INT_CWRD		0x1000	/* 1=Invalid 4B5B code detected on rx */
#define PHY_INT_SSD		0x0800	/* 1=No Start Of Stream detected on rx */
#define PHY_INT_ESD		0x0400	/* 1=No End Of Stream detected on rx */
#define PHY_INT_RPOL		0x0200	/* 1=Reverse Polarity detected */
#define PHY_INT_JAB		0x0100	/* 1=Jabber detected */
#define PHY_INT_SPDDET		0x0080	/* 1=100Base-TX mode, 0=10Base-T mode */
#define PHY_INT_DPLXDET		0x0040	/* 1=Device in Full Duplex */

/* PHY Interrupt/Status Mask Register */
#define PHY_MASK_REG		0x13	/* Interrupt Mask */

#define LAN91CXX_RPCR_LEDA_LINK       (0 << 2)
#define LAN91CXX_RPCR_LEDA_TXRX       (4 << 2)
#define LAN91CXX_RPCR_LEDA_RX         (6 << 2)
#define LAN91CXX_RPCR_LEDA_TX         (7 << 2)
#define LAN91CXX_RPCR_LEDB_LINK       (0 << 5)
#define LAN91CXX_RPCR_LEDB_TXRX       (4 << 5)
#define LAN91CXX_RPCR_LEDB_RX         (6 << 5)
#define LAN91CXX_RPCR_LEDB_TX         (7 << 5)
#define LAN91CXX_RPCR_ANEG            (1 << 11)
#define LAN91CXX_RPCR_DPLX            (1 << 12)
#define LAN91CXX_RPCR_SPEED           (1 << 13)

/* PHY Control Register */
#define PHY_CNTL_REG		0x00
#define PHY_CNTL_RST		0x8000	/* 1=PHY Reset */
#define PHY_CNTL_LPBK		0x4000	/* 1=PHY Loopback */
#define PHY_CNTL_SPEED		0x2000	/* 1=100Mbps, 0=10Mpbs */
#define PHY_CNTL_ANEG_EN	0x1000 /* 1=Enable Auto negotiation */
#define PHY_CNTL_PDN		0x0800	/* 1=PHY Power Down mode */
#define PHY_CNTL_MII_DIS	0x0400	/* 1=MII 4 bit interface disabled */
#define PHY_CNTL_ANEG_RST	0x0200 /* 1=Reset Auto negotiate */
#define PHY_CNTL_DPLX		0x0100	/* 1=Full Duplex, 0=Half Duplex */
#define PHY_CNTL_COLTST		0x0080	/* 1= MII Colision Test */

/* PHY Status Register */
#define PHY_STAT_REG		0x01
#define PHY_STAT_CAP_T4		0x8000	/* 1=100Base-T4 capable */
#define PHY_STAT_CAP_TXF	0x4000	/* 1=100Base-X full duplex capable */
#define PHY_STAT_CAP_TXH	0x2000	/* 1=100Base-X half duplex capable */
#define PHY_STAT_CAP_TF		0x1000	/* 1=10Mbps full duplex capable */
#define PHY_STAT_CAP_TH		0x0800	/* 1=10Mbps half duplex capable */
#define PHY_STAT_CAP_SUPR	0x0040	/* 1=recv mgmt frames with not preamble */
#define PHY_STAT_ANEG_ACK	0x0020	/* 1=ANEG has completed */
#define PHY_STAT_REM_FLT	0x0010	/* 1=Remote Fault detected */
#define PHY_STAT_CAP_ANEG	0x0008	/* 1=Auto negotiate capable */
#define PHY_STAT_LINK		0x0004	/* 1=valid link */
#define PHY_STAT_JAB		0x0002	/* 1=10Mbps jabber condition */
#define PHY_STAT_EXREG		0x0001	/* 1=extended registers implemented */
#define PHY_STAT_RESERVED   0x0780  /* Reserved bits mask. */

/* PHY Identifier Registers */
#define PHY_ID1_REG		0x02	/* PHY Identifier 1 */
#define PHY_ID2_REG		0x03	/* PHY Identifier 2 */

/* PHY Auto-Negotiation Advertisement Register */
#define PHY_AD_REG		0x04
#define PHY_AD_NP		0x8000	/* 1=PHY requests exchange of Next Page */
#define PHY_AD_ACK		0x4000	/* 1=got link code word from remote */
#define PHY_AD_RF		0x2000	/* 1=advertise remote fault */
#define PHY_AD_T4		0x0200	/* 1=PHY is capable of 100Base-T4 */
#define PHY_AD_TX_FDX		0x0100	/* 1=PHY is capable of 100Base-TX FDPLX */
#define PHY_AD_TX_HDX		0x0080	/* 1=PHY is capable of 100Base-TX HDPLX */
#define PHY_AD_10_FDX		0x0040	/* 1=PHY is capable of 10Base-T FDPLX */
#define PHY_AD_10_HDX		0x0020	/* 1=PHY is capable of 10Base-T HDPLX */
#define PHY_AD_CSMA		0x0001	/* 1=PHY is capable of 802.3 CMSA */


static int debugflag_out = 0;

#define dbc_printf(lvl,format, args...) do { \
  if (!debugflag_out) { \
    if (lvl & DEBUG) { \
      printk(format,##args); \
    } \
  } \
} while(0)

#define db64_printf(format, args...) dbc_printf(64,format,##args);
#define db16_printf(format, args...) dbc_printf(16,format,##args);
#define db9_printf(format, args...) dbc_printf(9,format,##args);
#define db4_printf(format, args...) dbc_printf(4,format,##args);
#define db2_printf(format, args...) dbc_printf(2,format,##args);
#define db1_printf(format, args...) dbc_printf(1,format,##args);
#define db_printf(format, args...) dbc_printf(0xffff,format,##args);

#if DEBUG & 1
#define DEBUG_FUNCTION() do { db_printf("# %s\n", __FUNCTION__); } while (0)
#else
#define DEBUG_FUNCTION() do {} while(0)
#endif


/* ------------------------------------------------------------------------*/

struct smsc_lan91cxx_stats {
    unsigned int tx_good             ;
    unsigned int tx_max_collisions   ;
    unsigned int tx_late_collisions  ;
    unsigned int tx_underrun         ;
    unsigned int tx_carrier_loss     ;
    unsigned int tx_deferred         ;
    unsigned int tx_sqetesterrors    ;
    unsigned int tx_single_collisions;
    unsigned int tx_mult_collisions  ;
    unsigned int tx_total_collisions ;
    unsigned int rx_good             ;
    unsigned int rx_crc_errors       ;
    unsigned int rx_align_errors     ;
    unsigned int rx_resource_errors  ;
    unsigned int rx_overrun_errors   ;
    unsigned int rx_collisions       ;
    unsigned int rx_short_frames     ;
    unsigned int rx_too_long_frames  ;
    unsigned int rx_symbol_errors    ;
    unsigned int interrupts          ;
    unsigned int rx_count            ;
    unsigned int rx_deliver          ;
    unsigned int rx_resource         ;
    unsigned int rx_restart          ;
    unsigned int tx_count            ;
    unsigned int tx_complete         ;
    unsigned int tx_dropped          ;
};
#define INCR_STAT(c,n) (((c)->stats.n)++)

struct lan91cxx_priv_data;

typedef struct lan91cxx_priv_data {

    /* frontend */
    struct arpcom arpcom;
    rtems_id rxDaemonTid;
    rtems_id txDaemonTid;

    scmv91111_configuration_t config;

    /* backend */
    int rpc_cur_mode;
    int autoneg_active;
    int phyaddr;
    unsigned int lastPhy18;

    int txbusy;                         /* A packet has been sent*/
    unsigned long txkey;                /* Used to ack when packet sent*/
    unsigned short* base;               /* Base I/O address of controller*/
                                        /* (as it comes out of reset)*/
    int interrupt;                      /* Interrupt vector used by controller*/
    unsigned char enaddr[6];            /* Controller ESA*/
    /* Function to configure the ESA - may fetch ESA from EPROM or */
    /* RedBoot config option.  Use of the 'config_enaddr()' function*/
    /* is depreciated in favor of the 'provide_esa()' function and*/
    /* 'hardwired_esa' boolean*/
    void (*config_enaddr)(struct lan91cxx_priv_data* cpd);
    int hardwired_esa;
    int txpacket;
    int rxpacket;
    int within_send;
    int c111_reva;                      /* true if this is a revA LAN91C111*/
    struct smsc_lan91cxx_stats stats;
} lan91cxx_priv_data;

/* ------------------------------------------------------------------------*/

#ifdef LAN91CXX_32BIT_RX
typedef unsigned int rxd_t;
#else
typedef unsigned short rxd_t;
#endif

typedef struct _debug_regs_pair {
  int reg; char *name; struct _debug_regs_pair *bits;
} debug_regs_pair;

static debug_regs_pair debug_regs[] = {
  {LAN91CXX_TCR        , "LAN91CXX_TCR"       ,0},
  {LAN91CXX_EPH_STATUS , "LAN91CXX_EPH_STATUS",0},
  {LAN91CXX_RCR        , "LAN91CXX_RCR"       ,0},
  {LAN91CXX_COUNTER    , "LAN91CXX_COUNTER"   ,0},
  {LAN91CXX_MIR        , "LAN91CXX_MIR"       ,0},
  {LAN91CXX_MCR        , "LAN91CXX_MCR"       ,0},
  {LAN91CXX_RPCR       , "LAN91CXX_RPCR"      ,0},
  {LAN91CXX_RESERVED_0 , "LAN91CXX_RESERVED_0",0},
  {LAN91CXX_BS         , "LAN91CXX_BS"        ,0},
  {LAN91CXX_CONFIG     , "LAN91CXX_CONFIG"    ,0},
  {LAN91CXX_BASE_REG   , "LAN91CXX_BASE_REG"  ,0},
  {LAN91CXX_IA01       , "LAN91CXX_IA01"      ,0},
  {LAN91CXX_IA23       , "LAN91CXX_IA23"      ,0},
  {LAN91CXX_IA45       , "LAN91CXX_IA45"      ,0},
  {LAN91CXX_GENERAL    , "LAN91CXX_GENERAL"   ,0},
  {LAN91CXX_CONTROL    , "LAN91CXX_CONTROL"   ,0},
  {LAN91CXX_BS2        , "LAN91CXX_BS2"       ,0},
  {LAN91CXX_MMU_COMMAND, "LAN91CXX_MMU_COMMAND",0},
  {LAN91CXX_PNR        , "LAN91CXX_PNR"        ,0},
  {LAN91CXX_FIFO_PORTS , "LAN91CXX_FIFO_PORTS" ,0},
  {LAN91CXX_POINTER    , "LAN91CXX_POINTER"    ,0},
  {LAN91CXX_DATA_HIGH  , "LAN91CXX_DATA_HIGH"  ,0},
  {LAN91CXX_DATA       , "LAN91CXX_DATA"       ,0},
  {LAN91CXX_INTERRUPT  , "LAN91CXX_INTERRUPT"  ,0},
  {LAN91CXX_BS3        , "LAN91CXX_BS3"        ,0},
  {LAN91CXX_MT01       , "LAN91CXX_MT01"       ,0},
  {LAN91CXX_MT23       , "LAN91CXX_MT23"       ,0},
  {LAN91CXX_MT45       , "LAN91CXX_MT45"       ,0},
  {LAN91CXX_MT67       , "LAN91CXX_MT67"       ,0},
/*{LAN91CXX_MGMT       , "LAN91CXX_MGMT"       ,0},      */
  {LAN91CXX_REVISION   , "LAN91CXX_REVISION"   ,0},
  {LAN91CXX_ERCV       , "LAN91CXX_ERCV"       ,0},
  {LAN91CXX_BS4        , "LAN91CXX_BS4"        ,0},



  {-1,0}
};

static char *dbg_prefix = "";

#ifndef SMSC_PLATFORM_DEFINED_GET_REG
static __inline__ unsigned short
get_reg(struct lan91cxx_priv_data *cpd, int regno)
{
    unsigned short val; debug_regs_pair *dbg = debug_regs; int c;
    uint32_t              Irql;

    /*rtems_interrupt_disable(Irql);*/

    HAL_WRITE_UINT16(cpd->base+(LAN91CXX_BS), CYG_CPU_TO_LE16(regno>>3));
    HAL_READ_UINT16(cpd->base+((regno&0x7)), val);
    val = CYG_LE16_TO_CPU(val);

    /*rtems_interrupt_enable(Irql);*/

#if DEBUG & 32
    while ((c = dbg->reg) != -1) {
      if (c == regno) {
        db_printf("%sread  reg [%d:%x] -> 0x%04x (%-20s)\n", dbg_prefix, regno>>3,(regno&0x7)*2, val, dbg->name);
        break;
      }
      dbg++;
    }
#else
    db2_printf("%sread  reg %d:%x -> 0x%04x\n", dbg_prefix, regno>>3,(regno&0x7)*2, val);
#endif

    return val;
}
#endif /* SMSC_PLATFORM_DEFINED_GET_REG*/

#ifndef SMSC_PLATFORM_DEFINED_PUT_REG
static __inline__ void
put_reg(struct lan91cxx_priv_data *cpd, int regno, unsigned short val)
{
    debug_regs_pair *dbg = debug_regs; int c;
    uint32_t              Irql;

#if DEBUG & 32
    while ((c = dbg->reg) != -1) {
      if (c == regno) {
        db_printf("%swrite reg [%d:%x] <- 0x%04x (%-20s)\n", dbg_prefix, regno>>3, (regno&0x07)*2, val, dbg->name);
        break;
      }
      dbg++;
    }
#else
    db2_printf("%swrite reg %d:%x <- 0x%04x\n", dbg_prefix, regno>>3,(regno&0x7)*2, val);
#endif

    /*rtems_interrupt_disable(Irql);*/

    HAL_WRITE_UINT16(cpd->base+(LAN91CXX_BS), CYG_CPU_TO_LE16(regno>>3));
    HAL_WRITE_UINT16(cpd->base+((regno&0x7)), CYG_CPU_TO_LE16(val));

    /*rtems_interrupt_enable(Irql);*/

}
#endif /* SMSC_PLATFORM_DEFINED_PUT_REG*/

#ifndef SMSC_PLATFORM_DEFINED_PUT_DATA
/* ------------------------------------------------------------------------*/
/* Assumes bank2 has been selected*/
static __inline__ void
put_data(struct lan91cxx_priv_data *cpd, unsigned short val)
{
    db2_printf("%s[wdata] <- 0x%04x\n", dbg_prefix, val);

    HAL_WRITE_UINT16(cpd->base+((LAN91CXX_DATA & 0x7)), val);

}

/* Assumes bank2 has been selected*/
static __inline__ void
put_data8(struct lan91cxx_priv_data *cpd, unsigned char val)
{
    db2_printf("%s[bdata] <- 0x%02x\n", dbg_prefix, val);

    HAL_WRITE_UINT8(((unsigned char *)(cpd->base+((LAN91CXX_DATA & 0x7))))+1, val);

}

#endif /* SMSC_PLATFORM_DEFINED_PUT_DATA*/

#ifndef SMSC_PLATFORM_DEFINED_GET_DATA
/* Assumes bank2 has been selected*/
static __inline__ rxd_t
get_data(struct lan91cxx_priv_data *cpd)
{
    rxd_t val;

#ifdef LAN91CXX_32BIT_RX
    HAL_READ_UINT32(cpd->base+((LAN91CXX_DATA_HIGH & 0x7)), val);
#else
    HAL_READ_UINT16(cpd->base+((LAN91CXX_DATA & 0x7)), val);
#endif

    db2_printf("%s[rdata] -> 0x%08x\n", dbg_prefix, val);
    return val;
}
#endif /* SMSC_PLATFORM_DEFINED_GET_DATA*/

/* ------------------------------------------------------------------------*/
/* Read the bank register (this one is bank-independent)*/
#ifndef SMSC_PLATFORM_DEFINED_GET_BANKSEL
static __inline__ unsigned short
get_banksel(struct lan91cxx_priv_data *cpd)
{
    unsigned short val;

    HAL_READ_UINT16(cpd->base+(LAN91CXX_BS), val);
    val = CYG_LE16_TO_CPU(val);
    db2_printf("read bank sel val 0x%04x\n", val);
    return val;
}
#endif





#endif  /* _SMC_91111_H_ */


