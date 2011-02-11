/*
 * Authorship
 * ----------
 * This software ('mvme3100' RTEMS BSP) was created by
 *
 *     Till Straumann <strauman@slac.stanford.edu>, 2005-2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * The 'mvme3100' BSP was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 *
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 *
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 *
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.
 *
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 *
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */

#include <rtems.h>
#include <rtems/error.h>
#include <bsp/irq.h>
#include <libcpu/byteorder.h>
#include <inttypes.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <bsp.h>

#ifndef KERNEL
#define KERNEL
#endif
#ifndef _KERNEL
#define _KERNEL
#endif

#include <rtems/rtems_bsdnet.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <rtems/rtems_mii_ioctl.h>

#include <bsp/if_tsec_pub.h>

#define STATIC
#define PARANOIA
#undef  DEBUG


#ifdef TEST_MII_TIMING

#include <libcpu/spr.h>

SPR_RO(TBRL)

static inline uint32_t tb_rd()
{
	return _read_TBRL();
}
#endif

struct tsec_private;

/* Forward declarations */
static void
phy_init_irq( int install, struct tsec_private *mp, void (*tsec_lisr)(rtems_irq_hdl_param) );

static void
phy_en_irq(struct tsec_private *mp);

static void
phy_en_irq_at_phy(struct tsec_private *mp);

static void
phy_dis_irq(struct tsec_private *mp);

static void
phy_dis_irq_at_phy(struct tsec_private *mp);

static int
phy_irq_pending(struct tsec_private *mp);

static uint32_t
phy_ack_irq(struct tsec_private *mp);

static void
tsec_update_mcast(struct ifnet *ifp);

#if defined(PARANOIA) || defined(DEBUG)
void tsec_dump_tring(struct tsec_private *mp);
void tsec_dump_rring(struct tsec_private *mp);
#endif

#ifdef DEBUG
#ifdef  TSEC_RX_RING_SIZE
#undef  TSEC_RX_RING_SIZE
#endif
#define TSEC_RX_RING_SIZE		4

#ifdef  TSEC_TX_RING_SIZE
#undef  TSEC_TX_RING_SIZE
#endif
#define TSEC_TX_RING_SIZE		2
#else
#ifndef TSEC_RX_RING_SIZE
#define TSEC_RX_RING_SIZE		40
#endif
#ifndef TSEC_TX_RING_SIZE
#define TSEC_TX_RING_SIZE		200
#endif
#endif

/********** Helper Macros and Definitions ******/

/*
 * Align 'p' up to a multiple of 'a' which must be
 * a power of two. Result is cast to (uintptr_t)
 */
#define ALIGNTO(p,a)	((((uintptr_t)(p)) + (a) - 1) & ~((a)-1))


/*
 * Not obvious from the doc: RX buffers apparently must be 32-byte
 * aligned :-(; TX buffers have no alignment requirement.
 * I found this by testing, T.S, 11/2007
 */
#define RX_BUF_ALIGNMENT		32

/*
 * Alignment req. for buffer descriptors (BDs)
 */
#define BD_ALIGNMENT 8


#define ETH_RX_OFFSET			0
#define ETH_CRC_LEN				0

#define CPU2BUS_ADDR(x)			(x)
#define BUS2CPU_ADDR(x)			(x)

/*
 * Whether to automatically try to reclaim descriptors
 * when enqueueing new packets
 */
#if 1
#define TSEC_CLEAN_ON_SEND(mp) (BSP_tsec_swipe_tx(mp))
#else
#define TSEC_CLEAN_ON_SEND(mp) (-1)
#endif

#define TX_AVAILABLE_RING_SIZE(mp)	(mp)->tx_ring_size

#define DRVNAME "tsec"

/*
 * Event(s) posted by ISRs to driver task
 */
#define EV_PER_UNIT				1

#define TSEC_ETH_EVENT( unit )	(   1 << (EV_PER_UNIT * (unit)    ))
#if EV_PER_UNIT > 1
#define TSEC_PHY_EVENT( unit )  (   1 << (EV_PER_UNIT * (unit) + 1))
#endif

#define EV_IS_ETH(ev)			( (ev) & 1 )
#if EV_PER_UNIT > 1
#define EV_IS_PHY(ev)			( (ev) & 2 )
#endif

#ifndef MAXEBERRS
#define MAXEBERRS 10
#endif

#define EV_IS_ANY(ev)			( (ev) & ((1<<EV_PER_UNIT) - 1) )

#define EV_MSK                  ( ( 1 << (EV_PER_UNIT * TSEC_NUM_DRIVER_SLOTS) ) - 1)


/********** Register Definitions ****************/

/*
 * Most registers/bits apply to the FEC also;
 * things that are not supported by the FEC
 * are commented 'TSEC only'
 */

#define TSEC_IEVENT							0x010
#define TSEC_IEVENT_BABR						(1<<(31- 0))
#define TSEC_IEVENT_RXC							(1<<(31- 1))
#define TSEC_IEVENT_BSY							(1<<(31- 2))
#define TSEC_IEVENT_EBERR						(1<<(31- 3))
/*
 * Misuse reserved bit 4 to flag link interrupts
 * (which are totally external to the TSEC).
 * Because reading the MII is so slow we don't
 * want to poll MII unnecessarily from RX/TX ISRs
 */
#define TSEC_LINK_INTR							(1<<(31- 4))
#define TSEC_IEVENT_MSRO						(1<<(31- 5))
#define TSEC_IEVENT_GTSC						(1<<(31- 6))
#define TSEC_IEVENT_BABT						(1<<(31- 7))
#define TSEC_IEVENT_TXC							(1<<(31- 8))
#define TSEC_IEVENT_TXE							(1<<(31- 9))
#define TSEC_IEVENT_TXB							(1<<(31-10))
#define TSEC_IEVENT_TXF							(1<<(31-11))
#define TSEC_IEVENT_LC							(1<<(31-13))
#define TSEC_IEVENT_CRLXDA						(1<<(31-14))
#define TSEC_IEVENT_XFUN						(1<<(31-15))
#define TSEC_IEVENT_RXB							(1<<(31-16))
#define TSEC_IEVENT_GRSC						(1<<(31-23))
#define TSEC_IEVENT_RXF							(1<<(31-24))
#define TSEC_IEVENT_ALL							(-1)

#if TSEC_TXIRQ != ( TSEC_IEVENT_TXE | TSEC_IEVENT_TXF )
#error "mismatch in definition: TSEC_TXIRQ"
#endif

#if TSEC_RXIRQ != ( TSEC_IEVENT_RXF | TSEC_IEVENT_BABR | TSEC_IEVENT_EBERR )
#error "mismatch in definition: TSEC_RXIRQ"
#endif

#if TSEC_LKIRQ != TSEC_LINK_INTR
#error "mismatch in definition: TSEC_LKIRQ"
#endif

#define TSEC_IMASK							0x014
#define TSEC_IMASK_BABREN						(1<<(31- 0))
#define TSEC_IMASK_RXCEN						(1<<(31- 1))
#define TSEC_IMASK_BSYEN						(1<<(31- 2))
#define TSEC_IMASK_EBERREN						(1<<(31- 3))
#define TSEC_IMASK_MSROEN						(1<<(31- 5))
#define TSEC_IMASK_GTSCEN						(1<<(31- 6))
#define TSEC_IMASK_BABTEN						(1<<(31- 7))
#define TSEC_IMASK_TXCEN						(1<<(31- 8))
#define TSEC_IMASK_TXEEN						(1<<(31- 9))
#define TSEC_IMASK_TXBEN						(1<<(31-10))
#define TSEC_IMASK_TXFEN						(1<<(31-11))
#define TSEC_IMASK_LCEN							(1<<(31-13))
#define TSEC_IMASK_CRLXDAEN						(1<<(31-14))
#define TSEC_IMASK_XFUNEN						(1<<(31-15))
#define TSEC_IMASK_RXBEN						(1<<(31-16))
#define TSEC_IMASK_GRSCEN						(1<<(31-23))
#define TSEC_IMASK_RXFEN						(1<<(31-24))
#define TSEC_IMASK_NONE							(0)
#define TSEC_EDIS							0x018
#define TSEC_ECNTRL							0x020 /* TSEC only */
#define TSEC_ECNTRL_CLRCNT						(1<<(31-17))
#define TSEC_ECNTRL_AUTOZ						(1<<(31-18))
#define TSEC_ECNTRL_STEN						(1<<(31-19))
#define TSEC_ECNTRL_TBIM						(1<<(31-26))
#define TSEC_ECNTRL_RPM							(1<<(31-27))
#define TSEC_ECNTRL_R100M						(1<<(31-28))
#define TSEC_MINFLR							0x024
#define TSEC_PTV							0x028
#define TSEC_DMACTRL						0x02c
#define TSEC_DMACTRL_TDSEN						(1<<(31-24))
#define TSEC_DMACTRL_TBDSEN						(1<<(31-25))
#define TSEC_DMACTRL_GRS						(1<<(31-27))
#define TSEC_DMACTRL_GTS						(1<<(31-28))
#define TSEC_DMACTRL_WWR						(1<<(31-30))
#define TSEC_DMACTRL_WOP						(1<<(31-31))
#define TSEC_TBIPA							0x030 /* TSEC only */
#define TSEC_FIFO_PAUSE_CTRL				0x04c
#define TSEC_FIFO_TX_THR					0x08c
#define TSEC_FIFO_TX_STARVE					0x098
#define TSEC_FIFO_TX_STARVE_SHUTOFF			0x09c
#define TSEC_TCTRL							0x100
#define TSEC_TCTRL_THDR							(1<<(31-20))
#define TSEC_TCTRL_RFC_PAUSE					(1<<(31-27))
#define TSEC_TCTRL_TFC_PAUSE					(1<<(31-28))
#define TSEC_TSTAT							0x104
#define TSEC_TSTAT_THLT							(1<<(31- 0))
#define TSEC_TBDLEN							0x10c
#define TSEC_TXIC							0x110 /* TSEC only */
#define TSEC_IC_ICEN							(1<<(31- 0))
#define TSEC_IC_ICFCT(x)						(((x)&0xff)<<(31-10))
#define TSEC_IC_ICTT(x)							(((x)&0xffff)<<(31-31))
#define TSEC_CTBPTR							0x124
#define TSEC_TBPTR							0x184
#define TSEC_TBASE							0x204
#define TSEC_OSTBD							0x2b0
#define TSEC_OSTBDP							0x2b4
#define TSEC_RCTRL							0x300
#define TSEC_RCTRL_BC_REJ						(1<<(31-27))
#define TSEC_RCTRL_PROM							(1<<(31-28))
#define TSEC_RCTRL_RSF							(1<<(31-29))
#define TSEC_RSTAT							0x304
#define TSEC_RSTAT_QHLT							(1<<(31- 8))
#define TSEC_RBDLEN							0x30c
#define TSEC_RXIC							0x310 /* TSEC only */
#define TSEC_CRBPTR							0x324
#define TSEC_MRBLR							0x340
#define TSEC_RBPTR							0x384
#define TSEC_RBASE							0x404
#define TSEC_MACCFG1						0x500
#define TSEC_MACCFG1_SOFT_RESET					(1<<(31- 0))
#define TSEC_MACCFG1_LOOP_BACK					(1<<(31-23))
#define TSEC_MACCFG1_RX_FLOW					(1<<(31-26))
#define TSEC_MACCFG1_TX_FLOW					(1<<(31-27))
#define TSEC_MACCFG1_SYNCD_RX_EN				(1<<(31-28))
#define TSEC_MACCFG1_RX_EN						(1<<(31-29))
#define TSEC_MACCFG1_SYNCD_TX_EN				(1<<(31-30))
#define TSEC_MACCFG1_TX_EN						(1<<(31-31))
#define TSEC_MACCFG2						0x504
#define TSEC_MACCFG2_PREAMBLE_7					(7<<(31-19))
#define TSEC_MACCFG2_PREAMBLE_15 				(15<<(31-19))
#define TSEC_MACCFG2_IF_MODE_MII				(1<<(31-23))
#define TSEC_MACCFG2_IF_MODE_GMII				(2<<(31-23))
#define TSEC_MACCFG2_HUGE_FRAME					(1<<(31-26))
#define TSEC_MACCFG2_LENGTH_CHECK				(1<<(31-27))
#define TSEC_MACCFG2_PAD_CRC					(1<<(31-29))
#define TSEC_MACCFG2_CRC_EN						(1<<(31-30))
#define TSEC_MACCFG2_FD							(1<<(31-31))
#define TSEC_IPGIFG							0x508
#define TSEC_HAFDUP							0x50c
#define TSEC_MAXFRM							0x510
#define TSEC_MIIMCFG						0x520 /* TSEC only */
#define TSEC_MIIMCOM						0x524 /* TSEC only */
#define TSEC_MIIMCOM_SCAN						(1<<(31-30))
#define TSEC_MIIMCOM_READ						(1<<(31-31))
#define TSEC_MIIMADD						0x528 /* TSEC only */
#define TSEC_MIIMADD_ADDR(phy, reg)				((((phy)&0x1f)<<8) | ((reg) & 0x1f))
#define TSEC_MIIMCON						0x52c /* TSEC only */
#define TSEC_MIIMSTAT						0x530 /* TSEC only */
#define TSEC_MIIMIND						0x534 /* TSEC only */
#define TSEC_MIIMIND_NV							(1<<(31-29))
#define TSEC_MIIMIND_SCAN						(1<<(31-30))
#define TSEC_MIIMIND_BUSY						(1<<(31-31))
#define TSEC_IFSTAT							0x53c
#define TSEC_MACSTNADDR1					0x540
#define TSEC_MACSTNADDR2					0x544
#define TSEC_TR64							0x680 /* TSEC only */
#define TSEC_TR127							0x684 /* TSEC only */
#define TSEC_TR255							0x688 /* TSEC only */
#define TSEC_TR511							0x68c /* TSEC only */
#define TSEC_TR1K							0x690 /* TSEC only */
#define TSEC_TRMAX							0x694 /* TSEC only */
#define TSEC_TRMGV							0x698 /* TSEC only */
#define TSEC_RBYT							0x69c /* TSEC only */
#define TSEC_RPKT							0x6a0 /* TSEC only */
#define TSEC_RFCS							0x6a4 /* TSEC only */
#define TSEC_RMCA							0x6a8 /* TSEC only */
#define TSEC_RBCA							0x6ac /* TSEC only */
#define TSEC_RXCF							0x6b0 /* TSEC only */
#define TSEC_RXPF							0x6b4 /* TSEC only */
#define TSEC_RXUO							0x6b8 /* TSEC only */
#define TSEC_RALN							0x6bc /* TSEC only */
#define TSEC_RFLR							0x6c0 /* TSEC only */
#define TSEC_RCDE							0x6c4 /* TSEC only */
#define TSEC_RCSE							0x6c8 /* TSEC only */
#define TSEC_RUND							0x6cc /* TSEC only */
#define TSEC_ROVR							0x6d0 /* TSEC only */
#define TSEC_RFRG							0x6d4 /* TSEC only */
#define TSEC_RJBR							0x6d8 /* TSEC only */
#define TSEC_RDRP							0x6dc /* TSEC only */
#define TSEC_TBYT							0x6e0 /* TSEC only */
#define TSEC_TPKT							0x6e4 /* TSEC only */
#define TSEC_TMCA							0x6e8 /* TSEC only */
#define TSEC_TBCA							0x6ec /* TSEC only */
#define TSEC_TXPF							0x6f0 /* TSEC only */
#define TSEC_TDFR							0x6f4 /* TSEC only */
#define TSEC_TEDF							0x6f8 /* TSEC only */
#define TSEC_TSCL							0x6fc /* TSEC only */
#define TSEC_TMCL							0x700 /* TSEC only */
#define TSEC_TLCL							0x704 /* TSEC only */
#define TSEC_TXCL							0x708 /* TSEC only */
#define TSEC_TNCL							0x70c /* TSEC only */
#define TSEC_TDRP							0x714 /* TSEC only */
#define TSEC_TJBR							0x718 /* TSEC only */
#define TSEC_TFCS							0x71c /* TSEC only */
#define TSEC_TXCF							0x720 /* TSEC only */
#define TSEC_TOVR							0x724 /* TSEC only */
#define TSEC_TUND							0x728 /* TSEC only */
#define TSEC_TFRG							0x72c /* TSEC only */
#define TSEC_CAR1							0x730 /* TSEC only */
#define TSEC_CAR2							0x734 /* TSEC only */
#define TSEC_CAM1							0x738 /* TSEC only */
#define TSEC_CAM2							0x73c /* TSEC only */
#define TSEC_IADDR0							0x800
#define TSEC_IADDR1							0x804
#define TSEC_IADDR2							0x808
#define TSEC_IADDR3							0x80c
#define TSEC_IADDR4							0x810
#define TSEC_IADDR5							0x814
#define TSEC_IADDR6							0x818
#define TSEC_IADDR7							0x81c
#define TSEC_GADDR0							0x880
#define TSEC_GADDR1							0x884
#define TSEC_GADDR2							0x888
#define TSEC_GADDR3							0x88c
#define TSEC_GADDR4							0x890
#define TSEC_GADDR5							0x894
#define TSEC_GADDR6							0x898
#define TSEC_GADDR7							0x89c
#define TSEC_ATTR							0xbf8
#define TSEC_ATTR_ELCWT_NONE					(0<<(31-18))
#define TSEC_ATTR_ELCWT_ALLOC					(2<<(31-18))
#define TSEC_ATTR_ELCWT_ALLOC_LOCK				(3<<(31-18))
#define TSEC_ATTR_BDLWT_NONE					(0<<(31-21))
#define TSEC_ATTR_BDLWT_ALLOC					(2<<(31-21))
#define TSEC_ATTR_BDLWT_ALLOC_LOCK				(3<<(31-21))
#define TSEC_ATTR_RDSEN							(1<<(31-24))
#define TSEC_ATTR_RBDSEN						(1<<(31-25))
#define TSEC_ATTRELI						0xbfc

