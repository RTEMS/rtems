/*
 *  VEGA PLUS registers declaration
 *
 * Copyright (c) 2000 Canon Research France SA.
 * Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 * The license and distribution terms for this file may be
 * found in found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 */


#ifndef  __LMREGS_H__
#define __LMREGS_H__  

/*
 * VARIABLE DECLARATION   
 ******************************************************************************
 */

/* register area size                                                         */
#define LM_REG_AREA_SIZ (0x4000/4)      

/*** Register mapping : defined by indexes in an array                      ***/
/*** NOTE : only 1 register every 4 byte address location (+ some holes)      */
#ifndef __asm__
extern volatile unsigned long *Regs;        /* Chip registers        */
#endif



/******************************************************************************
 * RADIO CONTROLLER BLOCK                 0x0C00 - 0x0FFF                    *
 ******************************************************************************
 */

#define RC_BASE         0xC00

#define RCCNTL          ((RC_BASE+0x00)/4)
#define RCIOCNTL0       ((RC_BASE+0x04)/4)
#define RCIOCNTL1       ((RC_BASE+0x08)/4)
#define SYNTCNTL0       ((RC_BASE+0x0C)/4)
#define SYNTCNTL1       ((RC_BASE+0x10)/4)
#define SYNTCNTL2       ((RC_BASE+0x14)/4)
#define SYNTFCNTL       ((RC_BASE+0x18)/4)
#define SYNTPCNTL       ((RC_BASE+0x1C)/4)
#define RSSICNTL        ((RC_BASE+0x20)/4)
#define RSSIBASEL       ((RC_BASE+0x24)/4)
#define RSSIBASEH       ((RC_BASE+0x28)/4)
#define CURRSSI         ((RC_BASE+0x2C)/4)
#define RFSCAN          ((RC_BASE+0x30)/4)
#define CURRF           ((RC_BASE+0x34)/4)
#define CURRSSIA        ((RC_BASE+0x38)/4)
#define CURRSSIB        ((RC_BASE+0x3C)/4)
#define CURRSSIAB       ((RC_BASE+0x40)/4)
#define ADCDATAL        ((RC_BASE+0x44)/4)
#define ADCDATAH        ((RC_BASE+0x48)/4)
#define SLICECNTL       ((RC_BASE+0x4C)/4)
#define RCIOCNTL2       ((RC_BASE+0x50)/4)
#define RCIOCNTL3       ((RC_BASE+0x54)/4)
#define ADCREF1L        ((RC_BASE+0x58)/4)
#define ADCREF1H        ((RC_BASE+0x5C)/4)
#define ADCREF2L        ((RC_BASE+0x60)/4)
#define ADCREF2H        ((RC_BASE+0x64)/4)
#define ADCCNTL1        ((RC_BASE+0x68)/4)
#define ADCCNTL2        ((RC_BASE+0x6C)/4)
#define TESTREG         ((RC_BASE+0x70)/4)
#define SYNTLCNTL       ((RC_BASE+0x74)/4)
#define SYNTCNTL3       ((RC_BASE+0x78)/4)
#define ADCPERIOD       ((RC_BASE+0x7C)/4)
#define SYNTIOCNTL      ((RC_BASE+0x80)/4)   /* added 30/08/99 */


/* modified 30/08/99 by LHT */
#define SHAPE0          ((RC_BASE+0x100)/4)  /* previously 0x80 */
#define SHAPE1          ((RC_BASE+0x104)/4)
#define SHAPE2          ((RC_BASE+0x108)/4)
#define SHAPE3          ((RC_BASE+0x10C)/4)
#define SHAPE4          ((RC_BASE+0x110)/4)
#define SHAPE5          ((RC_BASE+0x114)/4)
#define SHAPE6          ((RC_BASE+0x118)/4)
#define SHAPE7          ((RC_BASE+0x11C)/4)
#define SHAPE8          ((RC_BASE+0x120)/4)
#define SHAPE9          ((RC_BASE+0x124)/4)
#define SHAPE10         ((RC_BASE+0x128)/4)
#define SHAPE11         ((RC_BASE+0x12C)/4)
#define SHAPE12         ((RC_BASE+0x130)/4)
#define SHAPERMID       ((RC_BASE+0x134)/4)
#define SHAPERCNTL      ((RC_BASE+0x138)/4)
#define CURSHAPE        ((RC_BASE+0x13C)/4)

/** PLP BLOCK                               0x1400 - 0x17FF                   */

#define PLP_BASE        0x1400
#define DCNTL0          ((PLP_BASE+0x00)/4)
#define DCNTL1          ((PLP_BASE+0x04)/4)
#define SYNC0           ((PLP_BASE+0x08)/4)
#define SYNC1           ((PLP_BASE+0x0C)/4)
#define RXSTARTDL       ((PLP_BASE+0x10)/4)
#define TXSTARTDL       ((PLP_BASE+0x14)/4)
#define RXSTOPDL        ((PLP_BASE+0x1C)/4)
#define RXSYNCT         ((PLP_BASE+0x20)/4)
#define HALF_TXSLOT     ((PLP_BASE+0x24)/4)
#define SCB_NUMBER      ((PLP_BASE+0x28)/4)
#define SCB_OPPNUMBER   ((PLP_BASE+0x2C)/4)
#define TXFRAME         ((PLP_BASE+0x40)/4)
#define MSLTPTR         ((PLP_BASE+0x44)/4)
#define CLOCK_CORR      ((PLP_BASE+0x48)/4)
#define PRESYNC         ((PLP_BASE+0x4C)/4)
#define PLPFINE         ((PLP_BASE+0x50)/4)
#define PLPINDEL        ((PLP_BASE+0x54)/4)
#define TXRXSKW         ((PLP_BASE+0x58)/4)
#define PLPALIN         ((PLP_BASE+0x5C)/4)
#define SUSPRFCNTL      ((PLP_BASE+0x60)/4)
#define SUSPCNTL        ((PLP_BASE+0x64)/4)
#define SUSPFC          ((PLP_BASE+0x68)/4)
#define TSTCNTL         ((PLP_BASE+0x6C)/4)
#define TSTDST          ((PLP_BASE+0x70)/4)
#define TSTTXD          ((PLP_BASE+0x74)/4)
#define TSTRXD          ((PLP_BASE+0x78)/4)
#define PLPID           ((PLP_BASE+0x7C)/4)


