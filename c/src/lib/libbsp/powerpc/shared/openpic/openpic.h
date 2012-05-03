/*
 *  openpic.h -- OpenPIC definitions
 *
 *  Copyright (C) 1997 Geert Uytterhoeven
 *
 *  This file is based on the following documentation:
 *
 *	The Open Programmable Interrupt Controller (PIC)
 *	Register Interface Specification Revision 1.2
 *
 *	Issue Date: October 1995
 *
 *	Issued jointly by Advanced Micro Devices and Cyrix Corporation
 *
 *	AMD is a registered trademark of Advanced Micro Devices, Inc.
 *	Copyright (C) 1995, Advanced Micro Devices, Inc. and Cyrix, Inc.
 *	All Rights Reserved.
 *
 *  To receive a copy of this documentation, send an email to openpic@amd.com.
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License.  See the file COPYING in the main directory of this archive
 *  for more details.
 *
 *  Modified to compile in RTEMS development environment
 *  by Eric Valette
 *
 *  Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_OPENPIC_H
#define _RTEMS_OPENPIC_H

    /*
     *  OpenPIC supports up to 2048 interrupt sources and up to 32 processors
     */
#define OPENPIC_MAX_SOURCES	2048
#define OPENPIC_MAX_PROCESSORS	32

#define OPENPIC_NUM_TIMERS	4
#define OPENPIC_NUM_IPI		4
#define OPENPIC_NUM_PRI		16
#define OPENPIC_NUM_VECTORS	256

    /*
     *  Vector numbers
     */

#define OPENPIC_VEC_SOURCE      0x10    /* and up */
#define OPENPIC_VEC_TIMER       0x40    /* and up */
#define OPENPIC_VEC_IPI         0x50    /* and up */
#define OPENPIC_VEC_SPURIOUS    99

    /*
     *  OpenPIC Registers are 32 bits and aligned on 128 bit boundaries
     */

typedef struct _OpenPIC_Reg {
    unsigned int Reg;					/* Little endian! */
    char Pad[0xc];
} OpenPIC_Reg;

    /*
     *  Per Processor Registers
     */

typedef struct _OpenPIC_Processor {
    /*
     *  Private Shadow Registers (for SLiC backwards compatibility)
     */
    unsigned int IPI0_Dispatch_Shadow;			/* Write Only */
    char Pad1[0x4];
    unsigned int IPI0_Vector_Priority_Shadow;		/* Read/Write */
    char Pad2[0x34];
    /*
     *  Interprocessor Interrupt Command Ports
     */
    OpenPIC_Reg _IPI_Dispatch[OPENPIC_NUM_IPI];	/* Write Only */
    /*
     *  Current Task Priority Register
     */
    OpenPIC_Reg _Current_Task_Priority;		/* Read/Write */
    char Pad3[0x10];
    /*
     *  Interrupt Acknowledge Register
     */
    OpenPIC_Reg _Interrupt_Acknowledge;		/* Read Only */
    /*
     *  End of Interrupt (EOI) Register
     */
    OpenPIC_Reg _EOI;				/* Read/Write */
    char Pad5[0xf40];
} OpenPIC_Processor;

    /*
     *  Timer Registers
     */

typedef struct _OpenPIC_Timer {
    OpenPIC_Reg _Current_Count;			/* Read Only */
    OpenPIC_Reg _Base_Count;			/* Read/Write */
    OpenPIC_Reg _Vector_Priority;		/* Read/Write */
    OpenPIC_Reg _Destination;			/* Read/Write */
} OpenPIC_Timer;

    /*
     *  Global Registers
     */

typedef struct _OpenPIC_Global {
    /*
     *  Feature Reporting Registers
     */
    OpenPIC_Reg _Feature_Reporting0;		/* Read Only */
    OpenPIC_Reg _Feature_Reporting1;		/* Future Expansion */
    /*
     *  Global Configuration Registers
     */
    OpenPIC_Reg _Global_Configuration0;		/* Read/Write */
    OpenPIC_Reg _Global_Configuration1;		/* Future Expansion */
    /*
     *  Vendor Specific Registers
     */
    OpenPIC_Reg _Vendor_Specific[4];
    /*
     *  Vendor Identification Register
     */
    OpenPIC_Reg _Vendor_Identification;		/* Read Only */
    /*
     *  Processor Initialization Register
     */
    OpenPIC_Reg _Processor_Initialization;	/* Read/Write */
    /*
     *  IPI Vector/Priority Registers
     */
    OpenPIC_Reg _IPI_Vector_Priority[OPENPIC_NUM_IPI];	/* Read/Write */
    /*
     *  Spurious Vector Register
     */
    OpenPIC_Reg _Spurious_Vector;		/* Read/Write */
    /*
     *  Global Timer Registers
     */
    OpenPIC_Reg _Timer_Frequency;		/* Read/Write */
    OpenPIC_Timer Timer[OPENPIC_NUM_TIMERS];
    char Pad1[0xee00];
} OpenPIC_Global;

    /*
     *  Interrupt Source Registers
     */

