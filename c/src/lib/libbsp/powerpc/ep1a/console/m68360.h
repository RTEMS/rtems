/*
 *  MOTOROLA MC68360 QUAD INTEGRATED COMMUNICATIONS CONTROLLER (QUICC)
 *
 *                        HARDWARE DECLARATIONS
 *
 *
 *  Submitted By:
 *
 *      W. Eric Norum
 *      Saskatchewan Accelerator Laboratory
 *      University of Saskatchewan
 *      107 North Road
 *      Saskatoon, Saskatchewan, CANADA
 *      S7N 5C6
 *
 *      eric@skatter.usask.ca
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __MC68360_h
#define __MC68360_h

#include "rsPMCQ1.h"

/*
 *************************************************************************
 *                         REGISTER SUBBLOCKS                            *
 *************************************************************************
 */

/*
 * Memory controller registers
 */
typedef struct m360MEMCRegisters_ {
  uint32_t         br;
  uint32_t         or;
  uint32_t         _pad[2];
} m360MEMCRegisters_t;


#define M360_GSMR_RFW            0x00000020

#define M360_GSMR_RINV           0x02000000
#define M360_GSMR_TINV           0x01000000
#define M360_GSMR_TDCR_16X       0x00020000
#define M360_GSMR_RDCR_16X       0x00008000
#define M360_GSMR_DIAG_LLOOP     0x00000040
#define M360_GSMR_ENR            0x00000020
#define M360_GSMR_ENT            0x00000010
#define M360_GSMR_MODE_UART      0x00000004

#define  M360_PSMR_FLC           0x8000
#define  M360_PSMR_SL_1          0x0000
#define  M360_PSMR_SL_2          0x4000
#define  M360_PSMR_CL5           0x0000
#define  M360_PSMR_CL6           0x1000
#define  M360_PSMR_CL7           0x2000
#define  M360_PSMR_CL8           0x3000
#define  M360_PSMR_UM_NORMAL     0x0000
#define  M360_PSMR_FRZ           0x0200
#define  M360_PSMR_RZS           0x0100
#define  M360_PSMR_SYN           0x0080
#define  M360_PSMR_DRT           0x0040
#define  M360_PSMR_PEN           0x0010
#define  M360_PSMR_RPM_ODD       0x0000
#define  M360_PSMR_RPM_LOW       0x0004
#define  M360_PSMR_RPM_EVEN      0x0008
#define  M360_PSMR_RPM_HI        0x000c
#define  M360_PSMR_TPM_ODD       0x0000
#define  M360_PSMR_TPM_LOW       0x0001
#define  M360_PSMR_TPM_EVEN      0x0002
#define  M360_PSMR_TPM_HI        0x0003

/*
 * Serial Communications Controller registers
 */
typedef struct m360SCCRegisters_ {
  uint32_t         gsmr_l;
  uint32_t         gsmr_h;
  uint16_t          psmr;
  uint16_t          _pad0;
  uint16_t          todr;
  uint16_t          dsr;
  uint16_t          scce;
  uint16_t          _pad1;
  uint16_t          sccm;
  uint8_t          _pad2;
  uint8_t          sccs;
  uint32_t         _pad3[2];
} m360SCCRegisters_t;

/*
 * Serial Management Controller registers
 */
typedef struct m360SMCRegisters_ {
  uint16_t          _pad0;
  uint16_t          smcmr;
  uint16_t          _pad1;
  uint8_t          smce;
  uint8_t          _pad2;
  uint16_t          _pad3;
  uint8_t          smcm;
  uint8_t          _pad4;
  uint32_t         _pad5;
} m360SMCRegisters_t;


/*
 *************************************************************************
 *                         Miscellaneous Parameters                      *
 *************************************************************************
 */
typedef struct m360MiscParms_ {
  uint16_t          rev_num;
  uint16_t          _res1;
  uint32_t         _res2;
  uint32_t         _res3;
} m360MiscParms_t;

/*
 *************************************************************************
 *                              RISC Timers                              *
 *************************************************************************
 */
typedef struct m360TimerParms_ {
  uint16_t          tm_base;
  uint16_t          _tm_ptr;
  uint16_t          _r_tmr;
  uint16_t          _r_tmv;
  uint32_t         tm_cmd;
  uint32_t         tm_cnt;
} m360TimerParms_t;

/*
 * RISC Controller Configuration Register (RCCR)
 * All other bits in this register are either reserved or
 * used only with a Motorola-supplied RAM microcode packge.
 */
#define M360_RCCR_TIME    (1<<15)  /* Enable timer */
#define M360_RCCR_TIMEP(x)  ((x)<<8)  /* Timer period */

/*
 * Command register
 * Set up this register before issuing a M360_CR_OP_SET_TIMER command.
 */
#define M360_TM_CMD_V    (1<<31)    /* Set to enable timer */
#define M360_TM_CMD_R    (1<<30)    /* Set for automatic restart */
#define M360_TM_CMD_TIMER(x)  ((x)<<16)  /* Select timer */
#define M360_TM_CMD_PERIOD(x)  (x)    /* Timer period (16 bits) */

