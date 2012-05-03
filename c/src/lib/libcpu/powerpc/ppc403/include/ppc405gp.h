
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
	uint32_t indivHash[4];
	uint32_t groupHash[4];
	uint32_t lastSrcLo;
	uint32_t lastSrcHi;
	uint32_t IPGap;
	uint32_t STAcontrol;
	uint32_t xmtReqThreshold;
	uint32_t rcvWatermark;
	uint32_t bytesXmtd;
	uint32_t bytesRcvd;
} EthernetRegisters_GP;

enum { EMACAddress = 0xEF600800 };

enum {
	// Mode 0 bits
	kEMACRxIdle   = 0x80000000,
	kEMACTxIdle   = 0x40000000,
	kEMACSoftRst  = 0x20000000,
	kEMACTxEnable = 0x10000000,
	kEMACRxEnable = 0x08000000,

	// Mode 1 bits
	kEMACFullDuplex = 0x80000000,
	kEMACIgnoreSQE  = 0x01000000,
	kEMAC100MBbps	 = 0x00400000,
	kEMAC4KRxFIFO   = 0x00300000,
	kEMAC2KTxFIFO	 = 0x00080000,
	kEMACTx0Multi	 = 0x00008000,
	kEMACTxDependent= 0x00014000,

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

	// Buffer descriptor control bits
	kMALTxReady			= 0x8000,
	kMALRxEmpty			= 0x8000,
	kMALWrap				= 0x4000,
	kMALContinuous		= 0x2000,
	kMALLast				= 0x1000,
	kMALRxFirst			= 0x0800,
	kMALInterrupt		= 0x0400,

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


