/*
**************************************************************************
**************************************************************************
**                                                                      **
**       MOTOROLA MPC860/MPC821 PORTABLE SYSTEMS MICROPROCESSOR         **
**                                                                      **
**                        HARDWARE DECLARATIONS                         **
**                                                                      **
**                                                                      **
**  Submitted By:                                                       **
**                                                                      **
**      W. Eric Norum                                                   **
**      Saskatchewan Accelerator Laboratory                             **
**      University of Saskatchewan                                      **
**      107 North Road                                                  **
**      Saskatoon, Saskatchewan, CANADA                                 **
**      S7N 5C6                                                         **
**                                                                      **
**      eric@skatter.usask.ca                                           **
**                                                                      **
**  Modified for use with the MPC860 (original code was for MC68360)    **
**  by                                                                  **
**      Jay Monkman                                                     **
**      Frasca International, Inc.                                      **
**      906 E. Airport Rd.                                              **
**      Urbana, IL, 61801                                               **
**                                                                      **
**      jmonkman@frasca.com                                             **
**                                                                      **
**  Modified further for use with the MPC821 by:                        **
**      Andrew Bray <andy@chaos.org.uk>                                 **
**                                                                      **
**  With some corrections/additions by:                                 **
**      Darlene A. Stewart and                                          **
**      Charles-Antoine Gauthier                                        **
**      Institute for Information Technology                            **
**      National Research Council of Canada                             **
**      Ottawa, ON  K1A 0R6                                             **
**                                                                      **
**      Darlene.Stewart@iit.nrc.ca                                      **
**      charles.gauthier@iit.nrc.ca                                     **
**                                                                      **
**      Corrections/additions:                                          **
**        Copyright (c) 1999, National Research Council of Canada       **
**************************************************************************
**************************************************************************
*/

#ifndef __MPC8xx_h
#define __MPC8xx_h

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Macros for accessing Special Purpose Registers (SPRs)
 */
#define _mtspr(_spr,_reg)   __asm__ volatile ( "mtspr %0, %1\n" : : "i" ((_spr)), "r" ((_reg)) )
#define _mfspr(_reg,_spr)   __asm__ volatile ( "mfspr %0, %1\n" : "=r" ((_reg)) : "i" ((_spr)) )

#define _isync   __asm__ volatile ("isync\n"::)

/*
 * Core Registers (SPRs)
 */
#define M8xx_DEC        22      /* Decrementer Register */
#define M8xx_DER        149     /* Debug Enable Register */
#define M8xx_ICTRL      158     /* Instruction Support Control Register */
#define M8xx_TBL_WR     284     /* Timebase Lower Write Register */
#define M8xx_TBU_WR     285     /* Timebase Upper Write Register */
#define M8xx_IMMR       638     /* Internal Memory Map Register */

/*
 * Cache Control Registers (SPRs)
 */
#define M8xx_IC_CST     560     /* Instruction Cache Control and Status Register */
#define M8xx_DC_CST     568     /* Data Cache Control and Status Register */
#define M8xx_IC_ADR     561     /* Instruction Cache Address Register */
#define M8xx_DC_ADR     569     /* Data Cache Address Register */
#define M8xx_IC_DAT     562     /* Instruction Cache Data Port Register */
#define M8xx_DC_DAT     570     /* Data Cache Data Port Register */

/*
 * MMU Registers (SPRs)
 */
/* Control Registers */
#define M8xx_MI_CTR     784     /* IMMU Control Register */
#define M8xx_MD_CTR     792     /* DMMU Control Register */
/* TLB Source Registers */
#define M8xx_MI_EPN     787     /* IMMU Effective Page Number Register (EPN) */
#define M8xx_MD_EPN     795     /* DMMU Effective Page Number Register (EPN) */
#define M8xx_MI_TWC     789     /* IMMU Tablewalk Control Register (TWC) */
#define M8xx_MD_TWC     797     /* DMMU Tablewalk Control Register (TWC) */
#define M8xx_MI_RPN     790     /* IMMU Real (physical) Page Number Register (RPN) */
#define M8xx_MD_RPN     798     /* DMMU Real (physical) Page Number Register (RPN) */
/* Tablewalk Assist Registers */
#define M8xx_M_TWB      796     /* MMU Tablewalk Base Register (TWB) */
/* Protection Registers */
#define M8xx_M_CASID    793     /* MMU Current Address Space ID Register */
#define M8xx_MI_AP      786     /* IMMU Access Protection Register */
#define M8xx_MD_AP      794     /* DMMU Access Protection Register */
/* Scratch Register */
#define M8xx_M_TW       799     /* MMU Tablewalk Special Register */
/* Debug Registers */
#define M8xx_MI_CAM     816     /* IMMU CAM Entry Read Register */
#define M8xx_MI_RAM0    817     /* IMMU RAM Entry Read Register 0 */
#define M8xx_MI_RAM1    818     /* IMMU RAM Entry Read Register 1 */
#define M8xx_MD_CAM     824     /* DMMU CAM Entry Read Register */
#define M8xx_MD_RAM0    825     /* DMMU RAM Entry Read Register 0 */
#define M8xx_MD_RAM1    826     /* DMMU RAM Entry Read Register 1 */

#define M8xx_MI_CTR_GPM       (1<<31)
#define M8xx_MI_CTR_PPM       (1<<30)
#define M8xx_MI_CTR_CIDEF     (1<<29)
#define M8xx_MI_CTR_RSV4I     (1<<27)
#define M8xx_MI_CTR_PPCS      (1<<25)
#define M8xx_MI_CTR_ITLB_INDX(x) ((x)<<8)   /* ITLB index */

#define M8xx_MD_CTR_GPM       (1<<31)
#define M8xx_MD_CTR_PPM       (1<<30)
#define M8xx_MD_CTR_CIDEF     (1<<29)
#define M8xx_MD_CTR_WTDEF     (1<<28)
#define M8xx_MD_CTR_RSV4D     (1<<27)
#define M8xx_MD_CTR_TWAM      (1<<26)
#define M8xx_MD_CTR_PPCS      (1<<25)
#define M8xx_MD_CTR_DTLB_INDX(x) ((x)<<8)   /* DTLB index */

#define M8xx_MI_EPN_VALID     (1<<9)

#define M8xx_MD_EPN_VALID     (1<<9)

#define M8xx_MI_TWC_G         (1<<4)
#define M8xx_MI_TWC_PSS       (0<<2)
#define M8xx_MI_TWC_PS512     (1<<2)
#define M8xx_MI_TWC_PS8       (3<<2)
#define M8xx_MI_TWC_VALID     (1)

#define M8xx_MD_TWC_G         (1<<4)
#define M8xx_MD_TWC_PSS       (0<<2)
#define M8xx_MD_TWC_PS512     (1<<2)
#define M8xx_MD_TWC_PS8       (3<<2)
#define M8xx_MD_TWC_WT        (1<<1)
#define M8xx_MD_TWC_VALID     (1)

#define M8xx_MI_RPN_F         (0xf<<4)
#define M8xx_MI_RPN_16K       (1<<3)
#define M8xx_MI_RPN_SHARED    (1<<2)
#define M8xx_MI_RPN_CI        (1<<1)
#define M8xx_MI_RPN_VALID     (1)

#define M8xx_MD_RPN_CHANGE    (1<<8)
#define M8xx_MD_RPN_F         (0xf<<4)
#define M8xx_MD_RPN_16K       (1<<3)
#define M8xx_MD_RPN_SHARED    (1<<2)
#define M8xx_MD_RPN_CI        (1<<1)
#define M8xx_MD_RPN_VALID     (1)

#define M8xx_MI_AP_Kp         (1)

#define M8xx_MD_AP_Kp         (1)

#define M8xx_CACHE_CMD_SFWT          (0x1<<24)
#define M8xx_CACHE_CMD_ENABLE        (0x2<<24)
#define M8xx_CACHE_CMD_CFWT          (0x3<<24)
#define M8xx_CACHE_CMD_DISABLE       (0x4<<24)
#define M8xx_CACHE_CMD_STLES         (0x5<<24)
#define M8xx_CACHE_CMD_LLCB          (0x6<<24)
#define M8xx_CACHE_CMD_CLES          (0x7<<24)
#define M8xx_CACHE_CMD_UNLOCK        (0x8<<24)
#define M8xx_CACHE_CMD_UNLOCKALL     (0xa<<24)
#define M8xx_CACHE_CMD_INVALIDATE    (0xc<<24)
#define M8xx_CACHE_CMD_FLUSH         (0xe<<24)

/*
*************************************************************************
*                         REGISTER SUBBLOCKS                            *
*************************************************************************
*/

/*
 * Memory controller registers
 */
