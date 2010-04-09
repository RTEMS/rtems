/************************************************
 * NAME     : s3c2410.h
 * Version  : 3.7.2002
 *
 * Based on 24x.h for the Samsung Development Board
 ************************************************/

#ifndef S3C2410_H_
#define S3C2410_H_

/* to be used in assembly code */
#define rINTOFFSET_ADDR 0x4A000014
/* Memory control */
#define rBWSCON         (*(volatile unsigned *)0x48000000)
#define rBANKCON0       (*(volatile unsigned *)0x48000004)
#define rBANKCON1       (*(volatile unsigned *)0x48000008)
#define rBANKCON2       (*(volatile unsigned *)0x4800000C)
#define rBANKCON3       (*(volatile unsigned *)0x48000010)
#define rBANKCON4       (*(volatile unsigned *)0x48000014)
#define rBANKCON5       (*(volatile unsigned *)0x48000018)
#define rBANKCON6       (*(volatile unsigned *)0x4800001C)
#define rBANKCON7       (*(volatile unsigned *)0x48000020)
#define rREFRESH        (*(volatile unsigned *)0x48000024)
#define rBANKSIZE       (*(volatile unsigned *)0x48000028)
#define rMRSRB6         (*(volatile unsigned *)0x4800002C)
#define rMRSRB7         (*(volatile unsigned *)0x48000030)

/* USB Host Controller */
#define rHcRevision		(*(volatile unsigned *)0x49000000)
#define rHcControl		(*(volatile unsigned *)0x49000004)
#define rHcCommonStatus		(*(volatile unsigned *)0x49000008)
#define rHcInterruptStatus	(*(volatile unsigned *)0x4900000C)
#define rHcInterruptEnable	(*(volatile unsigned *)0x49000010)
#define rHcInterruptDisable	(*(volatile unsigned *)0x49000014)
#define rHcHCCA			(*(volatile unsigned *)0x49000018)
#define rHcPeriodCuttendED	(*(volatile unsigned *)0x4900001C)
#define rHcControlHeadED	(*(volatile unsigned *)0x49000020)
#define rHcControlCurrentED	(*(volatile unsigned *)0x49000024)
#define rHcBulkHeadED		(*(volatile unsigned *)0x49000028)
#define rHcBuldCurrentED	(*(volatile unsigned *)0x4900002C)
#define rHcDoneHead		(*(volatile unsigned *)0x49000030)
#define rHcRmInterval		(*(volatile unsigned *)0x49000034)
#define rHcFmRemaining		(*(volatile unsigned *)0x49000038)
#define rHcFmNumber		(*(volatile unsigned *)0x4900003C)
#define rHcPeriodicStart	(*(volatile unsigned *)0x49000040)
#define rHcLSThreshold		(*(volatile unsigned *)0x49000044)
#define rHcRhDescriptorA	(*(volatile unsigned *)0x49000048)
#define rHcRhDescriptorB	(*(volatile unsigned *)0x4900004C)
#define rHcRhStatus		(*(volatile unsigned *)0x49000050)
#define rHcRhPortStatus1	(*(volatile unsigned *)0x49000054)
#define rHcRhPortStatus2	(*(volatile unsigned *)0x49000058)

/* INTERRUPT */
#define rSRCPND         (*(volatile unsigned *)0x4A000000)
#define rINTMOD         (*(volatile unsigned *)0x4A000004)
#define rINTMSK         (*(volatile unsigned *)0x4A000008)
#define rPRIORITY       (*(volatile unsigned *)0x4A00000C)
#define rINTPND         (*(volatile unsigned *)0x4A000010)
#define rINTOFFSET      (*(volatile unsigned *)0x4A000014)
#define rSUBSRCPND  	(*(volatile unsigned *)0x4A000018)
#define rINTSUBMSK  	(*(volatile unsigned *)0x4A00001c)


/* DMA */
#define rDISRC0         (*(volatile unsigned *)0x4B000000)
#define rDISRCC0        (*(volatile unsigned *)0x4B000004)
#define rDIDST0         (*(volatile unsigned *)0x4B000008)
#define rDIDSTC0        (*(volatile unsigned *)0x4B00000C)
#define rDCON0          (*(volatile unsigned *)0x4B000010)
#define rDSTAT0         (*(volatile unsigned *)0x4B000014)
#define rDCSRC0         (*(volatile unsigned *)0x4B000018)
#define rDCDST0         (*(volatile unsigned *)0x4B00001C)
#define rDMASKTRIG0     (*(volatile unsigned *)0x4B000020)
#define rDISRC1         (*(volatile unsigned *)0x4B000040)
#define rDISRCC1        (*(volatile unsigned *)0x4B000044)
#define rDIDST1         (*(volatile unsigned *)0x4B000048)
#define rDIDSTC1        (*(volatile unsigned *)0x4B00004C)
#define rDCON1          (*(volatile unsigned *)0x4B000050)
#define rDSTAT1         (*(volatile unsigned *)0x4B000054)
#define rDCSRC1         (*(volatile unsigned *)0x4B000058)
#define rDCDST1         (*(volatile unsigned *)0x4B00005C)
#define rDMASKTRIG1     (*(volatile unsigned *)0x4B000060)
#define rDISRC2         (*(volatile unsigned *)0x4B000080)
#define rDISRCC2        (*(volatile unsigned *)0x4B000084)
#define rDIDST2         (*(volatile unsigned *)0x4B000088)
#define rDIDSTC2        (*(volatile unsigned *)0x4B00008C)
#define rDCON2          (*(volatile unsigned *)0x4B000090)
#define rDSTAT2         (*(volatile unsigned *)0x4B000094)
#define rDCSRC2         (*(volatile unsigned *)0x4B000098)
#define rDCDST2         (*(volatile unsigned *)0x4B00009C)
#define rDMASKTRIG2     (*(volatile unsigned *)0x4B0000A0)
#define rDISRC3         (*(volatile unsigned *)0x4B0000C0)
#define rDISRCC3        (*(volatile unsigned *)0x4B0000C4)
#define rDIDST3         (*(volatile unsigned *)0x4B0000C8)
#define rDIDSTC3        (*(volatile unsigned *)0x4B0000CC)
#define rDCON3          (*(volatile unsigned *)0x4B0000D0)
#define rDSTAT3         (*(volatile unsigned *)0x4B0000D4)
#define rDCSRC3         (*(volatile unsigned *)0x4B0000D8)
#define rDCDST3         (*(volatile unsigned *)0x4B0000DC)
#define rDMASKTRIG3     (*(volatile unsigned *)0x4B0000E0)


