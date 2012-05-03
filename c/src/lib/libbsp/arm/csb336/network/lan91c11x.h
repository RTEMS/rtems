/*
 *  Header file for SMSC LAN91C11x ethernet devices
 *
 *  Copyright (c) 2004 by Cogent Computer Systems
 *  Written by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
#ifndef __LAN91C11X_H__
#define __LAN91C11X_H__

#include <rtems.h>
#include <bsp.h>

uint16_t lan91c11x_read_reg(int);
void lan91c11x_write_reg(int, uint16_t);
uint16_t lan91c11x_read_reg_fast(int);
void lan91c11x_write_reg_fast(int, uint16_t);
void lan91c11x_write_phy_reg(int , uint16_t);
uint16_t lan91c11x_read_phy_reg(int);
void lan91c11x_unlock(void);
void lan91c11x_lock(void);

#define LAN91C11X_BASE_ADDR 0x12000000

#define LAN91C11X_REG(_b_, _r_) ((((_b_) & 0xf) << 4) | ((_r_) & 0xf))


#define LAN91C11X_TCR             (LAN91C11X_REG(0, 0x0))
#define LAN91C11X_EPHSTAT         (LAN91C11X_REG(0, 0x2))
#define LAN91C11X_RCR             (LAN91C11X_REG(0, 0x4))
#define LAN91C11X_CNTR            (LAN91C11X_REG(0, 0x6))
#define LAN91C11X_MIR             (LAN91C11X_REG(0, 0x8))
#define LAN91C11X_RPCR            (LAN91C11X_REG(0, 0xa))
#define LAN91C11X_BANK            (LAN91C11X_REG(0, 0xe))
#define LAN91C11X_CONFIG          (LAN91C11X_REG(1, 0x0))
#define LAN91C11X_BASE            (LAN91C11X_REG(1, 0x2))
#define LAN91C11X_IA0             (LAN91C11X_REG(1, 0x4))
#define LAN91C11X_IA2             (LAN91C11X_REG(1, 0x6))
#define LAN91C11X_IA4             (LAN91C11X_REG(1, 0x8))
#define LAN91C11X_GNRL            (LAN91C11X_REG(1, 0xa))
#define LAN91C11X_CTRL            (LAN91C11X_REG(1, 0xc))
#define LAN91C11X_MMUCMD          (LAN91C11X_REG(2, 0x0))
#define LAN91C11X_PNR             (LAN91C11X_REG(2, 0x2))
#define LAN91C11X_FIFO            (LAN91C11X_REG(2, 0x4))
#define LAN91C11X_PTR             (LAN91C11X_REG(2, 0x6))
#define LAN91C11X_DATA            (LAN91C11X_REG(2, 0x8))
#define LAN91C11X_INT             (LAN91C11X_REG(2, 0xc))
#define LAN91C11X_MT0             (LAN91C11X_REG(3, 0x0))
#define LAN91C11X_MT2             (LAN91C11X_REG(3, 0x2))
#define LAN91C11X_MT4             (LAN91C11X_REG(3, 0x4))
#define LAN91C11X_MT6             (LAN91C11X_REG(3, 0x6))
#define LAN91C11X_MGMT            (LAN91C11X_REG(3, 0x8))
#define LAN91C11X_REV             (LAN91C11X_REG(3, 0xa))
#define LAN91C11X_ERCV            (LAN91C11X_REG(3, 0xc))


#define LAN91C11X_TCR_TXENA      (bit(0))
#define LAN91C11X_TCR_LOOP       (bit(1))
#define LAN91C11X_TCR_FORCOL     (bit(2))
#define LAN91C11X_TCR_PADEN      (bit(7))
#define LAN91C11X_TCR_NOCRC      (bit(8))
#define LAN91C11X_TCR_MONCSN     (bit(10))
#define LAN91C11X_TCR_FDUPLX     (bit(11))
#define LAN91C11X_TCR_STPSQET    (bit(12))
#define LAN91C11X_TCR_EPHLOOP    (bit(13))
#define LAN91C11X_TCR_SWFDUP     (bit(15))

#define LAN91C11X_EPHSTAT_TXSUC             (bit(0))
#define LAN91C11X_EPHSTAT_SNGLCOL           (bit(1))
#define LAN91C11X_EPHSTAT_MULCOL            (bit(2))
#define LAN91C11X_EPHSTAT_LTXMUL            (bit(3))
#define LAN91C11X_EPHSTAT_16COL             (bit(4))
#define LAN91C11X_EPHSTAT_SQET              (bit(5))
#define LAN91C11X_EPHSTAT_LTXBRD            (bit(6))
#define LAN91C11X_EPHSTAT_TXDFR             (bit(7))
#define LAN91C11X_EPHSTAT_LATCOL            (bit(9))
#define LAN91C11X_EPHSTAT_LOST              (bit(10))
#define LAN91C11X_EPHSTAT_EXCDEF            (bit(11))
#define LAN91C11X_EPHSTAT_CTRROL            (bit(12))
#define LAN91C11X_EPHSTAT_LINK              (bit(14))
#define LAN91C11X_EPHSTAT_TXUNRN            (bit(15))

#define LAN91C11X_RCR_RXABT                 (bit(0))
#define LAN91C11X_RCR_PRMS                  (bit(1))
#define LAN91C11X_RCR_ALMUL                 (bit(2))
#define LAN91C11X_RCR_RXEN                  (bit(8))
#define LAN91C11X_RCR_STRIP                 (bit(9))
#define LAN91C11X_RCR_ABTENB                (bit(13))
#define LAN91C11X_RCR_FILT                  (bit(14))
#define LAN91C11X_RCR_RST                   (bit(15))

#define LAN91C11X_RPCR_LS0B                  (bit(2))
#define LAN91C11X_RPCR_LS1B                  (bit(3))
#define LAN91C11X_RPCR_LS2B                  (bit(4))
#define LAN91C11X_RPCR_LS0A                  (bit(5))
#define LAN91C11X_RPCR_LS1A                  (bit(6))
#define LAN91C11X_RPCR_LS2A                  (bit(7))
#define LAN91C11X_RPCR_ANEG                  (bit(11))
#define LAN91C11X_RPCR_DPLX                  (bit(12))
#define LAN91C11X_RPCR_SPEED                 (bit(13))

#define LAN91C11X_CONFIG_EXTPHY              (bit(9))
#define LAN91C11X_CONFIG_GPCTRL              (bit(10))
#define LAN91C11X_CONFIG_NOWAIT              (bit(12))
#define LAN91C11X_CONFIG_PWR                 (bit(15))

#define LAN91C11X_CTRL_STORE                 (bit(0))
#define LAN91C11X_CTRL_RELOAD                (bit(1))
#define LAN91C11X_CTRL_EEPROM                (bit(2))
#define LAN91C11X_CTRL_TEEN                  (bit(5))
#define LAN91C11X_CTRL_CREN                  (bit(6))
#define LAN91C11X_CTRL_LEEN                  (bit(7))
#define LAN91C11X_CTRL_AUTO                  (bit(11))
#define LAN91C11X_CTRL_RCVBAD                (bit(14))

#define LAN91C11X_MMUCMD_BUSY                (bit(0))
#define LAN91C11X_MMUCMD_NOOP                (0 << 5)
#define LAN91C11X_MMUCMD_ALLOCTX             (1 << 5)
#define LAN91C11X_MMUCMD_RESETMMU            (2 << 5)
#define LAN91C11X_MMUCMD_REMFRM              (3 << 5)
#define LAN91C11X_MMUCMD_REMTOP              (4 << 5)
#define LAN91C11X_MMUCMD_RELEASE             (5 << 5)
#define LAN91C11X_MMUCMD_ENQUEUE             (6 << 5)
#define LAN91C11X_MMUCMD_RESETTX             (7 << 5)

#define LAN91C11X_PTR_MASK                   (0x7ff)
#define LAN91C11X_PTR_NE                     (bit(11))
#define LAN91C11X_PTR_ETEN                   (bit(12))
#define LAN91C11X_PTR_READ                   (bit(13))
#define LAN91C11X_PTR_AUTOINC                (bit(14))
#define LAN91C11X_PTR_RCV                    (bit(15))

#define LAN91C11X_INT_RX                     (bit(0))
#define LAN91C11X_INT_TX                     (bit(1))
#define LAN91C11X_INT_TXE                    (bit(2))
#define LAN91C11X_INT_ALLOC                  (bit(3))
#define LAN91C11X_INT_RXOV                   (bit(4))
#define LAN91C11X_INT_EPH                    (bit(5))
#define LAN91C11X_INT_ERX                    (bit(6))
#define LAN91C11X_INT_MD                     (bit(7))
#define LAN91C11X_INT_RXMASK                 (bit(8))
#define LAN91C11X_INT_TXMASK                 (bit(9))
#define LAN91C11X_INT_TXEMASK                (bit(10))
#define LAN91C11X_INT_ALLOCMASK              (bit(11))
#define LAN91C11X_INT_RXOVMASK               (bit(12))
#define LAN91C11X_INT_EPHMASK                (bit(13))
#define LAN91C11X_INT_ERXMASK                (bit(14))
#define LAN91C11X_INT_MDMASK                 (bit(15))

#define LAN91C11X_MGMT_MDO                   (bit(0))
#define LAN91C11X_MGMT_MDI                   (bit(1))
#define LAN91C11X_MGMT_MCLK                  (bit(2))
#define LAN91C11X_MGMT_MDOE                  (bit(3))
#define LAN91C11X_MGMT_MSKCRS100             (bit(14))


#define LAN91C11X_PKT_CTRL_CRC               (bit(4))
#define LAN91C11X_PKT_CTRL_ODD               (bit(5))


/* PHY Registers */
#define PHY_CTRL        0x00    /* PHY Control  */
#define PHY_STAT        0x01    /* PHY Status */
#define PHY_ID1         0x02    /* PHY Identifier 1 */
#define PHY_ID2         0x03    /* PHY Identifier 2 */
#define PHY_AD          0x04    /* PHY Auto-negotiate Control */
#define PHY_RMT         0x05    /* PHY Auto-neg Remote End Cap Register */
#define PHY_CFG1        0x10    /* PHY Configuration 1 */
#define PHY_CFG2        0x11    /* PHY Configuration 2 */
#define PHY_INT         0x12    /* Status Output (Interrupt Status) */
#define PHY_MASK        0x13    /* Interrupt Mask */