/*
 *************************************************************************
 *                               DMA Controllers                         *
 *************************************************************************
 */
typedef struct m360IDMAparms_ {
  uint16_t          ibase;
  uint16_t          ibptr;
  uint32_t         _istate;
  uint32_t         _itemp;
} m360IDMAparms_t;

/*
 *************************************************************************
 *                   Serial Communication Controllers                    *
 *************************************************************************
 */
typedef struct m360SCCparms_ {
  uint16_t          rbase;
  uint16_t          tbase;
  uint8_t          rfcr;
  uint8_t          tfcr;
  uint16_t          mrblr;
  uint32_t         _rstate;
  uint32_t         _pad0;
  uint16_t          _rbptr;
  uint16_t          _pad1;
  uint32_t         _pad2;
  uint32_t         _tstate;
  uint32_t         _pad3;
  uint16_t          _tbptr;
  uint16_t          _pad4;
  uint32_t         _pad5;
  uint32_t         _rcrc;
  uint32_t         _tcrc;
  union {
    struct {
      uint32_t         _res0;
      uint32_t         _res1;
      uint16_t          max_idl;
      uint16_t          _idlc;
      uint16_t          brkcr;
      uint16_t          parec;
      uint16_t          frmec;
      uint16_t          nosec;
      uint16_t          brkec;
      uint16_t          brklen;
      uint16_t          uaddr[2];
      uint16_t          _rtemp;
      uint16_t          toseq;
      uint16_t          character[8];
      uint16_t          rccm;
      uint16_t          rccr;
      uint16_t          rlbc;
    } uart;
    struct {
      uint32_t         crc_p;
      uint32_t         crc_c;
    } transparent;

  } un;
} m360SCCparms_t;

typedef struct m360SCCENparms_ {
  uint16_t          rbase;
  uint16_t          tbase;
  uint8_t          rfcr;
  uint8_t          tfcr;
  uint16_t          mrblr;
  uint32_t         _rstate;
  uint32_t         _pad0;
  uint16_t          _rbptr;
  uint16_t          _pad1;
  uint32_t         _pad2;
  uint32_t         _tstate;
  uint32_t         _pad3;
  uint16_t          _tbptr;
  uint16_t          _pad4;
  uint32_t         _pad5;
  uint32_t         _rcrc;
  uint32_t         _tcrc;
  union {
    struct {
      uint32_t         _res0;
      uint32_t         _res1;
      uint16_t          max_idl;
      uint16_t          _idlc;
      uint16_t          brkcr;
      uint16_t          parec;
      uint16_t          frmec;
      uint16_t          nosec;
      uint16_t          brkec;
      uint16_t          brklen;
      uint16_t          uaddr[2];
      uint16_t          _rtemp;
      uint16_t          toseq;
      uint16_t          character[8];
      uint16_t          rccm;
      uint16_t          rccr;
      uint16_t          rlbc;
    } uart;
    struct {
      uint32_t         c_pres;
      uint32_t         c_mask;
      uint32_t         crcec;
      uint32_t         alec;
      uint32_t         disfc;
      uint16_t          pads;
      uint16_t          ret_lim;
      uint16_t          _ret_cnt;
      uint16_t          mflr;
      uint16_t          minflr;
      uint16_t          maxd1;
      uint16_t          maxd2;
      uint16_t          _maxd;
      uint16_t          dma_cnt;
      uint16_t          _max_b;
      uint16_t          gaddr1;
      uint16_t          gaddr2;
      uint16_t          gaddr3;
      uint16_t          gaddr4;
      uint32_t         _tbuf0data0;
      uint32_t         _tbuf0data1;
      uint32_t         _tbuf0rba0;
      uint32_t         _tbuf0crc;
      uint16_t          _tbuf0bcnt;
      uint16_t          paddr_h;
      uint16_t          paddr_m;
      uint16_t          paddr_l;
      uint16_t          p_per;
      uint16_t          _rfbd_ptr;
      uint16_t          _tfbd_ptr;
      uint16_t          _tlbd_ptr;
      uint32_t         _tbuf1data0;
      uint32_t         _tbuf1data1;
      uint32_t         _tbuf1rba0;
      uint32_t         _tbuf1crc;
      uint16_t          _tbuf1bcnt;
      uint16_t          _tx_len;
      uint16_t          iaddr1;
      uint16_t          iaddr2;
      uint16_t          iaddr3;
      uint16_t          iaddr4;
      uint16_t          _boff_cnt;
      uint16_t          taddr_h;
      uint16_t          taddr_m;
      uint16_t          taddr_l;
    } ethernet;
    struct {
      uint32_t         crc_p;
      uint32_t         crc_c;
    } transparent;
  } un;
} m360SCCENparms_t;

/*
 * Receive and transmit function code register bits
 * These apply to the function code registers of all devices, not just SCC.
 */
#define M360_RFCR_MOT    (1<<4)
#define M360_RFCR_DMA_SPACE  0x8
#define M360_TFCR_MOT    (1<<4)
#define M360_TFCR_DMA_SPACE  0x8

/*
 *************************************************************************
 *                     Serial Management Controllers                     *
 *************************************************************************
 */
