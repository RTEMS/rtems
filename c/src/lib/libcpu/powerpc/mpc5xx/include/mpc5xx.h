/*
 * mpc5xx.h
 *
 * MPC5xx Internal I/O Definitions
 *
 *
 *  MPC5xx port sponsored by Defence Research and Development Canada - Suffield
 *  Copyright (C) 2004, Real-Time Systems Inc. (querbach@realtime.bc.ca)
 *
 *  Derived from c/src/lib/libcpu/powerpc/mpc8xx/include/mpc8xx.h:
 *
 *  Submitted By:                                                        *
 *                                                                       *
 *      W. Eric Norum                                                    *
 *      Saskatchewan Accelerator Laboratory                              *
 *      University of Saskatchewan                                       *
 *      107 North Road                                                   *
 *      Saskatoon, Saskatchewan, CANADA                                  *
 *      S7N 5C6                                                          *
 *                                                                       *
 *      eric@skatter.usask.ca                                            *
 *                                                                       *
 *  Modified for use with the MPC860 (original code was for MC68360)     *
 *  by                                                                   *
 *      Jay Monkman                                                      *
 *      Frasca International, Inc.                                       *
 *      906 E. Airport Rd.                                               *
 *      Urbana, IL, 61801                                                *
 *                                                                       *
 *      jmonkman@frasca.com                                              *
 *                                                                       *
 *  Modified further for use with the MPC821 by:                         *
 *      Andrew Bray <andy@chaos.org.uk>                                  *
 *                                                                       *
 *  With some corrections/additions by:                                  *
 *      Darlene A. Stewart and                                           *
 *      Charles-Antoine Gauthier                                         *
 *      Institute for Information Technology                             *
 *      National Research Council of Canada                              *
 *      Ottawa, ON  K1A 0R6                                              *
 *                                                                       *
 *      Darlene.Stewart@iit.nrc.ca                                       *
 *      charles.gauthier@iit.nrc.ca                                      *
 *                                                                       *
 *      Corrections/additions:                                           *
 *        Copyright (c) 1999, National Research Council of Canada        *
 *  
 *  MPC5xx port sponsored by Defence Research and Development Canada - Suffield
 *  Copyright (C) 2004, Real-Time Systems Inc. (querbach@realtime.bc.ca)
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _MPC5xx_h
#define _MPC5xx_h

#include <libcpu/spr.h>


#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Macros for accessing Special Purpose Registers (SPRs)
 */

#define _eieio   __asm__ volatile ("eieio\n"::)
#define _sync    __asm__ volatile ("sync\n"::)
#define _isync   __asm__ volatile ("isync\n"::)

/*
 * Core Registers (SPRs)
 */
#define DER             149     /* Debug Enable Register */
#define IMMR            638     /* Internal Memory Map Register */
#define IMMR_FLEN	(1<<11)	   /* Internal flash ROM enabled */

/*
 * Interrupt Control Registers (SPRs)
 */
#define EIE              80     /* External Interrupt Enable Register */
#define EID              81     /* External Interrupt Disable Register */
#define NRI              82     /* Non-Recoverable Interrupt Register */

#define ECR             148     /* Exception Cause Register */

/*
 * Bus Control Registers (SPRs)
 */
#define LCTRL1          156     /* L-Bus Support Control Register 1 */
#define LCTRL2          157     /* L-Bus Support Control Register 2 */
#define ICTRL           158     /* I-Bus Support Control Register */

/*
 * Burst Buffer Control Registers (SPRs)
 */
#define BBCMCR          560     /* Burst Buffer Configuration Register */
#define BBCMCR_BE       (1<<13) /* Burst enable */
#define BBCMCR_ETRE     (1<<12) /* Exception table relocation enable */

#define MI_RBA0         784     /* Region 0 Address Register */
#define MI_RBA1         785     /* Region 1 Address Register */
#define MI_RBA2         786     /* Region 2 Address Register */
#define MI_RBA3         787     /* Region 3 Address Register */

