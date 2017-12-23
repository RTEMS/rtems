/*
 * File:    mcf5223x.h
 * Purpose: Register and bit definitions
 */

#ifndef __MCF5223x_H__
#define __MCF5223x_H__

typedef volatile unsigned char vuint8;
typedef volatile unsigned short vuint16;
typedef volatile unsigned long vuint32;

/*********************************************************************
*
* System Control Module (SCM)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_SCM_IPSBAR               (*(vuint32*)(&__IPSBAR[0x000000]))
#define MCF_SCM_RAMBAR               (*(vuint32*)(&__IPSBAR[0x000008]))
#define MCF_SCM_PPMRH                (*(vuint32*)(&__IPSBAR[0x00000C]))
#define MCF_SCM_CRSR                 (*(vuint8 *)(&__IPSBAR[0x000010]))
#define MCF_SCM_CWCR                 (*(vuint8 *)(&__IPSBAR[0x000011]))
#define MCF_SCM_LPICR                (*(vuint8 *)(&__IPSBAR[0x000012]))
#define MCF_SCM_CWSR                 (*(vuint8 *)(&__IPSBAR[0x000013]))
#define MCF_SCM_PPMRL                (*(vuint32*)(&__IPSBAR[0x000018]))
#define MCF_SCM_MPARK                (*(vuint32*)(&__IPSBAR[0x00001C]))
#define MCF_SCM_MPR                  (*(vuint32*)(&__IPSBAR[0x000020]))
#define MCF_SCM_PPMRS                (*(vuint8 *)(&__IPSBAR[0x000021]))
#define MCF_SCM_PPMRC                (*(vuint8 *)(&__IPSBAR[0x000022]))
#define MCF_SCM_IPSBMT               (*(vuint8 *)(&__IPSBAR[0x000023]))
#define MCF_SCM_PACR0                (*(vuint8 *)(&__IPSBAR[0x000024]))
#define MCF_SCM_PACR1                (*(vuint8 *)(&__IPSBAR[0x000025]))
#define MCF_SCM_PACR2                (*(vuint8 *)(&__IPSBAR[0x000026]))
#define MCF_SCM_PACR3                (*(vuint8 *)(&__IPSBAR[0x000027]))
#define MCF_SCM_PACR4                (*(vuint8 *)(&__IPSBAR[0x000028]))
#define MCF_SCM_PACR5                (*(vuint8 *)(&__IPSBAR[0x000029]))
#define MCF_SCM_PACR6                (*(vuint8 *)(&__IPSBAR[0x00002A]))
#define MCF_SCM_PACR7                (*(vuint8 *)(&__IPSBAR[0x00002B]))
#define MCF_SCM_PACR8                (*(vuint8 *)(&__IPSBAR[0x00002C]))
#define MCF_SCM_GPACR0               (*(vuint8 *)(&__IPSBAR[0x000030]))
#define MCF_SCM_GPACR1               (*(vuint8 *)(&__IPSBAR[0x000031]))

/* Bit definitions and macros for MCF_SCM_IPSBAR */
#define MCF_SCM_IPSBAR_V             (0x00000001)
#define MCF_SCM_IPSBAR_BA(x)         ((x)&0xC0000000)

/* Bit definitions and macros for MCF_SCM_RAMBAR */
#define MCF_SCM_RAMBAR_BDE           (0x00000200)
#define MCF_SCM_RAMBAR_BA(x)         ((x)&0xFFFF0000)

/* Bit definitions and macros for MCF_SCM_CRSR */
#define MCF_SCM_CRSR_CWDR            (0x20)
#define MCF_SCM_CRSR_EXT             (0x80)

/* Bit definitions and macros for MCF_SCM_CWCR */
#define MCF_SCM_CWCR_CWTIC           (0x01)
#define MCF_SCM_CWCR_CWTAVAL         (0x02)
#define MCF_SCM_CWCR_CWTA            (0x04)
#define MCF_SCM_CWCR_CWT(x)          (((x)&0x07)<<3)
#define MCF_SCM_CWCR_CWRI            (0x40)
#define MCF_SCM_CWCR_CWE             (0x80)

/* Bit definitions and macros for MCF_SCM_LPICR */
#define MCF_SCM_LPICR_XIPL(x)        (((x)&0x07)<<4)
#define MCF_SCM_LPICR_ENBSTOP        (0x80)

/* Bit definitions and macros for MCF_SCM_CWSR */
#define MCF_SCM_CWSR_CWSR(x)         (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_SCM_PPMRH */
#define MCF_SCM_PPMRH_CDPORTS        (0x00000001)
#define MCF_SCM_PPMRH_CDEPORT        (0x00000002)
#define MCF_SCM_PPMRH_CDPIT0         (0x00000008)
#define MCF_SCM_PPMRH_CDPIT1         (0x00000010)
#define MCF_SCM_PPMRH_CDADC          (0x00000080)
#define MCF_SCM_PPMRH_CDGPT          (0x00000100)
#define MCF_SCM_PPMRH_CDPWN          (0x00000200)
#define MCF_SCM_PPMRH_CDFCAN         (0x00000400)
#define MCF_SCM_PPMRH_CDCFM          (0x00000800)

/* Bit definitions and macros for MCF_SCM_PPMRL */
#define MCF_SCM_PPMRL_CDG            (0x00000002)
#define MCF_SCM_PPMRL_CDEIM          (0x00000008)
#define MCF_SCM_PPMRL_CDDMA          (0x00000010)
#define MCF_SCM_PPMRL_CDUART0        (0x00000020)
#define MCF_SCM_PPMRL_CDUART1        (0x00000040)
#define MCF_SCM_PPMRL_CDUART2        (0x00000080)
#define MCF_SCM_PPMRL_CDI2C          (0x00000200)
#define MCF_SCM_PPMRL_CDQSPI         (0x00000400)
#define MCF_SCM_PPMRL_CDDTIM0        (0x00002000)
#define MCF_SCM_PPMRL_CDDTIM1        (0x00004000)
#define MCF_SCM_PPMRL_CDDTIM2        (0x00008000)
#define MCF_SCM_PPMRL_CDDTIM3        (0x00010000)
#define MCF_SCM_PPMRL_CDINTC0        (0x00020000)

/* Bit definitions and macros for MCF_SCM_MPARK */
#define MCF_SCM_MPARK_LCKOUT_TIME(x)  (((x)&0x0000000F)<<8)
#define MCF_SCM_MPARK_PRKLAST         (0x00001000)
#define MCF_SCM_MPARK_TIMEOUT         (0x00002000)
#define MCF_SCM_MPARK_FIXED           (0x00004000)
#define MCF_SCM_MPARK_M0PRTY(x)       (((x)&0x00000003)<<18)
#define MCF_SCM_MPARK_M2PRTY(x)       (((x)&0x00000003)<<20)
#define MCF_SCM_MPARK_M3PRTY(x)       (((x)&0x00000003)<<22)
#define MCF_SCM_MPARK_BCR24BIT        (0x01000000)
#define MCF_SCM_MPARK_M2_P_EN         (0x02000000)

/* Bit definitions and macros for MCF_SCM_PPMRS */
#define MCF_SCM_PPMRS_DISABLE_ALL    (64)
#define MCF_SCM_PPMRS_DISABLE_CFM    (43)
#define MCF_SCM_PPMRS_DISABLE_CAN    (42)
#define MCF_SCM_PPMRS_DISABLE_PWM    (41)
#define MCF_SCM_PPMRS_DISABLE_GPT    (40)
#define MCF_SCM_PPMRS_DISABLE_ADC    (39)
#define MCF_SCM_PPMRS_DISABLE_PIT1   (36)
#define MCF_SCM_PPMRS_DISABLE_PIT0   (35)
#define MCF_SCM_PPMRS_DISABLE_EPORT  (33)
#define MCF_SCM_PPMRS_DISABLE_PORTS  (32)
#define MCF_SCM_PPMRS_DISABLE_INTC   (17)
#define MCF_SCM_PPMRS_DISABLE_DTIM3  (16)
#define MCF_SCM_PPMRS_DISABLE_DTIM2  (15)
#define MCF_SCM_PPMRS_DISABLE_DTIM1  (14)
#define MCF_SCM_PPMRS_DISABLE_DTIM0  (13)
#define MCF_SCM_PPMRS_DISABLE_QSPI   (10)
#define MCF_SCM_PPMRS_DISABLE_I2C    (9)
#define MCF_SCM_PPMRS_DISABLE_UART2  (7)
#define MCF_SCM_PPMRS_DISABLE_UART1  (6)
#define MCF_SCM_PPMRS_DISABLE_UART0  (5)
#define MCF_SCM_PPMRS_DISABLE_DMA    (4)
#define MCF_SCM_PPMRS_SET_CDG        (1)

/* Bit definitions and macros for MCF_SCM_PPMRC */
#define MCF_SCM_PPMRC_ENABLE_ALL     (64)
#define MCF_SCM_PPMRC_ENABLE_CFM     (43)
#define MCF_SCM_PPMRC_ENABLE_CAN     (42)
#define MCF_SCM_PPMRC_ENABLE_PWM     (41)
#define MCF_SCM_PPMRC_ENABLE_GPT     (40)
#define MCF_SCM_PPMRC_ENABLE_ADC     (39)
#define MCF_SCM_PPMRC_ENABLE_PIT1    (36)
#define MCF_SCM_PPMRC_ENABLE_PIT0    (35)
#define MCF_SCM_PPMRC_ENABLE_EPORT   (33)
#define MCF_SCM_PPMRC_ENABLE_PORTS   (32)
#define MCF_SCM_PPMRC_ENABLE_INTC    (17)
#define MCF_SCM_PPMRC_ENABLE_DTIM3   (16)
#define MCF_SCM_PPMRC_ENABLE_DTIM2   (15)
#define MCF_SCM_PPMRC_ENABLE_DTIM1   (14)
#define MCF_SCM_PPMRC_ENABLE_DTIM0   (13)
#define MCF_SCM_PPMRC_ENABLE_QSPI    (10)
#define MCF_SCM_PPMRC_ENABLE_I2C     (9)
#define MCF_SCM_PPMRC_ENABLE_UART2   (7)
#define MCF_SCM_PPMRC_ENABLE_UART1   (6)
#define MCF_SCM_PPMRC_ENABLE_UART0   (5)
#define MCF_SCM_PPMRC_ENABLE_DMA     (4)
#define MCF_SCM_PPMRC_CLEAR_CDG      (1)


/*********************************************************************
*
* Power Management Module (PMM)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_PMM_PPMRH           (*(vuint32*)(&__IPSBAR[0x00000C]))
#define MCF_PMM_PPMRL           (*(vuint32*)(&__IPSBAR[0x000018]))
#define MCF_PMM_LPICR           (*(vuint8 *)(&__IPSBAR[0x000012]))
#define MCF_PMM_LPCR            (*(vuint8 *)(&__IPSBAR[0x110007]))

/* Bit definitions and macros for MCF_PMM_PPMRH */
#define MCF_PMM_PPMRH_CDPORTS   (0x00000001)
#define MCF_PMM_PPMRH_CDEPORT   (0x00000002)
#define MCF_PMM_PPMRH_CDPIT0    (0x00000008)
#define MCF_PMM_PPMRH_CDPIT1    (0x00000010)
#define MCF_PMM_PPMRH_CDADC     (0x00000080)
#define MCF_PMM_PPMRH_CDGPT     (0x00000100)
#define MCF_PMM_PPMRH_CDPWM     (0x00000200)
#define MCF_PMM_PPMRH_CDFCAN    (0x00000400)
#define MCF_PMM_PPMRH_CDCFM     (0x00000800)

/* Bit definitions and macros for MCF_PMM_PPMRL */
#define MCF_PMM_PPMRL_CDG       (0x00000002)
#define MCF_PMM_PPMRL_CDEIM     (0x00000008)
#define MCF_PMM_PPMRL_CDDMA     (0x00000010)
#define MCF_PMM_PPMRL_CDUART0   (0x00000020)
#define MCF_PMM_PPMRL_CDUART1   (0x00000040)
#define MCF_PMM_PPMRL_CDUART2   (0x00000080)
#define MCF_PMM_PPMRL_CDI2C     (0x00000200)
#define MCF_PMM_PPMRL_CDQSPI    (0x00000400)
#define MCF_PMM_PPMRL_CDDTIM0   (0x00002000)
#define MCF_PMM_PPMRL_CDDTIM1   (0x00004000)
#define MCF_PMM_PPMRL_CDDTIM2   (0x00008000)
#define MCF_PMM_PPMRL_CDDTIM3   (0x00010000)
#define MCF_PMM_PPMRL_CDINTC0   (0x00020000)

/* Bit definitions and macros for MCF_PMM_LPICR */
#define MCF_PMM_LPICR_XIPL(x)   (((x)&0x07)<<4)
#define MCF_PMM_LPICR_ENBSTOP   (0x80)

/* Bit definitions and macros for MCF_PMM_LPCR */
#define MCF_PMM_LPCR_LVDSE      (0x02)
#define MCF_PMM_LPCR_STPMD(x)   (((x)&0x03)<<3)
#define MCF_PMM_LPCR_LPMD(x)    (((x)&0x03)<<6)
#define MCF_PMM_LPCR_LPMD_STOP  (0xC0)
#define MCF_PMM_LPCR_LPMD_WAIT  (0x80)
#define MCF_PMM_LPCR_LPMD_DOZE  (0x40)
#define MCF_PMM_LPCR_LPMD_RUN   (0x00)


/*********************************************************************
*
* DMA Controller Module (DMA)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_DMA_DMAREQC                 (*(vuint32*)(&__IPSBAR[0x000014]))
#define MCF_DMA_SAR0                    (*(vuint32*)(&__IPSBAR[0x000100]))
#define MCF_DMA_SAR1                    (*(vuint32*)(&__IPSBAR[0x000110]))
#define MCF_DMA_SAR2                    (*(vuint32*)(&__IPSBAR[0x000120]))
#define MCF_DMA_SAR3                    (*(vuint32*)(&__IPSBAR[0x000130]))
#define MCF_DMA_SAR(x)                  (*(vuint32*)(&__IPSBAR[0x000100+((x)*0x010)]))
#define MCF_DMA_DAR0                    (*(vuint32*)(&__IPSBAR[0x000104]))
#define MCF_DMA_DAR1                    (*(vuint32*)(&__IPSBAR[0x000114]))
#define MCF_DMA_DAR2                    (*(vuint32*)(&__IPSBAR[0x000124]))
#define MCF_DMA_DAR3                    (*(vuint32*)(&__IPSBAR[0x000134]))
#define MCF_DMA_DAR(x)                  (*(vuint32*)(&__IPSBAR[0x000104+((x)*0x010)]))
#define MCF_DMA_DSR0                    (*(vuint8 *)(&__IPSBAR[0x000108]))
#define MCF_DMA_DSR1                    (*(vuint8 *)(&__IPSBAR[0x000118]))
#define MCF_DMA_DSR2                    (*(vuint8 *)(&__IPSBAR[0x000128]))
#define MCF_DMA_DSR3                    (*(vuint8 *)(&__IPSBAR[0x000138]))
#define MCF_DMA_DSR(x)                  (*(vuint8 *)(&__IPSBAR[0x000108+((x)*0x010)]))
#define MCF_DMA_BCR0                    (*(vuint32*)(&__IPSBAR[0x000108]))
#define MCF_DMA_BCR1                    (*(vuint32*)(&__IPSBAR[0x000118]))
#define MCF_DMA_BCR2                    (*(vuint32*)(&__IPSBAR[0x000128]))
#define MCF_DMA_BCR3                    (*(vuint32*)(&__IPSBAR[0x000138]))
#define MCF_DMA_BCR(x)                  (*(vuint32*)(&__IPSBAR[0x000108+((x)*0x010)]))
#define MCF_DMA_DCR0                    (*(vuint32*)(&__IPSBAR[0x00010C]))
#define MCF_DMA_DCR1                    (*(vuint32*)(&__IPSBAR[0x00011C]))
#define MCF_DMA_DCR2                    (*(vuint32*)(&__IPSBAR[0x00012C]))
#define MCF_DMA_DCR3                    (*(vuint32*)(&__IPSBAR[0x00013C]))
#define MCF_DMA_DCR(x)                  (*(vuint32*)(&__IPSBAR[0x00010C+((x)*0x010)]))

/* Bit definitions and macros for MCF_DMA_DMAREQC */
#define MCF_DMA_DMAREQC_DMAC0(x)        (((x)&0x0000000F)<<0)
#define MCF_DMA_DMAREQC_DMAC1(x)        (((x)&0x0000000F)<<4)
#define MCF_DMA_DMAREQC_DMAC2(x)        (((x)&0x0000000F)<<8)
#define MCF_DMA_DMAREQC_DMAC3(x)        (((x)&0x0000000F)<<12)
#define MCF_DMA_DMAREQC_DMAREQC_EXT(x)  (((x)&0x0000000F)<<16)

/* Bit definitions and macros for MCF_DMA_SAR */
#define MCF_DMA_SAR_SAR(x)              (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_DMA_DAR */
#define MCF_DMA_DAR_DAR(x)              (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_DMA_DSR */
#define MCF_DMA_DSR_DONE                (0x01)
#define MCF_DMA_DSR_BSY                 (0x02)
#define MCF_DMA_DSR_REQ                 (0x04)
#define MCF_DMA_DSR_BED                 (0x10)
#define MCF_DMA_DSR_BES                 (0x20)
#define MCF_DMA_DSR_CE                  (0x40)

/* Bit definitions and macros for MCF_DMA_BCR */
#define MCF_DMA_BCR_BCR(x)              (((x)&0x00FFFFFF)<<0)
#define MCF_DMA_BCR_DSR(x)              (((x)&0x000000FF)<<24)

/* Bit definitions and macros for MCF_DMA_DCR */
#define MCF_DMA_DCR_LCH2(x)             (((x)&0x00000003)<<0)
#define MCF_DMA_DCR_LCH1(x)             (((x)&0x00000003)<<2)
#define MCF_DMA_DCR_LINKCC(x)           (((x)&0x00000003)<<4)
#define MCF_DMA_DCR_D_REQ               (0x00000080)
#define MCF_DMA_DCR_DMOD(x)             (((x)&0x0000000F)<<8)
#define MCF_DMA_DCR_SMOD(x)             (((x)&0x0000000F)<<12)
#define MCF_DMA_DCR_START               (0x00010000)
#define MCF_DMA_DCR_DSIZE(x)            (((x)&0x00000003)<<17)
#define MCF_DMA_DCR_DINC                (0x00080000)
#define MCF_DMA_DCR_SSIZE(x)            (((x)&0x00000003)<<20)
#define MCF_DMA_DCR_SINC                (0x00400000)
#define MCF_DMA_DCR_BWC(x)              (((x)&0x00000007)<<25)
#define MCF_DMA_DCR_AA                  (0x10000000)
#define MCF_DMA_DCR_CS                  (0x20000000)
#define MCF_DMA_DCR_EEXT                (0x40000000)
#define MCF_DMA_DCR_INT                 (0x80000000)
#define MCF_DMA_DCR_BWC_16K             (0x1)
#define MCF_DMA_DCR_BWC_32K             (0x2)
#define MCF_DMA_DCR_BWC_64K             (0x3)
#define MCF_DMA_DCR_BWC_128K            (0x4)
#define MCF_DMA_DCR_BWC_256K            (0x5)
#define MCF_DMA_DCR_BWC_512K            (0x6)
#define MCF_DMA_DCR_BWC_1024K           (0x7)
#define MCF_DMA_DCR_DMOD_DIS            (0x0)
#define MCF_DMA_DCR_DMOD_16             (0x1)
#define MCF_DMA_DCR_DMOD_32             (0x2)
#define MCF_DMA_DCR_DMOD_64             (0x3)
#define MCF_DMA_DCR_DMOD_128            (0x4)
#define MCF_DMA_DCR_DMOD_256            (0x5)
#define MCF_DMA_DCR_DMOD_512            (0x6)
#define MCF_DMA_DCR_DMOD_1K             (0x7)
#define MCF_DMA_DCR_DMOD_2K             (0x8)
#define MCF_DMA_DCR_DMOD_4K             (0x9)
#define MCF_DMA_DCR_DMOD_8K             (0xA)
#define MCF_DMA_DCR_DMOD_16K            (0xB)
#define MCF_DMA_DCR_DMOD_32K            (0xC)
#define MCF_DMA_DCR_DMOD_64K            (0xD)
#define MCF_DMA_DCR_DMOD_128K           (0xE)
#define MCF_DMA_DCR_DMOD_256K           (0xF)
#define MCF_DMA_DCR_SMOD_DIS            (0x0)
#define MCF_DMA_DCR_SMOD_16             (0x1)
#define MCF_DMA_DCR_SMOD_32             (0x2)
#define MCF_DMA_DCR_SMOD_64             (0x3)
#define MCF_DMA_DCR_SMOD_128            (0x4)
#define MCF_DMA_DCR_SMOD_256            (0x5)
#define MCF_DMA_DCR_SMOD_512            (0x6)
#define MCF_DMA_DCR_SMOD_1K             (0x7)
#define MCF_DMA_DCR_SMOD_2K             (0x8)
#define MCF_DMA_DCR_SMOD_4K             (0x9)
#define MCF_DMA_DCR_SMOD_8K             (0xA)
#define MCF_DMA_DCR_SMOD_16K            (0xB)
#define MCF_DMA_DCR_SMOD_32K            (0xC)
#define MCF_DMA_DCR_SMOD_64K            (0xD)
#define MCF_DMA_DCR_SMOD_128K           (0xE)
#define MCF_DMA_DCR_SMOD_256K           (0xF)
#define MCF_DMA_DCR_SSIZE_LONG          (0x0)
#define MCF_DMA_DCR_SSIZE_BYTE          (0x1)
#define MCF_DMA_DCR_SSIZE_WORD          (0x2)
#define MCF_DMA_DCR_SSIZE_LINE          (0x3)
#define MCF_DMA_DCR_DSIZE_LONG          (0x0)
#define MCF_DMA_DCR_DSIZE_BYTE          (0x1)
#define MCF_DMA_DCR_DSIZE_WORD          (0x2)
#define MCF_DMA_DCR_DSIZE_LINE          (0x3)
#define MCF_DMA_DCR_LCH1_CH0            (0x0)
#define MCF_DMA_DCR_LCH1_CH1            (0x1)
#define MCF_DMA_DCR_LCH1_CH2            (0x2)
#define MCF_DMA_DCR_LCH1_CH3            (0x3)
#define MCF_DMA_DCR_LCH2_CH0            (0x0)
#define MCF_DMA_DCR_LCH2_CH1            (0x1)
#define MCF_DMA_DCR_LCH2_CH2            (0x2)
#define MCF_DMA_DCR_LCH2_CH3            (0x3)


/*********************************************************************
*
* Universal Asynchronous Receiver Transmitter (UART)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_UART0_UMR                 (*(vuint8 *)(&__IPSBAR[0x000200]))
#define MCF_UART0_USR                 (*(vuint8 *)(&__IPSBAR[0x000204]))
#define MCF_UART0_UCSR                (*(vuint8 *)(&__IPSBAR[0x000204]))
#define MCF_UART0_UCR                 (*(vuint8 *)(&__IPSBAR[0x000208]))
#define MCF_UART0_URB                 (*(vuint8 *)(&__IPSBAR[0x00020C]))
#define MCF_UART0_UTB                 (*(vuint8 *)(&__IPSBAR[0x00020C]))
#define MCF_UART0_UIPCR               (*(vuint8 *)(&__IPSBAR[0x000210]))
#define MCF_UART0_UACR                (*(vuint8 *)(&__IPSBAR[0x000210]))
#define MCF_UART0_UISR                (*(vuint8 *)(&__IPSBAR[0x000214]))
#define MCF_UART0_UIMR                (*(vuint8 *)(&__IPSBAR[0x000214]))
#define MCF_UART0_UBG1                (*(vuint8 *)(&__IPSBAR[0x000218]))
#define MCF_UART0_UBG2                (*(vuint8 *)(&__IPSBAR[0x00021C]))
#define MCF_UART0_UIP                 (*(vuint8 *)(&__IPSBAR[0x000234]))
#define MCF_UART0_UOP1                (*(vuint8 *)(&__IPSBAR[0x000238]))
#define MCF_UART0_UOP0                (*(vuint8 *)(&__IPSBAR[0x00023C]))
#define MCF_UART1_UMR                 (*(vuint8 *)(&__IPSBAR[0x000240]))
#define MCF_UART1_USR                 (*(vuint8 *)(&__IPSBAR[0x000244]))
#define MCF_UART1_UCSR                (*(vuint8 *)(&__IPSBAR[0x000244]))
#define MCF_UART1_UCR                 (*(vuint8 *)(&__IPSBAR[0x000248]))
#define MCF_UART1_URB                 (*(vuint8 *)(&__IPSBAR[0x00024C]))
#define MCF_UART1_UTB                 (*(vuint8 *)(&__IPSBAR[0x00024C]))
#define MCF_UART1_UIPCR               (*(vuint8 *)(&__IPSBAR[0x000250]))
#define MCF_UART1_UACR                (*(vuint8 *)(&__IPSBAR[0x000250]))
#define MCF_UART1_UISR                (*(vuint8 *)(&__IPSBAR[0x000254]))
#define MCF_UART1_UIMR                (*(vuint8 *)(&__IPSBAR[0x000254]))
#define MCF_UART1_UBG1                (*(vuint8 *)(&__IPSBAR[0x000258]))
#define MCF_UART1_UBG2                (*(vuint8 *)(&__IPSBAR[0x00025C]))
#define MCF_UART1_UIP                 (*(vuint8 *)(&__IPSBAR[0x000274]))
#define MCF_UART1_UOP1                (*(vuint8 *)(&__IPSBAR[0x000278]))
#define MCF_UART1_UOP0                (*(vuint8 *)(&__IPSBAR[0x00027C]))
#define MCF_UART2_UMR                 (*(vuint8 *)(&__IPSBAR[0x000280]))
#define MCF_UART2_USR                 (*(vuint8 *)(&__IPSBAR[0x000284]))
#define MCF_UART2_UCSR                (*(vuint8 *)(&__IPSBAR[0x000284]))
#define MCF_UART2_UCR                 (*(vuint8 *)(&__IPSBAR[0x000288]))
#define MCF_UART2_URB                 (*(vuint8 *)(&__IPSBAR[0x00028C]))
#define MCF_UART2_UTB                 (*(vuint8 *)(&__IPSBAR[0x00028C]))
#define MCF_UART2_UIPCR               (*(vuint8 *)(&__IPSBAR[0x000290]))
#define MCF_UART2_UACR                (*(vuint8 *)(&__IPSBAR[0x000290]))
#define MCF_UART2_UISR                (*(vuint8 *)(&__IPSBAR[0x000294]))
#define MCF_UART2_UIMR                (*(vuint8 *)(&__IPSBAR[0x000294]))
#define MCF_UART2_UBG1                (*(vuint8 *)(&__IPSBAR[0x000298]))
#define MCF_UART2_UBG2                (*(vuint8 *)(&__IPSBAR[0x00029C]))
#define MCF_UART2_UIP                 (*(vuint8 *)(&__IPSBAR[0x0002B4]))
#define MCF_UART2_UOP1                (*(vuint8 *)(&__IPSBAR[0x0002B8]))
#define MCF_UART2_UOP0                (*(vuint8 *)(&__IPSBAR[0x0002BC]))
#define MCF_UART_UMR(x)               (*(vuint8 *)(&__IPSBAR[0x000200+((x)*0x040)]))
#define MCF_UART_USR(x)               (*(vuint8 *)(&__IPSBAR[0x000204+((x)*0x040)]))
#define MCF_UART_UCSR(x)              (*(vuint8 *)(&__IPSBAR[0x000204+((x)*0x040)]))
#define MCF_UART_UCR(x)               (*(vuint8 *)(&__IPSBAR[0x000208+((x)*0x040)]))
#define MCF_UART_URB(x)               (*(vuint8 *)(&__IPSBAR[0x00020C+((x)*0x040)]))
#define MCF_UART_UTB(x)               (*(vuint8 *)(&__IPSBAR[0x00020C+((x)*0x040)]))
#define MCF_UART_UIPCR(x)             (*(vuint8 *)(&__IPSBAR[0x000210+((x)*0x040)]))
#define MCF_UART_UACR(x)              (*(vuint8 *)(&__IPSBAR[0x000210+((x)*0x040)]))
#define MCF_UART_UISR(x)              (*(vuint8 *)(&__IPSBAR[0x000214+((x)*0x040)]))
#define MCF_UART_UIMR(x)              (*(vuint8 *)(&__IPSBAR[0x000214+((x)*0x040)]))
#define MCF_UART_UBG1(x)              (*(vuint8 *)(&__IPSBAR[0x000218+((x)*0x040)]))
#define MCF_UART_UBG2(x)              (*(vuint8 *)(&__IPSBAR[0x00021C+((x)*0x040)]))
#define MCF_UART_UIP(x)               (*(vuint8 *)(&__IPSBAR[0x000234+((x)*0x040)]))
#define MCF_UART_UOP1(x)              (*(vuint8 *)(&__IPSBAR[0x000238+((x)*0x040)]))
#define MCF_UART_UOP0(x)              (*(vuint8 *)(&__IPSBAR[0x00023C+((x)*0x040)]))

/* Bit definitions and macros for MCF_UART_UMR */
#define MCF_UART_UMR_BC(x)            (((x)&0x03)<<0)
#define MCF_UART_UMR_PT               (0x04)
#define MCF_UART_UMR_PM(x)            (((x)&0x03)<<3)
#define MCF_UART_UMR_ERR              (0x20)
#define MCF_UART_UMR_RXIRQ            (0x40)
#define MCF_UART_UMR_RXRTS            (0x80)
#define MCF_UART_UMR_SB(x)            (((x)&0x0F)<<0)
#define MCF_UART_UMR_TXCTS            (0x10)
#define MCF_UART_UMR_TXRTS            (0x20)
#define MCF_UART_UMR_CM(x)            (((x)&0x03)<<6)
#define MCF_UART_UMR_PM_MULTI_ADDR    (0x1C)
#define MCF_UART_UMR_PM_MULTI_DATA    (0x18)
#define MCF_UART_UMR_PM_NONE          (0x10)
#define MCF_UART_UMR_PM_FORCE_HI      (0x0C)
#define MCF_UART_UMR_PM_FORCE_LO      (0x08)
#define MCF_UART_UMR_PM_ODD           (0x04)
#define MCF_UART_UMR_PM_EVEN          (0x00)
#define MCF_UART_UMR_BC_5             (0x00)
#define MCF_UART_UMR_BC_6             (0x01)
#define MCF_UART_UMR_BC_7             (0x02)
#define MCF_UART_UMR_BC_8             (0x03)
#define MCF_UART_UMR_CM_NORMAL        (0x00)
#define MCF_UART_UMR_CM_ECHO          (0x40)
#define MCF_UART_UMR_CM_LOCAL_LOOP    (0x80)
#define MCF_UART_UMR_CM_REMOTE_LOOP   (0xC0)
#define MCF_UART_UMR_SB_STOP_BITS_1   (0x07)
#define MCF_UART_UMR_SB_STOP_BITS_15  (0x08)
#define MCF_UART_UMR_SB_STOP_BITS_2   (0x0F)

