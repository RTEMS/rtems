/*
 *  RTEMS driver for Etherlink based Ethernet Controllers
 *
 *  Copyright (C) 2003, Gregory Menke, NASA/GSFC
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 * elnk.c
 *
 *
 */


/*
 * Portions of this driver are taken from the Freebsd if_xl.c driver,
 * version "1.133 2003/03/19 01:48:14" and are covered by the license
 * text included below that was taken verbatim from the original file.
 * More particularly, all structures, variables, and #defines prefixed
 * with XL_ or xl_, along with their associated comments were taken
 * directly from the Freebsd driver and modified as required to suit the
 * purposes of this one.  Additionally, much of the device setup &
 * manipulation logic was also copied and modified to suit.  All types
 * and functions beginning with elnk are either my own creations or were
 * adapted from other RTEMS components, and regardless, are subject to
 * the standard OAR licensing terms given in the comments at the top of
 * this file.
 *
 * Greg Menke, 6/11/2003
 */

 /*
 * Copyright (c) 1997, 1998, 1999
 *	Bill Paul <wpaul@ctr.columbia.edu>.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Bill Paul.
 * 4. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Bill Paul AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Bill Paul OR THE VOICES IN HIS HEAD
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <rtems.h>

/*
 *  This driver only supports architectures with the new style
 *  exception processing.  The following checks try to keep this
 *  from being compiled on systems which can't support this driver.
 */

#if defined(__i386__)
#define ELNK_SUPPORTED
  #define PCI_DRAM_OFFSET 0
#endif

#if defined(__PPC__)
#define ELNK_SUPPORTED
#endif

#include <bsp.h>

#if !defined(PCI_DRAM_OFFSET)
  #undef ELNK_SUPPORTED
#endif

/* #undef ELNK_SUPPORTED */


#if defined(ELNK_SUPPORTED)
#include <rtems/pci.h>

#if defined(__PPC__)
#include <libcpu/byteorder.h>
#include <libcpu/io.h>
#endif

#if defined(__i386__)
#include <libcpu/byteorder.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <rtems/error.h>
#include <rtems/bspIo.h>
#include <rtems/rtems_bsdnet.h>

#include <sys/param.h>
#include <sys/mbuf.h>

#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>

#include <net/if_media.h>
#include <dev/mii/mii.h>
#include <bsp/irq.h>

#if defined(__i386__)
#define IO_MASK   0x3
#define MEM_MASK  0xF
#endif

#ifdef malloc
#undef malloc
#endif
#ifdef free
#undef free
#endif

#define ELNK_DEBUG


#define DRIVER_PREFIX   "elnk"





/*
* These buffers allocated for each unit, so ensure
*
*   rtems_bsdnet_config.mbuf_bytecount
*   rtems_bsdnet_config.mbuf_cluster_bytecount
*
* are adequately sized to provide enough clusters and mbufs for all the
* units.  The default bsdnet configuration is sufficient for one unit,
* but will be nearing exhaustion with 2 or more.  Although a little
* expensive in memory, the following configuration should eliminate all
* mbuf/cluster issues;
*
*   rtems_bsdnet_config.mbuf_bytecount           = 128*1024;
*   rtems_bsdnet_config.mbuf_cluster_bytecount   = 256*1024;
*
* The default size in buffers of the rx & tx rings are given below.
* This driver honors the rtems_bsdnet_ifconfig fields 'rbuf_count' and
* 'xbuf_count', allowing the user to specify something else.
*/

#define RX_RING_SIZE 16	/* default number of receive buffers */
#define TX_RING_SIZE 16	/* default number of transmit buffers */


/*
 * Number of boards supported by this driver
 */
#define NUM_UNITS	8

/*
 * Receive buffer size -- Allow for a full ethernet packet including CRC
 */
#define XL_PACKET_SIZE	1540


/*
** Events, one per unit.  The event is sent to the rx task from the isr
** or from the stack to the tx task whenever a unit needs service.  The
** rx/tx tasks identify the requesting unit(s) by their particular
** events so only requesting units are serviced.
*/

static rtems_event_set unit_signals[NUM_UNITS]= { RTEMS_EVENT_1,
                                                  RTEMS_EVENT_2,
                                                  RTEMS_EVENT_3,
                                                  RTEMS_EVENT_4,
                                                  RTEMS_EVENT_5,
                                                  RTEMS_EVENT_6,
                                                  RTEMS_EVENT_7,
                                                  RTEMS_EVENT_8 };




#if defined(__PPC__)
#define phys_to_bus(address) ((unsigned int)((address)) + PCI_DRAM_OFFSET)
#define bus_to_phys(address) ((unsigned int)((address)) - PCI_DRAM_OFFSET)
#define CPU_CACHE_ALIGNMENT_FOR_BUFFER PPC_CACHE_ALIGNMENT
#else
extern void Wait_X_ms( unsigned int timeToWait );
#define phys_to_bus(address) ((unsigned int) ((address)))
#define bus_to_phys(address) ((unsigned int) ((address)))
#define rtems_bsp_delay_in_bus_cycles(cycle) Wait_X_ms( cycle/100 )
#define CPU_CACHE_ALIGNMENT_FOR_BUFFER PG_SIZE
#endif

/* the actual duration waited in DELAY is not especially predictable,
 * though it will be consistent on a given host.  It should not be
 * relied upon for specific timing given the vague per-bsp,
 * per-architecture implementation of the actual delay function.  It
 * would probably be helpful to make this more accurate at some point...
 */
#define DELAY(n)        rtems_bsp_delay_in_bus_cycles( n*20  )




/*
 * Register layouts.
 */
#define XL_COMMAND		0x0E
#define XL_STATUS		0x0E

#define XL_TX_STATUS		0x1B
#define XL_TX_FREE		0x1C
#define XL_DMACTL		0x20
#define XL_DOWNLIST_PTR		0x24
#define XL_DOWN_POLL		0x2D /* 3c90xB only */
#define XL_TX_FREETHRESH	0x2F
#define XL_UPLIST_PTR		0x38
#define XL_UPLIST_STATUS	0x30
#define XL_UP_POLL		0x3D /* 3c90xB only */

#define XL_PKTSTAT_UP_STALLED		0x00002000
#define XL_PKTSTAT_UP_ERROR		0x00004000
#define XL_PKTSTAT_UP_CMPLT		0x00008000

#define XL_DMACTL_DN_CMPLT_REQ		0x00000002
#define XL_DMACTL_DOWN_STALLED		0x00000004
#define XL_DMACTL_UP_CMPLT		0x00000008
#define XL_DMACTL_DOWN_CMPLT		0x00000010
#define XL_DMACTL_UP_RX_EARLY		0x00000020
#define XL_DMACTL_ARM_COUNTDOWN		0x00000040
#define XL_DMACTL_DOWN_INPROG		0x00000080
#define XL_DMACTL_COUNTER_SPEED		0x00000100
#define XL_DMACTL_DOWNDOWN_MODE		0x00000200
#define XL_DMACTL_TARGET_ABORT		0x40000000
#define XL_DMACTL_MASTER_ABORT		0x80000000

/*
 * Command codes. Some command codes require that we wait for
 * the CMD_BUSY flag to clear. Those codes are marked as 'mustwait.'
 */
#define XL_CMD_RESET		0x0000	/* mustwait */
#define XL_CMD_WINSEL		0x0800
#define XL_CMD_COAX_START	0x1000
#define XL_CMD_RX_DISABLE	0x1800
#define XL_CMD_RX_ENABLE	0x2000
#define XL_CMD_RX_RESET		0x2800	/* mustwait */
#define XL_CMD_UP_STALL		0x3000	/* mustwait */
#define XL_CMD_UP_UNSTALL	0x3001
#define XL_CMD_DOWN_STALL	0x3002	/* mustwait */
#define XL_CMD_DOWN_UNSTALL	0x3003
#define XL_CMD_RX_DISCARD	0x4000
#define XL_CMD_TX_ENABLE	0x4800
#define XL_CMD_TX_DISABLE	0x5000
#define XL_CMD_TX_RESET		0x5800	/* mustwait */
#define XL_CMD_INTR_FAKE	0x6000
#define XL_CMD_INTR_ACK		0x6800
#define XL_CMD_INTR_ENB		0x7000
#define XL_CMD_STAT_ENB		0x7800
#define XL_CMD_RX_SET_FILT	0x8000
#define XL_CMD_RX_SET_THRESH	0x8800
#define XL_CMD_TX_SET_THRESH	0x9000
#define XL_CMD_TX_SET_START	0x9800
#define XL_CMD_DMA_UP		0xA000
#define XL_CMD_DMA_STOP		0xA001
#define XL_CMD_STATS_ENABLE	0xA800
#define XL_CMD_STATS_DISABLE	0xB000
#define XL_CMD_COAX_STOP	0xB800

#define XL_CMD_SET_TX_RECLAIM	0xC000 /* 3c905B only */
#define XL_CMD_RX_SET_HASH	0xC800 /* 3c905B only */

#define XL_HASH_SET		0x0400
#define XL_HASHFILT_SIZE	256

/*
 * status codes
 * Note that bits 15 to 13 indicate the currently visible register window
 * which may be anything from 0 to 7.
 */
#define XL_STAT_INTLATCH	0x0001	/* 0 */
#define XL_STAT_ADFAIL		0x0002	/* 1 */
#define XL_STAT_TX_COMPLETE	0x0004	/* 2 */
#define XL_STAT_TX_AVAIL	0x0008	/* 3 first generation */
#define XL_STAT_RX_COMPLETE	0x0010  /* 4 */
#define XL_STAT_RX_EARLY	0x0020	/* 5 */
#define XL_STAT_INTREQ		0x0040  /* 6 */
#define XL_STAT_STATSOFLOW	0x0080  /* 7 */
#define XL_STAT_DMADONE		0x0100	/* 8 first generation */
#define XL_STAT_LINKSTAT	0x0100	/* 8 3c509B */
#define XL_STAT_DOWN_COMPLETE	0x0200	/* 9 */
#define XL_STAT_UP_COMPLETE	0x0400	/* 10 */
#define XL_STAT_DMABUSY		0x0800	/* 11 first generation */
#define XL_STAT_CMDBUSY		0x1000  /* 12 */

/*
 * Interrupts we normally want enabled.
 */
#define XL_INTRS	\
                  (XL_STAT_UP_COMPLETE | XL_STAT_STATSOFLOW | XL_STAT_ADFAIL|	\
	           XL_STAT_DOWN_COMPLETE | XL_STAT_TX_COMPLETE | XL_STAT_INTLATCH)



/*
 * General constants that are fun to know.
 *
 * 3Com PCI vendor ID
 */
#define	TC_VENDORID		0x10B7

/*
 * 3Com chip device IDs.
 */
#define	TC_DEVICEID_BOOMERANG_10BT		0x9000
#define TC_DEVICEID_BOOMERANG_10BT_COMBO	0x9001
#define TC_DEVICEID_BOOMERANG_10_100BT		0x9050
#define TC_DEVICEID_BOOMERANG_100BT4		0x9051
#define TC_DEVICEID_KRAKATOA_10BT		0x9004
#define TC_DEVICEID_KRAKATOA_10BT_COMBO		0x9005
#define TC_DEVICEID_KRAKATOA_10BT_TPC		0x9006
#define TC_DEVICEID_CYCLONE_10FL		0x900A
#define TC_DEVICEID_HURRICANE_10_100BT		0x9055
#define TC_DEVICEID_CYCLONE_10_100BT4		0x9056
#define TC_DEVICEID_CYCLONE_10_100_COMBO	0x9058
#define TC_DEVICEID_CYCLONE_10_100FX		0x905A
#define TC_DEVICEID_TORNADO_10_100BT		0x9200
#define TC_DEVICEID_TORNADO_10_100BT_920B	0x9201
#define TC_DEVICEID_HURRICANE_10_100BT_SERV	0x9800
#define TC_DEVICEID_TORNADO_10_100BT_SERV	0x9805
#define TC_DEVICEID_HURRICANE_SOHO100TX		0x7646
#define TC_DEVICEID_TORNADO_HOMECONNECT		0x4500
#define TC_DEVICEID_HURRICANE_555		0x5055
#define TC_DEVICEID_HURRICANE_556		0x6055
#define TC_DEVICEID_HURRICANE_556B		0x6056
#define TC_DEVICEID_HURRICANE_575A		0x5057
#define TC_DEVICEID_HURRICANE_575B		0x5157
#define TC_DEVICEID_HURRICANE_575C		0x5257
#define TC_DEVICEID_HURRICANE_656		0x6560
#define TC_DEVICEID_HURRICANE_656B		0x6562
#define TC_DEVICEID_TORNADO_656C		0x6564



#define XL_RXSTAT_LENMASK	0x00001FFF
#define XL_RXSTAT_UP_ERROR	0x00004000
#define XL_RXSTAT_UP_CMPLT	0x00008000
#define XL_RXSTAT_UP_OVERRUN	0x00010000
#define XL_RXSTAT_RUNT		0x00020000
#define XL_RXSTAT_ALIGN		0x00040000
#define XL_RXSTAT_CRC		0x00080000
#define XL_RXSTAT_OVERSIZE	0x00100000
#define XL_RXSTAT_DRIBBLE	0x00800000
#define XL_RXSTAT_UP_OFLOW	0x01000000
#define XL_RXSTAT_IPCKERR	0x02000000	/* 3c905B only */
#define XL_RXSTAT_TCPCKERR	0x04000000	/* 3c905B only */
#define XL_RXSTAT_UDPCKERR	0x08000000	/* 3c905B only */
#define XL_RXSTAT_BUFEN		0x10000000	/* 3c905B only */
#define XL_RXSTAT_IPCKOK	0x20000000	/* 3c905B only */
#define XL_RXSTAT_TCPCOK	0x40000000	/* 3c905B only */
#define XL_RXSTAT_UDPCKOK	0x80000000	/* 3c905B only */

#define XL_TXSTAT_LENMASK	0x00001FFF
#define XL_TXSTAT_CRCDIS	0x00002000
#define XL_TXSTAT_TX_INTR	0x00008000
#define XL_TXSTAT_DL_COMPLETE	0x00010000
#define XL_TXSTAT_IPCKSUM	0x02000000	/* 3c905B only */
#define XL_TXSTAT_TCPCKSUM	0x04000000	/* 3c905B only */
#define XL_TXSTAT_UDPCKSUM	0x08000000	/* 3c905B only */
#define XL_TXSTAT_RND_DEFEAT	0x10000000	/* 3c905B only */
#define XL_TXSTAT_EMPTY		0x20000000	/* 3c905B only */
#define XL_TXSTAT_DL_INTR	0x80000000


#define XL_FLAG_FUNCREG			0x0001
#define XL_FLAG_PHYOK			0x0002
#define XL_FLAG_EEPROM_OFFSET_30	0x0004
#define XL_FLAG_WEIRDRESET		0x0008
#define XL_FLAG_8BITROM			0x0010
#define XL_FLAG_INVERT_LED_PWR		0x0020
#define XL_FLAG_INVERT_MII_PWR		0x0040
#define XL_FLAG_NO_XCVR_PWR		0x0080
#define XL_FLAG_USE_MMIO		0x0100



