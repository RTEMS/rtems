/*
 * MPC8xx Communication Processor Module.
 * Copyright (c) 1997 Dan Malek (dmalek@jlc.net)
 *
 * This file contains structures and information for the communication
 * processor channels.  Some CPM control and status is available
 * throught the MPC8xx internal memory map.  See immap.h for details.
 * This file only contains what I need for the moment, not the total
 * CPM capabilities.  I (or someone else) will add definitions as they
 * are needed.  -- Dan
 *
 * On the MBX board, EPPC-Bug loads CPM microcode into the first 512
 * bytes of the DP RAM and relocates the I2C parameter area to the
 * IDMA1 space.  The remaining DP RAM is available for buffer descriptors
 * or other use.
 */
#ifndef __CPM_8XX__
#define __CPM_8XX__

#include <bsp/8xx_immap.h>

/* CPM Command register.
*/
#define CPM_CR_RST	((unsigned short)0x8000)
#define CPM_CR_OPCODE	((unsigned short)0x0f00)
#define CPM_CR_CHAN	((unsigned short)0x00f0)
#define CPM_CR_FLG	((unsigned short)0x0001)

/* Some commands (there are more...later)
*/
#define CPM_CR_INIT_TRX		((unsigned short)0x0000)
#define CPM_CR_INIT_RX		((unsigned short)0x0001)
#define CPM_CR_INIT_TX		((unsigned short)0x0002)
#define CPM_CR_STOP_TX		((unsigned short)0x0004)
#define CPM_CR_RESTART_TX	((unsigned short)0x0006)
#define CPM_CR_SET_GADDR	((unsigned short)0x0008)

/* Channel numbers.
*/
#define CPM_CR_CH_SCC1	((unsigned short)0x0000)
#define CPM_CR_CH_I2C	((unsigned short)0x0001)	/* I2C and IDMA1 */
#define CPM_CR_CH_SCC2	((unsigned short)0x0004)
#define CPM_CR_CH_SPI	((unsigned short)0x0005)	/* SPI / IDMA2 / Timers */
#define CPM_CR_CH_SCC3	((unsigned short)0x0008)
#define CPM_CR_CH_SMC1	((unsigned short)0x0009)	/* SMC1 / DSP1 */
#define CPM_CR_CH_SCC4	((unsigned short)0x000c)
#define CPM_CR_CH_SMC2	((unsigned short)0x000d)	/* SMC2 / DSP2 */

#define mk_cr_cmd(CH, CMD)	((CMD << 8) | (CH << 4))

/* The dual ported RAM is multi-functional.  Some areas can be (and are
 * being) used for microcode.  There is an area that can only be used
 * as data ram for buffer descriptors, which is all we use right now.
 * Currently the first 512 and last 256 bytes are used for microcode.
 */
#define CPM_DATAONLY_BASE	((unsigned int)0x0800)
#define CPM_DATAONLY_SIZE	((unsigned int)0x0700)
#define CPM_DP_NOSPACE		((unsigned int)0x7fffffff)

/* Export the base address of the communication processor registers
 * and dual port ram.
 */
unsigned int		m8xx_cpm_dpalloc(unsigned int size);
unsigned int		m8xx_cpm_hostalloc(unsigned int size);
void			m8xx_cpm_setbrg(unsigned int brg, unsigned int rate);

/* Buffer descriptors used by many of the CPM protocols.
*/
typedef struct cpm_buf_desc {
	unsigned short	cbd_sc;		/* Status and Control */
	unsigned short	cbd_datlen;	/* Data length in buffer */
	unsigned int	cbd_bufaddr;	/* Buffer address in host memory */
} cbd_t;

