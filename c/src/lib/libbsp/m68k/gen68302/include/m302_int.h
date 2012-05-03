/*
 *  Implements the Motorola 68302 multi-protocol chip parameter
 *  definition header.
 */

#ifndef __M302_INT_h
#define __M302_INT_h

#ifndef uchar
#define uchar unsigned char
#endif
#ifndef ushort
#define ushort unsigned short
#endif
#ifndef ulong
#define ulong unsigned long
#endif

/* Ethernet Control Register ECNTRL */
#define ECNTRL_BIT_RESET    0x0001
#define ECNTRL_BIT_ETHER_EN  0x0002
#define ECNTRL_BIT_GTS      0x0004

/* Ethernet DMA Configuration Status Register EDMA */
#define EDMA_BDERR_MASK      0xFE00

#define EDMA_BDSIZE_MASK      0x00C0
#define EDMA_BDSIZE_8T_120R    0x0000
#define EDMA_BDSIZE_16T_112R  0x0040
#define EDMA_BDSIZE_32T_96R    0x0080
#define EDMA_BDSIZE_64T_64R    0x00C0

#define EDMA_BIT_TSRLY      0x0020

#define EDMA_WMRK_MASK      0x0018
#define EDMA_WMRK_8FIFO      0x0000
#define EDMA_WMRK_16FIFO    0x0008
#define EDMA_WMRK_24FIFO    0x0010
#define EDMA_WMRK_32FIFO    0x0018

#define EDMA_BLIM_MASK      0x0007
#define EDMA_BLIM_8ACCESS    0x0003

/* Ethernet Maximum Receive Buffer Length EMRBLR */
#define EMRBLR_MASK          0x07FFE

/* Interrupt Vector Register IVEC */
#define IVEC_BIT_VG          0x0100
#define IVEC_INV_MASK        0x00FF

/* Interrupt Event Register INTR_EVENT */
#define INTR_EVENT_BIT_RXB      0x0001
#define INTR_EVENT_BIT_TXB      0x0002
#define INTR_EVENT_BIT_BSY      0x0004
#define INTR_EVENT_BIT_RFINT    0x0008
#define INTR_EVENT_BIT_TFINT    0x0010
#define INTR_EVENT_BIT_EBERR    0x0020
#define INTR_EVENT_BIT_BOD      0x0040
#define INTR_EVENT_BIT_GRA      0x0080
#define INTR_EVENT_BIT_BABT      0x0100
#define INTR_EVENT_BIT_BABR      0x0200
#define INTR_EVENT_BIT_HBERR    0x0400

/* Interrupt Mask Register INTR_MASK */
#define INTR_MASK_BIT_RXIEN      0x0001
#define INTR_MASK_BIT_TXIEN      0x0002
#define INTR_MASK_BIT_BSYEN      0x0004
#define INTR_MASK_BIT_RFIEN      0x0008
#define INTR_MASK_BIT_TFIEN      0x0010
#define INTR_MASK_BIT_EBERREN    0x0020
#define INTR_MASK_BIT_BODEN      0x0040
#define INTR_MASK_BIT_GRAEN      0x0080
#define INTR_MASK_BIT_BTEN      0x0100
#define INTR_MASK_BIT_BREN      0x0200
#define INTR_MASK_BIT_HBEEN      0x0400

/* Ethernet Configuration ECNFIG */
#define ECNFIG_BIT_LOOP          0x0001
#define ECNFIG_BIT_FDEN          0x0002
#define ECNFIG_BIT_HBC          0x0004
#define ECNFIG_BIT_RDT          0x0008

/* Ethernet Test ETHER_TEST */
#define ETHER_TEST_BIT_TWS      0x0001
#define ETHER_TEST_BIT_RWS      0x0002
#define ETHER_TEST_BIT_DRTY      0x0004
#define ETHER_TEST_BIT_COLL      0x0008
#define ETHER_TEST_BIT_SLOT      0x0010
#define ETHER_TEST_BIT_TRND      0x0020
#define ETHER_TEST_BIT_TBO      0x0040
#define ETHER_TEST_BIT_RNGT      0x0080
#define ETHER_TEST_REV_MASK      0xF000