#define XL_EE_READ	0x0080	/* read, 5 bit address */
#define XL_EE_WRITE	0x0040	/* write, 5 bit address */
#define XL_EE_ERASE	0x00c0	/* erase, 5 bit address */
#define XL_EE_EWEN	0x0030	/* erase, no data needed */
#define XL_EE_8BIT_READ	0x0200	/* read, 8 bit address */
#define XL_EE_BUSY	0x8000

#define XL_EE_EADDR0	0x00	/* station address, first word */
#define XL_EE_EADDR1	0x01	/* station address, next word, */
#define XL_EE_EADDR2	0x02	/* station address, last word */
#define XL_EE_PRODID	0x03	/* product ID code */
#define XL_EE_MDATA_DATE	0x04	/* manufacturing data, date */
#define XL_EE_MDATA_DIV		0x05	/* manufacturing data, division */
#define XL_EE_MDATA_PCODE	0x06	/* manufacturing data, product code */
#define XL_EE_MFG_ID	0x07
#define XL_EE_PCI_PARM	0x08
#define XL_EE_ROM_ONFO	0x09
#define XL_EE_OEM_ADR0	0x0A
#define	XL_EE_OEM_ADR1	0x0B
#define XL_EE_OEM_ADR2	0x0C
#define XL_EE_SOFTINFO1	0x0D
#define XL_EE_COMPAT	0x0E
#define XL_EE_SOFTINFO2	0x0F
#define XL_EE_CAPS	0x10	/* capabilities word */
#define XL_EE_RSVD0	0x11
#define XL_EE_ICFG_0	0x12
#define XL_EE_ICFG_1	0x13
#define XL_EE_RSVD1	0x14
#define XL_EE_SOFTINFO3	0x15
#define XL_EE_RSVD_2	0x16

/*
 * Bits in the capabilities word
 */
#define XL_CAPS_PNP		0x0001
#define XL_CAPS_FULL_DUPLEX	0x0002
#define XL_CAPS_LARGE_PKTS	0x0004
#define XL_CAPS_SLAVE_DMA	0x0008
#define XL_CAPS_SECOND_DMA	0x0010
#define XL_CAPS_FULL_BM		0x0020
#define XL_CAPS_FRAG_BM		0x0040
#define XL_CAPS_CRC_PASSTHRU	0x0080
#define XL_CAPS_TXDONE		0x0100
#define XL_CAPS_NO_TXLENGTH	0x0200
#define XL_CAPS_RX_REPEAT	0x0400
#define XL_CAPS_SNOOPING	0x0800
#define XL_CAPS_100MBPS		0x1000
#define XL_CAPS_PWRMGMT		0x2000



/*
 * Window 0 registers
 */
#define XL_W0_EE_DATA		0x0C
#define XL_W0_EE_CMD		0x0A
#define XL_W0_RSRC_CFG		0x08
#define XL_W0_ADDR_CFG		0x06
#define XL_W0_CFG_CTRL		0x04

#define XL_W0_PROD_ID		0x02
#define XL_W0_MFG_ID		0x00

/*
 * Window 1
 */

#define XL_W1_TX_FIFO		0x10

#define XL_W1_FREE_TX		0x0C
#define XL_W1_TX_STATUS		0x0B
#define XL_W1_TX_TIMER		0x0A
#define XL_W1_RX_STATUS		0x08
#define XL_W1_RX_FIFO		0x00

/*
 * RX status codes
 */
#define XL_RXSTATUS_OVERRUN	0x01
#define XL_RXSTATUS_RUNT	0x02
#define XL_RXSTATUS_ALIGN	0x04
#define XL_RXSTATUS_CRC		0x08
#define XL_RXSTATUS_OVERSIZE	0x10
#define XL_RXSTATUS_DRIBBLE	0x20

/*
 * TX status codes
 */
#define XL_TXSTATUS_RECLAIM	0x02 /* 3c905B only */
#define XL_TXSTATUS_OVERFLOW	0x04
#define XL_TXSTATUS_MAXCOLS	0x08
#define XL_TXSTATUS_UNDERRUN	0x10
#define XL_TXSTATUS_JABBER	0x20
#define XL_TXSTATUS_INTREQ	0x40
#define XL_TXSTATUS_COMPLETE	0x80

/*
 * Window 2
 */
#define XL_W2_RESET_OPTIONS	0x0C	/* 3c905B only */
#define XL_W2_STATION_MASK_HI	0x0A
#define XL_W2_STATION_MASK_MID	0x08
#define XL_W2_STATION_MASK_LO	0x06
#define XL_W2_STATION_ADDR_HI	0x04
#define XL_W2_STATION_ADDR_MID	0x02
#define XL_W2_STATION_ADDR_LO	0x00

#define XL_RESETOPT_FEATUREMASK	0x0001|0x0002|0x004
#define XL_RESETOPT_D3RESETDIS	0x0008
#define XL_RESETOPT_DISADVFD	0x0010
#define XL_RESETOPT_DISADV100	0x0020
#define XL_RESETOPT_DISAUTONEG	0x0040
#define XL_RESETOPT_DEBUGMODE	0x0080
#define XL_RESETOPT_FASTAUTO	0x0100
#define XL_RESETOPT_FASTEE	0x0200
#define XL_RESETOPT_FORCEDCONF	0x0400
#define XL_RESETOPT_TESTPDTPDR	0x0800
#define XL_RESETOPT_TEST100TX	0x1000
#define XL_RESETOPT_TEST100RX	0x2000

#define XL_RESETOPT_INVERT_LED	0x0010
#define XL_RESETOPT_INVERT_MII	0x4000

/*
 * Window 3 (fifo management)
 */
#define XL_W3_INTERNAL_CFG	0x00
#define XL_W3_MAXPKTSIZE	0x04    /* 3c905B only */
#define XL_W3_RESET_OPT		0x08
#define XL_W3_FREE_TX		0x0C
#define XL_W3_FREE_RX		0x0A
#define XL_W3_MAC_CTRL		0x06

#define XL_ICFG_CONNECTOR_MASK	0x00F00000
#define XL_ICFG_CONNECTOR_BITS	20

#define XL_ICFG_RAMSIZE_MASK	0x00000007
#define XL_ICFG_RAMWIDTH	0x00000008
#define XL_ICFG_ROMSIZE_MASK	(0x00000040|0x00000080)
#define XL_ICFG_DISABLE_BASSD	0x00000100
#define XL_ICFG_RAMLOC		0x00000200
#define XL_ICFG_RAMPART		(0x00010000|0x00020000)
#define XL_ICFG_XCVRSEL		(0x00100000|0x00200000|0x00400000)
#define XL_ICFG_AUTOSEL		0x01000000

#define XL_XCVR_10BT		0x00
#define XL_XCVR_AUI		0x01
#define XL_XCVR_RSVD_0		0x02
#define XL_XCVR_COAX		0x03
#define XL_XCVR_100BTX		0x04
#define XL_XCVR_100BFX		0x05
#define XL_XCVR_MII		0x06
#define XL_XCVR_RSVD_1		0x07
#define XL_XCVR_AUTO		0x08	/* 3c905B only */

#define XL_MACCTRL_DEFER_EXT_END	0x0001
#define XL_MACCTRL_DEFER_0		0x0002
#define XL_MACCTRL_DEFER_1		0x0004
#define XL_MACCTRL_DEFER_2		0x0008
#define XL_MACCTRL_DEFER_3		0x0010
#define XL_MACCTRL_DUPLEX		0x0020
#define XL_MACCTRL_ALLOW_LARGE_PACK	0x0040
#define XL_MACCTRL_EXTEND_AFTER_COL	0x0080 (3c905B only)
#define XL_MACCTRL_FLOW_CONTROL_ENB	0x0100 (3c905B only)
#define XL_MACCTRL_VLT_END		0x0200 (3c905B only)

/*
 * The 'reset options' register contains power-on reset values
 * loaded from the EEPROM. This includes the supported media
 * types on the card. It is also known as the media options register.
 */
#define XL_W3_MEDIA_OPT		0x08

#define XL_MEDIAOPT_BT4		0x0001	/* MII */
#define XL_MEDIAOPT_BTX		0x0002	/* on-chip */
#define XL_MEDIAOPT_BFX		0x0004	/* on-chip */
#define XL_MEDIAOPT_BT		0x0008	/* on-chip */
#define XL_MEDIAOPT_BNC		0x0010	/* on-chip */
#define XL_MEDIAOPT_AUI		0x0020	/* on-chip */
#define XL_MEDIAOPT_MII		0x0040	/* MII */
#define XL_MEDIAOPT_VCO		0x0100	/* 1st gen chip only */

#define XL_MEDIAOPT_10FL	0x0100	/* 3x905B only, on-chip */
#define XL_MEDIAOPT_MASK	0x01FF

/*
 * Window 4 (diagnostics)
 */
#define XL_W4_UPPERBYTESOK	0x0D
#define XL_W4_BADSSD		0x0C
#define XL_W4_MEDIA_STATUS	0x0A
#define XL_W4_PHY_MGMT		0x08
#define XL_W4_NET_DIAG		0x06
#define XL_W4_FIFO_DIAG		0x04
#define XL_W4_VCO_DIAG		0x02

#define XL_W4_CTRLR_STAT	0x08
#define XL_W4_TX_DIAG		0x00

#define XL_MII_CLK		0x01
#define XL_MII_DATA		0x02
#define XL_MII_DIR		0x04

#define XL_MEDIA_SQE		0x0008
#define XL_MEDIA_10TP		0x00C0
#define XL_MEDIA_LNK		0x0080
#define XL_MEDIA_LNKBEAT	0x0800

#define XL_MEDIASTAT_CRCSTRIP	0x0004
#define XL_MEDIASTAT_SQEENB	0x0008
#define XL_MEDIASTAT_COLDET	0x0010
#define XL_MEDIASTAT_CARRIER	0x0020
#define XL_MEDIASTAT_JABGUARD	0x0040
#define XL_MEDIASTAT_LINKBEAT	0x0080
#define XL_MEDIASTAT_JABDETECT	0x0200
#define XL_MEDIASTAT_POLREVERS	0x0400
#define XL_MEDIASTAT_LINKDETECT	0x0800
#define XL_MEDIASTAT_TXINPROG	0x1000
#define XL_MEDIASTAT_DCENB	0x4000
#define XL_MEDIASTAT_AUIDIS	0x8000

#define XL_NETDIAG_TEST_LOWVOLT		0x0001
#define XL_NETDIAG_ASIC_REVMASK		(0x0002|0x0004|0x0008|0x0010|0x0020)
#define XL_NETDIAG_UPPER_BYTES_ENABLE	0x0040
#define XL_NETDIAG_STATS_ENABLED	0x0080
#define XL_NETDIAG_TX_FATALERR		0x0100
#define XL_NETDIAG_TRANSMITTING		0x0200
#define XL_NETDIAG_RX_ENABLED		0x0400
#define XL_NETDIAG_TX_ENABLED		0x0800
#define XL_NETDIAG_FIFO_LOOPBACK	0x1000
#define XL_NETDIAG_MAC_LOOPBACK		0x2000
#define XL_NETDIAG_ENDEC_LOOPBACK	0x4000
#define XL_NETDIAG_EXTERNAL_LOOP	0x8000

/*
 * Window 5
 */
#define XL_W5_STAT_ENB		0x0C
#define XL_W5_INTR_ENB		0x0A
#define XL_W5_RECLAIM_THRESH	0x09	/* 3c905B only */
#define XL_W5_RX_FILTER		0x08
#define XL_W5_RX_EARLYTHRESH	0x06
#define XL_W5_TX_AVAILTHRESH	0x02
#define XL_W5_TX_STARTTHRESH	0x00

/*
 * RX filter bits
 */
#define XL_RXFILTER_INDIVIDUAL	0x01
#define XL_RXFILTER_ALLMULTI	0x02
#define XL_RXFILTER_BROADCAST	0x04
#define XL_RXFILTER_ALLFRAMES	0x08
#define XL_RXFILTER_MULTIHASH	0x10 /* 3c905B only */

/*
 * Window 6 (stats)
 */
#define XL_W6_TX_BYTES_OK	0x0C
#define XL_W6_RX_BYTES_OK	0x0A
#define XL_W6_UPPER_FRAMES_OK	0x09
#define XL_W6_DEFERRED		0x08
#define XL_W6_RX_OK		0x07
#define XL_W6_TX_OK		0x06
#define XL_W6_RX_OVERRUN	0x05
#define XL_W6_COL_LATE		0x04
#define XL_W6_COL_SINGLE	0x03
#define XL_W6_COL_MULTIPLE	0x02
#define XL_W6_SQE_ERRORS	0x01
#define XL_W6_CARRIER_LOST	0x00

/*
 * Window 7 (bus master control)
 */
#define XL_W7_BM_ADDR		0x00
#define XL_W7_BM_LEN		0x06
#define XL_W7_BM_STATUS		0x0B
#define XL_W7_BM_TIMEr		0x0A

/*
 * bus master control registers
 */
#define XL_BM_PKTSTAT		0x20
#define XL_BM_DOWNLISTPTR	0x24
#define XL_BM_FRAGADDR		0x28
#define XL_BM_FRAGLEN		0x2C
#define XL_BM_TXFREETHRESH	0x2F
#define XL_BM_UPPKTSTAT		0x30
#define XL_BM_UPLISTPTR		0x38








struct xl_mii_frame {
	u_int8_t		mii_stdelim;
	u_int8_t		mii_opcode;
	u_int8_t		mii_phyaddr;
	u_int8_t		mii_regaddr;
	u_int8_t		mii_turnaround;
	u_int16_t		mii_data;
};

/*
 * MII constants
 */
#define XL_MII_STARTDELIM	0x01
#define XL_MII_READOP		0x02
#define XL_MII_WRITEOP		0x01
#define XL_MII_TURNAROUND	0x02



/*
 * The 3C905B adapters implement a few features that we want to
 * take advantage of, namely the multicast hash filter. With older
 * chips, you only have the option of turning on reception of all
 * multicast frames, which is kind of lame.
 *
 * We also use this to decide on a transmit strategy. For the 3c90xB
 * cards, we can use polled descriptor mode, which reduces CPU overhead.
 */
#define XL_TYPE_905B	1
#define XL_TYPE_90X	2

#define XL_FLAG_FUNCREG			0x0001
#define XL_FLAG_PHYOK			0x0002
#define XL_FLAG_EEPROM_OFFSET_30	0x0004
#define XL_FLAG_WEIRDRESET		0x0008
#define XL_FLAG_8BITROM			0x0010
#define XL_FLAG_INVERT_LED_PWR		0x0020
#define XL_FLAG_INVERT_MII_PWR		0x0040
#define XL_FLAG_NO_XCVR_PWR		0x0080
#define XL_FLAG_USE_MMIO		0x0100

#define XL_NO_XCVR_PWR_MAGICBITS	0x0900


#define XL_MIN_FRAMELEN		60

#define XL_LAST_FRAG		0x80000000







struct xl_stats
{
      /* accumulated stats */
      u_int16_t		xl_carrier_lost;
      u_int16_t		xl_sqe_errs;
      u_int16_t		xl_tx_multi_collision;
      u_int16_t		xl_tx_single_collision;
      u_int16_t		xl_tx_late_collision;
      u_int16_t		xl_rx_overrun;
      u_int16_t		xl_tx_deferred;

