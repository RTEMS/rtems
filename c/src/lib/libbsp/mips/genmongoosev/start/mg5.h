/*
**
** Section 1: Registers
**
*/

/*
**
*/
#define  PMON_ADDRESS  0xbfc00000

/*
** Mongoose V Peripheral Function Registers
*/

#define MG5_COMMAND_REG     0xfffe0180

/*
** Extended Interrupt Registers
** These registers are used as follows:
** 1. The mask register is used to allow peripheral function and external
**    interrupts to function. To enable an interrupt, set the appropriate
**    bit.
** 2. The status register contains the state of the peripheral functions
**    and external devices. This register should be read to poll devices
**    such as the uart when the interrupts are not enabled. Writing to this
**    register will clear the interrupt.
** 3. The Cause register is similar to the status register, with the
**    exception that it only shows the bits that are set in the mask
**    register. This register should be read to determine which interrupt(s)
**    need to be serviced. This register when written to will CAUSE the interrupt.
*/
#define MG5_INT_STATUS_REG      0xfffe0184   /* Read to determine state/Write to clear */
#define MG5_INT_CAUSE_REG       0xfffe0188   /* Read to determine int/Write to cause int */
#define MG5_INT_MASK_REG        0xfffe018c   /* Set bit here to enable int */

/*
** EDAC Registers
*/
#define MG5_EDAC_ADDR_REG       0xfffe0190   /* edac error address */
#define MG5_EDAC_PARITY_REG     0xfffe0194   /* edac parity */

/*
** High speed serial port registers
** This section is reserved for future Mongoose Processors
*/

/*
** Floating Point Register
*/
#define MG5_FPU_CNTRL_REG     0xfffe0020     /* FPU control register */

/*
** MAVN Registers
*/
#define MG5_MAVN_TEST_REG   0xfffe01b4   /* test mavn */
#define MG5_MAVN_PRIVLEGE_REG   0xfffe01b8   /* privlege bits */
#define MG5_MAVN_VIOLATION_REG  0xfffe01bc   /* address of violation */
#define MG5_MAVN_RANGE_0_REG    0xfffe01c0   /* Range 0 */
#define MG5_MAVN_RANGE_1_REG    0xfffe01c4   /* Range 1 */
#define MG5_MAVN_RANGE_2_REG    0xfffe01c8   /* Range 2 */
#define MG5_MAVN_RANGE_3_REG    0xfffe01cc   /* Range 3 */
#define MG5_MAVN_RANGE_4_REG    0xfffe01d0   /* Range 4 */
#define MG5_MAVN_RANGE_5_REG    0xfffe01d4   /* Range 5 */

/*
** Uart Specific Peripheral Function Registers
*/
#define MG5_UART_0_RX_REG   0xfffe01e8
#define MG5_UART_0_TX_REG       0xfffe01ec
#define MG5_UART_0_BAUD_REG     0xfffe01f0
#define MG5_UART_1_RX_REG       0xfffe01f4
#define MG5_UART_1_TX_REG       0xfffe01f8
#define MG5_UART_1_BAUD_REG     0xfffe01fc

/*
** Section 2: Bit definitions
**
*/

/*
** Command Register Bits - defined from 31 to 0
*/
#define EDAC_ENABLE_BIT          0x80000000
#define EDAC_OVERRIDE_BIT        0x40000000
/* 29 - 16 reserved */
#define UART_1_PARITY_EVEN_BIT  0x00008000
#define UART_1_PARITY_ENABLE_BIT   0x00004000
#define UART_1_RTS_BIT           0x00002000
#define UART_1_TX_ENABLE_BIT         0x00001000
#define UART_1_RX_ENABLE_BIT         0x00000800
#define UART_1_TX_BREAK_BIT      0x00000400

#define UART_0_PARITY_EVEN_BIT  0x00000200
#define UART_0_PARITY_ENABLE_BIT   0x00000100
#define UART_0_RTS_BIT           0x00000080
#define UART_0_TX_ENABLE_BIT         0x00000040
#define UART_0_RX_ENABLE_BIT         0x00000020
#define UART_0_TX_BREAK_BIT      0x00000010

#define UART_LOOPBACK_MODE_BIT  0x00000008
#define UART_CTSN_TEST_BIT       0x00000004
#define UART_RESET_BIT           0x00000002

