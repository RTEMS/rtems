/*  bsp.h
 *
 *  This include file contains all board IO definitions.
 *
 *  XXX : put yours in here
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __MVME2307_BSP_h
#define __MVME2307_BSP_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <console.h>
#include <clockdrv.h>

/*
 * Network driver configuration
 */

struct rtems_bsdnet_ifconfig;
extern int rtems_dec21140_driver_attach (struct rtems_bsdnet_ifconfig *config);

#define RTEMS_BSP_NETWORK_DRIVER_NAME "dc1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH       rtems_dec21140_driver_attach


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

#define Install_tm27_vector( handler ) \
    set_vector( (handler), PPC_IRQ_DECREMENTER, 1 )

#define Cause_tm27_intr()  \
  { \
    unsigned32 _clicks = 1; \
    asm volatile( "mtdec %0" : : "r" ((_clicks)) ); \
  }


#define Clear_tm27_intr() \
  { \
    unsigned32 _clicks = 0xffffffff; \
    asm volatile( "mtdec %0" : : "r" ((_clicks)) ); \
  }


#define Lower_tm27_intr() \
  { \
    unsigned32 _msr = 0; \
    _ISR_Set_level( 0 ); \
    asm volatile( "mfmsr %0 ;" : "=r" (_msr) :); \
    _msr |=  0x8002; \
    asm volatile( "mtmsr %0 ;" : : "r" (_msr) ); \
  }



/* miscellaneous stuff assumed to exist */

extern rtems_configuration_table BSP_Configuration;

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
 *  Falcon memory controller registers
 */

typedef struct {
    unsigned32 SystemId       :8;
#       define SYSID_MVME2300 0xFD

    unsigned32 SystemClock   :4;
#       define SYSCLK_50_MHZ 0xD
#       define SYSCLK_60_MHZ 0xE
#       define SYSCLK_67_MHZ 0xF

    unsigned32 ExternalCacheSize :4;
#       define NO_EXT_CACHE      0xF

    unsigned32 ProcessorInfo1       :4;
    unsigned32 ProcessorInfo2       :4;
#       define PROC_WITH_1M_CACHE   0x4
#       define PROC_WITH_512K_CACHE 0x5
#       define PROC_WITH_256K_CACHE 0x6
#       define PROC_WITH_NO_CACHE   0x7

    unsigned32 _ :8;

} FalconSystemConfReg_t;

#define Falcon_SYSCR (*(FalconSystemConfReg_t *) 0xFEF80400)
 
typedef struct {
    unsigned32 MemSize    :2;
#       define RAM_16_MB  0x0
#       define RAM_32_MB  0x1
#       define RAM_64_MB  0x2
#       define RAM_128_MB 0x3

    unsigned32 _                   :1;
    unsigned32 MemFastRefresh      :1;
#       define DRAM_NORMAL_REFRESH 0x0
#       define DRAM_FAST_REFRESH   0x1

    unsigned32 __             :2;
    unsigned32 MemSpeed       :2;
#       define DRAM_70_NSEC   0x0
#       define DRAM_60_NSEC   0x1
#       define DRAM_50_NSEC   0x3

    unsigned32 ___           :1;
    unsigned32 RomAType      :3;
    unsigned32 ____          :1;
    unsigned32 RomBType      :3;
#       define INTEL_FLASH   0x6;
#       define UNKNOWN_FLASH 0x7;

    unsigned32 L2Type               :4;
#       define LATE_WRITE_SYNC      0x0
#       define PIPELINED_SYNC_BURST 0x1

    unsigned32 L2FreqDivider  :4;
#       define PLL_DISABLED   0x0
#       define PLL_DIV_BY_1   0x1
#       define PLL_DIV_BY_1_5 0x2
#       define PLL_DIV_BY_2   0x3
#       define PLL_DIV_BY_2_5 0x4
#       define PLL_DIV_BY_3   0x5

    unsigned32 _____       :2;
    unsigned32 RomASize    :3;
#       define ROM_A_1_MB  0x0
#       define ROM_A_2_MB  0x1
#       define ROM_A_4_MB  0x2
#       define ROM_A_8_MB  0x3
#       define ROM_A_16_MB 0x4
#       define ROM_A_32_MB 0x5
#       define ROM_A_64_MB 0x6
#       define ROM_A_0_MB  0x7

    unsigned32 ______         :3;
} FalconMemConfReg_t;

#define Falcon_MEMCR (*(FalconMemConfReg_t *) 0xFEF80404)

extern int   end;        /* last address in the program */

/*
 *  PCI bus functions
 */

/* 
 *  address space conversions
 *
 *  address spaces:
 *  LOCAL - memory space of the PowerPC
 *  PCI   - memory space on the PCI bus
 *  IO    - I/O space on the PCI bus
 *  VME   - memory space on the VME bus
 */