/********** Memory Barriers *********************/

#ifdef __PPC__
static inline void membarrier(void)
{
	asm volatile("sync":::"memory");
}

#define EIEIO(mem) do { __asm__ volatile("eieio"); } while (0)

#else
#error "memory barrier not implemented for your CPU architecture"
#endif

/********** Register Access Primitives **********/

/*
 * Typedef for base address (uint8_t *) so that
 * we can do easy pointer arithmetic.
 */
typedef volatile uint8_t *FEC_Enet_Base;

/*
 * All TSEC/FEC registers are 32-bit only.
 */
typedef volatile uint32_t FEC_Reg __attribute__((may_alias));

static inline uint32_t
fec_rd(FEC_Enet_Base b, uint32_t reg)
{
#ifdef __BIG_ENDIAN__
uint32_t rval = *(FEC_Reg *)(b + reg);
	EIEIO(*(FEC_Reg*)(b+reg));
	return rval;
#else
	return in_be32( (volatile uint32_t*) (b+reg) );
#endif
}

static inline void
fec_wr(FEC_Enet_Base b, uint32_t reg, uint32_t val)
{
#ifdef __BIG_ENDIAN__
	*(FEC_Reg *)(b + reg) = val;
	EIEIO(*(FEC_Reg*)(b+reg));
#else
	out_be32( (volatile uint32_t*) (b+reg), val );
#endif
}

/* Set bits in a register */
static inline void
fec_set(FEC_Enet_Base b, uint32_t reg, uint32_t val)
{
	fec_wr(b, reg, fec_rd(b, reg) | val );
}

/* Clear bits in a register */
static inline void
fec_clr(FEC_Enet_Base b, uint32_t reg, uint32_t val)
{
	fec_wr(b, reg, fec_rd(b, reg) & ~val );
}

/* Clear and set bits in a register */
static inline void
fec_csl(FEC_Enet_Base b, uint32_t reg, uint32_t clr, uint32_t set)
{
	fec_wr(b, reg, (fec_rd(b, reg) & ~clr) | set);
}


/********** Memory Access Primitives ************/

#ifdef __BIG_ENDIAN__
static inline uint16_t	ld_be16(volatile uint16_t *a)
{
	return *a;
}

static inline uint32_t	ld_be32(volatile uint32_t *a)
{
	return *a;
}

static inline void	st_be16(volatile uint16_t *a, uint16_t v)
{
	*a = v;
}

static inline void	st_be32(volatile uint32_t *a, uint32_t v)
{
	*a = v;
}
#else
#error "ld_be32 & friends not implemented"
#endif

/********** Note About Coherency ****************/

#ifdef	SW_COHERENCY
#error 	"SW_COHERENCY not implemented"
/* Note: maintaining BD coherency in software is not trivial
 *       because BDs are smaller than a cache line;
 *       we cannot pad a BD to the length of a cache line because
 *       the TSEC assumes BDs layed out sequentially in memory.
 *       We cannot use zero-length BDs to pad to a full cache
 *       line either because the manual says that the length
 *       field of a TX BD must not be zero.
 *
 *       We probably would need MMU resources to map BDs
 *       as non-cachable.
 *
 *       Maintaining buffer coherency would be easier:
 *         - make RX buffers cache aligned so we can
 *           invalidate them w/o overlapping other data.
 *         - TX buffers may be flushed to memory. If cache
 *           lines overlap anything else (besides TX buffers)
 *           then that would only be harmful if (part of) a
 *           TX buffer would share a cache line with another
 *           type of DMA buffer that is written by another
 *           master. Note that BDs have exactly this problem;
 *           we may not flush one BD because the TSEC could
 *           have written another BD to memory covered by
 *           the same cache line.
 *           This second BD could be lost by a DBCF operation:
 *            - writes 1st BD to memory OK
 *            - overwrites 2nd BD with stale data from cache
 */
#else
#define FLUSH_BUF(addr, len) 	do {} while(0)
#endif

/********** Driver Data Structures **************/

/* Buffer descriptor layout (defined by hardware) */
struct tsec_bd {
	volatile uint16_t	flags;
	volatile uint16_t	len;
	volatile uint32_t	buf;
};

typedef struct tsec_bd TSEC_BD __attribute__((aligned(BD_ALIGNMENT)));

/* BD access primitives */

static inline uint16_t  bd_rdfl(TSEC_BD *bd)
{
	return ld_be16( & bd->flags );
}

static inline void bd_wrfl(TSEC_BD *bd, uint16_t v)
{
	st_be16( &bd->flags, v );
}

static inline void bd_setfl(TSEC_BD *bd, uint16_t v)
{
	bd_wrfl(bd, bd_rdfl(bd) | v );
}

static inline void bd_clrfl(TSEC_BD *bd, uint16_t v)
{
	bd_wrfl(bd, bd_rdfl(bd) & ~v );
}

static inline void bd_cslfl(TSEC_BD *bd, uint16_t s, uint16_t c)
{
	bd_wrfl( bd, ( bd_rdfl(bd) & ~c ) | s );
}

static inline uint32_t bd_rdbuf(TSEC_BD *bd)
{
	return BUS2CPU_ADDR( ld_be32( &bd->buf ) );
}

static inline void bd_wrbuf(TSEC_BD *bd, uint32_t addr)
{
	st_be32( &bd->buf, CPU2BUS_ADDR(addr) );
}

/* BD bit definitions */

#define	TSEC_TXBD_R			((uint16_t)(1<<(15- 0)))
#define TSEC_TXBD_PAD_CRC	((uint16_t)(1<<(15- 1)))
#define TSEC_TXBD_W			((uint16_t)(1<<(15- 2)))
#define TSEC_TXBD_I			((uint16_t)(1<<(15- 3)))
#define TSEC_TXBD_L			((uint16_t)(1<<(15- 4)))
#define TSEC_TXBD_TC		((uint16_t)(1<<(15- 5)))
#define TSEC_TXBD_DEF		((uint16_t)(1<<(15- 6)))
#define TSEC_TXBD_TO1		((uint16_t)(1<<(15- 7)))
#define TSEC_TXBD_HFE_LC	((uint16_t)(1<<(15- 8)))
#define TSEC_TXBD_RL		((uint16_t)(1<<(15- 9)))
#define TSEC_TXBD_RC(x)		((uint16_t)(((x)>>2)&0xf))
#define TSEC_TXBD_UN		((uint16_t)(1<<(15-14)))
#define TSEC_TXBD_TXTRUNC	((uint16_t)(1<<(15-15)))
#define TSEC_TXBD_ERRS		(TSEC_TXBD_RL | TSEC_TXBD_UN | TSEC_TXBD_TXTRUNC)

#define TSEC_RXBD_E			((uint16_t)(1<<(15- 0)))
#define TSEC_RXBD_RO1		((uint16_t)(1<<(15- 1)))
#define TSEC_RXBD_W			((uint16_t)(1<<(15- 2)))
#define TSEC_RXBD_I			((uint16_t)(1<<(15- 3)))
#define TSEC_RXBD_L			((uint16_t)(1<<(15- 4)))
#define TSEC_RXBD_F			((uint16_t)(1<<(15- 5)))
#define TSEC_RXBD_M			((uint16_t)(1<<(15- 7)))
#define TSEC_RXBD_BC		((uint16_t)(1<<(15- 8)))
#define TSEC_RXBD_MC		((uint16_t)(1<<(15- 9)))
#define TSEC_RXBD_LG		((uint16_t)(1<<(15-10)))
#define TSEC_RXBD_NO		((uint16_t)(1<<(15-11)))
#define TSEC_RXBD_SH		((uint16_t)(1<<(15-12)))
#define TSEC_RXBD_CR		((uint16_t)(1<<(15-13)))
#define TSEC_RXBD_OV		((uint16_t)(1<<(15-14)))
#define TSEC_RXBD_TR		((uint16_t)(1<<(15-15)))

#define TSEC_RXBD_ERROR	\
	(TSEC_RXBD_LG | TSEC_RXBD_NO | TSEC_RXBD_SH | TSEC_RXBD_CR | TSEC_RXBD_OV | TSEC_RXBD_TR )

/* Driver 'private' data */

#define NUM_MC_HASHES		256

struct tsec_private {
	FEC_Enet_Base	base;            /* Controller base address                  */
	FEC_Enet_Base	phy_base;        /* Phy base address (not necessarily identical
	                                  * with controller base address);
	                                  * e.g., phy attached to 2nd controller may be
									  * connected to mii bus of 1st controller.
									  */
	unsigned        phy;             /* Phy address on mii bus                   */
	unsigned        unit;            /* Driver instance (one-based               */
	int				isfec;           /* Set if a FEC (not TSEC) controller       */
	struct tsec_softc *sc;           /* Pointer to BSD driver struct             */
	TSEC_BD			*ring_area;      /* Not necessarily aligned                  */
	TSEC_BD			*tx_ring;        /* Aligned array of TX BDs                  */
	void			**tx_ring_user;  /* Array of user pointers (1 per BD)        */
	unsigned		tx_ring_size;
	unsigned		tx_head;         /* first 'dirty' BD; chip is working on     */
	unsigned		tx_tail;         /* BDs between head and tail                */
	unsigned		tx_avail;        /* Number of available/free TX BDs          */
	TSEC_BD			*rx_ring;        /* Aligned array of RX BDs                  */
	void			**rx_ring_user;  /* Array of user pointers (1 per BD)        */
	unsigned		rx_tail;         /* Where we left off scanning for full bufs */
	unsigned		rx_ring_size;
	void            (*isr)(void*);
	void            *isr_arg;
	void			(*cleanup_txbuf) /* Callback to cleanup TX ring              */
	                  (void*, void*, int);
	void			*cleanup_txbuf_arg;
	void			*(*alloc_rxbuf)  /* Callback for allocating RX buffer        */
	                  (int *psize, uintptr_t *paddr);
	void			(*consume_rxbuf) /* callback to consume RX buffer            */
	                  (void*, void*,  int);
	void			*consume_rxbuf_arg;
	rtems_id		tid;             /* driver task ID                           */
	uint32_t		irq_mask;
	uint32_t		irq_mask_cache;
	uint32_t		irq_pending;
	rtems_event_set	event;           /* Task synchronization events              */
	struct {                         /* Statistics                               */
		unsigned	xirqs;
		unsigned	rirqs;
		unsigned	eirqs;
		unsigned	lirqs;
		unsigned	maxchain;
		unsigned	packet;
		unsigned	odrops;
		unsigned	repack;
		unsigned	eberrs;
		unsigned	dmarst;
	}               stats;
	uint16_t		mc_refcnt[NUM_MC_HASHES];
};

#define NEXT_TXI(mp, i)	(((i)+1) < (mp)->tx_ring_size ? (i)+1 : 0 )
#define NEXT_RXI(mp, i)	(((i)+1) < (mp)->rx_ring_size ? (i)+1 : 0 )

/* Stuff needed for bsdnet support */
struct tsec_bsdsupp {
	int				oif_flags;					/* old / cached if_flags */
};

/* bsdnet driver data              */
struct tsec_softc {
	struct arpcom		arpcom;
	struct tsec_bsdsupp	bsd;
	struct tsec_private	pvt;
};

