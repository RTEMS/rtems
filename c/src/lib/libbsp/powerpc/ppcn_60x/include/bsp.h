/*  bsp.h
 *
 *  This include file contains all board IO definitions.
 *
 *  COPYRIGHT (c) 1998 by Radstone Technology
 *
 *
 * THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 * AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 * You are hereby granted permission to use, copy, modify, and distribute
 * this file, provided that this notice, plus the above copyright notice
 * and disclaimer, appears in all copies. Radstone Technology will provide
 * no support for this code.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http:www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __BSP_h
#define __BSP_h

#include <bspopts.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  confdefs.h overrides for this BSP:
 *   - termios serial ports (defaults to 1)
 *   - Interrupt stack space is not minimum if defined.
 */

#define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 2
#define CONFIGURE_INTERRUPT_STACK_MEMORY  (32 * 1024)
  
/*  Define processor identification. */

#define MPC601	1
#define MPC603	3
#define MPC604	4
#define MPC603e 6
#define MPC603ev 7
#define MPC604e 9

#ifdef ASM
/* Definition of where to store registers in alignment handler */
#define ALIGN_REGS 0x0140

/*  BAT register definitions for the MPC603 and MPC604. */
/*  Define bit fields for upper MPC603/4 BAT registers. */

#define BEPI_FIELD_60X      0xFFFE0000
#define VALID_SUPERVISOR    0x2
#define VALID_PROBLEM       0x1
#define KEY_USER_60X        0x1
#define BL_128K        0x0
#define BL_256K        (0x1<2)
#define BL_512K        (0x3<2)
#define BL_1M          (0x7<2)
#define BL_2M          (0xF<2)
#define BL_4M          (0x1F<2)
#define BL_8M          (0x3F<2)
#define BL_16M         (0x7F<2)
#define BL_32M         (0xFF<2)
#define BL_64M         (0x1FF<2)
#define BL_128M        (0x3FF<2)
#define BL_256M        (0x7FF<2)


/*  Define bit fields for lower MPC603/4 BAT registers. */

#define BRPN_FIELD_60X      0xFFFE0000

/*  Common defines for BAT registers. */
/*  Depending on the processor, the following may be in the upper */
/*  and lower BAT register. */

#define WRITE_THRU      0x40
#define WRITE_BK        0x0
#define COHERE_EN       0x10
#define COHERE_DIS      0x0
#define CACHE_DIS	0x20
#define CACHE_EN	0x0
#define GUARDED_EN      0x8
#define GUARDED_DIS     0x0
#define PP_00           0x0
#define PP_01           0x1
#define PP_10           0x2
#define PP_11           0x3

/*  HID0 definitions for MPC603 and MPC604 */
#define HID0		0x3f0      /*  HID0 Special Purpose Register # */
/*  HID1 definitions for MPC603e and MPC604e */
#define HID1		0x3f1      /*  HID1 Special Purpose Register # */

#define	H0_603_ICFI	0x0800     /*  HID0 I-Cache Flash Invalidate */
#define	H0_603_DCI	0x0400     /*  HID0 D-Cache Flash Invalidate */

#define	H0_60X_ICE	0x8000     /*  HID0 I-Cache Enable */
#define	H0_60X_DCE	0x4000     /*  HID0 D-Cache Enable */

#define	H0_604_BHTE	0x0004     /*  HID0 Branch History Table enable */
#define	H0_604_SIED	0x0080     /*  HID0 Serial Instruction Execution */
#define	H0_604_ICIA	0x0800     /*  HID0 I-Cache Invalidate All */
#define	H0_604_DCIA	0x0400     /*  HID0 D-Cache Invalidate All */

#define BAT0U           528
#define BAT0L           529
#define BAT1U           530
#define BAT1L           531
#define BAT2U           532
#define BAT2L           533
#define BAT3U           534
#define BAT3L           535
#define SPRG0           272
#define SPRG1           273

/* MSR bit settings */
#define MSR_LE		0x0001
#define MSR_RI		0x0002
#define MSR_DR		0x0010
#define MSR_IR		0x0020
#define MSR_IP		0x0040
#define MSR_FE1		0x0100
#define MSR_BE		0x0200
#define MSR_SE		0x0400
#define MSR_FE0		0x0800
#define MSR_ME		0x1000
#define MSR_FP		0x2000
#define MSR_PR		0x4000
#define MSR_EE		0x8000
#define MSR_ILE		0x0001	/* Upper 16 bits */
#define MSR_POW		0x0004	/* Upper 16 bits */
#else
#include <rtems.h>
#include <console.h>
#include <clockdrv.h>
#include <iosupp.h>
#include <tod.h>
#include <nvram.h>

/*
 *  PPCn_60x Interupt Definations.
 */
#define PPCN_60X_8259_IRQ_BASE ( PPC_IRQ_LAST +  1 )

/*
 * 8259 IRQ definations.
 */