typedef struct m360SMCparms_ {
  uint16_t          rbase;
  uint16_t          tbase;
  uint8_t          rfcr;
  uint8_t          tfcr;
  uint16_t          mrblr;
  uint32_t         _rstate;
  uint32_t         _pad0;
  uint16_t          _rbptr;
  uint16_t          _pad1;
  uint32_t         _pad2;
  uint32_t         _tstate;
  uint32_t         _pad3;
  uint16_t          _tbptr;
  uint16_t          _pad4;
  uint32_t         _pad5;
  union {
    struct {
      uint16_t          max_idl;
      uint16_t          _pad0;
      uint16_t          brklen;
      uint16_t          brkec;
      uint16_t          brkcr;
      uint16_t          _r_mask;
    } uart;
    struct {
      uint16_t          _pad0[5];
    } transparent;
  } un;
} m360SMCparms_t;

/*
 * Mode register
 */
#define M360_SMCMR_CLEN(x)    ((x)<<11)  /* Character length */
#define M360_SMCMR_2STOP    (1<<10)  /* 2 stop bits */
#define M360_SMCMR_PARITY    (1<<9)  /* Enable parity */
#define M360_SMCMR_EVEN      (1<<8)  /* Even parity */
#define M360_SMCMR_SM_GCI    (0<<4)  /* GCI Mode */
#define M360_SMCMR_SM_UART    (2<<4)  /* UART Mode */
#define M360_SMCMR_SM_TRANSPARENT  (3<<4)  /* Transparent Mode */
#define M360_SMCMR_DM_LOOPBACK    (1<<2)  /* Local loopback mode */
#define M360_SMCMR_DM_ECHO    (2<<2)  /* Echo mode */
#define M360_SMCMR_TEN      (1<<1)  /* Enable transmitter */
#define M360_SMCMR_REN      (1<<0)  /* Enable receiver */

/*
 * Event and mask registers (SMCE, SMCM)
 */
#define M360_SMCE_BRK  (1<<4)
#define M360_SMCE_BSY  (1<<2)
#define M360_SMCE_TX  (1<<1)
#define M360_SMCE_RX  (1<<0)

/*
 *************************************************************************
 *                      Serial Peripheral Interface                      *
 *************************************************************************
 */
typedef struct m360SPIparms_ {
  uint16_t          rbase;
  uint16_t          tbase;
  uint8_t          rfcr;
  uint8_t          tfcr;
  uint16_t          mrblr;
  uint32_t         _rstate;
  uint32_t         _pad0;
  uint16_t          _rbptr;
  uint16_t          _pad1;
  uint32_t         _pad2;
  uint32_t         _tstate;
  uint32_t         _pad3;
  uint16_t          _tbptr;
  uint16_t          _pad4;
  uint32_t         _pad5;
} m360SPIparms_t;

/*
 * Mode register (SPMODE)
 */
#define M360_SPMODE_LOOP    (1<<14)  /* Local loopback mode */
#define M360_SPMODE_CI      (1<<13)  /* Clock invert */
#define M360_SPMODE_CP      (1<<12)  /* Clock phase */
#define M360_SPMODE_DIV16    (1<<11)  /* Divide BRGCLK by 16 */
#define M360_SPMODE_REV      (1<<10)  /* Reverse data */
#define M360_SPMODE_MASTER    (1<<9)  /* SPI is master */
#define M360_SPMODE_EN      (1<<8)  /* Enable SPI */
#define M360_SPMODE_CLEN(x)    ((x)<<4)  /* Character length */
#define M360_SPMODE_PM(x)    (x)  /* Prescaler modulus */

/*
 * Mode register (SPCOM)
 */
#define M360_SPCOM_STR      (1<<7)  /* Start transmit */

/*
 * Event and mask registers (SPIE, SPIM)
 */
#define M360_SPIE_MME  (1<<5)    /* Multi-master error */
#define M360_SPIE_TXE  (1<<4)    /* Tx error */
#define M360_SPIE_BSY  (1<<2)    /* Busy condition*/
#define M360_SPIE_TXB  (1<<1)    /* Tx buffer */
#define M360_SPIE_RXB  (1<<0)    /* Rx buffer */

/*
 *************************************************************************
 *                 SDMA (SCC, SMC, SPI) Buffer Descriptors               *
 *************************************************************************
 */
typedef struct m360BufferDescriptor_ {
  uint16_t          status;
  uint16_t          length;
  uint32_t          buffer;  /* this is a void * to the 360 */
} m360BufferDescriptor_t;

/*
 * Bits in receive buffer descriptor status word
 */