/** ENCRYPTION ENGINE                       0x1800 - 0x1BFF                   */

#define EE_BASE          0x1800
#define EECNTL          ((EE_BASE+0x00)/4)
#define EEBASEL         ((EE_BASE+0x08)/4)
#define EEBASEH         ((EE_BASE+0x0C)/4)
#define MFL             ((EE_BASE+0x10)/4)
#define MFM             ((EE_BASE+0x14)/4)
#define MFH             ((EE_BASE+0x18)/4)

/** TELEPHONE ANSWERING DEVICE              0x1C00 - 0x1FFF                   */

#define TAD_BASE        0x1C00
#define TADCNTL         ((TAD_BASE+0x00)/4)
#define TADCODE1        ((TAD_BASE+0x04)/4)
#define TADCODE2        ((TAD_BASE+0x08)/4)
#define TADCODE3        ((TAD_BASE+0x0C)/4)
#define TADSTAT         ((TAD_BASE+0x10)/4)
#define TADADDRL        ((TAD_BASE+0x14)/4)
#define TADADDRM        ((TAD_BASE+0x18)/4)
#define TADADDRH        ((TAD_BASE+0x1C)/4)
#define TADLEN          ((TAD_BASE+0x20)/4)
#define TADAUXDAT1      ((TAD_BASE+0x24)/4)
#define TADAUXDAT2      ((TAD_BASE+0x28)/4)
#define TADSHMEML       ((TAD_BASE+0x2C)/4)
#define TADSHMEMH       ((TAD_BASE+0x30)/4)
#define TADCMD          ((TAD_BASE+0x34)/4)

/** VOICE INTERFACE BLOCK                   0x2000 - 0x23FF                   */

#define PAINT_BASE      0x2000
#define PAINTCNTL       ((PAINT_BASE+0x00)/4)
#define PAINTPLLCNTL    ((PAINT_BASE+0x08)/4)
#define PAINTPLLSTAT    ((PAINT_BASE+0x0C)/4)
#define VBAFECNTL       ((PAINT_BASE+0x10)/4)
#define VBAFEAMP        ((PAINT_BASE+0x14)/4)
#define VBAFEPREAMP     ((PAINT_BASE+0x18)/4)
#define PCMAUX          ((PAINT_BASE+0x1C)/4)
#define PCM0RX          ((PAINT_BASE+0x20)/4)
#define PCM0TX          ((PAINT_BASE+0x24)/4)
#define PCM1RX          ((PAINT_BASE+0x28)/4)
#define PCM1TX          ((PAINT_BASE+0x2C)/4)
#define ADPCM0RX        ((PAINT_BASE+0x30)/4)
#define ADPCM0TX        ((PAINT_BASE+0x34)/4)
#define ADPCM1RX        ((PAINT_BASE+0x38)/4)
#define ADPCM1TX        ((PAINT_BASE+0x3C)/4)
#define MPDCNTL         ((PAINT_BASE+0x40)/4)
#define MPDREADY        ((PAINT_BASE+0x44)/4)
#define MPDABS          ((PAINT_BASE+0x48)/4)
#define MPDS1           ((PAINT_BASE+0x4C)/4)
#define MPDS2           ((PAINT_BASE+0x50)/4)
#define HPPCMCNTL       ((PAINT_BASE+0x60)/4)
#define HPOUT           ((PAINT_BASE+0x64)/4)
#define HPIN            ((PAINT_BASE+0x68)/4)
#define PAINTBASE0      ((PAINT_BASE+0x70)/4)
#define PAINTBASE1      ((PAINT_BASE+0x74)/4)
#define G726AI0         ((PAINT_BASE+0x80)/4)
#define G726AI1         ((PAINT_BASE+0x84)/4)
#define G726GAIN0       ((PAINT_BASE+0x88)/4)
#define G726GAIN1       ((PAINT_BASE+0x8C)/4)
#define G726VOL         ((PAINT_BASE+0x90)/4)
#define G726GST         ((PAINT_BASE+0x94)/4)
#define G726CNTL0       ((PAINT_BASE+0x98)/4)
#define G726CNTL1       ((PAINT_BASE+0x9C)/4)
#define G726CHANNEL     ((PAINT_BASE+0xA0)/4)
#define G726CHANENB     ((PAINT_BASE+0xA4)/4)

/** GENERAL REGISTERS BLOCK                 0x2800 - 0x2CFF                   */

#define MISC_BASE       0x2800
#define CHIPID          ((MISC_BASE+0x00)/4)
#define DEVICEID        ((MISC_BASE+0x04)/4)
#define IOACNTL         ((MISC_BASE+0x10)/4)
#define IOADATA         ((MISC_BASE+0x18)/4)
#define IOBCNTL         ((MISC_BASE+0x20)/4)
#define IOBDATA         ((MISC_BASE+0x28)/4)
#define IOCCNTL1        ((MISC_BASE+0x30)/4)
#define IOCCNTL2        ((MISC_BASE+0x34)/4)
#define IOCDATA         ((MISC_BASE+0x38)/4)
#define IODCNTL1        ((MISC_BASE+0x40)/4)
#define IODCNTL2        ((MISC_BASE+0x44)/4)
#define IODDATA         ((MISC_BASE+0x48)/4)
#define IOECNTL1        ((MISC_BASE+0x50)/4)
#define IOECNTL2        ((MISC_BASE+0x54)/4)
#define IOEDATA         ((MISC_BASE+0x58)/4)
#define IOFCNTL         ((MISC_BASE+0x60)/4)
#define IOFDATA         ((MISC_BASE+0x68)/4)
#define IOGCNTL         ((MISC_BASE+0x70)/4)
#define IOGDATA         ((MISC_BASE+0x78)/4)
#define IOHCNTL         ((MISC_BASE+0x80)/4)
#define IOHDATA         ((MISC_BASE+0x88)/4)
#define RINGCNTL        ((MISC_BASE+0x90)/4)
#define RINGFREQ        ((MISC_BASE+0x94)/4)
#define RSCNTL          ((MISC_BASE+0xA0)/4)
/*#ifndef PRODUCT_VERSION*/	
#define RSRXD           ((MISC_BASE+0xA4)/4)
#define RSTXD           ((MISC_BASE+0xA8)/4)
/*#endif*/
#define PWMCNTL         ((MISC_BASE+0xB0)/4)
#define PWMTIMER0       ((MISC_BASE+0xB4)/4)
#define PWMTIMER1       ((MISC_BASE+0xB8)/4)
#define LCDEECNTL1      ((MISC_BASE+0xC0)/4)
#define LCDEECNTL2      ((MISC_BASE+0xC4)/4)
#define LCDEEDAIN       ((MISC_BASE+0xC8)/4)
#define LCDEEDAOUT      ((MISC_BASE+0xCC)/4)
#define KEYROW          ((MISC_BASE+0xE0)/4)
#define KEYCOL          ((MISC_BASE+0xE4)/4)
#define KEYDEBOUNCE     ((MISC_BASE+0xE8)/4)
#define DIAGCNTL1       ((MISC_BASE+0xEC)/4)
#define DIAGCNTL2       ((MISC_BASE+0xF0)/4)
#define CLKCNTL         ((MISC_BASE+0xF4)/4)
#define OSCCOR          ((MISC_BASE+0xF8)/4)

