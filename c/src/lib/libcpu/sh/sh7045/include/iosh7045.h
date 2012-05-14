/*
 *  This include file contains information pertaining to the Hitachi SH
 *  processor.
 *
 *  NOTE: NOT ALL VALUES HAVE BEEN CHECKED !!
 *
 *  Authors: Ralf Corsepius (corsepiu@faw.uni-ulm.de) and
 *           Bernd Becker (becker@faw.uni-ulm.de)
 *
 *  Based on "iosh7030.h" distributed with Hitachi's EVB's tutorials, which
 *  contained no copyright notice.
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *  COPYRIGHT (c) 1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *	Modified to reflect on-chip registers for sh7045 processor, based on
 *	"Register.h" distributed with Hitachi's EVB7045F tutorials, and which
 *  contained no copyright notice:
 *	John M. Mills (jmills@tga.com)
 *	TGA Technologies, Inc.
 *  100 Pinnacle Way, Suite 140
 *	Norcross, GA 30071 U.S.A.
 *	August, 1999
 *
 *	This modified file may be copied and distributed in accordance
 *	the above-referenced license. It is provided for critique and
 *	developmental purposes without any warranty nor representation
 *	by the authors or by TGA Technologies.
 */

#ifndef __IOSH7045_H
#define __IOSH7045_H

/*
 * After each line is explained whether the access is char short or long.
 * The functions read/writeb, w, l, 8, 16, 32 can be found
 * in exec/score/cpu/sh/sh_io.h
 *
 * 8 bit  == char     ( readb, writeb, read8, write8)
 * 16 bit == short    ( readw, writew, read16, write16 )
 * 32 bit == long     ( readl, writel, read32, write32 )
 * JMM: Addresses noted "[char, ]short,word" are per Hitachi _SuperH_RISC_
 *      ENGINE_..Hardware_Manual; alignment access-restrictions may apply
 */

#define REG_BASE    0xFFFF8000

/* SCI0 Registers */
#define SCI_SMR0   (REG_BASE + 0x01a0) /*char: Serial mode     ch 0 */
#define SCI_BRR0   (REG_BASE + 0x01a1) /*char: Bit rate        ch 0 */
#define SCI_SCR0   (REG_BASE + 0x01a2) /*char: Serial control  ch 0 */
#define SCI_TDR0   (REG_BASE + 0x01a3) /*char: Transmit data   ch 0 */
#define SCI_SSR0   (REG_BASE + 0x01a4) /*char: Serial status   ch 0 */
#define SCI_RDR0   (REG_BASE + 0x01a5) /*char: Receive data    ch 0 */

#define SCI0_SMR   SCI_SMR0

/* SCI1 Registers */
#define SCI_SMR1   (REG_BASE + 0x01b0) /* char: Serial mode     ch 1 */
#define SCI_BRR1   (REG_BASE + 0x01b1) /* char: Bit rate        ch 1 */
#define SCI_SCR1   (REG_BASE + 0x01b2) /* char: Serial control  ch 1 */
#define SCI_TDR1   (REG_BASE + 0x01b3) /* char: Transmit data   ch 1 */
#define SCI_SSR1   (REG_BASE + 0x01b4) /* char: Serial status   ch 1 */
#define SCI_RDR1   (REG_BASE + 0x01b5) /* char: Receive data    ch 1 */

#define SCI1_SMR   SCI_SMR1

/* ADI */
/* High Speed A/D (Excluding A-Mask Part)*/
#define ADDRA	   (REG_BASE + 0x03F0) /* short */
#define ADDRB	   (REG_BASE + 0x03F2) /* short */
#define ADDRC	   (REG_BASE + 0x03F4) /* short */
#define ADDRD	   (REG_BASE + 0x03F6) /* short */
#define ADDRE	   (REG_BASE + 0x03F8) /* short */
#define ADDRF	   (REG_BASE + 0x03FA) /* short */
#define ADDRG	   (REG_BASE + 0x03FC) /* short */
#define ADDRH	   (REG_BASE + 0x03FE) /* short */
#define ADCSR	   (REG_BASE + 0x03E0) /* char  */
#define ADCR	   (REG_BASE + 0x03E1) /* char  */