/* BSP glue information            */
typedef struct tsec_bsp_config {
	uint32_t	  base;
	int			  xirq, rirq, eirq;
	uint32_t      phy_base;
	int           phy_addr;
} TsecBspConfig;

/********** Global Variables ********************/

/* You may override base addresses
 * externally - but you must
 * then also define TSEC_NUM_DRIVER_SLOTS.
 */
#ifndef TSEC_CONFIG

static TsecBspConfig tsec_config[] =
{
	{
		base:     BSP_8540_CCSR_BASE         + 0x24000,
		xirq:     BSP_CORE_IRQ_LOWEST_OFFSET + 13,
		rirq:     BSP_CORE_IRQ_LOWEST_OFFSET + 14,
		eirq:     BSP_CORE_IRQ_LOWEST_OFFSET + 18,
		phy_base: BSP_8540_CCSR_BASE         + 0x24000,
		phy_addr: 1,
	},
	{
		base:     BSP_8540_CCSR_BASE         + 0x25000,
		xirq:     BSP_CORE_IRQ_LOWEST_OFFSET + 19,
		rirq:     BSP_CORE_IRQ_LOWEST_OFFSET + 20,
		eirq:     BSP_CORE_IRQ_LOWEST_OFFSET + 23,
		/* all PHYs are on the 1st adapter's mii bus */
		phy_base: BSP_8540_CCSR_BASE         + 0x24000,
		phy_addr: 2,
	},
};

#define TSEC_CONFIG tsec_config

#endif

#ifndef TSEC_NUM_DRIVER_SLOTS
#define TSEC_NUM_DRIVER_SLOTS	(sizeof(TSEC_CONFIG)/sizeof(TSEC_CONFIG[0]))
#endif

/* Driver data structs */
STATIC struct tsec_softc theTsecEths[TSEC_NUM_DRIVER_SLOTS] = { {{{0}}} };

/* Bsdnet driver task ID; since the BSD stack is single-threaded
 * there is no point having multiple tasks. A single
 * task handling all adapters (attached to BSD stack)
 * is good enough.
 * Note that an adapter might well be used independently
 * from the BSD stack (use the low-level driver interface)
 * and be serviced by a separate task.
 */
STATIC rtems_id tsec_tid = 0;

/* If we anticipate using adapters independently
 * from the BSD stack AND if all PHYs are on a single
 * adapter's MII bus THEN we must mutex-protect
 * that MII bus.
 * If not all of these conditions hold then you
 * may define TSEC_CONFIG_NO_PHY_REGLOCK and
 * avoid the creation and use of a mutex.
 */
#ifndef TSEC_CONFIG_NO_PHY_REGLOCK
/*
 * PHY register access protection mutex;
 * multiple instances of tsec hardware
 * may share e.g., the first tsec's registers
 * for accessing the mii bus where all PHYs
 * may be connected. If we would only deal
 * with BSD networking then using the normal
 * networking semaphore would be OK. However,
 * we want to support standalone drivers and
 * therefore might require a separate lock.
 */
STATIC rtems_id tsec_mtx = 0;
#define REGLOCK()	do { \
		if ( RTEMS_SUCCESSFUL != rtems_semaphore_obtain(tsec_mtx, RTEMS_WAIT, RTEMS_NO_TIMEOUT) ) \
			rtems_panic(DRVNAME": unable to lock phy register protection mutex"); \
		} while (0)
#define REGUNLOCK()	rtems_semaphore_release(tsec_mtx)
#else
#define REGLOCK()	do { } while (0)
#define REGUNLOCK()	do { } while (0)
#endif

static void tsec_xisr(rtems_irq_hdl_param arg);
static void tsec_risr(rtems_irq_hdl_param arg);
static void tsec_eisr(rtems_irq_hdl_param arg);
static void tsec_lisr(rtems_irq_hdl_param arg);

static void noop(const rtems_irq_connect_data *unused)  {            }
static int  nopf(const rtems_irq_connect_data *unused)  { return -1; }

/********** Low-level Driver API ****************/

/*
 * This API provides driver access to applications that
 * want to use e.g., the second ethernet interface
 * independently from the BSD TCP/IP stack. E.g., for
 * raw ethernet packet communication...
 */

/*
 * Descriptor scavenger; cleanup the TX ring, passing all buffers
 * that have been sent to the cleanup_tx() callback.
 * This routine is called from BSP_tsec_send_buf(), BSP_tsec_init_hw(),
 * BSP_tsec_stop_hw().
 *
 * RETURNS: number of buffers processed.
 */

int
BSP_tsec_swipe_tx(struct tsec_private *mp)
{
int						rval = 0;
int                     i;
TSEC_BD					*bd;
uint16_t				flags;
void                   *u;

#if DEBUG > 2
printf("Swipe TX entering:\n");
tsec_dump_tring(mp);
#endif

	for ( i = mp->tx_head; bd_rdbuf( (bd = &mp->tx_ring[i]) ); i = NEXT_TXI(mp, i) ) {

		flags = bd_rdfl( bd );
		if ( (TSEC_TXBD_R & flags) ) {
			/* nothing more to clean */
			break;
		}

		/* tx_ring_user[i] is only set on the last descriptor in a chain;
		 * we only count errors in the last descriptor;
		 */
		if ( (u=mp->tx_ring_user[i]) ) {
			mp->cleanup_txbuf(u, mp->cleanup_txbuf_arg, (flags & TSEC_TXBD_ERRS));
			mp->tx_ring_user[i] = 0;
		}

		bd_wrbuf( bd, 0 );

		mp->tx_avail++;

		rval++;
	}
	mp->tx_head = i;

#if DEBUG > 2
tsec_dump_tring(mp);
printf("Swipe TX leaving\n");
#endif

	return rval;
}


/*
 * Reset the controller and bring into a known state;
 * all interrupts are off
 */
STATIC void
tsec_reset_hw(struct tsec_private *mp)
{
FEC_Enet_Base b = mp->base;

	/* Make sure all interrupts are off */
	fec_wr(b, TSEC_IMASK, TSEC_IMASK_NONE);

#ifndef TSEC_CONFIG_NO_PHY_REGLOCK
	/* don't bother disabling irqs in the PHY if this is
	 * called before the mutex is created;
	 * the PHY ISR is not hooked yet and there can be no
	 * interrupts...
	 */
	if ( tsec_mtx )
#endif
	phy_dis_irq_at_phy( mp );

	mp->irq_mask_cache = 0;

	/* Follow the manual resetting the chip */

	/* Do graceful stop (if not in stop condition already) */
	if ( ! (TSEC_DMACTRL_GTS & fec_rd(b, TSEC_DMACTRL)) ) {
		/* Make sure GTSC is clear */
		fec_wr(b, TSEC_IEVENT, TSEC_IEVENT_GTSC);
		fec_set(b, TSEC_DMACTRL, TSEC_DMACTRL_GTS);
		while ( ! (TSEC_IEVENT_GTSC & fec_rd(b, TSEC_IEVENT)) )
			/* wait */;
	}

	/* Clear RX/TX enable in MAC */
	fec_clr(b, TSEC_MACCFG1, TSEC_MACCFG1_RX_EN | TSEC_MACCFG1_TX_EN);

	/* wait for > 8ms */
	rtems_task_wake_after(1);

	/* set GRS if not already stopped */
	if ( ! (TSEC_DMACTRL_GRS & fec_rd(b, TSEC_DMACTRL)) ) {
		/* Make sure GRSC is clear */
		fec_wr(b, TSEC_IEVENT, TSEC_IEVENT_GRSC);
		fec_set(b, TSEC_DMACTRL, TSEC_DMACTRL_GRS);
		while ( ! (TSEC_IEVENT_GRSC & fec_rd(b, TSEC_IEVENT)) )
			/* wait */;
	}

	fec_set(b, TSEC_MACCFG1, TSEC_MACCFG1_SOFT_RESET);
	fec_clr(b, TSEC_MACCFG1, TSEC_MACCFG1_SOFT_RESET);

	/* clear all irqs */
	fec_wr (b, TSEC_IEVENT, TSEC_IEVENT_ALL);
}

/* Helper to hook/unhook interrupts */

static void
install_remove_isrs(int install, struct tsec_private *mp, uint32_t irq_mask)
{
	rtems_irq_connect_data xxx;
	int                    installed = 0;
	int                    line;
	int                    unit = mp->unit;

	xxx.on     = noop;
	xxx.off    = noop;
	xxx.isOn   = nopf;
	xxx.handle = mp;

	if ( irq_mask & TSEC_TXIRQ ) {
		xxx.name = TSEC_CONFIG[unit-1].xirq;
		xxx.hdl  = tsec_xisr;
		if ( ! (install ?
				BSP_install_rtems_irq_handler( &xxx ) :
				BSP_remove_rtems_irq_handler( &xxx ) ) ) {
			rtems_panic(DRVNAME": Unable to install TX ISR\n");
		}
		installed++;
	}

	if ( (irq_mask & TSEC_RXIRQ) ) {
		if ( (line = TSEC_CONFIG[unit-1].rirq) < 0 && ! installed ) {
			/* have no dedicated RX IRQ line; install TX ISR if not already done */
			line = TSEC_CONFIG[unit-1].xirq;
		}
		xxx.name = line;
		xxx.hdl  = tsec_risr;
		if ( ! (install ?
				BSP_install_rtems_irq_handler( &xxx ) :
				BSP_remove_rtems_irq_handler( &xxx ) ) ) {
			rtems_panic(DRVNAME": Unable to install RX ISR\n");
		}
		installed++;
	}

	if ( (line = TSEC_CONFIG[unit-1].eirq) < 0 && ! installed ) {
		/* have no dedicated RX IRQ line; install TX ISR if not already done */
		line = TSEC_CONFIG[unit-1].xirq;
	}
	xxx.name = line;
	xxx.hdl  = tsec_eisr;
	if ( ! (install ?
			BSP_install_rtems_irq_handler( &xxx ) :
			BSP_remove_rtems_irq_handler( &xxx ) ) ) {
		rtems_panic(DRVNAME": Unable to install ERR ISR\n");
	}

	if ( irq_mask & TSEC_LINK_INTR ) {
		phy_init_irq( install, mp, tsec_lisr );
	}
}

/*
 * Setup an interface.
 * Allocates resources for descriptor rings and sets up the driver software structure.
 *
 * Arguments:
 *	unit:
 *		interface # (1..2). The interface must not be attached to BSD already.
 *
 *  driver_tid:
 *		ISR posts RTEMS event # ('unit' - 1) to task with ID 'driver_tid' and disables interrupts
 *		from this interface.
 *
 *	void (*cleanup_txbuf)(void *user_buf, void *cleanup_txbuf_arg, int error_on_tx_occurred):
 *		Pointer to user-supplied callback to release a buffer that had been sent
 *		by BSP_tsec_send_buf() earlier. The callback is passed 'cleanup_txbuf_arg'
 *		and a flag indicating whether the send had been successful.
 *		The driver no longer accesses 'user_buf' after invoking this callback.
 *		CONTEXT: This callback is executed either by BSP_tsec_swipe_tx() or
 *		BSP_tsec_send_buf(), BSP_tsec_init_hw(), BSP_tsec_stop_hw() (the latter
 *		ones calling BSP_tsec_swipe_tx()).
 *	void *cleanup_txbuf_arg:
 *		Closure argument that is passed on to 'cleanup_txbuf()' callback;
 *
 *	void *(*alloc_rxbuf)(int *p_size, uintptr_t *p_data_addr),
 *		Pointer to user-supplied callback to allocate a buffer for subsequent
 *		insertion into the RX ring by the driver.
 *		RETURNS: opaque handle to the buffer (which may be a more complex object
 *				 such as an 'mbuf'). The handle is not used by the driver directly
 *				 but passed back to the 'consume_rxbuf()' callback.
 *				 Size of the available data area and pointer to buffer's data area
 *				 in '*psize' and '*p_data_area', respectively.
 *				 If no buffer is available, this routine should return NULL in which
 *				 case the driver drops the last packet and re-uses the last buffer
 *				 instead of handing it out to 'consume_rxbuf()'.
 *		CONTEXT: Called when initializing the RX ring (BSP_tsec_init_hw()) or when
 *				 swiping it (BSP_tsec_swipe_rx()).
 *
 *
 *	void (*consume_rxbuf)(void *user_buf, void *consume_rxbuf_arg, int len);
 *		Pointer to user-supplied callback to pass a received buffer back to
 *		the user. The driver no longer accesses the buffer after invoking this
 *		callback (with 'len'>0, see below). 'user_buf' is the buffer handle
 *		previously generated by 'alloc_rxbuf()'.
 *		The callback is passed 'cleanup_rxbuf_arg' and a 'len'
 *		argument giving the number of bytes that were received.
 *		'len' may be <=0 in which case the 'user_buf' argument is NULL.
 *		'len' == 0 means that the last 'alloc_rxbuf()' had failed,
 *		'len' < 0 indicates a receiver error. In both cases, the last packet
 *		was dropped/missed and the last buffer will be re-used by the driver.
 *		NOTE: the data are 'prefixed' with two bytes, i.e., the ethernet packet header
 *		      is stored at offset 2 in the buffer's data area. Also, the FCS (4 bytes)
 *		      is appended. 'len' accounts for both.
 *		CONTEXT: Called from BSP_tsec_swipe_rx().
 *	void *cleanup_rxbuf_arg:
 *		Closure argument that is passed on to 'consume_rxbuf()' callback;
 *
 *  rx_ring_size, tx_ring_size:
 *		How many big to make the RX and TX descriptor rings. Note that the sizes
 *		may be 0 in which case a reasonable default will be used.
 *		If either ring size is < 0 then the RX or TX will be disabled.
 *		Note that it is illegal in this case to use BSP_tsec_swipe_rx() or
 *		BSP_tsec_swipe_tx(), respectively.
 *
 *  irq_mask:
 *		Interrupts to enable. OR of flags from above.
 *
 */

