/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Header file for RTEMS GRSLINK SLINK master driver 
 * 
 * COPYRIGHT (c) 2009.
 * Cobham Gaisler AB.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __GRSLINK_H__
#define __GRSLINK_H__

#ifdef __cplusplus
extern "C" {
#endif

/**** Configuration ****/
/* Collect statistics ? */
#define SLINK_COLLECT_STATISTICS

/* Frequency of SLINK SCLK */
#define SLINK_FREQ_HZ 6000000
/* Number of queues used in driver */
#define SLINK_NUMQUEUES 4

/* The four values below are only used in the demo software */
#define SLINK_CORE_REGBASE 0x80000600
#define SLINK_CORE_IRQ 6
#define IRQ_CNTRL_REG 0x80000200
#define IRQ_CNTRL_MASK_OFFSET 0x40

/* 
 * Structure returned by SLINK_statistics if SLINK_COLLECT_STATISTCS has
 * been defined
 */
typedef struct {
	unsigned int parerr;     /* Number of parity errors */
	unsigned int recov;      /* Number of receive overflows */
	unsigned int reads;      /* Number of completed READs */
	unsigned int writes;     /* Number of performed WRITES */
	unsigned int sequences;  /* Number of started SEQUENCEs */
	unsigned int seqcomp;    /* Number of completed SEQUENCEs */
	unsigned int interrupts; /* Number of INTERRUPT transfers */
	unsigned int lostwords;  /* Number of lost words due to full queue */
} SLINK_stats;

/**** SLINK status codes ****/
#define SLINK_ABORTED   0
#define SLINK_QFULL     1
#define SLINK_ACTIVE    2
#define SLINK_AMBAERR   3
#define SLINK_COMPLETED 4
#define SLINK_PARERR    5
#define SLINK_ROV       6 /* Only used internally in driver */

/**** SLINK master register fields *****/
/* Control register */
#define SLINK_C_SLEN_POS 16
#define SLINK_C_SRO  (1 << 8)
#define SLINK_C_SCN_POS 4
#define SLINK_C_PAR  (1 << 3)
#define SLINK_C_AS   (1 << 2)
#define SLINK_C_SE   (1 << 1)
#define SLINK_C_SLE  (1 << 0)

/* Status register fields */
#define SLINK_S_SI_POS 16
#define SLINK_S_PERR (1 << 7)
#define SLINK_S_AERR (1 << 6)
#define SLINK_S_ROV  (1 << 5)
#define SLINK_S_RNE  (1 << 4)
#define SLINK_S_TNF  (1 << 3)
#define SLINK_S_SC   (1 << 2)
#define SLINK_S_SA   (1 << 1)
#define SLINK_S_SRX  (1 << 0)

/* Mask register fields */
#define SLINK_M_PERRE (1 << 7)
#define SLINK_M_AERRE (1 << 6)
#define SLINK_M_ROVE  (1 << 5)
#define SLINK_M_RNEE  (1 << 4)
#define SLINK_M_TNFE  (1 << 3)
#define SLINK_M_SCE   (1 << 2)
#define SLINK_M_SAE   (1 << 1)
#define SLINK_M_SRXE  (1 << 0)

/**** Macros ****/
/* Get channel field from received SLINK word */
#define SLINK_WRD_CHAN(x) ((x >> 16) & 0xF)
/* Get IO card # from received SLINK word */
#define SLINK_WRD_CARDNUM(x) ((x >> 21) & 0x3)
/* Get data part from SLINK word */
#define SLINK_WRD_PAYLOAD(x) (x & 0xFFFF)

/* Checks status value to see if transmit queue has free slot */
#define SLINK_STS_TRANSFREE(x) (x & SLINK_S_TNF)
/* Get Sequence Index value */
#define SLINK_STS_SI(x) ((x >> 16) & 0xFF)

/**** Function declarations, driver interface ****/
/* Initializes the SLINK core */
int SLINK_init(unsigned int nullwrd, int parity, int qsize,
	       void (*interrupt_trans_handler)(int),
	       void (*sequence_callback)(int));

/* Enables the core */
void SLINK_start(void);

/* Disables the core */
void SLINK_stop(void);

/* Reads one word */
int SLINK_read(int data, int channel, int *reply);

/* Writes one word */
int SLINK_write(int data, int channel);

/* Peforms a SEQUENCE */
int SLINK_seqstart(int *a, int *b, int n, int channel, int reconly);

/* Aborts a SEQUENCE */
void SLINK_seqabort(void);

/* Status of current or last SEQUENCE */
int SLINK_seqstatus(void);

/* Number of words transferred in last SEQUENCE */
int SLINK_seqwrds(void);

/* Returns value of core's status register */
int SLINK_hwstatus(void);

/* Returns number of elements in queue associated with IO card */
int SLINK_queuestatus(int iocard);

/* Take first element from queue for IO card # 'iocard' */
int SLINK_dequeue(int iocard, int *elem);

/* Returns structure containing core driver statistics */
SLINK_stats *SLINK_statistics(void);

#ifdef __cplusplus
}
#endif

#endif /* __GRSLINK_H__ */