/*
** Interrupt Status/Cause/Mask register bits - from 31 to 0
*/
#define EDAC_SERR_BIT            0x80000000
#define EDAC_MERR_BIT            0x40000000
/* 29 - 24 reserved */
#define UART_0_RX_READY_BIT      0x00008000
#define UART_0_TX_READY_BIT        0x00004000
#define UART_0_TX_EMPTY_BIT      0x00002000
#define UART_0_RX_OVERRUN_BIT      0x00001000
#define UART_0_FRAME_ERR_BIT       0x00000800
#define UART_0_RESERVED_BIT        0x00000400
#define UART_1_RX_READY_BIT        0x00200000
#define UART_1_TX_READY_BIT        0x00100000
#define UART_1_TX_EMPTY_BIT        0x00080000
#define UART_1_RX_OVERRUN_BIT      0x00040000
#define UART_1_FRAME_ERR_BIT       0x00020000
#define UART_1_RESERVED_BIT        0x00010000
#define MAVN_WRITE_ACC_BIT       0x00400000
#define MAVN_READ_ACC_BIT        0x00800000
#define EXTERN_INT_9_BIT           0x00000200
#define EXTERN_INT_8_BIT           0x00000100
#define EXTERN_INT_7_BIT           0x00000080
#define EXTERN_INT_6_BIT           0x00000040
#define EXTERN_INT_5_BIT           0x00000020
#define EXTERN_INT_4_BIT           0x00000010
#define EXTERN_INT_3_BIT           0x00000008
#define EXTERN_INT_2_BIT           0x00000004
#define EXTERN_INT_1_BIT           0x00000002
#define EXTERN_INT_0_BIT           0x00000001

/*
** MAVN Range Bits
*/

#define MAVN_RANGE_0_WRITE_BIT  0x00000001
#define MAVN_RANGE_1_WRITE_BIT  0x00000002
#define MAVN_RANGE_2_WRITE_BIT  0x00000004
#define MAVN_RANGE_3_WRITE_BIT  0x00000008
#define MAVN_RANGE_4_WRITE_BIT  0x00000010
#define MAVN_RANGE_5_WRITE_BIT  0x00000020

#define MAVN_GLOBAL_WRITE_BIT     0x00000200

#define MAVN_RANGE_0_READ_BIT     0x00000400
#define MAVN_RANGE_1_READ_BIT   0x00000800
#define MAVN_RANGE_2_READ_BIT   0x00001000
#define MAVN_RANGE_3_READ_BIT   0x00002000
#define MAVN_RANGE_4_READ_BIT   0x00004000
#define MAVN_RANGE_5_READ_BIT   0x00008000

#define MAVN_GLOBAL_READ_BIT      0x00080000

#define MAVN_ENABLE_BIT         0x80000000
#define MAVN_TEST_BIT           0x40000000

#define MAVN_RANGE_NO_ACESS  0x00000000

#define MAVN_PS_CODE_512     0x00000009
#define MAVN_PS_CODE_1K      0x0000000a
#define MAVN_PS_CODE_2K      0x0000000b
#define MAVN_PS_CODE_4K      0x0000000c
#define MAVN_PS_CODE_8K      0x0000000d
#define MAVN_PS_CODE_16K     0x0000000e
#define MAVN_PS_CODE_32K     0x0000000f
#define MAVN_PS_CODE_64K     0x00000010
#define MAVN_PS_CODE_128K    0x00000011
#define MAVN_PS_CODE_256K    0x00000012
#define MAVN_PS_CODE_512K    0x00000013
#define MAVN_PS_CODE_1M      0x00000014
#define MAVN_PS_CODE_2M      0x00000015

/*
** FPU Control Bits
*/
#define FPU_CNTRL_CONDITION   0x00800000
#define FPU_CNTRL_EXCEPT_E    0x00020000
#define FPU_CNTRL_EXCEPT_V    0x00010000
#define FPU_CNTRL_EXCEPT_Z    0x00008000
#define FPU_CNTRL_EXCEPT_O    0x00004000
#define FPU_CNTRL_EXCEPT_U    0x00002000
#define FPU_CNTRL_EXCEPT_I    0x00001000
#define FPU_CNTRL_TRAP_V      0x00000800
#define FPU_CNTRL_TRAP_Z      0x00000400
#define FPU_CNTRL_TRAP_O      0x00000200
#define FPU_CNTRL_TRAP_U      0x00000100
#define FPU_CNTRL_TRAP_I      0x00000080
#define FPU_CNTRL_STICKY_V    0x00000040
#define FPU_CNTRL_STICKY_Z    0x00000020
#define FPU_CNTRL_STICKY_O    0x00000010
#define FPU_CNTRL_STICKY_U    0x00000008
#define FPU_CNTRL_STICKY_I    0x00000004
#define FPU_CNTRL_ROUND_RN    0x00000000
#define FPU_CNTRL_ROUND_RZ    0x00000001
#define FPU_CNTRL_ROUND_RP    0x00000002
#define FPU_CNTRL_ROUND_RM    0x00000003