#define PPCN_60X_IRQ_SYS_TIMER	(PPCN_60X_8259_IRQ_BASE +  0)  
#define PPCN_60X_IRQ_KBD		(PPCN_60X_8259_IRQ_BASE +  1)  
#define PPCN_60X_IRQ_COM2		(PPCN_60X_8259_IRQ_BASE +  3)
#define PPCN_60X_IRQ_COM1		(PPCN_60X_8259_IRQ_BASE +  4)
#define PPCN_60X_IRQ_CIO		(PPCN_60X_8259_IRQ_BASE +  5)
#define PPCN_60X_IRQ_FDC		(PPCN_60X_8259_IRQ_BASE +  6)
#define PPCN_60X_IRQ_LPT		(PPCN_60X_8259_IRQ_BASE +  7)
#define PPCN_60X_IRQ_RTC		(PPCN_60X_8259_IRQ_BASE +  8)
#define PPCN_60X_IRQ_COM3_4		(PPCN_60X_8259_IRQ_BASE + 10)
#define PPCN_60X_IRQ_MSE		(PPCN_60X_8259_IRQ_BASE + 12)
#define PPCN_60X_IRQ_SCSI		(PPCN_60X_8259_IRQ_BASE + 13)

/*
 * PCI interrupts as read from line register map directly to
 * ISA interrupt lines 9, 11, 14 and 15.
 */
#define PPCN_60X_IRQ_PCI(n)		(PPCN_60X_8259_IRQ_BASE +  (n))

#define MAX_BOARD_IRQS          (PPCN_60X_8259_IRQ_BASE + 15)

#define ISA8259_M_CTRL 0x20
#define ISA8259_S_CTRL 0xa0
#define ISA8259_M_MASK 0x21
#define ISA8259_S_MASK 0xa1
#define ISA8259_M_ELCR 0x4d0
#define ISA8259_S_ELCR 0x4d1

#define ELCRS_INT15_LVL		0x80
#define ELCRS_INT14_LVL		0x40
#define ELCRS_INT12_LVL		0x10
#define ELCRS_INT11_LVL		0x08
#define ELCRS_INT10_LVL		0x04
#define ELCRS_INT9_LVL		0x02
#define ELCRS_INT8_LVL		0x01
#define ELCRM_INT7_LVL		0x80
#define ELCRM_INT5_LVL		0x20


#define NONSPECIFIC_EOI 0x20

extern void En_Ext_Interrupt(int level);
extern void Dis_Ext_Interrupt(int level);

#define IRQ_VECTOR_BASE	0xbffffff0

/*
 * i8042 addresses
 */
#define I8042_DATA	0x60
#define I8042_CS	0x64

/*
 * ns16550 addresses
 */
#define NS16550_PORT_A	0x3f8
#define NS16550_PORT_B	0x2f8

/*
 * z85c30 addresses
 */
#define Z85C30_CTRL_B	0x840
#define Z85C30_DATA_B	0x841
#define Z85C30_CTRL_A	0x842
#define Z85C30_DATA_A	0x843

/*
 *  Z85C30 Definations for the 422 interface.
 */
#define Z85C30_CLOCK     14745600

#define PCI_SYS_MEM_BASE	0x80000000
#define PCI_MEM_BASE		0xc0000000
#define PCI_IO_BASE		0x80000000

#define EIEIO asm volatile("eieio")

/*
 * As ports are all little endian we will perform swaps here on 16 and 32
 * bit transfers
 */
extern unsigned16 Swap16(unsigned16 usVal);
extern unsigned32 Swap32(unsigned32 ulVal);

#define outport_byte(port, val)			\
	EIEIO;					\
	*(volatile unsigned8 *)(PCI_IO_BASE+	\
				(unsigned long)(port))=(val)

#define outport_16(port, val) 			\
	EIEIO;					\
	*(volatile unsigned16 *)(PCI_IO_BASE+	\
				 (unsigned long)(port))=Swap16(val)

#define outport_32(port, val) 			\
	EIEIO;					\
	*(volatile unsigned32 *)(PCI_IO_BASE+	\
				 (unsigned long)(port))=Swap32(val)

#define inport_byte(port, val)				\
	EIEIO;						\
	(val)=*(volatile unsigned8 *)(PCI_IO_BASE+	\
				      (unsigned long)(port))

#define inport_16(port, val)					\
	EIEIO;							\
	(val)=Swap16(*(volatile unsigned16 *)(PCI_IO_BASE+	\
					      (unsigned long)(port)))

#define inport_32(port, val)					\
	EIEIO;							\
	(val)=Swap32(*(volatile unsigned32 *)(PCI_IO_BASE+	\
					      (unsigned long)(port)))

/*
 *  System Planar Board Registers
 */
