/* RTEMS driver for the mv643xx gigabit ethernet chip */

/* Acknowledgement:
 *
 * Valuable information for developing this driver was obtained
 * from the linux open-source driver mv643xx_eth.c which was written
 * by the following people and organizations:
 *
 * Matthew Dharm <mdharm@momenco.com>
 * rabeeh@galileo.co.il
 * PMC-Sierra, Inc., Manish Lachwani
 * Ralf Baechle <ralf@linux-mips.org>
 * MontaVista Software, Inc., Dale Farnsworth <dale@farnsworth.org>
 * Steven J. Hill <sjhill1@rockwellcollins.com>/<sjhill@realitydiluted.com>
 *
 * Note however, that in spite of the identical name of this file
 * (and some of the symbols used herein) this file provides a
 * new implementation and is the original work by the author.
 */

/*
 * Authorship
 * ----------
 * This software (mv643xx ethernet driver for RTEMS) was
 *     created by Till Straumann <strauman@slac.stanford.edu>, 2005-2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * The 'mv643xx ethernet driver for RTEMS' was produced by
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

/*
 * NOTE: Some register (e.g., the SMI register) are SHARED among the
 *       three devices. Concurrent access protection is provided by
 *       the global networking semaphore.
 *       If other drivers are running on a subset of IFs then proper
 *       locking of all shared registers must be implemented!
 *
 *       Some things I learned about this hardware can be found
 *       further down...
 */

/*
#ifndef KERNEL
#define KERNEL
#endif
#ifndef _KERNEL
#define _KERNEL
#endif
*/

#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/error.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/gtreg.h>
#include <libcpu/byteorder.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>

#include <bsp/mv643xx_eth.h>

/* CONFIGURABLE PARAMETERS */

/* Enable Hardware Snooping; if this is disabled (undefined),
 * cache coherency is maintained by software.
 */
#undef  ENABLE_HW_SNOOPING

/* Compile-time debugging features */

/* Enable paranoia assertions and checks; reduce # of descriptors to minimum for stressing   */
#define MVETH_TESTING

/* Enable debugging messages and some support routines  (dump rings etc.)                    */
#undef  MVETH_DEBUG

#define TX_NUM_TAG_SLOTS			1 /* leave room for tag; must not be 0 */

/* This is REAL; chip reads from 64-bit down-aligned buffer
 * if the buffer size is < 8 !!! for buffer sizes 8 and upwards
 * alignment is not an issue. This was verified using the
 * 'mve_smallbuf_test.c'
 */
#define ENABLE_TX_WORKAROUND_8_BYTE_PROBLEM

/* Chip register configuration values */
#define	MVETH_PORT_CONFIG_VAL			(0				\
			| MV643XX_ETH_DFLT_RX_Q(0)					\
			| MV643XX_ETH_DFLT_RX_ARP_Q(0)				\
			| MV643XX_ETH_DFLT_RX_TCP_Q(0)				\
			| MV643XX_ETH_DFLT_RX_UDP_Q(0)				\
			| MV643XX_ETH_DFLT_RX_BPDU_Q(0)				\
			)


#define	MVETH_PORT_XTEND_CONFIG_VAL		0

#ifdef OLDCONFIGVAL
#define	MVETH_SERIAL_CTRL_CONFIG_VAL	(0				\
			 | MV643XX_ETH_FORCE_LINK_PASS 				\
			 | MV643XX_ETH_DISABLE_AUTO_NEG_FOR_FLOWCTL	\
			 | MV643XX_ETH_ADVERTISE_SYMMETRIC_FLOWCTL	\
			 | MV643XX_ETH_BIT9_UNKNOWN					\
			 | MV643XX_ETH_FORCE_LINK_FAIL_DISABLE		\
			 | MV643XX_ETH_SC_MAX_RX_1552				\
			 | MV643XX_ETH_SET_FULL_DUPLEX				\
			 | MV643XX_ETH_ENBL_FLOWCTL_TX_RX_IN_FD		\
			 )
#endif
/* If we enable autoneg (duplex, speed, ...) then it seems
 * that the chip automatically updates link settings
 * (correct link settings are reflected in PORT_STATUS_R).
 * However, when we disable aneg in the PHY then things
 * can get messed up and the port doesn't work anymore.
 * => we follow the linux driver in disabling all aneg
 * in the serial config reg. and manually updating the
 * speed & duplex bits when the phy link status changes.
 * FIXME: don't know what to do about pause/flow-ctrl.
 * It is best to just use ANEG anyways!!!
 */
#define	MVETH_SERIAL_CTRL_CONFIG_VAL	(0				\
			 | MV643XX_ETH_DISABLE_AUTO_NEG_FOR_DUPLEX	\
			 | MV643XX_ETH_DISABLE_AUTO_NEG_FOR_FLOWCTL	\
			 | MV643XX_ETH_ADVERTISE_SYMMETRIC_FLOWCTL	\
			 | MV643XX_ETH_BIT9_UNKNOWN					\
			 | MV643XX_ETH_FORCE_LINK_FAIL_DISABLE		\
			 | MV643XX_ETH_DISABLE_AUTO_NEG_SPEED_GMII	\
			 | MV643XX_ETH_SC_MAX_RX_1552				\
			 )

#define	MVETH_SERIAL_CTRL_CONFIG_MSK	(0				\
			 | MV643XX_ETH_SERIAL_PORT_ENBL				\
			 | MV643XX_ETH_FORCE_LINK_PASS				\
			 | MV643XX_ETH_SC_MAX_RX_MASK				\
			 )


#ifdef __PPC__
#define MVETH_SDMA_CONFIG_VAL			(0				\
			| MV643XX_ETH_RX_BURST_SZ_4_64BIT			\
			| MV643XX_ETH_TX_BURST_SZ_4_64BIT			\
			)
#else
#define MVETH_SDMA_CONFIG_VAL			(0				\
			| MV643XX_ETH_RX_BURST_SZ_16_64BIT			\
			| MV643XX_ETH_TX_BURST_SZ_16_64BIT			\
			)
#endif

/* minimal frame size we accept */
#define MVETH_MIN_FRAMSZ_CONFIG_VAL	40

/* END OF CONFIGURABLE SECTION */

/*
 * Here's stuff I learned about this chip:
 *
 *
 * RX interrupt flags:
 *
 * broadcast packet RX: 0x00000005
 *           last buf:  0x00000c05
 *           overrun:   0x00000c00
 * unicast   packet RX: 0x00000005
 * bad CRC received:    0x00000005
 *
 * clearing 0x00000004 -> clears 0x00000001
 * clearing 0x00000400 -> clears 0x00000800
 *
 * --> 0x0801 are probably some sort of summary bits.
 *
 * TX interrupt flags:
 *
 * broadcast packet in 1 buf: xcause: 0x00000001 (cause 0x00080000)
 *        into disconn. link:             "                 "
 *
 * in some cases, I observed  xcause: 0x00000101 (reason for 0x100 unknown
 * but the linux driver accepts it also).
 *
 *
 * Here a few more ugly things about this piece of hardware I learned
 * (painfully, painfully; spending many many hours & nights :-()
 *
 * a) Especially in the case of 'chained' descriptors, the DMA keeps
 *    clobbering 'cmd_sts' long after it cleared the OWNership flag!!!
 *    Only after the whole chain is processed (OWN cleared on the
 *    last descriptor) it is safe to change cmd_sts.
 *    However, in the case of hardware snooping I found that the
 *    last descriptor in chain has its cmd_sts still clobbered *after*
 *    checking ownership!, I.e.,
 *        if ( ! OWN & cmd_sts ) {
 *            cmd_sts = 0;
 *        }
 *    --> sometimes, cmd_sts is STILL != 0 here
 *
 * b) Sometimes, the OWNership flag is *not cleared*.
 *
 * c) Weird things happen if the chip finds a descriptor with 'OWN'
 *    still set (i.e., not properly loaded), i.e., corrupted packets
 *    are sent [with OK checksum since the chip calculates it].
 *
 * Combine a+b+c and we end up with a real mess.
 *
 * The fact that the chip doesn't reliably reset OWN and that OTOH,
 * it can't be reliably reset by the driver and still, the chip needs
 * it for proper communication doesn't make things easy...
 *
 * Here the basic workarounds:
 *
 *     - In addition to check OWN, the scavenger compares the "currently
 *       served desc" register to the descriptor it tries to recover and
 *       ignores OWN if they do not match. Hope this is OK.
 *       Otherwise, we could scan the list of used descriptors and proceed
 *       recycling descriptors if we find a !OWNed one behind the target...
 *
 *     - Always keep an empty slot around to mark the end of the list of
 *       jobs. The driver clears the descriptor ahead when enqueueing a new
 *       packet.
 */

#define DRVNAME			"mve"
#define MAX_NUM_SLOTS	3

#if MV643XXETH_NUM_DRIVER_SLOTS > MAX_NUM_SLOTS
#error "mv643xxeth: only MAX_NUM_SLOTS supported"
#endif

#ifdef NDEBUG
#error "Driver uses assert() statements with side-effects; MUST NOT define NDEBUG"
#endif

#ifdef MVETH_DEBUG
#define STATIC
#else
#define STATIC static
#endif

#define TX_AVAILABLE_RING_SIZE(mp)		((mp)->xbuf_count - (TX_NUM_TAG_SLOTS))

/* macros for ring alignment; proper alignment is a hardware req; . */

#ifdef ENABLE_HW_SNOOPING

#define RING_ALIGNMENT				16
/* rx buffers must be 64-bit aligned (chip requirement) */
#define RX_BUF_ALIGNMENT			8

#else /* ENABLE_HW_SNOOPING */

/* Software cache management */

#ifndef __PPC__
#error	"Dont' know how to deal with cache on this CPU architecture"
#endif

/* Ring entries are 32 bytes; coherency-critical chunks are 16 -> software coherency
 * management works for cache line sizes of 16 and 32 bytes only. If the line size
 * is bigger, the descriptors could be padded...
 */
#if	PPC_CACHE_ALIGMENT != 16 && PPC_CACHE_ALIGNMENT != 32
#error "Cache line size must be 16 or 32"
#else
#define RING_ALIGNMENT				PPC_CACHE_ALIGNMENT
#define RX_BUF_ALIGNMENT			PPC_CACHE_ALIGNMENT
#endif

#endif /* ENABLE_HW_SNOOPING */


/* HELPER MACROS */

/* Align base to alignment 'a' */
#define MV643XX_ALIGN(b, a)	((((uint32_t)(b)) + (a)-1) & (~((a)-1)))

#define NOOP()			do {} while(0)

/* Function like macros */
#define MV_READ(off) \
		ld_le32((volatile uint32_t *)(BSP_MV64x60_BASE + (off)))
#define MV_WRITE(off, data)		\
		st_le32((volatile uint32_t *)(BSP_MV64x60_BASE + (off)), ((unsigned)data))


/* ENET window mapped 1:1 to CPU addresses by our BSP/MotLoad
 * -- if this is changed, we should think about caching the 'next' and 'buf' pointers.
 */
#define CPUADDR2ENET(a) ((Dma_addr_t)(a))
#define ENET2CPUADDR(a) (a)

#if 1	/* Whether to automatically try to reclaim descriptors when enqueueing new packets */
#define MVETH_CLEAN_ON_SEND(mp) (BSP_mve_swipe_tx(mp))
#else
#define MVETH_CLEAN_ON_SEND(mp) (-1)
#endif

#define NEXT_TXD(d)	(d)->next
#define NEXT_RXD(d)	(d)->next

/* REGISTER AND DESCRIPTOR OFFSET AND BIT DEFINITIONS */

/* Descriptor Definitions */
/* Rx descriptor */
#define RDESC_ERROR									(1<< 0)	/* Error summary    */

/* Error code (bit 1&2) is only valid if summary bit is set */
#define RDESC_CRC_ERROR								(    1)
#define RDESC_OVERRUN_ERROR							(    3)
#define RDESC_MAX_FRAMELENGTH_ERROR					(    5)
#define RDESC_RESOURCE_ERROR						(    7)

