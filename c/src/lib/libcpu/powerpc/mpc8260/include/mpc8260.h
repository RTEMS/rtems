/* buggy version of CPU */
#define REV_0_2

/*
**************************************************************************
**************************************************************************
**                                                                      **
**  MOTOROLA MPC8260 POWER QUAD INTEGRATED COMMUNICATIONS CONTROLLER    **
**                             POWERQUICC II                            **
**                                                                      **
**                        HARDWARE DECLARATIONS                         **
**                                                                      **
**                                                                      **
**  Submitted by:							**
**      Andy Dachs							**							**
**	Surrey Satellite Technology Limited				**					**
**	http://www.sstl.co.uk						**						**
**	a.dachs@sstl.co.uk						**								**
**                                                                      **
**  Based on previous submissions for other PPC variants by:	        **
**								        **
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
**																		**
**                                                                      **
**************************************************************************
**************************************************************************
*/

#ifndef __MPC8260_h
#define __MPC8260_h

#ifndef ASM
/*
  Macros for SPRs
*/




/*
*************************************************************************
*                         REGISTER SUBBLOCKS                            *
*************************************************************************
*/


/*
 * Memory controller registers
 */
typedef struct m8260MEMCRegisters_ {
  rtems_unsigned32        br;
  rtems_unsigned32        or;
} m8260MEMCRegisters_t;


/*
 * Fast Communication Controller Registers
*/
typedef struct m8260FCCRegisters_ {
  rtems_unsigned32    gfmr;
  rtems_unsigned32	  fpsmr;
  rtems_unsigned16	  ftodr;
  rtems_unsigned8	  fcc_pad0[2];
  rtems_unsigned16	  fdsr;
  rtems_unsigned8	  fcc_pad1[2];
  rtems_unsigned32	  fcce;
  rtems_unsigned32	  fccm;
  rtems_unsigned8	  fccs;
  rtems_unsigned8	  fcc_pad2[3];
  rtems_unsigned8	  ftirr_phy0;			/* n/a on FCC3 */
  rtems_unsigned8	  ftirr_phy1;           /* n/a on FCC3 */
  rtems_unsigned8	  ftirr_phy2;			/* n/a on FCC3 */
  rtems_unsigned8	  ftirr_phy3;			/* n/a on FCC3 */
} m8260FCCRegisters_t;


/*
 * Serial Communications Controller registers
 */
typedef struct m8260SCCRegisters_ {
  rtems_unsigned32        gsmr_l;
  rtems_unsigned32        gsmr_h;
  rtems_unsigned16        psmr;
  rtems_unsigned8         scc_pad0[2];
  rtems_unsigned16        todr;
  rtems_unsigned16        dsr;
  rtems_unsigned16        scce;
  rtems_unsigned8	  scc_pad2[2];
  rtems_unsigned16        sccm;
  rtems_unsigned8	  scc_pad3[1];
  rtems_unsigned8         sccs;
  rtems_unsigned8         scc_pad1[8];
} m8260SCCRegisters_t;

/*
 * Serial Management Controller registers
 */
typedef struct m8260SMCRegisters_ {
  rtems_unsigned8         smc_pad0[2];
  rtems_unsigned16        smcmr;
  rtems_unsigned8	  smc_pad2[2];
  rtems_unsigned8         smce;
  rtems_unsigned8	  smc_pad3[3];
  rtems_unsigned8         smcm;
  rtems_unsigned8         smc_pad1[5];
} m8260SMCRegisters_t;


/*
 * Serial Interface With Time Slot Assigner Registers
 */
typedef struct m8260SIRegisters_ {
  rtems_unsigned16	  siamr;
  rtems_unsigned16	  sibmr;
  rtems_unsigned16	  sicmr;
  rtems_unsigned16	  sidmr;
  rtems_unsigned8	  sigmr;
  rtems_unsigned8	  si_pad0[1];
  rtems_unsigned8	  sicmdr;
  rtems_unsigned8	  si_pad1[1];
  rtems_unsigned8	  sistr;
  rtems_unsigned8	  si_pad2[1];
  rtems_unsigned16	  sirsr;
} m8260SIRegisters_t;


/*
 * Multi Channel Controller registers
 */
typedef struct m8260MCCRegisters_ {
  rtems_unsigned16	  mcce;
  rtems_unsigned8	  mcc_pad2[2];
  rtems_unsigned16	  mccm;
  rtems_unsigned16	  mcc_pad0;
  rtems_unsigned8	  mccf;
  rtems_unsigned8	  mcc_pad1[7];
} m8260MCCRegisters_t;


/*
*************************************************************************
*                              RISC Timers                              *
*************************************************************************
*/
/*
typedef struct m8260TimerParms_ {
  rtems_unsigned16        tm_base;
  rtems_unsigned16        _tm_ptr;
  rtems_unsigned16        _r_tmr;
  rtems_unsigned16        _r_tmv;
  rtems_unsigned32        tm_cmd;
  rtems_unsigned32        tm_cnt;
} m8260TimerParms_t;
*/

/*
 * RISC Controller Configuration Register (RCCR)
 * All other bits in this register are reserved.
 */
#define M8260_RCCR_TIME          (1<<31)    /* Enable timer */
#define M8260_RCCR_TIMEP(x)      ((x)<<24)  /* Timer period */
#define M8260_RCCR_DR1M          (1<<23)    /* IDMA Rqst 1 Mode */
#define M8260_RCCR_DR2M          (1<<22)    /* IDMA Rqst 2 Mode */
#define M8260_RCCR_DR1QP(x)      ((x)<<20)  /* IDMA1 Rqst Priority */
#define M8260_RCCR_EIE           (1<<19)    /* External Interrupt Enable */
#define M8260_RCCR_SCD           (1<<18)    /* Scheduler Configuration */
#define M8260_RCCR_DR2QP(x)      ((x)<<16)  /* IDMA2 Rqst Priority */
#define M8260_RCCR_ERAM(x)       ((x)<<13)  /* Enable RAM Microcode */
#define M8260_RCCR_EDM1          (1<<11)    /* DRQ1 Edge detect mode */
#define M8260_RCCR_EDM2          (1<<10)    /* DRQ2 Edge detect mode */
#define M8260_RCCR_EDM3          (1<<9)     /* DRQ3 Edge detect mode */
#define M8260_RCCR_EDM4          (1<<8)     /* DRQ4 Edge detect mode */
#define M8260_RCCR_DR3M          (1<<7)     /* IDMA Rqst 1 Mode */
#define M8260_RCCR_DR4M          (1<<6)     /* IDMA Rqst 2 Mode */
#define M8260_RCCR_DR3QP(x)      ((x)<<4)   /* IDMA3 Rqst Priority */
#define M8260_RCCR_DEM12         (1<<3)     /* DONE1,2 Edge detect mode */
#define M8260_RCCR_DEM34         (1<<2)     /* DONE3,4 Edge detect mode */
#define M8260_RCCR_DR4QP(x)      (x)   	    /* IDMA4 Rqst Priority */



/*
 * Command register
 * Set up this register before issuing a M8260_CR_OP_SET_TIMER command.
 */
#if 0
#define M8260_TM_CMD_V           (1<<31)         /* Set to enable timer */
#define M8260_TM_CMD_R           (1<<30)         /* Set for automatic restart */
#define M8260_TM_CMD_PWM         (1<<29)         /* Set for PWM operation */
#define M8260_TM_CMD_TIMER(x)    ((x)<<16)       /* Select timer */
#define M8260_TM_CMD_PERIOD(x)   (x)             /* Timer period (16 bits) */
#endif