/* PHY Control Register Bit Defines */
#define PHY_CTRL_RST            0x8000  /* PHY Reset */
#define PHY_CTRL_LPBK           0x4000  /* PHY Loopback */
#define PHY_CTRL_SPEED          0x2000  /* 100Mbps, 0=10Mpbs */
#define PHY_CTRL_ANEGEN         0x1000  /* Enable Auto negotiation */
#define PHY_CTRL_PDN            0x0800  /* PHY Power Down mode */
#define PHY_CTRL_MIIDIS         0x0400  /* MII 4 bit interface disabled */
#define PHY_CTRL_ANEGRST        0x0200  /* Reset Auto negotiate */
#define PHY_CTRL_DPLX           0x0100  /* Full Duplex, 0=Half Duplex */
#define PHY_CTRL_COLTST         0x0080  /* MII Colision Test */

#define PHY_STAT_CAPT4          0x8000
#define PHY_STAT_CAPTXF         0x4000
#define PHY_STAT_CAPTXH         0x2000
#define PHY_STAT_CAPTF          0x1000
#define PHY_STAT_CAPTH          0x0800
#define PHY_STAT_CAPSUPR        0x0040
#define PHY_STAT_ANEGACK        0x0020
#define PHY_STAT_REMFLT         0x0010
#define PHY_STAT_CAPANEG        0x0008
#define PHY_STAT_LINK           0x0004
#define PHY_STAT_JAB            0x0002
#define PHY_STAT_EXREG          0x0001

#define PHY_ADV_NP              0x8000
#define PHY_ADV_ACK             0x4000
#define PHY_ADV_RF              0x2000
#define PHY_ADV_T4              0x0200
#define PHY_ADV_TXFDX           0x0100
#define PHY_ADV_TXHDX           0x0080
#define PHY_ADV_10FDX           0x0040
#define PHY_ADV_10HDX           0x0020
#define PHY_ADV_CSMA            0x0001




#endif /* __LAN91C11X_H__ */