#define RDESC_LAST									(1<<26)	/* Last Descriptor   */
#define RDESC_FRST									(1<<27)	/* First Descriptor  */
#define RDESC_INT_ENA								(1<<29) /* Enable Interrupts */
#define RDESC_DMA_OWNED								(1<<31)

/* Tx descriptor */
#define TDESC_ERROR									(1<< 0) /* Error summary     */
#define TDESC_ZERO_PAD								(1<<19)
#define TDESC_LAST									(1<<20)	/* Last Descriptor   */
#define TDESC_FRST									(1<<21)	/* First Descriptor  */
#define TDESC_GEN_CRC								(1<<22)
#define TDESC_INT_ENA								(1<<23) /* Enable Interrupts */
#define TDESC_DMA_OWNED								(1<<31)



/* Register Definitions */
#define MV643XX_ETH_PHY_ADDR_R						(0x2000)
#define MV643XX_ETH_SMI_R							(0x2004)
#define MV643XX_ETH_SMI_BUSY						(1<<28)
#define MV643XX_ETH_SMI_VALID						(1<<27)
#define MV643XX_ETH_SMI_OP_WR						(0<<26)
#define MV643XX_ETH_SMI_OP_RD						(1<<26)

#define MV643XX_ETH_TRANSMIT_QUEUE_COMMAND_R(port)	(0x2448 + ((port)<<10))
#define MV643XX_ETH_TX_START(queue)					(0x0001<<(queue))
#define MV643XX_ETH_TX_STOP(queue)					(0x0100<<(queue))
#define MV643XX_ETH_TX_START_M(queues)				((queues)&0xff)
#define MV643XX_ETH_TX_STOP_M(queues)				(((queues)&0xff)<<8)
#define MV643XX_ETH_TX_STOP_ALL						(0xff00)
#define MV643XX_ETH_TX_ANY_RUNNING					(0x00ff)

#define MV643XX_ETH_RECEIVE_QUEUE_COMMAND_R(port)	(0x2680 + ((port)<<10))
#define MV643XX_ETH_RX_START(queue)					(0x0001<<(queue))
#define MV643XX_ETH_RX_STOP(queue)					(0x0100<<(queue))
#define MV643XX_ETH_RX_STOP_ALL						(0xff00)
#define MV643XX_ETH_RX_ANY_RUNNING					(0x00ff)

#define MV643XX_ETH_CURRENT_SERVED_TX_DESC(port)	(0x2684 + ((port)<<10))

/* The chip puts the ethernet header at offset 2 into the buffer so
 * that the payload is aligned
 */
#define ETH_RX_OFFSET								2
#define ETH_CRC_LEN									4	/* strip FCS at end of packet */


#define MV643XX_ETH_INTERRUPT_CAUSE_R(port)			(0x2460 + ((port)<<10))
/* not fully understood; RX seems to raise 0x0005 or 0x0c05 if last buffer is filled and 0x0c00
 * if there are no buffers
 */
#define MV643XX_ETH_ALL_IRQS						(0x07ffffff)
#define MV643XX_ETH_KNOWN_IRQS						(0x00080c07)
#define MV643XX_ETH_IRQ_EXT_ENA						(1<<1)
/* defined in public header
#define MV643XX_ETH_IRQ_RX_DONE						(1<<2)
 */
#define MV643XX_ETH_IRQ_RX_NO_DESC					(1<<10)
#define MV643XX_ETH_TX_Q_N_END(n)                   (1<<((n)+19))
/* We just use queue 0 */
#define MV643XX_ETH_TX_Q_END						MV643XX_ETH_TX_Q_N_END(0)

#define MV643XX_ETH_INTERRUPT_EXTEND_CAUSE_R(port)	(0x2464 + ((port)<<10))
/* not fully understood; TX seems to raise 0x0001 and link change is 0x00010000
 * if there are no buffers
 */
#define MV643XX_ETH_ALL_EXT_IRQS					(0x0011ffff)
/* Recent (2013) linux driver mentions both bits 0x00110000 as 'link change' causes */
#define MV643XX_ETH_KNOWN_EXT_IRQS					(0x00110101)
/* TX queues 0..7 */
#define MV643XX_ETH_EXT_IRQ_TXN_DONE(n)				(1<<(n))
/* We just use queue 0 */
/* defined in public header
#define MV643XX_ETH_EXT_IRQ_TX_DONE 				MV643XX_ETH_EXT_IRQ_TXN_DONE(0)
#define MV643XX_ETH_EXT_IRQ_LINK_CHG				(1<<16)
 */
#define MV643XX_ETH_INTERRUPT_ENBL_R(port)			(0x2468 + ((port)<<10))
#define MV643XX_ETH_INTERRUPT_EXTEND_ENBL_R(port)	(0x246c + ((port)<<10))

/* port configuration */
#define MV643XX_ETH_PORT_CONFIG_R(port)				(0x2400 + ((port)<<10))
#define	MV643XX_ETH_UNICAST_PROMISC_MODE			(1<<0)
#define	MV643XX_ETH_DFLT_RX_Q(q)					((q)<<1)
#define	MV643XX_ETH_DFLT_RX_ARP_Q(q)				((q)<<4)
#define MV643XX_ETH_REJ_BCAST_IF_NOT_IP_OR_ARP		(1<<7)
#define MV643XX_ETH_REJ_BCAST_IF_IP					(1<<8)
#define MV643XX_ETH_REJ_BCAST_IF_ARP				(1<<9)
#define MV643XX_ETH_TX_AM_NO_UPDATE_ERR_SUMMARY		(1<<12)
#define MV643XX_ETH_CAPTURE_TCP_FRAMES_ENBL			(1<<14)
#define MV643XX_ETH_CAPTURE_UDP_FRAMES_ENBL			(1<<15)
#define	MV643XX_ETH_DFLT_RX_TCP_Q(q)				((q)<<16)
#define	MV643XX_ETH_DFLT_RX_UDP_Q(q)				((q)<<19)
#define	MV643XX_ETH_DFLT_RX_BPDU_Q(q)				((q)<<22)



#define MV643XX_ETH_PORT_CONFIG_XTEND_R(port)		(0x2404 + ((port)<<10))
#define MV643XX_ETH_CLASSIFY_ENBL					(1<<0)
#define MV643XX_ETH_SPAN_BPDU_PACKETS_AS_NORMAL		(0<<1)
#define MV643XX_ETH_SPAN_BPDU_PACKETS_2_Q7			(1<<1)
#define MV643XX_ETH_PARTITION_DISBL					(0<<2)
#define MV643XX_ETH_PARTITION_ENBL					(1<<2)

#define MV643XX_ETH_SDMA_CONFIG_R(port)				(0x241c + ((port)<<10))
#define MV643XX_ETH_SDMA_RIFB						(1<<0)
#define MV643XX_ETH_RX_BURST_SZ_1_64BIT				(0<<1)
#define MV643XX_ETH_RX_BURST_SZ_2_64BIT				(1<<1)
#define MV643XX_ETH_RX_BURST_SZ_4_64BIT				(2<<1)
#define MV643XX_ETH_RX_BURST_SZ_8_64BIT				(3<<1)
#define MV643XX_ETH_RX_BURST_SZ_16_64BIT			(4<<1)
#define MV643XX_ETH_SMDA_BLM_RX_NO_SWAP				(1<<4)
#define MV643XX_ETH_SMDA_BLM_TX_NO_SWAP				(1<<5)
#define MV643XX_ETH_SMDA_DESC_BYTE_SWAP				(1<<6)
#define MV643XX_ETH_TX_BURST_SZ_1_64BIT				(0<<22)
#define MV643XX_ETH_TX_BURST_SZ_2_64BIT				(1<<22)
#define MV643XX_ETH_TX_BURST_SZ_4_64BIT				(2<<22)
#define MV643XX_ETH_TX_BURST_SZ_8_64BIT				(3<<22)
#define MV643XX_ETH_TX_BURST_SZ_16_64BIT			(4<<22)

#define	MV643XX_ETH_RX_MIN_FRAME_SIZE_R(port)		(0x247c + ((port)<<10))


#define MV643XX_ETH_SERIAL_CONTROL_R(port)			(0x243c + ((port)<<10))
#define MV643XX_ETH_SERIAL_PORT_ENBL				(1<<0)	/* Enable serial port */
#define MV643XX_ETH_FORCE_LINK_PASS					(1<<1)
#define MV643XX_ETH_DISABLE_AUTO_NEG_FOR_DUPLEX		(1<<2)
#define MV643XX_ETH_DISABLE_AUTO_NEG_FOR_FLOWCTL	(1<<3)
#define MV643XX_ETH_ADVERTISE_SYMMETRIC_FLOWCTL		(1<<4)
#define MV643XX_ETH_FORCE_FC_MODE_TX_PAUSE_DIS		(1<<5)
#define MV643XX_ETH_FORCE_BP_MODE_JAM_TX			(1<<7)
#define MV643XX_ETH_FORCE_BP_MODE_JAM_TX_ON_RX_ERR	(1<<8)
#define MV643XX_ETH_BIT9_UNKNOWN					(1<<9)	/* unknown purpose; linux sets this */
#define MV643XX_ETH_FORCE_LINK_FAIL_DISABLE			(1<<10)
#define MV643XX_ETH_RETRANSMIT_FOREVER				(1<<11) /* limit to 16 attempts if clear    */
#define MV643XX_ETH_DISABLE_AUTO_NEG_SPEED_GMII		(1<<13)
#define MV643XX_ETH_DTE_ADV_1						(1<<14)
#define MV643XX_ETH_AUTO_NEG_BYPASS_ENBL			(1<<15)
#define MV643XX_ETH_RESTART_AUTO_NEG				(1<<16)
#define MV643XX_ETH_SC_MAX_RX_1518					(0<<17)	/* Limit RX packet size */
#define MV643XX_ETH_SC_MAX_RX_1522					(1<<17)	/* Limit RX packet size */
#define MV643XX_ETH_SC_MAX_RX_1552					(2<<17)	/* Limit RX packet size */
#define MV643XX_ETH_SC_MAX_RX_9022					(3<<17)	/* Limit RX packet size */
#define MV643XX_ETH_SC_MAX_RX_9192					(4<<17)	/* Limit RX packet size */
#define MV643XX_ETH_SC_MAX_RX_9700					(5<<17)	/* Limit RX packet size */
#define MV643XX_ETH_SC_MAX_RX_MASK					(7<<17)	/* bitmask */
#define MV643XX_ETH_SET_EXT_LOOPBACK				(1<<20)
#define MV643XX_ETH_SET_FULL_DUPLEX					(1<<21)
#define MV643XX_ETH_ENBL_FLOWCTL_TX_RX_IN_FD		(1<<22)	/* enable flow ctrl on rx and tx in full-duplex */
#define MV643XX_ETH_SET_GMII_SPEED_1000				(1<<23)	/* 10/100 if clear */
#define MV643XX_ETH_SET_MII_SPEED_100				(1<<24)	/* 10 if clear     */

#define MV643XX_ETH_PORT_STATUS_R(port)				(0x2444 + ((port)<<10))

#define MV643XX_ETH_PORT_STATUS_MODE_10_BIT			(1<<0)
#define MV643XX_ETH_PORT_STATUS_LINK_UP				(1<<1)
#define MV643XX_ETH_PORT_STATUS_FDX					(1<<2)
#define MV643XX_ETH_PORT_STATUS_FC					(1<<3)
#define MV643XX_ETH_PORT_STATUS_1000				(1<<4)
#define MV643XX_ETH_PORT_STATUS_100					(1<<5)
/* PSR bit 6 unknown */
#define MV643XX_ETH_PORT_STATUS_TX_IN_PROGRESS		(1<<7)
#define MV643XX_ETH_PORT_STATUS_ANEG_BYPASSED		(1<<8)
#define MV643XX_ETH_PORT_STATUS_PARTITION			(1<<9)
#define MV643XX_ETH_PORT_STATUS_TX_FIFO_EMPTY		(1<<10)

#define MV643XX_ETH_MIB_COUNTERS(port)				(0x3000 + ((port)<<7))
#define MV643XX_ETH_NUM_MIB_COUNTERS				32