/* Ethernet AR Control Registere AR_CNTRL */
#define AR_CNTRL_BIT_PROM        0x0400
#define AR_CNTRL_BIT_PA_REJ      0x0800
#define AR_CNTRL_BIT_NO_BROADCAST  0x1000
#define AR_CNTRL_BIT_MULTI1      0x2000
#define AR_CNTRL_BIT_INDEX_EN    0x4000
#define AR_CNTRL_BIT_HASH_EN    0x8000

#define AR_CNTRL_MULTI_MASK      (AR_CNTRL_BIT_MULTI0 | AR_CNTRL_BIT_MULTI1)

/* Ethernet buffer Status TX */
#define BUF_STAT_CARRIER_LOST    0x0001
#define BUF_STAT_UNDERRUN        0x0002
#define BUF_STAT_RETRANS_COUNT  0x003C
#define BUF_STAT_RETRY_LIMIT    0x0040
#define BUF_STAT_LATE_COLLISION  0x0080
#define BUF_STAT_HEARTBIT        0x0100
#define BUF_STAT_DEFER          0x0200
#define BUF_STAT_TX_CRC          0x0400
#define BUF_STAT_LAST            0x0800
#define BUF_STAT_INTERRUPT      0x1000
#define BUF_STAT_WRAP            0x2000
#define BUF_STAT_TO              0x4000
#define BUF_STAT_READY          0x8000

/* Ethernet buffer Status RX */
#define BUF_STAT_COLLISION      0x0001
#define BUF_STAT_OVERRUN        0x0002
#define BUF_STAT_CRC_ERROR      0x0004
#define BUF_STAT_SHORT          0x0008
#define BUF_STAT_NONALIGNED      0x0010
#define BUF_STAT_LONG            0x0020
#define BUF_STAT_FIRST_IN_FRAME  0x0400
#define BUF_STAT_EMPTY          0x8000

  /* SCC Buffer Descriptor structure
  ----------------------------------*/

struct m68302_scc_bd {
  ushort      stat_ctrl;
  ushort      data_lgth;
  uchar      *p_buffer;
};

#define M68302_scc_bd_stat_ctrl(p) \
    (((struct m68302_scc_bd *)(p)) -> stat_ctrl)
#define M68302_scc_bd_data_lgth(p) \
    (((struct m68302_scc_bd *)(p)) -> data_lgth)
#define M68302_scc_bd_p_buffer(p)  \
   (((struct m68302_scc_bd *)(p)) -> p_buffer)

struct m68302_imp {

/* BASE : user data memory  */

  uchar  user_data[0x240];        /* 0x240 bytes user data */
  uchar  user_reserved[0x1c0];    /* empty till 0x400 */

/* BASE + 400H: PARAMETER RAM */
  struct {
    struct m68302_scc_bd scc_bd_rx[8];  /* Rx buffer descriptors */
    struct m68302_scc_bd scc_bd_tx[8];  /* Tx buffer descriptors */

    uchar      rfcr;        /* Rx function code */
    uchar      tfcr;        /* Tx function code */
    ushort     mrblr;        /* maximum Rx buffer length */
    ushort     rist;        /* internal state */
    uchar      res1;
    uchar      rbdn;        /* Rx internal buffer number */
    ulong      ridp;
    ushort     ribc;
    ushort     rtmp;
    ushort     tist;
    uchar      res2;
    uchar      tbdn;        /* Tx internal buffer number */
    ulong      tidp;
    ushort     tibc;
    ushort     ttmp;

    unsigned char  scc_spp  [0x64];    /* SCC specific parameters */
  } parm_ram [3];

  uchar reserved_1 [0x100];

/* BASE + 800H: INTERNAL REGISTERS */

    /* DMA */