typedef struct m8xxMEMCRegisters_ {
  rtems_unsigned32        _br;
  rtems_unsigned32        _or;    /* Used to be called 'or'; reserved ANSI C++ keyword */
} m8xxMEMCRegisters_t;

/*
 * Serial Communications Controller registers
 */
typedef struct m8xxSCCRegisters_ {
  rtems_unsigned32        gsmr_l;
  rtems_unsigned32        gsmr_h;
  rtems_unsigned16        psmr;
  rtems_unsigned16        _pad0;
  rtems_unsigned16        todr;
  rtems_unsigned16        dsr;
  rtems_unsigned16        scce;
  rtems_unsigned16        _pad1;
  rtems_unsigned16        sccm;
  rtems_unsigned8         _pad2;
  rtems_unsigned8         sccs;
  rtems_unsigned32        _pad3[2];
} m8xxSCCRegisters_t;

/*
 * Serial Management Controller registers
 */
typedef struct m8xxSMCRegisters_ {
  rtems_unsigned16        _pad0;
  rtems_unsigned16        smcmr;
  rtems_unsigned16        _pad1;
  rtems_unsigned8         smce;
  rtems_unsigned8         _pad2;
  rtems_unsigned16        _pad3;
  rtems_unsigned8         smcm;
  rtems_unsigned8         _pad4;
  rtems_unsigned32        _pad5;
} m8xxSMCRegisters_t;

/*
 * Fast Ethernet Controller registers (Only on MPC8xxT)
 */
typedef struct m8xxFECRegisters_ {
  rtems_unsigned32    addr_low;
  rtems_unsigned32    addr_high;
  rtems_unsigned32    hash_table_high;
  rtems_unsigned32    hash_table_low;
  rtems_unsigned32    r_des_start;
  rtems_unsigned32    x_des_start;
  rtems_unsigned32    r_buf_size;
  rtems_unsigned32    _pad0[9];
  rtems_unsigned32    ecntrl;
  rtems_unsigned32    ievent;
  rtems_unsigned32    imask;
  rtems_unsigned32    ivec;
  rtems_unsigned32    r_des_active;
  rtems_unsigned32    x_des_active;
  rtems_unsigned32    _pad1[10];
  rtems_unsigned32    mii_data;
  rtems_unsigned32    mii_speed;
  rtems_unsigned32    _pad2[17];
  rtems_unsigned32    r_bound;
  rtems_unsigned32    r_fstart;
  rtems_unsigned32    _pad3[6];
  rtems_unsigned32    x_fstart;
  rtems_unsigned32    _pad4[17];
  rtems_unsigned32    fun_code;
  rtems_unsigned32    _pad5[3];
  rtems_unsigned32    r_cntrl;
  rtems_unsigned32    r_hash;
  rtems_unsigned32    _pad6[14];
  rtems_unsigned32    x_cntrl;
  rtems_unsigned32    _pad7[30];

} m8xxFECRegisters_t;

#define M8xx_FEC_IEVENT_HBERR  (1 << 31)
#define M8xx_FEC_IEVENT_BABR   (1 << 30)
#define M8xx_FEC_IEVENT_BABT   (1 << 29)
#define M8xx_FEC_IEVENT_GRA    (1 << 28)
#define M8xx_FEC_IEVENT_TFINT  (1 << 27)
#define M8xx_FEC_IEVENT_TXB    (1 << 26)
#define M8xx_FEC_IEVENT_RFINT  (1 << 25)
#define M8xx_FEC_IEVENT_RXB    (1 << 24)
#define M8xx_FEC_IEVENT_MII    (1 << 23)
#define M8xx_FEC_IEVENT_EBERR  (1 << 22)
#define M8xx_FEC_IMASK_HBEEN   (1 << 31)
#define M8xx_FEC_IMASK_BREEN   (1 << 30)
#define M8xx_FEC_IMASK_BTEN    (1 << 29)
#define M8xx_FEC_IMASK_GRAEN   (1 << 28)
#define M8xx_FEC_IMASK_TFIEN   (1 << 27)
#define M8xx_FEC_IMASK_TBIEN   (1 << 26)
#define M8xx_FEC_IMASK_RFIEN   (1 << 25)
#define M8xx_FEC_IMASK_RBIEN   (1 << 24)
#define M8xx_FEC_IMASK_MIIEN   (1 << 23)
#define M8xx_FEC_IMASK_EBERREN (1 << 22)

/*
*************************************************************************
*                         Miscellaneous Parameters                      *
*************************************************************************
*/
typedef struct m8xxMiscParms_ {
  rtems_unsigned16        rev_num;
  rtems_unsigned16        _res1;
  rtems_unsigned32        _res2;
  rtems_unsigned32        _res3;
} m8xxMiscParms_t;

/*
*************************************************************************
*                              RISC Timers                              *
*************************************************************************
*/
typedef struct m8xxTimerParms_ {
  rtems_unsigned16        tm_base;
  rtems_unsigned16        _tm_ptr;
  rtems_unsigned16        _r_tmr;
  rtems_unsigned16        _r_tmv;
  rtems_unsigned32        tm_cmd;
  rtems_unsigned32        tm_cnt;
} m8xxTimerParms_t;

/*
 * RISC Controller Configuration Register (RCCR)
 * All other bits in this register are reserved.
 */
#define M8xx_RCCR_TIME          (1<<15)    /* Enable timer */
#define M8xx_RCCR_TIMEP(x)      ((x)<<8)   /* Timer period */
#define M8xx_RCCR_DR1M          (1<<7)     /* IDMA Rqst 1 Mode */
#define M8xx_RCCR_DR0M          (1<<6)     /* IDMA Rqst 0 Mode */
#define M8xx_RCCR_DRQP(x)       ((x)<<4)   /* IDMA Rqst Priority */
#define M8xx_RCCR_EIE           (1<<3)     /* External Interrupt Enable */
#define M8xx_RCCR_SCD           (1<<2)     /* Scheduler Configuration */
#define M8xx_RCCR_ERAM(x)       (x)        /* Enable RAM Microcode */

/*
 * Command register
 * Set up this register before issuing a M8xx_CR_OP_SET_TIMER command.
 */
#define M8xx_TM_CMD_V           (1<<31)         /* Set to enable timer */
#define M8xx_TM_CMD_R           (1<<30)         /* Set for automatic restart */
#define M8xx_TM_CMD_PWM         (1<<29)         /* Set for PWM operation */
#define M8xx_TM_CMD_TIMER(x)    ((x)<<16)       /* Select timer */
#define M8xx_TM_CMD_PERIOD(x)   (x)             /* Timer period (16 bits) */

/*
*************************************************************************
*                               DMA Controllers                         *
*************************************************************************
*/
typedef struct m8xxIDMAparms_ {
  rtems_unsigned16        ibase;
  rtems_unsigned16        dcmr;
  rtems_unsigned32        _sapr;
  rtems_unsigned32        _dapr;
  rtems_unsigned16        ibptr;
  rtems_unsigned16        _write_sp;
  rtems_unsigned32        _s_byte_c;
  rtems_unsigned32        _d_byte_c;
  rtems_unsigned32        _s_state;
  rtems_unsigned32        _itemp[4];
  rtems_unsigned32        _sr_mem;
  rtems_unsigned16        _read_sp;
  rtems_unsigned16        _res0;
  rtems_unsigned16        _res1;
  rtems_unsigned16        _res2;
  rtems_unsigned32        _d_state;
} m8xxIDMAparms_t;


/*
*************************************************************************
*                               DSP                                     *
*************************************************************************
*/
typedef struct m8xxDSPparms_ {
  rtems_unsigned32        fdbase;
  rtems_unsigned32        _fd_ptr;
  rtems_unsigned32        _dstate;
  rtems_unsigned32        _pad0;
  rtems_unsigned16        _dstatus;
  rtems_unsigned16        _i;
  rtems_unsigned16        _tap;
  rtems_unsigned16        _cbase;
  rtems_unsigned16        _pad1;
  rtems_unsigned16        _xptr;
  rtems_unsigned16        _pad2;
  rtems_unsigned16        _yptr;
  rtems_unsigned16        _m;
  rtems_unsigned16        _pad3;
  rtems_unsigned16        _n;
  rtems_unsigned16        _pad4;
  rtems_unsigned16        _k;
  rtems_unsigned16        _pad5;
} m8xxDSPparms_t;