/*
*************************************************************************
*                               DMA Controllers                         *
*************************************************************************
*/
typedef struct m8260IDMAparms_ {
  rtems_unsigned16        ibase;
  rtems_unsigned16        dcm;
  rtems_unsigned16		  ibdptr;
  rtems_unsigned16		  dpr_buf;
  rtems_unsigned16		  _buf_inv;
  rtems_unsigned16		  ssmax;
  rtems_unsigned16		  _dpr_in_ptr;
  rtems_unsigned16		  sts;
  rtems_unsigned16		  _dpr_out_ptr;
  rtems_unsigned16		  seob;
  rtems_unsigned16		  deob;
  rtems_unsigned16		  dts;
  rtems_unsigned16		  _ret_add;
  rtems_unsigned16		  reserved;
  rtems_unsigned32		  _bd_cnt;
  rtems_unsigned32		  _s_ptr;
  rtems_unsigned32		  _d_ptr;
  rtems_unsigned32		  istate;
} m8260IDMAparms_t;


/*
*************************************************************************
*                   Serial Communication Controllers                    *
*************************************************************************
*/


typedef struct m8260SCCparms_ {
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
      rtems_unsigned16        idlc;
      rtems_unsigned16        brkcr;
      rtems_unsigned16        parec;
      rtems_unsigned16        frmec;
      rtems_unsigned16        nosec;
      rtems_unsigned16        brkec;
      rtems_unsigned16        brklen;
      rtems_unsigned16        uaddr[2];
      rtems_unsigned16        rtemp;
      rtems_unsigned16        toseq;
      rtems_unsigned16        character[8];
      rtems_unsigned16        rccm;
      rtems_unsigned16        rccr;
      rtems_unsigned16        rlbc;
    } uart;
    struct {
      rtems_unsigned32	      _pad0;
      rtems_unsigned32	      c_mask;
      rtems_unsigned32	      c_pres;
      rtems_unsigned16	      disfc;
      rtems_unsigned16	      crcec;
      rtems_unsigned16	      abtsc;
      rtems_unsigned16	      nmarc;
      rtems_unsigned16	      retrc;
      rtems_unsigned16	      mflr;
      rtems_unsigned16	      _max_cnt;
      rtems_unsigned16	      rfthr;
      rtems_unsigned16	      _rfcnt;
      rtems_unsigned16	      hmask;
      rtems_unsigned16	      haddr1;
      rtems_unsigned16	      haddr2;
      rtems_unsigned16	      haddr3;
      rtems_unsigned16	      haddr4;
      rtems_unsigned16	      _tmp;
      rtems_unsigned16	      _tmp_mb;
    } hdlc;
    struct {
      rtems_unsigned32        _pad0;
      rtems_unsigned32        crcc;
      rtems_unsigned16        prcrc;
      rtems_unsigned16        ptcrc;
      rtems_unsigned16        parec;
      rtems_unsigned16        bsync;
      rtems_unsigned16        bdle;
      rtems_unsigned16        character[8];
      rtems_unsigned16        rccm;
    } bisync;
    struct {
      rtems_unsigned32	      _crc_p;
      rtems_unsigned32	      _crc_c;
    } transparent;
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
      rtems_unsigned16        _dma_cnt;
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
} m8260SCCparms_t;


/*
 * Event and mask registers (SCCE, SCCM)
 */
#define M8260_SCCE_BRKE  (1<<6)
#define M8260_SCCE_BRK   (1<<5)
#define M8260_SCCE_TXE   (1<<4)
#define M8260_SCCE_RXF   (1<<3)
#define M8260_SCCE_BSY   (1<<2)
#define M8260_SCCE_TX    (1<<1)
#define M8260_SCCE_RX    (1<<0)


/*
*************************************************************************
*                   Fast Serial Communication Controllers               *
*************************************************************************
*/


typedef struct m8260FCCparms_ {
  rtems_unsigned16        riptr;
  rtems_unsigned16        tiptr;
  rtems_unsigned16        _pad0;
  rtems_unsigned16        mrblr;
  rtems_unsigned32        rstate;
  rtems_unsigned32        rbase;
  rtems_unsigned16        _rbdstat;
  rtems_unsigned16        _rbdlen;
  rtems_unsigned32        _rdptr;
  rtems_unsigned32        tstate;
  rtems_unsigned32        tbase;
  rtems_unsigned16        _tbdstat;
  rtems_unsigned16        _tbdlen;
  rtems_unsigned32        _tdptr;
  rtems_unsigned32        _rbptr;
  rtems_unsigned32        _tbptr;
  rtems_unsigned32        _rcrc;
  rtems_unsigned32        _pad1;
  rtems_unsigned32        _tcrc;

  union {
    struct {
      rtems_unsigned32	      _pad0;
      rtems_unsigned32	      _pad1;
      rtems_unsigned32	      c_mask;
      rtems_unsigned32	      c_pres;
      rtems_unsigned16	      disfc;
      rtems_unsigned16	      crcec;
      rtems_unsigned16	      abtsc;
      rtems_unsigned16	      nmarc;
      rtems_unsigned32	      _max_cnt;
      rtems_unsigned16	      mflr;
      rtems_unsigned16	      rfthr;
      rtems_unsigned16	      rfcnt;
      rtems_unsigned16	      hmask;
      rtems_unsigned16	      haddr1;
      rtems_unsigned16	      haddr2;
      rtems_unsigned16	      haddr3;
      rtems_unsigned16	      haddr4;
      rtems_unsigned16	      _ts_tmp;
      rtems_unsigned16	      _tmp_mb;
    } hdlc;
    struct {
      rtems_unsigned32	      _pad0;
      rtems_unsigned32	      _pad1;
      rtems_unsigned32	      c_mask;
      rtems_unsigned32	      c_pres;
      rtems_unsigned16	      disfc;
      rtems_unsigned16	      crcec;
      rtems_unsigned16	      abtsc;
      rtems_unsigned16	      nmarc;
      rtems_unsigned32	      _max_cnt;
      rtems_unsigned16	      mflr;
      rtems_unsigned16	      rfthr;
      rtems_unsigned16	      rfcnt;
      rtems_unsigned16	      hmask;
      rtems_unsigned16	      haddr1;
      rtems_unsigned16	      haddr2;
      rtems_unsigned16	      haddr3;
      rtems_unsigned16	      haddr4;
      rtems_unsigned16	      _ts_tmp;
      rtems_unsigned16	      _tmp_mb;
    } transparent;
    struct {
      rtems_unsigned32        _stat_buf;
      rtems_unsigned32        cam_ptr;
      rtems_unsigned32        c_mask;
      rtems_unsigned32        c_pres;
      rtems_unsigned32        crcec;
      rtems_unsigned32        alec;
      rtems_unsigned32        disfc;
      rtems_unsigned16        ret_lim;
      rtems_unsigned16        _ret_cnt;
      rtems_unsigned16        p_per;
      rtems_unsigned16        _boff_cnt;
      rtems_unsigned32        gaddr_h;
      rtems_unsigned32        gaddr_l;
      rtems_unsigned16        tfcstat;
      rtems_unsigned16        tfclen;
      rtems_unsigned32        tfcptr;
      rtems_unsigned16        mflr;
      rtems_unsigned16        paddr1_h;
      rtems_unsigned16        paddr1_m;
      rtems_unsigned16        paddr1_l;
      rtems_unsigned16        _ibd_cnt;
      rtems_unsigned16        _ibd_start;
      rtems_unsigned16        _ibd_end;
      rtems_unsigned16        _tx_len;
      rtems_unsigned16        _ibd_base;
      rtems_unsigned32        iaddr_h;
      rtems_unsigned32        iaddr_l;
      rtems_unsigned16        minflr;
      rtems_unsigned16        taddr_h;
      rtems_unsigned16        taddr_m;
      rtems_unsigned16        taddr_l;
      rtems_unsigned16        pad_ptr;
      rtems_unsigned16        _pad0;
      rtems_unsigned16        _cf_range;
      rtems_unsigned16        _max_b;
      rtems_unsigned16        maxd1;
      rtems_unsigned16        maxd2;
      rtems_unsigned16        _maxd;
      rtems_unsigned16        _dma_cnt;
      rtems_unsigned32        octc;
      rtems_unsigned32        colc;
      rtems_unsigned32        broc;
      rtems_unsigned32        mulc;
      rtems_unsigned32        uspc;
      rtems_unsigned32        frgc;
      rtems_unsigned32        ospc;
      rtems_unsigned32        jbrc;
      rtems_unsigned32        p64c;
      rtems_unsigned32        p65c;
      rtems_unsigned32        p128c;
      rtems_unsigned32        p256c;
      rtems_unsigned32        p512c;
      rtems_unsigned32        p1024c;
      rtems_unsigned32        _cam_buf;
      rtems_unsigned32        _pad1;
    } ethernet;
  } un;
} m8260FCCparms_t;


