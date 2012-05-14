/**
 *@file bf52x.h
 *
 *  This file defines basic MMR for the Blackfin 52x CPU.
 *  The MMR have been taken from the ADSP-BF52x Blackfin Processor
 *  Hardware Reference from Analog Devices. Mentioned Chapters
 *  refer to this Documentation.
 *
 *	Based on bf533.h
 *
 *  COPYRIGHT (c) 2006.
 *  Atos Automacao Industrial LTDA.
 *             modified by Alain Schaefer <alain.schaefer@easc.ch>
 *                     and Antonio Giovanini <antonio@atos.com.br>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *
 * @author    Rohan Kangralkar, ECE Department Northeastern University
 * @date	  02/15/2011
 *
 * HISTORY:
 *
 */

#ifndef _RTEMS_BFIN_52x_H
#define _RTEMS_BFIN_52x_H

#include <rtems/bfin/bfin.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Clock and System Control  Chapter 8 */
#define PLL_CTL                0xFFC00000L
#define PLL_DIV                0xFFC00004L
#define VR_CTL                 0xFFC00008L
#define PLL_STAT               0xFFC0000CL
#define PLL_LOCKCNT            0xFFC00010L
#define SWRST                  0xFFC00100L
#define SYSCR                  0xFFC00104L

/* SPI Controller           Chapter 10 */
#define SPI_CTL                0xFFC00500L
#define SPI_FLG                0xFFC00504L
#define SPI_STAT               0xFFC00508L
#define SPI_TDBR               0xFFC0050CL
#define SPI_RDBR               0xFFC00510L
#define SPI_BAUD               0xFFC00514L
#define SPI_SHADOW             0xFFC00518L

/* SPORT0 Controller */
#define SPORT0_TCR1            0xFFC00800L
#define SPORT0_TCR2            0xFFC00804L
#define SPORT0_TCLKDIV         0xFFC00808L
#define SPORT0_TFSDIV          0xFFC0080CL
#define SPORT0_TX              0xFFC00810L
#define SPORT0_RX              0xFFC00818L
#define SPORT0_RCR1            0xFFC00820L
#define SPORT0_RCR2            0xFFC00824L
#define SPORT0_RCLKDIV         0xFFC00828L
#define SPORT0_RFSDIV          0xFFC0082CL
#define SPORT0_STAT            0xFFC00830L
#define SPORT0_CHNL            0xFFC00834L
#define SPORT0_MCMC1           0xFFC00838L
#define SPORT0_MCMC2           0xFFC0083CL
#define SPORT0_MTCS0           0xFFC00840L
#define SPORT0_MTCS1           0xFFC00844L
#define SPORT0_MTCS2           0xFFC00848L
#define SPORT0_MTCS3           0xFFC0084CL
#define SPORT0_MRCS0           0xFFC00850L
#define SPORT0_MRCS1           0xFFC00854L
#define SPORT0_MRCS2           0xFFC00858L
#define SPORT0_MRCS3           0xFFC0085CL

/* Parallel Peripheral Interface (PPI) Chapter 11 */

#define PPI_CONTROL            0xFFC01000L
#define PPI_STATUS             0xFFC01004L
#define PPI_COUNT              0xFFC01008L
#define PPI_DELAY              0xFFC0100CL
#define PPI_FRAME              0xFFC01010L

/*********  PPI MASKS ***********/
/*  PPI_CONTROL Masks */
#define PORT_EN                0x00000001
#define PORT_DIR               0x00000002
#define XFR_TYPE               0x0000000C
#define PORT_CFG               0x00000030
#define FLD_SEL                0x00000040
#define PACK_EN                0x00000080
#define DMA32                  0x00000100
#define SKIP_EN                0x00000200
#define SKIP_EO                0x00000400
#define DLENGTH                0x00003800
#define DLEN_8                 0x0
#define DLEN(x)                (((x-9) & 0x07) << 11)
#define POL                    0x0000C000

/* PPI_STATUS Masks */
#define FLD                    0x00000400
#define FT_ERR                 0x00000800
#define OVR                    0x00001000
#define UNDR                   0x00002000
#define ERR_DET                0x00004000
#define ERR_NCOR               0x00008000