#define M360_BD_EMPTY    (1<<15)  /* Ethernet, SCC UART, SMC UART, SPI */
#define M360_BD_WRAP    (1<<13)  /* Ethernet, SCC UART, SMC UART, SPI */
#define M360_BD_INTERRUPT  (1<<12)  /* Ethernet, SCC UART, SMC UART, SPI */
#define M360_BD_LAST    (1<<11)  /* Ethernet, SPI */
#define M360_BD_CONTROL_CHAR  (1<<11)  /* SCC UART */
#define M360_BD_FIRST_IN_FRAME  (1<<10)  /* Ethernet */
#define M360_BD_ADDRESS    (1<<10)  /* SCC UART */
#define M360_BD_CONTINUOUS  (1<<9)  /* SCC UART, SMC UART, SPI */
#define M360_BD_MISS    (1<<8)  /* Ethernet */
#define M360_BD_IDLE    (1<<8)  /* SCC UART, SMC UART */
#define M360_BD_ADDRSS_MATCH  (1<<7)  /* SCC UART */
#define M360_BD_LONG    (1<<5)  /* Ethernet */
#define M360_BD_BREAK    (1<<5)  /* SCC UART, SMC UART */
#define M360_BD_NONALIGNED  (1<<4)  /* Ethernet */
#define M360_BD_FRAMING_ERROR  (1<<4)  /* SCC UART, SMC UART */
#define M360_BD_SHORT    (1<<3)  /* Ethernet */
#define M360_BD_PARITY_ERROR  (1<<3)  /* SCC UART, SMC UART */
#define M360_BD_CRC_ERROR  (1<<2)  /* Ethernet */
#define M360_BD_OVERRUN    (1<<1)  /* Ethernet, SCC UART, SMC UART, SPI */
#define M360_BD_COLLISION  (1<<0)  /* Ethernet */
#define M360_BD_CARRIER_LOST  (1<<0)  /* SCC UART */
#define M360_BD_MASTER_ERROR  (1<<0)  /* SPI */

/*
 * Bits in transmit buffer descriptor status word
 * Many bits have the same meaning as those in receiver buffer descriptors.
 */
#define M360_BD_READY    (1<<15)  /* Ethernet, SCC UART, SMC UART, SPI */
#define M360_BD_PAD    (1<<14)  /* Ethernet */
#define M360_BD_CTS_REPORT  (1<<11)  /* SCC UART */
#define M360_BD_TX_CRC    (1<<10)  /* Ethernet */
#define M360_BD_DEFER    (1<<9)  /* Ethernet */
#define M360_BD_HEARTBEAT  (1<<8)  /* Ethernet */
#define M360_BD_PREAMBLE  (1<<8)  /* SCC UART, SMC UART */
#define M360_BD_LATE_COLLISION  (1<<7)  /* Ethernet */
#define M360_BD_NO_STOP_BIT  (1<<7)  /* SCC UART */
#define M360_BD_RETRY_LIMIT  (1<<6)  /* Ethernet */
#define M360_BD_RETRY_COUNT(x)  (((x)&0x3C)>>2)  /* Ethernet */
#define M360_BD_UNDERRUN  (1<<1)  /* Ethernet, SPI */
#define M360_BD_CARRIER_LOST  (1<<0)  /* Ethernet */
#define M360_BD_CTS_LOST  (1<<0)  /* SCC UART */

/*
 *************************************************************************
 *                           IDMA Buffer Descriptors                     *
 *************************************************************************
 */
typedef struct m360IDMABufferDescriptor_ {
  uint16_t          status;
  uint16_t          _pad;
  uint32_t         length;
  void      *source;
  void      *destination;
} m360IDMABufferDescriptor_t;

/*
 *************************************************************************
 *       RISC Communication Processor Module Command Register (CR)       *
 *************************************************************************
 */
#define M360_CR_RST    (1<<15)  /* Reset communication processor */
#define M360_CR_OP_INIT_RX_TX  (0<<8)  /* SCC, SMC UART, SMC GCI, SPI */
#define M360_CR_OP_INIT_RX  (1<<8)  /* SCC, SMC UART, SPI */
#define M360_CR_OP_INIT_TX  (2<<8)  /* SCC, SMC UART, SPI */
#define M360_CR_OP_INIT_HUNT  (3<<8)  /* SCC, SMC UART */
#define M360_CR_OP_STOP_TX  (4<<8)  /* SCC, SMC UART */
#define M360_CR_OP_GR_STOP_TX  (5<<8)  /* SCC */
#define M360_CR_OP_INIT_IDMA  (5<<8)  /* IDMA */
#define M360_CR_OP_RESTART_TX  (6<<8)  /* SCC, SMC UART */
#define M360_CR_OP_CLOSE_RX_BD  (7<<8)  /* SCC, SMC UART, SPI */
#define M360_CR_OP_SET_GRP_ADDR  (8<<8)  /* SCC */
#define M360_CR_OP_SET_TIMER  (8<<8)  /* Timer */
#define M360_CR_OP_GCI_TIMEOUT  (9<<8)  /* SMC GCI */
#define M360_CR_OP_RESERT_BCS  (10<<8)  /* SCC */
#define M360_CR_OP_GCI_ABORT  (10<<8)  /* SMC GCI */
#define M360_CR_CHAN_SCC1  (0<<4)  /* Channel selection */
#define M360_CR_CHAN_SCC2  (4<<4)
#define M360_CR_CHAN_SPI  (5<<4)
#define M360_CR_CHAN_TIMER  (5<<4)
#define M360_CR_CHAN_SCC3  (8<<4)
#define M360_CR_CHAN_SMC1  (9<<4)
#define M360_CR_CHAN_IDMA1  (9<<4)
#define M360_CR_CHAN_SCC4  (12<<4)
#define M360_CR_CHAN_SMC2  (13<<4)
#define M360_CR_CHAN_IDMA2  (13<<4)
#define M360_CR_FLG    (1<<0)  /* Command flag */