typedef struct _OpenPIC_Source {
    OpenPIC_Reg _Vector_Priority;		/* Read/Write */
    OpenPIC_Reg _Destination;			/* Read/Write */
} OpenPIC_Source;

    /*
     *  OpenPIC Register Map
     */

struct OpenPIC {
    char Pad1[0x1000];
    /*
     *  Global Registers
     */
    OpenPIC_Global Global;
    /*
     *  Interrupt Source Configuration Registers
     */
    OpenPIC_Source Source[OPENPIC_MAX_SOURCES];
    /*
     *  Per Processor Registers
     */
    OpenPIC_Processor Processor[OPENPIC_MAX_PROCESSORS];
};

extern volatile struct OpenPIC *OpenPIC;

    /*
     *  Current Task Priority Register
     */

#define OPENPIC_CURRENT_TASK_PRIORITY_MASK	0x0000000f

    /*
     *  Who Am I Register
     */

#define OPENPIC_WHO_AM_I_ID_MASK		0x0000001f

    /*
     *  Feature Reporting Register 0
     */

#define OPENPIC_FEATURE_LAST_SOURCE_MASK	0x07ff0000
#define OPENPIC_FEATURE_LAST_SOURCE_SHIFT	16
#define OPENPIC_FEATURE_LAST_PROCESSOR_MASK	0x00001f00
#define OPENPIC_FEATURE_LAST_PROCESSOR_SHIFT	8
#define OPENPIC_FEATURE_VERSION_MASK		0x000000ff

    /*
     *  Global Configuration Register 0
     */

#define OPENPIC_CONFIG_RESET			0x80000000
#define OPENPIC_CONFIG_8259_PASSTHROUGH_DISABLE	0x20000000
#define OPENPIC_CONFIG_BASE_MASK		0x000fffff

    /*
     *  Vendor Identification Register
     */

#define OPENPIC_VENDOR_ID_STEPPING_MASK		0x00ff0000
#define OPENPIC_VENDOR_ID_STEPPING_SHIFT	16
#define OPENPIC_VENDOR_ID_DEVICE_ID_MASK	0x0000ff00
#define OPENPIC_VENDOR_ID_DEVICE_ID_SHIFT	8
#define OPENPIC_VENDOR_ID_VENDOR_ID_MASK	0x000000ff

    /*
     *  Vector/Priority Registers
     */

#define OPENPIC_MASK				0x80000000
#define OPENPIC_ACTIVITY			0x40000000	/* Read Only */
#define OPENPIC_PRIORITY_MASK			0x000f0000
#define OPENPIC_PRIORITY_SHIFT			16
#define OPENPIC_VECTOR_MASK			0x000000ff

    /*
     *  Interrupt Source Registers
     */

#define OPENPIC_SENSE_POLARITY			0x00800000	/* Undoc'd */
#define OPENPIC_SENSE_LEVEL			0x00400000

    /*
     *  Timer Registers
     */

#define OPENPIC_COUNT_MASK			0x7fffffff
#define OPENPIC_TIMER_TOGGLE			0x80000000
#define OPENPIC_TIMER_COUNT_INHIBIT		0x80000000

    /*
     *  Aliases to make life simpler
     */

/* Per Processor Registers */
#define IPI_Dispatch(i)			_IPI_Dispatch[i].Reg
#define Current_Task_Priority		_Current_Task_Priority.Reg
#define Interrupt_Acknowledge		_Interrupt_Acknowledge.Reg
#define EOI				_EOI.Reg

/* Global Registers */
#define Feature_Reporting0		_Feature_Reporting0.Reg
#define Feature_Reporting1		_Feature_Reporting1.Reg
#define Global_Configuration0		_Global_Configuration0.Reg
#define Global_Configuration1		_Global_Configuration1.Reg
#define Vendor_Specific(i)		_Vendor_Specific[i].Reg
#define Vendor_Identification		_Vendor_Identification.Reg
#define Processor_Initialization	_Processor_Initialization.Reg
#define IPI_Vector_Priority(i)		_IPI_Vector_Priority[i].Reg
#define Spurious_Vector			_Spurious_Vector.Reg
#define Timer_Frequency			_Timer_Frequency.Reg

/* Timer Registers */
#define Current_Count			_Current_Count.Reg
#define Base_Count			_Base_Count.Reg
#define Vector_Priority			_Vector_Priority.Reg
#define Destination			_Destination.Reg