  ushort  dma_res1;           /* reserved */
  ushort  dma_mode;           /* dma mode reg */
  ulong   dma_src;            /* dma source */
  ulong   dma_dest;           /* dma destination */
  ushort  dma_count;          /* dma byte count */
  uchar   dma_status;         /* dma status */
  uchar   dma_res2;           /* reserved */
  uchar   dma_fct_code;       /* dma function code */
  uchar   dma_res3;           /* reserved */

    /* Interrupt Controller */

  ushort  it_mode;            /* interrupt mode register */
  ushort  it_pending;         /* interrupt pending register */
  ushort  it_mask;            /* interrupt mask register */
  ushort  it_inservice;       /* interrupt in service register */
  ulong   it_reserved;        /* reserved */

    /* Parallel I/O */

  struct {
    ushort  control;         /* port control register */
    ushort  direction;       /* port data direction register */
    ushort  data;            /* port data value register */
  } port[2];

  ushort  p_reserved;        /* reserved */

    /* Chip Select */

  ulong  cs_reserved;

  struct {
    ushort  base;            /* chip select base register */
    ushort  option;          /* chip select option register */
  } cs[4];

    /* Timer */

  ushort  t1_mode;             /* timer 1 mode register */
  ushort  t1_reference;        /* timer 1 reference register */
  ushort  t1_capture;          /* timer 1 capture register */
  ushort  t1_counter;          /* timer 1 counter */
  uchar   tim_res1;            /* reserved */
  uchar   t1_event;            /* timer 1 event */

  ushort  t3_reference;        /* timer 3 reference register */
  ushort  t3_counter;          /* timer 3 counter */
  ushort  tim_res2;            /* reserved */

  ushort  t2_mode;              /* timer 2 mode register */
  ushort  t2_reference;         /* timer 2 reference register */
  ushort  t2_capture;           /* timer 2 capture register */
  ushort  t2_counter;           /* timer 2 counter */
  uchar   tim_res3;             /* reserved */
  uchar   t2_event;             /* timer 2 event */
  ushort  tim_res4[3];          /* reserved */

    /* command register */

  uchar  cp_cmd;                /* communication processor command register */
  uchar  cp_cmd_res;            /* reserved */

    /* reserved */

  uchar  reserved_2[0x1e];

    /* SCC registers */

  struct scc_regs {
    ushort  resvd;           /* reserved */
    ushort  scon;            /* SCC configuration register */
    ushort  scm;             /* SCC mode register */
    ushort  dsr;             /* SCC sync register */
    uchar   scce;            /* SCC event register */
    uchar   res1;            /* reserved */
    uchar   sccm;            /* SCC mask register */
    uchar   res2;            /* reserved */
    uchar   sccs;            /* SCC status register */
    uchar   res3;            /* reserved */
    ushort  res4;            /* reserved */
  } scc_regs[3];

    /* SP (SCP + SMI) */

  ushort  scc_mode_reg;          /* scp, smi mode + clock control */

    /* Serial Interface */

  ushort  serial_int_mask;        /* mask register */
  ushort  serial_int_mode;        /* mode register */

    /* reserved */

  uchar  reserved_3[0x74A];

/****************** 68 EN 302 specific registers **********************/
/** only available here if
    M68302_INTERNAL_RAM_BASE_ADD+0x1000=M68EN302_INTERNAL_RAM_BASE_ADD*/

    /* Module Bus Control Registers */

  ushort  mbc;              /* module bus control register MBC */
  ushort  ier;              /* interrupt extension register IER */
  ushort  cser[4];          /* Chip Select extension registers CSERx */
  ushort  pcsr;             /* parity control & status register PCSR */

  ushort  mbc_reserved;

    /* DRAM Controller Registers */

  ushort  dcr;              /* DRAM Configuration register DCR */
  ushort  drfrsh;           /* DRAM Refresh register DRFRSH */
  ushort  dba[2];           /* DRAM Bank Base Address Register */

  uchar  dram_reserved[0x7E8];

    /* Ethernet Controller Registers */