/* SPORT1 Controller        Chapter 12 */
#define SPORT1_TCR1            0xFFC00900L
#define SPORT1_TCR2            0xFFC00904L
#define SPORT1_TCLKDIV         0xFFC00908L
#define SPORT1_TFSDIV          0xFFC0090CL
#define SPORT1_TX              0xFFC00910L
#define SPORT1_RX              0xFFC00918L
#define SPORT1_RCR1            0xFFC00920L
#define SPORT1_RCR2            0xFFC00924L
#define SPORT1_RCLKDIV         0xFFC00928L
#define SPORT1_RFSDIV          0xFFC0092CL
#define SPORT1_STAT            0xFFC00930L
#define SPORT1_CHNL            0xFFC00934L
#define SPORT1_MCMC1           0xFFC00938L
#define SPORT1_MCMC2           0xFFC0093CL
#define SPORT1_MTCS0           0xFFC00940L
#define SPORT1_MTCS1           0xFFC00944L
#define SPORT1_MTCS2           0xFFC00948L
#define SPORT1_MTCS3           0xFFC0094CL
#define SPORT1_MRCS0           0xFFC00950L
#define SPORT1_MRCS1           0xFFC00954L
#define SPORT1_MRCS2           0xFFC00958L
#define SPORT1_MRCS3           0xFFC0095CL

/* SPORTx_TCR1 Masks */
#define TSPEN                  0x0001
#define ITCLK                  0x0002
#define TDTYPE                 0x000C
#define TLSBIT                 0x0010
#define ITFS                   0x0200
#define TFSR                   0x0400
#define DITFS                  0x0800
#define LTFS                   0x1000
#define LATFS                  0x2000
#define TCKFE                  0x4000

/* SPORTx_TCR2 Masks */
#define SLEN                   0x001F
#define TXSE                   0x0100
#define TSFSE                  0x0200
#define TRFST                  0x0400

/* SPORTx_RCR1 Masks */
#define RSPEN                  0x0001
#define IRCLK                  0x0002
#define RDTYPE                 0x000C
#define RULAW                  0x0008
#define RALAW                  0x000C
#define RLSBIT                 0x0010
#define IRFS                   0x0200
#define RFSR                   0x0400
#define LRFS                   0x1000
#define LARFS                  0x2000
#define RCKFE                  0x4000

/* SPORTx_RCR2 Masks */
#define SLEN                   0x001F
#define RXSE                   0x0100
#define RSFSE                  0x0200
#define RRFST                  0x0400

/* SPORTx_STAT Masks */
#define RXNE                   0x0001
#define RUVF                   0x0002
#define ROVF                   0x0004
#define TXF                    0x0008
#define TUVF                   0x0010
#define TOVF                   0x0020
#define TXHRE                  0x0040

/* SPORTx_MCMC1 Masks */
#define WSIZE                  0x0000F000
#define WOFF                   0x000003FF

/* SPORTx_MCMC2 Masks */
#define MCCRM                  0x00000003
#define MCDTXPE                0x00000004
#define MCDRXPE                0x00000008
#define MCMEN                  0x00000010
#define FSDR                   0x00000080
#define MFD                    0x0000F000

/* UART Controller          Chapter 13 */
#define UART_THR               0xFFC00400L
#define UART_RBR               0xFFC00400L
#define UART_DLL               0xFFC00400L
#define UART_IER               0xFFC00404L
#define UART_DLH               0xFFC00404L
#define UART_IIR               0xFFC00408L
#define UART_LCR               0xFFC0040CL
#define UART_MCR               0xFFC00410L
#define UART_LSR               0xFFC00414L
#define UART_SCR               0xFFC0041CL
#define UART_GCTL              0xFFC00424L

/*
 * UART CONTROLLER MASKS
 */

/* UART_LCR */
#define DLAB                   0x80
#define SB                     0x40
#define STP                    0x20
#define EPS                    0x10
#define PEN                    0x08
#define STB                    0x04
#define WLS(x)                 ((x-5) & 0x03)

#define DLAB_P                 0x07
#define SB_P                   0x06
#define STP_P                  0x05
#define EPS_P                  0x04
#define PEN_P                  0x03
#define STB_P                  0x02
#define WLS_P1                 0x01
#define WLS_P0                 0x00