/* Bit definitions and macros for MCF_UART_USR */
#define MCF_UART_USR_RXRDY            (0x01)
#define MCF_UART_USR_FFULL            (0x02)
#define MCF_UART_USR_TXRDY            (0x04)
#define MCF_UART_USR_TXEMP            (0x08)
#define MCF_UART_USR_OE               (0x10)
#define MCF_UART_USR_PE               (0x20)
#define MCF_UART_USR_FE               (0x40)
#define MCF_UART_USR_RB               (0x80)

/* Bit definitions and macros for MCF_UART_UCSR */
#define MCF_UART_UCSR_TCS(x)          (((x)&0x0F)<<0)
#define MCF_UART_UCSR_RCS(x)          (((x)&0x0F)<<4)
#define MCF_UART_UCSR_RCS_SYS_CLK     (0xD0)
#define MCF_UART_UCSR_RCS_CTM16       (0xE0)
#define MCF_UART_UCSR_RCS_CTM         (0xF0)
#define MCF_UART_UCSR_TCS_SYS_CLK     (0x0D)
#define MCF_UART_UCSR_TCS_CTM16       (0x0E)
#define MCF_UART_UCSR_TCS_CTM         (0x0F)

/* Bit definitions and macros for MCF_UART_UCR */
#define MCF_UART_UCR_RXC(x)           (((x)&0x03)<<0)
#define MCF_UART_UCR_TXC(x)           (((x)&0x03)<<2)
#define MCF_UART_UCR_MISC(x)          (((x)&0x07)<<4)
#define MCF_UART_UCR_NONE             (0x00)
#define MCF_UART_UCR_STOP_BREAK       (0x70)
#define MCF_UART_UCR_START_BREAK      (0x60)
#define MCF_UART_UCR_BKCHGINT         (0x50)
#define MCF_UART_UCR_RESET_ERROR      (0x40)
#define MCF_UART_UCR_RESET_TX         (0x30)
#define MCF_UART_UCR_RESET_RX         (0x20)
#define MCF_UART_UCR_RESET_MR         (0x10)
#define MCF_UART_UCR_TX_DISABLED      (0x08)
#define MCF_UART_UCR_TX_ENABLED       (0x04)
#define MCF_UART_UCR_RX_DISABLED      (0x02)
#define MCF_UART_UCR_RX_ENABLED       (0x01)

/* Bit definitions and macros for MCF_UART_UIPCR */
#define MCF_UART_UIPCR_CTS            (0x01)
#define MCF_UART_UIPCR_COS            (0x10)

/* Bit definitions and macros for MCF_UART_UACR */
#define MCF_UART_UACR_IEC             (0x01)

/* Bit definitions and macros for MCF_UART_UISR */
#define MCF_UART_UISR_TXRDY           (0x01)
#define MCF_UART_UISR_RXRDY_FU        (0x02)
#define MCF_UART_UISR_DB              (0x04)
#define MCF_UART_UISR_RXFTO           (0x08)
#define MCF_UART_UISR_TXFIFO          (0x10)
#define MCF_UART_UISR_RXFIFO          (0x20)
#define MCF_UART_UISR_COS             (0x80)

/* Bit definitions and macros for MCF_UART_UIMR */
#define MCF_UART_UIMR_TXRDY           (0x01)
#define MCF_UART_UIMR_RXRDY_FU        (0x02)
#define MCF_UART_UIMR_DB              (0x04)
#define MCF_UART_UIMR_COS             (0x80)

/* Bit definitions and macros for MCF_UART_UIP */
#define MCF_UART_UIP_CTS              (0x01)

/* Bit definitions and macros for MCF_UART_UOP1 */
#define MCF_UART_UOP1_RTS             (0x01)

/* Bit definitions and macros for MCF_UART_UOP0 */
#define MCF_UART_UOP0_RTS             (0x01)

/*********************************************************************
*
* I2C Module (I2C)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_I2C_I2AR          (*(vuint8 *)(&__IPSBAR[0x000300]))
#define MCF_I2C_I2FDR         (*(vuint8 *)(&__IPSBAR[0x000304]))
#define MCF_I2C_I2CR          (*(vuint8 *)(&__IPSBAR[0x000308]))
#define MCF_I2C_I2SR          (*(vuint8 *)(&__IPSBAR[0x00030C]))
#define MCF_I2C_I2DR          (*(vuint8 *)(&__IPSBAR[0x000310]))

/* Bit definitions and macros for MCF_I2C_I2AR */
#define MCF_I2C_I2AR_ADR(x)   (((x)&0x7F)<<1)

/* Bit definitions and macros for MCF_I2C_I2FDR */
#define MCF_I2C_I2FDR_IC(x)   (((x)&0x3F)<<0)

/* Bit definitions and macros for MCF_I2C_I2CR */
#define MCF_I2C_I2CR_RSTA     (0x04)
#define MCF_I2C_I2CR_TXAK     (0x08)
#define MCF_I2C_I2CR_MTX      (0x10)
#define MCF_I2C_I2CR_MSTA     (0x20)
#define MCF_I2C_I2CR_IIEN     (0x40)
#define MCF_I2C_I2CR_IEN      (0x80)

/* Bit definitions and macros for MCF_I2C_I2SR */
#define MCF_I2C_I2SR_RXAK     (0x01)
#define MCF_I2C_I2SR_IIF      (0x02)
#define MCF_I2C_I2SR_SRW      (0x04)
#define MCF_I2C_I2SR_IAL      (0x10)
#define MCF_I2C_I2SR_IBB      (0x20)
#define MCF_I2C_I2SR_IAAS     (0x40)
#define MCF_I2C_I2SR_ICF      (0x80)

/* Bit definitions and macros for MCF_I2C_I2DR */
#define MCF_I2C_I2DR_DATA(x)  (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_I2C_I2ICR */
#define MCF_I2C_I2ICR_IE      (0x01)
#define MCF_I2C_I2ICR_RE      (0x02)
#define MCF_I2C_I2ICR_TE      (0x04)
#define MCF_I2C_I2ICR_BNBE    (0x08)

/*********************************************************************
*
* Queued Serial Peripheral Interface (QSPI)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_QSPI_QMR           (*(vuint16*)(&__IPSBAR[0x000340]))
#define MCF_QSPI_QDLYR         (*(vuint16*)(&__IPSBAR[0x000344]))
#define MCF_QSPI_QWR           (*(vuint16*)(&__IPSBAR[0x000348]))
#define MCF_QSPI_QIR           (*(vuint16*)(&__IPSBAR[0x00034C]))
#define MCF_QSPI_QAR           (*(vuint16*)(&__IPSBAR[0x000350]))
#define MCF_QSPI_QDR           (*(vuint16*)(&__IPSBAR[0x000354]))

/* Bit definitions and macros for MCF_QSPI_QMR */
#define MCF_QSPI_QMR_BAUD(x)   (((x)&0x00FF)<<0)
#define MCF_QSPI_QMR_CPHA      (0x0100)
#define MCF_QSPI_QMR_CPOL      (0x0200)
#define MCF_QSPI_QMR_BITS(x)   (((x)&0x000F)<<10)
#define MCF_QSPI_QMR_DOHIE     (0x4000)
#define MCF_QSPI_QMR_MSTR      (0x8000)

/* Bit definitions and macros for MCF_QSPI_QDLYR */
#define MCF_QSPI_QDLYR_DTL(x)  (((x)&0x00FF)<<0)
#define MCF_QSPI_QDLYR_QCD(x)  (((x)&0x007F)<<8)
#define MCF_QSPI_QDLYR_SPE     (0x8000)

/* Bit definitions and macros for MCF_QSPI_QWR */
#define MCF_QSPI_QWR_NEWQP(x)  (((x)&0x000F)<<0)
#define MCF_QSPI_QWR_ENDQP(x)  (((x)&0x000F)<<8)
#define MCF_QSPI_QWR_CSIV      (0x1000)
#define MCF_QSPI_QWR_WRTO      (0x2000)
#define MCF_QSPI_QWR_WREN      (0x4000)
#define MCF_QSPI_QWR_HALT      (0x8000)

/* Bit definitions and macros for MCF_QSPI_QIR */
#define MCF_QSPI_QIR_SPIF      (0x0001)
#define MCF_QSPI_QIR_ABRT      (0x0004)
#define MCF_QSPI_QIR_WCEF      (0x0008)
#define MCF_QSPI_QIR_SPIFE     (0x0100)
#define MCF_QSPI_QIR_ABRTE     (0x0400)
#define MCF_QSPI_QIR_WCEFE     (0x0800)
#define MCF_QSPI_QIR_ABRTL     (0x1000)
#define MCF_QSPI_QIR_ABRTB     (0x4000)
#define MCF_QSPI_QIR_WCEFB     (0x8000)

/* Bit definitions and macros for MCF_QSPI_QAR */
#define MCF_QSPI_QAR_ADDR(x)   (((x)&0x003F)<<0)

/* Bit definitions and macros for MCF_QSPI_QDR */
#define MCF_QSPI_QDR_DATA(x)   (((x)&0xFFFF)<<0)

/*********************************************************************
*
* DMA Timers (DTIM)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_DTIM0_DTMR           (*(vuint16*)(&__IPSBAR[0x000400]))
#define MCF_DTIM0_DTXMR          (*(vuint8 *)(&__IPSBAR[0x000402]))
#define MCF_DTIM0_DTER           (*(vuint8 *)(&__IPSBAR[0x000403]))
#define MCF_DTIM0_DTRR           (*(vuint32*)(&__IPSBAR[0x000404]))
#define MCF_DTIM0_DTCR           (*(vuint32*)(&__IPSBAR[0x000408]))
#define MCF_DTIM0_DTCN           (*(vuint32*)(&__IPSBAR[0x00040C]))
#define MCF_DTIM1_DTMR           (*(vuint16*)(&__IPSBAR[0x000440]))
#define MCF_DTIM1_DTXMR          (*(vuint8 *)(&__IPSBAR[0x000442]))
#define MCF_DTIM1_DTER           (*(vuint8 *)(&__IPSBAR[0x000443]))
#define MCF_DTIM1_DTRR           (*(vuint32*)(&__IPSBAR[0x000444]))
#define MCF_DTIM1_DTCR           (*(vuint32*)(&__IPSBAR[0x000448]))
#define MCF_DTIM1_DTCN           (*(vuint32*)(&__IPSBAR[0x00044C]))
#define MCF_DTIM2_DTMR           (*(vuint16*)(&__IPSBAR[0x000480]))
#define MCF_DTIM2_DTXMR          (*(vuint8 *)(&__IPSBAR[0x000482]))
#define MCF_DTIM2_DTER           (*(vuint8 *)(&__IPSBAR[0x000483]))
#define MCF_DTIM2_DTRR           (*(vuint32*)(&__IPSBAR[0x000484]))
#define MCF_DTIM2_DTCR           (*(vuint32*)(&__IPSBAR[0x000488]))
#define MCF_DTIM2_DTCN           (*(vuint32*)(&__IPSBAR[0x00048C]))
#define MCF_DTIM3_DTMR           (*(vuint16*)(&__IPSBAR[0x0004C0]))
#define MCF_DTIM3_DTXMR          (*(vuint8 *)(&__IPSBAR[0x0004C2]))
#define MCF_DTIM3_DTER           (*(vuint8 *)(&__IPSBAR[0x0004C3]))
#define MCF_DTIM3_DTRR           (*(vuint32*)(&__IPSBAR[0x0004C4]))
#define MCF_DTIM3_DTCR           (*(vuint32*)(&__IPSBAR[0x0004C8]))
#define MCF_DTIM3_DTCN           (*(vuint32*)(&__IPSBAR[0x0004CC]))
#define MCF_DTIM_DTMR(x)         (*(vuint16*)(&__IPSBAR[0x000400+((x)*0x040)]))
#define MCF_DTIM_DTXMR(x)        (*(vuint8 *)(&__IPSBAR[0x000402+((x)*0x040)]))
#define MCF_DTIM_DTER(x)         (*(vuint8 *)(&__IPSBAR[0x000403+((x)*0x040)]))
#define MCF_DTIM_DTRR(x)         (*(vuint32*)(&__IPSBAR[0x000404+((x)*0x040)]))
#define MCF_DTIM_DTCR(x)         (*(vuint32*)(&__IPSBAR[0x000408+((x)*0x040)]))
#define MCF_DTIM_DTCN(x)         (*(vuint32*)(&__IPSBAR[0x00040C+((x)*0x040)]))

/* Bit definitions and macros for MCF_DTIM_DTMR */
#define MCF_DTIM_DTMR_RST        (0x0001)
#define MCF_DTIM_DTMR_CLK(x)     (((x)&0x0003)<<1)
#define MCF_DTIM_DTMR_FRR        (0x0008)
#define MCF_DTIM_DTMR_ORRI       (0x0010)
#define MCF_DTIM_DTMR_OM         (0x0020)
#define MCF_DTIM_DTMR_CE(x)      (((x)&0x0003)<<6)
#define MCF_DTIM_DTMR_PS(x)      (((x)&0x00FF)<<8)
#define MCF_DTIM_DTMR_CE_ANY     (0x00C0)
#define MCF_DTIM_DTMR_CE_FALL    (0x0080)
#define MCF_DTIM_DTMR_CE_RISE    (0x0040)
#define MCF_DTIM_DTMR_CE_NONE    (0x0000)
#define MCF_DTIM_DTMR_CLK_DTIN   (0x0006)
#define MCF_DTIM_DTMR_CLK_DIV16  (0x0004)
#define MCF_DTIM_DTMR_CLK_DIV1   (0x0002)
#define MCF_DTIM_DTMR_CLK_STOP   (0x0000)

/* Bit definitions and macros for MCF_DTIM_DTXMR */
#define MCF_DTIM_DTXMR_MODE16    (0x01)
#define MCF_DTIM_DTXMR_DMAEN     (0x80)

/* Bit definitions and macros for MCF_DTIM_DTER */
#define MCF_DTIM_DTER_CAP        (0x01)
#define MCF_DTIM_DTER_REF        (0x02)

/* Bit definitions and macros for MCF_DTIM_DTRR */
#define MCF_DTIM_DTRR_REF(x)     (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_DTIM_DTCR */
#define MCF_DTIM_DTCR_CAP(x)     (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_DTIM_DTCN */
#define MCF_DTIM_DTCN_CNT(x)     (((x)&0xFFFFFFFF)<<0)

/*********************************************************************
*
* Interrupt Controller (INTC)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_INTC0_IPRH             (*(vuint32*)(&__IPSBAR[0x000C00]))
#define MCF_INTC0_IPRL             (*(vuint32*)(&__IPSBAR[0x000C04]))
#define MCF_INTC0_IMRH             (*(vuint32*)(&__IPSBAR[0x000C08]))
#define MCF_INTC0_IMRL             (*(vuint32*)(&__IPSBAR[0x000C0C]))
#define MCF_INTC0_INTFRCH          (*(vuint32*)(&__IPSBAR[0x000C10]))
#define MCF_INTC0_INTFRCL          (*(vuint32*)(&__IPSBAR[0x000C14]))
#define MCF_INTC0_IRLR             (*(vuint8 *)(&__IPSBAR[0x000C18]))
#define MCF_INTC0_IACKLPR          (*(vuint8 *)(&__IPSBAR[0x000C19]))
#define MCF_INTC0_ICR1             (*(vuint8 *)(&__IPSBAR[0x000C41]))
#define MCF_INTC0_ICR2             (*(vuint8 *)(&__IPSBAR[0x000C42]))
#define MCF_INTC0_ICR3             (*(vuint8 *)(&__IPSBAR[0x000C43]))
#define MCF_INTC0_ICR4             (*(vuint8 *)(&__IPSBAR[0x000C44]))
#define MCF_INTC0_ICR5             (*(vuint8 *)(&__IPSBAR[0x000C45]))
#define MCF_INTC0_ICR6             (*(vuint8 *)(&__IPSBAR[0x000C46]))
#define MCF_INTC0_ICR7             (*(vuint8 *)(&__IPSBAR[0x000C47]))
#define MCF_INTC0_ICR8             (*(vuint8 *)(&__IPSBAR[0x000C48]))
#define MCF_INTC0_ICR9             (*(vuint8 *)(&__IPSBAR[0x000C49]))
#define MCF_INTC0_ICR10            (*(vuint8 *)(&__IPSBAR[0x000C4A]))
#define MCF_INTC0_ICR11            (*(vuint8 *)(&__IPSBAR[0x000C4B]))
#define MCF_INTC0_ICR12            (*(vuint8 *)(&__IPSBAR[0x000C4C]))
#define MCF_INTC0_ICR13            (*(vuint8 *)(&__IPSBAR[0x000C4D]))
#define MCF_INTC0_ICR14            (*(vuint8 *)(&__IPSBAR[0x000C4E]))
#define MCF_INTC0_ICR15            (*(vuint8 *)(&__IPSBAR[0x000C4F]))
#define MCF_INTC0_ICR16            (*(vuint8 *)(&__IPSBAR[0x000C50]))
#define MCF_INTC0_ICR17            (*(vuint8 *)(&__IPSBAR[0x000C51]))
#define MCF_INTC0_ICR18            (*(vuint8 *)(&__IPSBAR[0x000C52]))
#define MCF_INTC0_ICR19            (*(vuint8 *)(&__IPSBAR[0x000C53]))
#define MCF_INTC0_ICR20            (*(vuint8 *)(&__IPSBAR[0x000C54]))
#define MCF_INTC0_ICR21            (*(vuint8 *)(&__IPSBAR[0x000C55]))
#define MCF_INTC0_ICR22            (*(vuint8 *)(&__IPSBAR[0x000C56]))
#define MCF_INTC0_ICR23            (*(vuint8 *)(&__IPSBAR[0x000C57]))
#define MCF_INTC0_ICR24            (*(vuint8 *)(&__IPSBAR[0x000C58]))
#define MCF_INTC0_ICR25            (*(vuint8 *)(&__IPSBAR[0x000C59]))
#define MCF_INTC0_ICR26            (*(vuint8 *)(&__IPSBAR[0x000C5A]))
#define MCF_INTC0_ICR27            (*(vuint8 *)(&__IPSBAR[0x000C5B]))
#define MCF_INTC0_ICR28            (*(vuint8 *)(&__IPSBAR[0x000C5C]))
#define MCF_INTC0_ICR29            (*(vuint8 *)(&__IPSBAR[0x000C5D]))
#define MCF_INTC0_ICR30            (*(vuint8 *)(&__IPSBAR[0x000C5E]))
#define MCF_INTC0_ICR31            (*(vuint8 *)(&__IPSBAR[0x000C5F]))
#define MCF_INTC0_ICR32            (*(vuint8 *)(&__IPSBAR[0x000C60]))
#define MCF_INTC0_ICR33            (*(vuint8 *)(&__IPSBAR[0x000C61]))
#define MCF_INTC0_ICR34            (*(vuint8 *)(&__IPSBAR[0x000C62]))
#define MCF_INTC0_ICR35            (*(vuint8 *)(&__IPSBAR[0x000C63]))
#define MCF_INTC0_ICR36            (*(vuint8 *)(&__IPSBAR[0x000C64]))
#define MCF_INTC0_ICR37            (*(vuint8 *)(&__IPSBAR[0x000C65]))
#define MCF_INTC0_ICR38            (*(vuint8 *)(&__IPSBAR[0x000C66]))
#define MCF_INTC0_ICR39            (*(vuint8 *)(&__IPSBAR[0x000C67]))
#define MCF_INTC0_ICR40            (*(vuint8 *)(&__IPSBAR[0x000C68]))
#define MCF_INTC0_ICR41            (*(vuint8 *)(&__IPSBAR[0x000C69]))
#define MCF_INTC0_ICR42            (*(vuint8 *)(&__IPSBAR[0x000C6A]))
#define MCF_INTC0_ICR43            (*(vuint8 *)(&__IPSBAR[0x000C6B]))
#define MCF_INTC0_ICR44            (*(vuint8 *)(&__IPSBAR[0x000C6C]))
#define MCF_INTC0_ICR45            (*(vuint8 *)(&__IPSBAR[0x000C6D]))
#define MCF_INTC0_ICR46            (*(vuint8 *)(&__IPSBAR[0x000C6E]))
#define MCF_INTC0_ICR47            (*(vuint8 *)(&__IPSBAR[0x000C6F]))
#define MCF_INTC0_ICR48            (*(vuint8 *)(&__IPSBAR[0x000C70]))
#define MCF_INTC0_ICR49            (*(vuint8 *)(&__IPSBAR[0x000C71]))
#define MCF_INTC0_ICR50            (*(vuint8 *)(&__IPSBAR[0x000C72]))
#define MCF_INTC0_ICR51            (*(vuint8 *)(&__IPSBAR[0x000C73]))
#define MCF_INTC0_ICR52            (*(vuint8 *)(&__IPSBAR[0x000C74]))
#define MCF_INTC0_ICR53            (*(vuint8 *)(&__IPSBAR[0x000C75]))
#define MCF_INTC0_ICR54            (*(vuint8 *)(&__IPSBAR[0x000C76]))
#define MCF_INTC0_ICR55            (*(vuint8 *)(&__IPSBAR[0x000C77]))
#define MCF_INTC0_ICR56            (*(vuint8 *)(&__IPSBAR[0x000C78]))
#define MCF_INTC0_ICR57            (*(vuint8 *)(&__IPSBAR[0x000C79]))
#define MCF_INTC0_ICR58            (*(vuint8 *)(&__IPSBAR[0x000C7A]))
#define MCF_INTC0_ICR59            (*(vuint8 *)(&__IPSBAR[0x000C7B]))
#define MCF_INTC0_ICR60            (*(vuint8 *)(&__IPSBAR[0x000C7C]))
#define MCF_INTC0_ICR61            (*(vuint8 *)(&__IPSBAR[0x000C7D]))
#define MCF_INTC0_ICR62            (*(vuint8 *)(&__IPSBAR[0x000C7E]))
#define MCF_INTC0_ICR63            (*(vuint8 *)(&__IPSBAR[0x000C7F]))
#define MCF_INTC0_ICR(x)           (*(vuint8 *)(&__IPSBAR[0x000C41+((x-1)*0x001)]))
#define MCF_INTC0_SWIACK           (*(vuint8 *)(&__IPSBAR[0x000CE0]))
#define MCF_INTC0_L1IACK           (*(vuint8 *)(&__IPSBAR[0x000CE4]))
#define MCF_INTC0_L2IACK           (*(vuint8 *)(&__IPSBAR[0x000CE8]))
#define MCF_INTC0_L3IACK           (*(vuint8 *)(&__IPSBAR[0x000CEC]))
#define MCF_INTC0_L4IACK           (*(vuint8 *)(&__IPSBAR[0x000CF0]))
#define MCF_INTC0_L5IACK           (*(vuint8 *)(&__IPSBAR[0x000CF4]))
#define MCF_INTC0_L6IACK           (*(vuint8 *)(&__IPSBAR[0x000CF8]))
#define MCF_INTC0_L7IACK           (*(vuint8 *)(&__IPSBAR[0x000CFC]))
#define MCF_INTC0_LIACK(x)         (*(vuint8 *)(&__IPSBAR[0x000CE4+((x-1)*0x004)]))
#define MCF_INTC1_IPRH             (*(vuint32*)(&__IPSBAR[0x000D00]))
#define MCF_INTC1_IPRL             (*(vuint32*)(&__IPSBAR[0x000D04]))
#define MCF_INTC1_IMRH             (*(vuint32*)(&__IPSBAR[0x000D08]))
#define MCF_INTC1_IMRL             (*(vuint32*)(&__IPSBAR[0x000D0C]))
#define MCF_INTC1_INTFRCH          (*(vuint32*)(&__IPSBAR[0x000D10]))
#define MCF_INTC1_INTFRCL          (*(vuint32*)(&__IPSBAR[0x000D14]))
#define MCF_INTC1_IRLR             (*(vuint8 *)(&__IPSBAR[0x000D18]))
#define MCF_INTC1_IACKLPR          (*(vuint8 *)(&__IPSBAR[0x000D19]))
#define MCF_INTC1_ICR1             (*(vuint8 *)(&__IPSBAR[0x000D41]))
#define MCF_INTC1_ICR2             (*(vuint8 *)(&__IPSBAR[0x000D42]))
#define MCF_INTC1_ICR3             (*(vuint8 *)(&__IPSBAR[0x000D43]))
#define MCF_INTC1_ICR4             (*(vuint8 *)(&__IPSBAR[0x000D44]))
#define MCF_INTC1_ICR5             (*(vuint8 *)(&__IPSBAR[0x000D45]))
#define MCF_INTC1_ICR6             (*(vuint8 *)(&__IPSBAR[0x000D46]))
#define MCF_INTC1_ICR7             (*(vuint8 *)(&__IPSBAR[0x000D47]))
#define MCF_INTC1_ICR8             (*(vuint8 *)(&__IPSBAR[0x000D48]))
#define MCF_INTC1_ICR9             (*(vuint8 *)(&__IPSBAR[0x000D49]))
#define MCF_INTC1_ICR10            (*(vuint8 *)(&__IPSBAR[0x000D4A]))
#define MCF_INTC1_ICR11            (*(vuint8 *)(&__IPSBAR[0x000D4B]))
#define MCF_INTC1_ICR12            (*(vuint8 *)(&__IPSBAR[0x000D4C]))
#define MCF_INTC1_ICR13            (*(vuint8 *)(&__IPSBAR[0x000D4D]))
#define MCF_INTC1_ICR14            (*(vuint8 *)(&__IPSBAR[0x000D4E]))
#define MCF_INTC1_ICR15            (*(vuint8 *)(&__IPSBAR[0x000D4F]))
#define MCF_INTC1_ICR16            (*(vuint8 *)(&__IPSBAR[0x000D50]))
#define MCF_INTC1_ICR17            (*(vuint8 *)(&__IPSBAR[0x000D51]))
#define MCF_INTC1_ICR18            (*(vuint8 *)(&__IPSBAR[0x000D52]))
#define MCF_INTC1_ICR19            (*(vuint8 *)(&__IPSBAR[0x000D53]))
#define MCF_INTC1_ICR20            (*(vuint8 *)(&__IPSBAR[0x000D54]))
#define MCF_INTC1_ICR21            (*(vuint8 *)(&__IPSBAR[0x000D55]))
#define MCF_INTC1_ICR22            (*(vuint8 *)(&__IPSBAR[0x000D56]))
#define MCF_INTC1_ICR23            (*(vuint8 *)(&__IPSBAR[0x000D57]))
#define MCF_INTC1_ICR24            (*(vuint8 *)(&__IPSBAR[0x000D58]))
#define MCF_INTC1_ICR25            (*(vuint8 *)(&__IPSBAR[0x000D59]))
#define MCF_INTC1_ICR26            (*(vuint8 *)(&__IPSBAR[0x000D5A]))
#define MCF_INTC1_ICR27            (*(vuint8 *)(&__IPSBAR[0x000D5B]))
#define MCF_INTC1_ICR28            (*(vuint8 *)(&__IPSBAR[0x000D5C]))
#define MCF_INTC1_ICR29            (*(vuint8 *)(&__IPSBAR[0x000D5D]))
#define MCF_INTC1_ICR30            (*(vuint8 *)(&__IPSBAR[0x000D5E]))
#define MCF_INTC1_ICR31            (*(vuint8 *)(&__IPSBAR[0x000D5F]))
#define MCF_INTC1_ICR32            (*(vuint8 *)(&__IPSBAR[0x000D60]))
#define MCF_INTC1_ICR33            (*(vuint8 *)(&__IPSBAR[0x000D61]))
#define MCF_INTC1_ICR34            (*(vuint8 *)(&__IPSBAR[0x000D62]))
#define MCF_INTC1_ICR35            (*(vuint8 *)(&__IPSBAR[0x000D63]))
#define MCF_INTC1_ICR36            (*(vuint8 *)(&__IPSBAR[0x000D64]))
#define MCF_INTC1_ICR37            (*(vuint8 *)(&__IPSBAR[0x000D65]))
#define MCF_INTC1_ICR38            (*(vuint8 *)(&__IPSBAR[0x000D66]))
#define MCF_INTC1_ICR39            (*(vuint8 *)(&__IPSBAR[0x000D67]))
#define MCF_INTC1_ICR40            (*(vuint8 *)(&__IPSBAR[0x000D68]))
#define MCF_INTC1_ICR41            (*(vuint8 *)(&__IPSBAR[0x000D69]))
#define MCF_INTC1_ICR42            (*(vuint8 *)(&__IPSBAR[0x000D6A]))
#define MCF_INTC1_ICR43            (*(vuint8 *)(&__IPSBAR[0x000D6B]))
#define MCF_INTC1_ICR44            (*(vuint8 *)(&__IPSBAR[0x000D6C]))
#define MCF_INTC1_ICR45            (*(vuint8 *)(&__IPSBAR[0x000D6D]))
#define MCF_INTC1_ICR46            (*(vuint8 *)(&__IPSBAR[0x000D6E]))
#define MCF_INTC1_ICR47            (*(vuint8 *)(&__IPSBAR[0x000D6F]))
#define MCF_INTC1_ICR48            (*(vuint8 *)(&__IPSBAR[0x000D70]))
#define MCF_INTC1_ICR49            (*(vuint8 *)(&__IPSBAR[0x000D71]))
#define MCF_INTC1_ICR50            (*(vuint8 *)(&__IPSBAR[0x000D72]))
#define MCF_INTC1_ICR51            (*(vuint8 *)(&__IPSBAR[0x000D73]))
#define MCF_INTC1_ICR52            (*(vuint8 *)(&__IPSBAR[0x000D74]))
#define MCF_INTC1_ICR53            (*(vuint8 *)(&__IPSBAR[0x000D75]))
#define MCF_INTC1_ICR54            (*(vuint8 *)(&__IPSBAR[0x000D76]))
#define MCF_INTC1_ICR55            (*(vuint8 *)(&__IPSBAR[0x000D77]))
#define MCF_INTC1_ICR56            (*(vuint8 *)(&__IPSBAR[0x000D78]))
#define MCF_INTC1_ICR57            (*(vuint8 *)(&__IPSBAR[0x000D79]))
#define MCF_INTC1_ICR58            (*(vuint8 *)(&__IPSBAR[0x000D7A]))
#define MCF_INTC1_ICR59            (*(vuint8 *)(&__IPSBAR[0x000D7B]))
#define MCF_INTC1_ICR60            (*(vuint8 *)(&__IPSBAR[0x000D7C]))
#define MCF_INTC1_ICR61            (*(vuint8 *)(&__IPSBAR[0x000D7D]))
#define MCF_INTC1_ICR62            (*(vuint8 *)(&__IPSBAR[0x000D7E]))
#define MCF_INTC1_ICR63            (*(vuint8 *)(&__IPSBAR[0x000D7F]))
#define MCF_INTC1_ICR(x)           (*(vuint8 *)(&__IPSBAR[0x000D41+((x-1)*0x001)]))
#define MCF_INTC1_SWIACK           (*(vuint8 *)(&__IPSBAR[0x000DE0]))
#define MCF_INTC1_L1IACK           (*(vuint8 *)(&__IPSBAR[0x000DE4]))
#define MCF_INTC1_L2IACK           (*(vuint8 *)(&__IPSBAR[0x000DE8]))
#define MCF_INTC1_L3IACK           (*(vuint8 *)(&__IPSBAR[0x000DEC]))
#define MCF_INTC1_L4IACK           (*(vuint8 *)(&__IPSBAR[0x000DF0]))
#define MCF_INTC1_L5IACK           (*(vuint8 *)(&__IPSBAR[0x000DF4]))
#define MCF_INTC1_L6IACK           (*(vuint8 *)(&__IPSBAR[0x000DF8]))
#define MCF_INTC1_L7IACK           (*(vuint8 *)(&__IPSBAR[0x000DFC]))
#define MCF_INTC1_LIACK(x)         (*(vuint8 *)(&__IPSBAR[0x000DE4+((x-1)*0x004)]))
#define MCF_INTC_IPRH(x)           (*(vuint32*)(&__IPSBAR[0x000C00+((x)*0x100)]))
#define MCF_INTC_IPRL(x)           (*(vuint32*)(&__IPSBAR[0x000C04+((x)*0x100)]))
#define MCF_INTC_IMRH(x)           (*(vuint32*)(&__IPSBAR[0x000C08+((x)*0x100)]))
#define MCF_INTC_IMRL(x)           (*(vuint32*)(&__IPSBAR[0x000C0C+((x)*0x100)]))
#define MCF_INTC_INTFRCH(x)        (*(vuint32*)(&__IPSBAR[0x000C10+((x)*0x100)]))
#define MCF_INTC_INTFRCL(x)        (*(vuint32*)(&__IPSBAR[0x000C14+((x)*0x100)]))
#define MCF_INTC_IRLR(x)           (*(vuint8 *)(&__IPSBAR[0x000C18+((x)*0x100)]))
#define MCF_INTC_IACKLPR(x)        (*(vuint8 *)(&__IPSBAR[0x000C19+((x)*0x100)]))
#define MCF_INTC_ICR1(x)           (*(vuint8 *)(&__IPSBAR[0x000C41+((x)*0x100)]))
#define MCF_INTC_ICR2(x)           (*(vuint8 *)(&__IPSBAR[0x000C42+((x)*0x100)]))
#define MCF_INTC_ICR3(x)           (*(vuint8 *)(&__IPSBAR[0x000C43+((x)*0x100)]))
#define MCF_INTC_ICR4(x)           (*(vuint8 *)(&__IPSBAR[0x000C44+((x)*0x100)]))
#define MCF_INTC_ICR5(x)           (*(vuint8 *)(&__IPSBAR[0x000C45+((x)*0x100)]))
#define MCF_INTC_ICR6(x)           (*(vuint8 *)(&__IPSBAR[0x000C46+((x)*0x100)]))
#define MCF_INTC_ICR7(x)           (*(vuint8 *)(&__IPSBAR[0x000C47+((x)*0x100)]))
#define MCF_INTC_ICR8(x)           (*(vuint8 *)(&__IPSBAR[0x000C48+((x)*0x100)]))
#define MCF_INTC_ICR9(x)           (*(vuint8 *)(&__IPSBAR[0x000C49+((x)*0x100)]))
#define MCF_INTC_ICR10(x)          (*(vuint8 *)(&__IPSBAR[0x000C4A+((x)*0x100)]))
#define MCF_INTC_ICR11(x)          (*(vuint8 *)(&__IPSBAR[0x000C4B+((x)*0x100)]))
#define MCF_INTC_ICR12(x)          (*(vuint8 *)(&__IPSBAR[0x000C4C+((x)*0x100)]))
#define MCF_INTC_ICR13(x)          (*(vuint8 *)(&__IPSBAR[0x000C4D+((x)*0x100)]))
#define MCF_INTC_ICR14(x)          (*(vuint8 *)(&__IPSBAR[0x000C4E+((x)*0x100)]))
#define MCF_INTC_ICR15(x)          (*(vuint8 *)(&__IPSBAR[0x000C4F+((x)*0x100)]))
#define MCF_INTC_ICR16(x)          (*(vuint8 *)(&__IPSBAR[0x000C50+((x)*0x100)]))
#define MCF_INTC_ICR17(x)          (*(vuint8 *)(&__IPSBAR[0x000C51+((x)*0x100)]))
#define MCF_INTC_ICR18(x)          (*(vuint8 *)(&__IPSBAR[0x000C52+((x)*0x100)]))
#define MCF_INTC_ICR19(x)          (*(vuint8 *)(&__IPSBAR[0x000C53+((x)*0x100)]))
#define MCF_INTC_ICR20(x)          (*(vuint8 *)(&__IPSBAR[0x000C54+((x)*0x100)]))
#define MCF_INTC_ICR21(x)          (*(vuint8 *)(&__IPSBAR[0x000C55+((x)*0x100)]))
#define MCF_INTC_ICR22(x)          (*(vuint8 *)(&__IPSBAR[0x000C56+((x)*0x100)]))
#define MCF_INTC_ICR23(x)          (*(vuint8 *)(&__IPSBAR[0x000C57+((x)*0x100)]))
#define MCF_INTC_ICR24(x)          (*(vuint8 *)(&__IPSBAR[0x000C58+((x)*0x100)]))
#define MCF_INTC_ICR25(x)          (*(vuint8 *)(&__IPSBAR[0x000C59+((x)*0x100)]))
#define MCF_INTC_ICR26(x)          (*(vuint8 *)(&__IPSBAR[0x000C5A+((x)*0x100)]))
#define MCF_INTC_ICR27(x)          (*(vuint8 *)(&__IPSBAR[0x000C5B+((x)*0x100)]))
#define MCF_INTC_ICR28(x)          (*(vuint8 *)(&__IPSBAR[0x000C5C+((x)*0x100)]))
#define MCF_INTC_ICR29(x)          (*(vuint8 *)(&__IPSBAR[0x000C5D+((x)*0x100)]))
#define MCF_INTC_ICR30(x)          (*(vuint8 *)(&__IPSBAR[0x000C5E+((x)*0x100)]))
#define MCF_INTC_ICR31(x)          (*(vuint8 *)(&__IPSBAR[0x000C5F+((x)*0x100)]))
#define MCF_INTC_ICR32(x)          (*(vuint8 *)(&__IPSBAR[0x000C60+((x)*0x100)]))
#define MCF_INTC_ICR33(x)          (*(vuint8 *)(&__IPSBAR[0x000C61+((x)*0x100)]))
#define MCF_INTC_ICR34(x)          (*(vuint8 *)(&__IPSBAR[0x000C62+((x)*0x100)]))
#define MCF_INTC_ICR35(x)          (*(vuint8 *)(&__IPSBAR[0x000C63+((x)*0x100)]))
#define MCF_INTC_ICR36(x)          (*(vuint8 *)(&__IPSBAR[0x000C64+((x)*0x100)]))
#define MCF_INTC_ICR37(x)          (*(vuint8 *)(&__IPSBAR[0x000C65+((x)*0x100)]))
#define MCF_INTC_ICR38(x)          (*(vuint8 *)(&__IPSBAR[0x000C66+((x)*0x100)]))
#define MCF_INTC_ICR39(x)          (*(vuint8 *)(&__IPSBAR[0x000C67+((x)*0x100)]))
#define MCF_INTC_ICR40(x)          (*(vuint8 *)(&__IPSBAR[0x000C68+((x)*0x100)]))
#define MCF_INTC_ICR41(x)          (*(vuint8 *)(&__IPSBAR[0x000C69+((x)*0x100)]))
#define MCF_INTC_ICR42(x)          (*(vuint8 *)(&__IPSBAR[0x000C6A+((x)*0x100)]))
#define MCF_INTC_ICR43(x)          (*(vuint8 *)(&__IPSBAR[0x000C6B+((x)*0x100)]))
#define MCF_INTC_ICR44(x)          (*(vuint8 *)(&__IPSBAR[0x000C6C+((x)*0x100)]))
#define MCF_INTC_ICR45(x)          (*(vuint8 *)(&__IPSBAR[0x000C6D+((x)*0x100)]))
#define MCF_INTC_ICR46(x)          (*(vuint8 *)(&__IPSBAR[0x000C6E+((x)*0x100)]))
#define MCF_INTC_ICR47(x)          (*(vuint8 *)(&__IPSBAR[0x000C6F+((x)*0x100)]))
#define MCF_INTC_ICR48(x)          (*(vuint8 *)(&__IPSBAR[0x000C70+((x)*0x100)]))
#define MCF_INTC_ICR49(x)          (*(vuint8 *)(&__IPSBAR[0x000C71+((x)*0x100)]))
#define MCF_INTC_ICR50(x)          (*(vuint8 *)(&__IPSBAR[0x000C72+((x)*0x100)]))
#define MCF_INTC_ICR51(x)          (*(vuint8 *)(&__IPSBAR[0x000C73+((x)*0x100)]))
#define MCF_INTC_ICR52(x)          (*(vuint8 *)(&__IPSBAR[0x000C74+((x)*0x100)]))
#define MCF_INTC_ICR53(x)          (*(vuint8 *)(&__IPSBAR[0x000C75+((x)*0x100)]))
#define MCF_INTC_ICR54(x)          (*(vuint8 *)(&__IPSBAR[0x000C76+((x)*0x100)]))
#define MCF_INTC_ICR55(x)          (*(vuint8 *)(&__IPSBAR[0x000C77+((x)*0x100)]))
#define MCF_INTC_ICR56(x)          (*(vuint8 *)(&__IPSBAR[0x000C78+((x)*0x100)]))
#define MCF_INTC_ICR57(x)          (*(vuint8 *)(&__IPSBAR[0x000C79+((x)*0x100)]))
#define MCF_INTC_ICR58(x)          (*(vuint8 *)(&__IPSBAR[0x000C7A+((x)*0x100)]))
#define MCF_INTC_ICR59(x)          (*(vuint8 *)(&__IPSBAR[0x000C7B+((x)*0x100)]))
#define MCF_INTC_ICR60(x)          (*(vuint8 *)(&__IPSBAR[0x000C7C+((x)*0x100)]))
#define MCF_INTC_ICR61(x)          (*(vuint8 *)(&__IPSBAR[0x000C7D+((x)*0x100)]))
#define MCF_INTC_ICR62(x)          (*(vuint8 *)(&__IPSBAR[0x000C7E+((x)*0x100)]))
#define MCF_INTC_ICR63(x)          (*(vuint8 *)(&__IPSBAR[0x000C7F+((x)*0x100)]))
#define MCF_INTC_SWIACK(x)         (*(vuint8 *)(&__IPSBAR[0x000CE0+((x)*0x100)]))
#define MCF_INTC_L1IACK(x)         (*(vuint8 *)(&__IPSBAR[0x000CE4+((x)*0x100)]))
#define MCF_INTC_L2IACK(x)         (*(vuint8 *)(&__IPSBAR[0x000CE8+((x)*0x100)]))
#define MCF_INTC_L3IACK(x)         (*(vuint8 *)(&__IPSBAR[0x000CEC+((x)*0x100)]))
#define MCF_INTC_L4IACK(x)         (*(vuint8 *)(&__IPSBAR[0x000CF0+((x)*0x100)]))
#define MCF_INTC_L5IACK(x)         (*(vuint8 *)(&__IPSBAR[0x000CF4+((x)*0x100)]))
#define MCF_INTC_L6IACK(x)         (*(vuint8 *)(&__IPSBAR[0x000CF8+((x)*0x100)]))
#define MCF_INTC_L7IACK(x)         (*(vuint8 *)(&__IPSBAR[0x000CFC+((x)*0x100)]))

/* Bit definitions and macros for MCF_INTC_IPRH */
#define MCF_INTC_IPRH_INT32        (0x00000001)
#define MCF_INTC_IPRH_INT33        (0x00000002)
#define MCF_INTC_IPRH_INT34        (0x00000004)
#define MCF_INTC_IPRH_INT35        (0x00000008)
#define MCF_INTC_IPRH_INT36        (0x00000010)
#define MCF_INTC_IPRH_INT37        (0x00000020)
#define MCF_INTC_IPRH_INT38        (0x00000040)
#define MCF_INTC_IPRH_INT39        (0x00000080)
#define MCF_INTC_IPRH_INT40        (0x00000100)
#define MCF_INTC_IPRH_INT41        (0x00000200)
#define MCF_INTC_IPRH_INT42        (0x00000400)
#define MCF_INTC_IPRH_INT43        (0x00000800)
#define MCF_INTC_IPRH_INT44        (0x00001000)
#define MCF_INTC_IPRH_INT45        (0x00002000)
#define MCF_INTC_IPRH_INT46        (0x00004000)
#define MCF_INTC_IPRH_INT47        (0x00008000)
#define MCF_INTC_IPRH_INT48        (0x00010000)
#define MCF_INTC_IPRH_INT49        (0x00020000)
#define MCF_INTC_IPRH_INT50        (0x00040000)
#define MCF_INTC_IPRH_INT51        (0x00080000)
#define MCF_INTC_IPRH_INT52        (0x00100000)
#define MCF_INTC_IPRH_INT53        (0x00200000)
#define MCF_INTC_IPRH_INT54        (0x00400000)
#define MCF_INTC_IPRH_INT55        (0x00800000)
#define MCF_INTC_IPRH_INT56        (0x01000000)
#define MCF_INTC_IPRH_INT57        (0x02000000)
#define MCF_INTC_IPRH_INT58        (0x04000000)
#define MCF_INTC_IPRH_INT59        (0x08000000)
#define MCF_INTC_IPRH_INT60        (0x10000000)
#define MCF_INTC_IPRH_INT61        (0x20000000)
#define MCF_INTC_IPRH_INT62        (0x40000000)
#define MCF_INTC_IPRH_INT63        (0x80000000)