#define MV643XX_ETH_MIB_GOOD_OCTS_RCVD_LO			(0)
#define MV643XX_ETH_MIB_GOOD_OCTS_RCVD_HI			(1<<2)
#define MV643XX_ETH_MIB_BAD_OCTS_RCVD				(2<<2)
#define MV643XX_ETH_MIB_INTERNAL_MAC_TX_ERR			(3<<2)
#define MV643XX_ETH_MIB_GOOD_FRAMES_RCVD			(4<<2)
#define MV643XX_ETH_MIB_BAD_FRAMES_RCVD				(5<<2)
#define MV643XX_ETH_MIB_BCAST_FRAMES_RCVD			(6<<2)
#define MV643XX_ETH_MIB_MCAST_FRAMES_RCVD			(7<<2)
#define MV643XX_ETH_MIB_FRAMES_64_OCTS				(8<<2)
#define MV643XX_ETH_MIB_FRAMES_65_127_OCTS			(9<<2)
#define MV643XX_ETH_MIB_FRAMES_128_255_OCTS			(10<<2)
#define MV643XX_ETH_MIB_FRAMES_256_511_OCTS			(11<<2)
#define MV643XX_ETH_MIB_FRAMES_512_1023_OCTS		(12<<2)
#define MV643XX_ETH_MIB_FRAMES_1024_MAX_OCTS		(13<<2)
#define MV643XX_ETH_MIB_GOOD_OCTS_SENT_LO			(14<<2)
#define MV643XX_ETH_MIB_GOOD_OCTS_SENT_HI			(15<<2)
#define MV643XX_ETH_MIB_GOOD_FRAMES_SENT			(16<<2)
#define MV643XX_ETH_MIB_EXCESSIVE_COLL				(17<<2)
#define MV643XX_ETH_MIB_MCAST_FRAMES_SENT			(18<<2)
#define MV643XX_ETH_MIB_BCAST_FRAMES_SENT			(19<<2)
#define MV643XX_ETH_MIB_UNREC_MAC_CTRL_RCVD			(20<<2)
#define MV643XX_ETH_MIB_FC_SENT						(21<<2)
#define MV643XX_ETH_MIB_GOOD_FC_RCVD				(22<<2)
#define MV643XX_ETH_MIB_BAD_FC_RCVD					(23<<2)
#define MV643XX_ETH_MIB_UNDERSIZE_RCVD				(24<<2)
#define MV643XX_ETH_MIB_FRAGMENTS_RCVD				(25<<2)
#define MV643XX_ETH_MIB_OVERSIZE_RCVD				(26<<2)
#define MV643XX_ETH_MIB_JABBER_RCVD					(27<<2)
#define MV643XX_ETH_MIB_MAC_RX_ERR					(28<<2)
#define MV643XX_ETH_MIB_BAD_CRC_EVENT				(29<<2)
#define MV643XX_ETH_MIB_COLL						(30<<2)
#define MV643XX_ETH_MIB_LATE_COLL					(31<<2)

#define MV643XX_ETH_DA_FILTER_SPECL_MCAST_TBL(port) (0x3400+((port)<<10))
#define MV643XX_ETH_DA_FILTER_OTHER_MCAST_TBL(port) (0x3500+((port)<<10))
#define MV643XX_ETH_DA_FILTER_UNICAST_TBL(port)		(0x3600+((port)<<10))
#define MV643XX_ETH_NUM_MCAST_ENTRIES				64
#define MV643XX_ETH_NUM_UNICAST_ENTRIES				4

#define MV643XX_ETH_BAR_0							(0x2200)
#define MV643XX_ETH_SIZE_R_0						(0x2204)
#define MV643XX_ETH_BAR_1							(0x2208)
#define MV643XX_ETH_SIZE_R_1						(0x220c)
#define MV643XX_ETH_BAR_2							(0x2210)
#define MV643XX_ETH_SIZE_R_2						(0x2214)
#define MV643XX_ETH_BAR_3							(0x2218)
#define MV643XX_ETH_SIZE_R_3						(0x221c)
#define MV643XX_ETH_BAR_4							(0x2220)
#define MV643XX_ETH_SIZE_R_4						(0x2224)
#define MV643XX_ETH_BAR_5							(0x2228)
#define MV643XX_ETH_SIZE_R_5						(0x222c)
#define MV643XX_ETH_NUM_BARS						6

/* Bits in the BAR reg to program cache snooping */
#define MV64360_ENET2MEM_SNOOP_NONE 0x0000
#define MV64360_ENET2MEM_SNOOP_WT	0x1000
#define MV64360_ENET2MEM_SNOOP_WB	0x2000
#define MV64360_ENET2MEM_SNOOP_MSK	0x3000


#define MV643XX_ETH_BAR_ENBL_R						(0x2290)
#define MV643XX_ETH_BAR_DISABLE(bar)				(1<<(bar))
#define MV643XX_ETH_BAR_DISBL_ALL					0x3f

#define MV643XX_ETH_RX_Q0_CURRENT_DESC_PTR(port)	(0x260c+((port)<<10))
#define MV643XX_ETH_RX_Q1_CURRENT_DESC_PTR(port)	(0x261c+((port)<<10))
#define MV643XX_ETH_RX_Q2_CURRENT_DESC_PTR(port)	(0x262c+((port)<<10))
#define MV643XX_ETH_RX_Q3_CURRENT_DESC_PTR(port)	(0x263c+((port)<<10))
#define MV643XX_ETH_RX_Q4_CURRENT_DESC_PTR(port)	(0x264c+((port)<<10))
#define MV643XX_ETH_RX_Q5_CURRENT_DESC_PTR(port)	(0x265c+((port)<<10))
#define MV643XX_ETH_RX_Q6_CURRENT_DESC_PTR(port)	(0x266c+((port)<<10))
#define MV643XX_ETH_RX_Q7_CURRENT_DESC_PTR(port)	(0x267c+((port)<<10))

#define MV643XX_ETH_TX_Q0_CURRENT_DESC_PTR(port)	(0x26c0+((port)<<10))
#define MV643XX_ETH_TX_Q1_CURRENT_DESC_PTR(port)	(0x26c4+((port)<<10))
#define MV643XX_ETH_TX_Q2_CURRENT_DESC_PTR(port)	(0x26c8+((port)<<10))
#define MV643XX_ETH_TX_Q3_CURRENT_DESC_PTR(port)	(0x26cc+((port)<<10))
#define MV643XX_ETH_TX_Q4_CURRENT_DESC_PTR(port)	(0x26d0+((port)<<10))
#define MV643XX_ETH_TX_Q5_CURRENT_DESC_PTR(port)	(0x26d4+((port)<<10))
#define MV643XX_ETH_TX_Q6_CURRENT_DESC_PTR(port)	(0x26d8+((port)<<10))
#define MV643XX_ETH_TX_Q7_CURRENT_DESC_PTR(port)	(0x26dc+((port)<<10))

#define MV643XX_ETH_MAC_ADDR_LO(port)				(0x2414+((port)<<10))
#define MV643XX_ETH_MAC_ADDR_HI(port)				(0x2418+((port)<<10))

/* TYPE DEFINITIONS */

/* just to make the purpose explicit; vars of this
 * type may need CPU-dependent address translation,
 * endian conversion etc.
 */
typedef uint32_t Dma_addr_t;

typedef volatile struct mveth_rx_desc {
#ifndef __BIG_ENDIAN__
#error	"descriptor declaration not implemented for little endian machines"
#endif
	uint16_t	byte_cnt;
	uint16_t	buf_size;
	uint32_t	cmd_sts;					/* control and status */
	Dma_addr_t	next_desc_ptr;				/* next descriptor (as seen from DMA) */
	Dma_addr_t	buf_ptr;
	/* fields below here are not used by the chip */
	void		*u_buf;						/* user buffer */
	volatile struct mveth_rx_desc *next;	/* next descriptor (CPU address; next_desc_ptr is a DMA address) */
	uint32_t	pad[2];
} __attribute__(( aligned(RING_ALIGNMENT) )) MvEthRxDescRec, *MvEthRxDesc;

typedef volatile struct mveth_tx_desc {
#ifndef __BIG_ENDIAN__
#error	"descriptor declaration not implemented for little endian machines"
#endif
	uint16_t	byte_cnt;
	uint16_t	l4i_chk;
	uint32_t	cmd_sts;					/* control and status */
	Dma_addr_t	next_desc_ptr;				/* next descriptor (as seen from DMA) */
	Dma_addr_t	buf_ptr;
	/* fields below here are not used by the chip */
	uint32_t	workaround[2];				/* use this space to work around the 8byte problem (is this real?) */
	void		*u_buf;						/* user buffer */
	volatile struct mveth_tx_desc *next;	/* next descriptor (CPU address; next_desc_ptr is a DMA address)   */
} __attribute__(( aligned(RING_ALIGNMENT) )) MvEthTxDescRec, *MvEthTxDesc;

/* Assume there are never more then 64k aliasing entries */
typedef uint16_t Mc_Refcnt[MV643XX_ETH_NUM_MCAST_ENTRIES*4];

/* driver private data and bsdnet interface structure */
struct mveth_private {
	MvEthRxDesc		rx_ring;					/* pointers to aligned ring area             */
	MvEthTxDesc		tx_ring;					/* pointers to aligned ring area             */
	MvEthRxDesc		ring_area;					/* allocated ring area                       */
	int				rbuf_count, xbuf_count;		/* saved ring sizes from ifconfig            */
	int				port_num;
	int				phy;
	MvEthRxDesc		d_rx_t;						/* tail of the RX ring; next received packet */
	MvEthTxDesc		d_tx_t, d_tx_h;
	uint32_t		rx_desc_dma, tx_desc_dma; 	/* ring address as seen by DMA;	(1:1 on this BSP) */
	int				avail;
	void            (*isr)(void*);
	void            *isr_arg;
	/* Callbacks to handle buffers */
	void			(*cleanup_txbuf)(void*, void*, int);	/* callback to cleanup TX buffer */
	void			*cleanup_txbuf_arg;
	void			*(*alloc_rxbuf)(int *psize, uintptr_t *paddr);	/* allocate RX buffer  */
	void			(*consume_rxbuf)(void*, void*,  int);	/* callback to consume RX buffer */
	void			*consume_rxbuf_arg;
	rtems_id        tid;
	uint32_t		irq_mask;					/* IRQs we use                              */
	uint32_t		xirq_mask;
    int             promisc;
	struct		{
		unsigned		irqs;
		unsigned		maxchain;
		unsigned		repack;
		unsigned		packet;
		unsigned		idrops;					/* no counter in core code                   */
		struct {
			uint64_t	good_octs_rcvd;         /* 64-bit */
			uint32_t	bad_octs_rcvd;
			uint32_t	internal_mac_tx_err;
			uint32_t	good_frames_rcvd;
			uint32_t	bad_frames_rcvd;
			uint32_t	bcast_frames_rcvd;
			uint32_t	mcast_frames_rcvd;
			uint32_t	frames_64_octs;
			uint32_t	frames_65_127_octs;
			uint32_t	frames_128_255_octs;
			uint32_t	frames_256_511_octs;
			uint32_t	frames_512_1023_octs;
			uint32_t	frames_1024_max_octs;
			uint64_t	good_octs_sent;         /* 64-bit */
			uint32_t	good_frames_sent;
			uint32_t	excessive_coll;
			uint32_t	mcast_frames_sent;
			uint32_t	bcast_frames_sent;
			uint32_t	unrec_mac_ctrl_rcvd;
			uint32_t	fc_sent;
			uint32_t	good_fc_rcvd;
			uint32_t	bad_fc_rcvd;
			uint32_t	undersize_rcvd;
			uint32_t	fragments_rcvd;
			uint32_t	oversize_rcvd;
			uint32_t	jabber_rcvd;
			uint32_t	mac_rx_err;
			uint32_t	bad_crc_event;
			uint32_t	coll;
			uint32_t	late_coll;
		} mib;
	}			stats;
	struct {
		Mc_Refcnt	specl, other;
	}           mc_refcnt;
};

/* GLOBAL VARIABLES */