#define MI_RA0          816     /* Region 0 Attribute Register */
#define MI_RA1          817     /* Region 1 Attribute Register */
#define MI_RA2          818     /* Region 2 Attribute Register */
#define MI_RA3          819     /* Region 3 Attribute Register */
#define MI_GRA          528     /* Region Global Attribute Register */
#define MI_RA_PP	(3 << 10)	/* Protection bits: */
#define MI_RA_PP_SUPV	(1 << 10)	/*   Supervisor */
#define MI_RA_PP_USER	(2 << 10)	/*   User */
#define MI_RA_G		(1 << 6)	/* Guarded region */


/*
 * L-Bus to U-Bus Interface (L2U) Registers (SPRs)
 */
#define L2U_MCR         568     /* L2U Module Configuration Register */

#define L2U_RBA0        792     /* L2U Region 0 Address Register */
#define L2U_RBA1        793     /* L2U Region 1 Address Register */
#define L2U_RBA2        794     /* L2U Region 2 Address Register */
#define L2U_RBA3        795     /* L2U Region 3 Address Register */

#define L2U_RA0         824     /* L2U Region 0 Attribute Register */
#define L2U_RA1         825     /* L2U Region 1 Attribute Register */
#define L2U_RA2         826     /* L2U Region 2 Attribute Register */
#define L2U_RA3         827     /* L2U Region 3 Attribute Register */
#define L2U_GRA         536     /* L2U Global Region Attribute Register */
#define L2U_RA_PP	(3 << 10)	/* Protection bits: */
#define L2U_RA_PP_SUPV	(1 << 10)	/*   Supervisor */
#define L2U_RA_PP_USER	(2 << 10)	/*   User */
#define L2U_RA_G	(1 << 6)	/* Guarded region */


/*
 *************************************************************************
 *                         REGISTER SUBBLOCKS                            *
 *************************************************************************
 */

/*
 *************************************************************************
 *                 System Protection Control Register (SYPCR)            *
 *************************************************************************
 */
#define USIU_SYPCR_SWTC(x)      ((x)<<16)   /* Software watchdog timer count */
#define USIU_SYPCR_BMT(x)       ((x)<<8)    /* Bus monitor timing */
#define USIU_SYPCR_BME          (1<<7)      /* Bus monitor enable */
#define USIU_SYPCR_SWF          (1<<3)      /* Software watchdog freeze */
#define USIU_SYPCR_SWE          (1<<2)      /* Software watchdog enable */
#define USIU_SYPCR_SWRI         (1<<1)      /* Watchdog reset/interrupt sel. */
#define USIU_SYPCR_SWP          (1<<0)      /* Software watchdog prescale */

#define USIU_SYPCR_BMT(x)       ((x)<<8)    /* Bus monitor timing */
#define USIU_SYPCR_BME          (1<<7)      /* Bus monitor enable */
#define USIU_SYPCR_SWF          (1<<3)      /* Software watchdog freeze */
#define USIU_SYPCR_SWE          (1<<2)      /* Software watchdog enable */
#define USIU_SYPCR_SWRI         (1<<1)      /* Watchdog reset/interrupt sel. */
#define USIU_SYPCR_SWP          (1<<0)      /* Software watchdog prescale */

/*
 *************************************************************************
 *                 Software Service Register (SWSR)                      *
 *************************************************************************
 */
#define TICKLE_WATCHDOG()		\
do {					\
  usiu.swsr = 0x556C;			\
  usiu.swsr = 0xAA39;			\
} while (0)				\

/*
 *************************************************************************
 *                        Memory Control Registers                       *
 *************************************************************************
 */
#define USIU_MEMC_BR_BA(x)      (((rtems_unsigned32)x)&0xffff8000)
					 /* Base address */