#define BD_SC_EMPTY	((unsigned short)0x8000)	/* Recieve is empty */
#define BD_SC_READY	((unsigned short)0x8000)	/* Transmit is ready */
#define BD_SC_WRAP	((unsigned short)0x2000)	/* Last buffer descriptor */
#define BD_SC_INTRPT	((unsigned short)0x1000)	/* Interrupt on change */
#define BD_SC_CM	((unsigned short)0x0200)	/* Continous mode */
#define BD_SC_ID	((unsigned short)0x0100)	/* Rec'd too many idles */
#define BD_SC_P		((unsigned short)0x0100)	/* xmt preamble */
#define BD_SC_BR	((unsigned short)0x0020)	/* Break received */
#define BD_SC_FR	((unsigned short)0x0010)	/* Framing error */
#define BD_SC_PR	((unsigned short)0x0008)	/* Parity error */
#define BD_SC_OV	((unsigned short)0x0002)	/* Overrun */
#define BD_SC_CD	((unsigned short)0x0001)	/* ?? */

/* Parameter RAM offsets.
*/
#define PROFF_SCC1	((unsigned int)0x0000)
#define PROFF_SCC2	((unsigned int)0x0100)
#define PROFF_SCC3	((unsigned int)0x0200)
#define PROFF_SMC1	((unsigned int)0x0280)
#define PROFF_SCC4	((unsigned int)0x0300)
#define PROFF_SMC2	((unsigned int)0x0380)

/* Define enough so I can at least use the serial port as a UART.
 */
typedef struct smc_uart {
	unsigned short	smc_rbase;	/* Rx Buffer descriptor base address */
	unsigned short	smc_tbase;	/* Tx Buffer descriptor base address */
	unsigned char	smc_rfcr;	/* Rx function code */
	unsigned char	smc_tfcr;	/* Tx function code */
	unsigned short	smc_mrblr;	/* Max receive buffer length */
	unsigned int	smc_rstate;	/* Internal */
	unsigned int	smc_idp;	/* Internal */
	unsigned short	smc_rbptr;	/* Internal */
	unsigned short	smc_ibc;	/* Internal */
	unsigned int	smc_rxtmp;	/* Internal */
	unsigned int	smc_tstate;	/* Internal */
	unsigned int	smc_tdp;	/* Internal */
	unsigned short	smc_tbptr;	/* Internal */
	unsigned short	smc_tbc;	/* Internal */
	unsigned int	smc_txtmp;	/* Internal */
	unsigned short	smc_maxidl;	/* Maximum idle characters */
	unsigned short	smc_tmpidl;	/* Temporary idle counter */
	unsigned short	smc_brklen;	/* Last received break length */
	unsigned short	smc_brkec;	/* rcv'd break condition counter */
	unsigned short	smc_brkcr;	/* xmt break count register */
	unsigned short	smc_rmask;	/* Temporary bit mask */
} smc_uart_t;

/* Function code bits.
*/
#define SMC_EB	((unsigned char)0x10)	/* Set big endian byte order */

/* SMC uart mode register.
*/
#define	SMCMR_REN	((unsigned short)0x0001)
#define SMCMR_TEN	((unsigned short)0x0002)
#define SMCMR_DM	((unsigned short)0x000c)
#define SMCMR_SM_GCI	((unsigned short)0x0000)
#define SMCMR_SM_UART	((unsigned short)0x0020)
#define SMCMR_SM_TRANS	((unsigned short)0x0030)
#define SMCMR_SM_MASK	((unsigned short)0x0030)
#define SMCMR_PM_EVEN	((unsigned short)0x0100)	/* Even parity, else odd */
#define SMCMR_PEN	((unsigned short)0x0200)	/* Parity enable */
#define SMCMR_SL	((unsigned short)0x0400)	/* Two stops, else one */
#define SMCR_CLEN_MASK	((unsigned short)0x7800)	/* Character length */
#define smcr_mk_clen(C)	(((C) << 11) & SMCR_CLEN_MASK)

/* SMC Event and Mask register.
*/
#define	SMCM_TXE	((unsigned char)0x10)
#define	SMCM_BSY	((unsigned char)0x04)
#define	SMCM_TX		((unsigned char)0x02)
#define	SMCM_RX		((unsigned char)0x01)