      u_int32_t		xl_rx_bytes_ok;
      u_int32_t		xl_tx_bytes_ok;

      u_int32_t		xl_tx_frames_ok;
      u_int32_t		xl_rx_frames_ok;

      u_int16_t		xl_badssd;

      /* non-accumulated stats */
      u_int16_t		intstatus;
      u_int16_t		rxstatus;
      u_int8_t		txstatus;
      u_int16_t         mediastatus;

      u_int32_t         txcomplete_ints;

      u_int16_t         miianr, miipar, miistatus, miicmd;

      u_int32_t         device_interrupts;
      u_int32_t         internalconfig;
      u_int16_t         mac_control;

      u_int16_t         smbstatus;
      u_int32_t         dmactl;
      u_int16_t         txfree;
};



struct xl_type
{
      u_int16_t		xl_vid;
      u_int16_t		xl_did;
      char		*xl_name;
};



/*
 * Various supported device vendors/types and their names.
 */
static struct xl_type xl_devs[] = {
	{ TC_VENDORID, TC_DEVICEID_BOOMERANG_10BT,
		"3Com 3c900-TPO Etherlink XL" },
	{ TC_VENDORID, TC_DEVICEID_BOOMERANG_10BT_COMBO,
		"3Com 3c900-COMBO Etherlink XL" },
	{ TC_VENDORID, TC_DEVICEID_BOOMERANG_10_100BT,
		"3Com 3c905-TX Fast Etherlink XL" },
	{ TC_VENDORID, TC_DEVICEID_BOOMERANG_100BT4,
		"3Com 3c905-T4 Fast Etherlink XL" },
	{ TC_VENDORID, TC_DEVICEID_KRAKATOA_10BT,
		"3Com 3c900B-TPO Etherlink XL" },
	{ TC_VENDORID, TC_DEVICEID_KRAKATOA_10BT_COMBO,
		"3Com 3c900B-COMBO Etherlink XL" },
	{ TC_VENDORID, TC_DEVICEID_KRAKATOA_10BT_TPC,
		"3Com 3c900B-TPC Etherlink XL" },
	{ TC_VENDORID, TC_DEVICEID_CYCLONE_10FL,
		"3Com 3c900B-FL Etherlink XL" },
	{ TC_VENDORID, TC_DEVICEID_HURRICANE_10_100BT,
		"3Com 3c905B-TX Fast Etherlink XL" },
	{ TC_VENDORID, TC_DEVICEID_CYCLONE_10_100BT4,
		"3Com 3c905B-T4 Fast Etherlink XL" },
	{ TC_VENDORID, TC_DEVICEID_CYCLONE_10_100FX,
		"3Com 3c905B-FX/SC Fast Etherlink XL" },
	{ TC_VENDORID, TC_DEVICEID_CYCLONE_10_100_COMBO,
		"3Com 3c905B-COMBO Fast Etherlink XL" },
	{ TC_VENDORID, TC_DEVICEID_TORNADO_10_100BT,
		"3Com 3c905C-TX Fast Etherlink XL" },
	{ TC_VENDORID, TC_DEVICEID_TORNADO_10_100BT_920B,
		"3Com 3c920B-EMB Integrated Fast Etherlink XL" },
	{ TC_VENDORID, TC_DEVICEID_HURRICANE_10_100BT_SERV,
		"3Com 3c980 Fast Etherlink XL" },
	{ TC_VENDORID, TC_DEVICEID_TORNADO_10_100BT_SERV,
		"3Com 3c980C Fast Etherlink XL" },
	{ TC_VENDORID, TC_DEVICEID_HURRICANE_SOHO100TX,
		"3Com 3cSOHO100-TX OfficeConnect" },
	{ TC_VENDORID, TC_DEVICEID_TORNADO_HOMECONNECT,
		"3Com 3c450-TX HomeConnect" },
	{ TC_VENDORID, TC_DEVICEID_HURRICANE_555,
		"3Com 3c555 Fast Etherlink XL" },
	{ TC_VENDORID, TC_DEVICEID_HURRICANE_556,
		"3Com 3c556 Fast Etherlink XL" },
	{ TC_VENDORID, TC_DEVICEID_HURRICANE_556B,
		"3Com 3c556B Fast Etherlink XL" },
	{ TC_VENDORID, TC_DEVICEID_HURRICANE_575A,
		"3Com 3c575TX Fast Etherlink XL" },
	{ TC_VENDORID, TC_DEVICEID_HURRICANE_575B,
		"3Com 3c575B Fast Etherlink XL" },
	{ TC_VENDORID, TC_DEVICEID_HURRICANE_575C,
		"3Com 3c575C Fast Etherlink XL" },
	{ TC_VENDORID, TC_DEVICEID_HURRICANE_656,
		"3Com 3c656 Fast Etherlink XL" },
	{ TC_VENDORID, TC_DEVICEID_HURRICANE_656B,
		"3Com 3c656B Fast Etherlink XL" },
	{ TC_VENDORID, TC_DEVICEID_TORNADO_656C,
		"3Com 3c656C Fast Etherlink XL" },
	{ 0, 0, NULL }
};


#define XL_TIMEOUT		1000






/* rx message descriptor entry, ensure the struct is aligned to 8 bytes */
struct RXMD
{
      /* used by hardware */
      volatile uint32_t   next;
      volatile uint32_t   status;
      volatile uint32_t   addr;
      volatile uint32_t   length;
      /* used by software */
      struct mbuf       *mbuf;        /* scratch variable used in the tx ring */
      struct RXMD       *next_md;
} __attribute__ ((aligned (8), packed));





#define NUM_FRAGS       6

/*
 * tx message descriptor entry, ensure the struct is aligned to 8 bytes
 */

struct tfrag
{
      volatile uint32_t   addr;
      volatile uint32_t   length;
} __attribute__ ((packed));

struct TXMD
{
      /* used by hardware */
      volatile uint32_t   next;
      volatile uint32_t   status;
      struct tfrag        txfrags[NUM_FRAGS];
      /* used by software */
      struct mbuf       *mbuf;        /* scratch variable used in the tx ring */
      struct TXMD       *next_md, *chainptr;
} __attribute__ ((aligned (8), packed));





#define NUM_CHAIN_LENGTHS       50



/*
 * Per-device data
 */
struct elnk_softc
{
      struct arpcom		arpcom;

      rtems_irq_connect_data    irqInfo;
      rtems_event_set           ioevent;
      unsigned int 		ioaddr;

      unsigned char             *bufferBase, *ringBase;

      struct RXMD      *rx_ring, *curr_rx_md;
      struct TXMD      *tx_ring, *last_tx_md, *last_txchain_head;

      rtems_id                  stat_timer_id;
      uint32_t                  stats_update_ticks;

      struct xl_stats           xl_stats;

      u_int8_t                  xl_unit;	/* interface number */
      u_int8_t                  xl_type;
      int			xl_flags;
      u_int16_t                 xl_media;
      u_int16_t                 xl_caps;
      u_int32_t                 xl_xcvr;
      u_int8_t                  xl_stats_no_timeout;
      u_int16_t                 xl_tx_thresh;

      int                       tx_idle;

      short                     chain_lengths[NUM_CHAIN_LENGTHS];
      int                       chlenIndex;

      unsigned short            vendorID, deviceID;
      int			acceptBroadcast;
      int                       numTxbuffers, numRxbuffers;
};

static struct elnk_softc elnk_softc[NUM_UNITS];
static rtems_id	rxDaemonTid;
static rtems_id	txDaemonTid;
static rtems_id chainRecoveryQueue;







#if defined(__i386__)

#define CSR_WRITE_4(sc, reg, val)	i386_outport_long( sc->ioaddr + reg, val )
#define CSR_WRITE_2(sc, reg, val)	i386_outport_word( sc->ioaddr + reg, val )
#define CSR_WRITE_1(sc, reg, val)	i386_outport_byte( sc->ioaddr + reg, val )


inline unsigned int CSR_READ_4( struct elnk_softc *sc, int reg)
{
   unsigned int myval;
   i386_inport_long( sc->ioaddr + reg, myval );
   return myval;
}

inline unsigned short  CSR_READ_2( struct elnk_softc *sc, int reg)
{
   unsigned short myval;
   i386_inport_word( sc->ioaddr + reg, myval );
   return myval;
}

inline unsigned char CSR_READ_1( struct elnk_softc *sc, int reg)
{
   unsigned char myval;
   i386_inport_byte( sc->ioaddr + reg, myval );
   return myval;
}

#endif

#if defined(__PPC__)

#define CSR_WRITE_4(sc, reg, val)	outl( val, sc->ioaddr + reg)
#define CSR_WRITE_2(sc, reg, val)	outw( val, sc->ioaddr + reg)
#define CSR_WRITE_1(sc, reg, val)	outb( val, sc->ioaddr + reg)

#define CSR_READ_4(sc, reg)	        inl(sc->ioaddr + reg)
#define CSR_READ_2(sc, reg)	        inw(sc->ioaddr + reg)
#define CSR_READ_1(sc, reg)	        inb(sc->ioaddr + reg)

#endif


#define XL_SEL_WIN(x)                   CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_WINSEL | x)











/*
 * Murphy's law says that it's possible the chip can wedge and
 * the 'command in progress' bit may never clear. Hence, we wait
 * only a finite amount of time to avoid getting caught in an
 * infinite loop. Normally this delay routine would be a macro,
 * but it isn't called during normal operation so we can afford
 * to make it a function.
 */
static void
xl_wait(struct elnk_softc	*sc)
{
   register int		i;

   for(i = 0; i < XL_TIMEOUT; i++)
   {
      if (!(CSR_READ_2(sc, XL_STATUS) & XL_STAT_CMDBUSY))
         break;
   }

   if (i == XL_TIMEOUT)
      printk("etherlink : unit elnk%d command never completed\n", sc->xl_unit );
   return;
}






/*
 * MII access routines are provided for adapters with external
 * PHYs (3c905-TX, 3c905-T4, 3c905B-T4) and those with built-in
 * autoneg logic that's faked up to look like a PHY (3c905B-TX).
 * Note: if you don't perform the MDIO operations just right,
 * it's possible to end up with code that works correctly with
 * some chips/CPUs/processor speeds/bus speeds/etc but not
 * with others.
 */
#define MII_SET(x)					\
	CSR_WRITE_2(sc, XL_W4_PHY_MGMT,			\
		CSR_READ_2(sc, XL_W4_PHY_MGMT) | (x))

#define MII_CLR(x)					\
	CSR_WRITE_2(sc, XL_W4_PHY_MGMT,			\
		CSR_READ_2(sc, XL_W4_PHY_MGMT) & ~(x))

/*
 * Sync the PHYs by setting data bit and strobing the clock 32 times.
 */
static void
xl_mii_sync(
   struct elnk_softc		*sc)
{
   register int		i;

   XL_SEL_WIN(4);
   MII_SET(XL_MII_DIR|XL_MII_DATA);

   for (i = 0; i < 32; i++) {
      MII_SET(XL_MII_CLK);
      MII_SET(XL_MII_DATA);
      MII_CLR(XL_MII_CLK);
      MII_SET(XL_MII_DATA);
   }

   return;
}

/*
 * Clock a series of bits through the MII.
 */
static void
xl_mii_send(
   struct elnk_softc		*sc,
   u_int32_t		bits,
   int			cnt )
{
   int			i;

   XL_SEL_WIN(4);
   MII_CLR(XL_MII_CLK);

   for (i = (0x1 << (cnt - 1)); i; i >>= 1) {
      if (bits & i) {
         MII_SET(XL_MII_DATA);
      } else {
         MII_CLR(XL_MII_DATA);
      }
      MII_CLR(XL_MII_CLK);
      MII_SET(XL_MII_CLK);
   }
}

/*
 * Read an PHY register through the MII.
 */
static int
xl_mii_readreg(
   struct elnk_softc	*sc,
   struct xl_mii_frame	*frame )
{
   int			i, ack;

   /*
    * Set up frame for RX.
    */
   frame->mii_stdelim = XL_MII_STARTDELIM;
   frame->mii_opcode = XL_MII_READOP;
   frame->mii_turnaround = 0;
   frame->mii_data = 0;

   /*
    * Select register window 4.
    */

   XL_SEL_WIN(4);

   CSR_WRITE_2(sc, XL_W4_PHY_MGMT, 0);
   /*
    * Turn on data xmit.
    */
   MII_SET(XL_MII_DIR);

   xl_mii_sync(sc);

   /*
    * Send command/address info.
    */
   xl_mii_send(sc, frame->mii_stdelim, 2);
   xl_mii_send(sc, frame->mii_opcode, 2);
   xl_mii_send(sc, frame->mii_phyaddr, 5);
   xl_mii_send(sc, frame->mii_regaddr, 5);

   /* Idle bit */
   MII_CLR((XL_MII_CLK|XL_MII_DATA));
   MII_SET(XL_MII_CLK);

   /* Turn off xmit. */
   MII_CLR(XL_MII_DIR);

   /* Check for ack */
   MII_CLR(XL_MII_CLK);
   ack = CSR_READ_2(sc, XL_W4_PHY_MGMT) & XL_MII_DATA;
   MII_SET(XL_MII_CLK);

   /*
    * Now try reading data bits. If the ack failed, we still
    * need to clock through 16 cycles to keep the PHY(s) in sync.
    */
   if (ack) {
      for(i = 0; i < 16; i++) {
         MII_CLR(XL_MII_CLK);
         MII_SET(XL_MII_CLK);
      }
      goto fail;
   }

   for (i = 0x8000; i; i >>= 1) {
      MII_CLR(XL_MII_CLK);
      if (!ack) {
         if (CSR_READ_2(sc, XL_W4_PHY_MGMT) & XL_MII_DATA)
            frame->mii_data |= i;
      }
      MII_SET(XL_MII_CLK);
   }

  fail:

   MII_CLR(XL_MII_CLK);
   MII_SET(XL_MII_CLK);

   if (ack)
      return(1);
   return(0);
}

/*
 * Write to a PHY register through the MII.
 */
static int
xl_mii_writereg(
   struct elnk_softc	*sc,
   struct xl_mii_frame	*frame )
{
   /*
    * Set up frame for TX.
    */

   frame->mii_stdelim = XL_MII_STARTDELIM;
   frame->mii_opcode = XL_MII_WRITEOP;
   frame->mii_turnaround = XL_MII_TURNAROUND;

   /*
    * Select the window 4.
    */
   XL_SEL_WIN(4);

   /*
    * Turn on data output.
    */
   MII_SET(XL_MII_DIR);

   xl_mii_sync(sc);

   xl_mii_send(sc, frame->mii_stdelim, 2);
   xl_mii_send(sc, frame->mii_opcode, 2);
   xl_mii_send(sc, frame->mii_phyaddr, 5);
   xl_mii_send(sc, frame->mii_regaddr, 5);
   xl_mii_send(sc, frame->mii_turnaround, 2);
   xl_mii_send(sc, frame->mii_data, 16);

   /* Idle bit. */
   MII_SET(XL_MII_CLK);
   MII_CLR(XL_MII_CLK);