#define USIU_MEMC_BR_AT(x)      ((x)<<12) /* Address type */
#define USIU_MEMC_BR_PS8        (1<<10)  /* 8 bit port */
#define USIU_MEMC_BR_PS16       (2<<10)  /* 16 bit port */
#define USIU_MEMC_BR_PS32       (0<<10)  /* 32 bit port */
#define USIU_MEMC_BR_WP         (1<<8)   /* Write protect */
#define USIU_MEMC_BR_WEBS       (1<<5)   /* Write enable/byte select */
#define USIU_MEMC_BR_TBDIP      (1<<4)   /* Toggle-Burst data in progress*/
#define USIU_MEMC_BR_LBDIP      (1<<3)   /* Late-burst data in progress */
#define USIU_MEMC_BR_SETA       (1<<2)   /* External transfer acknowledge */
#define USIU_MEMC_BR_BI         (1<<1)   /* Burst inhibit */
#define USIU_MEMC_BR_V          (1<<0)   /* Base/Option register are valid */

#define USIU_MEMC_OR_32K        0xffff8000      /* Address range */
#define USIU_MEMC_OR_64K        0xffff0000
#define USIU_MEMC_OR_128K       0xfffe0000
#define USIU_MEMC_OR_256K       0xfffc0000
#define USIU_MEMC_OR_512K       0xfff80000
#define USIU_MEMC_OR_1M         0xfff00000
#define USIU_MEMC_OR_2M         0xffe00000
#define USIU_MEMC_OR_4M         0xffc00000
#define USIU_MEMC_OR_8M         0xff800000
#define USIU_MEMC_OR_16M        0xff000000
#define USIU_MEMC_OR_32M        0xfe000000
#define USIU_MEMC_OR_64M        0xfc000000
#define USIU_MEMC_OR_128        0xf8000000
#define USIU_MEMC_OR_256M       0xf0000000
#define USIU_MEMC_OR_512M       0xe0000000
#define USIU_MEMC_OR_1G         0xc0000000
#define USIU_MEMC_OR_2G         0x80000000
#define USIU_MEMC_OR_4G         0x00000000
#define USIU_MEMC_OR_ATM(x)     ((x)<<12)   /* Address type mask */
#define USIU_MEMC_OR_CSNT       (1<<11)     /* Chip select is negated early */
#define USIU_MEMC_OR_ACS_NORM   (0<<9)      /* *CS asserted with addr lines */
#define USIU_MEMC_OR_ACS_QRTR   (2<<9)      /* *CS asserted 1/4 after addr */
#define USIU_MEMC_OR_ACS_HALF   (3<<9)      /* *CS asserted 1/2 after addr */
#define USIU_MEMC_OR_ETHR       (1<<8)      /* Extended hold time on reads */
#define USIU_MEMC_OR_SCY(x)     ((x)<<4)    /* Cycle length in clocks */
#define USIU_MEMC_OR_BSCY(x)    ((x)<<1)    /* Burst beat length in clocks */
#define USIU_MEMC_OR_TRLX       (1<<0)      /* Relaxed timing in GPCM */

/*
 *************************************************************************
 *                      Clocks and Reset Controlmer                      *
 *************************************************************************
 */

#define USIU_SCCR_DBCT          (1<<31)    /* Disable backup clock for timers */
#define USIU_SCCR_COM(x)	((x)<<29)  /* Clock output mode */
#define USIU_SCCR_RTDIV         (1<<24)    /* RTC, PIT divide by 256, not 4 */
#define USIU_PRQEN              (1<<21)    /* MSR[POW] controls frequency */
#define USIU_SCCR_EBDF(x)       ((x)<<17)  /* External bus division factor */
#define USIU_LME                (1<<16)    /* Enable limp mode */
#define USIU_ENGDIV(x)          ((x)<<9)   /* Set engineering clock divisor */