/* Format strings for statistics messages */
static const char *mibfmt[] = {
	"  GOOD_OCTS_RCVD:      %"PRIu64"\n",
	0,
	"  BAD_OCTS_RCVD:       %"PRIu32"\n",
	"  INTERNAL_MAC_TX_ERR: %"PRIu32"\n",
	"  GOOD_FRAMES_RCVD:    %"PRIu32"\n",
	"  BAD_FRAMES_RCVD:     %"PRIu32"\n",
	"  BCAST_FRAMES_RCVD:   %"PRIu32"\n",
	"  MCAST_FRAMES_RCVD:   %"PRIu32"\n",
	"  FRAMES_64_OCTS:      %"PRIu32"\n",
	"  FRAMES_65_127_OCTS:  %"PRIu32"\n",
	"  FRAMES_128_255_OCTS: %"PRIu32"\n",
	"  FRAMES_256_511_OCTS: %"PRIu32"\n",
	"  FRAMES_512_1023_OCTS:%"PRIu32"\n",
	"  FRAMES_1024_MAX_OCTS:%"PRIu32"\n",
	"  GOOD_OCTS_SENT:      %"PRIu64"\n",
	0,
	"  GOOD_FRAMES_SENT:    %"PRIu32"\n",
	"  EXCESSIVE_COLL:      %"PRIu32"\n",
	"  MCAST_FRAMES_SENT:   %"PRIu32"\n",
	"  BCAST_FRAMES_SENT:   %"PRIu32"\n",
	"  UNREC_MAC_CTRL_RCVD: %"PRIu32"\n",
	"  FC_SENT:             %"PRIu32"\n",
	"  GOOD_FC_RCVD:        %"PRIu32"\n",
	"  BAD_FC_RCVD:         %"PRIu32"\n",
	"  UNDERSIZE_RCVD:      %"PRIu32"\n",
	"  FRAGMENTS_RCVD:      %"PRIu32"\n",
	"  OVERSIZE_RCVD:       %"PRIu32"\n",
	"  JABBER_RCVD:         %"PRIu32"\n",
	"  MAC_RX_ERR:          %"PRIu32"\n",
	"  BAD_CRC_EVENT:       %"PRIu32"\n",
	"  COLL:                %"PRIu32"\n",
	"  LATE_COLL:           %"PRIu32"\n",
};

/* Interrupt Handler Connection */

/* forward decls + implementation for IRQ API funcs */

STATIC int
mveth_init_rx_desc_ring(struct mveth_private *mp);

STATIC int
mveth_init_tx_desc_ring(struct mveth_private *mp);

int
BSP_mve_dring_nonsync(struct mveth_private *mp);

static void mveth_isr(rtems_irq_hdl_param unit);
static void noop(const rtems_irq_connect_data *unused)  {}
static int  noop1(const rtems_irq_connect_data *unused) { return 0; }

static rtems_irq_connect_data irq_data[MAX_NUM_SLOTS] = {
	{
		BSP_IRQ_ETH0,
		0,
		(rtems_irq_hdl_param)0,
		noop,
		noop,
		noop1
	},
	{
		BSP_IRQ_ETH1,
		0,
		(rtems_irq_hdl_param)1,
		noop,
		noop,
		noop1
	},
	{
		BSP_IRQ_ETH2,
		0,
		(rtems_irq_hdl_param)2,
		noop,
		noop,
		noop1
	},
};

/* LOW LEVEL SUPPORT ROUTINES */

/* Software Cache Coherency */
#ifndef ENABLE_HW_SNOOPING
#ifndef __PPC__
#error "Software cache coherency maintenance is not implemented for your CPU architecture"
#endif

static inline unsigned INVAL_DESC(volatile void *d)
{
typedef const char cache_line[PPC_CACHE_ALIGNMENT];
	asm volatile("dcbi 0, %1":"=m"(*(cache_line*)d):"r"(d));
	return (unsigned)d;	/* so this can be used in comma expression */
}

static inline void FLUSH_DESC(volatile void *d)
{
typedef const char cache_line[PPC_CACHE_ALIGNMENT];
	asm volatile("dcbf 0, %0"::"r"(d),"m"(*(cache_line*)d));
}

static inline void FLUSH_BARRIER(void)
{
	asm volatile("eieio");
}

/* RX buffers are always cache-line aligned
 * ASSUMPTIONS:
 *   - 'addr' is cache aligned
 *   -  len   is a multiple >0 of cache lines
 */
static inline void INVAL_BUF(register uintptr_t addr, register int len)
{
typedef char maxbuf[2048]; /* more than an ethernet packet */
	do {
		len -= RX_BUF_ALIGNMENT;
		asm volatile("dcbi %0, %1"::"b"(addr),"r"(len));
	} while (len > 0);
	asm volatile("":"=m"(*(maxbuf*)addr));
}

/* Flushing TX buffers is a little bit trickier; we don't really know their
 * alignment but *assume* adjacent addresses are covering 'ordinary' memory
 * so that flushing them does no harm!
 */
static inline void FLUSH_BUF(register uintptr_t addr, register int len)
{
	asm volatile("":::"memory");
	len = MV643XX_ALIGN(len, RX_BUF_ALIGNMENT);
	do {
		asm volatile("dcbf %0, %1"::"b"(addr),"r"(len));
		len -= RX_BUF_ALIGNMENT;
	} while ( len >= 0 );
}

#else /* hardware snooping enabled */

/* inline this to silence compiler warnings */
static inline int INVAL_DESC(volatile void *d)
{ return 0; }

#define FLUSH_DESC(d)	NOOP()
#define INVAL_BUF(b,l)	NOOP()
#define FLUSH_BUF(b,l)	NOOP()
#define FLUSH_BARRIER()	NOOP()

#endif	/* cache coherency support */

/* Synchronize memory access */
#ifdef __PPC__
static inline void membarrier(void)
{
	asm volatile("sync":::"memory");
}
#else
#error "memory barrier instruction not defined (yet) for this CPU"
#endif

/* Enable and disable interrupts at the device */
static inline void
mveth_enable_irqs(struct mveth_private *mp, uint32_t mask)
{
rtems_interrupt_level l;
uint32_t val;
	rtems_interrupt_disable(l);

	val  = MV_READ(MV643XX_ETH_INTERRUPT_ENBL_R(mp->port_num));
	val  = (val | mask | MV643XX_ETH_IRQ_EXT_ENA) & mp->irq_mask;

	MV_WRITE(MV643XX_ETH_INTERRUPT_ENBL_R(mp->port_num),        val);

	val  = MV_READ(MV643XX_ETH_INTERRUPT_EXTEND_ENBL_R(mp->port_num));
	val  = (val | mask) & mp->xirq_mask;
	MV_WRITE(MV643XX_ETH_INTERRUPT_EXTEND_ENBL_R(mp->port_num), val);

	rtems_interrupt_enable(l);
}

static inline uint32_t
mveth_disable_irqs(struct mveth_private *mp, uint32_t mask)
{
rtems_interrupt_level l;
uint32_t val,xval,tmp;
	rtems_interrupt_disable(l);

	val  = MV_READ(MV643XX_ETH_INTERRUPT_ENBL_R(mp->port_num));
	tmp  = ( (val & ~mask) | MV643XX_ETH_IRQ_EXT_ENA ) & mp->irq_mask;
	MV_WRITE(MV643XX_ETH_INTERRUPT_ENBL_R(mp->port_num),        tmp);

	xval = MV_READ(MV643XX_ETH_INTERRUPT_EXTEND_ENBL_R(mp->port_num));
	tmp  = (xval & ~mask) & mp->xirq_mask;
	MV_WRITE(MV643XX_ETH_INTERRUPT_EXTEND_ENBL_R(mp->port_num), tmp);

	rtems_interrupt_enable(l);

	return (val | xval);
}

/* This should be safe even w/o turning off interrupts if multiple
 * threads ack different bits in the cause register (and ignore
 * other ones) since writing 'ones' into the cause register doesn't
 * 'stick'.
 */

static inline uint32_t
mveth_ack_irqs(struct mveth_private *mp, uint32_t mask)
{
register uint32_t x,xe,p;
register uint32_t rval;

		p  = mp->port_num;
		/* Get cause */
		x  = MV_READ(MV643XX_ETH_INTERRUPT_CAUSE_R(p));

		/* Ack interrupts filtering the ones we're interested in */

		/* Note: EXT_IRQ bit clears by itself if EXT interrupts are cleared */
		MV_WRITE(MV643XX_ETH_INTERRUPT_CAUSE_R(p), ~ (x & mp->irq_mask & mask));

				/* linux driver tests 1<<1 as a summary bit for extended interrupts;
				 * the mv64360 seems to use 1<<19 for that purpose; for the moment,
				 * I just check both.
				 * Update: link status irq (1<<16 in xe) doesn't set (1<<19) in x!
				 */
		if ( 1 /* x & 2 */ )
		{
			xe = MV_READ(MV643XX_ETH_INTERRUPT_EXTEND_CAUSE_R(p));

			MV_WRITE(MV643XX_ETH_INTERRUPT_EXTEND_CAUSE_R(p), ~ (xe & mp->xirq_mask & mask));
		} else {
			xe = 0;
		}
#ifdef MVETH_TESTING
		if (    ((x & MV643XX_ETH_ALL_IRQS) & ~MV643XX_ETH_KNOWN_IRQS)
			 || ((xe & MV643XX_ETH_ALL_EXT_IRQS) & ~MV643XX_ETH_KNOWN_EXT_IRQS) ) {
			fprintf(stderr, "Unknown IRQs detected; leaving all disabled for debugging:\n");
			fprintf(stderr, "Cause reg was 0x%08x, ext cause 0x%08x\n", x, xe);
/*
			mp->irq_mask  = 0;
			mp->xirq_mask = 0;
*/
		}
#endif
		/* luckily, the extended and 'normal' interrupts we use don't overlap so
		 * we can just OR them into a single word
		 */
		rval = (xe & mp->xirq_mask) | (x & mp->irq_mask);

#ifdef MVETH_DEBUG
		printk(DRVNAME"%i: mveth_ack_irqs 0x%08x\n", rval);
#endif
		return rval;
}

static void mveth_isr(rtems_irq_hdl_param arg)
{
struct mveth_private *mp   = (struct mveth_private*) arg;

#ifdef MVETH_DEBUG
	printk(DRVNAME": mveth_isr\n");
#endif
	mp->stats.irqs++;
	mp->isr(mp->isr_arg);
}

static void
mveth_clear_mib_counters(struct mveth_private *mp)
{
register int		i;
register uint32_t	b;
	/* reading the counters resets them */
	b = MV643XX_ETH_MIB_COUNTERS(mp->port_num);
	for (i=0; i< MV643XX_ETH_NUM_MIB_COUNTERS; i++, b+=4)
		(void)MV_READ(b);
}

/* Reading a MIB register also clears it. Hence we read the lo
 * register first, then the hi one. Correct reading is guaranteed since
 * the 'lo' register cannot overflow after it is read since it had
 * been reset to 0.
 */
static unsigned long long
read_long_mib_counter(int port_num, int idx)
{
unsigned long lo;
unsigned long long hi;
	lo = MV_READ(MV643XX_ETH_MIB_COUNTERS(port_num)+(idx<<2));
	idx++;
	hi = MV_READ(MV643XX_ETH_MIB_COUNTERS(port_num)+(idx<<2));
	return (hi<<32) | lo;
}

static inline unsigned long
read_mib_counter(int port_num, int idx)
{
	return MV_READ(MV643XX_ETH_MIB_COUNTERS(port_num)+(idx<<2));
}


/* write ethernet address from buffer to hardware (need to change unicast filter after this) */
static void
mveth_write_eaddr(struct mveth_private *mp, unsigned char *eaddr)
{
int			i;
uint32_t	x;

	/* build hi word */
 	for (i=4,x=0; i; i--, eaddr++) {
		x = (x<<8) | *eaddr;
	}
	MV_WRITE(MV643XX_ETH_MAC_ADDR_HI(mp->port_num), x);

	/* build lo word */
 	for (i=2,x=0; i; i--, eaddr++) {
		x = (x<<8) | *eaddr;
	}
	MV_WRITE(MV643XX_ETH_MAC_ADDR_LO(mp->port_num), x);
}

