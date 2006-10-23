/*  bfin.h
 *
 *  This file sets up basic CPU dependency settings based on 
 *  compiler settings.  For example, it can determine if
 *  floating point is available.  This particular implementation
 *  is specified to the Blackfin port.
 *
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *             modified by Alain Schaefer <alain.schaefer@easc.ch>
 *                     and Antonio Giovanini <antonio@atos.com.br>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 *
 */

#ifndef _RTEMS_SCORE_BFIN_H
#define _RTEMS_SCORE_BFIN_H

#ifdef __cplusplus
extern "C" {
#endif


/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the Blackfin family.
 *  It does this by setting variables to indicate which
 *  implementation dependent features are present in a particular
 *  member of the family.
 *
 *  This is a good place to list all the known CPU models
 *  that this port supports and which RTEMS CPU model they correspond
 *  to.
 */
 
/*
 *  Figure out all CPU Model Feature Flags based upon compiler 
 *  predefines. 
 */
#if defined(BFIN)
#define CPU_MODEL_NAME  "BF533"
#define BF_HAS_FPU   0
#else
 
#error "Unsupported CPU Model"
 
#endif

/*
 *  Define the name of the CPU family.
 */
 
#define CPU_NAME "BFIN"

#define MK_BMSK_( x ) (1<<x)    

#define LO(con32) ((con32) & 0xFFFF)
#define lo(con32) ((con32) & 0xFFFF)
#define HI(con32) (((con32) >> 16) & 0xFFFF)
#define hi(con32) (((con32) >> 16) & 0xFFFF)

/* Scratchpad SRAM */
 
#define SCRATCH                0xFFB00000
#define SCRATCH_SIZE           0x1000
#define SCRATCH_TOP            0xFFB00ffc



/* System Interrupt Controller Chapter 4*/
#define SIC_RVECT              0xFFC00108
#define SIC_IMASK              0xFFC0010C
#define SIC_IAR0               0xFFC00110
#define SIC_IAR1               0xFFC00114
#define SIC_IAR2               0xFFC00118
#define SIC_ISR                0xFFC00120
#define SIC_IWR                0xFFC00124

/* Event Vector Table        Chapter 4 */

#define EVT0                   0xFFE02000  
#define EVT1                   0xFFE02004  
#define EVT2                   0xFFE02008  
#define EVT3                   0xFFE0200C  
#define EVT4                   0xFFE02010  
#define EVT5                   0xFFE02014  
#define EVT6                   0xFFE02018  
#define EVT7                   0xFFE0201C  
#define EVT8                   0xFFE02020  
#define EVT9                   0xFFE02024  
#define EVT10                  0xFFE02028  
#define EVT11                  0xFFE0202C  
#define EVT12                  0xFFE02030  
#define EVT13                  0xFFE02034  
#define EVT14                  0xFFE02038  
#define EVT15                  0xFFE0203C  
#define IMASK                  0xFFE02104  
#define IPEND                  0xFFE02108  
#define ILAT                   0xFFE0210C  
#define IPRIO                  0xFFE02110  

/* Clock and System Control  Chapter 8 */
#define PLL_CTL                0xFFC00000
#define PLL_DIV                0xFFC00004
#define VR_CTL                 0xFFC00008
#define PLL_STAT               0xFFC0000C
#define PLL_LOCKCNT            0xFFC00010
#define SWRST                  0xFFC00100
#define SYSCR                  0xFFC00104

/* SPI Controller           Chapter 10 */
#define SPI_CTL                0xFFC00500
#define SPI_FLG                0xFFC00504
#define SPI_STAT               0xFFC00508
#define SPI_TDBR               0xFFC0050C
#define SPI_RDBR               0xFFC00510
#define SPI_BAUD               0xFFC00514
#define SPI_SHADOW             0xFFC00518

/* SPORT0 Controller */
#define SPORT0_TCR1            0xFFC00800 
#define SPORT0_TCR2            0xFFC00804
#define SPORT0_TCLKDIV         0xFFC00808
#define SPORT0_TFSDIV          0xFFC0080C
#define SPORT0_TX              0xFFC00810
#define SPORT0_RX              0xFFC00818
#define SPORT0_RCR1            0xFFC00820
#define SPORT0_RCR2            0xFFC00824
#define SPORT0_RCLKDIV         0xFFC00828
#define SPORT0_RFSDIV          0xFFC0082C
#define SPORT0_STAT            0xFFC00830
#define SPORT0_CHNL            0xFFC00834
#define SPORT0_MCMC1           0xFFC00838
#define SPORT0_MCMC2           0xFFC0083C
#define SPORT0_MTCS0           0xFFC00840
#define SPORT0_MTCS1           0xFFC00844
#define SPORT0_MTCS2           0xFFC00848
#define SPORT0_MTCS3           0xFFC0084C
#define SPORT0_MRCS0           0xFFC00850
#define SPORT0_MRCS1           0xFFC00854
#define SPORT0_MRCS2           0xFFC00858
#define SPORT0_MRCS3           0xFFC0085C

/* Parallel Peripheral Interface (PPI) Chapter 11 */
  
#define PPI_CONTROL            0xFFC01000
#define PPI_STATUS             0xFFC01004
#define PPI_COUNT              0xFFC01008
#define PPI_DELAY              0xFFC0100C
#define PPI_FRAME              0xFFC01010

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
#define SPORT1_TCR1            0xFFC00900
#define SPORT1_TCR2            0xFFC00904
#define SPORT1_TCLKDIV         0xFFC00908
#define SPORT1_TFSDIV          0xFFC0090C
#define SPORT1_TX              0xFFC00910
#define SPORT1_RX              0xFFC00918
#define SPORT1_RCR1            0xFFC00920
#define SPORT1_RCR2            0xFFC00924
#define SPORT1_RCLKDIV         0xFFC00928
#define SPORT1_RFSDIV          0xFFC0092C
#define SPORT1_STAT            0xFFC00930
#define SPORT1_CHNL            0xFFC00934
#define SPORT1_MCMC1           0xFFC00938
#define SPORT1_MCMC2           0xFFC0093C
#define SPORT1_MTCS0           0xFFC00940
#define SPORT1_MTCS1           0xFFC00944
#define SPORT1_MTCS2           0xFFC00948
#define SPORT1_MTCS3           0xFFC0094C
#define SPORT1_MRCS0           0xFFC00950
#define SPORT1_MRCS1           0xFFC00954
#define SPORT1_MRCS2           0xFFC00958
#define SPORT1_MRCS3           0xFFC0095C

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
#define UART_THR               0xFFC00400
#define UART_RBR               0xFFC00400
#define UART_DLL               0xFFC00400
#define UART_IER               0xFFC00404
#define UART_DLH               0xFFC00404
#define UART_IIR               0xFFC00408
#define UART_LCR               0xFFC0040C
#define UART_MCR               0xFFC00410
#define UART_LSR               0xFFC00414

#define UART_SCR               0xFFC0041C
#define UART_GCTL              0xFFC00424

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
#define FIO_FLAG_D             0xFFC00700
#define FIO_FLAG_C             0xFFC00704  
#define FIO_FLAG_S             0xFFC00708  
#define FIO_FLAG_T             0xFFC0070C  
#define FIO_MASKA_D            0xFFC00710  
#define FIO_MASKA_C            0xFFC00714  
#define FIO_MASKA_S            0xFFC00718
#define FIO_MASKA_T            0xFFC0071C
#define FIO_MASKB_D            0xFFC00720
#define FIO_MASKB_C            0xFFC00724
#define FIO_MASKB_S            0xFFC00728
#define FIO_MASKB_T            0xFFC0072C
#define FIO_DIR                0xFFC00730
#define FIO_POLAR              0xFFC00734
#define FIO_EDGE               0xFFC00738
#define FIO_BOTH               0xFFC0073C
#define FIO_INEN               0xFFC00740

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
#define TIMER0_CONFIG          0xFFC00600
#define TIMER0_COUNTER         0xFFC00604
#define TIMER0_PERIOD          0xFFC00608
#define TIMER0_WIDTH           0xFFC0060C

#define TIMER1_CONFIG          0xFFC00610  
#define TIMER1_COUNTER         0xFFC00614        
#define TIMER1_PERIOD          0xFFC00618         
#define TIMER1_WIDTH           0xFFC0061C          

#define TIMER2_CONFIG          0xFFC00620  
#define TIMER2_COUNTER         0xFFC00624        
#define TIMER2_PERIOD          0xFFC00628         
#define TIMER2_WIDTH           0xFFC0062C          

#define TIMER_ENABLE           0xFFC00640
#define TIMER_DISABLE          0xFFC00644
#define TIMER_STATUS           0xFFC00648
 
/* Core Timer               Chapter 15 */
#define TCNTL                  0xFFE03000
#define TPERIOD                0xFFE03004
#define TSCALE                 0xFFE03008
#define TCOUNT                 0xFFE0300C

/* Masks for Timer Control */
#define TMPWR                  0x00000001  
#define TMREN                  0x00000002  
#define TAUTORLD               0x00000004  
#define TINT                   0x00000008 

/* Event Bit Positions */
#define EVT_IVTMR_P            0x00000006
#define EVT_IVTMR              MK_BMSK_(EVT_IVTMR_P )  

/* Real Time Clock          Chapter 16 */
#define RTC_STAT               0xFFC00300
#define RTC_ICTL               0xFFC00304
#define RTC_ISTAT              0xFFC00308
#define RTC_SWCNT              0xFFC0030C
#define RTC_ALARM              0xFFC00310
#define RTC_FAST               0xFFC00314
#define RTC_PREN               0xFFC00314

/* RTC_FAST Mask (RTC_PREN Mask) */
#define ENABLE_PRESCALE        0x00000001
#define PREN                   0x00000001

/* Asynchronous Memory Controller EBUI, Chapter 17*/ 
#define EBIU_AMGCTL            0xFFC00A00  
#define EBIU_AMBCTL0           0xFFC00A04
#define EBIU_AMBCTL1           0xFFC00A08

/* SDRAM Controller External Bus Interface Unit */

#define EBIU_SDGCTL            0xFFC00A10
#define EBIU_SDBCTL            0xFFC00A14
#define EBIU_SDRRC             0xFFC00A18
#define EBIU_SDSTAT            0xFFC00A1C


#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_BFIN_H */