#define FPU_EXCEPTIONS        FPU_CNTRL_TRAP_V|FPU_CNTRL_TRAP_Z|FPU_CNTRL_TRAP_O|FPU_CNTRL_TRAP_U|FPU_CNTRL_TRAP_I
#define FPU_CONFIGURATION     FPU_EXCEPTIONS|FPU_CNTRL_ROUND_RN

/*
**
** Section 3 -- Masks
**
*/

#define UART_TX_BAUD_MASK       0x00007FFF
#define UART_RX_BAUD_MASK       0x7FFF0000
#define UART_DATA_MASK          0x000000FF

#define UART_TX_BAUD_4800(x)    ((((x*1000000)/4800) - 1) & UART_TX_BAUD_MASK)
#define UART_TX_BAUD_9600(x)    ((((x*1000000)/9600) - 1) & UART_TX_BAUD_MASK)
#define UART_TX_BAUD_19200(x)   ((((x*1000000)/19200) - 1)& UART_TX_BAUD_MASK)
#define UART_TX_BAUD_38400(x)   ((((x*1000000)/38400) - 1)& UART_TX_BAUD_MASK)

#define UART_RX_BAUD_4800(x)    (((((x*1000000)/4800) - 1)  << 16) & UART_RX_BAUD_MASK)
#define UART_RX_BAUD_9600(x)    (((((x*1000000)/9600) - 1)  << 16) & UART_RX_BAUD_MASK)
#define UART_RX_BAUD_19200(x)   (((((x*1000000)/19200) - 1) << 16)& UART_RX_BAUD_MASK)
#define UART_RX_BAUD_38400(x)   (((((x*1000000)/38400) - 1) << 16)& UART_RX_BAUD_MASK)

#define UART_BAUD_4800(x)    ((((x*1000000)/4800) - 1) & UART_TX_BAUD_MASK) | (((((x*1000000)/4800) - 1)  << 16) & UART_RX_BAUD_MASK)
#define UART_BAUD_9600(x)    ((((x*1000000)/9600) - 1) & UART_TX_BAUD_MASK) | (((((x*1000000)/9600) - 1)  << 16) & UART_RX_BAUD_MASK)
#define UART_BAUD_19200(x)   ((((x*1000000)/19200) - 1)& UART_TX_BAUD_MASK) | (((((x*1000000)/19200) - 1) << 16)& UART_RX_BAUD_MASK)
#define UART_BAUD_38400(x)   ((((x*1000000)/38400) - 1)& UART_TX_BAUD_MASK) | (((((x*1000000)/38400) - 1) << 16)& UART_RX_BAUD_MASK)

#define EDAC_PARITY_MASK        0x000000FF

#define MAVN_START_ADDR_MASK    0xFFFFFE00
#define MAVN_PS_CODE_MASK       0x0000001F

/* lr33000.h - defines for LSI Logic LR33000 */

/* Define counter/timer register addresses */
#define M_TIC1  0xfffe0000  /* timer 1 initial count */
#define M_TC1   0xfffe0004  /* timer 1 control   */
#define M_TIC2  0xfffe0008  /* timer 2 initial count */
#define M_TC2   0xfffe000c  /* timer 2 control   */
#define M_RTIC  0xfffe0010  /* refresh timer     */

#ifdef LANGUAGE_C
#define TIC1   (*((volatile unsigned long *)M_TIC1)) /* timer1 count */
#define TC1    (*((volatile unsigned long *)M_TC1))  /* timer1 cntrl */
#define TIC2   (*((volatile unsigned long *)M_TIC2)) /* timer2 count */
#define TC2    (*((volatile unsigned long *)M_TC2))  /* timer2 cntrl */
#define RTIC   (*((volatile unsigned long *)M_RTIC)) /* refrsh timer */
#endif

/* Definitions for counter/timer control register bits */
#define TC_CE    0x00000004 /* count enable */
#define TC_IE    0x00000002 /* interrupt enable */
#define TC_INT   0x00000001 /* interrupt request */

/* lr33000.h */

#define _LR33300_

#define M_SRAM      0xfffe0100  /* SRAM config reg */
#define M_SPEC0     0xfffe0104
#define M_SPEC1     0xfffe0108
#define M_SPEC2     0xfffe010c
#define M_SPEC3     0xfffe0110
#define M_DRAM      0xfffe0120  /* DRAM config reg */

#ifdef LANGUAGE_C
#define SRAM        (*((volatile unsigned long *)M_SRAM))
#define SPEC0       (*((volatile unsigned long *)M_SPEC0))
#define SPEC1       (*((volatile unsigned long *)M_SPEC1))
#define SPEC2       (*((volatile unsigned long *)M_SPEC2))
#define SPEC3       (*((volatile unsigned long *)M_SPEC3))
#define DRAM        (*((volatile unsigned long *)M_DRAM))
#endif

    /* wait-state config registers */