   /*
    * Turn off xmit.
    */
   MII_CLR(XL_MII_DIR);

   return(0);
}

static int
xl_miibus_readreg(
   struct elnk_softc	*sc,
   int			phy,
   int			reg )
{
   struct xl_mii_frame	frame;

   /*
    * Pretend that PHYs are only available at MII address 24.
    * This is to guard against problems with certain 3Com ASIC
    * revisions that incorrectly map the internal transceiver
    * control registers at all MII addresses. This can cause
    * the miibus code to attach the same PHY several times over.
    */
   if ((!(sc->xl_flags & XL_FLAG_PHYOK)) && phy != 24)
   {
      printk("etherlink : unit elnk%d xl_miibus_readreg returned\n", sc->xl_unit);
      return(0);
   }

   memset((char *)&frame, 0, sizeof(frame));

   frame.mii_phyaddr = phy;
   frame.mii_regaddr = reg;
   xl_mii_readreg(sc, &frame);

   return(frame.mii_data);
}

static int
xl_miibus_writereg(
   struct elnk_softc		*sc,
   int			phy,
   int			reg,
   int			data )
{
   struct xl_mii_frame	frame;

   if ((!(sc->xl_flags & XL_FLAG_PHYOK)) && phy != 24)
   {
      printk("etherlink : unit elnk%d xl_miibus_writereg returned\n", sc->xl_unit);
      return(0);
   }

   memset((char *)&frame, 0, sizeof(frame));

   frame.mii_phyaddr = phy;
   frame.mii_regaddr = reg;
   frame.mii_data = data;

   xl_mii_writereg(sc, &frame);

   return(0);
}









/*
 * The EEPROM is slow: give it time to come ready after issuing
 * it a command.
 */
static int
xl_eeprom_wait(struct elnk_softc *sc)
{
   int			i;

   for (i = 0; i < 100; i++) {
      if (CSR_READ_2(sc, XL_W0_EE_CMD) & XL_EE_BUSY)
         DELAY(162);
      else
         break;
   }

   if (i == 100) {
      printk("etherlink : unit elnk%d eeprom failed to come ready\n", sc->xl_unit);
      return(1);
   }

   return(0);
}

/*
 * Read a sequence of words from the EEPROM. Note that ethernet address
 * data is stored in the EEPROM in network byte order.
 */
static int
xl_read_eeprom(
   struct elnk_softc		*sc,
   caddr_t			dest,
   int			off,
   int			cnt,
   int			swap)
{
   int			err = 0, i;
   u_int16_t		word = 0, *ptr;
#define EEPROM_5BIT_OFFSET(A) ((((A) << 2) & 0x7F00) | ((A) & 0x003F))
#define EEPROM_8BIT_OFFSET(A) ((A) & 0x003F)
   /* WARNING! DANGER!
    * It's easy to accidentally overwrite the rom content!
    * Note: the 3c575 uses 8bit EEPROM offsets.
    */
   XL_SEL_WIN(0);

   if (xl_eeprom_wait(sc))
      return(1);

   if (sc->xl_flags & XL_FLAG_EEPROM_OFFSET_30)
      off += 0x30;

   for (i = 0; i < cnt; i++) {
      if (sc->xl_flags & XL_FLAG_8BITROM)
         CSR_WRITE_2(sc, XL_W0_EE_CMD,
                     XL_EE_8BIT_READ | EEPROM_8BIT_OFFSET(off + i));
      else
         CSR_WRITE_2(sc, XL_W0_EE_CMD,
                     XL_EE_READ | EEPROM_5BIT_OFFSET(off + i));
      err = xl_eeprom_wait(sc);
      if (err)
         break;
      word = CSR_READ_2(sc, XL_W0_EE_DATA);
      ptr = (u_int16_t*)(dest + (i * 2));
      if (swap)
         *ptr = ntohs(word);
      else
         *ptr = word;
   }

   return(err ? 1 : 0);
}




static void
xl_stats_update(
   rtems_id timerid,
   void *xsc)
{
   struct elnk_softc	*sc = (struct elnk_softc *)xsc;
   struct ifnet         *ifp = &sc->arpcom.ac_if;
   u_int32_t            t1;

   sc->xl_stats.intstatus = CSR_READ_2(sc, XL_STATUS);

   sc->xl_stats.miianr    = xl_miibus_readreg(sc, 0x18, MII_ANAR );
   sc->xl_stats.miipar    = xl_miibus_readreg(sc, 0x18, MII_ANLPAR );
   sc->xl_stats.miistatus = xl_miibus_readreg(sc, 0x18, MII_BMSR );
   sc->xl_stats.miicmd    = xl_miibus_readreg(sc, 0x18, MII_BMCR );

   XL_SEL_WIN(1);
   sc->xl_stats.rxstatus  = CSR_READ_2(sc, XL_W1_RX_STATUS );
   sc->xl_stats.txstatus  = CSR_READ_1(sc, XL_W1_TX_STATUS );
   sc->xl_stats.smbstatus = CSR_READ_2(sc, 2 );

   XL_SEL_WIN(3);
   sc->xl_stats.internalconfig = CSR_READ_4(sc, XL_W3_INTERNAL_CFG);
   sc->xl_stats.mac_control    = CSR_READ_2(sc, XL_W3_MAC_CTRL);
   sc->xl_stats.txfree         = CSR_READ_2(sc, XL_W3_FREE_TX );


   /* Read all the stats registers. */
   XL_SEL_WIN(6);

   sc->xl_stats.xl_carrier_lost              += CSR_READ_1(sc, XL_W6_CARRIER_LOST);
   sc->xl_stats.xl_sqe_errs                  += CSR_READ_1(sc, XL_W6_SQE_ERRORS);
   sc->xl_stats.xl_tx_multi_collision        += CSR_READ_1(sc, XL_W6_COL_MULTIPLE);
   sc->xl_stats.xl_tx_single_collision       += CSR_READ_1(sc, XL_W6_COL_SINGLE);
   sc->xl_stats.xl_tx_late_collision         += CSR_READ_1(sc, XL_W6_COL_LATE);
   sc->xl_stats.xl_rx_overrun                += CSR_READ_1(sc, XL_W6_RX_OVERRUN);
   sc->xl_stats.xl_tx_deferred               += CSR_READ_1(sc, XL_W6_DEFERRED);

   sc->xl_stats.xl_tx_frames_ok              += CSR_READ_1(sc, XL_W6_TX_OK);
   sc->xl_stats.xl_rx_frames_ok              += CSR_READ_1(sc, XL_W6_RX_OK);

   sc->xl_stats.xl_rx_bytes_ok               += CSR_READ_2(sc, XL_W6_TX_BYTES_OK );
   sc->xl_stats.xl_tx_bytes_ok               += CSR_READ_2(sc, XL_W6_RX_BYTES_OK );

   t1 = CSR_READ_1(sc, XL_W6_UPPER_FRAMES_OK);
   sc->xl_stats.xl_rx_frames_ok +=  ((t1 & 0x3) << 8);
   sc->xl_stats.xl_tx_frames_ok +=  (((t1 >> 4) & 0x3) << 8);


   ifp->if_ierrors += sc->xl_stats.xl_rx_overrun;

   ifp->if_collisions += sc->xl_stats.xl_tx_multi_collision +
      sc->xl_stats.xl_tx_single_collision +
      sc->xl_stats.xl_tx_late_collision;

   /*
    * Boomerang and cyclone chips have an extra stats counter
    * in window 4 (BadSSD). We have to read this too in order
    * to clear out all the stats registers and avoid a statsoflow
    * interrupt.
    */
   XL_SEL_WIN(4);

   t1 = CSR_READ_1(sc, XL_W4_UPPERBYTESOK);
   sc->xl_stats.xl_rx_bytes_ok += ((t1 & 0xf) << 16);
   sc->xl_stats.xl_tx_bytes_ok += (((t1 >> 4) & 0xf) << 16);

   sc->xl_stats.xl_badssd               += CSR_READ_1(sc, XL_W4_BADSSD);

   sc->xl_stats.mediastatus             = CSR_READ_2(sc, XL_W4_MEDIA_STATUS );
   sc->xl_stats.dmactl                  = CSR_READ_4(sc, XL_DMACTL );


   XL_SEL_WIN(7);

   if (!sc->xl_stats_no_timeout)
      rtems_timer_fire_after( sc->stat_timer_id, sc->stats_update_ticks, xl_stats_update, (void *)sc );
   return;
}







static void
xl_reset(struct elnk_softc *sc)
{
   register int		i;

   XL_SEL_WIN(0);
   CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_RESET |
               ((sc->xl_flags & XL_FLAG_WEIRDRESET) ?
                XL_RESETOPT_DISADVFD:0));

   for (i = 0; i < XL_TIMEOUT; i++) {
      DELAY(10);
      if (!(CSR_READ_2(sc, XL_STATUS) & XL_STAT_CMDBUSY))
         break;
   }

   if (i == XL_TIMEOUT)
      printk("etherlink : unit elnk%d reset didn't complete\n", sc->xl_unit);

   /* Reset TX and RX. */
   /* Note: the RX reset takes an absurd amount of time
    * on newer versions of the Tornado chips such as those
    * on the 3c905CX and newer 3c908C cards. We wait an
    * extra amount of time so that xl_wait() doesn't complain
    * and annoy the users.
    */
   CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_RX_RESET);
   DELAY(100000);
   xl_wait(sc);
   CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_TX_RESET);
   xl_wait(sc);

   if (sc->xl_flags & XL_FLAG_INVERT_LED_PWR ||
       sc->xl_flags & XL_FLAG_INVERT_MII_PWR)
   {
      XL_SEL_WIN(2);
      CSR_WRITE_2(sc, XL_W2_RESET_OPTIONS, CSR_READ_2(sc,
                                                      XL_W2_RESET_OPTIONS)
                  | ((sc->xl_flags & XL_FLAG_INVERT_LED_PWR)?XL_RESETOPT_INVERT_LED:0)
                  | ((sc->xl_flags & XL_FLAG_INVERT_MII_PWR)?XL_RESETOPT_INVERT_MII:0)
         );
   }

   /* Wait a little while for the chip to get its brains in order. */
   DELAY(100000);
   return;
}




static void
xl_stop(struct elnk_softc *sc)
{
   struct ifnet		*ifp;

   ifp = &sc->arpcom.ac_if;
   ifp->if_timer = 0;

   rtems_timer_cancel( sc->stat_timer_id );

   CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_RX_DISABLE);
   CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_STATS_DISABLE);
   CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_INTR_ENB);
   CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_RX_DISCARD);
   xl_wait(sc);
   CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_TX_DISABLE);
   CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_COAX_STOP);
   DELAY(800);

   CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_INTR_ACK|XL_STAT_INTLATCH);
   CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_STAT_ENB|0);
   CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_INTR_ENB|0);

   return;
}















static void
xl_setcfg(struct elnk_softc *sc)
{
   u_int32_t		icfg;

   XL_SEL_WIN(3);
   icfg = CSR_READ_4(sc, XL_W3_INTERNAL_CFG);

   icfg &= ~XL_ICFG_CONNECTOR_MASK;

   if (sc->xl_media & XL_MEDIAOPT_MII || sc->xl_media & XL_MEDIAOPT_BT4)
      icfg |= (XL_XCVR_MII << XL_ICFG_CONNECTOR_BITS);

   if (sc->xl_media & XL_MEDIAOPT_BTX)
      icfg |= (XL_XCVR_AUTO << XL_ICFG_CONNECTOR_BITS);

   CSR_WRITE_4(sc, XL_W3_INTERNAL_CFG, icfg);
   CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_COAX_STOP);

   XL_SEL_WIN(7);
   return;
}



static void
xl_setmode(
   struct elnk_softc	*sc,
   int			media)
{
   u_int32_t		icfg;
   u_int16_t		mediastat;

   printk("etherlink : unit elnk%d selecting ", sc->xl_unit);

   XL_SEL_WIN(4);
   mediastat = CSR_READ_2(sc, XL_W4_MEDIA_STATUS);
   XL_SEL_WIN(3);
   icfg = CSR_READ_4(sc, XL_W3_INTERNAL_CFG);

   if (sc->xl_media & XL_MEDIAOPT_BT) {
      if (IFM_SUBTYPE(media) == IFM_10_T) {
         printk("10baseT transceiver, ");
         sc->xl_xcvr = XL_XCVR_10BT;
         icfg &= ~XL_ICFG_CONNECTOR_MASK;
         icfg |= (XL_XCVR_10BT << XL_ICFG_CONNECTOR_BITS);
         mediastat |= XL_MEDIASTAT_LINKBEAT|
            XL_MEDIASTAT_JABGUARD;
         mediastat &= ~XL_MEDIASTAT_SQEENB;
      }
   }

   if (sc->xl_media & XL_MEDIAOPT_BFX) {
      if (IFM_SUBTYPE(media) == IFM_100_FX) {
         printk("100baseFX port, ");
         sc->xl_xcvr = XL_XCVR_100BFX;
         icfg &= ~XL_ICFG_CONNECTOR_MASK;
         icfg |= (XL_XCVR_100BFX << XL_ICFG_CONNECTOR_BITS);
         mediastat |= XL_MEDIASTAT_LINKBEAT;
         mediastat &= ~XL_MEDIASTAT_SQEENB;
      }
   }

   if (sc->xl_media & (XL_MEDIAOPT_AUI|XL_MEDIAOPT_10FL)) {
      if (IFM_SUBTYPE(media) == IFM_10_5) {
         printk("AUI port, ");
         sc->xl_xcvr = XL_XCVR_AUI;
         icfg &= ~XL_ICFG_CONNECTOR_MASK;
         icfg |= (XL_XCVR_AUI << XL_ICFG_CONNECTOR_BITS);
         mediastat &= ~(XL_MEDIASTAT_LINKBEAT|
                        XL_MEDIASTAT_JABGUARD);
         mediastat |= ~XL_MEDIASTAT_SQEENB;
      }
      if (IFM_SUBTYPE(media) == IFM_10_FL) {
         printk("10baseFL transceiver, ");
         sc->xl_xcvr = XL_XCVR_AUI;
         icfg &= ~XL_ICFG_CONNECTOR_MASK;
         icfg |= (XL_XCVR_AUI << XL_ICFG_CONNECTOR_BITS);
         mediastat &= ~(XL_MEDIASTAT_LINKBEAT|
                        XL_MEDIASTAT_JABGUARD);
         mediastat |= ~XL_MEDIASTAT_SQEENB;
      }
   }

   if (sc->xl_media & XL_MEDIAOPT_BNC) {
      if (IFM_SUBTYPE(media) == IFM_10_2) {
         printk("BNC port, ");
         sc->xl_xcvr = XL_XCVR_COAX;
         icfg &= ~XL_ICFG_CONNECTOR_MASK;
         icfg |= (XL_XCVR_COAX << XL_ICFG_CONNECTOR_BITS);
         mediastat &= ~(XL_MEDIASTAT_LINKBEAT|
                        XL_MEDIASTAT_JABGUARD|
                        XL_MEDIASTAT_SQEENB);
      }
   }

   if ((media & IFM_GMASK) == IFM_FDX ||
       IFM_SUBTYPE(media) == IFM_100_FX) {
      printk("full duplex\n");
      XL_SEL_WIN(3);
      CSR_WRITE_1(sc, XL_W3_MAC_CTRL, XL_MACCTRL_DUPLEX);
   } else {
      printk("half duplex\n");
      XL_SEL_WIN(3);
      CSR_WRITE_1(sc, XL_W3_MAC_CTRL,
                  (CSR_READ_1(sc, XL_W3_MAC_CTRL) & ~XL_MACCTRL_DUPLEX));
   }

   if (IFM_SUBTYPE(media) == IFM_10_2)
      CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_COAX_START);
   else
      CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_COAX_STOP);

   CSR_WRITE_4(sc, XL_W3_INTERNAL_CFG, icfg);
   XL_SEL_WIN(4);
   CSR_WRITE_2(sc, XL_W4_MEDIA_STATUS, mediastat);
   DELAY(800);
   XL_SEL_WIN(7);

   return;
}