/* Bit definitions and macros for MCF_INTC_IPRL */
#define MCF_INTC_IPRL_INT1         (0x00000002)
#define MCF_INTC_IPRL_INT2         (0x00000004)
#define MCF_INTC_IPRL_INT3         (0x00000008)
#define MCF_INTC_IPRL_INT4         (0x00000010)
#define MCF_INTC_IPRL_INT5         (0x00000020)
#define MCF_INTC_IPRL_INT6         (0x00000040)
#define MCF_INTC_IPRL_INT7         (0x00000080)
#define MCF_INTC_IPRL_INT8         (0x00000100)
#define MCF_INTC_IPRL_INT9         (0x00000200)
#define MCF_INTC_IPRL_INT10        (0x00000400)
#define MCF_INTC_IPRL_INT11        (0x00000800)
#define MCF_INTC_IPRL_INT12        (0x00001000)
#define MCF_INTC_IPRL_INT13        (0x00002000)
#define MCF_INTC_IPRL_INT14        (0x00004000)
#define MCF_INTC_IPRL_INT15        (0x00008000)
#define MCF_INTC_IPRL_INT16        (0x00010000)
#define MCF_INTC_IPRL_INT17        (0x00020000)
#define MCF_INTC_IPRL_INT18        (0x00040000)
#define MCF_INTC_IPRL_INT19        (0x00080000)
#define MCF_INTC_IPRL_INT20        (0x00100000)
#define MCF_INTC_IPRL_INT21        (0x00200000)
#define MCF_INTC_IPRL_INT22        (0x00400000)
#define MCF_INTC_IPRL_INT23        (0x00800000)
#define MCF_INTC_IPRL_INT24        (0x01000000)
#define MCF_INTC_IPRL_INT25        (0x02000000)
#define MCF_INTC_IPRL_INT26        (0x04000000)
#define MCF_INTC_IPRL_INT27        (0x08000000)
#define MCF_INTC_IPRL_INT28        (0x10000000)
#define MCF_INTC_IPRL_INT29        (0x20000000)
#define MCF_INTC_IPRL_INT30        (0x40000000)
#define MCF_INTC_IPRL_INT31        (0x80000000)

/* Bit definitions and macros for MCF_INTC_IMRH */
#define MCF_INTC_IMRH_MASK32       (0x00000001)
#define MCF_INTC_IMRH_MASK33       (0x00000002)
#define MCF_INTC_IMRH_MASK34       (0x00000004)
#define MCF_INTC_IMRH_MASK35       (0x00000008)
#define MCF_INTC_IMRH_MASK36       (0x00000010)
#define MCF_INTC_IMRH_MASK37       (0x00000020)
#define MCF_INTC_IMRH_MASK38       (0x00000040)
#define MCF_INTC_IMRH_MASK39       (0x00000080)
#define MCF_INTC_IMRH_MASK40       (0x00000100)
#define MCF_INTC_IMRH_MASK41       (0x00000200)
#define MCF_INTC_IMRH_MASK42       (0x00000400)
#define MCF_INTC_IMRH_MASK43       (0x00000800)
#define MCF_INTC_IMRH_MASK44       (0x00001000)
#define MCF_INTC_IMRH_MASK45       (0x00002000)
#define MCF_INTC_IMRH_MASK46       (0x00004000)
#define MCF_INTC_IMRH_MASK47       (0x00008000)
#define MCF_INTC_IMRH_MASK48       (0x00010000)
#define MCF_INTC_IMRH_MASK49       (0x00020000)
#define MCF_INTC_IMRH_MASK50       (0x00040000)
#define MCF_INTC_IMRH_MASK51       (0x00080000)
#define MCF_INTC_IMRH_MASK52       (0x00100000)
#define MCF_INTC_IMRH_MASK53       (0x00200000)
#define MCF_INTC_IMRH_MASK54       (0x00400000)
#define MCF_INTC_IMRH_MASK55       (0x00800000)
#define MCF_INTC_IMRH_MASK56       (0x01000000)
#define MCF_INTC_IMRH_MASK57       (0x02000000)
#define MCF_INTC_IMRH_MASK58       (0x04000000)
#define MCF_INTC_IMRH_MASK59       (0x08000000)
#define MCF_INTC_IMRH_MASK60       (0x10000000)
#define MCF_INTC_IMRH_MASK61       (0x20000000)
#define MCF_INTC_IMRH_MASK62       (0x40000000)
#define MCF_INTC_IMRH_MASK63       (0x80000000)

/* Bit definitions and macros for MCF_INTC_IMRL */
#define MCF_INTC_IMRL_MASKALL      (0x00000001)
#define MCF_INTC_IMRL_MASK1        (0x00000002)
#define MCF_INTC_IMRL_MASK2        (0x00000004)
#define MCF_INTC_IMRL_MASK3        (0x00000008)
#define MCF_INTC_IMRL_MASK4        (0x00000010)
#define MCF_INTC_IMRL_MASK5        (0x00000020)
#define MCF_INTC_IMRL_MASK6        (0x00000040)
#define MCF_INTC_IMRL_MASK7        (0x00000080)
#define MCF_INTC_IMRL_MASK8        (0x00000100)
#define MCF_INTC_IMRL_MASK9        (0x00000200)
#define MCF_INTC_IMRL_MASK10       (0x00000400)
#define MCF_INTC_IMRL_MASK11       (0x00000800)
#define MCF_INTC_IMRL_MASK12       (0x00001000)
#define MCF_INTC_IMRL_MASK13       (0x00002000)
#define MCF_INTC_IMRL_MASK14       (0x00004000)
#define MCF_INTC_IMRL_MASK15       (0x00008000)
#define MCF_INTC_IMRL_MASK16       (0x00010000)
#define MCF_INTC_IMRL_MASK17       (0x00020000)
#define MCF_INTC_IMRL_MASK18       (0x00040000)
#define MCF_INTC_IMRL_MASK19       (0x00080000)
#define MCF_INTC_IMRL_MASK20       (0x00100000)
#define MCF_INTC_IMRL_MASK21       (0x00200000)
#define MCF_INTC_IMRL_MASK22       (0x00400000)
#define MCF_INTC_IMRL_MASK23       (0x00800000)
#define MCF_INTC_IMRL_MASK24       (0x01000000)
#define MCF_INTC_IMRL_MASK25       (0x02000000)
#define MCF_INTC_IMRL_MASK26       (0x04000000)
#define MCF_INTC_IMRL_MASK27       (0x08000000)
#define MCF_INTC_IMRL_MASK28       (0x10000000)
#define MCF_INTC_IMRL_MASK29       (0x20000000)
#define MCF_INTC_IMRL_MASK30       (0x40000000)
#define MCF_INTC_IMRL_MASK31       (0x80000000)

/* Bit definitions and macros for MCF_INTC_INTFRCH */
#define MCF_INTC_INTFRCH_INTFRC32  (0x00000001)
#define MCF_INTC_INTFRCH_INTFRC33  (0x00000002)
#define MCF_INTC_INTFRCH_INTFRC34  (0x00000004)
#define MCF_INTC_INTFRCH_INTFRC35  (0x00000008)
#define MCF_INTC_INTFRCH_INTFRC36  (0x00000010)
#define MCF_INTC_INTFRCH_INTFRC37  (0x00000020)
#define MCF_INTC_INTFRCH_INTFRC38  (0x00000040)
#define MCF_INTC_INTFRCH_INTFRC39  (0x00000080)
#define MCF_INTC_INTFRCH_INTFRC40  (0x00000100)
#define MCF_INTC_INTFRCH_INTFRC41  (0x00000200)
#define MCF_INTC_INTFRCH_INTFRC42  (0x00000400)
#define MCF_INTC_INTFRCH_INTFRC43  (0x00000800)
#define MCF_INTC_INTFRCH_INTFRC44  (0x00001000)
#define MCF_INTC_INTFRCH_INTFRC45  (0x00002000)
#define MCF_INTC_INTFRCH_INTFRC46  (0x00004000)
#define MCF_INTC_INTFRCH_INTFRC47  (0x00008000)
#define MCF_INTC_INTFRCH_INTFRC48  (0x00010000)
#define MCF_INTC_INTFRCH_INTFRC49  (0x00020000)
#define MCF_INTC_INTFRCH_INTFRC50  (0x00040000)
#define MCF_INTC_INTFRCH_INTFRC51  (0x00080000)
#define MCF_INTC_INTFRCH_INTFRC52  (0x00100000)
#define MCF_INTC_INTFRCH_INTFRC53  (0x00200000)
#define MCF_INTC_INTFRCH_INTFRC54  (0x00400000)
#define MCF_INTC_INTFRCH_INTFRC55  (0x00800000)
#define MCF_INTC_INTFRCH_INTFRC56  (0x01000000)
#define MCF_INTC_INTFRCH_INTFRC57  (0x02000000)
#define MCF_INTC_INTFRCH_INTFRC58  (0x04000000)
#define MCF_INTC_INTFRCH_INTFRC59  (0x08000000)
#define MCF_INTC_INTFRCH_INTFRC60  (0x10000000)
#define MCF_INTC_INTFRCH_INTFRC61  (0x20000000)
#define MCF_INTC_INTFRCH_INTFRC62  (0x40000000)
#define MCF_INTC_INTFRCH_INTFRC63  (0x80000000)

/* Bit definitions and macros for MCF_INTC_INTFRCL */
#define MCF_INTC_INTFRCL_INTFRC1   (0x00000002)
#define MCF_INTC_INTFRCL_INTFRC2   (0x00000004)
#define MCF_INTC_INTFRCL_INTFRC3   (0x00000008)
#define MCF_INTC_INTFRCL_INTFRC4   (0x00000010)
#define MCF_INTC_INTFRCL_INTFRC5   (0x00000020)
#define MCF_INTC_INTFRCL_INTFRC6   (0x00000040)
#define MCF_INTC_INTFRCL_INTFRC7   (0x00000080)
#define MCF_INTC_INTFRCL_INTFRC8   (0x00000100)
#define MCF_INTC_INTFRCL_INTFRC9   (0x00000200)
#define MCF_INTC_INTFRCL_INTFRC10  (0x00000400)
#define MCF_INTC_INTFRCL_INTFRC11  (0x00000800)
#define MCF_INTC_INTFRCL_INTFRC12  (0x00001000)
#define MCF_INTC_INTFRCL_INTFRC13  (0x00002000)
#define MCF_INTC_INTFRCL_INTFRC14  (0x00004000)
#define MCF_INTC_INTFRCL_INTFRC15  (0x00008000)
#define MCF_INTC_INTFRCL_INTFRC16  (0x00010000)
#define MCF_INTC_INTFRCL_INTFRC17  (0x00020000)
#define MCF_INTC_INTFRCL_INTFRC18  (0x00040000)
#define MCF_INTC_INTFRCL_INTFRC19  (0x00080000)
#define MCF_INTC_INTFRCL_INTFRC20  (0x00100000)
#define MCF_INTC_INTFRCL_INTFRC21  (0x00200000)
#define MCF_INTC_INTFRCL_INTFRC22  (0x00400000)
#define MCF_INTC_INTFRCL_INTFRC23  (0x00800000)
#define MCF_INTC_INTFRCL_INTFRC24  (0x01000000)
#define MCF_INTC_INTFRCL_INTFRC25  (0x02000000)
#define MCF_INTC_INTFRCL_INTFRC26  (0x04000000)
#define MCF_INTC_INTFRCL_INTFRC27  (0x08000000)
#define MCF_INTC_INTFRCL_INTFRC28  (0x10000000)
#define MCF_INTC_INTFRCL_INTFRC29  (0x20000000)
#define MCF_INTC_INTFRCL_INTFRC30  (0x40000000)
#define MCF_INTC_INTFRCL_INTFRC31  (0x80000000)

/* Bit definitions and macros for MCF_INTC_IRLR */
#define MCF_INTC_IRLR_IRQ(x)       (((x)&0x7F)<<1)

/* Bit definitions and macros for MCF_INTC_IACKLPR */
#define MCF_INTC_IACKLPR_PRI(x)    (((x)&0x0F)<<0)
#define MCF_INTC_IACKLPR_LEVEL(x)  (((x)&0x07)<<4)

/* Bit definitions and macros for MCF_INTC_ICR */
#define MCF_INTC_ICR_IP(x)         (((x)&0x07)<<0)
#define MCF_INTC_ICR_IL(x)         (((x)&0x07)<<3)

/* Bit definitions and macros for MCF_INTC_SWIACK */
#define MCF_INTC_SWIACK_VECTOR(x)  (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_INTC_LIACK */
#define MCF_INTC_LIACK_VECTOR(x)   (((x)&0xFF)<<0)