#define SPC_INHIBITMASK (0xf<<24)
#define SPC_INHIBITSHFT 24
#define SPC_EXTGNT  (1<<23)
#define SPC_16WIDE  (1<<22)
#define SPC_8WIDE   (1<<21)
#define SPC_PENA    (1<<20)
#define SPC_CACHED  (1<<19)
#define SPC_CSDLYMASK   (3<<17)
#define SPC_CSDLYSHFT   17
#define SPC_BLKENA  (1<<16)
#define SPC_BLKWAITMASK (7<<13)
#define SPC_BLKWAITSHFT 13
#define SPC_RECMASK (63<<7)
#define SPC_RECSHFT 7
#define SPC_WAITENA (1<<6)
#define SPC_WAITMASK    (63<<0)
#define SPC_WAITSHFT    0

    /* DCR */
#define DRAM_DLP1   (1<<28)
#define DRAM_SYNC   (1<<27)
#define DRAM_SCFG   (1<<26)
#define DRAM_DMARDY (1<<25)
#define DRAM_DMABLKMASK (7<<22)
#define DRAM_DMABLKSHFT 22
#define DRAM_DPTHMASK   (3<<20)
#define DRAM_DPTHSHFT   20
#define DRAM_RDYW   (1<<19)
#define DRAM_PGSZMASK   (7<<16)
#define DRAM_PGSZSHFT   16
#define DRAM_PGMW   (1<<15)
#define DRAM_RFWEMASK   (3<<13)
#define DRAM_RFWESHFT   13
#define DRAM_RFEN   (1<<12)
#define DRAM_RDYEN  (1<<11)
#define DRAM_BFD    (1<<10)
#define DRAM_PE     (1<<9)
#define DRAM_RPCMASK    (3<<7)
#define DRAM_RPCSHFT    7
#define DRAM_RCDMASK    (3<<5)
#define DRAM_RCDSHFT    5
#define DRAM_CS     (1<<4)
#define DRAM_CLMASK (7<<1)
#define DRAM_CLSHFT 1
#define DRAM_DCE    (1<<0)

/* _LR33300_ */

#define _ERNIE_CORE_

#define M_BIU       0xfffe0130

#ifdef LANGUAGE_C
#define BIU     (*((volatile unsigned long *)M_BIU))

#define C0_TAR      6       /* target address register */
#define C0_BDAM     9       /* breakpoint data addr mask */
#define C0_BPCM     11      /* breakpoint instr addr mask */
#else
#define C0_TAR      $6      /* target address register */
#define C0_BDAM     $9      /* breakpoint data addr mask */
#define C0_BPCM     $11     /* breakpoint instr addr mask */
#endif

    /* cause register */
#define CAUSE_BT    (1<<30)     /* branch taken */

    /* BIU */
#define BIU_NOSTR   (1<<17)
#define BIU_LDSCH   (1<<16)
#define BIU_BGNT    (1<<15)
#define BIU_NOPAD   (1<<14)
#define BIU_RDPRI   (1<<13)
#define BIU_INTP    (1<<12)
#define BIU_IS1     (1<<11)
#define BIU_IS0     (1<<10)
#define BIU_IBLKSZMASK  (3<<8)
#define BIU_IBLKSZSHFT  8
#define BIU_IBLKSZ2 (0<<BIU_IBLKSZSHFT)
#define BIU_IBLKSZ4 (1<<BIU_IBLKSZSHFT)
#define BIU_IBLKSZ8 (2<<BIU_IBLKSZSHFT)
#define BIU_IBLKSZ16    (3<<BIU_IBLKSZSHFT)
#define BIU_DS      (1<<7)
#define BIU_DS1     (1<<7)
#define BIU_DS0     (1<<6)
#define BIU_DBLKSZMASK  (3<<4)
#define BIU_DBLKSZSHFT  4
#define BIU_DBLKSZ2 (0<<BIU_DBLKSZSHFT)
#define BIU_DBLKSZ4 (1<<BIU_DBLKSZSHFT)
#define BIU_DBLKSZ8 (2<<BIU_DBLKSZSHFT)
#define BIU_DBLKSZ16    (3<<BIU_DBLKSZSHFT)
#define BIU_RAM     (1<<3)
#define BIU_TAG     (1<<2)
#define BIU_INV     (1<<1)
#define BIU_LOCK    (1<<0)

/* _ERNIE_CORE_ */

/* Definitions for cache sizes */

#define LR33300_IC_SIZE 0x1000      /* 33300 Inst cache = 4Kbytes */
#define LR33300_DC_SIZE 0x800       /* 33300 Data cache = 2Kbytes */