/*
*************************************************************************
*                   Serial Communication Controllers                    *
*************************************************************************
*/
typedef struct m8xxSCCparms_ {
  rtems_unsigned16        rbase;
  rtems_unsigned16        tbase;
  rtems_unsigned8         rfcr;
  rtems_unsigned8         tfcr;
  rtems_unsigned16        mrblr;
  rtems_unsigned32        _rstate;
  rtems_unsigned32        _pad0;
  rtems_unsigned16        _rbptr;
  rtems_unsigned16        _pad1;
  rtems_unsigned32        _pad2;
  rtems_unsigned32        _tstate;
  rtems_unsigned32        _pad3;
  rtems_unsigned16        _tbptr;
  rtems_unsigned16        _pad4;
  rtems_unsigned32        _pad5;
  rtems_unsigned32        _rcrc;
  rtems_unsigned32        _tcrc;
  union {
    struct {
      rtems_unsigned32        _res0;
      rtems_unsigned32        _res1;
      rtems_unsigned16        max_idl;
      rtems_unsigned16        _idlc;
      rtems_unsigned16        brkcr;
      rtems_unsigned16        parec;
      rtems_unsigned16        frmec;
      rtems_unsigned16        nosec;
      rtems_unsigned16        brkec;
      rtems_unsigned16        brkln;
      rtems_unsigned16        uaddr[2];
      rtems_unsigned16        _rtemp;
      rtems_unsigned16        toseq;
      rtems_unsigned16        character[8];
      rtems_unsigned16        rccm;
      rtems_unsigned16        rccr;
      rtems_unsigned16        rlbc;
    } uart;
  } un;
} m8xxSCCparms_t;

typedef struct m8xxSCCENparms_ {
  rtems_unsigned16        rbase;
  rtems_unsigned16        tbase;
  rtems_unsigned8         rfcr;
  rtems_unsigned8         tfcr;
  rtems_unsigned16        mrblr;
  rtems_unsigned32        _rstate;
  rtems_unsigned32        _pad0;
  rtems_unsigned16        _rbptr;
  rtems_unsigned16        _pad1;
  rtems_unsigned32        _pad2;
  rtems_unsigned32        _tstate;
  rtems_unsigned32        _pad3;
  rtems_unsigned16        _tbptr;
  rtems_unsigned16        _pad4;
  rtems_unsigned32        _pad5;
  rtems_unsigned32        _rcrc;
  rtems_unsigned32        _tcrc;
  union {
    struct {
      rtems_unsigned32        _res0;
      rtems_unsigned32        _res1;
      rtems_unsigned16        max_idl;
      rtems_unsigned16        _idlc;
      rtems_unsigned16        brkcr;
      rtems_unsigned16        parec;
      rtems_unsigned16        frmec;
      rtems_unsigned16        nosec;
      rtems_unsigned16        brkec;
      rtems_unsigned16        brkln;
      rtems_unsigned16        uaddr[2];
      rtems_unsigned16        _rtemp;
      rtems_unsigned16        toseq;
      rtems_unsigned16        character[8];
      rtems_unsigned16        rccm;
      rtems_unsigned16        rccr;
      rtems_unsigned16        rlbc;
    } uart;
    struct {
      rtems_unsigned32        c_pres;
      rtems_unsigned32        c_mask;
      rtems_unsigned32        crcec;
      rtems_unsigned32        alec;
      rtems_unsigned32        disfc;
      rtems_unsigned16        pads;
      rtems_unsigned16        ret_lim;
      rtems_unsigned16        _ret_cnt;
      rtems_unsigned16        mflr;
      rtems_unsigned16        minflr;
      rtems_unsigned16        maxd1;
      rtems_unsigned16        maxd2;
      rtems_unsigned16        _maxd;
      rtems_unsigned16        dma_cnt;
      rtems_unsigned16        _max_b;
      rtems_unsigned16        gaddr1;
      rtems_unsigned16        gaddr2;
      rtems_unsigned16        gaddr3;
      rtems_unsigned16        gaddr4;
      rtems_unsigned32        _tbuf0data0;
      rtems_unsigned32        _tbuf0data1;
      rtems_unsigned32        _tbuf0rba0;
      rtems_unsigned32        _tbuf0crc;
      rtems_unsigned16        _tbuf0bcnt;
      rtems_unsigned16        paddr_h;
      rtems_unsigned16        paddr_m;
      rtems_unsigned16        paddr_l;
      rtems_unsigned16        p_per;
      rtems_unsigned16        _rfbd_ptr;
      rtems_unsigned16        _tfbd_ptr;
      rtems_unsigned16        _tlbd_ptr;
      rtems_unsigned32        _tbuf1data0;
      rtems_unsigned32        _tbuf1data1;
      rtems_unsigned32        _tbuf1rba0;
      rtems_unsigned32        _tbuf1crc;
      rtems_unsigned16        _tbuf1bcnt;
      rtems_unsigned16        _tx_len;
      rtems_unsigned16        iaddr1;
      rtems_unsigned16        iaddr2;
      rtems_unsigned16        iaddr3;
      rtems_unsigned16        iaddr4;
      rtems_unsigned16        _boff_cnt;
      rtems_unsigned16        taddr_l;
      rtems_unsigned16        taddr_m;
      rtems_unsigned16        taddr_h;
    } ethernet;
  } un;
} m8xxSCCENparms_t;

/*
 * Receive and transmit function code register bits
 * These apply to the function code registers of all devices, not just SCC.
 */
#define M8xx_RFCR_BO(x)         ((x)<<3)
#define M8xx_RFCR_MOT           (2<<3)
#define M8xx_RFCR_DMA_SPACE(x)  (x)
#define M8xx_TFCR_BO(x)         ((x)<<3)
#define M8xx_TFCR_MOT           (2<<3)
#define M8xx_TFCR_DMA_SPACE(x)  (x)

/*
 * Event and mask registers (SCCE, SCCM)
 */
#define M8xx_SCCE_BRKE  (1<<6)
#define M8xx_SCCE_BRK   (1<<4)
#define M8xx_SCCE_BSY   (1<<2)
#define M8xx_SCCE_TX    (1<<1)
#define M8xx_SCCE_RX    (1<<0)

/*
*************************************************************************
*                     Serial Management Controllers                     *
*************************************************************************
*/
typedef struct m8xxSMCparms_ {
  rtems_unsigned16        rbase;
  rtems_unsigned16        tbase;
  rtems_unsigned8         rfcr;
  rtems_unsigned8         tfcr;
  rtems_unsigned16        mrblr;
  rtems_unsigned32        _rstate;
  rtems_unsigned32        _pad0;
  rtems_unsigned16        _rbptr;
  rtems_unsigned16        _pad1;
  rtems_unsigned32        _pad2;
  rtems_unsigned32        _tstate;
  rtems_unsigned32        _pad3;
  rtems_unsigned16        _tbptr;
  rtems_unsigned16        _pad4;
  rtems_unsigned32        _pad5;
  union {
    struct {
      rtems_unsigned16        max_idl;
      rtems_unsigned16        _idlc;
      rtems_unsigned16        brkln;
      rtems_unsigned16        brkec;
      rtems_unsigned16        brkcr;
      rtems_unsigned16        _r_mask;
    } uart;
    struct {
      rtems_unsigned16        _pad0[5];
    } transparent;
  } un;
} m8xxSMCparms_t;

/*
 * Mode register
 */
#define M8xx_SMCMR_CLEN(x)              ((x)<<11)    /* Character length */
#define M8xx_SMCMR_2STOP                (1<<10)      /* 2 stop bits */
#define M8xx_SMCMR_PARITY               (1<<9)       /* Enable parity */
#define M8xx_SMCMR_EVEN                 (1<<8)       /* Even parity */
#define M8xx_SMCMR_SM_GCI               (0<<4)       /* GCI Mode */
#define M8xx_SMCMR_SM_UART              (2<<4)       /* UART Mode */
#define M8xx_SMCMR_SM_TRANSPARENT       (3<<4)       /* Transparent Mode */
#define M8xx_SMCMR_DM_LOOPBACK          (1<<2)       /* Local loopback mode */
#define M8xx_SMCMR_DM_ECHO              (2<<2)       /* Echo mode */
#define M8xx_SMCMR_TEN                  (1<<1)       /* Enable transmitter */
#define M8xx_SMCMR_REN                  (1<<0)       /* Enable receiver */

/*
 * Event and mask registers (SMCE, SMCM)
 */
#define M8xx_SMCE_BRKE  (1<<6)
#define M8xx_SMCE_BRK   (1<<4)
#define M8xx_SMCE_BSY   (1<<2)
#define M8xx_SMCE_TX    (1<<1)
#define M8xx_SMCE_RX    (1<<0)

/*
*************************************************************************
*                      Serial Peripheral Interface                      *
*************************************************************************
*/
typedef struct m8xxSPIparms_ {
  rtems_unsigned16        rbase;
  rtems_unsigned16        tbase;
  rtems_unsigned8         rfcr;
  rtems_unsigned8         tfcr;
  rtems_unsigned16        mrblr;
  rtems_unsigned32        _rstate;
  rtems_unsigned32        _pad0;
  rtems_unsigned16        _rbptr;
  rtems_unsigned16        _pad1;
  rtems_unsigned32        _pad2;
  rtems_unsigned32        _tstate;
  rtems_unsigned32        _pad3;
  rtems_unsigned16        _tbptr;
  rtems_unsigned16        _pad4;
  rtems_unsigned32        _pad5;
} m8xxSPIparms_t;