static struct tsec_private *
tsec_setup_internal(
	int		 unit,
	rtems_id driver_tid,
	void     (*isr)(void *),
	void *   isr_arg,
	void     (*cleanup_txbuf)(void *user_buf, void *cleanup_txbuf_arg, int error_on_tx_occurred),
	void *   cleanup_txbuf_arg,
	void *   (*alloc_rxbuf)(int *p_size, uintptr_t *p_data_addr),
	void     (*consume_rxbuf)(void *user_buf, void *consume_rxbuf_arg, int len),
	void *   consume_rxbuf_arg,
	int		 rx_ring_size,
	int		 tx_ring_size,
	int		 irq_mask
)
{
struct tsec_private *mp;
int                  i;
struct ifnet         *ifp;

	if ( unit <= 0 || unit > TSEC_NUM_DRIVER_SLOTS ) {
		printk(DRVNAME": Bad unit number %i; must be 1..%i\n", unit, TSEC_NUM_DRIVER_SLOTS);
		return 0;
	}

	ifp = &theTsecEths[unit-1].arpcom.ac_if;
	if ( ifp->if_init ) {
		if ( ifp->if_init ) {
			printk(DRVNAME": instance %i already attached.\n", unit);
			return 0;
		}
	}


	if ( rx_ring_size < 0 && tx_ring_size < 0 )
		return 0;

	mp                    = &theTsecEths[unit - 1].pvt;

	memset(mp, 0, sizeof(*mp));

	mp->sc                = &theTsecEths[unit - 1];
	mp->unit              = unit;

	mp->base              = (FEC_Enet_Base)TSEC_CONFIG[unit-1].base;
	mp->phy_base          = (FEC_Enet_Base)TSEC_CONFIG[unit-1].phy_base;
	mp->phy               = TSEC_CONFIG[unit-1].phy_addr;
	mp->tid               = driver_tid;
	/* use odd event flags for link status IRQ */
	mp->event             = TSEC_ETH_EVENT((unit-1));

	mp->cleanup_txbuf     = cleanup_txbuf;
	mp->cleanup_txbuf_arg = cleanup_txbuf_arg;
	mp->alloc_rxbuf       = alloc_rxbuf;
	mp->consume_rxbuf     = consume_rxbuf;
	mp->consume_rxbuf_arg = consume_rxbuf_arg;

	/* stop hw prior to setting ring-size to anything nonzero
	 * so that the rings are not swept.
	 */
	BSP_tsec_stop_hw(mp);

	if ( 0 == rx_ring_size )
		rx_ring_size = TSEC_RX_RING_SIZE;
	if ( 0 == tx_ring_size )
		tx_ring_size = TSEC_TX_RING_SIZE;

	mp->rx_ring_size = rx_ring_size < 0 ? 0 : rx_ring_size;
	mp->tx_ring_size = tx_ring_size < 0 ? 0 : tx_ring_size;

	/* allocate ring area; add 1 entry -- room for alignment */
	assert( !mp->ring_area );
	mp->ring_area = malloc(
						sizeof(*mp->ring_area) *
							(mp->rx_ring_size + mp->tx_ring_size + 1),
						M_DEVBUF,
						M_WAIT );
	assert( mp->ring_area );

	mp->tx_ring_user = malloc( sizeof(*mp->tx_ring_user) *
							(mp->rx_ring_size + mp->tx_ring_size),
						M_DEVBUF,
						M_WAIT );
	assert( mp->tx_ring_user );

	mp->rx_ring_user = mp->tx_ring_user + mp->tx_ring_size;

	/* Initialize TX ring */
	mp->tx_ring = (TSEC_BD *) ALIGNTO(mp->ring_area,BD_ALIGNMENT);

	mp->rx_ring = mp->tx_ring + mp->tx_ring_size;

	for ( i=0; i<mp->tx_ring_size; i++ ) {
		bd_wrbuf( &mp->tx_ring[i], 0 );
		bd_wrfl( &mp->tx_ring[i], TSEC_TXBD_I );
		mp->tx_ring_user[i] = 0;
	}
	/* set wrap-around flag on last BD */
	if ( mp->tx_ring_size )
		bd_setfl( &mp->tx_ring[i-1], TSEC_TXBD_W );

	mp->tx_tail = mp->tx_head = 0;
	mp->tx_avail = mp->tx_ring_size;

	/* Initialize RX ring (buffers are allocated later) */
	for ( i=0; i<mp->rx_ring_size; i++ ) {
		bd_wrbuf( &mp->rx_ring[i], 0 );
		bd_wrfl( &mp->rx_ring[i], TSEC_RXBD_I );
		mp->rx_ring_user[i] = 0;
	}
	/* set wrap-around flag on last BD */
	if ( mp->rx_ring_size )
		bd_setfl( &mp->rx_ring[i-1], TSEC_RXBD_W );

	if ( irq_mask ) {
		if ( rx_ring_size == 0 )
			irq_mask &= ~ TSEC_RXIRQ;
		if ( tx_ring_size == 0 )
			irq_mask &= ~ TSEC_TXIRQ;
	}

#ifndef TSEC_CONFIG_NO_PHY_REGLOCK
	if ( ! tsec_mtx ) {
		rtems_status_code     sc;
		rtems_id              new_mtx;
		rtems_interrupt_level l;
		sc = rtems_semaphore_create(
				rtems_build_name('t','s','e','X'),
				1,
				RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY | RTEMS_DEFAULT_ATTRIBUTES,
				0,
				&new_mtx);
		if ( RTEMS_SUCCESSFUL != sc ) {
			rtems_error(sc,DRVNAME": creating mutex\n");
			rtems_panic("unable to proceed\n");
		}
		rtems_interrupt_disable( l );
			if ( ! tsec_mtx ) {
				tsec_mtx = new_mtx;
				new_mtx  = 0;
			}
		rtems_interrupt_enable( l );

		if ( new_mtx ) {
			/* another task was faster installing the mutex */
			rtems_semaphore_delete( new_mtx );
		}

	}
#endif

	if ( irq_mask ) {
		install_remove_isrs( 1, mp, irq_mask );
	}

	mp->irq_mask = irq_mask;

	/* mark as used */
	ifp->if_init = (void*)(-1);

	return mp;
}

struct tsec_private *
BSP_tsec_setup(
	int		 unit,
	rtems_id driver_tid,
	void     (*cleanup_txbuf)(void *user_buf, void *cleanup_txbuf_arg, int error_on_tx_occurred),
	void *   cleanup_txbuf_arg,
	void *   (*alloc_rxbuf)(int *p_size, uintptr_t *p_data_addr),
	void     (*consume_rxbuf)(void *user_buf, void *consume_rxbuf_arg, int len),
	void *   consume_rxbuf_arg,
	int		 rx_ring_size,
	int		 tx_ring_size,
	int		 irq_mask
)
{
	if ( irq_mask && ! driver_tid ) {
		printk(DRVNAME": must supply a TID if irq_mask not zero\n");
		return 0;
	}
	return tsec_setup_internal(
								unit,
								driver_tid,
								0, 0,
								cleanup_txbuf, cleanup_txbuf_arg,
								alloc_rxbuf,
								consume_rxbuf, consume_rxbuf_arg,
								rx_ring_size,
								tx_ring_size,
								irq_mask
							   );
}

struct tsec_private *
BSP_tsec_setup_1(
	int		 unit,
	void     (*isr)(void*),
	void *   isr_arg,
	void     (*cleanup_txbuf)(void *user_buf, void *cleanup_txbuf_arg, int error_on_tx_occurred),
	void *   cleanup_txbuf_arg,
	void *   (*alloc_rxbuf)(int *p_size, uintptr_t *p_data_addr),
	void     (*consume_rxbuf)(void *user_buf, void *consume_rxbuf_arg, int len),
	void *   consume_rxbuf_arg,
	int		 rx_ring_size,
	int		 tx_ring_size,
	int		 irq_mask
)
{
	if ( irq_mask && ! isr ) {
		printk(DRVNAME": must supply a ISR if irq_mask not zero\n");
		return 0;
	}
	return tsec_setup_internal(
								unit,
								0,
								isr, isr_arg,
								cleanup_txbuf, cleanup_txbuf_arg,
								alloc_rxbuf,
								consume_rxbuf, consume_rxbuf_arg,
								rx_ring_size,
								tx_ring_size,
								irq_mask
							   );
}

void
BSP_tsec_reset_stats(struct tsec_private *mp)
{
FEC_Enet_Base b = mp->base;
int i;
	memset( &mp->stats, 0, sizeof( mp->stats ) );
	if ( mp->isfec )
		return;
	for ( i=TSEC_TR64; i<=TSEC_TFRG; i+=4 )
		fec_wr( b, i, 0 );

}

/*
 * retrieve media status from the PHY
 * and set duplex mode in MACCFG2 based
 * on the result.
 *
 * RETURNS: media word (or -1 if BSP_tsec_media_ioctl() fails)
 */
static int
mac_set_duplex(struct tsec_private *mp)
{
int media = IFM_MAKEWORD(0, 0, 0, 0);

	if ( 0 == BSP_tsec_media_ioctl(mp, SIOCGIFMEDIA, &media)) {
		if ( IFM_LINK_OK & media ) {
			/* update duplex setting in MACCFG2 */
			if ( IFM_FDX & media ) {
				fec_set( mp->base, TSEC_MACCFG2, TSEC_MACCFG2_FD );
			} else {
				fec_clr( mp->base, TSEC_MACCFG2, TSEC_MACCFG2_FD );
			}
		}
		return media;
	}
	return -1;
}

/*
 * Initialize interface hardware
 *
 * 'mp'			handle obtained by from BSP_tsec_setup().
 * 'promisc'	whether to set promiscuous flag.
 * 'enaddr'		pointer to six bytes with MAC address. Read
 *				from the device if NULL.
 */
void
BSP_tsec_init_hw(struct tsec_private *mp, int promisc, unsigned char *enaddr)
{
FEC_Enet_Base b = mp->base;
unsigned      i;
uint32_t      v;
int           sz;

	BSP_tsec_stop_hw(mp);

#ifdef PARANOIA
	assert( mp->tx_avail == mp->tx_ring_size );
	assert( mp->tx_head  == mp->tx_tail );
	for ( i=0; i<mp->tx_ring_size; i++ ) {
		assert( mp->tx_ring_user[i] == 0 );
	}
#endif

	/* make sure RX ring is filled */
	for ( i=0; i<mp->rx_ring_size; i++ ) {
		uintptr_t data_area;
		if ( ! (mp->rx_ring_user[i] = mp->alloc_rxbuf( &sz, &data_area)) ) {
			rtems_panic(DRVNAME": unable to fill RX ring");
		}
		if ( data_area & (RX_BUF_ALIGNMENT-1) )
			rtems_panic(DRVNAME": RX buffers must be %i-byte aligned", RX_BUF_ALIGNMENT);

		bd_wrbuf( &mp->rx_ring[i], data_area );
		st_be16 ( &mp->rx_ring[i].len, sz        );
		bd_setfl( &mp->rx_ring[i], TSEC_RXBD_E | TSEC_RXBD_I );
	}

	mp->tx_tail = mp->tx_head = 0;

	mp->rx_tail = 0;

	/* tell chip what the ring areas are */
	fec_wr( b, TSEC_TBASE, (uint32_t)mp->tx_ring );
	fec_wr( b, TSEC_RBASE, (uint32_t)mp->rx_ring );

	/* clear and disable IRQs */
	fec_wr( b, TSEC_IEVENT, TSEC_IEVENT_ALL );
	fec_wr( b, TSEC_IMASK,  TSEC_IMASK_NONE );
	mp->irq_mask_cache = 0;

	/* bring other regs. into a known state */
	fec_wr( b, TSEC_EDIS,   0 );

	if ( !mp->isfec )
		fec_wr( b, TSEC_ECNTRL, TSEC_ECNTRL_CLRCNT | TSEC_ECNTRL_STEN );

	fec_wr( b, TSEC_MINFLR, 64 );
	fec_wr( b, TSEC_PTV,     0 );

	v = TSEC_DMACTRL_WWR;

	if ( mp->tx_ring_size )
		v |= TSEC_DMACTRL_TDSEN | TSEC_DMACTRL_TBDSEN | TSEC_DMACTRL_WOP;

	fec_wr( b, TSEC_DMACTRL, v );

	fec_wr( b, TSEC_FIFO_PAUSE_CTRL,           0 );
	fec_wr( b, TSEC_FIFO_TX_THR,             256 );
	fec_wr( b, TSEC_FIFO_TX_STARVE,          128 );
	fec_wr( b, TSEC_FIFO_TX_STARVE_SHUTOFF,  256 );
	fec_wr( b, TSEC_TCTRL,                     0 );
	if ( !mp->isfec ) {
		/* FIXME: use IRQ coalescing ? not sure how to
		 * set the timer (bad if it depends on the speed
		 * setting).
		 */
		fec_wr( b, TSEC_TXIC, 0);
	}
	fec_wr( b, TSEC_OSTBD,                     0 );
	fec_wr( b, TSEC_RCTRL, (promisc ? TSEC_RCTRL_PROM : 0) );
	fec_wr( b, TSEC_RSTAT,       TSEC_RSTAT_QHLT );
	if ( !mp->isfec ) {
		/* FIXME: use IRQ coalescing ? not sure how to
		 * set the timer (bad if it depends on the speed
		 * setting).
		 */
		fec_wr( b, TSEC_RXIC, 0 );
	}
	fec_wr( b, TSEC_MRBLR,              sz & ~63 );

	/* Reset config. as per manual */
	fec_wr( b, TSEC_IPGIFG,           0x40605060 );
	fec_wr( b, TSEC_HAFDUP,           0x00a1f037 );
	fec_wr( b, TSEC_MAXFRM,                 1536 );

	if ( enaddr ) {
		union {
			uint32_t u;
			uint16_t s[2];
			uint8_t  c[4];
		} x;
		fec_wr( b, TSEC_MACSTNADDR1, ld_le32( (volatile uint32_t*)(enaddr + 2) ) );
		x.s[0] = ld_le16( (volatile uint16_t *)(enaddr) );
		fec_wr( b, TSEC_MACSTNADDR2, x.u );
	}

	for ( i=0; i<8*4; i+=4 ) {
		fec_wr( b, TSEC_IADDR0 + i, 0 );
	}

	BSP_tsec_mcast_filter_clear(mp);

	BSP_tsec_reset_stats(mp);

	fec_wr( b, TSEC_ATTR, (TSEC_ATTR_RDSEN | TSEC_ATTR_RBDSEN) );
	fec_wr( b, TSEC_ATTRELI,        0 );

	/* The interface type is probably board dependent; leave alone...
	v  = mp->isfec ? TSEC_MACCFG2_IF_MODE_MII : TSEC_MACCFG2_IF_MODE_GMII;
	*/

	fec_clr( b, TSEC_MACCFG2,
			  TSEC_MACCFG2_PREAMBLE_15
			| TSEC_MACCFG2_HUGE_FRAME
			| TSEC_MACCFG2_LENGTH_CHECK );

	fec_set( b, TSEC_MACCFG2,
			  TSEC_MACCFG2_PREAMBLE_7
			| TSEC_MACCFG2_PAD_CRC );

	mac_set_duplex( mp );

	v = 0;
	if ( mp->rx_ring_size ) {
		v |= TSEC_MACCFG1_RX_EN;
	}
	if ( mp->tx_ring_size ) {
		v |= TSEC_MACCFG1_TX_EN;
	}

	fec_wr( b, TSEC_MACCFG1, v);

	/* The following sequency (FWIW) ensures that
	 *
	 * - PHY and TSEC interrupts are enabled atomically
	 * - IRQS are not globally off while accessing the PHY
	 *   (slow MII)
	 */

	if ( (TSEC_LINK_INTR & mp->irq_mask) ) {
		/* disable PHY irq at PIC (fast) */
		phy_dis_irq( mp );
		/* enable PHY irq (MII operation, slow) */
		phy_en_irq_at_phy (mp );
	}

	BSP_tsec_enable_irq_mask( mp, mp->irq_mask );
}

