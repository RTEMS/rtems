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
 */

#ifndef __IOSH7030_H
#define __IOSH7030_H

/*
 * After each line is explained whether the access is char short or long.
 * The functions read/writeb, w, l, 8, 16, 32 can be found
 * in exec/score/cpu/sh/sh_io.h
 *
 * 8 bit  == char     ( readb, writeb, read8, write8)
 * 16 bit == short    ( readw, writew, read16, write16 )
 * 32 bit == long     ( readl, writel, read32, write32 )
 */

#define SCI0_SMR 	0x05fffec0 /* char  */
#define SCI0_BRR 	0x05fffec1 /* char  */
#define SCI0_SCR 	0x05fffec2 /* char  */
#define SCI0_TDR 	0x05fffec3 /* char  */
#define SCI0_SSR 	0x05fffec4 /* char  */
#define SCI0_RDR 	0x05fffec5 /* char  */

#define SCI1_SMR 	0x05fffec8 /* char  */
#define SCI1_BRR 	0x05fffec9 /* char  */
#define SCI1_SCR 	0x05fffeca /* char  */
#define SCI1_TDR 	0x05fffecb /* char  */
#define SCI1_SSR 	0x05fffecc /* char  */
#define SCI1_RDR 	0x05fffecd /* char  */


#define ADDRAH 		0x05fffee0 /* char  */
#define ADDRAL 		0x05fffee1 /* char  */
#define ADDRBH 		0x05fffee2 /* char  */
#define ADDRBL 		0x05fffee3 /* char  */
#define ADDRCH 		0x05fffee4 /* char  */
#define ADDRCL 		0x05fffee5 /* char  */
#define ADDRDH 		0x05fffee6 /* char  */
#define ADDRDL 		0x05fffee7 /* char  */
#define AD_DRA 		0x05fffee0 /* short */
#define AD_DRB 		0x05fffee2 /* short */
#define AD_DRC 		0x05fffee4 /* short */
#define AD_DRD 		0x05fffee6 /* short */
#define ADCSR  		0x05fffee8 /* char  */
#define ADCR   		0x05fffee9 /* char  */

/*ITU SHARED*/
#define ITU_TSTR  	0x05ffff00 /* char  */
#define ITU_TSNC  	0x05ffff01 /* char  */
#define ITU_TMDR  	0x05ffff02 /* char  */
#define ITU_TFCR  	0x05ffff03 /* char  */

/*ITU CHANNEL 0*/
#define ITU_TCR0  	0x05ffff04 /* char  */
#define ITU_TIOR0 	0x05ffff05 /* char  */
#define ITU_TIER0 	0x05ffff06 /* char  */
#define ITU_TSR0  	0x05ffff07 /* char  */
#define ITU_TCNT0 	0x05ffff08 /* short */
#define ITU_GRA0  	0x05ffff0a /* short */
#define ITU_GRB0  	0x05ffff0c /* short */

 /*ITU CHANNEL 1*/
#define ITU_TCR1  	0x05ffff0E /* char  */
#define ITU_TIOR1 	0x05ffff0F /* char  */
#define ITU_TIER1 	0x05ffff10 /* char  */
#define ITU_TSR1  	0x05ffff11 /* char  */
#define ITU_TCNT1 	0x05ffff12 /* short */
#define ITU_GRA1  	0x05ffff14 /* short */
#define ITU_GRB1  	0x05ffff16 /* short */


 /*ITU CHANNEL 2*/
#define ITU_TCR2  	0x05ffff18 /* char  */
#define ITU_TIOR2 	0x05ffff19 /* char  */
#define ITU_TIER2 	0x05ffff1A /* char  */
#define ITU_TSR2  	0x05ffff1B /* char  */
#define ITU_TCNT2 	0x05ffff1C /* short */
#define ITU_GRA2  	0x05ffff1E /* short */
#define ITU_GRB2  	0x05ffff20 /* short */

 /*ITU CHANNEL 3*/
#define ITU_TCR3  	0x05ffff22 /* char  */
#define ITU_TIOR3 	0x05ffff23 /* char  */
#define ITU_TIER3 	0x05ffff24 /* char  */
#define ITU_TSR3  	0x05ffff25 /* char  */
#define ITU_TCNT3 	0x05ffff26 /* short */
#define ITU_GRA3  	0x05ffff28 /* short */
#define ITU_GRB3  	0x05ffff2A /* short */
#define ITU_BRA3  	0x05ffff2C /* short */
#define ITU_BRB3  	0x05ffff2E /* short */

 /*ITU CHANNELS 0-4 SHARED*/
#define ITU_TOCR  	0x05ffff31 /* char  */

 /*ITU CHANNEL 4*/
