/*
 *  i960RP Related Definitions.
 *
 *  NOTE:  There is some commonality with the JX series which is
 *         not currently supported by RTEMS.
 *
 *  $Id$
 */

#ifndef __I960RP_h
#define __I960RP_h

/* i960RP control structures */

/* Intel i960RP Control Table */

typedef struct {
                            /* Control Group 0 */
  unsigned int rsvd00;
  unsigned int rsvd01;
  unsigned int rsvd02;
  unsigned int rsvd03;
                            /* Control Group 1 */
  unsigned int imap0;             /* interrupt map 0 */
  unsigned int imap1;             /* interrupt map 1 */
  unsigned int imap2;             /* interrupt map 2 */
  unsigned int icon;              /* interrupt control */
                            /* Control Group 2 */
  unsigned int pmcon0;            /* memory region 0 configuration */
  unsigned int rsvd1;
  unsigned int pmcon2;            /* memory region 2 configuration */
  unsigned int rsvd2;
                            /* Control Group 3 */
  unsigned int pmcon4;            /* memory region 4 configuration */
  unsigned int rsvd3;
  unsigned int pmcon6;            /* memory region 6 configuration */
  unsigned int rsvd4;
                            /* Control Group 4 */
  unsigned int pmcon8;            /* memory region 8 configuration */
  unsigned int rsvd5;
  unsigned int pmcon10;           /* memory region 10 configuration */
  unsigned int rsvd6;
                            /* Control Group 5 */
  unsigned int pmcon12;           /* memory region 12 configuration */
  unsigned int rsvd7;
  unsigned int pmcon14;           /* memory region 14 configuration */
  unsigned int rsvd8;
                            /* Control Group 6 */
  unsigned int rsvd9;
  unsigned int rsvd10;
  unsigned int tc;                /* trace control */
  unsigned int bcon;              /* bus configuration control */
}   i960rp_control_table;

/* Intel i960RP Processor Control Block */

/* Intel i960RP Processor Control Block */

typedef struct {  
  unsigned int    *fault_tbl;     /* fault table base address */
  i960rp_control_table
                  *control_tbl;   /* control table base address */
  unsigned int     initial_ac;    /* AC register initial value */
  unsigned int     fault_config;  /* fault configuration word */
  void           **intr_tbl;      /* interrupt table base address */
  void            *sys_proc_tbl;  /* system procedure table
                                     base address */
  unsigned int     reserved;      /* reserved */
  unsigned int    *intr_stack;    /* interrupt stack pointer */
  unsigned int     ins_cache_cfg; /* instruction cache
                                     configuration word */
  unsigned int     reg_cache_cfg; /* register cache configuration word */
}   i960rp_PRCB;

typedef i960rp_control_table i960_control_table;
typedef i960rp_PRCB i960_PRCB;

/* Addresses shared with JX */

#include <libcpu/i960JX_RP_common.h>

/* RP-only addresses: */
/* RP MMRs */

/* PCI-to-PCI Bridge Unit 0000 1000H through 0000 10FFH */
#define VIDR_ADDR 0x00001000
#define DIDR_ADDR 0x00001002
#define PCMDR_ADDR 0x00001004
#define PSR_ADDR 0x00001006
#define RIDR_ADDR 0x00001008
#define CCR_ADDR 0x00001009
#define CLSR_ADDR 0x0000100C
#define PLTR_ADDR 0x0000100D
#define HTR_ADDR 0x0000100E
/* Reserved 0x0000100F through  0x00001017 */
#define PBNR_ADDR 0x00001018
#define SBNR_ADDR 0x00001019
#define SUBBNR_ADDR 0x0000101A
#define SLTR_ADDR 0x0000101B
#define IOBR_ADDR 0x0000101C
#define IOLR_ADDR 0x0000101D
#define SSR_ADDR 0x0000101E
#define MBR_ADDR 0x00001020
#define MLR_ADDR 0x00001022
#define PMBR_ADDR 0x00001024
#define PMLR_ADDR 0x00001026
/* Reserved 0x00001028 through 0x00001033 */
#define BSVIR_ADDR 0x00001034
#define BSIR_ADDR 0x00001036
/* Reserved 0x00001038 through 0x0000103D */
#define BCR_ADDR 0x0000103E
#define EBCR_ADDR 0x00001040
#define SISR_ADDR 0x00001042
#define PBISR_ADDR 0x00001044
#define SBISR_ADDR 0x00001048
#define SACR_ADDR 0x0000104C
#define PIRSR_ADDR 0x00001050
#define SIOBR_ADDR 0x00001054
#define SIOLR_ADDR 0x00001055
#define SMBR_ADDR 0x00001058
#define SMLR_ADDR 0x0000105A
#define SDER_ADDR 0x0000105C
/* Reserved 0x0000105E through 0x000011FFH */