  ushort  ecntrl;            /* Ethernet Control Register */
  ushort  edma;              /* Ethernet DMA Configuration Register */
  ushort  emrblr;            /* Ethernet Max receive buffer length */
  ushort  intr_vect;         /* Interruppt vector register */
  ushort  intr_event;        /* Interruppt event register */
  ushort  intr_mask;         /* Interruppt mask register */
  ushort  ecnfig;            /* Ethernet Configuration */
  ushort  ether_test;        /* Ethernet Test register */
  ushort  ar_cntrl;          /* Address Recognition Control register */

  uchar  eth_reserved[0x1EE];

  uchar  cet[0x200];                  /* CAM Entry Table */
  struct m68302_scc_bd eth_bd[128];   /* Ethernet Buffer Descriptors Table */

};

#define M68302imp_     a_m68302_imp ->

#define M68302imp_a_scc_bd_rx(scc,bd) \
     (struct m68302_scc_bd FAR *)(&(M68302imp_ parm_ram[scc].scc_bd_rx[bd]))
#define M68302imp_a_scc_bd_tx(scc,bd) \
     (struct m68302_scc_bd FAR *)(&(M68302imp_ parm_ram[scc].scc_bd_tx[bd]))

#define M68302imp_scc_rfcr(scc)      (M68302imp_ parm_ram[scc].rfcr)
#define M68302imp_scc_tfcr(scc)      (M68302imp_ parm_ram[scc].tfcr)
#define M68302imp_scc_mrblr(scc)     (M68302imp_ parm_ram[scc].mrblr)
#define M68302imp_scc_rbdn(scc)      (M68302imp_ parm_ram[scc].rbdn)
#define M68302imp_scc_tbdn(scc)      (M68302imp_ parm_ram[scc].tbdn)

#define M68302imp_a_scc_spp(scc)    ((struct m68302_scc_spp FAR *)(M68302imp_ parm_ram[scc].scc_spp))

#define M68302imp_dma_res1        (M68302imp_ dma_res1)
#define M68302imp_dma_mode        (M68302imp_ dma_mode)
#define M68302imp_dma_src         (M68302imp_ dma_src)
#define M68302imp_dma_dest        (M68302imp_ dma_dest)
#define M68302imp_dma_count       (M68302imp_ dma_count)
#define M68302imp_dma_status      (M68302imp_ dma_status)
#define M68302imp_dma_fct_code    (M68302imp_ dma_fct_code)

#define M68302imp_it_mode         (M68302imp_ it_mode)
#define M68302imp_it_pending      (M68302imp_ it_pending)
#define M68302imp_it_mask         (M68302imp_ it_mask)
#define M68302imp_it_inservice    (M68302imp_ it_inservice)

#define M68302imp_cs_base(i)      (M68302imp_ cs[i].base)
#define M68302imp_cs_option(i)    (M68302imp_ cs[i].option)

#define M68302imp_port_control(i)    (M68302imp_ port[i].control)
#define M68302imp_port_direction(i)  (M68302imp_ port[i].direction)
#define M68302imp_port_data(i)       (M68302imp_ port[i].data)

#define M68302imp_timer1_mode        (M68302imp_ t1_mode)
#define M68302imp_timer1_reference   (M68302imp_ t1_reference)
#define M68302imp_timer1_capture     (M68302imp_ t1_capture)
#define M68302imp_timer1_counter     (M68302imp_ t1_counter)
#define M68302imp_timer1_event       (M68302imp_ t1_event)
#define M68302imp_timer3_reference   (M68302imp_ t3_reference)
#define M68302imp_timer3_counter     (M68302imp_ t3_counter)
#define M68302imp_timer2_mode        (M68302imp_ t2_mode)
#define M68302imp_timer2_reference   (M68302imp_ t2_reference)
#define M68302imp_timer2_capture     (M68302imp_ t2_capture)
#define M68302imp_timer2_counter     (M68302imp_ t2_counter)
#define M68302imp_timer2_event       (M68302imp_ t2_event)

#define M68302imp_cp_cmd             (M68302imp_ cp_cmd)

#define M68302imp_scc_mode_reg       (M68302imp_ scc_mode_reg)