static void
xl_choose_xcvr(
   struct elnk_softc	*sc,
   int			verbose)
{
   u_int16_t		devid;

   /*
    * Read the device ID from the EEPROM.
    * This is what's loaded into the PCI device ID register, so it has
    * to be correct otherwise we wouldn't have gotten this far.
    */
   xl_read_eeprom(sc, (caddr_t)&devid, XL_EE_PRODID, 1, 0);

   switch(devid) {
      case TC_DEVICEID_BOOMERANG_10BT:	/* 3c900-TPO */
      case TC_DEVICEID_KRAKATOA_10BT:		/* 3c900B-TPO */
         sc->xl_media = XL_MEDIAOPT_BT;
         sc->xl_xcvr = XL_XCVR_10BT;
         if (verbose)
            printk("etherlink : unit elnk%d guessing 10BaseT "
                   "transceiver\n", sc->xl_unit);
         break;
      case TC_DEVICEID_BOOMERANG_10BT_COMBO:	/* 3c900-COMBO */
      case TC_DEVICEID_KRAKATOA_10BT_COMBO:	/* 3c900B-COMBO */
         sc->xl_media = XL_MEDIAOPT_BT|XL_MEDIAOPT_BNC|XL_MEDIAOPT_AUI;
         sc->xl_xcvr = XL_XCVR_10BT;
         if (verbose)
            printk("etherlink : unit elnk%d guessing COMBO "
                   "(AUI/BNC/TP)\n", sc->xl_unit);
         break;
      case TC_DEVICEID_KRAKATOA_10BT_TPC:	/* 3c900B-TPC */
         sc->xl_media = XL_MEDIAOPT_BT|XL_MEDIAOPT_BNC;
         sc->xl_xcvr = XL_XCVR_10BT;
         if (verbose)
            printk("etherlink : unit elnk%d guessing TPC (BNC/TP)\n", sc->xl_unit);
         break;
      case TC_DEVICEID_CYCLONE_10FL:		/* 3c900B-FL */
         sc->xl_media = XL_MEDIAOPT_10FL;
         sc->xl_xcvr = XL_XCVR_AUI;
         if (verbose)
            printk("etherlink : unit elnk%d guessing 10baseFL\n", sc->xl_unit);
         break;
      case TC_DEVICEID_BOOMERANG_10_100BT:	/* 3c905-TX */
      case TC_DEVICEID_HURRICANE_555:		/* 3c555 */
      case TC_DEVICEID_HURRICANE_556:		/* 3c556 */
      case TC_DEVICEID_HURRICANE_556B:	/* 3c556B */
      case TC_DEVICEID_HURRICANE_575A:	/* 3c575TX */
      case TC_DEVICEID_HURRICANE_575B:	/* 3c575B */
      case TC_DEVICEID_HURRICANE_575C:	/* 3c575C */
      case TC_DEVICEID_HURRICANE_656:		/* 3c656 */
      case TC_DEVICEID_HURRICANE_656B:	/* 3c656B */
      case TC_DEVICEID_TORNADO_656C:		/* 3c656C */
      case TC_DEVICEID_TORNADO_10_100BT_920B:	/* 3c920B-EMB */
         sc->xl_media = XL_MEDIAOPT_MII;
         sc->xl_xcvr = XL_XCVR_MII;
         if (verbose)
            printk("etherlink : unit elnk%d guessing MII\n", sc->xl_unit);
         break;
      case TC_DEVICEID_BOOMERANG_100BT4:	/* 3c905-T4 */
      case TC_DEVICEID_CYCLONE_10_100BT4:	/* 3c905B-T4 */
         sc->xl_media = XL_MEDIAOPT_BT4;
         sc->xl_xcvr = XL_XCVR_MII;
         if (verbose)
            printk("etherlink : unit elnk%d guessing 100BaseT4/MII\n", sc->xl_unit);
         break;
      case TC_DEVICEID_HURRICANE_10_100BT:	/* 3c905B-TX */
      case TC_DEVICEID_HURRICANE_10_100BT_SERV:/*3c980-TX */
      case TC_DEVICEID_TORNADO_10_100BT_SERV:	/* 3c980C-TX */
      case TC_DEVICEID_HURRICANE_SOHO100TX:	/* 3cSOHO100-TX */
      case TC_DEVICEID_TORNADO_10_100BT:	/* 3c905C-TX */
      case TC_DEVICEID_TORNADO_HOMECONNECT:	/* 3c450-TX */
         sc->xl_media = XL_MEDIAOPT_BTX;
         sc->xl_xcvr = XL_XCVR_AUTO;
         if (verbose)
            printk("etherlink : unit elnk%d guessing 10/100 internal\n", sc->xl_unit);
         break;
      case TC_DEVICEID_CYCLONE_10_100_COMBO:	/* 3c905B-COMBO */
         sc->xl_media = XL_MEDIAOPT_BTX|XL_MEDIAOPT_BNC|XL_MEDIAOPT_AUI;
         sc->xl_xcvr = XL_XCVR_AUTO;
         if (verbose)
            printk("etherlink : unit elnk%d guessing 10/100 "
                   "plus BNC/AUI\n", sc->xl_unit);
         break;
      default:
         printk("etherlink : unit elnk%d unknown device ID: %x -- "
                "defaulting to 10baseT\n", sc->xl_unit, devid);
         sc->xl_media = XL_MEDIAOPT_BT;
         break;
   }

   return;
}







/*
 * This routine is a kludge to work around possible hardware faults
 * or manufacturing defects that can cause the media options register
 * (or reset options register, as it's called for the first generation
 * 3c90x adapters) to return an incorrect result. I have encountered
 * one Dell Latitude laptop docking station with an integrated 3c905-TX
 * which doesn't have any of the 'mediaopt' bits set. This screws up
 * the attach routine pretty badly because it doesn't know what media
 * to look for. If we find ourselves in this predicament, this routine
 * will try to guess the media options values and warn the user of a
 * possible manufacturing defect with his adapter/system/whatever.
 */
static void
xl_mediacheck(struct elnk_softc		*sc)
{

   xl_choose_xcvr(sc, 1);

   /*
    * If some of the media options bits are set, assume they are
    * correct. If not, try to figure it out down below.
    * XXX I should check for 10baseFL, but I don't have an adapter
    * to test with.
    */
   if (sc->xl_media & (XL_MEDIAOPT_MASK & ~XL_MEDIAOPT_VCO)) {
      /*
       * Check the XCVR value. If it's not in the normal range
       * of values, we need to fake it up here.
       */
      if (sc->xl_xcvr <= XL_XCVR_AUTO)
         return;
      else {
         printk("etherlink : unit elnk%d bogus xcvr value "
                "in EEPROM (%x)\n", sc->xl_unit, sc->xl_xcvr);
         printk("etherlink : unit elnk%d choosing new default based "
                "on card type\n", sc->xl_unit);
      }
   } else {
      if (sc->xl_type == XL_TYPE_905B &&
          sc->xl_media & XL_MEDIAOPT_10FL)
         return;
      printk("etherlink : unit elnk%d WARNING: no media options bits set in "
             "the media options register!!\n", sc->xl_unit);
      printk("etherlink : unit elnk%d this could be a manufacturing defect in "
             "your adapter or system\n", sc->xl_unit);
      printk("etherlink : unit elnk%d attempting to guess media type; you "
             "should probably consult your vendor\n", sc->xl_unit);
   }

   return;
}

























static void no_op(const rtems_irq_connect_data* irq)
{
   return;
}




static int elnkIsOn(const rtems_irq_connect_data* irq)
{
  return BSP_irq_enabled_at_i8259s (irq->name);
}






static void
elnk_start_txchain( struct elnk_softc *sc, struct TXMD *chainhead )
{
   xl_wait(sc);
   CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_DOWN_STALL);

   /* save the address of the TX list */
   sc->last_txchain_head = chainhead;
   sc->tx_idle = 0;

   xl_wait(sc);

   CSR_WRITE_4(sc, XL_DOWNLIST_PTR, phys_to_bus( sc->last_txchain_head ));
   CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_DOWN_UNSTALL);
}





/*
 * ELNK interrupt handler
 */
static rtems_isr
elnk_interrupt_handler ( struct elnk_softc *sc )
{
   struct ifnet		*ifp = &sc->arpcom.ac_if;
   u_int16_t		status;

   while( ((status = CSR_READ_2(sc, XL_STATUS)) & XL_INTRS) && status != 0xFFFF)
   {
      sc->xl_stats.device_interrupts++;

      CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_INTR_ACK | (status & XL_INTRS));

#if 0
      printk("etherlink : unit elnk%d intstatus %04x\n", sc->xl_unit, status  );
#endif

      if (status & XL_STAT_UP_COMPLETE)
      {
#if 0
         printk("etherlink : unit elnk%d rx\n", sc->xl_unit );
#endif
         /* received packets */
         rtems_event_send(rxDaemonTid, sc->ioevent);
      }

      if( (status & XL_STAT_DOWN_COMPLETE) || (status & XL_STAT_TX_COMPLETE) )
      {
         /* all packets uploaded to the device */
         struct TXMD *chaintailmd = NULL;


         if( status & XL_STAT_TX_COMPLETE )
         {
            /* if we got a tx complete error, count it, then reset the
               transmitter.  Consider the entire chain lost.. */

            ifp->if_oerrors++;
            sc->xl_stats.txcomplete_ints++;

            printk("etherlink : unit elnk%d transmit error\n", sc->xl_unit );

            /* reset, re-enable fifo */

            xl_wait(sc);
            CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_TX_DISABLE);

            xl_wait(sc);
            CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_TX_RESET | 1 );

            xl_wait(sc);
            CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_TX_ENABLE);

            xl_wait(sc);
         }


         /* send the chain head to the tx task which will recover the
            whole chain */
         rtems_message_queue_send( chainRecoveryQueue, &sc->last_txchain_head, sizeof(struct TXMD *));


         /* set up the next chain */
         if( sc->last_txchain_head->chainptr )
         {
            /* check the head of the chain of packets we just finished,
             * if != 0, either this is a chain of 2 or more packets or
             * its a single packet chain and another chain is ready to
             * send.
             */
            if( (int)sc->last_txchain_head->chainptr == -1 )
            {
               /*
               ** single packet was sent so no indirection to the last
               ** entry in the chain.  since chainptr is != 0, then
               ** another chain is ready starting from the packet AFTER
               ** the chain we just finished. - in this case the last
               ** chain's head == its tail
               */
               chaintailmd = sc->last_txchain_head;
            }
            else
            {
               /*
               ** otherwise, this is a pointer to the last packet in the
               ** chain of 2 or more packets.  If the chain's last
               ** packet's chainptr is != 0, then another chain is ready
               ** to send.
               */
               chaintailmd = sc->last_txchain_head->chainptr;
               if( !chaintailmd->chainptr ) chaintailmd = NULL;
            }
         }

         if( chaintailmd )
         {
            /* the next MD is the start of another chain */
            elnk_start_txchain(sc, chaintailmd->next_md );
         }
         else
         {
            /* otherwise nothing to send, so go idle */
            sc->tx_idle = -1;

            /* wake up the tx daemon once so we're sure this last chain
               will be freed */
            rtems_event_send( txDaemonTid, sc->ioevent );
#if 0
            printk("unit elnk%d tx done\n", sc->xl_unit );
#endif
         }
      }


      if (status & XL_STAT_ADFAIL)
      {
         printk("etherlink : unit elnk%d Catastrophic bus failure\n", sc->xl_unit );
      }
      if (status & XL_STAT_STATSOFLOW)
      {
         sc->xl_stats_no_timeout = 1;
         xl_stats_update(sc->stat_timer_id,sc);
         sc->xl_stats_no_timeout = 0;
      }
   }


#if 0
   {
      uint16_t   intstatus, intenable, indenable;

      intstatus = CSR_READ_2(sc, XL_STATUS );

      XL_SEL_WIN(5);
      intenable = CSR_READ_2(sc, XL_W5_INTR_ENB );
      indenable = CSR_READ_2(sc, XL_W5_STAT_ENB );
      XL_SEL_WIN(7);
      printk("etherlink : unit elnk%d istat %04x, ien %04x, ind %04x\n", sc->xl_unit, intstatus, intenable, indenable  );
   }
#endif
}





static rtems_isr
elnk_interrupt_handler_entry(void)
{
   int i;

   /*
   ** Check all the initialized units for interrupt service
   */

   for(i=0; i< NUM_UNITS; i++ )
   {
      if( elnk_softc[i].ioaddr )
         elnk_interrupt_handler( &elnk_softc[i] );
   }
}











/*
 * Initialize the ethernet hardware
 */