/*
 *************************************************************************
 *                 System Protection Control Register (SYPCR)            *
 *************************************************************************
 */
#define M360_SYPCR_SWE    (1<<7)  /* Software watchdog enable */
#define M360_SYPCR_SWRI    (1<<6)  /* Software watchdog reset select */
#define M360_SYPCR_SWT1    (1<<5)  /* Software watchdog timing bit 1 */
#define M360_SYPCR_SWT0    (1<<4)  /* Software watchdog timing bit 0 */
#define M360_SYPCR_DBFE    (1<<3)  /* Double bus fault monitor enable */
#define M360_SYPCR_BME    (1<<2)  /* Bus monitor external enable */
#define M360_SYPCR_BMT1    (1<<1)  /* Bus monitor timing bit 1 */
#define M360_SYPCR_BMT0    (1<<0)  /* Bus monitor timing bit 0 */

/*
 *************************************************************************
 *                        Memory Control Registers                       *
 *************************************************************************
 */
#define M360_GMR_RCNT(x)  ((x)<<24)  /* Refresh count */
#define M360_GMR_RFEN    (1<<23)  /* Refresh enable */
#define M360_GMR_RCYC(x)  ((x)<<21)  /* Refresh cycle length */
#define M360_GMR_PGS(x)    ((x)<<18)  /* Page size */
#define M360_GMR_DPS_32BIT  (0<<16)  /* DRAM port size */
#define M360_GMR_DPS_16BIT  (1<<16)
#define M360_GMR_DPS_8BIT  (2<<16)
#define M360_GMR_DPS_DSACK  (3<<16)
#define M360_GMR_WBT40    (1<<15)  /* Wait between 040 transfers */
#define M360_GMR_WBTQ    (1<<14)  /* Wait between 360 transfers */
#define M360_GMR_SYNC    (1<<13)  /* Synchronous external access */
#define M360_GMR_EMWS    (1<<12)  /* External master wait state */
#define M360_GMR_OPAR    (1<<11)  /* Odd parity */
#define M360_GMR_PBEE    (1<<10)  /* Parity bus error enable */
#define M360_GMR_TSS40    (1<<9)  /* TS* sample for 040 */
#define M360_GMR_NCS    (1<<8)  /* No CPU space */
#define M360_GMR_DWQ    (1<<7)  /* Delay write for 360 */
#define M360_GMR_DW40    (1<<6)  /* Delay write for 040 */
#define M360_GMR_GAMX    (1<<5)  /* Global address mux enable */

#define M360_MEMC_BR_FC(x)  ((x)<<7)  /* Function code limit */
#define M360_MEMC_BR_TRLXQ  (1<<6)  /* Relax timing requirements */
#define M360_MEMC_BR_BACK40  (1<<5)  /* Burst acknowledge to 040 */
#define M360_MEMC_BR_CSNT40  (1<<4)  /* CS* negate timing for 040 */
#define M360_MEMC_BR_CSNTQ  (1<<3)  /* CS* negate timing for 360 */
#define M360_MEMC_BR_PAREN  (1<<2)  /* Enable parity checking */
#define M360_MEMC_BR_WP    (1<<1)  /* Write Protect */
#define M360_MEMC_BR_V    (1<<0)  /* Base/Option register are valid */

#define M360_MEMC_OR_TCYC(x)  ((x)<<28)  /* Cycle length (clocks) */
#define M360_MEMC_OR_WAITS(x)  M360_MEMC_OR_TCYC((x)+1)
#define M360_MEMC_OR_2KB  0x0FFFF800  /* Address range */
#define M360_MEMC_OR_4KB  0x0FFFF000
#define M360_MEMC_OR_8KB  0x0FFFE000
#define M360_MEMC_OR_16KB  0x0FFFC000
#define M360_MEMC_OR_32KB  0x0FFF8000
#define M360_MEMC_OR_64KB  0x0FFF0000
#define M360_MEMC_OR_128KB  0x0FFE0000
#define M360_MEMC_OR_256KB  0x0FFC0000
#define M360_MEMC_OR_512KB  0x0FF80000
#define M360_MEMC_OR_1MB  0x0FF00000
#define M360_MEMC_OR_2MB  0x0FE00000
#define M360_MEMC_OR_4MB  0x0FC00000
#define M360_MEMC_OR_8MB  0x0F800000
#define M360_MEMC_OR_16MB  0x0F000000
#define M360_MEMC_OR_32MB  0x0E000000
#define M360_MEMC_OR_64MB  0x0C000000
#define M360_MEMC_OR_128MB  0x08000000
#define M360_MEMC_OR_256MB  0x00000000
#define M360_MEMC_OR_FCMC(x)  ((x)<<7)  /* Function code mask */
#define M360_MEMC_OR_BCYC(x)  ((x)<<5)  /* Burst cycle length (clocks) */
#define M360_MEMC_OR_PGME  (1<<3)    /* Page mode enable */
#define M360_MEMC_OR_32BIT  (0<<1)    /* Port size */
#define M360_MEMC_OR_16BIT  (1<<1)
#define M360_MEMC_OR_8BIT  (2<<1)
#define M360_MEMC_OR_DSACK  (3<<1)
#define M360_MEMC_OR_DRAM  (1<<0)    /* Dynamic RAM select */

