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

#ifndef KERNEL
#define KERNEL
#endif
#ifndef _KERNEL
#define _KERNEL
#endif

#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/error.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/gtreg.h>
#include <libcpu/byteorder.h>

#include <dev/mii/mii.h>
#include <net/if_media.h>

/* Not so nice; would be more elegant not to depend on C library but the
 * RTEMS-specific ioctl for dumping statistics needs stdio anyways.
 */

/*#define NDEBUG effectively removes all assertions
 * If defining NDEBUG, MAKE SURE assert() EXPRESSION HAVE NO SIDE_EFFECTS!!
 * This driver DOES have side-effects, so DONT DEFINE NDEBUG
 * Performance-critical assertions are removed by undefining MVETH_TESTING.
 */

#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <inttypes.h>

#include <rtems/rtems_bsdnet.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>

#include <rtems/rtems_mii_ioctl.h>
#include <bsp/early_enet_link_status.h>
#include <bsp/if_mve_pub.h>

/* CONFIGURABLE PARAMETERS */

/* Enable Hardware Snooping; if this is disabled (undefined),
 * cache coherency is maintained by software.
 */
#undef  ENABLE_HW_SNOOPING

/* Compile-time debugging features */

/* Enable paranoia assertions and checks; reduce # of descriptors to minimum for stressing   */
#undef  MVETH_TESTING

/* Enable debugging messages and some support routines  (dump rings etc.)                    */      
#undef  MVETH_DEBUG

#ifndef DISABLE_DETACHING	/* can override from Makefile */
/* Hack for driver development; rtems bsdnet doesn't implement detaching an interface :-(
 * but this hack allows us to unload/reload the driver module which makes development
 * a lot less painful.
 */
#define MVETH_DETACH_HACK
#endif

/* Ring sizes */

#ifdef MVETH_TESTING

/* hard and small defaults */
#undef  MV643XX_RX_RING_SIZE
#define MV643XX_RX_RING_SIZE	2
#undef  MV643XX_TX_RING_SIZE
#define MV643XX_TX_RING_SIZE	4

#else /* MVETH_TESTING */

/* Define default ring sizes, allow override from bsp.h, Makefile,... and from ifcfg->rbuf_count/xbuf_count */

#ifndef MV643XX_RX_RING_SIZE
#define MV643XX_RX_RING_SIZE	40	/* attached buffers are always 2k clusters, i.e., this
									 * driver - with a configured ring size of 40 - constantly
									 * locks 80k of cluster memory - your app config better
									 * provides enough space!
									 */
#endif

#ifndef MV643XX_TX_RING_SIZE
/* NOTE: tx ring size MUST be > max. # of fragments / mbufs in a chain;
 *       in 'TESTING' mode, special code is compiled in to repackage
 *		 chains that are longer than the ring size. Normally, this is
 *		 disabled for sake of speed.
 *		 I observed chains of >17 entries regularly!
 *
 *       Also, TX_NUM_TAG_SLOTS (1) must be left empty as a marker, hence
 *       the ring size must be > max. #frags + 1.
 */
#define MV643XX_TX_RING_SIZE	200	/* these are smaller fragments and not occupied when
									 * the driver is idle.
									 */
#endif

#endif /* MVETH_TESTING */

/* How many instances to we support (bsp.h could override) */
#ifndef MV643XXETH_NUM_DRIVER_SLOTS
#define MV643XXETH_NUM_DRIVER_SLOTS	2
#endif

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
#define _ALIGN(b, a)	((((uint32_t)(b)) + (a)-1) & (~((a)-1)))

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
#define MV643XX_ETH_ALL_IRQS						(0x0007ffff)
#define MV643XX_ETH_KNOWN_IRQS						(0x00000c05)
#define MV643XX_ETH_IRQ_EXT_ENA						(1<<1)
#define MV643XX_ETH_IRQ_RX_DONE						(1<<2)
#define MV643XX_ETH_IRQ_RX_NO_DESC					(1<<10)

#define MV643XX_ETH_INTERRUPT_EXTEND_CAUSE_R(port)	(0x2464 + ((port)<<10))
/* not fully understood; TX seems to raise 0x0001 and link change is 0x00010000
 * if there are no buffers
 */
#define MV643XX_ETH_ALL_EXT_IRQS					(0x0011ffff)
#define MV643XX_ETH_KNOWN_EXT_IRQS					(0x00010101)
#define MV643XX_ETH_EXT_IRQ_TX_DONE					(1<<0)
#define MV643XX_ETH_EXT_IRQ_LINK_CHG				(1<<16)
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
		unsigned		odrops;					/* no counter in core code                   */
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

/* stuff needed for bsdnet support */
struct mveth_bsdsupp {
	int				oif_flags;					/* old / cached if_flags */
};

struct mveth_softc {
	struct arpcom			arpcom;
	struct mveth_bsdsupp	bsd;
	struct mveth_private	pvt;
};

/* GLOBAL VARIABLES */
#ifdef MVETH_DEBUG_TX_DUMP
int mveth_tx_dump = 0;
#endif

/* THE array of driver/bsdnet structs */

/* If detaching/module unloading is enabled, the main driver data
 * structure must remain in memory; hence it must reside in its own
 * 'dummy' module...
 */