/* PRODUCT_VERSION */	
/* Added 30/08/99 : New Control register for UART control */
#define UART_BASE       0x3000
#define RSRBR           ((UART_BASE+0x00)/4)
#define RSTHR           ((UART_BASE+0x00)/4)
#define RSIER           ((UART_BASE+0x04)/4)
#define RSIIR           ((UART_BASE+0x08)/4)
#define RSFCR           ((UART_BASE+0x08)/4)
#define RSLCR           ((UART_BASE+0x0C)/4)
#define RSLSR           ((UART_BASE+0x14)/4)
#define RSDLL           ((UART_BASE+0x00)/4)
#define RSDLH           ((UART_BASE+0x04)/4)
#define RSCNT           ((UART_BASE+0x20)/4)
/*PRODUCT_VERSION*/ 


/** THUMB and INTERFACES BLOCK              0x3400 - 0x4FFF                   */

#define TIM_BASE        0x3400
#define WDCNTL          ((TIM_BASE+0x00)/4)
#define TIMERLOAD0      ((TIM_BASE+0x80)/4)
#define TIMER0          ((TIM_BASE+0x8C)/4)
#define TIMERCNTL0      ((TIM_BASE+0x98)/4)
#define TIMERLOAD1      ((TIM_BASE+0xA0)/4)
#define TIMER1          ((TIM_BASE+0xAC)/4)
#define TIMERCNTL1      ((TIM_BASE+0xB8)/4)

#define INTC_BASE       0x3800
#define INTMASK         ((INTC_BASE+0x20)/4)
#define INTSTAT         ((INTC_BASE+0x24)/4)
#define INTACK          ((INTC_BASE+0x24)/4)
#define INTACK2         ((INTC_BASE+0x24))
#define INTIS           ((INTC_BASE+0x28)/4)
#define INTIS2          ((INTC_BASE+0x28))
#define INTHPAI         ((INTC_BASE+0x00)/4)
#define INTHPAI2        ((INTC_BASE+0x00))
#define INTLEVEL        ((INTC_BASE+0x04)/4)
#define INTEOI          ((INTC_BASE+0x08)/4)
#define INTEOI2         ((INTC_BASE+0x08))
#define INTMASKALL      ((INTC_BASE+0x0C)/4)
#define INTTAB          ((INTC_BASE+0x10)/4)
#define INTCNTL0        ((INTC_BASE+0x80)/4)
#define INTCNTL1        ((INTC_BASE+0x84)/4)
#define INTCNTL2        ((INTC_BASE+0x88)/4)
#define INTCNTL3        ((INTC_BASE+0x8C)/4)
#define INTCNTL4        ((INTC_BASE+0x90)/4)
#define INTCNTL5        ((INTC_BASE+0x94)/4)
#define INTCNTL6        ((INTC_BASE+0x98)/4)
#define INTCNTL7        ((INTC_BASE+0x9C)/4)
#define INTCNTL8        ((INTC_BASE+0xA0)/4)
#define INTCNTL9        ((INTC_BASE+0xA4)/4)
#define INTCNTL10       ((INTC_BASE+0xA8)/4)
#define INTCNTL11       ((INTC_BASE+0xAC)/4)
#define INTCNTL12       ((INTC_BASE+0xB0)/4)
#define INTCNTL13       ((INTC_BASE+0xB4)/4)
#define INTCNTL14       ((INTC_BASE+0xB8)/4)
#define INTCNTL15       ((INTC_BASE+0xBC)/4)
#define INTGCNTL        ((INTC_BASE+0x7C)/4)

/* these "define" are used for the asm code of int managment */
#define INTPHAI3  0xF3800
#define INTSTAT3  0xF3824
#define INTIS3    0xF3828
#define INTACK3   0xF3824
#define INTEOI3   0xF3808