/*
 * Receive and transmit function code register bits
 * These apply to the function code registers of all devices, not just SCC.
 */
#define M8260_RFCR_BO(x)         ((x)<<3)
#define M8260_RFCR_MOT           (2<<3)
#define M8260_RFCR_LOCAL_BUS	 (2)
#define M8260_RFCR_60X_BUS	 (0)
#define M8260_TFCR_BO(x)         ((x)<<3)
#define M8260_TFCR_MOT           (2<<3)
#define M8260_TFCR_LOCAL_BUS	 (2)
#define M8260_TFCR_60X_BUS	 (0)

/*
*************************************************************************
*                     Serial Management Controllers                     *
*************************************************************************
*/
typedef struct m8260SMCparms_ {
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
      rtems_unsigned16	  max_idl;
      rtems_unsigned16	  _idlc;
      rtems_unsigned16	  _brkln;
      rtems_unsigned16	  brkec;
      rtems_unsigned16	  brkcr;
      rtems_unsigned16	  _r_mask;
    } uart;
    struct {
      rtems_unsigned16	  _pad0[6];
    } transparent;
  } un;
  rtems_unsigned32	  _pad6;
} m8260SMCparms_t;

/*
 * Mode register
 */
#define M8260_SMCMR_CLEN(x)              ((x)<<11)    /* Character length */
#define M8260_SMCMR_2STOP                (1<<10)      /* 2 stop bits */
#define M8260_SMCMR_PARITY               (1<<9)       /* Enable parity */
#define M8260_SMCMR_EVEN                 (1<<8)       /* Even parity */
#define M8260_SMCMR_SM_GCI               (0<<4)       /* GCI Mode */
#define M8260_SMCMR_SM_UART              (2<<4)       /* UART Mode */
#define M8260_SMCMR_SM_TRANSPARENT       (3<<4)       /* Transparent Mode */
#define M8260_SMCMR_DM_LOOPBACK          (1<<2)       /* Local loopback mode */
#define M8260_SMCMR_DM_ECHO              (2<<2)       /* Echo mode */
#define M8260_SMCMR_TEN                  (1<<1)       /* Enable transmitter */
#define M8260_SMCMR_REN                  (1<<0)       /* Enable receiver */

/*
 * Event and mask registers (SMCE, SMCM)
 */
#define M8260_SMCE_TXE   (1<<4)
#define M8260_SMCE_BSY   (1<<2)
#define M8260_SMCE_TX    (1<<1)
#define M8260_SMCE_RX    (1<<0)

/*
*************************************************************************
*                      Serial Peripheral Interface                      *
*************************************************************************
*/
typedef struct m8260SPIparms_ {
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
} m8260SPIparms_t;

/*
 * Mode register (SPMODE)
 */
#define M8260_SPMODE_LOOP                (1<<14)  /* Local loopback mode */
#define M8260_SPMODE_CI                  (1<<13)  /* Clock invert */
#define M8260_SPMODE_CP                  (1<<12)  /* Clock phase */
#define M8260_SPMODE_DIV16               (1<<11)  /* Divide BRGCLK by 16 */
#define M8260_SPMODE_REV                 (1<<10)  /* Reverse data */
#define M8260_SPMODE_MASTER              (1<<9)   /* SPI is master */
#define M8260_SPMODE_EN                  (1<<8)   /* Enable SPI */
#define M8260_SPMODE_CLEN(x)             ((x)<<4) /* Character length */
#define M8260_SPMODE_PM(x)               (x)      /* Prescaler modulus */

/*
 * Mode register (SPCOM)
 */
#define M8260_SPCOM_STR                  (1<<7)  /* Start transmit */

/*
 * Event and mask registers (SPIE, SPIM)
 */
#define M8260_SPIE_MME   (1<<5)          /* Multi-master error */
#define M8260_SPIE_TXE   (1<<4)          /* Tx error */
#define M8260_SPIE_BSY   (1<<2)          /* Busy condition*/
#define M8260_SPIE_TXB   (1<<1)          /* Tx buffer */
#define M8260_SPIE_RXB   (1<<0)          /* Rx buffer */

/*
*************************************************************************
*                 SDMA (SCC, SMC, SPI) Buffer Descriptors               *
*************************************************************************
*/
typedef struct m8260BufferDescriptor_ {
  rtems_unsigned16        status;
  rtems_unsigned16        length;
  volatile void           *buffer;
} m8260BufferDescriptor_t;

/*
 * Bits in receive buffer descriptor status word
 */
#define M8260_BD_EMPTY           (1<<15) /* Ethernet, SCC UART, SMC UART, SPI */
#define M8260_BD_WRAP            (1<<13) /* Ethernet, SCC UART, SMC UART, SPI */
#define M8260_BD_INTERRUPT       (1<<12) /* Ethernet, SCC UART, SMC UART, SPI */
#define M8260_BD_LAST            (1<<11) /* Ethernet, SPI */
#define M8260_BD_CONTROL_CHAR    (1<<11) /* SCC UART */
#define M8260_BD_FIRST_IN_FRAME  (1<<10) /* Ethernet */
#define M8260_BD_ADDRESS         (1<<10) /* SCC UART */
#define M8260_BD_CONTINUOUS      (1<<9)  /* SCC UART, SMC UART, SPI */
#define M8260_BD_MISS            (1<<8)  /* Ethernet */
#define M8260_BD_IDLE            (1<<8)  /* SCC UART, SMC UART */
#define M8260_BD_ADDRSS_MATCH    (1<<7)  /* SCC UART */
#define M8260_BD_LONG            (1<<5)  /* Ethernet, SCC HDLC */
#define M8260_BD_BREAK           (1<<5)  /* SCC UART, SMC UART */
#define M8260_BD_NONALIGNED      (1<<4)  /* Ethernet, SCC HDLC */
#define M8260_BD_FRAMING_ERROR   (1<<4)  /* SCC UART, SMC UART */
#define M8260_BD_SHORT           (1<<3)  /* Ethernet */
#define M8260_BD_PARITY_ERROR    (1<<3)  /* SCC UART, SMC UART */
#define M8260_BD_ABORT		 (1<<3)  /* SCC HDLC */
#define M8260_BD_CRC_ERROR       (1<<2)  /* Ethernet, SCC HDLC */
#define M8260_BD_OVERRUN         (1<<1)  /* Ethernet, SCC UART, SMC UART, SPI */
#define M8260_BD_COLLISION       (1<<0)  /* Ethernet */
#define M8260_BD_CARRIER_LOST    (1<<0)  /* SCC UART, SMC UART */
#define M8260_BD_MASTER_ERROR    (1<<0)  /* SPI */

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
#define M8xx_BD_LONG            (1<<5)  /* Ethernet, SCC HDLC */
#define M8xx_BD_BREAK           (1<<5)  /* SCC UART, SMC UART */
#define M8xx_BD_NONALIGNED      (1<<4)  /* Ethernet, SCC HDLC */
#define M8xx_BD_FRAMING_ERROR   (1<<4)  /* SCC UART, SMC UART */
#define M8xx_BD_SHORT           (1<<3)  /* Ethernet */
#define M8xx_BD_PARITY_ERROR    (1<<3)  /* SCC UART, SMC UART */
#define M8xx_BD_ABORT		 (1<<3)  /* SCC HDLC */
#define M8xx_BD_CRC_ERROR       (1<<2)  /* Ethernet, SCC HDLC */
#define M8xx_BD_OVERRUN         (1<<1)  /* Ethernet, SCC UART, SMC UART, SPI */
#define M8xx_BD_COLLISION       (1<<0)  /* Ethernet */
#define M8xx_BD_CARRIER_LOST    (1<<0)  /* SCC UART, SMC UART */
#define M8xx_BD_MASTER_ERROR    (1<<0)  /* SPI */