/*
 * Mode register (SPMODE)
 */
#define M8xx_SPMODE_LOOP                (1<<14) /* Local loopback mode */
#define M8xx_SPMODE_CI                  (1<<13) /* Clock invert */
#define M8xx_SPMODE_CP                  (1<<12) /* Clock phase */
#define M8xx_SPMODE_DIV16               (1<<11) /* Divide BRGCLK by 16 */
#define M8xx_SPMODE_REV                 (1<<10) /* Reverse data */
#define M8xx_SPMODE_MASTER              (1<<9)  /* SPI is master */
#define M8xx_SPMODE_EN                  (1<<8)  /* Enable SPI */
#define M8xx_SPMODE_CLEN(x)             ((x)<<4)        /* Character length */
#define M8xx_SPMODE_PM(x)               (x)     /* Prescaler modulus */

/*
 * Mode register (SPCOM)
 */
#define M8xx_SPCOM_STR                  (1<<7)  /* Start transmit */

/*
 * Event and mask registers (SPIE, SPIM)
 */
#define M8xx_SPIE_MME   (1<<5)          /* Multi-master error */
#define M8xx_SPIE_TXE   (1<<4)          /* Tx error */
#define M8xx_SPIE_BSY   (1<<2)          /* Busy condition*/
#define M8xx_SPIE_TXB   (1<<1)          /* Tx buffer */
#define M8xx_SPIE_RXB   (1<<0)          /* Rx buffer */

/*
*************************************************************************
*                 SDMA (SCC, SMC, SPI) Buffer Descriptors               *
*************************************************************************
*/
typedef struct m8xxBufferDescriptor_ {
  volatile rtems_unsigned16        status;
  rtems_unsigned16        	   length;
  volatile void           	   *buffer;
} m8xxBufferDescriptor_t;

/*
 * Bits in receive buffer descriptor status word
 */
#define M8xx_BD_EMPTY           (1<<15) /* Ethernet, SCC UART, SMC UART, SPI */
#define M8xx_BD_WRAP            (1<<13) /* Ethernet, SCC UART, SMC UART, SPI */
#define M8xx_BD_INTERRUPT       (1<<12) /* Ethernet, SCC UART, SMC UART, SPI */
#define M8xx_BD_LAST            (1<<11) /* Ethernet, SPI */
#define M8xx_BD_CONTROL_CHAR    (1<<11) /* SCC UART */
#define M8xx_BD_FIRST_IN_FRAME  (1<<10) /* Ethernet */
#define M8xx_BD_ADDRESS         (1<<10) /* SCC UART */
#define M8xx_BD_CONTINUOUS      (1<<9)  /* SCC UART, SMC UART, SPI */
#define M8xx_BD_MISS            (1<<8)  /* Ethernet */
#define M8xx_BD_IDLE            (1<<8)  /* SCC UART, SMC UART */
#define M8xx_BD_ADDRSS_MATCH    (1<<7)  /* SCC UART */
#define M8xx_BD_LONG            (1<<5)  /* Ethernet */
#define M8xx_BD_BREAK           (1<<5)  /* SCC UART, SMC UART */
#define M8xx_BD_NONALIGNED      (1<<4)  /* Ethernet */
#define M8xx_BD_FRAMING_ERROR   (1<<4)  /* SCC UART, SMC UART */
#define M8xx_BD_SHORT           (1<<3)  /* Ethernet */
#define M8xx_BD_PARITY_ERROR    (1<<3)  /* SCC UART, SMC UART */
#define M8xx_BD_CRC_ERROR       (1<<2)  /* Ethernet */
#define M8xx_BD_OVERRUN         (1<<1)  /* Ethernet, SCC UART, SMC UART, SPI */
#define M8xx_BD_COLLISION       (1<<0)  /* Ethernet */
#define M8xx_BD_CARRIER_LOST    (1<<0)  /* SCC UART, SMC UART */
#define M8xx_BD_MASTER_ERROR    (1<<0)  /* SPI */

/*
 * Bits in transmit buffer descriptor status word
 * Many bits have the same meaning as those in receiver buffer descriptors.
 */
#define M8xx_BD_READY           (1<<15) /* Ethernet, SCC UART, SMC UART, SPI */
#define M8xx_BD_PAD             (1<<14) /* Ethernet */
#define M8xx_BD_CTS_REPORT      (1<<11) /* SCC UART */
#define M8xx_BD_TX_CRC          (1<<10) /* Ethernet */
#define M8xx_BD_DEFER           (1<<9)  /* Ethernet */
#define M8xx_BD_HEARTBEAT       (1<<8)  /* Ethernet */
#define M8xx_BD_PREAMBLE        (1<<8)  /* SCC UART, SMC UART */
#define M8xx_BD_LATE_COLLISION  (1<<7)  /* Ethernet */
#define M8xx_BD_NO_STOP_BIT     (1<<7)  /* SCC UART */
#define M8xx_BD_RETRY_LIMIT     (1<<6)  /* Ethernet */
#define M8xx_BD_RETRY_COUNT(x)  (((x)&0x3C)>>2) /* Ethernet */
#define M8xx_BD_UNDERRUN        (1<<1)  /* Ethernet, SPI */
#define M8xx_BD_CARRIER_LOST    (1<<0)  /* Ethernet */
#define M8xx_BD_CTS_LOST        (1<<0)  /* SCC UART */

/*
*************************************************************************
*                           IDMA Buffer Descriptors                     *
*************************************************************************
*/
typedef struct m8xxIDMABufferDescriptor_ {
  rtems_unsigned16        status;
  rtems_unsigned8         dfcr;       
  rtems_unsigned8         sfcr;       
  rtems_unsigned32        length;
  void                    *source;
  void                    *destination;
} m8xxIDMABufferDescriptor_t;

/*
*************************************************************************
*       RISC Communication Processor Module Command Register (CR)       *
*************************************************************************
*/
#define M8xx_CR_RST             (1<<15) /* Reset communication processor */
#define M8xx_CR_OP_INIT_RX_TX   (0<<8)  /* SCC, SMC UART, SMC GCI, SPI */
#define M8xx_CR_OP_INIT_RX      (1<<8)  /* SCC, SMC UART, SPI */
#define M8xx_CR_OP_INIT_TX      (2<<8)  /* SCC, SMC UART, SPI */
#define M8xx_CR_OP_INIT_HUNT    (3<<8)  /* SCC, SMC UART */
#define M8xx_CR_OP_STOP_TX      (4<<8)  /* SCC, SMC UART */
#define M8xx_CR_OP_GR_STOP_TX   (5<<8)  /* SCC */
#define M8xx_CR_OP_INIT_IDMA    (5<<8)  /* IDMA */
#define M8xx_CR_OP_RESTART_TX   (6<<8)  /* SCC, SMC UART */
#define M8xx_CR_OP_CLOSE_RX_BD  (7<<8)  /* SCC, SMC UART, SPI */
#define M8xx_CR_OP_SET_GRP_ADDR (8<<8)  /* SCC */
#define M8xx_CR_OP_SET_TIMER    (8<<8)  /* Timer */
#define M8xx_CR_OP_GCI_TIMEOUT  (9<<8)  /* SMC GCI */
#define M8xx_CR_OP_RESERT_BCS   (10<<8) /* SCC */
#define M8xx_CR_OP_GCI_ABORT    (10<<8) /* SMC GCI */
#define M8xx_CR_OP_STOP_IDMA    (11<<8) /* IDMA */
#define M8xx_CR_OP_START_DSP    (12<<8) /* DSP */
#define M8xx_CR_OP_INIT_DSP     (13<<8) /* DSP */

#define M8xx_CR_CHAN_SCC1       (0<<4)  /* Channel selection */
#define M8xx_CR_CHAN_I2C        (1<<4)
#define M8xx_CR_CHAN_IDMA1      (1<<4)
#define M8xx_CR_CHAN_SCC2       (4<<4)
#define M8xx_CR_CHAN_SPI        (5<<4)
#define M8xx_CR_CHAN_IDMA2      (5<<4)
#define M8xx_CR_CHAN_TIMER      (5<<4)
#define M8xx_CR_CHAN_SCC3       (8<<4)
#define M8xx_CR_CHAN_SMC1       (9<<4)
#define M8xx_CR_CHAN_DSP1       (9<<4)
#define M8xx_CR_CHAN_SCC4       (12<<4)
#define M8xx_CR_CHAN_SMC2       (13<<4)
#define M8xx_CR_CHAN_DSP2       (13<<4)
#define M8xx_CR_FLG             (1<<0)  /* Command flag */