#define TI_BASE         0x3C00
#define CSCNTL0_0       ((TI_BASE+0x00)/4)
#define CSCNTL0_1       ((TI_BASE+0x04)/4)
#define CSCNTL0_2       ((TI_BASE+0x08)/4)
#define CSCNTL0_3       ((TI_BASE+0x0C)/4)
#define CSCNTL0_4       ((TI_BASE+0x10)/4)
#define CSCNTL0_5       ((TI_BASE+0x14)/4)
#define CSCNTL0_6       ((TI_BASE+0x18)/4)
#define CSCNTL1_0       ((TI_BASE+0x20)/4)
#define CSCNTL1_1       ((TI_BASE+0x24)/4)
#define CSCNTL1_2       ((TI_BASE+0x28)/4)
#define CSCNTL1_3       ((TI_BASE+0x2C)/4)
#define CSCNTL1_4       ((TI_BASE+0x30)/4)
#define CSCNTL1_5       ((TI_BASE+0x34)/4)
#define CSCNTL1_6       ((TI_BASE+0x38)/4)
#define CSGCNTL         ((TI_BASE+0x40)/4)
#define MUXADCNTL       ((TI_BASE+0x48)/4)
#define PORTCNTL        ((TI_BASE+0x60)/4)
#define DCC             ((TI_BASE+0x78)/4)
#define BRK0            ((TI_BASE+0x100)/4)
#define BRK1            ((TI_BASE+0x104)/4)
#define BRK2            ((TI_BASE+0x108)/4)
#define BRK3            ((TI_BASE+0x10C)/4)
#define BRK4            ((TI_BASE+0x110)/4)
#define BRK5            ((TI_BASE+0x114)/4)
#define BRK6            ((TI_BASE+0x118)/4)
#define BRK7            ((TI_BASE+0x11C)/4)
#define BRKMSK          ((TI_BASE+0x140)/4)
#define BRKSTAT         ((TI_BASE+0x144)/4)
#define SLEEPTIMER      ((TI_BASE+0x204)/4)
#define SLEEPCNTL       ((TI_BASE+0x208)/4)



/******************************************************************************
 * BIT MASKS for Chip registers
 ******************************************************************************
 */

/** TELEPHONE ANSWERING DEVICE BLOCK (TAD)                                    */

/* TADCNTL register                                                           */
#define IRQCNTL         0x01
#define CE_CNTL         0x02
#define MSKTAD          0x04
#define TAD_PAD_ENB     0x40
#define TADENB          0x80

/* TADSTAT register                                                           */
#define RBN             0x01
#define TRANSFER        0x02
#define ACTIVE          0x04

/* TADCMD register                                                            */
#define MSK_TADCMD      0x0F                /* Mask on TADCMD                 */
#define CONTINUE        0x10



/** RADIO CONTROLER BLOCK (RC3)                                               */

/* SLICECNTL register                                                         */
#define MSK_SLICEDL     0x07                /* Mask on SLICEDL field          */
#define MSK_SCNTL       0x18                /* Mask on SCNTL field            */
#define SELOCK          0x20
#define MSK_MUXSLICE    0xC0

/* RCIOCNTL0 register                                                         */
#define D2SBYPASS       0x01
#define DRBYPASS        0x02
#define RXINV           0x04
#define SELANT          0x10
#define ANT             0x20
#define LDINV0          0x40
#define LDINV1          0x80

/* RCIOCNTL1 register                                                         */
#define MSK_GPO         0x0F                /* Mask on GPO field              */
#define P00ENB          0x10
#define ABORT           0x20
#define TXIO            0x40
#define TXINV           0x80

/* RCIOCNTL2 register                                                         */
#define SELRFCLK        0x01
#define SELRXPWR        0x02
#define TXPWRINV        0x04
#define RXPWRINV        0x08
#define SYNTHPWR0INV    0x10
#define SYNTHPWR1INV    0x20
#define TXDRONINV       0x40
#define SELTXDRON       0x80

/* RCIOCNTL3 register                                                         */
#define D2SMODE         0x01
#define DRCNTL          0x02
#define MSK_SELGPO      0x0C                /* Mask on SELGPO field           */
#define MSK_SUSPANT     0x30                /* Mask on SUSO/ANT field         */

/* SHAPERCNTL register                                                        */
#define INV             0x01
#define MID             0x02
#define PWRDWN          0x04
#define DACENB          0x80

/* SYNTFCNTL register                                                         */
#define SYNT_FREQ       0x80

/* SYNTCNTL0 register                                                         */
#define PWRFREQ         0x01
#define CLKPHASE        0x08
#define LETYPE          0x10
#define RPIPEON         0x20
#define PWRSGN          0x40
#define OUTLOCK         0x80

/* SYNTCNTL1 register                                                         */
#define SLE0            0x01
#define SLE1            0x02
#define SLE2            0x04
#define LEMODE          0x08
#define LESEL           0x10
#define SCLK            0x20
#define SDATA           0x40
#define HPMODE_SYNT     0x80

/* SYNTCNTL2 register                                                         */
#define START_DONE      0x80
#define MSK_N           0x03                /* Mask on N field                */

/* SYNTCNTL3 register                                                         */
#define RXPWRDNSEN      0x01
#define RXPWRUNSEN      0x02
#define TXPWRDNSEN      0x04
#define TXPWRUNSEN      0x08
#define SEQCNTL         0x10
#define ALIGN           0x20
#define PLLWORD         0x40
#define PRESEQ          0x80

/* RFSCAN register                                                            */
#define MSK_RF          0x0F                /* Mask on RF field               */
#define RFMAX           0x80

/* RSSIRANGE register                                                         */
#define MSK_VREFP       0x03                /* Mask on VREFP field            */
#define MSK_VREFN       0x0C                /* Mask on VREFN field            */

/* RSSICNTL register                                                          */
#define MSK_MODE_RSSI   0x03                /* Mask on MODE field             */
#define MARKFF          0x04
#define TXMARK          0x08
#define ALL_RSSI        0x10
#define RSSIACT         0x20
#define RSSIDIS         0x40
#define RSSIENB         0x80

/* RCCNTL register                                                            */
#define RCCNTL_ENABLE   0x80


/* ADCCNTL1 register                                                          */
#define ADCSTART        0x80
#define SCAN            0x40
#define ADCENB          0x80
#define MSK_ADCSEL      0x07

/* ADCCNTL2 register                                                          */
#define ADCOVER1        0x01
#define ADCOVER2        0x02
#define ADCDWN1         0x04
#define ADCDWN2         0x08



/** PLP BLOCK                                                                 */

/* DCNTL0 register                                                            */
#define TMUXINT         0x01
#define MUTEIP          0x02
#define Q1Q2_PLP        0x04
#define TRANSP          0x08
#define CRYPTALL        0x10
#define TXSENSE         0x20
#define ZACT            0x40
#define PLPENB          0x80

/* DCNTL1 register                                                            */
#define ONECT           0x10
#define WOMODE          0x40
#define WOENB           0x80
#define MSK_RPIPE       0x07                /* mask on RPIPE field            */