#ifdef  MVETH_DETACH_HACK
extern
#else
STATIC
#endif
struct mveth_softc theMvEths[MV643XXETH_NUM_DRIVER_SLOTS]
#ifndef MVETH_DETACH_HACK
= {{{{0}},}}
#endif
;

/* daemon task id */
STATIC rtems_id	mveth_tid = 0;
/* register access protection mutex */
STATIC rtems_id mveth_mtx = 0;
#define REGLOCK()	do { \
		if ( RTEMS_SUCCESSFUL != rtems_semaphore_obtain(mveth_mtx, RTEMS_WAIT, RTEMS_NO_TIMEOUT) ) \
			rtems_panic(DRVNAME": unable to lock register protection mutex"); \
		} while (0)
#define REGUNLOCK()	rtems_semaphore_release(mveth_mtx)

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

static void mveth_isr(rtems_irq_hdl_param unit);
static void mveth_isr_1(rtems_irq_hdl_param unit);
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

/* MII Ioctl Interface */

STATIC unsigned
mveth_mii_read(struct mveth_private *mp, unsigned addr);

STATIC unsigned
mveth_mii_write(struct mveth_private *mp, unsigned addr, unsigned v);


/* mdio / mii interface wrappers for rtems_mii_ioctl API */

static int mveth_mdio_r(int phy, void *uarg, unsigned reg, uint32_t *pval)
{
	if ( phy > 1 )
		return -1;

	*pval = mveth_mii_read(uarg, reg);
	return 0;
}

static int mveth_mdio_w(int phy, void *uarg, unsigned reg, uint32_t val)
{
	if ( phy > 1 )
		return -1;
	mveth_mii_write(uarg, reg, val);
	return 0;
}

static struct rtems_mdio_info mveth_mdio = {
	mdio_r:	  mveth_mdio_r,
	mdio_w:	  mveth_mdio_w,
	has_gmii: 1,
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
typedef char maxbuf[2048]; /* more than an ethernet packet */
	asm volatile("":::"memory");
	len = _ALIGN(len, RX_BUF_ALIGNMENT);
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
			mp->irq_mask  = 0;
			mp->xirq_mask = 0;
		}
#endif
		/* luckily, the extended and 'normal' interrupts we use don't overlap so
		 * we can just OR them into a single word
		 */
		return  (xe & mp->xirq_mask) | (x & mp->irq_mask);
}

static void mveth_isr(rtems_irq_hdl_param arg)
{
unsigned unit = (unsigned)arg;
	mveth_disable_irqs(&theMvEths[unit].pvt, -1);
	theMvEths[unit].pvt.stats.irqs++;
	rtems_event_send( theMvEths[unit].pvt.tid, 1<<unit );
}