/* CLOCK & POWER MANAGEMENT */
#define rLOCKTIME       (*(volatile unsigned *)0x4C000000)
#define rMPLLCON        (*(volatile unsigned *)0x4C000004)
#define rUPLLCON        (*(volatile unsigned *)0x4C000008)
#define rCLKCON         (*(volatile unsigned *)0x4C00000C)
#define rCLKSLOW        (*(volatile unsigned *)0x4C000010)
#define rCLKDIVN        (*(volatile unsigned *)0x4C000014)


/* LCD CONTROLLER */
#define rLCDCON1        (*(volatile unsigned *)0x4D000000)
#define rLCDCON2        (*(volatile unsigned *)0x4D000004)
#define rLCDCON3        (*(volatile unsigned *)0x4D000008)
#define rLCDCON4        (*(volatile unsigned *)0x4D00000C)
#define rLCDCON5        (*(volatile unsigned *)0x4D000010)
#define rLCDSADDR1      (*(volatile unsigned *)0x4D000014)
#define rLCDSADDR2      (*(volatile unsigned *)0x4D000018)
#define rLCDSADDR3      (*(volatile unsigned *)0x4D00001C)
#define rREDLUT         (*(volatile unsigned *)0x4D000020)
#define rGREENLUT       (*(volatile unsigned *)0x4D000024)
#define rBLUELUT        (*(volatile unsigned *)0x4D000028)
#define rREDLUT     	(*(volatile unsigned *)0x4D000020)
#define rGREENLUT   	(*(volatile unsigned *)0x4D000024)
#define rBLUELUT    	(*(volatile unsigned *)0x4D000028)
#define rDITHMODE   	(*(volatile unsigned *)0x4D00004C)
#define rTPAL       	(*(volatile unsigned *)0x4D000050)
#define rLCDINTPND  	(*(volatile unsigned *)0x4D000054)
#define rLCDSRCPND  	(*(volatile unsigned *)0x4D000058)
#define rLCDINTMSK  	(*(volatile unsigned *)0x4D00005C)
#define rTCONSEL     	(*(volatile unsigned *)0x4D000060)
#define PALETTE     	0x4d000400

/* NAND Flash */
#define rNFCONF			(*(volatile unsigned *)0x4E000000)
#define rNFCMD			(*(volatile unsigned *)0x4E000004)
#define rNFADDR			(*(volatile unsigned *)0x4E000008)
#define rNFDATA			(*(volatile unsigned *)0x4E00000C)
#define rNFSTAT			(*(volatile unsigned *)0x4E000010)
#define rNFECC			(*(volatile unsigned *)0x4E000014)

/* UART */
#define rULCON0         (*(volatile unsigned char  *)0x50000000)
#define rUCON0          (*(volatile unsigned short *)0x50000004)
#define rUFCON0         (*(volatile unsigned char  *)0x50000008)
#define rUMCON0         (*(volatile unsigned char  *)0x5000000C)
#define rUTRSTAT0       (*(volatile unsigned char  *)0x50000010)
#define rUERSTAT0       (*(volatile unsigned char  *)0x50000014)
#define rUFSTAT0        (*(volatile unsigned short *)0x50000018)
#define rUMSTAT0        (*(volatile unsigned char  *)0x5000001C)
#define rUBRDIV0        (*(volatile unsigned short *)0x50000028)

#define rULCON1         (*(volatile unsigned char  *)0x50004000)
#define rUCON1          (*(volatile unsigned short *)0x50004004)
#define rUFCON1         (*(volatile unsigned char  *)0x50004008)
#define rUMCON1         (*(volatile unsigned char  *)0x5000400C)
#define rUTRSTAT1       (*(volatile unsigned char  *)0x50004010)
#define rUERSTAT1       (*(volatile unsigned char  *)0x50004014)
#define rUFSTAT1        (*(volatile unsigned short *)0x50004018)
#define rUMSTAT1        (*(volatile unsigned char  *)0x5000401C)
#define rUBRDIV1        (*(volatile unsigned short *)0x50004028)

#define rULCON2         (*(volatile unsigned char  *)0x50008000)
#define rUCON2          (*(volatile unsigned short *)0x50008004)
#define rUFCON2         (*(volatile unsigned char  *)0x50008008)
#define rUTRSTAT2       (*(volatile unsigned char  *)0x50008010)
#define rUERSTAT2       (*(volatile unsigned char  *)0x50008014)
#define rUFSTAT2        (*(volatile unsigned short *)0x50008018)
#define rUBRDIV2        (*(volatile unsigned short *)0x50008028)

#ifdef __BIG_ENDIAN
#define rUTXH0          (*(volatile unsigned char *)0x50000023)
#define rURXH0          (*(volatile unsigned char *)0x50000027)
#define rUTXH1          (*(volatile unsigned char *)0x50004023)
#define rURXH1          (*(volatile unsigned char *)0x50004027)
#define rUTXH2          (*(volatile unsigned char *)0x50008023)
#define rURXH2          (*(volatile unsigned char *)0x50008027)

#define WrUTXH0(ch)     (*(volatile unsigned char *)0x50000023)=(unsigned char)(ch)
#define RdURXH0()       (*(volatile unsigned char *)0x50000027)
#define WrUTXH1(ch)     (*(volatile unsigned char *)0x50004023)=(unsigned char)(ch)
#define RdURXH1()       (*(volatile unsigned char *)0x50004027)
#define WrUTXH2(ch)     (*(volatile unsigned char *)0x50008023)=(unsigned char)(ch)
#define RdURXH2()       (*(volatile unsigned char *)0x50008027)

#define UTXH0           (0x50000020+3)  /* byte_access address by DMA */
#define URXH0           (0x50000024+3)
#define UTXH1           (0x50004020+3)
#define URXH1           (0x50004024+3)
#define UTXH2           (0x50008020+3)
#define URXH2           (0x50008024+3)

#else /* Little Endian */
#define rUTXH0          (*(volatile unsigned char *)0x50000020)
#define rURXH0          (*(volatile unsigned char *)0x50000024)
#define rUTXH1          (*(volatile unsigned char *)0x50004020)
#define rURXH1          (*(volatile unsigned char *)0x50004024)
#define rUTXH2          (*(volatile unsigned char *)0x50008020)
#define rURXH2          (*(volatile unsigned char *)0x50008024)

#define WrUTXH0(ch)     (*(volatile unsigned char *)0x50000020)=(unsigned char)(ch)
#define RdURXH0()       (*(volatile unsigned char *)0x50000024)
#define WrUTXH1(ch)     (*(volatile unsigned char *)0x50004020)=(unsigned char)(ch)
#define RdURXH1()       (*(volatile unsigned char *)0x50004024)
#define WrUTXH2(ch)     (*(volatile unsigned char *)0x50008020)=(unsigned char)(ch)
#define RdURXH2()       (*(volatile unsigned char *)0x50008024)