/*
 *************************************************************************
 *                         SI Mode Register (SIMODE)                     *
 *************************************************************************
 */
#define M360_SI_SMC2_BITS  0xFFFF0000  /* All SMC2 bits */
#define M360_SI_SMC2_TDM  (1<<31)  /* Multiplexed SMC2 */
#define M360_SI_SMC2_BRG1  (0<<28)  /* SMC2 clock souce */
#define M360_SI_SMC2_BRG2  (1<<28)
#define M360_SI_SMC2_BRG3  (2<<28)
#define M360_SI_SMC2_BRG4  (3<<28)
#define M360_SI_SMC2_CLK5  (0<<28)
#define M360_SI_SMC2_CLK6  (1<<28)
#define M360_SI_SMC2_CLK7  (2<<28)
#define M360_SI_SMC2_CLK8  (3<<28)
#define M360_SI_SMC1_BITS  0x0000FFFF  /* All SMC1 bits */
#define M360_SI_SMC1_TDM  (1<<15)  /* Multiplexed SMC1 */
#define M360_SI_SMC1_BRG1  (0<<12)  /* SMC1 clock souce */
#define M360_SI_SMC1_BRG2  (1<<12)
#define M360_SI_SMC1_BRG3  (2<<12)
#define M360_SI_SMC1_BRG4  (3<<12)
#define M360_SI_SMC1_CLK1  (0<<12)
#define M360_SI_SMC1_CLK2  (1<<12)
#define M360_SI_SMC1_CLK3  (2<<12)
#define M360_SI_SMC1_CLK4  (3<<12)

/*
 *************************************************************************
 *                  SDMA Configuration Register (SDMA)                   *
 *************************************************************************
 */
#define M360_SDMA_FREEZE  (2<<13)  /* Freeze on next bus cycle */
#define M360_SDMA_SISM_7  (7<<8)  /* Normal interrupt service mask */
#define M360_SDMA_SAID_4  (4<<4)  /* Normal arbitration ID */
#define M360_SDMA_INTE    (1<<1)  /* SBER interrupt enable */
#define M360_SDMA_INTB    (1<<0)  /* SBKP interrupt enable */

/*
 *************************************************************************
 *                      Baud (sic) Rate Generators                       *
 *************************************************************************
 */
#define M360_BRG_RST    (1<<17)    /* Reset generator */
#define M360_BRG_EN    (1<<16)    /* Enable generator */
#define M360_BRG_EXTC_BRGCLK  (0<<14)    /* Source is BRGCLK */
#define M360_BRG_EXTC_CLK2  (1<<14)    /* Source is CLK2 pin */
#define M360_BRG_EXTC_CLK6  (2<<14)    /* Source is CLK6 pin */
#define M360_BRG_ATB    (1<<13)    /* Autobaud */
#define M360_BRG_115200    (13<<1)    /* Assume 25 MHz clock */
#define M360_BRG_57600    (26<<1)
#define M360_BRG_38400    (40<<1)
#define M360_BRG_19200    (80<<1)
#define M360_BRG_9600    (162<<1)
#define M360_BRG_4800    (324<<1)
#define M360_BRG_2400    (650<<1)
#define M360_BRG_1200    (1301<<1)
#define M360_BRG_600    (2603<<1)
#define M360_BRG_300    ((324<<1) | 1)
#define M360_BRG_150    ((650<<1) | 1)
#define M360_BRG_75    ((1301<<1) | 1)

/*
 *************************************************************************
 *                sccm  Bit Settings                                     *
 *************************************************************************
 */
#define M360_SCCE_TX              0x02
#define M360_SCCE_RX              0x01

#define M360_CR_INIT_TX_RX_PARAMS  0x0000
#define M360_CR_CH_NUM             0x0040

#define M360_NUM_DPRAM_REAGONS     4
/*
 *************************************************************************
 *                 MC68360 DUAL-PORT RAM AND REGISTERS                   *
 *************************************************************************
 */