#define USIU_PLPRCR_MF(x)	(((x)-1)<<20)	/* PLL mult. factor (true) */
#define USIU_PLPRCR_SPLS        (1<<16)    /* System PLL locked */
#define	USIU_PLPRCR_TEXPS	(1<<14)    /* Assert TEXP always */

/*
 *************************************************************************
 *                      Programmable Interval Timer                      *
 *************************************************************************
 */
#define USIU_PISCR_PIRQ(x)      (1<<(15-x))  /* PIT interrupt level */
#define USIU_PISCR_PS           (1<<7)    /* PIT Interrupt state */
#define USIU_PISCR_PIE          (1<<2)    /* PIT interrupt enable */
#define USIU_PISCR_PITF         (1<<1)    /* Stop timer when freeze asserted */
#define USIU_PISCR_PTE          (1<<0)    /* PIT enable */

/*
 *************************************************************************
 *                             Time Base                                 *
 *************************************************************************
 */
#define USIU_TBSCR_TBIRQ(x)     (1<<(15-x))  /* TB interrupt level */
#define USIU_TBSCR_REFA         (1<<7)    /* TB matches TBREFF0 */
#define USIU_TBSCR_REFB         (1<<6)    /* TB matches TBREFF1 */
#define USIU_TBSCR_REFAE        (1<<3)    /* Enable ints for REFA */
#define USIU_TBSCR_REFBE        (1<<2)    /* Enable ints for REFB */
#define USIU_TBSCR_TBF          (1<<1)    /* TB stops on FREEZE */
#define USIU_TBSCR_TBE          (1<<0)    /* enable TB and decrementer */

/*
 *************************************************************************
 *                       SIU Interrupt Mask                              *
 *************************************************************************
 */
#define USIU_SIMASK_IRM0        (1<<31)
#define USIU_SIMASK_LVM0        (1<<30)
#define USIU_SIMASK_IRM1        (1<<29)
#define USIU_SIMASK_LVM1        (1<<28)
#define USIU_SIMASK_IRM2        (1<<27)
#define USIU_SIMASK_LVM2        (1<<26)
#define USIU_SIMASK_IRM3        (1<<25)
#define USIU_SIMASK_LVM3        (1<<24)
#define USIU_SIMASK_IRM4        (1<<23)
#define USIU_SIMASK_LVM4        (1<<22)
#define USIU_SIMASK_IRM5        (1<<21)
#define USIU_SIMASK_LVM5        (1<<20)
#define USIU_SIMASK_IRM6        (1<<19)
#define USIU_SIMASK_LVM6        (1<<18)
#define USIU_SIMASK_IRM7        (1<<17)
#define USIU_SIMASK_LVM7        (1<<16)

/*
 *************************************************************************
 *                       SIU Module Control                              *
 *************************************************************************
 */
#define USIU_SIUMCR_EARB        (1<<31)
#define USIU_SIUMCR_EARP0       (0<<28)
#define USIU_SIUMCR_EARP1       (1<<28)
#define USIU_SIUMCR_EARP2       (2<<28)
#define USIU_SIUMCR_EARP3       (3<<28)
#define USIU_SIUMCR_EARP4       (4<<28)
#define USIU_SIUMCR_EARP5       (5<<28)
#define USIU_SIUMCR_EARP6       (6<<28)
#define USIU_SIUMCR_EARP7       (7<<28)
#define USIU_SIUMCR_DSHW        (1<<23)
#define USIU_SIUMCR_DBGC0       (0<<21)
#define USIU_SIUMCR_DBGC1       (1<<21)
#define USIU_SIUMCR_DBGC2       (2<<21)
#define USIU_SIUMCR_DBGC3       (3<<21)
#define USIU_SIUMCR_DBPC        (1<<20)
#define USIU_SIUMCR_ATWC        (1<<19)
#define USIU_SIUMCR_GPC0        (0<<17)
#define USIU_SIUMCR_GPC1        (1<<17)
#define USIU_SIUMCR_GPC2        (2<<17)
#define USIU_SIUMCR_GPC3        (3<<17)
#define USIU_SIUMCR_DLK         (1<<16)
#define USIU_SIUMCR_SC0         (0<<13)
#define USIU_SIUMCR_SC1         (1<<13)
#define USIU_SIUMCR_SC2         (2<<13)
#define USIU_SIUMCR_SC3         (3<<13)
#define USIU_SIUMCR_RCTX        (1<<12)
#define USIU_SIUMCR_MLRC0       (0<<10)
#define USIU_SIUMCR_MLRC1       (1<<10)
#define USIU_SIUMCR_MLRC2       (2<<10)
#define USIU_SIUMCR_MLRC3       (3<<10)
#define USIU_SIUMCR_MTSC        (1<<7)