#define UTXH0           (0x50000020)
#define URXH0           (0x50000024)
#define UTXH1           (0x50004020)
#define URXH1           (0x50004024)
#define UTXH2           (0x50008020)
#define URXH2           (0x50008024)
#endif


/* PWM TIMER */
#define rTCFG0          (*(volatile unsigned *)0x51000000)
#define rTCFG1          (*(volatile unsigned *)0x51000004)
#define rTCON           (*(volatile unsigned *)0x51000008)
#define rTCNTB0         (*(volatile unsigned *)0x5100000C)
#define rTCMPB0         (*(volatile unsigned *)0x51000010)
#define rTCNTO0         (*(volatile unsigned *)0x51000014)
#define rTCNTB1         (*(volatile unsigned *)0x51000018)
#define rTCMPB1         (*(volatile unsigned *)0x5100001C)
#define rTCNTO1         (*(volatile unsigned *)0x51000020)
#define rTCNTB2         (*(volatile unsigned *)0x51000024)
#define rTCMPB2         (*(volatile unsigned *)0x51000028)
#define rTCNTO2         (*(volatile unsigned *)0x5100002C)
#define rTCNTB3         (*(volatile unsigned *)0x51000030)
#define rTCMPB3         (*(volatile unsigned *)0x51000034)
#define rTCNTO3         (*(volatile unsigned *)0x51000038)
#define rTCNTB4         (*(volatile unsigned *)0x5100003C)
#define rTCNTO4         (*(volatile unsigned *)0x51000040)


/* USB DEVICE */
#ifdef __BIG_ENDIAN
#define rFUNC_ADDR_REG     (*(volatile unsigned char *)0x52000143)	//Function address
#define rPWR_REG           (*(volatile unsigned char *)0x52000147)	//Power management
#define rEP_INT_REG        (*(volatile unsigned char *)0x5200014b)	//EP Interrupt pending and clear
#define rUSB_INT_REG       (*(volatile unsigned char *)0x5200015b)	//USB Interrupt pending and clear
#define rEP_INT_EN_REG     (*(volatile unsigned char *)0x5200015f)	//Interrupt enable
#define rUSB_INT_EN_REG    (*(volatile unsigned char *)0x5200016f)
#define rFRAME_NUM1_REG    (*(volatile unsigned char *)0x52000173)	//Frame number lower byte
#define rFRAME_NUM2_REG    (*(volatile unsigned char *)0x52000177)	//Frame number higher byte
#define rINDEX_REG         (*(volatile unsigned char *)0x5200017b)	//Register index
#define rMAXP_REG          (*(volatile unsigned char *)0x52000183)	//Endpoint max packet
#define rEP0_CSR           (*(volatile unsigned char *)0x52000187)	//Endpoint 0 status
#define rIN_CSR1_REG       (*(volatile unsigned char *)0x52000187)	//In endpoint control status
#define rIN_CSR2_REG       (*(volatile unsigned char *)0x5200018b)
#define rOUT_CSR1_REG      (*(volatile unsigned char *)0x52000193)	//Out endpoint control status
#define rOUT_CSR2_REG      (*(volatile unsigned char *)0x52000197)
#define rOUT_FIFO_CNT1_REG (*(volatile unsigned char *)0x5200019b)	//Endpoint out write count
#define rOUT_FIFO_CNT2_REG (*(volatile unsigned char *)0x5200019f)
#define rEP0_FIFO          (*(volatile unsigned char *)0x520001c3)	//Endpoint 0 FIFO
#define rEP1_FIFO          (*(volatile unsigned char *)0x520001c7)	//Endpoint 1 FIFO
#define rEP2_FIFO          (*(volatile unsigned char *)0x520001cb)	//Endpoint 2 FIFO
#define rEP3_FIFO          (*(volatile unsigned char *)0x520001cf)	//Endpoint 3 FIFO
#define rEP4_FIFO          (*(volatile unsigned char *)0x520001d3)	//Endpoint 4 FIFO
#define rEP1_DMA_CON       (*(volatile unsigned char *)0x52000203)	//EP1 DMA interface control
#define rEP1_DMA_UNIT      (*(volatile unsigned char *)0x52000207)	//EP1 DMA Tx unit counter
#define rEP1_DMA_FIFO      (*(volatile unsigned char *)0x5200020b)	//EP1 DMA Tx FIFO counter
#define rEP1_DMA_TTC_L     (*(volatile unsigned char *)0x5200020f)	//EP1 DMA total Tx counter
#define rEP1_DMA_TTC_M     (*(volatile unsigned char *)0x52000213)
#define rEP1_DMA_TTC_H     (*(volatile unsigned char *)0x52000217)
#define rEP2_DMA_CON       (*(volatile unsigned char *)0x5200021b)	//EP2 DMA interface control
#define rEP2_DMA_UNIT      (*(volatile unsigned char *)0x5200021f)	//EP2 DMA Tx unit counter
#define rEP2_DMA_FIFO      (*(volatile unsigned char *)0x52000223)	//EP2 DMA Tx FIFO counter
#define rEP2_DMA_TTC_L     (*(volatile unsigned char *)0x52000227)	//EP2 DMA total Tx counter
#define rEP2_DMA_TTC_M     (*(volatile unsigned char *)0x5200022b)
#define rEP2_DMA_TTC_H     (*(volatile unsigned char *)0x5200022f)
#define rEP3_DMA_CON       (*(volatile unsigned char *)0x52000243)	//EP3 DMA interface control
#define rEP3_DMA_UNIT      (*(volatile unsigned char *)0x52000247)	//EP3 DMA Tx unit counter
#define rEP3_DMA_FIFO      (*(volatile unsigned char *)0x5200024b)	//EP3 DMA Tx FIFO counter
#define rEP3_DMA_TTC_L     (*(volatile unsigned char *)0x5200024f)	//EP3 DMA total Tx counter
#define rEP3_DMA_TTC_M     (*(volatile unsigned char *)0x52000253)
#define rEP3_DMA_TTC_H     (*(volatile unsigned char *)0x52000257)
#define rEP4_DMA_CON       (*(volatile unsigned char *)0x5200025b)	//EP4 DMA interface control
#define rEP4_DMA_UNIT      (*(volatile unsigned char *)0x5200025f)	//EP4 DMA Tx unit counter
#define rEP4_DMA_FIFO      (*(volatile unsigned char *)0x52000263)	//EP4 DMA Tx FIFO counter
#define rEP4_DMA_TTC_L     (*(volatile unsigned char *)0x52000267)	//EP4 DMA total Tx counter
#define rEP4_DMA_TTC_M     (*(volatile unsigned char *)0x5200026b)
#define rEP4_DMA_TTC_H     (*(volatile unsigned char *)0x5200026f)