/*
 * Bits in transmit buffer descriptor status word
 * Many bits have the same meaning as those in receiver buffer descriptors.
 */
#define M8260_BD_READY           (1<<15) /* Ethernet, SCC UART, SMC UART, SPI */
#define M8260_BD_PAD             (1<<14) /* Ethernet */
#define M8260_BD_CTS_REPORT      (1<<11) /* SCC UART */
#define M8260_BD_TX_CRC          (1<<10) /* Ethernet */
#define M8260_BD_DEFER           (1<<9)  /* Ethernet */
#define M8260_BD_HEARTBEAT       (1<<8)  /* Ethernet */
#define M8260_BD_PREAMBLE        (1<<8)  /* SCC UART, SMC UART */
#define M8260_BD_LATE_COLLISION  (1<<7)  /* Ethernet */
#define M8260_BD_NO_STOP_BIT     (1<<7)  /* SCC UART */
#define M8260_BD_RETRY_LIMIT     (1<<6)  /* Ethernet */
#define M8260_BD_RETRY_COUNT(x)  (((x)&0x3C)>>2) /* Ethernet */
#define M8260_BD_UNDERRUN        (1<<1)  /* Ethernet, SPI, SCC HDLC */
#define M8260_BD_CARRIER_LOST    (1<<0)  /* Ethernet */
#define M8260_BD_CTS_LOST        (1<<0)  /* SCC UART, SCC HDLC */

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
#define M8xx_BD_UNDERRUN        (1<<1)  /* Ethernet, SPI, SCC HDLC */
#define M8xx_BD_CARRIER_LOST    (1<<0)  /* Ethernet */
#define M8xx_BD_CTS_LOST        (1<<0)  /* SCC UART, SCC HDLC */

/*
*************************************************************************
*                           IDMA Buffer Descriptors                     *
*************************************************************************
*/
typedef struct m8260IDMABufferDescriptor_ {
  rtems_unsigned16        status;
  rtems_unsigned8         dfcr;       
  rtems_unsigned8         sfcr;       
  rtems_unsigned32        length;
  void                    *source;
  void                    *destination;
} m8260IDMABufferDescriptor_t;

/*
*************************************************************************
*       RISC Communication Processor Module Command Register (CR)       *
*************************************************************************
*/
#define M8260_CR_RST             (1<<31)   	     /* Reset communication processor */

#define M8260_CR_FCC1		 ((4<<26)|(16<<21))  /* FCC1 page and code */
#define M8260_CR_FCC1_ATM	 ((4<<26)|(14<<21))  /* FCC1 ATM mode page and code */
#define M8260_CR_FCC2		 ((5<<26)|(17<<21))  /* FCC2 page and code */
#define M8260_CR_FCC2_ATM	 ((5<<26)|(14<<21))  /* FCC2 ATM mode page and code */
#define M8260_CR_FCC3		 ((6<<26)|(18<<21))  /* FCC3 page and code */
#define M8260_CR_SCC1		 ((0<<26)|(4<<21))   /* SCC1 page and code */
#define M8260_CR_SCC2		 ((1<<26)|(5<<21))   /* SCC2 page and code */
#define M8260_CR_SCC3		 ((2<<26)|(6<<21))   /* SCC3 page and code */
#define M8260_CR_SCC4		 ((3<<26)|(7<<21))   /* SCC4 page and code */
#define M8260_CR_SMC1		 ((7<<26)|(8<<21))   /* SMC1 page and code */
#define M8260_CR_SMC2		 ((8<<26)|(9<<21))   /* SMC2 page and code */
#define M8260_CR_RAND		 ((10<<26)|(14<<21)) /* SMC2 page and code */
#define M8260_CR_SPI		 ((9<<26)|(10<<21))  /* SPI page and code */
#define M8260_CR_I2C		 ((10<<26)|(11<<21)) /* I2C page and code */
#define M8260_CR_TMR		 ((10<<26)|(15<<21)) /* Timer page and code */
#define M8260_CR_MCC1		 ((7<<26)|(28<<21))  /* MCC1 page and code */
#define M8260_CR_MCC2		 ((8<<26)|(29<<21))  /* MCC2 page and code */
#define M8260_CR_IDMA1		 ((7<<26)|(20<<21))  /* IDMA1 page and code */
#define M8260_CR_IDMA2		 ((8<<26)|(21<<21))  /* IDMA2 page and code */
#define M8260_CR_IDMA3		 ((9<<26)|(22<<21))  /* IDMA3 page and code */
#define M8260_CR_IDMA4		 ((10<<26)|(23<<21)) /* IDMA4 page and code */

#define M8260_CR_FLG		 (1<<16)   /* Command sempahore flag */

#define M8260_CR_MCC_CHAN(x)	 ((x)<<6)  /* MCC channel number */
#define M8260_CR_FCC_HDLC	 (0<<6)	   /* FCC HDLC/Transparent protocol code */
#define M8260_CR_FCC_ATM	 (10<<6)   /* FCC ATM protocol code */
#define M8260_CR_FCC_ETH	 (12<<6)   /* FCC Ethernet protocol code */

#define M8260_CR_OP_INIT_RX_TX   (0)       /* FCC, SCC, SMC UART, SMC GCI, SPI, I2C, MCC */
#define M8260_CR_OP_INIT_RX      (1)       /* FCC, SCC, SMC UART, SPI, I2C, MCC */
#define M8260_CR_OP_INIT_TX      (2)       /* FCC, SCC, SMC UART, SPI, I2C, MCC */
#define M8260_CR_OP_INIT_HUNT    (3)       /* FCC, SCC, SMC UART */
#define M8260_CR_OP_STOP_TX      (4)       /* FCC, SCC, SMC UART, MCC */
#define M8260_CR_OP_GR_STOP_TX   (5)       /* FCC, SCC */
#define M8260_CR_OP_RESTART_TX   (6)       /* FCC, SCC, SMC UART */
#define M8260_CR_OP_CLOSE_RX_BD  (7)       /* FCC, SCC, SMC UART, SPI, I2C */
#define M8260_CR_OP_SET_GRP_ADDR (8)       /* FCC, SCC */
#define M8260_CR_OP_SET_TIMER    (8)       /* Timer */
#define M8260_CR_OP_GCI_TIMEOUT  (9)       /* SMC GCI */
#define M8260_CR_OP_START_IDMA   (9)       /* IDMA */
#define M8260_CR_OP_STOP_RX	 	 (9)	   /* MCC */
#define M8260_CR_OP_ATM_TX	 	 (10)	   /* FCC */
#define M8260_CR_OP_RESET_BCS    (10)      /* SCC */
#define M8260_CR_OP_GCI_ABORT    (10)      /* SMC GCI */
#define M8260_CR_OP_STOP_IDMA    (11)      /* IDMA */
#define M8260_CR_OP_RANDOM	 	 (12)      /* RAND */

/*
*************************************************************************
*                 System Protection Control Register (SYPCR)            *
*************************************************************************
*/
#define M8260_SYPCR_SWTC(x)      ((x)<<16)   /* Software watchdog timer count */
#define M8260_SYPCR_BMT(x)       ((x)<<8)    /* Bus monitor timing */
#define M8260_SYPCR_BME          (1<<7)      /* Bus monitor enable */
#define M8260_SYPCR_SWF          (1<<3)      /* Software watchdog freeze */
#define M8260_SYPCR_SWE          (1<<2)      /* Software watchdog enable */
#define M8260_SYPCR_SWRI         (1<<1)      /* Watchdog reset/interrupt sel. */
#define M8260_SYPCR_SWP          (1<<0)      /* Software watchdog prescale */