/*
 *  Value to write to a key register to unlock the corresponding SIU register
 */
#define USIU_UNLOCK_KEY         0x55CCAA33

/*
 *************************************************************************
 *                       UIMB Module Control                             *
 *************************************************************************
 */
#define UIMB_UMCR_STOP          (1<<31)
#define UIMB_UMCR_IRQMUX(x)     ((x)<<29)
#define UIMB_UMCR_HSPEED        (1<<28)
 
/*
 *************************************************************************
 *              QSMCM Serial Communications Interface (SCI)              *
 *************************************************************************
 */
 
 
#define QSMCM_ILDSCI(x)        ((x)<<8)    /* SCI interrupt level */

#define QSMCM_SCI_BAUD(x)      ((x)&0x1FFF) /* Baud rate field */

#define QSMCM_SCI_LOOPS        (1<<14)     /* Loopback test mode */
#define QSMCM_SCI_WOMS         (1<<13)     /* Wire-or mode select */
#define QSMCM_SCI_ILT          (1<<12)     /* Idle-line detect type */
#define QSMCM_SCI_PT           (1<<11)     /* Parity type */
#define QSMCM_SCI_PE           (1<<10)     /* Parity enable */
#define QSMCM_SCI_M            (1<<9)      /* 11-bit mode */
#define QSMCM_SCI_WAKE         (1<<8)      /* Wakeup mode */

#define QSMCM_SCI_TIE          (1<<7)      /* Transmitter interrupt enable */
#define QSMCM_SCI_TCIE         (1<<6)      /* Transmit complete intr. enable */
#define QSMCM_SCI_RIE          (1<<5)      /* Receiver interrupt enable */
#define QSMCM_SCI_ILIE         (1<<4)      /* Idle line interrupt enable */
#define QSMCM_SCI_TE           (1<<3)      /* Transmitter enable */
#define QSMCM_SCI_RE           (1<<2)      /* Receiver enable */
#define QSMCM_SCI_RWU          (1<<1)      /* Receiver wake-up enable */
#define QSMCM_SCI_SBK          (1<<0)      /* Send break */

#define QSMCM_SCI_TDRE         (1<<8)      /* Transmit data register empty */
#define QSMCM_SCI_TC           (1<<7)      /* Transmit complete */
#define QSMCM_SCI_RDRF         (1<<6)      /* Receive data register full */
#define QSMCM_SCI_RAF          (1<<5)      /* Receiver active flag */
#define QSMCM_SCI_IDLE         (1<<4)      /* Idle line detected */
#define QSMCM_SCI_OR           (1<<3)      /* Receiver overrun error */
#define QSMCM_SCI_NF           (1<<2)      /* Receiver noise error flag */
#define QSMCM_SCI_FE           (1<<1)      /* Receiver framing error */
#define QSMCM_SCI_PF           (1<<0)      /* Receiver parity error */

/*
 *************************************************************************
 *   Unified System Interface Unit                                       *
 *************************************************************************
 */

/*
 * Memory controller registers
 */