static inline int
port2phy(int port)
{
	port &= 0x1f;
	/* during early init we may not know the phy and we are given a port number instead! */
	return  ( (MV_READ(MV643XX_ETH_PHY_ADDR_R) >> (5*port)) & 0x1f );
}

/* PHY/MII Interface
 *
 * Read/write a PHY register;
 *
 * NOTE: The SMI register is shared among the three devices.
 *       Protection is provided by the global networking semaphore.
 *       If non-bsd drivers are running on a subset of IFs proper
 *       locking of all shared registers must be implemented!
 */
static unsigned
do_mii_read(int phy, unsigned addr)
{
unsigned v;
unsigned wc = 0;

	addr  &= 0x1f;

	/* wait until not busy */
	do {
		v = MV_READ(MV643XX_ETH_SMI_R);
		wc++;
	} while ( MV643XX_ETH_SMI_BUSY & v );

	MV_WRITE(MV643XX_ETH_SMI_R, (addr <<21 ) | (phy<<16) | MV643XX_ETH_SMI_OP_RD );

	do {
		v = MV_READ(MV643XX_ETH_SMI_R);
		wc++;
	} while ( MV643XX_ETH_SMI_BUSY & v );

	if (wc>0xffff)
		wc = 0xffff;
	return (wc<<16) | (v & 0xffff);
}

unsigned
BSP_mve_mii_read(struct mveth_private *mp, unsigned addr)
{
unsigned rval = do_mii_read(mp->phy, addr);
#ifdef MVETH_DEBUG
	printk(DRVNAME": BSP_mve_mii_read(%d): 0x%08x\n", addr, rval);
#endif
	return rval;
}

unsigned
BSP_mve_mii_read_early(int port, unsigned addr)
{
	return do_mii_read(port2phy(port), addr);
}

static unsigned
do_mii_write(int phy, unsigned addr, unsigned v)
{
unsigned wc = 0;

	addr  &= 0x1f;
	v     &= 0xffff;

	/* busywait is ugly but not preventing ISRs or high priority tasks from
	 * preempting us
	 */

	/* wait until not busy */
	while ( MV643XX_ETH_SMI_BUSY & MV_READ(MV643XX_ETH_SMI_R) )
		wc++ /* wait */;

	MV_WRITE(MV643XX_ETH_SMI_R, (addr <<21 ) | (phy<<16) | MV643XX_ETH_SMI_OP_WR | v );

	return wc;
}

unsigned
BSP_mve_mii_write(struct mveth_private *mp, unsigned addr, unsigned v)
{
#ifdef MVETH_DEBUG
	printk(DRVNAME": BSP_mve_mii_write(%d): 0x%08x\n", addr, v);
#endif
	return do_mii_write( mp->phy, addr, v );
}

unsigned
BSP_mve_mii_write_early(int port, unsigned addr, unsigned v)
{
	return do_mii_write(port2phy(port), addr, v);
}


/* MID-LAYER SUPPORT ROUTINES */

/* Start TX if descriptors are exhausted */
static __inline__ void
mveth_start_tx(struct mveth_private *mp)
{
uint32_t running;
	if ( mp->avail <= 0 ) {
		running = MV_READ(MV643XX_ETH_TRANSMIT_QUEUE_COMMAND_R(mp->port_num));
		if ( ! (running & MV643XX_ETH_TX_START(0)) ) {
			MV_WRITE(MV643XX_ETH_TRANSMIT_QUEUE_COMMAND_R(mp->port_num), MV643XX_ETH_TX_START(0));
		}
	}
}

/* Stop TX and wait for the command queues to stop and the fifo to drain */
static uint32_t
mveth_stop_tx(int port)
{
uint32_t active_q;

	active_q = (MV_READ(MV643XX_ETH_TRANSMIT_QUEUE_COMMAND_R(port)) & MV643XX_ETH_TX_ANY_RUNNING);

	if ( active_q ) {
		/* Halt TX and wait for activity to stop */
		MV_WRITE(MV643XX_ETH_TRANSMIT_QUEUE_COMMAND_R(port), MV643XX_ETH_TX_STOP_ALL);
		while ( MV643XX_ETH_TX_ANY_RUNNING & MV_READ(MV643XX_ETH_TRANSMIT_QUEUE_COMMAND_R(port)) )
			/* poll-wait */;
		/* Wait for Tx FIFO to drain */
		while ( ! (MV643XX_ETH_PORT_STATUS_R(port) & MV643XX_ETH_PORT_STATUS_TX_FIFO_EMPTY) )
			/* poll-wait */;
	}

	return active_q;
}

void
BSP_mve_promisc_set(struct mveth_private *mp, int promisc)
{
uint32_t              v;

	v = MV_READ(MV643XX_ETH_PORT_CONFIG_R(mp->port_num));
	if ( (mp->promisc = promisc) )
		v |= MV643XX_ETH_UNICAST_PROMISC_MODE;
	else
		v &= ~MV643XX_ETH_UNICAST_PROMISC_MODE;
	MV_WRITE(MV643XX_ETH_PORT_CONFIG_R(mp->port_num), v);
}

/* update serial port settings from current link status */

void
BSP_mve_mcast_filter_clear(struct mveth_private *mp)
{
int                 i;
register uint32_t 	s,o;
uint32_t            v = mp->promisc ? 0x01010101 : 0x00000000;
	s = MV643XX_ETH_DA_FILTER_SPECL_MCAST_TBL(mp->port_num);
	o = MV643XX_ETH_DA_FILTER_OTHER_MCAST_TBL(mp->port_num);
	for (i=0; i<MV643XX_ETH_NUM_MCAST_ENTRIES; i++) {
		MV_WRITE(s,v);
		MV_WRITE(o,v);
		s+=4;
		o+=4;
	}
	for (i=0; i<sizeof(mp->mc_refcnt.specl)/sizeof(mp->mc_refcnt.specl[0]); i++) {
		mp->mc_refcnt.specl[i] = 0;
		mp->mc_refcnt.other[i] = 0;
	}
}

void
BSP_mve_mcast_filter_accept_all(struct mveth_private *mp)
{
int                 i;
register uint32_t 	s,o;
	s = MV643XX_ETH_DA_FILTER_SPECL_MCAST_TBL(mp->port_num);
	o = MV643XX_ETH_DA_FILTER_OTHER_MCAST_TBL(mp->port_num);
	for (i=0; i<MV643XX_ETH_NUM_MCAST_ENTRIES; i++) {
		MV_WRITE(s,0x01010101);
		MV_WRITE(o,0x01010101);
		s+=4;
		o+=4;
		/* Not clear what we should do with the reference count.
		 * For now just increment it.
		 */
		for (i=0; i<sizeof(mp->mc_refcnt.specl)/sizeof(mp->mc_refcnt.specl[0]); i++) {
			mp->mc_refcnt.specl[i]++;
			mp->mc_refcnt.other[i]++;
		}
	}
}

static void add_entry(uint32_t off, uint8_t hash, Mc_Refcnt *refcnt)
{
uint32_t val;
uint32_t slot = hash & 0xfc;

	if ( 0 == (*refcnt)[hash]++ ) {
		val = MV_READ(off+slot) | ( 1 << ((hash&3)<<3) );
		MV_WRITE(off+slot, val);
	}
}

static void del_entry(uint32_t off, uint8_t hash, Mc_Refcnt *refcnt)
{
uint32_t val;
uint32_t slot = hash & 0xfc;

	if ( (*refcnt)[hash] > 0 && 0 == --(*refcnt)[hash] ) {
		val = MV_READ(off+slot) & ~( 1 << ((hash&3)<<3) );
		MV_WRITE(off+slot, val);
	}
}

void
BSP_mve_mcast_filter_accept_add(struct mveth_private *mp, unsigned char *enaddr)
{
uint32_t   hash;
static const char spec[]={0x01,0x00,0x5e,0x00,0x00};
static const char bcst[]={0xff,0xff,0xff,0xff,0xff,0xff};
uint32_t   tabl;
Mc_Refcnt  *refcnt;

	if ( ! (0x01 & enaddr[0]) ) {
		/* not a multicast address; ignore */
		return;
	}

	if ( 0 == memcmp(enaddr, bcst, sizeof(bcst)) ) {
		/* broadcast address; ignore */
		return;
	}

	if ( 0 == memcmp(enaddr, spec, sizeof(spec)) ) {
		hash   = enaddr[5];
		tabl   = MV643XX_ETH_DA_FILTER_SPECL_MCAST_TBL(mp->port_num);
		refcnt = &mp->mc_refcnt.specl;
	} else {
		uint32_t test, mask;
		int      i;
		/* algorithm used by linux driver */
		for ( hash=0, i=0; i<6; i++ ) {
			hash = (hash ^ enaddr[i]) << 8;
			for ( test=0x8000, mask=0x8380; test>0x0080; test>>=1, mask>>=1 ) {
				if ( hash & test )
					hash ^= mask;
			}
		}
		tabl   = MV643XX_ETH_DA_FILTER_OTHER_MCAST_TBL(mp->port_num);
		refcnt = &mp->mc_refcnt.other;
	}
	add_entry(tabl, hash, refcnt);
}

void
BSP_mve_mcast_filter_accept_del(struct mveth_private *mp, unsigned char *enaddr)
{
uint32_t   hash;
static const char spec[]={0x01,0x00,0x5e,0x00,0x00};
static const char bcst[]={0xff,0xff,0xff,0xff,0xff,0xff};
uint32_t   tabl;
Mc_Refcnt  *refcnt;

	if ( ! (0x01 & enaddr[0]) ) {
		/* not a multicast address; ignore */
		return;
	}

	if ( 0 == memcmp(enaddr, bcst, sizeof(bcst)) ) {
		/* broadcast address; ignore */
		return;
	}

	if ( 0 == memcmp(enaddr, spec, sizeof(spec)) ) {
		hash   = enaddr[5];
		tabl   = MV643XX_ETH_DA_FILTER_SPECL_MCAST_TBL(mp->port_num);
		refcnt = &mp->mc_refcnt.specl;
	} else {
		uint32_t test, mask;
		int      i;
		/* algorithm used by linux driver */
		for ( hash=0, i=0; i<6; i++ ) {
			hash = (hash ^ enaddr[i]) << 8;
			for ( test=0x8000, mask=0x8380; test>0x0080; test>>=1, mask>>=1 ) {
				if ( hash & test )
					hash ^= mask;
			}
		}
		tabl   = MV643XX_ETH_DA_FILTER_OTHER_MCAST_TBL(mp->port_num);
		refcnt = &mp->mc_refcnt.other;
	}
	del_entry(tabl, hash, refcnt);
}

/* Clear all address filters (multi- and unicast) */
static void
mveth_clear_addr_filters(struct mveth_private *mp)
{
register int      i;
register uint32_t u;
	u = MV643XX_ETH_DA_FILTER_UNICAST_TBL(mp->port_num);
	for (i=0; i<MV643XX_ETH_NUM_UNICAST_ENTRIES; i++) {
		MV_WRITE(u,0);
		u+=4;
	}
	BSP_mve_mcast_filter_clear(mp);
}

/* Setup unicast filter for a given MAC address (least significant nibble) */
static void
mveth_ucfilter(struct mveth_private *mp, unsigned char mac_lsbyte, int accept)
{
unsigned nib, slot, bit;
uint32_t	val;
	/* compute slot in table */
	nib  = mac_lsbyte & 0xf;	/* strip nibble     */
	slot = nib & ~3;			/* (nibble/4)*4     */
	bit  = (nib &  3)<<3;		/*  8*(nibble % 4)  */
	val = MV_READ(MV643XX_ETH_DA_FILTER_UNICAST_TBL(mp->port_num) + slot);
	if ( accept ) {
		val |= 0x01 << bit;
	} else {
		val &= 0x0e << bit;
	}
	MV_WRITE(MV643XX_ETH_DA_FILTER_UNICAST_TBL(mp->port_num) + slot, val);
}