typedef struct m360_ {
  /*
   * Dual-port RAM
   */
  volatile uint8_t          dpram0[0x400];  /* Microcode program */
  volatile uint8_t          dpram1[0x200];
  volatile uint8_t          dpram2[0x100];  /* Microcode scratch */
  volatile uint8_t          dpram3[0x100];  /* Not on REV A or B masks */
  volatile uint8_t          _rsv0[0xC00-0x800];
  volatile m360SCCENparms_t  scc1p;
  volatile uint8_t          _rsv1[0xCB0-0xC00-sizeof(m360SCCENparms_t)];
  volatile m360MiscParms_t  miscp;
  volatile uint8_t          _rsv2[0xD00-0xCB0-sizeof(m360MiscParms_t)];
  volatile m360SCCparms_t    scc2p;
  volatile uint8_t          _rsv3[0xD80-0xD00-sizeof(m360SCCparms_t)];
  volatile m360SPIparms_t    spip;
  volatile uint8_t          _rsv4[0xDB0-0xD80-sizeof(m360SPIparms_t)];
  volatile m360TimerParms_t  tmp;
  volatile uint8_t          _rsv5[0xE00-0xDB0-sizeof(m360TimerParms_t)];
  volatile m360SCCparms_t    scc3p;
  volatile uint8_t          _rsv6[0xE70-0xE00-sizeof(m360SCCparms_t)];
  volatile m360IDMAparms_t  idma1p;
  volatile uint8_t          _rsv7[0xE80-0xE70-sizeof(m360IDMAparms_t)];
  volatile m360SMCparms_t    smc1p;
  volatile uint8_t          _rsv8[0xF00-0xE80-sizeof(m360SMCparms_t)];
  volatile m360SCCparms_t    scc4p;
  volatile uint8_t          _rsv9[0xF70-0xF00-sizeof(m360SCCparms_t)];
  volatile m360IDMAparms_t  idma2p;
  volatile uint8_t          _rsv10[0xF80-0xF70-sizeof(m360IDMAparms_t)];
  volatile m360SMCparms_t    smc2p;
  volatile uint8_t          _rsv11[0x1000-0xF80-sizeof(m360SMCparms_t)];

  /*
   * SIM Block
   */
  volatile uint32_t         mcr;
  volatile uint32_t         _pad00;
  volatile uint8_t          avr;
  volatile uint8_t          rsr;
  volatile uint16_t          _pad01;
  volatile uint8_t          clkocr;
  volatile uint8_t          _pad02;
  volatile uint16_t          _pad03;
  volatile uint16_t          pllcr;
  volatile uint16_t          _pad04;
  volatile uint16_t          cdvcr;
  volatile uint16_t          pepar;
  volatile uint32_t         _pad05[2];
  volatile uint16_t          _pad06;
  volatile uint8_t          sypcr;
  volatile uint8_t          swiv;
  volatile uint16_t          _pad07;
  volatile uint16_t          picr;
  volatile uint16_t          _pad08;
  volatile uint16_t          pitr;
  volatile uint16_t          _pad09;
  volatile uint8_t          _pad10;
  volatile uint8_t          swsr;
  volatile uint32_t         bkar;
  volatile uint32_t         bcar;
  volatile uint32_t         _pad11[2];

  /*
   * MEMC Block
   */
  volatile uint32_t         gmr;
  volatile uint16_t          mstat;
  volatile uint16_t          _pad12;
  volatile uint32_t         _pad13[2];
  volatile m360MEMCRegisters_t  memc[8];
  volatile uint8_t          _pad14[0xF0-0xD0];
  volatile uint8_t          _pad15[0x100-0xF0];
  volatile uint8_t          _pad16[0x500-0x100];

  /*
   * IDMA1 Block
   */
  volatile uint16_t          iccr;
  volatile uint16_t          _pad17;
  volatile uint16_t          cmr1;
  volatile uint16_t          _pad18;
  volatile uint32_t         sapr1;
  volatile uint32_t         dapr1;
  volatile uint32_t         bcr1;
  volatile uint8_t          fcr1;
  volatile uint8_t          _pad19;
  volatile uint8_t          cmar1;
  volatile uint8_t          _pad20;
  volatile uint8_t          csr1;
  volatile uint8_t          _pad21;
  volatile uint16_t          _pad22;

  /*
   * SDMA Block
   */
  volatile uint8_t          sdsr;
  volatile uint8_t          _pad23;
  volatile uint16_t          sdcr;
  volatile uint32_t         sdar;

  /*
   * IDMA2 Block
   */
  volatile uint16_t          _pad24;
  volatile uint16_t          cmr2;
  volatile uint32_t         sapr2;
  volatile uint32_t         dapr2;
  volatile uint32_t         bcr2;
  volatile uint8_t          fcr2;
  volatile uint8_t          _pad26;
  volatile uint8_t          cmar2;
  volatile uint8_t          _pad27;
  volatile uint8_t          csr2;
  volatile uint8_t          _pad28;
  volatile uint16_t          _pad29;
  volatile uint32_t         _pad30;

  /*
   * CPIC Block
   */
  volatile uint32_t         cicr;
  volatile uint32_t         cipr;
  volatile uint32_t         cimr;
  volatile uint32_t         cisr;

  /*
   * Parallel I/O Block
   */
  volatile uint16_t          padir;
  volatile uint16_t          papar;
  volatile uint16_t          paodr;
  volatile uint16_t          padat;
  volatile uint32_t         _pad31[2];
  volatile uint16_t          pcdir;
  volatile uint16_t          pcpar;
  volatile uint16_t          pcso;
  volatile uint16_t          pcdat;
  volatile uint16_t          pcint;
  volatile uint16_t          _pad32;
  volatile uint32_t         _pad33[5];

  /*
   * TIMER Block
   */
  volatile uint16_t          tgcr;
  volatile uint16_t          _pad34;
  volatile uint32_t         _pad35[3];
  volatile uint16_t          tmr1;
  volatile uint16_t          tmr2;
  volatile uint16_t          trr1;
  volatile uint16_t          trr2;
  volatile uint16_t          tcr1;
  volatile uint16_t          tcr2;
  volatile uint16_t          tcn1;
  volatile uint16_t          tcn2;
  volatile uint16_t          tmr3;
  volatile uint16_t          tmr4;
  volatile uint16_t          trr3;
  volatile uint16_t          trr4;
  volatile uint16_t          tcr3;
  volatile uint16_t          tcr4;
  volatile uint16_t          tcn3;
  volatile uint16_t          tcn4;
  volatile uint16_t          ter1;
  volatile uint16_t          ter2;
  volatile uint16_t          ter3;
  volatile uint16_t          ter4;
  volatile uint32_t         _pad36[2];

  /*
   * CP Block
   */
  volatile uint16_t          cr;
  volatile uint16_t          _pad37;
  volatile uint16_t          rccr;
  volatile uint16_t          _pad38;
  volatile uint32_t         _pad39[3];
  volatile uint16_t          _pad40;
  volatile uint16_t          rter;
  volatile uint16_t          _pad41;
  volatile uint16_t          rtmr;
  volatile uint32_t         _pad42[5];

  /*
   * BRG Block
   */
  volatile uint32_t         brgc1;
  volatile uint32_t         brgc2;
  volatile uint32_t         brgc3;
  volatile uint32_t         brgc4;

  /*
   * SCC Block
   */
  volatile m360SCCRegisters_t  scc1;
  volatile m360SCCRegisters_t  scc2;
  volatile m360SCCRegisters_t  scc3;
  volatile m360SCCRegisters_t  scc4;

  /*
   * SMC Block
   */
  volatile m360SMCRegisters_t  smc1;
  volatile m360SMCRegisters_t  smc2;

  /*
   * SPI Block
   */
  volatile uint16_t          spmode;
  volatile uint16_t          _pad43[2];
  volatile uint8_t          spie;
  volatile uint8_t          _pad44;
  volatile uint16_t          _pad45;
  volatile uint8_t          spim;
  volatile uint8_t          _pad46[2];
  volatile uint8_t          spcom;
  volatile uint16_t          _pad47[2];

  /*
   * PIP Block
   */
  volatile uint16_t          pipc;
  volatile uint16_t          _pad48;
  volatile uint16_t          ptpr;
  volatile uint32_t         pbdir;
  volatile uint32_t         pbpar;
  volatile uint16_t          _pad49;
  volatile uint16_t          pbodr;
  volatile uint32_t         pbdat;
  volatile uint32_t         _pad50[6];

  /*
   * SI Block
   */
  volatile uint32_t         simode;
  volatile uint8_t          sigmr;
  volatile uint8_t          _pad51;
  volatile uint8_t          sistr;
  volatile uint8_t          sicmr;
  volatile uint32_t         _pad52;
  volatile uint32_t         sicr;
  volatile uint16_t          _pad53;
  volatile uint16_t          sirp[2];
  volatile uint16_t          _pad54;
  volatile uint32_t         _pad55[2];
  volatile uint8_t          siram[256];
} m360_t;