#define IO_TO_LOCAL(address)  ((unsigned int)(address) + 0x80000000)
#define PCI_TO_LOCAL(address) ((unsigned int)(address) + 0xC0000000)
#define LOCAL_TO_PCI(address) ((unsigned int)(address) + 0x80000000)
#define INVERSE_LOCAL_TO_PCI(address) ((unsigned int)(address) - 0x80000000)

#define PCI_NOT_FOUND (-1)

#define RAVEN_VENDOR_ID    0x1057
#define RAVEN_DEVICE_ID    0x4801
#define UNIVERSE_VENDOR_ID 0x10E3
#define UNIVERSE_DEVICE_ID 0x0000
#define DEC21140_VENDOR_ID 0x1011
#define DEC21140_DEVICE_ID 0x0009

#define PCI_BASE_ADDRESS_0  0x10
#define PCI_BASE_ADDRESS_1 0x14
#define PCI_INTERRUPT_LINE 60

int  pci_make_devsig(int bus, int device, int function);    /* returns sig */
int  pci_find_by_devid(int vendorId, int devId, int idx);   /* returns sig */
int  pci_conf_read8(int sig, int off);
int  pci_conf_read16(int sig, int off);
int  pci_conf_read32(int sig, int off);
void pci_conf_write8(int sig, int off, unsigned int data);
void pci_conf_write16(int sig, int off, unsigned int data);
void pci_conf_write32(int sig, int off, unsigned int data);

static inline void synchronize_io(void) {
    asm volatile ("eieio" : :);
}  

/*
 *  functions for accessing little-endian data
 */
static inline void st_le32(volatile int *addr, int data) {
    asm volatile ("stwbrx %0,0,%1" : : "r" (data), "r" (addr));
}

static inline void st_le16(volatile short *addr, short data) {
    asm volatile ("sthbrx %0,0,%1" : : "r" (data), "r" (addr));
}

static inline int ld_le32(volatile int *addr) {
    int data;
    asm volatile ("lwbrx %0,0,%1" : "=r" (data) : "r" (addr));
    return data;
}

static inline int ld_le16(volatile short *addr) {
    int data;
    asm volatile ("lhbrx %0,0,%1" : "=r" (data) : "r" (addr));
    return data;
}                                                    

/*
 *  interrupt and timer handling
 */
    /* ISA interrupts - vectors 0-15 */
#define IRQ_ISA_TIMER    0
#define IRQ_UART         4
#define IRQ_ISA_LM_SIG   5
#define IRQ_ABORT_SWITCH 8
#define IRQ_ISA_ETHERNET 10
#define IRQ_ISA_UNIVERSE 11
#define IRQ_ISA_PMC_PCIX 15
    /* Raven interrupts  */
#define IRQ_PIC_CASCADE  16
#define IRQ_FALCON_ECC   17
#define IRQ_ETHERNET     18
#define IRQ_VME_LINT0    19
#define IRQ_VME_LINT1    20
#define IRQ_VME_LINT2    21
#define IRQ_VME_LINT3    22
#define IRQ_PMC1A_PMC2B  23
#define IRQ_PMC1B_PMC2C  24
#define IRQ_PMC1C_PMC2D  25
#define IRQ_PMC1D_PMC2A  26
#define IRQ_LM_SIG_0     27
#define IRQ_LM_SIG_1     28
    /* Raven timers */
#define IRQ_TIMER_0      29
#define IRQ_TIMER_1      30
#define IRQ_TIMER_2      31
#define IRQ_TIMER_3      32
    /* Raven interprocessor interrupts */
#define IRQ_IPI_0        33
#define IRQ_IPI_1        34
#define IRQ_IPI_2        35
#define IRQ_IPI_3        36
    /* other */
#define IRQ_SPURIOUS     37
#define IRQ_RAVEN_ERROR  38

    /* Raven priority that ISA interrupts use */
#define PRIORITY_ISA_INT 8

rtems_status_code interrupt_controller_init(void);
rtems_status_code bsp_interrupt_catch(rtems_isr_entry      new_isr_handler,
                                      rtems_vector_number  vector,
                                      rtems_isr_entry     *old_isr_handler);
rtems_status_code bsp_interrupt_enable(rtems_vector_number vector,
                                       int priority);
rtems_status_code bsp_interrupt_disable(rtems_vector_number vector);
rtems_status_code bsp_start_timer(int timer, int period_usec);
rtems_status_code bsp_stop_timer(int timer);
rtems_status_code bsp_read_timer(int timer, int *value);
rtems_status_code generate_interprocessor_interrupt(int interrupt);
rtems_status_code set_interrupt_task_priority(int priority);


/* functions */

void bsp_cleanup( void );

rtems_isr_entry set_vector(                    /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
);

int read_nvram(char *buff, int offset, int len);

void printk( char *fmt, ... );

void BSP_fatal_return( void );

void bsp_spurious_initialize( void );

extern rtems_configuration_table BSP_Configuration;     /* owned by BSP */

extern rtems_cpu_table           Cpu_table;             /* owned by BSP */

extern rtems_unsigned32          bsp_isr_level;


#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