/*
*************************************************************************
*                        Memory Control Registers                       *
*************************************************************************
*/
#define M8260_UPM_AMX_8col       (0<<20)   /* 8 column DRAM */
#define M8260_UPM_AMX_9col       (1<<20)   /* 9 column DRAM */
#define M8260_UPM_AMX_10col      (2<<20)   /* 10 column DRAM */
#define M8260_UPM_AMX_11col      (3<<20)   /* 11 column DRAM */
#define M8260_UPM_AMX_12col      (4<<20)   /* 12 column DRAM */
#define M8260_UPM_AMX_13col      (5<<20)   /* 13 column DRAM */
#define M8260_MSR_PER(x)         (0x100<<(7-x)) /* Perity error bank (x) */
#define M8260_MSR_WPER           (1<<7)    /* Write protection error */
#define M8260_MPTPR_PTP(x)       ((x)<<8)  /* Periodic timer prescaler */
#define M8260_BR_BA(x)           ((x)&0xffff8000) /* Base address */
#define M8260_BR_AT(x)           ((x)<<12) /* Address type */
#define M8260_BR_PS8             (1<<10)   /* 8 bit port */
#define M8260_BR_PS16            (2<<10)   /* 16 bit port */
#define M8260_BR_PS32            (0<<10)   /* 32 bit port */
#define M8260_BR_PARE            (1<<9)    /* Parity checking enable */
#define M8260_BR_WP              (1<<8)    /* Write protect */
#define M8260_BR_MS_GPCM         (0<<6)    /* GPCM */
#define M8260_BR_MS_UPMA         (2<<6)    /* UPM A */
#define M8260_BR_MS_UPMB         (3<<6)    /* UPM B */
#define M8260_MEMC_BR_V          (1<<0)    /* Base/Option register are valid */

#define M8260_MEMC_OR_32K        0xffff8000      /* Address range */
#define M8260_MEMC_OR_64K        0xffff0000
#define M8260_MEMC_OR_128K       0xfffe0000
#define M8260_MEMC_OR_256K       0xfffc0000
#define M8260_MEMC_OR_512K       0xfff80000
#define M8260_MEMC_OR_1M         0xfff00000
#define M8260_MEMC_OR_2M         0xffe00000
#define M8260_MEMC_OR_4M         0xffc00000
#define M8260_MEMC_OR_8M         0xff800000
#define M8260_MEMC_OR_16M        0xff000000
#define M8260_MEMC_OR_32M        0xfe000000
#define M8260_MEMC_OR_64M        0xfc000000
#define M8260_MEMC_OR_128        0xf8000000
#define M8260_MEMC_OR_256M       0xf0000000
#define M8260_MEMC_OR_512M       0xe0000000
#define M8260_MEMC_OR_1G         0xc0000000
#define M8260_MEMC_OR_2G         0x80000000
#define M8260_MEMC_OR_4G         0x00000000
#define M8260_MEMC_OR_ATM(x)     ((x)<<12)   /* Address type mask */
#define M8260_MEMC_OR_CSNT       (1<<11)     /* Chip select is negated early */
#define M8260_MEMC_OR_SAM        (1<<11)     /* Address lines are multiplexed */
#define M8260_MEMC_OR_ACS_NORM   (0<<9)      /* *CS asserted with addr lines */
#define M8260_MEMC_OR_ACS_QRTR   (2<<9)      /* *CS asserted 1/4 after addr */
#define M8260_MEMC_OR_ACS_HALF   (3<<9)      /* *CS asserted 1/2 after addr */
#define M8260_MEMC_OR_BI         (1<8)       /* Burst inhibit */
#define M8260_MEMC_OR_SCY(x)     ((x)<<4)    /* Cycle length in clocks */
#define M8260_MEMC_OR_SETA       (1<<3)      /* *TA generated externally */
#define M8260_MEMC_OR_TRLX       (1<<2)      /* Relaxed timing in GPCM */
#define M8260_MEMC_OR_EHTR       (1<<1)      /* Extended hold time on reads */

/*
*************************************************************************
*                         UPM Registers (MxMR)                          *
*************************************************************************
*/
#define M8260_MEMC_MMR_PTP(x)   ((x)<<24)    /* Periodic timer period */
#define M8260_MEMC_MMR_PTE      (1<<23)      /* Periodic timer enable */
#define M8260_MEMC_MMR_DSP(x)   ((x)<<17)    /* Disable timer period */
#define M8260_MEMC_MMR_G0CL(x)  ((x)<<13)    /* General line 0 control */
#define M8260_MEMC_MMR_UPWAIT   (1<<12)      /* GPL_x4 is UPWAITx */
#define M8260_MEMC_MMR_RLF(x)   ((x)<<8)     /* Read loop field */
#define M8260_MEMC_MMR_WLF(x)   ((x)<<4)     /* Write loop field */
#define M8260_MEMC_MMR_TLF(x)   ((x)<<0)     /* Timer loop field */
/*
*************************************************************************
*                         Memory Command Register (MCR)                 *
*************************************************************************
*/
#define M8260_MEMC_MCR_WRITE     (0<<30)     /* WRITE command */
#define M8260_MEMC_MCR_READ      (1<<30)     /* READ command */
#define M8260_MEMC_MCR_RUN       (2<<30)     /* RUN command */
#define M8260_MEMC_MCR_UPMA      (0<<23)     /* Cmd is for UPMA */
#define M8260_MEMC_MCR_UPMB      (1<<23)     /* Cmd is for UPMB */
#define M8260_MEMC_MCR_MB(x)     ((x)<<13)   /* Memory bank when RUN cmd */
#define M8260_MEMC_MCR_MCLF(x)   ((x)<<8)    /* Memory command loop field */
#define M8260_MEMC_MCR_MAD(x)    (x)         /* Machine address */



/*
*************************************************************************
*                         SI Mode Register (SIMODE)                     *
*************************************************************************
*/
#define M8260_SI_SMC2_BITS       0xFFFF0000	/* All SMC2 bits */
#define M8260_SI_SMC2_TDM        (1<<31) 	/* Multiplexed SMC2 */
#define M8260_SI_SMC2_BRG1       (0<<28) 	/* SMC2 clock souce */
#define M8260_SI_SMC2_BRG2       (1<<28)
#define M8260_SI_SMC2_BRG3       (2<<28)
#define M8260_SI_SMC2_BRG4       (3<<28)
#define M8260_SI_SMC2_CLK5       (0<<28)
#define M8260_SI_SMC2_CLK6       (1<<28)
#define M8260_SI_SMC2_CLK7       (2<<28)
#define M8260_SI_SMC2_CLK8       (3<<28)
#define M8260_SI_SMC1_BITS       0x0000FFFF	/* All SMC1 bits */
#define M8260_SI_SMC1_TDM        (1<<15) 	/* Multiplexed SMC1 */
#define M8260_SI_SMC1_BRG1       (0<<12) 	/* SMC1 clock souce */
#define M8260_SI_SMC1_BRG2       (1<<12)
#define M8260_SI_SMC1_BRG3       (2<<12)
#define M8260_SI_SMC1_BRG4       (3<<12)
#define M8260_SI_SMC1_CLK1       (0<<12)
#define M8260_SI_SMC1_CLK2       (1<<12)
#define M8260_SI_SMC1_CLK3       (2<<12)
#define M8260_SI_SMC1_CLK4       (3<<12)

/*
*************************************************************************
*                  SDMA Configuration Register (SDCR)                   *
*************************************************************************
*/
#define M8260_SDCR_FREEZE        (2<<13) /* Freeze on next bus cycle */
#define M8260_SDCR_RAID_5        (1<<0)  /* Normal arbitration ID */

/*
*************************************************************************
*                  SDMA Status Register (SDSR)                          *
*************************************************************************
*/
#define M8260_SDSR_SBER          (1<<7)  /* SDMA Channel bus error */
#define M8260_SDSR_DSP2          (1<<1)  /* DSP Chain 2 interrupt */
#define M8260_SDSR_DSP1          (1<<0)  /* DSP Chain 1 interrupt */