static void
hash_prog(struct tsec_private *mp, uint32_t tble, const uint8_t *enaddr, int accept)
{
uint8_t  s;
uint32_t reg, bit;

	s = ether_crc32_le(enaddr, ETHER_ADDR_LEN);

	/* bit-reverse */
    s = ((s&0x0f) << 4) | ((s&0xf0) >> 4);
    s = ((s&0x33) << 2) | ((s&0xcc) >> 2);
    s = ((s&0x55) << 1) | ((s&0xaa) >> 1);

	reg = tble + ((s >> (5-2)) & ~3);
	bit = 1 << (31 - (s & 31));

	if ( accept ) {
		if ( 0 == mp->mc_refcnt[s]++ )
			fec_set( mp->base, reg, bit );
	} else {
		if ( mp->mc_refcnt[s] > 0 && 0 == --mp->mc_refcnt[s] )
			fec_clr( mp->base, reg, bit );
	}
}

void
BSP_tsec_mcast_filter_clear(struct tsec_private *mp)
{
int i;
	for ( i=0; i<8*4; i+=4 ) {
		fec_wr( mp->base, TSEC_GADDR0 + i, 0 );
	}
	for ( i=0; i<NUM_MC_HASHES; i++ )
		mp->mc_refcnt[i] = 0;
}

void
BSP_tsec_mcast_filter_accept_all(struct tsec_private *mp)
{
int i;
	for ( i=0; i<8*4; i+=4 ) {
		fec_wr( mp->base, TSEC_GADDR0 + i, 0xffffffff );
	}
	for ( i=0; i<NUM_MC_HASHES; i++ )
		mp->mc_refcnt[i]++;
}