#define M68302imp_serial_int_mask    (M68302imp_ serial_int_mask)
#define M68302imp_serial_int_mode    (M68302imp_ serial_int_mode)
#define M68302imp_simask             (M68302imp_serial_int_mask)
#define M68302imp_simode             (M68302imp_serial_int_mode)

#define M68302imp_scon(i)            (M68302imp_ scc_regs[i].scon)
#define M68302imp_scm(i)             (M68302imp_ scc_regs[i].scm)
#define M68302imp_dsr(i)             (M68302imp_ scc_regs[i].dsr)
#define M68302imp_scce(i)            (M68302imp_ scc_regs[i].scce)
#define M68302imp_sccm(i)            (M68302imp_ scc_regs[i].sccm)
#define M68302imp_sccs(i)            (M68302imp_ scc_regs[i].sccs)

/*----------------------------------------------------------------------------*/

#define M68en302imp_mbc              (M68302imp_ mbc)
#define M68en302imp_ier              (M68302imp_ ier)
#define M68en302imp_cser(i)          (M68302imp_ cser[i])
#define M68en302imp_pcsr             (M68302imp_ pcsr)

#define M68en302imp_dcr              (M68302imp_ dcr)
#define M68en302imp_drfrsh           (M68302imp_ drfrsh)
#define M68en302imp_dba(i)           (M68302imp_ dba[i])

#define M68en302imp_ecntrl           (M68302imp_ ecntrl)
#define M68en302imp_edma             (M68302imp_ edma)
#define M68en302imp_emrblr           (M68302imp_ emrblr)
#define M68en302imp_intr_vect        (M68302imp_ intr_vect)
#define M68en302imp_intr_event       (M68302imp_ intr_event)
#define M68en302imp_intr_mask        (M68302imp_ intr_mask)
#define M68en302imp_ecnfig           (M68302imp_ ecnfig)
#define M68en302imp_ether_test       (M68302imp_ ether_test)
#define M68en302imp_ar_cntrl         (M68302imp_ ar_cntrl)

#define M68en302imp_cet              (M68302imp_ cet)

#define M68302imp_a_eth_bd(bd) \
    (struct m68302_scc_bd *)(&(M68302imp_ eth_bd[bd]))

/* PORTS */

#define PA0     0x0001    /* PORT A bit 0 */
#define PA1     0x0002    /* PORT A bit 1 */
#define PA2     0x0004    /* PORT A bit 2 */
#define PA3     0x0008    /* PORT A bit 3 */
#define PA4     0x0010    /* PORT A bit 4 */
#define PA5     0x0020    /* PORT A bit 5 */
#define PA6     0x0040    /* PORT A bit 6 */
#define PA7     0x0080    /* PORT A bit 7 */
#define PA8     0x0100    /* PORT A bit 8 */
#define PA9     0x0200    /* PORT A bit 9 */
#define PA10    0x0400    /* PORT A bit 10 */
#define PA11    0x0800    /* PORT A bit 11 */
#define PA12    0x1000    /* PORT A bit 12 */
#define PA13    0x2000    /* PORT A bit 13 */
#define PA14    0x4000    /* PORT A bit 14 */
#define PA15    0x8000    /* PORT A bit 15 */

#define PB0     0x0001    /* PORT B bit 0 */
#define PB1     0x0002    /* PORT B bit 1 */
#define PB2     0x0004    /* PORT B bit 2 */
#define PB3     0x0008    /* PORT B bit 3 */
#define PB4     0x0010    /* PORT B bit 4 */
#define PB5     0x0020    /* PORT B bit 5 */
#define PB6     0x0040    /* PORT B bit 6 */
#define PB7     0x0080    /* PORT B bit 7 */
#define PB8     0x0100    /* PORT B bit 8 */
#define PB9     0x0200    /* PORT B bit 9 */
#define PB10    0x0400    /* PORT B bit 10 */

#define PB11    0x0800    /* PORT B bit 11 */

/* MODULE BUS CONTROL (MBCTL) */