#else  // Little Endian
#define rFUNC_ADDR_REG     (*(volatile unsigned char *)0x52000140)	//Function address
#define rPWR_REG           (*(volatile unsigned char *)0x52000144)	//Power management
#define rEP_INT_REG        (*(volatile unsigned char *)0x52000148)	//EP Interrupt pending and clear
#define rUSB_INT_REG       (*(volatile unsigned char *)0x52000158)	//USB Interrupt pending and clear
#define rEP_INT_EN_REG     (*(volatile unsigned char *)0x5200015c)	//Interrupt enable
#define rUSB_INT_EN_REG    (*(volatile unsigned char *)0x5200016c)
#define rFRAME_NUM1_REG    (*(volatile unsigned char *)0x52000170)	//Frame number lower byte
#define rFRAME_NUM2_REG    (*(volatile unsigned char *)0x52000174)	//Frame number higher byte
#define rINDEX_REG         (*(volatile unsigned char *)0x52000178)	//Register index
#define rMAXP_REG          (*(volatile unsigned char *)0x52000180)	//Endpoint max packet
#define rEP0_CSR           (*(volatile unsigned char *)0x52000184)	//Endpoint 0 status
#define rIN_CSR1_REG       (*(volatile unsigned char *)0x52000184)	//In endpoint control status
#define rIN_CSR2_REG       (*(volatile unsigned char *)0x52000188)
#define rOUT_CSR1_REG      (*(volatile unsigned char *)0x52000190)	//Out endpoint control status
#define rOUT_CSR2_REG      (*(volatile unsigned char *)0x52000194)
#define rOUT_FIFO_CNT1_REG (*(volatile unsigned char *)0x52000198)	//Endpoint out write count
#define rOUT_FIFO_CNT2_REG (*(volatile unsigned char *)0x5200019c)
#define rEP0_FIFO          (*(volatile unsigned char *)0x520001c0)	//Endpoint 0 FIFO
#define rEP1_FIFO          (*(volatile unsigned char *)0x520001c4)	//Endpoint 1 FIFO
#define rEP2_FIFO          (*(volatile unsigned char *)0x520001c8)	//Endpoint 2 FIFO
#define rEP3_FIFO          (*(volatile unsigned char *)0x520001cc)	//Endpoint 3 FIFO
#define rEP4_FIFO          (*(volatile unsigned char *)0x520001d0)	//Endpoint 4 FIFO
#define rEP1_DMA_CON       (*(volatile unsigned char *)0x52000200)	//EP1 DMA interface control
#define rEP1_DMA_UNIT      (*(volatile unsigned char *)0x52000204)	//EP1 DMA Tx unit counter
#define rEP1_DMA_FIFO      (*(volatile unsigned char *)0x52000208)	//EP1 DMA Tx FIFO counter
#define rEP1_DMA_TTC_L     (*(volatile unsigned char *)0x5200020c)	//EP1 DMA total Tx counter
#define rEP1_DMA_TTC_M     (*(volatile unsigned char *)0x52000210)
#define rEP1_DMA_TTC_H     (*(volatile unsigned char *)0x52000214)
#define rEP2_DMA_CON       (*(volatile unsigned char *)0x52000218)	//EP2 DMA interface control
#define rEP2_DMA_UNIT      (*(volatile unsigned char *)0x5200021c)	//EP2 DMA Tx unit counter
#define rEP2_DMA_FIFO      (*(volatile unsigned char *)0x52000220)	//EP2 DMA Tx FIFO counter
#define rEP2_DMA_TTC_L     (*(volatile unsigned char *)0x52000224)	//EP2 DMA total Tx counter
#define rEP2_DMA_TTC_M     (*(volatile unsigned char *)0x52000228)
#define rEP2_DMA_TTC_H     (*(volatile unsigned char *)0x5200022c)
#define rEP3_DMA_CON       (*(volatile unsigned char *)0x52000240)	//EP3 DMA interface control
#define rEP3_DMA_UNIT      (*(volatile unsigned char *)0x52000244)	//EP3 DMA Tx unit counter
#define rEP3_DMA_FIFO      (*(volatile unsigned char *)0x52000248)	//EP3 DMA Tx FIFO counter
#define rEP3_DMA_TTC_L     (*(volatile unsigned char *)0x5200024c)	//EP3 DMA total Tx counter
#define rEP3_DMA_TTC_M     (*(volatile unsigned char *)0x52000250)
#define rEP3_DMA_TTC_H     (*(volatile unsigned char *)0x52000254)
#define rEP4_DMA_CON       (*(volatile unsigned char *)0x52000258)	//EP4 DMA interface control
#define rEP4_DMA_UNIT      (*(volatile unsigned char *)0x5200025c)	//EP4 DMA Tx unit counter
#define rEP4_DMA_FIFO      (*(volatile unsigned char *)0x52000260)	//EP4 DMA Tx FIFO counter
#define rEP4_DMA_TTC_L     (*(volatile unsigned char *)0x52000264)	//EP4 DMA total Tx counter
#define rEP4_DMA_TTC_M     (*(volatile unsigned char *)0x52000268)
#define rEP4_DMA_TTC_H     (*(volatile unsigned char *)0x5200026c)
#endif   // __BIG_ENDIAN

/* WATCH DOG TIMER */
#define rWTCON          (*(volatile unsigned *)0x53000000)
#define rWTDAT          (*(volatile unsigned *)0x53000004)
#define rWTCNT          (*(volatile unsigned *)0x53000008)


/* IIC */
#define rIICCON         (*(volatile unsigned *)0x54000000)
#define rIICSTAT        (*(volatile unsigned *)0x54000004)
#define rIICADD         (*(volatile unsigned *)0x54000008)
#define rIICDS          (*(volatile unsigned *)0x5400000C)


/* IIS */
#define rIISCON         (*(volatile unsigned *)0x55000000)
#define rIISMOD         (*(volatile unsigned *)0x55000004)
#define rIISPSR         (*(volatile unsigned *)0x55000008)
#define rIISFIFCON      (*(volatile unsigned *)0x5500000C)

#ifdef __BIG_ENDIAN
#define IISFIFO          ((volatile unsigned short *)0x55000012)

#else /* Little Endian */
#define IISFIFO          ((volatile unsigned short *)0x55000010)
#endif


/* I/O PORT */
#define rGPACON    (*(volatile unsigned *)0x56000000)	//Port A control
#define rGPADAT    (*(volatile unsigned *)0x56000004)	//Port A data