/*
*************************************************************************
*                 System Protection Control Register (SYPCR)            *
*************************************************************************
*/
#define M8xx_SYPCR_SWTC(x)      ((x)<<16)   /* Software watchdog timer count */
#define M8xx_SYPCR_BMT(x)       ((x)<<8)    /* Bus monitor timing */
#define M8xx_SYPCR_BME          (1<<7)      /* Bus monitor enable */
#define M8xx_SYPCR_SWF          (1<<3)      /* Software watchdog freeze */
#define M8xx_SYPCR_SWE          (1<<2)      /* Software watchdog enable */
#define M8xx_SYPCR_SWRI         (1<<1)      /* Watchdog reset/interrupt sel. */
#define M8xx_SYPCR_SWP          (1<<0)      /* Software watchdog prescale */

/*
*************************************************************************
*                 PCMCIA Control Registers
*************************************************************************
*/
#define M8xx_PCMCIA_POR_BSIZE_1B    (0x00 << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_2B    (0x01 << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_4B    (0x03 << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_8B    (0x02 << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_16B   (0x06 << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_32B   (0x07 << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_64B   (0x05 << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_128B  (0x04 << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_256B  (0x0C << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_512B  (0x0D << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_1KB   (0x0F << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_2KB   (0x0E << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_4KB   (0x0A << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_8KB   (0x0B << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_16KB  (0x09 << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_32KB  (0x08 << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_64KB  (0x18 << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_128KB (0x19 << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_256KB (0x1B << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_512KB (0x1A << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_1MB   (0x1E << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_2MB   (0x1F << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_4MB   (0x1D << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_8MB   (0x1C << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_16MB  (0x14 << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_32MB  (0x15 << (31-4))
#define M8xx_PCMCIA_POR_BSIZE_64MB  (0x17 << (31-4))

#define M8xx_PCMCIA_POR_PSHT(x)     (((x) & 0x0f) << (31-15))
#define M8xx_PCMCIA_POR_PSST(x)     (((x) & 0x0f) << (31-19))
#define M8xx_PCMCIA_POR_PSL(x)      (((x) & 0x1f) << (31-24))
#define M8xx_PCMCIA_POR_PPS_8       ((0) << (31-19))
#define M8xx_PCMCIA_POR_PPS_16      ((1) << (31-19))

#define M8xx_PCMCIA_POR_PRS_MEM     ((0) << (31-28))
#define M8xx_PCMCIA_POR_PRS_ATT     ((2) << (31-28))
#define M8xx_PCMCIA_POR_PRS_IO      ((3) << (31-28))
#define M8xx_PCMCIA_POR_PRS_DMA     ((4) << (31-28))
#define M8xx_PCMCIA_POR_PRS_DML     ((5) << (31-28))

#define M8xx_PCMCIA_POR_PSLOT_A     ((0) << (31-29))
#define M8xx_PCMCIA_POR_PSLOT_B     ((1) << (31-29))

#define M8xx_PCMCIA_POR_WP          ((1) << (31-30))
#define M8xx_PCMCIA_POR_VALID       ((1) << (31-31))

#define M8xx_PCMCIA_PGCR_CIRQLVL(x) (((x) & 0xff) << (31- 7))
#define M8xx_PCMCIA_PGCR_CSCHLVL(x) (((x) & 0xff) << (31-15))
#define M8xx_PCMCIA_PGCR_CDRQ_OFF    ((0) << (31-17))
#define M8xx_PCMCIA_PGCR_CDRQ_IOIS16 ((2) << (31-17))
#define M8xx_PCMCIA_PGCR_CDRQ_SPKR   ((3) << (31-17))
#define M8xx_PCMCIA_PGCR_COE         ((1) << (31-24))
#define M8xx_PCMCIA_PGCR_CRESET      ((1) << (31-25))

#define M8xx_PCMCIA_PIPR_CAVS1      ((1) << (31- 0))
#define M8xx_PCMCIA_PIPR_CAVS2      ((1) << (31- 1))
#define M8xx_PCMCIA_PIPR_CAWP       ((1) << (31- 2))
#define M8xx_PCMCIA_PIPR_CACD2      ((1) << (31- 3))
#define M8xx_PCMCIA_PIPR_CACD1      ((1) << (31- 4))
#define M8xx_PCMCIA_PIPR_CABVD2     ((1) << (31- 5))
#define M8xx_PCMCIA_PIPR_CABVD1     ((1) << (31- 6))
#define M8xx_PCMCIA_PIPR_CARDY      ((1) << (31- 7))
#define M8xx_PCMCIA_PIPR_CBVS1      ((1) << (31-16))
#define M8xx_PCMCIA_PIPR_CBVS2      ((1) << (31-17))
#define M8xx_PCMCIA_PIPR_CBWP       ((1) << (31-18))
#define M8xx_PCMCIA_PIPR_CBCD2      ((1) << (31-19))
#define M8xx_PCMCIA_PIPR_CBCD1      ((1) << (31-20))
#define M8xx_PCMCIA_PIPR_CBBVD2     ((1) << (31-21))
#define M8xx_PCMCIA_PIPR_CBBVD1     ((1) << (31-22))
#define M8xx_PCMCIA_PIPR_CBRDY      ((1) << (31-23))


#define M8xx_SYPCR_BMT(x)       ((x)<<8)    /* Bus monitor timing */
#define M8xx_SYPCR_BME          (1<<7)      /* Bus monitor enable */
#define M8xx_SYPCR_SWF          (1<<3)      /* Software watchdog freeze */
#define M8xx_SYPCR_SWE          (1<<2)      /* Software watchdog enable */
#define M8xx_SYPCR_SWRI         (1<<1)      /* Watchdog reset/interrupt sel. */
#define M8xx_SYPCR_SWP          (1<<0)      /* Software watchdog prescale */

/*
*************************************************************************
*                        Memory Control Registers                       *
*************************************************************************
*/
#define M8xx_UPM_AMX_8col       (0<<20) /* 8 column DRAM */
#define M8xx_UPM_AMX_9col       (1<<20) /* 9 column DRAM */
#define M8xx_UPM_AMX_10col      (2<<20) /* 10 column DRAM */
#define M8xx_UPM_AMX_11col      (3<<20) /* 11 column DRAM */
#define M8xx_UPM_AMX_12col      (4<<20) /* 12 column DRAM */
#define M8xx_UPM_AMX_13col      (5<<20) /* 13 column DRAM */
#define M8xx_MSR_PER(x)         (0x100<<(7-x)) /* Perity error bank (x) */
#define M8xx_MSR_WPER           (1<<7)  /* Write protection error */
#define M8xx_MPTPR_PTP(x)       ((x)<<8) /* Periodic timer prescaler */
#define M8xx_BR_BA(x)           ((x)&0xffff8000) /* Base address */
#define M8xx_BR_AT(x)           ((x)<<12) /* Address type */
#define M8xx_BR_PS8             (1<<10)  /* 8 bit port */
#define M8xx_BR_PS16            (2<<10)  /* 16 bit port */
#define M8xx_BR_PS32            (0<<10)  /* 32 bit port */
#define M8xx_BR_PARE            (1<<9)   /* Parity checking enable */
#define M8xx_BR_WP              (1<<8)   /* Write protect */
#define M8xx_BR_MS_GPCM         (0<<6)   /* GPCM */
#define M8xx_BR_MS_UPMA         (2<<6)   /* UPM A */
#define M8xx_BR_MS_UPMB         (3<<6)   /* UPM B */
#define M8xx_MEMC_BR_V          (1<<0)  /* Base/Option register are valid */