typedef struct m5xxMEMCRegisters_ {
  rtems_unsigned32        _br;
  rtems_unsigned32        _or;    /* Used to be called 'or'; reserved ANSI C++ keyword */
} m5xxMEMCRegisters_t;

/*
 * USIU itself
 */
typedef struct usiu_ {
  /*
   * SIU Block
   */
  rtems_unsigned32      siumcr;
  rtems_unsigned32      sypcr;
  rtems_unsigned32      _pad70;
  rtems_unsigned16      _pad0;
  rtems_unsigned16      swsr;
  rtems_unsigned32      sipend;
  rtems_unsigned32      simask;
  rtems_unsigned32      siel;
  rtems_unsigned32      sivec;
  rtems_unsigned32      tesr;
  rtems_unsigned32      sgpiodt1;
  rtems_unsigned32      sgpiodt2;
  rtems_unsigned32      sgpiocr;
  rtems_unsigned32      emcr;
  rtems_unsigned8	_pad71[0x03C-0x034];
  rtems_unsigned32      pdmcr;
  rtems_unsigned8       _pad2[0x100-0x40];
  
  /*
   * MEMC Block
   */
  m5xxMEMCRegisters_t   memc[4];
  rtems_unsigned8       _pad7[0x140-0x120];
  rtems_unsigned32      dmbr;
  rtems_unsigned32      dmor;
  rtems_unsigned8       _pad8[0x178-0x148];
  rtems_unsigned16      mstat;
  rtems_unsigned8       _pad9[0x200-0x17A];
  
  /*
   * System integration timers
   */
  rtems_unsigned16      tbscr;
  rtems_unsigned16      _pad10;
  rtems_unsigned32      tbreff0;
  rtems_unsigned32      tbreff1;
  rtems_unsigned8       _pad11[0x220-0x20c];
  rtems_unsigned16      rtcsc;
  rtems_unsigned16      _pad12;
  rtems_unsigned32      rtc;
  rtems_unsigned32      rtsec;
  rtems_unsigned32      rtcal;
  rtems_unsigned32      _pad13[4];
  rtems_unsigned16      piscr;
  rtems_unsigned16      _pad14;
  rtems_unsigned16      pitc;
  rtems_unsigned16      _pad_14_1;
  rtems_unsigned16      pitr;
  rtems_unsigned16      _pad_14_2;
  rtems_unsigned8       _pad15[0x280-0x24c];
  
  /*
   * Clocks and Reset
   */
  rtems_unsigned32      sccr;
  rtems_unsigned32      plprcr;
  rtems_unsigned16      rsr;
  rtems_unsigned16	_pad72;
  rtems_unsigned16	colir;
  rtems_unsigned16	_pad73;
  rtems_unsigned16	vsrmcr;
  rtems_unsigned8       _pad16[0x300-0x292];
  
  /*
   * System integration timers keys
   */
  rtems_unsigned32      tbscrk;
  rtems_unsigned32      tbreff0k;
  rtems_unsigned32      tbreff1k;
  rtems_unsigned32      tbk;
  rtems_unsigned32      _pad17[4];
  rtems_unsigned32      rtcsk;
  rtems_unsigned32      rtck;
  rtems_unsigned32      rtseck;
  rtems_unsigned32      rtcalk;
  rtems_unsigned32      _pad18[4];
  rtems_unsigned32      piscrk;
  rtems_unsigned32      pitck;
  rtems_unsigned8       _pad19[0x380-0x348];
  
  /*
   * Clocks and Reset Keys
   */
  rtems_unsigned32      sccrk;
  rtems_unsigned32      plprck;
  rtems_unsigned32      rsrk;
  rtems_unsigned8       _pad20[0x400-0x38c];
} usiu_t;

extern volatile usiu_t usiu;	/* defined in linkcmds */

/*
 *************************************************************************
 *   Inter-Module Bus and Devices                                        *
 *************************************************************************
 */