/* RXSYNCT register                                                           */
#define PRSIZE          0x04
#define PREEN           0x08
#define PRETYPE         0x40
#define PROLONG         0x80

#define MSK_SYCNT       0x30                /* mask on SYCNT field            */
#define MSK_PTHR        0x03                /* mask on PTHR field             */

/* CLOCK_CORR register                                                        */
#define SIGN            0x80

/* PRESYNC register                                                           */
#define PRESENB         0x80

#define MSK_PRES        0x0F                /* mask on PRES field             */


/* PLPALIN register                                                           */
#define SYNM            0x08
#define BITSLIP         0x10
#define SLOTFAIL        0x20
#define DFFAIL          0x40
#define LONGDF          0x80
#define MODE_PLPALIN    0x03                /* mask on PLP alignment mode     */

/* SUSPCNTL register                                                          */
#define SUSPENB         0x01

/* TSTCNTL register                                                           */
#define DISBSCR         0x01
#define TX_TST          0x02
#define DATADIR         0x04

/* TSTDST register                                                            */
#define RDY             0x80

/** ENCRYPTION ENGINE                                                         */

/* EECNTL register                                                            */
/* Bit ENABLE already defined                                                 */
#define EECNTL_ENABLE   0x80


/** PAINT+ BLOCK                                                              */

/* PAINTCNTL register                                                         */
#define MUTEDIS0        0x0001
#define MUTEDIS1        0x0002
#define MEMLOOP0        0x0004
#define MEMLOOP1        0x0008
#define RATE0           0x0010
#define RATE1           0x0020
#define CHAN0ENB        0x0040
#define CHAN1ENB        0x0080
#define BG0ENABLE       0x0100
#define BG1ENABLE       0x0200
#define PADENABLE       0x2000
#define FORCE13         0x4000
#define PAINTENB        0x8000


/* PAINTPLLCNTL register                                                      */
#define MSK_MC          0x001F              /* Mask on MC field               */
#define MCSIGN          0x0020
#define MANUAL          0x0080
#define RANG0           0x0100
#define RANG1           0x0200
#define RANG2           0x0400
#define MSK_RANG        0x0700              /* Mask on RANG field             */
#define FREEZD          0x1000
#define FREEZP          0x2000
#define PPFP            0x8000

/* PAINTPLLSTAT register                                                      */
#define MSK_DPHI        0x01FF              /* Mask on DPHI field             */
#define LOCKD           0x1000
#define LOCKP           0x2000
#define NOSIG           0x8000

/* HPPCMCNTL register                                                         */
#define LEN0            0x0001
#define LEN1            0x0002
#define LEN2            0x0004
#define MSK_LEN         0x0007              /* Mask on LEN field              */
#define FREQ0           0x0010
#define FREQ1           0x0020
#define MSK_PCMFREQ     0x0030              /* Mask on FREQ field             */
#define FSTYP0          0x0100
#define FSTYP1          0x0200
#define MSK_FSTYP       0x0300              /* Mask on FSTYP field            */
#define IOD0            0x0400
#define IOD1            0x0800
#define MSK_IOD         0x0C00              /* Mask on FSTYP field            */
#define IOCK            0x1000
#define MASTER          0x4000
#define PCMENB          0x8000

/* VBAFECNTL register                                                         */
#define MSK_VOLMIC      0x0007              /* Mask on VOLMIV field           */
#define MICDIF          0x0010
#define ENBMICREF       0x0080
#define MODE0           0x0100
#define MODE1           0x0200
#define MODE2           0x0400
#define LOOP0           0x1000
#define LOOP1           0x2000
#define FLOAT           0x4000
#define VBAFENB         0x8000

/* VBAFEAMP register                                                          */
#define MSK_VOL1OUT     0x000F              /* Mask on VOL1OUT field          */
#define ENBCH1          0x0010
#define MSK_VOL2OUT     0x0F00              /* Mask on VOL2OUT field          */
#define ENBCH2          0x1000

/* VBAFEPREAMP register                                                       */
#define MSK_VOLIN       0x000F              /* Mask on VOLIN field            */
#define MSK_ATT         0x0070              /* Mask on ATT field              */
#define PRCNF0          0x0100
#define PRCNF1          0x0200
#define PRCNF2          0x0400

/* MPDCNTL register                                                           */
#define MPD_FREQ        0x0001
#define MPD_ENB         0x0080

/* MPDREADY register                                                          */
#define MPD_RDY         0x0001

/* G726CNTL0 register                                                         */
#define RXTONE0         0x0001
#define RXTONE1         0x0002
#define TXTONE0         0x0004
#define TXTONE1         0x0008
#define SCALE0          0x0010
#define SCALE1          0x0020
#define MSK_SCALE       0x0030              /* Mask on SCALE field            */

/* G726CNTL1 register                                                         */
#define LAW             0x0001
#define UPCM            0x0002
#define G726_TXMUTE     0x0004
#define G726_RXMUTE     0x0008
#define SIDETONE        0x0010
#define SCA             0x0020
#define G726ENB         0x0080

/* G726CHANNEL register                                                       */
#define CHAN            0x0002

/* G726CHANENB register                                                       */
#define G726ENB0        0x0001
#define G726ENB1        0x0003


/** GENERAL REGISTERS BLOCK                                                   */

/* RINGCNTL register                                                          */
/* Bit ENABLE already defined                                                 */
#define RINGCNTL_ENABLE   0x80
#define FULL_BRIDGE     0x40
#define MSK_DELAY       0x30
#define RING_PADENB     0x08
#define MSK_LEVEL       0x07                /* mask on LEVEL field            */

/* RSIER register   (UART Interrupt enable register definition)               */
#define		LINE_STATUS_ENABLE	0x04
#define		TX_INT_ENABLE			0x02
#define		RX_INT_ENABLE			0x01

/* RSIIR register   (UART Interrupt identification register definition)       */
#define		FIFO_ENABLE_MASK		0xC0
#define		INT_ID_MASK				0x0E
#define		PENDING_INT_FLAG		0x01
#define		LINE_STATUS_INT		0x06 /* values for interrupt identification  */
#define		RX_INT					0x04
#define		FIFO_TIMEOUT_INT		0x0C
#define		TX_EMPTY_INT			0x02