/* UART_MCR */
#define LOOP_ENA               0x10
#define LOOP_ENA_P             0x04

/* UART_LSR */
#define TEMT                   0x40
#define THRE                   0x20
#define BI                     0x10
#define FE                     0x08
#define PE                     0x04
#define OE                     0x02
#define DR                     0x01

#define TEMP_P                 0x06
#define THRE_P                 0x05
#define BI_P                   0x04
#define FE_P                   0x03
#define PE_P                   0x02
#define OE_P                   0x01
#define DR_P                   0x00

/* UART_IER */
#define ELSI                   0x04
#define ETBEI                  0x02
#define ERBFI                  0x01

#define ELSI_P                 0x02
#define ETBEI_P                0x01
#define ERBFI_P                0x00

/* UART_IIR */
#define STATUS(x)              ((x << 1) & 0x06)
#define NINT                   0x01
#define STATUS_P1              0x02
#define STATUS_P0              0x01
#define NINT_P                 0x00

/* UART_GCTL */
#define FFE                    0x20
#define FPE                    0x10
#define RPOLC                  0x08
#define TPOLC                  0x04
#define IREN                   0x02
#define UCEN                   0x01

#define FFE_P                  0x05
#define FPE_P                  0x04
#define RPOLC_P                0x03
#define TPOLC_P                0x02
#define IREN_P                 0x01
#define UCEN_P                 0x00

/* General Purpose IO        Chapter 14*/
#define FIO_FLAG_D             0xFFC00700L
#define FIO_FLAG_C             0xFFC00704L
#define FIO_FLAG_S             0xFFC00708L
#define FIO_FLAG_T             0xFFC0070CL
#define FIO_MASKA_D            0xFFC00710L
#define FIO_MASKA_C            0xFFC00714L
#define FIO_MASKA_S            0xFFC00718L
#define FIO_MASKA_T            0xFFC0071CL
#define FIO_MASKB_D            0xFFC00720L
#define FIO_MASKB_C            0xFFC00724L
#define FIO_MASKB_S            0xFFC00728L
#define FIO_MASKB_T            0xFFC0072CL
#define FIO_DIR                0xFFC00730L
#define FIO_POLAR              0xFFC00734L
#define FIO_EDGE               0xFFC00738L
#define FIO_BOTH               0xFFC0073CL
#define FIO_INEN               0xFFC00740L


/* General Purpose IO        Chapter 9*/
#define PORTH_FER              0xFFC03208
#define PORTH_MUX              0xFFC03218
#define PORTHIO_DIR            0xFFC01730
#define PORTHIO_INEN           0xFFC01740
#define PORTHIO                0xFFC01700
#define PORTHIO_SET            0xFFC01708
#define PORTHIO_CLEAR          0xFFC01704
#define PORTHIO_TOGGLE         0xFFC0170C


#define FIO_INEN               0xFFC00740L
#define FIO_POLAR              0xFFC00734L
#define FIO_EDGE               0xFFC00738L
#define FIO_BOTH               0xFFC0073CL



#define FIO_FLAG_C             0xFFC00704L
#define FIO_FLAG_S             0xFFC00708L
#define FIO_FLAG_T             0xFFC0070CL
#define FIO_MASKA_D            0xFFC00710L
#define FIO_MASKA_C            0xFFC00714L
#define FIO_MASKA_S            0xFFC00718L
#define FIO_MASKA_T            0xFFC0071CL
#define FIO_MASKB_D            0xFFC00720L
#define FIO_MASKB_C            0xFFC00724L
#define FIO_MASKB_S            0xFFC00728L
#define FIO_MASKB_T            0xFFC0072CL


/*  General Purpose IO Masks */
#define PF0                    0x0001
#define PF1                    0x0002
#define PF2                    0x0004
#define PF3                    0x0008
#define PF4                    0x0010
#define PF5                    0x0020
#define PF6                    0x0040
#define PF7                    0x0080
#define PF8                    0x0100
#define PF9                    0x0200
#define PF10                   0x0400
#define PF11                   0x0800
#define PF12                   0x1000
#define PF13                   0x2000
#define PF14                   0x4000
#define PF15                   0x8000