/*********************************************************************
*
* General Purpose I/O (GPIO)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_GPIO_PORTNQ                (*(vuint8 *)(&__IPSBAR[0x100008]))
#define MCF_GPIO_PORTAN                (*(vuint8 *)(&__IPSBAR[0x10000A]))
#define MCF_GPIO_PORTAS                (*(vuint8 *)(&__IPSBAR[0x10000B]))
#define MCF_GPIO_PORTQS                (*(vuint8 *)(&__IPSBAR[0x10000C]))
#define MCF_GPIO_PORTTA                (*(vuint8 *)(&__IPSBAR[0x10000E]))
#define MCF_GPIO_PORTTC                (*(vuint8 *)(&__IPSBAR[0x10000F]))
#define MCF_GPIO_PORTTD                (*(vuint8 *)(&__IPSBAR[0x100010]))
#define MCF_GPIO_PORTUA                (*(vuint8 *)(&__IPSBAR[0x100011]))
#define MCF_GPIO_PORTUB                (*(vuint8 *)(&__IPSBAR[0x100012]))
#define MCF_GPIO_PORTUC                (*(vuint8 *)(&__IPSBAR[0x100013]))
#define MCF_GPIO_PORTDD                (*(vuint8 *)(&__IPSBAR[0x100014]))
#define MCF_GPIO_PORTLD                (*(vuint8 *)(&__IPSBAR[0x100015]))
#define MCF_GPIO_PORTGP                (*(vuint8 *)(&__IPSBAR[0x100016]))
#define MCF_GPIO_DDRNQ                 (*(vuint8 *)(&__IPSBAR[0x100020]))
#define MCF_GPIO_DDRAN                 (*(vuint8 *)(&__IPSBAR[0x100022]))
#define MCF_GPIO_DDRAS                 (*(vuint8 *)(&__IPSBAR[0x100023]))
#define MCF_GPIO_DDRQS                 (*(vuint8 *)(&__IPSBAR[0x100024]))
#define MCF_GPIO_DDRTA                 (*(vuint8 *)(&__IPSBAR[0x100026]))
#define MCF_GPIO_DDRTC                 (*(vuint8 *)(&__IPSBAR[0x100027]))
#define MCF_GPIO_DDRTD                 (*(vuint8 *)(&__IPSBAR[0x100028]))
#define MCF_GPIO_DDRUA                 (*(vuint8 *)(&__IPSBAR[0x100029]))
#define MCF_GPIO_DDRUB                 (*(vuint8 *)(&__IPSBAR[0x10002A]))
#define MCF_GPIO_DDRUC                 (*(vuint8 *)(&__IPSBAR[0x10002B]))
#define MCF_GPIO_DDRDD                 (*(vuint8 *)(&__IPSBAR[0x10002C]))
#define MCF_GPIO_DDRLD                 (*(vuint8 *)(&__IPSBAR[0x10002D]))
#define MCF_GPIO_DDRGP                 (*(vuint8 *)(&__IPSBAR[0x10002E]))
#define MCF_GPIO_SETNQ                 (*(vuint8 *)(&__IPSBAR[0x100038]))
#define MCF_GPIO_SETAN                 (*(vuint8 *)(&__IPSBAR[0x10003A]))
#define MCF_GPIO_SETAS                 (*(vuint8 *)(&__IPSBAR[0x10003B]))
#define MCF_GPIO_SETQS                 (*(vuint8 *)(&__IPSBAR[0x10003C]))
#define MCF_GPIO_SETTA                 (*(vuint8 *)(&__IPSBAR[0x10003E]))
#define MCF_GPIO_SETTC                 (*(vuint8 *)(&__IPSBAR[0x10003F]))
#define MCF_GPIO_SETTD                 (*(vuint8 *)(&__IPSBAR[0x100040]))
#define MCF_GPIO_SETUA                 (*(vuint8 *)(&__IPSBAR[0x100041]))
#define MCF_GPIO_SETUB                 (*(vuint8 *)(&__IPSBAR[0x100042]))
#define MCF_GPIO_SETUC                 (*(vuint8 *)(&__IPSBAR[0x100043]))
#define MCF_GPIO_SETDD                 (*(vuint8 *)(&__IPSBAR[0x100044]))
#define MCF_GPIO_SETLD                 (*(vuint8 *)(&__IPSBAR[0x100045]))
#define MCF_GPIO_SETGP                 (*(vuint8 *)(&__IPSBAR[0x100046]))
#define MCF_GPIO_CLRNQ                 (*(vuint8 *)(&__IPSBAR[0x100050]))
#define MCF_GPIO_CLRAN                 (*(vuint8 *)(&__IPSBAR[0x100052]))
#define MCF_GPIO_CLRAS                 (*(vuint8 *)(&__IPSBAR[0x100053]))
#define MCF_GPIO_CLRQS                 (*(vuint8 *)(&__IPSBAR[0x100054]))
#define MCF_GPIO_CLRTA                 (*(vuint8 *)(&__IPSBAR[0x100056]))
#define MCF_GPIO_CLRTC                 (*(vuint8 *)(&__IPSBAR[0x100057]))
#define MCF_GPIO_CLRTD                 (*(vuint8 *)(&__IPSBAR[0x100058]))
#define MCF_GPIO_CLRUA                 (*(vuint8 *)(&__IPSBAR[0x100059]))
#define MCF_GPIO_CLRUB                 (*(vuint8 *)(&__IPSBAR[0x10005A]))
#define MCF_GPIO_CLRUC                 (*(vuint8 *)(&__IPSBAR[0x10005B]))
#define MCF_GPIO_CLRDD                 (*(vuint8 *)(&__IPSBAR[0x10005C]))
#define MCF_GPIO_CLRLD                 (*(vuint8 *)(&__IPSBAR[0x10005D]))
#define MCF_GPIO_CLRGP                 (*(vuint8 *)(&__IPSBAR[0x10005E]))
#define MCF_GPIO_PNQPAR                (*(vuint16*)(&__IPSBAR[0x100068]))
#define MCF_GPIO_PANPAR                (*(vuint8 *)(&__IPSBAR[0x10006A]))
#define MCF_GPIO_PASPAR                (*(vuint8 *)(&__IPSBAR[0x10006B]))
#define MCF_GPIO_PQSPAR                (*(vuint16*)(&__IPSBAR[0x10006C]))
#define MCF_GPIO_PTAPAR                (*(vuint8 *)(&__IPSBAR[0x10006E]))
#define MCF_GPIO_PTCPAR                (*(vuint8 *)(&__IPSBAR[0x10006F]))
#define MCF_GPIO_PTDPAR                (*(vuint8 *)(&__IPSBAR[0x100070]))
#define MCF_GPIO_PUAPAR                (*(vuint8 *)(&__IPSBAR[0x100071]))
#define MCF_GPIO_PUBPAR                (*(vuint8 *)(&__IPSBAR[0x100072]))
#define MCF_GPIO_PUCPAR                (*(vuint8 *)(&__IPSBAR[0x100073]))
#define MCF_GPIO_PDDPAR                (*(vuint8 *)(&__IPSBAR[0x100074]))
#define MCF_GPIO_PLDPAR                (*(vuint8 *)(&__IPSBAR[0x100075]))
#define MCF_GPIO_PGPPAR                (*(vuint8 *)(&__IPSBAR[0x100076]))
#define MCF_GPIO_PWOR                  (*(vuint16*)(&__IPSBAR[0x100078]))
#define MCF_GPIO_PDSRH                 (*(vuint16*)(&__IPSBAR[0x10007A]))
#define MCF_GPIO_PDSRL                 (*(vuint32*)(&__IPSBAR[0x10007C]))

/* Bit definitions and macros for MCF_GPIO_PORTNQ */
#define MCF_GPIO_PORTNQ_PORTNQ0        (0x01)
#define MCF_GPIO_PORTNQ_PORTNQ1        (0x02)
#define MCF_GPIO_PORTNQ_PORTNQ2        (0x04)
#define MCF_GPIO_PORTNQ_PORTNQ3        (0x08)
#define MCF_GPIO_PORTNQ_PORTNQ4        (0x10)
#define MCF_GPIO_PORTNQ_PORTNQ5        (0x20)
#define MCF_GPIO_PORTNQ_PORTNQ6        (0x40)
#define MCF_GPIO_PORTNQ_PORTNQ7        (0x80)

/* Bit definitions and macros for MCF_GPIO_PORTAN */
#define MCF_GPIO_PORTAN_PORTAN0        (0x01)
#define MCF_GPIO_PORTAN_PORTAN1        (0x02)
#define MCF_GPIO_PORTAN_PORTAN2        (0x04)
#define MCF_GPIO_PORTAN_PORTAN3        (0x08)
#define MCF_GPIO_PORTAN_PORTAN4        (0x10)
#define MCF_GPIO_PORTAN_PORTAN5        (0x20)
#define MCF_GPIO_PORTAN_PORTAN6        (0x40)
#define MCF_GPIO_PORTAN_PORTAN7        (0x80)

/* Bit definitions and macros for MCF_GPIO_PORTAS */
#define MCF_GPIO_PORTAS_PORTAS0        (0x01)
#define MCF_GPIO_PORTAS_PORTAS1        (0x02)
#define MCF_GPIO_PORTAS_PORTAS2        (0x04)
#define MCF_GPIO_PORTAS_PORTAS3        (0x08)
#define MCF_GPIO_PORTAS_PORTAS4        (0x10)
#define MCF_GPIO_PORTAS_PORTAS5        (0x20)
#define MCF_GPIO_PORTAS_PORTAS6        (0x40)
#define MCF_GPIO_PORTAS_PORTAS7        (0x80)

/* Bit definitions and macros for MCF_GPIO_PORTQS */
#define MCF_GPIO_PORTQS_PORTQS0        (0x01)
#define MCF_GPIO_PORTQS_PORTQS1        (0x02)
#define MCF_GPIO_PORTQS_PORTQS2        (0x04)
#define MCF_GPIO_PORTQS_PORTQS3        (0x08)
#define MCF_GPIO_PORTQS_PORTQS4        (0x10)
#define MCF_GPIO_PORTQS_PORTQS5        (0x20)
#define MCF_GPIO_PORTQS_PORTQS6        (0x40)
#define MCF_GPIO_PORTQS_PORTQS7        (0x80)

/* Bit definitions and macros for MCF_GPIO_PORTTA */
#define MCF_GPIO_PORTTA_PORTTA0        (0x01)
#define MCF_GPIO_PORTTA_PORTTA1        (0x02)
#define MCF_GPIO_PORTTA_PORTTA2        (0x04)
#define MCF_GPIO_PORTTA_PORTTA3        (0x08)
#define MCF_GPIO_PORTTA_PORTTA4        (0x10)
#define MCF_GPIO_PORTTA_PORTTA5        (0x20)
#define MCF_GPIO_PORTTA_PORTTA6        (0x40)
#define MCF_GPIO_PORTTA_PORTTA7        (0x80)

/* Bit definitions and macros for MCF_GPIO_PORTTC */
#define MCF_GPIO_PORTTC_PORTTC0        (0x01)
#define MCF_GPIO_PORTTC_PORTTC1        (0x02)
#define MCF_GPIO_PORTTC_PORTTC2        (0x04)
#define MCF_GPIO_PORTTC_PORTTC3        (0x08)
#define MCF_GPIO_PORTTC_PORTTC4        (0x10)
#define MCF_GPIO_PORTTC_PORTTC5        (0x20)
#define MCF_GPIO_PORTTC_PORTTC6        (0x40)
#define MCF_GPIO_PORTTC_PORTTC7        (0x80)

/* Bit definitions and macros for MCF_GPIO_PORTTD */
#define MCF_GPIO_PORTTD_PORTTD0        (0x01)
#define MCF_GPIO_PORTTD_PORTTD1        (0x02)
#define MCF_GPIO_PORTTD_PORTTD2        (0x04)
#define MCF_GPIO_PORTTD_PORTTD3        (0x08)
#define MCF_GPIO_PORTTD_PORTTD4        (0x10)
#define MCF_GPIO_PORTTD_PORTTD5        (0x20)
#define MCF_GPIO_PORTTD_PORTTD6        (0x40)
#define MCF_GPIO_PORTTD_PORTTD7        (0x80)

/* Bit definitions and macros for MCF_GPIO_PORTUA */
#define MCF_GPIO_PORTUA_PORTUA0        (0x01)
#define MCF_GPIO_PORTUA_PORTUA1        (0x02)
#define MCF_GPIO_PORTUA_PORTUA2        (0x04)
#define MCF_GPIO_PORTUA_PORTUA3        (0x08)
#define MCF_GPIO_PORTUA_PORTUA4        (0x10)
#define MCF_GPIO_PORTUA_PORTUA5        (0x20)
#define MCF_GPIO_PORTUA_PORTUA6        (0x40)
#define MCF_GPIO_PORTUA_PORTUA7        (0x80)

/* Bit definitions and macros for MCF_GPIO_PORTUB */
#define MCF_GPIO_PORTUB_PORTUB0        (0x01)
#define MCF_GPIO_PORTUB_PORTUB1        (0x02)
#define MCF_GPIO_PORTUB_PORTUB2        (0x04)
#define MCF_GPIO_PORTUB_PORTUB3        (0x08)
#define MCF_GPIO_PORTUB_PORTUB4        (0x10)
#define MCF_GPIO_PORTUB_PORTUB5        (0x20)
#define MCF_GPIO_PORTUB_PORTUB6        (0x40)
#define MCF_GPIO_PORTUB_PORTUB7        (0x80)

/* Bit definitions and macros for MCF_GPIO_PORTUC */
#define MCF_GPIO_PORTUC_PORTUC0        (0x01)
#define MCF_GPIO_PORTUC_PORTUC1        (0x02)
#define MCF_GPIO_PORTUC_PORTUC2        (0x04)
#define MCF_GPIO_PORTUC_PORTUC3        (0x08)
#define MCF_GPIO_PORTUC_PORTUC4        (0x10)
#define MCF_GPIO_PORTUC_PORTUC5        (0x20)
#define MCF_GPIO_PORTUC_PORTUC6        (0x40)
#define MCF_GPIO_PORTUC_PORTUC7        (0x80)

/* Bit definitions and macros for MCF_GPIO_PORTDD */
#define MCF_GPIO_PORTDD_PORTDD0        (0x01)
#define MCF_GPIO_PORTDD_PORTDD1        (0x02)
#define MCF_GPIO_PORTDD_PORTDD2        (0x04)
#define MCF_GPIO_PORTDD_PORTDD3        (0x08)
#define MCF_GPIO_PORTDD_PORTDD4        (0x10)
#define MCF_GPIO_PORTDD_PORTDD5        (0x20)
#define MCF_GPIO_PORTDD_PORTDD6        (0x40)
#define MCF_GPIO_PORTDD_PORTDD7        (0x80)

/* Bit definitions and macros for MCF_GPIO_PORTLD */
#define MCF_GPIO_PORTLD_PORTLD0        (0x01)
#define MCF_GPIO_PORTLD_PORTLD1        (0x02)
#define MCF_GPIO_PORTLD_PORTLD2        (0x04)
#define MCF_GPIO_PORTLD_PORTLD3        (0x08)
#define MCF_GPIO_PORTLD_PORTLD4        (0x10)
#define MCF_GPIO_PORTLD_PORTLD5        (0x20)
#define MCF_GPIO_PORTLD_PORTLD6        (0x40)
#define MCF_GPIO_PORTLD_PORTLD7        (0x80)

/* Bit definitions and macros for MCF_GPIO_PORTGP */
#define MCF_GPIO_PORTGP_PORTGP0        (0x01)
#define MCF_GPIO_PORTGP_PORTGP1        (0x02)
#define MCF_GPIO_PORTGP_PORTGP2        (0x04)
#define MCF_GPIO_PORTGP_PORTGP3        (0x08)
#define MCF_GPIO_PORTGP_PORTGP4        (0x10)
#define MCF_GPIO_PORTGP_PORTGP5        (0x20)
#define MCF_GPIO_PORTGP_PORTGP6        (0x40)
#define MCF_GPIO_PORTGP_PORTGP7        (0x80)

/* Bit definitions and macros for MCF_GPIO_DDRNQ */
#define MCF_GPIO_DDRNQ_DDRNQ0          (0x01)
#define MCF_GPIO_DDRNQ_DDRNQ1          (0x02)
#define MCF_GPIO_DDRNQ_DDRNQ2          (0x04)
#define MCF_GPIO_DDRNQ_DDRNQ3          (0x08)
#define MCF_GPIO_DDRNQ_DDRNQ4          (0x10)
#define MCF_GPIO_DDRNQ_DDRNQ5          (0x20)
#define MCF_GPIO_DDRNQ_DDRNQ6          (0x40)
#define MCF_GPIO_DDRNQ_DDRNQ7          (0x80)

/* Bit definitions and macros for MCF_GPIO_DDRAN */
#define MCF_GPIO_DDRAN_DDRAN0          (0x01)
#define MCF_GPIO_DDRAN_DDRAN1          (0x02)
#define MCF_GPIO_DDRAN_DDRAN2          (0x04)
#define MCF_GPIO_DDRAN_DDRAN3          (0x08)
#define MCF_GPIO_DDRAN_DDRAN4          (0x10)
#define MCF_GPIO_DDRAN_DDRAN5          (0x20)
#define MCF_GPIO_DDRAN_DDRAN6          (0x40)
#define MCF_GPIO_DDRAN_DDRAN7          (0x80)

/* Bit definitions and macros for MCF_GPIO_DDRAS */
#define MCF_GPIO_DDRAS_DDRAS0          (0x01)
#define MCF_GPIO_DDRAS_DDRAS1          (0x02)
#define MCF_GPIO_DDRAS_DDRAS2          (0x04)
#define MCF_GPIO_DDRAS_DDRAS3          (0x08)
#define MCF_GPIO_DDRAS_DDRAS4          (0x10)
#define MCF_GPIO_DDRAS_DDRAS5          (0x20)
#define MCF_GPIO_DDRAS_DDRAS6          (0x40)
#define MCF_GPIO_DDRAS_DDRAS7          (0x80)

/* Bit definitions and macros for MCF_GPIO_DDRQS */
#define MCF_GPIO_DDRQS_DDRQS0          (0x01)
#define MCF_GPIO_DDRQS_DDRQS1          (0x02)
#define MCF_GPIO_DDRQS_DDRQS2          (0x04)
#define MCF_GPIO_DDRQS_DDRQS3          (0x08)
#define MCF_GPIO_DDRQS_DDRQS4          (0x10)
#define MCF_GPIO_DDRQS_DDRQS5          (0x20)
#define MCF_GPIO_DDRQS_DDRQS6          (0x40)
#define MCF_GPIO_DDRQS_DDRQS7          (0x80)

/* Bit definitions and macros for MCF_GPIO_DDRTA */
#define MCF_GPIO_DDRTA_DDRTA0          (0x01)
#define MCF_GPIO_DDRTA_DDRTA1          (0x02)
#define MCF_GPIO_DDRTA_DDRTA2          (0x04)
#define MCF_GPIO_DDRTA_DDRTA3          (0x08)
#define MCF_GPIO_DDRTA_DDRTA4          (0x10)
#define MCF_GPIO_DDRTA_DDRTA5          (0x20)
#define MCF_GPIO_DDRTA_DDRTA6          (0x40)
#define MCF_GPIO_DDRTA_DDRTA7          (0x80)

/* Bit definitions and macros for MCF_GPIO_DDRTC */
#define MCF_GPIO_DDRTC_DDRTC0          (0x01)
#define MCF_GPIO_DDRTC_DDRTC1          (0x02)
#define MCF_GPIO_DDRTC_DDRTC2          (0x04)
#define MCF_GPIO_DDRTC_DDRTC3          (0x08)
#define MCF_GPIO_DDRTC_DDRTC4          (0x10)
#define MCF_GPIO_DDRTC_DDRTC5          (0x20)
#define MCF_GPIO_DDRTC_DDRTC6          (0x40)
#define MCF_GPIO_DDRTC_DDRTC7          (0x80)

/* Bit definitions and macros for MCF_GPIO_DDRTD */
#define MCF_GPIO_DDRTD_DDRTD0          (0x01)
#define MCF_GPIO_DDRTD_DDRTD1          (0x02)
#define MCF_GPIO_DDRTD_DDRTD2          (0x04)
#define MCF_GPIO_DDRTD_DDRTD3          (0x08)
#define MCF_GPIO_DDRTD_DDRTD4          (0x10)
#define MCF_GPIO_DDRTD_DDRTD5          (0x20)
#define MCF_GPIO_DDRTD_DDRTD6          (0x40)
#define MCF_GPIO_DDRTD_DDRTD7          (0x80)

/* Bit definitions and macros for MCF_GPIO_DDRUA */
#define MCF_GPIO_DDRUA_DDRUA0          (0x01)
#define MCF_GPIO_DDRUA_DDRUA1          (0x02)
#define MCF_GPIO_DDRUA_DDRUA2          (0x04)
#define MCF_GPIO_DDRUA_DDRUA3          (0x08)
#define MCF_GPIO_DDRUA_DDRUA4          (0x10)
#define MCF_GPIO_DDRUA_DDRUA5          (0x20)
#define MCF_GPIO_DDRUA_DDRUA6          (0x40)
#define MCF_GPIO_DDRUA_DDRUA7          (0x80)

/* Bit definitions and macros for MCF_GPIO_DDRUB */
#define MCF_GPIO_DDRUB_DDRUB0          (0x01)
#define MCF_GPIO_DDRUB_DDRUB1          (0x02)
#define MCF_GPIO_DDRUB_DDRUB2          (0x04)
#define MCF_GPIO_DDRUB_DDRUB3          (0x08)
#define MCF_GPIO_DDRUB_DDRUB4          (0x10)
#define MCF_GPIO_DDRUB_DDRUB5          (0x20)
#define MCF_GPIO_DDRUB_DDRUB6          (0x40)
#define MCF_GPIO_DDRUB_DDRUB7          (0x80)

/* Bit definitions and macros for MCF_GPIO_DDRUC */
#define MCF_GPIO_DDRUC_DDRUC0          (0x01)
#define MCF_GPIO_DDRUC_DDRUC1          (0x02)
#define MCF_GPIO_DDRUC_DDRUC2          (0x04)
#define MCF_GPIO_DDRUC_DDRUC3          (0x08)
#define MCF_GPIO_DDRUC_DDRUC4          (0x10)
#define MCF_GPIO_DDRUC_DDRUC5          (0x20)
#define MCF_GPIO_DDRUC_DDRUC6          (0x40)
#define MCF_GPIO_DDRUC_DDRUC7          (0x80)

/* Bit definitions and macros for MCF_GPIO_DDRDD */
#define MCF_GPIO_DDRDD_DDRDD0          (0x01)
#define MCF_GPIO_DDRDD_DDRDD1          (0x02)
#define MCF_GPIO_DDRDD_DDRDD2          (0x04)
#define MCF_GPIO_DDRDD_DDRDD3          (0x08)
#define MCF_GPIO_DDRDD_DDRDD4          (0x10)
#define MCF_GPIO_DDRDD_DDRDD5          (0x20)
#define MCF_GPIO_DDRDD_DDRDD6          (0x40)
#define MCF_GPIO_DDRDD_DDRDD7          (0x80)

/* Bit definitions and macros for MCF_GPIO_DDRLD */
#define MCF_GPIO_DDRLD_DDRLD0          (0x01)
#define MCF_GPIO_DDRLD_DDRLD1          (0x02)
#define MCF_GPIO_DDRLD_DDRLD2          (0x04)
#define MCF_GPIO_DDRLD_DDRLD3          (0x08)
#define MCF_GPIO_DDRLD_DDRLD4          (0x10)
#define MCF_GPIO_DDRLD_DDRLD5          (0x20)
#define MCF_GPIO_DDRLD_DDRLD6          (0x40)
#define MCF_GPIO_DDRLD_DDRLD7          (0x80)

/* Bit definitions and macros for MCF_GPIO_DDRGP */
#define MCF_GPIO_DDRGP_DDRGP0          (0x01)
#define MCF_GPIO_DDRGP_DDRGP1          (0x02)
#define MCF_GPIO_DDRGP_DDRGP2          (0x04)
#define MCF_GPIO_DDRGP_DDRGP3          (0x08)
#define MCF_GPIO_DDRGP_DDRGP4          (0x10)
#define MCF_GPIO_DDRGP_DDRGP5          (0x20)
#define MCF_GPIO_DDRGP_DDRGP6          (0x40)
#define MCF_GPIO_DDRGP_DDRGP7          (0x80)

/* Bit definitions and macros for MCF_GPIO_SETNQ */
#define MCF_GPIO_SETNQ_SETNQ0          (0x01)
#define MCF_GPIO_SETNQ_SETNQ1          (0x02)
#define MCF_GPIO_SETNQ_SETNQ2          (0x04)
#define MCF_GPIO_SETNQ_SETNQ3          (0x08)
#define MCF_GPIO_SETNQ_SETNQ4          (0x10)
#define MCF_GPIO_SETNQ_SETNQ5          (0x20)
#define MCF_GPIO_SETNQ_SETNQ6          (0x40)
#define MCF_GPIO_SETNQ_SETNQ7          (0x80)

/* Bit definitions and macros for MCF_GPIO_SETAN */
#define MCF_GPIO_SETAN_SETAN0          (0x01)
#define MCF_GPIO_SETAN_SETAN1          (0x02)
#define MCF_GPIO_SETAN_SETAN2          (0x04)
#define MCF_GPIO_SETAN_SETAN3          (0x08)
#define MCF_GPIO_SETAN_SETAN4          (0x10)
#define MCF_GPIO_SETAN_SETAN5          (0x20)
#define MCF_GPIO_SETAN_SETAN6          (0x40)
#define MCF_GPIO_SETAN_SETAN7          (0x80)

/* Bit definitions and macros for MCF_GPIO_SETAS */
#define MCF_GPIO_SETAS_SETAS0          (0x01)
#define MCF_GPIO_SETAS_SETAS1          (0x02)
#define MCF_GPIO_SETAS_SETAS2          (0x04)
#define MCF_GPIO_SETAS_SETAS3          (0x08)
#define MCF_GPIO_SETAS_SETAS4          (0x10)
#define MCF_GPIO_SETAS_SETAS5          (0x20)
#define MCF_GPIO_SETAS_SETAS6          (0x40)
#define MCF_GPIO_SETAS_SETAS7          (0x80)

/* Bit definitions and macros for MCF_GPIO_SETQS */
#define MCF_GPIO_SETQS_SETQS0          (0x01)
#define MCF_GPIO_SETQS_SETQS1          (0x02)
#define MCF_GPIO_SETQS_SETQS2          (0x04)
#define MCF_GPIO_SETQS_SETQS3          (0x08)
#define MCF_GPIO_SETQS_SETQS4          (0x10)
#define MCF_GPIO_SETQS_SETQS5          (0x20)
#define MCF_GPIO_SETQS_SETQS6          (0x40)
#define MCF_GPIO_SETQS_SETQS7          (0x80)

/* Bit definitions and macros for MCF_GPIO_SETTA */
#define MCF_GPIO_SETTA_SETTA0          (0x01)
#define MCF_GPIO_SETTA_SETTA1          (0x02)
#define MCF_GPIO_SETTA_SETTA2          (0x04)
#define MCF_GPIO_SETTA_SETTA3          (0x08)
#define MCF_GPIO_SETTA_SETTA4          (0x10)
#define MCF_GPIO_SETTA_SETTA5          (0x20)
#define MCF_GPIO_SETTA_SETTA6          (0x40)
#define MCF_GPIO_SETTA_SETTA7          (0x80)

/* Bit definitions and macros for MCF_GPIO_SETTC */
#define MCF_GPIO_SETTC_SETTC0          (0x01)
#define MCF_GPIO_SETTC_SETTC1          (0x02)
#define MCF_GPIO_SETTC_SETTC2          (0x04)
#define MCF_GPIO_SETTC_SETTC3          (0x08)
#define MCF_GPIO_SETTC_SETTC4          (0x10)
#define MCF_GPIO_SETTC_SETTC5          (0x20)
#define MCF_GPIO_SETTC_SETTC6          (0x40)
#define MCF_GPIO_SETTC_SETTC7          (0x80)

/* Bit definitions and macros for MCF_GPIO_SETTD */
#define MCF_GPIO_SETTD_SETTD0          (0x01)
#define MCF_GPIO_SETTD_SETTD1          (0x02)
#define MCF_GPIO_SETTD_SETTD2          (0x04)
#define MCF_GPIO_SETTD_SETTD3          (0x08)
#define MCF_GPIO_SETTD_SETTD4          (0x10)
#define MCF_GPIO_SETTD_SETTD5          (0x20)
#define MCF_GPIO_SETTD_SETTD6          (0x40)
#define MCF_GPIO_SETTD_SETTD7          (0x80)

/* Bit definitions and macros for MCF_GPIO_SETUA */
#define MCF_GPIO_SETUA_SETUA0          (0x01)
#define MCF_GPIO_SETUA_SETUA1          (0x02)
#define MCF_GPIO_SETUA_SETUA2          (0x04)
#define MCF_GPIO_SETUA_SETUA3          (0x08)
#define MCF_GPIO_SETUA_SETUA4          (0x10)
#define MCF_GPIO_SETUA_SETUA5          (0x20)
#define MCF_GPIO_SETUA_SETUA6          (0x40)
#define MCF_GPIO_SETUA_SETUA7          (0x80)

/* Bit definitions and macros for MCF_GPIO_SETUB */
#define MCF_GPIO_SETUB_SETUB0          (0x01)
#define MCF_GPIO_SETUB_SETUB1          (0x02)
#define MCF_GPIO_SETUB_SETUB2          (0x04)
#define MCF_GPIO_SETUB_SETUB3          (0x08)
#define MCF_GPIO_SETUB_SETUB4          (0x10)
#define MCF_GPIO_SETUB_SETUB5          (0x20)
#define MCF_GPIO_SETUB_SETUB6          (0x40)
#define MCF_GPIO_SETUB_SETUB7          (0x80)

/* Bit definitions and macros for MCF_GPIO_SETUC */
#define MCF_GPIO_SETUC_SETUC0          (0x01)
#define MCF_GPIO_SETUC_SETUC1          (0x02)
#define MCF_GPIO_SETUC_SETUC2          (0x04)
#define MCF_GPIO_SETUC_SETUC3          (0x08)
#define MCF_GPIO_SETUC_SETUC4          (0x10)
#define MCF_GPIO_SETUC_SETUC5          (0x20)
#define MCF_GPIO_SETUC_SETUC6          (0x40)
#define MCF_GPIO_SETUC_SETUC7          (0x80)

/* Bit definitions and macros for MCF_GPIO_SETDD */
#define MCF_GPIO_SETDD_SETDD0          (0x01)
#define MCF_GPIO_SETDD_SETDD1          (0x02)
#define MCF_GPIO_SETDD_SETDD2          (0x04)
#define MCF_GPIO_SETDD_SETDD3          (0x08)
#define MCF_GPIO_SETDD_SETDD4          (0x10)
#define MCF_GPIO_SETDD_SETDD5          (0x20)
#define MCF_GPIO_SETDD_SETDD6          (0x40)
#define MCF_GPIO_SETDD_SETDD7          (0x80)

/* Bit definitions and macros for MCF_GPIO_SETLD */
#define MCF_GPIO_SETLD_SETLD0          (0x01)
#define MCF_GPIO_SETLD_SETLD1          (0x02)
#define MCF_GPIO_SETLD_SETLD2          (0x04)
#define MCF_GPIO_SETLD_SETLD3          (0x08)
#define MCF_GPIO_SETLD_SETLD4          (0x10)
#define MCF_GPIO_SETLD_SETLD5          (0x20)
#define MCF_GPIO_SETLD_SETLD6          (0x40)
#define MCF_GPIO_SETLD_SETLD7          (0x80)

/* Bit definitions and macros for MCF_GPIO_SETGP */
#define MCF_GPIO_SETGP_SETGP0          (0x01)
#define MCF_GPIO_SETGP_SETGP1          (0x02)
#define MCF_GPIO_SETGP_SETGP2          (0x04)
#define MCF_GPIO_SETGP_SETGP3          (0x08)
#define MCF_GPIO_SETGP_SETGP4          (0x10)
#define MCF_GPIO_SETGP_SETGP5          (0x20)
#define MCF_GPIO_SETGP_SETGP6          (0x40)
#define MCF_GPIO_SETGP_SETGP7          (0x80)

/* Bit definitions and macros for MCF_GPIO_CLRNQ */
#define MCF_GPIO_CLRNQ_CLRNQ0          (0x01)
#define MCF_GPIO_CLRNQ_CLRNQ1          (0x02)
#define MCF_GPIO_CLRNQ_CLRNQ2          (0x04)
#define MCF_GPIO_CLRNQ_CLRNQ3          (0x08)
#define MCF_GPIO_CLRNQ_CLRNQ4          (0x10)
#define MCF_GPIO_CLRNQ_CLRNQ5          (0x20)
#define MCF_GPIO_CLRNQ_CLRNQ6          (0x40)
#define MCF_GPIO_CLRNQ_CLRNQ7          (0x80)

/* Bit definitions and macros for MCF_GPIO_CLRAN */
#define MCF_GPIO_CLRAN_CLRAN0          (0x01)
#define MCF_GPIO_CLRAN_CLRAN1          (0x02)
#define MCF_GPIO_CLRAN_CLRAN2          (0x04)
#define MCF_GPIO_CLRAN_CLRAN3          (0x08)
#define MCF_GPIO_CLRAN_CLRAN4          (0x10)
#define MCF_GPIO_CLRAN_CLRAN5          (0x20)
#define MCF_GPIO_CLRAN_CLRAN6          (0x40)
#define MCF_GPIO_CLRAN_CLRAN7          (0x80)