/* RSFCR register   (UART Tx/Rx FIFO control register definition)             */
#define		RX_LEVEL_MASK			0xC0
#define		CLEAR_TX_FIFO			0x04
#define		CLEAR_RX_FIFO			0x02
#define		FIFO_ENABLE				0x01
#define		_1_BYTE_RECEIVED		0x00 /* RX level values (Interrupt trigger ) */
#define		_4_BYTE_RECEIVED		0x40
#define		_8_BYTE_RECEIVED		0x80
#define		_14_BYTE_RECEIVED		0xC0

/* RSLCR register   (UART line control register definition)                   */
#define		DIV_ENABLE				0x80
#define		TX_BREAK_ENABLE		0x40
#define		PARITY_ENABLE			0x08
#define		PARITY_MASK				0x30
#define		_1_STOP_BIT				0x00
#define		_2_STOP_BIT				0x04
#define		WORD_LENGTH_MASK		0x03
#define		ODD_PARITY				0x00	/* possible value for the parity */
#define		EVEN_PARITY				0x10
#define		PARITY_EQUAL1			0x20
#define		PARITY_EQUAL0			0x30
#define		_5_BITS_CHAR			0x00	/* possible value for the word length  */
#define		_6_BITS_CHAR			0x01
#define		_7_BITS_CHAR			0x02
#define		_8_BITS_CHAR			0x03

/* RSLSR Register   (UART line status register definition)                    */
#define		RX_FIFO_ERROR			0x80
#define		TXEMPTY					0x40
#define		HOLD_EMPTY				0x20
#define		BREAK						0x10
#define		FRAME_ERROR				0x08
#define		PARITY_ERROR			0x04
#define		OVERRUN_ERROR			0x02
#define		RX_READY					0x01

/* RSDLL Register   (UART clock divider low register definition)              */
/* note RSDLH is always 0x00 */
#define	RS_4800						0x18
#define	RS_9600						0x0C
#define	RS_19200					0x06
#define	RS_38400						0x03
#define	RS_57600						0x02
#define	RS_115200					0x01

/* RSCNT Register   (UART control register definition)                        */
#define		UART_PAD_ENABLE		0x02

/* PWMCNTL register                                                           */
#define PWMENB          0x80
#define MSK_PWMFREQ     0x03                /* mask on PWMFREQ field          */
#define PWM1_PADENB     0x40
#define PWM0_PADENB     0x20
#define MIRROR          0x10


/* LCDEECNTL1 register                                                        */
/* Bit ENABLE already defined                                                 */
#define LCDEE_ENABLE   0x80
#define DA1_DA0         0x40
#define MSK_LCDEEFREQ   0x03                /* mask on LCDEEFREQ field        */
#define LCDEE_PADENB    0x40

/* LCDEECNTL2 register                                                        */
#define SENDACK         0x01
#define RXACK           0x02
#define STOP            0x08
#define START           0x10
#define RX_LCDEE        0x20
#define TX_LCDEE        0x40

/* DIAGCNTL1 register                                                         */
#define DIAGL_PADENB    0x01
#define DIAGH_PADENB    0x02

/* DIAGCNTL2 register                                                         */
#define DLSEL           0x0F                /* mask on DLSEL field            */
#define DHSEL           0xF0                /* mask on DHSEL field            */

/* KEYROW register                                                            */
#define MSK_ROW         0x1F                /* mask on ROW field              */
#define KEYRELEASE      0x80

/* CLKCNTL register                                                           */
#define MOSCDISABLE     0x02
#define OVERSAM         0x04
#define SQUARER         0x08
#define SWRESET         0x80
#define SWFLAG          0x80
#define TSTN_DISABLE    0x40
#define MODE55          0x10

/** THUMB and INTERFACES BLOCK                                                */

/* CSCNTL0[6:0] registers                                                     */
#define MSK_SETUP       0x0007
#define SETUP_RD        0x0040
#define SETUP_HZ        0x0080
#define MSK_WIDTH       0x1F00
#define MSK_HZWS        0xE000

/* CSCNTL1[6:0] registers                                                     */
#define MSK_CSMODE      0x0003
#define MSK_HOLD        0x0070
#define HOLD_RD         0x0080
#define MSK_WIDTH_WR    0x0700
#define USE_WIDTH_WR    0x4000
#define WR_SHIFED       0x8000

#define CSMODE_8        0x0000
#define CSMODE_16_WHWL  0x0002
#define CSMODE_16_BHBL  0x0003


/* MUXADCNTL register                                                         */
#define MSK_AHOLD       0x0007
#define MSK_ALEWIDTH    0x0070

/* PORTCNTL register                                                          */
#define CS0_             0x0001
#define CS1_             0x0002
#define CS2_             0x0004
#define CS3_             0x0008
#define CS4_             0x0010
#define CS5_             0x0020
#define CS6_             0x0040
#define DATA_H          0x0080
#define INT0            0x0100
#define INT1            0x0200
#define INT2            0x0400
#define INT3            0x0800
#define MSK_ADDRESS     0x7000
#define EXTMEM          0x8000

#define ADDRESS_128K    0x0000
#define ADDRESS_256K    0x1000
#define ADDRESS_512K    0x2000
#define ADDRESS_1M      0x3000
#define ADDRESS_2M      0x4000


/* CSGCNTL register                                                           */
#define CSSWITCH        0x0040


/* SLEEPCNTL register                                                         */
#define EXPIRED         0x01
#define SLEEP_ENABLE    0x80

/* WDCNTL register                                                            */
#define WDSTROKE        0x80
#define WDFLAG          0x80                /* same bit                       */


/* DCC register                                                               */
/* bit ENABLE=0x80 already defined */ 
#define DCC_ENABLE   0x80


/* TIMERCNTL[0:1] register                                                    */
/* bit ENABLE=0x80 already defined */ 
#define TIMER_ENABLE   0x80
#define RELOAD          0x0040
#define MSK_FREQ        0x0003              /* mask on FREQ field             */
#define TIMER_13824kHz  0x0003
#define TIMER_864kHz    0x0002
#define TIMER_216kHz    0x0001
#define TIMER_27kHz     0x0000


