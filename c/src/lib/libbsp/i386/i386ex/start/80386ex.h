/*
 *  Submitted by:
 *
 *    Erik Ivanenko
 *    University of Toronto
 *    erik.ivanenko@utoronto.ca
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *  
 *  $Id$
 */

/* REMAP ADDRESSING Registers */
#define REMAPCFGH	  0x0023
#define REMAPCFGL	  0x0022
#define REMAPCFG	  0x0022
/* INTERRUPT CONTROL REGISTERS -- SLOT 15 ADDRESSES */
#define ICW1M	  0xF020
#define ICW1S	  0xF0A0
#define ICW2M	  0xF021
#define ICW2S	  0xF0A1
#define ICW3M	  0xF021
#define ICW3S	  0xF0A1
#define ICW4M	  0xF021
#define ICW4S	  0xF0A1
#define OCW1M	  0xF021
#define OCW1S	  0xF0A1
#define OCW2M	  0xF020
#define OCW2S	  0xF0A0
#define OCW3M	  0xF020
#define OCW3S	  0xF0A0
/* INTERRUPT CONTROL REGISTERS -- SLOT 0 ADDRESSES */
#define ICW1MDOS	  0x0020
#define ICW1SDOS	  0x00A0
#define ICW2MDOS	  0x0021
#define ICW2SDOS	  0x00A1
#define ICW3MDOS	  0x0021
#define ICW3SDOS	  0x00A1
#define ICW4MDOS	  0x0021
#define ICW4SDOS	  0x00A1
#define OCW1MDOS	  0x0021
#define OCW1SDOS	  0x00A1
#define OCW2MDOS	  0x0020
#define OCW2SDOS	  0x00A0
#define OCW3MDOS	  0x0020
#define OCW3SDOS	  0x00A0


/* CONFIGURATION Registers */
#define DMACFG	  0xF830
#define INTCFG	  0xF832
#define TMRCFG	  0xF834
#define SIOCFG	  0xF836
#define P1CFG	  0xF820
#define P2CFG	  0xF822
#define P3CFG	  0xF824
#define PINCFG	  0xF826

/* WATCHDOG TIMER Registers */
#define WDTRLDH	  0xF4C0
#define WDTRLDL	  0xF4C2
#define WDTCNTH	  0xF4C4
#define WDTCNTL	  0xF4C6
#define WDTCLR	  0xF4C8
#define WDTSTATUS    0xF4CA

/* TIMER CONTROL REGISTERS -- SLOT 15 ADDRESSES */
#define TMR0	  0xF040
#define TMR1	  0xF041
#define TMR2	  0xF042
#define TMRCON	  0xF043
/* TIMER CONTROL REGISTERS -- SLOT 0 ADDRESSES */
#define TMR0DOS	  0x0040
#define TMR1DOS	  0x0041
#define TMR2DOS	  0x0042
#define TMRCONDOS    0x0043

/* INPUT/OUTPUT PORT UNIT Registers */
#define P1PIN	  0xF860
#define P1LTC	  0xF862
#define P1DIR	  0xF864
#define P2PIN	  0xF868
#define P2LTC	  0xF86A
#define P2DIR	  0xF86C
#define P3PIN	  0xF870
#define P3LTC	  0xF872
#define P3DIR	  0xF874

/* ASYNCHRONOUS SERIAL CHANNEL 0 -- SLOT 15 ADDRESSES */
#define RBR0	  0xF4F8
#define THR0	  0xF4F8
#define TBR0	  0xF4F8
#define DLL0	  0xF4F8
#define IER0	  0xF4F9
#define DLH0	  0xF4F9
#define IIR0	  0xF4FA
#define LCR0	  0xF4FB
#define MCR0	  0xF4FC
#define LSR0	  0xF4FD
#define MSR0	  0xF4FE
#define SCR0	  0xF4FF
/* ASYNCHRONOUS SERIAL CHANNEL 0 -- SLOT 0 ADDRESSES */
#define RBR0DOS	  0x03F8
#define THR0DOS	  0x03F8
#define TBR0DOS	  0x03F8
#define DLL0DOS	  0x03F8
#define IER0DOS	  0x03F9
#define DLH0DOS	  0x03F9
#define IIR0DOS	  0x03FA
#define LCR0DOS	  0x03FB
#define MCR0DOS	  0x03FC
#define LSR0DOS	  0x03FD
#define MSR0DOS	  0x03FE
#define SCR0DOS	  0x03FF