/* Bit definitions and macros for MCF_GPIO_CLRAS */
#define MCF_GPIO_CLRAS_CLRAS0          (0x01)
#define MCF_GPIO_CLRAS_CLRAS1          (0x02)
#define MCF_GPIO_CLRAS_CLRAS2          (0x04)
#define MCF_GPIO_CLRAS_CLRAS3          (0x08)
#define MCF_GPIO_CLRAS_CLRAS4          (0x10)
#define MCF_GPIO_CLRAS_CLRAS5          (0x20)
#define MCF_GPIO_CLRAS_CLRAS6          (0x40)
#define MCF_GPIO_CLRAS_CLRAS7          (0x80)

/* Bit definitions and macros for MCF_GPIO_CLRQS */
#define MCF_GPIO_CLRQS_CLRQS0          (0x01)
#define MCF_GPIO_CLRQS_CLRQS1          (0x02)
#define MCF_GPIO_CLRQS_CLRQS2          (0x04)
#define MCF_GPIO_CLRQS_CLRQS3          (0x08)
#define MCF_GPIO_CLRQS_CLRQS4          (0x10)
#define MCF_GPIO_CLRQS_CLRQS5          (0x20)
#define MCF_GPIO_CLRQS_CLRQS6          (0x40)
#define MCF_GPIO_CLRQS_CLRQS7          (0x80)

/* Bit definitions and macros for MCF_GPIO_CLRTA */
#define MCF_GPIO_CLRTA_CLRTA0          (0x01)
#define MCF_GPIO_CLRTA_CLRTA1          (0x02)
#define MCF_GPIO_CLRTA_CLRTA2          (0x04)
#define MCF_GPIO_CLRTA_CLRTA3          (0x08)
#define MCF_GPIO_CLRTA_CLRTA4          (0x10)
#define MCF_GPIO_CLRTA_CLRTA5          (0x20)
#define MCF_GPIO_CLRTA_CLRTA6          (0x40)
#define MCF_GPIO_CLRTA_CLRTA7          (0x80)

/* Bit definitions and macros for MCF_GPIO_CLRTC */
#define MCF_GPIO_CLRTC_CLRTC0          (0x01)
#define MCF_GPIO_CLRTC_CLRTC1          (0x02)
#define MCF_GPIO_CLRTC_CLRTC2          (0x04)
#define MCF_GPIO_CLRTC_CLRTC3          (0x08)
#define MCF_GPIO_CLRTC_CLRTC4          (0x10)
#define MCF_GPIO_CLRTC_CLRTC5          (0x20)
#define MCF_GPIO_CLRTC_CLRTC6          (0x40)
#define MCF_GPIO_CLRTC_CLRTC7          (0x80)

/* Bit definitions and macros for MCF_GPIO_CLRTD */
#define MCF_GPIO_CLRTD_CLRTD0          (0x01)
#define MCF_GPIO_CLRTD_CLRTD1          (0x02)
#define MCF_GPIO_CLRTD_CLRTD2          (0x04)
#define MCF_GPIO_CLRTD_CLRTD3          (0x08)
#define MCF_GPIO_CLRTD_CLRTD4          (0x10)
#define MCF_GPIO_CLRTD_CLRTD5          (0x20)
#define MCF_GPIO_CLRTD_CLRTD6          (0x40)
#define MCF_GPIO_CLRTD_CLRTD7          (0x80)

/* Bit definitions and macros for MCF_GPIO_CLRUA */
#define MCF_GPIO_CLRUA_CLRUA0          (0x01)
#define MCF_GPIO_CLRUA_CLRUA1          (0x02)
#define MCF_GPIO_CLRUA_CLRUA2          (0x04)
#define MCF_GPIO_CLRUA_CLRUA3          (0x08)
#define MCF_GPIO_CLRUA_CLRUA4          (0x10)
#define MCF_GPIO_CLRUA_CLRUA5          (0x20)
#define MCF_GPIO_CLRUA_CLRUA6          (0x40)
#define MCF_GPIO_CLRUA_CLRUA7          (0x80)

/* Bit definitions and macros for MCF_GPIO_CLRUB */
#define MCF_GPIO_CLRUB_CLRUB0          (0x01)
#define MCF_GPIO_CLRUB_CLRUB1          (0x02)
#define MCF_GPIO_CLRUB_CLRUB2          (0x04)
#define MCF_GPIO_CLRUB_CLRUB3          (0x08)
#define MCF_GPIO_CLRUB_CLRUB4          (0x10)
#define MCF_GPIO_CLRUB_CLRUB5          (0x20)
#define MCF_GPIO_CLRUB_CLRUB6          (0x40)
#define MCF_GPIO_CLRUB_CLRUB7          (0x80)

/* Bit definitions and macros for MCF_GPIO_CLRUC */
#define MCF_GPIO_CLRUC_CLRUC0          (0x01)
#define MCF_GPIO_CLRUC_CLRUC1          (0x02)
#define MCF_GPIO_CLRUC_CLRUC2          (0x04)
#define MCF_GPIO_CLRUC_CLRUC3          (0x08)
#define MCF_GPIO_CLRUC_CLRUC4          (0x10)
#define MCF_GPIO_CLRUC_CLRUC5          (0x20)
#define MCF_GPIO_CLRUC_CLRUC6          (0x40)
#define MCF_GPIO_CLRUC_CLRUC7          (0x80)

/* Bit definitions and macros for MCF_GPIO_CLRDD */
#define MCF_GPIO_CLRDD_CLRDD0          (0x01)
#define MCF_GPIO_CLRDD_CLRDD1          (0x02)
#define MCF_GPIO_CLRDD_CLRDD2          (0x04)
#define MCF_GPIO_CLRDD_CLRDD3          (0x08)
#define MCF_GPIO_CLRDD_CLRDD4          (0x10)
#define MCF_GPIO_CLRDD_CLRDD5          (0x20)
#define MCF_GPIO_CLRDD_CLRDD6          (0x40)
#define MCF_GPIO_CLRDD_CLRDD7          (0x80)

/* Bit definitions and macros for MCF_GPIO_CLRLD */
#define MCF_GPIO_CLRLD_CLRLD0          (0x01)
#define MCF_GPIO_CLRLD_CLRLD1          (0x02)
#define MCF_GPIO_CLRLD_CLRLD2          (0x04)
#define MCF_GPIO_CLRLD_CLRLD3          (0x08)
#define MCF_GPIO_CLRLD_CLRLD4          (0x10)
#define MCF_GPIO_CLRLD_CLRLD5          (0x20)
#define MCF_GPIO_CLRLD_CLRLD6          (0x40)
#define MCF_GPIO_CLRLD_CLRLD7          (0x80)

/* Bit definitions and macros for MCF_GPIO_CLRGP */
#define MCF_GPIO_CLRGP_CLRGP0          (0x01)
#define MCF_GPIO_CLRGP_CLRGP1          (0x02)
#define MCF_GPIO_CLRGP_CLRGP2          (0x04)
#define MCF_GPIO_CLRGP_CLRGP3          (0x08)
#define MCF_GPIO_CLRGP_CLRGP4          (0x10)
#define MCF_GPIO_CLRGP_CLRGP5          (0x20)
#define MCF_GPIO_CLRGP_CLRGP6          (0x40)
#define MCF_GPIO_CLRGP_CLRGP7          (0x80)

/* Bit definitions and macros for MCF_GPIO_PNQPAR */
#define MCF_GPIO_PNQPAR_PNQPAR1(x)     (((x)&0x0003)<<2)
#define MCF_GPIO_PNQPAR_PNQPAR2(x)     (((x)&0x0003)<<4)
#define MCF_GPIO_PNQPAR_PNQPAR3(x)     (((x)&0x0003)<<6)
#define MCF_GPIO_PNQPAR_PNQPAR4(x)     (((x)&0x0003)<<8)
#define MCF_GPIO_PNQPAR_PNQPAR5(x)     (((x)&0x0003)<<10)
#define MCF_GPIO_PNQPAR_PNQPAR6(x)     (((x)&0x0003)<<12)
#define MCF_GPIO_PNQPAR_PNQPAR7(x)     (((x)&0x0003)<<14)
#define MCF_GPIO_PNQPAR_IRQ1_GPIO      (0x0000)
#define MCF_GPIO_PNQPAR_IRQ2_GPIO      (0x0000)
#define MCF_GPIO_PNQPAR_IRQ3_GPIO      (0x0000)
#define MCF_GPIO_PNQPAR_IRQ4_GPIO      (0x0000)
#define MCF_GPIO_PNQPAR_IRQ5_GPIO      (0x0000)
#define MCF_GPIO_PNQPAR_IRQ6_GPIO      (0x0000)
#define MCF_GPIO_PNQPAR_IRQ7_GPIO      (0x0000)
#define MCF_GPIO_PNQPAR_IRQ1_IRQ1      (0x0004)
#define MCF_GPIO_PNQPAR_IRQ2_IRQ2      (0x0010)
#define MCF_GPIO_PNQPAR_IRQ3_IRQ3      (0x0040)
#define MCF_GPIO_PNQPAR_IRQ4_IRQ4      (0x0100)
#define MCF_GPIO_PNQPAR_IRQ5_IRQ5      (0x0400)
#define MCF_GPIO_PNQPAR_IRQ6_IRQ6      (0x1000)
#define MCF_GPIO_PNQPAR_IRQ7_IRQ7      (0x4000)
#define MCF_GPIO_PNQPAR_IRQ1_SYNCA     (0x0008)
#define MCF_GPIO_PNQPAR_IRQ1_PWM1      (0x000C)

/* Bit definitions and macros for MCF_GPIO_PANPAR */
#define MCF_GPIO_PANPAR_PANPAR0        (0x01)
#define MCF_GPIO_PANPAR_PANPAR1        (0x02)
#define MCF_GPIO_PANPAR_PANPAR2        (0x04)
#define MCF_GPIO_PANPAR_PANPAR3        (0x08)
#define MCF_GPIO_PANPAR_PANPAR4        (0x10)
#define MCF_GPIO_PANPAR_PANPAR5        (0x20)
#define MCF_GPIO_PANPAR_PANPAR6        (0x40)
#define MCF_GPIO_PANPAR_PANPAR7        (0x80)
#define MCF_GPIO_PANPAR_AN0_GPIO       (0x00)
#define MCF_GPIO_PANPAR_AN1_GPIO       (0x00)
#define MCF_GPIO_PANPAR_AN2_GPIO       (0x00)
#define MCF_GPIO_PANPAR_AN3_GPIO       (0x00)
#define MCF_GPIO_PANPAR_AN4_GPIO       (0x00)
#define MCF_GPIO_PANPAR_AN5_GPIO       (0x00)
#define MCF_GPIO_PANPAR_AN6_GPIO       (0x00)
#define MCF_GPIO_PANPAR_AN7_GPIO       (0x00)
#define MCF_GPIO_PANPAR_AN0_AN0        (0x01)
#define MCF_GPIO_PANPAR_AN1_AN1        (0x02)
#define MCF_GPIO_PANPAR_AN2_AN2        (0x04)
#define MCF_GPIO_PANPAR_AN3_AN3        (0x08)
#define MCF_GPIO_PANPAR_AN4_AN4        (0x10)
#define MCF_GPIO_PANPAR_AN5_AN5        (0x20)
#define MCF_GPIO_PANPAR_AN6_AN6        (0x40)
#define MCF_GPIO_PANPAR_AN7_AN7        (0x80)

/* Bit definitions and macros for MCF_GPIO_PASPAR */
#define MCF_GPIO_PASPAR_PASPAR0(x)     (((x)&0x03)<<0)
#define MCF_GPIO_PASPAR_PASPAR1(x)     (((x)&0x03)<<2)
#define MCF_GPIO_PASPAR_PASPAR2(x)     (((x)&0x03)<<4)
#define MCF_GPIO_PASPAR_PASPAR3(x)     (((x)&0x03)<<6)
#define MCF_GPIO_PASPAR_SCL_GPIO       (0x00)
#define MCF_GPIO_PASPAR_SDA_GPIO       (0x00)
#define MCF_GPIO_PASPAR_SYNCA_GPIO     (0x00)
#define MCF_GPIO_PASPAR_SYNCB_GPIO     (0x00)
#define MCF_GPIO_PASPAR_SCL_SCL        (0x01)
#define MCF_GPIO_PASPAR_SDA_SDA        (0x04)
#define MCF_GPIO_PASPAR_SYNCA_SYNCA    (0x10)
#define MCF_GPIO_PASPAR_SYNCB_SYNCB    (0x40)
#define MCF_GPIO_PASPAR_SCL_CANTX      (0x02)
#define MCF_GPIO_PASPAR_SDA_CANRX      (0x08)
#define MCF_GPIO_PASPAR_SYNCA_CANRX    (0x20)
#define MCF_GPIO_PASPAR_SYNCB_CANTX    (0x80)
#define MCF_GPIO_PASPAR_SCL_TXD2       (0x30)
#define MCF_GPIO_PASPAR_SDA_RXD2       (0xC0)

/* Bit definitions and macros for MCF_GPIO_PQSPAR */
#define MCF_GPIO_PQSPAR_PQSPAR0(x)     (((x)&0x0003)<<0)
#define MCF_GPIO_PQSPAR_PQSPAR1(x)     (((x)&0x0003)<<2)
#define MCF_GPIO_PQSPAR_PQSPAR2(x)     (((x)&0x0003)<<4)
#define MCF_GPIO_PQSPAR_PQSPAR3(x)     (((x)&0x0003)<<6)
#define MCF_GPIO_PQSPAR_PQSPAR4(x)     (((x)&0x0003)<<8)
#define MCF_GPIO_PQSPAR_PQSPAR5(x)     (((x)&0x0003)<<10)
#define MCF_GPIO_PQSPAR_PQSPAR6(x)     (((x)&0x0003)<<12)
#define MCF_GPIO_PQSPAR_DOUT_GPIO      (0x0000)
#define MCF_GPIO_PQSPAR_DIN_GPIO       (0x0000)
#define MCF_GPIO_PQSPAR_SCK_GPIO       (0x0000)
#define MCF_GPIO_PQSPAR_CS0_GPIO       (0x0000)
#define MCF_GPIO_PQSPAR_CS1_GPIO       (0x0000)
#define MCF_GPIO_PQSPAR_CS2_GPIO       (0x0000)
#define MCF_GPIO_PQSPAR_CS3_GPIO       (0x0000)
#define MCF_GPIO_PQSPAR_DOUT_DOUT      (0x0001)
#define MCF_GPIO_PQSPAR_DIN_DIN        (0x0004)
#define MCF_GPIO_PQSPAR_SCK_SCK        (0x0010)
#define MCF_GPIO_PQSPAR_CS0_CS0        (0x0040)
#define MCF_GPIO_PQSPAR_CS1_CS1        (0x0100)
#define MCF_GPIO_PQSPAR_CS2_CS2        (0x0400)
#define MCF_GPIO_PQSPAR_CS3_CS3        (0x1000)
#define MCF_GPIO_PQSPAR_DOUT_CANTX     (0x0002)
#define MCF_GPIO_PQSPAR_DIN_CANRX      (0x0008)
#define MCF_GPIO_PQSPAR_SCK_SCL        (0x0020)
#define MCF_GPIO_PQSPAR_CS0_SDA        (0x0080)
#define MCF_GPIO_PQSPAR_CS3_SYNCA      (0x2000)
#define MCF_GPIO_PQSPAR_DOUT_TXD1      (0x0003)
#define MCF_GPIO_PQSPAR_DIN_RXD1       (0x000C)
#define MCF_GPIO_PQSPAR_SCK_RTS1       (0x0030)
#define MCF_GPIO_PQSPAR_CS0_CTS1       (0x00C0)
#define MCF_GPIO_PQSPAR_CS3_SYNCB      (0x3000)

/* Bit definitions and macros for MCF_GPIO_PTAPAR */
#define MCF_GPIO_PTAPAR_PTAPAR0(x)     (((x)&0x03)<<0)
#define MCF_GPIO_PTAPAR_PTAPAR1(x)     (((x)&0x03)<<2)
#define MCF_GPIO_PTAPAR_PTAPAR2(x)     (((x)&0x03)<<4)
#define MCF_GPIO_PTAPAR_PTAPAR3(x)     (((x)&0x03)<<6)
#define MCF_GPIO_PTAPAR_ICOC0_GPIO     (0x00)
#define MCF_GPIO_PTAPAR_ICOC1_GPIO     (0x00)
#define MCF_GPIO_PTAPAR_ICOC2_GPIO     (0x00)
#define MCF_GPIO_PTAPAR_ICOC3_GPIO     (0x00)
#define MCF_GPIO_PTAPAR_ICOC0_ICOC0    (0x01)
#define MCF_GPIO_PTAPAR_ICOC1_ICOC1    (0x04)
#define MCF_GPIO_PTAPAR_ICOC2_ICOC2    (0x10)
#define MCF_GPIO_PTAPAR_ICOC3_ICOC3    (0x40)
#define MCF_GPIO_PTAPAR_ICOC0_PWM1     (0x02)
#define MCF_GPIO_PTAPAR_ICOC1_PWM3     (0x08)
#define MCF_GPIO_PTAPAR_ICOC2_PWM5     (0x20)
#define MCF_GPIO_PTAPAR_ICOC3_PWM7     (0x80)

/* Bit definitions and macros for MCF_GPIO_PTCPAR */
#define MCF_GPIO_PTCPAR_PTCPAR0(x)     (((x)&0x03)<<0)
#define MCF_GPIO_PTCPAR_PTCPAR1(x)     (((x)&0x03)<<2)
#define MCF_GPIO_PTCPAR_PTCPAR2(x)     (((x)&0x03)<<4)
#define MCF_GPIO_PTCPAR_PTCPAR3(x)     (((x)&0x03)<<6)
#define MCF_GPIO_PTCPAR_TIN0_GPIO      (0x00)
#define MCF_GPIO_PTCPAR_TIN1_GPIO      (0x00)
#define MCF_GPIO_PTCPAR_TIN2_GPIO      (0x00)
#define MCF_GPIO_PTCPAR_TIN3_GPIO      (0x00)
#define MCF_GPIO_PTCPAR_TIN0_TIN0      (0x01)
#define MCF_GPIO_PTCPAR_TIN1_TIN1      (0x04)
#define MCF_GPIO_PTCPAR_TIN2_TIN2      (0x10)
#define MCF_GPIO_PTCPAR_TIN3_TIN3      (0x40)
#define MCF_GPIO_PTCPAR_TIN0_TOUT0     (0x02)
#define MCF_GPIO_PTCPAR_TIN1_TOUT1     (0x08)
#define MCF_GPIO_PTCPAR_TIN2_TOUT2     (0x20)
#define MCF_GPIO_PTCPAR_TIN3_TOUT3     (0x80)
#define MCF_GPIO_PTCPAR_TIN0_PWM0      (0x03)
#define MCF_GPIO_PTCPAR_TIN1_PWM2      (0x0C)
#define MCF_GPIO_PTCPAR_TIN2_PWM4      (0x30)
#define MCF_GPIO_PTCPAR_TIN3_PWM6      (0xC0)

/* Bit definitions and macros for MCF_GPIO_PTDPAR */
#define MCF_GPIO_PTDPAR_PTDPAR0        (0x01)
#define MCF_GPIO_PTDPAR_PTDPAR1        (0x02)
#define MCF_GPIO_PTDPAR_PTDPAR2        (0x04)
#define MCF_GPIO_PTDPAR_PTDPAR3        (0x08)
#define MCF_GPIO_PTDPAR_PWM1_GPIO      (0x00)
#define MCF_GPIO_PTDPAR_PWM3_GPIO      (0x00)
#define MCF_GPIO_PTDPAR_PWM5_GPIO      (0x00)
#define MCF_GPIO_PTDPAR_PWM7_GPIO      (0x00)
#define MCF_GPIO_PTDPAR_PWM1_PWM1      (0x01)
#define MCF_GPIO_PTDPAR_PWM3_PWM3      (0x02)
#define MCF_GPIO_PTDPAR_PWM5_PWM5      (0x04)
#define MCF_GPIO_PTDPAR_PWM7_PWM7      (0x08)

/* Bit definitions and macros for MCF_GPIO_PUAPAR */
#define MCF_GPIO_PUAPAR_PUAPAR0(x)     (((x)&0x03)<<0)
#define MCF_GPIO_PUAPAR_PUAPAR1(x)     (((x)&0x03)<<2)
#define MCF_GPIO_PUAPAR_PUAPAR2(x)     (((x)&0x03)<<4)
#define MCF_GPIO_PUAPAR_PUAPAR3(x)     (((x)&0x03)<<6)
#define MCF_GPIO_PUAPAR_TXD0_GPIO      (0x00)
#define MCF_GPIO_PUAPAR_RXD0_GPIO      (0x00)
#define MCF_GPIO_PUAPAR_RTS0_GPIO      (0x00)
#define MCF_GPIO_PUAPAR_CTS0_GPIO      (0x00)
#define MCF_GPIO_PUAPAR_TXD0_TXD0      (0x01)
#define MCF_GPIO_PUAPAR_RXD0_RXD0      (0x04)
#define MCF_GPIO_PUAPAR_RTS0_RTS0      (0x10)
#define MCF_GPIO_PUAPAR_CTS0_CTS0      (0x40)
#define MCF_GPIO_PUAPAR_RTS0_CANTX     (0x20)
#define MCF_GPIO_PUAPAR_CTS0_CANRX     (0x80)

/* Bit definitions and macros for MCF_GPIO_PUBPAR */
#define MCF_GPIO_PUBPAR_PUBPAR0(x)     (((x)&0x03)<<0)
#define MCF_GPIO_PUBPAR_PUBPAR1(x)     (((x)&0x03)<<2)
#define MCF_GPIO_PUBPAR_PUBPAR2(x)     (((x)&0x03)<<4)
#define MCF_GPIO_PUBPAR_PUBPAR3(x)     (((x)&0x03)<<6)
#define MCF_GPIO_PUBPAR_TXD1_GPIO      (0x00)
#define MCF_GPIO_PUBPAR_RXD1_GPIO      (0x00)
#define MCF_GPIO_PUBPAR_RTS1_GPIO      (0x00)
#define MCF_GPIO_PUBPAR_CTS1_GPIO      (0x00)
#define MCF_GPIO_PUBPAR_TXD1_TXD1      (0x01)
#define MCF_GPIO_PUBPAR_RXD1_RXD1      (0x04)
#define MCF_GPIO_PUBPAR_RTS1_RTS1      (0x10)
#define MCF_GPIO_PUBPAR_CTS1_CTS1      (0x40)
#define MCF_GPIO_PUBPAR_RTS1_SYNCB     (0x20)
#define MCF_GPIO_PUBPAR_CTS1_SYNCA     (0x80)
#define MCF_GPIO_PUBPAR_RTS1_TXD2      (0x30)
#define MCF_GPIO_PUBPAR_CTS1_RXD2      (0xC0)

/* Bit definitions and macros for MCF_GPIO_PUCPAR */
#define MCF_GPIO_PUCPAR_PUCPAR0        (0x01)
#define MCF_GPIO_PUCPAR_PUCPAR1        (0x02)
#define MCF_GPIO_PUCPAR_PUCPAR2        (0x04)
#define MCF_GPIO_PUCPAR_PUCPAR3        (0x08)
#define MCF_GPIO_PUCPAR_TXD2_GPIO      (0x00)
#define MCF_GPIO_PUCPAR_RXD2_GPIO      (0x00)
#define MCF_GPIO_PUCPAR_RTS2_GPIO      (0x00)
#define MCF_GPIO_PUCPAR_CTS2_GPIO      (0x00)
#define MCF_GPIO_PUCPAR_TXD2_TXD2      (0x01)
#define MCF_GPIO_PUCPAR_RXD2_RXD2      (0x02)
#define MCF_GPIO_PUCPAR_RTS2_RTS2      (0x04)
#define MCF_GPIO_PUCPAR_CTS2_CTS2      (0x08)

/* Bit definitions and macros for MCF_GPIO_PDDPAR */
#define MCF_GPIO_PDDPAR_PDDPAR0        (0x01)
#define MCF_GPIO_PDDPAR_PDDPAR1        (0x02)
#define MCF_GPIO_PDDPAR_PDDPAR2        (0x04)
#define MCF_GPIO_PDDPAR_PDDPAR3        (0x08)
#define MCF_GPIO_PDDPAR_PDDPAR4        (0x10)
#define MCF_GPIO_PDDPAR_PDDPAR5        (0x20)
#define MCF_GPIO_PDDPAR_PDDPAR6        (0x40)
#define MCF_GPIO_PDDPAR_PDDPAR7        (0x80)
#define MCF_GPIO_PDDPAR_PDD0_GPIO      (0x00)
#define MCF_GPIO_PDDPAR_PDD1_GPIO      (0x00)
#define MCF_GPIO_PDDPAR_PDD2_GPIO      (0x00)
#define MCF_GPIO_PDDPAR_PDD3_GPIO      (0x00)
#define MCF_GPIO_PDDPAR_PDD4_GPIO      (0x00)
#define MCF_GPIO_PDDPAR_PDD5_GPIO      (0x00)
#define MCF_GPIO_PDDPAR_PDD6_GPIO      (0x00)
#define MCF_GPIO_PDDPAR_PDD7_GPIO      (0x00)
#define MCF_GPIO_PDDPAR_PDD0_PST0      (0x01)
#define MCF_GPIO_PDDPAR_PDD1_PST1      (0x02)
#define MCF_GPIO_PDDPAR_PDD2_PST2      (0x04)
#define MCF_GPIO_PDDPAR_PDD3_PST3      (0x08)
#define MCF_GPIO_PDDPAR_PDD4_DDATA0    (0x10)
#define MCF_GPIO_PDDPAR_PDD5_DDATA1    (0x20)
#define MCF_GPIO_PDDPAR_PDD6_DDATA2    (0x40)
#define MCF_GPIO_PDDPAR_PDD7_DDATA3    (0x80)

/* Bit definitions and macros for MCF_GPIO_PLDPAR */
#define MCF_GPIO_PLDPAR_PLDPAR0        (0x01)
#define MCF_GPIO_PLDPAR_PLDPAR1        (0x02)
#define MCF_GPIO_PLDPAR_PLDPAR2        (0x04)
#define MCF_GPIO_PLDPAR_PLDPAR3        (0x08)
#define MCF_GPIO_PLDPAR_PLDPAR4        (0x10)
#define MCF_GPIO_PLDPAR_PLDPAR5        (0x20)
#define MCF_GPIO_PLDPAR_PLDPAR6        (0x40)
#define MCF_GPIO_PLDPAR_ACTLED_GPIO    (0x00)
#define MCF_GPIO_PLDPAR_LNKLED_GPIO    (0x00)
#define MCF_GPIO_PLDPAR_SPDLED_GPIO    (0x00)
#define MCF_GPIO_PLDPAR_DUPLED_GPIO    (0x00)
#define MCF_GPIO_PLDPAR_COLLED_GPIO    (0x00)
#define MCF_GPIO_PLDPAR_RXLED_GPIO     (0x00)
#define MCF_GPIO_PLDPAR_TXLED_GPIO     (0x00)
#define MCF_GPIO_PLDPAR_ACTLED_ACTLED  (0x01)
#define MCF_GPIO_PLDPAR_LNKLED_LNKLED  (0x02)
#define MCF_GPIO_PLDPAR_SPDLED_SPDLED  (0x04)
#define MCF_GPIO_PLDPAR_DUPLED_DUPLED  (0x08)
#define MCF_GPIO_PLDPAR_COLLED_COLLED  (0x10)
#define MCF_GPIO_PLDPAR_RXLED_RXLED    (0x20)
#define MCF_GPIO_PLDPAR_TXLED_TXLED    (0x40)

/* Bit definitions and macros for MCF_GPIO_PGPPAR */
#define MCF_GPIO_PGPPAR_PGPPAR0        (0x01)
#define MCF_GPIO_PGPPAR_PGPPAR1        (0x02)
#define MCF_GPIO_PGPPAR_PGPPAR2        (0x04)
#define MCF_GPIO_PGPPAR_PGPPAR3        (0x08)
#define MCF_GPIO_PGPPAR_PGPPAR4        (0x10)
#define MCF_GPIO_PGPPAR_PGPPAR5        (0x20)
#define MCF_GPIO_PGPPAR_PGPPAR6        (0x40)
#define MCF_GPIO_PGPPAR_PGPPAR7        (0x80)
#define MCF_GPIO_PGPPAR_IRQ8_GPIO      (0x00)
#define MCF_GPIO_PGPPAR_IRQ9_GPIO      (0x00)
#define MCF_GPIO_PGPPAR_IRQ10_GPIO     (0x00)
#define MCF_GPIO_PGPPAR_IRQ11_GPIO     (0x00)
#define MCF_GPIO_PGPPAR_IRQ12_GPIO     (0x00)
#define MCF_GPIO_PGPPAR_IRQ13_GPIO     (0x00)
#define MCF_GPIO_PGPPAR_IRQ14_GPIO     (0x00)
#define MCF_GPIO_PGPPAR_IRQ15_GPIO     (0x00)
#define MCF_GPIO_PGPPAR_IRQ8_IRQ8      (0x01)
#define MCF_GPIO_PGPPAR_IRQ9_IRQ9      (0x02)
#define MCF_GPIO_PGPPAR_IRQ10_IRQ10    (0x04)
#define MCF_GPIO_PGPPAR_IRQ11_IRQ11    (0x08)
#define MCF_GPIO_PGPPAR_IRQ12_IRQ12    (0x10)
#define MCF_GPIO_PGPPAR_IRQ13_IRQ13    (0x30)
#define MCF_GPIO_PGPPAR_IRQ14_IRQ14    (0x40)
#define MCF_GPIO_PGPPAR_IRQ15_IRQ15    (0x80)

/* Bit definitions and macros for MCF_GPIO_PWOR */
#define MCF_GPIO_PWOR_PWOR0            (0x0001)
#define MCF_GPIO_PWOR_PWOR1            (0x0002)
#define MCF_GPIO_PWOR_PWOR2            (0x0004)
#define MCF_GPIO_PWOR_PWOR3            (0x0008)
#define MCF_GPIO_PWOR_PWOR4            (0x0010)
#define MCF_GPIO_PWOR_PWOR5            (0x0020)
#define MCF_GPIO_PWOR_PWOR6            (0x0040)
#define MCF_GPIO_PWOR_PWOR7            (0x0080)
#define MCF_GPIO_PWOR_PWOR8            (0x0100)
#define MCF_GPIO_PWOR_PWOR9            (0x0200)
#define MCF_GPIO_PWOR_PWOR10           (0x0400)
#define MCF_GPIO_PWOR_PWOR11           (0x0800)
#define MCF_GPIO_PWOR_PWOR12           (0x1000)
#define MCF_GPIO_PWOR_PWOR13           (0x2000)
#define MCF_GPIO_PWOR_PWOR14           (0x4000)
#define MCF_GPIO_PWOR_PWOR15           (0x8000)