/*
*************************************************************************
*                      Baud (sic) Rate Generators                       *
*************************************************************************
*/
#define M8260_BRG_RST            (1<<17)         /* Reset generator */
#define M8260_BRG_EN             (1<<16)         /* Enable generator */
#define M8260_BRG_EXTC_BRGCLK    (0<<14)         /* Source is BRGCLK */
#define M8260_BRG_EXTC_CLK2      (1<<14)         /* Source is CLK2 pin */
#define M8260_BRG_EXTC_CLK6      (2<<14)         /* Source is CLK6 pin */
#define M8260_BRG_ATB            (1<<13)         /* Autobaud */
#define M8260_BRG_115200         (21<<1)         /* Assume 40 MHz clock */
#define M8260_BRG_57600          (32<<1)
#define M8260_BRG_38400          (64<<1)
#define M8260_BRG_19200          (129<<1)
#define M8260_BRG_9600           (259<<1)
#define M8260_BRG_4800           (520<<1)
#define M8260_BRG_2400           (1040<<1)
#define M8260_BRG_1200           (2082<<1)
#define M8260_BRG_600            ((259<<1) | 1)
#define M8260_BRG_300            ((520<<1) | 1)
#define M8260_BRG_150            ((1040<<1) | 1)
#define M8260_BRG_75             ((2080<<1) | 1)

#define M8xx_BRG_RST             (1<<17)         /* Reset generator */
#define M8xx_BRG_EN              (1<<16)         /* Enable generator */
#define M8xx_BRG_EXTC_BRGCLK     (0<<14)         /* Source is BRGCLK */

#define M8260_BRG1		(1<<7)
#define M8260_BRG2		(1<<6)
#define M8260_BRG3		(1<<5)
#define M8260_BRG4		(1<<4)
#define M8260_BRG5		(1<<3)
#define M8260_BRG6		(1<<2)
#define M8260_BRG7		(1<<1)
#define M8260_BRG8		(1<<0)



#define M8260_TGCR_CAS4          (1<<15)   /* Cascade timers 3 and 4 */
#define M8260_TGCR_CAS2          (1<<7)    /* Cascade timers 1 and 2 */
#define M8260_TGCR_FRZ1          (1<<2)    /* Halt timer if FREEZE asserted */
#define M8260_TGCR_FRZ2          (1<<6)    /* Halt timer if FREEZE asserted */
#define M8260_TGCR_FRZ3          (1<<10)   /* Halt timer if FREEZE asserted */
#define M8260_TGCR_FRZ4          (1<<14)   /* Halt timer if FREEZE asserted */
#define M8260_TGCR_STP1          (1<<1)    /* Stop timer */
#define M8260_TGCR_STP2          (1<<5)    /* Stop timer */
#define M8260_TGCR_STP3          (1<<9)    /* Stop timer */
#define M8260_TGCR_STP4          (1<<13)   /* Stop timer */
#define M8260_TGCR_RST1          (1<<0)    /* Enable timer */
#define M8260_TGCR_RST2          (1<<4)    /* Enable timer */
#define M8260_TGCR_RST3          (1<<8)    /* Enable timer */
#define M8260_TGCR_RST4          (1<<12)   /* Enable timer */
#define M8260_TGCR_GM1           (1<<3)    /* Gate Mode 1 for TMR1 or TMR2 */
#define M8260_TGCR_GM2           (1<<11)   /* Gate Mode 2 for TMR3 or TMR4 */

#define M8260_TMR_PS(x)          ((x)<<8)  /* Timer prescaler */
#define M8260_TMR_CE_RISE        (1<<6)    /* Capture on rising edge */
#define M8260_TMR_CE_FALL        (2<<6)    /* Capture on falling edge */
#define M8260_TMR_CE_ANY         (3<<6)    /* Capture on any edge */
#define M8260_TMR_OM_TOGGLE      (1<<5)    /* Toggle TOUTx pin */
#define M8260_TMR_ORI            (1<<4)    /* Interrupt on reaching reference */
#define M8260_TMR_RESTART        (1<<3)    /* Restart timer after reference */
#define M8260_TMR_ICLK_INT       (1<<1)    /* Internal clock is timer source */
#define M8260_TMR_ICLK_INT16     (2<<1)    /* Internal clock/16 is tmr src */
#define M8260_TMR_ICLK_TIN       (3<<1)    /* TIN pin is timer source */
#define M8260_TMR_TGATE          (1<<0)    /* TGATE controls timer */

#ifdef REV_0_2
#define M8260_PISCR_PS           (1<<6)    /* PIT Interrupt state */
#else
#define M8260_PISCR_PS           (1<<7)    /* PIT Interrupt state */
#endif
#define M8260_PISCR_PIE          (1<<2)    /* PIT interrupt enable */
#define M8260_PISCR_PTF          (1<<1)    /* Stop timer when freeze asserted */
#define M8260_PISCR_PTE          (1<<0)    /* PIT enable */

#if 0
#define M8260_TBSCR_TBIRQ(x)     (1<<(15-x))  /* TB interrupt level */
#define M8260_TBSCR_REFA         (1<<7)    /* TB matches TBREFF0 */
#define M8260_TBSCR_REFB         (1<<6)    /* TB matches TBREFF1 */
#define M8260_TBSCR_REFAE        (1<<3)    /* Enable ints for REFA */
#define M8260_TBSCR_REFBE        (1<<2)    /* Enable ints for REFB */
#define M8260_TBSCR_TBF          (1<<1)    /* TB stops on FREEZE */
#define M8260_TBSCR_TBE          (1<<0)    /* enable TB and decrementer */
#endif

#define M8260_TMCNTSC_SEC	 (1<<7)	   /* per second flag */
#define M8260_TMCNTSC_ALR	 (1<<6)	   /* Alarm interrupt flag */
#define M8260_TMCNTSC_SIE	 (1<<3)	   /* per second interrupt enable */
#define M8260_TMCNTSC_ALE	 (1<<2)	   /* Alarm interrupt enable */
#define M8260_TMCNTSC_TCF	 (1<<1)	   /* Time count frequency */
#define M8260_TMCNTSC_TCE	 (1<<0)	   /* Time count enable */

#define M8260_SIMASK_PC0		 (1<<31)
#define M8260_SIMASK_PC1		 (1<<30)
#define M8260_SIMASK_PC2		 (1<<29)
#define M8260_SIMASK_PC3		 (1<<28)
#define M8260_SIMASK_PC4		 (1<<27)
#define M8260_SIMASK_PC5		 (1<<26)
#define M8260_SIMASK_PC6		 (1<<25)
#define M8260_SIMASK_PC7		 (1<<24)
#define M8260_SIMASK_PC8		 (1<<23)
#define M8260_SIMASK_PC9		 (1<<22)
#define M8260_SIMASK_PC10		 (1<<21)
#define M8260_SIMASK_PC11		 (1<<20)
#define M8260_SIMASK_PC12		 (1<<19)
#define M8260_SIMASK_PC13		 (1<<18)
#define M8260_SIMASK_PC14		 (1<<17)
#define M8260_SIMASK_PC15		 (1<<16)
#define M8260_SIMASK_IRQ1		 (1<<14)
#define M8260_SIMASK_IRQ2		 (1<<13)
#define M8260_SIMASK_IRQ3		 (1<<12)
#define M8260_SIMASK_IRQ4		 (1<<11)
#define M8260_SIMASK_IRQ5		 (1<<10)
#define M8260_SIMASK_IRQ6		 (1<<9)
#define M8260_SIMASK_IRQ7		 (1<<8)
#define M8260_SIMASK_TMCNT		 (1<<2)
#define M8260_SIMASK_PIT		 (1<<1)