static void mveth_isr_1(rtems_irq_hdl_param arg)
{
unsigned              unit = (unsigned)arg;
struct mveth_private *mp   = &theMvEths[unit].pvt;

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

/* PHY/MII Interface
 *
 * Read/write a PHY register;
 * 
 * NOTE: The SMI register is shared among the three devices.
 *       Protection is provided by the global networking semaphore.
 *       If non-bsd drivers are running on a subset of IFs proper
 *       locking of all shared registers must be implemented!
 */
STATIC unsigned
mveth_mii_read(struct mveth_private *mp, unsigned addr)
{
unsigned v;
unsigned wc = 0;

	addr  &= 0x1f;

	/* wait until not busy */
	do {
		v = MV_READ(MV643XX_ETH_SMI_R);
		wc++;
	} while ( MV643XX_ETH_SMI_BUSY & v );

	MV_WRITE(MV643XX_ETH_SMI_R, (addr <<21 ) | (mp->phy<<16) | MV643XX_ETH_SMI_OP_RD );

	do {
		v = MV_READ(MV643XX_ETH_SMI_R);
		wc++;
	} while ( MV643XX_ETH_SMI_BUSY & v );

	if (wc>0xffff)
		wc = 0xffff;
	return (wc<<16) | (v & 0xffff);
}

STATIC unsigned
mveth_mii_write(struct mveth_private *mp, unsigned addr, unsigned v)
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

	MV_WRITE(MV643XX_ETH_SMI_R, (addr <<21 ) | (mp->phy<<16) | MV643XX_ETH_SMI_OP_WR | v );

	return wc;
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

/* update serial port settings from current link status */
static void
mveth_update_serial_port(struct mveth_private *mp, int media)
{
int port = mp->port_num;
uint32_t old, new;

	new = old = MV_READ(MV643XX_ETH_SERIAL_CONTROL_R(port));

	/* mask speed and duplex settings */
	new &= ~(  MV643XX_ETH_SET_GMII_SPEED_1000
			 | MV643XX_ETH_SET_MII_SPEED_100
			 | MV643XX_ETH_SET_FULL_DUPLEX );

	if ( IFM_FDX & media )
		new |= MV643XX_ETH_SET_FULL_DUPLEX;

	switch ( IFM_SUBTYPE(media) ) {
		default: /* treat as 10 */
			break;
		case IFM_100_TX:
			new |= MV643XX_ETH_SET_MII_SPEED_100;
			break;
		case IFM_1000_T:
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
}

/* Clear multicast filters                        */
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

/* Assign values (buffer + user data) to a tx descriptor slot */
static int
mveth_assign_desc(MvEthTxDesc d, struct mbuf *m, unsigned long extra)
{
int rval = (d->byte_cnt = m->m_len);

#ifdef MVETH_TESTING
	assert( !d->mb      );
	assert(  m->m_len   );
#endif

	/* set CRC on all descriptors; seems to be necessary */
	d->cmd_sts  = extra | (TDESC_GEN_CRC | TDESC_ZERO_PAD);

#ifdef ENABLE_TX_WORKAROUND_8_BYTE_PROBLEM
	/* The buffer must be 64bit aligned if the payload is <8 (??) */
	if ( rval < 8 && ((mtod(m, uintptr_t)) & 7) ) {
		d->buf_ptr = CPUADDR2ENET( d->workaround );
		memcpy((void*)d->workaround, mtod(m, void*), rval);
	} else
#endif
	{
		d->buf_ptr  = CPUADDR2ENET( mtod(m, unsigned long) );
	}
	d->l4i_chk  = 0;
	return rval;
}

static int
mveth_assign_desc_raw(MvEthTxDesc d, void *buf, int len, unsigned long extra)
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

static struct mveth_private *
mve_setup_internal(
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
struct ifnet         *ifp;
int                  InstallISRSuccessful;

	if ( unit <= 0 || unit > MV643XXETH_NUM_DRIVER_SLOTS ) {
		printk(DRVNAME": Bad unit number %i; must be 1..%i\n", unit, MV643XXETH_NUM_DRIVER_SLOTS);
		return 0;
	}
	ifp = &theMvEths[unit-1].arpcom.ac_if;
	if ( ifp->if_init ) {
		if ( ifp->if_init ) {
			printk(DRVNAME": instance %i already attached.\n", unit);
			return 0;
		}
	}

	if ( rx_ring_size < 0 && tx_ring_size < 0 )
		return 0;

	if ( MV_64360 != BSP_getDiscoveryVersion(0) ) {
		printk(DRVNAME": not mv64360 chip\n");
		return 0;
	}

	/* lazy init of mutex (non thread-safe! - we assume 1st initialization is single-threaded) */
	if ( ! mveth_mtx ) {
		rtems_status_code sc;
		sc = rtems_semaphore_create(
				rtems_build_name('m','v','e','X'),
				1,
				RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY | RTEMS_DEFAULT_ATTRIBUTES,
				0,
				&mveth_mtx);
		if ( RTEMS_SUCCESSFUL != sc ) {
			rtems_error(sc,DRVNAME": creating mutex\n");
			rtems_panic("unable to proceed\n");
		}
	}

	mp = &theMvEths[unit-1].pvt;

	memset(mp, 0, sizeof(*mp));

	mp->port_num          = unit-1;
	mp->phy               = (MV_READ(MV643XX_ETH_PHY_ADDR_R) >> (5*mp->port_num)) & 0x1f;

	mp->tid               = tid;
	mp->isr               = isr;
	mp->isr_arg           = isr_arg;

	mp->cleanup_txbuf     = cleanup_txbuf;
	mp->cleanup_txbuf_arg = cleanup_txbuf_arg;
	mp->alloc_rxbuf       = alloc_rxbuf;
	mp->consume_rxbuf     = consume_rxbuf;
	mp->consume_rxbuf_arg = consume_rxbuf_arg;

	mp->rbuf_count = rx_ring_size ? rx_ring_size : MV643XX_RX_RING_SIZE;
	mp->xbuf_count = tx_ring_size ? tx_ring_size : MV643XX_TX_RING_SIZE;

	if ( mp->xbuf_count > 0 )
		mp->xbuf_count += TX_NUM_TAG_SLOTS;

	if ( mp->rbuf_count < 0 )
		mp->rbuf_count = 0;
	if ( mp->xbuf_count < 0 )
		mp->xbuf_count = 0;

	/* allocate ring area; add 1 entry -- room for alignment */
	assert( !mp->ring_area );
	mp->ring_area = malloc(
							sizeof(*mp->ring_area) *
								(mp->rbuf_count + mp->xbuf_count + 1),
							M_DEVBUF,
							M_WAIT );
	assert( mp->ring_area );

	BSP_mve_stop_hw(mp);

	if ( irq_mask ) {
		irq_data[mp->port_num].hdl = tid ? mveth_isr : mveth_isr_1;	
		InstallISRSuccessful = BSP_install_rtems_irq_handler( &irq_data[mp->port_num] );
		assert( InstallISRSuccessful );
	}

	/* mark as used */
	ifp->if_init = (void*)(-1);

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

struct mveth_private *
BSP_mve_setup(
	int		 unit,
	rtems_id tid,
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
	if ( irq_mask && 0 == tid ) {
		printk(DRVNAME": must supply a TID if irq_msk not zero\n");
		return 0;	
	}

	return mve_setup_internal(
				unit,
				tid,
				0, 0,
				cleanup_txbuf, cleanup_txbuf_arg,
				alloc_rxbuf,
				consume_rxbuf, consume_rxbuf_arg,
				rx_ring_size, tx_ring_size,
				irq_mask);
}

struct mveth_private *
BSP_mve_setup_1(
	int		 unit,
	void     (*isr)(void *isr_arg),
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
	if ( irq_mask && 0 == isr ) {
		printk(DRVNAME": must supply an ISR if irq_msk not zero\n");
		return 0;	
	}

	return mve_setup_internal(
				unit,
				0,
				isr, isr_arg,
				cleanup_txbuf, cleanup_txbuf_arg,
				alloc_rxbuf,
				consume_rxbuf, consume_rxbuf_arg,
				rx_ring_size, tx_ring_size,
				irq_mask);
}

rtems_id
BSP_mve_get_tid(struct mveth_private *mp)
{
    return mp->tid;
}

int
BSP_mve_detach(struct mveth_private *mp)
{
int unit = mp->port_num;
	BSP_mve_stop_hw(mp);
	if ( mp->irq_mask || mp->xirq_mask ) {
		if ( !BSP_remove_rtems_irq_handler( &irq_data[mp->port_num] ) )
			return -1;
	}
	free( (void*)mp->ring_area, M_DEVBUF );
	memset(mp, 0, sizeof(*mp));
	__asm__ __volatile__("":::"memory");
	/* mark as unused */
	theMvEths[unit].arpcom.ac_if.if_init = 0;
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

	return rval;
}

/* allocate a new cluster and copy an existing chain there;
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

/* Enqueue a mbuf chain or a raw data buffer for transmission;
 * RETURN: #bytes sent or -1 if there are not enough descriptors
 *
 * If 'len' is <=0 then 'm_head' is assumed to point to a mbuf chain.
 * OTOH, a raw data packet may be send (non-BSD driver) by pointing
 * m_head to the start of the data and passing 'len' > 0.
 *
 * Comments: software cache-flushing incurs a penalty if the
 *           packet cannot be queued since it is flushed anyways.
 *           The algorithm is slightly more efficient in the normal
 *			 case, though.
 */
int
BSP_mve_send_buf(struct mveth_private *mp, void *m_head, void *data_p, int len)
{
int						rval;
register MvEthTxDesc	l,d,h;
register struct mbuf	*m1;
int						nmbs;
int						ismbuf = (len <= 0);

/* Only way to get here is when we discover that the mbuf chain
 * is too long for the tx ring
 */
startover:

	rval = 0;

#ifdef MVETH_TESTING 
	assert(m_head);
#endif

	/* if no descriptor is available; try to wipe the queue */
	if ( (mp->avail < 1) && MVETH_CLEAN_ON_SEND(mp)<=0 ) {
		/* Maybe TX is stalled and needs to be restarted */
		mveth_start_tx(mp);
		return -1;
	}

	h = mp->d_tx_h;

#ifdef MVETH_TESTING 
	assert( !h->buf_ptr );
	assert( !h->mb      );
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

	/* Don't use the first descriptor yet because BSP_mve_swipe_tx()
	 * needs mp->d_tx_h->buf_ptr == NULL as a marker. Hence, we
	 * start with the second mbuf and fill the first descriptor
	 * last.
	 */

	l = h;
	d = NEXT_TXD(h);

	mp->avail--;

	nmbs = 1;
	if ( ismbuf ) {
			register struct mbuf *m;
			for ( m=m1->m_next; m; m=m->m_next ) {
					if ( 0 == m->m_len )
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
									assert( l->mb == 0 );
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
									if ( ! (m_head = repackage_chain(m_head)) ) {
											/* no cluster available */
											mp->stats.odrops++;
											return 0;
									}
									goto startover;
							}
							return -1;
					}

					mp->avail--;

#ifdef MVETH_TESTING
					assert( d != h      );
					assert( !d->buf_ptr );
#endif

					/* fill this slot */
					rval += mveth_assign_desc(d, m, TDESC_DMA_OWNED);

					FLUSH_BUF(mtod(m, uint32_t), m->m_len);

					l = d;
					d = NEXT_TXD(d);

					FLUSH_DESC(l);
			}

		/* fill first slot - don't release to DMA yet */
		rval += mveth_assign_desc(h, m1, TDESC_FRST);


		FLUSH_BUF(mtod(m1, uint32_t), m1->m_len);

	} else {
		/* fill first slot with raw buffer - don't release to DMA yet */
		rval += mveth_assign_desc_raw(h, data_p, len, TDESC_FRST);

		FLUSH_BUF( (uint32_t)data_p, len);
	}

	/* tag last slot; this covers the case where 1st==last */
	l->cmd_sts      |= TDESC_LAST | TDESC_INT_ENA;
	/* mbuf goes into last desc */
	l->u_buf         = m_head;


	FLUSH_DESC(l);

	/* Tag end; make sure chip doesn't try to read ahead of here! */
	l->next->cmd_sts = 0;
	FLUSH_DESC(l->next);

#ifdef MVETH_DEBUG_TX_DUMP
	if ( (mveth_tx_dump & (1<<mp->port_num)) ) {
		int ll,kk;
		if ( ismbuf ) {
			struct mbuf *m;
			for ( kk=0, m=m_head; m; m=m->m_next) {
				for ( ll=0; ll<m->m_len; ll++ ) {
					printf("%02X ",*(mtod(m,char*) + ll));
					if ( ((++kk)&0xf) == 0 )
						printf("\n");
				}
			}
		} else {
			for ( ll=0; ll<len; ) {
				printf("%02X ",*((char*)data_p + ll));
				if ( ((++ll)&0xf) == 0 )
					printf("\n");
			}	
		}
		printf("\n");
	}
#endif

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

	rval = 0;

#ifdef MVETH_TESTING 
	assert(header || data);
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
	assert( !h->mb      );
#endif

	/* find the 'first' user buffer */
	if ( (frst_buf = head_p) ) {
		frst_len = h_len;
	} else {
		frst_buf = data_p;
		frst_len = d_len;
	}

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
		rval += mveth_assign_desc_raw(d, data_p, d_len, TDESC_DMA_OWNED);
		FLUSH_BUF( (uint32_t)data_p, d_len );
		d->u_buf = data_p;

		l = d;
		d = NEXT_TXD(d);

		FLUSH_DESC(l);
	}

	/* fill first slot with raw buffer - don't release to DMA yet */
	rval       += mveth_assign_desc_raw(h, frst_buf, frst_len, TDESC_FRST);

	FLUSH_BUF( (uint32_t)frst_buf, frst_len);

	/* tag last slot; this covers the case where 1st==last */
	l->cmd_sts |= TDESC_LAST | TDESC_INT_ENA;

	/* first buffer of 'chain' goes into last desc */
	l->u_buf    = frst_buf;

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
			mp->consume_rxbuf(0, mp->consume_rxbuf_arg, err ? -1 : 0);
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
BSP_mve_init_hw(struct mveth_private *mp, int promisc, unsigned char *enaddr)
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
		mp->rx_ring = (MvEthRxDesc)_ALIGN(mp->ring_area, RING_ALIGNMENT);
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

	i = IFM_MAKEWORD(0, 0, 0, 0);
	if ( 0 == BSP_mve_media_ioctl(mp, SIOCGIFMEDIA, &i) ) {
	    if ( (IFM_LINK_OK & i) ) {
			mveth_update_serial_port(mp, i);
		}
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

int
BSP_mve_media_ioctl(struct mveth_private *mp, int cmd, int *parg)
{
int rval;
	/* alias cmd == 0,1 */
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
	rval = rtems_mii_ioctl(&mveth_mdio, mp, cmd, parg);
	REGUNLOCK();
	return rval;
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

int
BSP_mve_ack_link_chg(struct mveth_private *mp, int *pmedia)
{
int media = IFM_MAKEWORD(0,0,0,0);

	if ( 0 == BSP_mve_media_ioctl(mp, SIOCGIFMEDIA, &media)) {
		if ( IFM_LINK_OK & media ) {
			mveth_update_serial_port(mp, media);
			/* If TX stalled because there was no buffer then whack it */
			mveth_start_tx(mp);
		}
		if ( pmedia )
			*pmedia = media;
		return 0;
	}
	return -1;
}

/* BSDNET SUPPORT/GLUE ROUTINES */

static void
mveth_set_filters(struct ifnet *ifp);

STATIC void
mveth_stop(struct mveth_softc *sc)
{
	BSP_mve_stop_hw(&sc->pvt);
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
	l = _ALIGN(o, RX_BUF_ALIGNMENT) - o;

	/* align start of buffer */
	m->m_data += l;

	/* reduced length */
	l = MCLBYTES - l;

	m->m_len   = m->m_pkthdr.len = l;
	*psz       = m->m_len;
	*paddr     = mtod(m, uintptr_t); 

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

			if (0) {
				/* Low-level debugging/testing without bsd stack */
				m_freem(m);
			} else {
				/* send buffer upwards */
				ether_input(ifp, eh, m);
			}
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

static void
dump_update_stats(struct mveth_private *mp, FILE *f)
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

void
BSP_mve_dump_stats(struct mveth_private *mp, FILE *f)
{
	dump_update_stats(mp, f);
}

/* BSDNET DRIVER CALLBACKS */

static void
mveth_init(void *arg)
{
struct mveth_softc	*sc  = arg;
struct ifnet		*ifp = &sc->arpcom.ac_if;
int                 media;

	BSP_mve_init_hw(&sc->pvt, ifp->if_flags & IFF_PROMISC, sc->arpcom.ac_enaddr);

	media = IFM_MAKEWORD(0, 0, 0, 0);
	if ( 0 == BSP_mve_media_ioctl(&sc->pvt, SIOCGIFMEDIA, &media) ) {
	    if ( (IFM_LINK_OK & media) ) {
			ifp->if_flags &= ~IFF_OACTIVE;
		} else {
			ifp->if_flags |= IFF_OACTIVE;
		}
	}

	/* if promiscuous then there is no need to change */
	if ( ! (ifp->if_flags & IFF_PROMISC) )
		mveth_set_filters(ifp);

	ifp->if_flags |= IFF_RUNNING;
	sc->arpcom.ac_if.if_timer = 0;
}

/* bsdnet driver entry to start transmission */
static void
mveth_start(struct ifnet *ifp)
{
struct mveth_softc	*sc = ifp->if_softc;
struct mbuf			*m  = 0;

	while ( ifp->if_snd.ifq_head ) {
		IF_DEQUEUE( &ifp->if_snd, m );
		if ( BSP_mve_send_buf(&sc->pvt, m, 0, 0) < 0 ) {
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
mveth_watchdog(struct ifnet *ifp)
{
struct mveth_softc	*sc = ifp->if_softc;

	ifp->if_oerrors++;
	printk(DRVNAME"%i: watchdog timeout; resetting\n", ifp->if_unit);

	mveth_init(sc);
	mveth_start(ifp);
}

static void
mveth_set_filters(struct ifnet *ifp)
{
struct mveth_softc  *sc = ifp->if_softc;
uint32_t              v;

	v = MV_READ(MV643XX_ETH_PORT_CONFIG_R(sc->pvt.port_num));
	if ( ifp->if_flags & IFF_PROMISC )
		v |= MV643XX_ETH_UNICAST_PROMISC_MODE;
	else
		v &= ~MV643XX_ETH_UNICAST_PROMISC_MODE;
	MV_WRITE(MV643XX_ETH_PORT_CONFIG_R(sc->pvt.port_num), v);

	if ( ifp->if_flags & (IFF_PROMISC | IFF_ALLMULTI) ) {
		BSP_mve_mcast_filter_accept_all(&sc->pvt);
	} else {
		struct ether_multi     *enm;
		struct ether_multistep step;

		BSP_mve_mcast_filter_clear( &sc->pvt );
		
		ETHER_FIRST_MULTI(step, (struct arpcom *)ifp, enm);

		while ( enm ) {
			if ( memcmp(enm->enm_addrlo, enm->enm_addrhi, ETHER_ADDR_LEN) )
				assert( !"Should never get here; IFF_ALLMULTI should be set!" );

			BSP_mve_mcast_filter_accept_add(&sc->pvt, enm->enm_addrlo);

			ETHER_NEXT_MULTI(step, enm);
		}
	}
}

/* bsdnet driver ioctl entry */
static int
mveth_ioctl(struct ifnet *ifp, ioctl_command_t cmd, caddr_t data)
{
struct mveth_softc	*sc   = ifp->if_softc;
struct ifreq		*ifr  = (struct ifreq *)data;
int					error = 0;
int					f;

	switch ( cmd ) {
  		case SIOCSIFFLAGS:
			f = ifp->if_flags;
			if ( f & IFF_UP ) {
				if ( ! ( f & IFF_RUNNING ) ) {
					mveth_init(sc);
				} else {
					if ( (f & IFF_PROMISC) != (sc->bsd.oif_flags & IFF_PROMISC) ) {
						/* Note: in all other scenarios the 'promisc' flag
						 * in the low-level driver [which affects the way
						 * the multicast filter is setup: accept none vs.
						 * accept all in promisc mode] is eventually
						 * set when the IF is brought up...
						 */
						sc->pvt.promisc = (f & IFF_PROMISC);

						mveth_set_filters(ifp);
					}
					/* FIXME: other flag changes are ignored/unimplemented */
				}
			} else {
				if ( f & IFF_RUNNING ) {
					mveth_stop(sc);
					ifp->if_flags  &= ~(IFF_RUNNING | IFF_OACTIVE);
				}
			}
			sc->bsd.oif_flags = ifp->if_flags;
		break;

  		case SIOCGIFMEDIA:
  		case SIOCSIFMEDIA:
			error = BSP_mve_media_ioctl(&sc->pvt, cmd, &ifr->ifr_media);
		break;
 
		case SIOCADDMULTI:
		case SIOCDELMULTI:
			error = (cmd == SIOCADDMULTI)
		    		? ether_addmulti(ifr, &sc->arpcom)
				    : ether_delmulti(ifr, &sc->arpcom);

			if (error == ENETRESET) {
				if (ifp->if_flags & IFF_RUNNING) {
					mveth_set_filters(ifp);
				}
				error = 0;
			}
		break;


		break;

 		case SIO_RTEMS_SHOW_STATS:
			dump_update_stats(&sc->pvt, stdout);
		break;

		default:
			error = ether_ioctl(ifp, cmd, data);
		break;
	}

	return error;
}

/* DRIVER TASK */

/* Daemon task does all the 'interrupt' work */
static void mveth_daemon(void *arg)
{
struct mveth_softc	*sc;
struct ifnet		*ifp;
rtems_event_set		evs;
	for (;;) {
		rtems_bsdnet_event_receive( 7, RTEMS_WAIT | RTEMS_EVENT_ANY, RTEMS_NO_TIMEOUT, &evs );
		evs &= 7;
		for ( sc = theMvEths; evs; evs>>=1, sc++ ) {
			if ( (evs & 1) ) {
				register uint32_t x;

				ifp = &sc->arpcom.ac_if;

				if ( !(ifp->if_flags & IFF_UP) ) {
					mveth_stop(sc);
					ifp->if_flags &= ~(IFF_UP|IFF_RUNNING);
					continue;
				}

				if ( !(ifp->if_flags & IFF_RUNNING) ) {
					/* event could have been pending at the time hw was stopped;
					 * just ignore...
					 */
					continue;
				}

				x = mveth_ack_irqs(&sc->pvt, -1);

				if ( MV643XX_ETH_EXT_IRQ_LINK_CHG & x ) {
					/* phy status changed */
					int media;

					if ( 0 == BSP_mve_ack_link_chg(&sc->pvt, &media) ) {
						if ( IFM_LINK_OK & media ) {
							ifp->if_flags &= ~IFF_OACTIVE;
							mveth_start(ifp);
						} else {
							/* stop sending */
							ifp->if_flags |= IFF_OACTIVE;
						}
					}
				}
				/* free tx chain */
				if ( (MV643XX_ETH_EXT_IRQ_TX_DONE & x) && BSP_mve_swipe_tx(&sc->pvt) ) {
					ifp->if_flags &= ~IFF_OACTIVE;
					if ( TX_AVAILABLE_RING_SIZE(&sc->pvt) == sc->pvt.avail )
						ifp->if_timer = 0;
					mveth_start(ifp);
				}
				if ( (MV643XX_ETH_IRQ_RX_DONE & x) )
					BSP_mve_swipe_rx(&sc->pvt);

				mveth_enable_irqs(&sc->pvt, -1);
			}
		}
	}
}

#ifdef  MVETH_DETACH_HACK
static int mveth_detach(struct mveth_softc *sc);
#endif


/* PUBLIC RTEMS BSDNET ATTACH FUNCTION */
int
rtems_mve_attach(struct rtems_bsdnet_ifconfig *ifcfg, int attaching)
{
char				*unitName;
int					unit,i,cfgUnits;
struct	mveth_softc *sc;
struct	ifnet		*ifp;

	unit = rtems_bsdnet_parse_driver_name(ifcfg, &unitName);
	if ( unit <= 0 || unit > MV643XXETH_NUM_DRIVER_SLOTS ) {
		printk(DRVNAME": Bad unit number %i; must be 1..%i\n", unit, MV643XXETH_NUM_DRIVER_SLOTS);
		return 1;
	}

	sc  = &theMvEths[unit-1];
	ifp = &sc->arpcom.ac_if;
	sc->pvt.port_num = unit-1;
	sc->pvt.phy      = (MV_READ(MV643XX_ETH_PHY_ADDR_R) >> (5*sc->pvt.port_num)) & 0x1f;

	if ( attaching ) {
		if ( ifp->if_init ) {
			printk(DRVNAME": instance %i already attached.\n", unit);
			return -1;
		}

		for ( i=cfgUnits = 0; i<MV643XXETH_NUM_DRIVER_SLOTS; i++ ) {
			if ( theMvEths[i].arpcom.ac_if.if_init )
				cfgUnits++;
		}
		cfgUnits++; /* this new one */

		/* lazy init of TID should still be thread-safe because we are protected
		 * by the global networking semaphore..
		 */
		if ( !mveth_tid ) {
			/* newproc uses the 1st 4 chars of name string to build an rtems name */
			mveth_tid = rtems_bsdnet_newproc("MVEd", 4096, mveth_daemon, 0);
		}

		if ( !BSP_mve_setup( unit,
						     mveth_tid,
						     release_tx_mbuf, ifp,
						     alloc_mbuf_rx,
						     consume_rx_mbuf, ifp,
						     ifcfg->rbuf_count,
						     ifcfg->xbuf_count,
			                 BSP_MVE_IRQ_TX | BSP_MVE_IRQ_RX | BSP_MVE_IRQ_LINK) ) {
			return -1;
		}

		if ( nmbclusters < sc->pvt.rbuf_count * cfgUnits + 60 /* arbitrary */ )  {
			printk(DRVNAME"%i: (mv643xx ethernet) Your application has not enough mbuf clusters\n", unit);
			printk(     "                         configured for this driver.\n");
			return -1;
		}

		if ( ifcfg->hardware_address ) {
			memcpy(sc->arpcom.ac_enaddr, ifcfg->hardware_address, ETHER_ADDR_LEN);
		} else {
			/* read back from hardware assuming that MotLoad already had set it up */
			BSP_mve_read_eaddr(&sc->pvt, sc->arpcom.ac_enaddr);
		}

		ifp->if_softc			= sc;
		ifp->if_unit			= unit;
		ifp->if_name			= unitName;

		ifp->if_mtu				= ifcfg->mtu ? ifcfg->mtu : ETHERMTU;

		ifp->if_init			= mveth_init;
		ifp->if_ioctl			= mveth_ioctl;
		ifp->if_start			= mveth_start;
		ifp->if_output			= ether_output;
		/*
		 * While nonzero, the 'if->if_timer' is decremented
		 * (by the networking code) at a rate of IFNET_SLOWHZ (1hz) and 'if_watchdog'
		 * is called when it expires. 
		 * If either of those fields is 0 the feature is disabled.
		 */
		ifp->if_watchdog		= mveth_watchdog;
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
		ifp->if_snd.ifq_maxlen	= sc->pvt.xbuf_count;
		*/
		ifp->if_snd.ifq_maxlen	= ifqmaxlen;

#ifdef  MVETH_DETACH_HACK
		if ( !ifp->if_addrlist ) /* do only the first time [reattach hack] */
#endif
		{
			if_attach(ifp);
			ether_ifattach(ifp);
		}

	} else {
#ifdef  MVETH_DETACH_HACK
		if ( !ifp->if_init ) {
			printk(DRVNAME": instance %i not attached.\n", unit);
			return -1;
		}
		return mveth_detach(sc);
#else
		printk(DRVNAME": interface detaching not implemented\n");
		return -1;
#endif
	}

	return 0;
}

/* EARLY PHY ACCESS */
static int
mveth_early_init(int idx)
{
	if ( idx < 0 || idx >= MV643XXETH_NUM_DRIVER_SLOTS )
		return -1;

	/* determine the phy */
	theMvEths[idx].pvt.phy = (MV_READ(MV643XX_ETH_PHY_ADDR_R) >> (5*idx)) & 0x1f;
	return 0;
}

static int
mveth_early_read_phy(int idx, unsigned reg)
{
int rval;

	if ( idx < 0 || idx >= MV643XXETH_NUM_DRIVER_SLOTS )
		return -1;

	rval = mveth_mii_read(&theMvEths[idx].pvt, reg);
	return rval < 0 ? rval : rval & 0xffff;
}

static int
mveth_early_write_phy(int idx, unsigned reg, unsigned val)
{
	if ( idx < 0 || idx >= MV643XXETH_NUM_DRIVER_SLOTS )
		return -1;

	mveth_mii_write(&theMvEths[idx].pvt, reg, val);
	return 0;
}

rtems_bsdnet_early_link_check_ops
rtems_mve_early_link_check_ops = {
	init:		mveth_early_init,
	read_phy:	mveth_early_read_phy,
	write_phy:	mveth_early_write_phy,
	name:		DRVNAME,
	num_slots:	MAX_NUM_SLOTS
};

/* DEBUGGING */

#ifdef MVETH_DEBUG
/* Display/dump descriptor rings */

int
mveth_dring(struct mveth_softc *sc)
{
int i;
if (1) {
MvEthRxDesc pr;
printf("RX:\n");

	for (i=0, pr=sc->pvt.rx_ring; i<sc->pvt.rbuf_count; i++, pr++) {
#ifndef ENABLE_HW_SNOOPING
		/* can't just invalidate the descriptor - if it contains
		 * data that hasn't been flushed yet, we create an inconsistency...
		 */
		rtems_bsdnet_semaphore_obtain();
		INVAL_DESC(pr);
#endif
		printf("cnt: 0x%04x, size: 0x%04x, stat: 0x%08x, next: 0x%08x, buf: 0x%08x\n",
			pr->byte_cnt, pr->buf_size, pr->cmd_sts, (uint32_t)pr->next_desc_ptr, pr->buf_ptr);

#ifndef ENABLE_HW_SNOOPING
		rtems_bsdnet_semaphore_release();
#endif
	}
}
if (1) {
MvEthTxDesc pt;
printf("TX:\n");
	for (i=0, pt=sc->pvt.tx_ring; i<sc->pvt.xbuf_count; i++, pt++) {
#ifndef ENABLE_HW_SNOOPING
		rtems_bsdnet_semaphore_obtain();
		INVAL_DESC(pt);
#endif
		printf("cnt: 0x%04x, stat: 0x%08x, next: 0x%08x, buf: 0x%08x, mb: 0x%08x\n",
			pt->byte_cnt, pt->cmd_sts, (uint32_t)pt->next_desc_ptr, pt->buf_ptr,
			(uint32_t)pt->mb);

#ifndef ENABLE_HW_SNOOPING
		rtems_bsdnet_semaphore_release();
#endif
	}
}
	return 0;
}

#endif

/* DETACH HACK DETAILS */

#ifdef  MVETH_DETACH_HACK
int
_cexpModuleFinalize(void *mh)
{
int i;
	for ( i=0; i<MV643XXETH_NUM_DRIVER_SLOTS; i++ ) {
		if ( theMvEths[i].arpcom.ac_if.if_init ) {
			printf("Interface %i still attached; refuse to unload\n", i+1);
			return -1;
		}
	}
	/* delete task; since there are no attached interfaces, it should block
	 * for events and hence not hold the semaphore or other resources...
	 */
	rtems_task_delete(mveth_tid);
	return 0;
}

/* ugly hack to allow unloading/reloading the driver core.
 * needed because rtems' bsdnet release doesn't implement
 * if_detach(). Therefore, we bring the interface down but
 * keep the device record alive...
 */
static void
ether_ifdetach_pvt(struct ifnet *ifp)
{
        ifp->if_flags = 0;
        ifp->if_ioctl = 0;
        ifp->if_start = 0;
        ifp->if_watchdog = 0;
        ifp->if_init  = 0;
}

static int
mveth_detach(struct mveth_softc *sc)
{
struct ifnet	*ifp = &sc->arpcom.ac_if;
	if ( ifp->if_init ) {
		if ( ifp->if_flags & (IFF_UP | IFF_RUNNING) ) {
			printf(DRVNAME"%i: refuse to detach; interface still up\n",sc->pvt.port_num+1);
			return -1;
		}
		mveth_stop(sc);
/* not implemented in BSDnet/RTEMS (yet) but declared in header */
#define ether_ifdetach ether_ifdetach_pvt
		ether_ifdetach(ifp);
	}
	free( (void*)sc->pvt.ring_area, M_DEVBUF );
	sc->pvt.ring_area = 0;
	sc->pvt.tx_ring   = 0;
	sc->pvt.rx_ring   = 0;
	sc->pvt.d_tx_t    = sc->pvt.d_tx_h   = 0;
	sc->pvt.d_rx_t    = 0;
	sc->pvt.avail     = 0;
	/* may fail if ISR was not installed yet */
	BSP_remove_rtems_irq_handler( &irq_data[sc->pvt.port_num] );
	return 0;
}

#ifdef MVETH_DEBUG
struct rtems_bsdnet_ifconfig mveth_dbg_config = {
	name:				DRVNAME"1",
	attach:				rtems_mve_attach,
	ip_address:			"192.168.2.10",		/* not used by rtems_bsdnet_attach */
	ip_netmask:			"255.255.255.0",	/* not used by rtems_bsdnet_attach */
	hardware_address:	0, /* (void *) */
	ignore_broadcast:	0,					/* TODO driver should honour this  */
	mtu:				0,
	rbuf_count:			0,					/* TODO driver should honour this  */
	xbuf_count:			0,					/* TODO driver should honour this  */
};
#endif
#endif