#if defined( ENABLE_TX_WORKAROUND_8_BYTE_PROBLEM ) && 0
/* Currently unused; small unaligned buffers seem to be rare
 * so we just use memcpy()...
 */

/* memcpy for 0..7 bytes; arranged so that gcc
 * optimizes for powerpc...
 */

static inline void memcpy8(void *to, void *fr, unsigned x)
{
register uint8_t *d = to, *s = fro;

	d+=l; s+=l;
	if ( l & 1 ) {
		*--d=*--s;
	}
	if ( l & 2 ) {
		/* pre-decrementing causes gcc to use auto-decrementing
		 * PPC instructions (lhzu rx, -2(ry))
		 */
		d-=2; s-=2;
		/* use memcpy; don't cast to short -- accessing
		 * misaligned data as short is not portable
		 * (but it works on PPC).
		 */
		__builtin_memcpy(d,s,2);
	}
	if ( l & 4 ) {
		d-=4; s-=4;
		/* see above */
		__builtin_memcpy(d,s,4);
	}
}
#endif

static int
mveth_assign_desc_raw(MvEthTxDesc d, void *buf, int len, void *uptr, unsigned long extra)
{
int rval = (d->byte_cnt = len);

#ifdef MVETH_TESTING
	assert( !d->u_buf );
	assert(  len   );
#endif

	/* set CRC on all descriptors; seems to be necessary */
	d->cmd_sts  = extra | (TDESC_GEN_CRC | TDESC_ZERO_PAD);

#ifdef ENABLE_TX_WORKAROUND_8_BYTE_PROBLEM
	/* The buffer must be 64bit aligned if the payload is <8 (??) */
	if ( rval < 8 && ( ((uintptr_t)buf) & 7) ) {
		d->buf_ptr = CPUADDR2ENET( d->workaround );
		memcpy((void*)d->workaround, buf, rval);
	} else
#endif
	{
		d->buf_ptr  = CPUADDR2ENET( (unsigned long)buf );
	}
	d->u_buf    = uptr;
	d->l4i_chk  = 0;
	return rval;
}

/*
 * Ring Initialization
 *
 * ENDIAN ASSUMPTION: DMA engine matches CPU endianness (???)
 *
 * Linux driver discriminates __LITTLE and __BIG endian for re-arranging
 * the u16 fields in the descriptor structs. However, no endian conversion
 * is done on the individual fields (SDMA byte swapping is disabled on LE).
 */

STATIC int
mveth_init_rx_desc_ring(struct mveth_private *mp)
{
int i,sz;
MvEthRxDesc	d;
uintptr_t baddr;

	memset((void*)mp->rx_ring, 0, sizeof(*mp->rx_ring)*mp->rbuf_count);

	mp->rx_desc_dma = CPUADDR2ENET(mp->rx_ring);

	for ( i=0, d = mp->rx_ring; i<mp->rbuf_count; i++, d++ ) {
		d->u_buf = mp->alloc_rxbuf(&sz, &baddr);
		assert( d->u_buf );

#ifndef ENABLE_HW_SNOOPING
		/* could reduce the area to max. ethernet packet size */
		INVAL_BUF(baddr, sz);
#endif

		d->buf_size = sz;
		d->byte_cnt = 0;
		d->cmd_sts  = RDESC_DMA_OWNED | RDESC_INT_ENA;
		d->next		= mp->rx_ring + (i+1) % mp->rbuf_count;

		d->buf_ptr  = CPUADDR2ENET( baddr );
		d->next_desc_ptr = CPUADDR2ENET(d->next);
		FLUSH_DESC(d);
	}
	FLUSH_BARRIER();

	mp->d_rx_t = mp->rx_ring;

	/* point the chip to the start of the ring */
	MV_WRITE(MV643XX_ETH_RX_Q0_CURRENT_DESC_PTR(mp->port_num),mp->rx_desc_dma);


	return i;
}

STATIC int
mveth_init_tx_desc_ring(struct mveth_private *mp)
{
int i;
MvEthTxDesc d;

	memset((void*)mp->tx_ring, 0, sizeof(*mp->tx_ring)*mp->xbuf_count);

	/* DMA and CPU live in the same address space (rtems) */
	mp->tx_desc_dma = CPUADDR2ENET(mp->tx_ring);
	mp->avail       = TX_AVAILABLE_RING_SIZE(mp);

	for ( i=0, d=mp->tx_ring; i<mp->xbuf_count; i++,d++ ) {
		d->l4i_chk  = 0;
		d->byte_cnt = 0;
		d->cmd_sts  = 0;
		d->buf_ptr  = 0;

		d->next     = mp->tx_ring + (i+1) % mp->xbuf_count;
		d->next_desc_ptr = CPUADDR2ENET(d->next);
		FLUSH_DESC(d);
	}
	FLUSH_BARRIER();

	mp->d_tx_h = mp->d_tx_t = mp->tx_ring;

	/* point the chip to the start of the ring */
	MV_WRITE(MV643XX_ETH_TX_Q0_CURRENT_DESC_PTR(mp->port_num),mp->tx_desc_dma);

	return i;
}

/* PUBLIC LOW-LEVEL DRIVER ACCESS */

struct mveth_private *
BSP_mve_create(
	int		 unit,
	rtems_id tid,
	void     (*isr)(void*isr_arg),
	void     *isr_arg,
	void (*cleanup_txbuf)(void *user_buf, void *closure, int error_on_tx_occurred),
	void *cleanup_txbuf_arg,
	void *(*alloc_rxbuf)(int *p_size, uintptr_t *p_data_addr),
	void (*consume_rxbuf)(void *user_buf, void *closure, int len),
	void *consume_rxbuf_arg,
	int		rx_ring_size,
	int		tx_ring_size,
	int		irq_mask
)
{
struct mveth_private *mp;
int                  InstallISRSuccessful;

	if ( unit <= 0 || unit > MV643XXETH_NUM_DRIVER_SLOTS ) {
		printk(DRVNAME": Bad unit number %i; must be 1..%i\n", unit, MV643XXETH_NUM_DRIVER_SLOTS);
		return 0;
	}

	if ( rx_ring_size < 0 && tx_ring_size < 0 )
		return 0;

	if ( MV_64360 != BSP_getDiscoveryVersion(0) ) {
		printk(DRVNAME": not mv64360 chip\n");
		return 0;
	}

	if ( tx_ring_size < 1 ) {
		printk(DRVNAME": tx ring size must not be zero (networking configuration issue?)\n");
		return 0;
	}

	if ( rx_ring_size < 1 ) {
		printk(DRVNAME": rx ring size must not be zero (networking configuration issue?)\n");
		return 0;
	}

	mp = calloc( 1, sizeof *mp );

	mp->port_num          = unit-1;
	mp->phy               = port2phy(mp->port_num);

	mp->tid               = tid;
	mp->isr               = isr;
	mp->isr_arg           = isr_arg;

	mp->cleanup_txbuf     = cleanup_txbuf;
	mp->cleanup_txbuf_arg = cleanup_txbuf_arg;
	mp->alloc_rxbuf       = alloc_rxbuf;
	mp->consume_rxbuf     = consume_rxbuf;
	mp->consume_rxbuf_arg = consume_rxbuf_arg;

	mp->rbuf_count = rx_ring_size;
	mp->xbuf_count = tx_ring_size;

	if ( mp->xbuf_count > 0 )
		mp->xbuf_count += TX_NUM_TAG_SLOTS;

	if ( mp->rbuf_count < 0 )
		mp->rbuf_count = 0;
	if ( mp->xbuf_count < 0 )
		mp->xbuf_count = 0;

	/* allocate ring area; add 1 entry -- room for alignment */
	assert( !mp->ring_area );
	mp->ring_area = malloc( sizeof(*mp->ring_area) * (mp->rbuf_count + mp->xbuf_count + 1) );
	assert( mp->ring_area );

	BSP_mve_stop_hw(mp);

	if ( irq_mask ) {
		irq_data[mp->port_num].hdl    = mveth_isr;
		irq_data[mp->port_num].handle = (rtems_irq_hdl_param)mp;
		InstallISRSuccessful = BSP_install_rtems_irq_handler( &irq_data[mp->port_num] );
		assert( InstallISRSuccessful );
	}

	if ( rx_ring_size < 0 )
		irq_mask &= ~ MV643XX_ETH_IRQ_RX_DONE;
	if ( tx_ring_size < 0 )
		irq_mask &= ~ MV643XX_ETH_EXT_IRQ_TX_DONE;

	mp->irq_mask = (irq_mask & MV643XX_ETH_IRQ_RX_DONE);
	if ( (irq_mask &= (MV643XX_ETH_EXT_IRQ_TX_DONE | MV643XX_ETH_EXT_IRQ_LINK_CHG)) ) {
		mp->irq_mask |= MV643XX_ETH_IRQ_EXT_ENA;
		mp->xirq_mask = irq_mask;
	} else {
		mp->xirq_mask = 0;
	}

	return mp;
}

void
BSP_mve_update_serial_port(struct mveth_private *mp, int media)
{
int port = mp->port_num;
uint32_t old, new;

#ifdef MVETH_DEBUG
	printk(DRVNAME": Entering BSP_mve_update_serial_port()\n");
#endif

	new = old = MV_READ(MV643XX_ETH_SERIAL_CONTROL_R(port));

	/* mask speed and duplex settings */
	new &= ~(  MV643XX_ETH_SET_GMII_SPEED_1000
			 | MV643XX_ETH_SET_MII_SPEED_100
			 | MV643XX_ETH_SET_FULL_DUPLEX );

	if ( (MV643XX_MEDIA_FD & media) )
		new |= MV643XX_ETH_SET_FULL_DUPLEX;

	switch ( (media & MV643XX_MEDIA_SPEED_MSK) ) {
		default: /* treat as 10 */
			break;
		case MV643XX_MEDIA_100:
			new |= MV643XX_ETH_SET_MII_SPEED_100;
			break;
		case MV643XX_MEDIA_1000:
			new |= MV643XX_ETH_SET_GMII_SPEED_1000;
			break;
	}


	if ( new != old ) {
		if ( ! (MV643XX_ETH_SERIAL_PORT_ENBL & new) ) {
			/* just write */
			MV_WRITE(MV643XX_ETH_SERIAL_CONTROL_R(port), new);
		} else {
			uint32_t were_running;

			were_running = mveth_stop_tx(port);

			old &= ~MV643XX_ETH_SERIAL_PORT_ENBL;
			MV_WRITE(MV643XX_ETH_SERIAL_CONTROL_R(port), old);
			MV_WRITE(MV643XX_ETH_SERIAL_CONTROL_R(port), new);
			/* linux driver writes twice... */
			MV_WRITE(MV643XX_ETH_SERIAL_CONTROL_R(port), new);

			if ( were_running ) {
				MV_WRITE(MV643XX_ETH_TRANSMIT_QUEUE_COMMAND_R(mp->port_num), MV643XX_ETH_TX_START(0));
			}
		}
	}
	/* If TX stalled because there was no buffer then whack it */
	mveth_start_tx(mp);
}

rtems_id
BSP_mve_get_tid(struct mveth_private *mp)
{
    return mp->tid;
}

int
BSP_mve_detach(struct mveth_private *mp)
{
	BSP_mve_stop_hw(mp);
	if ( mp->irq_mask || mp->xirq_mask ) {
		if ( !BSP_remove_rtems_irq_handler( &irq_data[mp->port_num] ) )
			return -1;
	}
	free( (void*)mp->ring_area );
	memset(mp, 0, sizeof(*mp));
	__asm__ __volatile__("":::"memory");
	return 0;
}

/* MAIN RX-TX ROUTINES
 *
 * BSP_mve_swipe_tx():  descriptor scavenger; releases mbufs
 * BSP_mve_send_buf():  xfer mbufs from IF to chip
 * BSP_mve_swipe_rx():  enqueue received mbufs to interface
 *                    allocate new ones and yield them to the
 *                    chip.
 */