/* Address Translation Unit 0000 1200H through 0000 12FFH */
#define ATUVID_ADDR 0x00001200
#define ATUDID_ADDR 0x00001202
#define PATUCMD_ADDR 0x00001204
#define PATUSR_ADDR 0x00001206
#define ATURID_ADDR 0x00001208
#define ATUCCR_ADDR 0x00001209
#define ATUCLSR_ADDR 0x0000120C
#define ATULT_ADDR 0x0000120D
#define ATUHTR_ADDR 0x0000120E
#define ATUBISTR_ADDR 0x0000120F
#define PIABAR_ADDR 0x00001210
/* Reserved 0x00001214 */
/* Reserved 0x00001218 */
/* Reserved 0x0000121C */
/* Reserved 0x00001220 */
/* Reserved 0x00001224 */
/* Reserved 0x00001228 */
#define ASVIR_ADDR 0x0000122C
#define ASIR_ADDR 0x0000122E
#define ERBAR_ADDR 0x00001230
/* Reserved 0x00001234 */
/* Reserved 0x00001238 */
#define ATUILR_ADDR 0x0000123C
#define ATUIPR_ADDR 0x0000123D
#define ATUMGNT_ADDR 0x0000123E
#define ATUMLAT_ADDR 0x0000123F
#define PIALR_ADDR 0x00001240
#define PIATVR_ADDR 0x00001244
#define SIABAR_ADDR 0x00001248
#define SIALR_ADDR 0x0000124C
#define SIATVR_ADDR 0x00001250
#define POMWVR_ADDR 0x00001254
/* Reserved 0x00001258 */
#define POIOWVR_ADDR 0x0000125C
#define PODWVR_ADDR 0x00001260
#define POUDR_ADDR 0x00001264
#define SOMWVR_ADDR 0x00001268
#define SOIOWVR_ADDR 0x0000126C
/* Reserved 0x00001270 */
#define ERLR_ADDR 0x00001274
#define ERTVR_ADDR 0x00001278
/* Reserved 0x0000127C */
/* Reserved 0x00001280 */
/* Reserved 0x00001284 */
#define ATUCR_ADDR 0x00001288
/* Reserved 0x0000128C */
#define PATUISR_ADDR 0x00001290
#define SATUISR_ADDR 0x00001294
#define SATUCMD_ADDR 0x00001298
#define SATUSR_ADDR 0x0000129A
#define SODWVR_ADDR 0x0000129C
#define SOUDR_ADDR 0x000012A0
#define POCCAR_ADDR 0x000012A4
#define SOCCAR_ADDR 0x000012A8
#define POCCDR_ADDR 0x000012AC
#define SOCCDR_ADDR 0x000012B0
/* Reserved 0x000012B4 through 0x000012FF */

/* Messaging Unit 0000 1300H through 0000 13FFH */
#define ARSR_ADDR 0x00001300
/* Reserved 0x00001304 */
#define AWR_ADDR 0x00001308
/* Reserved 0x0000130C */
#define IMR0_ADDR 0x00001310
#define IMR1_ADDR 0x00001314
#define OMR0_ADDR 0x00001318
#define OMR1_ADDR 0x0000131C
#define IDR_ADDR 0x00001320
#define IISR_ADDR 0x00001324
#define IIMR_ADDR 0x00001328
#define ODR_ADDR 0x0000132C
#define OISR_ADDR 0x00001330
#define OIMR_ADDR 0x00001334
/* Reserved 0x00001338 through 0x0000134F */
#define MUCR_ADDR 0x00001350
#define QBAR_ADDR 0x00001354
/* Reserved 0x00001358 */
/* Reserved 0x0000135C */
#define IFHPR_ADDR 0x00001360
#define IFTPR_ADDR 0x00001364
#define IPHPR_ADDR 0x00001368
#define IPTPR_ADDR 0x0000136C
#define OFHPR_ADDR 0x00001370
#define OFTPR_ADDR 0x00001374
#define OPHPR_ADDR 0x00001378
#define OPTPR_ADDR 0x0000137C
#define IAR_ADDR 0x00001380
/* Reserved 0x00001384 through 0x000013FF */

