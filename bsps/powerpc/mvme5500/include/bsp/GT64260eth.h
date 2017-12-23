/* GT64260eth.h
 *
 * Copyright (c) 2002 Allegro Networks, Inc., Wasabi Systems, Inc.
 * All rights reserved.
 *
 * RTEMS/Mvme5500 port 2004  by S. Kate Feng, <feng1@bnl.gov>,
 * All rights reserved.
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
 *      This product includes software developed for the NetBSD Project by
 *      Allegro Networks, Inc., and Wasabi Systems, Inc.
 * 4. The name of Allegro Networks, Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 * 5. The name of Wasabi Systems, Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ALLEGRO NETWORKS, INC. AND
 * WASABI SYSTEMS, INC. ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL EITHER ALLEGRO NETWORKS, INC. OR WASABI SYSTEMS, INC.
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* Keep the ring sizes a power of two for efficiency.
   Making the Tx ring too long decreases the effectiveness of channel
   bonding and packet priority.
   There are no ill effects from too-large receive rings. */
#define TX_RING_SIZE	32
#define GT_NEXTTX(x)    ((x + 1) % TX_RING_SIZE )
#define TX_QUARTER_FULL	TX_RING_SIZE/2
#define TX_HALF_FULL	TX_RING_SIZE/2
#define RX_RING_SIZE	16
#define HASH_TABLE_SIZE 16
#define HASH_DRAM_SIZE  HASH_TABLE_SIZE*1024  /* size of DRAM for hash table */
#define INTR_ERR_SIZE   16

enum GTeth_txprio {
	GE_TXPRIO_HI=1,
	GE_TXPRIO_LO=0,
	GE_TXPRIO_NONE=2
};
enum GTeth_rxprio {
	GE_RXPRIO_HI=3,
	GE_RXPRIO_MEDHI=2,
	GE_RXPRIO_MEDLO=1,
	GE_RXPRIO_LO=0
};

struct GTeth_softc {
  struct GTeth_desc txq_desc[TX_RING_SIZE]; /* transmit descriptor memory */
  struct GTeth_desc rxq_desc[RX_RING_SIZE]; /* receive descriptor memory */
  struct mbuf* txq_mbuf[TX_RING_SIZE];	/* transmit buffer memory */
  struct mbuf* rxq_mbuf[RX_RING_SIZE];	/* receive buffer memory */
  struct GTeth_softc *next_module;
  volatile unsigned int intr_errsts[INTR_ERR_SIZE]; /* capture the right intr_status */
  unsigned int intr_err_ptr1;   /* ptr used in GTeth_error() */
  unsigned int intr_err_ptr2;   /* ptr used in ISR */
  struct ifqueue txq_pendq;	/* these are ready to go to the GT */
  unsigned int txq_pending;
  unsigned int txq_lo;		/* next to be given to GT DMA */
  unsigned int txq_fi; 		/* next to be free */
  unsigned int txq_to_cpu;      /* next to be returned to CPU */
  unsigned int txq_ei_gapcount;	/* counter until next EI */
  unsigned int txq_nactive;	/* number of active descriptors */
  unsigned int txq_nintr;       /* number of txq desc. send TX_EVENT */
  unsigned int txq_outptr;	/* where to put next transmit packet */
  unsigned int txq_inptr;       /* start of 1st queued tx packet */
  unsigned int txq_free;	/* free Tx queue slots. */
  unsigned txq_intrbits;	/* bits to write to EIMR */
  unsigned txq_esdcmrbits;	/* bits to write to ESDCMR */
  unsigned txq_epsrbits;	/* bits to test with EPSR */

  caddr_t txq_ectdp;		/* offset to cur. tx desc ptr reg */
  unsigned long txq_desc_busaddr;	/* bus addr of tx descriptors */
  caddr_t txq_buf_busaddr;	/* bus addr of tx buffers */

  struct mbuf *rxq_curpkt;	/* mbuf for current packet */
  struct GTeth_desc *rxq_head_desc;  /* rxq head descriptor */
  unsigned int rxq_fi; 		/* next to be returned to CPU */
  unsigned int rxq_active;	/* # of descriptors given to GT */
  unsigned rxq_intrbits;		/* bits to write to EIMR */
  unsigned long rxq_desc_busaddr;	/* bus addr of rx descriptors */

  struct arpcom arpcom;	        /* rtems if structure, contains ifnet */
  int sc_macno;			/* which mac? 0, 1, or 2 */

  unsigned int sc_tickflags;
  #define	GE_TICK_TX_IFSTART	0x0001
  #define	GE_TICK_RX_RESTART	0x0002
  unsigned int sc_flags;
  #define	GE_ALLMULTI	0x0001
  #define	GE_PHYSTSCHG	0x0002
  #define	GE_RXACTIVE	0x0004
  unsigned sc_pcr;		/* current EPCR value */
  unsigned sc_pcxr;		/* current EPCXR value */
  unsigned sc_intrmask;		/* current EIMR value */
  unsigned sc_idlemask;		/* suspended EIMR bits */
  unsigned sc_max_frame_length;	/* maximum frame length */
  unsigned rx_buf_sz;

  /* Hash table related members */
  unsigned long long *sc_hashtable;
  unsigned int sc_hashmask;	/* 0x1ff or 0x1fff */

  rtems_id	daemonTid;
  rtems_id	daemonSync; /* synchronization with the daemon */
  /* statistics */
  struct {
    volatile unsigned long       rxInterrupts;

    volatile unsigned long       txInterrupts;
    unsigned long	txMultiBuffPacket;
    unsigned long       txMultiMaxLen;
    unsigned long       txSinglMaxLen;
    unsigned long       txMultiMaxLoop;
    unsigned long       txBuffMaxLen;
    unsigned long 	length_errors;
    unsigned long	frame_errors;
    unsigned long	crc_errors;
    unsigned long       or_errors; /* overrun error */
  } stats;
};