/* Baud rate generators.
*/
#define CPM_BRG_RST		((unsigned int)0x00020000)
#define CPM_BRG_EN		((unsigned int)0x00010000)
#define CPM_BRG_EXTC_INT	((unsigned int)0x00000000)
#define CPM_BRG_EXTC_CLK2	((unsigned int)0x00004000)
#define CPM_BRG_EXTC_CLK6	((unsigned int)0x00008000)
#define CPM_BRG_ATB		((unsigned int)0x00002000)
#define CPM_BRG_CD_MASK		((unsigned int)0x00001ffe)
#define CPM_BRG_DIV16		((unsigned int)0x00000001)

/* SCCs.
*/
#define SCC_GSMRH_IRP		((unsigned int)0x00040000)
#define SCC_GSMRH_GDE		((unsigned int)0x00010000)
#define SCC_GSMRH_TCRC_CCITT	((unsigned int)0x00008000)
#define SCC_GSMRH_TCRC_BISYNC	((unsigned int)0x00004000)
#define SCC_GSMRH_TCRC_HDLC	((unsigned int)0x00000000)
#define SCC_GSMRH_REVD		((unsigned int)0x00002000)
#define SCC_GSMRH_TRX		((unsigned int)0x00001000)
#define SCC_GSMRH_TTX		((unsigned int)0x00000800)
#define SCC_GSMRH_CDP		((unsigned int)0x00000400)
#define SCC_GSMRH_CTSP		((unsigned int)0x00000200)
#define SCC_GSMRH_CDS		((unsigned int)0x00000100)
#define SCC_GSMRH_CTSS		((unsigned int)0x00000080)
#define SCC_GSMRH_TFL		((unsigned int)0x00000040)
#define SCC_GSMRH_RFW		((unsigned int)0x00000020)
#define SCC_GSMRH_TXSY		((unsigned int)0x00000010)
#define SCC_GSMRH_SYNL16	((unsigned int)0x0000000c)
#define SCC_GSMRH_SYNL8		((unsigned int)0x00000008)
#define SCC_GSMRH_SYNL4		((unsigned int)0x00000004)
#define SCC_GSMRH_RTSM		((unsigned int)0x00000002)
#define SCC_GSMRH_RSYN		((unsigned int)0x00000001)

