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
	SDR0_PINSTP = 0x40,
	SDR0_UART0	= 0x120,
	SDR0_UART1  = 0x121,
	SDR0_C405	= 0x180,
	SDR0_SRST0  = 0x200,
	SDR0_MALTBL = 0x280,
	SDR0_MALRBL = 0x2A0,
	SDR0_MALTBS = 0x2C0,
	SDR0_MALRBS = 0x2E0,
	SDR0_PFC2   = 0x4102,
	SDR0_MFR    = 0x4300,
	SDR0_EMAC0RXST = 0x4301,
	SDR0_HSF    = 0x4400
};

enum {
	CPR0_CLKUPD = 0x20,
	CPR0_PLLC = 0x40,
	CPR0_PLLD = 0x60,
	CPR0_CPUD = 0x80,
	CPR0_PLBD = 0xA0,
	CPR0_OPBD = 0xC0,
	CPR0_PERD = 0xE0,
	CPR0_AHBD = 0x100,
	CPR0_ICFG = 0x140
};

/* Memory-mapped registers */


/*======================= Ethernet =================== */

enum {
	EMAC0EXAddress = 0xEF600900,
	EMAC1EXAddress = 0xEF600A00,
	
	/* 405EX-specific bits in EMAC_MR1 */
	keEMAC1000Mbps = 0x00800000,
	keEMAC16KRxFIFO = 0x00280000,
	keEMAC8KRxFIFO  = 0x00200000,
	keEMAC4KRxFIFO  = 0x00180000,
	keEMAC2KRxFIFO  = 0x00100000,
	keEMAC1KRxFIFO  = 0x00080000,
	keEMAC16KTxFIFO = 0x00050000,
	keEMAC8KTxFIFO  = 0x00040000,
	keEMAC4KTxFIFO  = 0x00030000,
	keEMAC2KTxFIFO  = 0x00020000,
	keEMAC1KTxFIFO  = 0x00010000,
	keEMACJumbo	    = 0x00000800,
	keEMACIPHYAddr4 = 0x180,
	keEMACOPB50MHz  = 0x00,
	keEMACOPB66MHz  = 0x08,
	keEMACOPB83MHz  = 0x10,
	keEMACOPB100MHz = 0x18,
	keEMACOPBGt100  = 0x20,
	
	/* 405EX-specific bits in MAL0_CFG */
	keMALRdMaxBurst4  = 0,
	keMALRdMaxBurst8  = 0x00100000,
	keMALRdMaxBurst16 = 0x00200000,
	keMALRdMaxBurst32 = 0x00300000,
	
	keMALWrLowPriority    = 0,
	keMALWrMedLowPriority = 0x00040000,
	keMALWrMedHiPriority  = 0x00080000,
	keMALWrHighPriority   = 0x000C0000,

	keMALWrMaxBurst4  = 0,
	keMALWrMaxBurst8  = 0x00010000,
	keMALWrMaxBurst16 = 0x00020000,
	keMALWrMaxBurst32 = 0x00030000,
	
	/* 405EX-specific STA bits */
	keSTARun	  = 0x8000,
	keSTADirectRd = 0x1000,
	keSTADirectWr = 0x0800,
	keSTAIndirAddr = 0x2000,
	keSTAIndirRd  = 0x3000,
	keSTAIndirWr  = 0x2800
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

typedef struct RGMIIRegisters {
	uint32_t FER;
	uint32_t SSR;
} RGMIIRegisters;

enum { RGMIIAddress = 0xEF600B00 };