#define M8xx_MEMC_OR_32K        0xffff8000      /* Address range */
#define M8xx_MEMC_OR_64K        0xffff0000
#define M8xx_MEMC_OR_128K       0xfffe0000
#define M8xx_MEMC_OR_256K       0xfffc0000
#define M8xx_MEMC_OR_512K       0xfff80000
#define M8xx_MEMC_OR_1M         0xfff00000
#define M8xx_MEMC_OR_2M         0xffe00000
#define M8xx_MEMC_OR_4M         0xffc00000
#define M8xx_MEMC_OR_8M         0xff800000
#define M8xx_MEMC_OR_16M        0xff000000
#define M8xx_MEMC_OR_32M        0xfe000000
#define M8xx_MEMC_OR_64M        0xfc000000
#define M8xx_MEMC_OR_128        0xf8000000
#define M8xx_MEMC_OR_256M       0xf0000000
#define M8xx_MEMC_OR_512M       0xe0000000
#define M8xx_MEMC_OR_1G         0xc0000000
#define M8xx_MEMC_OR_2G         0x80000000
#define M8xx_MEMC_OR_4G         0x00000000
#define M8xx_MEMC_OR_ATM(x)     ((x)<<12)   /* Address type mask */
#define M8xx_MEMC_OR_CSNT       (1<<11)     /* Chip select is negated early */
#define M8xx_MEMC_OR_SAM        (1<<11)     /* Address lines are multiplexed */
#define M8xx_MEMC_OR_ACS_NORM   (0<<9)      /* *CS asserted with addr lines */
#define M8xx_MEMC_OR_ACS_QRTR   (2<<9)      /* *CS asserted 1/4 after addr */
#define M8xx_MEMC_OR_ACS_HALF   (3<<9)      /* *CS asserted 1/2 after addr */
#define M8xx_MEMC_OR_BI         (1<<8)      /* Burst inhibit */
#define M8xx_MEMC_OR_SCY(x)     ((x)<<4)    /* Cycle length in clocks */
#define M8xx_MEMC_OR_SETA       (1<<3)      /* *TA generated externally */
#define M8xx_MEMC_OR_TRLX       (1<<2)      /* Relaxed timing in GPCM */
#define M8xx_MEMC_OR_EHTR       (1<<1)      /* Extended hold time on reads */

/*
*************************************************************************
*                         UPM Registers (MxMR)                          *
*************************************************************************
*/
#define M8xx_MEMC_MMR_PTP(x)   ((x)<<24)    /* Periodic timer period */
#define M8xx_MEMC_MMR_PTE      (1<<23)      /* Periodic timer enable */
#define M8xx_MEMC_MMR_DSP(x)   ((x)<<17)    /* Disable timer period */
#define M8xx_MEMC_MMR_G0CL(x)  ((x)<<13)    /* General line 0 control */
#define M8xx_MEMC_MMR_UPWAIT   (1<<12)      /* GPL_x4 is UPWAITx */
#define M8xx_MEMC_MMR_RLF(x)   ((x)<<8)     /* Read loop field */
#define M8xx_MEMC_MMR_WLF(x)   ((x)<<4)     /* Write loop field */
#define M8xx_MEMC_MMR_TLF(x)   ((x)<<0)     /* Timer loop field */
/*
*************************************************************************
*                         Memory Command Register (MCR)                 *
*************************************************************************
*/
#define M8xx_MEMC_MCR_WRITE     (0<<30)     /* WRITE command */
#define M8xx_MEMC_MCR_READ      (1<<30)     /* READ command */
#define M8xx_MEMC_MCR_RUN       (2<<30)     /* RUN command */
#define M8xx_MEMC_MCR_UPMA      (0<<23)     /* Cmd is for UPMA */
#define M8xx_MEMC_MCR_UPMB      (1<<23)     /* Cmd is for UPMB */
#define M8xx_MEMC_MCR_MB(x)     ((x)<<13)   /* Memory bank when RUN cmd */
#define M8xx_MEMC_MCR_MCLF(x)   ((x)<<8)    /* Memory command loop field */
#define M8xx_MEMC_MCR_MAD(x)    (x)         /* Machine address */



/*
*************************************************************************
*                         SI Mode Register (SIMODE)                     *
*************************************************************************
*/
#define M8xx_SI_SMC2_BITS       0xFFFF0000      /* All SMC2 bits */
#define M8xx_SI_SMC2_TDM        (1<<31) /* Multiplexed SMC2 */
#define M8xx_SI_SMC2_BRG1       (0<<28) /* SMC2 clock souce */
#define M8xx_SI_SMC2_BRG2       (1<<28)
#define M8xx_SI_SMC2_BRG3       (2<<28)
#define M8xx_SI_SMC2_BRG4       (3<<28)
#define M8xx_SI_SMC2_CLK5       (0<<28)
#define M8xx_SI_SMC2_CLK6       (1<<28)
#define M8xx_SI_SMC2_CLK7       (2<<28)
#define M8xx_SI_SMC2_CLK8       (3<<28)
#define M8xx_SI_SMC1_BITS       0x0000FFFF      /* All SMC1 bits */
#define M8xx_SI_SMC1_TDM        (1<<15) /* Multiplexed SMC1 */
#define M8xx_SI_SMC1_BRG1       (0<<12) /* SMC1 clock souce */
#define M8xx_SI_SMC1_BRG2       (1<<12)
#define M8xx_SI_SMC1_BRG3       (2<<12)
#define M8xx_SI_SMC1_BRG4       (3<<12)
#define M8xx_SI_SMC1_CLK1       (0<<12)
#define M8xx_SI_SMC1_CLK2       (1<<12)
#define M8xx_SI_SMC1_CLK3       (2<<12)
#define M8xx_SI_SMC1_CLK4       (3<<12)

/*
*************************************************************************
*                  SDMA Configuration Register (SDCR)                   *
*************************************************************************
*/
#define M8xx_SDCR_FREEZE        (2<<13) /* Freeze on next bus cycle */
#define M8xx_SDCR_RAID_5        (1<<0)  /* Normal arbitration ID */

/*
*************************************************************************
*                  SDMA Status Register (SDSR)                          *
*************************************************************************
*/
#define M8xx_SDSR_SBER          (1<<7)  /* SDMA Channel bus error */
#define M8xx_SDSR_DSP2          (1<<1)  /* DSP Chain 2 interrupt */
#define M8xx_SDSR_DSP1          (1<<0)  /* DSP Chain 1 interrupt */

/*
*************************************************************************
*                      Baud (sic) Rate Generators                       *
*************************************************************************
*/
#define M8xx_BRG_RST            (1<<17)         /* Reset generator */
#define M8xx_BRG_EN             (1<<16)         /* Enable generator */
#define M8xx_BRG_EXTC_BRGCLK    (0<<14)         /* Source is BRGCLK */
#define M8xx_BRG_EXTC_CLK2      (1<<14)         /* Source is CLK2 pin */
#define M8xx_BRG_EXTC_CLK6      (2<<14)         /* Source is CLK6 pin */
#define M8xx_BRG_ATB            (1<<13)         /* Autobaud */
#define M8xx_BRG_115200         (21<<1)         /* Assume 40 MHz clock */
#define M8xx_BRG_57600          (32<<1)
#define M8xx_BRG_38400          (64<<1)
#define M8xx_BRG_19200          (129<<1)
#define M8xx_BRG_9600           (259<<1)
#define M8xx_BRG_4800           (520<<1)
#define M8xx_BRG_2400           (1040<<1)
#define M8xx_BRG_1200           (2082<<1)
#define M8xx_BRG_600            ((259<<1) | 1)
#define M8xx_BRG_300            ((520<<1) | 1)
#define M8xx_BRG_150            ((1040<<1) | 1)
#define M8xx_BRG_75             ((2080<<1) | 1)

#define M8xx_TGCR_CAS4          (1<<15)   /* Cascade timers 3 and 4 */
#define M8xx_TGCR_CAS2          (1<<7)    /* Cascade timers 1 and 2 */
#define M8xx_TGCR_FRZ1          (1<<2)    /* Halt timer if FREEZE asserted */
#define M8xx_TGCR_FRZ2          (1<<6)    /* Halt timer if FREEZE asserted */
#define M8xx_TGCR_FRZ3          (1<<10)   /* Halt timer if FREEZE asserted */
#define M8xx_TGCR_FRZ4          (1<<14)   /* Halt timer if FREEZE asserted */
#define M8xx_TGCR_STP1          (1<<1)    /* Stop timer */
#define M8xx_TGCR_STP2          (1<<5)    /* Stop timer */
#define M8xx_TGCR_STP3          (1<<9)    /* Stop timer */
#define M8xx_TGCR_STP4          (1<<13)   /* Stop timer */
#define M8xx_TGCR_RST1          (1<<0)    /* Enable timer */
#define M8xx_TGCR_RST2          (1<<4)    /* Enable timer */
#define M8xx_TGCR_RST3          (1<<8)    /* Enable timer */
#define M8xx_TGCR_RST4          (1<<12)   /* Enable timer */
#define M8xx_TGCR_GM1           (1<<3)    /* Gate Mode 1 for TMR1 or TMR2 */
#define M8xx_TGCR_GM2           (1<<11)   /* Gate Mode 2 for TMR3 or TMR4 */

#define M8xx_TMR_PS(x)          ((x)<<8)  /* Timer prescaler */
#define M8xx_TMR_CE_RISE        (1<<6)    /* Capture on rising edge */
#define M8xx_TMR_CE_FALL        (2<<6)    /* Capture on falling edge */
#define M8xx_TMR_CE_ANY         (3<<6)    /* Capture on any edge */
#define M8xx_TMR_OM_TOGGLE      (1<<5)    /* Toggle TOUTx pin */
#define M8xx_TMR_ORI            (1<<4)    /* Interrupt on reaching reference */
#define M8xx_TMR_RESTART        (1<<3)    /* Restart timer after reference */
#define M8xx_TMR_ICLK_INT       (1<<1)    /* Internal clock is timer source */
#define M8xx_TMR_ICLK_INT16     (2<<1)    /* Internal clock/16 is tmr src */
#define M8xx_TMR_ICLK_TIN       (3<<1)    /* TIN pin is timer source */
#define M8xx_TMR_TGATE          (1<<0)    /* TGATE controls timer */