/* clean up the TX ring freeing up buffers */
int
BSP_mve_swipe_tx(struct mveth_private *mp)
{
int						rval = 0;
register MvEthTxDesc	d;

	for ( d = mp->d_tx_t; d->buf_ptr; d = NEXT_TXD(d) ) {

		INVAL_DESC(d);

		if (	(TDESC_DMA_OWNED & d->cmd_sts)
			 &&	(uint32_t)d == MV_READ(MV643XX_ETH_CURRENT_SERVED_TX_DESC(mp->port_num)) )
			break;

		/* d->u_buf is only set on the last descriptor in a chain;
		 * we only count errors in the last descriptor;
		 */
		if ( d->u_buf ) {
			mp->cleanup_txbuf(d->u_buf, mp->cleanup_txbuf_arg, (d->cmd_sts & TDESC_ERROR) ? 1 : 0);
			d->u_buf = 0;
		}

		d->buf_ptr = 0;

		rval++;
	}
	mp->d_tx_t = d;
	mp->avail += rval;

	return mp->avail;
}

int
BSP_mve_send_buf_chain(struct mveth_private *mp, MveEthBufIterNext next, MveEthBufIter *it)
{
int						rval;
register MvEthTxDesc	l,d,h;
int						nmbs;
MveEthBufIter           head = *it;

	rval = 0;

	/* if no descriptor is available; try to wipe the queue */
	if ( (mp->avail < 1) && MVETH_CLEAN_ON_SEND(mp)<=0 ) {
		/* Maybe TX is stalled and needs to be restarted */
		mveth_start_tx(mp);
		return -1;
	}

	h = mp->d_tx_h;

#ifdef MVETH_TESTING
	assert( !h->buf_ptr );
	assert( !h->u_buf   );
#endif

	/* Don't use the first descriptor yet because BSP_mve_swipe_tx()
	 * needs mp->d_tx_h->buf_ptr == NULL as a marker. Hence, we
	 * start with the second mbuf and fill the first descriptor
	 * last.
	 */

	l = h;
	d = NEXT_TXD(h);

	mp->avail--;

	nmbs = 1;
	while ( (it = next(it)) ) {
		if ( 0 == it->len )
			continue;	/* skip empty mbufs */

		nmbs++;

		if ( mp->avail < 1 && MVETH_CLEAN_ON_SEND(mp)<=0 ) {
			/* Maybe TX was stalled - try to restart */
			mveth_start_tx(mp);

			/* not enough descriptors; cleanup...
			 * the first slot was never used, so we start
			 * at mp->d_tx_h->next;
			 */
			for ( l = NEXT_TXD(h); l!=d; l=NEXT_TXD(l) ) {
#ifdef MVETH_TESTING
				assert( l->u_buf == 0 );
#endif
				l->buf_ptr  = 0;
				l->cmd_sts  = 0;
				mp->avail++;
			}
			mp->avail++;
			if ( nmbs > TX_AVAILABLE_RING_SIZE(mp) ) {
				/* this chain will never fit into the ring */
				if ( nmbs > mp->stats.maxchain )
					mp->stats.maxchain = nmbs;
				mp->stats.repack++;
				/* caller may reorganize chain */
				return -2;
			}
			return -1;
		}

		mp->avail--;

#ifdef MVETH_TESTING
		assert( d != h      );
		assert( !d->buf_ptr );
#endif

		/* fill this slot */
		rval += mveth_assign_desc_raw(d, it->data, it->len, it->uptr, TDESC_DMA_OWNED);

		FLUSH_BUF( (uint32_t)it->data, it->len );

		l = d;
		d = NEXT_TXD(d);

		FLUSH_DESC(l);
	}

	/* fill first slot - don't release to DMA yet */
	rval += mveth_assign_desc_raw(h, head.data, head.len, head.uptr, TDESC_FRST);


	FLUSH_BUF((uint32_t)head.data, head.len);


	/* tag last slot; this covers the case where 1st==last */
	l->cmd_sts      |= TDESC_LAST | TDESC_INT_ENA;

	FLUSH_DESC(l);

	/* Tag end; make sure chip doesn't try to read ahead of here! */
	l->next->cmd_sts = 0;
	FLUSH_DESC(l->next);

	membarrier();

	/* turn over the whole chain by flipping ownership of the first desc */
	h->cmd_sts |= TDESC_DMA_OWNED;

	FLUSH_DESC(h);

	membarrier();

	/* notify the device */
	MV_WRITE(MV643XX_ETH_TRANSMIT_QUEUE_COMMAND_R(mp->port_num), MV643XX_ETH_TX_START(0));

	/* Update softc */
	mp->stats.packet++;
	if ( nmbs > mp->stats.maxchain )
		mp->stats.maxchain = nmbs;

	/* remember new head */
	mp->d_tx_h = d;

	return rval; /* #bytes sent */
}

int
BSP_mve_send_buf_raw(
	struct mveth_private *mp,
	void                 *head_p,
	int                   h_len,
	void                 *data_p,
    int                   d_len)
{
int						rval;
register MvEthTxDesc	l,d,h;
int						needed;
void                    *frst_buf;
int                     frst_len;
void                    *uarg;

	rval = 0;

#ifdef MVETH_TESTING
	assert(head_p || data_p);
#endif

	needed = head_p && data_p ? 2 : 1;

	/* if no descriptor is available; try to wipe the queue */
	if (   ( mp->avail < needed )
        && ( MVETH_CLEAN_ON_SEND(mp) <= 0 || mp->avail < needed ) ) {
		/* Maybe TX was stalled and needs a restart */
		mveth_start_tx(mp);
		return -1;
	}

	h = mp->d_tx_h;

#ifdef MVETH_TESTING
	assert( !h->buf_ptr );
	assert( !h->u_buf   );
#endif

	/* find the 'first' user buffer */
	if ( (frst_buf = head_p) && (h_len > 0) ) {
		frst_len = h_len;
	} else {
		frst_buf = data_p;
		frst_len = d_len;
	}

	uarg = (head_p && ! h_len) ? head_p : frst_buf;

	/* Legacy: if h_len == 0 but head_p is not then use that for the user arg */

	/* Don't use the first descriptor yet because BSP_mve_swipe_tx()
	 * needs mp->d_tx_h->buf_ptr == NULL as a marker. Hence, we
	 * start with the second (optional) slot and fill the first
     * descriptor last.
	 */

	l = h;
	d = NEXT_TXD(h);

	mp->avail--;

	if ( needed > 1 ) {
		mp->avail--;
#ifdef MVETH_TESTING
		assert( d != h      );
		assert( !d->buf_ptr );
#endif
		rval += mveth_assign_desc_raw(d, data_p, d_len, 0, TDESC_DMA_OWNED);
		FLUSH_BUF( (uint32_t)data_p, d_len );
		d->u_buf = data_p;

		l = d;
		d = NEXT_TXD(d);

		FLUSH_DESC(l);
	}

	/* fill first slot with raw buffer - don't release to DMA yet */
	rval       += mveth_assign_desc_raw(h, frst_buf, frst_len, 0, TDESC_FRST);

	FLUSH_BUF( (uint32_t)frst_buf, frst_len);

	/* tag last slot; this covers the case where 1st==last */
	l->cmd_sts |= TDESC_LAST | TDESC_INT_ENA;

	/* first buffer of 'chain' goes into last desc */
	l->u_buf    = uarg;

	FLUSH_DESC(l);

	/* Tag end; make sure chip doesn't try to read ahead of here! */
	l->next->cmd_sts = 0;
	FLUSH_DESC(l->next);

	membarrier();

	/* turn over the whole chain by flipping ownership of the first desc */
	h->cmd_sts |= TDESC_DMA_OWNED;

	FLUSH_DESC(h);

	membarrier();

	/* notify the device */
	MV_WRITE(MV643XX_ETH_TRANSMIT_QUEUE_COMMAND_R(mp->port_num), MV643XX_ETH_TX_START(0));

	/* Update softc */
	mp->stats.packet++;
	if ( needed > mp->stats.maxchain )
		mp->stats.maxchain = needed;

	/* remember new head */
	mp->d_tx_h = d;

	return rval; /* #bytes sent */
}

/* send received buffers upwards and replace them
 * with freshly allocated ones;
 * ASSUMPTION:	buffer length NEVER changes and is set
 *				when the ring is initialized.
 * TS 20060727: not sure if this assumption is still necessary - I believe it isn't.
 */

int
BSP_mve_swipe_rx(struct mveth_private *mp)
{
int						rval = 0, err;
register MvEthRxDesc	d;
void					*newbuf;
int						sz;
uintptr_t 				baddr;

	for ( d = mp->d_rx_t; ! (INVAL_DESC(d), (RDESC_DMA_OWNED & d->cmd_sts)); d=NEXT_RXD(d) ) {

#ifdef MVETH_TESTING
		assert(d->u_buf);
#endif

		err = (RDESC_ERROR & d->cmd_sts);

		if ( err || !(newbuf = mp->alloc_rxbuf(&sz, &baddr)) ) {
			/* drop packet and recycle buffer */
			newbuf = d->u_buf;
			mp->stats.idrops++;
		} else {
#ifdef MVETH_TESTING
			assert( d->byte_cnt > 0 );
#endif
			mp->consume_rxbuf(d->u_buf, mp->consume_rxbuf_arg, d->byte_cnt);

#ifndef ENABLE_HW_SNOOPING
			/* could reduce the area to max. ethernet packet size */
			INVAL_BUF(baddr, sz);
#endif
			d->u_buf    = newbuf;
			d->buf_ptr  = CPUADDR2ENET(baddr);
			d->buf_size = sz;
			FLUSH_DESC(d);
		}

		membarrier();

		d->cmd_sts = RDESC_DMA_OWNED | RDESC_INT_ENA;

		FLUSH_DESC(d);

		rval++;
	}
	MV_WRITE(MV643XX_ETH_RECEIVE_QUEUE_COMMAND_R(mp->port_num), MV643XX_ETH_RX_START(0));
	mp->d_rx_t = d;
	return rval;
}

/* Stop hardware and clean out the rings */
void
BSP_mve_stop_hw(struct mveth_private *mp)
{
MvEthTxDesc	d;
MvEthRxDesc	r;
int			i;

	mveth_disable_irqs(mp, -1);

	mveth_stop_tx(mp->port_num);

	/* cleanup TX rings */
	if (mp->d_tx_t) { /* maybe ring isn't initialized yet */
		for ( i=0, d=mp->tx_ring; i<mp->xbuf_count; i++, d++ ) {
			/* should be safe to clear ownership */
			d->cmd_sts &= ~TDESC_DMA_OWNED;
			FLUSH_DESC(d);
		}
		FLUSH_BARRIER();

		BSP_mve_swipe_tx(mp);

#ifdef MVETH_TESTING
		assert( mp->d_tx_h == mp->d_tx_t );
		for ( i=0, d=mp->tx_ring; i<mp->xbuf_count; i++, d++ ) {
			assert( !d->buf_ptr );
		}
#endif
	}

	MV_WRITE(MV643XX_ETH_RECEIVE_QUEUE_COMMAND_R(mp->port_num), MV643XX_ETH_RX_STOP_ALL);
	while ( MV643XX_ETH_RX_ANY_RUNNING & MV_READ(MV643XX_ETH_RECEIVE_QUEUE_COMMAND_R(mp->port_num)) )
		/* poll-wait */;

	/* stop serial port */
	MV_WRITE(MV643XX_ETH_SERIAL_CONTROL_R(mp->port_num),
		MV_READ(MV643XX_ETH_SERIAL_CONTROL_R(mp->port_num))
		& ~( MV643XX_ETH_SERIAL_PORT_ENBL | MV643XX_ETH_FORCE_LINK_FAIL_DISABLE | MV643XX_ETH_FORCE_LINK_PASS)
		);

	/* clear pending interrupts */
	MV_WRITE(MV643XX_ETH_INTERRUPT_CAUSE_R(mp->port_num), 0);
	MV_WRITE(MV643XX_ETH_INTERRUPT_EXTEND_CAUSE_R(mp->port_num), 0);

	/* cleanup RX rings */
	if ( mp->rx_ring ) {
		for ( i=0, r=mp->rx_ring; i<mp->rbuf_count; i++, r++ ) {
			/* should be OK to clear ownership flag */
			r->cmd_sts = 0;
			FLUSH_DESC(r);
			mp->consume_rxbuf(r->u_buf, mp->consume_rxbuf_arg, 0);
			r->u_buf = 0;
		}
		FLUSH_BARRIER();
	}


}