#define rGPBCON    (*(volatile unsigned *)0x56000010)	//Port B control
#define rGPBDAT    (*(volatile unsigned *)0x56000014)	//Port B data
#define rGPBUP     (*(volatile unsigned *)0x56000018)	//Pull-up control B

#define rGPCCON    (*(volatile unsigned *)0x56000020)	//Port C control
#define rGPCDAT    (*(volatile unsigned *)0x56000024)	//Port C data
#define rGPCUP     (*(volatile unsigned *)0x56000028)	//Pull-up control C

#define rGPDCON    (*(volatile unsigned *)0x56000030)	//Port D control
#define rGPDDAT    (*(volatile unsigned *)0x56000034)	//Port D data
#define rGPDUP     (*(volatile unsigned *)0x56000038)	//Pull-up control D

#define rGPECON    (*(volatile unsigned *)0x56000040)	//Port E control
#define rGPEDAT    (*(volatile unsigned *)0x56000044)	//Port E data
#define rGPEUP     (*(volatile unsigned *)0x56000048)	//Pull-up control E

#define rGPFCON    (*(volatile unsigned *)0x56000050)	//Port F control
#define rGPFDAT    (*(volatile unsigned *)0x56000054)	//Port F data
#define rGPFUP     (*(volatile unsigned *)0x56000058)	//Pull-up control F

#define rGPGCON    (*(volatile unsigned *)0x56000060)	//Port G control
#define rGPGDAT    (*(volatile unsigned *)0x56000064)	//Port G data
#define rGPGUP     (*(volatile unsigned *)0x56000068)	//Pull-up control G

#define rGPHCON    (*(volatile unsigned *)0x56000070)	//Port H control
#define rGPHDAT    (*(volatile unsigned *)0x56000074)	//Port H data
#define rGPHUP     (*(volatile unsigned *)0x56000078)	//Pull-up control H

#define rMISCCR    (*(volatile unsigned *)0x56000080)	//Miscellaneous control
#define rDCLKCON   (*(volatile unsigned *)0x56000084)	//DCLK0/1 control
#define rEXTINT0   (*(volatile unsigned *)0x56000088)	//External interrupt control register 0
#define rEXTINT1   (*(volatile unsigned *)0x5600008c)	//External interrupt control register 1
#define rEXTINT2   (*(volatile unsigned *)0x56000090)	//External interrupt control register 2
#define rEINTFLT0  (*(volatile unsigned *)0x56000094)	//Reserved
#define rEINTFLT1  (*(volatile unsigned *)0x56000098)	//Reserved
#define rEINTFLT2  (*(volatile unsigned *)0x5600009c)	//External interrupt filter control register 2
#define rEINTFLT3  (*(volatile unsigned *)0x560000a0)	//External interrupt filter control register 3
#define rEINTMASK  (*(volatile unsigned *)0x560000a4)	//External interrupt mask
#define rEINTPEND  (*(volatile unsigned *)0x560000a8)	//External interrupt pending
#define rGSTATUS0  (*(volatile unsigned *)0x560000ac)	//External pin status
#define rGSTATUS1  (*(volatile unsigned *)0x560000b0)	//Chip ID(0x32440000)

/* RTC */
#ifdef __BIG_ENDIAN
#define rRTCCON    (*(volatile unsigned char *)0x57000043)	//RTC control
#define rTICNT     (*(volatile unsigned char *)0x57000047)	//Tick time count
#define rRTCALM    (*(volatile unsigned char *)0x57000053)	//RTC alarm control
#define rALMSEC    (*(volatile unsigned char *)0x57000057)	//Alarm second
#define rALMMIN    (*(volatile unsigned char *)0x5700005b)	//Alarm minute
#define rALMHOUR   (*(volatile unsigned char *)0x5700005f)	//Alarm Hour
#define rALMDATE   (*(volatile unsigned char *)0x57000063)	//Alarm date   //edited by junon
#define rALMMON    (*(volatile unsigned char *)0x57000067)	//Alarm month
#define rALMYEAR   (*(volatile unsigned char *)0x5700006b)	//Alarm year
#define rRTCRST    (*(volatile unsigned char *)0x5700006f)	//RTC round reset
#define rBCDSEC    (*(volatile unsigned char *)0x57000073)	//BCD second
#define rBCDMIN    (*(volatile unsigned char *)0x57000077)	//BCD minute
#define rBCDHOUR   (*(volatile unsigned char *)0x5700007b)	//BCD hour
#define rBCDDATE   (*(volatile unsigned char *)0x5700007f)	//BCD date  //edited by junon
#define rBCDDAY    (*(volatile unsigned char *)0x57000083)	//BCD day   //edited by junon
#define rBCDMON    (*(volatile unsigned char *)0x57000087)	//BCD month
#define rBCDYEAR   (*(volatile unsigned char *)0x5700008b)	//BCD year

#else //Little Endian
#define rRTCCON    (*(volatile unsigned char *)0x57000040)	//RTC control
#define rTICNT     (*(volatile unsigned char *)0x57000044)	//Tick time count
#define rRTCALM    (*(volatile unsigned char *)0x57000050)	//RTC alarm control
#define rALMSEC    (*(volatile unsigned char *)0x57000054)	//Alarm second
#define rALMMIN    (*(volatile unsigned char *)0x57000058)	//Alarm minute
#define rALMHOUR   (*(volatile unsigned char *)0x5700005c)	//Alarm Hour
#define rALMDATE   (*(volatile unsigned char *)0x57000060)	//Alarm date  // edited by junon
#define rALMMON    (*(volatile unsigned char *)0x57000064)	//Alarm month
#define rALMYEAR   (*(volatile unsigned char *)0x57000068)	//Alarm year
#define rRTCRST    (*(volatile unsigned char *)0x5700006c)	//RTC round reset
#define rBCDSEC    (*(volatile unsigned char *)0x57000070)	//BCD second
#define rBCDMIN    (*(volatile unsigned char *)0x57000074)	//BCD minute
#define rBCDHOUR   (*(volatile unsigned char *)0x57000078)	//BCD hour
#define rBCDDATE   (*(volatile unsigned char *)0x5700007c)	//BCD date  //edited by junon
#define rBCDDAY    (*(volatile unsigned char *)0x57000080)	//BCD day   //edited by junon
#define rBCDMON    (*(volatile unsigned char *)0x57000084)	//BCD month
#define rBCDYEAR   (*(volatile unsigned char *)0x57000088)	//BCD year
#endif  //RTC