#define ITU_TCR4  	0x05ffff32 /* char  */
#define ITU_TIOR4 	0x05ffff33 /* char  */
#define ITU_TIER4 	0x05ffff34 /* char  */
#define ITU_TSR4  	0x05ffff35 /* char  */
#define ITU_TCNT4 	0x05ffff36 /* short */
#define ITU_GRA4  	0x05ffff38 /* short */
#define ITU_GRB4  	0x05ffff3A /* short */
#define ITU_BRA4  	0x05ffff3C /* short */
#define ITU_BRB4  	0x05ffff3E /* short */

 /*DMAC CHANNELS 0-3 SHARED*/
#define DMAOR           0x05ffff48 /* short */

 /*DMAC CHANNEL 0*/
#define DMA_SAR0        0x05ffff40 /* long  */
#define DMA_DAR0        0x05ffff44 /* long  */
#define DMA_TCR0        0x05ffff4a /* short */
#define DMA_CHCR0       0x05ffff4e /* short */

 /*DMAC CHANNEL 1*/
#define DMA_SAR1        0x05ffff50 /* long  */
#define DMA_DAR1        0x05ffff54 /* long  */
#define DMA_TCR1        0x05fffF5a /* short */
#define DMA_CHCR1       0x05ffff5e /* short */

 /*DMAC CHANNEL 3*/
#define DMA_SAR3        0x05ffff60 /* long  */
#define DMA_DAR3        0x05ffff64 /* long  */
#define DMA_TCR3        0x05fffF6a /* short */
#define DMA_CHCR3       0x05ffff6e /* short */

/*DMAC CHANNEL 4*/
#define DMA_SAR4        0x05ffff70 /* long  */
#define DMA_DAR4        0x05ffff74 /* long  */
#define DMA_TCR4        0x05fffF7a /* short */
#define DMA_CHCR4       0x05ffff7e /* short */

/*INTC*/
#define INTC_IPRA 	0x05ffff84 /* short */
#define INTC_IPRB 	0x05ffff86 /* short */
#define INTC_IPRC 	0x05ffff88 /* short */
#define INTC_IPRD 	0x05ffff8A /* short */
#define INTC_IPRE 	0x05ffff8C /* short */
#define INTC_ICR  	0x05ffff8E /* short */

/*UBC*/
#define UBC_BARH  	0x05ffff90 /* short */
#define UBC_BARL  	0x05ffff92 /* short */
#define UBC_BAMRH 	0x05ffff94 /* short */
#define UBC_BAMRL 	0x05ffff96 /* short */
#define UBC_BBR   	0x05ffff98 /* short */

/*BSC*/
#define BSC_BCR   	0x05ffffA0 /* short */
#define BSC_WCR1  	0x05ffffA2 /* short */
#define BSC_WCR2  	0x05ffffA4 /* short */
#define BSC_WCR3  	0x05ffffA6 /* short */
#define BSC_DCR   	0x05ffffA8 /* short */
#define BSC_PCR   	0x05ffffAA /* short */
#define BSC_RCR   	0x05ffffAC /* short */
#define BSC_RTCSR 	0x05ffffAE /* short */
#define BSC_RTCNT 	0x05ffffB0 /* short */
#define BSC_RTCOR 	0x05ffffB2 /* short */

/*WDT*/
#define WDT_TCSR  	0x05ffffB8 /* char  */
#define WDT_TCNT  	0x05ffffB9 /* char  */
#define WDT_RSTCSR 	0x05ffffBB /* char  */

/*POWER DOWN STATE*/
#define PDT_SBYCR 	0x05ffffBC /* char  */

/*PORT A*/
#define PADR 		0x05ffffC0 /* short */

/*PORT B*/
#define PBDR 		0x05ffffC2 /* short */

 /*PORT C*/
#define PCDR 		0x05ffffD0 /* short */

/*PFC*/
#define PFC_PAIOR 	0x05ffffC4 /* short */
#define PFC_PBIOR 	0x05ffffC6 /* short */
#define PFC_PACR1 	0x05ffffC8 /* short */
#define PFC_PACR2 	0x05ffffCA /* short */
#define PFC_PBCR1 	0x05ffffCC /* short */
#define PFC_PBCR2 	0x05ffffCE /* short */
#define PFC_CASCR 	0x05ffffEE /* short */

/*TPC*/
#define TPC_TPMR 	0x05ffffF0 /* short */
#define TPC_TPCR 	0x05ffffF1 /* short */
#define TPC_NDERH 	0x05ffffF2 /* short */
#define TPC_NDERL 	0x05ffffF3 /* short */
#define TPC_NDRB 	0x05ffffF4 /* char  */
#define TPC_NDRA 	0x05ffffF5 /* char  */
#define TPC_NDRB1 	0x05ffffF6 /* char  */
#define TPC_NDRA1 	0x05ffffF7 /* char  */

#endif