/* Bit definitions and macros for MCF_GPIO_PDSRH */
#define MCF_GPIO_PDSRH_PDSR32          (0x0001)
#define MCF_GPIO_PDSRH_PDSR33          (0x0002)
#define MCF_GPIO_PDSRH_PDSR34          (0x0004)
#define MCF_GPIO_PDSRH_PDSR35          (0x0008)
#define MCF_GPIO_PDSRH_PDSR36          (0x0010)
#define MCF_GPIO_PDSRH_PDSR37          (0x0020)
#define MCF_GPIO_PDSRH_PDSR38          (0x0040)
#define MCF_GPIO_PDSRH_PDSR39          (0x0080)
#define MCF_GPIO_PDSRH_PDSR40          (0x0100)
#define MCF_GPIO_PDSRH_PDSR41          (0x0200)
#define MCF_GPIO_PDSRH_PDSR42          (0x0400)
#define MCF_GPIO_PDSRH_PDSR43          (0x0800)
#define MCF_GPIO_PDSRH_PDSR44          (0x1000)
#define MCF_GPIO_PDSRH_PDSR45          (0x2000)
#define MCF_GPIO_PDSRH_PDSR46          (0x4000)
#define MCF_GPIO_PDSRH_PDSR47          (0x8000)

/* Bit definitions and macros for MCF_GPIO_PDSRL */
#define MCF_GPIO_PDSRL_PDSR0           (0x00000001)
#define MCF_GPIO_PDSRL_PDSR1           (0x00000002)
#define MCF_GPIO_PDSRL_PDSR2           (0x00000004)
#define MCF_GPIO_PDSRL_PDSR3           (0x00000008)
#define MCF_GPIO_PDSRL_PDSR4           (0x00000010)
#define MCF_GPIO_PDSRL_PDSR5           (0x00000020)
#define MCF_GPIO_PDSRL_PDSR6           (0x00000040)
#define MCF_GPIO_PDSRL_PDSR7           (0x00000080)
#define MCF_GPIO_PDSRL_PDSR8           (0x00000100)
#define MCF_GPIO_PDSRL_PDSR9           (0x00000200)
#define MCF_GPIO_PDSRL_PDSR10          (0x00000400)
#define MCF_GPIO_PDSRL_PDSR11          (0x00000800)
#define MCF_GPIO_PDSRL_PDSR12          (0x00001000)
#define MCF_GPIO_PDSRL_PDSR13          (0x00002000)
#define MCF_GPIO_PDSRL_PDSR14          (0x00004000)
#define MCF_GPIO_PDSRL_PDSR15          (0x00008000)
#define MCF_GPIO_PDSRL_PDSR16          (0x00010000)
#define MCF_GPIO_PDSRL_PDSR17          (0x00020000)
#define MCF_GPIO_PDSRL_PDSR18          (0x00040000)
#define MCF_GPIO_PDSRL_PDSR19          (0x00080000)
#define MCF_GPIO_PDSRL_PDSR20          (0x00100000)
#define MCF_GPIO_PDSRL_PDSR21          (0x00200000)
#define MCF_GPIO_PDSRL_PDSR22          (0x00400000)
#define MCF_GPIO_PDSRL_PDSR23          (0x00800000)
#define MCF_GPIO_PDSRL_PDSR24          (0x01000000)
#define MCF_GPIO_PDSRL_PDSR25          (0x02000000)
#define MCF_GPIO_PDSRL_PDSR26          (0x04000000)
#define MCF_GPIO_PDSRL_PDSR27          (0x08000000)
#define MCF_GPIO_PDSRL_PDSR28          (0x10000000)
#define MCF_GPIO_PDSRL_PDSR29          (0x20000000)
#define MCF_GPIO_PDSRL_PDSR30          (0x40000000)
#define MCF_GPIO_PDSRL_PDSR31          (0x80000000)

/*********************************************************************
*
* ColdFire Integration Module (CIM)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_CIM_RCR             (*(vuint8 *)(&__IPSBAR[0x110000]))
#define MCF_CIM_RSR             (*(vuint8 *)(&__IPSBAR[0x110001]))
#define MCF_CIM_CCR             (*(vuint16*)(&__IPSBAR[0x110004]))
#define MCF_CIM_LPCR            (*(vuint8 *)(&__IPSBAR[0x110007]))
#define MCF_CIM_RCON            (*(vuint16*)(&__IPSBAR[0x110008]))
#define MCF_CIM_CIR             (*(vuint16*)(&__IPSBAR[0x11000A]))

/* Bit definitions and macros for MCF_CIM_RCR */
#define MCF_CIM_RCR_LVDE        (0x01)
#define MCF_CIM_RCR_LVDRE       (0x04)
#define MCF_CIM_RCR_LVDIE       (0x08)
#define MCF_CIM_RCR_LVDF        (0x10)
#define MCF_CIM_RCR_FRCRSTOUT   (0x40)
#define MCF_CIM_RCR_SOFTRST     (0x80)

/* Bit definitions and macros for MCF_CIM_RSR */
#define MCF_CIM_RSR_LOL         (0x01)
#define MCF_CIM_RSR_LOC         (0x02)
#define MCF_CIM_RSR_EXT         (0x04)
#define MCF_CIM_RSR_POR         (0x08)
#define MCF_CIM_RSR_WDR         (0x10)
#define MCF_CIM_RSR_SOFT        (0x20)
#define MCF_CIM_RSR_LVD         (0x40)

/* Bit definitions and macros for MCF_CIM_CCR */
#define MCF_CIM_CCR_LOAD        (0x8000)

/* Bit definitions and macros for MCF_CIM_LPCR */
#define MCF_CIM_LPCR_LVDSE      (0x02)
#define MCF_CIM_LPCR_STPMD(x)   (((x)&0x03)<<3)
#define MCF_CIM_LPCR_LPMD(x)    (((x)&0x03)<<6)
#define MCF_CIM_LPCR_LPMD_STOP  (0xC0)
#define MCF_CIM_LPCR_LPMD_WAIT  (0x80)
#define MCF_CIM_LPCR_LPMD_DOZE  (0x40)
#define MCF_CIM_LPCR_LPMD_RUN   (0x00)

/* Bit definitions and macros for MCF_CIM_RCON */
#define MCF_CIM_RCON_RLOAD      (0x0020)

/*********************************************************************
*
* Clock Module (CLOCK)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_CLOCK_SYNCR           (*(vuint16*)(&__IPSBAR[0x120000]))
#define MCF_CLOCK_SYNSR           (*(vuint8 *)(&__IPSBAR[0x120002]))
#define MCF_CLOCK_LPCR            (*(vuint8 *)(&__IPSBAR[0x120007]))
#define MCF_CLOCK_CCHR            (*(vuint8 *)(&__IPSBAR[0x120008]))
#define MCF_CLOCK_RTCDR           (*(vuint32*)(&__IPSBAR[0x12000C]))

/* Bit definitions and macros for MCF_CLOCK_SYNCR */
#define MCF_CLOCK_SYNCR_PLLEN     (0x0001)
#define MCF_CLOCK_SYNCR_PLLMODE   (0x0002)
#define MCF_CLOCK_SYNCR_CLKSRC    (0x0004)
#define MCF_CLOCK_SYNCR_FWKUP     (0x0020)
#define MCF_CLOCK_SYNCR_DISCLK    (0x0040)
#define MCF_CLOCK_SYNCR_LOCEN     (0x0080)
#define MCF_CLOCK_SYNCR_RFD(x)    (((x)&0x0007)<<8)
#define MCF_CLOCK_SYNCR_LOCRE     (0x0800)
#define MCF_CLOCK_SYNCR_MFD(x)    (((x)&0x0007)<<12)
#define MCF_CLOCK_SYNCR_LOLRE     (0x8000)

/* Bit definitions and macros for MCF_CLOCK_SYNSR */
#define MCF_CLOCK_SYNSR_LOCS      (0x04)
#define MCF_CLOCK_SYNSR_LOCK      (0x08)
#define MCF_CLOCK_SYNSR_LOCKS     (0x10)
#define MCF_CLOCK_SYNSR_CRYOSC    (0x20)
#define MCF_CLOCK_SYNSR_OCOSC     (0x40)
#define MCF_CLOCK_SYNSR_EXTOSC    (0x80)

/* Bit definitions and macros for MCF_CLOCK_LPCR */
#define MCF_CLOCK_LPCR_LPD(x)     (((x)&0x0F)<<0)

/* Bit definitions and macros for MCF_CLOCK_CCHR */
#define MCF_CLOCK_CCHR_PFD(x)     (((x)&0x07)<<0)

/* Bit definitions and macros for MCF_CLOCK_RTCDR */
#define MCF_CLOCK_RTCDR_RTCDF(x)  (((x)&0xFFFFFFFF)<<0)

/*********************************************************************
*
* Edge Port Module (EPORT)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_EPORT_EPPAR0                (*(vuint16*)(&__IPSBAR[0x130000]))
#define MCF_EPORT_EPPAR1                (*(vuint16*)(&__IPSBAR[0x140000]))
#define MCF_EPORT_EPDDR0                (*(vuint8 *)(&__IPSBAR[0x130002]))
#define MCF_EPORT_EPDDR1                (*(vuint8 *)(&__IPSBAR[0x140002]))
#define MCF_EPORT_EPIER0                (*(vuint8 *)(&__IPSBAR[0x130003]))
#define MCF_EPORT_EPIER1                (*(vuint8 *)(&__IPSBAR[0x140003]))
#define MCF_EPORT_EPDR0                 (*(vuint8 *)(&__IPSBAR[0x130004]))
#define MCF_EPORT_EPDR1                 (*(vuint8 *)(&__IPSBAR[0x140004]))
#define MCF_EPORT_EPPDR0                (*(vuint8 *)(&__IPSBAR[0x130005]))
#define MCF_EPORT_EPPDR1                (*(vuint8 *)(&__IPSBAR[0x140005]))
#define MCF_EPORT_EPFR0                 (*(vuint8 *)(&__IPSBAR[0x130006]))
#define MCF_EPORT_EPFR1                 (*(vuint8 *)(&__IPSBAR[0x140006]))

/* Bit definitions and macros for MCF_EPORT_EPPAR */
#define MCF_EPORT_EPPAR_EPPA1(x)        (((x)&0x0003)<<2)
#define MCF_EPORT_EPPAR_EPPA2(x)        (((x)&0x0003)<<4)
#define MCF_EPORT_EPPAR_EPPA3(x)        (((x)&0x0003)<<6)
#define MCF_EPORT_EPPAR_EPPA4(x)        (((x)&0x0003)<<8)
#define MCF_EPORT_EPPAR_EPPA5(x)        (((x)&0x0003)<<10)
#define MCF_EPORT_EPPAR_EPPA6(x)        (((x)&0x0003)<<12)
#define MCF_EPORT_EPPAR_EPPA7(x)        (((x)&0x0003)<<14)
#define MCF_EPORT_EPPAR_EPPA8(x)        (((x)&0x0003)<<0)
#define MCF_EPORT_EPPAR_EPPA9(x)        (((x)&0x0003)<<2)
#define MCF_EPORT_EPPAR_EPPA10(x)       (((x)&0x0003)<<4)
#define MCF_EPORT_EPPAR_EPPA11(x)       (((x)&0x0003)<<6)
#define MCF_EPORT_EPPAR_EPPA12(x)       (((x)&0x0003)<<8)
#define MCF_EPORT_EPPAR_EPPA13(x)       (((x)&0x0003)<<10)
#define MCF_EPORT_EPPAR_EPPA14(x)       (((x)&0x0003)<<12)
#define MCF_EPORT_EPPAR_EPPA15(x)       (((x)&0x0003)<<14)
#define MCF_EPORT_EPPAR_LEVEL           (0)
#define MCF_EPORT_EPPAR_RISING          (1)
#define MCF_EPORT_EPPAR_FALLING         (2)
#define MCF_EPORT_EPPAR_BOTH            (3)
#define MCF_EPORT_EPPAR_EPPA15_LEVEL    (0x0000)
#define MCF_EPORT_EPPAR_EPPA15_RISING   (0x4000)
#define MCF_EPORT_EPPAR_EPPA15_FALLING  (0x8000)
#define MCF_EPORT_EPPAR_EPPA15_BOTH     (0xC000)
#define MCF_EPORT_EPPAR_EPPA14_LEVEL    (0x0000)
#define MCF_EPORT_EPPAR_EPPA14_RISING   (0x1000)
#define MCF_EPORT_EPPAR_EPPA14_FALLING  (0x2000)
#define MCF_EPORT_EPPAR_EPPA14_BOTH     (0x3000)
#define MCF_EPORT_EPPAR_EPPA13_LEVEL    (0x0000)
#define MCF_EPORT_EPPAR_EPPA13_RISING   (0x0400)
#define MCF_EPORT_EPPAR_EPPA13_FALLING  (0x0800)
#define MCF_EPORT_EPPAR_EPPA13_BOTH     (0x0C00)
#define MCF_EPORT_EPPAR_EPPA12_LEVEL    (0x0000)
#define MCF_EPORT_EPPAR_EPPA12_RISING   (0x0100)
#define MCF_EPORT_EPPAR_EPPA12_FALLING  (0x0200)
#define MCF_EPORT_EPPAR_EPPA12_BOTH     (0x0300)
#define MCF_EPORT_EPPAR_EPPA11_LEVEL    (0x0000)
#define MCF_EPORT_EPPAR_EPPA11_RISING   (0x0040)
#define MCF_EPORT_EPPAR_EPPA11_FALLING  (0x0080)
#define MCF_EPORT_EPPAR_EPPA11_BOTH     (0x00C0)
#define MCF_EPORT_EPPAR_EPPA10_LEVEL    (0x0000)
#define MCF_EPORT_EPPAR_EPPA10_RISING   (0x0010)
#define MCF_EPORT_EPPAR_EPPA10_FALLING  (0x0020)
#define MCF_EPORT_EPPAR_EPPA10_BOTH     (0x0030)
#define MCF_EPORT_EPPAR_EPPA9_LEVEL     (0x0000)
#define MCF_EPORT_EPPAR_EPPA9_RISING    (0x0004)
#define MCF_EPORT_EPPAR_EPPA9_FALLING   (0x0008)
#define MCF_EPORT_EPPAR_EPPA9_BOTH      (0x000C)
#define MCF_EPORT_EPPAR_EPPA8_LEVEL     (0x0000)
#define MCF_EPORT_EPPAR_EPPA8_RISING    (0x0001)
#define MCF_EPORT_EPPAR_EPPA8_FALLING   (0x0002)
#define MCF_EPORT_EPPAR_EPPA8_BOTH      (0x0003)
#define MCF_EPORT_EPPAR_EPPA7_LEVEL     (0x0000)
#define MCF_EPORT_EPPAR_EPPA7_RISING    (0x4000)
#define MCF_EPORT_EPPAR_EPPA7_FALLING   (0x8000)
#define MCF_EPORT_EPPAR_EPPA7_BOTH      (0xC000)
#define MCF_EPORT_EPPAR_EPPA6_LEVEL     (0x0000)
#define MCF_EPORT_EPPAR_EPPA6_RISING    (0x1000)
#define MCF_EPORT_EPPAR_EPPA6_FALLING   (0x2000)
#define MCF_EPORT_EPPAR_EPPA6_BOTH      (0x3000)
#define MCF_EPORT_EPPAR_EPPA5_LEVEL     (0x0000)
#define MCF_EPORT_EPPAR_EPPA5_RISING    (0x0400)
#define MCF_EPORT_EPPAR_EPPA5_FALLING   (0x0800)
#define MCF_EPORT_EPPAR_EPPA5_BOTH      (0x0C00)
#define MCF_EPORT_EPPAR_EPPA4_LEVEL     (0x0000)
#define MCF_EPORT_EPPAR_EPPA4_RISING    (0x0100)
#define MCF_EPORT_EPPAR_EPPA4_FALLING   (0x0200)
#define MCF_EPORT_EPPAR_EPPA4_BOTH      (0x0300)
#define MCF_EPORT_EPPAR_EPPA3_LEVEL     (0x0000)
#define MCF_EPORT_EPPAR_EPPA3_RISING    (0x0040)
#define MCF_EPORT_EPPAR_EPPA3_FALLING   (0x0080)
#define MCF_EPORT_EPPAR_EPPA3_BOTH      (0x00C0)
#define MCF_EPORT_EPPAR_EPPA2_LEVEL     (0x0000)
#define MCF_EPORT_EPPAR_EPPA2_RISING    (0x0010)
#define MCF_EPORT_EPPAR_EPPA2_FALLING   (0x0020)
#define MCF_EPORT_EPPAR_EPPA2_BOTH      (0x0030)
#define MCF_EPORT_EPPAR_EPPA1_LEVEL     (0x0000)
#define MCF_EPORT_EPPAR_EPPA1_RISING    (0x0004)
#define MCF_EPORT_EPPAR_EPPA1_FALLING   (0x0008)
#define MCF_EPORT_EPPAR_EPPA1_BOTH      (0x000C)

/* Bit definitions and macros for MCF_EPORT_EPDDR */
#define MCF_EPORT_EPDDR_EPDD1           (0x02)
#define MCF_EPORT_EPDDR_EPDD2           (0x04)
#define MCF_EPORT_EPDDR_EPDD3           (0x08)
#define MCF_EPORT_EPDDR_EPDD4           (0x10)
#define MCF_EPORT_EPDDR_EPDD5           (0x20)
#define MCF_EPORT_EPDDR_EPDD6           (0x40)
#define MCF_EPORT_EPDDR_EPDD7           (0x80)
#define MCF_EPORT_EPDDR_EPDD8           (0x01)
#define MCF_EPORT_EPDDR_EPDD9           (0x02)
#define MCF_EPORT_EPDDR_EPDD10          (0x04)
#define MCF_EPORT_EPDDR_EPDD11          (0x08)
#define MCF_EPORT_EPDDR_EPDD12          (0x10)
#define MCF_EPORT_EPDDR_EPDD13          (0x20)
#define MCF_EPORT_EPDDR_EPDD14          (0x40)
#define MCF_EPORT_EPDDR_EPDD15          (0x80)

/* Bit definitions and macros for MCF_EPORT_EPIER */
#define MCF_EPORT_EPIER_EPIE1           (0x02)
#define MCF_EPORT_EPIER_EPIE2           (0x04)
#define MCF_EPORT_EPIER_EPIE3           (0x08)
#define MCF_EPORT_EPIER_EPIE4           (0x10)
#define MCF_EPORT_EPIER_EPIE5           (0x20)
#define MCF_EPORT_EPIER_EPIE6           (0x40)
#define MCF_EPORT_EPIER_EPIE7           (0x80)
#define MCF_EPORT_EPIER_EPIE8           (0x01)
#define MCF_EPORT_EPIER_EPIE9           (0x02)
#define MCF_EPORT_EPIER_EPIE10          (0x04)
#define MCF_EPORT_EPIER_EPIE11          (0x08)
#define MCF_EPORT_EPIER_EPIE12          (0x10)
#define MCF_EPORT_EPIER_EPIE13          (0x20)
#define MCF_EPORT_EPIER_EPIE14          (0x40)
#define MCF_EPORT_EPIER_EPIE15          (0x80)

/* Bit definitions and macros for MCF_EPORT_EPDR */
#define MCF_EPORT_EPDR_EPD1             (0x02)
#define MCF_EPORT_EPDR_EPD2             (0x04)
#define MCF_EPORT_EPDR_EPD3             (0x08)
#define MCF_EPORT_EPDR_EPD4             (0x10)
#define MCF_EPORT_EPDR_EPD5             (0x20)
#define MCF_EPORT_EPDR_EPD6             (0x40)
#define MCF_EPORT_EPDR_EPD7             (0x80)
#define MCF_EPORT_EPDR_EPD8             (0x01)
#define MCF_EPORT_EPDR_EPD9             (0x02)
#define MCF_EPORT_EPDR_EPD10            (0x04)
#define MCF_EPORT_EPDR_EPD11            (0x08)
#define MCF_EPORT_EPDR_EPD12            (0x10)
#define MCF_EPORT_EPDR_EPD13            (0x20)
#define MCF_EPORT_EPDR_EPD14            (0x40)
#define MCF_EPORT_EPDR_EPD15            (0x80)

/* Bit definitions and macros for MCF_EPORT_EPPDR */
#define MCF_EPORT_EPPDR_EPPD1           (0x02)
#define MCF_EPORT_EPPDR_EPPD2           (0x04)
#define MCF_EPORT_EPPDR_EPPD3           (0x08)
#define MCF_EPORT_EPPDR_EPPD4           (0x10)
#define MCF_EPORT_EPPDR_EPPD5           (0x20)
#define MCF_EPORT_EPPDR_EPPD6           (0x40)
#define MCF_EPORT_EPPDR_EPPD7           (0x80)
#define MCF_EPORT_EPPDR_EPPD8           (0x01)
#define MCF_EPORT_EPPDR_EPPD9           (0x02)
#define MCF_EPORT_EPPDR_EPPD10          (0x04)
#define MCF_EPORT_EPPDR_EPPD11          (0x08)
#define MCF_EPORT_EPPDR_EPPD12          (0x10)
#define MCF_EPORT_EPPDR_EPPD13          (0x20)
#define MCF_EPORT_EPPDR_EPPD14          (0x40)
#define MCF_EPORT_EPPDR_EPPD15          (0x80)

/* Bit definitions and macros for MCF_EPORT_EPFR */
#define MCF_EPORT_EPFR_EPF1             (0x02)
#define MCF_EPORT_EPFR_EPF2             (0x04)
#define MCF_EPORT_EPFR_EPF3             (0x08)
#define MCF_EPORT_EPFR_EPF4             (0x10)
#define MCF_EPORT_EPFR_EPF5             (0x20)
#define MCF_EPORT_EPFR_EPF6             (0x40)
#define MCF_EPORT_EPFR_EPF7             (0x80)
#define MCF_EPORT_EPFR_EPF8             (0x01)
#define MCF_EPORT_EPFR_EPF9             (0x02)
#define MCF_EPORT_EPFR_EPF10            (0x04)
#define MCF_EPORT_EPFR_EPF11            (0x08)
#define MCF_EPORT_EPFR_EPF12            (0x10)
#define MCF_EPORT_EPFR_EPF13            (0x20)
#define MCF_EPORT_EPFR_EPF14            (0x40)
#define MCF_EPORT_EPFR_EPF15            (0x80)

/*********************************************************************
*
* Programmable Interrupt Timer Modules (PIT)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_PIT0_PCSR        (*(vuint16*)(&__IPSBAR[0x150000]))
#define MCF_PIT0_PMR         (*(vuint16*)(&__IPSBAR[0x150002]))
#define MCF_PIT0_PCNTR       (*(vuint16*)(&__IPSBAR[0x150004]))
#define MCF_PIT1_PCSR        (*(vuint16*)(&__IPSBAR[0x160000]))
#define MCF_PIT1_PMR         (*(vuint16*)(&__IPSBAR[0x160002]))
#define MCF_PIT1_PCNTR       (*(vuint16*)(&__IPSBAR[0x160004]))
#define MCF_PIT_PCSR(x)      (*(vuint16*)(&__IPSBAR[0x150000+((x)*0x10000)]))
#define MCF_PIT_PMR(x)       (*(vuint16*)(&__IPSBAR[0x150002+((x)*0x10000)]))
#define MCF_PIT_PCNTR(x)     (*(vuint16*)(&__IPSBAR[0x150004+((x)*0x10000)]))

/* Bit definitions and macros for MCF_PIT_PCSR */
#define MCF_PIT_PCSR_EN      (0x0001)
#define MCF_PIT_PCSR_RLD     (0x0002)
#define MCF_PIT_PCSR_PIF     (0x0004)
#define MCF_PIT_PCSR_PIE     (0x0008)
#define MCF_PIT_PCSR_OVW     (0x0010)
#define MCF_PIT_PCSR_HALTED  (0x0020)
#define MCF_PIT_PCSR_DOZE    (0x0040)
#define MCF_PIT_PCSR_PRE(x)  (((x)&0x000F)<<8)

/* Bit definitions and macros for MCF_PIT_PMR */
#define MCF_PIT_PMR_PM(x)    (((x)&0xFFFF)<<0)

/* Bit definitions and macros for MCF_PIT_PCNTR */
#define MCF_PIT_PCNTR_PC(x)  (((x)&0xFFFF)<<0)

/*********************************************************************
*
* Analog-to-Digital Converter (ADC)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_ADC_CTRL1              (*(vuint16*)(&__IPSBAR[0x190000]))
#define MCF_ADC_CTRL2              (*(vuint16*)(&__IPSBAR[0x190002]))
#define MCF_ADC_ADZCC              (*(vuint16*)(&__IPSBAR[0x190004]))
#define MCF_ADC_ADLST1             (*(vuint16*)(&__IPSBAR[0x190006]))
#define MCF_ADC_ADLST2             (*(vuint16*)(&__IPSBAR[0x190008]))
#define MCF_ADC_ADSDIS             (*(vuint16*)(&__IPSBAR[0x19000A]))
#define MCF_ADC_ADSTAT             (*(vuint16*)(&__IPSBAR[0x19000C]))
#define MCF_ADC_ADLSTAT            (*(vuint16*)(&__IPSBAR[0x19000E]))
#define MCF_ADC_ADZCSTAT           (*(vuint16*)(&__IPSBAR[0x190010]))
#define MCF_ADC_ADRSLT0            (*(vuint16*)(&__IPSBAR[0x190012]))
#define MCF_ADC_ADRSLT1            (*(vuint16*)(&__IPSBAR[0x190014]))
#define MCF_ADC_ADRSLT2            (*(vuint16*)(&__IPSBAR[0x190016]))
#define MCF_ADC_ADRSLT3            (*(vuint16*)(&__IPSBAR[0x190018]))
#define MCF_ADC_ADRSLT4            (*(vuint16*)(&__IPSBAR[0x19001A]))
#define MCF_ADC_ADRSLT5            (*(vuint16*)(&__IPSBAR[0x19001C]))
#define MCF_ADC_ADRSLT6            (*(vuint16*)(&__IPSBAR[0x19001E]))
#define MCF_ADC_ADRSLT7            (*(vuint16*)(&__IPSBAR[0x190020]))
#define MCF_ADC_ADRSLT(x)          (*(vuint16*)(&__IPSBAR[0x190012+((x)*0x002)]))
#define MCF_ADC_ADLLMT0            (*(vuint16*)(&__IPSBAR[0x190022]))
#define MCF_ADC_ADLLMT1            (*(vuint16*)(&__IPSBAR[0x190024]))
#define MCF_ADC_ADLLMT2            (*(vuint16*)(&__IPSBAR[0x190026]))
#define MCF_ADC_ADLLMT3            (*(vuint16*)(&__IPSBAR[0x190028]))
#define MCF_ADC_ADLLMT4            (*(vuint16*)(&__IPSBAR[0x19002A]))
#define MCF_ADC_ADLLMT5            (*(vuint16*)(&__IPSBAR[0x19002C]))
#define MCF_ADC_ADLLMT6            (*(vuint16*)(&__IPSBAR[0x19002E]))
#define MCF_ADC_ADLLMT7            (*(vuint16*)(&__IPSBAR[0x190030]))
#define MCF_ADC_ADLLMT(x)          (*(vuint16*)(&__IPSBAR[0x190022+((x)*0x002)]))
#define MCF_ADC_ADHLMT0            (*(vuint16*)(&__IPSBAR[0x190032]))
#define MCF_ADC_ADHLMT1            (*(vuint16*)(&__IPSBAR[0x190034]))
#define MCF_ADC_ADHLMT2            (*(vuint16*)(&__IPSBAR[0x190036]))
#define MCF_ADC_ADHLMT3            (*(vuint16*)(&__IPSBAR[0x190038]))
#define MCF_ADC_ADHLMT4            (*(vuint16*)(&__IPSBAR[0x19003A]))
#define MCF_ADC_ADHLMT5            (*(vuint16*)(&__IPSBAR[0x19003C]))
#define MCF_ADC_ADHLMT6            (*(vuint16*)(&__IPSBAR[0x19003E]))
#define MCF_ADC_ADHLMT7            (*(vuint16*)(&__IPSBAR[0x190040]))
#define MCF_ADC_ADHLMT(x)          (*(vuint16*)(&__IPSBAR[0x190032+((x)*0x002)]))
#define MCF_ADC_ADOFS0             (*(vuint16*)(&__IPSBAR[0x190042]))
#define MCF_ADC_ADOFS1             (*(vuint16*)(&__IPSBAR[0x190044]))
#define MCF_ADC_ADOFS2             (*(vuint16*)(&__IPSBAR[0x190046]))
#define MCF_ADC_ADOFS3             (*(vuint16*)(&__IPSBAR[0x190048]))
#define MCF_ADC_ADOFS4             (*(vuint16*)(&__IPSBAR[0x19004A]))
#define MCF_ADC_ADOFS5             (*(vuint16*)(&__IPSBAR[0x19004C]))
#define MCF_ADC_ADOFS6             (*(vuint16*)(&__IPSBAR[0x19004E]))
#define MCF_ADC_ADOFS7             (*(vuint16*)(&__IPSBAR[0x190050]))
#define MCF_ADC_ADOFS(x)           (*(vuint16*)(&__IPSBAR[0x190042+((x)*0x002)]))
#define MCF_ADC_POWER              (*(vuint16*)(&__IPSBAR[0x190052]))
#define MCF_ADC_CAL                (*(vuint16*)(&__IPSBAR[0x190054]))

/* Bit definitions and macros for MCF_ADC_CTRL1 */
#define MCF_ADC_CTRL1_SMODE(x)     (((x)&0x0007)<<0)
#define MCF_ADC_CTRL1_CHNCFG(x)    (((x)&0x000F)<<4)
#define MCF_ADC_CTRL1_HLMTIE       (0x0100)
#define MCF_ADC_CTRL1_LLMTIE       (0x0200)
#define MCF_ADC_CTRL1_ZCIE         (0x0400)
#define MCF_ADC_CTRL1_EOSIE0       (0x0800)
#define MCF_ADC_CTRL1_SYNC0        (0x1000)
#define MCF_ADC_CTRL1_START0       (0x2000)
#define MCF_ADC_CTRL1_STOP0        (0x4000)