/* INTMASKALL register                                                        */
#define MASKIRQ         0x80
#define MASKFIQ         0x40

/* INTEOI register                                                            */
#define EOI             0x80

/* INTMASK register                                                           */
/* INTSTAT register                                                           */
/* INTIS register                                                             */

#define PLP             0x0001
#define PCM             0x0002
#define SRX             0x0004
#define STX             0x0008
#define TMR0            0x0010
#define TMR1            0x0020
#define LCDEE           0x0100
#define KPAD            0x0200
#define TAD             0x0400
#define ADC             0x0800
#define EXT0            0x1000
#define EXT1            0x2000
#define EXT2            0x4000
#define EXT3            0x8000

/* INTCNTL[0:15] register                                                     */
#define MSK_PRIO        0x0007
#define RE              0x0008
#define RISING          0x0040
#define EDGE            0x0080

/* INTHPAI register                                                           */
#define AUTOACK         0x0080


/******************************************************************************
 * Memory Mapping definition 
 ******************************************************************************
 */

#define CSN0_BASE_ADR   0x00200000          /* Base Address of CSN0           */
#define CSN1_BASE_ADR   0x00400000          /* Base Address of CSN1           */
#define CSN2_BASE_ADR   0x00600000          /* Base Address of CSN2           */
#define CSN3_BASE_ADR   0x00800000          /* Base Address of CSN3           */
#define CSN4_BASE_ADR   0x00A00000          /* Base Address of CSN4           */
#define CSN5_BASE_ADR   0x00C00000          /* Base Address of CSN5           */
#define CSN6_BASE_ADR   0x00E00000          /* Base Address of CSN6           */

#define IRAM_BASE_ADR   0x00000000          /* Base Addr. of int. Data Memory */
#define SHRAM_BASE_ADR  0x00080000          /* Base Address of Share Memory   */
#define REGS_BASE_ADR   0x000F0000          /* Base Address of registers      */
#define RADRAM_BASE_ADR 0x000F0000          /* Base Address of registers      */


/******************************************************************************
 * Slot Control bloc 
 ******************************************************************************
 */

#ifndef __asm__
/*** Slot Control Block structure                                           ***/
typedef volatile struct                     /* normal Slot Control Block      */
{
    unsigned char RAD0;
    unsigned char RAD1;
    unsigned char MODE;
    unsigned char CNTL0;
    unsigned char CNTL1;
    unsigned char CNTL2;
    unsigned char STAT0;                       
    unsigned char STAT1;
    unsigned char STAT2;
    unsigned char CRYPT;
    unsigned char MUTE;
    unsigned char INT_;
    unsigned char AMSG;
    unsigned char AHDR;
    unsigned short APTR;
    unsigned short IPTR;
    unsigned short CfPTR;
    unsigned short OtPTR;
    unsigned char OFFCNTL;
    unsigned char WINCNTL;
} LM_SCB;

typedef LM_SCB *LM_SCB_P;                   /* pointer to Slot Control Block  */
#endif

/*** BIT MASKS for Slot Control Block parameters                            ***/

/* RAD0 parameter                                                             */
#define RC_RSSIENB  0x80                    /* RSSI measurement control       */
#define RC_ANTENNA2 0x08                    /* antenna[2] selection           */
#define RC_ANTENNA1 0x04                    /* antenna[1] selection           */
#define RC_ANTENNA0 0x02                    /* antenna[0] selection           */
#define RC_SYNOUT   0x01                    /* synthesiser selection          */

#define RC_ANTSEL   0x0E                    /* mask on RC antenna selection   */

/* RAD1 parameter                                                             */
#define RC_RFC      0xF0                    /* mask on RC RF carrier number   */
#define RC_RFSCAN   0x08                    /* RF carrier source selection    */
#define RC_SYNLATCH 0x04                    /* synthesizer #n Latch Enabled   */
#define RC_SYN_TX   0x03                    /* slot is TX (synthesizer data)  */
#define RC_SYN_RX   0x02                    /* slot is RX (synthesizer data)  */

#define RC_SYNSLOT  0x03                    /* mask on Synthesizer slot type  */

/* MODE parameter                                                             */
#define AUTOB1      0x80
#define AUTOB0      0x40
#define P00         0x20

#define MSK_MODE    0x1F                    /* mask on SCB MODE field         */

/* CNTL0 parameter                                                            */
#define LU7CH       0x80

#define MSK_BOFF    0x7F                    /* mask on BOFF field             */

/* CNTL1 parameter                                                            */
#define TX          0x80
#define RESYNC      0x40
#define Q1          0x40                    /* Q1/RESYNC mapped on same bit   */
#define INHBST      0x20
#define Q2          0x20                    /* Q2/INHBST mapped on same bit   */
#define CTPACK      0x10

#define MSK_CTFLEN  0x0F                    /* mask on CTFLEN field           */

/* CNTL2 parameter                                                            */
#define SLTEN       0x80
#define SINV        0x40
#define ALL         0x20
#define CO_CL       0x20                   /* CO_CL/ALL mapped on same bit    */
#define INTEN       0x10
#define SCOR1       0x08
#define SCOR0       0x04
#define BINTE       0x02
#define BINTU       0x01

#define MSK_SCOR    0x0C                    /* mask on SCOR field             */

/* STAT0 parameter                                                            */
#define ZFAIL0      0x80
#define ZFAIL1      0x40
#define ZFAIL2      0x20
#define ZFAIL3      0x10
#define SCRD1       0x08
#define SCRD0       0x04
#define PRED1       0x02
#define PRED0       0x01

#define MSK_ZFAIL   0xF0                    /* mask on ZFAIL field            */
#define MSK_SCRD    0x0C                    /* mask on SCRD field             */
#define MSK_PRED    0x03                    /* mask on PRED field             */

/* STAT1 parameter                                                            */
#define BCRC7       0x80
#define BCRC6       0x40
#define BCRC5       0x20
#define BCRC4       0x10
#define BCRC3       0x08
#define BCRC2       0x04
#define BCRC1       0x02
#define BCRC0       0x01