/* ADC */
#define rADCCON    		(*(volatile unsigned *)0x58000000)
#define rADCTSC    		(*(volatile unsigned *)0x58000004)
#define rADCDLY    		(*(volatile unsigned *)0x58000008)
#define rADCDAT0   		(*(volatile unsigned *)0x5800000c)
#define rADCDAT1   		(*(volatile unsigned *)0x58000010)


/* SPI */
#define rSPCON0    (*(volatile unsigned *)0x59000000)	//SPI0 control
#define rSPSTA0    (*(volatile unsigned *)0x59000004)	//SPI0 status
#define rSPPIN0    (*(volatile unsigned *)0x59000008)	//SPI0 pin control
#define rSPPRE0    (*(volatile unsigned *)0x5900000c)	//SPI0 baud rate prescaler
#define rSPTDAT0   (*(volatile unsigned *)0x59000010)	//SPI0 Tx data
#define rSPRDAT0   (*(volatile unsigned *)0x59000014)	//SPI0 Rx data

#define rSPCON1    (*(volatile unsigned *)0x59000020)	//SPI1 control
#define rSPSTA1    (*(volatile unsigned *)0x59000024)	//SPI1 status
#define rSPPIN1    (*(volatile unsigned *)0x59000028)	//SPI1 pin control
#define rSPPRE1    (*(volatile unsigned *)0x5900002c)	//SPI1 baud rate prescaler
#define rSPTDAT1   (*(volatile unsigned *)0x59000030)	//SPI1 Tx data
#define rSPRDAT1   (*(volatile unsigned *)0x59000034)	//SPI1 Rx data

/* SD interface */
#define rSDICON     (*(volatile unsigned *)0x5a000000)	//SDI control
#define rSDIPRE     (*(volatile unsigned *)0x5a000004)	//SDI baud rate prescaler
#define rSDICARG    (*(volatile unsigned *)0x5a000008)	//SDI command argument
#define rSDICCON    (*(volatile unsigned *)0x5a00000c)	//SDI command control
#define rSDICSTA    (*(volatile unsigned *)0x5a000010)	//SDI command status
#define rSDIRSP0    (*(volatile unsigned *)0x5a000014)	//SDI response 0
#define rSDIRSP1    (*(volatile unsigned *)0x5a000018)	//SDI response 1
#define rSDIRSP2    (*(volatile unsigned *)0x5a00001c)	//SDI response 2
#define rSDIRSP3    (*(volatile unsigned *)0x5a000020)	//SDI response 3
#define rSDIDTIMER  (*(volatile unsigned *)0x5a000024)	//SDI data/busy timer
#define rSDIBSIZE   (*(volatile unsigned *)0x5a000028)	//SDI block size
#define rSDIDATCON  (*(volatile unsigned *)0x5a00002c)	//SDI data control
#define rSDIDATCNT  (*(volatile unsigned *)0x5a000030)	//SDI data remain counter
#define rSDIDATSTA  (*(volatile unsigned *)0x5a000034)	//SDI data status
#define rSDIFSTA    (*(volatile unsigned *)0x5a000038)	//SDI FIFO status
#define rSDIIMSK    (*(volatile unsigned *)0x5a000040)	//SDI interrupt mask. edited for 2440A

#ifdef __BIG_ENDIAN
#define rSDIDAT    (*(volatile unsigned *)0x5a00003F)	//SDI data
#define SDIDAT     0x5a00003F
#else  // Little Endian
#define rSDIDAT    (*(volatile unsigned *)0x5a00003C)	//SDI data
#define SDIDAT     0x5a00003C
#endif   //SD Interface


#define _ISR_STARTADDRESS rtems_vector_table
/* ISR */
#define pISR_RESET      (*(unsigned *)(_ISR_STARTADDRESS+0x0))
#define pISR_UNDEF      (*(unsigned *)(_ISR_STARTADDRESS+0x4))
#define pISR_SWI        (*(unsigned *)(_ISR_STARTADDRESS+0x8))
#define pISR_PABORT     (*(unsigned *)(_ISR_STARTADDRESS+0xC))
#define pISR_DABORT     (*(unsigned *)(_ISR_STARTADDRESS+0x10))
#define pISR_RESERVED   (*(unsigned *)(_ISR_STARTADDRESS+0x14))
#define pISR_IRQ        (*(unsigned *)(_ISR_STARTADDRESS+0x18))
#define pISR_FIQ        (*(unsigned *)(_ISR_STARTADDRESS+0x1C))

#define pISR_EINT0      (*(unsigned *)(_ISR_STARTADDRESS+0x20))
#define pISR_EINT1      (*(unsigned *)(_ISR_STARTADDRESS+0x24))
#define pISR_EINT2      (*(unsigned *)(_ISR_STARTADDRESS+0x28))
#define pISR_EINT3      (*(unsigned *)(_ISR_STARTADDRESS+0x2C))
#define pISR_EINT4_7    (*(unsigned *)(_ISR_STARTADDRESS+0x30))
#define pISR_EINT8_23   (*(unsigned *)(_ISR_STARTADDRESS+0x34))
#define pISR_BAT_FLT    (*(unsigned *)(_ISR_STARTADDRESS+0x3C))
#define pISR_TICK       (*(unsigned *)(_ISR_STARTADDRESS+0x40))
#define pISR_WDT        (*(unsigned *)(_ISR_STARTADDRESS+0x44))
#define pISR_TIMER0     (*(unsigned *)(_ISR_STARTADDRESS+0x48))
#define pISR_TIMER1     (*(unsigned *)(_ISR_STARTADDRESS+0x4C))
#define pISR_TIMER2     (*(unsigned *)(_ISR_STARTADDRESS+0x50))
#define pISR_TIMER3     (*(unsigned *)(_ISR_STARTADDRESS+0x54))
#define pISR_TIMER4     (*(unsigned *)(_ISR_STARTADDRESS+0x58))
#define pISR_UART2      (*(unsigned *)(_ISR_STARTADDRESS+0x5C))
#define pISR_NOTUSED    (*(unsigned *)(_ISR_STARTADDRESS+0x60))
#define pISR_DMA0       (*(unsigned *)(_ISR_STARTADDRESS+0x64))
#define pISR_DMA1       (*(unsigned *)(_ISR_STARTADDRESS+0x68))
#define pISR_DMA2       (*(unsigned *)(_ISR_STARTADDRESS+0x6C))
#define pISR_DMA3       (*(unsigned *)(_ISR_STARTADDRESS+0x70))
#define pISR_SDI        (*(unsigned *)(_ISR_STARTADDRESS+0x74))
#define pISR_SPI0       (*(unsigned *)(_ISR_STARTADDRESS+0x78))
#define pISR_UART1      (*(unsigned *)(_ISR_STARTADDRESS+0x7C))
#define pISR_USBD       (*(unsigned *)(_ISR_STARTADDRESS+0x84))
#define pISR_USBH       (*(unsigned *)(_ISR_STARTADDRESS+0x88))
#define pISR_IIC        (*(unsigned *)(_ISR_STARTADDRESS+0x8C))
#define pISR_UART0      (*(unsigned *)(_ISR_STARTADDRESS+0x90))
#define pISR_SPI1       (*(unsigned *)(_ISR_STARTADDRESS+0x94))
#define pISR_RTC        (*(unsigned *)(_ISR_STARTADDRESS+0x98))
#define pISR_ADC        (*(unsigned *)(_ISR_STARTADDRESS+0xA0))