/* Bit definitions and macros for MCF_ADC_CTRL2 */
#define MCF_ADC_CTRL2_DIV(x)       (((x)&0x001F)<<0)
#define MCF_ADC_CTRL2_SIMULT       (0x0020)
#define MCF_ADC_CTRL2_EOSIE1       (0x0800)
#define MCF_ADC_CTRL2_SYNC1        (0x1000)
#define MCF_ADC_CTRL2_START1       (0x2000)
#define MCF_ADC_CTRL2_STOP1        (0x4000)

/* Bit definitions and macros for MCF_ADC_ADZCC */
#define MCF_ADC_ADZCC_ZCE0(x)      (((x)&0x0003)<<0)
#define MCF_ADC_ADZCC_ZCE1(x)      (((x)&0x0003)<<2)
#define MCF_ADC_ADZCC_ZCE2(x)      (((x)&0x0003)<<4)
#define MCF_ADC_ADZCC_ZCE3(x)      (((x)&0x0003)<<6)
#define MCF_ADC_ADZCC_ZCE4(x)      (((x)&0x0003)<<8)
#define MCF_ADC_ADZCC_ZCE5(x)      (((x)&0x0003)<<10)
#define MCF_ADC_ADZCC_ZCE6(x)      (((x)&0x0003)<<12)
#define MCF_ADC_ADZCC_ZCE7(x)      (((x)&0x0003)<<14)

/* Bit definitions and macros for MCF_ADC_ADLST1 */
#define MCF_ADC_ADLST1_SAMPLE0(x)  (((x)&0x0007)<<0)
#define MCF_ADC_ADLST1_SAMPLE1(x)  (((x)&0x0007)<<4)
#define MCF_ADC_ADLST1_SAMPLE2(x)  (((x)&0x0007)<<8)
#define MCF_ADC_ADLST1_SAMPLE3(x)  (((x)&0x0007)<<12)

/* Bit definitions and macros for MCF_ADC_ADLST2 */
#define MCF_ADC_ADLST2_SAMPLE4(x)  (((x)&0x0007)<<0)
#define MCF_ADC_ADLST2_SAMPLE5(x)  (((x)&0x0007)<<4)
#define MCF_ADC_ADLST2_SAMPLE6(x)  (((x)&0x0007)<<8)
#define MCF_ADC_ADLST2_SAMPLE7(x)  (((x)&0x0007)<<12)

/* Bit definitions and macros for MCF_ADC_ADSDIS */
#define MCF_ADC_ADSDIS_DS0         (0x0001)
#define MCF_ADC_ADSDIS_DS1         (0x0002)
#define MCF_ADC_ADSDIS_DS2         (0x0004)
#define MCF_ADC_ADSDIS_DS3         (0x0008)
#define MCF_ADC_ADSDIS_DS4         (0x0010)
#define MCF_ADC_ADSDIS_DS5         (0x0020)
#define MCF_ADC_ADSDIS_DS6         (0x0040)
#define MCF_ADC_ADSDIS_DS7         (0x0080)

/* Bit definitions and macros for MCF_ADC_ADSTAT */
#define MCF_ADC_ADSTAT_RDY0        (0x0001)
#define MCF_ADC_ADSTAT_RDY1        (0x0002)
#define MCF_ADC_ADSTAT_RDY2        (0x0004)
#define MCF_ADC_ADSTAT_RDY3        (0x0008)
#define MCF_ADC_ADSTAT_RDY4        (0x0010)
#define MCF_ADC_ADSTAT_RDY5        (0x0020)
#define MCF_ADC_ADSTAT_RDY6        (0x0040)
#define MCF_ADC_ADSTAT_RDY7        (0x0080)
#define MCF_ADC_ADSTAT_HLMT        (0x0100)
#define MCF_ADC_ADSTAT_LLMTI       (0x0200)
#define MCF_ADC_ADSTAT_ZCI         (0x0400)
#define MCF_ADC_ADSTAT_EOSI        (0x0800)
#define MCF_ADC_ADSTAT_CIP         (0x8000)

/* Bit definitions and macros for MCF_ADC_ADLSTAT */
#define MCF_ADC_ADLSTAT_LLS0       (0x0001)
#define MCF_ADC_ADLSTAT_LLS1       (0x0002)
#define MCF_ADC_ADLSTAT_LLS2       (0x0004)
#define MCF_ADC_ADLSTAT_LLS3       (0x0008)
#define MCF_ADC_ADLSTAT_LLS4       (0x0010)
#define MCF_ADC_ADLSTAT_LLS5       (0x0020)
#define MCF_ADC_ADLSTAT_LLS6       (0x0040)
#define MCF_ADC_ADLSTAT_LLS7       (0x0080)
#define MCF_ADC_ADLSTAT_HLS0       (0x0100)
#define MCF_ADC_ADLSTAT_HLS1       (0x0200)
#define MCF_ADC_ADLSTAT_HLS2       (0x0400)
#define MCF_ADC_ADLSTAT_HLS3       (0x0800)
#define MCF_ADC_ADLSTAT_HLS4       (0x1000)
#define MCF_ADC_ADLSTAT_HLS5       (0x2000)
#define MCF_ADC_ADLSTAT_HLS6       (0x4000)
#define MCF_ADC_ADLSTAT_HLS7       (0x8000)

/* Bit definitions and macros for MCF_ADC_ADZCSTAT */
#define MCF_ADC_ADZCSTAT_ZCS0      (0x0001)
#define MCF_ADC_ADZCSTAT_ZCS1      (0x0002)
#define MCF_ADC_ADZCSTAT_ZCS2      (0x0004)
#define MCF_ADC_ADZCSTAT_ZCS3      (0x0008)
#define MCF_ADC_ADZCSTAT_ZCS4      (0x0010)
#define MCF_ADC_ADZCSTAT_ZCS5      (0x0020)
#define MCF_ADC_ADZCSTAT_ZCS6      (0x0040)
#define MCF_ADC_ADZCSTAT_ZCS7      (0x0080)

/* Bit definitions and macros for MCF_ADC_ADRSLT */
#define MCF_ADC_ADRSLT_RSLT(x)     (((x)&0x0FFF)<<3)
#define MCF_ADC_ADRSLT_SEXT        (0x8000)

/* Bit definitions and macros for MCF_ADC_ADLLMT */
#define MCF_ADC_ADLLMT_LLMT(x)     (((x)&0x0FFF)<<3)

/* Bit definitions and macros for MCF_ADC_ADHLMT */
#define MCF_ADC_ADHLMT_HLMT(x)     (((x)&0x0FFF)<<3)

/* Bit definitions and macros for MCF_ADC_ADOFS */
#define MCF_ADC_ADOFS_OFFSET(x)    (((x)&0x0FFF)<<3)

/* Bit definitions and macros for MCF_ADC_POWER */
#define MCF_ADC_POWER_PD0          (0x0001)
#define MCF_ADC_POWER_PD1          (0x0002)
#define MCF_ADC_POWER_PD2          (0x0004)
#define MCF_ADC_POWER_APD          (0x0008)
#define MCF_ADC_POWER_PUDELAY(x)   (((x)&0x003F)<<4)
#define MCF_ADC_POWER_PSTS0        (0x0400)
#define MCF_ADC_POWER_PSTS1        (0x0800)
#define MCF_ADC_POWER_PSTS2        (0x1000)
#define MCF_ADC_POWER_ASTBY        (0x8000)

/* Bit definitions and macros for MCF_ADC_CAL */
#define MCF_ADC_CAL_CAL0           (0x0001)
#define MCF_ADC_CAL_CRS0           (0x0002)
#define MCF_ADC_CAL_CAL1           (0x0004)
#define MCF_ADC_CAL_CRS1           (0x0008)

/*********************************************************************
*
* General Purpose Timer (GPT)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_GPT_GPTIOS                    (*(vuint8 *)(&__IPSBAR[0x1A0000]))
#define MCF_GPT_GPTCFORC                  (*(vuint8 *)(&__IPSBAR[0x1A0001]))
#define MCF_GPT_GPTOC3M                   (*(vuint8 *)(&__IPSBAR[0x1A0002]))
#define MCF_GPT_GPTOC3D                   (*(vuint8 *)(&__IPSBAR[0x1A0003]))
#define MCF_GPT_GPTCNT                    (*(vuint16*)(&__IPSBAR[0x1A0004]))
#define MCF_GPT_GPTSCR1                   (*(vuint8 *)(&__IPSBAR[0x1A0006]))
#define MCF_GPT_GPTTOV                    (*(vuint8 *)(&__IPSBAR[0x1A0008]))
#define MCF_GPT_GPTCTL1                   (*(vuint8 *)(&__IPSBAR[0x1A0009]))
#define MCF_GPT_GPTCTL2                   (*(vuint8 *)(&__IPSBAR[0x1A000B]))
#define MCF_GPT_GPTIE                     (*(vuint8 *)(&__IPSBAR[0x1A000C]))
#define MCF_GPT_GPTSCR2                   (*(vuint8 *)(&__IPSBAR[0x1A000D]))
#define MCF_GPT_GPTFLG1                   (*(vuint8 *)(&__IPSBAR[0x1A000E]))
#define MCF_GPT_GPTFLG2                   (*(vuint8 *)(&__IPSBAR[0x1A000F]))
#define MCF_GPT_GPTC0                     (*(vuint16*)(&__IPSBAR[0x1A0010]))
#define MCF_GPT_GPTC1                     (*(vuint16*)(&__IPSBAR[0x1A0012]))
#define MCF_GPT_GPTC2                     (*(vuint16*)(&__IPSBAR[0x1A0014]))
#define MCF_GPT_GPTC3                     (*(vuint16*)(&__IPSBAR[0x1A0016]))
#define MCF_GPT_GPTC(x)                   (*(vuint16*)(&__IPSBAR[0x1A0010+((x)*0x002)]))
#define MCF_GPT_GPTPACTL                  (*(vuint8 *)(&__IPSBAR[0x1A0018]))
#define MCF_GPT_GPTPAFLG                  (*(vuint8 *)(&__IPSBAR[0x1A0019]))
#define MCF_GPT_GPTPACNT                  (*(vuint16*)(&__IPSBAR[0x1A001A]))
#define MCF_GPT_GPTPORT                   (*(vuint8 *)(&__IPSBAR[0x1A001D]))
#define MCF_GPT_GPTDDR                    (*(vuint8 *)(&__IPSBAR[0x1A001E]))

/* Bit definitions and macros for MCF_GPT_GPTIOS */
#define MCF_GPT_GPTIOS_IOS0               (0x01)
#define MCF_GPT_GPTIOS_IOS1               (0x02)
#define MCF_GPT_GPTIOS_IOS2               (0x04)
#define MCF_GPT_GPTIOS_IOS3               (0x08)

/* Bit definitions and macros for MCF_GPT_GPTCFORC */
#define MCF_GPT_GPTCFORC_FOC0             (0x01)
#define MCF_GPT_GPTCFORC_FOC1             (0x02)
#define MCF_GPT_GPTCFORC_FOC2             (0x04)
#define MCF_GPT_GPTCFORC_FOC3             (0x08)

/* Bit definitions and macros for MCF_GPT_GPTOC3D */
#define MCF_GPT_GPTOC3D_OC3D0             (0x01)
#define MCF_GPT_GPTOC3D_OC3D1             (0x02)
#define MCF_GPT_GPTOC3D_OC3D2             (0x04)
#define MCF_GPT_GPTOC3D_OC3D3             (0x08)

/* Bit definitions and macros for MCF_GPT_GPTSCR1 */
#define MCF_GPT_GPTSCR1_TFFCA             (0x10)
#define MCF_GPT_GPTSCR1_GPTEN             (0x80)

/* Bit definitions and macros for MCF_GPT_GPTTOV */
#define MCF_GPT_GPTTOV_TOV0               (0x01)
#define MCF_GPT_GPTTOV_TOV1               (0x02)
#define MCF_GPT_GPTTOV_TOV2               (0x04)
#define MCF_GPT_GPTTOV_TOV3               (0x08)

/* Bit definitions and macros for MCF_GPT_GPTCTL1 */
#define MCF_GPT_GPTCTL1_OL0               (0x01)
#define MCF_GPT_GPTCTL1_OM0               (0x02)
#define MCF_GPT_GPTCTL1_OL1               (0x04)
#define MCF_GPT_GPTCTL1_OM1               (0x08)
#define MCF_GPT_GPTCTL1_OL2               (0x10)
#define MCF_GPT_GPTCTL1_OM2               (0x20)
#define MCF_GPT_GPTCTL1_OL3               (0x40)
#define MCF_GPT_GPTCTL1_OM3               (0x80)
#define MCF_GPT_GPTCTL1_OUTPUT3_NOTHING   ((0x00))
#define MCF_GPT_GPTCTL1_OUTPUT3_TOGGLE    ((0x40))
#define MCF_GPT_GPTCTL1_OUTPUT3_CLEAR     ((0x80))
#define MCF_GPT_GPTCTL1_OUTPUT3_SET       ((0xC0))
#define MCF_GPT_GPTCTL1_OUTPUT2_NOTHING   ((0x00))
#define MCF_GPT_GPTCTL1_OUTPUT2_TOGGLE    ((0x10))
#define MCF_GPT_GPTCTL1_OUTPUT2_CLEAR     ((0x20))
#define MCF_GPT_GPTCTL1_OUTPUT2_SET       ((0x30))
#define MCF_GPT_GPTCTL1_OUTPUT1_NOTHING   ((0x00))
#define MCF_GPT_GPTCTL1_OUTPUT1_TOGGLE    ((0x04))
#define MCF_GPT_GPTCTL1_OUTPUT1_CLEAR     ((0x08))
#define MCF_GPT_GPTCTL1_OUTPUT1_SET       ((0x0C))
#define MCF_GPT_GPTCTL1_OUTPUT0_NOTHING   ((0x00))
#define MCF_GPT_GPTCTL1_OUTPUT0_TOGGLE    ((0x01))
#define MCF_GPT_GPTCTL1_OUTPUT0_CLEAR     ((0x02))
#define MCF_GPT_GPTCTL1_OUTPUT0_SET       ((0x03))

/* Bit definitions and macros for MCF_GPT_GPTCTL2 */
#define MCF_GPT_GPTCTL2_EDG0A             (0x01)
#define MCF_GPT_GPTCTL2_EDG0B             (0x02)
#define MCF_GPT_GPTCTL2_EDG1A             (0x04)
#define MCF_GPT_GPTCTL2_EDG1B             (0x08)
#define MCF_GPT_GPTCTL2_EDG2A             (0x10)
#define MCF_GPT_GPTCTL2_EDG2B             (0x20)
#define MCF_GPT_GPTCTL2_EDG3A             (0x40)
#define MCF_GPT_GPTCTL2_EDG3B             (0x80)
#define MCF_GPT_GPTCTL2_INPUT3_DISABLED   ((0x00))
#define MCF_GPT_GPTCTL2_INPUT3_RISING     ((0x40))
#define MCF_GPT_GPTCTL2_INPUT3_FALLING    ((0x80))
#define MCF_GPT_GPTCTL2_INPUT3_ANY        ((0xC0))
#define MCF_GPT_GPTCTL2_INPUT2_DISABLED   ((0x00))
#define MCF_GPT_GPTCTL2_INPUT2_RISING     ((0x10))
#define MCF_GPT_GPTCTL2_INPUT2_FALLING    ((0x20))
#define MCF_GPT_GPTCTL2_INPUT2_ANY        ((0x30))
#define MCF_GPT_GPTCTL2_INPUT1_DISABLED   ((0x00))
#define MCF_GPT_GPTCTL2_INPUT1_RISING     ((0x04))
#define MCF_GPT_GPTCTL2_INPUT1_FALLING    ((0x08))
#define MCF_GPT_GPTCTL2_INPUT1_ANY        ((0x0C))
#define MCF_GPT_GPTCTL2_INPUT0_DISABLED   ((0x00))
#define MCF_GPT_GPTCTL2_INPUT0_RISING     ((0x01))
#define MCF_GPT_GPTCTL2_INPUT0_FALLING    ((0x02))
#define MCF_GPT_GPTCTL2_INPUT0_ANY        ((0x03))

/* Bit definitions and macros for MCF_GPT_GPTIE */
#define MCF_GPT_GPTIE_CI0                 (0x01)
#define MCF_GPT_GPTIE_CI1                 (0x02)
#define MCF_GPT_GPTIE_CI2                 (0x04)
#define MCF_GPT_GPTIE_CI3                 (0x08)

/* Bit definitions and macros for MCF_GPT_GPTSCR2 */
#define MCF_GPT_GPTSCR2_PR(x)             (((x)&0x07)<<0)
#define MCF_GPT_GPTSCR2_TCRE              (0x08)
#define MCF_GPT_GPTSCR2_RDPT              (0x10)
#define MCF_GPT_GPTSCR2_PUPT              (0x20)
#define MCF_GPT_GPTSCR2_TOI               (0x80)
#define MCF_GPT_GPTSCR2_PR_1              ((0x00))
#define MCF_GPT_GPTSCR2_PR_2              ((0x01))
#define MCF_GPT_GPTSCR2_PR_4              ((0x02))
#define MCF_GPT_GPTSCR2_PR_8              ((0x03))
#define MCF_GPT_GPTSCR2_PR_16             ((0x04))
#define MCF_GPT_GPTSCR2_PR_32             ((0x05))
#define MCF_GPT_GPTSCR2_PR_64             ((0x06))
#define MCF_GPT_GPTSCR2_PR_128            ((0x07))

/* Bit definitions and macros for MCF_GPT_GPTFLG1 */
#define MCF_GPT_GPTFLG1_CF0               (0x01)
#define MCF_GPT_GPTFLG1_CF1               (0x02)
#define MCF_GPT_GPTFLG1_CF2               (0x04)
#define MCF_GPT_GPTFLG1_CF3               (0x08)

/* Bit definitions and macros for MCF_GPT_GPTFLG2 */
#define MCF_GPT_GPTFLG2_CF0               (0x01)
#define MCF_GPT_GPTFLG2_CF1               (0x02)
#define MCF_GPT_GPTFLG2_CF2               (0x04)
#define MCF_GPT_GPTFLG2_CF3               (0x08)
#define MCF_GPT_GPTFLG2_TOF               (0x80)

/* Bit definitions and macros for MCF_GPT_GPTC */
#define MCF_GPT_GPTC_CCNT(x)              (((x)&0xFFFF)<<0)

/* Bit definitions and macros for MCF_GPT_GPTPACTL */
#define MCF_GPT_GPTPACTL_PAI              (0x01)
#define MCF_GPT_GPTPACTL_PAOVI            (0x02)
#define MCF_GPT_GPTPACTL_CLK(x)           (((x)&0x03)<<2)
#define MCF_GPT_GPTPACTL_PEDGE            (0x10)
#define MCF_GPT_GPTPACTL_PAMOD            (0x20)
#define MCF_GPT_GPTPACTL_PAE              (0x40)
#define MCF_GPT_GPTPACTL_CLK_GPTPR        ((0x00))
#define MCF_GPT_GPTPACTL_CLK_PACLK        ((0x01))
#define MCF_GPT_GPTPACTL_CLK_PACLK_256    ((0x02))
#define MCF_GPT_GPTPACTL_CLK_PACLK_65536  ((0x03))

/* Bit definitions and macros for MCF_GPT_GPTPAFLG */
#define MCF_GPT_GPTPAFLG_PAIF             (0x01)
#define MCF_GPT_GPTPAFLG_PAOVF            (0x02)

/* Bit definitions and macros for MCF_GPT_GPTPACNT */
#define MCF_GPT_GPTPACNT_PACNT(x)         (((x)&0xFFFF)<<0)

/* Bit definitions and macros for MCF_GPT_GPTPORT */
#define MCF_GPT_GPTPORT_PORTT(x)          (((x)&0x0F)<<0)

/* Bit definitions and macros for MCF_GPT_GPTDDR */
#define MCF_GPT_GPTDDR_DDRT0              (0x01)
#define MCF_GPT_GPTDDR_DDRT1              (0x02)
#define MCF_GPT_GPTDDR_DDRT2              (0x04)
#define MCF_GPT_GPTDDR_DDRT3              (0x08)

/*********************************************************************
*
* Pulse Width Modulation (PWM)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_PWM_PWME               (*(vuint8 *)(&__IPSBAR[0x1B0000]))
#define MCF_PWM_PWMPOL             (*(vuint8 *)(&__IPSBAR[0x1B0001]))
#define MCF_PWM_PWMCLK             (*(vuint8 *)(&__IPSBAR[0x1B0002]))
#define MCF_PWM_PWMPRCLK           (*(vuint8 *)(&__IPSBAR[0x1B0003]))
#define MCF_PWM_PWMCAE             (*(vuint8 *)(&__IPSBAR[0x1B0004]))
#define MCF_PWM_PWMCTL             (*(vuint8 *)(&__IPSBAR[0x1B0005]))
#define MCF_PWM_PWMSCLA            (*(vuint8 *)(&__IPSBAR[0x1B0008]))
#define MCF_PWM_PWMSCLB            (*(vuint8 *)(&__IPSBAR[0x1B0009]))
#define MCF_PWM_PWMCNT0            (*(vuint8 *)(&__IPSBAR[0x1B000C]))
#define MCF_PWM_PWMCNT1            (*(vuint8 *)(&__IPSBAR[0x1B000D]))
#define MCF_PWM_PWMCNT2            (*(vuint8 *)(&__IPSBAR[0x1B000E]))
#define MCF_PWM_PWMCNT3            (*(vuint8 *)(&__IPSBAR[0x1B000F]))
#define MCF_PWM_PWMCNT4            (*(vuint8 *)(&__IPSBAR[0x1B0010]))
#define MCF_PWM_PWMCNT5            (*(vuint8 *)(&__IPSBAR[0x1B0011]))
#define MCF_PWM_PWMCNT6            (*(vuint8 *)(&__IPSBAR[0x1B0012]))
#define MCF_PWM_PWMCNT7            (*(vuint8 *)(&__IPSBAR[0x1B0013]))
#define MCF_PWM_PWMCNT(x)          (*(vuint8 *)(&__IPSBAR[0x1B000C+((x)*0x001)]))
#define MCF_PWM_PWMPER0            (*(vuint8 *)(&__IPSBAR[0x1B0014]))
#define MCF_PWM_PWMPER1            (*(vuint8 *)(&__IPSBAR[0x1B0015]))
#define MCF_PWM_PWMPER2            (*(vuint8 *)(&__IPSBAR[0x1B0016]))
#define MCF_PWM_PWMPER3            (*(vuint8 *)(&__IPSBAR[0x1B0017]))
#define MCF_PWM_PWMPER4            (*(vuint8 *)(&__IPSBAR[0x1B0018]))
#define MCF_PWM_PWMPER5            (*(vuint8 *)(&__IPSBAR[0x1B0019]))
#define MCF_PWM_PWMPER6            (*(vuint8 *)(&__IPSBAR[0x1B001A]))
#define MCF_PWM_PWMPER7            (*(vuint8 *)(&__IPSBAR[0x1B001B]))
#define MCF_PWM_PWMPER(x)          (*(vuint8 *)(&__IPSBAR[0x1B0014+((x)*0x001)]))
#define MCF_PWM_PWMDTY0            (*(vuint8 *)(&__IPSBAR[0x1B001C]))
#define MCF_PWM_PWMDTY1            (*(vuint8 *)(&__IPSBAR[0x1B001D]))
#define MCF_PWM_PWMDTY2            (*(vuint8 *)(&__IPSBAR[0x1B001E]))
#define MCF_PWM_PWMDTY3            (*(vuint8 *)(&__IPSBAR[0x1B001F]))
#define MCF_PWM_PWMDTY4            (*(vuint8 *)(&__IPSBAR[0x1B0020]))
#define MCF_PWM_PWMDTY5            (*(vuint8 *)(&__IPSBAR[0x1B0021]))
#define MCF_PWM_PWMDTY6            (*(vuint8 *)(&__IPSBAR[0x1B0022]))
#define MCF_PWM_PWMDTY7            (*(vuint8 *)(&__IPSBAR[0x1B0023]))
#define MCF_PWM_PWMDTY(x)          (*(vuint8 *)(&__IPSBAR[0x1B001C+((x)*0x001)]))
#define MCF_PWM_PWMSDN             (*(vuint8 *)(&__IPSBAR[0x1B0024]))

/* Bit definitions and macros for MCF_PWM_PWME */
#define MCF_PWM_PWME_PWME0         (0x01)
#define MCF_PWM_PWME_PWME1         (0x02)
#define MCF_PWM_PWME_PWME2         (0x04)
#define MCF_PWM_PWME_PWME3         (0x08)

/* Bit definitions and macros for MCF_PWM_PWMPOL */
#define MCF_PWM_PWMPOL_PPOL0       (0x01)
#define MCF_PWM_PWMPOL_PPOL1       (0x02)
#define MCF_PWM_PWMPOL_PPOL2       (0x04)
#define MCF_PWM_PWMPOL_PPOL3       (0x08)

/* Bit definitions and macros for MCF_PWM_PWMCLK */
#define MCF_PWM_PWMCLK_PCLK0       (0x01)
#define MCF_PWM_PWMCLK_PCLK1       (0x02)
#define MCF_PWM_PWMCLK_PCLK2       (0x04)
#define MCF_PWM_PWMCLK_PCLK3       (0x08)

/* Bit definitions and macros for MCF_PWM_PWMPRCLK */
#define MCF_PWM_PWMPRCLK_PCKA(x)   (((x)&0x07)<<0)
#define MCF_PWM_PWMPRCLK_PCKB(x)   (((x)&0x07)<<4)

/* Bit definitions and macros for MCF_PWM_PWMCAE */
#define MCF_PWM_PWMCAE_CAE0        (0x01)
#define MCF_PWM_PWMCAE_CAE1        (0x02)
#define MCF_PWM_PWMCAE_CAE2        (0x04)
#define MCF_PWM_PWMCAE_CAE3        (0x08)

/* Bit definitions and macros for MCF_PWM_PWMCTL */
#define MCF_PWM_PWMCTL_PFRZ        (0x04)
#define MCF_PWM_PWMCTL_PSWAI       (0x08)
#define MCF_PWM_PWMCTL_CON01       (0x10)
#define MCF_PWM_PWMCTL_CON23       (0x20)

/* Bit definitions and macros for MCF_PWM_PWMSCLA */
#define MCF_PWM_PWMSCLA_SCALEA(x)  (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_PWM_PWMSCLB */
#define MCF_PWM_PWMSCLB_SCALEB(x)  (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_PWM_PWMCNT */
#define MCF_PWM_PWMCNT_COUNT(x)    (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_PWM_PWMPER */
#define MCF_PWM_PWMPER_PERIOD(x)   (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_PWM_PWMDTY */
#define MCF_PWM_PWMDTY_DUTY(x)     (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_PWM_PWMSDN */
#define MCF_PWM_PWMSDN_SDNEN       (0x01)
#define MCF_PWM_PWMSDN_PWM7IL      (0x02)
#define MCF_PWM_PWMSDN_PWM7IN      (0x04)
#define MCF_PWM_PWMSDN_LVL         (0x10)
#define MCF_PWM_PWMSDN_RESTART     (0x20)
#define MCF_PWM_PWMSDN_IE          (0x40)
#define MCF_PWM_PWMSDN_IF          (0x80)

/*********************************************************************
*
* FlexCAN Module (CAN)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_CAN_CANMCR                   (*(vuint32*)(&__IPSBAR[0x1C0000]))
#define MCF_CAN_CANCTRL                  (*(vuint32*)(&__IPSBAR[0x1C0004]))
#define MCF_CAN_TIMER                    (*(vuint32*)(&__IPSBAR[0x1C0008]))
#define MCF_CAN_RXGMASK                  (*(vuint32*)(&__IPSBAR[0x1C0010]))
#define MCF_CAN_RX14MASK                 (*(vuint32*)(&__IPSBAR[0x1C0014]))
#define MCF_CAN_RX15MASK                 (*(vuint32*)(&__IPSBAR[0x1C0018]))
#define MCF_CAN_ERRCNT                   (*(vuint32*)(&__IPSBAR[0x1C001C]))
#define MCF_CAN_ERRSTAT                  (*(vuint32*)(&__IPSBAR[0x1C0020]))
#define MCF_CAN_IMASK                    (*(vuint32*)(&__IPSBAR[0x1C0028]))
#define MCF_CAN_IFLAG                    (*(vuint32*)(&__IPSBAR[0x1C0030]))

/* Bit definitions and macros for MCF_CAN_CANMCR */
#define MCF_CAN_CANMCR_MAXMB(x)          (((x)&0x0000000F)<<0)
#define MCF_CAN_CANMCR_SUPV              (0x00800000)
#define MCF_CAN_CANMCR_FRZACK            (0x01000000)
#define MCF_CAN_CANMCR_SOFTRST           (0x02000000)
#define MCF_CAN_CANMCR_HALT              (0x10000000)
#define MCF_CAN_CANMCR_FRZ               (0x40000000)
#define MCF_CAN_CANMCR_MDIS              (0x80000000)

/* Bit definitions and macros for MCF_CAN_CANCTRL */
#define MCF_CAN_CANCTRL_PROPSEG(x)       (((x)&0x00000007)<<0)
#define MCF_CAN_CANCTRL_LOM              (0x00000008)
#define MCF_CAN_CANCTRL_LBUF             (0x00000010)
#define MCF_CAN_CANCTRL_TSYNC            (0x00000020)
#define MCF_CAN_CANCTRL_BOFFREC          (0x00000040)
#define MCF_CAN_CANCTRL_SAMP             (0x00000080)
#define MCF_CAN_CANCTRL_LPB              (0x00001000)
#define MCF_CAN_CANCTRL_CLKSRC           (0x00002000)
#define MCF_CAN_CANCTRL_ERRMSK           (0x00004000)
#define MCF_CAN_CANCTRL_BOFFMSK          (0x00008000)
#define MCF_CAN_CANCTRL_PSEG2(x)         (((x)&0x00000007)<<16)
#define MCF_CAN_CANCTRL_PSEG1(x)         (((x)&0x00000007)<<19)
#define MCF_CAN_CANCTRL_RJW(x)           (((x)&0x00000003)<<22)
#define MCF_CAN_CANCTRL_PRESDIV(x)       (((x)&0x000000FF)<<24)

/* Bit definitions and macros for MCF_CAN_TIMER */
#define MCF_CAN_TIMER_TIMER(x)           (((x)&0x0000FFFF)<<0)

/* Bit definitions and macros for MCF_CAN_RXGMASK */
#define MCF_CAN_RXGMASK_MI(x)            (((x)&0x1FFFFFFF)<<0)