uint32_t mveth_serial_ctrl_config_val = MVETH_SERIAL_CTRL_CONFIG_VAL;

/* Fire up the low-level driver
 *
 * - make sure hardware is halted
 * - enable cache snooping
 * - clear address filters
 * - clear mib counters
 * - reset phy
 * - initialize (or reinitialize) descriptor rings
 * - check that the firmware has set up a reasonable mac address.
 * - generate unicast filter entry for our mac address
 * - write register config values to the chip
 * - start hardware (serial port and SDMA)
 */

void
BSP_mve_init_hw(struct mveth_private *mp, int promisc, unsigned char *enaddr, int media)
{
int					i;
uint32_t			v;
static int			inited = 0;

#ifdef MVETH_DEBUG
	printk(DRVNAME"%i: Entering BSP_mve_init_hw()\n", mp->port_num+1);
#endif

	/* since enable/disable IRQ routine only operate on select bitsets
	 * we must make sure everything is masked initially.
	 */
	MV_WRITE(MV643XX_ETH_INTERRUPT_ENBL_R(mp->port_num),        0);
	MV_WRITE(MV643XX_ETH_INTERRUPT_EXTEND_ENBL_R(mp->port_num), 0);

	BSP_mve_stop_hw(mp);

	memset(&mp->stats, 0, sizeof(mp->stats));

	mp->promisc = promisc;

	/* MotLoad has cache snooping disabled on the ENET2MEM windows.
	 * Some comments in (linux) indicate that there are errata
	 * which cause problems which would be a real bummer.
	 * We try it anyways...
	 */
	if ( !inited ) {
	unsigned long disbl, bar;
		inited = 1;	/* FIXME: non-thread safe lazy init */
		disbl = MV_READ(MV643XX_ETH_BAR_ENBL_R);
			/* disable all 6 windows */
			MV_WRITE(MV643XX_ETH_BAR_ENBL_R, MV643XX_ETH_BAR_DISBL_ALL);
			/* set WB snooping on enabled bars */
			for ( i=0; i<MV643XX_ETH_NUM_BARS*8; i+=8 ) {
				if ( (bar = MV_READ(MV643XX_ETH_BAR_0 + i)) && MV_READ(MV643XX_ETH_SIZE_R_0 + i) ) {
#ifdef ENABLE_HW_SNOOPING
					MV_WRITE(MV643XX_ETH_BAR_0 + i, bar | MV64360_ENET2MEM_SNOOP_WB);
#else
					MV_WRITE(MV643XX_ETH_BAR_0 + i, bar & ~MV64360_ENET2MEM_SNOOP_MSK);
#endif
					/* read back to flush fifo [linux comment] */
					(void)MV_READ(MV643XX_ETH_BAR_0 + i);
				}
			}
			/* restore/re-enable */
		MV_WRITE(MV643XX_ETH_BAR_ENBL_R, disbl);
	}

	mveth_clear_mib_counters(mp);
	mveth_clear_addr_filters(mp);

/*	Just leave it alone...
	reset_phy();
*/

	if ( mp->rbuf_count > 0 ) {
		mp->rx_ring = (MvEthRxDesc)MV643XX_ALIGN(mp->ring_area, RING_ALIGNMENT);
		mveth_init_rx_desc_ring(mp);
	}

	if ( mp->xbuf_count > 0 ) {
		mp->tx_ring = (MvEthTxDesc)mp->rx_ring + mp->rbuf_count;
		mveth_init_tx_desc_ring(mp);
	}

	if ( enaddr ) {
		/* set ethernet address from arpcom struct */
#ifdef MVETH_DEBUG
		printk(DRVNAME"%i: Writing MAC addr ", mp->port_num+1);
		for (i=5; i>=0; i--) {
			printk("%02X%c", enaddr[i], i?':':'\n');
		}
#endif
		mveth_write_eaddr(mp, enaddr);
	}

	/* set mac address and unicast filter */

	{
	uint32_t machi, maclo;
		maclo = MV_READ(MV643XX_ETH_MAC_ADDR_LO(mp->port_num));
		machi = MV_READ(MV643XX_ETH_MAC_ADDR_HI(mp->port_num));
		/* ASSUME: firmware has set the mac address for us
		 *         - if assertion fails, we have to do more work...
		 */
		assert( maclo && machi && maclo != 0xffffffff && machi != 0xffffffff );
		mveth_ucfilter(mp, maclo&0xff, 1/* accept */);
	}

	/* port, serial and sdma configuration */
	v = MVETH_PORT_CONFIG_VAL;
	if ( promisc ) {
		/* multicast filters were already set up to
		 * accept everything (mveth_clear_addr_filters())
		 */
		v |= MV643XX_ETH_UNICAST_PROMISC_MODE;
	} else {
		v &= ~MV643XX_ETH_UNICAST_PROMISC_MODE;
	}
	MV_WRITE(MV643XX_ETH_PORT_CONFIG_R(mp->port_num),
				v);
	MV_WRITE(MV643XX_ETH_PORT_CONFIG_XTEND_R(mp->port_num),
				MVETH_PORT_XTEND_CONFIG_VAL);

	v  = MV_READ(MV643XX_ETH_SERIAL_CONTROL_R(mp->port_num));
	v &= ~(MVETH_SERIAL_CTRL_CONFIG_MSK);
	v |= mveth_serial_ctrl_config_val;
	MV_WRITE(MV643XX_ETH_SERIAL_CONTROL_R(mp->port_num), v);

	if ( (MV643XX_MEDIA_LINK & media) ) {
		BSP_mve_update_serial_port(mp, media);
	}

	/* enable serial port */
	v  = MV_READ(MV643XX_ETH_SERIAL_CONTROL_R(mp->port_num));
	MV_WRITE(MV643XX_ETH_SERIAL_CONTROL_R(mp->port_num),
				v | MV643XX_ETH_SERIAL_PORT_ENBL);

#ifndef __BIG_ENDIAN__
#error	"byte swapping needs to be disabled for little endian machines"
#endif
	MV_WRITE(MV643XX_ETH_SDMA_CONFIG_R(mp->port_num), MVETH_SDMA_CONFIG_VAL);

	/* allow short frames */
	MV_WRITE(MV643XX_ETH_RX_MIN_FRAME_SIZE_R(mp->port_num), MVETH_MIN_FRAMSZ_CONFIG_VAL);

	MV_WRITE(MV643XX_ETH_INTERRUPT_CAUSE_R(mp->port_num), 0);
	MV_WRITE(MV643XX_ETH_INTERRUPT_EXTEND_CAUSE_R(mp->port_num), 0);
	/* TODO: set irq coalescing */

	/* enable Rx */
	if ( mp->rbuf_count > 0 ) {
		MV_WRITE(MV643XX_ETH_RECEIVE_QUEUE_COMMAND_R(mp->port_num), MV643XX_ETH_RX_START(0));
	}

	mveth_enable_irqs(mp, -1);

#ifdef MVETH_DEBUG
	printk(DRVNAME"%i: Leaving BSP_mve_init_hw()\n", mp->port_num+1);
#endif
}

/* read ethernet address from hw to buffer */
void
BSP_mve_read_eaddr(struct mveth_private *mp, unsigned char *oeaddr)
{
int				i;
uint32_t		x;
unsigned char	buf[6], *eaddr;

	eaddr = oeaddr ? oeaddr : buf;

	eaddr += 5;
	x = MV_READ(MV643XX_ETH_MAC_ADDR_LO(mp->port_num));

	/* lo word */
 	for (i=2; i; i--, eaddr--) {
		*eaddr = (unsigned char)(x & 0xff);
		x>>=8;
	}

	x = MV_READ(MV643XX_ETH_MAC_ADDR_HI(mp->port_num));
	/* hi word */
 	for (i=4; i; i--, eaddr--) {
		*eaddr = (unsigned char)(x & 0xff);
		x>>=8;
	}

	if ( !oeaddr ) {
		printf("%02X",buf[0]);
		for (i=1; i<sizeof(buf); i++)
			printf(":%02X",buf[i]);
		printf("\n");
	}
}

void
BSP_mve_enable_irqs(struct mveth_private *mp)
{
	mveth_enable_irqs(mp, -1);
}

void
BSP_mve_disable_irqs(struct mveth_private *mp)
{
	mveth_disable_irqs(mp, -1);
}

uint32_t
BSP_mve_ack_irqs(struct mveth_private *mp)
{
	return mveth_ack_irqs(mp, -1);
}


void
BSP_mve_enable_irq_mask(struct mveth_private *mp, uint32_t mask)
{
	mveth_enable_irqs(mp, mask);
}

uint32_t
BSP_mve_disable_irq_mask(struct mveth_private *mp, uint32_t mask)
{
	return mveth_disable_irqs(mp, mask);
}

uint32_t
BSP_mve_ack_irq_mask(struct mveth_private *mp, uint32_t mask)
{
	return mveth_ack_irqs(mp, mask);
}

void
BSP_mve_dump_stats(struct mveth_private *mp, FILE *f)
{
int      p = mp->port_num;
int      idx;
uint32_t v;

	if ( !f )
		f = stdout;

	fprintf(f, DRVNAME"%i Statistics:\n",        mp->port_num + 1);
	fprintf(f, "  # IRQS:                 %i\n", mp->stats.irqs);
	fprintf(f, "  Max. mbuf chain length: %i\n", mp->stats.maxchain);
	fprintf(f, "  # repacketed:           %i\n", mp->stats.repack);
	fprintf(f, "  # packets:              %i\n", mp->stats.packet);
	fprintf(f, "  # buffer alloc failed:  %i\n", mp->stats.idrops);
	fprintf(f, "MIB Counters:\n");
	for ( idx = MV643XX_ETH_MIB_GOOD_OCTS_RCVD_LO>>2;
			idx < MV643XX_ETH_NUM_MIB_COUNTERS;
			idx++ ) {
		switch ( idx ) {
			case MV643XX_ETH_MIB_GOOD_OCTS_RCVD_LO>>2:
				mp->stats.mib.good_octs_rcvd += read_long_mib_counter(p, idx);
				fprintf(f, mibfmt[idx], mp->stats.mib.good_octs_rcvd);
				idx++;
				break;

			case MV643XX_ETH_MIB_GOOD_OCTS_SENT_LO>>2:
				mp->stats.mib.good_octs_sent += read_long_mib_counter(p, idx);
				fprintf(f, mibfmt[idx], mp->stats.mib.good_octs_sent);
				idx++;
				break;

			default:
				v = ((uint32_t*)&mp->stats.mib)[idx] += read_mib_counter(p, idx);
				fprintf(f, mibfmt[idx], v);
				break;
		}
	}
	fprintf(f, "\n");
}

#ifdef MVETH_DEBUG
/* Display/dump descriptor rings */

/* These low-level routines need to be synchronized with
 * any Tx/Rx threads!
 */
int
BSP_mve_dring_nonsync(struct mveth_private *mp)
{
int i;
if (1) {
MvEthRxDesc pr;
printf("RX:\n");

	for (i=0, pr=mp->rx_ring; i<mp->rbuf_count; i++, pr++) {
		printf("cnt: 0x%04x, size: 0x%04x, stat: 0x%08x, next: 0x%08x, buf: 0x%08x\n",
			pr->byte_cnt, pr->buf_size, pr->cmd_sts, (uint32_t)pr->next_desc_ptr, pr->buf_ptr);
	}
}
if (1) {
MvEthTxDesc pt;
printf("TX:\n");
	for (i=0, pt=mp->tx_ring; i<mp->xbuf_count; i++, pt++) {
		printf("cnt: 0x%04x, stat: 0x%08x, next: 0x%08x, buf: 0x%08x, mb: 0x%08x\n",
			pt->byte_cnt, pt->cmd_sts, (uint32_t)pt->next_desc_ptr, pt->buf_ptr,
			(uint32_t)pt->u_buf);
	}
}
	return 0;
}
#endif
