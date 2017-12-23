
/* SDRAM DCRs */
enum {
	SDRAM0_BESR0	=	0,
	SDRAM0_BESR1	=	8,
	SDRAM0_BEAR		=	0x10,
	SDRAM0_CFG		=	0x20,
	SDRAM0_STATUS	=	0x24,
	SDRAM0_RTR		=	0x30,
	SDRAM0_PMIT		=	0x34,
	SDRAM0_TR		=	0x80
};


/* EBC DCRs */
enum {
	EBC0_B0CR	=	0,
	EBC0_B1CR	=	1,
	EBC0_B2CR	=	2,
	EBC0_B3CR	=	3,
	EBC0_B4CR	=	4,
	EBC0_B5CR	=	5,
	EBC0_B6CR	=	6,
	EBC0_B7CR	=	7,
	EBC0_B0AP	=	0x10,
	EBC0_B1AP	=	0x11,
	EBC0_B2AP	=	0x12,
	EBC0_B3AP	=	0x13,
	EBC0_B4AP	=	0x14,
	EBC0_B5AP	=	0x15,
	EBC0_B6AP	=	0x16,
	EBC0_B7AP	=	0x17,
	EBC0_BEAR	=	0x20,
	EBC0_BESR0	=	0x21,
	EBC0_BESR1	=	0x22,
	EBC0_CFG	=	0x23
};

/* MAL DCRs, have to be #defines */
#define	MAL0_CFG		0x180
#define	MAL0_ESR		0x181
#define	MAL0_IER		0x182
#define	MAL0_TXCASR		0x184
#define	MAL0_TXCARR		0x185
#define	MAL0_TXEOBISR	0x186
#define	MAL0_TXDEIR		0x187
#define	MAL0_RXCASR		0x190
#define	MAL0_RXCARR		0x191
#define	MAL0_RXEOBISR	0x192
#define	MAL0_RXDEIR		0x193
#define	MAL0_TXCTP0R	0x1A0
#define	MAL0_TXCTP1R	0x1A1
#define	MAL0_RXCTP0R	0x1C0
#define	MAL0_RXCTP1R	0x1C1
#define	MAL0_RCBS0		0x1E0
#define	MAL0_RCBS1		0x1E1

/* Memory-mapped registers */

typedef struct EthernetRegisters_GP {
	uint32_t mode0;
	uint32_t mode1;
	uint32_t xmtMode0;
	uint32_t xmtMode1;
	uint32_t rcvMode;
	uint32_t intStatus;
	uint32_t intEnable;
	uint32_t addrHi;
	uint32_t addrLo;
	uint32_t VLANTPID;
	uint32_t VLANTCI;
	uint32_t pauseTimer;
	uint32_t g_indivHash[4];  /* EX non-IP multicast addr/mask */
	uint32_t g_groupHash[4];
	uint32_t lastSrcLo;
	uint32_t lastSrcHi;
	uint32_t IPGap;
	uint32_t STAcontrol;
	uint32_t xmtReqThreshold;
	uint32_t rcvWatermarks;
	uint32_t bytesXmtd;
	uint32_t bytesRcvd;
	uint32_t e_unused2;
	uint32_t e_revID;
	uint32_t e_unused3[2];
	uint32_t e_indivHash[8];
	uint32_t e_groupHash[8];
	uint32_t e_xmtPause;
} EthernetRegisters_GP;

typedef struct EthernetRegisters_GP EthernetRegisters_EX;

enum { EMACAddress = 0xEF600800 };
enum { EMAC0GPAddress = 0xEF600800 };

enum {
	// Mode 0 bits
	kEMACRxIdle   = 0x80000000,
	kEMACTxIdle   = 0x40000000,
	kEMACSoftRst  = 0x20000000,
	kEMACTxEnable = 0x10000000,
	kEMACRxEnable = 0x08000000,

	// Mode 1 bits
	kEMACFullDuplex = 0x80000000,
	kEMACDoFlowControl = 0x10000000,
	kEMACIgnoreSQE  = 0x01000000,
	kEMAC100MBbps	 = 0x00400000,
	kEMAC4KRxFIFO   = 0x00300000,
	kEMAC2KTxFIFO	 = 0x00080000,
	kEMACTx0Multi	 = 0x00008000,
	kEMACTxDependent= 0x00014000,
	kEMAC100Mbps	 = 0x00400000,
	kgEMAC4KRxFIFO   = 0x00300000,
	kgEMAC2KTxFIFO	 = 0x00080000,
	kgEMACTx0Multi	 = 0x00008000,
	kgEMACTxDependent= 0x00014000,