/* PENDING BIT */
#define BIT_EINT0       (0x1)
#define BIT_EINT1       (0x1<<1)
#define BIT_EINT2       (0x1<<2)
#define BIT_EINT3       (0x1<<3)
#define BIT_EINT4_7     (0x1<<4)
#define BIT_EINT8_23    (0x1<<5)
#define BIT_BAT_FLT     (0x1<<7)
#define BIT_TICK        (0x1<<8)
#define BIT_WDT         (0x1<<9)
#define BIT_TIMER0      (0x1<<10)
#define BIT_TIMER1      (0x1<<11)
#define BIT_TIMER2      (0x1<<12)
#define BIT_TIMER3      (0x1<<13)
#define BIT_TIMER4      (0x1<<14)
#define BIT_UART2       (0x1<<15)
#define BIT_LCD         (0x1<<16)
#define BIT_DMA0        (0x1<<17)
#define BIT_DMA1        (0x1<<18)
#define BIT_DMA2        (0x1<<19)
#define BIT_DMA3        (0x1<<20)
#define BIT_SDI         (0x1<<21)
#define BIT_SPI0        (0x1<<22)
#define BIT_UART1       (0x1<<23)
#define BIT_USBD        (0x1<<25)
#define BIT_USBH        (0x1<<26)
#define BIT_IIC         (0x1<<27)
#define BIT_UART0       (0x1<<28)
#define BIT_SPI1       (0x1<<29)
#define BIT_RTC         (0x1<<30)
#define BIT_ADC         (0x1<<31)
#define BIT_ALLMSK      (0xFFFFFFFF)

#define ClearPending(bit) {\
                 rSRCPND = bit;\
                 rINTPND = bit;\
                 rINTPND;\
                 }
/* Wait until rINTPND is changed for the case that the ISR is very short. */
#ifndef ASM
/* Typedefs */
typedef union {
  struct _reg {
    unsigned SM_BIT:1;   /* Enters STOP mode. This bit isn't be */
                           /*    cleared automatically. */
    unsigned Reserved:1;    /* SL_IDLE mode option. This bit isn't cleared */
                           /*    automatically. To enter SL_IDLE mode, */
                           /* CLKCON register has to be 0xe. */
    unsigned IDLE_BIT:1;   /* Enters IDLE mode. This bit isn't be cleared */
    					   /*    automatically. */
	unsigned POWER_OFF:1;
	unsigned NAND_flash:1;
    unsigned LCDC:1;       /* Controls HCLK into LCDC block */
    unsigned USB_host:1;   /* Controls HCLK into USB host block */
    unsigned USB_device:1; /* Controls PCLK into USB device block */
    unsigned PWMTIMER:1;   /* Controls PCLK into PWMTIMER block */
    unsigned SDI:1;        /* Controls PCLK into MMC interface block */
    unsigned UART0:1;      /* Controls PCLK into UART0 block */
    unsigned UART1:1;      /* Controls PCLK into UART1 block */
    unsigned UART2:1;      /* Controls PCLK into UART1 block */
    unsigned GPIO:1;       /* Controls PCLK into GPIO block */
    unsigned RTC:1;        /* Controls PCLK into RTC control block. Even if */
                           /*   this bit is cleared to 0, RTC timer is alive. */
    unsigned ADC:1;        /* Controls PCLK into ADC block */
    unsigned IIC:1;        /* Controls PCLK into IIC block */
    unsigned IIS:1;        /* Controls PCLK into IIS block */
    unsigned SPI:1;        /* Controls PCLK into SPI block */
  } reg;
  unsigned long all;
} CLKCON;

typedef union
{
  struct {
    unsigned ENVID:1;    /* LCD video output and the logic 1=enable/0=disable. */
    unsigned BPPMODE:4;  /* 1011 = 8 bpp for TFT, 1100 = 16 bpp for TFT, */
                         /*   1110 = 16 bpp TFT skipmode */
    unsigned PNRMODE:2;  /* TFT: 3 */
    unsigned MMODE:1;    /* This bit determines the toggle rate of the VM. */
                         /*   0 = Each Frame, 1 = The rate defined by the MVAL */
    unsigned CLKVAL:10;  /* TFT: VCLK = HCLK / [(CLKVAL+1) x 2] (CLKVAL >= 1) */
    unsigned LINECNT:10; /* (read only) These bits provide the status of the */
                         /*   line counter. Down count from LINEVAL to 0 */
  } reg;
  unsigned long all;
} LCDCON1;

typedef union {
  struct {
    unsigned VSPW:6;    /* TFT: Vertical sync pulse width determines the */
                        /*   VSYNC pulse's high level width by counting the */
                        /*   number of inactive lines. */
    unsigned VFPD:8;    /* TFT: Vertical front porch is the number of */
                        /*   inactive lines at the end of a frame, before */
                        /*   vertical synchronization period. */
    unsigned LINEVAL:10;  /* TFT/STN: These bits determine the vertical size */
                        /*   of LCD panel. */
    unsigned VBPD:8;    /* TFT: Vertical back porch is the number of inactive */
                        /*   lines at the start of a frame, after */
                        /*   vertical synchronization period. */
  } reg;
  unsigned long all;
} LCDCON2;

typedef union {
  struct {
    unsigned HFPD:8;    /* TFT: Horizontal front porch is the number of */
                        /*   VCLK periods between the end of active data */
                        /*   and the rising edge of HSYNC. */
    unsigned HOZVAL:11; /* TFT/STN: These bits determine the horizontal */
                        /*   size of LCD panel. 2n bytes. */
    unsigned HBPD:7;    /* TFT: Horizontal back porch is the number of VCLK */
                        /*   periods between the falling edge of HSYNC and */
                        /*   the start of active data. */
  } reg;
  unsigned long all;
} LCDCON3;