#define MBC_BCE   0x8000
#define MBC_MFC2  0x4000
#define MBC_MFC1  0x2000
#define MBC_MFC0  0x1000
#define MBC_BB    0x0800
#define MBC_PPE   0x0400
#define MBC_PM9   0x0200
#define MBC_PM8   0x0100
#define MBC_PM7   0x0080
#define MBC_PM6   0x0040
#define MBC_PM5   0x0020
#define MBC_PM4   0x0010
#define MBC_PM3   0x0008
#define MBC_PM2   0x0004
#define MBC_PM1   0x0002
#define MBC_PM0   0x0001

  /* DRAM CONFIGURATION REG (DCR) */

#define DCR_SU0   0x0001
#define DCR_SU1   0x0002
#define DCR_WP0   0x0004
#define DCR_WP1   0x0008
#define DCR_W0    0x0010
#define DCR_W1    0x0020
#define DCR_P0    0x0040
#define DCR_P1    0x0080
#define DCR_PE0   0x0100
#define DCR_PE1   0x0200
#define DCR_E0    0x0400
#define DCR_E1    0x0800

/* M68302 INTERNAL RAM BASE ADDRESS INSTALLATION */

#define M68302_ram_base_add_install(base_reg_add,ram_base_add) \
  do { \
    *((ushort *)base_reg_add) =  (ushort)(ram_base_add >> 12); \
   a_m68302_imp = (struct m68302_imp *)ram_base_add; \
  } while (0)

#define M68302_system_ctrl_reg_install(val) (*((ulong *)M68302_SCR_ADD) = val)

  /* INTERRUPTION */

  /* Interrupt mode selection */

#define M68302_it_mode_install(mode,vector_bank, \
  extvect1,extvect6,extvect7,edgetrig1,edgetrig6,edgetrig7) \
    M68302imp_it_mode = 0 | (mode << 15) | (vector_bank << 5) | \
        (extvect7 << 14) | (extvect6 << 13) | (extvect1 <<12) | \
        (edgetrig7 << 10) | (edgetrig6 << 9)|(edgetrig1 << 8)

  /* CHIP SELECTION */

  /* 'read_write' support values :
   *
   *  M68302_CS_READ_ONLY      for read only memory access chip select
   *  M68302_CS_READ_WRITE_ONLY  for write only memory access chip select
   *  M68302_CS_READ_AND_WRITE  for read & write memory access chip select
   *
   *  'nb_wait_state' : number of wait-state(s) from 0 to 6, 7 for external
   *
   */
#define M68302_CS_READ_ONLY      0x02 /* read only memory access */
#define M68302_CS_WRITE_ONLY     0x22 /* write only memory access */
#define M68302_CS_READ_AND_WRITE 0x00 /* read and write memory access */

#define M68302_cs_install(cs_nb,base_add,range,nb_wait_state,read_write) \
  do { \
    M68302imp_cs_option(cs_nb) = (((~(range - 1)) >> 11) & 0x1FFC) | \
      (nb_wait_state << 13) | (read_write & 0x2); \
    M68302imp_cs_base(cs_nb) = (((base_add >> 11) & 0x1FFC) | \
      ((read_write >> 4) & 0x2) | 1); \
  } while (0)

#define M68302_set_cs_br(base_add, read_write)  \
    ((((base_add) >> 11) & 0x1FFC) | (((read_write) >> 4) & 0x2) | 1)

#define M68302_set_cs_or(range, nb_wait_state, read_write) \
   ((((~(range - 1)) >> 11) & 0x1FFC) | \
       ((nb_wait_state) << 13) | ((read_write) & 0x2))

#define M68302_get_cs_br(cs)  \
    (((ulong)((M68302imp_cs_base(cs)) & 0x1FFC)) << 11 )

/* DRAM */

#define M68en302_dram_install(bank,base_add,range)  \
  M68en302imp_dba (bank)  = \
    (((base_add >> 8) & 0xFE00) | (((~(range-1))>>16) & 0x007E) | 1)

#endif