typedef volatile struct _PLANARREGISTERS{
    unsigned8 Reserved0[0x803];     /* Offset 0x000 */
    unsigned8 SimmId;               /* Offset 0x803 */
    unsigned8 SimmPresent;          /* Offset 0x804 */
    unsigned8 Reserved1[3];
    unsigned8 HardfileLight;        /* Offset 0x808 */
    unsigned8 Reserved2[3];
    unsigned8 EquipmentPresent1;    /* Offset 0x80C */
    unsigned8 Reserved3;
    unsigned8 EquipmentPresent2;    /* Offset 0x80e */
    unsigned8 Reserved4;
    unsigned8 PasswordProtect1;     /* Offset 0x810 */
    unsigned8 Reserved5;
    unsigned8 PasswordProtect2;     /* Offset 0x812 */
    unsigned8 Reserved6;
    unsigned8 L2Flush;              /* Offset 0x814 */
    unsigned8 Reserved7[3];
    unsigned8 Keylock;              /* Offset 0x818 */
    unsigned8 Reserved8[0x3c];
    unsigned8 BoardRevision;		/* Offset 0x854 */
    unsigned8 Reserved9[0xf];
    unsigned8 BoardID;				/* Offset 0x864 */
    unsigned8 Reserved10;
    unsigned8 MotherboardMemoryType; /* Offset 0x866 */
    unsigned8 Reserved11;
    unsigned8 MezzanineMemoryType;  /* Offset 0x868 */
} PLANARREGISTERS, *PPLANARREGISTERS;

extern unsigned char ucSystemType;
extern unsigned char ucBoardRevMaj;
extern unsigned char ucBoardRevMin;
extern unsigned long ulMemorySize;
extern unsigned long ulCpuBusClock;

#define SYS_TYPE_PPC1	0
#define SYS_TYPE_PPC2	1
#define SYS_TYPE_PPC1a	2
#define SYS_TYPE_PPC2a	3
#define SYS_TYPE_PPC4	4

/*
 * PCI initialisation
 */
void InitializePCI(void);

/*
 * VME initiaisation
 */
void InitializeUniverse();

/*
 * RTC initialisation
 */
void InitializeRTC(void);

/*
 * NvRAM initialisation
 */
void InitializeNvRAM(void);

/*
 *  BSP_TIMER_AVG_OVERHEAD and BSP_TIMER_LEAST_VALID for the shared timer
 *  driver.
 */

#define BSP_TIMER_AVG_OVERHEAD   4  /* It typically takes xx clicks        */
                                    /*     to start/stop the timer.        */
#define BSP_TIMER_LEAST_VALID    1  /* Don't trust a value lower than this */

/*
 *  Convert decrement value to tenths of microsecnds (used by 
 *  shared timer driver).
 *
 *    + There are 4 bus cycles per click
 *    + We return value in 1/10 microsecond units.
 *   Modified following equation to integer equation to remove
 *   floating point math.
 *   (int) ((float)(_value) / ((66.67 * 0.1) / 4.0))
 */

#define BSP_Convert_decrementer( _value ) \
  (int) (((_value) * 4000) / (ulCpuBusClock/10000))

/*
 *  Define the time limits for RTEMS Test Suite test durations.
 *  Long test and short test duration limits are provided.  These
 *  values are in seconds and need to be converted to ticks for the
 *  application.
 *
 */

#define MAX_LONG_TEST_DURATION       300 /* 5 minutes = 300 seconds */
#define MAX_SHORT_TEST_DURATION      3   /* 3 seconds */

/*
 *  Stuff for Time Test 27
 */

#define MUST_WAIT_FOR_INTERRUPT 1

#define Install_tm27_vector( _handler ) \
  set_vector( (_handler), PPC_IRQ_DECREMENTER, 1 )

#define Cause_tm27_intr()  \
  do { \
    unsigned32 _clicks = 8; \
    asm volatile( "mtdec %0" : "=r" ((_clicks)) : "r" ((_clicks)) ); \
  } while (0)


#define Clear_tm27_intr() \
  do { \
    unsigned32 _clicks = 0xffffffff; \
    asm volatile( "mtdec %0" : "=r" ((_clicks)) : "r" ((_clicks)) ); \
  } while (0)

#define Lower_tm27_intr() \
  do { \
    unsigned32 _msr = 0; \
    _ISR_Set_level( 0 ); \
    asm volatile( "mfmsr %0 ;" : "=r" (_msr) : "r" (_msr) ); \
    _msr |=  0x8002; \
    asm volatile( "mtmsr %0 ;" : "=r" (_msr) : "r" (_msr) ); \
  } while (0)


/* Constants */

/*
 *  Device Driver Table Entries
 */
 
/*
 * NOTE: Use the standard Console driver entry
 */
 
/*
 * NOTE: Use the standard Clock driver entry
 */
 
/*
 * How many libio files we want
 */
 
#define BSP_LIBIO_MAX_FDS       20

/* functions */

void bsp_start( void );

void bsp_cleanup( void );

rtems_isr_entry set_vector(                    /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
);

/*
 * spurious.c
 */
rtems_isr bsp_stub_handler(
   rtems_vector_number trap
); 
rtems_isr bsp_spurious_handler(
   rtems_vector_number trap
);
void bsp_spurious_initialize();

/* 
 * genvec.c
 */
void set_EE_vector(
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector                    /* vector number      */
);
void initialize_external_exception_vector();

/*
 * console.c
 */
void DEBUG_puts( char *string );
void DEBUG_puth( unsigned32 ulHexNum );

void BSP_fatal_return( void );

extern rtems_configuration_table BSP_Configuration;     /* owned by BSP */

extern rtems_cpu_table           Cpu_table;             /* owned by BSP */

extern unsigned32          bsp_isr_level;

#endif /* ASM */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