#define M8xx_PISCR_PIRQ(x)      (1<<(15-x))  /* PIT interrupt level */
#define M8xx_PISCR_PS           (1<<7)    /* PIT Interrupt state */
#define M8xx_PISCR_PIE          (1<<2)    /* PIT interrupt enable */
#define M8xx_PISCR_PITF         (1<<1)    /* Stop timer when freeze asserted */
#define M8xx_PISCR_PTE          (1<<0)    /* PIT enable */

#define M8xx_TBSCR_TBIRQ(x)     (1<<(15-x))  /* TB interrupt level */
#define M8xx_TBSCR_REFA         (1<<7)    /* TB matches TBREFF0 */
#define M8xx_TBSCR_REFB         (1<<6)    /* TB matches TBREFF1 */
#define M8xx_TBSCR_REFAE        (1<<3)    /* Enable ints for REFA */
#define M8xx_TBSCR_REFBE        (1<<2)    /* Enable ints for REFB */
#define M8xx_TBSCR_TBF          (1<<1)    /* TB stops on FREEZE */
#define M8xx_TBSCR_TBE          (1<<0)    /* enable TB and decrementer */

#define M8xx_SIMASK_IRM0        (1<<31)
#define M8xx_SIMASK_LVM0        (1<<30)
#define M8xx_SIMASK_IRM1        (1<<29)
#define M8xx_SIMASK_LVM1        (1<<28)
#define M8xx_SIMASK_IRM2        (1<<27)
#define M8xx_SIMASK_LVM2        (1<<26)
#define M8xx_SIMASK_IRM3        (1<<25)
#define M8xx_SIMASK_LVM3        (1<<24)
#define M8xx_SIMASK_IRM4        (1<<23)
#define M8xx_SIMASK_LVM4        (1<<22)
#define M8xx_SIMASK_IRM5        (1<<21)
#define M8xx_SIMASK_LVM5        (1<<20)
#define M8xx_SIMASK_IRM6        (1<<19)
#define M8xx_SIMASK_LVM6        (1<<18)
#define M8xx_SIMASK_IRM7        (1<<17)
#define M8xx_SIMASK_LVM7        (1<<16)

#define M8xx_SIUMCR_EARB        (1<<31)
#define M8xx_SIUMCR_EARP0       (0<<28)
#define M8xx_SIUMCR_EARP1       (1<<28)
#define M8xx_SIUMCR_EARP2       (2<<28)
#define M8xx_SIUMCR_EARP3       (3<<28)
#define M8xx_SIUMCR_EARP4       (4<<28)
#define M8xx_SIUMCR_EARP5       (5<<28)
#define M8xx_SIUMCR_EARP6       (6<<28)
#define M8xx_SIUMCR_EARP7       (7<<28)
#define M8xx_SIUMCR_DSHW        (1<<23)
#define M8xx_SIUMCR_DBGC0       (0<<21)
#define M8xx_SIUMCR_DBGC1       (1<<21)
#define M8xx_SIUMCR_DBGC2       (2<<21)
#define M8xx_SIUMCR_DBGC3       (3<<21)
#define M8xx_SIUMCR_DBPC0       (0<<19)
#define M8xx_SIUMCR_DBPC1       (1<<19)
#define M8xx_SIUMCR_DBPC2       (2<<19)
#define M8xx_SIUMCR_DBPC3       (3<<19)
#define M8xx_SIUMCR_FRC         (1<<17)
#define M8xx_SIUMCR_DLK         (1<<16)
#define M8xx_SIUMCR_PNCS        (1<<15)
#define M8xx_SIUMCR_OPAR        (1<<14)
#define M8xx_SIUMCR_DPC         (1<<13)
#define M8xx_SIUMCR_MPRE        (1<<12)
#define M8xx_SIUMCR_MLRC0       (0<<10)
#define M8xx_SIUMCR_MLRC1       (1<<10)
#define M8xx_SIUMCR_MLRC2       (2<<10)
#define M8xx_SIUMCR_MLRC3       (3<<10)
#define M8xx_SIUMCR_AEME        (1<<9)
#define M8xx_SIUMCR_SEME        (1<<8)
#define M8xx_SIUMCR_BSC         (1<<7)
#define M8xx_SIUMCR_GB5E        (1<<6)
#define M8xx_SIUMCR_B2DD        (1<<5)
#define M8xx_SIUMCR_B3DD        (1<<4)

/*
 *  Value to write to a key register to unlock the corresponding SIU register
 */
#define M8xx_UNLOCK_KEY         0x55CCAA33
 