#define M8260_SIMASK_FCC1		 (1<<31)
#define M8260_SIMASK_FCC2		 (1<<30)
#define M8260_SIMASK_FCC3		 (1<<29)
#define M8260_SIMASK_MCC1		 (1<<27)
#define M8260_SIMASK_MCC2		 (1<<26)
#define M8260_SIMASK_SCC1		 (1<<23)
#define M8260_SIMASK_SCC2		 (1<<22)
#define M8260_SIMASK_SCC3		 (1<<21)
#define M8260_SIMASK_SCC4		 (1<<20)
#define M8260_SIMASK_I2C		 (1<<15)
#define M8260_SIMASK_SPI		 (1<<14)
#define M8260_SIMASK_RTT		 (1<<13)
#define M8260_SIMASK_SMC1		 (1<<12)
#define M8260_SIMASK_SMC2		 (1<<11)
#define M8260_SIMASK_IDMA1		 (1<<10)
#define M8260_SIMASK_IDMA2		 (1<<9)
#define M8260_SIMASK_IDMA3		 (1<<8)
#define M8260_SIMASK_IDMA4		 (1<<7)
#define M8260_SIMASK_SDMA		 (1<<6)
#define M8260_SIMASK_TIMER1		 (1<<4)
#define M8260_SIMASK_TIMER2		 (1<<3)
#define M8260_SIMASK_TIMER3		 (1<<2)
#define M8260_SIMASK_TIMER4		 (1<<1)

#define M8260_SIUMCR_EARB        (1<<31)
#define M8260_SIUMCR_EARP0       (0<<28)
#define M8260_SIUMCR_EARP1       (1<<28)
#define M8260_SIUMCR_EARP2       (2<<28)
#define M8260_SIUMCR_EARP3       (3<<28)
#define M8260_SIUMCR_EARP4       (4<<28)
#define M8260_SIUMCR_EARP5       (5<<28)
#define M8260_SIUMCR_EARP6       (6<<28)
#define M8260_SIUMCR_EARP7       (7<<28)
#define M8260_SIUMCR_DSHW        (1<<23)
#define M8260_SIUMCR_DBGC0       (0<<21)
#define M8260_SIUMCR_DBGC1       (1<<21)
#define M8260_SIUMCR_DBGC2       (2<<21)
#define M8260_SIUMCR_DBGC3       (3<<21)
#define M8260_SIUMCR_DBPC0       (0<<19)
#define M8260_SIUMCR_DBPC1       (1<<19)
#define M8260_SIUMCR_DBPC2       (2<<19)
#define M8260_SIUMCR_DBPC3       (3<<19)
#define M8260_SIUMCR_FRC         (1<<17)
#define M8260_SIUMCR_DLK         (1<<16)
#define M8260_SIUMCR_PNCS        (1<<15)
#define M8260_SIUMCR_OPAR        (1<<14)
#define M8260_SIUMCR_DPC         (1<<13)
#define M8260_SIUMCR_MPRE        (1<<12)
#define M8260_SIUMCR_MLRC0       (0<<10)
#define M8260_SIUMCR_MLRC1       (1<<10)
#define M8260_SIUMCR_MLRC2       (2<<10)
#define M8260_SIUMCR_MLRC3       (3<<10)
#define M8260_SIUMCR_AEME        (1<<9)
#define M8260_SIUMCR_SEME        (1<<8)
#define M8260_SIUMCR_BSC         (1<<7)
#define M8260_SIUMCR_GB5E        (1<<6)
#define M8260_SIUMCR_B2DD        (1<<5)
#define M8260_SIUMCR_B3DD        (1<<4)