/* ASYNCHRONOUS SERIAL CHANNEL 1 -- SLOT 15 ADDRESSES */
#define RBR1	  0xF8F8
#define THR1	  0xF8F8
#define TBR1	  0XF8F8
#define DLL1	  0xF8F8
#define IER1	  0xF8F9
#define DLH1	  0xF8F9
#define IIR1	  0xF8FA
#define LCR1	  0xF8FB
#define MCR1	  0xF8FC
#define LSR1	  0xF8FD
#define MSR1	  0xF8FE
#define SCR1	  0xF8FF
/* ASYNCHRONOUS SERIAL CHANNEL 1 -- SLOT 0 ADDRESSES */
#define RBR1DOS	  0x02F8
#define THR1DOS	  0x02F8
#define TBR1DOS	  0x02F8
#define DLL1DOS	  0x02F8
#define IER1DOS	  0x02F9
#define DLH1DOS	  0x02F9
#define IIR1DOS	  0x02FA
#define LCR1DOS	  0x02FB
#define MCR1DOS	  0x02FC
#define LSR1DOS	  0x02FD
#define MSR1DOS	  0x02FE
#define SCR1DOS	  0x02FF

/* SYNCHRONOUS SERIAL CHANNEL REGISTERS */
#define SSIOTBUF	  0xF480
#define SSIORBUF	  0xF482
#define SSIOBAUD	  0xF484
#define SSIOCON1	  0xF486
#define SSIOCON2	  0xF488
#define SSIOCTR	  0xF48A

/* CHIP SELECT UNIT Registers */
#define CS0ADL	  0xF400
#define CS0ADH	  0xF402
#define CS0MSKL	  0xF404
#define CS0MSKH	  0xF406
#define CS1ADL	  0xF408
#define CS1ADH	  0xF40A
#define CS1MSKL	  0xF40C
#define CS1MSKH	  0xF40E
#define CS2ADL	  0xF410
#define CS2ADH	  0xF412
#define CS2MSKL	  0xF414
#define CS2MSKH	  0xF416
#define CS3ADL	  0xF418
#define CS3ADH	  0xF41A
#define CS3MSKL	  0xF41C
#define CS3MSKH	  0xF41E
#define CS4ADL	  0xF420
#define CS4ADH	  0xF422
#define CS4MSKL	  0xF424
#define CS4MSKH	  0xF426
#define CS5ADL	  0xF428
#define CS5ADH	  0xF42A
#define CS5MSKL	  0xF42C
#define CS5MSKH	  0xF42E
#define CS6ADL	  0xF430
#define CS6ADH	  0xF432
#define CS6MSKL	  0xF434
#define CS6MSKH	  0xF436
#define UCSADL	  0xF438
#define UCSADH	  0xF43A
#define UCSMSKL	  0xF43C
#define UCSMSKH	  0xF43E

/* REFRESH CONTROL UNIT Registers */

#define RFSBAD	  0xF4A0
#define RFSCIR	  0xF4A2
#define RFSCON	  0xF4A4
#define RFSADD	  0xF4A6

/* POWER MANAGEMENT CONTROL Registers */

#define PWRCON	  0xF800
#define CLKPRS	  0xF804

/* DMA UNIT REGISTERS -- SLOT 15 ADDRESSES */
#define DMA0TAR	  0xF000
#define DMA0BYC	  0xF001
#define DMA1TAR	  0xF002
#define DMA1BYC	  0xF003
#define DMACMD1	  0xF008
#define DMASTS	  0xF008
#define DMASRR	  0xF009
#define DMAMSK	  0xF00A
#define DMAMOD1	  0xF00B
#define DMACLRBP	  0xF00C
#define DMACLR	  0xF00D
#define DMACLRMSK    0xF00E
#define DMAGRPMSK    0xF00F
#define DMA0REQL	  0xF010
#define DMA0REQH	  0xF011
#define DMA1REQL	  0xF012
#define DMA1REQH	  0xF013
#define DMABSR	  0xF018
#define DMACHR	  0xF019
#define DMAIS	  0xF019
#define DMACMD2	  0xF01A
#define DMAMOD2	  0xF01B
#define DMAIEN	  0xF01C
#define DMAOVFE	  0xF01D
#define DMACLRTC	  0xF01E
#define DMA1TARPL    0xF083
#define DMA1TARPH    0xF085
#define DMA0TARPH    0xF086
#define DMA0TARPL    0xF087
#define DMA0BYCH	  0xF098
#define DMA1BYCH	  0xF099

/* DMA UNIT REGISTERS -- SLOT 0 ADDRESSES */
#define DMA0TARDOS	  0x0000
#define DMA0BYCDOS	  0x0001
#define DMA1TARDOS	  0x0002
#define DMA1BYCDOS	  0x0003
#define DMACMD1DOS	  0x0008
#define DMASTSDOS	  0x0008
#define DMASRRDOS	  0x0009
#define DMAMSKDOS	  0x000A
#define DMAMOD1DOS	  0x000B
#define DMACLRBPDOS  0x000C
#define DMACLRDOS	  0x000D
#define DMACLRMSKDOS   0x000E
#define DMAGRPMSKDOS   0x000F
#define DMA1TARPLDOS   0x0083
#define DMA0TARPLDOS   0x0087

/* A20GATE AND FAST CPU RESET -- SLOT 15 ADDRESS */
#define PORT92	  0xF092
/* A20GATE AND FAST CPU RESET -- SLOT 0 ADDRESS */
#define PORT92DOS    0x0092

/* end of include file */