/* Mid-Speed A/D (A-Mask part)*/
#define ADDRA0	   (REG_BASE + 0x0400) /* char, short */
#define ADDRA0H	   (REG_BASE + 0x0400) /* char, short */
#define ADDRA0L    (REG_BASE + 0x0401) /* char  */
#define ADDRB0	   (REG_BASE + 0x0402) /* char, short */
#define ADDRB0H	   (REG_BASE + 0x0402) /* char, short */
#define ADDRB0L	   (REG_BASE + 0x0403) /* char  */
#define ADDRC0	   (REG_BASE + 0x0404) /* char, short */
#define ADDRC0H	   (REG_BASE + 0x0404) /* char, short */
#define ADDRC0L	   (REG_BASE + 0x0405) /* char  */
#define ADDRD0	   (REG_BASE + 0x0406) /* char, short */
#define ADDRD0H	   (REG_BASE + 0x0406) /* char, short  */
#define ADDRD0L	   (REG_BASE + 0x0407) /* char  */
#define ADCSR0	   (REG_BASE + 0x0410) /* char  */
#define ADCR0	   (REG_BASE + 0x0412) /* char  */
#define ADDRA1	   (REG_BASE + 0x0408) /* char, short */
#define ADDRA1H	   (REG_BASE + 0x0408) /* char, short */
#define ADDRA1L	   (REG_BASE + 0x0409) /* char  */
#define ADDRB1	   (REG_BASE + 0x040A) /* char, short */
#define ADDRB1H	   (REG_BASE + 0x040A) /* char, short */
#define ADDRB1L	   (REG_BASE + 0x040B) /* char  */
#define ADDRC1	   (REG_BASE + 0x040C) /* char, short */
#define ADDRC1H	   (REG_BASE + 0x040C) /* char, short */
#define ADDRC1L	   (REG_BASE + 0x040D) /* char  */
#define ADDRD1	   (REG_BASE + 0x040E) /* char, short */
#define ADDRD1H	   (REG_BASE + 0x040E) /* char, short  */
#define ADDRD1L	   (REG_BASE + 0x040F) /* char  */
#define ADCSR1	   (REG_BASE + 0x0411) /* char  */
#define ADCR1	   (REG_BASE + 0x0413) /* char  */

/*MTU SHARED*/
#define MTU_TSTR   (REG_BASE + 0x0240) /* char, short, word  */
#define MTU_TSYR   (REG_BASE + 0x0241) /* char, short, word  */
#define MTU_ICSR   (REG_BASE + 0x03C0) /* input lev. CSR */
#define MTU_OCSR   (REG_BASE + 0x03C0) /* output lev. CSR */

/*MTU CHANNEL 0*/
#define MTU_TCR0   (REG_BASE + 0x0260) /* char, short, word  */
#define MTU_TMDR0  (REG_BASE + 0x0261) /* char, short, word  */
#define MTU_TIORH0 (REG_BASE + 0x0262) /* char, short, word  */
#define MTU_TIORL0 (REG_BASE + 0x0263) /* char, short, word  */
#define MTU_TIER0  (REG_BASE + 0x0264) /* char, short, word  */
#define MTU_TSR0   (REG_BASE + 0x0265) /* char, short, word  */
#define MTU_TCNT0  (REG_BASE + 0x0266) /* short, word */
#define MTU_GR0A   (REG_BASE + 0x0268) /* short, word */
#define MTU_GR0B   (REG_BASE + 0x026A) /* short, word */
#define MTU_GR0C   (REG_BASE + 0x026C) /* short, word */
#define MTU_GR0D   (REG_BASE + 0x026E) /* short, word */

/*MTU CHANNEL 1*/
#define MTU_TCR1   (REG_BASE + 0x0280) /* char, short, word  */
#define MTU_TMDR1  (REG_BASE + 0x0281) /* char, short, word  */
#define MTU_TIOR1  (REG_BASE + 0x0282) /* char, short, word  */
#define MTU_TIER1  (REG_BASE + 0x0284) /* char, short, word  */
#define MTU_TSR1   (REG_BASE + 0x0285) /* char, short, word  */
#define MTU_TCNT1  (REG_BASE + 0x0286) /* short, word */
#define MTU_GR1A   (REG_BASE + 0x0288) /* short, word */
#define MTU_GR1B   (REG_BASE + 0x028A) /* short, word */