	// Tx mode bits
	kEMACNewPacket0	= 0x80000000,
	kEMACNewPacket1	= 0x40000000,

	// Receive mode bits
	kEMACStripPadding = 0x80000000,
	kEMACStripFCS     = 0x40000000,
	kEMACRcvRunts		= 0x20000000,
	kEMACRcvFCSErrs	= 0x10000000,
	kEMACRcvOversize  = 0x08000000,
	kEMACPromiscRcv	= 0x01000000,
	kEMACPromMultRcv  = 0x00800000,
	kEMACIndivRcv		= 0x00400000,
	kEMACHashRcv		= 0x00200000,
	kEMACBrcastRcv		= 0x00100000,
	kEMACMultcastRcv	= 0x00080000,
	keEMACNonIPMultcast = 0x00040000,
	keEMACRxFIFOAFMax   = 7,
	
	// EMAC_STACR bits
	kgSTAComplete   = 0x8000,
	kSTAErr		= 0x4000,
	
	// Interrupt status bits
	kEMACIOverrun = 0x02000000,
	kEMACIPause   = 0x01000000,
	kEMACIBadPkt  = 0x00800000,
	kEMACIRuntPkt = 0x00400000,
	kEMACIShortEvt= 0x00200000,
	kEMACIAlignErr= 0x00100000,
	kEMACIBadFCS  = 0x00080000,
	kEMACIOverSize= 0x00040000,
	kEMACILLCRange= 0x00020000,
	kEMACISQEErr  = 0x00000080,
	kEMACITxErr   = 0x00000040,

	// Buffer descriptor control bits
	kMALTxReady			= 0x8000,
	kMALRxEmpty			= 0x8000,
	kMALWrap				= 0x4000,
	kMALContinuous		= 0x2000,
	kMALLast				= 0x1000,
	kMALRxFirst			= 0x0800,
	kMALInterrupt		= 0x0400,

	kMALReset			= 0x80000000,
	kMALLowPriority    = 0,
	kMALMedLowPriority = 0x00400000,
	kMALMedHiPriority  = 0x00800000,
	kMALHighPriority   = 0x00C00000,
	kMALLatency8       = 0x00040000,
	kMALLockErr	    = 0x8000,
	kMALCanBurst	= 0x4000,
	kMALLocksOPB    = 0x80,
	kMALLocksErrs	= 0x2,
	
	// MAL channel masks
	kMALChannel0 = 0x80000000,
	kMALChannel1 = 0x40000000,

	// EMAC Tx descriptor bits sent
	kEMACGenFCS			= 0x200,
	kEMACGenPad			= 0x100,
	kEMACInsSrcAddr	= 0x080,
	kEMACRepSrcAddr	= 0x040,
	kEMACInsVLAN		= 0x020,
	kEMACRepVLAN		= 0x010,

	// EMAC TX descriptor bits returned
	kEMACErrMask      = 0x3FF,
	kEMACFCSWrong		= 0x200,
	kEMACBadPrev		= 0x100,
	kEMACLostCarrier	= 0x080,
	kEMACDeferred		= 0x040,
	kEMACCollFail		= 0x020,
	kEMACLateColl		= 0x010,
	kEMACMultColl		= 0x008,
	kEMACOneColl		= 0x004,
	kEMACUnderrun		= 0x002,
	kEMACSQEFail		= 0x001,

	// EMAC Rx descriptor bits returned
	kEMACOverrun  		= 0x200,
	kEMACPausePkt 		= 0x100,
	kEMACBadPkt	  		= 0x080,
	kEMACRuntPkt  		= 0x040,
	kEMACShortEvt 		= 0x020,
	kEMACAlignErr 		= 0x010,
	kEMACBadFCS   		= 0x008,
	kEMACPktLong  		= 0x004,
	kEMACPktOOR   		= 0x002,
	kEMACPktIRL   		= 0x001
};