struct bdregions_t {
  char            *base;
  unsigned int    size;
  unsigned int    used;
};

#define M68360_RX_BUF_SIZE        1
#define M68360_TX_BUF_SIZE        0x100

struct _m68360_per_chip;
typedef struct _m68360_per_chip *M68360_t;

typedef struct _m68360_per_port {
  uint32_t                         channel;
  M68360_t                         chip;
  volatile uint32_t               *pBRGC;      /* m360->brgc#             */
  volatile m360SCCparms_t         *pSCCB;      /* m360->scc#p             */
  volatile m360SCCRegisters_t     *pSCCR;      /* m360->scc#              */
  uint32_t                         baud;
  int                              minor;
  volatile uint8_t                *rxBuf;
  volatile uint8_t                *txBuf;
  volatile m360BufferDescriptor_t *sccRxBd;
  volatile m360BufferDescriptor_t *sccTxBd;
}m68360_per_port_t, *M68360_serial_ports_t;

typedef struct _m68360_per_chip {
  struct _m68360_per_chip        *next;
  struct bdregions_t              bdregions[4];
  volatile m360_t                *m360;       /* Pointer to base Address */
  int                             m360_interrupt;
  int                             m360_clock_rate;
  PPMCQ1BoardData                 board_data;
  m68360_per_port_t               port[4];
} m68360_per_chip_t;

extern M68360_t M68360_chips;

void M360SetupMemory( M68360_t ptr );
void *M360AllocateBufferDescriptors (M68360_t ptr, int count);
void M360ExecuteRISC( volatile m360_t *m360, uint16_t command);
int mc68360_scc_create_chip( PPMCQ1BoardData BoardData, uint8_t int_vector );

#if 0
extern volatile m360_t *m360;
#endif

#endif /* __MC68360_h */