/*
*************************************************************************
*                 MPC8260 DUAL-PORT RAM AND REGISTERS                   *
*************************************************************************
*/
typedef struct m8260_ {

  /*
   * CPM Dual-Port RAM
   */
  rtems_unsigned8   dpram1[16384];		/* 0x0000 - 0x3FFF BD/data/ucode */
  rtems_unsigned8	cpm_pad0[16384];	/* 0x4000 - 0x7FFF Reserved      */

  m8260SCCparms_t	scc1p;
  rtems_unsigned8	pad_scc1[256-sizeof(m8260SCCparms_t)];
  m8260SCCparms_t	scc2p;
  rtems_unsigned8	pad_scc2[256-sizeof(m8260SCCparms_t)];
  m8260SCCparms_t	scc3p;
  rtems_unsigned8	pad_scc3[256-sizeof(m8260SCCparms_t)];
  m8260SCCparms_t	scc4p;
  rtems_unsigned8	pad_scc4[256-sizeof(m8260SCCparms_t)];

  m8260FCCparms_t	fcc1p;
  rtems_unsigned8	pad_fcc1[256-sizeof(m8260FCCparms_t)];
  m8260FCCparms_t	fcc2p;
  rtems_unsigned8	pad_fcc2[256-sizeof(m8260FCCparms_t)];
  m8260FCCparms_t	fcc3p;
  rtems_unsigned8	pad_fcc3[256-sizeof(m8260FCCparms_t)];

  rtems_unsigned8	mcc1p[128];
  rtems_unsigned8	pad_mcc1[124];
  rtems_unsigned16	smc1_base;
  rtems_unsigned16	idma1_base;
  rtems_unsigned8	mcc2p[128];
  rtems_unsigned8	pad_mcc2[124];
  rtems_unsigned16	smc2_base;
  rtems_unsigned16	idma2_base;
  rtems_unsigned8	pad_spi[252];
  rtems_unsigned16	spi_base;
  rtems_unsigned16	idma3_base;
  rtems_unsigned8	pad_risc[224];
  rtems_unsigned8	risc_timers[16];
  rtems_unsigned16	rev_num;
  rtems_unsigned16	cpm_pad7;
  rtems_unsigned32	cpm_pad8;
  rtems_unsigned16	rand;
  rtems_unsigned16	i2c_base;
  rtems_unsigned16	idma4_base;
  rtems_unsigned8	cpm_pad9[1282];

  rtems_unsigned8	cpm_pad1[8192];		/* 0x9000 - 0xAFFF Reserved      */

  m8260SMCparms_t	smc1p;
  m8260SMCparms_t	smc2p;
  rtems_unsigned8	dpram3[4096-2*sizeof(m8260SMCparms_t)];

  rtems_unsigned8	cpm_pad2[16384];	/* 0xC000 - 0xFFFF Reserved      */

        
  /*
   * General SIU Block
   */
  rtems_unsigned32      siumcr;
  rtems_unsigned32      sypcr;
  rtems_unsigned8       siu_pad0[6];
  rtems_unsigned16      swsr;
  rtems_unsigned8       siu_pad1[20];
  rtems_unsigned32		bcr;
  rtems_unsigned8		ppc_acr;
  rtems_unsigned8		siu_pad4[3];
  rtems_unsigned32		ppc_alrh;
  rtems_unsigned32		ppc_alr1;
  rtems_unsigned8		lcl_acr;
  rtems_unsigned8		siu_pad5[3];
  rtems_unsigned32		lcl_alrh;
  rtems_unsigned32		lcl_alr1;
  rtems_unsigned32		tescr1;
  rtems_unsigned32		tescr2;
  rtems_unsigned32		l_tescr1;
  rtems_unsigned32		l_tescr2;
  rtems_unsigned32		pdtea;
  rtems_unsigned8		pdtem;
  rtems_unsigned8		siu_pad2[3];
  rtems_unsigned32		ldtea;
  rtems_unsigned8		ldtem;
  rtems_unsigned8		siu_pad3[163];


  /*
   * Memory Controller Block
   */
  m8260MEMCRegisters_t  memc[12];
  rtems_unsigned8       mem_pad0[8];
  rtems_unsigned32      mar;
  rtems_unsigned8       mem_pad1[4];
  rtems_unsigned32      mamr;
  rtems_unsigned32      mbmr;
  rtems_unsigned32      mcmr;
  rtems_unsigned32		mdmr;
  rtems_unsigned8       mem_pad2[4];
  rtems_unsigned16      mptpr;
  rtems_unsigned8		mem_pad5[2];
  rtems_unsigned32      mdr;
  rtems_unsigned8       mem_pad3[4];
  rtems_unsigned32      psdmr;
  rtems_unsigned32      lsdmr;
  rtems_unsigned8		purt;
  rtems_unsigned8		mem_pad6[3];
  rtems_unsigned8		psrt;
  rtems_unsigned8		mem_pad7[3];
  rtems_unsigned8		lurt;
  rtems_unsigned8		mem_pad8[3];
  rtems_unsigned8		lsrt;
  rtems_unsigned8		mem_pad9[3];
  rtems_unsigned32      immr;
  rtems_unsigned8       mem_pad4[84];

  
  /*
   * System integration timers
   */
  rtems_unsigned8		sit_pad0[32];
  rtems_unsigned16      tmcntsc;
  rtems_unsigned8		sit_pad6[2];
  rtems_unsigned32      tmcnt;
  rtems_unsigned32		tmcntsec;
  rtems_unsigned32      tmcntal;
  rtems_unsigned8		sit_pad2[16];
  rtems_unsigned16      piscr;
  rtems_unsigned8		sit_pad5[2];
  rtems_unsigned32      pitc;
  rtems_unsigned32      pitr;
  rtems_unsigned8		sit_pad3[94];
  rtems_unsigned8		sit_pad4[2390];

  
  /*
   * Interrupt Controller
   */
  rtems_unsigned16		sicr;
  rtems_unsigned8		ict_pad1[2];
  rtems_unsigned32		sivec;
  rtems_unsigned32		sipnr_h;
  rtems_unsigned32		sipnr_l;
  rtems_unsigned32		siprr;
  rtems_unsigned32		scprr_h;
  rtems_unsigned32		scprr_l;
  rtems_unsigned32		simr_h;
  rtems_unsigned32		simr_l;
  rtems_unsigned32		siexr;
  rtems_unsigned8		ict_pad0[88];


  /*
   * Clocks and Reset
   */
  rtems_unsigned32      sccr;
  rtems_unsigned8		clr_pad1[4];
  rtems_unsigned32      scmr;
  rtems_unsigned8		clr_pad2[4];
  rtems_unsigned32      rsr;
  rtems_unsigned32      rmr;
  rtems_unsigned8       clr_pad0[104];
  

  /*
   * Input/ Output Port
   */
  rtems_unsigned32      pdira;
  rtems_unsigned32      ppara;
  rtems_unsigned32      psora;
  rtems_unsigned32      podra;
  rtems_unsigned32      pdata;
  rtems_unsigned8       iop_pad0[12];
  rtems_unsigned32      pdirb;
  rtems_unsigned32      pparb;
  rtems_unsigned32      psorb;
  rtems_unsigned32      podrb;
  rtems_unsigned32      pdatb;
  rtems_unsigned8       iop_pad1[12];
  rtems_unsigned32      pdirc;
  rtems_unsigned32      pparc;
  rtems_unsigned32      psorc;
  rtems_unsigned32      podrc;
  rtems_unsigned32      pdatc;
  rtems_unsigned8       iop_pad2[12];
  rtems_unsigned32      pdird;
  rtems_unsigned32      ppard;
  rtems_unsigned32      psord;
  rtems_unsigned32      podrd;
  rtems_unsigned32      pdatd;
  rtems_unsigned8       iop_pad3[12];


  /*
   * CPM Timers
   */
  rtems_unsigned8       tgcr1;
  rtems_unsigned8       cpt_pad0[3];
  rtems_unsigned8       tgcr2;
  rtems_unsigned8       cpt_pad1[11];
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
  rtems_unsigned8       cpt_pad2[608];


  /*
   * DMA Block
   */
  rtems_unsigned8       sdsr;
  rtems_unsigned8       dma_pad0[3];
  rtems_unsigned8       sdmr;
  rtems_unsigned8       dma_pad1[3];

  rtems_unsigned8       idsr1;
  rtems_unsigned8       dma_pad2[3];
  rtems_unsigned8       idmr1;
  rtems_unsigned8       dma_pad3[3];
  rtems_unsigned8       idsr2;
  rtems_unsigned8       dma_pad4[3];
  rtems_unsigned8       idmr2;
  rtems_unsigned8       dma_pad5[3];
  rtems_unsigned8       idsr3;
  rtems_unsigned8       dma_pad6[3];
  rtems_unsigned8       idmr3;
  rtems_unsigned8       dma_pad7[3];
  rtems_unsigned8       idsr4;
  rtems_unsigned8       dma_pad8[3];
  rtems_unsigned8       idmr4;
  rtems_unsigned8       dma_pad9[707];

  
  /*
   * FCC Block
   */
  m8260FCCRegisters_t   fcc1;
  m8260FCCRegisters_t   fcc2;
  m8260FCCRegisters_t   fcc3;

  rtems_unsigned8		fcc_pad0[656];

  /*
   * BRG 5-8 Block
   */
  rtems_unsigned32      brgc5;
  rtems_unsigned32      brgc6;
  rtems_unsigned32      brgc7;
  rtems_unsigned32      brgc8;
  rtems_unsigned8		brg_pad0[608];


  /*
   * I2C
   */
  rtems_unsigned8       i2mod;
  rtems_unsigned8       i2m_pad0[3];
  rtems_unsigned8       i2add;
  rtems_unsigned8       i2m_pad1[3];
  rtems_unsigned8       i2brg;
  rtems_unsigned8       i2m_pad2[3];
  rtems_unsigned8       i2com;
  rtems_unsigned8       i2m_pad3[3];
  rtems_unsigned8       i2cer;
  rtems_unsigned8       i2m_pad4[3];
  rtems_unsigned8       i2cmr;
  rtems_unsigned8       i2m_pad5[331];
  
  
  /*
   * CPM Block
   */
  rtems_unsigned32      cpcr;
  rtems_unsigned32      rccr;
  rtems_unsigned8       cpm_pad3[14];
  rtems_unsigned16      rter;
  rtems_unsigned8		cpm_pad[2];
  rtems_unsigned16      rtmr;
  rtems_unsigned16      rtscr;
  rtems_unsigned8       cpm_pad4[2];
  rtems_unsigned32      rtsr;
  rtems_unsigned8       cpm_pad5[12];
  

  /*
   * BRG 1-4 Block
   */
  rtems_unsigned32      brgc1;
  rtems_unsigned32      brgc2;
  rtems_unsigned32      brgc3;
  rtems_unsigned32      brgc4;
  

  /*
   * SCC Block
   */
  m8260SCCRegisters_t   scc1;
  m8260SCCRegisters_t   scc2;
  m8260SCCRegisters_t   scc3;
  m8260SCCRegisters_t   scc4;

  
  /*
   * SMC Block
   */
  m8260SMCRegisters_t    smc1;
  m8260SMCRegisters_t    smc2;

  
  /*
   * SPI Block
   */
  rtems_unsigned16      spmode;
  rtems_unsigned8       spi_pad0[4];
  rtems_unsigned8       spie;
  rtems_unsigned8       spi_pad1[3];
  rtems_unsigned8       spim;
  rtems_unsigned8       spi_pad2[2];
  rtems_unsigned8       spcom;
  rtems_unsigned8       spi_pad3[82];

  
  /*
   * CPM Mux Block
   */
  rtems_unsigned8       cmxsi1cr;
  rtems_unsigned8       cmx_pad0[1];
  rtems_unsigned8       cmxsi2cr;
  rtems_unsigned8       cmx_pad1[1];
  rtems_unsigned32      cmxfcr;
  rtems_unsigned32      cmxscr;
  rtems_unsigned8       cmxsmr;
  rtems_unsigned8       cmx_pad2[1];
  rtems_unsigned16      cmxuar;
  rtems_unsigned8       cmx_pad3[16];


  /*
   * SI & MCC Blocks
   */
  m8260SIRegisters_t	si1;
  m8260MCCRegisters_t	mcc1;
  m8260SIRegisters_t	si2;
  m8260MCCRegisters_t	mcc2;

  rtems_unsigned8	mcc_pad0[1152];

  /*
   * SI1 RAM
   */
  rtems_unsigned8		si1txram[512];
  rtems_unsigned8		ram_pad0[512];
  rtems_unsigned8		si1rxram[512];
  rtems_unsigned8		ram_pad1[512];


  /*
   * SI2 RAM
   */
  rtems_unsigned8		si2txram[512];
  rtems_unsigned8		ram_pad2[512];
  rtems_unsigned8		si2rxram[512];
  rtems_unsigned8		ram_pad3[512];


} m8260_t;

extern volatile m8260_t m8260;
#endif /* ASM */

#endif /* __MPC8260_h */
