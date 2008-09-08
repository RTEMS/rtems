/*===============================================================*\
| Project: RTEMS generic mcf548x BSP                              |
+-----------------------------------------------------------------+
| File: mcf548x.h                                                 |
+-----------------------------------------------------------------+
| The file contains all register an bit definitions of the        |
| generic MCF548x BSP.                                            |
+-----------------------------------------------------------------+
|                    Copyright (c) 2007                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
|                                                                 |
| Parts of the code has been derived from the "dBUG source code"  |
| package Freescale is providing for M548X EVBs. The usage of     |
| the modified or unmodified code and it's integration into the   |
| generic mcf548x BSP has been done according to the Freescale    |
| license terms.                                                  |
|                                                                 |
| The Freescale license terms can be reviewed in the file         |
|                                                                 |
|    Freescale_license.txt                                        |
|                                                                 |
+-----------------------------------------------------------------+
|                                                                 |
| The generic mcf548x BSP has been developed on the basic         |
| structures and modules of the av5282 BSP.                       |
|                                                                 |
+-----------------------------------------------------------------+
|                                                                 |
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
|                                                                 |
|   date                      history                        ID   |
| ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |
| 12.11.07                    1.0                            ras  |
|                                                                 |
\*===============================================================*/

#ifndef __MCF548X_H__
#define __MCF548X_H__

/*********************************************************************/
extern unsigned char __MBAR[];

/*********************************************************************
*
* Cache Control Register (CACR)
*
*********************************************************************/

/* Bit definitions and macros for MCF548X_CACR */
#define MCF548X_CACR_DEC		(0x80000000)
#define MCF548X_CACR_DW		    (0x40000000)
#define MCF548X_CACR_DESB		(0x20000000)
#define MCF548X_CACR_DDPI		(0x10000000)
#define MCF548X_CACR_DHLCK		(0x08000000)
#define MCF548X_CACR_DDCM(x)    (((x)<<25)&0x06000000)
#define MCF548X_CACR_DCINVA		(0x01000000)
#define MCF548X_CACR_DDSP		(0x00800000)
#define MCF548X_CACR_BEC		(0x00080000)
#define MCF548X_CACR_BCINVA		(0x00040000)
#define MCF548X_CACR_IEC		(0x00008000)
#define MCF548X_CACR_DNFB		(0x00002000)
#define MCF548X_CACR_ÍDPI		(0x00001000)
#define MCF548X_CACR_IEC		(0x00008000)
#define MCF548X_CACR_DNFB		(0x00002000)
#define MCF548X_CACR_ÍDPI		(0x00001000)
#define MCF548X_CACR_IHLCK		(0x00000800)
#define MCF548X_CACR_IDCM		(0x00000400)
#define MCF548X_CACR_ICINVA		(0x00000100)
#define MCF548X_CACR_IDSP		(0x00000080)
#define MCF548X_CACR_EUSP		(0x00000020)
#define MCF548X_CACR_DF		    (0x00000010)

/* Bit definitions and macros for MCF548X_CACR_DDCM (data cache mode) */
#define DCACHE_ON_WRIGHTTHROUGH 0
#define DCACHE_ON_COPYBACK      1
#define DCACHE_OFF_PRECISE      2
#define DCACHE_OFF_IMPRECISE    3

/*********************************************************************
*
* Access Control Registers (ACR0-3)
*
*********************************************************************/

/* Bit definitions and macros for MCF548X_ACRn */
#define MCF548X_ACR_BA(x)		 ((x)&0xFF000000)
#define MCF548X_ACR_ADMSK_AMM(x) (((x)>=0x1000000) ? (((x)&0xFF000000)>>8) : (((x)&0x00FF0000)|0x00000400))
#define MCF548X_ACR_E			 (0x00008000)
#define MCF548X_ACR_S(x)		 (((x)<<13)&0x00006000)
#define MCF548X_ACR_CM(x)        (((x)<<5)&0x00000060)
#define MCF548X_ACR_SP			 (0x00000008)
#define MCF548X_ACR_W			 (0x00000004)

/* Bit definitions and macros for MCF548X_ACR_S (supervisor/user access) */
#define S_ACCESS_USER           0
#define S_ACCESS_SUPV           1
#define S_ACCESS_BOTH           2

/* Bit definitions and macros for MCF548X_ACR_CM (cache mode) */
#define CM_ON_WRIGHTTHROUGH     0
#define CM_ON_COPYBACK          1
#define CM_OFF_PRECISE          2
#define CM_OFF_IMPRECISE        3

/*********************************************************************
*
* System PLL Control Register (SPCR)
*
*********************************************************************/

/* Register read/write macro */
#define MCF548X_PLL_SPCR                      (*(volatile uint32_t*)(void*)(&__MBAR[0x000300]))

/* Bit definitions and macros for MCF548X_PLL_SPCR (supervisor/user access) */
#define MCF548X_PLL_SPCR_PLLK   0x80000000
#define MCF548X_PLL_SPCR_COREN  0x00004000
#define MCF548X_PLL_SPCR_CRYENB 0x00002000
#define MCF548X_PLL_SPCR_CRYENA 0x00001000
#define MCF548X_PLL_SPCR_CAN1EN 0x00000800
#define MCF548X_PLL_SPCR_PSCEN  0x00000200
#define MCF548X_PLL_SPCR_USBEN  0x00000080
#define MCF548X_PLL_SPCR_FEC1EN 0x00000040
#define MCF548X_PLL_SPCR_FEC0EN 0x00000020
#define MCF548X_PLL_SPCR_DMAEN  0x00000010
#define MCF548X_PLL_SPCR_CAN0EN 0x00000008
#define MCF548X_PLL_SPCR_FBEN   0x00000004
#define MCF548X_PLL_SPCR_PCIEN  0x00000002
#define MCF548X_PLL_SPCR_MEMEN  0x00000001

/*********************************************************************
*
* XLB Arbiter Control (XLB)
*
*********************************************************************/

/* Register read/write macros */
#define MCF548X_XLB_CFG                      (*(volatile uint32_t*)(void*)(&__MBAR[0x000240]))
#define MCF548X_XLB_ADRTO                    (*(volatile uint32_t*)(void*)(&__MBAR[0x000258]))
#define MCF548X_XLB_DATTO                    (*(volatile uint32_t*)(void*)(&__MBAR[0x00025C]))
#define MCF548X_XLB_BUSTO                    (*(volatile uint32_t*)(void*)(&__MBAR[0x000260]))

/*********************************************************************
*
* Fast Ethernet Controller (FEC)
*
*********************************************************************/

/* Register read/write macros */
#define MCF548X_FEC_EIR0                     (*(volatile uint32_t*)(void*)(&__MBAR[0x009004]))
#define MCF548X_FEC_EIMR0                    (*(volatile uint32_t*)(void*)(&__MBAR[0x009008]))
#define MCF548X_FEC_ECR0                     (*(volatile uint32_t*)(void*)(&__MBAR[0x009024]))
#define MCF548X_FEC_MMFR0                    (*(volatile uint32_t*)(void*)(&__MBAR[0x009040]))
#define MCF548X_FEC_MSCR0                    (*(volatile uint32_t*)(void*)(&__MBAR[0x009044]))
#define MCF548X_FEC_MIBC0                    (*(volatile uint32_t*)(void*)(&__MBAR[0x009064]))
#define MCF548X_FEC_RCR0                     (*(volatile uint32_t*)(void*)(&__MBAR[0x009084]))
#define MCF548X_FEC_R_HASH0                  (*(volatile uint32_t*)(void*)(&__MBAR[0x009088]))
#define MCF548X_FEC_TCR0                     (*(volatile uint32_t*)(void*)(&__MBAR[0x0090C4]))
#define MCF548X_FEC_PALR0                    (*(volatile uint32_t*)(void*)(&__MBAR[0x0090E4]))
#define MCF548X_FEC_PAUR0                    (*(volatile uint32_t*)(void*)(&__MBAR[0x0090E8]))
#define MCF548X_FEC_OPD0                     (*(volatile uint32_t*)(void*)(&__MBAR[0x0090EC]))
#define MCF548X_FEC_IAUR0                    (*(volatile uint32_t*)(void*)(&__MBAR[0x009118]))
#define MCF548X_FEC_IALR0                    (*(volatile uint32_t*)(void*)(&__MBAR[0x00911C]))
#define MCF548X_FEC_GAUR0                    (*(volatile uint32_t*)(void*)(&__MBAR[0x009120]))
#define MCF548X_FEC_GALR0                    (*(volatile uint32_t*)(void*)(&__MBAR[0x009124]))
#define MCF548X_FEC_FECTFWR0                 (*(volatile uint32_t*)(void*)(&__MBAR[0x009144]))
#define MCF548X_FEC_FECRFDR0                 (*(volatile uint32_t*)(void*)(&__MBAR[0x009184]))
#define MCF548X_FEC_FECRFSR0                 (*(volatile uint32_t*)(void*)(&__MBAR[0x009188]))
#define MCF548X_FEC_FECRFCR0                 (*(volatile uint32_t*)(void*)(&__MBAR[0x00918C]))
#define MCF548X_FEC_FECRLRFP0                (*(volatile uint32_t*)(void*)(&__MBAR[0x009190]))
#define MCF548X_FEC_FECRLWFP0                (*(volatile uint32_t*)(void*)(&__MBAR[0x009194]))
#define MCF548X_FEC_FECRFAR0                 (*(volatile uint32_t*)(void*)(&__MBAR[0x009198]))
#define MCF548X_FEC_FECRFRP0                 (*(volatile uint32_t*)(void*)(&__MBAR[0x00919C]))
#define MCF548X_FEC_FECRFWP0                 (*(volatile uint32_t*)(void*)(&__MBAR[0x0091A0]))
#define MCF548X_FEC_FECTFDR0                 (*(volatile uint32_t*)(void*)(&__MBAR[0x0091A4]))
#define MCF548X_FEC_FECTFSR0                 (*(volatile uint32_t*)(void*)(&__MBAR[0x0091A8]))
#define MCF548X_FEC_FECTFCR0                 (*(volatile uint32_t*)(void*)(&__MBAR[0x0091AC]))
#define MCF548X_FEC_FECTLRFP0                (*(volatile uint32_t*)(void*)(&__MBAR[0x0091B0]))
#define MCF548X_FEC_FECTLWFP0                (*(volatile uint32_t*)(void*)(&__MBAR[0x0091B4]))
#define MCF548X_FEC_FECTFAR0                 (*(volatile uint32_t*)(void*)(&__MBAR[0x0091B8]))
#define MCF548X_FEC_FECTFRP0                 (*(volatile uint32_t*)(void*)(&__MBAR[0x0091BC]))
#define MCF548X_FEC_FECTFWP0                 (*(volatile uint32_t*)(void*)(&__MBAR[0x0091C0]))
#define MCF548X_FEC_FRST0                    (*(volatile uint32_t*)(void*)(&__MBAR[0x0091C4]))
#define MCF548X_FEC_CTCWR0                   (*(volatile uint32_t*)(void*)(&__MBAR[0x0091C8]))
#define MCF548X_FEC_RMON_T_DROP0             (*(volatile uint32_t*)(void*)(&__MBAR[0x009200]))
#define MCF548X_FEC_RMON_T_PACKETS0          (*(volatile uint32_t*)(void*)(&__MBAR[0x009204]))
#define MCF548X_FEC_RMON_T_BC_PKT0           (*(volatile uint32_t*)(void*)(&__MBAR[0x009208]))
#define MCF548X_FEC_RMON_T_MC_PKT0           (*(volatile uint32_t*)(void*)(&__MBAR[0x00920C]))
#define MCF548X_FEC_RMON_T_CRC_ALIGN0        (*(volatile uint32_t*)(void*)(&__MBAR[0x009210]))
#define MCF548X_FEC_RMON_T_UNDERSIZE0        (*(volatile uint32_t*)(void*)(&__MBAR[0x009214]))
#define MCF548X_FEC_RMON_T_OVERSIZE0         (*(volatile uint32_t*)(void*)(&__MBAR[0x009218]))
#define MCF548X_FEC_RMON_T_FRAG0             (*(volatile uint32_t*)(void*)(&__MBAR[0x00921C]))
#define MCF548X_FEC_RMON_T_JAB0              (*(volatile uint32_t*)(void*)(&__MBAR[0x009220]))
#define MCF548X_FEC_RMON_T_COL0              (*(volatile uint32_t*)(void*)(&__MBAR[0x009224]))
#define MCF548X_FEC_RMON_T_P640              (*(volatile uint32_t*)(void*)(&__MBAR[0x009228]))
#define MCF548X_FEC_RMON_T_P65TO1270         (*(volatile uint32_t*)(void*)(&__MBAR[0x00922C]))
#define MCF548X_FEC_RMON_T_P128TO2550        (*(volatile uint32_t*)(void*)(&__MBAR[0x009230]))
#define MCF548X_FEC_RMON_T_P256TO5110        (*(volatile uint32_t*)(void*)(&__MBAR[0x009234]))
#define MCF548X_FEC_RMON_T_P512TO10230       (*(volatile uint32_t*)(void*)(&__MBAR[0x009238]))
#define MCF548X_FEC_RMON_T_P1024TO20470      (*(volatile uint32_t*)(void*)(&__MBAR[0x00923C]))
#define MCF548X_FEC_RMON_T_P_GTE20480        (*(volatile uint32_t*)(void*)(&__MBAR[0x009240]))
#define MCF548X_FEC_RMON_T_OCTETS0           (*(volatile uint32_t*)(void*)(&__MBAR[0x009244]))
#define MCF548X_FEC_IEEE_T_DROP0             (*(volatile uint32_t*)(void*)(&__MBAR[0x009248]))
#define MCF548X_FEC_IEEE_T_FRAME_OK0         (*(volatile uint32_t*)(void*)(&__MBAR[0x00924C]))
#define MCF548X_FEC_IEEE_T_1COL0             (*(volatile uint32_t*)(void*)(&__MBAR[0x009250]))
#define MCF548X_FEC_IEEE_T_MCOL0             (*(volatile uint32_t*)(void*)(&__MBAR[0x009254]))
#define MCF548X_FEC_IEEE_T_DEF0              (*(volatile uint32_t*)(void*)(&__MBAR[0x009258]))
#define MCF548X_FEC_IEEE_T_LCOL0             (*(volatile uint32_t*)(void*)(&__MBAR[0x00925C]))
#define MCF548X_FEC_IEEE_T_EXCOL0            (*(volatile uint32_t*)(void*)(&__MBAR[0x009260]))
#define MCF548X_FEC_IEEE_T_MACERR0           (*(volatile uint32_t*)(void*)(&__MBAR[0x009264]))
#define MCF548X_FEC_IEEE_T_CSERR0            (*(volatile uint32_t*)(void*)(&__MBAR[0x009268]))
#define MCF548X_FEC_IEEE_T_SQE0              (*(volatile uint32_t*)(void*)(&__MBAR[0x00926C]))
#define MCF548X_FEC_IEEE_T_FDXFC0            (*(volatile uint32_t*)(void*)(&__MBAR[0x009270]))
#define MCF548X_FEC_IEEE_T_OCTETS_OK0        (*(volatile uint32_t*)(void*)(&__MBAR[0x009274]))
#define MCF548X_FEC_RMON_R_PACKETS0          (*(volatile uint32_t*)(void*)(&__MBAR[0x009284]))
#define MCF548X_FEC_RMON_R_BC_PKT0           (*(volatile uint32_t*)(void*)(&__MBAR[0x009288]))
#define MCF548X_FEC_RMON_R_MC_PKT0           (*(volatile uint32_t*)(void*)(&__MBAR[0x00928C]))
#define MCF548X_FEC_RMON_R_CRC_ALIGN0        (*(volatile uint32_t*)(void*)(&__MBAR[0x009290]))
#define MCF548X_FEC_RMON_R_UNDERSIZE0        (*(volatile uint32_t*)(void*)(&__MBAR[0x009294]))
#define MCF548X_FEC_RMON_R_OVERSIZE0         (*(volatile uint32_t*)(void*)(&__MBAR[0x009298]))
#define MCF548X_FEC_RMON_R_FRAG0             (*(volatile uint32_t*)(void*)(&__MBAR[0x00929C]))
#define MCF548X_FEC_RMON_R_JAB0              (*(volatile uint32_t*)(void*)(&__MBAR[0x0092A0]))
#define MCF548X_FEC_RMON_R_RESVD_00          (*(volatile uint32_t*)(void*)(&__MBAR[0x0092A4]))
#define MCF548X_FEC_RMON_R_P640              (*(volatile uint32_t*)(void*)(&__MBAR[0x0092A8]))
#define MCF548X_FEC_RMON_R_P65TO1270         (*(volatile uint32_t*)(void*)(&__MBAR[0x0092AC]))
#define MCF548X_FEC_RMON_R_P128TO2550        (*(volatile uint32_t*)(void*)(&__MBAR[0x0092B0]))
#define MCF548X_FEC_RMON_R_P256TO5110        (*(volatile uint32_t*)(void*)(&__MBAR[0x0092B4]))
#define MCF548X_FEC_RMON_R_512TO10230        (*(volatile uint32_t*)(void*)(&__MBAR[0x0092B8]))
#define MCF548X_FEC_RMON_R_1024TO20470       (*(volatile uint32_t*)(void*)(&__MBAR[0x0092BC]))
#define MCF548X_FEC_RMON_R_P_GTE20480        (*(volatile uint32_t*)(void*)(&__MBAR[0x0092C0]))
#define MCF548X_FEC_RMON_R_OCTETS0           (*(volatile uint32_t*)(void*)(&__MBAR[0x0092C4]))
#define MCF548X_FEC_IEEE_R_DROP0             (*(volatile uint32_t*)(void*)(&__MBAR[0x0092C8]))
#define MCF548X_FEC_IEEE_R_FRAME_OK0         (*(volatile uint32_t*)(void*)(&__MBAR[0x0092CC]))
#define MCF548X_FEC_IEEE_R_CRC0              (*(volatile uint32_t*)(void*)(&__MBAR[0x0092D0]))
#define MCF548X_FEC_IEEE_R_ALIGN0            (*(volatile uint32_t*)(void*)(&__MBAR[0x0092D4]))
#define MCF548X_FEC_IEEE_R_MACERR0           (*(volatile uint32_t*)(void*)(&__MBAR[0x0092D8]))
#define MCF548X_FEC_IEEE_R_FDXFC0            (*(volatile uint32_t*)(void*)(&__MBAR[0x0092DC]))
#define MCF548X_FEC_IEEE_R_OCTETS_OK0        (*(volatile uint32_t*)(void*)(&__MBAR[0x0092E0]))
#define MCF548X_FEC_EIR1                     (*(volatile uint32_t*)(void*)(&__MBAR[0x009804]))
#define MCF548X_FEC_EIMR1                    (*(volatile uint32_t*)(void*)(&__MBAR[0x009808]))
#define MCF548X_FEC_ECR1                     (*(volatile uint32_t*)(void*)(&__MBAR[0x009824]))
#define MCF548X_FEC_MMFR1                    (*(volatile uint32_t*)(void*)(&__MBAR[0x009840]))
#define MCF548X_FEC_MSCR1                    (*(volatile uint32_t*)(void*)(&__MBAR[0x009844]))
#define MCF548X_FEC_MIBC1                    (*(volatile uint32_t*)(void*)(&__MBAR[0x009864]))
#define MCF548X_FEC_RCR1                     (*(volatile uint32_t*)(void*)(&__MBAR[0x009884]))
#define MCF548X_FEC_R_HASH1                  (*(volatile uint32_t*)(void*)(&__MBAR[0x009888]))
#define MCF548X_FEC_TCR1                     (*(volatile uint32_t*)(void*)(&__MBAR[0x0098C4]))
#define MCF548X_FEC_PALR1                    (*(volatile uint32_t*)(void*)(&__MBAR[0x0098E4]))
#define MCF548X_FEC_PAUR1                    (*(volatile uint32_t*)(void*)(&__MBAR[0x0098E8]))
#define MCF548X_FEC_OPD1                     (*(volatile uint32_t*)(void*)(&__MBAR[0x0098EC]))
#define MCF548X_FEC_IAUR1                    (*(volatile uint32_t*)(void*)(&__MBAR[0x009918]))
#define MCF548X_FEC_IALR1                    (*(volatile uint32_t*)(void*)(&__MBAR[0x00991C]))
#define MCF548X_FEC_GAUR1                    (*(volatile uint32_t*)(void*)(&__MBAR[0x009920]))
#define MCF548X_FEC_GALR1                    (*(volatile uint32_t*)(void*)(&__MBAR[0x009924]))
#define MCF548X_FEC_FECTFWR1                 (*(volatile uint32_t*)(void*)(&__MBAR[0x009944]))
#define MCF548X_FEC_FECRFDR1                 (*(volatile uint32_t*)(void*)(&__MBAR[0x009984]))
#define MCF548X_FEC_FECRFSR1                 (*(volatile uint32_t*)(void*)(&__MBAR[0x009988]))
#define MCF548X_FEC_FECRFCR1                 (*(volatile uint32_t*)(void*)(&__MBAR[0x00998C]))
#define MCF548X_FEC_FECRLRFP1                (*(volatile uint32_t*)(void*)(&__MBAR[0x009990]))
#define MCF548X_FEC_FECRLWFP1                (*(volatile uint32_t*)(void*)(&__MBAR[0x009994]))
#define MCF548X_FEC_FECRFAR1                 (*(volatile uint32_t*)(void*)(&__MBAR[0x009998]))
#define MCF548X_FEC_FECRFRP1                 (*(volatile uint32_t*)(void*)(&__MBAR[0x00999C]))
#define MCF548X_FEC_FECRFWP1                 (*(volatile uint32_t*)(void*)(&__MBAR[0x0099A0]))
#define MCF548X_FEC_FECTFDR1                 (*(volatile uint32_t*)(void*)(&__MBAR[0x0099A4]))
#define MCF548X_FEC_FECTFSR1                 (*(volatile uint32_t*)(void*)(&__MBAR[0x0099A8]))
#define MCF548X_FEC_FECTFCR1                 (*(volatile uint32_t*)(void*)(&__MBAR[0x0099AC]))
#define MCF548X_FEC_FECTLRFP1                (*(volatile uint32_t*)(void*)(&__MBAR[0x0099B0]))
#define MCF548X_FEC_FECTLWFP1                (*(volatile uint32_t*)(void*)(&__MBAR[0x0099B4]))
#define MCF548X_FEC_FECTFAR1                 (*(volatile uint32_t*)(void*)(&__MBAR[0x0099B8]))
#define MCF548X_FEC_FECTFRP1                 (*(volatile uint32_t*)(void*)(&__MBAR[0x0099BC]))
#define MCF548X_FEC_FECTFWP1                 (*(volatile uint32_t*)(void*)(&__MBAR[0x0099C0]))
#define MCF548X_FEC_FRST1                    (*(volatile uint32_t*)(void*)(&__MBAR[0x0099C4]))
#define MCF548X_FEC_CTCWR1                   (*(volatile uint32_t*)(void*)(&__MBAR[0x0099C8]))
#define MCF548X_FEC_RMON_T_DROP1             (*(volatile uint32_t*)(void*)(&__MBAR[0x009A00]))
#define MCF548X_FEC_RMON_T_PACKETS1          (*(volatile uint32_t*)(void*)(&__MBAR[0x009A04]))
#define MCF548X_FEC_RMON_T_BC_PKT1           (*(volatile uint32_t*)(void*)(&__MBAR[0x009A08]))
#define MCF548X_FEC_RMON_T_MC_PKT1           (*(volatile uint32_t*)(void*)(&__MBAR[0x009A0C]))
#define MCF548X_FEC_RMON_T_CRC_ALIGN1        (*(volatile uint32_t*)(void*)(&__MBAR[0x009A10]))
#define MCF548X_FEC_RMON_T_UNDERSIZE1        (*(volatile uint32_t*)(void*)(&__MBAR[0x009A14]))
#define MCF548X_FEC_RMON_T_OVERSIZE1         (*(volatile uint32_t*)(void*)(&__MBAR[0x009A18]))
#define MCF548X_FEC_RMON_T_FRAG1             (*(volatile uint32_t*)(void*)(&__MBAR[0x009A1C]))
#define MCF548X_FEC_RMON_T_JAB1              (*(volatile uint32_t*)(void*)(&__MBAR[0x009A20]))
#define MCF548X_FEC_RMON_T_COL1              (*(volatile uint32_t*)(void*)(&__MBAR[0x009A24]))
#define MCF548X_FEC_RMON_T_P641              (*(volatile uint32_t*)(void*)(&__MBAR[0x009A28]))
#define MCF548X_FEC_RMON_T_P65TO1271         (*(volatile uint32_t*)(void*)(&__MBAR[0x009A2C]))
#define MCF548X_FEC_RMON_T_P128TO2551        (*(volatile uint32_t*)(void*)(&__MBAR[0x009A30]))
#define MCF548X_FEC_RMON_T_P256TO5111        (*(volatile uint32_t*)(void*)(&__MBAR[0x009A34]))
#define MCF548X_FEC_RMON_T_P512TO10231       (*(volatile uint32_t*)(void*)(&__MBAR[0x009A38]))
#define MCF548X_FEC_RMON_T_P1024TO20471      (*(volatile uint32_t*)(void*)(&__MBAR[0x009A3C]))
#define MCF548X_FEC_RMON_T_P_GTE20481        (*(volatile uint32_t*)(void*)(&__MBAR[0x009A40]))
#define MCF548X_FEC_RMON_T_OCTETS1           (*(volatile uint32_t*)(void*)(&__MBAR[0x009A44]))
#define MCF548X_FEC_IEEE_T_DROP1             (*(volatile uint32_t*)(void*)(&__MBAR[0x009A48]))
#define MCF548X_FEC_IEEE_T_FRAME_OK1         (*(volatile uint32_t*)(void*)(&__MBAR[0x009A4C]))
#define MCF548X_FEC_IEEE_T_1COL1             (*(volatile uint32_t*)(void*)(&__MBAR[0x009A50]))
#define MCF548X_FEC_IEEE_T_MCOL1             (*(volatile uint32_t*)(void*)(&__MBAR[0x009A54]))
#define MCF548X_FEC_IEEE_T_DEF1              (*(volatile uint32_t*)(void*)(&__MBAR[0x009A58]))
#define MCF548X_FEC_IEEE_T_LCOL1             (*(volatile uint32_t*)(void*)(&__MBAR[0x009A5C]))
#define MCF548X_FEC_IEEE_T_EXCOL1            (*(volatile uint32_t*)(void*)(&__MBAR[0x009A60]))
#define MCF548X_FEC_IEEE_T_MACERR1           (*(volatile uint32_t*)(void*)(&__MBAR[0x009A64]))
#define MCF548X_FEC_IEEE_T_CSERR1            (*(volatile uint32_t*)(void*)(&__MBAR[0x009A68]))
#define MCF548X_FEC_IEEE_T_SQE1              (*(volatile uint32_t*)(void*)(&__MBAR[0x009A6C]))
#define MCF548X_FEC_IEEE_T_FDXFC1            (*(volatile uint32_t*)(void*)(&__MBAR[0x009A70]))
#define MCF548X_FEC_IEEE_T_OCTETS_OK1        (*(volatile uint32_t*)(void*)(&__MBAR[0x009A74]))
#define MCF548X_FEC_RMON_R_PACKETS1          (*(volatile uint32_t*)(void*)(&__MBAR[0x009A84]))
#define MCF548X_FEC_RMON_R_BC_PKT1           (*(volatile uint32_t*)(void*)(&__MBAR[0x009A88]))
#define MCF548X_FEC_RMON_R_MC_PKT1           (*(volatile uint32_t*)(void*)(&__MBAR[0x009A8C]))
#define MCF548X_FEC_RMON_R_CRC_ALIGN1        (*(volatile uint32_t*)(void*)(&__MBAR[0x009A90]))
#define MCF548X_FEC_RMON_R_UNDERSIZE1        (*(volatile uint32_t*)(void*)(&__MBAR[0x009A94]))
#define MCF548X_FEC_RMON_R_OVERSIZE1         (*(volatile uint32_t*)(void*)(&__MBAR[0x009A98]))
#define MCF548X_FEC_RMON_R_FRAG1             (*(volatile uint32_t*)(void*)(&__MBAR[0x009A9C]))
#define MCF548X_FEC_RMON_R_JAB1              (*(volatile uint32_t*)(void*)(&__MBAR[0x009AA0]))
#define MCF548X_FEC_RMON_R_RESVD_01          (*(volatile uint32_t*)(void*)(&__MBAR[0x009AA4]))
#define MCF548X_FEC_RMON_R_P641              (*(volatile uint32_t*)(void*)(&__MBAR[0x009AA8]))
#define MCF548X_FEC_RMON_R_P65TO1271         (*(volatile uint32_t*)(void*)(&__MBAR[0x009AAC]))
#define MCF548X_FEC_RMON_R_P128TO2551        (*(volatile uint32_t*)(void*)(&__MBAR[0x009AB0]))
#define MCF548X_FEC_RMON_R_P256TO5111        (*(volatile uint32_t*)(void*)(&__MBAR[0x009AB4]))
#define MCF548X_FEC_RMON_R_512TO10231        (*(volatile uint32_t*)(void*)(&__MBAR[0x009AB8]))
#define MCF548X_FEC_RMON_R_1024TO20471       (*(volatile uint32_t*)(void*)(&__MBAR[0x009ABC]))
#define MCF548X_FEC_RMON_R_P_GTE20481        (*(volatile uint32_t*)(void*)(&__MBAR[0x009AC0]))
#define MCF548X_FEC_RMON_R_OCTETS1           (*(volatile uint32_t*)(void*)(&__MBAR[0x009AC4]))
#define MCF548X_FEC_IEEE_R_DROP1             (*(volatile uint32_t*)(void*)(&__MBAR[0x009AC8]))
#define MCF548X_FEC_IEEE_R_FRAME_OK1         (*(volatile uint32_t*)(void*)(&__MBAR[0x009ACC]))
#define MCF548X_FEC_IEEE_R_CRC1              (*(volatile uint32_t*)(void*)(&__MBAR[0x009AD0]))
#define MCF548X_FEC_IEEE_R_ALIGN1            (*(volatile uint32_t*)(void*)(&__MBAR[0x009AD4]))
#define MCF548X_FEC_IEEE_R_MACERR1           (*(volatile uint32_t*)(void*)(&__MBAR[0x009AD8]))
#define MCF548X_FEC_IEEE_R_FDXFC1            (*(volatile uint32_t*)(void*)(&__MBAR[0x009ADC]))
#define MCF548X_FEC_IEEE_R_OCTETS_OK1        (*(volatile uint32_t*)(void*)(&__MBAR[0x009AE0]))
#define MCF548X_FEC_EIR(x)                   (*(volatile uint32_t*)(void*)(&__MBAR[0x009004U+((x)*0x800)]))
#define MCF548X_FEC_EIMR(x)                  (*(volatile uint32_t*)(void*)(&__MBAR[0x009008U+((x)*0x800)]))
#define MCF548X_FEC_ECR(x)                   (*(volatile uint32_t*)(void*)(&__MBAR[0x009024U+((x)*0x800)]))
#define MCF548X_FEC_MMFR(x)                  (*(volatile uint32_t*)(void*)(&__MBAR[0x009040U+((x)*0x800)]))
#define MCF548X_FEC_MSCR(x)                  (*(volatile uint32_t*)(void*)(&__MBAR[0x009044U+((x)*0x800)]))
#define MCF548X_FEC_MIBC(x)                  (*(volatile uint32_t*)(void*)(&__MBAR[0x009064U+((x)*0x800)]))
#define MCF548X_FEC_RCR(x)                   (*(volatile uint32_t*)(void*)(&__MBAR[0x009084U+((x)*0x800)]))
#define MCF548X_FEC_R_HASH(x)                (*(volatile uint32_t*)(void*)(&__MBAR[0x009088U+((x)*0x800)]))
#define MCF548X_FEC_TCR(x)                   (*(volatile uint32_t*)(void*)(&__MBAR[0x0090C4U+((x)*0x800)]))
#define MCF548X_FEC_PALR(x)                  (*(volatile uint32_t*)(void*)(&__MBAR[0x0090E4U+((x)*0x800)]))
#define MCF548X_FEC_PAUR(x)                  (*(volatile uint32_t*)(void*)(&__MBAR[0x0090E8U+((x)*0x800)]))
#define MCF548X_FEC_OPD(x)                   (*(volatile uint32_t*)(void*)(&__MBAR[0x0090ECU+((x)*0x800)]))
#define MCF548X_FEC_IAUR(x)                  (*(volatile uint32_t*)(void*)(&__MBAR[0x009118U+((x)*0x800)]))
#define MCF548X_FEC_IALR(x)                  (*(volatile uint32_t*)(void*)(&__MBAR[0x00911CU+((x)*0x800)]))
#define MCF548X_FEC_GAUR(x)                  (*(volatile uint32_t*)(void*)(&__MBAR[0x009120U+((x)*0x800)]))
#define MCF548X_FEC_GALR(x)                  (*(volatile uint32_t*)(void*)(&__MBAR[0x009124U+((x)*0x800)]))
#define MCF548X_FEC_FECTFWR(x)               (*(volatile uint32_t*)(void*)(&__MBAR[0x009144U+((x)*0x800)]))
#define MCF548X_FEC_FECRFDR(x)               (*(volatile uint32_t*)(void*)(&__MBAR[0x009184U+((x)*0x800)]))
#define MCF548X_FEC_FECRFSR(x)               (*(volatile uint32_t*)(void*)(&__MBAR[0x009188U+((x)*0x800)]))
#define MCF548X_FEC_FECRFCR(x)               (*(volatile uint32_t*)(void*)(&__MBAR[0x00918CU+((x)*0x800)]))
#define MCF548X_FEC_FECRLRFP(x)              (*(volatile uint32_t*)(void*)(&__MBAR[0x009190U+((x)*0x800)]))
#define MCF548X_FEC_FECRLWFP(x)              (*(volatile uint32_t*)(void*)(&__MBAR[0x009194U+((x)*0x800)]))
#define MCF548X_FEC_FECRFAR(x)               (*(volatile uint32_t*)(void*)(&__MBAR[0x009198U+((x)*0x800)]))
#define MCF548X_FEC_FECRFRP(x)               (*(volatile uint32_t*)(void*)(&__MBAR[0x00919CU+((x)*0x800)]))
#define MCF548X_FEC_FECRFWP(x)               (*(volatile uint32_t*)(void*)(&__MBAR[0x0091A0U+((x)*0x800)]))
#define MCF548X_FEC_FECTFDR(x)               (*(volatile uint32_t*)(void*)(&__MBAR[0x0091A4U+((x)*0x800)]))
#define MCF548X_FEC_FECTFSR(x)               (*(volatile uint32_t*)(void*)(&__MBAR[0x0091A8U+((x)*0x800)]))
#define MCF548X_FEC_FECTFCR(x)               (*(volatile uint32_t*)(void*)(&__MBAR[0x0091ACU+((x)*0x800)]))
#define MCF548X_FEC_FECTLRFP(x)              (*(volatile uint32_t*)(void*)(&__MBAR[0x0091B0U+((x)*0x800)]))
#define MCF548X_FEC_FECTLWFP(x)              (*(volatile uint32_t*)(void*)(&__MBAR[0x0091B4U+((x)*0x800)]))
#define MCF548X_FEC_FECTFAR(x)               (*(volatile uint32_t*)(void*)(&__MBAR[0x0091B8U+((x)*0x800)]))
#define MCF548X_FEC_FECTFRP(x)               (*(volatile uint32_t*)(void*)(&__MBAR[0x0091BCU+((x)*0x800)]))
#define MCF548X_FEC_FECTFWP(x)               (*(volatile uint32_t*)(void*)(&__MBAR[0x0091C0U+((x)*0x800)]))
#define MCF548X_FEC_FRST(x)                  (*(volatile uint32_t*)(void*)(&__MBAR[0x0091C4U+((x)*0x800)]))
#define MCF548X_FEC_CTCWR(x)                 (*(volatile uint32_t*)(void*)(&__MBAR[0x0091C8U+((x)*0x800)]))
#define MCF548X_FEC_RMON_T_DROP(x)           (*(volatile uint32_t*)(void*)(&__MBAR[0x009200U+((x)*0x800)]))
#define MCF548X_FEC_RMON_T_PACKETS(x)        (*(volatile uint32_t*)(void*)(&__MBAR[0x009204U+((x)*0x800)]))
#define MCF548X_FEC_RMON_T_BC_PKT(x)         (*(volatile uint32_t*)(void*)(&__MBAR[0x009208U+((x)*0x800)]))
#define MCF548X_FEC_RMON_T_MC_PKT(x)         (*(volatile uint32_t*)(void*)(&__MBAR[0x00920CU+((x)*0x800)]))
#define MCF548X_FEC_RMON_T_CRC_ALIGN(x)      (*(volatile uint32_t*)(void*)(&__MBAR[0x009210U+((x)*0x800)]))
#define MCF548X_FEC_RMON_T_UNDERSIZE(x)      (*(volatile uint32_t*)(void*)(&__MBAR[0x009214U+((x)*0x800)]))
#define MCF548X_FEC_RMON_T_OVERSIZE(x)       (*(volatile uint32_t*)(void*)(&__MBAR[0x009218U+((x)*0x800)]))
#define MCF548X_FEC_RMON_T_FRAG(x)           (*(volatile uint32_t*)(void*)(&__MBAR[0x00921CU+((x)*0x800)]))
#define MCF548X_FEC_RMON_T_JAB(x)            (*(volatile uint32_t*)(void*)(&__MBAR[0x009220U+((x)*0x800)]))
#define MCF548X_FEC_RMON_T_COL(x)            (*(volatile uint32_t*)(void*)(&__MBAR[0x009224U+((x)*0x800)]))
#define MCF548X_FEC_RMON_T_P64(x)            (*(volatile uint32_t*)(void*)(&__MBAR[0x009228U+((x)*0x800)]))
#define MCF548X_FEC_RMON_T_P65TO127(x)       (*(volatile uint32_t*)(void*)(&__MBAR[0x00922CU+((x)*0x800)]))
#define MCF548X_FEC_RMON_T_P128TO255(x)      (*(volatile uint32_t*)(void*)(&__MBAR[0x009230U+((x)*0x800)]))
#define MCF548X_FEC_RMON_T_P256TO511(x)      (*(volatile uint32_t*)(void*)(&__MBAR[0x009234U+((x)*0x800)]))
#define MCF548X_FEC_RMON_T_P512TO1023(x)     (*(volatile uint32_t*)(void*)(&__MBAR[0x009238U+((x)*0x800)]))
#define MCF548X_FEC_RMON_T_P1024TO2047(x)    (*(volatile uint32_t*)(void*)(&__MBAR[0x00923CU+((x)*0x800)]))
#define MCF548X_FEC_RMON_T_P_GTE2048(x)      (*(volatile uint32_t*)(void*)(&__MBAR[0x009240U+((x)*0x800)]))
#define MCF548X_FEC_RMON_T_OCTETS(x)         (*(volatile uint32_t*)(void*)(&__MBAR[0x009244U+((x)*0x800)]))
#define MCF548X_FEC_IEEE_T_DROP(x)           (*(volatile uint32_t*)(void*)(&__MBAR[0x009248U+((x)*0x800)]))
#define MCF548X_FEC_IEEE_T_FRAME_OK(x)       (*(volatile uint32_t*)(void*)(&__MBAR[0x00924CU+((x)*0x800)]))
#define MCF548X_FEC_IEEE_T_1COL(x)           (*(volatile uint32_t*)(void*)(&__MBAR[0x009250U+((x)*0x800)]))
#define MCF548X_FEC_IEEE_T_MCOL(x)           (*(volatile uint32_t*)(void*)(&__MBAR[0x009254U+((x)*0x800)]))
#define MCF548X_FEC_IEEE_T_DEF(x)            (*(volatile uint32_t*)(void*)(&__MBAR[0x009258U+((x)*0x800)]))
#define MCF548X_FEC_IEEE_T_LCOL(x)           (*(volatile uint32_t*)(void*)(&__MBAR[0x00925CU+((x)*0x800)]))
#define MCF548X_FEC_IEEE_T_EXCOL(x)          (*(volatile uint32_t*)(void*)(&__MBAR[0x009260U+((x)*0x800)]))
#define MCF548X_FEC_IEEE_T_MACERR(x)         (*(volatile uint32_t*)(void*)(&__MBAR[0x009264U+((x)*0x800)]))
#define MCF548X_FEC_IEEE_T_CSERR(x)          (*(volatile uint32_t*)(void*)(&__MBAR[0x009268U+((x)*0x800)]))
#define MCF548X_FEC_IEEE_T_SQE(x)            (*(volatile uint32_t*)(void*)(&__MBAR[0x00926CU+((x)*0x800)]))
#define MCF548X_FEC_IEEE_T_FDXFC(x)          (*(volatile uint32_t*)(void*)(&__MBAR[0x009270U+((x)*0x800)]))
#define MCF548X_FEC_IEEE_T_OCTETS_OK(x)      (*(volatile uint32_t*)(void*)(&__MBAR[0x009274U+((x)*0x800)]))
#define MCF548X_FEC_RMON_R_PACKETS(x)        (*(volatile uint32_t*)(void*)(&__MBAR[0x009284U+((x)*0x800)]))
#define MCF548X_FEC_RMON_R_BC_PKT(x)         (*(volatile uint32_t*)(void*)(&__MBAR[0x009288U+((x)*0x800)]))
#define MCF548X_FEC_RMON_R_MC_PKT(x)         (*(volatile uint32_t*)(void*)(&__MBAR[0x00928CU+((x)*0x800)]))
#define MCF548X_FEC_RMON_R_CRC_ALIGN(x)      (*(volatile uint32_t*)(void*)(&__MBAR[0x009290U+((x)*0x800)]))
#define MCF548X_FEC_RMON_R_UNDERSIZE(x)      (*(volatile uint32_t*)(void*)(&__MBAR[0x009294U+((x)*0x800)]))
#define MCF548X_FEC_RMON_R_OVERSIZE(x)       (*(volatile uint32_t*)(void*)(&__MBAR[0x009298U+((x)*0x800)]))
#define MCF548X_FEC_RMON_R_FRAG(x)           (*(volatile uint32_t*)(void*)(&__MBAR[0x00929CU+((x)*0x800)]))
#define MCF548X_FEC_RMON_R_JAB(x)            (*(volatile uint32_t*)(void*)(&__MBAR[0x0092A0U+((x)*0x800)]))
#define MCF548X_FEC_RMON_R_RESVD_0(x)        (*(volatile uint32_t*)(void*)(&__MBAR[0x0092A4U+((x)*0x800)]))
#define MCF548X_FEC_RMON_R_P64(x)            (*(volatile uint32_t*)(void*)(&__MBAR[0x0092A8U+((x)*0x800)]))
#define MCF548X_FEC_RMON_R_P65TO127(x)       (*(volatile uint32_t*)(void*)(&__MBAR[0x0092ACU+((x)*0x800)]))
#define MCF548X_FEC_RMON_R_P128TO255(x)      (*(volatile uint32_t*)(void*)(&__MBAR[0x0092B0U+((x)*0x800)]))
#define MCF548X_FEC_RMON_R_P256TO511(x)      (*(volatile uint32_t*)(void*)(&__MBAR[0x0092B4U+((x)*0x800)]))
#define MCF548X_FEC_RMON_R_512TO1023(x)      (*(volatile uint32_t*)(void*)(&__MBAR[0x0092B8U+((x)*0x800)]))
#define MCF548X_FEC_RMON_R_1024TO2047(x)     (*(volatile uint32_t*)(void*)(&__MBAR[0x0092BCU+((x)*0x800)]))
#define MCF548X_FEC_RMON_R_P_GTE2048(x)      (*(volatile uint32_t*)(void*)(&__MBAR[0x0092C0U+((x)*0x800)]))
#define MCF548X_FEC_RMON_R_OCTETS(x)         (*(volatile uint32_t*)(void*)(&__MBAR[0x0092C4U+((x)*0x800)]))
#define MCF548X_FEC_IEEE_R_DROP(x)           (*(volatile uint32_t*)(void*)(&__MBAR[0x0092C8U+((x)*0x800)]))
#define MCF548X_FEC_IEEE_R_FRAME_OK(x)       (*(volatile uint32_t*)(void*)(&__MBAR[0x0092CCU+((x)*0x800)]))
#define MCF548X_FEC_IEEE_R_CRC(x)            (*(volatile uint32_t*)(void*)(&__MBAR[0x0092D0U+((x)*0x800)]))
#define MCF548X_FEC_IEEE_R_ALIGN(x)          (*(volatile uint32_t*)(void*)(&__MBAR[0x0092D4U+((x)*0x800)]))
#define MCF548X_FEC_IEEE_R_MACERR(x)         (*(volatile uint32_t*)(void*)(&__MBAR[0x0092D8U+((x)*0x800)]))
#define MCF548X_FEC_IEEE_R_FDXFC(x)          (*(volatile uint32_t*)(void*)(&__MBAR[0x0092DCU+((x)*0x800)]))
#define MCF548X_FEC_IEEE_R_OCTETS_OK(x)      (*(volatile uint32_t*)(void*)(&__MBAR[0x0092E0U+((x)*0x800)]))

/* Bit definitions and macros for MCF548X_FEC_EIR */
#define MCF548X_FEC_EIR_RFERR              (0x00020000)
#define MCF548X_FEC_EIR_XFERR              (0x00040000)
#define MCF548X_FEC_EIR_XFUN               (0x00080000)
#define MCF548X_FEC_EIR_RL                 (0x00100000)
#define MCF548X_FEC_EIR_LC                 (0x00200000)
#define MCF548X_FEC_EIR_MII                (0x00800000)
#define MCF548X_FEC_EIR_TXF                (0x08000000)
#define MCF548X_FEC_EIR_GRA                (0x10000000)
#define MCF548X_FEC_EIR_BABT               (0x20000000)
#define MCF548X_FEC_EIR_BABR               (0x40000000)
#define MCF548X_FEC_EIR_HBERR              (0x80000000)
#define MCF548X_FEC_EIR_CLEAR_ALL          (0xFFFFFFFF)

/* Bit definitions and macros for MCF548X_FEC_EIMR */
#define MCF548X_FEC_EIMR_RFERR             (0x00020000)
#define MCF548X_FEC_EIMR_XFERR             (0x00040000)
#define MCF548X_FEC_EIMR_XFUN              (0x00080000)
#define MCF548X_FEC_EIMR_RL                (0x00100000)
#define MCF548X_FEC_EIMR_LC                (0x00200000)
#define MCF548X_FEC_EIMR_MII               (0x00800000)
#define MCF548X_FEC_EIMR_TXF               (0x08000000)
#define MCF548X_FEC_EIMR_GRA               (0x10000000)
#define MCF548X_FEC_EIMR_BABT              (0x20000000)
#define MCF548X_FEC_EIMR_BABR              (0x40000000)
#define MCF548X_FEC_EIMR_HBERR             (0x80000000)
#define MCF548X_FEC_EIMR_MASK_ALL          (0x00000000)
#define MCF548X_FEC_EIMR_UNMASK_ALL        (0xFFFFFFFF)

/* Bit definitions and macros for MCF548X_FEC_ECR */
#define MCF548X_FEC_ECR_RESET              (0x00000001)
#define MCF548X_FEC_ECR_ETHER_EN           (0x00000002)

/* Bit definitions and macros for MCF548X_FEC_MMFR */
#define MCF548X_FEC_MMFR_DATA(x)           (((x)&0x0000FFFF)<<0)
#define MCF548X_FEC_MMFR_TA(x)             (((x)&0x00000003)<<16)
#define MCF548X_FEC_MMFR_RA(x)             (((x)&0x0000001F)<<18)
#define MCF548X_FEC_MMFR_PA(x)             (((x)&0x0000001F)<<23)
#define MCF548X_FEC_MMFR_OP(x)             (((x)&0x00000003)<<28)
#define MCF548X_FEC_MMFR_ST(x)             (((x)&0x00000003)<<30)
#define MCF548X_FEC_MMFR_ST_01             (0x40000000)
#define MCF548X_FEC_MMFR_OP_READ           (0x20000000)
#define MCF548X_FEC_MMFR_OP_WRITE          (0x10000000)
#define MCF548X_FEC_MMFR_TA_10             (0x00020000)

/* Bit definitions and macros for MCF548X_FEC_MSCR */
#define MCF548X_FEC_MSCR_MII_SPEED(x)      (((x)&0x0000003F)<<1)
#define MCF548X_FEC_MSCR_DIS_PREAMBLE      (0x00000080)
#define MCF548X_FEC_MSCR_MII_SPEED_133     (0x1B<<1)
#define MCF548X_FEC_MSCR_MII_SPEED_120     (0x18<<1)
#define MCF548X_FEC_MSCR_MII_SPEED_66      (0xE<<1)
#define MCF548X_FEC_MSCR_MII_SPEED_60      (0xC<<1)

/* Bit definitions and macros for MCF548X_FEC_MIBC */
#define MCF548X_FEC_MIBC_MIB_IDLE          (0x40000000)
#define MCF548X_FEC_MIBC_MIB_DISABLE       (0x80000000)

/* Bit definitions and macros for MCF548X_FEC_RCR */
#define MCF548X_FEC_RCR_LOOP               (0x00000001)
#define MCF548X_FEC_RCR_DRT                (0x00000002)
#define MCF548X_FEC_RCR_MII_MODE           (0x00000004)
#define MCF548X_FEC_RCR_PROM               (0x00000008)
#define MCF548X_FEC_RCR_BC_REJ             (0x00000010)
#define MCF548X_FEC_RCR_FCE                (0x00000020)
#define MCF548X_FEC_RCR_MAX_FL(x)          (((x)&0x000007FF)<<16)

/* Bit definitions and macros for MCF548X_FEC_R_HASH */
#define MCF548X_FEC_R_HASH_HASH(x)         (((x)&0x0000003F)<<24)
#define MCF548X_FEC_R_HASH_MULTCAST        (0x40000000)
#define MCF548X_FEC_R_HASH_FCE_DC          (0x80000000)

/* Bit definitions and macros for MCF548X_FEC_TCR */
#define MCF548X_FEC_TCR_GTS                (0x00000001)
#define MCF548X_FEC_TCR_HBC                (0x00000002)
#define MCF548X_FEC_TCR_FDEN               (0x00000004)
#define MCF548X_FEC_TCR_TFC_PAUSE          (0x00000008)
#define MCF548X_FEC_TCR_RFC_PAUSE          (0x00000010)

/* Bit definitions and macros for MCF548X_FEC_PAUR */
#define MCF548X_FEC_PAUR_TYPE(x)           (((x)&0x0000FFFF)<<0)
#define MCF548X_FEC_PAUR_PADDR2(x)         (((x)&0x0000FFFF)<<16)

/* Bit definitions and macros for MCF548X_FEC_OPD */
#define MCF548X_FEC_OPD_OP_PAUSE(x)        (((x)&0x0000FFFF)<<0)
#define MCF548X_FEC_OPD_OPCODE(x)          (((x)&0x0000FFFF)<<16)

/* Bit definitions and macros for MCF548X_FEC_FECTFWR */
#define MCF548X_FEC_FECTFWR_X_WMRK(x)      (((x)&0x0000000F)<<0)
#define MCF548X_FEC_FECTFWR_X_WMRK_64      (0x00000000)
#define MCF548X_FEC_FECTFWR_X_WMRK_128     (0x00000001)
#define MCF548X_FEC_FECTFWR_X_WMRK_192     (0x00000002)
#define MCF548X_FEC_FECTFWR_X_WMRK_256     (0x00000003)
#define MCF548X_FEC_FECTFWR_X_WMRK_320     (0x00000004)
#define MCF548X_FEC_FECTFWR_X_WMRK_384     (0x00000005)
#define MCF548X_FEC_FECTFWR_X_WMRK_448     (0x00000006)
#define MCF548X_FEC_FECTFWR_X_WMRK_512     (0x00000007)
#define MCF548X_FEC_FECTFWR_X_WMRK_576     (0x00000008)
#define MCF548X_FEC_FECTFWR_X_WMRK_640     (0x00000009)
#define MCF548X_FEC_FECTFWR_X_WMRK_704     (0x0000000A)
#define MCF548X_FEC_FECTFWR_X_WMRK_768     (0x0000000B)
#define MCF548X_FEC_FECTFWR_X_WMRK_832     (0x0000000C)
#define MCF548X_FEC_FECTFWR_X_WMRK_896     (0x0000000D)
#define MCF548X_FEC_FECTFWR_X_WMRK_960     (0x0000000E)
#define MCF548X_FEC_FECTFWR_X_WMRK_1024    (0x0000000F)

/* Bit definitions and macros for MCF548X_FEC_FECRFDR */
#define MCF548X_FEC_FECRFDR_ADDR0            ((void*)(&__MBAR[0x009184]))
#define MCF548X_FEC_FECRFDR_ADDR1            ((void*)(&__MBAR[0x009984]))
#define MCF548X_FEC_FECRFDR_ADDR(x)          ((void*)(&__MBAR[0x009184U+(0x800*x)]))

/* Bit definitions and macros for MCF548X_FEC_FECRFSR */
#define MCF548X_FEC_FECRFSR_EMT            (0x00010000)
#define MCF548X_FEC_FECRFSR_ALARM          (0x00020000)
#define MCF548X_FEC_FECRFSR_FU             (0x00040000)
#define MCF548X_FEC_FECRFSR_FR             (0x00080000)
#define MCF548X_FEC_FECRFSR_OF             (0x00100000)
#define MCF548X_FEC_FECRFSR_UF             (0x00200000)
#define MCF548X_FEC_FECRFSR_RXW            (0x00400000)
#define MCF548X_FEC_FECRFSR_FAE            (0x00800000)
#define MCF548X_FEC_FECRFSR_FRM(x)         (((x)&0x0000000F)<<24)
#define MCF548X_FEC_FECRFSR_IP             (0x80000000)

/* Bit definitions and macros for MCF548X_FEC_FECRFCR */
#define MCF548X_FEC_FECRFCR_COUNTER(x)     (((x)&0x0000FFFF)<<0)
#define MCF548X_FEC_FECRFCR_OF_MSK         (0x00080000)
#define MCF548X_FEC_FECRFCR_UF_MSK         (0x00100000)
#define MCF548X_FEC_FECRFCR_RXW_MSK        (0x00200000)
#define MCF548X_FEC_FECRFCR_FAE_MSK        (0x00400000)
#define MCF548X_FEC_FECRFCR_IP_MSK         (0x00800000)
#define MCF548X_FEC_FECRFCR_GR(x)          (((x)&0x00000007)<<24)
#define MCF548X_FEC_FECRFCR_FRM            (0x08000000)
#define MCF548X_FEC_FECRFCR_TIMER          (0x10000000)
#define MCF548X_FEC_FECRFCR_WFR            (0x20000000)
#define MCF548X_FEC_FECRFCR_WCTL           (0x40000000)

/* Bit definitions and macros for MCF548X_FEC_FECRLRFP */
#define MCF548X_FEC_FECRLRFP_LRFP(x)       (((x)&0x00000FFF)<<0)

/* Bit definitions and macros for MCF548X_FEC_FECRLWFP */
#define MCF548X_FEC_FECRLWFP_LWFP(x)       (((x)&0x00000FFF)<<0)

/* Bit definitions and macros for MCF548X_FEC_FECRFAR */
#define MCF548X_FEC_FECRFAR_ALARM(x)       (((x)&0x00000FFF)<<0)

/* Bit definitions and macros for MCF548X_FEC_FECRFRP */
#define MCF548X_FEC_FECRFRP_READ(x)        (((x)&0x00000FFF)<<0)

/* Bit definitions and macros for MCF548X_FEC_FECRFWP */
#define MCF548X_FEC_FECRFWP_WRITE(x)       (((x)&0x00000FFF)<<0)

/* Bit definitions and macros for MCF548X_FEC_FECTFDR */
#define MCF548X_FEC_FECTFDR_TFCW_TC        (0x04000000)
#define MCF548X_FEC_FECTFDR_TFCW_ABC       (0x02000000)
#define MCF548X_FEC_FECTFDR_ADDR0            ((void*)(&__MBAR[0x0091A4]))
#define MCF548X_FEC_FECTFDR_ADDR1            ((void*)(&__MBAR[0x0099A4]))
#define MCF548X_FEC_FECTFDR_ADDR(x)          ((void*)(&__MBAR[0x0091A4U+(0x800*x)]))

/* Bit definitions and macros for MCF548X_FEC_FECTFSR */
#define MCF548X_FEC_FECTFSR_EMT            (0x00010000)
#define MCF548X_FEC_FECTFSR_ALARM          (0x00020000)
#define MCF548X_FEC_FECTFSR_FU             (0x00040000)
#define MCF548X_FEC_FECTFSR_FR             (0x00080000)
#define MCF548X_FEC_FECTFSR_OF             (0x00100000)
#define MCF548X_FEC_FECTFSR_UP             (0x00200000)
#define MCF548X_FEC_FECTFSR_FAE            (0x00800000)
#define MCF548X_FEC_FECTFSR_FRM(x)         (((x)&0x0000000F)<<24)
#define MCF548X_FEC_FECTFSR_TXW            (0x40000000)
#define MCF548X_FEC_FECTFSR_IP             (0x80000000)

/* Bit definitions and macros for MCF548X_FEC_FECTFCR */
#define MCF548X_FEC_FECTFCR_RESERVED       (0x00200000)
#define MCF548X_FEC_FECTFCR_COUNTER(x)     (((x)&0x0000FFFF)<<0|0x00200000)
#define MCF548X_FEC_FECTFCR_TXW_MSK        (0x00240000)
#define MCF548X_FEC_FECTFCR_OF_MSK         (0x00280000)
#define MCF548X_FEC_FECTFCR_UF_MSK         (0x00300000)
#define MCF548X_FEC_FECTFCR_FAE_MSK        (0x00600000)
#define MCF548X_FEC_FECTFCR_IP_MSK         (0x00A00000)
#define MCF548X_FEC_FECTFCR_GR(x)          (((x)&0x00000007)<<24|0x00200000)
#define MCF548X_FEC_FECTFCR_FRM            (0x08200000)
#define MCF548X_FEC_FECTFCR_TIMER          (0x10200000)
#define MCF548X_FEC_FECTFCR_WFR            (0x20200000)
#define MCF548X_FEC_FECTFCR_WCTL           (0x40200000)

/* Bit definitions and macros for MCF548X_FEC_FECTLRFP */
#define MCF548X_FEC_FECTLRFP_LRFP(x)       (((x)&0x00000FFF)<<0)

/* Bit definitions and macros for MCF548X_FEC_FECTLWFP */
#define MCF548X_FEC_FECTLWFP_LWFP(x)       (((x)&0x00000FFF)<<0)

/* Bit definitions and macros for MCF548X_FEC_FECTFAR */
#define MCF548X_FEC_FECTFAR_ALARM(x)       (((x)&0x00000FFF)<<0)

/* Bit definitions and macros for MCF548X_FEC_FECTFRP */
#define MCF548X_FEC_FECTFRP_READ(x)        (((x)&0x00000FFF)<<0)

/* Bit definitions and macros for MCF548X_FEC_FECTFWP */
#define MCF548X_FEC_FECTFWP_WRITE(x)       (((x)&0x00000FFF)<<0)

/* Bit definitions and macros for MCF548X_FEC_FRST */
#define MCF548X_FEC_FRST_RST_CTL           (0x01000000)
#define MCF548X_FEC_FRST_SW_RST            (0x02000000)

/* Bit definitions and macros for MCF548X_FEC_CTCWR */
#define MCF548X_FEC_CTCWR_TFCW             (0x01000000)
#define MCF548X_FEC_CTCWR_CRC              (0x02000000)


/*********************************************************************
*
* System Integration Unit (SIU)
*
*********************************************************************/

/* Register read/write macros */
#define MCF548X_SIU_SBCR       (*(volatile uint32_t*)(void*)(&__MBAR[0x000010]))
#define MCF548X_SIU_SECSACR    (*(volatile uint32_t*)(void*)(&__MBAR[0x000038]))
#define MCF548X_SIU_RSR        (*(volatile uint32_t*)(void*)(&__MBAR[0x000044]))
#define MCF548X_SIU_JTAGID     (*(volatile uint32_t*)(void*)(&__MBAR[0x000050]))

/* Bit definitions and macros for MCF548X_SIU_SBCR */
#define MCF548X_SIU_SBCR_PIN2DSPI       (0x08000000)
#define MCF548X_SIU_SBCR_DMA2CPU        (0x10000000)
#define MCF548X_SIU_SBCR_CPU2DMA        (0x20000000)
#define MCF548X_SIU_SBCR_PIN2DMA        (0x40000000)
#define MCF548X_SIU_SBCR_PIN2CPU        (0x80000000)

/* Bit definitions and macros for MCF548X_SIU_SECSACR */
#define MCF548X_SIU_SECSACR_SEQEN       (0x00000001)

/* Bit definitions and macros for MCF548X_SIU_RSR */
#define MCF548X_SIU_RSR_RST             (0x00000001)
#define MCF548X_SIU_RSR_RSTWD           (0x00000002)
#define MCF548X_SIU_RSR_RSTJTG          (0x00000008)

/* Bit definitions and macros for MCF548X_SIU_JTAGID */
#define MCF548X_SIU_JTAGID_REV          (0xF0000000)
#define MCF548X_SIU_JTAGID_PROCESSOR    (0x0FFFFFFF)
#define MCF548X_SIU_JTAGID_MCF5485      (0x0800C01D)
#define MCF548X_SIU_JTAGID_MCF5484      (0x0800D01D)
#define MCF548X_SIU_JTAGID_MCF5483      (0x0800E01D)
#define MCF548X_SIU_JTAGID_MCF5482      (0x0800F01D)
#define MCF548X_SIU_JTAGID_MCF5481      (0x0801001D)
#define MCF548X_SIU_JTAGID_MCF5480      (0x0801101D)
#define MCF548X_SIU_JTAGID_MCF5475      (0x0801201D)
#define MCF548X_SIU_JTAGID_MCF5474      (0x0801301D)
#define MCF548X_SIU_JTAGID_MCF5473      (0x0801401D)
#define MCF548X_SIU_JTAGID_MCF5472      (0x0801501D)
#define MCF548X_SIU_JTAGID_MCF5471      (0x0801601D)
#define MCF548X_SIU_JTAGID_MCF5470      (0x0801701D)

/*********************************************************************
*
* Comm Timer Module (CTM)
*
*********************************************************************/

/* Register read/write macros */
#define MCF548X_CTM_CTCRF0       (*(volatile uint32_t*)(void*)(&__MBAR[0x007F00]))
#define MCF548X_CTM_CTCRF1       (*(volatile uint32_t*)(void*)(&__MBAR[0x007F04]))
#define MCF548X_CTM_CTCRF2       (*(volatile uint32_t*)(void*)(&__MBAR[0x007F08]))
#define MCF548X_CTM_CTCRF3       (*(volatile uint32_t*)(void*)(&__MBAR[0x007F0C]))
#define MCF548X_CTM_CTCRFn(x)    (*(volatile uint32_t*)(void*)(&__MBAR[0x007F00U+((x)*0x004)]))
#define MCF548X_CTM_CTCRV4       (*(volatile uint32_t*)(void*)(&__MBAR[0x007F10]))
#define MCF548X_CTM_CTCRV5       (*(volatile uint32_t*)(void*)(&__MBAR[0x007F14]))
#define MCF548X_CTM_CTCRV6       (*(volatile uint32_t*)(void*)(&__MBAR[0x007F18]))
#define MCF548X_CTM_CTCRV7       (*(volatile uint32_t*)(void*)(&__MBAR[0x007F1C]))
#define MCF548X_CTM_CTCRVn(x)    (*(volatile uint32_t*)(void*)(&__MBAR[0x007F10U+((x)*0x004)]))

/* Bit definitions and macros for MCF548X_CTM_CTCRFn */
#define MCF548X_CTM_CTCRFn_CRV(x)       (((x)&0x0000FFFF)<<0)
#define MCF548X_CTM_CTCRFn_S(x)         (((x)&0x0000000F)<<16)
#define MCF548X_CTM_CTCRFn_PCT(x)       (((x)&0x00000007)<<20)
#define MCF548X_CTM_CTCRFn_M            (0x00800000)
#define MCF548X_CTM_CTCRFn_IM           (0x01000000)
#define MCF548X_CTM_CTCRFn_I            (0x80000000)
#define MCF548X_CTM_CTCRFn_PCT_100      (0x00000000)
#define MCF548X_CTM_CTCRFn_PCT_50       (0x00100000)
#define MCF548X_CTM_CTCRFn_PCT_25       (0x00200000)
#define MCF548X_CTM_CTCRFn_PCT_12p5     (0x00300000)
#define MCF548X_CTM_CTCRFn_PCT_6p25     (0x00400000)
#define MCF548X_CTM_CTCRFn_PCT_OFF      (0x00500000)
#define MCF548X_CTM_CTCRFn_S_CLK_1      (0x00000000)
#define MCF548X_CTM_CTCRFn_S_CLK_2      (0x00010000)
#define MCF548X_CTM_CTCRFn_S_CLK_4      (0x00020000)
#define MCF548X_CTM_CTCRFn_S_CLK_8      (0x00030000)
#define MCF548X_CTM_CTCRFn_S_CLK_16     (0x00040000)
#define MCF548X_CTM_CTCRFn_S_CLK_32     (0x00050000)
#define MCF548X_CTM_CTCRFn_S_CLK_64     (0x00060000)
#define MCF548X_CTM_CTCRFn_S_CLK_128    (0x00070000)
#define MCF548X_CTM_CTCRFn_S_CLK_256    (0x00080000)

/* Bit definitions and macros for MCF548X_CTM_CTCRVn */
#define MCF548X_CTM_CTCRVn_CRV(x)       (((x)&0x00FFFFFF)<<0)
#define MCF548X_CTM_CTCRVn_PCT(x)       (((x)&0x00000007)<<24)
#define MCF548X_CTM_CTCRVn_M            (0x08000000)
#define MCF548X_CTM_CTCRVn_S(x)         (((x)&0x0000000F)<<28)
#define MCF548X_CTM_CTCRVn_S_CLK_1      (0x00000000)
#define MCF548X_CTM_CTCRVn_S_CLK_2      (0x10000000)
#define MCF548X_CTM_CTCRVn_S_CLK_4      (0x20000000)
#define MCF548X_CTM_CTCRVn_S_CLK_8      (0x30000000)
#define MCF548X_CTM_CTCRVn_S_CLK_16     (0x40000000)
#define MCF548X_CTM_CTCRVn_S_CLK_32     (0x50000000)
#define MCF548X_CTM_CTCRVn_S_CLK_64     (0x60000000)
#define MCF548X_CTM_CTCRVn_S_CLK_128    (0x70000000)
#define MCF548X_CTM_CTCRVn_S_CLK_256    (0x80000000)
#define MCF548X_CTM_CTCRVn_PCT_100      (0x00000000)
#define MCF548X_CTM_CTCRVn_PCT_50       (0x01000000)
#define MCF548X_CTM_CTCRVn_PCT_25       (0x02000000)
#define MCF548X_CTM_CTCRVn_PCT_12p5     (0x03000000)
#define MCF548X_CTM_CTCRVn_PCT_6p25     (0x04000000)
#define MCF548X_CTM_CTCRVn_PCT_OFF      (0x05000000)

/*********************************************************************
*
* DMA Serial Peripheral Interface (DSPI)
*
*********************************************************************/

/* Register read/write macros */
#define MCF548X_DSPI_DMCR         (*(volatile uint32_t*)(void*)(&__MBAR[0x008A00]))
#define MCF548X_DSPI_DTCR         (*(volatile uint32_t*)(void*)(&__MBAR[0x008A08]))
#define MCF548X_DSPI_DCTAR0       (*(volatile uint32_t*)(void*)(&__MBAR[0x008A0C]))
#define MCF548X_DSPI_DCTAR1       (*(volatile uint32_t*)(void*)(&__MBAR[0x008A10]))
#define MCF548X_DSPI_DCTAR2       (*(volatile uint32_t*)(void*)(&__MBAR[0x008A14]))
#define MCF548X_DSPI_DCTAR3       (*(volatile uint32_t*)(void*)(&__MBAR[0x008A18]))
#define MCF548X_DSPI_DCTAR4       (*(volatile uint32_t*)(void*)(&__MBAR[0x008A1C]))
#define MCF548X_DSPI_DCTAR5       (*(volatile uint32_t*)(void*)(&__MBAR[0x008A20]))
#define MCF548X_DSPI_DCTAR6       (*(volatile uint32_t*)(void*)(&__MBAR[0x008A24]))
#define MCF548X_DSPI_DCTAR7       (*(volatile uint32_t*)(void*)(&__MBAR[0x008A28]))
#define MCF548X_DSPI_DCTARn(x)    (*(volatile uint32_t*)(void*)(&__MBAR[0x008A0CU+((x)*0x004)]))
#define MCF548X_DSPI_DSR          (*(volatile uint32_t*)(void*)(&__MBAR[0x008A2C]))
#define MCF548X_DSPI_DIRSR        (*(volatile uint32_t*)(void*)(&__MBAR[0x008A30]))
#define MCF548X_DSPI_DTFR         (*(volatile uint32_t*)(void*)(&__MBAR[0x008A34]))
#define MCF548X_DSPI_DRFR         (*(volatile uint32_t*)(void*)(&__MBAR[0x008A38]))
#define MCF548X_DSPI_DTFDR0       (*(volatile uint32_t*)(void*)(&__MBAR[0x008A3C]))
#define MCF548X_DSPI_DTFDR1       (*(volatile uint32_t*)(void*)(&__MBAR[0x008A40]))
#define MCF548X_DSPI_DTFDR2       (*(volatile uint32_t*)(void*)(&__MBAR[0x008A44]))
#define MCF548X_DSPI_DTFDR3       (*(volatile uint32_t*)(void*)(&__MBAR[0x008A48]))
#define MCF548X_DSPI_DTFDRn(x)    (*(volatile uint32_t*)(void*)(&__MBAR[0x008A3CU+((x)*0x004)]))
#define MCF548X_DSPI_DRFDR0       (*(volatile uint32_t*)(void*)(&__MBAR[0x008A7C]))
#define MCF548X_DSPI_DRFDR1       (*(volatile uint32_t*)(void*)(&__MBAR[0x008A80]))
#define MCF548X_DSPI_DRFDR2       (*(volatile uint32_t*)(void*)(&__MBAR[0x008A84]))
#define MCF548X_DSPI_DRFDR3       (*(volatile uint32_t*)(void*)(&__MBAR[0x008A88]))
#define MCF548X_DSPI_DRFDRn(x)    (*(volatile uint32_t*)(void*)(&__MBAR[0x008A7CU+((x)*0x004)]))

/* Bit definitions and macros for MCF548X_DSPI_DMCR */
#define MCF548X_DSPI_DMCR_HALT             (0x00000001)
#define MCF548X_DSPI_DMCR_SMPL_PT(x)       (((x)&0x00000003)<<8)
#define MCF548X_DSPI_DMCR_CRXF             (0x00000400)
#define MCF548X_DSPI_DMCR_CTXF             (0x00000800)
#define MCF548X_DSPI_DMCR_DRXF             (0x00001000)
#define MCF548X_DSPI_DMCR_DTXF             (0x00002000)
#define MCF548X_DSPI_DMCR_CSIS0            (0x00010000)
#define MCF548X_DSPI_DMCR_CSIS2            (0x00040000)
#define MCF548X_DSPI_DMCR_CSIS3            (0x00080000)
#define MCF548X_DSPI_DMCR_CSIS5            (0x00200000)
#define MCF548X_DSPI_DMCR_ROOE             (0x01000000)
#define MCF548X_DSPI_DMCR_PCSSE            (0x02000000)
#define MCF548X_DSPI_DMCR_MTFE             (0x04000000)
#define MCF548X_DSPI_DMCR_FRZ              (0x08000000)
#define MCF548X_DSPI_DMCR_DCONF(x)         (((x)&0x00000003)<<28)
#define MCF548X_DSPI_DMCR_CSCK             (0x40000000)
#define MCF548X_DSPI_DMCR_MSTR             (0x80000000)

/* Bit definitions and macros for MCF548X_DSPI_DTCR */
#define MCF548X_DSPI_DTCR_SPI_TCNT(x)      (((x)&0x0000FFFF)<<16)

/* Bit definitions and macros for MCF548X_DSPI_DCTARn */
#define MCF548X_DSPI_DCTARn_BR(x)          (((x)&0x0000000F)<<0)
#define MCF548X_DSPI_DCTARn_DT(x)          (((x)&0x0000000F)<<4)
#define MCF548X_DSPI_DCTARn_ASC(x)         (((x)&0x0000000F)<<8)
#define MCF548X_DSPI_DCTARn_CSSCK(x)       (((x)&0x0000000F)<<12)
#define MCF548X_DSPI_DCTARn_PBR(x)         (((x)&0x00000003)<<16)
#define MCF548X_DSPI_DCTARn_PDT(x)         (((x)&0x00000003)<<18)
#define MCF548X_DSPI_DCTARn_PASC(x)        (((x)&0x00000003)<<20)
#define MCF548X_DSPI_DCTARn_PCSSCK(x)      (((x)&0x00000003)<<22)
#define MCF548X_DSPI_DCTARn_LSBFE          (0x01000000)
#define MCF548X_DSPI_DCTARn_CPHA           (0x02000000)
#define MCF548X_DSPI_DCTARn_CPOL           (0x04000000)
#define MCF548X_DSPI_DCTARn_TRSZ(x)        (((x)&0x0000000F)<<27)
#define MCF548X_DSPI_DCTARn_PCSSCK_1CLK    (0x00000000)
#define MCF548X_DSPI_DCTARn_PCSSCK_3CLK    (0x00400000)
#define MCF548X_DSPI_DCTARn_PCSSCK_5CLK    (0x00800000)
#define MCF548X_DSPI_DCTARn_PCSSCK_7CLK    (0x00A00000)
#define MCF548X_DSPI_DCTARn_PASC_1CLK      (0x00000000)
#define MCF548X_DSPI_DCTARn_PASC_3CLK      (0x00100000)
#define MCF548X_DSPI_DCTARn_PASC_5CLK      (0x00200000)
#define MCF548X_DSPI_DCTARn_PASC_7CLK      (0x00300000)
#define MCF548X_DSPI_DCTARn_PDT_1CLK       (0x00000000)
#define MCF548X_DSPI_DCTARn_PDT_3CLK       (0x00040000)
#define MCF548X_DSPI_DCTARn_PDT_5CLK       (0x00080000)
#define MCF548X_DSPI_DCTARn_PDT_7CLK       (0x000A0000)
#define MCF548X_DSPI_DCTARn_PBR_1CLK       (0x00000000)
#define MCF548X_DSPI_DCTARn_PBR_3CLK       (0x00010000)
#define MCF548X_DSPI_DCTARn_PBR_5CLK       (0x00020000)
#define MCF548X_DSPI_DCTARn_PBR_7CLK       (0x00030000)

/* Bit definitions and macros for MCF548X_DSPI_DSR */
#define MCF548X_DSPI_DSR_RXPTR(x)          (((x)&0x0000000F)<<0)
#define MCF548X_DSPI_DSR_RXCTR(x)          (((x)&0x0000000F)<<4)
#define MCF548X_DSPI_DSR_TXPTR(x)          (((x)&0x0000000F)<<8)
#define MCF548X_DSPI_DSR_TXCTR(x)          (((x)&0x0000000F)<<12)
#define MCF548X_DSPI_DSR_RFDF              (0x00020000)
#define MCF548X_DSPI_DSR_RFOF              (0x00080000)
#define MCF548X_DSPI_DSR_TFFF              (0x02000000)
#define MCF548X_DSPI_DSR_TFUF              (0x08000000)
#define MCF548X_DSPI_DSR_EOQF              (0x10000000)
#define MCF548X_DSPI_DSR_TXRXS             (0x40000000)
#define MCF548X_DSPI_DSR_TCF               (0x80000000)

/* Bit definitions and macros for MCF548X_DSPI_DIRSR */
#define MCF548X_DSPI_DIRSR_RFDFS           (0x00010000)
#define MCF548X_DSPI_DIRSR_RFDFE           (0x00020000)
#define MCF548X_DSPI_DIRSR_RFOFE           (0x00080000)
#define MCF548X_DSPI_DIRSR_TFFFS           (0x01000000)
#define MCF548X_DSPI_DIRSR_TFFFE           (0x02000000)
#define MCF548X_DSPI_DIRSR_TFUFE           (0x08000000)
#define MCF548X_DSPI_DIRSR_EOQFE           (0x10000000)
#define MCF548X_DSPI_DIRSR_TCFE            (0x80000000)

/* Bit definitions and macros for MCF548X_DSPI_DTFR */
#define MCF548X_DSPI_DTFR_TXDATA(x)        (((x)&0x0000FFFF)<<0)
#define MCF548X_DSPI_DTFR_CS0              (0x00010000)
#define MCF548X_DSPI_DTFR_CS2              (0x00040000)
#define MCF548X_DSPI_DTFR_CS3              (0x00080000)
#define MCF548X_DSPI_DTFR_CS5              (0x00200000)
#define MCF548X_DSPI_DTFR_CTCNT            (0x04000000)
#define MCF548X_DSPI_DTFR_EOQ              (0x08000000)
#define MCF548X_DSPI_DTFR_CTAS(x)          (((x)&0x00000007)<<28)
#define MCF548X_DSPI_DTFR_CONT             (0x80000000)

/* Bit definitions and macros for MCF548X_DSPI_DRFR */
#define MCF548X_DSPI_DRFR_RXDATA(x)        (((x)&0x0000FFFF)<<0)

/* Bit definitions and macros for MCF548X_DSPI_DTFDRn */
#define MCF548X_DSPI_DTFDRn_TXDATA(x)      (((x)&0x0000FFFF)<<0)
#define MCF548X_DSPI_DTFDRn_TXCMD(x)       (((x)&0x0000FFFF)<<16)

/* Bit definitions and macros for MCF548X_DSPI_DRFDRn */
#define MCF548X_DSPI_DRFDRn_RXDATA(x)      (((x)&0x0000FFFF)<<0)


/*********************************************************************
*
* Edge Port Module (EPORT)
*
*********************************************************************/

/* Register read/write macros */
#define MCF548X_EPORT_EPPAR    (*(volatile uint16_t*)(void*)(&__MBAR[0x000F00]))
#define MCF548X_EPORT_EPDDR    (*(volatile uint8_t *)(void*)(&__MBAR[0x000F04]))
#define MCF548X_EPORT_EPIER    (*(volatile uint8_t *)(void*)(&__MBAR[0x000F05]))
#define MCF548X_EPORT_EPDR     (*(volatile uint8_t *)(void*)(&__MBAR[0x000F08]))
#define MCF548X_EPORT_EPPDR    (*(volatile uint8_t *)(void*)(&__MBAR[0x000F09]))
#define MCF548X_EPORT_EPFR     (*(volatile uint8_t *)(void*)(&__MBAR[0x000F0C]))

/* Bit definitions and macros for MCF548X_EPORT_EPPAR */
#define MCF548X_EPORT_EPPAR_EPPA1(x)         (((x)&0x0003)<<2)
#define MCF548X_EPORT_EPPAR_EPPA2(x)         (((x)&0x0003)<<4)
#define MCF548X_EPORT_EPPAR_EPPA3(x)         (((x)&0x0003)<<6)
#define MCF548X_EPORT_EPPAR_EPPA4(x)         (((x)&0x0003)<<8)
#define MCF548X_EPORT_EPPAR_EPPA5(x)         (((x)&0x0003)<<10)
#define MCF548X_EPORT_EPPAR_EPPA6(x)         (((x)&0x0003)<<12)
#define MCF548X_EPORT_EPPAR_EPPA7(x)         (((x)&0x0003)<<14)
#define MCF548X_EPORT_EPPAR_EPPAx_LEVEL      (0)
#define MCF548X_EPORT_EPPAR_EPPAx_RISING     (1)
#define MCF548X_EPORT_EPPAR_EPPAx_FALLING    (2)
#define MCF548X_EPORT_EPPAR_EPPAx_BOTH       (3)

/* Bit definitions and macros for MCF548X_EPORT_EPDDR */
#define MCF548X_EPORT_EPDDR_EPDD1            (0x02)
#define MCF548X_EPORT_EPDDR_EPDD2            (0x04)
#define MCF548X_EPORT_EPDDR_EPDD3            (0x08)
#define MCF548X_EPORT_EPDDR_EPDD4            (0x10)
#define MCF548X_EPORT_EPDDR_EPDD5            (0x20)
#define MCF548X_EPORT_EPDDR_EPDD6            (0x40)
#define MCF548X_EPORT_EPDDR_EPDD7            (0x80)

/* Bit definitions and macros for MCF548X_EPORT_EPIER */
#define MCF548X_EPORT_EPIER_EPIE1            (0x02)
#define MCF548X_EPORT_EPIER_EPIE2            (0x04)
#define MCF548X_EPORT_EPIER_EPIE3            (0x08)
#define MCF548X_EPORT_EPIER_EPIE4            (0x10)
#define MCF548X_EPORT_EPIER_EPIE5            (0x20)
#define MCF548X_EPORT_EPIER_EPIE6            (0x40)
#define MCF548X_EPORT_EPIER_EPIE7            (0x80)

/* Bit definitions and macros for MCF548X_EPORT_EPDR */
#define MCF548X_EPORT_EPDR_EPD1              (0x02)
#define MCF548X_EPORT_EPDR_EPD2              (0x04)
#define MCF548X_EPORT_EPDR_EPD3              (0x08)
#define MCF548X_EPORT_EPDR_EPD4              (0x10)
#define MCF548X_EPORT_EPDR_EPD5              (0x20)
#define MCF548X_EPORT_EPDR_EPD6              (0x40)
#define MCF548X_EPORT_EPDR_EPD7              (0x80)

/* Bit definitions and macros for MCF548X_EPORT_EPPDR */
#define MCF548X_EPORT_EPPDR_EPPD1            (0x02)
#define MCF548X_EPORT_EPPDR_EPPD2            (0x04)
#define MCF548X_EPORT_EPPDR_EPPD3            (0x08)
#define MCF548X_EPORT_EPPDR_EPPD4            (0x10)
#define MCF548X_EPORT_EPPDR_EPPD5            (0x20)
#define MCF548X_EPORT_EPPDR_EPPD6            (0x40)
#define MCF548X_EPORT_EPPDR_EPPD7            (0x80)

/* Bit definitions and macros for MCF548X_EPORT_EPFR */
#define MCF548X_EPORT_EPFR_EPF1              (0x02)
#define MCF548X_EPORT_EPFR_EPF2              (0x04)
#define MCF548X_EPORT_EPFR_EPF3              (0x08)
#define MCF548X_EPORT_EPFR_EPF4              (0x10)
#define MCF548X_EPORT_EPFR_EPF5              (0x20)
#define MCF548X_EPORT_EPFR_EPF6              (0x40)
#define MCF548X_EPORT_EPFR_EPF7              (0x80)

/*********************************************************************
*
* FlexBus Chip Selects (FBCS)
*
*********************************************************************/

/* Register read/write macros */
#define MCF548X_FBCS_CSAR0      (*(volatile uint32_t*)(void*)(&__MBAR[0x000500]))
#define MCF548X_FBCS_CSMR0      (*(volatile uint32_t*)(void*)(&__MBAR[0x000504]))
#define MCF548X_FBCS_CSCR0      (*(volatile uint32_t*)(void*)(&__MBAR[0x000508]))
#define MCF548X_FBCS_CSAR1      (*(volatile uint32_t*)(void*)(&__MBAR[0x00050C]))
#define MCF548X_FBCS_CSMR1      (*(volatile uint32_t*)(void*)(&__MBAR[0x000510]))
#define MCF548X_FBCS_CSCR1      (*(volatile uint32_t*)(void*)(&__MBAR[0x000514]))
#define MCF548X_FBCS_CSAR2      (*(volatile uint32_t*)(void*)(&__MBAR[0x000518]))
#define MCF548X_FBCS_CSMR2      (*(volatile uint32_t*)(void*)(&__MBAR[0x00051C]))
#define MCF548X_FBCS_CSCR2      (*(volatile uint32_t*)(void*)(&__MBAR[0x000520]))
#define MCF548X_FBCS_CSAR3      (*(volatile uint32_t*)(void*)(&__MBAR[0x000524]))
#define MCF548X_FBCS_CSMR3      (*(volatile uint32_t*)(void*)(&__MBAR[0x000528]))
#define MCF548X_FBCS_CSCR3      (*(volatile uint32_t*)(void*)(&__MBAR[0x00052C]))
#define MCF548X_FBCS_CSAR4      (*(volatile uint32_t*)(void*)(&__MBAR[0x000530]))
#define MCF548X_FBCS_CSMR4      (*(volatile uint32_t*)(void*)(&__MBAR[0x000534]))
#define MCF548X_FBCS_CSCR4      (*(volatile uint32_t*)(void*)(&__MBAR[0x000538]))
#define MCF548X_FBCS_CSAR5      (*(volatile uint32_t*)(void*)(&__MBAR[0x00053C]))
#define MCF548X_FBCS_CSMR5      (*(volatile uint32_t*)(void*)(&__MBAR[0x000540]))
#define MCF548X_FBCS_CSCR5      (*(volatile uint32_t*)(void*)(&__MBAR[0x000544]))
#define MCF548X_FBCS_CSAR(x)    (*(volatile uint32_t*)(void*)(&__MBAR[0x000500U+((x)*0x00C)]))
#define MCF548X_FBCS_CSMR(x)    (*(volatile uint32_t*)(void*)(&__MBAR[0x000504U+((x)*0x00C)]))
#define MCF548X_FBCS_CSCR(x)    (*(volatile uint32_t*)(void*)(&__MBAR[0x000508U+((x)*0x00C)]))

/* Bit definitions and macros for MCF548X_FBCS_CSAR */
#define MCF548X_FBCS_CSAR_BA(x)        ((x)&0xFFFF0000)

/* Bit definitions and macros for MCF548X_FBCS_CSMR */
#define MCF548X_FBCS_CSMR_V            (0x00000001)
#define MCF548X_FBCS_CSMR_WP           (0x00000100)
#define MCF548X_FBCS_CSMR_BAM(x)       (((x)&0x0000FFFF)<<16)
#define MCF548X_FBCS_CSMR_BAM_4G       (0xFFFF0000)
#define MCF548X_FBCS_CSMR_BAM_2G       (0x7FFF0000)
#define MCF548X_FBCS_CSMR_BAM_1G       (0x3FFF0000)
#define MCF548X_FBCS_CSMR_BAM_1024M    (0x3FFF0000)
#define MCF548X_FBCS_CSMR_BAM_512M     (0x1FFF0000)
#define MCF548X_FBCS_CSMR_BAM_256M     (0x0FFF0000)
#define MCF548X_FBCS_CSMR_BAM_128M     (0x07FF0000)
#define MCF548X_FBCS_CSMR_BAM_64M      (0x03FF0000)
#define MCF548X_FBCS_CSMR_BAM_32M      (0x01FF0000)
#define MCF548X_FBCS_CSMR_BAM_16M      (0x00FF0000)
#define MCF548X_FBCS_CSMR_BAM_8M       (0x007F0000)
#define MCF548X_FBCS_CSMR_BAM_4M       (0x003F0000)
#define MCF548X_FBCS_CSMR_BAM_2M       (0x001F0000)
#define MCF548X_FBCS_CSMR_BAM_1M       (0x000F0000)
#define MCF548X_FBCS_CSMR_BAM_1024K    (0x000F0000)
#define MCF548X_FBCS_CSMR_BAM_512K     (0x00070000)
#define MCF548X_FBCS_CSMR_BAM_256K     (0x00030000)
#define MCF548X_FBCS_CSMR_BAM_128K     (0x00010000)
#define MCF548X_FBCS_CSMR_BAM_64K      (0x00000000)

/* Bit definitions and macros for MCF548X_FBCS_CSCR */
#define MCF548X_FBCS_CSCR_BSTW         (0x00000008)
#define MCF548X_FBCS_CSCR_BSTR         (0x00000010)
#define MCF548X_FBCS_CSCR_PS(x)        (((x)&0x00000003)<<6)
#define MCF548X_FBCS_CSCR_AA           (0x00000100)
#define MCF548X_FBCS_CSCR_WS(x)        (((x)&0x0000003F)<<10)
#define MCF548X_FBCS_CSCR_WRAH(x)      (((x)&0x00000003)<<16)
#define MCF548X_FBCS_CSCR_RDAH(x)      (((x)&0x00000003)<<18)
#define MCF548X_FBCS_CSCR_ASET(x)      (((x)&0x00000003)<<20)
#define MCF548X_FBCS_CSCR_SWSEN        (0x00800000)
#define MCF548X_FBCS_CSCR_SWS(x)       (((x)&0x0000003F)<<26)
#define MCF548X_FBCS_CSCR_PS_8         (0x00000040)
#define MCF548X_FBCS_CSCR_PS_16        (0x00000080)
#define MCF548X_FBCS_CSCR_PS_32        (0x00000000)


/*********************************************************************
*
* General Purpose I/O (GPIO)
*
*********************************************************************/

/* Register read/write macros */
#define MCF548X_GPIO_PODR_FBCTL         (*(volatile uint8_t *)(void*)(&__MBAR[0x000A00]))
#define MCF548X_GPIO_PODR_FBCS          (*(volatile uint8_t *)(void*)(&__MBAR[0x000A01]))
#define MCF548X_GPIO_PODR_DMA           (*(volatile uint8_t *)(void*)(&__MBAR[0x000A02]))
#define MCF548X_GPIO_PODR_FEC0H         (*(volatile uint8_t *)(void*)(&__MBAR[0x000A04]))
#define MCF548X_GPIO_PODR_FEC0L         (*(volatile uint8_t *)(void*)(&__MBAR[0x000A05]))
#define MCF548X_GPIO_PODR_FEC1H         (*(volatile uint8_t *)(void*)(&__MBAR[0x000A06]))
#define MCF548X_GPIO_PODR_FEC1L         (*(volatile uint8_t *)(void*)(&__MBAR[0x000A07]))
#define MCF548X_GPIO_PODR_FECI2C        (*(volatile uint8_t *)(void*)(&__MBAR[0x000A08]))
#define MCF548X_GPIO_PODR_PCIBG         (*(volatile uint8_t *)(void*)(&__MBAR[0x000A09]))
#define MCF548X_GPIO_PODR_PCIBR         (*(volatile uint8_t *)(void*)(&__MBAR[0x000A0A]))
#define MCF548X_GPIO_PODR_PSC3PSC2      (*(volatile uint8_t *)(void*)(&__MBAR[0x000A0C]))
#define MCF548X_GPIO_PODR_PSC1PSC0      (*(volatile uint8_t *)(void*)(&__MBAR[0x000A0D]))
#define MCF548X_GPIO_PODR_DSPI          (*(volatile uint8_t *)(void*)(&__MBAR[0x000A0E]))
#define MCF548X_GPIO_PDDR_FBCTL         (*(volatile uint8_t *)(void*)(&__MBAR[0x000A10]))
#define MCF548X_GPIO_PDDR_FBCS          (*(volatile uint8_t *)(void*)(&__MBAR[0x000A11]))
#define MCF548X_GPIO_PDDR_DMA           (*(volatile uint8_t *)(void*)(&__MBAR[0x000A12]))
#define MCF548X_GPIO_PDDR_FEC0H         (*(volatile uint8_t *)(void*)(&__MBAR[0x000A14]))
#define MCF548X_GPIO_PDDR_FEC0L         (*(volatile uint8_t *)(void*)(&__MBAR[0x000A15]))
#define MCF548X_GPIO_PDDR_FEC1H         (*(volatile uint8_t *)(void*)(&__MBAR[0x000A16]))
#define MCF548X_GPIO_PDDR_FEC1L         (*(volatile uint8_t *)(void*)(&__MBAR[0x000A17]))
#define MCF548X_GPIO_PDDR_FECI2C        (*(volatile uint8_t *)(void*)(&__MBAR[0x000A18]))
#define MCF548X_GPIO_PDDR_PCIBG         (*(volatile uint8_t *)(void*)(&__MBAR[0x000A19]))
#define MCF548X_GPIO_PDDR_PCIBR         (*(volatile uint8_t *)(void*)(&__MBAR[0x000A1A]))
#define MCF548X_GPIO_PDDR_PSC3PSC2      (*(volatile uint8_t *)(void*)(&__MBAR[0x000A1C]))
#define MCF548X_GPIO_PDDR_PSC1PSC0      (*(volatile uint8_t *)(void*)(&__MBAR[0x000A1D]))
#define MCF548X_GPIO_PDDR_DSPI          (*(volatile uint8_t *)(void*)(&__MBAR[0x000A1E]))
#define MCF548X_GPIO_PPDSDR_FBCTL       (*(volatile uint8_t *)(void*)(&__MBAR[0x000A20]))
#define MCF548X_GPIO_PPDSDR_FBCS        (*(volatile uint8_t *)(void*)(&__MBAR[0x000A21]))
#define MCF548X_GPIO_PPDSDR_DMA         (*(volatile uint8_t *)(void*)(&__MBAR[0x000A22]))
#define MCF548X_GPIO_PPDSDR_FEC0H       (*(volatile uint8_t *)(void*)(&__MBAR[0x000A24]))
#define MCF548X_GPIO_PPDSDR_FEC0L       (*(volatile uint8_t *)(void*)(&__MBAR[0x000A25]))
#define MCF548X_GPIO_PPDSDR_FEC1H       (*(volatile uint8_t *)(void*)(&__MBAR[0x000A26]))
#define MCF548X_GPIO_PPDSDR_FEC1L       (*(volatile uint8_t *)(void*)(&__MBAR[0x000A27]))
#define MCF548X_GPIO_PPDSDR_FECI2C      (*(volatile uint8_t *)(void*)(&__MBAR[0x000A28]))
#define MCF548X_GPIO_PPDSDR_PCIBG       (*(volatile uint8_t *)(void*)(&__MBAR[0x000A29]))
#define MCF548X_GPIO_PPDSDR_PCIBR       (*(volatile uint8_t *)(void*)(&__MBAR[0x000A2A]))
#define MCF548X_GPIO_PPDSDR_PSC3PSC2    (*(volatile uint8_t *)(void*)(&__MBAR[0x000A2C]))
#define MCF548X_GPIO_PPDSDR_PSC1PSC0    (*(volatile uint8_t *)(void*)(&__MBAR[0x000A2D]))
#define MCF548X_GPIO_PPDSDR_DSPI        (*(volatile uint8_t *)(void*)(&__MBAR[0x000A2E]))
#define MCF548X_GPIO_PCLRR_FBCTL        (*(volatile uint8_t *)(void*)(&__MBAR[0x000A30]))
#define MCF548X_GPIO_PCLRR_FBCS         (*(volatile uint8_t *)(void*)(&__MBAR[0x000A31]))
#define MCF548X_GPIO_PCLRR_DMA          (*(volatile uint8_t *)(void*)(&__MBAR[0x000A32]))
#define MCF548X_GPIO_PCLRR_FEC0H        (*(volatile uint8_t *)(void*)(&__MBAR[0x000A34]))
#define MCF548X_GPIO_PCLRR_FEC0L        (*(volatile uint8_t *)(void*)(&__MBAR[0x000A35]))
#define MCF548X_GPIO_PCLRR_FEC1H        (*(volatile uint8_t *)(void*)(&__MBAR[0x000A36]))
#define MCF548X_GPIO_PCLRR_FEC1L        (*(volatile uint8_t *)(void*)(&__MBAR[0x000A37]))
#define MCF548X_GPIO_PCLRR_FECI2C       (*(volatile uint8_t *)(void*)(&__MBAR[0x000A38]))
#define MCF548X_GPIO_PCLRR_PCIBG        (*(volatile uint8_t *)(void*)(&__MBAR[0x000A39]))
#define MCF548X_GPIO_PCLRR_PCIBR        (*(volatile uint8_t *)(void*)(&__MBAR[0x000A3A]))
#define MCF548X_GPIO_PCLRR_PSC3PSC2     (*(volatile uint8_t *)(void*)(&__MBAR[0x000A3C]))
#define MCF548X_GPIO_PCLRR_PSC1PSC0     (*(volatile uint8_t *)(void*)(&__MBAR[0x000A3D]))
#define MCF548X_GPIO_PCLRR_DSPI         (*(volatile uint8_t *)(void*)(&__MBAR[0x000A3E]))
#define MCF548X_GPIO_PAR_FBCTL          (*(volatile uint16_t*)(void*)(&__MBAR[0x000A40]))
#define MCF548X_GPIO_PAR_FBCS           (*(volatile uint8_t *)(void*)(&__MBAR[0x000A42]))
#define MCF548X_GPIO_PAR_DMA            (*(volatile uint8_t *)(void*)(&__MBAR[0x000A43]))
#define MCF548X_GPIO_PAR_FECI2CIRQ      (*(volatile uint16_t*)(void*)(&__MBAR[0x000A44]))
#define MCF548X_GPIO_PAR_PCIBG          (*(volatile uint16_t*)(void*)(&__MBAR[0x000A48]))
#define MCF548X_GPIO_PAR_PCIBR          (*(volatile uint16_t*)(void*)(&__MBAR[0x000A4A]))
#define MCF548X_GPIO_PAR_PSC3           (*(volatile uint8_t *)(void*)(&__MBAR[0x000A4C]))
#define MCF548X_GPIO_PAR_PSC2           (*(volatile uint8_t *)(void*)(&__MBAR[0x000A4D]))
#define MCF548X_GPIO_PAR_PSC1           (*(volatile uint8_t *)(void*)(&__MBAR[0x000A4E]))
#define MCF548X_GPIO_PAR_PSC0           (*(volatile uint8_t *)(void*)(&__MBAR[0x000A4F]))
#define MCF548X_GPIO_PAR_DSPI           (*(volatile uint16_t*)(void*)(&__MBAR[0x000A50]))
#define MCF548X_GPIO_PAR_TIMER          (*(volatile uint8_t *)(void*)(&__MBAR[0x000A52]))

/* Bit definitions and macros for MCF548X_GPIO_PODR_FBCTL */
#define MCF548X_GPIO_PODR_FBCTL_PODR_FBCTL0              (0x01)
#define MCF548X_GPIO_PODR_FBCTL_PODR_FBCTL1              (0x02)
#define MCF548X_GPIO_PODR_FBCTL_PODR_FBCTL2              (0x04)
#define MCF548X_GPIO_PODR_FBCTL_PODR_FBCTL3              (0x08)
#define MCF548X_GPIO_PODR_FBCTL_PODR_FBCTL4              (0x10)
#define MCF548X_GPIO_PODR_FBCTL_PODR_FBCTL5              (0x20)
#define MCF548X_GPIO_PODR_FBCTL_PODR_FBCTL6              (0x40)
#define MCF548X_GPIO_PODR_FBCTL_PODR_FBCTL7              (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PODR_FBCS */
#define MCF548X_GPIO_PODR_FBCS_PODR_FBCS1                (0x02)
#define MCF548X_GPIO_PODR_FBCS_PODR_FBCS2                (0x04)
#define MCF548X_GPIO_PODR_FBCS_PODR_FBCS3                (0x08)
#define MCF548X_GPIO_PODR_FBCS_PODR_FBCS4                (0x10)
#define MCF548X_GPIO_PODR_FBCS_PODR_FBCS5                (0x20)

/* Bit definitions and macros for MCF548X_GPIO_PODR_DMA */
#define MCF548X_GPIO_PODR_DMA_PODR_DMA0                  (0x01)
#define MCF548X_GPIO_PODR_DMA_PODR_DMA1                  (0x02)
#define MCF548X_GPIO_PODR_DMA_PODR_DMA2                  (0x04)
#define MCF548X_GPIO_PODR_DMA_PODR_DMA3                  (0x08)

/* Bit definitions and macros for MCF548X_GPIO_PODR_FEC0H */
#define MCF548X_GPIO_PODR_FEC0H_PODR_FEC0H0              (0x01)
#define MCF548X_GPIO_PODR_FEC0H_PODR_FEC0H1              (0x02)
#define MCF548X_GPIO_PODR_FEC0H_PODR_FEC0H2              (0x04)
#define MCF548X_GPIO_PODR_FEC0H_PODR_FEC0H3              (0x08)
#define MCF548X_GPIO_PODR_FEC0H_PODR_FEC0H4              (0x10)
#define MCF548X_GPIO_PODR_FEC0H_PODR_FEC0H5              (0x20)
#define MCF548X_GPIO_PODR_FEC0H_PODR_FEC0H6              (0x40)
#define MCF548X_GPIO_PODR_FEC0H_PODR_FEC0H7              (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PODR_FEC0L */
#define MCF548X_GPIO_PODR_FEC0L_PODR_FEC0L0              (0x01)
#define MCF548X_GPIO_PODR_FEC0L_PODR_FEC0L1              (0x02)
#define MCF548X_GPIO_PODR_FEC0L_PODR_FEC0L2              (0x04)
#define MCF548X_GPIO_PODR_FEC0L_PODR_FEC0L3              (0x08)
#define MCF548X_GPIO_PODR_FEC0L_PODR_FEC0L4              (0x10)
#define MCF548X_GPIO_PODR_FEC0L_PODR_FEC0L5              (0x20)
#define MCF548X_GPIO_PODR_FEC0L_PODR_FEC0L6              (0x40)
#define MCF548X_GPIO_PODR_FEC0L_PODR_FEC0L7              (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PODR_FEC1H */
#define MCF548X_GPIO_PODR_FEC1H_PODR_FEC1H0              (0x01)
#define MCF548X_GPIO_PODR_FEC1H_PODR_FEC1H1              (0x02)
#define MCF548X_GPIO_PODR_FEC1H_PODR_FEC1H2              (0x04)
#define MCF548X_GPIO_PODR_FEC1H_PODR_FEC1H3              (0x08)
#define MCF548X_GPIO_PODR_FEC1H_PODR_FEC1H4              (0x10)
#define MCF548X_GPIO_PODR_FEC1H_PODR_FEC1H5              (0x20)
#define MCF548X_GPIO_PODR_FEC1H_PODR_FEC1H6              (0x40)
#define MCF548X_GPIO_PODR_FEC1H_PODR_FEC1H7              (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PODR_FEC1L */
#define MCF548X_GPIO_PODR_FEC1L_PODR_FEC1L0              (0x01)
#define MCF548X_GPIO_PODR_FEC1L_PODR_FEC1L1              (0x02)
#define MCF548X_GPIO_PODR_FEC1L_PODR_FEC1L2              (0x04)
#define MCF548X_GPIO_PODR_FEC1L_PODR_FEC1L3              (0x08)
#define MCF548X_GPIO_PODR_FEC1L_PODR_FEC1L4              (0x10)
#define MCF548X_GPIO_PODR_FEC1L_PODR_FEC1L5              (0x20)
#define MCF548X_GPIO_PODR_FEC1L_PODR_FEC1L6              (0x40)
#define MCF548X_GPIO_PODR_FEC1L_PODR_FEC1L7              (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PODR_FECI2C */
#define MCF548X_GPIO_PODR_FECI2C_PODR_FECI2C0            (0x01)
#define MCF548X_GPIO_PODR_FECI2C_PODR_FECI2C1            (0x02)
#define MCF548X_GPIO_PODR_FECI2C_PODR_FECI2C2            (0x04)
#define MCF548X_GPIO_PODR_FECI2C_PODR_FECI2C3            (0x08)

/* Bit definitions and macros for MCF548X_GPIO_PODR_PCIBG */
#define MCF548X_GPIO_PODR_PCIBG_PODR_PCIBG0              (0x01)
#define MCF548X_GPIO_PODR_PCIBG_PODR_PCIBG1              (0x02)
#define MCF548X_GPIO_PODR_PCIBG_PODR_PCIBG2              (0x04)
#define MCF548X_GPIO_PODR_PCIBG_PODR_PCIBG3              (0x08)
#define MCF548X_GPIO_PODR_PCIBG_PODR_PCIBG4              (0x10)

/* Bit definitions and macros for MCF548X_GPIO_PODR_PCIBR */
#define MCF548X_GPIO_PODR_PCIBR_PODR_PCIBR0              (0x01)
#define MCF548X_GPIO_PODR_PCIBR_PODR_PCIBR1              (0x02)
#define MCF548X_GPIO_PODR_PCIBR_PODR_PCIBR2              (0x04)
#define MCF548X_GPIO_PODR_PCIBR_PODR_PCIBR3              (0x08)
#define MCF548X_GPIO_PODR_PCIBR_PODR_PCIBR4              (0x10)

/* Bit definitions and macros for MCF548X_GPIO_PODR_PSC3PSC2 */
#define MCF548X_GPIO_PODR_PSC3PSC2_PODR_PSC3PSC20        (0x01)
#define MCF548X_GPIO_PODR_PSC3PSC2_PODR_PSC3PSC21        (0x02)
#define MCF548X_GPIO_PODR_PSC3PSC2_PODR_PSC3PSC22        (0x04)
#define MCF548X_GPIO_PODR_PSC3PSC2_PODR_PSC3PSC23        (0x08)
#define MCF548X_GPIO_PODR_PSC3PSC2_PODR_PSC3PSC24        (0x10)
#define MCF548X_GPIO_PODR_PSC3PSC2_PODR_PSC3PSC25        (0x20)
#define MCF548X_GPIO_PODR_PSC3PSC2_PODR_PSC3PSC26        (0x40)
#define MCF548X_GPIO_PODR_PSC3PSC2_PODR_PSC3PSC27        (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PODR_PSC1PSC0 */
#define MCF548X_GPIO_PODR_PSC1PSC0_PODR_PSC1PSC00        (0x01)
#define MCF548X_GPIO_PODR_PSC1PSC0_PODR_PSC1PSC01        (0x02)
#define MCF548X_GPIO_PODR_PSC1PSC0_PODR_PSC1PSC02        (0x04)
#define MCF548X_GPIO_PODR_PSC1PSC0_PODR_PSC1PSC03        (0x08)
#define MCF548X_GPIO_PODR_PSC1PSC0_PODR_PSC1PSC04        (0x10)
#define MCF548X_GPIO_PODR_PSC1PSC0_PODR_PSC1PSC05        (0x20)
#define MCF548X_GPIO_PODR_PSC1PSC0_PODR_PSC1PSC06        (0x40)
#define MCF548X_GPIO_PODR_PSC1PSC0_PODR_PSC1PSC07        (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PODR_DSPI */
#define MCF548X_GPIO_PODR_DSPI_PODR_DSPI0                (0x01)
#define MCF548X_GPIO_PODR_DSPI_PODR_DSPI1                (0x02)
#define MCF548X_GPIO_PODR_DSPI_PODR_DSPI2                (0x04)
#define MCF548X_GPIO_PODR_DSPI_PODR_DSPI3                (0x08)
#define MCF548X_GPIO_PODR_DSPI_PODR_DSPI4                (0x10)
#define MCF548X_GPIO_PODR_DSPI_PODR_DSPI5                (0x20)
#define MCF548X_GPIO_PODR_DSPI_PODR_DSPI6                (0x40)

/* Bit definitions and macros for MCF548X_GPIO_PDDR_FBCTL */
#define MCF548X_GPIO_PDDR_FBCTL_PDDR_FBCTL0              (0x01)
#define MCF548X_GPIO_PDDR_FBCTL_PDDR_FBCTL1              (0x02)
#define MCF548X_GPIO_PDDR_FBCTL_PDDR_FBCTL2              (0x04)
#define MCF548X_GPIO_PDDR_FBCTL_PDDR_FBCTL3              (0x08)
#define MCF548X_GPIO_PDDR_FBCTL_PDDR_FBCTL4              (0x10)
#define MCF548X_GPIO_PDDR_FBCTL_PDDR_FBCTL5              (0x20)
#define MCF548X_GPIO_PDDR_FBCTL_PDDR_FBCTL6              (0x40)
#define MCF548X_GPIO_PDDR_FBCTL_PDDR_FBCTL7              (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PDDR_FBCS */
#define MCF548X_GPIO_PDDR_FBCS_PDDR_FBCS1                (0x02)
#define MCF548X_GPIO_PDDR_FBCS_PDDR_FBCS2                (0x04)
#define MCF548X_GPIO_PDDR_FBCS_PDDR_FBCS3                (0x08)
#define MCF548X_GPIO_PDDR_FBCS_PDDR_FBCS4                (0x10)
#define MCF548X_GPIO_PDDR_FBCS_PDDR_FBCS5                (0x20)

/* Bit definitions and macros for MCF548X_GPIO_PDDR_DMA */
#define MCF548X_GPIO_PDDR_DMA_PDDR_DMA0                  (0x01)
#define MCF548X_GPIO_PDDR_DMA_PDDR_DMA1                  (0x02)
#define MCF548X_GPIO_PDDR_DMA_PDDR_DMA2                  (0x04)
#define MCF548X_GPIO_PDDR_DMA_PDDR_DMA3                  (0x08)

/* Bit definitions and macros for MCF548X_GPIO_PDDR_FEC0H */
#define MCF548X_GPIO_PDDR_FEC0H_PDDR_FEC0H0              (0x01)
#define MCF548X_GPIO_PDDR_FEC0H_PDDR_FEC0H1              (0x02)
#define MCF548X_GPIO_PDDR_FEC0H_PDDR_FEC0H2              (0x04)
#define MCF548X_GPIO_PDDR_FEC0H_PDDR_FEC0H3              (0x08)
#define MCF548X_GPIO_PDDR_FEC0H_PDDR_FEC0H4              (0x10)
#define MCF548X_GPIO_PDDR_FEC0H_PDDR_FEC0H5              (0x20)
#define MCF548X_GPIO_PDDR_FEC0H_PDDR_FEC0H6              (0x40)
#define MCF548X_GPIO_PDDR_FEC0H_PDDR_FEC0H7              (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PDDR_FEC0L */
#define MCF548X_GPIO_PDDR_FEC0L_PDDR_FEC0L0              (0x01)
#define MCF548X_GPIO_PDDR_FEC0L_PDDR_FEC0L1              (0x02)
#define MCF548X_GPIO_PDDR_FEC0L_PDDR_FEC0L2              (0x04)
#define MCF548X_GPIO_PDDR_FEC0L_PDDR_FEC0L3              (0x08)
#define MCF548X_GPIO_PDDR_FEC0L_PDDR_FEC0L4              (0x10)
#define MCF548X_GPIO_PDDR_FEC0L_PDDR_FEC0L5              (0x20)
#define MCF548X_GPIO_PDDR_FEC0L_PDDR_FEC0L6              (0x40)
#define MCF548X_GPIO_PDDR_FEC0L_PDDR_FEC0L7              (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PDDR_FEC1H */
#define MCF548X_GPIO_PDDR_FEC1H_PDDR_FEC1H0              (0x01)
#define MCF548X_GPIO_PDDR_FEC1H_PDDR_FEC1H1              (0x02)
#define MCF548X_GPIO_PDDR_FEC1H_PDDR_FEC1H2              (0x04)
#define MCF548X_GPIO_PDDR_FEC1H_PDDR_FEC1H3              (0x08)
#define MCF548X_GPIO_PDDR_FEC1H_PDDR_FEC1H4              (0x10)
#define MCF548X_GPIO_PDDR_FEC1H_PDDR_FEC1H5              (0x20)
#define MCF548X_GPIO_PDDR_FEC1H_PDDR_FEC1H6              (0x40)
#define MCF548X_GPIO_PDDR_FEC1H_PDDR_FEC1H7              (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PDDR_FEC1L */
#define MCF548X_GPIO_PDDR_FEC1L_PDDR_FEC1L0              (0x01)
#define MCF548X_GPIO_PDDR_FEC1L_PDDR_FEC1L1              (0x02)
#define MCF548X_GPIO_PDDR_FEC1L_PDDR_FEC1L2              (0x04)
#define MCF548X_GPIO_PDDR_FEC1L_PDDR_FEC1L3              (0x08)
#define MCF548X_GPIO_PDDR_FEC1L_PDDR_FEC1L4              (0x10)
#define MCF548X_GPIO_PDDR_FEC1L_PDDR_FEC1L5              (0x20)
#define MCF548X_GPIO_PDDR_FEC1L_PDDR_FEC1L6              (0x40)
#define MCF548X_GPIO_PDDR_FEC1L_PDDR_FEC1L7              (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PDDR_FECI2C */
#define MCF548X_GPIO_PDDR_FECI2C_PDDR_FECI2C0            (0x01)
#define MCF548X_GPIO_PDDR_FECI2C_PDDR_FECI2C1            (0x02)
#define MCF548X_GPIO_PDDR_FECI2C_PDDR_FECI2C2            (0x04)
#define MCF548X_GPIO_PDDR_FECI2C_PDDR_FECI2C3            (0x08)

/* Bit definitions and macros for MCF548X_GPIO_PDDR_PCIBG */
#define MCF548X_GPIO_PDDR_PCIBG_PDDR_PCIBG0              (0x01)
#define MCF548X_GPIO_PDDR_PCIBG_PDDR_PCIBG1              (0x02)
#define MCF548X_GPIO_PDDR_PCIBG_PDDR_PCIBG2              (0x04)
#define MCF548X_GPIO_PDDR_PCIBG_PDDR_PCIBG3              (0x08)
#define MCF548X_GPIO_PDDR_PCIBG_PDDR_PCIBG4              (0x10)

/* Bit definitions and macros for MCF548X_GPIO_PDDR_PCIBR */
#define MCF548X_GPIO_PDDR_PCIBR_PDDR_PCIBR0              (0x01)
#define MCF548X_GPIO_PDDR_PCIBR_PDDR_PCIBR1              (0x02)
#define MCF548X_GPIO_PDDR_PCIBR_PDDR_PCIBR2              (0x04)
#define MCF548X_GPIO_PDDR_PCIBR_PDDR_PCIBR3              (0x08)
#define MCF548X_GPIO_PDDR_PCIBR_PDDR_PCIBR4              (0x10)

/* Bit definitions and macros for MCF548X_GPIO_PDDR_PSC3PSC2 */
#define MCF548X_GPIO_PDDR_PSC3PSC2_PDDR_PSC3PSC20        (0x01)
#define MCF548X_GPIO_PDDR_PSC3PSC2_PDDR_PSC3PSC21        (0x02)
#define MCF548X_GPIO_PDDR_PSC3PSC2_PDDR_PSC3PSC22        (0x04)
#define MCF548X_GPIO_PDDR_PSC3PSC2_PDDR_PSC3PSC23        (0x08)
#define MCF548X_GPIO_PDDR_PSC3PSC2_PDDR_PSC3PSC24        (0x10)
#define MCF548X_GPIO_PDDR_PSC3PSC2_PDDR_PSC3PSC25        (0x20)
#define MCF548X_GPIO_PDDR_PSC3PSC2_PDDR_PSC3PSC26        (0x40)
#define MCF548X_GPIO_PDDR_PSC3PSC2_PDDR_PSC3PSC27        (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PDDR_PSC1PSC0 */
#define MCF548X_GPIO_PDDR_PSC1PSC0_PDDR_PSC1PSC00        (0x01)
#define MCF548X_GPIO_PDDR_PSC1PSC0_PDDR_PSC1PSC01        (0x02)
#define MCF548X_GPIO_PDDR_PSC1PSC0_PDDR_PSC1PSC02        (0x04)
#define MCF548X_GPIO_PDDR_PSC1PSC0_PDDR_PSC1PSC03        (0x08)
#define MCF548X_GPIO_PDDR_PSC1PSC0_PDDR_PSC1PSC04        (0x10)
#define MCF548X_GPIO_PDDR_PSC1PSC0_PDDR_PSC1PSC05        (0x20)
#define MCF548X_GPIO_PDDR_PSC1PSC0_PDDR_PSC1PSC06        (0x40)
#define MCF548X_GPIO_PDDR_PSC1PSC0_PDDR_PSC1PSC07        (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PDDR_DSPI */
#define MCF548X_GPIO_PDDR_DSPI_PDDR_DSPI0                (0x01)
#define MCF548X_GPIO_PDDR_DSPI_PDDR_DSPI1                (0x02)
#define MCF548X_GPIO_PDDR_DSPI_PDDR_DSPI2                (0x04)
#define MCF548X_GPIO_PDDR_DSPI_PDDR_DSPI3                (0x08)
#define MCF548X_GPIO_PDDR_DSPI_PDDR_DSPI4                (0x10)
#define MCF548X_GPIO_PDDR_DSPI_PDDR_DSPI5                (0x20)
#define MCF548X_GPIO_PDDR_DSPI_PDDR_DSPI6                (0x40)

/* Bit definitions and macros for MCF548X_GPIO_PPDSDR_FBCTL */
#define MCF548X_GPIO_PPDSDR_FBCTL_PPDSDR_FBCTL0          (0x01)
#define MCF548X_GPIO_PPDSDR_FBCTL_PPDSDR_FBCTL1          (0x02)
#define MCF548X_GPIO_PPDSDR_FBCTL_PPDSDR_FBCTL2          (0x04)
#define MCF548X_GPIO_PPDSDR_FBCTL_PPDSDR_FBCTL3          (0x08)
#define MCF548X_GPIO_PPDSDR_FBCTL_PPDSDR_FBCTL4          (0x10)
#define MCF548X_GPIO_PPDSDR_FBCTL_PPDSDR_FBCTL5          (0x20)
#define MCF548X_GPIO_PPDSDR_FBCTL_PPDSDR_FBCTL6          (0x40)
#define MCF548X_GPIO_PPDSDR_FBCTL_PPDSDR_FBCTL7          (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PPDSDR_FBCS */
#define MCF548X_GPIO_PPDSDR_FBCS_PPDSDR_FBCS1            (0x02)
#define MCF548X_GPIO_PPDSDR_FBCS_PPDSDR_FBCS2            (0x04)
#define MCF548X_GPIO_PPDSDR_FBCS_PPDSDR_FBCS3            (0x08)
#define MCF548X_GPIO_PPDSDR_FBCS_PPDSDR_FBCS4            (0x10)
#define MCF548X_GPIO_PPDSDR_FBCS_PPDSDR_FBCS5            (0x20)

/* Bit definitions and macros for MCF548X_GPIO_PPDSDR_DMA */
#define MCF548X_GPIO_PPDSDR_DMA_PPDSDR_DMA0              (0x01)
#define MCF548X_GPIO_PPDSDR_DMA_PPDSDR_DMA1              (0x02)
#define MCF548X_GPIO_PPDSDR_DMA_PPDSDR_DMA2              (0x04)
#define MCF548X_GPIO_PPDSDR_DMA_PPDSDR_DMA3              (0x08)

/* Bit definitions and macros for MCF548X_GPIO_PPDSDR_FEC0H */
#define MCF548X_GPIO_PPDSDR_FEC0H_PPDSDR_FEC0H0          (0x01)
#define MCF548X_GPIO_PPDSDR_FEC0H_PPDSDR_FEC0H1          (0x02)
#define MCF548X_GPIO_PPDSDR_FEC0H_PPDSDR_FEC0H2          (0x04)
#define MCF548X_GPIO_PPDSDR_FEC0H_PPDSDR_FEC0H3          (0x08)
#define MCF548X_GPIO_PPDSDR_FEC0H_PPDSDR_FEC0H4          (0x10)
#define MCF548X_GPIO_PPDSDR_FEC0H_PPDSDR_FEC0H5          (0x20)
#define MCF548X_GPIO_PPDSDR_FEC0H_PPDSDR_FEC0H6          (0x40)
#define MCF548X_GPIO_PPDSDR_FEC0H_PPDSDR_FEC0H7          (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PPDSDR_FEC0L */
#define MCF548X_GPIO_PPDSDR_FEC0L_PPDSDR_FEC0L0          (0x01)
#define MCF548X_GPIO_PPDSDR_FEC0L_PPDSDR_FEC0L1          (0x02)
#define MCF548X_GPIO_PPDSDR_FEC0L_PPDSDR_FEC0L2          (0x04)
#define MCF548X_GPIO_PPDSDR_FEC0L_PPDSDR_FEC0L3          (0x08)
#define MCF548X_GPIO_PPDSDR_FEC0L_PPDSDR_FEC0L4          (0x10)
#define MCF548X_GPIO_PPDSDR_FEC0L_PPDSDR_FEC0L5          (0x20)
#define MCF548X_GPIO_PPDSDR_FEC0L_PPDSDR_FEC0L6          (0x40)
#define MCF548X_GPIO_PPDSDR_FEC0L_PPDSDR_FEC0L7          (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PPDSDR_FEC1H */
#define MCF548X_GPIO_PPDSDR_FEC1H_PPDSDR_FEC1H0          (0x01)
#define MCF548X_GPIO_PPDSDR_FEC1H_PPDSDR_FEC1H1          (0x02)
#define MCF548X_GPIO_PPDSDR_FEC1H_PPDSDR_FEC1H2          (0x04)
#define MCF548X_GPIO_PPDSDR_FEC1H_PPDSDR_FEC1H3          (0x08)
#define MCF548X_GPIO_PPDSDR_FEC1H_PPDSDR_FEC1H4          (0x10)
#define MCF548X_GPIO_PPDSDR_FEC1H_PPDSDR_FEC1H5          (0x20)
#define MCF548X_GPIO_PPDSDR_FEC1H_PPDSDR_FEC1H6          (0x40)
#define MCF548X_GPIO_PPDSDR_FEC1H_PPDSDR_FEC1H7          (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PPDSDR_FEC1L */
#define MCF548X_GPIO_PPDSDR_FEC1L_PPDSDR_FEC1L0          (0x01)
#define MCF548X_GPIO_PPDSDR_FEC1L_PPDSDR_FEC1L1          (0x02)
#define MCF548X_GPIO_PPDSDR_FEC1L_PPDSDR_FEC1L2          (0x04)
#define MCF548X_GPIO_PPDSDR_FEC1L_PPDSDR_FEC1L3          (0x08)
#define MCF548X_GPIO_PPDSDR_FEC1L_PPDSDR_FEC1L4          (0x10)
#define MCF548X_GPIO_PPDSDR_FEC1L_PPDSDR_FEC1L5          (0x20)
#define MCF548X_GPIO_PPDSDR_FEC1L_PPDSDR_FEC1L6          (0x40)
#define MCF548X_GPIO_PPDSDR_FEC1L_PPDSDR_FEC1L7          (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PPDSDR_FECI2C */
#define MCF548X_GPIO_PPDSDR_FECI2C_PPDSDR_FECI2C0        (0x01)
#define MCF548X_GPIO_PPDSDR_FECI2C_PPDSDR_FECI2C1        (0x02)
#define MCF548X_GPIO_PPDSDR_FECI2C_PPDSDR_FECI2C2        (0x04)
#define MCF548X_GPIO_PPDSDR_FECI2C_PPDSDR_FECI2C3        (0x08)

/* Bit definitions and macros for MCF548X_GPIO_PPDSDR_PCIBG */
#define MCF548X_GPIO_PPDSDR_PCIBG_PPDSDR_PCIBG0          (0x01)
#define MCF548X_GPIO_PPDSDR_PCIBG_PPDSDR_PCIBG1          (0x02)
#define MCF548X_GPIO_PPDSDR_PCIBG_PPDSDR_PCIBG2          (0x04)
#define MCF548X_GPIO_PPDSDR_PCIBG_PPDSDR_PCIBG3          (0x08)
#define MCF548X_GPIO_PPDSDR_PCIBG_PPDSDR_PCIBG4          (0x10)

/* Bit definitions and macros for MCF548X_GPIO_PPDSDR_PCIBR */
#define MCF548X_GPIO_PPDSDR_PCIBR_PPDSDR_PCIBR0          (0x01)
#define MCF548X_GPIO_PPDSDR_PCIBR_PPDSDR_PCIBR1          (0x02)
#define MCF548X_GPIO_PPDSDR_PCIBR_PPDSDR_PCIBR2          (0x04)
#define MCF548X_GPIO_PPDSDR_PCIBR_PPDSDR_PCIBR3          (0x08)
#define MCF548X_GPIO_PPDSDR_PCIBR_PPDSDR_PCIBR4          (0x10)

/* Bit definitions and macros for MCF548X_GPIO_PPDSDR_PSC3PSC2 */
#define MCF548X_GPIO_PPDSDR_PSC3PSC2_PPDSDR_PSC3PSC20    (0x01)
#define MCF548X_GPIO_PPDSDR_PSC3PSC2_PPDSDR_PSC3PSC21    (0x02)
#define MCF548X_GPIO_PPDSDR_PSC3PSC2_PPDSDR_PSC3PSC22    (0x04)
#define MCF548X_GPIO_PPDSDR_PSC3PSC2_PPDSDR_PSC3PSC23    (0x08)
#define MCF548X_GPIO_PPDSDR_PSC3PSC2_PDDR_PSC3PSC24      (0x10)
#define MCF548X_GPIO_PPDSDR_PSC3PSC2_PDDR_PSC3PSC25      (0x20)
#define MCF548X_GPIO_PPDSDR_PSC3PSC2_PPDSDR_PSC3PSC26    (0x40)
#define MCF548X_GPIO_PPDSDR_PSC3PSC2_PPDSDR_PSC3PSC27    (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PPDSDR_PSC1PSC0 */
#define MCF548X_GPIO_PPDSDR_PSC1PSC0_PPDSDR_PSC1PSC00    (0x01)
#define MCF548X_GPIO_PPDSDR_PSC1PSC0_PDDR_PSC1PSC01      (0x02)
#define MCF548X_GPIO_PPDSDR_PSC1PSC0_PPDSDR_PSC1PSC02    (0x04)
#define MCF548X_GPIO_PPDSDR_PSC1PSC0_PDDR_PSC1PSC03      (0x08)
#define MCF548X_GPIO_PPDSDR_PSC1PSC0_PPDSDR_PSC1PSC04    (0x10)
#define MCF548X_GPIO_PPDSDR_PSC1PSC0_PPDSDR_PSC1PSC05    (0x20)
#define MCF548X_GPIO_PPDSDR_PSC1PSC0_PPDSDR_PSC1PSC06    (0x40)
#define MCF548X_GPIO_PPDSDR_PSC1PSC0_PPDSDR_PSC1PSC07    (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PPDSDR_DSPI */
#define MCF548X_GPIO_PPDSDR_DSPI_PPDSDR_DSPI0            (0x01)
#define MCF548X_GPIO_PPDSDR_DSPI_PPDSDR_DSPI1            (0x02)
#define MCF548X_GPIO_PPDSDR_DSPI_PPDSDR_DSPI2            (0x04)
#define MCF548X_GPIO_PPDSDR_DSPI_PPDSDR_DSPI3            (0x08)
#define MCF548X_GPIO_PPDSDR_DSPI_PDDR_DSPI4              (0x10)
#define MCF548X_GPIO_PPDSDR_DSPI_PPDSDR_DSPI5            (0x20)
#define MCF548X_GPIO_PPDSDR_DSPI_PPDSDR_DSPI6            (0x40)

/* Bit definitions and macros for MCF548X_GPIO_PCLRR_FBCTL */
#define MCF548X_GPIO_PCLRR_FBCTL_PCLRR_FBCTL0            (0x01)
#define MCF548X_GPIO_PCLRR_FBCTL_PCLRR_FBCTL1            (0x02)
#define MCF548X_GPIO_PCLRR_FBCTL_PCLRR_FBCTL2            (0x04)
#define MCF548X_GPIO_PCLRR_FBCTL_PCLRR_FBCTL3            (0x08)
#define MCF548X_GPIO_PCLRR_FBCTL_PCLRR_FBCTL4            (0x10)
#define MCF548X_GPIO_PCLRR_FBCTL_PCLRR_FBCTL5            (0x20)
#define MCF548X_GPIO_PCLRR_FBCTL_PCLRR_FBCTL6            (0x40)
#define MCF548X_GPIO_PCLRR_FBCTL_PCLRR_FBCTL7            (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PCLRR_FBCS */
#define MCF548X_GPIO_PCLRR_FBCS_PCLRR_FBCS1              (0x02)
#define MCF548X_GPIO_PCLRR_FBCS_PCLRR_FBCS2              (0x04)
#define MCF548X_GPIO_PCLRR_FBCS_PCLRR_FBCS3              (0x08)
#define MCF548X_GPIO_PCLRR_FBCS_PCLRR_FBCS4              (0x10)
#define MCF548X_GPIO_PCLRR_FBCS_PCLRR_FBCS5              (0x20)

/* Bit definitions and macros for MCF548X_GPIO_PCLRR_DMA */
#define MCF548X_GPIO_PCLRR_DMA_PCLRR_DMA0                (0x01)
#define MCF548X_GPIO_PCLRR_DMA_PCLRR_DMA1                (0x02)
#define MCF548X_GPIO_PCLRR_DMA_PCLRR_DMA2                (0x04)
#define MCF548X_GPIO_PCLRR_DMA_PCLRR_DMA3                (0x08)

/* Bit definitions and macros for MCF548X_GPIO_PCLRR_FEC0H */
#define MCF548X_GPIO_PCLRR_FEC0H_PCLRR_FEC0H0            (0x01)
#define MCF548X_GPIO_PCLRR_FEC0H_PCLRR_FEC0H1            (0x02)
#define MCF548X_GPIO_PCLRR_FEC0H_PCLRR_FEC0H2            (0x04)
#define MCF548X_GPIO_PCLRR_FEC0H_PCLRR_FEC0H3            (0x08)
#define MCF548X_GPIO_PCLRR_FEC0H_PCLRR_FEC0H4            (0x10)
#define MCF548X_GPIO_PCLRR_FEC0H_PCLRR_FEC0H5            (0x20)
#define MCF548X_GPIO_PCLRR_FEC0H_PCLRR_FEC0H6            (0x40)
#define MCF548X_GPIO_PCLRR_FEC0H_PCLRR_FEC0H7            (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PCLRR_FEC0L */
#define MCF548X_GPIO_PCLRR_FEC0L_PCLRR_FEC0L0            (0x01)
#define MCF548X_GPIO_PCLRR_FEC0L_PODR_FEC0L1             (0x02)
#define MCF548X_GPIO_PCLRR_FEC0L_PCLRR_FEC0L2            (0x04)
#define MCF548X_GPIO_PCLRR_FEC0L_PCLRR_FEC0L3            (0x08)
#define MCF548X_GPIO_PCLRR_FEC0L_PODR_FEC0L4             (0x10)
#define MCF548X_GPIO_PCLRR_FEC0L_PODR_FEC0L5             (0x20)
#define MCF548X_GPIO_PCLRR_FEC0L_PODR_FEC0L6             (0x40)
#define MCF548X_GPIO_PCLRR_FEC0L_PCLRR_FEC0L7            (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PCLRR_FEC1H */
#define MCF548X_GPIO_PCLRR_FEC1H_PCLRR_FEC1H0            (0x01)
#define MCF548X_GPIO_PCLRR_FEC1H_PCLRR_FEC1H1            (0x02)
#define MCF548X_GPIO_PCLRR_FEC1H_PCLRR_FEC1H2            (0x04)
#define MCF548X_GPIO_PCLRR_FEC1H_PODR_FEC1H3             (0x08)
#define MCF548X_GPIO_PCLRR_FEC1H_PODR_FEC1H4             (0x10)
#define MCF548X_GPIO_PCLRR_FEC1H_PCLRR_FEC1H5            (0x20)
#define MCF548X_GPIO_PCLRR_FEC1H_PCLRR_FEC1H6            (0x40)
#define MCF548X_GPIO_PCLRR_FEC1H_PCLRR_FEC1H7            (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PCLRR_FEC1L */
#define MCF548X_GPIO_PCLRR_FEC1L_PCLRR_FEC1L0            (0x01)
#define MCF548X_GPIO_PCLRR_FEC1L_PCLRR_FEC1L1            (0x02)
#define MCF548X_GPIO_PCLRR_FEC1L_PCLRR_FEC1L2            (0x04)
#define MCF548X_GPIO_PCLRR_FEC1L_PCLRR_FEC1L3            (0x08)
#define MCF548X_GPIO_PCLRR_FEC1L_PODR_FEC1L4             (0x10)
#define MCF548X_GPIO_PCLRR_FEC1L_PCLRR_FEC1L5            (0x20)
#define MCF548X_GPIO_PCLRR_FEC1L_PCLRR_FEC1L6            (0x40)
#define MCF548X_GPIO_PCLRR_FEC1L_PCLRR_FEC1L7            (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PCLRR_FECI2C */
#define MCF548X_GPIO_PCLRR_FECI2C_PCLRR_FECI2C0          (0x01)
#define MCF548X_GPIO_PCLRR_FECI2C_PCLRR_FECI2C1          (0x02)
#define MCF548X_GPIO_PCLRR_FECI2C_PODR_FECI2C2           (0x04)
#define MCF548X_GPIO_PCLRR_FECI2C_PCLRR_FECI2C3          (0x08)

/* Bit definitions and macros for MCF548X_GPIO_PCLRR_PCIBG */
#define MCF548X_GPIO_PCLRR_PCIBG_PODR_PCIBG0             (0x01)
#define MCF548X_GPIO_PCLRR_PCIBG_PODR_PCIBG1             (0x02)
#define MCF548X_GPIO_PCLRR_PCIBG_PODR_PCIBG2             (0x04)
#define MCF548X_GPIO_PCLRR_PCIBG_PCLRR_PCIBG3            (0x08)
#define MCF548X_GPIO_PCLRR_PCIBG_PCLRR_PCIBG4            (0x10)

/* Bit definitions and macros for MCF548X_GPIO_PCLRR_PCIBR */
#define MCF548X_GPIO_PCLRR_PCIBR_PCLRR_PCIBR0            (0x01)
#define MCF548X_GPIO_PCLRR_PCIBR_PCLRR_PCIBR1            (0x02)
#define MCF548X_GPIO_PCLRR_PCIBR_PCLRR_PCIBR2            (0x04)
#define MCF548X_GPIO_PCLRR_PCIBR_PODR_PCIBR3             (0x08)
#define MCF548X_GPIO_PCLRR_PCIBR_PODR_PCIBR4             (0x10)

/* Bit definitions and macros for MCF548X_GPIO_PCLRR_PSC3PSC2 */
#define MCF548X_GPIO_PCLRR_PSC3PSC2_PODR_PSC3PSC20       (0x01)
#define MCF548X_GPIO_PCLRR_PSC3PSC2_PODR_PSC3PSC21       (0x02)
#define MCF548X_GPIO_PCLRR_PSC3PSC2_PCLRR_PSC3PSC22      (0x04)
#define MCF548X_GPIO_PCLRR_PSC3PSC2_PCLRR_PSC3PSC23      (0x08)
#define MCF548X_GPIO_PCLRR_PSC3PSC2_PCLRR_PSC3PSC24      (0x10)
#define MCF548X_GPIO_PCLRR_PSC3PSC2_PODR_PSC3PSC25       (0x20)
#define MCF548X_GPIO_PCLRR_PSC3PSC2_PODR_PSC3PSC26       (0x40)
#define MCF548X_GPIO_PCLRR_PSC3PSC2_PCLRR_PSC3PSC27      (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PCLRR_PSC1PSC0 */
#define MCF548X_GPIO_PCLRR_PSC1PSC0_PCLRR_PSC1PSC00      (0x01)
#define MCF548X_GPIO_PCLRR_PSC1PSC0_PCLRR_PSC1PSC01      (0x02)
#define MCF548X_GPIO_PCLRR_PSC1PSC0_PCLRR_PSC1PSC02      (0x04)
#define MCF548X_GPIO_PCLRR_PSC1PSC0_PCLRR_PSC1PSC03      (0x08)
#define MCF548X_GPIO_PCLRR_PSC1PSC0_PCLRR_PSC1PSC04      (0x10)
#define MCF548X_GPIO_PCLRR_PSC1PSC0_PCLRR_PSC1PSC05      (0x20)
#define MCF548X_GPIO_PCLRR_PSC1PSC0_PODR_PSC1PSC06       (0x40)
#define MCF548X_GPIO_PCLRR_PSC1PSC0_PCLRR_PSC1PSC07      (0x80)

/* Bit definitions and macros for MCF548X_GPIO_PCLRR_DSPI */
#define MCF548X_GPIO_PCLRR_DSPI_PCLRR_DSPI0              (0x01)
#define MCF548X_GPIO_PCLRR_DSPI_PCLRR_DSPI1              (0x02)
#define MCF548X_GPIO_PCLRR_DSPI_PCLRR_DSPI2              (0x04)
#define MCF548X_GPIO_PCLRR_DSPI_PCLRR_DSPI3              (0x08)
#define MCF548X_GPIO_PCLRR_DSPI_PCLRR_DSPI4              (0x10)
#define MCF548X_GPIO_PCLRR_DSPI_PCLRR_DSPI5              (0x20)
#define MCF548X_GPIO_PCLRR_DSPI_PCLRR_DSPI6              (0x40)

/* Bit definitions and macros for MCF548X_GPIO_PAR_FBCTL */
#define MCF548X_GPIO_PAR_FBCTL_PAR_TS(x)                 (((x)&0x0003)<<0)
#define MCF548X_GPIO_PAR_FBCTL_PAR_TA                    (0x0004)
#define MCF548X_GPIO_PAR_FBCTL_PAR_RWB(x)                (((x)&0x0003)<<4)
#define MCF548X_GPIO_PAR_FBCTL_PAR_OE                    (0x0040)
#define MCF548X_GPIO_PAR_FBCTL_PAR_BWE0                  (0x0100)
#define MCF548X_GPIO_PAR_FBCTL_PAR_BWE1                  (0x0400)
#define MCF548X_GPIO_PAR_FBCTL_PAR_BWE2                  (0x1000)
#define MCF548X_GPIO_PAR_FBCTL_PAR_BWE3                  (0x4000)
#define MCF548X_GPIO_PAR_FBCTL_PAR_TS_GPIO               (0)
#define MCF548X_GPIO_PAR_FBCTL_PAR_TS_TBST               (2)
#define MCF548X_GPIO_PAR_FBCTL_PAR_TS_TS                 (3)
#define MCF548X_GPIO_PAR_FBCTL_PAR_RWB_GPIO              (0x0000)
#define MCF548X_GPIO_PAR_FBCTL_PAR_RWB_TBST              (0x0020)
#define MCF548X_GPIO_PAR_FBCTL_PAR_RWB_RWB               (0x0030)

/* Bit definitions and macros for MCF548X_GPIO_PAR_FBCS */
#define MCF548X_GPIO_PAR_FBCS_PAR_CS1                    (0x02)
#define MCF548X_GPIO_PAR_FBCS_PAR_CS2                    (0x04)
#define MCF548X_GPIO_PAR_FBCS_PAR_CS3                    (0x08)
#define MCF548X_GPIO_PAR_FBCS_PAR_CS4                    (0x10)
#define MCF548X_GPIO_PAR_FBCS_PAR_CS5                    (0x20)

/* Bit definitions and macros for MCF548X_GPIO_PAR_DMA */
#define MCF548X_GPIO_PAR_DMA_PAR_DREQ0(x)                (((x)&0x03)<<0)
#define MCF548X_GPIO_PAR_DMA_PAR_DREQ1(x)                (((x)&0x03)<<2)
#define MCF548X_GPIO_PAR_DMA_PAR_DACK0(x)                (((x)&0x03)<<4)
#define MCF548X_GPIO_PAR_DMA_PAR_DACK1(x)                (((x)&0x03)<<6)
#define MCF548X_GPIO_PAR_DMA_PAR_DACKx_GPIO              (0)
#define MCF548X_GPIO_PAR_DMA_PAR_DACKx_TOUT              (2)
#define MCF548X_GPIO_PAR_DMA_PAR_DACKx_DACK              (3)
#define MCF548X_GPIO_PAR_DMA_PAR_DREQx_GPIO              (0)
#define MCF548X_GPIO_PAR_DMA_PAR_DREQx_TIN               (2)
#define MCF548X_GPIO_PAR_DMA_PAR_DREQx_DREQ              (3)

/* Bit definitions and macros for MCF548X_GPIO_PAR_FECI2CIRQ */
#define MCF548X_GPIO_PAR_FECI2CIRQ_PAR_IRQ5              (0x0001)
#define MCF548X_GPIO_PAR_FECI2CIRQ_PAR_IRQ6              (0x0002)
#define MCF548X_GPIO_PAR_FECI2CIRQ_PAR_SCL               (0x0004)
#define MCF548X_GPIO_PAR_FECI2CIRQ_PAR_SDA               (0x0008)
#define MCF548X_GPIO_PAR_FECI2CIRQ_PAR_E1MDC(x)          (((x)&0x0003)<<6)
#define MCF548X_GPIO_PAR_FECI2CIRQ_PAR_E1MDIO(x)         (((x)&0x0003)<<8)
#define MCF548X_GPIO_PAR_FECI2CIRQ_PAR_E1MII             (0x0400)
#define MCF548X_GPIO_PAR_FECI2CIRQ_PAR_E17               (0x0800)
#define MCF548X_GPIO_PAR_FECI2CIRQ_PAR_E0MDC             (0x1000)
#define MCF548X_GPIO_PAR_FECI2CIRQ_PAR_E0MDIO            (0x2000)
#define MCF548X_GPIO_PAR_FECI2CIRQ_PAR_E0MII             (0x4000)
#define MCF548X_GPIO_PAR_FECI2CIRQ_PAR_E07               (0x8000)
#define MCF548X_GPIO_PAR_FECI2CIRQ_PAR_E1MDIO_CANRX      (0x0000)
#define MCF548X_GPIO_PAR_FECI2CIRQ_PAR_E1MDIO_SDA        (0x0200)
#define MCF548X_GPIO_PAR_FECI2CIRQ_PAR_E1MDIO_EMDIO      (0x0300)
#define MCF548X_GPIO_PAR_FECI2CIRQ_PAR_E1MDC_CANTX       (0x0000)
#define MCF548X_GPIO_PAR_FECI2CIRQ_PAR_E1MDC_SCL         (0x0080)
#define MCF548X_GPIO_PAR_FECI2CIRQ_PAR_E1MDC_EMDC        (0x00C0)

/* Bit definitions and macros for MCF548X_GPIO_PAR_PCIBG */
#define MCF548X_GPIO_PAR_PCIBG_PAR_PCIBG0(x)             (((x)&0x0003)<<0)
#define MCF548X_GPIO_PAR_PCIBG_PAR_PCIBG1(x)             (((x)&0x0003)<<2)
#define MCF548X_GPIO_PAR_PCIBG_PAR_PCIBG2(x)             (((x)&0x0003)<<4)
#define MCF548X_GPIO_PAR_PCIBG_PAR_PCIBG3(x)             (((x)&0x0003)<<6)
#define MCF548X_GPIO_PAR_PCIBG_PAR_PCIBG4(x)             (((x)&0x0003)<<8)

/* Bit definitions and macros for MCF548X_GPIO_PAR_PCIBR */
#define MCF548X_GPIO_PAR_PCIBR_PAR_PCIBG0(x)             (((x)&0x0003)<<0)
#define MCF548X_GPIO_PAR_PCIBR_PAR_PCIBG1(x)             (((x)&0x0003)<<2)
#define MCF548X_GPIO_PAR_PCIBR_PAR_PCIBG2(x)             (((x)&0x0003)<<4)
#define MCF548X_GPIO_PAR_PCIBR_PAR_PCIBG3(x)             (((x)&0x0003)<<6)
#define MCF548X_GPIO_PAR_PCIBR_PAR_PCIBR4(x)             (((x)&0x0003)<<8)

/* Bit definitions and macros for MCF548X_GPIO_PAR_PSC3 */
#define MCF548X_GPIO_PAR_PSC3_PAR_TXD3                   (0x04)
#define MCF548X_GPIO_PAR_PSC3_PAR_RXD3                   (0x08)
#define MCF548X_GPIO_PAR_PSC3_PAR_RTS3(x)                (((x)&0x03)<<4)
#define MCF548X_GPIO_PAR_PSC3_PAR_CTS3(x)                (((x)&0x03)<<6)
#define MCF548X_GPIO_PAR_PSC3_PAR_CTS3_GPIO              (0x00)
#define MCF548X_GPIO_PAR_PSC3_PAR_CTS3_BCLK              (0x80)
#define MCF548X_GPIO_PAR_PSC3_PAR_CTS3_CTS               (0xC0)
#define MCF548X_GPIO_PAR_PSC3_PAR_RTS3_GPIO              (0x00)
#define MCF548X_GPIO_PAR_PSC3_PAR_RTS3_FSYNC             (0x20)
#define MCF548X_GPIO_PAR_PSC3_PAR_RTS3_RTS               (0x30)
#define MCF548X_GPIO_PAR_PSC3_PAR_CTS2_CANRX             (0x40)

/* Bit definitions and macros for MCF548X_GPIO_PAR_PSC2 */
#define MCF548X_GPIO_PAR_PSC2_PAR_TXD2                   (0x04)
#define MCF548X_GPIO_PAR_PSC2_PAR_RXD2                   (0x08)
#define MCF548X_GPIO_PAR_PSC2_PAR_RTS2(x)                (((x)&0x03)<<4)
#define MCF548X_GPIO_PAR_PSC2_PAR_CTS2(x)                (((x)&0x03)<<6)
#define MCF548X_GPIO_PAR_PSC2_PAR_CTS2_GPIO              (0x00)
#define MCF548X_GPIO_PAR_PSC2_PAR_CTS2_BCLK              (0x80)
#define MCF548X_GPIO_PAR_PSC2_PAR_CTS2_CTS               (0xC0)
#define MCF548X_GPIO_PAR_PSC2_PAR_RTS2_GPIO              (0x00)
#define MCF548X_GPIO_PAR_PSC2_PAR_RTS2_CANTX             (0x10)
#define MCF548X_GPIO_PAR_PSC2_PAR_RTS2_FSYNC             (0x20)
#define MCF548X_GPIO_PAR_PSC2_PAR_RTS2_RTS               (0x30)

/* Bit definitions and macros for MCF548X_GPIO_PAR_PSC1 */
#define MCF548X_GPIO_PAR_PSC1_PAR_TXD1                   (0x04)
#define MCF548X_GPIO_PAR_PSC1_PAR_RXD1                   (0x08)
#define MCF548X_GPIO_PAR_PSC1_PAR_RTS1(x)                (((x)&0x03)<<4)
#define MCF548X_GPIO_PAR_PSC1_PAR_CTS1(x)                (((x)&0x03)<<6)
#define MCF548X_GPIO_PAR_PSC1_PAR_CTS1_GPIO              (0x00)
#define MCF548X_GPIO_PAR_PSC1_PAR_CTS1_BCLK              (0x80)
#define MCF548X_GPIO_PAR_PSC1_PAR_CTS1_CTS               (0xC0)
#define MCF548X_GPIO_PAR_PSC1_PAR_RTS1_GPIO              (0x00)
#define MCF548X_GPIO_PAR_PSC1_PAR_RTS1_FSYNC             (0x20)
#define MCF548X_GPIO_PAR_PSC1_PAR_RTS1_RTS               (0x30)

/* Bit definitions and macros for MCF548X_GPIO_PAR_PSC0 */
#define MCF548X_GPIO_PAR_PSC0_PAR_TXD0                   (0x04)
#define MCF548X_GPIO_PAR_PSC0_PAR_RXD0                   (0x08)
#define MCF548X_GPIO_PAR_PSC0_PAR_RTS0(x)                (((x)&0x03)<<4)
#define MCF548X_GPIO_PAR_PSC0_PAR_CTS0(x)                (((x)&0x03)<<6)
#define MCF548X_GPIO_PAR_PSC0_PAR_CTS0_GPIO              (0x00)
#define MCF548X_GPIO_PAR_PSC0_PAR_CTS0_BCLK              (0x80)
#define MCF548X_GPIO_PAR_PSC0_PAR_CTS0_CTS               (0xC0)
#define MCF548X_GPIO_PAR_PSC0_PAR_RTS0_GPIO              (0x00)
#define MCF548X_GPIO_PAR_PSC0_PAR_RTS0_FSYNC             (0x20)
#define MCF548X_GPIO_PAR_PSC0_PAR_RTS0_RTS               (0x30)

/* Bit definitions and macros for MCF548X_GPIO_PAR_DSPI */
#define MCF548X_GPIO_PAR_DSPI_PAR_SOUT(x)                (((x)&0x0003)<<0)
#define MCF548X_GPIO_PAR_DSPI_PAR_SIN(x)                 (((x)&0x0003)<<2)
#define MCF548X_GPIO_PAR_DSPI_PAR_SCK(x)                 (((x)&0x0003)<<4)
#define MCF548X_GPIO_PAR_DSPI_PAR_CS0(x)                 (((x)&0x0003)<<6)
#define MCF548X_GPIO_PAR_DSPI_PAR_CS2(x)                 (((x)&0x0003)<<8)
#define MCF548X_GPIO_PAR_DSPI_PAR_CS3(x)                 (((x)&0x0003)<<10)
#define MCF548X_GPIO_PAR_DSPI_PAR_CS5                    (0x1000)
#define MCF548X_GPIO_PAR_DSPI_PAR_CS3_GPIO               (0x0000)
#define MCF548X_GPIO_PAR_DSPI_PAR_CS3_CANTX              (0x0400)
#define MCF548X_GPIO_PAR_DSPI_PAR_CS3_TOUT               (0x0800)
#define MCF548X_GPIO_PAR_DSPI_PAR_CS3_DSPICS             (0x0C00)
#define MCF548X_GPIO_PAR_DSPI_PAR_CS2_GPIO               (0x0000)
#define MCF548X_GPIO_PAR_DSPI_PAR_CS2_CANTX              (0x0100)
#define MCF548X_GPIO_PAR_DSPI_PAR_CS2_TOUT               (0x0200)
#define MCF548X_GPIO_PAR_DSPI_PAR_CS2_DSPICS             (0x0300)
#define MCF548X_GPIO_PAR_DSPI_PAR_CS0_GPIO               (0x0000)
#define MCF548X_GPIO_PAR_DSPI_PAR_CS0_FSYNC              (0x0040)
#define MCF548X_GPIO_PAR_DSPI_PAR_CS0_RTS                (0x0080)
#define MCF548X_GPIO_PAR_DSPI_PAR_CS0_DSPICS             (0x00C0)
#define MCF548X_GPIO_PAR_DSPI_PAR_SCK_GPIO               (0x0000)
#define MCF548X_GPIO_PAR_DSPI_PAR_SCK_BCLK               (0x0010)
#define MCF548X_GPIO_PAR_DSPI_PAR_SCK_CTS                (0x0020)
#define MCF548X_GPIO_PAR_DSPI_PAR_SCK_SCK                (0x0030)
#define MCF548X_GPIO_PAR_DSPI_PAR_SIN_GPIO               (0x0000)
#define MCF548X_GPIO_PAR_DSPI_PAR_SIN_RXD                (0x0008)
#define MCF548X_GPIO_PAR_DSPI_PAR_SIN_SIN                (0x000C)
#define MCF548X_GPIO_PAR_DSPI_PAR_SOUT_GPIO              (0x0000)
#define MCF548X_GPIO_PAR_DSPI_PAR_SOUT_TXD               (0x0002)
#define MCF548X_GPIO_PAR_DSPI_PAR_SOUT_SOUT              (0x0003)

/* Bit definitions and macros for MCF548X_GPIO_PAR_TIMER */
#define MCF548X_GPIO_PAR_TIMER_PAR_TOUT2                 (0x01)
#define MCF548X_GPIO_PAR_TIMER_PAR_TIN2(x)               (((x)&0x03)<<1)
#define MCF548X_GPIO_PAR_TIMER_PAR_TOUT3                 (0x08)
#define MCF548X_GPIO_PAR_TIMER_PAR_TIN3(x)               (((x)&0x03)<<4)
#define MCF548X_GPIO_PAR_TIMER_PAR_TIN3_CANRX            (0x00)
#define MCF548X_GPIO_PAR_TIMER_PAR_TIN3_IRQ              (0x20)
#define MCF548X_GPIO_PAR_TIMER_PAR_TIN3_TIN              (0x30)
#define MCF548X_GPIO_PAR_TIMER_PAR_TIN2_CANRX            (0x00)
#define MCF548X_GPIO_PAR_TIMER_PAR_TIN2_IRQ              (0x04)
#define MCF548X_GPIO_PAR_TIMER_PAR_TIN2_TIN              (0x06)

/*********************************************************************
*
* General Purpose Timers (GPT)
*
*********************************************************************/

/* Register read/write macros */
#define MCF548X_GPT_GMS0       (*(volatile uint32_t*)(void*)(&__MBAR[0x000800]))
#define MCF548X_GPT_GCIR0      (*(volatile uint32_t*)(void*)(&__MBAR[0x000804]))
#define MCF548X_GPT_GPWM0      (*(volatile uint32_t*)(void*)(&__MBAR[0x000808]))
#define MCF548X_GPT_GSR0       (*(volatile uint32_t*)(void*)(&__MBAR[0x00080C]))
#define MCF548X_GPT_GMS1       (*(volatile uint32_t*)(void*)(&__MBAR[0x000810]))
#define MCF548X_GPT_GCIR1      (*(volatile uint32_t*)(void*)(&__MBAR[0x000814]))
#define MCF548X_GPT_GPWM1      (*(volatile uint32_t*)(void*)(&__MBAR[0x000818]))
#define MCF548X_GPT_GSR1       (*(volatile uint32_t*)(void*)(&__MBAR[0x00081C]))
#define MCF548X_GPT_GMS2       (*(volatile uint32_t*)(void*)(&__MBAR[0x000820]))
#define MCF548X_GPT_GCIR2      (*(volatile uint32_t*)(void*)(&__MBAR[0x000824]))
#define MCF548X_GPT_GPWM2      (*(volatile uint32_t*)(void*)(&__MBAR[0x000828]))
#define MCF548X_GPT_GSR2       (*(volatile uint32_t*)(void*)(&__MBAR[0x00082C]))
#define MCF548X_GPT_GMS3       (*(volatile uint32_t*)(void*)(&__MBAR[0x000830]))
#define MCF548X_GPT_GCIR3      (*(volatile uint32_t*)(void*)(&__MBAR[0x000834]))
#define MCF548X_GPT_GPWM3      (*(volatile uint32_t*)(void*)(&__MBAR[0x000838]))
#define MCF548X_GPT_GSR3       (*(volatile uint32_t*)(void*)(&__MBAR[0x00083C]))
#define MCF548X_GPT_GMS(x)     (*(volatile uint32_t*)(void*)(&__MBAR[0x000800U+((x)*0x010)]))
#define MCF548X_GPT_GCIR(x)    (*(volatile uint32_t*)(void*)(&__MBAR[0x000804U+((x)*0x010)]))
#define MCF548X_GPT_GPWM(x)    (*(volatile uint32_t*)(void*)(&__MBAR[0x000808U+((x)*0x010)]))
#define MCF548X_GPT_GSR(x)     (*(volatile uint32_t*)(void*)(&__MBAR[0x00080CU+((x)*0x010)]))

/* Bit definitions and macros for MCF548X_GPT_GMS */
#define MCF548X_GPT_GMS_TMS(x)         (((x)&0x00000007)<<0)
#define MCF548X_GPT_GMS_GPIO(x)        (((x)&0x00000003)<<4)
#define MCF548X_GPT_GMS_IEN            (0x00000100)
#define MCF548X_GPT_GMS_OD             (0x00000200)
#define MCF548X_GPT_GMS_SC             (0x00000400)
#define MCF548X_GPT_GMS_CE             (0x00001000)
#define MCF548X_GPT_GMS_WDEN           (0x00008000)
#define MCF548X_GPT_GMS_ICT(x)         (((x)&0x00000003)<<16)
#define MCF548X_GPT_GMS_OCT(x)         (((x)&0x00000003)<<20)
#define MCF548X_GPT_GMS_OCPW(x)        (((x)&0x000000FF)<<24)
#define MCF548X_GPT_GMS_OCT_FRCLOW     (0x00000000)
#define MCF548X_GPT_GMS_OCT_PULSEHI    (0x00100000)
#define MCF548X_GPT_GMS_OCT_PULSELO    (0x00200000)
#define MCF548X_GPT_GMS_OCT_TOGGLE     (0x00300000)
#define MCF548X_GPT_GMS_ICT_ANY        (0x00000000)
#define MCF548X_GPT_GMS_ICT_RISE       (0x00010000)
#define MCF548X_GPT_GMS_ICT_FALL       (0x00020000)
#define MCF548X_GPT_GMS_ICT_PULSE      (0x00030000)
#define MCF548X_GPT_GMS_GPIO_INPUT     (0x00000000)
#define MCF548X_GPT_GMS_GPIO_OUTLO     (0x00000020)
#define MCF548X_GPT_GMS_GPIO_OUTHI     (0x00000030)
#define MCF548X_GPT_GMS_TMS_DISABLE    (0x00000000)
#define MCF548X_GPT_GMS_TMS_INCAPT     (0x00000001)
#define MCF548X_GPT_GMS_TMS_OUTCAPT    (0x00000002)
#define MCF548X_GPT_GMS_TMS_PWM        (0x00000003)
#define MCF548X_GPT_GMS_TMS_GPIO       (0x00000004)

/* Bit definitions and macros for MCF548X_GPT_GCIR */
#define MCF548X_GPT_GCIR_CNT(x)        (((x)&0x0000FFFF)<<0)
#define MCF548X_GPT_GCIR_PRE(x)        (((x)&0x0000FFFF)<<16)

/* Bit definitions and macros for MCF548X_GPT_GPWM */
#define MCF548X_GPT_GPWM_LOAD          (0x00000001)
#define MCF548X_GPT_GPWM_PWMOP         (0x00000100)
#define MCF548X_GPT_GPWM_WIDTH(x)      (((x)&0x0000FFFF)<<16)

/* Bit definitions and macros for MCF548X_GPT_GSR */
#define MCF548X_GPT_GSR_CAPT           (0x00000001)
#define MCF548X_GPT_GSR_COMP           (0x00000002)
#define MCF548X_GPT_GSR_PWMP           (0x00000004)
#define MCF548X_GPT_GSR_TEXP           (0x00000008)
#define MCF548X_GPT_GSR_PIN            (0x00000100)
#define MCF548X_GPT_GSR_OVF(x)         (((x)&0x00000007)<<12)
#define MCF548X_GPT_GSR_CAPTURE(x)     (((x)&0x0000FFFF)<<16)


/*********************************************************************
*
* I2C Module (I2C)
*
*********************************************************************/

/* Register read/write macros */
#define MCF548X_I2C_I2AR     (*(volatile uint8_t *)(void*)(&__MBAR[0x008F00]))
#define MCF548X_I2C_I2FDR    (*(volatile uint8_t *)(void*)(&__MBAR[0x008F04]))
#define MCF548X_I2C_I2CR     (*(volatile uint8_t *)(void*)(&__MBAR[0x008F08]))
#define MCF548X_I2C_I2SR     (*(volatile uint8_t *)(void*)(&__MBAR[0x008F0C]))
#define MCF548X_I2C_I2DR     (*(volatile uint8_t *)(void*)(&__MBAR[0x008F10]))
#define MCF548X_I2C_I2ICR    (*(volatile uint8_t *)(void*)(&__MBAR[0x008F20]))

/* Bit definitions and macros for MCF548X_I2C_I2AR */
#define MCF548X_I2C_I2AR_ADR(x)    (((x)&0x7F)<<1)

/* Bit definitions and macros for MCF548X_I2C_I2FDR */
#define MCF548X_I2C_I2FDR_IC(x)    (((x)&0x3F)<<0)

/* Bit definitions and macros for MCF548X_I2C_I2CR */
#define MCF548X_I2C_I2CR_RSTA      (0x04)
#define MCF548X_I2C_I2CR_TXAK      (0x08)
#define MCF548X_I2C_I2CR_MTX       (0x10)
#define MCF548X_I2C_I2CR_MSTA      (0x20)
#define MCF548X_I2C_I2CR_IIEN      (0x40)
#define MCF548X_I2C_I2CR_IEN       (0x80)

/* Bit definitions and macros for MCF548X_I2C_I2SR */
#define MCF548X_I2C_I2SR_RXAK      (0x01)
#define MCF548X_I2C_I2SR_IIF       (0x02)
#define MCF548X_I2C_I2SR_SRW       (0x04)
#define MCF548X_I2C_I2SR_IAL       (0x10)
#define MCF548X_I2C_I2SR_IBB       (0x20)
#define MCF548X_I2C_I2SR_IAAS      (0x40)
#define MCF548X_I2C_I2SR_ICF       (0x80)

/* Bit definitions and macros for MCF548X_I2C_I2ICR */
#define MCF548X_I2C_I2ICR_IE       (0x01)
#define MCF548X_I2C_I2ICR_RE       (0x02)
#define MCF548X_I2C_I2ICR_TE       (0x04)
#define MCF548X_I2C_I2ICR_BNBE     (0x08)

/*********************************************************************
*
* Interrupt Controller (INTC)
*
*********************************************************************/

/* Register read/write macros */
#define MCF548X_INTC_IPRH         (*(volatile uint32_t*)(void*)(&__MBAR[0x000700]))
#define MCF548X_INTC_IPRL         (*(volatile uint32_t*)(void*)(&__MBAR[0x000704]))
#define MCF548X_INTC_IMRH         (*(volatile uint32_t*)(void*)(&__MBAR[0x000708]))
#define MCF548X_INTC_IMRL         (*(volatile uint32_t*)(void*)(&__MBAR[0x00070C]))
#define MCF548X_INTC_INTFRCH      (*(volatile uint32_t*)(void*)(&__MBAR[0x000710]))
#define MCF548X_INTC_INTFRCL      (*(volatile uint32_t*)(void*)(&__MBAR[0x000714]))
#define MCF548X_INTC_IRLR         (*(volatile uint8_t *)(void*)(&__MBAR[0x000718]))
#define MCF548X_INTC_IACKLPR      (*(volatile uint8_t *)(void*)(&__MBAR[0x000719]))
#define MCF548X_INTC_ICR0         (*(volatile uint8_t *)(void*)(&__MBAR[0x000740]))
#define MCF548X_INTC_ICR1         (*(volatile uint8_t *)(void*)(&__MBAR[0x000741]))
#define MCF548X_INTC_ICR2         (*(volatile uint8_t *)(void*)(&__MBAR[0x000742]))
#define MCF548X_INTC_ICR3         (*(volatile uint8_t *)(void*)(&__MBAR[0x000743]))
#define MCF548X_INTC_ICR4         (*(volatile uint8_t *)(void*)(&__MBAR[0x000744]))
#define MCF548X_INTC_ICR5         (*(volatile uint8_t *)(void*)(&__MBAR[0x000745]))
#define MCF548X_INTC_ICR6         (*(volatile uint8_t *)(void*)(&__MBAR[0x000746]))
#define MCF548X_INTC_ICR7         (*(volatile uint8_t *)(void*)(&__MBAR[0x000747]))
#define MCF548X_INTC_ICR8         (*(volatile uint8_t *)(void*)(&__MBAR[0x000748]))
#define MCF548X_INTC_ICR9         (*(volatile uint8_t *)(void*)(&__MBAR[0x000749]))
#define MCF548X_INTC_ICR10        (*(volatile uint8_t *)(void*)(&__MBAR[0x00074A]))
#define MCF548X_INTC_ICR11        (*(volatile uint8_t *)(void*)(&__MBAR[0x00074B]))
#define MCF548X_INTC_ICR12        (*(volatile uint8_t *)(void*)(&__MBAR[0x00074C]))
#define MCF548X_INTC_ICR13        (*(volatile uint8_t *)(void*)(&__MBAR[0x00074D]))
#define MCF548X_INTC_ICR14        (*(volatile uint8_t *)(void*)(&__MBAR[0x00074E]))
#define MCF548X_INTC_ICR15        (*(volatile uint8_t *)(void*)(&__MBAR[0x00074F]))
#define MCF548X_INTC_ICR16        (*(volatile uint8_t *)(void*)(&__MBAR[0x000750]))
#define MCF548X_INTC_ICR17        (*(volatile uint8_t *)(void*)(&__MBAR[0x000751]))
#define MCF548X_INTC_ICR18        (*(volatile uint8_t *)(void*)(&__MBAR[0x000752]))
#define MCF548X_INTC_ICR19        (*(volatile uint8_t *)(void*)(&__MBAR[0x000753]))
#define MCF548X_INTC_ICR20        (*(volatile uint8_t *)(void*)(&__MBAR[0x000754]))
#define MCF548X_INTC_ICR21        (*(volatile uint8_t *)(void*)(&__MBAR[0x000755]))
#define MCF548X_INTC_ICR22        (*(volatile uint8_t *)(void*)(&__MBAR[0x000756]))
#define MCF548X_INTC_ICR23        (*(volatile uint8_t *)(void*)(&__MBAR[0x000757]))
#define MCF548X_INTC_ICR24        (*(volatile uint8_t *)(void*)(&__MBAR[0x000758]))
#define MCF548X_INTC_ICR25        (*(volatile uint8_t *)(void*)(&__MBAR[0x000759]))
#define MCF548X_INTC_ICR26        (*(volatile uint8_t *)(void*)(&__MBAR[0x00075A]))
#define MCF548X_INTC_ICR27        (*(volatile uint8_t *)(void*)(&__MBAR[0x00075B]))
#define MCF548X_INTC_ICR28        (*(volatile uint8_t *)(void*)(&__MBAR[0x00075C]))
#define MCF548X_INTC_ICR29        (*(volatile uint8_t *)(void*)(&__MBAR[0x00075D]))
#define MCF548X_INTC_ICR30        (*(volatile uint8_t *)(void*)(&__MBAR[0x00075E]))
#define MCF548X_INTC_ICR31        (*(volatile uint8_t *)(void*)(&__MBAR[0x00075F]))
#define MCF548X_INTC_ICR32        (*(volatile uint8_t *)(void*)(&__MBAR[0x000760]))
#define MCF548X_INTC_ICR33        (*(volatile uint8_t *)(void*)(&__MBAR[0x000761]))
#define MCF548X_INTC_ICR34        (*(volatile uint8_t *)(void*)(&__MBAR[0x000762]))
#define MCF548X_INTC_ICR35        (*(volatile uint8_t *)(void*)(&__MBAR[0x000763]))
#define MCF548X_INTC_ICR36        (*(volatile uint8_t *)(void*)(&__MBAR[0x000764]))
#define MCF548X_INTC_ICR37        (*(volatile uint8_t *)(void*)(&__MBAR[0x000765]))
#define MCF548X_INTC_ICR38        (*(volatile uint8_t *)(void*)(&__MBAR[0x000766]))
#define MCF548X_INTC_ICR39        (*(volatile uint8_t *)(void*)(&__MBAR[0x000767]))
#define MCF548X_INTC_ICR40        (*(volatile uint8_t *)(void*)(&__MBAR[0x000768]))
#define MCF548X_INTC_ICR41        (*(volatile uint8_t *)(void*)(&__MBAR[0x000769]))
#define MCF548X_INTC_ICR42        (*(volatile uint8_t *)(void*)(&__MBAR[0x00076A]))
#define MCF548X_INTC_ICR43        (*(volatile uint8_t *)(void*)(&__MBAR[0x00076B]))
#define MCF548X_INTC_ICR44        (*(volatile uint8_t *)(void*)(&__MBAR[0x00076C]))
#define MCF548X_INTC_ICR45        (*(volatile uint8_t *)(void*)(&__MBAR[0x00076D]))
#define MCF548X_INTC_ICR46        (*(volatile uint8_t *)(void*)(&__MBAR[0x00076E]))
#define MCF548X_INTC_ICR47        (*(volatile uint8_t *)(void*)(&__MBAR[0x00076F]))
#define MCF548X_INTC_ICR48        (*(volatile uint8_t *)(void*)(&__MBAR[0x000770]))
#define MCF548X_INTC_ICR49        (*(volatile uint8_t *)(void*)(&__MBAR[0x000771]))
#define MCF548X_INTC_ICR50        (*(volatile uint8_t *)(void*)(&__MBAR[0x000772]))
#define MCF548X_INTC_ICR51        (*(volatile uint8_t *)(void*)(&__MBAR[0x000773]))
#define MCF548X_INTC_ICR52        (*(volatile uint8_t *)(void*)(&__MBAR[0x000774]))
#define MCF548X_INTC_ICR53        (*(volatile uint8_t *)(void*)(&__MBAR[0x000775]))
#define MCF548X_INTC_ICR54        (*(volatile uint8_t *)(void*)(&__MBAR[0x000776]))
#define MCF548X_INTC_ICR55        (*(volatile uint8_t *)(void*)(&__MBAR[0x000777]))
#define MCF548X_INTC_ICR56        (*(volatile uint8_t *)(void*)(&__MBAR[0x000778]))
#define MCF548X_INTC_ICR57        (*(volatile uint8_t *)(void*)(&__MBAR[0x000779]))
#define MCF548X_INTC_ICR58        (*(volatile uint8_t *)(void*)(&__MBAR[0x00077A]))
#define MCF548X_INTC_ICR59        (*(volatile uint8_t *)(void*)(&__MBAR[0x00077B]))
#define MCF548X_INTC_ICR60        (*(volatile uint8_t *)(void*)(&__MBAR[0x00077C]))
#define MCF548X_INTC_ICR61        (*(volatile uint8_t *)(void*)(&__MBAR[0x00077D]))
#define MCF548X_INTC_ICR62        (*(volatile uint8_t *)(void*)(&__MBAR[0x00077E]))
#define MCF548X_INTC_ICR63        (*(volatile uint8_t *)(void*)(&__MBAR[0x00077F]))
#define MCF548X_INTC_ICRn(x)      (*(volatile uint8_t *)(void*)(&__MBAR[0x000740U+((x)*0x001)]))
#define MCF548X_INTC_SWIACK       (*(volatile uint8_t *)(void*)(&__MBAR[0x0007E0]))
#define MCF548X_INTC_L1IACK       (*(volatile uint8_t *)(void*)(&__MBAR[0x0007E4]))
#define MCF548X_INTC_L2IACK       (*(volatile uint8_t *)(void*)(&__MBAR[0x0007E8]))
#define MCF548X_INTC_L3IACK       (*(volatile uint8_t *)(void*)(&__MBAR[0x0007EC]))
#define MCF548X_INTC_L4IACK       (*(volatile uint8_t *)(void*)(&__MBAR[0x0007F0]))
#define MCF548X_INTC_L5IACK       (*(volatile uint8_t *)(void*)(&__MBAR[0x0007F4]))
#define MCF548X_INTC_L6IACK       (*(volatile uint8_t *)(void*)(&__MBAR[0x0007F8]))
#define MCF548X_INTC_L7IACK       (*(volatile uint8_t *)(void*)(&__MBAR[0x0007FC]))
#define MCF548X_INTC_LnIACK(x)    (*(volatile uint8_t *)(void*)(&__MBAR[0x0007E4U+((x)*0x004)]))

/* Bit definitions and macros for MCF548X_INTC_IPRH */
#define MCF548X_INTC_IPRH_INT32          (0x00000001)
#define MCF548X_INTC_IPRH_INT33          (0x00000002)
#define MCF548X_INTC_IPRH_INT34          (0x00000004)
#define MCF548X_INTC_IPRH_INT35          (0x00000008)
#define MCF548X_INTC_IPRH_INT36          (0x00000010)
#define MCF548X_INTC_IPRH_INT37          (0x00000020)
#define MCF548X_INTC_IPRH_INT38          (0x00000040)
#define MCF548X_INTC_IPRH_INT39          (0x00000080)
#define MCF548X_INTC_IPRH_INT40          (0x00000100)
#define MCF548X_INTC_IPRH_INT41          (0x00000200)
#define MCF548X_INTC_IPRH_INT42          (0x00000400)
#define MCF548X_INTC_IPRH_INT43          (0x00000800)
#define MCF548X_INTC_IPRH_INT44          (0x00001000)
#define MCF548X_INTC_IPRH_INT45          (0x00002000)
#define MCF548X_INTC_IPRH_INT46          (0x00004000)
#define MCF548X_INTC_IPRH_INT47          (0x00008000)
#define MCF548X_INTC_IPRH_INT48          (0x00010000)
#define MCF548X_INTC_IPRH_INT49          (0x00020000)
#define MCF548X_INTC_IPRH_INT50          (0x00040000)
#define MCF548X_INTC_IPRH_INT51          (0x00080000)
#define MCF548X_INTC_IPRH_INT52          (0x00100000)
#define MCF548X_INTC_IPRH_INT53          (0x00200000)
#define MCF548X_INTC_IPRH_INT54          (0x00400000)
#define MCF548X_INTC_IPRH_INT55          (0x00800000)
#define MCF548X_INTC_IPRH_INT56          (0x01000000)
#define MCF548X_INTC_IPRH_INT57          (0x02000000)
#define MCF548X_INTC_IPRH_INT58          (0x04000000)
#define MCF548X_INTC_IPRH_INT59          (0x08000000)
#define MCF548X_INTC_IPRH_INT60          (0x10000000)
#define MCF548X_INTC_IPRH_INT61          (0x20000000)
#define MCF548X_INTC_IPRH_INT62          (0x40000000)
#define MCF548X_INTC_IPRH_INT63          (0x80000000)

/* Bit definitions and macros for MCF548X_INTC_IPRL */
#define MCF548X_INTC_IPRL_INT1           (0x00000002)
#define MCF548X_INTC_IPRL_INT2           (0x00000004)
#define MCF548X_INTC_IPRL_INT3           (0x00000008)
#define MCF548X_INTC_IPRL_INT4           (0x00000010)
#define MCF548X_INTC_IPRL_INT5           (0x00000020)
#define MCF548X_INTC_IPRL_INT6           (0x00000040)
#define MCF548X_INTC_IPRL_INT7           (0x00000080)
#define MCF548X_INTC_IPRL_INT8           (0x00000100)
#define MCF548X_INTC_IPRL_INT9           (0x00000200)
#define MCF548X_INTC_IPRL_INT10          (0x00000400)
#define MCF548X_INTC_IPRL_INT11          (0x00000800)
#define MCF548X_INTC_IPRL_INT12          (0x00001000)
#define MCF548X_INTC_IPRL_INT13          (0x00002000)
#define MCF548X_INTC_IPRL_INT14          (0x00004000)
#define MCF548X_INTC_IPRL_INT15          (0x00008000)
#define MCF548X_INTC_IPRL_INT16          (0x00010000)
#define MCF548X_INTC_IPRL_INT17          (0x00020000)
#define MCF548X_INTC_IPRL_INT18          (0x00040000)
#define MCF548X_INTC_IPRL_INT19          (0x00080000)
#define MCF548X_INTC_IPRL_INT20          (0x00100000)
#define MCF548X_INTC_IPRL_INT21          (0x00200000)
#define MCF548X_INTC_IPRL_INT22          (0x00400000)
#define MCF548X_INTC_IPRL_INT23          (0x00800000)
#define MCF548X_INTC_IPRL_INT24          (0x01000000)
#define MCF548X_INTC_IPRL_INT25          (0x02000000)
#define MCF548X_INTC_IPRL_INT26          (0x04000000)
#define MCF548X_INTC_IPRL_INT27          (0x08000000)
#define MCF548X_INTC_IPRL_INT28          (0x10000000)
#define MCF548X_INTC_IPRL_INT29          (0x20000000)
#define MCF548X_INTC_IPRL_INT30          (0x40000000)
#define MCF548X_INTC_IPRL_INT31          (0x80000000)

/* Bit definitions and macros for MCF548X_INTC_IMRH */
#define MCF548X_INTC_IMRH_INT_MASK32     (0x00000001)
#define MCF548X_INTC_IMRH_INT_MASK33     (0x00000002)
#define MCF548X_INTC_IMRH_INT_MASK34     (0x00000004)
#define MCF548X_INTC_IMRH_INT_MASK35     (0x00000008)
#define MCF548X_INTC_IMRH_INT_MASK36     (0x00000010)
#define MCF548X_INTC_IMRH_INT_MASK37     (0x00000020)
#define MCF548X_INTC_IMRH_INT_MASK38     (0x00000040)
#define MCF548X_INTC_IMRH_INT_MASK39     (0x00000080)
#define MCF548X_INTC_IMRH_INT_MASK40     (0x00000100)
#define MCF548X_INTC_IMRH_INT_MASK41     (0x00000200)
#define MCF548X_INTC_IMRH_INT_MASK42     (0x00000400)
#define MCF548X_INTC_IMRH_INT_MASK43     (0x00000800)
#define MCF548X_INTC_IMRH_INT_MASK44     (0x00001000)
#define MCF548X_INTC_IMRH_INT_MASK45     (0x00002000)
#define MCF548X_INTC_IMRH_INT_MASK46     (0x00004000)
#define MCF548X_INTC_IMRH_INT_MASK47     (0x00008000)
#define MCF548X_INTC_IMRH_INT_MASK48     (0x00010000)
#define MCF548X_INTC_IMRH_INT_MASK49     (0x00020000)
#define MCF548X_INTC_IMRH_INT_MASK50     (0x00040000)
#define MCF548X_INTC_IMRH_INT_MASK51     (0x00080000)
#define MCF548X_INTC_IMRH_INT_MASK52     (0x00100000)
#define MCF548X_INTC_IMRH_INT_MASK53     (0x00200000)
#define MCF548X_INTC_IMRH_INT_MASK54     (0x00400000)
#define MCF548X_INTC_IMRH_INT_MASK55     (0x00800000)
#define MCF548X_INTC_IMRH_INT_MASK56     (0x01000000)
#define MCF548X_INTC_IMRH_INT_MASK57     (0x02000000)
#define MCF548X_INTC_IMRH_INT_MASK58     (0x04000000)
#define MCF548X_INTC_IMRH_INT_MASK59     (0x08000000)
#define MCF548X_INTC_IMRH_INT_MASK60     (0x10000000)
#define MCF548X_INTC_IMRH_INT_MASK61     (0x20000000)
#define MCF548X_INTC_IMRH_INT_MASK62     (0x40000000)
#define MCF548X_INTC_IMRH_INT_MASK63     (0x80000000)

/* Bit definitions and macros for MCF548X_INTC_IMRL */
#define MCF548X_INTC_IMRL_MASKALL        (0x00000001)
#define MCF548X_INTC_IMRL_INT_MASK1      (0x00000002)
#define MCF548X_INTC_IMRL_INT_MASK2      (0x00000004)
#define MCF548X_INTC_IMRL_INT_MASK3      (0x00000008)
#define MCF548X_INTC_IMRL_INT_MASK4      (0x00000010)
#define MCF548X_INTC_IMRL_INT_MASK5      (0x00000020)
#define MCF548X_INTC_IMRL_INT_MASK6      (0x00000040)
#define MCF548X_INTC_IMRL_INT_MASK7      (0x00000080)
#define MCF548X_INTC_IMRL_INT_MASK8      (0x00000100)
#define MCF548X_INTC_IMRL_INT_MASK9      (0x00000200)
#define MCF548X_INTC_IMRL_INT_MASK10     (0x00000400)
#define MCF548X_INTC_IMRL_INT_MASK11     (0x00000800)
#define MCF548X_INTC_IMRL_INT_MASK12     (0x00001000)
#define MCF548X_INTC_IMRL_INT_MASK13     (0x00002000)
#define MCF548X_INTC_IMRL_INT_MASK14     (0x00004000)
#define MCF548X_INTC_IMRL_INT_MASK15     (0x00008000)
#define MCF548X_INTC_IMRL_INT_MASK16     (0x00010000)
#define MCF548X_INTC_IMRL_INT_MASK17     (0x00020000)
#define MCF548X_INTC_IMRL_INT_MASK18     (0x00040000)
#define MCF548X_INTC_IMRL_INT_MASK19     (0x00080000)
#define MCF548X_INTC_IMRL_INT_MASK20     (0x00100000)
#define MCF548X_INTC_IMRL_INT_MASK21     (0x00200000)
#define MCF548X_INTC_IMRL_INT_MASK22     (0x00400000)
#define MCF548X_INTC_IMRL_INT_MASK23     (0x00800000)
#define MCF548X_INTC_IMRL_INT_MASK24     (0x01000000)
#define MCF548X_INTC_IMRL_INT_MASK25     (0x02000000)
#define MCF548X_INTC_IMRL_INT_MASK26     (0x04000000)
#define MCF548X_INTC_IMRL_INT_MASK27     (0x08000000)
#define MCF548X_INTC_IMRL_INT_MASK28     (0x10000000)
#define MCF548X_INTC_IMRL_INT_MASK29     (0x20000000)
#define MCF548X_INTC_IMRL_INT_MASK30     (0x40000000)
#define MCF548X_INTC_IMRL_INT_MASK31     (0x80000000)

/* Bit definitions and macros for MCF548X_INTC_INTFRCH */
#define MCF548X_INTC_INTFRCH_INTFRC32    (0x00000001)
#define MCF548X_INTC_INTFRCH_INTFRC33    (0x00000002)
#define MCF548X_INTC_INTFRCH_INTFRC34    (0x00000004)
#define MCF548X_INTC_INTFRCH_INTFRC35    (0x00000008)
#define MCF548X_INTC_INTFRCH_INTFRC36    (0x00000010)
#define MCF548X_INTC_INTFRCH_INTFRC37    (0x00000020)
#define MCF548X_INTC_INTFRCH_INTFRC38    (0x00000040)
#define MCF548X_INTC_INTFRCH_INTFRC39    (0x00000080)
#define MCF548X_INTC_INTFRCH_INTFRC40    (0x00000100)
#define MCF548X_INTC_INTFRCH_INTFRC41    (0x00000200)
#define MCF548X_INTC_INTFRCH_INTFRC42    (0x00000400)
#define MCF548X_INTC_INTFRCH_INTFRC43    (0x00000800)
#define MCF548X_INTC_INTFRCH_INTFRC44    (0x00001000)
#define MCF548X_INTC_INTFRCH_INTFRC45    (0x00002000)
#define MCF548X_INTC_INTFRCH_INTFRC46    (0x00004000)
#define MCF548X_INTC_INTFRCH_INTFRC47    (0x00008000)
#define MCF548X_INTC_INTFRCH_INTFRC48    (0x00010000)
#define MCF548X_INTC_INTFRCH_INTFRC49    (0x00020000)
#define MCF548X_INTC_INTFRCH_INTFRC50    (0x00040000)
#define MCF548X_INTC_INTFRCH_INTFRC51    (0x00080000)
#define MCF548X_INTC_INTFRCH_INTFRC52    (0x00100000)
#define MCF548X_INTC_INTFRCH_INTFRC53    (0x00200000)
#define MCF548X_INTC_INTFRCH_INTFRC54    (0x00400000)
#define MCF548X_INTC_INTFRCH_INTFRC55    (0x00800000)
#define MCF548X_INTC_INTFRCH_INTFRC56    (0x01000000)
#define MCF548X_INTC_INTFRCH_INTFRC57    (0x02000000)
#define MCF548X_INTC_INTFRCH_INTFRC58    (0x04000000)
#define MCF548X_INTC_INTFRCH_INTFRC59    (0x08000000)
#define MCF548X_INTC_INTFRCH_INTFRC60    (0x10000000)
#define MCF548X_INTC_INTFRCH_INTFRC61    (0x20000000)
#define MCF548X_INTC_INTFRCH_INTFRC62    (0x40000000)
#define MCF548X_INTC_INTFRCH_INTFRC63    (0x80000000)

/* Bit definitions and macros for MCF548X_INTC_INTFRCL */
#define MCF548X_INTC_INTFRCL_INTFRC1     (0x00000002)
#define MCF548X_INTC_INTFRCL_INTFRC2     (0x00000004)
#define MCF548X_INTC_INTFRCL_INTFRC3     (0x00000008)
#define MCF548X_INTC_INTFRCL_INTFRC4     (0x00000010)
#define MCF548X_INTC_INTFRCL_INTFRC5     (0x00000020)
#define MCF548X_INTC_INTFRCL_INT6        (0x00000040)
#define MCF548X_INTC_INTFRCL_INT7        (0x00000080)
#define MCF548X_INTC_INTFRCL_INT8        (0x00000100)
#define MCF548X_INTC_INTFRCL_INT9        (0x00000200)
#define MCF548X_INTC_INTFRCL_INT10       (0x00000400)
#define MCF548X_INTC_INTFRCL_INTFRC11    (0x00000800)
#define MCF548X_INTC_INTFRCL_INTFRC12    (0x00001000)
#define MCF548X_INTC_INTFRCL_INTFRC13    (0x00002000)
#define MCF548X_INTC_INTFRCL_INTFRC14    (0x00004000)
#define MCF548X_INTC_INTFRCL_INT15       (0x00008000)
#define MCF548X_INTC_INTFRCL_INTFRC16    (0x00010000)
#define MCF548X_INTC_INTFRCL_INTFRC17    (0x00020000)
#define MCF548X_INTC_INTFRCL_INTFRC18    (0x00040000)
#define MCF548X_INTC_INTFRCL_INTFRC19    (0x00080000)
#define MCF548X_INTC_INTFRCL_INTFRC20    (0x00100000)
#define MCF548X_INTC_INTFRCL_INTFRC21    (0x00200000)
#define MCF548X_INTC_INTFRCL_INTFRC22    (0x00400000)
#define MCF548X_INTC_INTFRCL_INTFRC23    (0x00800000)
#define MCF548X_INTC_INTFRCL_INTFRC24    (0x01000000)
#define MCF548X_INTC_INTFRCL_INTFRC25    (0x02000000)
#define MCF548X_INTC_INTFRCL_INTFRC26    (0x04000000)
#define MCF548X_INTC_INTFRCL_INTFRC27    (0x08000000)
#define MCF548X_INTC_INTFRCL_INTFRC28    (0x10000000)
#define MCF548X_INTC_INTFRCL_INTFRC29    (0x20000000)
#define MCF548X_INTC_INTFRCL_INTFRC30    (0x40000000)
#define MCF548X_INTC_INTFRCL_INTFRC31    (0x80000000)

/* Bit definitions and macros for MCF548X_INTC_IRLR */
#define MCF548X_INTC_IRLR_IRQ(x)         (((x)&0x7F)<<1)

/* Bit definitions and macros for MCF548X_INTC_IACKLPR */
#define MCF548X_INTC_IACKLPR_PRI(x)      (((x)&0x0F)<<0)
#define MCF548X_INTC_IACKLPR_LEVEL(x)    (((x)&0x07)<<4)

/* Bit definitions and macros for MCF548X_INTC_ICRn */
#define MCF548X_INTC_ICRn_IP(x)          (((x)&0x07)<<0)
#define MCF548X_INTC_ICRn_IL(x)          (((x)&0x07)<<3)


/*********************************************************************
*
* SDRAM Controller (SDRAMC)
*
*********************************************************************/

/* Register read/write macros */
#define MCF548X_SDRAMC_SDRAMDS      (*(volatile uint32_t*)(void*)(&__MBAR[0x000004]))
#define MCF548X_SDRAMC_CS0CFG       (*(volatile uint32_t*)(void*)(&__MBAR[0x000020]))
#define MCF548X_SDRAMC_CS1CFG       (*(volatile uint32_t*)(void*)(&__MBAR[0x000024]))
#define MCF548X_SDRAMC_CS2CFG       (*(volatile uint32_t*)(void*)(&__MBAR[0x000028]))
#define MCF548X_SDRAMC_CS3CFG       (*(volatile uint32_t*)(void*)(&__MBAR[0x00002C]))
#define MCF548X_SDRAMC_CSnCFG(x)    (*(volatile uint32_t*)(void*)(&__MBAR[0x000020U+((x)*0x004)]))
#define MCF548X_SDRAMC_SDMR         (*(volatile uint32_t*)(void*)(&__MBAR[0x000100]))
#define MCF548X_SDRAMC_SDCR         (*(volatile uint32_t*)(void*)(&__MBAR[0x000104]))
#define MCF548X_SDRAMC_SDCFG1       (*(volatile uint32_t*)(void*)(&__MBAR[0x000108]))
#define MCF548X_SDRAMC_SDCFG2       (*(volatile uint32_t*)(void*)(&__MBAR[0x00010C]))

/* Bit definitions and macros for MCF548X_SDRAMC_SDRAMDS */
#define MCF548X_SDRAMC_SDRAMDS_SB_D(x)         (((x)&0x00000003)<<0)
#define MCF548X_SDRAMC_SDRAMDS_SB_S(x)         (((x)&0x00000003)<<2)
#define MCF548X_SDRAMC_SDRAMDS_SB_A(x)         (((x)&0x00000003)<<4)
#define MCF548X_SDRAMC_SDRAMDS_SB_C(x)         (((x)&0x00000003)<<6)
#define MCF548X_SDRAMC_SDRAMDS_SB_E(x)         (((x)&0x00000003)<<8)
#define MCF548X_SDRAMC_SDRAMDS_DRIVE_8MA       (0x02)
#define MCF548X_SDRAMC_SDRAMDS_DRIVE_16MA      (0x01)
#define MCF548X_SDRAMC_SDRAMDS_DRIVE_24MA      (0x00)
#define MCF548X_SDRAMC_SDRAMDS_DRIVE_NONE      (0x03)

/* Bit definitions and macros for MCF548X_SDRAMC_CSnCFG */
#define MCF548X_SDRAMC_CSnCFG_CSSZ(x)          (((x)&0x0000001F)<<0)
#define MCF548X_SDRAMC_CSnCFG_CSBA(x)          (((x)&0x00000FFF)<<20)
#define MCF548X_SDRAMC_CSnCFG_CSSZ_DIABLE      (0x00000000)
#define MCF548X_SDRAMC_CSnCFG_CSSZ_1MBYTE      (0x00000013)
#define MCF548X_SDRAMC_CSnCFG_CSSZ_2MBYTE      (0x00000014)
#define MCF548X_SDRAMC_CSnCFG_CSSZ_4MBYTE      (0x00000015)
#define MCF548X_SDRAMC_CSnCFG_CSSZ_8MBYTE      (0x00000016)
#define MCF548X_SDRAMC_CSnCFG_CSSZ_16MBYTE     (0x00000017)
#define MCF548X_SDRAMC_CSnCFG_CSSZ_32MBYTE     (0x00000018)
#define MCF548X_SDRAMC_CSnCFG_CSSZ_64MBYTE     (0x00000019)
#define MCF548X_SDRAMC_CSnCFG_CSSZ_128MBYTE    (0x0000001A)
#define MCF548X_SDRAMC_CSnCFG_CSSZ_256MBYTE    (0x0000001B)
#define MCF548X_SDRAMC_CSnCFG_CSSZ_512MBYTE    (0x0000001C)
#define MCF548X_SDRAMC_CSnCFG_CSSZ_1GBYTE      (0x0000001D)
#define MCF548X_SDRAMC_CSnCFG_CSSZ_2GBYTE      (0x0000001E)
#define MCF548X_SDRAMC_CSnCFG_CSSZ_4GBYTE      (0x0000001F)

/* Bit definitions and macros for MCF548X_SDRAMC_SDMR */
#define MCF548X_SDRAMC_SDMR_CMD                (0x00010000)
#define MCF548X_SDRAMC_SDMR_AD(x)              (((x)&0x00000FFF)<<18)
#define MCF548X_SDRAMC_SDMR_BNKAD(x)           (((x)&0x00000003)<<30)
#define MCF548X_SDRAMC_SDMR_BNKAD_LMR          (0x00000000)
#define MCF548X_SDRAMC_SDMR_BNKAD_LEMR         (0x40000000)

/* Bit definitions and macros for MCF548X_SDRAMC_SDCR */
#define MCF548X_SDRAMC_SDCR_IPALL              (0x00000002)
#define MCF548X_SDRAMC_SDCR_IREF               (0x00000004)
#define MCF548X_SDRAMC_SDCR_BUFF               (0x00000010)
#define MCF548X_SDRAMC_SDCR_DQS_OE(x)          (((x)&0x0000000F)<<8)
#define MCF548X_SDRAMC_SDCR_RCNT(x)            (((x)&0x0000003F)<<16)
#define MCF548X_SDRAMC_SDCR_DRIVE              (0x00400000)
#define MCF548X_SDRAMC_SDCR_AP                 (0x00800000)
#define MCF548X_SDRAMC_SDCR_MUX(x)             (((x)&0x00000003)<<24)
#define MCF548X_SDRAMC_SDCR_REF                (0x10000000)
#define MCF548X_SDRAMC_SDCR_DDR                (0x20000000)
#define MCF548X_SDRAMC_SDCR_CKE                (0x40000000)
#define MCF548X_SDRAMC_SDCR_MODE_EN            (0x80000000)

/* Bit definitions and macros for MCF548X_SDRAMC_SDCFG1 */
#define MCF548X_SDRAMC_SDCFG1_WTLAT(x)         (((x)&0x00000007)<<4)
#define MCF548X_SDRAMC_SDCFG1_REF2ACT(x)       (((x)&0x0000000F)<<8)
#define MCF548X_SDRAMC_SDCFG1_PRE2ACT(x)       (((x)&0x00000007)<<12)
#define MCF548X_SDRAMC_SDCFG1_ACT2RW(x)        (((x)&0x00000007)<<16)
#define MCF548X_SDRAMC_SDCFG1_RDLAT(x)         (((x)&0x0000000F)<<20)
#define MCF548X_SDRAMC_SDCFG1_SWT2RD(x)        (((x)&0x00000007)<<24)
#define MCF548X_SDRAMC_SDCFG1_SRD2RW(x)        (((x)&0x0000000F)<<28)

/* Bit definitions and macros for MCF548X_SDRAMC_SDCFG2 */
#define MCF548X_SDRAMC_SDCFG2_BL(x)            (((x)&0x0000000F)<<16)
#define MCF548X_SDRAMC_SDCFG2_BRD2WT(x)        (((x)&0x0000000F)<<20)
#define MCF548X_SDRAMC_SDCFG2_BWT2RW(x)        (((x)&0x0000000F)<<24)
#define MCF548X_SDRAMC_SDCFG2_BRD2PRE(x)       (((x)&0x0000000F)<<28)

/*********************************************************************
*
* Integrated Security Engine (SEC)
*
*********************************************************************/

/* Register read/write macros */
#define MCF548X_SEC_EUACRH     (*(volatile uint32_t*)(void*)(&__MBAR[0x021000]))
#define MCF548X_SEC_EUACRL     (*(volatile uint32_t*)(void*)(&__MBAR[0x021004]))
#define MCF548X_SEC_EUASRH     (*(volatile uint32_t*)(void*)(&__MBAR[0x021028]))
#define MCF548X_SEC_EUASRL     (*(volatile uint32_t*)(void*)(&__MBAR[0x02102C]))
#define MCF548X_SEC_SIMRH      (*(volatile uint32_t*)(void*)(&__MBAR[0x021008]))
#define MCF548X_SEC_SIMRL      (*(volatile uint32_t*)(void*)(&__MBAR[0x02100C]))
#define MCF548X_SEC_SISRH      (*(volatile uint32_t*)(void*)(&__MBAR[0x021010]))
#define MCF548X_SEC_SISRL      (*(volatile uint32_t*)(void*)(&__MBAR[0x021014]))
#define MCF548X_SEC_SICRH      (*(volatile uint32_t*)(void*)(&__MBAR[0x021018]))
#define MCF548X_SEC_SICRL      (*(volatile uint32_t*)(void*)(&__MBAR[0x02101C]))
#define MCF548X_SEC_SIDR       (*(volatile uint32_t*)(void*)(&__MBAR[0x021020]))
#define MCF548X_SEC_SMCR       (*(volatile uint32_t*)(void*)(&__MBAR[0x021030]))
#define MCF548X_SEC_MEAR       (*(volatile uint32_t*)(void*)(&__MBAR[0x021038]))
#define MCF548X_SEC_CCCR0      (*(volatile uint32_t*)(void*)(&__MBAR[0x02200C]))
#define MCF548X_SEC_CCCR1      (*(volatile uint32_t*)(void*)(&__MBAR[0x02300C]))
#define MCF548X_SEC_CCPSRH0    (*(volatile uint32_t*)(void*)(&__MBAR[0x022010]))
#define MCF548X_SEC_CCPSRH1    (*(volatile uint32_t*)(void*)(&__MBAR[0x023010]))
#define MCF548X_SEC_CCPSRL0    (*(volatile uint32_t*)(void*)(&__MBAR[0x022014]))
#define MCF548X_SEC_CCPSRL1    (*(volatile uint32_t*)(void*)(&__MBAR[0x023014]))
#define MCF548X_SEC_CDPR0      (*(volatile uint32_t*)(void*)(&__MBAR[0x022044]))
#define MCF548X_SEC_CDPR1      (*(volatile uint32_t*)(void*)(&__MBAR[0x023044]))
#define MCF548X_SEC_FR0        (*(volatile uint32_t*)(void*)(&__MBAR[0x02204C]))
#define MCF548X_SEC_FR1        (*(volatile uint32_t*)(void*)(&__MBAR[0x02304C]))
#define MCF548X_SEC_AFRCR      (*(volatile uint32_t*)(void*)(&__MBAR[0x028018]))
#define MCF548X_SEC_AFSR       (*(volatile uint32_t*)(void*)(&__MBAR[0x028028]))
#define MCF548X_SEC_AFISR      (*(volatile uint32_t*)(void*)(&__MBAR[0x028030]))
#define MCF548X_SEC_AFIMR      (*(volatile uint32_t*)(void*)(&__MBAR[0x028038]))
#define MCF548X_SEC_DRCR       (*(volatile uint32_t*)(void*)(&__MBAR[0x02A018]))
#define MCF548X_SEC_DSR        (*(volatile uint32_t*)(void*)(&__MBAR[0x02A028]))
#define MCF548X_SEC_DISR       (*(volatile uint32_t*)(void*)(&__MBAR[0x02A030]))
#define MCF548X_SEC_DIMR       (*(volatile uint32_t*)(void*)(&__MBAR[0x02A038]))
#define MCF548X_SEC_MDRCR      (*(volatile uint32_t*)(void*)(&__MBAR[0x02C018]))
#define MCF548X_SEC_MDSR       (*(volatile uint32_t*)(void*)(&__MBAR[0x02C028]))
#define MCF548X_SEC_MDISR      (*(volatile uint32_t*)(void*)(&__MBAR[0x02C030]))
#define MCF548X_SEC_MDIMR      (*(volatile uint32_t*)(void*)(&__MBAR[0x02C038]))
#define MCF548X_SEC_RNGRCR     (*(volatile uint32_t*)(void*)(&__MBAR[0x02E018]))
#define MCF548X_SEC_RNGSR      (*(volatile uint32_t*)(void*)(&__MBAR[0x02E028]))
#define MCF548X_SEC_RNGISR     (*(volatile uint32_t*)(void*)(&__MBAR[0x02E030]))
#define MCF548X_SEC_RNGIMR     (*(volatile uint32_t*)(void*)(&__MBAR[0x02E038]))
#define MCF548X_SEC_AESRCR     (*(volatile uint32_t*)(void*)(&__MBAR[0x032018]))
#define MCF548X_SEC_AESSR      (*(volatile uint32_t*)(void*)(&__MBAR[0x032028]))
#define MCF548X_SEC_AESISR     (*(volatile uint32_t*)(void*)(&__MBAR[0x032030]))
#define MCF548X_SEC_AESIMR     (*(volatile uint32_t*)(void*)(&__MBAR[0x032038]))

/* Bit definitions and macros for MCF548X_SEC_EUACRH */
#define MCF548X_SEC_EUACRH_AFEU(x)          (((x)&0x0000000F)<<0)
#define MCF548X_SEC_EUACRH_MDEU(x)          (((x)&0x0000000F)<<8)
#define MCF548X_SEC_EUACRH_RNG(x)           (((x)&0x0000000F)<<24)
#define MCF548X_SEC_EUACRH_RNG_NOASSIGN     (0x00000000)
#define MCF548X_SEC_EUACRH_RNG_CHA0         (0x01000000)
#define MCF548X_SEC_EUACRH_RNG_CHA1         (0x02000000)
#define MCF548X_SEC_EUACRH_MDEU_NOASSIGN    (0x00000000)
#define MCF548X_SEC_EUACRH_MDEU_CHA0        (0x00000100)
#define MCF548X_SEC_EUACRH_MDEU_CHA1        (0x00000200)
#define MCF548X_SEC_EUACRH_AFEU_NOASSIGN    (0x00000000)
#define MCF548X_SEC_EUACRH_AFEU_CHA0        (0x00000001)
#define MCF548X_SEC_EUACRH_AFEU_CHA1        (0x00000002)

/* Bit definitions and macros for MCF548X_SEC_EUACRL */
#define MCF548X_SEC_EUACRL_AESU(x)          (((x)&0x0000000F)<<16)
#define MCF548X_SEC_EUACRL_DEU(x)           (((x)&0x0000000F)<<24)
#define MCF548X_SEC_EUACRL_DEU_NOASSIGN     (0x00000000)
#define MCF548X_SEC_EUACRL_DEU_CHA0         (0x01000000)
#define MCF548X_SEC_EUACRL_DEU_CHA1         (0x02000000)
#define MCF548X_SEC_EUACRL_AESU_NOASSIGN    (0x00000000)
#define MCF548X_SEC_EUACRL_AESU_CHA0        (0x00010000)
#define MCF548X_SEC_EUACRL_AESU_CHA1        (0x00020000)

/* Bit definitions and macros for MCF548X_SEC_EUASRH */
#define MCF548X_SEC_EUASRH_AFEU(x)          (((x)&0x0000000F)<<0)
#define MCF548X_SEC_EUASRH_MDEU(x)          (((x)&0x0000000F)<<8)
#define MCF548X_SEC_EUASRH_RNG(x)           (((x)&0x0000000F)<<24)

/* Bit definitions and macros for MCF548X_SEC_EUASRL */
#define MCF548X_SEC_EUASRL_AESU(x)          (((x)&0x0000000F)<<16)
#define MCF548X_SEC_EUASRL_DEU(x)           (((x)&0x0000000F)<<24)

/* Bit definitions and macros for MCF548X_SEC_SIMRH */
#define MCF548X_SEC_SIMRH_AERR              (0x08000000)
#define MCF548X_SEC_SIMRH_CHA0DN            (0x10000000)
#define MCF548X_SEC_SIMRH_CHA0ERR           (0x20000000)
#define MCF548X_SEC_SIMRH_CHA1DN            (0x40000000)
#define MCF548X_SEC_SIMRH_CHA1ERR           (0x80000000)

/* Bit definitions and macros for MCF548X_SEC_SIMRL */
#define MCF548X_SEC_SIMRL_TEA               (0x00000040)
#define MCF548X_SEC_SIMRL_DEUDN             (0x00000100)
#define MCF548X_SEC_SIMRL_DEUERR            (0x00000200)
#define MCF548X_SEC_SIMRL_AESUDN            (0x00001000)
#define MCF548X_SEC_SIMRL_AESUERR           (0x00002000)
#define MCF548X_SEC_SIMRL_MDEUDN            (0x00010000)
#define MCF548X_SEC_SIMRL_MDEUERR           (0x00020000)
#define MCF548X_SEC_SIMRL_AFEUDN            (0x00100000)
#define MCF548X_SEC_SIMRL_AFEUERR           (0x00200000)
#define MCF548X_SEC_SIMRL_RNGDN             (0x01000000)
#define MCF548X_SEC_SIMRL_RNGERR            (0x02000000)

/* Bit definitions and macros for MCF548X_SEC_SISRH */
#define MCF548X_SEC_SISRH_AERR              (0x08000000)
#define MCF548X_SEC_SISRH_CHA0DN            (0x10000000)
#define MCF548X_SEC_SISRH_CHA0ERR           (0x20000000)
#define MCF548X_SEC_SISRH_CHA1DN            (0x40000000)
#define MCF548X_SEC_SISRH_CHA1ERR           (0x80000000)

/* Bit definitions and macros for MCF548X_SEC_SISRL */
#define MCF548X_SEC_SISRL_TEA               (0x00000040)
#define MCF548X_SEC_SISRL_DEUDN             (0x00000100)
#define MCF548X_SEC_SISRL_DEUERR            (0x00000200)
#define MCF548X_SEC_SISRL_AESUDN            (0x00001000)
#define MCF548X_SEC_SISRL_AESUERR           (0x00002000)
#define MCF548X_SEC_SISRL_MDEUDN            (0x00010000)
#define MCF548X_SEC_SISRL_MDEUERR           (0x00020000)
#define MCF548X_SEC_SISRL_AFEUDN            (0x00100000)
#define MCF548X_SEC_SISRL_AFEUERR           (0x00200000)
#define MCF548X_SEC_SISRL_RNGDN             (0x01000000)
#define MCF548X_SEC_SISRL_RNGERR            (0x02000000)

/* Bit definitions and macros for MCF548X_SEC_SICRH */
#define MCF548X_SEC_SICRH_AERR              (0x08000000)
#define MCF548X_SEC_SICRH_CHA0DN            (0x10000000)
#define MCF548X_SEC_SICRH_CHA0ERR           (0x20000000)
#define MCF548X_SEC_SICRH_CHA1DN            (0x40000000)
#define MCF548X_SEC_SICRH_CHA1ERR           (0x80000000)

/* Bit definitions and macros for MCF548X_SEC_SICRL */
#define MCF548X_SEC_SICRL_TEA               (0x00000040)
#define MCF548X_SEC_SICRL_DEUDN             (0x00000100)
#define MCF548X_SEC_SICRL_DEUERR            (0x00000200)
#define MCF548X_SEC_SICRL_AESUDN            (0x00001000)
#define MCF548X_SEC_SICRL_AESUERR           (0x00002000)
#define MCF548X_SEC_SICRL_MDEUDN            (0x00010000)
#define MCF548X_SEC_SICRL_MDEUERR           (0x00020000)
#define MCF548X_SEC_SICRL_AFEUDN            (0x00100000)
#define MCF548X_SEC_SICRL_AFEUERR           (0x00200000)
#define MCF548X_SEC_SICRL_RNGDN             (0x01000000)
#define MCF548X_SEC_SICRL_RNGERR            (0x02000000)

/* Bit definitions and macros for MCF548X_SEC_SMCR */
#define MCF548X_SEC_SMCR_CURR_CHAN(x)       (((x)&0x0000000F)<<4)
#define MCF548X_SEC_SMCR_SWR                (0x01000000)
#define MCF548X_SEC_SMCR_CURR_CHAN_1        (0x00000010)
#define MCF548X_SEC_SMCR_CURR_CHAN_2        (0x00000020)

/* Bit definitions and macros for MCF548X_SEC_CCCRn */
#define MCF548X_SEC_CCCRn_RST               (0x00000001)
#define MCF548X_SEC_CCCRn_CDIE              (0x00000002)
#define MCF548X_SEC_CCCRn_NT                (0x00000004)
#define MCF548X_SEC_CCCRn_NE                (0x00000008)
#define MCF548X_SEC_CCCRn_WE                (0x00000010)
#define MCF548X_SEC_CCCRn_BURST_SIZE(x)     (((x)&0x00000007)<<8)
#define MCF548X_SEC_CCCRn_BURST_SIZE_2      (0x00000000)
#define MCF548X_SEC_CCCRn_BURST_SIZE_8      (0x00000100)
#define MCF548X_SEC_CCCRn_BURST_SIZE_16     (0x00000200)
#define MCF548X_SEC_CCCRn_BURST_SIZE_24     (0x00000300)
#define MCF548X_SEC_CCCRn_BURST_SIZE_32     (0x00000400)
#define MCF548X_SEC_CCCRn_BURST_SIZE_40     (0x00000500)
#define MCF548X_SEC_CCCRn_BURST_SIZE_48     (0x00000600)
#define MCF548X_SEC_CCCRn_BURST_SIZE_56     (0x00000700)

/* Bit definitions and macros for MCF548X_SEC_CCPSRHn */
#define MCF548X_SEC_CCPSRHn_STATE(x)        (((x)&0x000000FF)<<0)

/* Bit definitions and macros for MCF548X_SEC_CCPSRLn */
#define MCF548X_SEC_CCPSRLn_PAIR_PTR(x)     (((x)&0x000000FF)<<0)
#define MCF548X_SEC_CCPSRLn_EUERR           (0x00000100)
#define MCF548X_SEC_CCPSRLn_SERR            (0x00000200)
#define MCF548X_SEC_CCPSRLn_DERR            (0x00000400)
#define MCF548X_SEC_CCPSRLn_PERR            (0x00001000)
#define MCF548X_SEC_CCPSRLn_TEA             (0x00002000)
#define MCF548X_SEC_CCPSRLn_SD              (0x00010000)
#define MCF548X_SEC_CCPSRLn_PD              (0x00020000)
#define MCF548X_SEC_CCPSRLn_SRD             (0x00040000)
#define MCF548X_SEC_CCPSRLn_PRD             (0x00080000)
#define MCF548X_SEC_CCPSRLn_SG              (0x00100000)
#define MCF548X_SEC_CCPSRLn_PG              (0x00200000)
#define MCF548X_SEC_CCPSRLn_SR              (0x00400000)
#define MCF548X_SEC_CCPSRLn_PR              (0x00800000)
#define MCF548X_SEC_CCPSRLn_MO              (0x01000000)
#define MCF548X_SEC_CCPSRLn_MI              (0x02000000)
#define MCF548X_SEC_CCPSRLn_STAT            (0x04000000)

/* Bit definitions and macros for MCF548X_SEC_AFRCR */
#define MCF548X_SEC_AFRCR_SR                (0x01000000)
#define MCF548X_SEC_AFRCR_MI                (0x02000000)
#define MCF548X_SEC_AFRCR_RI                (0x04000000)

/* Bit definitions and macros for MCF548X_SEC_AFSR */
#define MCF548X_SEC_AFSR_RD                 (0x01000000)
#define MCF548X_SEC_AFSR_ID                 (0x02000000)
#define MCF548X_SEC_AFSR_IE                 (0x04000000)
#define MCF548X_SEC_AFSR_OFE                (0x08000000)
#define MCF548X_SEC_AFSR_IFW                (0x10000000)
#define MCF548X_SEC_AFSR_HALT               (0x20000000)

/* Bit definitions and macros for MCF548X_SEC_AFISR */
#define MCF548X_SEC_AFISR_DSE               (0x00010000)
#define MCF548X_SEC_AFISR_KSE               (0x00020000)
#define MCF548X_SEC_AFISR_CE                (0x00040000)
#define MCF548X_SEC_AFISR_ERE               (0x00080000)
#define MCF548X_SEC_AFISR_IE                (0x00100000)
#define MCF548X_SEC_AFISR_OFU               (0x02000000)
#define MCF548X_SEC_AFISR_IFO               (0x04000000)
#define MCF548X_SEC_AFISR_IFE               (0x10000000)
#define MCF548X_SEC_AFISR_OFE               (0x20000000)
#define MCF548X_SEC_AFISR_AE                (0x40000000)
#define MCF548X_SEC_AFISR_ME                (0x80000000)

/* Bit definitions and macros for MCF548X_SEC_AFIMR */
#define MCF548X_SEC_AFIMR_DSE               (0x00010000)
#define MCF548X_SEC_AFIMR_KSE               (0x00020000)
#define MCF548X_SEC_AFIMR_CE                (0x00040000)
#define MCF548X_SEC_AFIMR_ERE               (0x00080000)
#define MCF548X_SEC_AFIMR_IE                (0x00100000)
#define MCF548X_SEC_AFIMR_OFU               (0x02000000)
#define MCF548X_SEC_AFIMR_IFO               (0x04000000)
#define MCF548X_SEC_AFIMR_IFE               (0x10000000)
#define MCF548X_SEC_AFIMR_OFE               (0x20000000)
#define MCF548X_SEC_AFIMR_AE                (0x40000000)
#define MCF548X_SEC_AFIMR_ME                (0x80000000)

/* Bit definitions and macros for MCF548X_SEC_DRCR */
#define MCF548X_SEC_DRCR_SR                 (0x01000000)
#define MCF548X_SEC_DRCR_MI                 (0x02000000)
#define MCF548X_SEC_DRCR_RI                 (0x04000000)

/* Bit definitions and macros for MCF548X_SEC_DSR */
#define MCF548X_SEC_DSR_RD                  (0x01000000)
#define MCF548X_SEC_DSR_ID                  (0x02000000)
#define MCF548X_SEC_DSR_IE                  (0x04000000)
#define MCF548X_SEC_DSR_OFR                 (0x08000000)
#define MCF548X_SEC_DSR_IFW                 (0x10000000)
#define MCF548X_SEC_DSR_HALT                (0x20000000)

/* Bit definitions and macros for MCF548X_SEC_DISR */
#define MCF548X_SEC_DISR_DSE                (0x00010000)
#define MCF548X_SEC_DISR_KSE                (0x00020000)
#define MCF548X_SEC_DISR_CE                 (0x00040000)
#define MCF548X_SEC_DISR_ERE                (0x00080000)
#define MCF548X_SEC_DISR_IE                 (0x00100000)
#define MCF548X_SEC_DISR_KPE                (0x00200000)
#define MCF548X_SEC_DISR_OFU                (0x02000000)
#define MCF548X_SEC_DISR_IFO                (0x04000000)
#define MCF548X_SEC_DISR_IFE                (0x10000000)
#define MCF548X_SEC_DISR_OFE                (0x20000000)
#define MCF548X_SEC_DISR_AE                 (0x40000000)
#define MCF548X_SEC_DISR_ME                 (0x80000000)

/* Bit definitions and macros for MCF548X_SEC_DIMR */
#define MCF548X_SEC_DIMR_DSE                (0x00010000)
#define MCF548X_SEC_DIMR_KSE                (0x00020000)
#define MCF548X_SEC_DIMR_CE                 (0x00040000)
#define MCF548X_SEC_DIMR_ERE                (0x00080000)
#define MCF548X_SEC_DIMR_IE                 (0x00100000)
#define MCF548X_SEC_DIMR_KPE                (0x00200000)
#define MCF548X_SEC_DIMR_OFU                (0x02000000)
#define MCF548X_SEC_DIMR_IFO                (0x04000000)
#define MCF548X_SEC_DIMR_IFE                (0x10000000)
#define MCF548X_SEC_DIMR_OFE                (0x20000000)
#define MCF548X_SEC_DIMR_AE                 (0x40000000)
#define MCF548X_SEC_DIMR_ME                 (0x80000000)

/* Bit definitions and macros for MCF548X_SEC_MDRCR */
#define MCF548X_SEC_MDRCR_SR                (0x01000000)
#define MCF548X_SEC_MDRCR_MI                (0x02000000)
#define MCF548X_SEC_MDRCR_RI                (0x04000000)

/* Bit definitions and macros for MCF548X_SEC_MDSR */
#define MCF548X_SEC_MDSR_RD                 (0x01000000)
#define MCF548X_SEC_MDSR_ID                 (0x02000000)
#define MCF548X_SEC_MDSR_IE                 (0x04000000)
#define MCF548X_SEC_MDSR_IFW                (0x10000000)
#define MCF548X_SEC_MDSR_HALT               (0x20000000)

/* Bit definitions and macros for MCF548X_SEC_MDISR */
#define MCF548X_SEC_MDISR_DSE               (0x00010000)
#define MCF548X_SEC_MDISR_KSE               (0x00020000)
#define MCF548X_SEC_MDISR_CE                (0x00040000)
#define MCF548X_SEC_MDISR_ERE               (0x00080000)
#define MCF548X_SEC_MDISR_IE                (0x00100000)
#define MCF548X_SEC_MDISR_IFO               (0x04000000)
#define MCF548X_SEC_MDISR_AE                (0x40000000)
#define MCF548X_SEC_MDISR_ME                (0x80000000)

/* Bit definitions and macros for MCF548X_SEC_MDIMR */
#define MCF548X_SEC_MDIMR_DSE               (0x00010000)
#define MCF548X_SEC_MDIMR_KSE               (0x00020000)
#define MCF548X_SEC_MDIMR_CE                (0x00040000)
#define MCF548X_SEC_MDIMR_ERE               (0x00080000)
#define MCF548X_SEC_MDIMR_IE                (0x00100000)
#define MCF548X_SEC_MDIMR_IFO               (0x04000000)
#define MCF548X_SEC_MDIMR_AE                (0x40000000)
#define MCF548X_SEC_MDIMR_ME                (0x80000000)

/* Bit definitions and macros for MCF548X_SEC_RNGRCR */
#define MCF548X_SEC_RNGRCR_SR               (0x01000000)
#define MCF548X_SEC_RNGRCR_MI               (0x02000000)
#define MCF548X_SEC_RNGRCR_RI               (0x04000000)

/* Bit definitions and macros for MCF548X_SEC_RNGSR */
#define MCF548X_SEC_RNGSR_RD                (0x01000000)
#define MCF548X_SEC_RNGSR_O                 (0x02000000)
#define MCF548X_SEC_RNGSR_IE                (0x04000000)
#define MCF548X_SEC_RNGSR_OFR               (0x08000000)
#define MCF548X_SEC_RNGSR_HALT              (0x20000000)

/* Bit definitions and macros for MCF548X_SEC_RNGISR */
#define MCF548X_SEC_RNGISR_IE               (0x00100000)
#define MCF548X_SEC_RNGISR_OFU              (0x02000000)
#define MCF548X_SEC_RNGISR_AE               (0x40000000)
#define MCF548X_SEC_RNGISR_ME               (0x80000000)

/* Bit definitions and macros for MCF548X_SEC_RNGIMR */
#define MCF548X_SEC_RNGIMR_IE               (0x00100000)
#define MCF548X_SEC_RNGIMR_OFU              (0x02000000)
#define MCF548X_SEC_RNGIMR_AE               (0x40000000)
#define MCF548X_SEC_RNGIMR_ME               (0x80000000)

/* Bit definitions and macros for MCF548X_SEC_AESRCR */
#define MCF548X_SEC_AESRCR_SR               (0x01000000)
#define MCF548X_SEC_AESRCR_MI               (0x02000000)
#define MCF548X_SEC_AESRCR_RI               (0x04000000)

/* Bit definitions and macros for MCF548X_SEC_AESSR */
#define MCF548X_SEC_AESSR_RD                (0x01000000)
#define MCF548X_SEC_AESSR_ID                (0x02000000)
#define MCF548X_SEC_AESSR_IE                (0x04000000)
#define MCF548X_SEC_AESSR_OFR               (0x08000000)
#define MCF548X_SEC_AESSR_IFW               (0x10000000)
#define MCF548X_SEC_AESSR_HALT              (0x20000000)

/* Bit definitions and macros for MCF548X_SEC_AESISR */
#define MCF548X_SEC_AESISR_DSE              (0x00010000)
#define MCF548X_SEC_AESISR_KSE              (0x00020000)
#define MCF548X_SEC_AESISR_CE               (0x00040000)
#define MCF548X_SEC_AESISR_ERE              (0x00080000)
#define MCF548X_SEC_AESISR_IE               (0x00100000)
#define MCF548X_SEC_AESISR_OFU              (0x02000000)
#define MCF548X_SEC_AESISR_IFO              (0x04000000)
#define MCF548X_SEC_AESISR_IFE              (0x10000000)
#define MCF548X_SEC_AESISR_OFE              (0x20000000)
#define MCF548X_SEC_AESISR_AE               (0x40000000)
#define MCF548X_SEC_AESISR_ME               (0x80000000)

/* Bit definitions and macros for MCF548X_SEC_AESIMR */
#define MCF548X_SEC_AESIMR_DSE              (0x00010000)
#define MCF548X_SEC_AESIMR_KSE              (0x00020000)
#define MCF548X_SEC_AESIMR_CE               (0x00040000)
#define MCF548X_SEC_AESIMR_ERE              (0x00080000)
#define MCF548X_SEC_AESIMR_IE               (0x00100000)
#define MCF548X_SEC_AESIMR_OFU              (0x02000000)
#define MCF548X_SEC_AESIMR_IFO              (0x04000000)
#define MCF548X_SEC_AESIMR_IFE              (0x10000000)
#define MCF548X_SEC_AESIMR_OFE              (0x20000000)
#define MCF548X_SEC_AESIMR_AE               (0x40000000)
#define MCF548X_SEC_AESIMR_ME               (0x80000000)


/*********************************************************************
*
* Slice Timers (SLT)
*
*********************************************************************/

/* Register read/write macros */
#define MCF548X_SLT_SLTCNT0      (*(volatile uint32_t*)(void*)(&__MBAR[0x000900]))
#define MCF548X_SLT_SCR0         (*(volatile uint32_t*)(void*)(&__MBAR[0x000904]))
#define MCF548X_SLT_SCNT0        (*(volatile uint32_t*)(void*)(&__MBAR[0x000908]))
#define MCF548X_SLT_SSR0         (*(volatile uint32_t*)(void*)(&__MBAR[0x00090C]))
#define MCF548X_SLT_SLTCNT1      (*(volatile uint32_t*)(void*)(&__MBAR[0x000910]))
#define MCF548X_SLT_SCR1         (*(volatile uint32_t*)(void*)(&__MBAR[0x000914]))
#define MCF548X_SLT_SCNT1        (*(volatile uint32_t*)(void*)(&__MBAR[0x000918]))
#define MCF548X_SLT_SSR1         (*(volatile uint32_t*)(void*)(&__MBAR[0x00091C]))
#define MCF548X_SLT_SLTCNT(x)    (*(volatile uint32_t*)(void*)(&__MBAR[0x000900U+((x)*0x010)]))
#define MCF548X_SLT_SCR(x)       (*(volatile uint32_t*)(void*)(&__MBAR[0x000904U+((x)*0x010)]))
#define MCF548X_SLT_SCNT(x)      (*(volatile uint32_t*)(void*)(&__MBAR[0x000908U+((x)*0x010)]))
#define MCF548X_SLT_SSR(x)       (*(volatile uint32_t*)(void*)(&__MBAR[0x00090CU+((x)*0x010)]))

/* Bit definitions and macros for MCF548X_SLT_SCR */
#define MCF548X_SLT_SCR_TEN    (0x01000000)
#define MCF548X_SLT_SCR_IEN    (0x02000000)
#define MCF548X_SLT_SCR_RUN    (0x04000000)

/* Bit definitions and macros for MCF548X_SLT_SSR */
#define MCF548X_SLT_SSR_ST     (0x01000000)
#define MCF548X_SLT_SSR_BE     (0x02000000)


/*********************************************************************
*
* Universal Serial Bus (USB)
*
*********************************************************************/

/* Register read/write macros */
#define MCF548X_USB_USBAISR          (*(volatile uint8_t *)(void*)(&__MBAR[0x00B000]))
#define MCF548X_USB_USBAIMR          (*(volatile uint8_t *)(void*)(&__MBAR[0x00B001]))
#define MCF548X_USB_EPINFO           (*(volatile uint8_t *)(void*)(&__MBAR[0x00B003]))
#define MCF548X_USB_CFGR             (*(volatile uint8_t *)(void*)(&__MBAR[0x00B004]))
#define MCF548X_USB_CFGAR            (*(volatile uint8_t *)(void*)(&__MBAR[0x00B005]))
#define MCF548X_USB_SPEEDR           (*(volatile uint8_t *)(void*)(&__MBAR[0x00B006]))
#define MCF548X_USB_FRMNUMR          (*(volatile uint16_t*)(void*)(&__MBAR[0x00B00E]))
#define MCF548X_USB_EPTNR            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B010]))
#define MCF548X_USB_IFUR             (*(volatile uint16_t*)(void*)(&__MBAR[0x00B014]))
#define MCF548X_USB_IFR0             (*(volatile uint16_t*)(void*)(&__MBAR[0x00B040]))
#define MCF548X_USB_IFR1             (*(volatile uint16_t*)(void*)(&__MBAR[0x00B042]))
#define MCF548X_USB_IFR2             (*(volatile uint16_t*)(void*)(&__MBAR[0x00B044]))
#define MCF548X_USB_IFR3             (*(volatile uint16_t*)(void*)(&__MBAR[0x00B046]))
#define MCF548X_USB_IFR4             (*(volatile uint16_t*)(void*)(&__MBAR[0x00B048]))
#define MCF548X_USB_IFR5             (*(volatile uint16_t*)(void*)(&__MBAR[0x00B04A]))
#define MCF548X_USB_IFR6             (*(volatile uint16_t*)(void*)(&__MBAR[0x00B04C]))
#define MCF548X_USB_IFR7             (*(volatile uint16_t*)(void*)(&__MBAR[0x00B04E]))
#define MCF548X_USB_IFR8             (*(volatile uint16_t*)(void*)(&__MBAR[0x00B050]))
#define MCF548X_USB_IFR9             (*(volatile uint16_t*)(void*)(&__MBAR[0x00B052]))
#define MCF548X_USB_IFR10            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B054]))
#define MCF548X_USB_IFR11            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B056]))
#define MCF548X_USB_IFR12            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B058]))
#define MCF548X_USB_IFR13            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B05A]))
#define MCF548X_USB_IFR14            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B05C]))
#define MCF548X_USB_IFR15            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B05E]))
#define MCF548X_USB_IFR16            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B060]))
#define MCF548X_USB_IFR17            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B062]))
#define MCF548X_USB_IFR18            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B064]))
#define MCF548X_USB_IFR19            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B066]))
#define MCF548X_USB_IFR20            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B068]))
#define MCF548X_USB_IFR21            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B06A]))
#define MCF548X_USB_IFR22            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B06C]))
#define MCF548X_USB_IFR23            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B06E]))
#define MCF548X_USB_IFR24            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B070]))
#define MCF548X_USB_IFR25            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B072]))
#define MCF548X_USB_IFR26            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B074]))
#define MCF548X_USB_IFR27            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B076]))
#define MCF548X_USB_IFR28            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B078]))
#define MCF548X_USB_IFR29            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B07A]))
#define MCF548X_USB_IFR30            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B07C]))
#define MCF548X_USB_IFR31            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B07E]))
#define MCF548X_USB_IFRn(x)          (*(volatile uint16_t*)(void*)(&__MBAR[0x00B040U+((x)*0x002)]))
#define MCF548X_USB_PPCNT            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B080]))
#define MCF548X_USB_DPCNT            (*(volatile uint16_t*)(void*)(&__MBAR[0x00B082]))
#define MCF548X_USB_CRCECNT          (*(volatile uint16_t*)(void*)(&__MBAR[0x00B084]))
#define MCF548X_USB_BSECNT           (*(volatile uint16_t*)(void*)(&__MBAR[0x00B086]))
#define MCF548X_USB_PIDECNT          (*(volatile uint16_t*)(void*)(&__MBAR[0x00B088]))
#define MCF548X_USB_FRMECNT          (*(volatile uint16_t*)(void*)(&__MBAR[0x00B08A]))
#define MCF548X_USB_TXPCNT           (*(volatile uint16_t*)(void*)(&__MBAR[0x00B08C]))
#define MCF548X_USB_CNTOVR           (*(volatile uint8_t *)(void*)(&__MBAR[0x00B08E]))
#define MCF548X_USB_EP0ACR           (*(volatile uint8_t *)(void*)(&__MBAR[0x00B101]))
#define MCF548X_USB_EP0MPSR          (*(volatile uint16_t*)(void*)(&__MBAR[0x00B102]))
#define MCF548X_USB_EP0IFR           (*(volatile uint8_t *)(void*)(&__MBAR[0x00B104]))
#define MCF548X_USB_EP0SR            (*(volatile uint8_t *)(void*)(&__MBAR[0x00B105]))
#define MCF548X_USB_BMRTR            (*(volatile uint8_t *)(void*)(&__MBAR[0x00B106]))
#define MCF548X_USB_BRTR             (*(volatile uint8_t *)(void*)(&__MBAR[0x00B107]))
#define MCF548X_USB_WVALUER          (*(volatile uint16_t*)(void*)(&__MBAR[0x00B108]))
#define MCF548X_USB_WINDEXR          (*(volatile uint16_t*)(void*)(&__MBAR[0x00B10A]))
#define MCF548X_USB_WLENGTH          (*(volatile uint16_t*)(void*)(&__MBAR[0x00B10C]))
#define MCF548X_USB_EP1OUTACR        (*(volatile uint8_t *)(void*)(&__MBAR[0x00B131]))
#define MCF548X_USB_EP2OUTACR        (*(volatile uint8_t *)(void*)(&__MBAR[0x00B161]))
#define MCF548X_USB_EP3OUTACR        (*(volatile uint8_t *)(void*)(&__MBAR[0x00B191]))
#define MCF548X_USB_EP4OUTACR        (*(volatile uint8_t *)(void*)(&__MBAR[0x00B1C1]))
#define MCF548X_USB_EP5OUTACR        (*(volatile uint8_t *)(void*)(&__MBAR[0x00B1F1]))
#define MCF548X_USB_EP6OUTACR        (*(volatile uint8_t *)(void*)(&__MBAR[0x00B221]))
#define MCF548X_USB_EPnOUTACR(x)     (*(volatile uint8_t *)(void*)(&__MBAR[0x00B131U+((x)*0x030)]))
#define MCF548X_USB_EP1OUTMPSR       (*(volatile uint16_t*)(void*)(&__MBAR[0x00B132]))
#define MCF548X_USB_EP2OUTMPSR       (*(volatile uint16_t*)(void*)(&__MBAR[0x00B162]))
#define MCF548X_USB_EP3OUTMPSR       (*(volatile uint16_t*)(void*)(&__MBAR[0x00B192]))
#define MCF548X_USB_EP4OUTMPSR       (*(volatile uint16_t*)(void*)(&__MBAR[0x00B1C2]))
#define MCF548X_USB_EP5OUTMPSR       (*(volatile uint16_t*)(void*)(&__MBAR[0x00B1F2]))
#define MCF548X_USB_EP6OUTMPSR       (*(volatile uint16_t*)(void*)(&__MBAR[0x00B222]))
#define MCF548X_USB_EPnOUTMPSR(x)    (*(volatile uint16_t*)(void*)(&__MBAR[0x00B132U+((x)*0x030)]))
#define MCF548X_USB_EP1OUTIFR        (*(volatile uint8_t *)(void*)(&__MBAR[0x00B134]))
#define MCF548X_USB_EP2OUTIFR        (*(volatile uint8_t *)(void*)(&__MBAR[0x00B164]))
#define MCF548X_USB_EP3OUTIFR        (*(volatile uint8_t *)(void*)(&__MBAR[0x00B194]))
#define MCF548X_USB_EP4OUTIFR        (*(volatile uint8_t *)(void*)(&__MBAR[0x00B1C4]))
#define MCF548X_USB_EP5OUTIFR        (*(volatile uint8_t *)(void*)(&__MBAR[0x00B1F4]))
#define MCF548X_USB_EP6OUTIFR        (*(volatile uint8_t *)(void*)(&__MBAR[0x00B224]))
#define MCF548X_USB_EPnOUTIFR(x)     (*(volatile uint8_t *)(void*)(&__MBAR[0x00B134U+((x)*0x030)]))
#define MCF548X_USB_EP1OUTSR         (*(volatile uint8_t *)(void*)(&__MBAR[0x00B135]))
#define MCF548X_USB_EP2OUTSR         (*(volatile uint8_t *)(void*)(&__MBAR[0x00B165]))
#define MCF548X_USB_EP3OUTSR         (*(volatile uint8_t *)(void*)(&__MBAR[0x00B195]))
#define MCF548X_USB_EP4OUTSR         (*(volatile uint8_t *)(void*)(&__MBAR[0x00B1C5]))
#define MCF548X_USB_EP5OUTSR         (*(volatile uint8_t *)(void*)(&__MBAR[0x00B1F5]))
#define MCF548X_USB_EP6OUTSR         (*(volatile uint8_t *)(void*)(&__MBAR[0x00B225]))
#define MCF548X_USB_EPnOUTSR(x)      (*(volatile uint8_t *)(void*)(&__MBAR[0x00B135U+((x)*0x030)]))
#define MCF548X_USB_EP1OUTSFR        (*(volatile uint16_t*)(void*)(&__MBAR[0x00B13E]))
#define MCF548X_USB_EP2OUTSFR        (*(volatile uint16_t*)(void*)(&__MBAR[0x00B16E]))
#define MCF548X_USB_EP3OUTSFR        (*(volatile uint16_t*)(void*)(&__MBAR[0x00B19E]))
#define MCF548X_USB_EP4OUTSFR        (*(volatile uint16_t*)(void*)(&__MBAR[0x00B1CE]))
#define MCF548X_USB_EP5OUTSFR        (*(volatile uint16_t*)(void*)(&__MBAR[0x00B1FE]))
#define MCF548X_USB_EP6OUTSFR        (*(volatile uint16_t*)(void*)(&__MBAR[0x00B22E]))
#define MCF548X_USB_EPnOUTSFR(x)     (*(volatile uint16_t*)(void*)(&__MBAR[0x00B13EU+((x)*0x030)]))
#define MCF548X_USB_EP1INACR         (*(volatile uint8_t *)(void*)(&__MBAR[0x00B149]))
#define MCF548X_USB_EP2INACR         (*(volatile uint8_t *)(void*)(&__MBAR[0x00B179]))
#define MCF548X_USB_EP3INACR         (*(volatile uint8_t *)(void*)(&__MBAR[0x00B1A9]))
#define MCF548X_USB_EP4INACR         (*(volatile uint8_t *)(void*)(&__MBAR[0x00B1D9]))
#define MCF548X_USB_EP5INACR         (*(volatile uint8_t *)(void*)(&__MBAR[0x00B209]))
#define MCF548X_USB_EP6INACR         (*(volatile uint8_t *)(void*)(&__MBAR[0x00B239]))
#define MCF548X_USB_EPnINACR(x)      (*(volatile uint8_t *)(void*)(&__MBAR[0x00B149U+((x)*0x030)]))
#define MCF548X_USB_EP1INMPSR        (*(volatile uint16_t*)(void*)(&__MBAR[0x00B14A]))
#define MCF548X_USB_EP2INMPSR        (*(volatile uint16_t*)(void*)(&__MBAR[0x00B17A]))
#define MCF548X_USB_EP3INMPSR        (*(volatile uint16_t*)(void*)(&__MBAR[0x00B1AA]))
#define MCF548X_USB_EP4INMPSR        (*(volatile uint16_t*)(void*)(&__MBAR[0x00B1DA]))
#define MCF548X_USB_EP5INMPSR        (*(volatile uint16_t*)(void*)(&__MBAR[0x00B20A]))
#define MCF548X_USB_EP6INMPSR        (*(volatile uint16_t*)(void*)(&__MBAR[0x00B23A]))
#define MCF548X_USB_EPnINMPSR(x)     (*(volatile uint16_t*)(void*)(&__MBAR[0x00B14AU+((x)*0x030)]))
#define MCF548X_USB_EP1INIFR         (*(volatile uint8_t *)(void*)(&__MBAR[0x00B14C]))
#define MCF548X_USB_EP2INIFR         (*(volatile uint8_t *)(void*)(&__MBAR[0x00B17C]))
#define MCF548X_USB_EP3INIFR         (*(volatile uint8_t *)(void*)(&__MBAR[0x00B1AC]))
#define MCF548X_USB_EP4INIFR         (*(volatile uint8_t *)(void*)(&__MBAR[0x00B1DC]))
#define MCF548X_USB_EP5INIFR         (*(volatile uint8_t *)(void*)(&__MBAR[0x00B20C]))
#define MCF548X_USB_EP6INIFR         (*(volatile uint8_t *)(void*)(&__MBAR[0x00B23C]))
#define MCF548X_USB_EPnINIFR(x)      (*(volatile uint8_t *)(void*)(&__MBAR[0x00B14CU+((x)*0x030)]))
#define MCF548X_USB_EP1INSR          (*(volatile uint8_t *)(void*)(&__MBAR[0x00B14D]))
#define MCF548X_USB_EP2INSR          (*(volatile uint8_t *)(void*)(&__MBAR[0x00B17D]))
#define MCF548X_USB_EP3INSR          (*(volatile uint8_t *)(void*)(&__MBAR[0x00B1AD]))
#define MCF548X_USB_EP4INSR          (*(volatile uint8_t *)(void*)(&__MBAR[0x00B1DD]))
#define MCF548X_USB_EP5INSR          (*(volatile uint8_t *)(void*)(&__MBAR[0x00B20D]))
#define MCF548X_USB_EP6INSR          (*(volatile uint8_t *)(void*)(&__MBAR[0x00B23D]))
#define MCF548X_USB_EPnINSR(x)       (*(volatile uint8_t *)(void*)(&__MBAR[0x00B14DU+((x)*0x030)]))
#define MCF548X_USB_EP1INSFR         (*(volatile uint16_t*)(void*)(&__MBAR[0x00B15A]))
#define MCF548X_USB_EP2INSFR         (*(volatile uint16_t*)(void*)(&__MBAR[0x00B18A]))
#define MCF548X_USB_EP3INSFR         (*(volatile uint16_t*)(void*)(&__MBAR[0x00B1BA]))
#define MCF548X_USB_EP4INSFR         (*(volatile uint16_t*)(void*)(&__MBAR[0x00B1EA]))
#define MCF548X_USB_EP5INSFR         (*(volatile uint16_t*)(void*)(&__MBAR[0x00B21A]))
#define MCF548X_USB_EP6INSFR         (*(volatile uint16_t*)(void*)(&__MBAR[0x00B24A]))
#define MCF548X_USB_EPnINSFR(x)      (*(volatile uint16_t*)(void*)(&__MBAR[0x00B15AU+((x)*0x030)]))
#define MCF548X_USB_USBSR            (*(volatile uint32_t*)(void*)(&__MBAR[0x00B400]))
#define MCF548X_USB_USBCR            (*(volatile uint32_t*)(void*)(&__MBAR[0x00B404]))
#define MCF548X_USB_DRAMCR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B408]))
#define MCF548X_USB_DRAMDR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B40C]))
#define MCF548X_USB_USBISR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B410]))
#define MCF548X_USB_USBIMR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B414]))
#define MCF548X_USB_EP0STAT          (*(volatile uint32_t*)(void*)(&__MBAR[0x00B440]))
#define MCF548X_USB_EP1STAT          (*(volatile uint32_t*)(void*)(&__MBAR[0x00B470]))
#define MCF548X_USB_EP2STAT          (*(volatile uint32_t*)(void*)(&__MBAR[0x00B4A0]))
#define MCF548X_USB_EP3STAT          (*(volatile uint32_t*)(void*)(&__MBAR[0x00B4D0]))
#define MCF548X_USB_EP4STAT          (*(volatile uint32_t*)(void*)(&__MBAR[0x00B500]))
#define MCF548X_USB_EP5STAT          (*(volatile uint32_t*)(void*)(&__MBAR[0x00B530]))
#define MCF548X_USB_EP6STAT          (*(volatile uint32_t*)(void*)(&__MBAR[0x00B560]))
#define MCF548X_USB_EPnSTAT(x)       (*(volatile uint32_t*)(void*)(&__MBAR[0x00B440U+((x)*0x030)]))
#define MCF548X_USB_EP0ISR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B444]))
#define MCF548X_USB_EP1ISR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B474]))
#define MCF548X_USB_EP2ISR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B4A4]))
#define MCF548X_USB_EP3ISR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B4D4]))
#define MCF548X_USB_EP4ISR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B504]))
#define MCF548X_USB_EP5ISR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B534]))
#define MCF548X_USB_EP6ISR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B564]))
#define MCF548X_USB_EPnISR(x)        (*(volatile uint32_t*)(void*)(&__MBAR[0x00B444U+((x)*0x030)]))
#define MCF548X_USB_EP0IMR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B448]))
#define MCF548X_USB_EP1IMR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B478]))
#define MCF548X_USB_EP2IMR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B4A8]))
#define MCF548X_USB_EP3IMR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B4D8]))
#define MCF548X_USB_EP4IMR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B508]))
#define MCF548X_USB_EP5IMR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B538]))
#define MCF548X_USB_EP6IMR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B568]))
#define MCF548X_USB_EPnIMR(x)        (*(volatile uint32_t*)(void*)(&__MBAR[0x00B448U+((x)*0x030)]))
#define MCF548X_USB_EP0FRCFGR        (*(volatile uint32_t*)(void*)(&__MBAR[0x00B44C]))
#define MCF548X_USB_EP1FRCFGR        (*(volatile uint32_t*)(void*)(&__MBAR[0x00B47C]))
#define MCF548X_USB_EP2FRCFGR        (*(volatile uint32_t*)(void*)(&__MBAR[0x00B4AC]))
#define MCF548X_USB_EP3FRCFGR        (*(volatile uint32_t*)(void*)(&__MBAR[0x00B4DC]))
#define MCF548X_USB_EP4FRCFGR        (*(volatile uint32_t*)(void*)(&__MBAR[0x00B50C]))
#define MCF548X_USB_EP5FRCFGR        (*(volatile uint32_t*)(void*)(&__MBAR[0x00B53C]))
#define MCF548X_USB_EP6FRCFGR        (*(volatile uint32_t*)(void*)(&__MBAR[0x00B56C]))
#define MCF548X_USB_EPnFRCFGR(x)     (*(volatile uint32_t*)(void*)(&__MBAR[0x00B44CU+((x)*0x030)]))
#define MCF548X_USB_EP0FDR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B450]))
#define MCF548X_USB_EP1FDR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B480]))
#define MCF548X_USB_EP2FDR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B4B0]))
#define MCF548X_USB_EP3FDR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B4E0]))
#define MCF548X_USB_EP4FDR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B510]))
#define MCF548X_USB_EP5FDR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B540]))
#define MCF548X_USB_EP6FDR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B570]))
#define MCF548X_USB_EPnFDR(x)        (*(volatile uint32_t*)(void*)(&__MBAR[0x00B450U+((x)*0x030)]))
#define MCF548X_USB_EP0FSR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B454]))
#define MCF548X_USB_EP1FSR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B484]))
#define MCF548X_USB_EP2FSR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B4B4]))
#define MCF548X_USB_EP3FSR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B4E4]))
#define MCF548X_USB_EP4FSR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B514]))
#define MCF548X_USB_EP5FSR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B544]))
#define MCF548X_USB_EP6FSR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B574]))
#define MCF548X_USB_EPnFSR(x)        (*(volatile uint32_t*)(void*)(&__MBAR[0x00B454U+((x)*0x030)]))
#define MCF548X_USB_EP0FCR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B458]))
#define MCF548X_USB_EP1FCR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B488]))
#define MCF548X_USB_EP2FCR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B4B8]))
#define MCF548X_USB_EP3FCR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B4E8]))
#define MCF548X_USB_EP4FCR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B518]))
#define MCF548X_USB_EP5FCR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B548]))
#define MCF548X_USB_EP6FCR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B578]))
#define MCF548X_USB_EPnFCR(x)        (*(volatile uint32_t*)(void*)(&__MBAR[0x00B458U+((x)*0x030)]))
#define MCF548X_USB_EP0FAR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B45C]))
#define MCF548X_USB_EP1FAR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B48C]))
#define MCF548X_USB_EP2FAR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B4BC]))
#define MCF548X_USB_EP3FAR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B4EC]))
#define MCF548X_USB_EP4FAR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B51C]))
#define MCF548X_USB_EP5FAR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B54C]))
#define MCF548X_USB_EP6FAR           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B57C]))
#define MCF548X_USB_EPnFAR(x)        (*(volatile uint32_t*)(void*)(&__MBAR[0x00B45CU+((x)*0x030)]))
#define MCF548X_USB_EP0FRP           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B460]))
#define MCF548X_USB_EP1FRP           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B490]))
#define MCF548X_USB_EP2FRP           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B4C0]))
#define MCF548X_USB_EP3FRP           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B4F0]))
#define MCF548X_USB_EP4FRP           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B520]))
#define MCF548X_USB_EP5FRP           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B550]))
#define MCF548X_USB_EP6FRP           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B580]))
#define MCF548X_USB_EPnFRP(x)        (*(volatile uint32_t*)(void*)(&__MBAR[0x00B460U+((x)*0x030)]))
#define MCF548X_USB_EP0FWP           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B464]))
#define MCF548X_USB_EP1FWP           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B494]))
#define MCF548X_USB_EP2FWP           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B4C4]))
#define MCF548X_USB_EP3FWP           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B4F4]))
#define MCF548X_USB_EP4FWP           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B524]))
#define MCF548X_USB_EP5FWP           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B554]))
#define MCF548X_USB_EP6FWP           (*(volatile uint32_t*)(void*)(&__MBAR[0x00B584]))
#define MCF548X_USB_EPnFWP(x)        (*(volatile uint32_t*)(void*)(&__MBAR[0x00B464U+((x)*0x030)]))
#define MCF548X_USB_EP0LRFP          (*(volatile uint32_t*)(void*)(&__MBAR[0x00B468]))
#define MCF548X_USB_EP1LRFP          (*(volatile uint32_t*)(void*)(&__MBAR[0x00B498]))
#define MCF548X_USB_EP2LRFP          (*(volatile uint32_t*)(void*)(&__MBAR[0x00B4C8]))
#define MCF548X_USB_EP3LRFP          (*(volatile uint32_t*)(void*)(&__MBAR[0x00B4F8]))
#define MCF548X_USB_EP4LRFP          (*(volatile uint32_t*)(void*)(&__MBAR[0x00B528]))
#define MCF548X_USB_EP5LRFP          (*(volatile uint32_t*)(void*)(&__MBAR[0x00B558]))
#define MCF548X_USB_EP6LRFP          (*(volatile uint32_t*)(void*)(&__MBAR[0x00B588]))
#define MCF548X_USB_EPnLRFP(x)       (*(volatile uint32_t*)(void*)(&__MBAR[0x00B468U+((x)*0x030)]))
#define MCF548X_USB_EP0LWFP          (*(volatile uint32_t*)(void*)(&__MBAR[0x00B46C]))
#define MCF548X_USB_EP1LWFP          (*(volatile uint32_t*)(void*)(&__MBAR[0x00B49C]))
#define MCF548X_USB_EP2LWFP          (*(volatile uint32_t*)(void*)(&__MBAR[0x00B4CC]))
#define MCF548X_USB_EP3LWFP          (*(volatile uint32_t*)(void*)(&__MBAR[0x00B4FC]))
#define MCF548X_USB_EP4LWFP          (*(volatile uint32_t*)(void*)(&__MBAR[0x00B52C]))
#define MCF548X_USB_EP5LWFP          (*(volatile uint32_t*)(void*)(&__MBAR[0x00B55C]))
#define MCF548X_USB_EP6LWFP          (*(volatile uint32_t*)(void*)(&__MBAR[0x00B58C]))
#define MCF548X_USB_EPnLWFP(x)       (*(volatile uint32_t*)(void*)(&__MBAR[0x00B46CU+((x)*0x030)]))

/* Bit definitions and macros for MCF548X_USB_USBAISR */
#define MCF548X_USB_USBAISR_SETUP             (0x01)
#define MCF548X_USB_USBAISR_IN                (0x02)
#define MCF548X_USB_USBAISR_OUT               (0x04)
#define MCF548X_USB_USBAISR_EPHALT            (0x08)
#define MCF548X_USB_USBAISR_TRANSERR          (0x10)
#define MCF548X_USB_USBAISR_ACK               (0x20)
#define MCF548X_USB_USBAISR_CTROVFL           (0x40)
#define MCF548X_USB_USBAISR_EPSTALL           (0x80)

/* Bit definitions and macros for MCF548X_USB_USBAIMR */
#define MCF548X_USB_USBAIMR_SETUPEN           (0x01)
#define MCF548X_USB_USBAIMR_INEN              (0x02)
#define MCF548X_USB_USBAIMR_OUTEN             (0x04)
#define MCF548X_USB_USBAIMR_EPHALTEN          (0x08)
#define MCF548X_USB_USBAIMR_TRANSERREN        (0x10)
#define MCF548X_USB_USBAIMR_ACKEN             (0x20)
#define MCF548X_USB_USBAIMR_CTROVFLEN         (0x40)
#define MCF548X_USB_USBAIMR_EPSTALLEN         (0x80)

/* Bit definitions and macros for MCF548X_USB_EPINFO */
#define MCF548X_USB_EPINFO_EPDIR              (0x01)
#define MCF548X_USB_EPINFO_EPNUM(x)           (((x)&0x07)<<1)

/* Bit definitions and macros for MCF548X_USB_CFGAR */
#define MCF548X_USB_CFGAR_RESERVED            (0xA0)
#define MCF548X_USB_CFGAR_RMTWKEUP            (0xE0)

/* Bit definitions and macros for MCF548X_USB_SPEEDR */
#define MCF548X_USB_SPEEDR_HS                 (0x01)
#define MCF548X_USB_SPEEDR_FS                 (0x02)

/* Bit definitions and macros for MCF548X_USB_FRMNUMR */
#define MCF548X_USB_FRMNUMR_FRMNUM(x)         (((x)&0x0FFF)<<0)

/* Bit definitions and macros for MCF548X_USB_EPTNR */
#define MCF548X_USB_EPTNR_EP1T(x)             (((x)&0x0003)<<0)
#define MCF548X_USB_EPTNR_EP2T(x)             (((x)&0x0003)<<2)
#define MCF548X_USB_EPTNR_EP3T(x)             (((x)&0x0003)<<4)
#define MCF548X_USB_EPTNR_EP4T(x)             (((x)&0x0003)<<6)
#define MCF548X_USB_EPTNR_EP5T(x)             (((x)&0x0003)<<8)
#define MCF548X_USB_EPTNR_EP6T(x)             (((x)&0x0003)<<10)
#define MCF548X_USB_EPTNR_EPnT1               (0)
#define MCF548X_USB_EPTNR_EPnT2               (1)
#define MCF548X_USB_EPTNR_EPnT3               (2)

/* Bit definitions and macros for MCF548X_USB_IFUR */
#define MCF548X_USB_IFUR_ALTSET(x)            (((x)&0x00FF)<<0)
#define MCF548X_USB_IFUR_IFNUM(x)             (((x)&0x00FF)<<8)

/* Bit definitions and macros for MCF548X_USB_IFRn */
#define MCF548X_USB_IFRn_ALTSET(x)            (((x)&0x00FF)<<0)
#define MCF548X_USB_IFRn_IFNUM(x)             (((x)&0x00FF)<<8)

/* Bit definitions and macros for MCF548X_USB_CNTOVR */
#define MCF548X_USB_CNTOVR_PPCNT              (0x01)
#define MCF548X_USB_CNTOVR_DPCNT              (0x02)
#define MCF548X_USB_CNTOVR_CRCECNT            (0x04)
#define MCF548X_USB_CNTOVR_BSECNT             (0x08)
#define MCF548X_USB_CNTOVR_PIDECNT            (0x10)
#define MCF548X_USB_CNTOVR_FRMECNT            (0x20)
#define MCF548X_USB_CNTOVR_TXPCNT             (0x40)

/* Bit definitions and macros for MCF548X_USB_EP0ACR */
#define MCF548X_USB_EP0ACR_TTYPE(x)           (((x)&0x03)<<0)
#define MCF548X_USB_EP0ACR_TTYPE_CTRL         (0)
#define MCF548X_USB_EP0ACR_TTYPE_ISOC         (1)
#define MCF548X_USB_EP0ACR_TTYPE_BULK         (2)
#define MCF548X_USB_EP0ACR_TTYPE_INT          (3)

/* Bit definitions and macros for MCF548X_USB_EP0MPSR */
#define MCF548X_USB_EP0MPSR_MAXPKTSZ(x)       (((x)&0x07FF)<<0)
#define MCF548X_USB_EP0MPSR_ADDTRANS(x)       (((x)&0x0003)<<11)

/* Bit definitions and macros for MCF548X_USB_EP0SR */
#define MCF548X_USB_EP0SR_HALT                (0x01)
#define MCF548X_USB_EP0SR_ACTIVE              (0x02)
#define MCF548X_USB_EP0SR_PSTALL              (0x04)
#define MCF548X_USB_EP0SR_CCOMP               (0x08)
#define MCF548X_USB_EP0SR_TXZERO              (0x20)
#define MCF548X_USB_EP0SR_INT                 (0x80)

/* Bit definitions and macros for MCF548X_USB_BMRTR */
#define MCF548X_USB_BMRTR_DIR                 (0x80)
#define MCF548X_USB_BMRTR_TYPE_STANDARD       (0x00)
#define MCF548X_USB_BMRTR_TYPE_CLASS          (0x20)
#define MCF548X_USB_BMRTR_TYPE_VENDOR         (0x40)
#define MCF548X_USB_BMRTR_REC_DEVICE          (0x00)
#define MCF548X_USB_BMRTR_REC_INTERFACE       (0x01)
#define MCF548X_USB_BMRTR_REC_ENDPOINT        (0x02)
#define MCF548X_USB_BMRTR_REC_OTHER           (0x03)

/* Bit definitions and macros for MCF548X_USB_EPnOUTACR */
#define MCF548X_USB_EPnOUTACR_TTYPE(x)        (((x)&0x03)<<0)

/* Bit definitions and macros for MCF548X_USB_EPnOUTMPSR */
#define MCF548X_USB_EPnOUTMPSR_MAXPKTSZ(x)    (((x)&0x07FF)<<0)
#define MCF548X_USB_EPnOUTMPSR_ADDTRANS(x)    (((x)&0x0003)<<11)

/* Bit definitions and macros for MCF548X_USB_EPnOUTSR */
#define MCF548X_USB_EPnOUTSR_HALT             (0x01)
#define MCF548X_USB_EPnOUTSR_ACTIVE           (0x02)
#define MCF548X_USB_EPnOUTSR_PSTALL           (0x04)
#define MCF548X_USB_EPnOUTSR_CCOMP            (0x08)
#define MCF548X_USB_EPnOUTSR_TXZERO           (0x20)
#define MCF548X_USB_EPnOUTSR_INT              (0x80)

/* Bit definitions and macros for MCF548X_USB_EPnOUTSFR */
#define MCF548X_USB_EPnOUTSFR_FRMNUM(x)       (((x)&0x07FF)<<0)

/* Bit definitions and macros for MCF548X_USB_EPnINACR */
#define MCF548X_USB_EPnINACR_TTYPE(x)         (((x)&0x03)<<0)

/* Bit definitions and macros for MCF548X_USB_EPnINMPSR */
#define MCF548X_USB_EPnINMPSR_MAXPKTSZ(x)     (((x)&0x07FF)<<0)
#define MCF548X_USB_EPnINMPSR_ADDTRANS(x)     (((x)&0x0003)<<11)

/* Bit definitions and macros for MCF548X_USB_EPnINSR */
#define MCF548X_USB_EPnINSR_HALT              (0x01)
#define MCF548X_USB_EPnINSR_ACTIVE            (0x02)
#define MCF548X_USB_EPnINSR_PSTALL            (0x04)
#define MCF548X_USB_EPnINSR_CCOMP             (0x08)
#define MCF548X_USB_EPnINSR_TXZERO            (0x20)
#define MCF548X_USB_EPnINSR_INT               (0x80)

/* Bit definitions and macros for MCF548X_USB_EPnINSFR */
#define MCF548X_USB_EPnINSFR_FRMNUM(x)        (((x)&0x07FF)<<0)

/* Bit definitions and macros for MCF548X_USB_USBSR */
#define MCF548X_USB_USBSR_SUSP                (0x00000080)
#define MCF548X_USB_USBSR_ISOERREP            (0x0000000F)

/* Bit definitions and macros for MCF548X_USB_USBCR */
#define MCF548X_USB_USBCR_RESUME              (0x00000001)
#define MCF548X_USB_USBCR_APPLOCK             (0x00000002)
#define MCF548X_USB_USBCR_RST                 (0x00000004)
#define MCF548X_USB_USBCR_RAMEN               (0x00000008)
#define MCF548X_USB_USBCR_RAMSPLIT            (0x00000020)

/* Bit definitions and macros for MCF548X_USB_DRAMCR */
#define MCF548X_USB_DRAMCR_DADR(x)            (((x)&0x000003FF)<<0)
#define MCF548X_USB_DRAMCR_DSIZE(x)           (((x)&0x000007FF)<<16)
#define MCF548X_USB_DRAMCR_BSY                (0x40000000)
#define MCF548X_USB_DRAMCR_START              (0x80000000)

/* Bit definitions and macros for MCF548X_USB_DRAMDR */
#define MCF548X_USB_DRAMDR_DDAT(x)            (((x)&0x000000FF)<<0)

/* Bit definitions and macros for MCF548X_USB_USBISR */
#define MCF548X_USB_USBISR_ISOERR             (0x00000001)
#define MCF548X_USB_USBISR_FTUNLCK            (0x00000002)
#define MCF548X_USB_USBISR_SUSP               (0x00000004)
#define MCF548X_USB_USBISR_RES                (0x00000008)
#define MCF548X_USB_USBISR_UPDSOF             (0x00000010)
#define MCF548X_USB_USBISR_RSTSTOP            (0x00000020)
#define MCF548X_USB_USBISR_SOF                (0x00000040)
#define MCF548X_USB_USBISR_MSOF               (0x00000080)

/* Bit definitions and macros for MCF548X_USB_USBIMR */
#define MCF548X_USB_USBIMR_ISOERR             (0x00000001)
#define MCF548X_USB_USBIMR_FTUNLCK            (0x00000002)
#define MCF548X_USB_USBIMR_SUSP               (0x00000004)
#define MCF548X_USB_USBIMR_RES                (0x00000008)
#define MCF548X_USB_USBIMR_UPDSOF             (0x00000010)
#define MCF548X_USB_USBIMR_RSTSTOP            (0x00000020)
#define MCF548X_USB_USBIMR_SOF                (0x00000040)
#define MCF548X_USB_USBIMR_MSOF               (0x00000080)

/* Bit definitions and macros for MCF548X_USB_EPnSTAT */
#define MCF548X_USB_EPnSTAT_RST               (0x00000001)
#define MCF548X_USB_EPnSTAT_FLUSH             (0x00000002)
#define MCF548X_USB_EPnSTAT_DIR               (0x00000080)
#define MCF548X_USB_EPnSTAT_BYTECNT(x)        (((x)&0x00000FFF)<<16)

/* Bit definitions and macros for MCF548X_USB_EPnISR */
#define MCF548X_USB_EPnISR_EOF                (0x00000001)
#define MCF548X_USB_EPnISR_EOT                (0x00000004)
#define MCF548X_USB_EPnISR_FIFOLO             (0x00000010)
#define MCF548X_USB_EPnISR_FIFOHI             (0x00000020)
#define MCF548X_USB_EPnISR_ERR                (0x00000040)
#define MCF548X_USB_EPnISR_EMT                (0x00000080)
#define MCF548X_USB_EPnISR_FU                 (0x00000100)

/* Bit definitions and macros for MCF548X_USB_EPnIMR */
#define MCF548X_USB_EPnIMR_EOF                (0x00000001)
#define MCF548X_USB_EPnIMR_EOT                (0x00000004)
#define MCF548X_USB_EPnIMR_FIFOLO             (0x00000010)
#define MCF548X_USB_EPnIMR_FIFOHI             (0x00000020)
#define MCF548X_USB_EPnIMR_ERR                (0x00000040)
#define MCF548X_USB_EPnIMR_EMT                (0x00000080)
#define MCF548X_USB_EPnIMR_FU                 (0x00000100)

/* Bit definitions and macros for MCF548X_USB_EPnFRCFGR */
#define MCF548X_USB_EPnFRCFGR_DEPTH(x)        (((x)&0x00001FFF)<<0)
#define MCF548X_USB_EPnFRCFGR_BASE(x)         (((x)&0x00000FFF)<<16)

/* Bit definitions and macros for MCF548X_USB_EPnFSR */
#define MCF548X_USB_EPnFSR_EMT                (0x00010000)
#define MCF548X_USB_EPnFSR_ALRM               (0x00020000)
#define MCF548X_USB_EPnFSR_FR                 (0x00040000)
#define MCF548X_USB_EPnFSR_FU                 (0x00080000)
#define MCF548X_USB_EPnFSR_OF                 (0x00100000)
#define MCF548X_USB_EPnFSR_UF                 (0x00200000)
#define MCF548X_USB_EPnFSR_RXW                (0x00400000)
#define MCF548X_USB_EPnFSR_FAE                (0x00800000)
#define MCF548X_USB_EPnFSR_FRM(x)             (((x)&0x0000000F)<<24)
#define MCF548X_USB_EPnFSR_TXW                (0x40000000)
#define MCF548X_USB_EPnFSR_IP                 (0x80000000)

/* Bit definitions and macros for MCF548X_USB_EPnFCR */
#define MCF548X_USB_EPnFCR_COUNTER(x)         (((x)&0x0000FFFF)<<0)
#define MCF548X_USB_EPnFCR_TXWMSK             (0x00040000)
#define MCF548X_USB_EPnFCR_OFMSK              (0x00080000)
#define MCF548X_USB_EPnFCR_UFMSK              (0x00100000)
#define MCF548X_USB_EPnFCR_RXWMSK             (0x00200000)
#define MCF548X_USB_EPnFCR_FAEMSK             (0x00400000)
#define MCF548X_USB_EPnFCR_IPMSK              (0x00800000)
#define MCF548X_USB_EPnFCR_GR(x)              (((x)&0x00000007)<<24)
#define MCF548X_USB_EPnFCR_FRM                (0x08000000)
#define MCF548X_USB_EPnFCR_TMR                (0x10000000)
#define MCF548X_USB_EPnFCR_WFR                (0x20000000)
#define MCF548X_USB_EPnFCR_SHAD               (0x80000000)

/* Bit definitions and macros for MCF548X_USB_EPnFAR */
#define MCF548X_USB_EPnFAR_ALRMP(x)           (((x)&0x00000FFF)<<0)

/* Bit definitions and macros for MCF548X_USB_EPnFRP */
#define MCF548X_USB_EPnFRP_RP(x)              (((x)&0x00000FFF)<<0)

/* Bit definitions and macros for MCF548X_USB_EPnFWP */
#define MCF548X_USB_EPnFWP_WP(x)              (((x)&0x00000FFF)<<0)

/* Bit definitions and macros for MCF548X_USB_EPnLRFP */
#define MCF548X_USB_EPnLRFP_LRFP(x)           (((x)&0x00000FFF)<<0)

/* Bit definitions and macros for MCF548X_USB_EPnLWFP */
#define MCF548X_USB_EPnLWFP_LWFP(x)           (((x)&0x00000FFF)<<0)


/*********************************************************************
*
* Programmable Serial Controller (PSC)
*
*********************************************************************/

/* Register read/write macros */
#define MCF548X_PSC_MR0            (*(volatile uint8_t *)(void*)(&__MBAR[0x008600]))
#define MCF548X_PSC_SR0            (*(volatile uint16_t*)(void*)(&__MBAR[0x008604]))
#define MCF548X_PSC_CSR0           (*(volatile uint8_t *)(void*)(&__MBAR[0x008604]))
#define MCF548X_PSC_CR0            (*(volatile uint8_t *)(void*)(&__MBAR[0x008608]))
#define MCF548X_PSC_RB0            (*(volatile uint32_t*)(void*)(&__MBAR[0x00860C]))
#define MCF548X_PSC_TB0            (*(volatile uint32_t*)(void*)(&__MBAR[0x00860C]))
#define MCF548X_PSC_TB_8BIT0       (*(volatile uint32_t*)(void*)(&__MBAR[0x00860C]))
#define MCF548X_PSC_TB_16BIT0      (*(volatile uint32_t*)(void*)(&__MBAR[0x00860C]))
#define MCF548X_PSC_TB_AC970       (*(volatile uint32_t*)(void*)(&__MBAR[0x00860C]))
#define MCF548X_PSC_IPCR0          (*(volatile uint8_t *)(void*)(&__MBAR[0x008610]))
#define MCF548X_PSC_ACR0           (*(volatile uint8_t *)(void*)(&__MBAR[0x008610]))
#define MCF548X_PSC_ISR0           (*(volatile uint16_t*)(void*)(&__MBAR[0x008614]))
#define MCF548X_PSC_IMR0           (*(volatile uint16_t*)(void*)(&__MBAR[0x008614]))
#define MCF548X_PSC_CTUR0          (*(volatile uint8_t *)(void*)(&__MBAR[0x008618]))
#define MCF548X_PSC_CTLR0          (*(volatile uint8_t *)(void*)(&__MBAR[0x00861C]))
#define MCF548X_PSC_IP0            (*(volatile uint8_t *)(void*)(&__MBAR[0x008634]))
#define MCF548X_PSC_OPSET0         (*(volatile uint8_t *)(void*)(&__MBAR[0x008638]))
#define MCF548X_PSC_OPRESET0       (*(volatile uint8_t *)(void*)(&__MBAR[0x00863C]))
#define MCF548X_PSC_SICR0          (*(volatile uint8_t *)(void*)(&__MBAR[0x008640]))
#define MCF548X_PSC_IRCR10         (*(volatile uint8_t *)(void*)(&__MBAR[0x008644]))
#define MCF548X_PSC_IRCR20         (*(volatile uint8_t *)(void*)(&__MBAR[0x008648]))
#define MCF548X_PSC_IRSDR0         (*(volatile uint8_t *)(void*)(&__MBAR[0x00864C]))
#define MCF548X_PSC_IRMDR0         (*(volatile uint8_t *)(void*)(&__MBAR[0x008650]))
#define MCF548X_PSC_IRFDR0         (*(volatile uint8_t *)(void*)(&__MBAR[0x008654]))
#define MCF548X_PSC_RFCNT0         (*(volatile uint16_t*)(void*)(&__MBAR[0x008658]))
#define MCF548X_PSC_TFCNT0         (*(volatile uint16_t*)(void*)(&__MBAR[0x00865C]))
#define MCF548X_PSC_RFSR0          (*(volatile uint16_t*)(void*)(&__MBAR[0x008664]))
#define MCF548X_PSC_TFSR0          (*(volatile uint16_t*)(void*)(&__MBAR[0x008684]))
#define MCF548X_PSC_RFCR0          (*(volatile uint32_t*)(void*)(&__MBAR[0x008668]))
#define MCF548X_PSC_TFCR0          (*(volatile uint32_t*)(void*)(&__MBAR[0x008688]))
#define MCF548X_PSC_RFAR0          (*(volatile uint16_t*)(void*)(&__MBAR[0x00866E]))
#define MCF548X_PSC_TFAR0          (*(volatile uint16_t*)(void*)(&__MBAR[0x00868E]))
#define MCF548X_PSC_RFRP0          (*(volatile uint16_t*)(void*)(&__MBAR[0x008672]))
#define MCF548X_PSC_TFRP0          (*(volatile uint16_t*)(void*)(&__MBAR[0x008692]))
#define MCF548X_PSC_RFWP0          (*(volatile uint16_t*)(void*)(&__MBAR[0x008676]))
#define MCF548X_PSC_TFWP0          (*(volatile uint16_t*)(void*)(&__MBAR[0x008696]))
#define MCF548X_PSC_RLRFP0         (*(volatile uint16_t*)(void*)(&__MBAR[0x00867A]))
#define MCF548X_PSC_TLRFP0         (*(volatile uint16_t*)(void*)(&__MBAR[0x00869A]))
#define MCF548X_PSC_RLWFP0         (*(volatile uint16_t*)(void*)(&__MBAR[0x00867E]))
#define MCF548X_PSC_TLWFP0         (*(volatile uint16_t*)(void*)(&__MBAR[0x00869E]))
#define MCF548X_PSC_MR1            (*(volatile uint8_t *)(void*)(&__MBAR[0x008700]))
#define MCF548X_PSC_SR1            (*(volatile uint16_t*)(void*)(&__MBAR[0x008704]))
#define MCF548X_PSC_CSR1           (*(volatile uint8_t *)(void*)(&__MBAR[0x008704]))
#define MCF548X_PSC_CR1            (*(volatile uint8_t *)(void*)(&__MBAR[0x008708]))
#define MCF548X_PSC_RB1            (*(volatile uint32_t*)(void*)(&__MBAR[0x00870C]))
#define MCF548X_PSC_TB1            (*(volatile uint32_t*)(void*)(&__MBAR[0x00870C]))
#define MCF548X_PSC_TB_8BIT1       (*(volatile uint32_t*)(void*)(&__MBAR[0x00870C]))
#define MCF548X_PSC_TB_16BIT1      (*(volatile uint32_t*)(void*)(&__MBAR[0x00870C]))
#define MCF548X_PSC_TB_AC971       (*(volatile uint32_t*)(void*)(&__MBAR[0x00870C]))
#define MCF548X_PSC_IPCR1          (*(volatile uint8_t *)(void*)(&__MBAR[0x008710]))
#define MCF548X_PSC_ACR1           (*(volatile uint8_t *)(void*)(&__MBAR[0x008710]))
#define MCF548X_PSC_ISR1           (*(volatile uint16_t*)(void*)(&__MBAR[0x008714]))
#define MCF548X_PSC_IMR1           (*(volatile uint16_t*)(void*)(&__MBAR[0x008714]))
#define MCF548X_PSC_CTUR1          (*(volatile uint8_t *)(void*)(&__MBAR[0x008718]))
#define MCF548X_PSC_CTLR1          (*(volatile uint8_t *)(void*)(&__MBAR[0x00871C]))
#define MCF548X_PSC_IP1            (*(volatile uint8_t *)(void*)(&__MBAR[0x008734]))
#define MCF548X_PSC_OPSET1         (*(volatile uint8_t *)(void*)(&__MBAR[0x008738]))
#define MCF548X_PSC_OPRESET1       (*(volatile uint8_t *)(void*)(&__MBAR[0x00873C]))
#define MCF548X_PSC_SICR1          (*(volatile uint8_t *)(void*)(&__MBAR[0x008740]))
#define MCF548X_PSC_IRCR11         (*(volatile uint8_t *)(void*)(&__MBAR[0x008744]))
#define MCF548X_PSC_IRCR21         (*(volatile uint8_t *)(void*)(&__MBAR[0x008748]))
#define MCF548X_PSC_IRSDR1         (*(volatile uint8_t *)(void*)(&__MBAR[0x00874C]))
#define MCF548X_PSC_IRMDR1         (*(volatile uint8_t *)(void*)(&__MBAR[0x008750]))
#define MCF548X_PSC_IRFDR1         (*(volatile uint8_t *)(void*)(&__MBAR[0x008754]))
#define MCF548X_PSC_RFCNT1         (*(volatile uint16_t*)(void*)(&__MBAR[0x008758]))
#define MCF548X_PSC_TFCNT1         (*(volatile uint16_t*)(void*)(&__MBAR[0x00875C]))
#define MCF548X_PSC_RFSR1          (*(volatile uint16_t*)(void*)(&__MBAR[0x008764]))
#define MCF548X_PSC_TFSR1          (*(volatile uint16_t*)(void*)(&__MBAR[0x008784]))
#define MCF548X_PSC_RFCR1          (*(volatile uint32_t*)(void*)(&__MBAR[0x008768]))
#define MCF548X_PSC_TFCR1          (*(volatile uint32_t*)(void*)(&__MBAR[0x008788]))
#define MCF548X_PSC_RFAR1          (*(volatile uint16_t*)(void*)(&__MBAR[0x00876E]))
#define MCF548X_PSC_TFAR1          (*(volatile uint16_t*)(void*)(&__MBAR[0x00878E]))
#define MCF548X_PSC_RFRP1          (*(volatile uint16_t*)(void*)(&__MBAR[0x008772]))
#define MCF548X_PSC_TFRP1          (*(volatile uint16_t*)(void*)(&__MBAR[0x008792]))
#define MCF548X_PSC_RFWP1          (*(volatile uint16_t*)(void*)(&__MBAR[0x008776]))
#define MCF548X_PSC_TFWP1          (*(volatile uint16_t*)(void*)(&__MBAR[0x008796]))
#define MCF548X_PSC_RLRFP1         (*(volatile uint16_t*)(void*)(&__MBAR[0x00877A]))
#define MCF548X_PSC_TLRFP1         (*(volatile uint16_t*)(void*)(&__MBAR[0x00879A]))
#define MCF548X_PSC_RLWFP1         (*(volatile uint16_t*)(void*)(&__MBAR[0x00877E]))
#define MCF548X_PSC_TLWFP1         (*(volatile uint16_t*)(void*)(&__MBAR[0x00879E]))
#define MCF548X_PSC_MR2            (*(volatile uint8_t *)(void*)(&__MBAR[0x008800]))
#define MCF548X_PSC_SR2            (*(volatile uint16_t*)(void*)(&__MBAR[0x008804]))
#define MCF548X_PSC_CSR2           (*(volatile uint8_t *)(void*)(&__MBAR[0x008804]))
#define MCF548X_PSC_CR2            (*(volatile uint8_t *)(void*)(&__MBAR[0x008808]))
#define MCF548X_PSC_RB2            (*(volatile uint32_t*)(void*)(&__MBAR[0x00880C]))
#define MCF548X_PSC_TB2            (*(volatile uint32_t*)(void*)(&__MBAR[0x00880C]))
#define MCF548X_PSC_TB_8BIT2       (*(volatile uint32_t*)(void*)(&__MBAR[0x00880C]))
#define MCF548X_PSC_TB_16BIT2      (*(volatile uint32_t*)(void*)(&__MBAR[0x00880C]))
#define MCF548X_PSC_TB_AC972       (*(volatile uint32_t*)(void*)(&__MBAR[0x00880C]))
#define MCF548X_PSC_IPCR2          (*(volatile uint8_t *)(void*)(&__MBAR[0x008810]))
#define MCF548X_PSC_ACR2           (*(volatile uint8_t *)(void*)(&__MBAR[0x008810]))
#define MCF548X_PSC_ISR2           (*(volatile uint16_t*)(void*)(&__MBAR[0x008814]))
#define MCF548X_PSC_IMR2           (*(volatile uint16_t*)(void*)(&__MBAR[0x008814]))
#define MCF548X_PSC_CTUR2          (*(volatile uint8_t *)(void*)(&__MBAR[0x008818]))
#define MCF548X_PSC_CTLR2          (*(volatile uint8_t *)(void*)(&__MBAR[0x00881C]))
#define MCF548X_PSC_IP2            (*(volatile uint8_t *)(void*)(&__MBAR[0x008834]))
#define MCF548X_PSC_OPSET2         (*(volatile uint8_t *)(void*)(&__MBAR[0x008838]))
#define MCF548X_PSC_OPRESET2       (*(volatile uint8_t *)(void*)(&__MBAR[0x00883C]))
#define MCF548X_PSC_SICR2          (*(volatile uint8_t *)(void*)(&__MBAR[0x008840]))
#define MCF548X_PSC_IRCR12         (*(volatile uint8_t *)(void*)(&__MBAR[0x008844]))
#define MCF548X_PSC_IRCR22         (*(volatile uint8_t *)(void*)(&__MBAR[0x008848]))
#define MCF548X_PSC_IRSDR2         (*(volatile uint8_t *)(void*)(&__MBAR[0x00884C]))
#define MCF548X_PSC_IRMDR2         (*(volatile uint8_t *)(void*)(&__MBAR[0x008850]))
#define MCF548X_PSC_IRFDR2         (*(volatile uint8_t *)(void*)(&__MBAR[0x008854]))
#define MCF548X_PSC_RFCNT2         (*(volatile uint16_t*)(void*)(&__MBAR[0x008858]))
#define MCF548X_PSC_TFCNT2         (*(volatile uint16_t*)(void*)(&__MBAR[0x00885C]))
#define MCF548X_PSC_RFSR2          (*(volatile uint16_t*)(void*)(&__MBAR[0x008864]))
#define MCF548X_PSC_TFSR2          (*(volatile uint16_t*)(void*)(&__MBAR[0x008884]))
#define MCF548X_PSC_RFCR2          (*(volatile uint32_t*)(void*)(&__MBAR[0x008868]))
#define MCF548X_PSC_TFCR2          (*(volatile uint32_t*)(void*)(&__MBAR[0x008888]))
#define MCF548X_PSC_RFAR2          (*(volatile uint16_t*)(void*)(&__MBAR[0x00886E]))
#define MCF548X_PSC_TFAR2          (*(volatile uint16_t*)(void*)(&__MBAR[0x00888E]))
#define MCF548X_PSC_RFRP2          (*(volatile uint16_t*)(void*)(&__MBAR[0x008872]))
#define MCF548X_PSC_TFRP2          (*(volatile uint16_t*)(void*)(&__MBAR[0x008892]))
#define MCF548X_PSC_RFWP2          (*(volatile uint16_t*)(void*)(&__MBAR[0x008876]))
#define MCF548X_PSC_TFWP2          (*(volatile uint16_t*)(void*)(&__MBAR[0x008896]))
#define MCF548X_PSC_RLRFP2         (*(volatile uint16_t*)(void*)(&__MBAR[0x00887A]))
#define MCF548X_PSC_TLRFP2         (*(volatile uint16_t*)(void*)(&__MBAR[0x00889A]))
#define MCF548X_PSC_RLWFP2         (*(volatile uint16_t*)(void*)(&__MBAR[0x00887E]))
#define MCF548X_PSC_TLWFP2         (*(volatile uint16_t*)(void*)(&__MBAR[0x00889E]))
#define MCF548X_PSC_MR3            (*(volatile uint8_t *)(void*)(&__MBAR[0x008900]))
#define MCF548X_PSC_SR3            (*(volatile uint16_t*)(void*)(&__MBAR[0x008904]))
#define MCF548X_PSC_CSR3           (*(volatile uint8_t *)(void*)(&__MBAR[0x008904]))
#define MCF548X_PSC_CR3            (*(volatile uint8_t *)(void*)(&__MBAR[0x008908]))
#define MCF548X_PSC_RB3            (*(volatile uint32_t*)(void*)(&__MBAR[0x00890C]))
#define MCF548X_PSC_TB3            (*(volatile uint32_t*)(void*)(&__MBAR[0x00890C]))
#define MCF548X_PSC_TB_8BIT3       (*(volatile uint32_t*)(void*)(&__MBAR[0x00890C]))
#define MCF548X_PSC_TB_16BIT3      (*(volatile uint32_t*)(void*)(&__MBAR[0x00890C]))
#define MCF548X_PSC_TB_AC973       (*(volatile uint32_t*)(void*)(&__MBAR[0x00890C]))
#define MCF548X_PSC_IPCR3          (*(volatile uint8_t *)(void*)(&__MBAR[0x008910]))
#define MCF548X_PSC_ACR3           (*(volatile uint8_t *)(void*)(&__MBAR[0x008910]))
#define MCF548X_PSC_ISR3           (*(volatile uint16_t*)(void*)(&__MBAR[0x008914]))
#define MCF548X_PSC_IMR3           (*(volatile uint16_t*)(void*)(&__MBAR[0x008914]))
#define MCF548X_PSC_CTUR3          (*(volatile uint8_t *)(void*)(&__MBAR[0x008918]))
#define MCF548X_PSC_CTLR3          (*(volatile uint8_t *)(void*)(&__MBAR[0x00891C]))
#define MCF548X_PSC_IP3            (*(volatile uint8_t *)(void*)(&__MBAR[0x008934]))
#define MCF548X_PSC_OPSET3         (*(volatile uint8_t *)(void*)(&__MBAR[0x008938]))
#define MCF548X_PSC_OPRESET3       (*(volatile uint8_t *)(void*)(&__MBAR[0x00893C]))
#define MCF548X_PSC_SICR3          (*(volatile uint8_t *)(void*)(&__MBAR[0x008940]))
#define MCF548X_PSC_IRCR13         (*(volatile uint8_t *)(void*)(&__MBAR[0x008944]))
#define MCF548X_PSC_IRCR23         (*(volatile uint8_t *)(void*)(&__MBAR[0x008948]))
#define MCF548X_PSC_IRSDR3         (*(volatile uint8_t *)(void*)(&__MBAR[0x00894C]))
#define MCF548X_PSC_IRMDR3         (*(volatile uint8_t *)(void*)(&__MBAR[0x008950]))
#define MCF548X_PSC_IRFDR3         (*(volatile uint8_t *)(void*)(&__MBAR[0x008954]))
#define MCF548X_PSC_RFCNT3         (*(volatile uint16_t*)(void*)(&__MBAR[0x008958]))
#define MCF548X_PSC_TFCNT3         (*(volatile uint16_t*)(void*)(&__MBAR[0x00895C]))
#define MCF548X_PSC_RFSR3          (*(volatile uint16_t*)(void*)(&__MBAR[0x008964]))
#define MCF548X_PSC_TFSR3          (*(volatile uint16_t*)(void*)(&__MBAR[0x008984]))
#define MCF548X_PSC_RFCR3          (*(volatile uint32_t*)(void*)(&__MBAR[0x008968]))
#define MCF548X_PSC_TFCR3          (*(volatile uint32_t*)(void*)(&__MBAR[0x008988]))
#define MCF548X_PSC_RFAR3          (*(volatile uint16_t*)(void*)(&__MBAR[0x00896E]))
#define MCF548X_PSC_TFAR3          (*(volatile uint16_t*)(void*)(&__MBAR[0x00898E]))
#define MCF548X_PSC_RFRP3          (*(volatile uint16_t*)(void*)(&__MBAR[0x008972]))
#define MCF548X_PSC_TFRP3          (*(volatile uint16_t*)(void*)(&__MBAR[0x008992]))
#define MCF548X_PSC_RFWP3          (*(volatile uint16_t*)(void*)(&__MBAR[0x008976]))
#define MCF548X_PSC_TFWP3          (*(volatile uint16_t*)(void*)(&__MBAR[0x008996]))
#define MCF548X_PSC_RLRFP3         (*(volatile uint16_t*)(void*)(&__MBAR[0x00897A]))
#define MCF548X_PSC_TLRFP3         (*(volatile uint16_t*)(void*)(&__MBAR[0x00899A]))
#define MCF548X_PSC_RLWFP3         (*(volatile uint16_t*)(void*)(&__MBAR[0x00897E]))
#define MCF548X_PSC_TLWFP3         (*(volatile uint16_t*)(void*)(&__MBAR[0x00899E]))
#define MCF548X_PSC_MR(x)          (*(volatile uint8_t *)(void*)(&__MBAR[0x008600U+((x)*0x100)]))
#define MCF548X_PSC_SR(x)          (*(volatile uint16_t*)(void*)(&__MBAR[0x008604U+((x)*0x100)]))
#define MCF548X_PSC_CSR(x)         (*(volatile uint8_t *)(void*)(&__MBAR[0x008604U+((x)*0x100)]))
#define MCF548X_PSC_CR(x)          (*(volatile uint8_t *)(void*)(&__MBAR[0x008608U+((x)*0x100)]))
#define MCF548X_PSC_RB(x)          (*(volatile uint32_t*)(void*)(&__MBAR[0x00860CU+((x)*0x100)]))
#define MCF548X_PSC_TB(x)          (*(volatile uint32_t*)(void*)(&__MBAR[0x00860CU+((x)*0x100)]))
#define MCF548X_PSC_TB_8BIT(x)     (*(volatile uint32_t*)(void*)(&__MBAR[0x00860CU+((x)*0x100)]))
#define MCF548X_PSC_TB_16BIT(x)    (*(volatile uint32_t*)(void*)(&__MBAR[0x00860CU+((x)*0x100)]))
#define MCF548X_PSC_TB_AC97(x)     (*(volatile uint32_t*)(void*)(&__MBAR[0x00860CU+((x)*0x100)]))
#define MCF548X_PSC_IPCR(x)        (*(volatile uint8_t *)(void*)(&__MBAR[0x008610U+((x)*0x100)]))
#define MCF548X_PSC_ACR(x)         (*(volatile uint8_t *)(void*)(&__MBAR[0x008610U+((x)*0x100)]))
#define MCF548X_PSC_ISR(x)         (*(volatile uint16_t*)(void*)(&__MBAR[0x008614U+((x)*0x100)]))
#define MCF548X_PSC_IMR(x)         (*(volatile uint16_t*)(void*)(&__MBAR[0x008614U+((x)*0x100)]))
#define MCF548X_PSC_CTUR(x)        (*(volatile uint8_t *)(void*)(&__MBAR[0x008618U+((x)*0x100)]))
#define MCF548X_PSC_CTLR(x)        (*(volatile uint8_t *)(void*)(&__MBAR[0x00861CU+((x)*0x100)]))
#define MCF548X_PSC_IP(x)          (*(volatile uint8_t *)(void*)(&__MBAR[0x008634U+((x)*0x100)]))
#define MCF548X_PSC_OPSET(x)       (*(volatile uint8_t *)(void*)(&__MBAR[0x008638U+((x)*0x100)]))
#define MCF548X_PSC_OPRESET(x)     (*(volatile uint8_t *)(void*)(&__MBAR[0x00863CU+((x)*0x100)]))
#define MCF548X_PSC_SICR(x)        (*(volatile uint8_t *)(void*)(&__MBAR[0x008640U+((x)*0x100)]))
#define MCF548X_PSC_IRCR1(x)       (*(volatile uint8_t *)(void*)(&__MBAR[0x008644U+((x)*0x100)]))
#define MCF548X_PSC_IRCR2(x)       (*(volatile uint8_t *)(void*)(&__MBAR[0x008648U+((x)*0x100)]))
#define MCF548X_PSC_IRSDR(x)       (*(volatile uint8_t *)(void*)(&__MBAR[0x00864CU+((x)*0x100)]))
#define MCF548X_PSC_IRMDR(x)       (*(volatile uint8_t *)(void*)(&__MBAR[0x008650U+((x)*0x100)]))
#define MCF548X_PSC_IRFDR(x)       (*(volatile uint8_t *)(void*)(&__MBAR[0x008654U+((x)*0x100)]))
#define MCF548X_PSC_RFCNT(x)       (*(volatile uint16_t*)(void*)(&__MBAR[0x008658U+((x)*0x100)]))
#define MCF548X_PSC_TFCNT(x)       (*(volatile uint16_t*)(void*)(&__MBAR[0x00865CU+((x)*0x100)]))
#define MCF548X_PSC_RFSR(x)        (*(volatile uint16_t*)(void*)(&__MBAR[0x008664U+((x)*0x100)]))
#define MCF548X_PSC_TFSR(x)        (*(volatile uint16_t*)(void*)(&__MBAR[0x008684U+((x)*0x100)]))
#define MCF548X_PSC_RFCR(x)        (*(volatile uint32_t*)(void*)(&__MBAR[0x008668U+((x)*0x100)]))
#define MCF548X_PSC_TFCR(x)        (*(volatile uint32_t*)(void*)(&__MBAR[0x008688U+((x)*0x100)]))
#define MCF548X_PSC_RFAR(x)        (*(volatile uint16_t*)(void*)(&__MBAR[0x00866EU+((x)*0x100)]))
#define MCF548X_PSC_TFAR(x)        (*(volatile uint16_t*)(void*)(&__MBAR[0x00868EU+((x)*0x100)]))
#define MCF548X_PSC_RFRP(x)        (*(volatile uint16_t*)(void*)(&__MBAR[0x008672U+((x)*0x100)]))
#define MCF548X_PSC_TFRP(x)        (*(volatile uint16_t*)(void*)(&__MBAR[0x008692U+((x)*0x100)]))
#define MCF548X_PSC_RFWP(x)        (*(volatile uint16_t*)(void*)(&__MBAR[0x008676U+((x)*0x100)]))
#define MCF548X_PSC_TFWP(x)        (*(volatile uint16_t*)(void*)(&__MBAR[0x008696U+((x)*0x100)]))
#define MCF548X_PSC_RLRFP(x)       (*(volatile uint16_t*)(void*)(&__MBAR[0x00867AU+((x)*0x100)]))
#define MCF548X_PSC_TLRFP(x)       (*(volatile uint16_t*)(void*)(&__MBAR[0x00869AU+((x)*0x100)]))
#define MCF548X_PSC_RLWFP(x)       (*(volatile uint16_t*)(void*)(&__MBAR[0x00867EU+((x)*0x100)]))
#define MCF548X_PSC_TLWFP(x)       (*(volatile uint16_t*)(void*)(&__MBAR[0x00869EU+((x)*0x100)]))

/* Bit definitions and macros for MCF548X_PSC_MR */
#define MCF548X_PSC_MR_BC(x)              (((x)&0x03)<<0)
#define MCF548X_PSC_MR_PT                 (0x04)
#define MCF548X_PSC_MR_PM(x)              (((x)&0x03)<<3)
#define MCF548X_PSC_MR_ERR                (0x20)
#define MCF548X_PSC_MR_RXIRQ              (0x40)
#define MCF548X_PSC_MR_RXRTS              (0x80)
#define MCF548X_PSC_MR_SB(x)              (((x)&0x0F)<<0)
#define MCF548X_PSC_MR_TXCTS              (0x10)
#define MCF548X_PSC_MR_TXRTS              (0x20)
#define MCF548X_PSC_MR_CM(x)              (((x)&0x03)<<6)
#define MCF548X_PSC_MR_PM_MULTI_ADDR      (0x1C)
#define MCF548X_PSC_MR_PM_MULTI_DATA      (0x18)
#define MCF548X_PSC_MR_PM_NONE            (0x10)
#define MCF548X_PSC_MR_PM_FORCE_HI        (0x0C)
#define MCF548X_PSC_MR_PM_FORCE_LO        (0x08)
#define MCF548X_PSC_MR_PM_ODD             (0x04)
#define MCF548X_PSC_MR_PM_EVEN            (0x00)
#define MCF548X_PSC_MR_BC_5               (0x00)
#define MCF548X_PSC_MR_BC_6               (0x01)
#define MCF548X_PSC_MR_BC_7               (0x02)
#define MCF548X_PSC_MR_BC_8               (0x03)
#define MCF548X_PSC_MR_CM_NORMAL          (0x00)
#define MCF548X_PSC_MR_CM_ECHO            (0x40)
#define MCF548X_PSC_MR_CM_LOCAL_LOOP      (0x80)
#define MCF548X_PSC_MR_CM_REMOTE_LOOP     (0xC0)
#define MCF548X_PSC_MR_SB_STOP_BITS_1     (0x07)
#define MCF548X_PSC_MR_SB_STOP_BITS_15    (0x08)
#define MCF548X_PSC_MR_SB_STOP_BITS_2     (0x0F)

/* Bit definitions and macros for MCF548X_PSC_SR */
#define MCF548X_PSC_SR_ERR                (0x0040)
#define MCF548X_PSC_SR_CDE_DEOF           (0x0080)
#define MCF548X_PSC_SR_RXRDY              (0x0100)
#define MCF548X_PSC_SR_FU                 (0x0200)
#define MCF548X_PSC_SR_TXRDY              (0x0400)
#define MCF548X_PSC_SR_TXEMP_URERR        (0x0800)
#define MCF548X_PSC_SR_OE                 (0x1000)
#define MCF548X_PSC_SR_PE_CRCERR          (0x2000)
#define MCF548X_PSC_SR_FE_PHYERR          (0x4000)
#define MCF548X_PSC_SR_RB_NEOF            (0x8000)

/* Bit definitions and macros for MCF548X_PSC_CSR */
#define MCF548X_PSC_CSR_TCSEL(x)          (((x)&0x0F)<<0)
#define MCF548X_PSC_CSR_RCSEL(x)          (((x)&0x0F)<<4)
#define MCF548X_PSC_CSR_RCSEL_SYS_CLK     (0xD0)
#define MCF548X_PSC_CSR_RCSEL_CTM16       (0xE0)
#define MCF548X_PSC_CSR_RCSEL_CTM         (0xF0)
#define MCF548X_PSC_CSR_TCSEL_SYS_CLK     (0x0D)
#define MCF548X_PSC_CSR_TCSEL_CTM16       (0x0E)
#define MCF548X_PSC_CSR_TCSEL_CTM         (0x0F)

/* Bit definitions and macros for MCF548X_PSC_CR */
#define MCF548X_PSC_CR_RXC(x)             (((x)&0x03)<<0)
#define MCF548X_PSC_CR_TXC(x)             (((x)&0x03)<<2)
#define MCF548X_PSC_CR_MISC(x)            (((x)&0x07)<<4)
#define MCF548X_PSC_CR_NONE               (0x00)
#define MCF548X_PSC_CR_STOP_BREAK         (0x70)
#define MCF548X_PSC_CR_START_BREAK        (0x60)
#define MCF548X_PSC_CR_BKCHGINT           (0x50)
#define MCF548X_PSC_CR_RESET_ERROR        (0x40)
#define MCF548X_PSC_CR_RESET_TX           (0x30)
#define MCF548X_PSC_CR_RESET_RX           (0x20)
#define MCF548X_PSC_CR_RESET_MR           (0x10)
#define MCF548X_PSC_CR_TX_DISABLED        (0x08)
#define MCF548X_PSC_CR_TX_ENABLED         (0x04)
#define MCF548X_PSC_CR_RX_DISABLED        (0x02)
#define MCF548X_PSC_CR_RX_ENABLED         (0x01)

/* Bit definitions and macros for MCF548X_PSC_TB_8BIT */
#define MCF548X_PSC_TB_8BIT_TB3(x)        (((x)&0x000000FF)<<0)
#define MCF548X_PSC_TB_8BIT_TB2(x)        (((x)&0x000000FF)<<8)
#define MCF548X_PSC_TB_8BIT_TB1(x)        (((x)&0x000000FF)<<16)
#define MCF548X_PSC_TB_8BIT_TB0(x)        (((x)&0x000000FF)<<24)

/* Bit definitions and macros for MCF548X_PSC_TB_16BIT */
#define MCF548X_PSC_TB_16BIT_TB1(x)       (((x)&0x0000FFFF)<<0)
#define MCF548X_PSC_TB_16BIT_TB0(x)       (((x)&0x0000FFFF)<<16)

/* Bit definitions and macros for MCF548X_PSC_TB_AC97 */
#define MCF548X_PSC_TB_AC97_SOF           (0x00000800)
#define MCF548X_PSC_TB_AC97_TB(x)         (((x)&0x000FFFFF)<<12)

/* Bit definitions and macros for MCF548X_PSC_IPCR */
#define MCF548X_PSC_IPCR_RESERVED         (0x0C)
#define MCF548X_PSC_IPCR_CTS              (0x0D)
#define MCF548X_PSC_IPCR_D_CTS            (0x1C)
#define MCF548X_PSC_IPCR_SYNC             (0x8C)

/* Bit definitions and macros for MCF548X_PSC_ACR */
#define MCF548X_PSC_ACR_IEC0              (0x01)
#define MCF548X_PSC_ACR_CTMS(x)           (((x)&0x07)<<4)
#define MCF548X_PSC_ACR_BRG               (0x80)

/* Bit definitions and macros for MCF548X_PSC_ISR */
#define MCF548X_PSC_ISR_ERR               (0x0040)
#define MCF548X_PSC_ISR_DEOF              (0x0080)
#define MCF548X_PSC_ISR_TXRDY             (0x0100)
#define MCF548X_PSC_ISR_RXRDY_FU          (0x0200)
#define MCF548X_PSC_ISR_DB                (0x0400)
#define MCF548X_PSC_ISR_IPC               (0x8000)

/* Bit definitions and macros for MCF548X_PSC_IMR */
#define MCF548X_PSC_IMR_ERR               (0x0040)
#define MCF548X_PSC_IMR_DEOF              (0x0080)
#define MCF548X_PSC_IMR_TXRDY             (0x0100)
#define MCF548X_PSC_IMR_RXRDY_FU          (0x0200)
#define MCF548X_PSC_IMR_DB                (0x0400)
#define MCF548X_PSC_IMR_IPC               (0x8000)

/* Bit definitions and macros for MCF548X_PSC_IP */
#define MCF548X_PSC_IP_CTS                (0x01)
#define MCF548X_PSC_IP_TGL                (0x40)
#define MCF548X_PSC_IP_LWPR_B             (0x80)

/* Bit definitions and macros for MCF548X_PSC_OPSET */
#define MCF548X_PSC_OPSET_RTS             (0x01)

/* Bit definitions and macros for MCF548X_PSC_OPRESET */
#define MCF548X_PSC_OPRESET_RTS           (0x01)

/* Bit definitions and macros for MCF548X_PSC_SICR */
#define MCF548X_PSC_SICR_SIM(x)           (((x)&0x07)<<0)
#define MCF548X_PSC_SICR_SHDIR            (0x10)
#define MCF548X_PSC_SICR_DTS              (0x20)
#define MCF548X_PSC_SICR_AWR              (0x40)
#define MCF548X_PSC_SICR_ACRB             (0x80)
#define MCF548X_PSC_SICR_SIM_UART         (0x00)
#define MCF548X_PSC_SICR_SIM_MODEM8       (0x01)
#define MCF548X_PSC_SICR_SIM_MODEM16      (0x02)
#define MCF548X_PSC_SICR_SIM_AC97         (0x03)
#define MCF548X_PSC_SICR_SIM_SIR          (0x04)
#define MCF548X_PSC_SICR_SIM_MIR          (0x05)
#define MCF548X_PSC_SICR_SIM_FIR          (0x06)

/* Bit definitions and macros for MCF548X_PSC_IRCR1 */
#define MCF548X_PSC_IRCR1_SPUL            (0x01)
#define MCF548X_PSC_IRCR1_SIPEN           (0x02)
#define MCF548X_PSC_IRCR1_FD              (0x04)

/* Bit definitions and macros for MCF548X_PSC_IRCR2 */
#define MCF548X_PSC_IRCR2_NXTEOF          (0x01)
#define MCF548X_PSC_IRCR2_ABORT           (0x02)
#define MCF548X_PSC_IRCR2_SIPREQ          (0x04)

/* Bit definitions and macros for MCF548X_PSC_IRMDR */
#define MCF548X_PSC_IRMDR_M_FDIV(x)       (((x)&0x7F)<<0)
#define MCF548X_PSC_IRMDR_FREQ            (0x80)

/* Bit definitions and macros for MCF548X_PSC_IRFDR */
#define MCF548X_PSC_IRFDR_F_FDIV(x)       (((x)&0x0F)<<0)

/* Bit definitions and macros for MCF548X_PSC_RFCNT */
#define MCF548X_PSC_RFCNT_CNT(x)          (((x)&0x01FF)<<0)

/* Bit definitions and macros for MCF548X_PSC_TFCNT */
#define MCF548X_PSC_TFCNT_CNT(x)          (((x)&0x01FF)<<0)

/* Bit definitions and macros for MCF548X_PSC_RFSR */
#define MCF548X_PSC_RFSR_EMT              (0x0001)
#define MCF548X_PSC_RFSR_ALARM            (0x0002)
#define MCF548X_PSC_RFSR_FU               (0x0004)
#define MCF548X_PSC_RFSR_FRMRY            (0x0008)
#define MCF548X_PSC_RFSR_OF               (0x0010)
#define MCF548X_PSC_RFSR_UF               (0x0020)
#define MCF548X_PSC_RFSR_RXW              (0x0040)
#define MCF548X_PSC_RFSR_FAE              (0x0080)
#define MCF548X_PSC_RFSR_FRM(x)           (((x)&0x000F)<<8)
#define MCF548X_PSC_RFSR_TAG              (0x1000)
#define MCF548X_PSC_RFSR_TXW              (0x4000)
#define MCF548X_PSC_RFSR_IP               (0x8000)
#define MCF548X_PSC_RFSR_FRM_BYTE0        (0x0800)
#define MCF548X_PSC_RFSR_FRM_BYTE1        (0x0400)
#define MCF548X_PSC_RFSR_FRM_BYTE2        (0x0200)
#define MCF548X_PSC_RFSR_FRM_BYTE3        (0x0100)

/* Bit definitions and macros for MCF548X_PSC_TFSR */
#define MCF548X_PSC_TFSR_EMT              (0x0001)
#define MCF548X_PSC_TFSR_ALARM            (0x0002)
#define MCF548X_PSC_TFSR_FU               (0x0004)
#define MCF548X_PSC_TFSR_FRMRY            (0x0008)
#define MCF548X_PSC_TFSR_OF               (0x0010)
#define MCF548X_PSC_TFSR_UF               (0x0020)
#define MCF548X_PSC_TFSR_RXW              (0x0040)
#define MCF548X_PSC_TFSR_FAE              (0x0080)
#define MCF548X_PSC_TFSR_FRM(x)           (((x)&0x000F)<<8)
#define MCF548X_PSC_TFSR_TAG              (0x1000)
#define MCF548X_PSC_TFSR_TXW              (0x4000)
#define MCF548X_PSC_TFSR_IP               (0x8000)
#define MCF548X_PSC_TFSR_FRM_BYTE0        (0x0800)
#define MCF548X_PSC_TFSR_FRM_BYTE1        (0x0400)
#define MCF548X_PSC_TFSR_FRM_BYTE2        (0x0200)
#define MCF548X_PSC_TFSR_FRM_BYTE3        (0x0100)

/* Bit definitions and macros for MCF548X_PSC_RFCR */
#define MCF548X_PSC_RFCR_CNTR(x)          (((x)&0x0000FFFF)<<0)
#define MCF548X_PSC_RFCR_TXW_MSK          (0x00040000)
#define MCF548X_PSC_RFCR_OF_MSK           (0x00080000)
#define MCF548X_PSC_RFCR_UF_MSK           (0x00100000)
#define MCF548X_PSC_RFCR_RXW_MSK          (0x00200000)
#define MCF548X_PSC_RFCR_FAE_MSK          (0x00400000)
#define MCF548X_PSC_RFCR_IP_MSK           (0x00800000)
#define MCF548X_PSC_RFCR_GR(x)            (((x)&0x00000007)<<24)
#define MCF548X_PSC_RFCR_FRMEN            (0x08000000)
#define MCF548X_PSC_RFCR_TIMER            (0x10000000)
#define MCF548X_PSC_RFCR_WRITETAG         (0x20000000)
#define MCF548X_PSC_RFCR_SHADOW           (0x80000000)

/* Bit definitions and macros for MCF548X_PSC_TFCR */
#define MCF548X_PSC_TFCR_CNTR(x)          (((x)&0x0000FFFF)<<0)
#define MCF548X_PSC_TFCR_TXW_MSK          (0x00040000)
#define MCF548X_PSC_TFCR_OF_MSK           (0x00080000)
#define MCF548X_PSC_TFCR_UF_MSK           (0x00100000)
#define MCF548X_PSC_TFCR_RXW_MSK          (0x00200000)
#define MCF548X_PSC_TFCR_FAE_MSK          (0x00400000)
#define MCF548X_PSC_TFCR_IP_MSK           (0x00800000)
#define MCF548X_PSC_TFCR_GR(x)            (((x)&0x00000007)<<24)
#define MCF548X_PSC_TFCR_FRMEN            (0x08000000)
#define MCF548X_PSC_TFCR_TIMER            (0x10000000)
#define MCF548X_PSC_TFCR_WRITETAG         (0x20000000)
#define MCF548X_PSC_TFCR_SHADOW           (0x80000000)

/* Bit definitions and macros for MCF548X_PSC_RFAR */
#define MCF548X_PSC_RFAR_ALARM(x)         (((x)&0x01FF)<<0)

/* Bit definitions and macros for MCF548X_PSC_TFAR */
#define MCF548X_PSC_TFAR_ALARM(x)         (((x)&0x01FF)<<0)

/* Bit definitions and macros for MCF548X_PSC_RFRP */
#define MCF548X_PSC_RFRP_READ(x)          (((x)&0x01FF)<<0)

/* Bit definitions and macros for MCF548X_PSC_TFRP */
#define MCF548X_PSC_TFRP_READ(x)          (((x)&0x01FF)<<0)

/* Bit definitions and macros for MCF548X_PSC_RFWP */
#define MCF548X_PSC_RFWP_WRITE(x)         (((x)&0x01FF)<<0)

/* Bit definitions and macros for MCF548X_PSC_TFWP */
#define MCF548X_PSC_TFWP_WRITE(x)         (((x)&0x01FF)<<0)

/* Bit definitions and macros for MCF548X_PSC_RLRFP */
#define MCF548X_PSC_RLRFP_LFP(x)          (((x)&0x01FF)<<0)

/* Bit definitions and macros for MCF548X_PSC_TLRFP */
#define MCF548X_PSC_TLRFP_LFP(x)          (((x)&0x01FF)<<0)

/* Bit definitions and macros for MCF548X_PSC_RLWFP */
#define MCF548X_PSC_RLWFP_LFP(x)          (((x)&0x01FF)<<0)

/* Bit definitions and macros for MCF548X_PSC_TLWFP */
#define MCF548X_PSC_TLWFP_LFP(x)          (((x)&0x01FF)<<0)


/*********************************************************************
*
* 32KByte System SRAM (SRAM)
*
*********************************************************************/

/* Register read/write macros */
#define MCF548X_SRAM_SSCR       (*(volatile uint32_t*)(void*)(&__MBAR[0x01FFC0]))
#define MCF548X_SRAM_TCCR       (*(volatile uint32_t*)(void*)(&__MBAR[0x01FFC4]))
#define MCF548X_SRAM_TCCRDR     (*(volatile uint32_t*)(void*)(&__MBAR[0x01FFC8]))
#define MCF548X_SRAM_TCCRDW     (*(volatile uint32_t*)(void*)(&__MBAR[0x01FFCC]))
#define MCF548X_SRAM_TCCRSEC    (*(volatile uint32_t*)(void*)(&__MBAR[0x01FFD0]))

/* Bit definitions and macros for MCF548X_SRAM_SSCR */
#define MCF548X_SRAM_SSCR_INLV              (0x00010000)

/* Bit definitions and macros for MCF548X_SRAM_TCCR */
#define MCF548X_SRAM_TCCR_BANK0_TC(x)       (((x)&0x0000000F)<<0)
#define MCF548X_SRAM_TCCR_BANK1_TC(x)       (((x)&0x0000000F)<<8)
#define MCF548X_SRAM_TCCR_BANK2_TC(x)       (((x)&0x0000000F)<<16)
#define MCF548X_SRAM_TCCR_BANK3_TC(x)       (((x)&0x0000000F)<<24)

/* Bit definitions and macros for MCF548X_SRAM_TCCRDR */
#define MCF548X_SRAM_TCCRDR_BANK0_TC(x)     (((x)&0x0000000F)<<0)
#define MCF548X_SRAM_TCCRDR_BANK1_TC(x)     (((x)&0x0000000F)<<8)
#define MCF548X_SRAM_TCCRDR_BANK2_TC(x)     (((x)&0x0000000F)<<16)
#define MCF548X_SRAM_TCCRDR_BANK3_TC(x)     (((x)&0x0000000F)<<24)

/* Bit definitions and macros for MCF548X_SRAM_TCCRDW */
#define MCF548X_SRAM_TCCRDW_BANK0_TC(x)     (((x)&0x0000000F)<<0)
#define MCF548X_SRAM_TCCRDW_BANK1_TC(x)     (((x)&0x0000000F)<<8)
#define MCF548X_SRAM_TCCRDW_BANK2_TC(x)     (((x)&0x0000000F)<<16)
#define MCF548X_SRAM_TCCRDW_BANK3_TC(x)     (((x)&0x0000000F)<<24)

/* Bit definitions and macros for MCF548X_SRAM_TCCRSEC */
#define MCF548X_SRAM_TCCRSEC_BANK0_TC(x)    (((x)&0x0000000F)<<0)
#define MCF548X_SRAM_TCCRSEC_BANK1_TC(x)    (((x)&0x0000000F)<<8)
#define MCF548X_SRAM_TCCRSEC_BANK2_TC(x)    (((x)&0x0000000F)<<16)
#define MCF548X_SRAM_TCCRSEC_BANK3_TC(x)    (((x)&0x0000000F)<<24)


/*********************************************************************
*
* PCI Bus Controller (PCI)
*
*********************************************************************/

/* Register read/write macros */
#define MCF548X_PCI_PCIIDR        (*(volatile uint32_t*)(void*)(&__MBAR[0x000B00]))
#define MCF548X_PCI_PCISCR        (*(volatile uint32_t*)(void*)(&__MBAR[0x000B04]))
#define MCF548X_PCI_PCICCRIR      (*(volatile uint32_t*)(void*)(&__MBAR[0x000B08]))
#define MCF548X_PCI_PCICR1        (*(volatile uint32_t*)(void*)(&__MBAR[0x000B0C]))
#define MCF548X_PCI_PCIBAR0       (*(volatile uint32_t*)(void*)(&__MBAR[0x000B10]))
#define MCF548X_PCI_PCIBAR1       (*(volatile uint32_t*)(void*)(&__MBAR[0x000B14]))
#define MCF548X_PCI_PCICR2        (*(volatile uint32_t*)(void*)(&__MBAR[0x000B3C]))
#define MCF548X_PCI_PCIGSCR       (*(volatile uint32_t*)(void*)(&__MBAR[0x000B60]))
#define MCF548X_PCI_PCITBATR0     (*(volatile uint32_t*)(void*)(&__MBAR[0x000B64]))
#define MCF548X_PCI_PCITBATR1     (*(volatile uint32_t*)(void*)(&__MBAR[0x000B68]))
#define MCF548X_PCI_PCITCR        (*(volatile uint32_t*)(void*)(&__MBAR[0x000B6C]))
#define MCF548X_PCI_PCIIW0BTAR    (*(volatile uint32_t*)(void*)(&__MBAR[0x000B70]))
#define MCF548X_PCI_PCIIW1BTAR    (*(volatile uint32_t*)(void*)(&__MBAR[0x000B74]))
#define MCF548X_PCI_PCIIW2BTAR    (*(volatile uint32_t*)(void*)(&__MBAR[0x000B78]))
#define MCF548X_PCI_PCIIWCR       (*(volatile uint32_t*)(void*)(&__MBAR[0x000B80]))
#define MCF548X_PCI_PCIICR        (*(volatile uint32_t*)(void*)(&__MBAR[0x000B84]))
#define MCF548X_PCI_PCIISR        (*(volatile uint32_t*)(void*)(&__MBAR[0x000B88]))
#define MCF548X_PCI_PCICAR        (*(volatile uint32_t*)(void*)(&__MBAR[0x000BF8]))
#define MCF548X_PCI_PCITPSR       (*(volatile uint32_t*)(void*)(&__MBAR[0x008400]))
#define MCF548X_PCI_PCITSAR       (*(volatile uint32_t*)(void*)(&__MBAR[0x008404]))
#define MCF548X_PCI_PCITTCR       (*(volatile uint32_t*)(void*)(&__MBAR[0x008408]))
#define MCF548X_PCI_PCITER        (*(volatile uint32_t*)(void*)(&__MBAR[0x00840C]))
#define MCF548X_PCI_PCITNAR       (*(volatile uint32_t*)(void*)(&__MBAR[0x008410]))
#define MCF548X_PCI_PCITLWR       (*(volatile uint32_t*)(void*)(&__MBAR[0x008414]))
#define MCF548X_PCI_PCITDCR       (*(volatile uint32_t*)(void*)(&__MBAR[0x008418]))
#define MCF548X_PCI_PCITSR        (*(volatile uint32_t*)(void*)(&__MBAR[0x00841C]))
#define MCF548X_PCI_PCITFDR       (*(volatile uint32_t*)(void*)(&__MBAR[0x008440]))
#define MCF548X_PCI_PCITFSR       (*(volatile uint32_t*)(void*)(&__MBAR[0x008444]))
#define MCF548X_PCI_PCITFCR       (*(volatile uint32_t*)(void*)(&__MBAR[0x008448]))
#define MCF548X_PCI_PCITFAR       (*(volatile uint32_t*)(void*)(&__MBAR[0x00844C]))
#define MCF548X_PCI_PCITFRPR      (*(volatile uint32_t*)(void*)(&__MBAR[0x008450]))
#define MCF548X_PCI_PCITFWPR      (*(volatile uint32_t*)(void*)(&__MBAR[0x008454]))
#define MCF548X_PCI_PCIRPSR       (*(volatile uint32_t*)(void*)(&__MBAR[0x008480]))
#define MCF548X_PCI_PCIRSAR       (*(volatile uint32_t*)(void*)(&__MBAR[0x008484]))
#define MCF548X_PCI_PCIRTCR       (*(volatile uint32_t*)(void*)(&__MBAR[0x008488]))
#define MCF548X_PCI_PCIRER        (*(volatile uint32_t*)(void*)(&__MBAR[0x00848C]))
#define MCF548X_PCI_PCIRNAR       (*(volatile uint32_t*)(void*)(&__MBAR[0x008490]))
#define MCF548X_PCI_PCIRDCR       (*(volatile uint32_t*)(void*)(&__MBAR[0x008498]))
#define MCF548X_PCI_PCIRSR        (*(volatile uint32_t*)(void*)(&__MBAR[0x00849C]))
#define MCF548X_PCI_PCIRFDR       (*(volatile uint32_t*)(void*)(&__MBAR[0x0084C0]))
#define MCF548X_PCI_PCIRFSR       (*(volatile uint32_t*)(void*)(&__MBAR[0x0084C4]))
#define MCF548X_PCI_PCIRFCR       (*(volatile uint32_t*)(void*)(&__MBAR[0x0084C8]))
#define MCF548X_PCI_PCIRFAR       (*(volatile uint32_t*)(void*)(&__MBAR[0x0084CC]))
#define MCF548X_PCI_PCIRFRPR      (*(volatile uint32_t*)(void*)(&__MBAR[0x0084D0]))
#define MCF548X_PCI_PCIRFWPR      (*(volatile uint32_t*)(void*)(&__MBAR[0x0084D4]))

/* Bit definitions and macros for MCF548X_PCI_PCIIDR */
#define MCF548X_PCI_PCIIDR_VENDORID(x)            (((x)&0x0000FFFF)<<0)
#define MCF548X_PCI_PCIIDR_DEVICEID(x)            (((x)&0x0000FFFF)<<16)

/* Bit definitions and macros for MCF548X_PCI_PCISCR */
#define MCF548X_PCI_PCISCR_M                      (0x00000002)
#define MCF548X_PCI_PCISCR_B                      (0x00000004)
#define MCF548X_PCI_PCISCR_SP                     (0x00000008)
#define MCF548X_PCI_PCISCR_MW                     (0x00000010)
#define MCF548X_PCI_PCISCR_PER                    (0x00000040)
#define MCF548X_PCI_PCISCR_S                      (0x00000100)
#define MCF548X_PCI_PCISCR_F                      (0x00000200)
#define MCF548X_PCI_PCISCR_C                      (0x00100000)
#define MCF548X_PCI_PCISCR_66M                    (0x00200000)
#define MCF548X_PCI_PCISCR_R                      (0x00400000)
#define MCF548X_PCI_PCISCR_FC                     (0x00800000)
#define MCF548X_PCI_PCISCR_DP                     (0x01000000)
#define MCF548X_PCI_PCISCR_DT(x)                  (((x)&0x00000003)<<25)
#define MCF548X_PCI_PCISCR_TS                     (0x08000000)
#define MCF548X_PCI_PCISCR_TR                     (0x10000000)
#define MCF548X_PCI_PCISCR_MA                     (0x20000000)
#define MCF548X_PCI_PCISCR_SE                     (0x40000000)
#define MCF548X_PCI_PCISCR_PE                     (0x80000000)

/* Bit definitions and macros for MCF548X_PCI_PCICCRIR */
#define MCF548X_PCI_PCICCRIR_REVID(x)             (((x)&0x000000FF)<<0)
#define MCF548X_PCI_PCICCRIR_CLASSCODE(x)         (((x)&0x00FFFFFF)<<8)

/* Bit definitions and macros for MCF548X_PCI_PCICR1 */
#define MCF548X_PCI_PCICR1_CACHELINESIZE(x)       (((x)&0x0000000F)<<0)
#define MCF548X_PCI_PCICR1_LATTIMER(x)            (((x)&0x000000FF)<<8)
#define MCF548X_PCI_PCICR1_HEADERTYPE(x)          (((x)&0x000000FF)<<16)
#define MCF548X_PCI_PCICR1_BIST(x)                (((x)&0x000000FF)<<24)

/* Bit definitions and macros for MCF548X_PCI_PCIBAR0 */
#define MCF548X_PCI_PCIBAR0_IO                    (0x00000001)
#define MCF548X_PCI_PCIBAR0_RANGE(x)              (((x)&0x00000003)<<1)
#define MCF548X_PCI_PCIBAR0_PREF                  (0x00000008)
#define MCF548X_PCI_PCIBAR0_BAR0(x)               (((x)&0x00003FFF)<<18)

/* Bit definitions and macros for MCF548X_PCI_PCIBAR1 */
#define MCF548X_PCI_PCIBAR1_IO                    (0x00000001)
#define MCF548X_PCI_PCIBAR1_PREF                  (0x00000008)
#define MCF548X_PCI_PCIBAR1_BAR1(x)               (((x)&0x00000003)<<30)

/* Bit definitions and macros for MCF548X_PCI_PCICR2 */
#define MCF548X_PCI_PCICR2_INTLINE(x)             (((x)&0x000000FF)<<0)
#define MCF548X_PCI_PCICR2_INTPIN(x)              (((x)&0x000000FF)<<8)
#define MCF548X_PCI_PCICR2_MINGNT(x)              (((x)&0x000000FF)<<16)
#define MCF548X_PCI_PCICR2_MAXLAT(x)              (((x)&0x000000FF)<<24)

/* Bit definitions and macros for MCF548X_PCI_PCIGSCR */
#define MCF548X_PCI_PCIGSCR_PR                    (0x00000001)
#define MCF548X_PCI_PCIGSCR_SEE                   (0x00001000)
#define MCF548X_PCI_PCIGSCR_PEE                   (0x00002000)
#define MCF548X_PCI_PCIGSCR_SE                    (0x10000000)
#define MCF548X_PCI_PCIGSCR_PE                    (0x20000000)

/* Bit definitions and macros for MCF548X_PCI_PCITBATR0 */
#define MCF548X_PCI_PCITBATR0_EN                  (0x00000001)
#define MCF548X_PCI_PCITBATR0_BAT0(x)             (((x)&0x00003FFF)<<18)

/* Bit definitions and macros for MCF548X_PCI_PCITBATR1 */
#define MCF548X_PCI_PCITBATR1_EN                  (0x00000001)
#define MCF548X_PCI_PCITBATR1_BAT1(x)             (((x)&0x00000003)<<30)

/* Bit definitions and macros for MCF548X_PCI_PCITCR */
#define MCF548X_PCI_PCITCR_P                      (0x00010000)
#define MCF548X_PCI_PCITCR_LD                     (0x01000000)

/* Bit definitions and macros for MCF548X_PCI_PCIIW0BTAR */
#define MCF548X_PCI_PCIIW0BTAR_WTA0(x)            (((x)&0x000000FF)<<8)
#define MCF548X_PCI_PCIIW0BTAR_WAM0(x)            (((x)&0x000000FF)<<16)
#define MCF548X_PCI_PCIIW0BTAR_WBA0(x)            (((x)&0x000000FF)<<24)

/* Bit definitions and macros for MCF548X_PCI_PCIIW1BTAR */
#define MCF548X_PCI_PCIIW1BTAR_WTA1(x)            (((x)&0x000000FF)<<8)
#define MCF548X_PCI_PCIIW1BTAR_WAM1(x)            (((x)&0x000000FF)<<16)
#define MCF548X_PCI_PCIIW1BTAR_WBA1(x)            (((x)&0x000000FF)<<24)

/* Bit definitions and macros for MCF548X_PCI_PCIIW2BTAR */
#define MCF548X_PCI_PCIIW2BTAR_WTA2(x)            (((x)&0x000000FF)<<8)
#define MCF548X_PCI_PCIIW2BTAR_WAM2(x)            (((x)&0x000000FF)<<16)
#define MCF548X_PCI_PCIIW2BTAR_WBA2(x)            (((x)&0x000000FF)<<24)

/* Bit definitions and macros for MCF548X_PCI_PCIIWCR */
#define MCF548X_PCI_PCIIWCR_WINCTRL2(x)           (((x)&0x0000000F)<<8)
#define MCF548X_PCI_PCIIWCR_WINCTRL1(x)           (((x)&0x0000000F)<<16)
#define MCF548X_PCI_PCIIWCR_WINCTRL0(x)           (((x)&0x0000000F)<<24)
#define MCF548X_PCI_PCIIWCR_WINCTRL0_MEMREAD      (0x01000000)
#define MCF548X_PCI_PCIIWCR_WINCTRL0_MEMRDLINE    (0x03000000)
#define MCF548X_PCI_PCIIWCR_WINCTRL0_MEMRDMUL     (0x05000000)
#define MCF548X_PCI_PCIIWCR_WINCTRL0_IO           (0x09000000)
#define MCF548X_PCI_PCIIWCR_WINCTRL1_MEMREAD      (0x00010000)
#define MCF548X_PCI_PCIIWCR_WINCTRL1_MEMRDLINE    (0x00030000)
#define MCF548X_PCI_PCIIWCR_WINCTRL1_MEMRDMUL     (0x00050000)
#define MCF548X_PCI_PCIIWCR_WINCTRL1_IO           (0x00090000)
#define MCF548X_PCI_PCIIWCR_WINCTRL2_MEMREAD      (0x00000100)
#define MCF548X_PCI_PCIIWCR_WINCTRL2_MEMRDLINE    (0x00000300)
#define MCF548X_PCI_PCIIWCR_WINCTRL2_MEMRDMUL     (0x00000500)
#define MCF548X_PCI_PCIIWCR_WINCTRL2_IO           (0x00000900)

/* Bit definitions and macros for MCF548X_PCI_PCIICR */
#define MCF548X_PCI_PCIICR_MAXRETRY(x)            (((x)&0x000000FF)<<0)
#define MCF548X_PCI_PCIICR_TAE                    (0x01000000)
#define MCF548X_PCI_PCIICR_IAE                    (0x02000000)
#define MCF548X_PCI_PCIICR_REE                    (0x04000000)

/* Bit definitions and macros for MCF548X_PCI_PCIISR */
#define MCF548X_PCI_PCIISR_TA                     (0x01000000)
#define MCF548X_PCI_PCIISR_IA                     (0x02000000)
#define MCF548X_PCI_PCIISR_RE                     (0x04000000)

/* Bit definitions and macros for MCF548X_PCI_PCICAR */
#define MCF548X_PCI_PCICAR_DWORD(x)               (((x)&0x0000003F)<<2)
#define MCF548X_PCI_PCICAR_FUNCNUM(x)             (((x)&0x00000007)<<8)
#define MCF548X_PCI_PCICAR_DEVNUM(x)              (((x)&0x0000001F)<<11)
#define MCF548X_PCI_PCICAR_BUSNUM(x)              (((x)&0x000000FF)<<16)
#define MCF548X_PCI_PCICAR_E                      (0x80000000)

/* Bit definitions and macros for MCF548X_PCI_PCITPSR */
#define MCF548X_PCI_PCITPSR_PKTSIZE(x)            (((x)&0x0000FFFF)<<16)

/* Bit definitions and macros for MCF548X_PCI_PCITTCR */
#define MCF548X_PCI_PCITTCR_DI                    (0x00000001)
#define MCF548X_PCI_PCITTCR_W                     (0x00000010)
#define MCF548X_PCI_PCITTCR_MAXBEATS(x)           (((x)&0x00000007)<<8)
#define MCF548X_PCI_PCITTCR_MAXRETRY(x)           (((x)&0x000000FF)<<16)
#define MCF548X_PCI_PCITTCR_PCICMD(x)             (((x)&0x0000000F)<<24)

/* Bit definitions and macros for MCF548X_PCI_PCITER */
#define MCF548X_PCI_PCITER_NE                     (0x00010000)
#define MCF548X_PCI_PCITER_IAE                    (0x00020000)
#define MCF548X_PCI_PCITER_TAE                    (0x00040000)
#define MCF548X_PCI_PCITER_RE                     (0x00080000)
#define MCF548X_PCI_PCITER_SE                     (0x00100000)
#define MCF548X_PCI_PCITER_FEE                    (0x00200000)
#define MCF548X_PCI_PCITER_ME                     (0x01000000)
#define MCF548X_PCI_PCITER_BE                     (0x08000000)
#define MCF548X_PCI_PCITER_CM                     (0x10000000)
#define MCF548X_PCI_PCITER_RF                     (0x40000000)
#define MCF548X_PCI_PCITER_RC                     (0x80000000)

/* Bit definitions and macros for MCF548X_PCI_PCITDCR */
#define MCF548X_PCI_PCITDCR_PKTSDONE(x)           (((x)&0x0000FFFF)<<0)
#define MCF548X_PCI_PCITDCR_BYTESDONE(x)          (((x)&0x0000FFFF)<<16)

/* Bit definitions and macros for MCF548X_PCI_PCITSR */
#define MCF548X_PCI_PCITSR_IA                     (0x00010000)
#define MCF548X_PCI_PCITSR_TA                     (0x00020000)
#define MCF548X_PCI_PCITSR_RE                     (0x00040000)
#define MCF548X_PCI_PCITSR_SE                     (0x00080000)
#define MCF548X_PCI_PCITSR_FE                     (0x00100000)
#define MCF548X_PCI_PCITSR_BE1                    (0x00200000)
#define MCF548X_PCI_PCITSR_BE2                    (0x00400000)
#define MCF548X_PCI_PCITSR_BE3                    (0x00800000)
#define MCF548X_PCI_PCITSR_NT                     (0x01000000)

/* Bit definitions and macros for MCF548X_PCI_PCITFSR */
#define MCF548X_PCI_PCITFSR_EMT                   (0x00010000)
#define MCF548X_PCI_PCITFSR_ALARM                 (0x00020000)
#define MCF548X_PCI_PCITFSR_FU                    (0x00040000)
#define MCF548X_PCI_PCITFSR_FR                    (0x00080000)
#define MCF548X_PCI_PCITFSR_OF                    (0x00100000)
#define MCF548X_PCI_PCITFSR_UF                    (0x00200000)
#define MCF548X_PCI_PCITFSR_RXW                   (0x00400000)

/* Bit definitions and macros for MCF548X_PCI_PCITFCR */
#define MCF548X_PCI_PCITFCR_OF_MSK                (0x00080000)
#define MCF548X_PCI_PCITFCR_UF_MSK                (0x00100000)
#define MCF548X_PCI_PCITFCR_RXW_MSK               (0x00200000)
#define MCF548X_PCI_PCITFCR_FAE_MSK               (0x00400000)
#define MCF548X_PCI_PCITFCR_IP_MSK                (0x00800000)
#define MCF548X_PCI_PCITFCR_GR(x)                 (((x)&0x00000007)<<24)

/* Bit definitions and macros for MCF548X_PCI_PCITFAR */
#define MCF548X_PCI_PCITFAR_ALARM(x)              (((x)&0x0000007F)<<0)

/* Bit definitions and macros for MCF548X_PCI_PCITFRPR */
#define MCF548X_PCI_PCITFRPR_READ(x)              (((x)&0x00000FFF)<<0)

/* Bit definitions and macros for MCF548X_PCI_PCITFWPR */
#define MCF548X_PCI_PCITFWPR_WRITE(x)             (((x)&0x00000FFF)<<0)

/* Bit definitions and macros for MCF548X_PCI_PCIRPSR */
#define MCF548X_PCI_PCIRPSR_PKTSIZE(x)            (((x)&0x0000FFFF)<<16)

/* Bit definitions and macros for MCF548X_PCI_PCIRTCR */
#define MCF548X_PCI_PCIRTCR_DI                    (0x00000001)
#define MCF548X_PCI_PCIRTCR_W                     (0x00000010)
#define MCF548X_PCI_PCIRTCR_MAXBEATS(x)           (((x)&0x00000007)<<8)
#define MCF548X_PCI_PCIRTCR_FB                    (0x00001000)
#define MCF548X_PCI_PCIRTCR_MAXRETRY(x)           (((x)&0x000000FF)<<16)
#define MCF548X_PCI_PCIRTCR_PCICMD(x)             (((x)&0x0000000F)<<24)

/* Bit definitions and macros for MCF548X_PCI_PCIRER */
#define MCF548X_PCI_PCIRER_NE                     (0x00010000)
#define MCF548X_PCI_PCIRER_IAE                    (0x00020000)
#define MCF548X_PCI_PCIRER_TAE                    (0x00040000)
#define MCF548X_PCI_PCIRER_RE                     (0x00080000)
#define MCF548X_PCI_PCIRER_SE                     (0x00100000)
#define MCF548X_PCI_PCIRER_FEE                    (0x00200000)
#define MCF548X_PCI_PCIRER_ME                     (0x01000000)
#define MCF548X_PCI_PCIRER_BE                     (0x08000000)
#define MCF548X_PCI_PCIRER_CM                     (0x10000000)
#define MCF548X_PCI_PCIRER_FE                     (0x20000000)
#define MCF548X_PCI_PCIRER_RF                     (0x40000000)
#define MCF548X_PCI_PCIRER_RC                     (0x80000000)

/* Bit definitions and macros for MCF548X_PCI_PCIRDCR */
#define MCF548X_PCI_PCIRDCR_PKTSDONE(x)           (((x)&0x0000FFFF)<<0)
#define MCF548X_PCI_PCIRDCR_BYTESDONE(x)          (((x)&0x0000FFFF)<<16)

/* Bit definitions and macros for MCF548X_PCI_PCIRSR */
#define MCF548X_PCI_PCIRSR_IA                     (0x00010000)
#define MCF548X_PCI_PCIRSR_TA                     (0x00020000)
#define MCF548X_PCI_PCIRSR_RE                     (0x00040000)
#define MCF548X_PCI_PCIRSR_SE                     (0x00080000)
#define MCF548X_PCI_PCIRSR_FE                     (0x00100000)
#define MCF548X_PCI_PCIRSR_BE1                    (0x00200000)
#define MCF548X_PCI_PCIRSR_BE2                    (0x00400000)
#define MCF548X_PCI_PCIRSR_BE3                    (0x00800000)
#define MCF548X_PCI_PCIRSR_NT                     (0x01000000)

/* Bit definitions and macros for MCF548X_PCI_PCIRFSR */
#define MCF548X_PCI_PCIRFSR_EMT                   (0x00010000)
#define MCF548X_PCI_PCIRFSR_ALARM                 (0x00020000)
#define MCF548X_PCI_PCIRFSR_FU                    (0x00040000)
#define MCF548X_PCI_PCIRFSR_FR                    (0x00080000)
#define MCF548X_PCI_PCIRFSR_OF                    (0x00100000)
#define MCF548X_PCI_PCIRFSR_UF                    (0x00200000)
#define MCF548X_PCI_PCIRFSR_RXW                   (0x00400000)

/* Bit definitions and macros for MCF548X_PCI_PCIRFCR */
#define MCF548X_PCI_PCIRFCR_OF_MSK                (0x00080000)
#define MCF548X_PCI_PCIRFCR_UF_MSK                (0x00100000)
#define MCF548X_PCI_PCIRFCR_RXW_MSK               (0x00200000)
#define MCF548X_PCI_PCIRFCR_FAE_MSK               (0x00400000)
#define MCF548X_PCI_PCIRFCR_IP_MSK                (0x00800000)
#define MCF548X_PCI_PCIRFCR_GR(x)                 (((x)&0x00000007)<<24)

/* Bit definitions and macros for MCF548X_PCI_PCIRFAR */
#define MCF548X_PCI_PCIRFAR_ALARM(x)              (((x)&0x0000007F)<<0)

/* Bit definitions and macros for MCF548X_PCI_PCIRFRPR */
#define MCF548X_PCI_PCIRFRPR_READ(x)              (((x)&0x00000FFF)<<0)

/* Bit definitions and macros for MCF548X_PCI_PCIRFWPR */
#define MCF548X_PCI_PCIRFWPR_WRITE(x)             (((x)&0x00000FFF)<<0)


/*********************************************************************
*
* PCI Arbiter Module (PCIARB)
*
*********************************************************************/

/* Register read/write macros */
#define MCF548X_PCIARB_PACR    (*(volatile uint32_t*)(void*)(&__MBAR[0x000C00]))
#define MCF548X_PCIARB_PASR    (*(volatile uint32_t*)(void*)(&__MBAR[0x000C04]))

/* Bit definitions and macros for MCF548X_PCIARB_PACR */
#define MCF548X_PCIARB_PACR_INTMPRI         (0x00000001)
#define MCF548X_PCIARB_PACR_EXTMPRI(x)      (((x)&0x0000001F)<<1)
#define MCF548X_PCIARB_PACR_INTMINTEN       (0x00010000)
#define MCF548X_PCIARB_PACR_EXTMINTEN(x)    (((x)&0x0000001F)<<17)
#define MCF548X_PCIARB_PACR_PKMD            (0x40000000)
#define MCF548X_PCIARB_PACR_DS              (0x80000000)

/* Bit definitions and macros for MCF548X_PCIARB_PASR */
#define MCF548X_PCIARB_PASR_ITLMBK          (0x00010000)
#define MCF548X_PCIARB_PASR_EXTMBK(x)       (((x)&0x0000001F)<<17)


/*********************************************************************
*
* Multi-Channel DMA (DMA)
*
*********************************************************************/

/* Register read/write macros */
#define MCF548X_DMA_TASKBAR    (*(volatile uint32_t*)(void*)(&__MBAR[0x008000]))
#define MCF548X_DMA_CP         (*(volatile uint32_t*)(void*)(&__MBAR[0x008004]))
#define MCF548X_DMA_EP         (*(volatile uint32_t*)(void*)(&__MBAR[0x008008]))
#define MCF548X_DMA_VP         (*(volatile uint32_t*)(void*)(&__MBAR[0x00800C]))
#define MCF548X_DMA_DIPR       (*(volatile uint32_t*)(void*)(&__MBAR[0x008014]))
#define MCF548X_DMA_DIMR       (*(volatile uint32_t*)(void*)(&__MBAR[0x008018]))
#define MCF548X_DMA_TCR0       (*(volatile uint16_t*)(void*)(&__MBAR[0x00801C]))
#define MCF548X_DMA_TCR1       (*(volatile uint16_t*)(void*)(&__MBAR[0x00801E]))
#define MCF548X_DMA_TCR2       (*(volatile uint16_t*)(void*)(&__MBAR[0x008020]))
#define MCF548X_DMA_TCR3       (*(volatile uint16_t*)(void*)(&__MBAR[0x008022]))
#define MCF548X_DMA_TCR4       (*(volatile uint16_t*)(void*)(&__MBAR[0x008024]))
#define MCF548X_DMA_TCR5       (*(volatile uint16_t*)(void*)(&__MBAR[0x008026]))
#define MCF548X_DMA_TCR6       (*(volatile uint16_t*)(void*)(&__MBAR[0x008028]))
#define MCF548X_DMA_TCR7       (*(volatile uint16_t*)(void*)(&__MBAR[0x00802A]))
#define MCF548X_DMA_TCR8       (*(volatile uint16_t*)(void*)(&__MBAR[0x00802C]))
#define MCF548X_DMA_TCR9       (*(volatile uint16_t*)(void*)(&__MBAR[0x00802E]))
#define MCF548X_DMA_TCR10      (*(volatile uint16_t*)(void*)(&__MBAR[0x008030]))
#define MCF548X_DMA_TCR11      (*(volatile uint16_t*)(void*)(&__MBAR[0x008032]))
#define MCF548X_DMA_TCR12      (*(volatile uint16_t*)(void*)(&__MBAR[0x008034]))
#define MCF548X_DMA_TCR13      (*(volatile uint16_t*)(void*)(&__MBAR[0x008036]))
#define MCF548X_DMA_TCR14      (*(volatile uint16_t*)(void*)(&__MBAR[0x008038]))
#define MCF548X_DMA_TCR15      (*(volatile uint16_t*)(void*)(&__MBAR[0x00803A]))
#define MCF548X_DMA_TCRn(x)    (*(volatile uint16_t*)(void*)(&__MBAR[0x00801CU+((x)*0x002)]))
#define MCF548X_DMA_IMCR       (*(volatile uint32_t*)(void*)(&__MBAR[0x00805C]))
#define MCF548X_DMA_PTDDBG     (*(volatile uint32_t*)(void*)(&__MBAR[0x008080]))

/* Bit definitions and macros for MCF548X_DMA_DIPR */
#define MCF548X_DMA_DIPR_TASK0           (0x00000001)
#define MCF548X_DMA_DIPR_TASK1           (0x00000002)
#define MCF548X_DMA_DIPR_TASK2           (0x00000004)
#define MCF548X_DMA_DIPR_TASK3           (0x00000008)
#define MCF548X_DMA_DIPR_TASK4           (0x00000010)
#define MCF548X_DMA_DIPR_TASK5           (0x00000020)
#define MCF548X_DMA_DIPR_TASK6           (0x00000040)
#define MCF548X_DMA_DIPR_TASK7           (0x00000080)
#define MCF548X_DMA_DIPR_TASK8           (0x00000100)
#define MCF548X_DMA_DIPR_TASK9           (0x00000200)
#define MCF548X_DMA_DIPR_TASK10          (0x00000400)
#define MCF548X_DMA_DIPR_TASK11          (0x00000800)
#define MCF548X_DMA_DIPR_TASK12          (0x00001000)
#define MCF548X_DMA_DIPR_TASK13          (0x00002000)
#define MCF548X_DMA_DIPR_TASK14          (0x00004000)
#define MCF548X_DMA_DIPR_TASK15          (0x00008000)

/* Bit definitions and macros for MCF548X_DMA_DIMR */
#define MCF548X_DMA_DIMR_TASK0           (0x00000001)
#define MCF548X_DMA_DIMR_TASK1           (0x00000002)
#define MCF548X_DMA_DIMR_TASK2           (0x00000004)
#define MCF548X_DMA_DIMR_TASK3           (0x00000008)
#define MCF548X_DMA_DIMR_TASK4           (0x00000010)
#define MCF548X_DMA_DIMR_TASK5           (0x00000020)
#define MCF548X_DMA_DIMR_TASK6           (0x00000040)
#define MCF548X_DMA_DIMR_TASK7           (0x00000080)
#define MCF548X_DMA_DIMR_TASK8           (0x00000100)
#define MCF548X_DMA_DIMR_TASK9           (0x00000200)
#define MCF548X_DMA_DIMR_TASK10          (0x00000400)
#define MCF548X_DMA_DIMR_TASK11          (0x00000800)
#define MCF548X_DMA_DIMR_TASK12          (0x00001000)
#define MCF548X_DMA_DIMR_TASK13          (0x00002000)
#define MCF548X_DMA_DIMR_TASK14          (0x00004000)
#define MCF548X_DMA_DIMR_TASK15          (0x00008000)

/* Bit definitions and macros for MCF548X_DMA_IMCR */
#define MCF548X_DMA_IMCR_SRC16(x)        (((x)&0x00000003)<<0)
#define MCF548X_DMA_IMCR_SRC17(x)        (((x)&0x00000003)<<2)
#define MCF548X_DMA_IMCR_SRC18(x)        (((x)&0x00000003)<<4)
#define MCF548X_DMA_IMCR_SRC19(x)        (((x)&0x00000003)<<6)
#define MCF548X_DMA_IMCR_SRC20(x)        (((x)&0x00000003)<<8)
#define MCF548X_DMA_IMCR_SRC21(x)        (((x)&0x00000003)<<10)
#define MCF548X_DMA_IMCR_SRC22(x)        (((x)&0x00000003)<<12)
#define MCF548X_DMA_IMCR_SRC23(x)        (((x)&0x00000003)<<14)
#define MCF548X_DMA_IMCR_SRC24(x)        (((x)&0x00000003)<<16)
#define MCF548X_DMA_IMCR_SRC25(x)        (((x)&0x00000003)<<18)
#define MCF548X_DMA_IMCR_SRC26(x)        (((x)&0x00000003)<<20)
#define MCF548X_DMA_IMCR_SRC27(x)        (((x)&0x00000003)<<22)
#define MCF548X_DMA_IMCR_SRC28(x)        (((x)&0x00000003)<<24)
#define MCF548X_DMA_IMCR_SRC29(x)        (((x)&0x00000003)<<26)
#define MCF548X_DMA_IMCR_SRC30(x)        (((x)&0x00000003)<<28)
#define MCF548X_DMA_IMCR_SRC31(x)        (((x)&0x00000003)<<30)
#define MCF548X_DMA_IMCR_SRC16_FEC0RX    (0x00000000)
#define MCF548X_DMA_IMCR_SRC17_FEC0TX    (0x00000000)
#define MCF548X_DMA_IMCR_SRC18_FEC0RX    (0x00000020)
#define MCF548X_DMA_IMCR_SRC19_FEC0TX    (0x00000080)
#define MCF548X_DMA_IMCR_SRC20_FEC1RX    (0x00000100)
#define MCF548X_DMA_IMCR_SRC21_DREQ1     (0x00000000)
#define MCF548X_DMA_IMCR_SRC21_FEC1TX    (0x00000400)
#define MCF548X_DMA_IMCR_SRC22_FEC0RX    (0x00001000)
#define MCF548X_DMA_IMCR_SRC23_FEC0TX    (0x00004000)
#define MCF548X_DMA_IMCR_SRC24_CTM0      (0x00010000)
#define MCF548X_DMA_IMCR_SRC24_FEC1RX    (0x00020000)
#define MCF548X_DMA_IMCR_SRC25_CTM1      (0x00040000)
#define MCF548X_DMA_IMCR_SRC25_FEC1TX    (0x00080000)
#define MCF548X_DMA_IMCR_SRC26_USBEP4    (0x00000000)
#define MCF548X_DMA_IMCR_SRC26_CTM2      (0x00200000)
#define MCF548X_DMA_IMCR_SRC27_USBEP5    (0x00000000)
#define MCF548X_DMA_IMCR_SRC27_CTM3      (0x00800000)
#define MCF548X_DMA_IMCR_SRC28_USBEP6    (0x00000000)
#define MCF548X_DMA_IMCR_SRC28_CTM4      (0x01000000)
#define MCF548X_DMA_IMCR_SRC28_DREQ1     (0x02000000)
#define MCF548X_DMA_IMCR_SRC28_PSC2RX    (0x03000000)
#define MCF548X_DMA_IMCR_SRC29_DREQ1     (0x04000000)
#define MCF548X_DMA_IMCR_SRC29_CTM5      (0x08000000)
#define MCF548X_DMA_IMCR_SRC29_PSC2TX    (0x0C000000)
#define MCF548X_DMA_IMCR_SRC30_FEC1RX    (0x00000000)
#define MCF548X_DMA_IMCR_SRC30_CTM6      (0x10000000)
#define MCF548X_DMA_IMCR_SRC30_PSC3RX    (0x30000000)
#define MCF548X_DMA_IMCR_SRC31_FEC1TX    (0x00000000)
#define MCF548X_DMA_IMCR_SRC31_CTM7      (0x80000000)
#define MCF548X_DMA_IMCR_SRC31_PSC3TX    (0xC0000000)


/*********************************************************************
*
* Multi-Channel DMA External Requests (DMA_EREQ)
*
*********************************************************************/

/* Register read/write macros */
#define MCF548X_DMA_EREQ_EREQBAR0       (*(volatile uint32_t*)(void*)(&__MBAR[0x000D00]))
#define MCF548X_DMA_EREQ_EREQMASK0      (*(volatile uint32_t*)(void*)(&__MBAR[0x000D04]))
#define MCF548X_DMA_EREQ_EREQCTRL0      (*(volatile uint32_t*)(void*)(&__MBAR[0x000D08]))
#define MCF548X_DMA_EREQ_EREQBAR1       (*(volatile uint32_t*)(void*)(&__MBAR[0x000D10]))
#define MCF548X_DMA_EREQ_EREQMASK1      (*(volatile uint32_t*)(void*)(&__MBAR[0x000D14]))
#define MCF548X_DMA_EREQ_EREQCTRL1      (*(volatile uint32_t*)(void*)(&__MBAR[0x000D18]))
#define MCF548X_DMA_EREQ_EREQBAR(x)     (*(volatile uint32_t*)(void*)(&__MBAR[0x000D00U+((x)*0x010)]))
#define MCF548X_DMA_EREQ_EREQMASK(x)    (*(volatile uint32_t*)(void*)(&__MBAR[0x000D04U+((x)*0x010)]))
#define MCF548X_DMA_EREQ_EREQCTRL(x)    (*(volatile uint32_t*)(void*)(&__MBAR[0x000D08U+((x)*0x010)]))

/* Bit definitions and macros for MCF548X_DMA_EREQ_EREQCTRL */
#define MCF548X_DMA_EREQ_EREQCTRL_EN                   (0x00000001)
#define MCF548X_DMA_EREQ_EREQCTRL_SYNC                 (0x00000002)
#define MCF548X_DMA_EREQ_EREQCTRL_DACKWID(x)           (((x)&0x00000003)<<2)
#define MCF548X_DMA_EREQ_EREQCTRL_BSEL(x)              (((x)&0x00000003)<<4)
#define MCF548X_DMA_EREQ_EREQCTRL_MD(x)                (((x)&0x00000003)<<6)
#define MCF548X_DMA_EREQ_EREQCTRL_MD_IDLE              (0x00000000)
#define MCF548X_DMA_EREQ_EREQCTRL_MD_LEVEL             (0x00000040)
#define MCF548X_DMA_EREQ_EREQCTRL_MD_EDGE              (0x00000080)
#define MCF548X_DMA_EREQ_EREQCTRL_MD_PIPED             (0x000000C0)
#define MCF548X_DMA_EREQ_EREQCTRL_BSEL_MEM_WRITE       (0x00000000)
#define MCF548X_DMA_EREQ_EREQCTRL_BSEL_MEM_READ        (0x00000010)
#define MCF548X_DMA_EREQ_EREQCTRL_BSEL_PERIPH_WRITE    (0x00000020)
#define MCF548X_DMA_EREQ_EREQCTRL_BSEL_PERIPH_READ     (0x00000030)
#define MCF548X_DMA_EREQ_EREQCTRL_DACKWID_ONE          (0x00000000)
#define MCF548X_DMA_EREQ_EREQCTRL_DACKWID_TWO          (0x00000004)
#define MCF548X_DMA_EREQ_EREQCTRL_DACKWID_THREE        (0x00000008)
#define MCF548X_DMA_EREQ_EREQCTRL_DACKWID_FOUR         (0x0000000C)

/*********************************************************************/

#endif /* __MCF548X_H__ */