#define SCC_GSMRL_SIR		((unsigned int)0x80000000)	/* SCC2 only */
#define SCC_GSMRL_EDGE_NONE	((unsigned int)0x60000000)
#define SCC_GSMRL_EDGE_NEG	((unsigned int)0x40000000)
#define SCC_GSMRL_EDGE_POS	((unsigned int)0x20000000)
#define SCC_GSMRL_EDGE_BOTH	((unsigned int)0x00000000)
#define SCC_GSMRL_TCI		((unsigned int)0x10000000)
#define SCC_GSMRL_TSNC_3	((unsigned int)0x0c000000)
#define SCC_GSMRL_TSNC_4	((unsigned int)0x08000000)
#define SCC_GSMRL_TSNC_14	((unsigned int)0x04000000)
#define SCC_GSMRL_TSNC_INF	((unsigned int)0x00000000)
#define SCC_GSMRL_RINV		((unsigned int)0x02000000)
#define SCC_GSMRL_TINV		((unsigned int)0x01000000)
#define SCC_GSMRL_TPL_128	((unsigned int)0x00c00000)
#define SCC_GSMRL_TPL_64	((unsigned int)0x00a00000)
#define SCC_GSMRL_TPL_48	((unsigned int)0x00800000)
#define SCC_GSMRL_TPL_32	((unsigned int)0x00600000)
#define SCC_GSMRL_TPL_16	((unsigned int)0x00400000)
#define SCC_GSMRL_TPL_8		((unsigned int)0x00200000)
#define SCC_GSMRL_TPL_NONE	((unsigned int)0x00000000)
#define SCC_GSMRL_TPP_ALL1	((unsigned int)0x00180000)
#define SCC_GSMRL_TPP_01	((unsigned int)0x00100000)
#define SCC_GSMRL_TPP_10	((unsigned int)0x00080000)
#define SCC_GSMRL_TPP_ZEROS	((unsigned int)0x00000000)
#define SCC_GSMRL_TEND		((unsigned int)0x00040000)
#define SCC_GSMRL_TDCR_32	((unsigned int)0x00030000)
#define SCC_GSMRL_TDCR_16	((unsigned int)0x00020000)
#define SCC_GSMRL_TDCR_8	((unsigned int)0x00010000)
#define SCC_GSMRL_TDCR_1	((unsigned int)0x00000000)
#define SCC_GSMRL_RDCR_32	((unsigned int)0x0000c000)
#define SCC_GSMRL_RDCR_16	((unsigned int)0x00008000)
#define SCC_GSMRL_RDCR_8	((unsigned int)0x00004000)
#define SCC_GSMRL_RDCR_1	((unsigned int)0x00000000)
#define SCC_GSMRL_RENC_DFMAN	((unsigned int)0x00003000)
#define SCC_GSMRL_RENC_MANCH	((unsigned int)0x00002000)
#define SCC_GSMRL_RENC_FM0	((unsigned int)0x00001000)
#define SCC_GSMRL_RENC_NRZI	((unsigned int)0x00000800)
#define SCC_GSMRL_RENC_NRZ	((unsigned int)0x00000000)
#define SCC_GSMRL_TENC_DFMAN	((unsigned int)0x00000600)
#define SCC_GSMRL_TENC_MANCH	((unsigned int)0x00000400)
#define SCC_GSMRL_TENC_FM0	((unsigned int)0x00000200)
#define SCC_GSMRL_TENC_NRZI	((unsigned int)0x00000100)
#define SCC_GSMRL_TENC_NRZ	((unsigned int)0x00000000)
#define SCC_GSMRL_DIAG_LE	((unsigned int)0x000000c0)	/* Loop and echo */
#define SCC_GSMRL_DIAG_ECHO	((unsigned int)0x00000080)
#define SCC_GSMRL_DIAG_LOOP	((unsigned int)0x00000040)
#define SCC_GSMRL_DIAG_NORM	((unsigned int)0x00000000)
#define SCC_GSMRL_ENR		((unsigned int)0x00000020)
#define SCC_GSMRL_ENT		((unsigned int)0x00000010)
#define SCC_GSMRL_MODE_ENET	((unsigned int)0x0000000c)
#define SCC_GSMRL_MODE_DDCMP	((unsigned int)0x00000009)
#define SCC_GSMRL_MODE_BISYNC	((unsigned int)0x00000008)
#define SCC_GSMRL_MODE_V14	((unsigned int)0x00000007)
#define SCC_GSMRL_MODE_AHDLC	((unsigned int)0x00000006)
#define SCC_GSMRL_MODE_PROFIBUS	((unsigned int)0x00000005)
#define SCC_GSMRL_MODE_UART	((unsigned int)0x00000004)
#define SCC_GSMRL_MODE_SS7	((unsigned int)0x00000003)
#define SCC_GSMRL_MODE_ATALK	((unsigned int)0x00000002)
#define SCC_GSMRL_MODE_HDLC	((unsigned int)0x00000000)

#define SCC_TODR_TOD		((unsigned short)0x8000)

/* SCC Event and Mask register.
*/
#define	SCCM_TXE	((unsigned char)0x10)
#define	SCCM_BSY	((unsigned char)0x04)
#define	SCCM_TX		((unsigned char)0x02)
#define	SCCM_RX		((unsigned char)0x01)

typedef struct scc_param {
	unsigned short	scc_rbase;	/* Rx Buffer descriptor base address */
	unsigned short	scc_tbase;	/* Tx Buffer descriptor base address */
	unsigned char	scc_rfcr;	/* Rx function code */
	unsigned char	scc_tfcr;	/* Tx function code */
	unsigned short	scc_mrblr;	/* Max receive buffer length */
	unsigned int	scc_rstate;	/* Internal */
	unsigned int	scc_idp;	/* Internal */
	unsigned short	scc_rbptr;	/* Internal */
	unsigned short	scc_ibc;	/* Internal */
	unsigned int	scc_rxtmp;	/* Internal */
	unsigned int	scc_tstate;	/* Internal */
	unsigned int	scc_tdp;	/* Internal */
	unsigned short	scc_tbptr;	/* Internal */
	unsigned short	scc_tbc;	/* Internal */
	unsigned int	scc_txtmp;	/* Internal */
	unsigned int	scc_rcrc;	/* Internal */
	unsigned int	scc_tcrc;	/* Internal */
} sccp_t;