static void
elnk_initialize_hardware (struct elnk_softc *sc)
{
   unsigned char *cp;
   int i, j, rxsize, txsize, ringsize;

   /*
    * Init RX ring
    */
   cp = (unsigned char *)malloc( (ringsize = ((rxsize = (sc->numRxbuffers * sizeof(struct RXMD))) +
                                              (txsize = (sc->numTxbuffers * sizeof(struct TXMD)))) ) +
                                 + CPU_CACHE_ALIGNMENT_FOR_BUFFER);
   sc->bufferBase = cp;
   cp += (CPU_CACHE_ALIGNMENT_FOR_BUFFER - (int)cp) & (CPU_CACHE_ALIGNMENT_FOR_BUFFER - 1);
#if defined(__i386__)
#ifdef PCI_BRIDGE_DOES_NOT_ENSURE_CACHE_COHERENCY_FOR_DMA
   if (_CPU_is_paging_enabled())
      _CPU_change_memory_mapping_attribute
         (NULL, cp, ringsize, PTE_CACHE_DISABLE | PTE_WRITABLE);
#endif
#endif
   sc->ringBase = cp;

   /* build tx and rx rings */

   sc->rx_ring = (struct RXMD *)sc->ringBase;
   sc->tx_ring = (struct TXMD *)&sc->ringBase[ rxsize ];

   {
      struct mbuf    *m;
      struct RXMD    *nxtmd;
      /*
       * The rx ring is easy as its just an array of RXMD structs.  New
       * mbuf entries are allocated from the stack whenever the rx
       * daemon forwards an incoming packet into it.  Here, we
       * pre-allocate the rx mbufs for the rx ring entries.
       */
      for(i=0 ; i<sc->numRxbuffers; i++)
      {
         if( ((uint32_t)&sc->rx_ring[i] & 0x7) )
         {
            rtems_panic ("etherlink : unit elnk%d rx ring entry %d not aligned to 8 bytes\n", sc->xl_unit, i );
         }

         /* allocate an mbuf for each receive descriptor */
         MGETHDR (m, M_WAIT, MT_DATA);
         MCLGET (m, M_WAIT);
         m->m_pkthdr.rcvif = &sc->arpcom.ac_if;

         if( i == sc->numRxbuffers-1 )
            nxtmd = &sc->rx_ring[0];
         else
            nxtmd = &sc->rx_ring[i+1];

         sc->rx_ring[i].next_md = nxtmd;
         sc->rx_ring[i].mbuf = m;

         st_le32( &sc->rx_ring[i].status, 0);
         st_le32( &sc->rx_ring[i].next, (uint32_t)phys_to_bus( nxtmd ));
         st_le32( &sc->rx_ring[i].addr, (uint32_t)phys_to_bus( mtod(m, void *) ));
         st_le32( &sc->rx_ring[i].length, XL_LAST_FRAG | XL_PACKET_SIZE );
      }
      sc->curr_rx_md = &sc->rx_ring[0];
   }


   {
      struct TXMD *thismd, *nxtmd;
      /*
       * The tx ring is more complex. Each MD has an array of fragment
       * descriptors that are loaded from each packet as they arrive
       * from the stack.  Each packet gets one ring entry, this allows
       * the lanboard to efficiently assemble the piecemeal packets into
       * a contiguous unit at transmit time, rather than spending
       * cputime concatenating them first.  Although the next_md fields
       * form a ring, the DPD next is filled only when packets are added
       * to the tx chain, thus last entry of a series of packets has the
       * requisite dpd->next value == 0 to terminate the dma.  mbuf
       * holds the packet info so it can be freed once the packet has
       * been sent.  chainptr is used to link the head & tail of a chain
       * of 2 or more packets.  A chain is formed when the tx daemon
       * gets 2 or more packets from the stack's queue in a service
       * period, so higher outgoing loads are handled as efficiently as
       * possible.
       */

      for(i=0 ; i<sc->numTxbuffers; i++)
      {
         if( ((uint32_t)&sc->tx_ring[i] & 0x7) )
         {
            rtems_panic ("etherlink : unit elnk%d tx ring entry %d not aligned to 8 bytes\n", sc->xl_unit, i );
         }

         if( i == sc->numTxbuffers-1 )
            nxtmd = &sc->tx_ring[0];
         else
            nxtmd = &sc->tx_ring[i+1];

         thismd = &sc->tx_ring[i];

         thismd->next_md = nxtmd;
         thismd->chainptr = NULL;
         thismd->mbuf = NULL;

         st_le32( &thismd->status, XL_TXSTAT_DL_COMPLETE );
         st_le32( &thismd->next, 0);

         for(j=0; j< NUM_FRAGS; j++)
         {
            st_le32( &thismd->txfrags[j].addr, 0 );
            st_le32( &thismd->txfrags[j].length, 0 );
         }
      }
      sc->last_tx_md = &sc->tx_ring[0];
   }




#ifdef ELNK_DEBUG
   printk("etherlink : %02x:%02x:%02x:%02x:%02x:%02x   name 'elnk%d', io %x, int %d\n",
          sc->arpcom.ac_enaddr[0], sc->arpcom.ac_enaddr[1],
          sc->arpcom.ac_enaddr[2], sc->arpcom.ac_enaddr[3],
          sc->arpcom.ac_enaddr[4], sc->arpcom.ac_enaddr[5],
          sc->xl_unit,
          (unsigned)sc->ioaddr, sc->irqInfo.name );
#endif


   sc->irqInfo.hdl  = (rtems_irq_hdl)elnk_interrupt_handler_entry;
   sc->irqInfo.on   = no_op;
   sc->irqInfo.off  = no_op;
   sc->irqInfo.isOn = elnkIsOn;

   if( sc->irqInfo.name != 255 )
   {
      int st;

#ifdef BSP_SHARED_HANDLER_SUPPORT
      st = BSP_install_rtems_shared_irq_handler( &sc->irqInfo );
#else
      st = BSP_install_rtems_irq_handler( &sc->irqInfo );
#endif
      if (!st)
         rtems_panic ("etherlink : unit elnk%d Interrupt name %d already in use\n", sc->xl_unit, sc->irqInfo.name );
   }
   else
   {
      printk("etherlink : unit elnk%d Interrupt not specified by device\n", sc->xl_unit );
   }
}











static void
elnk_rxDaemon (void *arg)
{
   struct elnk_softc     *sc;
   struct ether_header   *eh;
   struct mbuf           *m;
   struct RXMD           *rmd;
   unsigned int          i,len, rxstat;
   rtems_event_set       events;

   for (;;)
   {

      rtems_bsdnet_event_receive( RTEMS_ALL_EVENTS,
                                  RTEMS_WAIT|RTEMS_EVENT_ANY,
                                  RTEMS_NO_TIMEOUT,
                                  &events);

      for(;;)
      {
         for(i=0; i< NUM_UNITS; i++ )
         {
            sc = &elnk_softc[i];
            if( sc->ioaddr )
            {
               if( events & sc->ioevent )
               {
                  struct ifnet *ifp = &sc->arpcom.ac_if;

                  rmd   = sc->curr_rx_md;

                  /*
                  ** Read off all the packets we've received on this unit
                  */
                  while( (rxstat = ld_le32(&rmd->status)) )
                  {
                     if (rxstat & XL_RXSTAT_UP_ERROR)
                     {
                        printk("unit %i up error\n", sc->xl_unit );
                        ifp->if_ierrors++;
                     }

                     if( (rxstat & XL_RXSTAT_UP_CMPLT) )
                     {

#if 0
                        {
                           char *pkt, *delim;
                           int  i;
                           pkt = mtod(rmd->mbuf, char *);
                           printk("unit %i rx  pkt (%08x) ", sc->xl_unit, pkt );
                           for(delim="", i=0; i < sizeof(struct ether_header)+8; i++, delim=":")
                              printk("%s%02x", delim, (char) pkt[i] );
                           printk("\n");
                        }
#endif

                        /* pass on the packet in the mbuf */
                        len = ( ld_le32(&rmd->status) & XL_RXSTAT_LENMASK);
                        m = rmd->mbuf;
                        m->m_len = m->m_pkthdr.len = len - sizeof(struct ether_header);
                        eh = mtod(m, struct ether_header *);
                        m->m_data += sizeof(struct ether_header);

                        ether_input(ifp, eh, m);

                        /* get a new mbuf */
                        MGETHDR (m, M_WAIT, MT_DATA);
                        MCLGET (m, M_WAIT);
                        m->m_pkthdr.rcvif = ifp;
                        rmd->mbuf   = m;
                        st_le32( &rmd->status, 0 );
                        st_le32( &rmd->addr, (uint32_t)phys_to_bus(mtod(m, void *)) );
                     }
                     else
                     {
                        /* some kind of packet failure */
                        printk("etherlink : unit elnk%d bad receive status -- packet dropped\n", sc->xl_unit);
                        ifp->if_ierrors++;
                     }
                     /* clear descriptor status */
                     rmd->status = 0;

                     rmd = rmd->next_md;
                  }

                  sc->curr_rx_md = rmd;
               }
            }
         }

         /*
         ** If more events are pending, service them before we go back to sleep
         */
         if( rtems_event_receive( RTEMS_ALL_EVENTS,
                                  RTEMS_NO_WAIT | RTEMS_EVENT_ANY,
                                  0,
                                  &events ) == RTEMS_UNSATISFIED ) break;
      }
   }
}








/*
 * Driver transmit daemon
 */
void
elnk_txDaemon (void *arg)
{
   struct elnk_softc     *sc;
   struct ifnet          *ifp;
   struct mbuf           *m;
   struct TXMD           *lastmd, *nextmd, *firstmd;
   int                   chainCount,i;
   rtems_event_set       events;

   for (;;)
   {
      /*
       * Wait for any unit's signal to wake us up
       */
      rtems_bsdnet_event_receive( RTEMS_ALL_EVENTS,
                                  RTEMS_EVENT_ANY | RTEMS_WAIT,
                                  RTEMS_NO_TIMEOUT, &events);

      for(i=0; i< NUM_UNITS; i++ )
      {
         sc  = &elnk_softc[i];
         if( sc->ioaddr )
         {
            if( events & sc->ioevent )
            {
               ifp = &sc->arpcom.ac_if;

               /*
                * Send packets till queue is empty or tx ring is full
                */

               chainCount = 0;
               firstmd = NULL;

               lastmd = sc->last_tx_md;

               for(;;)
               {
                  /*
                  ** Check the chain recovery queue whenever the tx
                  ** daemon services the stack.  Note this routine does
                  ** not assume the context of one of the lanboard units
                  ** because used tx mbufs are no longer associated with
                  ** any unit.
                  */
                  {
                     struct TXMD *chainhead, *chaintail;
                     size_t esize;

                     if( rtems_message_queue_receive( chainRecoveryQueue, &chainhead, &esize,
                                                      RTEMS_NO_WAIT, 0) == RTEMS_SUCCESSFUL )
                     {
                        /* get a pointer to the tail */
                        chaintail = chainhead->chainptr;

                        /* if the tail points somewhere, free the entire
                           chain */
                        if( chaintail && (int)chaintail != -1 )
                        {
                           for(;;)
                           {
                              m_freem( chainhead->mbuf );
                              st_le32( &chainhead->status, XL_TXSTAT_DL_COMPLETE );
                              chainhead->mbuf = NULL;

                              if(  chainhead == chaintail ) break;
                              chainhead = chainhead->next_md;
                           }
                        }
                        else
                        {
                           /* a single packet chain */
                           m_freem( chainhead->mbuf );
                           st_le32( &chainhead->status, XL_TXSTAT_DL_COMPLETE );
                           chainhead->mbuf = NULL;
                        }
                     }
                  }

                  nextmd = lastmd->next_md;

                  /* stop when ring is full */
                  if( ! (ld_le32(&nextmd->status) & XL_TXSTAT_DL_COMPLETE) )
                  {
                     printk("etherlink : unit elnk%d tx ring full!\n", sc->xl_unit);
                     break;
                  }
                  /* sanity check the next packet descriptor */
                  if( nextmd->mbuf )
                  {
                     printk("etherlink : unit elnk%d tx ring corrupt!\n", sc->xl_unit);
                     break;
                  }



                  IF_DEQUEUE(&ifp->if_snd, m);
                  if( !m ) break;

                  {
                     int i;

                     nextmd->mbuf = m;

                     for(i=0; i< NUM_FRAGS; i++)
                     {
                        st_le32( &nextmd->txfrags[i].length, ((m->m_next)?0:XL_LAST_FRAG) | ( m->m_len & XL_TXSTAT_LENMASK) );
                        st_le32( &nextmd->txfrags[i].addr, (uint32_t)phys_to_bus( m->m_data ) );
                        if ((m = m->m_next) == NULL)
                           break;
                     }
                     if( m )
                     {
                        printk("etherlink : unit elnk%d tx fragments exhausted, truncating packet!\n", sc->xl_unit);
                        st_le32( &nextmd->txfrags[NUM_FRAGS-1].length, XL_LAST_FRAG |
                                 ld_le32( &nextmd->txfrags[NUM_FRAGS-1].length) );
                     }
                  }

#if 0
                  {
                     char *pkt = bus_to_phys( ld_le32( &nextmd->txfrags[i].addr )), *delim;
                     int  i;
                     printk("unit %d queued  pkt (%08x) ", sc->xl_unit, (uint32_t)pkt );
                     for(delim="", i=0; i < sizeof(struct ether_header); i++, delim=":")
                        printk("%s%02x", delim, (char) pkt[i] );
                     printk("\n");
                  }
#endif


                  /* this packet will be the new end of the list */
                  st_le32( &nextmd->next, 0);
                  st_le32( &nextmd->status, 0);

                  if( !firstmd )
                  {
                     /* keep track of the first packet we add to the chain */
                     firstmd = nextmd;

                     /*
                      ** use the chainbuf pointer of the last packet of
                      ** the previous chain as a flag so when a
                      ** dnComplete interrupt indicates the card is
                      ** finished downloading the chain, the isr can
                      ** immediately start the next which always begins
                      ** with the next packet in the ring.  Note several
                      ** chains of packets may be assembled this way.
                      */
                     lastmd->chainptr = (struct TXMD *)-1;
                  }
                  else
                  {
                     /* hook this packet to the previous one */
                     st_le32( &lastmd->next, (uint32_t)phys_to_bus( nextmd ));
                  }

                  ++chainCount;
                  lastmd = nextmd;
               }





               if( firstmd )
               {
                  /* only enter if we've queued one or more packets */

                  /* save the last descriptor we set up in the chain */
                  sc->last_tx_md = lastmd;

                  /*
                   * We've added one or more packets to a chain, flag
                   * the last packet so we get an dnComplete interrupt
                   * when the card finishes accepting the chain
                   */
                  st_le32( &lastmd->status, XL_TXSTAT_DL_INTR );

                  /*
                   * point the chain head's chainptr to the tail so we
                   * can jump to the next chain to send inside the isr.
                   * If we're only sending one packet, then don't bother
                   * with the link, as the chainptr value will either be
                   * 0 if theres no next chain or -1 if there is.
                   */
                  if( chainCount > 1 )
                  {
                     firstmd->chainptr = lastmd;

                     sc->chain_lengths[sc->chlenIndex]= (short)chainCount;
                     if( ++sc->chlenIndex == NUM_CHAIN_LENGTHS ) sc->chlenIndex = 0;
                  }

                  /*
                  ** clear the last packet's chainptr flag.  If another
                  ** chain is added later but before this chain is
                  ** finished being sent, this flag on this packet will
                  ** be re-set to -1
                  */
                  lastmd->chainptr = NULL;

#if 0
                  printk("unit %d queued %d pkts, lastpkt status %08X\n",
                         sc->xl_unit,
                         chainCount,
                         (uint32_t)ld_le32( &lastmd->status) );
#endif

                  if( sc->tx_idle == 0 && CSR_READ_4(sc, XL_DOWNLIST_PTR) == 0 )
                  {
                     printk("etherlink : unit elnk%d tx forced!\n", sc->xl_unit);
                     sc->tx_idle = -1;
                  }

                  /*
                  ** start sending this chain of packets if tx isn't
                  ** busy, else the dnComplete interrupt will see there
                  ** is another chain waiting and begin it immediately.
                  */
                  if( sc->tx_idle )
                  {
#if 0
                     printk("etherlink : unit elnk%d tx started %d packets\n", sc->xl_unit, chainCount );
#endif
                     elnk_start_txchain(sc, firstmd);
                  }
               }


               ifp->if_flags &= ~IFF_OACTIVE;
            }
         }
      }
   }
}











static void
elnk_start (struct ifnet *ifp)
{
   struct elnk_softc *sc = ifp->if_softc;
#if 0
   printk("unit %i tx signaled\n", sc->xl_unit );
#endif
   ifp->if_flags |= IFF_OACTIVE;
   rtems_event_send( txDaemonTid, sc->ioevent );
}