typedef union {
  struct {
    unsigned HSPW:8;     /* TFT: Horizontal sync pulse width determines the */
                         /*   HSYNC pulse's high level width by counting the */
                         /*   number of the VCLK. */
    unsigned MVAL:8;     /* STN: */
  } reg;
  unsigned long all;
} LCDCON4;

typedef union {
  struct {
    unsigned HWSWP:1;     /* STN/TFT: Half-Word swap control bit. */
                          /*    0 = Swap Disable 1 = Swap Enable */
    unsigned BSWP:1;      /* STN/TFT: Byte swap control bit. */
                          /*    0 = Swap Disable 1 = Swap Enable */
    unsigned ENLEND:1;    /* TFT: LEND output signal enable/disable. */
                          /*    0 = Disable LEND signal. */
                          /*    1 = Enable LEND signal */
    unsigned PWREN:1;
    unsigned INVLEND:1;/* TFT: This bit indicates the LEND signal */
                          /*    polarity. 0 = normal 1 = inverted */
    unsigned INVPWREN:1;
    unsigned INVVDEN:1;   /* TFT: This bit indicates the VDEN signal */
                          /*    polarity. */
                          /*    0 = normal 1 = inverted */
    unsigned INVVD:1;     /* STN/TFT: This bit indicates the VD (video data) */
                          /*    pulse polarity. 0 = Normal. */
                          /*    1 = VD is inverted. */
    unsigned INVVFRAME:1; /* STN/TFT: This bit indicates the VFRAME/VSYNC */
                          /*    pulse polarity. 0 = normal 1 = inverted */
    unsigned INVVLINE:1;  /* STN/TFT: This bit indicates the VLINE/HSYNC */
                          /*    pulse polarity. 0 = normal 1 = inverted */
    unsigned INVVCLK:1;   /* STN/TFT: This bit controls the polarity of the */
                          /*    VCLK active edge. 0 = The video data is */
                          /*    fetched at VCLK falling edge. 1 = The video */
                          /*    data is fetched at VCLK rising edge */
    unsigned FRM565:1;
    unsigned BPP24BL:1;
    unsigned HSTATUS:2;   /* TFT: Horizontal Status (Read only) */
                          /*    00 = HSYNC */
                          /*    01 = BACK Porch. */
                          /*    10 = ACTIVE */
                          /*    11 = FRONT Porch */
    unsigned VSTATUS:2;   /* TFT: Vertical Status (Read only). */
                          /*    00 = VSYNC */
                          /*    01 = BACK Porch. */
                          /*    10 = ACTIVE */
                          /*    11 = FRONT Porch */
    unsigned RESERVED:16;
  } reg;
  unsigned long all;
} LCDCON5;

typedef union {
  struct {
    unsigned LCDBASEU:21; /* For single-scan LCD: These bits indicate */
                          /*    A[21:1] of the start address of the LCD */
                          /*    frame buffer. */
    unsigned LCDBANK:9;   /* A[28:22] */
  } reg;
  unsigned long all;
} LCDSADDR1;

typedef union {
  struct {
    unsigned LCDBASEL:21; /* For single scan LCD: These bits indicate A[21:1]*/
                          /*    of the end address of the LCD frame buffer. */
                          /*    LCDBASEL = ((the fame end address) >>1) + 1 */
                          /*    = LCDBASEU + (PAGEWIDTH+OFFSIZE)x(LINEVAL+1) */
  } reg;
  unsigned long all;
} LCDSADDR2;

typedef union {
  struct {
    unsigned PAGEWIDTH:11; /* Virtual screen page width(the number of half */
                          /*    words) This value defines the width of the */
                          /*    view port in the frame */
    unsigned OFFSIZE:11;  /* Virtual screen offset size(the number of half */
                          /*    words) This value defines the difference */
                          /*    between the address of the last half word */
                          /*    displayed on the previous LCD line and the */
                          /*    address of the first half word to be */
                          /*    displayed in the new LCD line. */
  } reg;
  unsigned long all;
} LCDSADDR3;

/*
 *
 */

typedef union {
  struct {
    unsigned IISIFENA:1;  /* IIS interface enable (start) */
    unsigned IISPSENA:1;  /* IIS prescaler enable */
    unsigned RXCHIDLE:1;  /* Receive channel idle command */
    unsigned TXCHIDLE:1;  /* Transmit channel idle command */
    unsigned RXDMAENA:1;  /* Receive DMA service request enable */
    unsigned TXDMAENA:1;  /* Transmit DMA service request enable */
    unsigned RXFIFORDY:1; /* Receive FIFO ready flag (read only) */
    unsigned TXFIFORDY:1; /* Transmit FIFO ready flag (read only) */
    unsigned LRINDEX:1;   /* Left/right channel index (read only) */
  } reg;
  unsigned long all;
} IISCON;

typedef union {
  struct {
    unsigned SBCLKFS:2;  /* Serial bit clock frequency select */
    unsigned MCLKFS:1;   /* Master clock frequency select */
    unsigned SDBITS:1;   /* Serial data bit per channel */
    unsigned SIFMT:1;    /* Serial interface format */
    unsigned ACTLEVCH:1; /* Active level pf left/right channel */
    unsigned TXRXMODE:2; /* Transmit/receive mode select */
    unsigned MODE:1;     /* Master/slave mode select */
  } reg;
  unsigned long all;
} IISMOD;

typedef union {
  struct {
    unsigned PSB:5;      /* Prescaler control B */
    unsigned PSA:5;      /* Prescaler control A */
  } reg;
  unsigned long all;
} IISPSR;

typedef union {
  struct {
    unsigned RXFIFOCNT:6;  /* (read only) */
    unsigned TXFIFOCNT:6;  /* (read only) */
    unsigned RXFIFOENA:1;  /* */
    unsigned TXFIFOENA:1;  /* */
    unsigned RXFIFOMODE:1; /* */
    unsigned TXFIFOMODE:1; /* */
  } reg;
  unsigned long all;
} IISSFIFCON;

typedef union {
  struct {
    unsigned FENTRY:16;    /* */
  } reg;
  unsigned long all;
} IISSFIF;
#endif //ASM

#define LCD_WIDTH 240
#define LCD_HEIGHT 320
#define LCD_ASPECT ((float)(LCD_WIDTH/LCD_HEIGHT))

#define SMDK2410_KEY_SELECT 512
#define SMDK2410_KEY_START 256
#define SMDK2410_KEY_A 64
#define SMDK2410_KEY_B 32
#define SMDK2410_KEY_L 16
#define SMDK2410_KEY_R 128
#define SMDK2410_KEY_UP 8
#define SMDK2410_KEY_DOWN 2
#define SMDK2410_KEY_LEFT 1
#define SMDK2410_KEY_RIGHT 4

#endif /*S3C2410_H_*/