/* Function code bits.
*/
#define SCC_EB	((unsigned char)0x10)	/* Set big endian byte order */

/* CPM Ethernet through SCC1.
 */
typedef struct scc_enet {
	sccp_t	sen_genscc;
	unsigned int	sen_cpres;	/* Preset CRC */
	unsigned int	sen_cmask;	/* Constant mask for CRC */
	unsigned int	sen_crcec;	/* CRC Error counter */
	unsigned int	sen_alec;	/* alignment error counter */
	unsigned int	sen_disfc;	/* discard frame counter */
	unsigned short	sen_pads;	/* Tx short frame pad character */
	unsigned short	sen_retlim;	/* Retry limit threshold */
	unsigned short	sen_retcnt;	/* Retry limit counter */
	unsigned short	sen_maxflr;	/* maximum frame length register */
	unsigned short	sen_minflr;	/* minimum frame length register */
	unsigned short	sen_maxd1;	/* maximum DMA1 length */
	unsigned short	sen_maxd2;	/* maximum DMA2 length */
	unsigned short	sen_maxd;	/* Rx max DMA */
	unsigned short	sen_dmacnt;	/* Rx DMA counter */
	unsigned short	sen_maxb;	/* Max BD byte count */
	unsigned short	sen_gaddr1;	/* Group address filter */
	unsigned short	sen_gaddr2;
	unsigned short	sen_gaddr3;
	unsigned short	sen_gaddr4;
	unsigned int	sen_tbuf0data0;	/* Save area 0 - current frame */
	unsigned int	sen_tbuf0data1;	/* Save area 1 - current frame */
	unsigned int	sen_tbuf0rba;	/* Internal */
	unsigned int	sen_tbuf0crc;	/* Internal */
	unsigned short	sen_tbuf0bcnt;	/* Internal */
	unsigned short	sen_paddrh;	/* physical address (MSB) */
	unsigned short	sen_paddrm;
	unsigned short	sen_paddrl;	/* physical address (LSB) */
	unsigned short	sen_pper;	/* persistence */
	unsigned short	sen_rfbdptr;	/* Rx first BD pointer */
	unsigned short	sen_tfbdptr;	/* Tx first BD pointer */
	unsigned short	sen_tlbdptr;	/* Tx last BD pointer */
	unsigned int	sen_tbuf1data0;	/* Save area 0 - current frame */
	unsigned int	sen_tbuf1data1;	/* Save area 1 - current frame */
	unsigned int	sen_tbuf1rba;	/* Internal */
	unsigned int	sen_tbuf1crc;	/* Internal */
	unsigned short	sen_tbuf1bcnt;	/* Internal */
	unsigned short	sen_txlen;	/* Tx Frame length counter */
	unsigned short	sen_iaddr1;	/* Individual address filter */
	unsigned short	sen_iaddr2;
	unsigned short	sen_iaddr3;
	unsigned short	sen_iaddr4;
	unsigned short	sen_boffcnt;	/* Backoff counter */

	/* NOTE: Some versions of the manual have the following items
	 * incorrectly documented.  Below is the proper order.
	 */
	unsigned short	sen_taddrh;	/* temp address (MSB) */
	unsigned short	sen_taddrm;
	unsigned short	sen_taddrl;	/* temp address (LSB) */
} scc_enet_t;

/* Bits in parallel I/O port registers that have to be set/cleared
 * to configure the pins for SCC1 use.  The TCLK and RCLK seem unique
 * to the MBX860 board.  Any two of the four available clocks could be
 * used, and the MPC860 cookbook manual has an example using different
 * clock pins.
 */
