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

/*----------------------------------------------------------*/
/*  Example 6. Include File (evrp.h)                        */
/*----------------------------------------------------------*/
/* Define JX Core memory mapped register addresses */
/* Common to Jx and RP: */
#define    DLMCON_ADDR    0xff008100
#define    LMAR0_ADDR     0xff008108
#define    LMMR0_ADDR     0xff00810c
#define    LMAR1_ADDR     0xff008110
#define    LMMR1_ADDR     0xff008114
#define    IPB0_ADDR      0xff008400
#define    IPB1_ADDR      0xff008404
#define    DAB0_ADDR      0xff008420
#define    DAB1_ADDR      0xff008424
#define    BPCON_ADDR     0xff008440
#define    IPND_ADDR      0xff008500
#define    IMSK_ADDR      0xff008504
#define    ICON_ADDR      0xff008510
#define    IMAP0_ADDR     0xff008520
#define    IMAP1_ADDR     0xff008524
#define    IMAP2_ADDR     0xff008528
#define    PMCON0_ADDR    0xff008600
#define    PMCON2_ADDR    0xff008608
#define    PMCON4_ADDR    0xff008610
#define    PMCON6_ADDR    0xff008618
#define    PMCON8_ADDR    0xff008620
#define    PMCON10_ADDR   0xff008628
#define    PMCON12_ADDR   0xff008630
#define    PMCON14_ADDR   0xff008638
#define    BCON_ADDR      0xff0086fc
#define    PRCB_ADDR      0xff008700
#define    ISP_ADDR       0xff008704
#define    SSP_ADDR       0xff008708
#define    DEVID_ADDR     0xff008710
#define    TRR0_ADDR      0xff000300
#define    TCR0_ADDR      0xff000304
#define    TMR0_ADDR      0xff000308
#define    TRR1_ADDR      0xff000310
#define    TCR1_ADDR      0xff000314
#define    TMR1_ADDR      0xff000318

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
#define BYTE_ORDER BIG_ENDIAN(0)
#define BUS_WIDTH(bw)  ((bw==16)?(1<<22):(0)) | ((bw==32)?(2<<22):(0))
#define BIG_ENDIAN(on) ((on)?(0x1<<31):0)
#define BYTE_N(n,data)  (((unsigned)(data) >> (n*8)) & 0xFF)
#define BUS_WIDTH_8 0
#define BUS_WIDTH_16 (1<<22)
#define BUS_WIDTH_32 (1<<23)


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

#endif
/* end of include file */