/*MTU CHANNEL 2*/
#define MTU_TCR2   (REG_BASE + 0x02A0) /* char, short, word  */
#define MTU_TMDR2  (REG_BASE + 0x02A1) /* char, short, word  */
#define MTU_TIOR2  (REG_BASE + 0x02A2) /* char, short, word  */
#define MTU_TIER2  (REG_BASE + 0x02A4) /* char, short, word  */
#define MTU_TSR2   (REG_BASE + 0x02A5) /* char, short, word  */
#define MTU_TCNT2  (REG_BASE + 0x02A6) /* short, word */
#define MTU_GR2A   (REG_BASE + 0x02A8) /* short, word */
#define MTU_GR2B   (REG_BASE + 0x02AA) /* short, word */

/*MTU CHANNELS 3-4 SHARED*/
#define MTU_TOER   (REG_BASE + 0x020A) /* char, short, word  */
#define MTU_TOCR   (REG_BASE + 0x020B) /* char, short, word  */
#define MTU_TGCR   (REG_BASE + 0x020D) /* char, short, word  */
#define MTU_TCDR   (REG_BASE + 0x0214) /* short, word  */
#define MTU_TDDR   (REG_BASE + 0x0216) /* short, word  */
#define MTU_TCNTS  (REG_BASE + 0x0220) /* short, word  */
#define MTU_TCBR   (REG_BASE + 0x0222) /* short, word  */

/*MTU CHANNEL 3*/
#define MTU_TCR3   (REG_BASE + 0x0200) /* char, short, word  */
#define MTU_TMDR3  (REG_BASE + 0x0202) /* char, short, word  */
#define MTU_TIORH3 (REG_BASE + 0x0204) /* char, short, word  */
#define MTU_TIORL3 (REG_BASE + 0x0205) /* char, short, word  */
#define MTU_TIER3  (REG_BASE + 0x0208) /* char, short, word  */
#define MTU_TSR3   (REG_BASE + 0x022C) /* char, short, word  */
#define MTU_TCNT3  (REG_BASE + 0x0210) /* short, word */
#define MTU_GR3A   (REG_BASE + 0x0218) /* short, word */
#define MTU_GR3B   (REG_BASE + 0x021A) /* short, word */
#define MTU_GR3C   (REG_BASE + 0x0224) /* short, word */
#define MTU_GR3D   (REG_BASE + 0x0226) /* short, word */

/*MTU CHANNEL 4*/
#define MTU_TCR4   (REG_BASE + 0x0201) /* char, short, word  */
#define MTU_TMDR4  (REG_BASE + 0x0203) /* char, short, word  */
#define MTU_TIOR4  (REG_BASE + 0x0206) /* char, short, word  */
#define MTU_TIORH4 (REG_BASE + 0x0206) /* char, short, word  */
#define MTU_TIORL4 (REG_BASE + 0x0207) /* char, short, word  */
#define MTU_TIER4  (REG_BASE + 0x0209) /* char, short, word  */
#define MTU_TSR4   (REG_BASE + 0x022D) /* char, short, word  */
#define MTU_TCNT4  (REG_BASE + 0x0212) /* short, word */
#define MTU_GR4A   (REG_BASE + 0x021C) /* short, word */
#define MTU_GR4B   (REG_BASE + 0x021E) /* short, word */
#define MTU_GR4C   (REG_BASE + 0x0228) /* short, word */
#define MTU_GR4D   (REG_BASE + 0x022A) /* short, word */

/*DMAC CHANNELS 0-3 SHARED*/
#define DMAOR      (REG_BASE + 0x06B0) /* short */

/*DMAC CHANNEL 0*/
#define DMA_SAR0    (REG_BASE + 0x06C0) /* short, word */
#define DMA_DAR0    (REG_BASE + 0x06C4) /* short, word */
#define DMA_DMATCR0 (REG_BASE + 0x06C8) /* short, word */
#define DMA_CHCR0   (REG_BASE + 0x06CC) /* short, word */

/*DMAC CHANNEL 1*/
#define DMA_SAR1    (REG_BASE + 0x06D0) /* short, word */
#define DMA_DAR1    (REG_BASE + 0x06D4) /* short, word */
#define DMA_DMATCR1 (REG_BASE + 0x06D8) /* short, wordt */
#define DMA_CHCR1   (REG_BASE + 0x06DC) /* short, word */