#define PA_ENET_RXD	((unsigned short)0x0001)
#define PA_ENET_TXD	((unsigned short)0x0002)
#define PA_ENET_TCLK	((unsigned short)0x0200)
#define PA_ENET_RCLK	((unsigned short)0x0800)
#define PC_ENET_TENA	((unsigned short)0x0001)
#define PC_ENET_CLSN	((unsigned short)0x0010)
#define PC_ENET_RENA	((unsigned short)0x0020)

/* Control bits in the SICR to route TCLK (CLK2) and RCLK (CLK4) to
 * SCC1.  Also, make sure GR1 (bit 24) and SC1 (bit 25) are zero.
 */
#define SICR_ENET_MASK	((unsigned int)0x000000ff)
#define SICR_ENET_CLKRT	((unsigned int)0x0000003d)

/* SCC Event register as used by Ethernet.
*/
#define SCCE_ENET_GRA	((unsigned short)0x0080)	/* Graceful stop complete */
#define SCCE_ENET_TXE	((unsigned short)0x0010)	/* Transmit Error */
#define SCCE_ENET_RXF	((unsigned short)0x0008)	/* Full frame received */
#define SCCE_ENET_BSY	((unsigned short)0x0004)	/* All incoming buffers full */
#define SCCE_ENET_TXB	((unsigned short)0x0002)	/* A buffer was transmitted */
#define SCCE_ENET_RXB	((unsigned short)0x0001)	/* A buffer was received */

/* SCC Mode Register (PMSR) as used by Ethernet.
*/
#define SCC_PMSR_HBC	((unsigned short)0x8000)	/* Enable heartbeat */
#define SCC_PMSR_FC	((unsigned short)0x4000)	/* Force collision */
#define SCC_PMSR_RSH	((unsigned short)0x2000)	/* Receive short frames */
#define SCC_PMSR_IAM	((unsigned short)0x1000)	/* Check individual hash */
#define SCC_PMSR_ENCRC	((unsigned short)0x0800)	/* Ethernet CRC mode */
#define SCC_PMSR_PRO	((unsigned short)0x0200)	/* Promiscuous mode */
#define SCC_PMSR_BRO	((unsigned short)0x0100)	/* Catch broadcast pkts */
#define SCC_PMSR_SBT	((unsigned short)0x0080)	/* Special backoff timer */
#define SCC_PMSR_LPB	((unsigned short)0x0040)	/* Set Loopback mode */
#define SCC_PMSR_SIP	((unsigned short)0x0020)	/* Sample Input Pins */
#define SCC_PMSR_LCW	((unsigned short)0x0010)	/* Late collision window */
#define SCC_PMSR_NIB22	((unsigned short)0x000a)	/* Start frame search */
#define SCC_PMSR_FDE	((unsigned short)0x0001)	/* Full duplex enable */

/* Buffer descriptor control/status used by Ethernet receive.
*/
#define BD_ENET_RX_EMPTY	((unsigned short)0x8000)
#define BD_ENET_RX_WRAP		((unsigned short)0x2000)
#define BD_ENET_RX_INTR		((unsigned short)0x1000)
#define BD_ENET_RX_LAST		((unsigned short)0x0800)
#define BD_ENET_RX_FIRST	((unsigned short)0x0400)
#define BD_ENET_RX_MISS		((unsigned short)0x0100)
#define BD_ENET_RX_LG		((unsigned short)0x0020)
#define BD_ENET_RX_NO		((unsigned short)0x0010)
#define BD_ENET_RX_SH		((unsigned short)0x0008)
#define BD_ENET_RX_CR		((unsigned short)0x0004)
#define BD_ENET_RX_OV		((unsigned short)0x0002)
#define BD_ENET_RX_CL		((unsigned short)0x0001)
#define BD_ENET_RX_STATS	((unsigned short)0x013f)	/* All status bits */