/*
 * Dual-Port TPU RAM (DPTRAM)
 */
typedef struct m5xxDPTRAMRegisters_ {
  rtems_unsigned8	pad[0x4000];		/* define later */
} m5xxDPTRAMRegisters_t;

/*
 * Time Processor Unit (TPU)
 */
typedef struct m5xxTPU3Registers_ {
  rtems_unsigned8	pad[0x400];		/* define later */
} m5xxTPU3Registers_t;

/*
 * Queued A/D Converter (QADC)
 */
typedef struct m5xxQADC64Registers_ {
  rtems_unsigned8	pad[0x400];		/* define later */
} m5xxQADC64Registers_t;

/*
 * Serial Communications Interface (SCI)
 */
typedef struct m5xxSCIRegisters_ {
  rtems_unsigned16        sccr0;
  rtems_unsigned16        sccr1;
  rtems_unsigned16        scsr;
  rtems_unsigned16        scdr;
} m5xxSCIRegisters_t;

/*
 * Serial Peripheral Interface (SPI)
 */
typedef struct m5xxSPIRegisters_ {
  rtems_unsigned16        spcr0;
  rtems_unsigned16        spcr1;
  rtems_unsigned16        spcr2;
  rtems_unsigned8         spcr3;
  rtems_unsigned8         spsr;
} m5xxSPIRegisters_t;

/*
 * Queued Serial Multi-Channel Module (QSMCM)
 */ 
typedef struct m5xxQSMCMRegisters_ {
  rtems_unsigned16      qsmcmmcr;
  rtems_unsigned16      qtest;
  rtems_unsigned16      qdsci_il;
  rtems_unsigned16      qspi_il;

  m5xxSCIRegisters_t    sci1;

  rtems_unsigned8       _pad10[0x14-0x10];

  rtems_unsigned16      portqs;
  rtems_unsigned16      pqspar;
  m5xxSPIRegisters_t    spi;

  m5xxSCIRegisters_t    sci2;

  rtems_unsigned16      qsci1cr;
  rtems_unsigned16      qsci1sr;
  rtems_unsigned16      sctq[0x10];
  rtems_unsigned16      scrq[0x10];

  rtems_unsigned8       _pad6C[0x140-0x06C];

  rtems_unsigned16      recram[0x20];
  rtems_unsigned16      tranram[0x20];
  rtems_unsigned16      comdram[0x20];
} m5xxQSMCMRegisters_t;

/*
 * Modular Input/Output System (MIOS)
 */
typedef struct m5xxMIOS1Registers_ {
  rtems_unsigned8	pad[0x1000];		/* define later */
} m5xxMIOS1Registers_t;

/*
 * Can 2.0B Controller (TouCAN)
 */
typedef struct m5xxTouCANRegisters_ {
  rtems_unsigned8	pad[0x400];		/* define later */
} m5xxTouCANRegisters_t;

/*
 * U-Bus to IMB3 Bus Interface Module (UIMB)
 */
typedef struct m5xxUIMBRegisters_ {
  rtems_unsigned32      umcr;
  rtems_unsigned32      utstcreg;
  rtems_unsigned32      uipend;
} m5xxUIMBRegisters_t;

/*
 * IMB itself
 */
typedef struct imb_ {
  m5xxDPTRAMRegisters_t	dptram;
  m5xxTPU3Registers_t	tpu[2];
  m5xxQADC64Registers_t	qadc[2];
  m5xxQSMCMRegisters_t	qsmcm;
  rtems_unsigned8       _pad5200[0x6000-0x5200];
  m5xxMIOS1Registers_t	mios;
  m5xxTouCANRegisters_t	toucan[2];
  rtems_unsigned8       _pad7800[0x7F80-0x7800];
  m5xxUIMBRegisters_t	uimb;
} imb_t;

extern volatile imb_t imb;		/* defined in linkcmds */


#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif /* _MPC5xx_h */