/*
 * Initialize and start the device
 */
static void
elnk_init (void *arg)
{
   int i;
   struct elnk_softc *sc = arg;
   struct ifnet *ifp = &sc->arpcom.ac_if;

   if( !(ifp->if_flags & IFF_RUNNING) )
   {
      xl_stop(sc);
      xl_reset(sc);
      sc->tx_idle = -1;

      {
         uint32_t   cr,sr;

         xl_miibus_writereg(sc, 0x18, MII_BMCR, BMCR_RESET );

         while( (cr = xl_miibus_readreg(sc, 0x18, MII_BMCR )) & BMCR_RESET )
         {
            DELAY(100000);
         }

         xl_miibus_writereg(sc, 0x18, MII_ANAR, ANAR_10 | ANAR_TX | ANAR_10_FD | ANAR_TX_FD );  /*  ANAR_T4 */
         xl_miibus_writereg(sc, 0x18, MII_BMCR, BMCR_STARTNEG | BMCR_AUTOEN );

         for (i=0; ((sr = xl_miibus_readreg(sc, 0x18, MII_BMSR)) & BMSR_ACOMP) == 0 && i < 20; i++)
            DELAY(10000);
      }


      /*
       * Set up hardware if its not already been done
       */
      if( !sc->irqInfo.hdl )
      {
         elnk_initialize_hardware(sc);
      }

      /*
       * Enable the card
       */
      {
         u_int8_t		rxfilt;

         /* Init our MAC address */
         XL_SEL_WIN(2);
         for (i = 0; i < ETHER_ADDR_LEN; i++)
         {
            CSR_WRITE_1(sc, XL_W2_STATION_ADDR_LO + i, sc->arpcom.ac_enaddr[i]);
         }

         {
            int  media = IFM_ETHER|IFM_100_TX|IFM_FDX;

            xl_mediacheck(sc);

            /* Choose a default media. */
            switch(sc->xl_xcvr) {
               case XL_XCVR_10BT:
                  media = IFM_ETHER|IFM_10_T;
                  xl_setmode(sc, media);
                  break;
               case XL_XCVR_AUI:
                  if (sc->xl_type == XL_TYPE_905B &&
                      sc->xl_media == XL_MEDIAOPT_10FL) {
                     media = IFM_ETHER|IFM_10_FL;
                     xl_setmode(sc, media);
                  } else {
                     media = IFM_ETHER|IFM_10_5;
                     xl_setmode(sc, media);
                  }
                  break;
               case XL_XCVR_COAX:
                  media = IFM_ETHER|IFM_10_2;
                  xl_setmode(sc, media);
                  break;
               case XL_XCVR_AUTO:
               case XL_XCVR_100BTX:
                  xl_setcfg(sc);
                  break;
               case XL_XCVR_MII:
                  printk("etherlink : unit elnk%d MII media not supported!\n", sc->xl_unit);
                  break;
               case XL_XCVR_100BFX:
                  media = IFM_ETHER|IFM_100_FX;
                  break;
               default:
                  printk("etherlink : unit elnk%d unknown XCVR type: %d\n", sc->xl_unit, sc->xl_xcvr);
                  /*
                   * This will probably be wrong, but it prevents
                   * the ifmedia code from panicking.
                   */
                  media = IFM_ETHER|IFM_10_T;
                  break;
            }


            if (sc->xl_flags & XL_FLAG_NO_XCVR_PWR) {
               XL_SEL_WIN(0);
               CSR_WRITE_2(sc, XL_W0_MFG_ID, XL_NO_XCVR_PWR_MAGICBITS);
            }
         }



         XL_SEL_WIN(2);
         /* Clear the station mask. */
         for (i = 0; i < 3; i++)
            CSR_WRITE_2(sc, XL_W2_STATION_MASK_LO + (i * 2), 0);

         /*
          * Set the TX freethresh value.
          * Note that this has no effect on 3c905B "cyclone"
          * cards but is required for 3c900/3c905 "boomerang"
          * cards in order to enable the download engine.
          */
         CSR_WRITE_1(sc, XL_TX_FREETHRESH, XL_PACKET_SIZE >> 8);

         /* Set the TX start threshold for best performance. */
         sc->xl_tx_thresh = XL_MIN_FRAMELEN;
         CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_TX_SET_START|sc->xl_tx_thresh);

         /*
          * If this is a 3c905B, also set the tx reclaim threshold.
          * This helps cut down on the number of tx reclaim errors
          * that could happen on a busy network. The chip multiplies
          * the register value by 16 to obtain the actual threshold
          * in bytes, so we divide by 16 when setting the value here.
          * The existing threshold value can be examined by reading
          * the register at offset 9 in window 5.
          */
         if (sc->xl_type == XL_TYPE_905B) {
            CSR_WRITE_2(sc, XL_COMMAND,
                        XL_CMD_SET_TX_RECLAIM|(XL_PACKET_SIZE >> 4));
         }

         /* Set RX filter bits. */
         XL_SEL_WIN(5);
         rxfilt = CSR_READ_1(sc, XL_W5_RX_FILTER);

         /* Set the individual bit to receive frames for this host only. */
         rxfilt |= XL_RXFILTER_INDIVIDUAL;

         /* If we want promiscuous mode, set the allframes bit. */
         if (ifp->if_flags & IFF_PROMISC) {
            rxfilt |= XL_RXFILTER_ALLFRAMES;
            CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_RX_SET_FILT|rxfilt);
         } else {
            rxfilt &= ~XL_RXFILTER_ALLFRAMES;
            CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_RX_SET_FILT|rxfilt);
         }

         /*
          * Set capture broadcast bit to capture broadcast frames.
          */
         if (ifp->if_flags & IFF_BROADCAST) {
            rxfilt |= XL_RXFILTER_BROADCAST;
            CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_RX_SET_FILT|rxfilt);
         } else {
            rxfilt &= ~XL_RXFILTER_BROADCAST;
            CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_RX_SET_FILT|rxfilt);
         }

#if 0
         /*
          * Program the multicast filter, if necessary.
          */
         if (sc->xl_type == XL_TYPE_905B)
            xl_setmulti_hash(sc);
         else
            xl_setmulti(sc);
#endif
         /*
          * Load the address of the RX list. We have to
          * stall the upload engine before we can manipulate
          * the uplist pointer register, then unstall it when
          * we're finished. We also have to wait for the
          * stall command to complete before proceeding.
          * Note that we have to do this after any RX resets
          * have completed since the uplist register is cleared
          * by a reset.
          */
         CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_UP_STALL);
         xl_wait(sc);
         CSR_WRITE_4(sc, XL_UPLIST_PTR, phys_to_bus( sc->curr_rx_md ));
         CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_UP_UNSTALL);
         xl_wait(sc);


#if 0
         if (sc->xl_type == XL_TYPE_905B) {
            /* Set polling interval */
            CSR_WRITE_1(sc, XL_DOWN_POLL, 64);
            xl_wait(sc);
            printk("etherlink : unit elnk%d tx polling enabled\n", sc->xl_unit );
         }
#endif

         /*
          * If the coax transceiver is on, make sure to enable
          * the DC-DC converter.
          */
         XL_SEL_WIN(3);
         if (sc->xl_xcvr == XL_XCVR_COAX)
            CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_COAX_START);
         else
            CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_COAX_STOP);

         /* increase packet size to allow reception of 802.1q or ISL packets */
         if (sc->xl_type == XL_TYPE_905B)
            CSR_WRITE_2(sc, XL_W3_MAXPKTSIZE, XL_PACKET_SIZE);
         /* Clear out the stats counters. */

         memset( &sc->xl_stats, 0, sizeof(struct xl_stats));

         CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_STATS_DISABLE);
         sc->xl_stats_no_timeout = 1;
         xl_stats_update(sc->stat_timer_id,sc);
         sc->xl_stats_no_timeout = 0;
         XL_SEL_WIN(4);
         CSR_WRITE_2(sc, XL_W4_NET_DIAG, XL_NETDIAG_UPPER_BYTES_ENABLE);
         CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_STATS_ENABLE);


         /*
          * Enable interrupts.
          */
         CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_INTR_ACK|0xFF);
         CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_STAT_ENB|XL_INTRS);
         CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_INTR_ENB|XL_INTRS);

         /* Set the RX early threshold */
         CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_RX_SET_THRESH|(XL_PACKET_SIZE >>2));
         CSR_WRITE_4(sc, XL_DMACTL, XL_DMACTL_UP_RX_EARLY );

         /* Enable receiver and transmitter. */
         CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_TX_ENABLE);
         xl_wait(sc);
         CSR_WRITE_2(sc, XL_COMMAND, XL_CMD_RX_ENABLE);
         xl_wait(sc);

         /* Select window 7 for normal operations. */
         XL_SEL_WIN(7);

         /* schedule the stats update timer */
         rtems_timer_fire_after( sc->stat_timer_id, sc->stats_update_ticks, xl_stats_update, (void *)sc );
      }

      /*
       * Tell the world that we're running.
       */
      ifp->if_flags |= IFF_RUNNING;
   }
}







/*
 * Stop the device
 */
static void
elnk_stop (struct elnk_softc *sc)
{
   struct ifnet *ifp = &sc->arpcom.ac_if;
   int i;

   /*
    * Stop the transmitter
    */
   xl_stop(sc);
   xl_reset(sc);
   sc->tx_idle = -1;

   ifp->if_flags &= ~IFF_RUNNING;

   /*
   ** Clear out the rx & tx rings
   */
   {
      struct TXMD *chainhead;
      size_t esize;

      while( rtems_message_queue_receive( chainRecoveryQueue, &chainhead, &esize,
                                          RTEMS_NO_WAIT, 0) == RTEMS_SUCCESSFUL );
   }

   for(i=0 ; i<sc->numRxbuffers; i++)
   {
      st_le32( &sc->rx_ring[i].status, 0);
      st_le32( &sc->rx_ring[i].length, XL_LAST_FRAG | XL_PACKET_SIZE );
   }

   for(i=0 ; i<sc->numTxbuffers; i++)
   {
      st_le32( &sc->tx_ring[i].status, XL_TXSTAT_DL_COMPLETE );
      st_le32( &sc->tx_ring[i].next, 0);
      if( sc->tx_ring[i].mbuf )
      {
         m_free( sc->tx_ring[i].mbuf );
         sc->tx_ring[i].mbuf = NULL;
      }
   }
}




/*
 * Show interface statistics
 */
static void
elnk_stats (struct elnk_softc *sc)
{
   printf("    MII PHY data { anr:%04x  lpar:%04x  stat:%04x  ctl:%04x }\n",
          sc->xl_stats.miianr,
          sc->xl_stats.miipar,
          sc->xl_stats.miistatus,
          sc->xl_stats.miicmd);

   printf("         internalcfg:%08" PRIx32 "            macctl:%04x      dmactl:%08" PRIx32 "\n",
          sc->xl_stats.internalconfig,
          sc->xl_stats.mac_control,
          sc->xl_stats.dmactl);

   printf("            rxstatus:%04x              txstatus:%02x       smbstat:%04x\n",
          sc->xl_stats.rxstatus,
          sc->xl_stats.txstatus,
          sc->xl_stats.smbstatus);

   printf("              txfree:%04X             intstatus:%04x   mediastat:%04x\n",
          sc->xl_stats.txfree,
          sc->xl_stats.intstatus,
          sc->xl_stats.mediastatus);


   {
      int       i, totalLengths= 0, numLengths= 0;

      for(i=0; i< NUM_CHAIN_LENGTHS; i++)
      {
         if( sc->chain_lengths[i] > -1 )
         {
            totalLengths += sc->chain_lengths[i];
            ++numLengths;
         }
      }

      printf("          interrupts:%-9" PRIu32 "       txcmp_ints:%-5" PRIu32 "  avg_chain_len:%-4d\n",
             sc->xl_stats.device_interrupts,
             sc->xl_stats.txcomplete_ints,
             numLengths ? (totalLengths / numLengths) : -1 );
   }

   printf("        carrier_lost:%-5d             sqe_errs:%-5d\n",
          sc->xl_stats.xl_carrier_lost,
          sc->xl_stats.xl_sqe_errs);

   printf("  tx_multi_collision:%-5d  tx_single_collision:%-5d\n",
          sc->xl_stats.xl_tx_multi_collision,
          sc->xl_stats.xl_tx_single_collision);

   printf("   tx_late_collision:%-5d           rx_overrun:%-5d\n",
          sc->xl_stats.xl_tx_late_collision,
          sc->xl_stats.xl_rx_overrun);

   printf("         tx_deferred:%-5d               badssd:%-5d\n",
          sc->xl_stats.xl_tx_deferred,
          sc->xl_stats.xl_badssd);

   printf("        rx_frames_ok:%-9" PRIu32 "     tx_frames_ok:%-9" PRIu32 "\n",
          sc->xl_stats.xl_rx_frames_ok,
          sc->xl_stats.xl_tx_frames_ok);

   printf("         rx_bytes_ok:%-9" PRIu32 "      tx_bytes_ok:%-9" PRIu32 "\n",
          sc->xl_stats.xl_rx_bytes_ok,
          sc->xl_stats.xl_tx_bytes_ok );
}







/*
 * Driver ioctl handler
 */
