/*

Constants for manipulating system registers of PPC 405EX in C

Michael Hamel ADInstruments May 2008

*/

#include <libcpu/powerpc-utility.h>
/* Indirect access to Clocking/Power-On registers */
#define CPR0_DCR_BASE	0x0C
#define cprcfga		(CPR0_DCR_BASE+0x0)
#define cprcfgd		(CPR0_DCR_BASE+0x1)

#define mtcpr(reg, d)					\
  do {							\
    PPC_SET_DEVICE_CONTROL_REGISTER(cprcfga,reg);	\
    PPC_SET_DEVICE_CONTROL_REGISTER(cprcfgd,d);		\
  } while (0)

#define mfcpr(reg, d)					\
  do {							\
    PPC_SET_DEVICE_CONTROL_REGISTER(cprcfga,reg);	\
    d = PPC_DEVICE_CONTROL_REGISTER(cprcfgd);		\
  } while (0)


/* Indirect access to System registers */
#define SDR_DCR_BASE	0x0E
#define sdrcfga		(SDR_DCR_BASE+0x0)
#define sdrcfgd		(SDR_DCR_BASE+0x1)

#define mtsdr(reg, d)					\
  do {							\
    PPC_SET_DEVICE_CONTROL_REGISTER(sdrcfga,reg);	\
    PPC_SET_DEVICE_CONTROL_REGISTER(sdrcfgd,d);		\
  } while (0)

#define mfsdr(reg, d)					\
    do {						\
      PPC_SET_DEVICE_CONTROL_REGISTER(sdrcfga,reg);	\
      d = PPC_DEVICE_CONTROL_REGISTER(sdrcfgd);		\
    } while (0)

/* Indirect access to EBC registers */
#define EBC_DCR_BASE	0x12
#define ebccfga		(EBC_DCR_BASE+0x0)
#define ebccfgd		(EBC_DCR_BASE+0x1)

#define mtebc(reg, d)					\
  do {							\
    PPC_SET_DEVICE_CONTROL_REGISTER(ebccfga,reg);	\
    PPC_SET_DEVICE_CONTROL_REGISTER(ebccfgd,d);		\
  } while (0)

#define mfebc(reg, d)					\
  do {							\
    PPC_SET_DEVICE_CONTROL_REGISTER(ebccfga,reg);	\
    d = PPC_DEVICE_CONTROL_REGISTER(ebccfgd);		\
  } while (0)

/* EBC DCRs */
enum {
	/*
	EBC0_B0CR	=	0,
	EBC0_B1CR	=	1,
	EBC0_B2CR	=	2,
	EBC0_B3CR	=	3,
	EBC0_B0AP	=	0x10,
	EBC0_B1AP	=	0x11,
	EBC0_B2AP	=	0x12,
	EBC0_B3AP	=	0x13,
	EBC0_BEAR	=	0x20,
	EBC0_BESR	=	0x21,
	EBC0_CFG	=	0x23,
	*/
	EBC0_CID	=	0x24
};

enum {
	SDR0_UART0	= 0x120,
	SDR0_UART1  = 0x121,
	SDR0_C405	= 0x180,
	SDR0_MALTBL = 0x280,
	SDR0_MALRBL = 0x2A0,
	SDR0_MALTBS = 0x2C0,
	SDR0_MALRBS = 0x2E0
};


/* Memory-mapped registers */


/*======================= Ethernet =================== */


typedef struct EthernetRegisters_EX {
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
	uint32_t multicastAddr[2];
	uint32_t multicastMask[2];
	uint32_t unused[4];
	uint32_t lastSrcLo;
	uint32_t lastSrcHi;
	uint32_t IPGap;
	uint32_t STAcontrol;
	uint32_t xmtReqThreshold;
	uint32_t rcvWatermark;
	uint32_t bytesXmtd;
	uint32_t bytesRcvd;
	uint32_t unused2;
	uint32_t revID;
	uint32_t unused3[2];
	uint32_t indivHash[8];
	uint32_t groupHash[8];
	uint32_t xmtPause;
} EthernetRegisters_EX;

enum {
	EMAC0Address = 0xEF600900,
	EMAC1Address = 0xEF600A00
};


typedef struct GPIORegisters {
	uint32_t OR;
	uint32_t GPIO_TCR;		/* Note that TCR is defined as a DCR name */
	uint32_t OSRL;
	uint32_t OSRH;
	uint32_t TSRL;
	uint32_t TSRH;
	uint32_t ODR;
	uint32_t IR;
	uint32_t RR1;
	uint32_t RR2;
	uint32_t RR3;
	uint32_t unknown;
	uint32_t ISR1L;
	uint32_t ISR1H;
	uint32_t ISR2L;
	uint32_t ISR2H;
	uint32_t ISR3L;
	uint32_t ISR3H;
} GPIORegisters;

enum { GPIOAddress = 0xEF600800 };