/* Buffer descriptor control/status used by Ethernet transmit.
*/
#define BD_ENET_TX_READY	((unsigned short)0x8000)
#define BD_ENET_TX_PAD		((unsigned short)0x4000)
#define BD_ENET_TX_WRAP		((unsigned short)0x2000)
#define BD_ENET_TX_INTR		((unsigned short)0x1000)
#define BD_ENET_TX_LAST		((unsigned short)0x0800)
#define BD_ENET_TX_TC		((unsigned short)0x0400)
#define BD_ENET_TX_DEF		((unsigned short)0x0200)
#define BD_ENET_TX_HB		((unsigned short)0x0100)
#define BD_ENET_TX_LC		((unsigned short)0x0080)
#define BD_ENET_TX_RL		((unsigned short)0x0040)
#define BD_ENET_TX_RCMASK	((unsigned short)0x003c)
#define BD_ENET_TX_UN		((unsigned short)0x0002)
#define BD_ENET_TX_CSL		((unsigned short)0x0001)
#define BD_ENET_TX_STATS	((unsigned short)0x03ff)	/* All status bits */

/* SCC as UART
*/
typedef struct scc_uart {
	sccp_t	scc_genscc;
	unsigned int	scc_res1;	/* Reserved */
	unsigned int	scc_res2;	/* Reserved */
	unsigned short	scc_maxidl;	/* Maximum idle chars */
	unsigned short	scc_idlc;	/* temp idle counter */
	unsigned short	scc_brkcr;	/* Break count register */
	unsigned short	scc_parec;	/* receive parity error counter */
	unsigned short	scc_frmec;	/* receive framing error counter */
	unsigned short	scc_nosec;	/* receive noise counter */
	unsigned short	scc_brkec;	/* receive break condition counter */
	unsigned short	scc_brkln;	/* last received break length */
	unsigned short	scc_uaddr1;	/* UART address character 1 */
	unsigned short	scc_uaddr2;	/* UART address character 2 */
	unsigned short	scc_rtemp;	/* Temp storage */
	unsigned short	scc_toseq;	/* Transmit out of sequence char */
	unsigned short	scc_char1;	/* control character 1 */
	unsigned short	scc_char2;	/* control character 2 */
	unsigned short	scc_char3;	/* control character 3 */
	unsigned short	scc_char4;	/* control character 4 */
	unsigned short	scc_char5;	/* control character 5 */
	unsigned short	scc_char6;	/* control character 6 */
	unsigned short	scc_char7;	/* control character 7 */
	unsigned short	scc_char8;	/* control character 8 */
	unsigned short	scc_rccm;	/* receive control character mask */
	unsigned short	scc_rccr;	/* receive control character register */
	unsigned short	scc_rlbc;	/* receive last break character */
} scc_uart_t;

/* SCC Event and Mask registers when it is used as a UART.
*/
#define UART_SCCM_GLR		((unsigned short)0x1000)
#define UART_SCCM_GLT		((unsigned short)0x0800)
#define UART_SCCM_AB		((unsigned short)0x0200)
#define UART_SCCM_IDL		((unsigned short)0x0100)
#define UART_SCCM_GRA		((unsigned short)0x0080)
#define UART_SCCM_BRKE		((unsigned short)0x0040)
#define UART_SCCM_BRKS		((unsigned short)0x0020)
#define UART_SCCM_CCR		((unsigned short)0x0008)
#define UART_SCCM_BSY		((unsigned short)0x0004)
#define UART_SCCM_TX		((unsigned short)0x0002)
#define UART_SCCM_RX		((unsigned short)0x0001)

/* The SCC PMSR when used as a UART.
*/
#define SCU_PMSR_FLC		((unsigned short)0x8000)
#define SCU_PMSR_SL		((unsigned short)0x4000)
#define SCU_PMSR_CL		((unsigned short)0x3000)
#define SCU_PMSR_UM		((unsigned short)0x0c00)
#define SCU_PMSR_FRZ		((unsigned short)0x0200)
#define SCU_PMSR_RZS		((unsigned short)0x0100)
#define SCU_PMSR_SYN		((unsigned short)0x0080)
#define SCU_PMSR_DRT		((unsigned short)0x0040)
#define SCU_PMSR_PEN		((unsigned short)0x0010)
#define SCU_PMSR_RPM		((unsigned short)0x000c)
#define SCU_PMSR_REVP		((unsigned short)0x0008)
#define SCU_PMSR_TPM		((unsigned short)0x0003)
#define SCU_PMSR_TEVP		((unsigned short)0x0003)