/*DMAC CHANNEL 3*/
#define DMA_SAR3    (REG_BASE + 0x06E0) /* short, word */
#define DMA_DAR3    (REG_BASE + 0x06E4) /* short, word */
#define DMA_DMATCR3 (REG_BASE + 0x06E8) /* short, word */
#define DMA_CHCR3   (REG_BASE + 0x06EC) /* short, word */

/*DMAC CHANNEL 4*/
#define DMA_SAR4    (REG_BASE + 0x06F0) /* short, word */
#define DMA_DAR4    (REG_BASE + 0x06F4) /* short, word */
#define DMA_DMATCR4 (REG_BASE + 0x06F8) /* short, word */
#define DMA_CHCR4   (REG_BASE + 0x06FC) /* short, word */

/*Data Transfer Controller*/
#define DTC_DTEA   (REG_BASE + 0x0700) /* char, short, word */
#define DTC_DTEB   (REG_BASE + 0x0701) /* char, short(?), word(?) */
#define DTC_DTEC   (REG_BASE + 0x0702) /* char, short(?), word(?) */
#define DTC_DTED   (REG_BASE + 0x0703) /* char, short(?), word(?) */
#define DTC_DTEE   (REG_BASE + 0x0704) /* char, short(?), word(?) */
#define DTC_DTCSR  (REG_BASE + 0x0706) /* char, short, word */
#define DTC_DTBR   (REG_BASE + 0x0708) /* short, word */

/*Cache Memory*/
#define CAC_CCR	   (REG_BASE + 0x0740) /* char, short, word */

/*INTC*/
#define INTC_IPRA  (REG_BASE + 0x0348) /* char, short, word */
#define INTC_IPRB  (REG_BASE + 0x034A) /* char, short, word */
#define INTC_IPRC  (REG_BASE + 0x034C) /* char, short, word */
#define INTC_IPRD  (REG_BASE + 0x034E) /* char, short, word */
#define INTC_IPRE  (REG_BASE + 0x0350) /* char, short, word */
#define INTC_IPRF  (REG_BASE + 0x0352) /* char, short, word */
#define INTC_IPRG  (REG_BASE + 0x0354) /* char, short, word */
#define INTC_IPRH  (REG_BASE + 0x0356) /* char, short, word */
#define INTC_ICR   (REG_BASE + 0x0358) /* char, short, word */
#define INTC_ISR   (REG_BASE + 0x035A) /* char, short, word */

/*Flash (F-ZTAT)*/
#define FL_FLMCR1  (REG_BASE + 0x0580) /* Fl.Mem.Contr.Reg 1: char */
#define FL_FLMCR2  (REG_BASE + 0x0581) /* Fl.Mem.Contr.Reg 2: char */
#define FL_EBR1	   (REG_BASE + 0x0582) /* Fl.Mem.Erase Blk.1: char */
#define FL_EBR2	   (REG_BASE + 0x0584) /* Fl.Mem.Erase Blk.2: char */
#define FL_RAMER   (REG_BASE + 0x0628) /* Ram Emul.Reg.- char,short,word */

/*UBC*/
#define UBC_BARH   (REG_BASE + 0x0600) /* char, short, word  */
#define UBC_BARL   (REG_BASE + 0x0602) /* char, short, word  */
#define UBC_BAMRH  (REG_BASE + 0x0604) /* char, short, word  */
#define UBC_BAMRL  (REG_BASE + 0x0606) /* char, short, word  */
#define UBC_BBR    (REG_BASE + 0x0608) /* char, short, word  */
/*BSC*/
#define BSC_BCR1   (REG_BASE + 0x0620) /* short */
#define BSC_BCR2   (REG_BASE + 0x0622) /* short */
#define BSC_WCR1   (REG_BASE + 0x0624) /* short */
#define BSC_WCR2   (REG_BASE + 0x0626) /* short */
#define BSC_DCR    (REG_BASE + 0x062A) /* short */
#define BSC_RTCSR  (REG_BASE + 0x062C) /* short */
#define BSC_RTCNT  (REG_BASE + 0x062E) /* short */
#define BSC_RTCOR  (REG_BASE + 0x0630) /* short */