static int
elnk_ioctl (struct ifnet *ifp, ioctl_command_t command, caddr_t data)
{
   struct elnk_softc *sc = ifp->if_softc;
   int error = 0;

   switch (command) {
      case SIOCGIFADDR:
      case SIOCSIFADDR:
         ether_ioctl (ifp, command, data);
         break;

      case SIOCSIFFLAGS:
         switch (ifp->if_flags & (IFF_UP | IFF_RUNNING)) {
            case IFF_RUNNING:
               elnk_stop (sc);
               break;

            case IFF_UP:
               elnk_init (sc);
               break;

            case IFF_UP | IFF_RUNNING:
               elnk_stop (sc);
               elnk_init (sc);
               break;

            default:
               break;
         }
         break;

      case SIO_RTEMS_SHOW_STATS:
         elnk_stats (sc);
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








#if 0
static int iftap(struct ifnet *ifp, struct ether_header *eh, struct mbuf *m )
{
   int i;
   char *delim, *pkt;

   printk("unit %i, src ", ifp->if_unit );
   for(delim= "", i=0; i< ETHER_ADDR_LEN; i++, delim=":")
      printk("%s%02x", delim, (char) eh->ether_shost[i] );

   printk(" dest ");

   for(delim= "", i=0; i< ETHER_ADDR_LEN; i++, delim=":")
      printk("%s%02x", delim, (char) eh->ether_dhost[i] );
   printk("  pkt ");

   pkt = (char *)eh;
   for(delim="", i=0; i < sizeof(struct ether_header); i++, delim=":")
      printk("%s%02x", delim, (char) pkt[i] );

   printk("\n");
   return 0;
}
#endif



struct el_boards
{
      int pbus,pdev,pfun, vid, did, tindex;
};

/*
 * Attach an ELNK driver to the system
 */
int
rtems_elnk_driver_attach (struct rtems_bsdnet_ifconfig *config, int attach)
{
   struct elnk_softc *sc;
   struct ifnet *ifp;
   char         *unitName;
   int          unitNumber;
   int          mtu, i;
   unsigned char cvalue;
   struct el_boards         sysboards[NUM_UNITS];
   int                      numFound = 0;
   int          pbus, pdev, pfun;
#if defined(__i386__)
   uint32_t	value;
   uint8_t      interrupt;
#endif
#if defined(__PPC__)
   uint32_t	lvalue;
#endif


   /*
    * Get the instance number for the board we're going to configure
    * from the user.
    */
   if( (unitNumber = rtems_bsdnet_parse_driver_name( config, &unitName)) == -1 )
   {
      return 0;
   }

   if( strcmp(unitName, DRIVER_PREFIX) )
   {
      printk("etherlink : invalid unit name '%s'\n", unitName );
      return 0;
   }

   if ((unitNumber < 1) || (unitNumber > NUM_UNITS))
   {
      printk("etherlink : unit %i is invalid, must be (1 <= n <= %d)\n", unitNumber, NUM_UNITS);
      return 0;
   }


   {
      int  done= 0, unum;

      /*
       * Run thru the list of boards, finding all that are present in
       * the system.  Sort by slot,dev - and then use the unitNumber-1
       * to index the list and select the device.  Yucky.
       */
      for( i=0; !done && xl_devs[i].xl_vid; i++)
      {
         for(unum= 1; !done &&
		pci_find_device( xl_devs[i].xl_vid, xl_devs[i].xl_did, unum-1,
                                         &sysboards[numFound].pbus,
                                         &sysboards[numFound].pdev,
                                                  &sysboards[numFound].pfun)==0; unum++)
         {
            if( numFound == NUM_UNITS )
            {
               printk("etherlink : Maximum of %d units found, extra devices ignored.\n", NUM_UNITS );
               done=-1;
            }
            else
            {
               sysboards[numFound].vid = xl_devs[i].xl_vid;
               sysboards[numFound].did = xl_devs[i].xl_did;
               sysboards[numFound].tindex = i;
               ++numFound;
            }
         }
      }

      if( ! numFound )
      {
         printk("etherlink : No Etherlink devices found\n");
         return 0;
      }

      if( unitNumber-1 >= numFound )
      {
         printk("etherlink : device '%s' not found\n", config->name );
         return 0;
      }

      /*
      * Got the list of etherlink boards in the system, now sort by
      * slot,device.  bubble sorts aren't all that wonderful, but this
      * is a short & infrequently sorted list.
      */
      if( numFound > 1 )
      {
         struct el_boards       tboard;
         int                    didsort;

         do
         {
            didsort = 0;

            for(i=1; i<numFound; i++)
            {
               if( sysboards[i-1].pbus > sysboards[i].pbus ||
                   (sysboards[i-1].pbus == sysboards[i].pbus && sysboards[i-1].pdev > sysboards[i].pdev) )
               {
                  memcpy(&tboard, &sysboards[i-1], sizeof(struct el_boards));
                  memcpy(&sysboards[i-1], &sysboards[i], sizeof(struct el_boards));
                  memcpy(&sysboards[i], &tboard, sizeof(struct el_boards));
                  didsort++;
               }
            }
         }
         while( didsort );
      }

      /*
      ** board list is sorted, now select the unit
      */

      pbus = sysboards[unitNumber-1].pbus;
      pdev = sysboards[unitNumber-1].pdev;
      pfun = sysboards[unitNumber-1].pfun;
   }

   sc = &elnk_softc[unitNumber - 1];
   ifp = &sc->arpcom.ac_if;
   if (ifp->if_softc != NULL)
   {
      printk("etherlink : unit %i already in use.\n", unitNumber );
      return 0;
   }

   /*
   ** Save various things
   */
   sc->xl_unit = unitNumber;
   sc->xl_type = sysboards[ unitNumber-1 ].tindex;

   sc->vendorID = sysboards[numFound].vid;
   sc->deviceID = sysboards[numFound].did;

   sc->numRxbuffers = (config->rbuf_count) ? config->rbuf_count : RX_RING_SIZE;
   sc->numTxbuffers = (config->xbuf_count) ? config->xbuf_count : TX_RING_SIZE;


   for(i=0; i< NUM_CHAIN_LENGTHS; i++) sc->chain_lengths[i]= -1;
   sc->chlenIndex = 0;


   if (config->mtu)
      mtu = config->mtu;
   else
      mtu = ETHERMTU;

   sc->acceptBroadcast = !config->ignore_broadcast;



#ifdef ELNK_DEBUG
   printk("etherlink : device '%s', name 'elnk%d', pci %02x:%02x.%02x, %d rx/%d tx buffers\n",
          xl_devs[sc->xl_type].xl_name, sc->xl_unit,
          pbus, pdev, pfun,
          sc->numRxbuffers, sc->numTxbuffers);
#endif


   /*
   ** Create this unit's stats timer
   */
   if( rtems_timer_create( rtems_build_name( 'X', 'L', 't', (char)(sc->xl_unit & 255)),
                           &sc->stat_timer_id ) != RTEMS_SUCCESSFUL )
   {
      printk("etherlink : unit elnk%d unable to create stats timer\n", sc->xl_unit );
      return 0;
   }

   /* update stats 1 times/second if things aren't incrementing fast
    * enough to trigger stats interrupts
    */
   sc->stats_update_ticks = rtems_clock_get_ticks_per_second();

   /*
   ** Get this unit's rx/tx event
   */
   sc->ioevent = unit_signals[unitNumber-1];


#if defined(__i386__)
   pci_read_config_dword(pbus, pdev, pfun, 16, &value);
   sc->ioaddr = value & ~IO_MASK;

   pci_read_config_byte(pbus, pdev, pfun, 60, &interrupt);
   cvalue = interrupt;
#endif
#if defined(__PPC__)
   /*
   ** Prep the board
   */
   pci_write_config_word(pbus, pdev, pfun,
                         PCI_COMMAND,
                         (uint16_t)( PCI_COMMAND_IO |
                                       PCI_COMMAND_MASTER |
                                       PCI_COMMAND_INVALIDATE |
                                       PCI_COMMAND_WAIT ) );
   /*
    * Get the device's base address
    */
   pci_read_config_dword(pbus, pdev, pfun,
                         PCI_BASE_ADDRESS_0,
                         &lvalue);

   sc->ioaddr = (uint32_t)lvalue & PCI_BASE_ADDRESS_IO_MASK;
   /*
   ** Store the interrupt name, we'll use it later when we initialize
   ** the board.
   */
   pci_read_config_byte(pbus, pdev, pfun,
                        PCI_INTERRUPT_LINE,
                        &cvalue);
#endif

   memset(&sc->irqInfo,0,sizeof(rtems_irq_connect_data));
   sc->irqInfo.name = cvalue;


   /*
   ** Establish basic board config, set node address from config or
   ** board eeprom, do stuff with additional device properties
   */

   {
      uint8_t   pci_latency;
      uint8_t   new_latency = 248;

      /* Check the PCI latency value.  On the 3c590 series the latency timer
         must be set to the maximum value to avoid data corruption that occurs
         when the timer expires during a transfer.  This bug exists the Vortex
         chip only. */
#if defined(__i386__)
      pci_read_config_byte(pbus, pdev, pfun, 0x0d, &pci_latency);
#endif
#if defined(__PPC__)
      pci_read_config_byte(pbus,pdev,pfun, PCI_LATENCY_TIMER, &pci_latency);
#endif
      if (pci_latency < new_latency)
      {
         printk("etherlink : unit elnk%d Overriding PCI latency, timer (CFLT) setting of %d, new value is %d.\n", sc->xl_unit, pci_latency, new_latency );
#if defined(__i386__)
         pci_write_config_byte(pbus, pdev, pfun, 0x0d, new_latency);
#endif
#if defined(__PPC__)
         pci_write_config_byte(pbus,pdev,pfun, PCI_LATENCY_TIMER, new_latency);
#endif
      }
   }

   /* Reset the adapter. */
   xl_reset(sc);


   {
      u_int16_t		xcvr[2];
      u_char            eaddr[ETHER_ADDR_LEN];

      sc->xl_flags = 0;
      if (sc->deviceID == TC_DEVICEID_HURRICANE_555)
         sc->xl_flags |= XL_FLAG_EEPROM_OFFSET_30 | XL_FLAG_PHYOK;
      if (sc->deviceID == TC_DEVICEID_HURRICANE_556 ||
          sc->deviceID == TC_DEVICEID_HURRICANE_556B)
         sc->xl_flags |= XL_FLAG_FUNCREG | XL_FLAG_PHYOK |
            XL_FLAG_EEPROM_OFFSET_30 | XL_FLAG_WEIRDRESET |
            XL_FLAG_INVERT_LED_PWR | XL_FLAG_INVERT_MII_PWR;
      if (sc->deviceID == TC_DEVICEID_HURRICANE_555 ||
          sc->deviceID == TC_DEVICEID_HURRICANE_556)
         sc->xl_flags |= XL_FLAG_8BITROM;
      if (sc->deviceID == TC_DEVICEID_HURRICANE_556B)
         sc->xl_flags |= XL_FLAG_NO_XCVR_PWR;

      if (sc->deviceID == TC_DEVICEID_HURRICANE_575A ||
          sc->deviceID == TC_DEVICEID_HURRICANE_575B ||
          sc->deviceID == TC_DEVICEID_HURRICANE_575C ||
          sc->deviceID == TC_DEVICEID_HURRICANE_656B ||
          sc->deviceID == TC_DEVICEID_TORNADO_656C)
         sc->xl_flags |= XL_FLAG_FUNCREG | XL_FLAG_PHYOK |
            XL_FLAG_EEPROM_OFFSET_30 | XL_FLAG_8BITROM;
      if (sc->deviceID == TC_DEVICEID_HURRICANE_656)
         sc->xl_flags |= XL_FLAG_FUNCREG | XL_FLAG_PHYOK;
      if (sc->deviceID == TC_DEVICEID_HURRICANE_575B)
         sc->xl_flags |= XL_FLAG_INVERT_LED_PWR;
      if (sc->deviceID == TC_DEVICEID_HURRICANE_575C)
         sc->xl_flags |= XL_FLAG_INVERT_MII_PWR;
      if (sc->deviceID == TC_DEVICEID_TORNADO_656C)
         sc->xl_flags |= XL_FLAG_INVERT_MII_PWR;
      if (sc->deviceID == TC_DEVICEID_HURRICANE_656 ||
          sc->deviceID == TC_DEVICEID_HURRICANE_656B)
         sc->xl_flags |= XL_FLAG_INVERT_MII_PWR |
            XL_FLAG_INVERT_LED_PWR;
      if (sc->deviceID == TC_DEVICEID_TORNADO_10_100BT_920B)
         sc->xl_flags |= XL_FLAG_PHYOK;


      if (config->hardware_address)
      {
         memcpy(sc->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);
      }
      else
      {
         if (xl_read_eeprom(sc, (caddr_t)&eaddr, XL_EE_OEM_ADR0, 3, 1))
         {
            printk("etherlink : unit elnk%d Failed to read station address\n", sc->xl_unit );
            return 0;
         }
         memcpy((char *)&sc->arpcom.ac_enaddr, eaddr, ETHER_ADDR_LEN);
      }

      /*
       * Figure out the card type. 3c905B adapters have the
       * 'supportsNoTxLength' bit set in the capabilities
       * word in the EEPROM.
       */
      xl_read_eeprom(sc, (caddr_t)&sc->xl_caps, XL_EE_CAPS, 1, 0);
      if (sc->xl_caps & XL_CAPS_NO_TXLENGTH)
         sc->xl_type = XL_TYPE_905B;
      else
         sc->xl_type = XL_TYPE_90X;


      /*
       * Now we have to see what sort of media we have.
       * This includes probing for an MII interace and a
       * possible PHY.
       */
      XL_SEL_WIN(3);
      sc->xl_media = CSR_READ_2(sc, XL_W3_MEDIA_OPT);

      xl_read_eeprom(sc, (char *)&xcvr, XL_EE_ICFG_0, 2, 0);
      sc->xl_xcvr = xcvr[0] | xcvr[1] << 16;
      sc->xl_xcvr &= XL_ICFG_CONNECTOR_MASK;
      sc->xl_xcvr >>= XL_ICFG_CONNECTOR_BITS;

#if 0
      printk("etherlink : unit elnk%d EEPROM set xcvr to 0x%x\n", sc->xl_unit, sc->xl_xcvr);
#endif

      {
         char   msg[255];
         int    i;

         struct _availmedia
         {
               int bit;
               char *name;
         } _am[]= {{ XL_MEDIAOPT_BT4, "100BaseT4" },
                   { XL_MEDIAOPT_BTX, "100BaseTX" },
                   { XL_MEDIAOPT_BFX, "100BaseFX" },
                   { XL_MEDIAOPT_BT,  "10BaseT" },
                   { XL_MEDIAOPT_BNC, "10Base2" },
                   { XL_MEDIAOPT_AUI, "10mbps AUI"},
                   { XL_MEDIAOPT_MII, "MII"},
                   { 0, NULL }};

         msg[0]= 0;
         for( i=0; _am[i].bit; i++)
         {
            if( sc->xl_media & _am[i].bit )
               sprintf( &msg[strlen(msg)], ",%s", _am[i].name );
         }
         if( !strlen(msg) ) strcpy( &msg[1], "<no media bits>");

         printk("etherlink : unit elnk%d available media : %s\n", sc->xl_unit, &msg[1]);
      }

      XL_SEL_WIN(7);
   }



   /*
    * Set up network interface
    */
   ifp->if_softc = sc;
   ifp->if_name = unitName;
   ifp->if_unit = sc->xl_unit;
   ifp->if_mtu = mtu;
   ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX;
   if (ifp->if_snd.ifq_maxlen == 0)
      ifp->if_snd.ifq_maxlen = ifqmaxlen;
   ifp->if_init = elnk_init;
   ifp->if_start = elnk_start;
   ifp->if_ioctl = elnk_ioctl;
   ifp->if_output = ether_output;

#if 0
   ifp->if_tap = iftap;
#endif

   /*
    * Attach the interface
    */
   if_attach (ifp);
   ether_ifattach (ifp);

#ifdef ELNK_DEBUG
   printk( "etherlink : unit elnk%d driver attached\n", sc->xl_unit );
#endif

   /*
    * Start driver tasks if this is the first unit initialized
    */
   if (txDaemonTid == 0)
   {
      if( rtems_message_queue_create( rtems_build_name('X','L','c','r'),
                                      sc->numTxbuffers+1,
                                      sizeof(struct TXMD *),
                                      RTEMS_FIFO | RTEMS_LOCAL,
                                      &chainRecoveryQueue ) != RTEMS_SUCCESSFUL )
      {
         rtems_panic( "etherlink : Unable to create TX buffer recovery queue\n" );
      }


      rxDaemonTid = rtems_bsdnet_newproc( "XLrx", 4096,
                                          elnk_rxDaemon, NULL);

      txDaemonTid = rtems_bsdnet_newproc( "XLtx", 4096,
                                          elnk_txDaemon, NULL);
#ifdef ELNK_DEBUG
      printk( "etherlink : driver tasks created\n" );
#endif
   }

   return 1;
};

#endif /* ELNK_SUPPORTED */

/* eof */