/* CPM Transparent mode SCC.
 */
typedef struct scc_trans {
	sccp_t	st_genscc;
	unsigned int	st_cpres;	/* Preset CRC */
	unsigned int	st_cmask;	/* Constant mask for CRC */
} scc_trans_t;

/* CPM interrupts.  There are nearly 32 interrupts generated by CPM
 * channels or devices.  All of these are presented to the PPC core
 * as a single interrupt.  The CPM interrupt handler dispatches its
 * own handlers, in a similar fashion to the PPC core handler.  We
 * use the table as defined in the manuals (i.e. no special high
 * priority and SCC1 == SCCa, etc...).
 */
#define CPMVEC_NR		32
#define	CPMVEC_PIO_PC15		((unsigned short)0x1f)
#define	CPMVEC_SCC1		((unsigned short)0x1e)
#define	CPMVEC_SCC2		((unsigned short)0x1d)
#define	CPMVEC_SCC3		((unsigned short)0x1c)
#define	CPMVEC_SCC4		((unsigned short)0x1b)
#define	CPMVEC_PIO_PC14		((unsigned short)0x1a)
#define	CPMVEC_TIMER1		((unsigned short)0x19)
#define	CPMVEC_PIO_PC13		((unsigned short)0x18)
#define	CPMVEC_PIO_PC12		((unsigned short)0x17)
#define	CPMVEC_SDMA_CB_ERR	((unsigned short)0x16)
#define CPMVEC_IDMA1		((unsigned short)0x15)
#define CPMVEC_IDMA2		((unsigned short)0x14)
#define CPMVEC_TIMER2		((unsigned short)0x12)
#define CPMVEC_RISCTIMER	((unsigned short)0x11)
#define CPMVEC_I2C		((unsigned short)0x10)
#define	CPMVEC_PIO_PC11		((unsigned short)0x0f)
#define	CPMVEC_PIO_PC10		((unsigned short)0x0e)
#define CPMVEC_TIMER3		((unsigned short)0x0c)
#define	CPMVEC_PIO_PC9		((unsigned short)0x0b)
#define	CPMVEC_PIO_PC8		((unsigned short)0x0a)
#define	CPMVEC_PIO_PC7		((unsigned short)0x09)
#define CPMVEC_TIMER4		((unsigned short)0x07)
#define	CPMVEC_PIO_PC6		((unsigned short)0x06)
#define	CPMVEC_SPI		((unsigned short)0x05)
#define	CPMVEC_SMC1		((unsigned short)0x04)
#define	CPMVEC_SMC2		((unsigned short)0x03)
#define	CPMVEC_PIO_PC5		((unsigned short)0x02)
#define	CPMVEC_PIO_PC4		((unsigned short)0x01)
#define	CPMVEC_ERROR		((unsigned short)0x00)

extern void cpm_install_handler(int vec, void (*handler)(void *), void *dev_id);

/* CPM interrupt configuration vector.
*/
#define	CICR_SCD_SCC4		((unsigned int)0x00c00000)	/* SCC4 @ SCCd */
#define	CICR_SCC_SCC3		((unsigned int)0x00200000)	/* SCC3 @ SCCc */
#define	CICR_SCB_SCC2		((unsigned int)0x00040000)	/* SCC2 @ SCCb */
#define	CICR_SCA_SCC1		((unsigned int)0x00000000)	/* SCC1 @ SCCa */
#define CICR_IRL_MASK		((unsigned int)0x0000e000)	/* Core interrrupt */
#define CICR_HP_MASK		((unsigned int)0x00001f00)	/* Hi-pri int. */
#define CICR_IEN		((unsigned int)0x00000080)	/* Int. enable */
#define CICR_SPS		((unsigned int)0x00000001)	/* SCC Spread */
#endif /* __CPM_8XX__ */