/* Interrupt Source Registers */
#define Vector_Priority			_Vector_Priority.Reg
#define Destination			_Destination.Reg

    /*
     *  Vendor and Device IDs
     */

#define OPENPIC_VENDOR_ID_APPLE		0x14
#define OPENPIC_DEVICE_ID_APPLE_HYDRA	0x46

    /*
     *  OpenPIC Operations
     */

/*
 * Handle EPIC differences. Unfortunately, I don't know of an easy
 * way to tell an EPIC from a normal PIC at run-time. Therefore,
 * the BSP must enable a few quirks if it knows that an EPIC is being
 * used:
 *  - If the BSP uses the serial interrupt mode / 'multiplexer' then
 *    EOI must be delayed by at least 16 SRAM_CLK cycles to avoid
 *    spurious interrupts.
 *    It is the BSP's responsibility to set up an appropriate delay
 *    (in timebase-clock cycles) at init time using
 *    'openpic_set_eoi_delay()'. This is ONLY necessary when using
 *    an EPIC in serial mode.
 *  - The EPIC sources start at an offset of 16 in the register
 *    map, i.e., on an EPIC you'd say Sources[ x + 16 ] where
 *    on a PIC you would say Sources[ x ].
 *    Again, the BSP can set an offset that is used by the
 *    calls dealing with 'Interrupt Sources'
 *      openpic_enable_irq()
 *      openpic_disable_irq()
 *      openpic_initirq()
 *      openpic_mapirq()
 *      openpic_set_sense()
 *      openpic_get_source_priority()
 *      openpic_set_source_priority()
 *    the desired source offset parameter is passed to openpic_init().
 *
 * The routine 'openpic_set_eoi_delay()' returns the previous/old
 * value of the delay parameter.
 */
extern unsigned openpic_set_eoi_delay(unsigned tb_cycles);


/* Global Operations */

/* num_sources: number of sources to use; if zero this value
 * is read from the device, if nonzero the value read from
 * the device is overridden.
 * 'polarities' and 'senses' are arrays defining the desired
 * polarities (active hi [nonzero]/lo [zero]) and
 * senses (level [nonzero]/edge [zero]).
 * Either of the two array pointers may be NULL resulting
 * in the driver choosing default values of: 'active low'
 * and 'level sensitive', respectively.
 * NOTE: if you do pass arrays then their size must either
 *       match the number of sources read from the device or
 *       that value must be overridden by specifying
 *       a non-zero 'num_sources' parameter.
 *
 * Nonzero 'epic_freq' activates the EOI delay if the EPIC is
 * configured in serial mode (driver assumes firmware performs initial
 * EPIC setup). The BSP must pass the clock frequency of the EPIC
 * serial interface here.
 */
extern void openpic_init(int main_pic, unsigned char *polarities, unsigned char *senses, int num_sources, int source_offset, unsigned long epic_freq);

extern void openpic_reset(void);
extern void openpic_enable_8259_pass_through(void);
extern void openpic_disable_8259_pass_through(void);
extern unsigned int openpic_irq(unsigned int cpu);
extern void openpic_eoi(unsigned int cpu);
extern unsigned int openpic_get_priority(unsigned int cpu);
extern void openpic_set_priority(unsigned int cpu, unsigned int pri);
extern unsigned int openpic_get_spurious(void);
extern void openpic_set_spurious(unsigned int vector);
extern void openpic_init_processor(unsigned int cpumask);

/* Interprocessor Interrupts */
extern void openpic_initipi(unsigned int ipi, unsigned int pri, unsigned int vector);
extern void openpic_cause_IPI(unsigned int cpu, unsigned int ipi, unsigned int cpumask);

/* Timer Interrupts */
extern void openpic_inittimer(unsigned int timer, unsigned int pri, unsigned int vector);
extern void openpic_settimer(unsigned int timer, unsigned int base_count, int irq_enable);
extern unsigned int openpic_gettimer(unsigned int timer);
extern void openpic_maptimer(unsigned int timer, unsigned int cpumask);

/* Interrupt Sources */
extern void openpic_enable_irq(unsigned int irq);
extern int  openpic_disable_irq(unsigned int irq);
extern void openpic_initirq(unsigned int irq, unsigned int pri, unsigned int vector, int polarity,
			    int is_level);
extern void openpic_mapirq(unsigned int irq, unsigned int cpumask);
extern void openpic_set_sense(unsigned int irq, int sense);
extern unsigned int openpic_get_source_priority(unsigned int irq);
extern void openpic_set_source_priority(unsigned int irq, unsigned int pri);

#endif /* RTEMS_OPENPIC_H */