/*WDT*/
#define WDT_R_TCSR   (REG_BASE + 0x0610) /* rd: char */
#define WDT_R_TCNT   (REG_BASE + 0x0611) /* rd: char */
#define WDT_R_RSTCSR (REG_BASE + 0x0613) /* rd: char */
#define WDT_W_TCSR   (REG_BASE + 0x0610) /* wrt: short */
#define WDT_W_TCNT   (REG_BASE + 0x0610) /* wrt: short */
#define WDT_W_RSTCSR (REG_BASE + 0x0612) /* wrt: short */

/*POWER DOWN STATE*/
#define PDT_SBYCR  (REG_BASE + 0x0614) /* char  */

/* Port I/O  Control Registers */
#define IO_PADRH   (REG_BASE + 0x0380) /* Port A Data Register */
#define IO_PADRL   (REG_BASE + 0x0382) /* Port A Data Register */
#define IO_PBDR    (REG_BASE + 0x0390) /* Port B Data Register */
#define IO_PCDR    (REG_BASE + 0x0392) /* Port C Data Register */
#define IO_PDDRH   (REG_BASE + 0x03A0) /* Port D Data Register */
#define IO_PDDRL   (REG_BASE + 0x03A2) /* Port D Data Register */
#define IO_PEDR    (REG_BASE + 0x03B0) /* Port E Data Register */
#define IO_PFDR    (REG_BASE + 0x03B2) /* Port F Data Register */

/*Pin Function Control Register*/
#define PFC_PAIORH (REG_BASE + 0x0384) /* Port A I/O Reg. H */
#define PFC_PAIORL (REG_BASE + 0x0386) /* Port A I/O Reg. L */
#define PFC_PACRH  (REG_BASE + 0x0388) /* Port A Ctr. Reg. H  */
#define PFC_PACRL1 (REG_BASE + 0x038C) /* Port A Ctr. Reg. L1 */
#define PFC_PACRL2 (REG_BASE + 0x038E) /* Port A Ctr. Reg. L2 */
#define PFC_PBIOR  (REG_BASE + 0x0394) /* Port B I/O Register */
#define PFC_PBCR1  (REG_BASE + 0x0398) /* Port B Ctr. Reg. R1 */
#define PFC_PBCR2  (REG_BASE + 0x039A) /* Port B Ctr. Reg. R2 */
#define PFC_PCIOR  (REG_BASE + 0x0396) /* Port C I/O Register */
#define PFC_PCCR   (REG_BASE + 0x039C) /* Port C Ctr. Reg. */
#define PFC_PDIORH (REG_BASE + 0x03A4) /* Port D I/O Reg. H */
#define PFC_PDIORL (REG_BASE + 0x03A6) /* Port D I/O Reg. L */
#define PFC_PDCRH1 (REG_BASE + 0x03A8) /* Port D Ctr. Reg. H1 */
#define PFC_PDCRH2 (REG_BASE + 0x03AA) /* Port D Ctr. Reg. H2 */
#define PFC_PDCRL  (REG_BASE + 0x03AC) /* Port D Ctr. Reg. L  */
#define PFC_PEIOR  (REG_BASE + 0x03B4) /* Port E I/O Register */
#define PFC_PECR1  (REG_BASE + 0x03B8) /* Port E Ctr. Reg. 1 */
#define PFC_PECR2  (REG_BASE + 0x03BA) /* Port E Ctr. Reg. 2 */
#define PFC_IFCR   (REG_BASE + 0x03C8) /* short */

/*Compare/Match Timer*/
#define CMT_CMSTR  (REG_BASE + 0x3D0) /* Start Reg. char, short, word */
#define CMT_CMCSR0 (REG_BASE + 0x3D2) /* C0 SCR short, word */
#define CMT_CMCNT0 (REG_BASE + 0x3D4) /* C0 Counter char, short, word */
#define CMT_CMCOR0 (REG_BASE + 0x3D6) /* C0 Const.Reg. char, short, word */
#define CMT_CMCSR1 (REG_BASE + 0x3D8) /* C1 SCR short, word */
#define CMT_CMCNT1 (REG_BASE + 0x3DA) /* C1 Counter char, short, word */
#define CMT_CMCOR1 (REG_BASE + 0x3DC) /* C1 Const.Reg. char, short, word */

#endif