/*
*************************************************************************
*   MPC8xx INTERNAL MEMORY MAP REGISTERS (IMMR provides base address)   *
*************************************************************************
*/
typedef struct m8xx_ {
        
  /*
   * SIU Block
   */
  rtems_unsigned32      siumcr;
  rtems_unsigned32      sypcr;
#if defined(mpc860)
  rtems_unsigned32      swt;
#elif defined(mpc821)
  rtems_unsigned32      _pad70;
#endif
  rtems_unsigned16      _pad0;
  rtems_unsigned16      swsr;
  rtems_unsigned32      sipend;
  rtems_unsigned32      simask;
  rtems_unsigned32      siel;
  rtems_unsigned32      sivec;
  rtems_unsigned32      tesr;
  rtems_unsigned32      _pad1[3];
  rtems_unsigned32      sdcr;
  rtems_unsigned8       _pad2[0x80-0x34];
  
  /*
   * PCMCIA Block
   */
  rtems_unsigned32      pbr0;
  rtems_unsigned32      por0;
  rtems_unsigned32      pbr1;
  rtems_unsigned32      por1;
  rtems_unsigned32      pbr2;
  rtems_unsigned32      por2;
  rtems_unsigned32      pbr3;
  rtems_unsigned32      por3;
  rtems_unsigned32      pbr4;
  rtems_unsigned32      por4;
  rtems_unsigned32      pbr5;
  rtems_unsigned32      por5;
  rtems_unsigned32      pbr6;
  rtems_unsigned32      por6;
  rtems_unsigned32      pbr7;
  rtems_unsigned32      por7;
  rtems_unsigned8       _pad3[0xe0-0xc0];
  rtems_unsigned32      pgcra;
  rtems_unsigned32      pgcrb;
  rtems_unsigned32      pscr;
  rtems_unsigned32      _pad4;
  rtems_unsigned32      pipr;
  rtems_unsigned32      _pad5;
  rtems_unsigned32      per;
  rtems_unsigned32      _pad6;
  
  /*
   * MEMC Block
   */
  m8xxMEMCRegisters_t   memc[8];
  rtems_unsigned8       _pad7[0x164-0x140];
  rtems_unsigned32      mar;
  rtems_unsigned32      mcr;
  rtems_unsigned32      _pad8;
  rtems_unsigned32      mamr;
  rtems_unsigned32      mbmr;
  rtems_unsigned16      mstat;
  rtems_unsigned16      mptpr;
  rtems_unsigned32      mdr;
  rtems_unsigned8       _pad9[0x200-0x180];
  
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
  rtems_unsigned32      rsr;
  rtems_unsigned8       _pad16[0x300-0x28c];
  
  
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
  rtems_unsigned8       _pad21[0x800-0x400];
  rtems_unsigned8       _pad22[0x860-0x800];
  
  
  /*
   * I2C
   */
  rtems_unsigned8       i2mod;
  rtems_unsigned8       _pad23[3];
  rtems_unsigned8       i2add;
  rtems_unsigned8       _pad24[3];
  rtems_unsigned8       i2brg;
  rtems_unsigned8       _pad25[3];
  rtems_unsigned8       i2com;
  rtems_unsigned8       _pad26[3];
  rtems_unsigned8       i2cer;
  rtems_unsigned8       _pad27[3];
  rtems_unsigned8       i2cmr;
  rtems_unsigned8       _pad28[0x900-0x875];
  
  /*
   * DMA Block
   */
  rtems_unsigned32      _pad29;
  rtems_unsigned32      sdar;
  rtems_unsigned8       sdsr;
  rtems_unsigned8       _pad30[3];
  rtems_unsigned8       sdmr;
  rtems_unsigned8       _pad31[3];
  rtems_unsigned8       idsr1;
  rtems_unsigned8       _pad32[3];
  rtems_unsigned8       idmr1;
  rtems_unsigned8       _pad33[3];
  rtems_unsigned8       idsr2;
  rtems_unsigned8       _pad34[3];
  rtems_unsigned8       idmr2;
  rtems_unsigned8       _pad35[0x930-0x91d];
  
  /*
   * CPM Interrupt Control Block
   */
  rtems_unsigned16      civr;
  rtems_unsigned8       _pad36[14];
  rtems_unsigned32      cicr;
  rtems_unsigned32      cipr;
  rtems_unsigned32      cimr;
  rtems_unsigned32      cisr;
  
  /*
   * I/O Port Block
   */
  rtems_unsigned16      padir;
  rtems_unsigned16      papar;
  rtems_unsigned16      paodr;
  rtems_unsigned16      padat;
  rtems_unsigned8       _pad37[8];
  rtems_unsigned16      pcdir;
  rtems_unsigned16      pcpar;
  rtems_unsigned16      pcso;
  rtems_unsigned16      pcdat;
  rtems_unsigned16      pcint;
  rtems_unsigned8       _pad39[6];
  rtems_unsigned16      pddir;
  rtems_unsigned16      pdpar;
  rtems_unsigned16      _pad40;
  rtems_unsigned16      pddat;
  rtems_unsigned8       _pad41[8];
  
  /*
   * CPM Timers Block
   */
  rtems_unsigned16      tgcr;
  rtems_unsigned8       _pad42[14];
  rtems_unsigned16      tmr1;
  rtems_unsigned16      tmr2;
  rtems_unsigned16      trr1;
  rtems_unsigned16      trr2;
  rtems_unsigned16      tcr1;
  rtems_unsigned16      tcr2;
  rtems_unsigned16      tcn1;
  rtems_unsigned16      tcn2;
  rtems_unsigned16      tmr3;
  rtems_unsigned16      tmr4;
  rtems_unsigned16      trr3;
  rtems_unsigned16      trr4;
  rtems_unsigned16      tcr3;
  rtems_unsigned16      tcr4;
  rtems_unsigned16      tcn3;
  rtems_unsigned16      tcn4;
  rtems_unsigned16      ter1;
  rtems_unsigned16      ter2;
  rtems_unsigned16      ter3;
  rtems_unsigned16      ter4;
  rtems_unsigned8       _pad43[8];

  /*
   * CPM Block
   */
  rtems_unsigned16      cpcr;
  rtems_unsigned16      _pad44;
  rtems_unsigned16      rccr;
  rtems_unsigned8       _pad45;
  rtems_unsigned8       rmds;
  rtems_unsigned32      rmdr;
  rtems_unsigned16      rctr1;
  rtems_unsigned16      rctr2;
  rtems_unsigned16      rctr3;
  rtems_unsigned16      rctr4;
  rtems_unsigned16      _pad46;
  rtems_unsigned16      rter;
  rtems_unsigned16      _pad47;
  rtems_unsigned16      rtmr;
  rtems_unsigned8       _pad48[0x9f0-0x9dc];
  
  /*
   * BRG Block
   */
  rtems_unsigned32      brgc1;
  rtems_unsigned32      brgc2;
  rtems_unsigned32      brgc3;
  rtems_unsigned32      brgc4;
  
  /*
   * SCC Block
   */
  m8xxSCCRegisters_t    scc1;
  m8xxSCCRegisters_t    scc2;
#if defined(mpc860)
  m8xxSCCRegisters_t    scc3;
  m8xxSCCRegisters_t    scc4;
#elif defined(mpc821)
  rtems_unsigned8	_pad72[0xa80-0xa40];
#endif

  /*
   * SMC Block
   */
  m8xxSMCRegisters_t    smc1;
  m8xxSMCRegisters_t    smc2;
  
  /*
   * SPI Block
   */
  rtems_unsigned16      spmode;
  rtems_unsigned16      _pad49[2];
  rtems_unsigned8       spie;
  rtems_unsigned8       _pad50;
  rtems_unsigned16      _pad51;
  rtems_unsigned8       spim;
  rtems_unsigned8       _pad52[2];
  rtems_unsigned8       spcom;
  rtems_unsigned16      _pad53[2];
  
  /*
   * PIP Block
   */
  rtems_unsigned16      pipc;
  rtems_unsigned16      _pad54;
  rtems_unsigned16      ptpr;
  rtems_unsigned32      pbdir;
  rtems_unsigned32      pbpar;
  rtems_unsigned16      _pad55;
  rtems_unsigned16      pbodr;
  rtems_unsigned32      pbdat;
  rtems_unsigned32      _pad56[6];
  
  /*
   * SI Block
   */
  rtems_unsigned32      simode;
  rtems_unsigned8       sigmr;
  rtems_unsigned8       _pad57;
  rtems_unsigned8       sistr;
  rtems_unsigned8       sicmr;
  rtems_unsigned32      _pad58;
  rtems_unsigned32      sicr;
  rtems_unsigned16      sirp[2];
  rtems_unsigned32      _pad59[3];
  rtems_unsigned8       _pad60[0xc00-0xb00];
  rtems_unsigned8       siram[512];
#if defined(mpc860)
  /*
   * This is only used on the MPC8xxT - for the Fast Ethernet Controller (FEC)
   */
  m8xxFECRegisters_t    fec;
#elif defined(mpc821)
  rtems_unsigned8       lcdram[512];
#endif
  rtems_unsigned8       _pad62[0x2000-0x1000];
  
  /*
   * Dual-port RAM
   */
  rtems_unsigned8       dpram0[0x200];  /* BD/DATA/UCODE */
  rtems_unsigned8       dpram1[0x200];  /* BD/DATA/UCODE */
  rtems_unsigned8       dpram2[0x400];  /* BD/DATA/UCODE */
  rtems_unsigned8       dpram3[0x600];  /* BD/DATA*/
  rtems_unsigned8       dpram4[0x200];  /* BD/DATA/UCODE */
  rtems_unsigned8       _pad63[0x3c00-0x3000];
  
  /* When using SCC1 for ethernet, we lose the use of I2C since
   *  their parameters would overlap. Motorola has a microcode
   *  patch to move parameters around so that both can be used
   *  together. It is available on their web site somewhere 
   *  under http://www.mot.com/mpc8xx. If ethernet is used on
   *  one (or more) of the other SCCs, then other CPM features
   *  will be unavailable:
   *    SCC2    -> lose SPI
   *    SCC3    -> lose SMC1
   *    SCC4    -> lose SMC2
   *  However, Ethernet only works on SCC1 on the 8xx.
   */
  m8xxSCCENparms_t      scc1p;
  rtems_unsigned8       _rsv1[0xCB0-0xC00-sizeof(m8xxSCCENparms_t)];
  m8xxMiscParms_t       miscp;
  rtems_unsigned8       _rsv2[0xcc0-0xCB0-sizeof(m8xxMiscParms_t)];
  m8xxIDMAparms_t       idma1p;
  rtems_unsigned8       _rsv3[0xd00-0xcc0-sizeof(m8xxIDMAparms_t)];
  
  m8xxSCCparms_t        scc2p;
  rtems_unsigned8       _rsv4[0xD80-0xD00-sizeof(m8xxSCCparms_t)];
  m8xxSPIparms_t        spip;
  rtems_unsigned8       _rsv5[0xDB0-0xD80-sizeof(m8xxSPIparms_t)];
  m8xxTimerParms_t      tmp;
  rtems_unsigned8       _rsv6[0xDC0-0xDB0-sizeof(m8xxTimerParms_t)];
  m8xxIDMAparms_t       idma2p;
  rtems_unsigned8       _rsv7[0xE00-0xDC0-sizeof(m8xxIDMAparms_t)];
  
  m8xxSCCparms_t        scc3p; /* Not available on MPC821 */
  rtems_unsigned8       _rsv8[0xE80-0xE00-sizeof(m8xxSCCparms_t)];
  m8xxSMCparms_t        smc1p;
  rtems_unsigned8       _rsv9[0xEC0-0xE80-sizeof(m8xxSMCparms_t)];
  m8xxDSPparms_t        dsp1p;
  rtems_unsigned8       _rsv10[0xF00-0xEC0-sizeof(m8xxDSPparms_t)];
  
  m8xxSCCparms_t        scc4p; /* Not available on MPC821 */
  rtems_unsigned8       _rsv11[0xF80-0xF00-sizeof(m8xxSCCparms_t)];
  m8xxSMCparms_t        smc2p;
  rtems_unsigned8       _rsv12[0xFC0-0xF80-sizeof(m8xxSMCparms_t)];
  m8xxDSPparms_t        dsp2p;
  rtems_unsigned8       _rsv13[0x1000-0xFC0-sizeof(m8xxDSPparms_t)];
} m8xx_t;

extern volatile m8xx_t m8xx;

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif /* __MPC8xx_h */