/* Bit definitions and macros for MCF_CAN_RX14MASK */
#define MCF_CAN_RX14MASK_MI(x)           (((x)&0x1FFFFFFF)<<0)

/* Bit definitions and macros for MCF_CAN_RX15MASK */
#define MCF_CAN_RX15MASK_MI(x)           (((x)&0x1FFFFFFF)<<0)

/* Bit definitions and macros for MCF_CAN_ERRCNT */
#define MCF_CAN_ERRCNT_TXECTR(x)         (((x)&0x000000FF)<<0)
#define MCF_CAN_ERRCNT_RXECTR(x)         (((x)&0x000000FF)<<8)

/* Bit definitions and macros for MCF_CAN_ERRSTAT */
#define MCF_CAN_ERRSTAT_WAKINT           (0x00000001)
#define MCF_CAN_ERRSTAT_ERRINT           (0x00000002)
#define MCF_CAN_ERRSTAT_BOFFINT          (0x00000004)
#define MCF_CAN_ERRSTAT_FLTCONF(x)       (((x)&0x00000003)<<4)
#define MCF_CAN_ERRSTAT_TXRX             (0x00000040)
#define MCF_CAN_ERRSTAT_IDLE             (0x00000080)
#define MCF_CAN_ERRSTAT_RXWRN            (0x00000100)
#define MCF_CAN_ERRSTAT_TXWRN            (0x00000200)
#define MCF_CAN_ERRSTAT_STFERR           (0x00000400)
#define MCF_CAN_ERRSTAT_FRMERR           (0x00000800)
#define MCF_CAN_ERRSTAT_CRCERR           (0x00001000)
#define MCF_CAN_ERRSTAT_ACKERR           (0x00002000)
#define MCF_CAN_ERRSTAT_BITERR(x)        (((x)&0x00000003)<<14)
#define MCF_CAN_ERRSTAT_FLTCONF_ACTIVE   (0x00000000)
#define MCF_CAN_ERRSTAT_FLTCONF_PASSIVE  (0x00000010)
#define MCF_CAN_ERRSTAT_FLTCONF_BUSOFF   (0x00000020)

/* Bit definitions and macros for MCF_CAN_IMASK */
#define MCF_CAN_IMASK_BUF(x)             (1<<x)

/* Bit definitions and macros for MCF_CAN_IFLAG */
#define MCF_CAN_IFLAG_BUF(x)             (1<<x)

/*********************************************************************
*
* ColdFire Flash Module (CFM)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_CFM_CFMMCR           (*(vuint16*)(&__IPSBAR[0x1D0000]))
#define MCF_CFM_CFMCLKD          (*(vuint8 *)(&__IPSBAR[0x1D0002]))
#define MCF_CFM_CFMSEC           (*(vuint32*)(&__IPSBAR[0x1D0008]))
#define MCF_CFM_CFMPROT          (*(vuint32*)(&__IPSBAR[0x1D0010]))
#define MCF_CFM_CFMSACC          (*(vuint32*)(&__IPSBAR[0x1D0014]))
#define MCF_CFM_CFMDACC          (*(vuint32*)(&__IPSBAR[0x1D0018]))
#define MCF_CFM_CFMUSTAT         (*(vuint8 *)(&__IPSBAR[0x1D0020]))
#define MCF_CFM_CFMCMD           (*(vuint8 *)(&__IPSBAR[0x1D0024]))

/* Bit definitions and macros for MCF_CFM_CFMMCR */
#define MCF_CFM_CFMMCR_KEYACC    (0x0020)
#define MCF_CFM_CFMMCR_CCIE      (0x0040)
#define MCF_CFM_CFMMCR_CBEIE     (0x0080)
#define MCF_CFM_CFMMCR_AEIE      (0x0100)
#define MCF_CFM_CFMMCR_PVIE      (0x0200)
#define MCF_CFM_CFMMCR_LOCK      (0x0400)

/* Bit definitions and macros for MCF_CFM_CFMCLKD */
#define MCF_CFM_CFMCLKD_DIV(x)   (((x)&0x3F)<<0)
#define MCF_CFM_CFMCLKD_PRDIV8   (0x40)
#define MCF_CFM_CFMCLKD_DIVLD    (0x80)

/* Bit definitions and macros for MCF_CFM_CFMSEC */
#define MCF_CFM_CFMSEC_SEC(x)    (((x)&0x0000FFFF)<<0)
#define MCF_CFM_CFMSEC_SECSTAT   (0x40000000)
#define MCF_CFM_CFMSEC_KEYEN     (0x80000000)

/* Bit definitions and macros for MCF_CFM_CFMUSTAT */
#define MCF_CFM_CFMUSTAT_BLANK   (0x04)
#define MCF_CFM_CFMUSTAT_ACCERR  (0x10)
#define MCF_CFM_CFMUSTAT_PVIOL   (0x20)
#define MCF_CFM_CFMUSTAT_CCIF    (0x40)
#define MCF_CFM_CFMUSTAT_CBEIF   (0x80)

/* Bit definitions and macros for MCF_CFM_CFMCMD */
#define MCF_CFM_CFMCMD_CMD(x)    (((x)&0x7F)<<0)
#define MCF_CFM_CFMCMD_RDARY1    (0x05)
#define MCF_CFM_CFMCMD_PGM       (0x20)
#define MCF_CFM_CFMCMD_PGERS     (0x40)
#define MCF_CFM_CFMCMD_MASERS    (0x41)
#define MCF_CFM_CFMCMD_PGERSVER  (0x06)

/*********************************************************************
*
* Interrupt Controller (INTC_IACK)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_INTC_IACK_GSWIACK            (*(vuint8 *)(&__IPSBAR[0x000FE0]))
#define MCF_INTC_IACK_GL1IACK            (*(vuint8 *)(&__IPSBAR[0x000FE4]))
#define MCF_INTC_IACK_GL2IACK            (*(vuint8 *)(&__IPSBAR[0x000FE8]))
#define MCF_INTC_IACK_GL3IACK            (*(vuint8 *)(&__IPSBAR[0x000FEC]))
#define MCF_INTC_IACK_GL4IACK            (*(vuint8 *)(&__IPSBAR[0x000FF0]))
#define MCF_INTC_IACK_GL5IACK            (*(vuint8 *)(&__IPSBAR[0x000FF4]))
#define MCF_INTC_IACK_GL6IACK            (*(vuint8 *)(&__IPSBAR[0x000FF8]))
#define MCF_INTC_IACK_GL7IACK            (*(vuint8 *)(&__IPSBAR[0x000FFC]))
#define MCF_INTC_IACK_GLIACK(x)          (*(vuint8 *)(&__IPSBAR[0x000FE4+((x-1)*0x004)]))

/* Bit definitions and macros for MCF_INTC_IACK_GSWIACK */
#define MCF_INTC_IACK_GSWIACK_VECTOR(x)  (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_INTC_IACK_GLIACK */
#define MCF_INTC_IACK_GLIACK_VECTOR(x)   (((x)&0xFF)<<0)

/*********************************************************************
*
* Fast Ethernet Controller (FEC)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_FEC_EIR                   (*(vuint32*)(&__IPSBAR[0x001004]))
#define MCF_FEC_EIMR                  (*(vuint32*)(&__IPSBAR[0x001008]))
#define MCF_FEC_RDAR                  (*(vuint32*)(&__IPSBAR[0x001010]))
#define MCF_FEC_TDAR                  (*(vuint32*)(&__IPSBAR[0x001014]))
#define MCF_FEC_ECR                   (*(vuint32*)(&__IPSBAR[0x001024]))
#define MCF_FEC_MMFR                  (*(vuint32*)(&__IPSBAR[0x001040]))
#define MCF_FEC_MSCR                  (*(vuint32*)(&__IPSBAR[0x001044]))
#define MCF_FEC_MIBC                  (*(vuint32*)(&__IPSBAR[0x001064]))
#define MCF_FEC_RCR                   (*(vuint32*)(&__IPSBAR[0x001084]))
#define MCF_FEC_TCR                   (*(vuint32*)(&__IPSBAR[0x0010C4]))
#define MCF_FEC_PALR                  (*(vuint32*)(&__IPSBAR[0x0010E4]))
#define MCF_FEC_PAUR                  (*(vuint32*)(&__IPSBAR[0x0010E8]))
#define MCF_FEC_OPD                   (*(vuint32*)(&__IPSBAR[0x0010EC]))
#define MCF_FEC_IAUR                  (*(vuint32*)(&__IPSBAR[0x001118]))
#define MCF_FEC_IALR                  (*(vuint32*)(&__IPSBAR[0x00111C]))
#define MCF_FEC_GAUR                  (*(vuint32*)(&__IPSBAR[0x001120]))
#define MCF_FEC_GALR                  (*(vuint32*)(&__IPSBAR[0x001124]))
#define MCF_FEC_TFWR                  (*(vuint32*)(&__IPSBAR[0x001144]))
#define MCF_FEC_FRBR                  (*(vuint32*)(&__IPSBAR[0x00114C]))
#define MCF_FEC_FRSR                  (*(vuint32*)(&__IPSBAR[0x001150]))
#define MCF_FEC_ERDSR                 (*(vuint32*)(&__IPSBAR[0x001180]))
#define MCF_FEC_ETDSR                 (*(vuint32*)(&__IPSBAR[0x001184]))
#define MCF_FEC_EMRBR                 (*(vuint32*)(&__IPSBAR[0x001188]))
#define MCF_FEC_RMON_T_DROP           (*(vuint32*)(&__IPSBAR[0x001200]))
#define MCF_FEC_RMON_T_PACKETS        (*(vuint32*)(&__IPSBAR[0x001204]))
#define MCF_FEC_RMON_T_BC_PKT         (*(vuint32*)(&__IPSBAR[0x001208]))
#define MCF_FEC_RMON_T_MC_PKT         (*(vuint32*)(&__IPSBAR[0x00120C]))
#define MCF_FEC_RMON_T_CRC_ALIGN      (*(vuint32*)(&__IPSBAR[0x001210]))
#define MCF_FEC_RMON_T_UNDERSIZE      (*(vuint32*)(&__IPSBAR[0x001214]))
#define MCF_FEC_RMON_T_OVERSIZE       (*(vuint32*)(&__IPSBAR[0x001218]))
#define MCF_FEC_RMON_T_FRAG           (*(vuint32*)(&__IPSBAR[0x00121C]))
#define MCF_FEC_RMON_T_JAB            (*(vuint32*)(&__IPSBAR[0x001220]))
#define MCF_FEC_RMON_T_COL            (*(vuint32*)(&__IPSBAR[0x001224]))
#define MCF_FEC_RMON_T_P64            (*(vuint32*)(&__IPSBAR[0x001228]))
#define MCF_FEC_RMON_T_P65TO127       (*(vuint32*)(&__IPSBAR[0x00122C]))
#define MCF_FEC_RMON_T_P128TO255      (*(vuint32*)(&__IPSBAR[0x001230]))
#define MCF_FEC_RMON_T_P256TO511      (*(vuint32*)(&__IPSBAR[0x001234]))
#define MCF_FEC_RMON_T_P512TO1023     (*(vuint32*)(&__IPSBAR[0x001238]))
#define MCF_FEC_RMON_T_P1024TO2047    (*(vuint32*)(&__IPSBAR[0x00123C]))
#define MCF_FEC_RMON_T_P_GTE2048      (*(vuint32*)(&__IPSBAR[0x001240]))
#define MCF_FEC_RMON_T_OCTETS         (*(vuint32*)(&__IPSBAR[0x001244]))
#define MCF_FEC_IEEE_T_DROP           (*(vuint32*)(&__IPSBAR[0x001248]))
#define MCF_FEC_IEEE_T_FRAME_OK       (*(vuint32*)(&__IPSBAR[0x00124C]))
#define MCF_FEC_IEEE_T_1COL           (*(vuint32*)(&__IPSBAR[0x001250]))
#define MCF_FEC_IEEE_T_MCOL           (*(vuint32*)(&__IPSBAR[0x001254]))
#define MCF_FEC_IEEE_T_DEF            (*(vuint32*)(&__IPSBAR[0x001258]))
#define MCF_FEC_IEEE_T_LCOL           (*(vuint32*)(&__IPSBAR[0x00125C]))
#define MCF_FEC_IEEE_T_EXCOL          (*(vuint32*)(&__IPSBAR[0x001260]))
#define MCF_FEC_IEEE_T_MACERR         (*(vuint32*)(&__IPSBAR[0x001264]))
#define MCF_FEC_IEEE_T_CSERR          (*(vuint32*)(&__IPSBAR[0x001268]))
#define MCF_FEC_IEEE_T_SQE            (*(vuint32*)(&__IPSBAR[0x00126C]))
#define MCF_FEC_IEEE_T_FDXFC          (*(vuint32*)(&__IPSBAR[0x001270]))
#define MCF_FEC_IEEE_T_OCTETS_OK      (*(vuint32*)(&__IPSBAR[0x001274]))
#define MCF_FEC_RMON_R_PACKETS        (*(vuint32*)(&__IPSBAR[0x001284]))
#define MCF_FEC_RMON_R_BC_PKT         (*(vuint32*)(&__IPSBAR[0x001288]))
#define MCF_FEC_RMON_R_MC_PKT         (*(vuint32*)(&__IPSBAR[0x00128C]))
#define MCF_FEC_RMON_R_CRC_ALIGN      (*(vuint32*)(&__IPSBAR[0x001290]))
#define MCF_FEC_RMON_R_UNDERSIZE      (*(vuint32*)(&__IPSBAR[0x001294]))
#define MCF_FEC_RMON_R_OVERSIZE       (*(vuint32*)(&__IPSBAR[0x001298]))
#define MCF_FEC_RMON_R_FRAG           (*(vuint32*)(&__IPSBAR[0x00129C]))
#define MCF_FEC_RMON_R_JAB            (*(vuint32*)(&__IPSBAR[0x0012A0]))
#define MCF_FEC_RMON_R_RESVD_0        (*(vuint32*)(&__IPSBAR[0x0012A4]))
#define MCF_FEC_RMON_R_P64            (*(vuint32*)(&__IPSBAR[0x0012A8]))
#define MCF_FEC_RMON_R_P65TO127       (*(vuint32*)(&__IPSBAR[0x0012AC]))
#define MCF_FEC_RMON_R_P128TO255      (*(vuint32*)(&__IPSBAR[0x0012B0]))
#define MCF_FEC_RMON_R_P256TO511      (*(vuint32*)(&__IPSBAR[0x0012B4]))
#define MCF_FEC_RMON_R_512TO1023      (*(vuint32*)(&__IPSBAR[0x0012B8]))
#define MCF_FEC_RMON_R_P_GTE2048      (*(vuint32*)(&__IPSBAR[0x0012C0]))
#define MCF_FEC_RMON_R_1024TO2047     (*(vuint32*)(&__IPSBAR[0x0012BC]))
#define MCF_FEC_RMON_R_OCTETS         (*(vuint32*)(&__IPSBAR[0x0012C4]))
#define MCF_FEC_IEEE_R_DROP           (*(vuint32*)(&__IPSBAR[0x0012C8]))
#define MCF_FEC_IEEE_R_FRAME_OK       (*(vuint32*)(&__IPSBAR[0x0012CC]))
#define MCF_FEC_IEEE_R_CRC            (*(vuint32*)(&__IPSBAR[0x0012D0]))
#define MCF_FEC_IEEE_R_ALIGN          (*(vuint32*)(&__IPSBAR[0x0012D4]))
#define MCF_FEC_IEEE_R_MACERR         (*(vuint32*)(&__IPSBAR[0x0012D8]))
#define MCF_FEC_IEEE_R_FDXFC          (*(vuint32*)(&__IPSBAR[0x0012DC]))
#define MCF_FEC_IEEE_R_OCTETS_OK      (*(vuint32*)(&__IPSBAR[0x0012E0]))

/* Bit definitions and macros for MCF_FEC_EIR */
#define MCF_FEC_EIR_UN                (0x00080000)
#define MCF_FEC_EIR_RL                (0x00100000)
#define MCF_FEC_EIR_LC                (0x00200000)
#define MCF_FEC_EIR_EBERR             (0x00400000)
#define MCF_FEC_EIR_MII               (0x00800000)
#define MCF_FEC_EIR_RXB               (0x01000000)
#define MCF_FEC_EIR_RXF               (0x02000000)
#define MCF_FEC_EIR_TXB               (0x04000000)
#define MCF_FEC_EIR_TXF               (0x08000000)
#define MCF_FEC_EIR_GRA               (0x10000000)
#define MCF_FEC_EIR_BABT              (0x20000000)
#define MCF_FEC_EIR_BABR              (0x40000000)
#define MCF_FEC_EIR_HBERR             (0x80000000)
#define MCF_FEC_EIR_CLEAR_ALL         (0xFFFFFFFF)

/* Bit definitions and macros for MCF_FEC_EIMR */
#define MCF_FEC_EIMR_UN               (0x00080000)
#define MCF_FEC_EIMR_RL               (0x00100000)
#define MCF_FEC_EIMR_LC               (0x00200000)
#define MCF_FEC_EIMR_EBERR            (0x00400000)
#define MCF_FEC_EIMR_MII              (0x00800000)
#define MCF_FEC_EIMR_RXB              (0x01000000)
#define MCF_FEC_EIMR_RXF              (0x02000000)
#define MCF_FEC_EIMR_TXB              (0x04000000)
#define MCF_FEC_EIMR_TXF              (0x08000000)
#define MCF_FEC_EIMR_GRA              (0x10000000)
#define MCF_FEC_EIMR_BABT             (0x20000000)
#define MCF_FEC_EIMR_BABR             (0x40000000)
#define MCF_FEC_EIMR_HBERR            (0x80000000)
#define MCF_FEC_EIMR_MASK_ALL         (0x00000000)
#define MCF_FEC_EIMR_UNMASK_ALL       (0xFFFFFFFF)

/* Bit definitions and macros for MCF_FEC_RDAR */
#define MCF_FEC_RDAR_R_DES_ACTIVE     (0x01000000)

/* Bit definitions and macros for MCF_FEC_TDAR */
#define MCF_FEC_TDAR_X_DES_ACTIVE     (0x01000000)

/* Bit definitions and macros for MCF_FEC_ECR */
#define MCF_FEC_ECR_RESET             (0x00000001)
#define MCF_FEC_ECR_ETHER_EN          (0x00000002)

/* Bit definitions and macros for MCF_FEC_MMFR */
#define MCF_FEC_MMFR_DATA(x)          (((x)&0x0000FFFF)<<0)
#define MCF_FEC_MMFR_TA(x)            (((x)&0x00000003)<<16)
#define MCF_FEC_MMFR_RA(x)            (((x)&0x0000001F)<<18)
#define MCF_FEC_MMFR_PA(x)            (((x)&0x0000001F)<<23)
#define MCF_FEC_MMFR_OP(x)            (((x)&0x00000003)<<28)
#define MCF_FEC_MMFR_ST(x)            (((x)&0x00000003)<<30)
#define MCF_FEC_MMFR_ST_01            (0x40000000)
#define MCF_FEC_MMFR_OP_READ          (0x20000000)
#define MCF_FEC_MMFR_OP_WRITE         (0x10000000)
#define MCF_FEC_MMFR_TA_10            (0x00020000)

/* Bit definitions and macros for MCF_FEC_MSCR */
#define MCF_FEC_MSCR_MII_SPEED(x)     (((x)&0x0000003F)<<1)
#define MCF_FEC_MSCR_DIS_PREAMBLE     (0x00000080)

/* Bit definitions and macros for MCF_FEC_MIBC */
#define MCF_FEC_MIBC_MIB_IDLE         (0x40000000)
#define MCF_FEC_MIBC_MIB_DISABLE      (0x80000000)

/* Bit definitions and macros for MCF_FEC_RCR */
#define MCF_FEC_RCR_LOOP              (0x00000001)
#define MCF_FEC_RCR_DRT               (0x00000002)
#define MCF_FEC_RCR_MII_MODE          (0x00000004)
#define MCF_FEC_RCR_PROM              (0x00000008)
#define MCF_FEC_RCR_BC_REJ            (0x00000010)
#define MCF_FEC_RCR_FCE               (0x00000020)
#define MCF_FEC_RCR_MAX_FL(x)         (((x)&0x000007FF)<<16)

/* Bit definitions and macros for MCF_FEC_TCR */
#define MCF_FEC_TCR_GTS               (0x00000001)
#define MCF_FEC_TCR_HBC               (0x00000002)
#define MCF_FEC_TCR_FDEN              (0x00000004)
#define MCF_FEC_TCR_TFC_PAUSE         (0x00000008)
#define MCF_FEC_TCR_RFC_PAUSE         (0x00000010)

/* Bit definitions and macros for MCF_FEC_PALR */
#define MCF_FEC_PALR_PADDR1(x)        (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_PAUR */
#define MCF_FEC_PAUR_TYPE(x)          (((x)&0x0000FFFF)<<0)
#define MCF_FEC_PAUR_PADDR2(x)        (((x)&0x0000FFFF)<<16)

/* Bit definitions and macros for MCF_FEC_OPD */
#define MCF_FEC_OPD_PAUSE_DUR(x)      (((x)&0x0000FFFF)<<0)
#define MCF_FEC_OPD_OPCODE(x)         (((x)&0x0000FFFF)<<16)

/* Bit definitions and macros for MCF_FEC_IAUR */
#define MCF_FEC_IAUR_IADDR1(x)        (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_IALR */
#define MCF_FEC_IALR_IADDR2(x)        (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_GAUR */
#define MCF_FEC_GAUR_GADDR1(x)        (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_GALR */
#define MCF_FEC_GALR_GADDR2(x)        (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_TFWR */
#define MCF_FEC_TFWR_X_WMRK(x)        (((x)&0x00000003)<<0)

/* Bit definitions and macros for MCF_FEC_FRBR */
#define MCF_FEC_FRBR_R_BOUND(x)       (((x)&0x000000FF)<<2)

/* Bit definitions and macros for MCF_FEC_FRSR */
#define MCF_FEC_FRSR_R_FSTART(x)      (((x)&0x000000FF)<<2)

/* Bit definitions and macros for MCF_FEC_ERDSR */
#define MCF_FEC_ERDSR_R_DES_START(x)  (((x)&0x3FFFFFFF)<<2)

/* Bit definitions and macros for MCF_FEC_ETDSR */
#define MCF_FEC_ETDSR_X_DES_START(x)  (((x)&0x3FFFFFFF)<<2)

/* Bit definitions and macros for MCF_FEC_EMRBR */
#define MCF_FEC_EMRBR_R_BUF_SIZE(x)   (((x)&0x0000007F)<<4)

/*********************************************************************
*
* Ethernet PHY (PHY)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_PHY_EPHYCTL0             (*(vuint8 *)(&__IPSBAR[0x1E0000]))
#define MCF_PHY_EPHYCTL1             (*(vuint8 *)(&__IPSBAR[0x1E0001]))
#define MCF_PHY_EPHYSR               (*(vuint8 *)(&__IPSBAR[0x1E0002]))

/* Bit definitions and macros for MCF_PHY_EPHYCTL0 */
#define MCF_PHY_EPHYCTL0_EPHYIEN     (0x01)
#define MCF_PHY_EPHYCTL0_EPHYWAI     (0x04)
#define MCF_PHY_EPHYCTL0_LEDEN       (0x08)
#define MCF_PHY_EPHYCTL0_DIS10       (0x10)
#define MCF_PHY_EPHYCTL0_DIS100      (0x20)
#define MCF_PHY_EPHYCTL0_ANDIS       (0x40)
#define MCF_PHY_EPHYCTL0_EPHYEN      (0x80)

/* Bit definitions and macros for MCF_PHY_EPHYCTL1 */
#define MCF_PHY_EPHYCTL1_PHYADDR(x)  (((x)&0x1F)<<0)

/* Bit definitions and macros for MCF_PHY_EPHYSR */
#define MCF_PHY_EPHYSR_EPHYIF        (0x01)
#define MCF_PHY_EPHYSR_10DIS         (0x10)
#define MCF_PHY_EPHYSR_100DIS        (0x20)

/*********************************************************************
*
* Random Number Generator (RNG)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_RNG_RNGCR             (*(vuint32*)(&__IPSBAR[0x1F0000]))
#define MCF_RNG_RNGSR             (*(vuint32*)(&__IPSBAR[0x1F0004]))
#define MCF_RNG_RNGER             (*(vuint32*)(&__IPSBAR[0x1F0008]))
#define MCF_RNG_RNGOUT            (*(vuint32*)(&__IPSBAR[0x1F000C]))

/* Bit definitions and macros for MCF_RNG_RNGCR */
#define MCF_RNG_RNGCR_GO          (0x00000001)
#define MCF_RNG_RNGCR_HA          (0x00000002)
#define MCF_RNG_RNGCR_IM          (0x00000004)
#define MCF_RNG_RNGCR_CI          (0x00000008)

/* Bit definitions and macros for MCF_RNG_RNGSR */
#define MCF_RNG_RNGSR_SV          (0x00000001)
#define MCF_RNG_RNGSR_LRS         (0x00000002)
#define MCF_RNG_RNGSR_FUF         (0x00000004)
#define MCF_RNG_RNGSR_EI          (0x00000008)
#define MCF_RNG_RNGSR_OFL(x)      (((x)&0x000000FF)<<8)
#define MCF_RNG_RNGSR_OFS(x)      (((x)&0x000000FF)<<16)

/* Bit definitions and macros for MCF_RNG_RNGER */
#define MCF_RNG_RNGER_ENTROPY(x)  (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_RNG_RNGOUT */
#define MCF_RNG_RNGOUT_OUTPUT(x)  (((x)&0xFFFFFFFF)<<0)

/*********************************************************************
*
* Real-time Clock (RTC)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_RTC_HOURMIN              (*(vuint32*)(&__IPSBAR[0x0003C0]))
#define MCF_RTC_SECONDS              (*(vuint32*)(&__IPSBAR[0x0003C4]))
#define MCF_RTC_ALRM_HM              (*(vuint32*)(&__IPSBAR[0x0003C8]))
#define MCF_RTC_ALRM_SEC             (*(vuint32*)(&__IPSBAR[0x0003CC]))
#define MCF_RTC_CR                   (*(vuint32*)(&__IPSBAR[0x0003D0]))
#define MCF_RTC_ISR                  (*(vuint32*)(&__IPSBAR[0x0003D4]))
#define MCF_RTC_IER                  (*(vuint32*)(&__IPSBAR[0x0003D8]))
#define MCF_RTC_STPWCH               (*(vuint32*)(&__IPSBAR[0x0003DC]))
#define MCF_RTC_DAYS                 (*(vuint32*)(&__IPSBAR[0x0003E0]))
#define MCF_RTC_ALRM_DAY             (*(vuint32*)(&__IPSBAR[0x0003E4]))

/* Bit definitions and macros for MCF_RTC_HOURMIN */
#define MCF_RTC_HOURMIN_MINUTES(x)   (((x)&0x0000003F)<<0)
#define MCF_RTC_HOURMIN_HOURS(x)     (((x)&0x0000001F)<<8)

/* Bit definitions and macros for MCF_RTC_SECONDS */
#define MCF_RTC_SECONDS_SECONDS(x)   (((x)&0x0000003F)<<0)

/* Bit definitions and macros for MCF_RTC_ALRM_HM */
#define MCF_RTC_ALRM_HM_MINUTES(x)   (((x)&0x0000003F)<<0)
#define MCF_RTC_ALRM_HM_HOURS(x)     (((x)&0x0000001F)<<8)

/* Bit definitions and macros for MCF_RTC_ALRM_SEC */
#define MCF_RTC_ALRM_SEC_SECONDS(x)  (((x)&0x0000003F)<<0)

/* Bit definitions and macros for MCF_RTC_CR */
#define MCF_RTC_CR_SWR               (0x00000001)
#define MCF_RTC_CR_XTL(x)            (((x)&0x00000003)<<5)
#define MCF_RTC_CR_EN                (0x00000080)
#define MCF_RTC_CR_32768             (0x0)
#define MCF_RTC_CR_32000             (0x1)
#define MCF_RTC_CR_38400             (0x2)

/* Bit definitions and macros for MCF_RTC_ISR */
#define MCF_RTC_ISR_SW               (0x00000001)
#define MCF_RTC_ISR_MIN              (0x00000002)
#define MCF_RTC_ISR_ALM              (0x00000004)
#define MCF_RTC_ISR_DAY              (0x00000008)
#define MCF_RTC_ISR_1HZ              (0x00000010)
#define MCF_RTC_ISR_HR               (0x00000020)
#define MCF_RTC_ISR_2HZ              (0x00000080)
#define MCF_RTC_ISR_SAM0             (0x00000100)
#define MCF_RTC_ISR_SAM1             (0x00000200)
#define MCF_RTC_ISR_SAM2             (0x00000400)
#define MCF_RTC_ISR_SAM3             (0x00000800)
#define MCF_RTC_ISR_SAM4             (0x00001000)
#define MCF_RTC_ISR_SAM5             (0x00002000)
#define MCF_RTC_ISR_SAM6             (0x00004000)
#define MCF_RTC_ISR_SAM7             (0x00008000)

/* Bit definitions and macros for MCF_RTC_IER */
#define MCF_RTC_IER_SW               (0x00000001)
#define MCF_RTC_IER_MIN              (0x00000002)
#define MCF_RTC_IER_ALM              (0x00000004)
#define MCF_RTC_IER_DAY              (0x00000008)
#define MCF_RTC_IER_1HZ              (0x00000010)
#define MCF_RTC_IER_HR               (0x00000020)
#define MCF_RTC_IER_2HZ              (0x00000080)
#define MCF_RTC_IER_SAM0             (0x00000100)
#define MCF_RTC_IER_SAM1             (0x00000200)
#define MCF_RTC_IER_SAM2             (0x00000400)
#define MCF_RTC_IER_SAM3             (0x00000800)
#define MCF_RTC_IER_SAM4             (0x00001000)
#define MCF_RTC_IER_SAM5             (0x00002000)
#define MCF_RTC_IER_SAM6             (0x00004000)
#define MCF_RTC_IER_SAM7             (0x00008000)

/* Bit definitions and macros for MCF_RTC_STPWCH */
#define MCF_RTC_STPWCH_CNT(x)        (((x)&0x0000003F)<<0)

/* Bit definitions and macros for MCF_RTC_DAYS */
#define MCF_RTC_DAYS_DAYS(x)         (((x)&0x0000FFFF)<<0)

/* Bit definitions and macros for MCF_RTC_ALRM_DAY */
#define MCF_RTC_ALRM_DAY_DAYS(x)     (((x)&0x0000FFFF)<<0)

/********************************************************************/

#endif /* __MCF5223x_H__ */