/* TIMER 0, 1, 2            Chapter 15 */
#define TIMER0_CONFIG          0xFFC00600L
#define TIMER0_COUNTER         0xFFC00604L
#define TIMER0_PERIOD          0xFFC00608L
#define TIMER0_WIDTH           0xFFC0060CL

#define TIMER1_CONFIG          0xFFC00610L
#define TIMER1_COUNTER         0xFFC00614L
#define TIMER1_PERIOD          0xFFC00618L
#define TIMER1_WIDTH           0xFFC0061CL

#define TIMER2_CONFIG          0xFFC00620L
#define TIMER2_COUNTER         0xFFC00624L
#define TIMER2_PERIOD          0xFFC00628L
#define TIMER2_WIDTH           0xFFC0062CL

#define TIMER_ENABLE           0xFFC00640L
#define TIMER_DISABLE          0xFFC00644L
#define TIMER_STATUS           0xFFC00648L

/* Real Time Clock          Chapter 16 */
#define RTC_STAT               0xFFC00300L
#define RTC_ICTL               0xFFC00304L
#define RTC_ISTAT              0xFFC00308L
#define RTC_SWCNT              0xFFC0030CL
#define RTC_ALARM              0xFFC00310L
#define RTC_FAST               0xFFC00314L
#define RTC_PREN               0xFFC00314L

/* RTC_FAST Mask (RTC_PREN Mask) */
#define ENABLE_PRESCALE        0x00000001
#define PREN                   0x00000001

/* Asynchronous Memory Controller EBUI, Chapter 17*/
#define EBIU_AMGCTL            0xFFC00A00L
#define EBIU_AMBCTL0           0xFFC00A04L
#define EBIU_AMBCTL1           0xFFC00A08L

/* SDRAM Controller External Bus Interface Unit */

#define EBIU_SDGCTL            0xFFC00A10L
#define EBIU_SDBCTL            0xFFC00A14L
#define EBIU_SDRRC             0xFFC00A18L
#define EBIU_SDSTAT            0xFFC00A1CL




/* DCPLB_DATA and ICPLB_DATA Registers */
/*** Bit Positions */
#define CPLB_VALID_P            0x00000000  /* 0=invalid entry, 1=valid entry */
#define CPLB_LOCK_P             0x00000001  /* 0=entry may be replaced, 1=entry locked */
#define CPLB_USER_RD_P          0x00000002  /* 0=no read access, 1=read access allowed (user mode) */
/*** Masks */
#define CPLB_VALID             0x00000001  /* 0=invalid entry, 1=valid entry */
#define CPLB_LOCK              0x00000002  /* 0=entry may be replaced, 1=entry locked */
#define CPLB_USER_RD           0x00000004  /* 0=no read access, 1=read access allowed (user mode) */
#define PAGE_SIZE_1KB          0x00000000  /* 1 KB page size */
#define PAGE_SIZE_4KB          0x00010000  /* 4 KB page size */
#define PAGE_SIZE_1MB          0x00020000  /* 1 MB page size */
#define PAGE_SIZE_4MB          0x00030000  /* 4 MB page size */
#define CPLB_PORTPRIO             0x00000200  /* 0=low priority port, 1= high priority port */
#define CPLB_L1_CHBL           0x00001000  /* 0=non-cacheable in L1, 1=cacheable in L1 */
/*** ICPLB_DATA only */
#define CPLB_LRUPRIO              0x00000100  /* 0=can be replaced by any line, 1=priority for non-replacement */
/*** DCPLB_DATA only */
#define CPLB_USER_WR           0x00000008  /* 0=no write access, 0=write access allowed (user mode) */
#define CPLB_SUPV_WR           0x00000010  /* 0=no write access, 0=write access allowed (supervisor mode) */
#define CPLB_DIRTY             0x00000080  /* 1=dirty, 0=clean */
#define CPLB_L1_AOW                       0x00008000  /* 0=do not allocate cache lines on write-through writes,  */
                                                                                  /* 1= allocate cache lines on write-through writes. */
#define CPLB_WT                0x00004000  /* 0=write-back, 1=write-through */


#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_BFIN_H */