static void
mcast_filter_prog(struct tsec_private *mp, uint8_t *enaddr, int accept)
{
static const uint8_t bcst[6]={0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	if ( ! (enaddr[0] & 0x01) ) {
		/* not a multicast address; ignore */
		return;
	}
	if ( 0 == memcmp( enaddr, bcst, sizeof(bcst) ) ) {
		/* broadcast; ignore */
		return;
	}
	hash_prog(mp, TSEC_GADDR0, enaddr, accept);
}

void
BSP_tsec_mcast_filter_accept_add(struct tsec_private *mp, uint8_t *enaddr)
{
	mcast_filter_prog(mp, enaddr, 1 /* accept */);
}

void
BSP_tsec_mcast_filter_accept_del(struct tsec_private *mp, uint8_t *enaddr)
{
	mcast_filter_prog(mp, enaddr, 0 /* delete */);
}

void
BSP_tsec_dump_stats(struct tsec_private *mp, FILE *f)
{
FEC_Enet_Base b;

	if ( !mp )
		mp = &theTsecEths[0].pvt;

	if ( ! f )
		f = stdout;

	fprintf(f, DRVNAME"%i Statistics:\n", mp->unit);

    b = mp->base;

	fprintf(f, "TX  IRQS: %u\n", mp->stats.xirqs);
	fprintf(f, "RX  IRQS: %u\n", mp->stats.rirqs);
	fprintf(f, "ERR IRQS: %u\n", mp->stats.eirqs);
	fprintf(f, "bus errs: %u\n", mp->stats.eberrs);
	fprintf(f, "dmawhack: %u\n", mp->stats.dmarst);
	fprintf(f, "LNK IRQS: %u\n", mp->stats.lirqs);
	fprintf(f, "maxchain: %u\n", mp->stats.maxchain);
	fprintf(f, "xpackets: %u\n", mp->stats.packet);
	fprintf(f, "odrops:   %u\n", mp->stats.odrops);
	fprintf(f, "repack:   %u\n", mp->stats.repack);

	if ( mp->isfec ) {
		fprintf(f,"FEC has no HW counters\n");
		return;
	}

	fprintf(f,"TSEC MIB counters (modulo 2^32):\n");

	fprintf(f,"RX bytes     %"PRIu32"\n", fec_rd( b, TSEC_RBYT ));
	fprintf(f,"RX      pkts %"PRIu32"\n", fec_rd( b, TSEC_RPKT ));
	fprintf(f,"RX FCS errs  %"PRIu32"\n", fec_rd( b, TSEC_RFCS ));
	fprintf(f,"RX mcst pkts %"PRIu32"\n", fec_rd( b, TSEC_RMCA ));
	fprintf(f,"RX bcst pkts %"PRIu32"\n", fec_rd( b, TSEC_RBCA ));
	fprintf(f,"RX pse frms  %"PRIu32"\n", fec_rd( b, TSEC_RXPF ));
	fprintf(f,"RX drop      %"PRIu32"\n", fec_rd( b, TSEC_RDRP ));
	fprintf(f,"TX bytes     %"PRIu32"\n", fec_rd( b, TSEC_TBYT ));
	fprintf(f,"TX      pkts %"PRIu32"\n", fec_rd( b, TSEC_TPKT ));
	fprintf(f,"TX FCS errs  %"PRIu32"\n", fec_rd( b, TSEC_TFCS ));
	fprintf(f,"TX mcst pkts %"PRIu32"\n", fec_rd( b, TSEC_TMCA ));
	fprintf(f,"TX bcst pkts %"PRIu32"\n", fec_rd( b, TSEC_TBCA ));
	fprintf(f,"TX pse frms  %"PRIu32"\n", fec_rd( b, TSEC_TXPF ));
	fprintf(f,"TX drop      %"PRIu32"\n", fec_rd( b, TSEC_TDRP ));
	fprintf(f,"TX coll      %"PRIu32"\n", fec_rd( b, TSEC_TSCL ));
	fprintf(f,"TX mcoll     %"PRIu32"\n", fec_rd( b, TSEC_TMCL ));
	fprintf(f,"TX late coll %"PRIu32"\n", fec_rd( b, TSEC_TLCL ));
	fprintf(f,"TX exc coll  %"PRIu32"\n", fec_rd( b, TSEC_TXCL ));
	fprintf(f,"TX defer     %"PRIu32"\n", fec_rd( b, TSEC_TDFR ));
	fprintf(f,"TX exc defer %"PRIu32"\n", fec_rd( b, TSEC_TEDF ));
	fprintf(f,"TX oversz    %"PRIu32"\n", fec_rd( b, TSEC_TOVR ));
	fprintf(f,"TX undersz   %"PRIu32"\n", fec_rd( b, TSEC_TUND ));
}

/*
 * Shutdown hardware and clean out the rings
 */
void
BSP_tsec_stop_hw(struct tsec_private *mp)
{
unsigned i;
	/* stop and reset hardware */
	tsec_reset_hw( mp );

	if ( mp->tx_ring_size ) {
		/* should be OK to clear all ownership flags */
		for ( i=0; i<mp->tx_ring_size; i++ ) {
			bd_clrfl( &mp->tx_ring[i], TSEC_TXBD_R );
		}
		BSP_tsec_swipe_tx(mp);
#if DEBUG > 0
		tsec_dump_tring(mp);
		fflush(stderr); fflush(stdout);
#endif
#ifdef PARANOIA
		assert( mp->tx_avail == mp->tx_ring_size );
		assert( mp->tx_head  == mp->tx_tail      );
		for ( i=0; i<mp->tx_ring_size; i++ ) {
			assert( !bd_rdbuf( & mp->tx_ring[i] ) );
			assert( !mp->tx_ring_user[i] );
		}
#endif
	}

	if ( mp->rx_ring_size ) {
		for ( i=0; i<mp->rx_ring_size; i++ ) {
			bd_clrfl( &mp->rx_ring[i], TSEC_RXBD_E );
			bd_wrbuf( &mp->rx_ring[i], 0 );
			if ( mp->rx_ring_user[i] )
				mp->consume_rxbuf( mp->rx_ring_user[i], mp->consume_rxbuf_arg, 0 );
			mp->rx_ring_user[i] = 0;
		}
	}
}

/*
 * calls BSP_tsec_stop_hw(), releases all resources and marks the interface
 * as unused.
 * RETURNS 0 on success, nonzero on failure.
 * NOTE:   the handle MUST NOT be used after successful execution of this
 *         routine.
 */
int
BSP_tsec_detach(struct tsec_private *mp)
{

	if ( ! mp || !mp->sc || ! mp->sc->arpcom.ac_if.if_init ) {
		fprintf(stderr,"Unit not setup -- programming error!\n");
		return -1;
	}

	BSP_tsec_stop_hw(mp);

	install_remove_isrs( 0, mp, mp->irq_mask );

	free( (void*)mp->ring_area, M_DEVBUF );
	free( (void*)mp->tx_ring_user, M_DEVBUF );
	memset(mp, 0, sizeof(*mp));
	__asm__ __volatile__("":::"memory");

	/* mark as unused */
	mp->sc->arpcom.ac_if.if_init = 0;

	return 0;
}

/*
 * Enqueue a mbuf chain or a raw data buffer for transmission;
 * RETURN: #bytes sent or -1 if there are not enough free descriptors
 *
 * If 'len' is <=0 then 'm_head' is assumed to point to a mbuf chain.
 * OTOH, a raw data packet (or a different type of buffer)
 * may be sent (non-BSD driver) by pointing data_p to the start of
 * the data and passing 'len' > 0.
 * 'm_head' is passed back to the 'cleanup_txbuf()' callback.
 *
 * Comments: software cache-flushing incurs a penalty if the
 *           packet cannot be queued since it is flushed anyways.
 *           The algorithm is slightly more efficient in the normal
 *			 case, though.
 *
 * RETURNS: # bytes enqueued to device for transmission or -1 if no
 *          space in the TX ring was available.
 */

#if 0
#define NEXT_TXD(mp, bd) ((bd_rdfl( bd ) & TSEC_TXBD_W) ? mp->tx_ring : (bd + 1))
#endif

/*
 * allocate a new cluster and copy an existing chain there;
 * old chain is released...
 */
static struct mbuf *
repackage_chain(struct mbuf *m_head)
{
struct mbuf *m;
	MGETHDR(m, M_DONTWAIT, MT_DATA);

	if ( !m ) {
		goto bail;
	}

	MCLGET(m, M_DONTWAIT);

	if ( !(M_EXT & m->m_flags) ) {
		m_freem(m);
		m = 0;
		goto bail;
	}

	m_copydata(m_head, 0, MCLBYTES, mtod(m, caddr_t));
	m->m_pkthdr.len = m->m_len = m_head->m_pkthdr.len;

bail:
	m_freem(m_head);
	return m;
}

static inline unsigned
tsec_assign_desc( TSEC_BD *bd, uint32_t buf, unsigned len, uint32_t flags)
{
	st_be16 ( &bd->len, (uint16_t)len );
	bd_wrbuf( bd, buf );
	bd_cslfl( bd, flags, TSEC_TXBD_R | TSEC_TXBD_L );
	return len;
}


int
BSP_tsec_send_buf(struct tsec_private *mp, void *m_head, void *data_p, int len)
{
int						rval;
register TSEC_BD		*bd;
register unsigned       l,d,t;
register struct mbuf	*m1;
int						nmbs;
int						ismbuf = (len <= 0);

#if DEBUG > 2
	printf("send entering...\n");
	tsec_dump_tring(mp);
#endif
/* Only way to get here is when we discover that the mbuf chain
 * is too long for the tx ring
 */
startover:

	rval = 0;

#ifdef PARANOIA
	assert(m_head);
#endif

	/* if no descriptor is available; try to wipe the queue */
	if ( (mp->tx_avail < 1) && TSEC_CLEAN_ON_SEND(mp)<=0 ) {
		return -1;
	}

	t = mp->tx_tail;

#ifdef PARANOIA
	assert( ! bd_rdbuf( &mp->tx_ring[t] ) );
	assert( ! mp->tx_ring_user[t]         );
#endif

	if ( ! (m1 = m_head) )
		return 0;

	if ( ismbuf ) {
		/* find first mbuf with actual data */
		while ( 0 == m1->m_len ) {
			if ( ! (m1 = m1->m_next) ) {
				/* end reached and still no data to send ?? */
				m_freem(m_head);
				return 0;
			}
		}
	}

	/* Don't use the first descriptor yet because BSP_tsec_swipe_tx()
	 * needs bd->buf == NULL as a marker. Hence, we
	 * start with the second mbuf and fill the first descriptor
	 * last.
	 */

	l = t;
	d = NEXT_TXI(mp,t);

	mp->tx_avail--;

	nmbs = 1;
	if ( ismbuf ) {
			register struct mbuf *m;
			for ( m=m1->m_next; m; m=m->m_next ) {
					if ( 0 == m->m_len )
							continue;	/* skip empty mbufs */

					nmbs++;

					if ( mp->tx_avail < 1 && TSEC_CLEAN_ON_SEND(mp)<=0 ) {
							/* not enough descriptors; cleanup...
							 * the first slot was never used, so we start
							 * at mp->d_tx_h->next;
							 */
							for ( l = NEXT_TXI(mp, t); l!=d; l=NEXT_TXI(mp, l) ) {
									bd = & mp->tx_ring[l];
#ifdef PARANOIA
									assert( mp->tx_ring_user[l] == 0 );
#endif
									bd_wrbuf( bd, 0 );
									bd_clrfl( bd, TSEC_TXBD_R | TSEC_TXBD_L );

									mp->tx_avail++;
							}
							mp->tx_avail++;
							if ( nmbs > TX_AVAILABLE_RING_SIZE(mp) ) {
									/* this chain will never fit into the ring */
									if ( nmbs > mp->stats.maxchain )
											mp->stats.maxchain = nmbs;
									mp->stats.repack++;
									if ( ! (m_head = repackage_chain(m_head)) ) {
											/* no cluster available */
											mp->stats.odrops++;
#ifdef PARANOIA
											printf("send return 0\n");
											tsec_dump_tring(mp);
#endif
											return 0;
									}
#ifdef PARANOIA
									printf("repackaged; start over\n");
#endif
									goto startover;
							}
#ifdef PARANOIA
							printf("send return -1\n");
							tsec_dump_tring(mp);
#endif
							return -1;
					}

					mp->tx_avail--;

#ifdef PARANOIA
					assert( ! mp->tx_ring_user[d]         );
					if ( d == t ) {
						tsec_dump_tring(mp);
						printf("l %i, d %i, t %i, nmbs %i\n", l,d,t, nmbs);
					} else
					assert( d != t                        );
					assert( ! bd_rdbuf( &mp->tx_ring[d] ) );
#endif

					/* fill this slot */
					rval += tsec_assign_desc( &mp->tx_ring[d], mtod(m, uint32_t), m->m_len, TSEC_TXBD_R);

					FLUSH_BUF(mtod(m, uint32_t), m->m_len);

					l = d;
					d = NEXT_TXI(mp, d);
			}

		/* fill first slot - don't release to DMA yet */
		rval += tsec_assign_desc( &mp->tx_ring[t], mtod(m1, uint32_t), m1->m_len, 0);


		FLUSH_BUF(mtod(m1, uint32_t), m1->m_len);

	} else {
		/* fill first slot with raw buffer - don't release to DMA yet */
		rval += tsec_assign_desc( &mp->tx_ring[t], (uint32_t)data_p, len, 0);

		FLUSH_BUF( (uint32_t)data_p, len);
	}

	/* tag last slot; this covers the case where 1st==last */
	bd_setfl( &mp->tx_ring[l], TSEC_TXBD_L );

	/* mbuf goes into last desc */
	mp->tx_ring_user[l] = m_head;

	membarrier();

	/* turn over the whole chain by flipping ownership of the first desc */
	bd_setfl( &mp->tx_ring[t], TSEC_TXBD_R );

	membarrier();

#if DEBUG > 2
	printf("send return (l=%i, t=%i, d=%i) %i\n", l, t, d, rval);
	tsec_dump_tring(mp);
#endif

	/* notify the device */
	fec_wr( mp->base, TSEC_TSTAT, TSEC_TSTAT_THLT );

	/* Update softc */
	mp->stats.packet++;
	if ( nmbs > mp->stats.maxchain )
		mp->stats.maxchain = nmbs;

	/* remember new tail */
	mp->tx_tail = d;

	return rval; /* #bytes sent */
}

/*
 * Retrieve all received buffers from the RX ring, replacing them
 * by fresh ones (obtained from the alloc_rxbuf() callback). The
 * received buffers are passed to consume_rxbuf().
 *
 * RETURNS: number of buffers processed.
 */
int
BSP_tsec_swipe_rx(struct tsec_private *mp)
{
int						rval = 0, err;
unsigned                i;
uint16_t				flags;
TSEC_BD                 *bd;
void					*newbuf;
int						sz;
uint16_t                len;
uintptr_t               baddr;

	i     = mp->rx_tail;
	bd    = mp->rx_ring + i;
	flags = bd_rdfl( bd );

	while ( ! (TSEC_RXBD_E & flags) ) {

		/* err is not valid if not qualified by TSEC_RXBD_L */
		if ( ( err = (TSEC_RXBD_ERROR & flags) ) ) {
			/* make sure error is < 0 */
			err |= 0xffff0000;
			/* pass 'L' flag out so they can verify... */
			err |= (flags & TSEC_RXBD_L);
		}

#ifdef PARANOIA
		assert( flags & TSEC_RXBD_L );
		assert( mp->rx_ring_user[i] );
#endif

		if ( err || !(newbuf = mp->alloc_rxbuf(&sz, &baddr)) ) {
			/* drop packet and recycle buffer */
			newbuf = mp->rx_ring_user[i];
			mp->consume_rxbuf( 0, mp->consume_rxbuf_arg, err );
		} else {
			len = ld_be16( &bd->len );

#ifdef PARANOIA
			assert( 0 == (baddr & (RX_BUF_ALIGNMENT-1)) );
			assert( len > 0 );
#endif

			mp->consume_rxbuf( mp->rx_ring_user[i], mp->consume_rxbuf_arg, len );

			mp->rx_ring_user[i] = newbuf;
			st_be16( &bd->len, sz );
			bd_wrbuf( bd, baddr );
		}

		RTEMS_COMPILER_MEMORY_BARRIER();

		bd_wrfl( &mp->rx_ring[i], (flags & ~TSEC_RXBD_ERROR) | TSEC_RXBD_E );

		rval++;

		i  = NEXT_RXI( mp, i );
		bd = mp->rx_ring + i;
		flags = bd_rdfl( bd );
	}

	fec_wr( mp->base, TSEC_RSTAT, TSEC_RSTAT_QHLT );

	mp->rx_tail = i;
	return rval;
}

/* read ethernet address from hw to buffer */
void
BSP_tsec_read_eaddr(struct tsec_private *mp, unsigned char *eaddr)
{
union {
	uint32_t u;
	uint16_t s[2];
	uint8_t  c[4];
} x;
	st_le32( (volatile uint32_t *)(eaddr+2), fec_rd(mp->base, TSEC_MACSTNADDR1) );
	x.u = fec_rd(mp->base, TSEC_MACSTNADDR2);
	st_le16( (volatile uint16_t *)(eaddr), x.s[0]);
}

/* mdio / mii interface wrappers for rtems_mii_ioctl API */

/*
 * Busy-wait -- this can take a while: I measured ~550 timebase-ticks
 * @333333333Hz, TB divisor is 8 -> 13us
 */
static inline void mii_wait(FEC_Enet_Base b)
{
	while ( (TSEC_MIIMIND_BUSY & fec_rd( b, TSEC_MIIMIND )) )
		;
}

/* MII operations are rather slow :-( */
static int
tsec_mdio_rd(int phyidx, void *uarg, unsigned reg, uint32_t *pval)
{
uint32_t             v;
#ifdef TEST_MII_TIMING
uint32_t             t;
#endif
struct tsec_private *mp = uarg;
FEC_Enet_Base        b  = mp->phy_base;

	if ( phyidx != 0 )
		return -1; /* only one phy supported for now */

	/* write phy and register address */
	fec_wr( b, TSEC_MIIMADD, TSEC_MIIMADD_ADDR(mp->phy,reg) );

	/* clear READ bit */
	v = fec_rd( b, TSEC_MIIMCOM );
	fec_wr( b, TSEC_MIIMCOM, v & ~TSEC_MIIMCOM_READ );

#ifdef TEST_MII_TIMING
	t = tb_rd();
#endif

	/* trigger READ operation by READ-bit 0-->1 transition */
	fec_wr( b, TSEC_MIIMCOM, v |  TSEC_MIIMCOM_READ );

	/* (busy) wait for completion */

	mii_wait( b );

	/* Ugly workaround: I observed that the link status
	 * is not correctly reported when the link changes to
	 * a good status - a failed link is reported until
	 * we read twice :-(
	 */
	if ( MII_BMSR == reg ) {
		/* trigger a second read operation */
		fec_wr( b, TSEC_MIIMCOM, v & ~TSEC_MIIMCOM_READ );
		fec_wr( b, TSEC_MIIMCOM, v |  TSEC_MIIMCOM_READ );
		mii_wait( b );
	}

#ifdef TEST_MII_TIMING
	t = tb_rd() - t;
	printf("Reading MII took %"PRIi32"\n", t);
#endif
	/* return result */
	*pval = fec_rd( b, TSEC_MIIMSTAT ) & 0xffff;
	return 0;
}

STATIC int
tsec_mdio_wr(int phyidx, void *uarg, unsigned reg, uint32_t val)
{
#ifdef TEST_MII_TIMING
uint32_t             t;
#endif
struct tsec_private *mp = uarg;
FEC_Enet_Base        b  = mp->phy_base;

	if ( phyidx != 0 )
		return -1; /* only one phy supported for now */

#ifdef TEST_MII_TIMING
	t = tb_rd();
#endif

	fec_wr( b, TSEC_MIIMADD, TSEC_MIIMADD_ADDR(mp->phy,reg) );
	fec_wr( b, TSEC_MIIMCON, val & 0xffff );

	mii_wait( b );

#ifdef TEST_MII_TIMING
	t = tb_rd() - t;
	printf("Writing MII took %"PRIi32"\n", t);
#endif

	return 0;
}

/* Public, locked versions */
uint32_t
BSP_tsec_mdio_rd(struct tsec_private *mp, unsigned reg)
{
uint32_t val, rval;

	REGLOCK();
	rval = tsec_mdio_rd(0, mp, reg, &val ) ? -1 : val;
	REGUNLOCK();

	return rval;
}

int
BSP_tsec_mdio_wr(struct tsec_private *mp, unsigned reg, uint32_t val)
{
int rval;

	REGLOCK();
	rval = tsec_mdio_wr(0, mp, reg, val);
	REGUNLOCK();

	return rval;
}

static struct rtems_mdio_info tsec_mdio = {
	mdio_r:	  tsec_mdio_rd,
	mdio_w:	  tsec_mdio_wr,
	has_gmii: 1,
};


/*
 * read/write media word.
 *   'cmd': can be SIOCGIFMEDIA, SIOCSIFMEDIA, 0 or 1. The latter
 *          are aliased to the former for convenience.
 *  'parg': pointer to media word.
 *
 * RETURNS: 0 on success, nonzero on error
 */
int
BSP_tsec_media_ioctl(struct tsec_private *mp, int cmd, int *parg)
{
int rval;
	/* alias cmd == 0,1 for convenience when calling from shell */
	switch ( cmd ) {
		case 0: cmd = SIOCGIFMEDIA;
			break;
		case 1: cmd = SIOCSIFMEDIA;
		case SIOCGIFMEDIA:
		case SIOCSIFMEDIA:
			break;
		default: return -1;
	}
	REGLOCK();
	tsec_mdio.has_gmii = mp->isfec ? 0 : 1;
	rval = rtems_mii_ioctl(&tsec_mdio, mp, cmd, parg);
	REGUNLOCK();
	return rval;
}

/* Interrupt related routines */

/*
 * When it comes to interrupts the chip has two rather
 * annoying features:
 *   1 once an IRQ is pending, clearing the IMASK does not
 *     de-assert the interrupt line.
 *   2 the chip has three physical interrupt lines even though
 *     all events are reported in a single register. Rather
 *     useless; we must hook 3 ISRs w/o any real benefit.
 *     In fact, it makes our life a bit more difficult:
 *
 * Hence, for (1) we would have to mask interrupts at the PIC
 * but to re-enable them we would have to do that three times
 * because of (2).
 *
 * Therefore, we take the following approach:
 *
 *   ISR masks interrupts on the TSEC, acks/clears them
 *   and stores the acked irqs in the device struct where
 *   it is picked up by BSP_tsec_ack_irq_mask().
 *
 */

static inline uint32_t
tsec_dis_irqs(struct tsec_private *mp, uint32_t mask)
{
uint32_t rval;

	rval = mp->irq_mask_cache;
	if ( (TSEC_LINK_INTR & mask & mp->irq_mask_cache) )
		phy_dis_irq( mp );
	mp->irq_mask_cache = rval & ~mask;
	fec_wr( mp->base, TSEC_IMASK, (mp->irq_mask_cache & ~TSEC_LINK_INTR) );

	return rval;
}

static inline uint32_t
tsec_dis_clr_irqs(struct tsec_private *mp)
{
uint32_t      rval;
FEC_Enet_Base b = mp->base;

	rval  = fec_rd( b, TSEC_IEVENT);

	/* Make sure we mask out the link intr */
	rval &= ~TSEC_LINK_INTR;

	tsec_dis_irqs( mp, rval );
	fec_wr( b, TSEC_IEVENT, rval );

	return rval;
}

/*
 * We have 3 different ISRs just so we can count
 * interrupt types independently...
 */

static void tsec_xisr(rtems_irq_hdl_param arg)
{
struct tsec_private   *mp = (struct tsec_private *)arg;
rtems_interrupt_level l;

	rtems_interrupt_disable( l );
		mp->irq_pending |= tsec_dis_clr_irqs( mp );
	rtems_interrupt_enable( l );

	mp->stats.xirqs++;

	if ( mp->isr )
		mp->isr( mp->isr_arg );
	else
		rtems_event_send( mp->tid, mp->event );
}

static void tsec_risr(rtems_irq_hdl_param arg)
{
struct tsec_private   *mp = (struct tsec_private *)arg;
rtems_interrupt_level l;

	rtems_interrupt_disable( l );
		mp->irq_pending |= tsec_dis_clr_irqs( mp );
	rtems_interrupt_enable( l );

	mp->stats.rirqs++;

	if ( mp->isr )
		mp->isr( mp->isr_arg );
	else
		rtems_event_send( mp->tid, mp->event );
}

static void tsec_eisr(rtems_irq_hdl_param arg)
{
struct tsec_private   *mp = (struct tsec_private *)arg;
rtems_interrupt_level l;
uint32_t              pending;

	rtems_interrupt_disable( l );
		/* make local copy since ISR may ack and clear mp->pending;
		 * also, we want the fresh bits not the ORed state including
		 * the past...
		 */
		mp->irq_pending |= (pending = tsec_dis_clr_irqs( mp ));
	rtems_interrupt_enable( l );

	mp->stats.eirqs++;

	if ( mp->isr )
		mp->isr( mp->isr_arg );
	else
		rtems_event_send( mp->tid, mp->event );

	if ( (TSEC_IEVENT_TXE & pending) ) {
		if ( (TSEC_IEVENT_EBERR & pending) && ++mp->stats.eberrs > MAXEBERRS ) {
			printk(DRVNAME" BAD error: max # of DMA bus errors reached\n");
		} else {
			/* Restart DMA -- do we have to clear DMACTRL[GTS], too ?? */
			fec_wr( mp->base, TSEC_TSTAT, TSEC_TSTAT_THLT );
			mp->stats.dmarst++;
		}
	}
}

static void tsec_lisr(rtems_irq_hdl_param arg)
{
struct tsec_private   *mp = (struct tsec_private *)arg;
rtems_interrupt_level l;

	if ( phy_irq_pending( mp ) ) {

		rtems_interrupt_disable( l );
			tsec_dis_irqs( mp, TSEC_LINK_INTR );
			mp->irq_pending |= TSEC_LINK_INTR;
		rtems_interrupt_enable( l );

		mp->stats.lirqs++;

		if ( mp->isr )
			mp->isr( mp->isr_arg );
		else
			rtems_event_send( mp->tid, mp->event );
	}
}

/* Enable interrupts at device */
void
BSP_tsec_enable_irq_mask(struct tsec_private *mp, uint32_t mask)
{
rtems_interrupt_level l;

	mask &= mp->irq_mask;

	rtems_interrupt_disable( l );
	if ( (TSEC_LINK_INTR & mask) && ! (TSEC_LINK_INTR & mp->irq_mask_cache) )
		phy_en_irq( mp );
	mp->irq_mask_cache |= mask;
	fec_wr( mp->base, TSEC_IMASK, (mp->irq_mask_cache & ~TSEC_LINK_INTR) );
	rtems_interrupt_enable( l );
}

void
BSP_tsec_enable_irqs(struct tsec_private *mp)
{
	BSP_tsec_enable_irq_mask(mp, -1);
}

/* Disable interrupts at device */
uint32_t
BSP_tsec_disable_irq_mask(struct tsec_private *mp, uint32_t mask)
{
uint32_t              rval;
rtems_interrupt_level l;

	rtems_interrupt_disable( l );
		rval = tsec_dis_irqs(mp, mask);
	rtems_interrupt_enable( l );

	return rval;
}

void
BSP_tsec_disable_irqs(struct tsec_private *mp)
{
rtems_interrupt_level l;

	rtems_interrupt_disable( l );
		tsec_dis_irqs(mp, -1);
	rtems_interrupt_enable( l );
}

/*
 * Acknowledge (and clear) interrupts.
 * RETURNS: interrupts that were raised.
 */
uint32_t
BSP_tsec_ack_irq_mask(struct tsec_private *mp, uint32_t mask)
{
uint32_t              rval;
rtems_interrupt_level l;

	rtems_interrupt_disable( l );
		rval = mp->irq_pending;
		mp->irq_pending &= ~ mask;
	rtems_interrupt_enable( l );

	if ( (rval & TSEC_LINK_INTR & mask) ) {
		/* interacting with the PHY is slow so
		 * we do it only if we have to...
		 */
		phy_ack_irq( mp );
	}

	return rval & mp->irq_mask;
}

uint32_t
BSP_tsec_ack_irqs(struct tsec_private *mp)
{
	return BSP_tsec_ack_irq_mask(mp, -1);
}

/* Retrieve the driver daemon TID that was passed to
 * BSP_tsec_setup().
 */

rtems_id
BSP_tsec_get_tid(struct tsec_private *mp)
{
	return mp->tid;
}

struct tsec_private *
BSP_tsec_getp(unsigned index)
{
	if ( index >= TSEC_NUM_DRIVER_SLOTS )
		return 0;
	return & theTsecEths[index].pvt;
}

/*
 *
 * Example driver task loop (note: no synchronization of
 * buffer access shown!).
 * RTEMS_EVENTx = 0,1 or 2 depending on IF unit.
 *
 *    / * setup (obtain handle) and initialize hw here * /
 *
 *    do {
 *      / * ISR disables IRQs and posts event * /
 *		rtems_event_receive( RTEMS_EVENTx, RTEMS_WAIT | RTEMS_EVENT_ANY, RTEMS_NO_TIMEOUT, &evs );
 *		irqs = BSP_tsec_ack_irqs(handle);
 *      if ( irqs & BSP_TSEC_IRQ_TX ) {
 *			BSP_tsec_swipe_tx(handle); / * cleanup_txbuf() callback executed * /
 *		}
 *      if ( irqs & BSP_TSEC_IRQ_RX ) {
 *			BSP_tsec_swipe_rx(handle); / * alloc_rxbuf() and consume_rxbuf() executed * /
 *		}
 *		BSP_tsec_enable_irq_mask(handle, -1);
 *    } while (1);
 *
 */

/* BSDNET SUPPORT/GLUE ROUTINES */

STATIC void
tsec_stop(struct tsec_softc *sc)
{
	BSP_tsec_stop_hw(&sc->pvt);
	sc->arpcom.ac_if.if_timer = 0;
}

/* allocate a mbuf for RX with a properly aligned data buffer
 * RETURNS 0 if allocation fails
 */
static void *
alloc_mbuf_rx(int *psz, uintptr_t *paddr)
{
struct mbuf		*m;
unsigned long	l,o;

	MGETHDR(m, M_DONTWAIT, MT_DATA);
	if ( !m )
		return 0;
	MCLGET(m, M_DONTWAIT);
	if ( ! (m->m_flags & M_EXT) ) {
		m_freem(m);
		return 0;
	}

	o = mtod(m, unsigned long);
	l = ALIGNTO(o, RX_BUF_ALIGNMENT) - o;

	/* align start of buffer */
	m->m_data += l;

	/* reduced length */
	l = MCLBYTES - l;

	m->m_len   = m->m_pkthdr.len = l;
	*psz       = m->m_len;
	*paddr     = mtod(m, unsigned long);

	return (void*) m;
}

static void consume_rx_mbuf(void *buf, void *arg, int len)
{
struct ifnet *ifp = arg;
struct mbuf    *m = buf;

	if ( len <= 0 ) {
		ifp->if_iqdrops++;
		if ( len < 0 ) {
			ifp->if_ierrors++;
		}
		if ( m )
			m_freem(m);
	} else {
		struct ether_header *eh;

			eh			= (struct ether_header *)(mtod(m, unsigned long) + ETH_RX_OFFSET);
			m->m_len	= m->m_pkthdr.len = len - sizeof(struct ether_header) - ETH_RX_OFFSET - ETH_CRC_LEN;
			m->m_data  += sizeof(struct ether_header) + ETH_RX_OFFSET;
			m->m_pkthdr.rcvif = ifp;

			ifp->if_ipackets++;
			ifp->if_ibytes  += m->m_pkthdr.len;

			/* send buffer upwards */
			if (0) {
				/* Low-level debugging */
				int i;
				for (i=0; i<13; i++) {
					printf("%02X:",((char*)eh)[i]);
				}
				printf("%02X\n",((char*)eh)[i]);
				for (i=0; i<m->m_len; i++) {
					if ( !(i&15) )
						printf("\n");
					printf("0x%02x ",mtod(m,char*)[i]);
				}
				printf("\n");
			}

			if (0) /* Low-level debugging/testing without bsd stack */
				m_freem(m);
			else
				ether_input(ifp, eh, m);
	}
}

static void release_tx_mbuf(void *buf, void *arg, int err)
{
struct ifnet *ifp = arg;
struct mbuf  *mb  = buf;

	if ( err ) {
		ifp->if_oerrors++;
	} else {
		ifp->if_opackets++;
	}
	ifp->if_obytes += mb->m_pkthdr.len;
	m_freem(mb);
}

/* BSDNET DRIVER CALLBACKS */

static void
tsec_init(void *arg)
{
struct tsec_softc	*sc  = arg;
struct ifnet		*ifp = &sc->arpcom.ac_if;
int                 media;

	BSP_tsec_init_hw(&sc->pvt, ifp->if_flags & IFF_PROMISC, sc->arpcom.ac_enaddr);

	/* Determine initial link status and block sender if there is no link */
	media = IFM_MAKEWORD(0, 0, 0, 0);
	if ( 0 == BSP_tsec_media_ioctl(&sc->pvt, SIOCGIFMEDIA, &media) ) {
		if ( (IFM_LINK_OK & media) ) {
			ifp->if_flags &= ~IFF_OACTIVE;
		} else {
			ifp->if_flags |=  IFF_OACTIVE;
		}
	}

	tsec_update_mcast(ifp);
	ifp->if_flags |= IFF_RUNNING;
	sc->arpcom.ac_if.if_timer = 0;
}

/* bsdnet driver entry to start transmission */
static void
tsec_start(struct ifnet *ifp)
{
struct tsec_softc	*sc = ifp->if_softc;
struct mbuf			*m  = 0;

	while ( ifp->if_snd.ifq_head ) {
		IF_DEQUEUE( &ifp->if_snd, m );
		if ( BSP_tsec_send_buf(&sc->pvt, m, 0, 0) < 0 ) {
			IF_PREPEND( &ifp->if_snd, m);
			ifp->if_flags |= IFF_OACTIVE;
			break;
		}
		/* need to do this really only once
		 * but it's cheaper this way.
		 */
		ifp->if_timer = 2*IFNET_SLOWHZ;
	}
}

/* bsdnet driver entry; */
static void
tsec_watchdog(struct ifnet *ifp)
{
struct tsec_softc	*sc = ifp->if_softc;

	ifp->if_oerrors++;
	printk(DRVNAME"%i: watchdog timeout; resetting\n", ifp->if_unit);

	tsec_init(sc);
	tsec_start(ifp);
}

static void
tsec_update_mcast(struct ifnet *ifp)
{
struct tsec_softc *sc = ifp->if_softc;
struct ether_multi     *enm;
struct ether_multistep step;

	if ( IFF_ALLMULTI & ifp->if_flags ) {
		BSP_tsec_mcast_filter_accept_all( &sc->pvt );
	} else {
		BSP_tsec_mcast_filter_clear( &sc->pvt );

		ETHER_FIRST_MULTI(step, (struct arpcom *)ifp, enm);

		while ( enm ) {
			if ( memcmp(enm->enm_addrlo, enm->enm_addrhi, ETHER_ADDR_LEN) )
				assert( !"Should never get here; IFF_ALLMULTI should be set!" );

			BSP_tsec_mcast_filter_accept_add(&sc->pvt, enm->enm_addrlo);

			ETHER_NEXT_MULTI(step, enm);
		}
	}
}

/* bsdnet driver ioctl entry */
static int
tsec_ioctl(struct ifnet *ifp, ioctl_command_t cmd, caddr_t data)
{
struct tsec_softc	*sc   = ifp->if_softc;
struct ifreq		*ifr  = (struct ifreq *)data;
#if 0
uint32_t			v;
#endif
int					error = 0;
int					f;

	switch ( cmd ) {
  		case SIOCSIFFLAGS:
			f = ifp->if_flags;
			if ( f & IFF_UP ) {
				if ( ! ( f & IFF_RUNNING ) ) {
					tsec_init(sc);
				} else {
					if ( (f & IFF_PROMISC) != (sc->bsd.oif_flags & IFF_PROMISC) ) {
						/* Hmm - the manual says we must change the RCTRL
						 * register only after a reset or if DMACTRL[GRS]
						 * is cleared which is the normal operating
						 * condition. I hope this is legal ??
						 */
						if ( (f & IFF_PROMISC) ) {
							fec_set( sc->pvt.base, TSEC_RCTRL, TSEC_RCTRL_PROM );
						} else {
							fec_clr( sc->pvt.base, TSEC_RCTRL, TSEC_RCTRL_PROM );
						}
					}
					/* FIXME: other flag changes are ignored/unimplemented */
				}
			} else {
				if ( f & IFF_RUNNING ) {
					tsec_stop(sc);
					ifp->if_flags  &= ~(IFF_RUNNING | IFF_OACTIVE);
				}
			}
			sc->bsd.oif_flags = ifp->if_flags;
		break;

  		case SIOCGIFMEDIA:
  		case SIOCSIFMEDIA:
			error = BSP_tsec_media_ioctl(&sc->pvt, cmd, &ifr->ifr_media);
		break;

 		case SIOCADDMULTI:
 		case SIOCDELMULTI:
			error = (cmd == SIOCADDMULTI)
		    		? ether_addmulti(ifr, &sc->arpcom)
				    : ether_delmulti(ifr, &sc->arpcom);

			if (error == ENETRESET) {
				if (ifp->if_flags & IFF_RUNNING) {
					tsec_update_mcast(ifp);
				}
				error = 0;
			}
		break;

 		case SIO_RTEMS_SHOW_STATS:
			BSP_tsec_dump_stats( &sc->pvt, stdout );
		break;

		default:
			error = ether_ioctl(ifp, cmd, data);
		break;
	}

	return error;
}

/* DRIVER TASK */

/* Daemon task does all the 'interrupt' work */
static void tsec_daemon(void *arg)
{
struct tsec_softc	*sc;
struct ifnet		*ifp;
rtems_event_set		evs;
	for (;;) {
		rtems_bsdnet_event_receive( EV_MSK, RTEMS_WAIT | RTEMS_EVENT_ANY, RTEMS_NO_TIMEOUT, &evs );
		evs &= EV_MSK;
		for ( sc = theTsecEths; evs; evs>>=EV_PER_UNIT, sc++ ) {
			if ( EV_IS_ANY(evs) ) {

				register uint32_t x;

				ifp = &sc->arpcom.ac_if;

				if ( !(ifp->if_flags & IFF_UP) ) {
					tsec_stop(sc);
					ifp->if_flags &= ~(IFF_UP|IFF_RUNNING);
					continue;
				}

				if ( !(ifp->if_flags & IFF_RUNNING) ) {
					/* event could have been pending at the time hw was stopped;
					 * just ignore...
					 */
					continue;
				}

				x = BSP_tsec_ack_irqs(&sc->pvt);

				if ( TSEC_LINK_INTR & x ) {
					/* phy status changed */
					int media;

#ifdef DEBUG
					printf("LINK INTR\n");
#endif
					if ( -1 != (media = mac_set_duplex( &sc->pvt )) ) {
#ifdef DEBUG
						rtems_ifmedia2str( media, 0, 0 );
						printf("\n");
#endif
						if ( IFM_LINK_OK & media ) {
							ifp->if_flags &= ~IFF_OACTIVE;
							tsec_start(ifp);
						} else {
							/* stop sending */
							ifp->if_flags |= IFF_OACTIVE;
						}
					}
				}

				/* free tx chain */
				if ( (TSEC_TXIRQ & x) && BSP_tsec_swipe_tx(&sc->pvt) ) {
					ifp->if_flags &= ~IFF_OACTIVE;
					if ( TX_AVAILABLE_RING_SIZE(&sc->pvt) == sc->pvt.tx_avail )
						ifp->if_timer = 0;
					tsec_start(ifp);
				}
				if ( (TSEC_RXIRQ & x) )
					BSP_tsec_swipe_rx(&sc->pvt);

				BSP_tsec_enable_irq_mask(&sc->pvt, -1);
			}
		}
	}
}

/* PUBLIC RTEMS BSDNET ATTACH FUNCTION */
int
rtems_tsec_attach(struct rtems_bsdnet_ifconfig *ifcfg, int attaching)
{
char				*unitName;
int					unit,i,cfgUnits;
struct	tsec_softc *sc;
struct	ifnet		*ifp;

	unit = rtems_bsdnet_parse_driver_name(ifcfg, &unitName);
	if ( unit <= 0 || unit > TSEC_NUM_DRIVER_SLOTS ) {
		printk(DRVNAME": Bad unit number %i; must be 1..%i\n", unit, TSEC_NUM_DRIVER_SLOTS);
		return 1;
	}

	sc         = &theTsecEths[unit-1];
	ifp        = &sc->arpcom.ac_if;

	if ( attaching ) {
		if ( ifp->if_init ) {
			printk(DRVNAME": instance %i already attached.\n", unit);
			return -1;
		}

		for ( i=cfgUnits = 0; i<TSEC_NUM_DRIVER_SLOTS; i++ ) {
			if ( theTsecEths[i].arpcom.ac_if.if_init )
				cfgUnits++;
		}
		cfgUnits++; /* this new one */

		/* lazy init of TID should still be thread-safe because we are protected
		 * by the global networking semaphore..
		 */
		if ( !tsec_tid ) {
			/* newproc uses the 1st 4 chars of name string to build an rtems name */
			tsec_tid = rtems_bsdnet_newproc("FECd", 4096, tsec_daemon, 0);
		}

		if ( !BSP_tsec_setup( unit,
						     tsec_tid,
						     release_tx_mbuf, ifp,
						     alloc_mbuf_rx,
						     consume_rx_mbuf, ifp,
						     ifcfg->rbuf_count,
						     ifcfg->xbuf_count,
			                 TSEC_RXIRQ | TSEC_TXIRQ | TSEC_LINK_INTR) ) {
			return -1;
		}

		if ( nmbclusters < sc->pvt.rx_ring_size * cfgUnits + 60 /* arbitrary */ )  {
			printk(DRVNAME"%i: (tsec ethernet) Your application has not enough mbuf clusters\n", unit);
			printk(     "                      configured for this driver.\n");
			return -1;
		}

		if ( ifcfg->hardware_address ) {
			memcpy(sc->arpcom.ac_enaddr, ifcfg->hardware_address, ETHER_ADDR_LEN);
		} else {
			/* read back from hardware assuming that MotLoad already had set it up */
			BSP_tsec_read_eaddr(&sc->pvt, sc->arpcom.ac_enaddr);
		}

		ifp->if_softc			= sc;
		ifp->if_unit			= unit;
		ifp->if_name			= unitName;

		ifp->if_mtu				= ifcfg->mtu ? ifcfg->mtu : ETHERMTU;

		ifp->if_init			= tsec_init;
		ifp->if_ioctl			= tsec_ioctl;
		ifp->if_start			= tsec_start;
		ifp->if_output			= ether_output;
		/*
		 * While nonzero, the 'if->if_timer' is decremented
		 * (by the networking code) at a rate of IFNET_SLOWHZ (1hz) and 'if_watchdog'
		 * is called when it expires.
		 * If either of those fields is 0 the feature is disabled.
		 */
		ifp->if_watchdog		= tsec_watchdog;
		ifp->if_timer			= 0;

		sc->bsd.oif_flags		= /* ... */
		ifp->if_flags			= IFF_BROADCAST | IFF_MULTICAST | IFF_SIMPLEX;

		/*
		 * if unset, this set to 10Mbps by ether_ifattach; seems to be unused by bsdnet stack;
		 * could be updated along with phy speed, though...
		ifp->if_baudrate		= 10000000;
		*/

		/* NOTE: ether_output drops packets if ifq_len >= ifq_maxlen
		 *       but this is the packet count, not the fragment count!
		ifp->if_snd.ifq_maxlen	= sc->pvt.tx_ring_size;
		*/
		ifp->if_snd.ifq_maxlen	= ifqmaxlen;

#ifdef  TSEC_DETACH_HACK
		if ( !ifp->if_addrlist ) /* do only the first time [reattach hack] */
#endif
		{
			if_attach(ifp);
			ether_ifattach(ifp);
		}

	} else {
#ifdef  TSEC_DETACH_HACK
		if ( !ifp->if_init ) {
			printk(DRVNAME": instance %i not attached.\n", unit);
			return -1;
		}
		return tsec_detach(sc);
#else
		printk(DRVNAME": interface detaching not implemented\n");
		return -1;
#endif
	}

	return 0;
}

/* PHY stuff should really not be in this driver but separate :-(
 * However, I don't have time right now to implement clean
 * boundaries:
 *  - PHY driver should only know about the PHY
 *  - TSEC driver only provides MII access and knows
 *    how to deal with a PHY interrupt.
 *  - BSP knows how interrupts are routed. E.g., the MVME3100
 *    shares a single IRQ line among 3 PHYs (for the three ports)
 *    and provides a special 'on-board' register for determining
 *    what PHY raised an interrupt w/o the need to do any MII IO.
 * Integrating all these bits in a clean way is not as easy as
 * just hacking away, sorry...
 */

/*
 * Broadcom 54xx PHY register definitions. Unfriendly Broadcom doesn't
 * release specs for their products :-( -- valuable info comes from
 * the linux driver by
 *    Maciej W. Rozycki <macro@linux-mips.org>
 *    Amy Fong
 */

#define BCM54xx_GBCR	0x09		/* gigabit control    */
#define BCM54xx_GBCR_FD		(1<<9)	/* full-duplex cap.   */

#define BCM54xx_ECR		0x10		/* extended control   */
#define BCM54xx_ECR_IM		(1<<12)	/* IRQ mask           */
#define BCM54xx_ECR_IF		(1<<12)	/* IRQ force          */

#define BCM54xx_ESR		0x11		/* extended status    */
#define BCM54xx_ESR_IRQ	    (1<<12) /* IRQ pending        */

#define BCM54xx_AUXCR	0x18		/* AUX control        */
#define BCM54xx_AUXCR_PWR10BASET	(1<<2)

#define BCM54xx_AUXST	0x19		/* AUX status         */
#define BCM54xx_AUXST_LNKMM	(7<<8)	/* link mode mask     */

/* link mode (linux' syngem_phy.c helped here...)
 *
 *  0: no link
 *  1: 10BT    half
 *  2: 10BT    full
 *  3: 100BT   half
 *  4: 100BT   half
 *  5: 100BT   full
 *  6: 1000BT  full
 *  7: 1000BT  full
 */

#define BCM54xx_ISR		0x1a		/* IRQ status         */
#define BCM54xx_IMR		0x1b		/* IRQ mask           */
#define BCM54xx_IRQ_CRC		(1<< 0)	/* CRC error          */
#define BCM54xx_IRQ_LNK		(1<< 1) /* LINK status chg.   */
#define BCM54xx_IRQ_SPD		(1<< 2) /* SPEED change       */
#define BCM54xx_IRQ_DUP		(1<< 3) /* LINK status chg.   */
#define BCM54xx_IRQ_LRS		(1<< 4) /* Lcl. RX status chg.*/
#define BCM54xx_IRQ_RRS		(1<< 5) /* Rem. RX status chg.*/
#define BCM54xx_IRQ_SSE		(1<< 6) /* Scrambler sync err */
#define BCM54xx_IRQ_UHCD	(1<< 7) /* Unsupp. HCD neg.   */
#define BCM54xx_IRQ_NHCD	(1<< 8) /* No HCD             */
#define BCM54xx_IRQ_HCDL	(1<< 9) /* No HCD Link        */
#define BCM54xx_IRQ_ANPR	(1<<10) /* Aneg. pg. req.     */
#define BCM54xx_IRQ_LC		(1<<11) /* All ctrs. < 128    */
#define BCM54xx_IRQ_HC		(1<<12) /* Ctr > 32768        */
#define BCM54xx_IRQ_MDIX	(1<<13) /* MDIX status chg.   */
#define BCM54xx_IRQ_PSERR	(1<<14) /* Pair swap error    */

#define PHY_IRQS ( BCM54xx_IRQ_LNK | BCM54xx_IRQ_SPD | BCM54xx_IRQ_DUP )


static void
phy_en_irq_at_phy( struct tsec_private *mp )
{
uint32_t ecr;

	REGLOCK();
	tsec_mdio_rd( 0, mp, BCM54xx_ECR, &ecr );
	ecr &= ~BCM54xx_ECR_IM;
	tsec_mdio_wr( 0, mp, BCM54xx_ECR,  ecr );
	REGUNLOCK();
}

static void
phy_dis_irq_at_phy( struct tsec_private *mp )
{
uint32_t ecr;

	REGLOCK();
	tsec_mdio_rd( 0, mp, BCM54xx_ECR, &ecr );
	ecr |=  BCM54xx_ECR_IM;
	tsec_mdio_wr( 0, mp, BCM54xx_ECR,  ecr );
	REGUNLOCK();
}

static void
phy_init_irq( int install, struct tsec_private *mp, void (*isr)(rtems_irq_hdl_param) )
{
uint32_t v;
rtems_irq_connect_data xxx;

	xxx.on     = noop;
	xxx.off    = noop;
	xxx.isOn   = nopf;
	xxx.name   = BSP_PHY_IRQ;
	xxx.handle = mp;
	xxx.hdl    = isr;

	phy_dis_irq_at_phy( mp );

	REGLOCK();
	/* Select IRQs we want */
	tsec_mdio_wr( 0, mp, BCM54xx_IMR, ~ PHY_IRQS );
	/* clear pending irqs */
	tsec_mdio_rd( 0, mp, BCM54xx_ISR, &v );
	REGUNLOCK();

	/* install shared ISR */
	if ( ! (install ?
			BSP_install_rtems_shared_irq_handler( &xxx ) :
			BSP_remove_rtems_irq_handler( &xxx )) ) {
		rtems_panic( "Unable to %s shared irq handler (PHY)\n", install ? "install" : "remove" );
	}
}

/* Because on the MVME3100 multiple PHYs (belonging to different
 * TSEC instances) share a single interrupt line and we want
 * to disable interrupts at the PIC rather than in the individual
 * PHYs (because access to those is slow) we must implement
 * nesting...
 */
STATIC int phy_irq_dis_level = 0;

/* assume 'en_irq' / 'dis_irq' cannot be interrupted.
 * Either because they are called from an ISR (all
 * tsec + phy isrs must have the same priority) or
 * from a IRQ-protected section of code
 */

static void
phy_en_irq(struct tsec_private *mp)
{
	phy_irq_dis_level &= ~(1<<mp->unit);
	if ( 0 == phy_irq_dis_level ) {
		BSP_enable_irq_at_pic( BSP_PHY_IRQ );
	}
}


static void
phy_dis_irq(struct tsec_private *mp)
{
	phy_irq_dis_level |= (1<<mp->unit);
	BSP_disable_irq_at_pic( BSP_PHY_IRQ );
}

static int
phy_irq_pending(struct tsec_private *mp)
{
	/* MVME3100 speciality: we can check for a pending
	 * PHY IRQ w/o having to access the MII bus :-)
	 */
	return in_8( BSP_MVME3100_IRQ_DETECT_REG ) & (1 << (mp->unit - 1));
}

static uint32_t
phy_ack_irq(struct tsec_private *mp)
{
uint32_t v;

	REGLOCK();
	tsec_mdio_rd( 0, mp, BCM54xx_ISR, &v );
	REGUNLOCK();

#ifdef DEBUG
	printf("phy_ack_irq: 0x%08"PRIx32"\n", v);
#endif

	return v;
}

#if defined(PARANOIA) || defined(DEBUG)

static void dumpbd(TSEC_BD *bd)
{
		printf("Flags 0x%04"PRIx16", len 0x%04"PRIx16", buf 0x%08"PRIx32"\n",
			bd_rdfl( bd ), ld_be16( &bd->len ), bd_rdbuf( bd ) );
}

void tsec_dump_rring(struct tsec_private *mp)
{
int i;
TSEC_BD *bd;
	if ( !mp ) {
		printf("Neet tsec_private * arg\n");
		return;
	}
	for ( i=0; i<mp->rx_ring_size; i++ ) {
		bd = &mp->rx_ring[i];
		printf("[%i]: ", i);
		dumpbd(bd);
	}
}

void tsec_dump_tring(struct tsec_private *mp)
{
int i;
TSEC_BD *bd;
	if ( !mp ) {
		printf("Neet tsec_private * arg\n");
		return;
	}
	for ( i=0; i<mp->tx_ring_size; i++ ) {
		bd = &mp->tx_ring[i];
		printf("[%i]: ", i);
		dumpbd(bd);
	}
	printf("Avail: %i; Head %i; Tail %i\n", mp->tx_avail, mp->tx_head, mp->tx_tail);
}
#endif


#ifdef DEBUG

#undef free
#undef malloc

#include <stdlib.h>

void cleanup_txbuf_test(void *u, void *a, int err)
{
	printf("cleanup_txbuf_test (releasing buf 0x%8p)\n", u);
	free(u);
	if ( err )
		printf("cleanup_txbuf_test: an error was detected\n");
}

void *alloc_rxbuf_test(int *p_size, uintptr_t *p_data_addr)
{
void *rval;

	*p_size = 1600;
	rval    = malloc( *p_size + RX_BUF_ALIGNMENT );
	*p_data_addr = (uintptr_t)ALIGNTO(rval,RX_BUF_ALIGNMENT);

	/* PRIxPTR is still broken :-( */
	printf("alloc_rxxbuf_test: allocated %i bytes @0x%8p/0x%08"PRIx32"\n",
		*p_size, rval, (uint32_t)*p_data_addr);

	return rval;
}

void consume_rxbuf_test(void *user_buf, void *consume_rxbuf_arg, int len)
{
int i;
uintptr_t d = (uintptr_t)ALIGNTO(user_buf,RX_BUF_ALIGNMENT);

	/* PRIxPTR is still broken */
	printf("consuming rx buf 0x%8p (data@ 0x%08"PRIx32")\n",user_buf, (uint32_t)d);
	if ( len > 32 )
	   len = 32;
	if ( len < 0 )
		printf("consume_rxbuf_test: ERROR occurred: 0x%x\n", len);
	else {
		printf("RX:");
		for ( i=0; i<len; i++ ) {
			if ( 0 == (i&0xf) )
				printf("\n  ");
			printf("0x%02x ", ((char*)d)[i]);
		}
		printf("\n");
		free(user_buf);
	}
}

unsigned char pkt[100];

void * tsec_up()
{
struct tsec_private *tsec =
	BSP_tsec_setup( 1, 0,
		cleanup_txbuf_test, 0,
		alloc_rxbuf_test,
		consume_rxbuf_test, 0,
		 2,
		 2,
		 0);
	BSP_tsec_init_hw(tsec, 0, 0);
	memset(pkt,0,100);
	memset(pkt,0xff,6);
	BSP_tsec_read_eaddr(tsec, pkt+6);
	pkt[12] = 0;
	pkt[13] = 64;
	return tsec;
}

#ifdef DEBUG_MODULAR
int
_cexpModuleInitialize(void*u)
{
extern int ifattach();
extern int ifconf();
extern int rtconf();
	ifattach("ts1",rtems_tsec_attach,0);
	ifconf("ts1","134.79.33.137","255.255.252.0");
	ifconf("pcn1",0,0);
	rtconf(0, "134.79.33.86",0,0);
	return 0;
}
#endif
#endif