/* STAT2 parameter                                                            */
#define TMUX        0x80
#define RADIO       0x40
#define RFPI        0x20
#define XCRC        0x10
#define ACRC        0x08
#define SYNC        0x04
#define BCRC        0x02
#define BCRC8       0x01

/* CRYPT parameter                                                            */
#define LONG        0x80
#define INIP        0x40
#define ACRYPT      0x20
#define BCRYPT      0x10

#define MSK_EETBL   0x0F                    /* mask on EETBL field            */

/* MUTE  parameter                                                            */
#define NOTI        0x80
#define XFAIL       0x40
#define AFAIL       0x20
#define NOSYNC      0x10                    /*NOSYNC/TXMUTE mapped on same bit*/
#define TXMUTE      0x10

#define MSK_CHAN    0x0F                    /* mask on CHAN field             */

/* INT   parameter                                                            */
/* Bit RADIO is already defined                                               */
#define Q1Q2        0x80
#define RFP_I       0x20
#define X_CRC       0x10
#define R_CRC       0x08
#define SYNCFAIL    0x04
#define ASYNCOK     0x02
#define ZFIELD      0x01

/* AMSG  parameter                                                            */
#define PP_FP       0x80   
#define CT          0x40
#define NT          0x20                    /* NT/CTSEND mapped on same bit   */
#define CTSEND      0x20
#define MTFIRST     0x10                    /* MTFIRST/QT mapped on same bit  */
#define QT          0x10
#define MT          0x08
#define MTWAIT      0x04
#define PT          0x02
#define ESCAPE      0x01

/* WINCNTL  parameter                                                         */
#define LM_WIN_NONE 0x00                    /* no sync window                 */
#define LM_WIN_OPEN 0x3F                    /* wide open window size          */
#define MSK_WINSZ   0x3F





/* 
 * Some macros to mask the VEGA+ interrupt sources                            
 ******************************************************************************
 */

#define LM_MaskPLP()           (LM_Regs[INTMASK] |= PLP)
#define LM_MaskPCM()           (LM_Regs[INTMASK] |= PCM)

/* Vega+ product version */
#define LM_MaskUART()          (LM_Regs[INTMASK] |= SRX)
#define LM_MaskSRX()           (LM_Regs[RSIER]   &= ~RX_INT_ENABLE)
#define LM_MaskSTX()           (LM_Regs[RSIER]   &= ~TX_INT_ENABLE)
#define LM_MaskUARTStatus()    (LM_Regs[RSIER]   &= ~LINE_STATUS_ENABLE)


#define LM_MaskTMR0()          (LM_Regs[INTMASK] |= TMR0)
#define LM_MaskTMR1()          (LM_Regs[INTMASK] |= TMR1)
#define LM_MaskLCDEE()         (LM_Regs[INTMASK] |= LCDEE)
#define LM_MaskKPAD()          (LM_Regs[INTMASK] |= KPAD)
#define LM_MaskTAD()           (LM_Regs[INTMASK] |= TAD)
#define LM_MaskADC()           (LM_Regs[INTMASK] |= ADC)
#define LM_MaskEXT0()          (LM_Regs[INTMASK] |= EXT0)
#define LM_MaskEXT1()          (LM_Regs[INTMASK] |= EXT1)
#define LM_MaskEXT2()          (LM_Regs[INTMASK] |= EXT2)
#define LM_MaskEXT3()          (LM_Regs[INTMASK] |= EXT3)

/* Some macros to ummask the VEGA+ interrupt sources                          */
#define LM_UnMaskPLP()         (LM_Regs[INTMASK] &= ~PLP)
#define LM_UnMaskPCM()         (LM_Regs[INTMASK] &= ~PCM)

/* Vega+ product version */
#define LM_UnMaskUART()        (LM_Regs[INTMASK] &= ~SRX)
#define LM_UnMaskSRX()         (LM_Regs[RSIER]   |= RX_INT_ENABLE)
#define LM_UnMaskSTX()         (LM_Regs[RSIER]   |= TX_INT_ENABLE)
#define LM_UnMaskUARTStatus()  (LM_Regs[RSIER]   |= LINE_STATUS_ENABLE)

#define LM_UnMaskTMR0()        (LM_Regs[INTMASK] &= ~TMR0)
#define LM_UnMaskTMR1()        (LM_Regs[INTMASK] &= ~TMR1)
#define LM_UnMaskLCDEE()       (LM_Regs[INTMASK] &= ~LCDEE)
#define LM_UnMaskKPAD()        (LM_Regs[INTMASK] &= ~KPAD)
#define LM_UnMaskTAD()         (LM_Regs[INTMASK] &= ~TAD)
#define LM_UnMaskADC()         (LM_Regs[INTMASK] &= ~ADC)
#define LM_UnMaskEXT0()        (LM_Regs[INTMASK] &= ~EXT0)
#define LM_UnMaskEXT1()        (LM_Regs[INTMASK] &= ~EXT1)
#define LM_UnMaskEXT2()        (LM_Regs[INTMASK] &= ~EXT2)
#define LM_UnMaskEXT3()        (LM_Regs[INTMASK] &= ~EXT3)

/* Some macros to Acknoledge the VEGA+ interrupt sources                      */
#define LM_AckPLP()            (LM_Regs[INTACK] |= PLP)
#define LM_AckPCM()            (LM_Regs[INTACK] |= PCM)
#define LM_AckTMR0()           (LM_Regs[INTACK] |= TMR0)
#define LM_AckTMR1()           (LM_Regs[INTACK] |= TMR1)
#define LM_AckEXT0()           (LM_Regs[INTACK] |= EXT0)
#define LM_AckEXT1()           (LM_Regs[INTACK] |= EXT1)
#define LM_AckEXT2()           (LM_Regs[INTACK] |= EXT2)
#define LM_AckEXT3()           (LM_Regs[INTACK] |= EXT3)

/*#define INIT_LMREGS_MAPPING()	{ LM_Regs = (unsigned long*)REGS_BASE_ADR; }*/

#endif /*__LMREGS_H__*/