/* DMA Controller 0000 1400H through 0000 14FFH */
#define CCR0_ADDR 0x00001400
#define CSR0_ADDR 0x00001404
/* Reserved 0x00001408 */
#define DAR0_ADDR 0x0000140C
#define NDAR0_ADDR 0x00001410
#define PADR0_ADDR 0x00001414
#define PUADR0_ADDR 0x00001418
#define LADR0_ADDR 0x0000141C
#define BCR0_ADDR 0x00001420
#define DCR0_ADDR 0x00001424
/* Reserved 0x00001428 through 0x0000143F */
#define CCR1_ADDR 0x00001440
#define CSR1_ADDR 0x00001444
/* Reserved 0x00001448 */
#define DAR1_ADDR 0x0000144C
#define NDAR1_ADDR 0x00001450
#define PADR1_ADDR 0x00001454
#define PUADR1_ADDR 0x00001458
#define LADR1_ADDR 0x0000145C
#define BCR1_ADDR 0x00001460
#define DCR1_ADDR 0x00001464
/* Reserved 0x00001468 through 0x0000147F */
#define CCR2_ADDR 0x00001480
#define CSR2_ADDR 0x00001484
/* Reserved 0x00001488 */
#define DAR2_ADDR 0x0000148C
#define NDAR2_ADDR 0x00001490
#define PADR2_ADDR 0x00001494
#define PUADR2_ADDR 0x00001498
#define LADR2_ADDR 0x0000149C
#define BCR2_ADDR 0x000014A0
#define DCR2_ADDR 0x000014A4
/* Reserved 0x000014A8 through 0x000014FF */

/* Memory Controller 0000 1500H through 0000 15FFH */
#define MBCR_ADDR 0x00001500
#define MBBAR0_ADDR 0x00001504
#define MBRWS0_ADDR 0x00001508
#define MBWWS0_ADDR 0x0000150C
#define MBBAR1_ADDR 0x00001510
#define MBRWS1_ADDR 0x00001514
#define MBWWS1_ADDR 0x00001518
#define DBCR_ADDR 0x0000151C
#define DBAR_ADDR 0x00001520
#define DRWS_ADDR 0x00001524
#define DWWS_ADDR 0x00001528
#define DRIR_ADDR 0x0000152C
#define DPER_ADDR 0x00001530
#define BMER_ADDR 0x00001534
#define MEAR_ADDR 0x00001538
#define LPISR_ADDR 0x0000153C
/* Reserved 0x00001540 through 0x000015FF */

/* Local Bus Arbitration Unit 0000 1600H through 0000 167FH 
*/
#define LBACR_ADDR 0x00001600
#define LBALCR_ADDR 0x00001604
/* Reserved 0x00001608 through 0x0000167F */

/* I2C Bus Interface Unit 0000 1680H through 0000 16FFH */
#define ICR_ADDR 0x00001680
#define ISR_ADDR 0x00001684
#define ISAR_ADDR 0x00001688
#define IDBR_ADDR 0x0000168C
#define ICCR_ADDR 0x00001690
/* Reserved 0x00001694 through 0x000016FF */

/* PCI And Peripheral Interrupt Controller 0000 1700H through 
0000 177FH */
#define NISR_ADDR 0x00001700
#define X7ISR_ADDR 0x00001704
#define X6ISR_ADDR 0x00001708
#define PDDIR_ADDR 0x00001710
/* Reserved 0x00001714 through 0x0000177F */

/* APIC Bus Interface Unit 0000 1780H through 0000 17FFH */
#define APICIDR_ADDR 0x00001780
#define APICARBID_ADDR 0x00001784
#define EVR_ADDR 0x00001788
#define IMR_ADDR 0x0000178C
#define APICCSR_ADDR 0x00001790
/* Reserved 0x00001794 through 0x000017FF  */

/* Byte order bit for region configuration */
/* Set to Little Endian for the 80960RP*/
#define I960RP_BYTE_ORDER I960RP_BIG_ENDIAN(0)
#define I960RP_BUS_WIDTH(bw)  ((bw==16)?(1<<22):(0)) | ((bw==32)?(2<<22):(0))
#define I960RP_BIG_ENDIAN(on) ((on)?(0x1<<31):0)
#define I960RP_BYTE_N(n,data)  (((unsigned)(data) >> (n*8)) & 0xFF)
#define I960RP_BUS_WIDTH_8 0
#define I960RP_BUS_WIDTH_16 (1<<22)
#define I960RP_BUS_WIDTH_32 (1<<23)


/* ATU Register Definitions */

#define ATUCR_SECOUTEN 0x4
#define ATUCR_PRIOUTEN 0x2
#define ATUCR_DADRSELEN  0x100
#define ATUCR_SECDADREN  0x80
#define AUTCR_SECERRINTEN 0x20 
#define AUTCR_PRIERRINTEN 0x10 

#define ATUSCMD_IOEN 0x1
#define ATUSCMD_MEMEN 0x2
#define ATUSCMD_BUSMSTEN 0x4

#define ATUPCMD_IOEN 0x1
#define ATUPCMD_MEMEN 0x2
#define ATUPCMD_BUSMSTEN 0x4

/* EBCR Register Definitions */
#define EBCR_CCR_MASK	0x4

#define rp_readreg32( x) ( *((unsigned int *) x))
#define rp_writereg32( x, v) ( *((unsigned int *) x) = v)
#define rp_readreg16( x) ( *((unsigned short *) x))
#define rp_writereg16( x, v) ( *((unsigned short *) x) = v)
#define rp_readreg8( x) ( *((unsigned char *) x))
#define rp_writereg8( x, v) ( *((unsigned char *) x) = v)


/* i960 Memory Map values */

#define RP_PRI_IO_WIND_BASE     0x90000000
#define RP_SEC_IO_WIND_BASE	0x90010000
#define RP_SEC_MEM_WIND_BASE	0x88000000
#define RP_PRI_MEM_WIND_BASE	0x80000000

#define i960_unmask_intr( xint ) \
 { register unsigned int _mask= (1<<(xint)); \
   register unsigned int *_imsk = (int * ) IMSK_ADDR; \
   register unsigned int _val= *_imsk; \
   asm volatile( "or %0,%2,%0; \
                  st %0,(%1)" \
                    : "=d" (_val), "=d" (_imsk), "=d" (_mask) \
                    : "0" (_val), "1" (_imsk), "2" (_mask) ); \
 }

#define i960_mask_intr( xint ) \
 { register unsigned int _mask= (1<<(xint)); \
   register unsigned int *_imsk = (int * ) IMSK_ADDR; \
   register unsigned int _val = *_imsk; \
   asm volatile( "andnot %2,%0,%0; \
                  st %0,(%1)" \
                    : "=d" (_val), "=d" (_imsk), "=d" (_mask) \
                    : "0" (_val), "1" (_imsk), "2" (_mask) ); \
 }
#define i960_clear_intr( xint ) \
 { register unsigned int _xint=xint; \
   register unsigned int _mask=(1<<(xint)); \
   register unsigned int *_ipnd = (int * ) IPND_ADDR; \
   register unsigned int          _rslt = 0; \
asm volatile( "99: mov 0, %0; \
                  atmod %1, %2, %0; \
                  bbs    %3,%0, 99b" \
                  : "=d" (_rslt), "=d" (_ipnd), "=d" (_mask), "=d" (_xint) \
                  : "0"  (_rslt), "1"  (_ipnd), "2"  (_mask), "3"  (_xint) ); \
 }

static inline unsigned int i960_pend_intrs()
{ register unsigned int _intr= *(unsigned int *) IPND_ADDR;
  /*register unsigned int *_ipnd = (int * ) IPND_ADDR; \
   asm volatile( "mov (%0),%1" \
                    : "=d" (_ipnd), "=d" (_mask) \
                    : "0" (_ipnd), "1" (_mask) ); \ */
  return ( _intr );
}

static inline unsigned int i960_mask_intrs()
{ register unsigned int _intr= *(unsigned int *) IMSK_ADDR;
  /*asm volatile( "mov sf1,%0" : "=d" (_intr) : "0" (_intr) );*/
  return( _intr );
}

#define I960_SOFT_RESET_COMMAND 0x300

#define i960_soft_reset( prcb ) \
 { register i960_PRCB    *_prcb = (prcb); \
   register unsigned int *_next=0; \
   register unsigned int  _cmd  = I960_SOFT_RESET_COMMAND; \
   asm volatile( "lda    next,%1; \
                  sysctl %0,%1,%2; \
            next: mov    g0,g0" \
                  : "=d" (_cmd), "=d" (_next), "=d" (_prcb) \
                  : "0"  (_cmd), "1"  (_next), "2"  (_prcb) ); \
 }


#endif
/* end of include file */
