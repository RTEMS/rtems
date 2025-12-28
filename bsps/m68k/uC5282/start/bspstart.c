/* SPDX-License-Identifier: GPL-2.0+-with-RTEMS-exception */

/*
 *  This routine does the bulk of the system initialisation.
 */

/*
 * Copyright (c) 2005 Eric Norum <eric@norum.ca>
 *
 * COPYRIGHT (c) 2005.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/irq-generic.h>
#include <rtems/error.h>
#include <errno.h>
#include <stdio.h>
#include <inttypes.h>
#include <mcf5282/mcf5282.h>

/*
 * Location of 'VME' access
 */
#define VME_ONE_BASE    0x30000000
#define VME_TWO_BASE    0x31000000

/*
 * Offset of first trap ISR. 16 of these total.
 */
#define UC5282_FIRST_TRAP_ISR 32
#define UC5282_TRAP_ISR(_n) (UC5282_FIRST_TRAP_ISR+((_n) & 0xF))

/*
 * Linker Script Defined Variables
 */
extern char RamSize[];
extern char RamBase[];
extern char _CPUClockSpeed[];
extern char _PLLRefClockSpeed[];

uint32_t BSP_sys_clk_speed = (uint32_t)_CPUClockSpeed;
uint32_t BSP_pll_ref_clock = (uint32_t)_PLLRefClockSpeed;

typedef void(*uC5282_ISR)(int);

/*
 * CPU-space access
 * The NOP after writing the CACR is there to address the following issue as
 * described in "Device Errata MCF5282DE", Rev. 1.7, 09/2004:
 *
 * 6 Possible Cache Corruption after Setting  CACR[CINV]
 * 6.1 Description
 * The cache on the MCF5282 was enhanced to function as a unified data and
 * instruction cache, an instruction cache, or an operand cache.  The cache
 * function and organization is controlled by the cache control
 * register (CACR).
 * The CINV (Bit 24 = cache invalidate) bit in the CACR causes a cache clear.
 * If the cache is configured as a unified cache and the CINV bit is set, the
 * scope of the cache clear is controlled by two other bits in the CACR,
 * INVI (BIT 21 = CINV instruction cache only) and INVD (BIT 20 = CINV data
 * cache only).  These bits allow the entire cache, just the instruction
 * portion of the cache, or just the data portion of the cache to be cleared.
 * If a write to the CACR is performed to clear the cache (CINV = BIT 24 set)
 * and only a partial clear will be done (INVI = BIT 21 or INVD = BIT 20 set),
 * then cache corruption may  occur.
 *
 * 6.2 Workaround
 * All loads of the CACR that perform a cache clear operation (CINV = BIT 24)
 * should be followed immediately by a NOP instruction.  This avoids the cache
 * corruption problem.
 * DATECODES AFFECTED: All
 *
 *
 * Buffered writes must be disabled as described in "MCF5282 Chip Errata",
 * MCF5282DE, Rev. 6, 5/2009:
 *   SECF124: Buffered Write May Be Executed Twice
 *   Errata type: Silicon
 *   Affected component: Cache
 *   Description: If buffered writes are enabled using the CACR or ACR
 *                registers, the imprecise write transaction generated
 *                by a buffered write may be executed twice.
 *   Workaround: Do not enable buffered writes in the CACR or ACR registers:
 *               CACR[8] = DBWE (default buffered write enable) must be 0
 *               ACRn[5] = BUFW (buffered write enable) must be 0
 *   Fix plan: Currently, there are no plans to fix this.
 */
#define m68k_set_cacr_nop(_cacr) \
  __asm__ volatile ("movec %0,%%cacr\n\tnop" : : "d" (_cacr))
#define m68k_set_cacr(_cacr) \
  __asm__ volatile ("movec %0,%%cacr" : : "d" (_cacr))
#define m68k_set_acr0(_acr0) \
  __asm__ volatile ("movec %0,%%acr0" : : "d" (_acr0))
#define m68k_set_acr1(_acr1) \
  __asm__ volatile ("movec %0,%%acr1" : : "d" (_acr1))

uint32_t mcf5282_acr0_mode = 0;
uint32_t mcf5282_acr1_mode = 0;

extern void bsp_fake_syscall(int);

void bsp_default_isr_handler(int pc);

/*
 * The Arcturus boot ROM prints exception information improperly
 * so use this default exception handler instead.  This one also
 * prints a call backtrace.
 */
void bsp_default_isr_handler(int param)
{
    /**
     * Mapped onto the stack. 'param' is pc here,
     * info is located at -4, fp is at -8.
     */
    typedef struct {
        uint32_t fp;
        uint32_t info;
        uint32_t pc;
    } stack_frame_t;

    int level;
    static volatile int reent;

    rtems_interrupt_disable(level);
    if (reent++) bsp_sysReset(0);
    {
        uintptr_t tmp = (uintptr_t)&param;
        tmp -= 2 * sizeof(uint32_t);
        stack_frame_t *sf = (stack_frame_t *) tmp;

        uint32_t info = sf->info;
        uint32_t pc   = sf->pc;
        uint32_t format = (info >> 28) & 0xF;
        uint32_t faultStatus = ((info >> 24) & 0xC) | ((info >> 16) & 0x3);
        uint32_t vector = (info >> 18) & 0xFF;
        uint32_t statusRegister = info & 0xFFFF;
        uint32_t *fp;

        printk("\n\nPC:%x  SR:%x  VEC:%x  FORMAT:%x  STATUS:%x\n", pc,
                                                               statusRegister,
                                                               vector,
                                                               format,
                                                               faultStatus);
        fp = &sf->fp;
        for(;;) {
            uint32_t *nfp = (uint32_t *)*fp;
            if ((nfp <= fp)
            || ((char *)nfp >= RamSize)
            || ((char *)(nfp[1]) >= RamSize))
               break;
            printk("FP:%p -> %p    PC:%x\n", fp, nfp, nfp[1]);
            fp = nfp;
        }
    }
    rtems_task_suspend(0);
    rtems_panic("done");
}

void bsp_start( void )
{
  int   i;
  const char *clk_speed_str;
  uint32_t clk_speed, mfd, rfd;
  uint8_t  byte;

  /*
   * Make sure UART TX is running - necessary for
   * early printk to work. The firmware monitor
   * usually enables this anyways but qemu doesn't!
   */
  MCF5282_UART_UCR(CONSOLE_PORT) = MCF5282_UART_UCR_TX_ENABLED;

  /*
   * Set up default exception handler
   */
  for (i = 2 ; i < 256 ; i++)
      if (i != UC5282_TRAP_ISR(2)) /* Catch all but bootrom system calls */
          *((uC5282_ISR*)(_VBR + i * 4)) = bsp_default_isr_handler;

  /*
   * Invalidate the cache and disable it
   */
  m68k_set_acr0(mcf5282_acr0_mode);
  m68k_set_acr1(mcf5282_acr1_mode);
  m68k_set_cacr_nop(MCF5XXX_CACR_CINV);

  /*
   * Cache SDRAM
   * Enable buffered writes
   * As Device Errata SECF124 notes this may cause double writes,
   * but that's not really a big problem and benchmarking tests have
   * shown that buffered writes do gain some performance.
   */
  mcf5282_acr0_mode = MCF5XXX_ACR_AB((uint32_t)RamBase)     |
                      MCF5XXX_ACR_AM((uint32_t)RamSize-1)   |
                      MCF5XXX_ACR_EN                        |
                      MCF5XXX_ACR_SM_IGNORE                 |
                      MCF5XXX_ACR_BWE;
  m68k_set_acr0(mcf5282_acr0_mode);

  /*
   * Qemu has no trap handler; install our fake syscall
   * implementation if there is no existing handler.
   */
  uC5282_ISR* trapIsr = (uC5282_ISR*)(_VBR + UC5282_TRAP_ISR(2) * 4);
  if ( *trapIsr == 0 )
    *trapIsr = bsp_fake_syscall;

  /*
   * Read/write copy of cache registers
   *   Split instruction/data or instruction-only
   *   Allow CPUSHL to invalidate a cache line
   *   Disable buffered writes
   *   No burst transfers on non-cacheable accesses
   *   Default cache mode is *disabled* (cache only ACRx areas)
   */
  mcf5xxx_initialize_cacr(
     MCF5XXX_CACR_CENB |
     #ifndef RTEMS_MCF5282_BSP_ENABLE_DATA_CACHE
       MCF5XXX_CACR_DISD |
     #endif
     MCF5XXX_CACR_DCM
  );

  /*
   * Set up CS* space (fake 'VME')
   *   Two A24/D16 spaces, supervisor data acces
   */
  MCF5282_CS1_CSAR = MCF5282_CS_CSAR_BA(VME_ONE_BASE);
  MCF5282_CS1_CSMR = MCF5282_CS_CSMR_BAM_16M |
                     MCF5282_CS_CSMR_CI |
                     MCF5282_CS_CSMR_SC |
                     MCF5282_CS_CSMR_UC |
                     MCF5282_CS_CSMR_UD |
                     MCF5282_CS_CSMR_V;
  MCF5282_CS1_CSCR = MCF5282_CS_CSCR_PS_16;
  MCF5282_CS2_CSAR = MCF5282_CS_CSAR_BA(VME_TWO_BASE);
  MCF5282_CS2_CSMR = MCF5282_CS_CSMR_BAM_16M |
                     MCF5282_CS_CSMR_CI |
                     MCF5282_CS_CSMR_SC |
                     MCF5282_CS_CSMR_UC |
                     MCF5282_CS_CSMR_UD |
                     MCF5282_CS_CSMR_V;
  MCF5282_CS2_CSCR = MCF5282_CS_CSCR_PS_16;
  MCF5282_GPIO_PJPAR |= 0x06;

  /*
   * Hopefully, the UART clock is still correctly set up
   * so they can see the printk() output...
   */
  clk_speed = 0;
  printk("Trying to figure out the system clock\n");
  printk("Checking ENV variable SYS_CLOCK_SPEED:\n");
  if ( (clk_speed_str = bsp_getbenv("SYS_CLOCK_SPEED")) ) {
    printk("Found: %s\n", clk_speed_str);
    for ( clk_speed = 0, i=0;
          clk_speed_str[i] >= '0' && clk_speed_str[i] <= '9';
          i++ ) {
      clk_speed = 10*clk_speed + clk_speed_str[i] - '0';
    }
    if ( 0 != clk_speed_str[i] ) {
      printk("Not a decimal number; I'm not using this setting\n");
      clk_speed = 0;
    }
  } else {
    printk("Not set.\n");
  }

  if ( 0 == clk_speed ) {
    clk_speed = BSP_sys_clk_speed;
  }

  if ( 0 == clk_speed ) {
    printk("Using some heuristics to determine clock speed...\n");
    byte = MCF5282_CLOCK_SYNSR;
    if ( 0 == byte ) {
      printk("SYNSR == 0; assuming QEMU at 66MHz\n");
      BSP_pll_ref_clock = 8250000;
      mfd = ( 0 << 8 ) | ( 2 << 12 );
    } else {
      if ( 0xf8 != byte ) {
        printk(
          "FATAL ERROR: Unexpected SYNSR contents "
          "(0x%02x), can't proceed\n", byte
        );
        bsp_sysReset(0);
      }
      mfd = MCF5282_CLOCK_SYNCR;
    }
    printk("Assuming %" PRIu32 "Hz PLL ref. clock\n", BSP_pll_ref_clock);
    rfd = (mfd >>  8) & 7;
    mfd = (mfd >> 12) & 7;
    /* Check against 'known' cases */
    if ( 0 != rfd || (2 != mfd && 3 != mfd) ) {
      printk("WARNING: Pll divisor/multiplier has unknown value; \n");
      printk("         either your board is not 64MHz or 80Mhz or\n");
      printk("         it uses a PLL reference other than 8MHz.\n");
      printk("         I'll proceed anyways but you might have to\n");
      printk("         reset the board and set uCbootloader ENV\n");
      printk("         variable \"SYS_CLOCK_SPEED\".\n");
    }
    mfd = 2 * (mfd + 2);
    /* sysclk = pll_ref * 2 * (MFD + 2) / 2^(rfd) */
    printk(
      "PLL multiplier: %" PRIu32", output divisor: %" PRIu32 "\n",
      mfd,
      rfd
    );
    clk_speed = (BSP_pll_ref_clock * mfd) >> rfd;
  }

  if ( 0 == clk_speed ) {
    printk("FATAL ERROR: Unable to determine system clock speed\n");
    bsp_sysReset(0);
  } else {
    BSP_sys_clk_speed = clk_speed;
    printk(
      "System clock speed: %" PRIu32 "Hz\n", bsp_get_CPU_clock_speed()
    );
  }

  bsp_interrupt_initialize();
}

uint32_t bsp_get_CPU_clock_speed(void)
{
  return( BSP_sys_clk_speed );
}

/*
 * Arcturus bootloader system calls
 */
#define syscall_return(type, ret)                      \
do {                                                   \
   if ((unsigned long)(ret) >= (unsigned long)(-64)) { \
      errno = -(ret);                                  \
      ret = -1;                                        \
   }                                                   \
   return (type)(ret);                                 \
} while (0)

#define syscall_1(type,name,d1type,d1)                      \
type bsp_##name(d1type d1);                                 \
type bsp_##name(d1type d1)                                  \
{                                                           \
   long ret;                                                \
   register long __d1 __asm__ ("%d1") = (long)d1;           \
   __asm__ __volatile__ ("move.l %1,%%d0\n\t"               \
                         "trap #2\n\t"                      \
                         "move.l %%d0,%0"                   \
                         : "=g" (ret)                       \
                         : "i" (SysCode_##name), "d" (__d1) \
                         : "d0" );                          \
   syscall_return(type,ret);                                \
}

#define syscall_2(type,name,d1type,d1,d2type,d2)            \
type bsp_##name(d1type d1, d2type d2);                      \
type bsp_##name(d1type d1, d2type d2)                       \
{                                                           \
   long ret;                                                \
   register long __d1 __asm__ ("%d1") = (long)d1;           \
   register long __d2 __asm__ ("%d2") = (long)d2;           \
   __asm__ __volatile__ ("move.l %1,%%d0\n\t"               \
                         "trap #2\n\t"                      \
                         "move.l %%d0,%0"                   \
                         : "=g" (ret)                       \
                         : "i" (SysCode_##name), "d" (__d1),\
                                                 "d" (__d2) \
                         : "d0" );                          \
   syscall_return(type,ret);                                \
}

#define syscall_3(type,name,d1type,d1,d2type,d2,d3type,d3)  \
type bsp_##name(d1type d1, d2type d2, d3type d3);           \
type bsp_##name(d1type d1, d2type d2, d3type d3)            \
{                                                           \
   long ret;                                                \
   register long __d1 __asm__ ("%d1") = (long)d1;           \
   register long __d2 __asm__ ("%d2") = (long)d2;           \
   register long __d3 __asm__ ("%d3") = (long)d3;           \
   __asm__ __volatile__ ("move.l %1,%%d0\n\t"               \
                         "trap #2\n\t"                      \
                         "move.l %%d0,%0"                   \
                         : "=g" (ret)                       \
                         : "i" (SysCode_##name), "d" (__d1),\
                                                 "d" (__d2),\
                                                 "d" (__d3) \
                         : "d0" );                          \
   syscall_return(type,ret);                                \
}

#define SysCode_sysReset           0 /* system reset */
#define SysCode_program            5 /* program flash memory */
#define SysCode_gethwaddr         12 /* get hardware address */
#define SysCode_getbenv           14 /* get bootloader environment variable */
#define SysCode_setbenv           15 /* set bootloader environment variable */
#define SysCode_flash_erase_range 19 /* erase a section of flash */
#define SysCode_flash_write_range 20 /* write a section of flash */
syscall_1(int, sysReset, int, flags)
syscall_1(unsigned const char *, gethwaddr, int, a)
syscall_1(const char *, getbenv, const char *, a)
syscall_1(int, setbenv, const char *, a)
syscall_2(int, program, bsp_mnode_t *, chain, int, flags)
syscall_3(int, flash_erase_range, volatile unsigned short *,
          flashptr, int, start, int, end);
syscall_3(int, flash_write_range, volatile unsigned short *,
          flashptr, bsp_mnode_t *, chain, int, offset);

/* Provide a dummy-implementation of these syscalls
 * for qemu (which lacks the firmware).
 */

#define __STR(x)    #x
#define __STRSTR(x) __STR(x)
#define ERRVAL      __STRSTR(EACCES)

/* reset-control register */
#define RCR "__IPSBAR + 0x110000"

__asm__ (
    "bsp_fake_syscall:         \n"
    "   cmpl  #0,  %d0         \n" /* sysreset    */
    "   bne   1f               \n"
    "   moveb #0x80, %d0       \n"
    "   moveb %d0, "RCR"       \n" /* reset-controller */
        /* should never get here - but we'd return -EACCESS if we do */
    "1:                        \n"
    "   cmpl  #12, %d0         \n" /* gethwaddr   */
    "   beq   2f               \n"
    "   cmpl  #14, %d0         \n" /* getbenv     */
    "   beq   2f               \n"
    "   movel #-"ERRVAL", %d0  \n" /* return -EACCESS */
    "   rte                    \n"
    "2:                        \n"
    "   movel #0,  %d0         \n" /* return NULL */
    "   rte                    \n"
);


/*
 * 'Extended BSP' routines
 * Should move to cpukit/score/cpu/m68k/cpu.c someday.
 */

rtems_status_code bspExtInit(void) { return RTEMS_SUCCESSFUL; }

int BSP_enableVME_int_lvl(unsigned int level)
{
  (void) level;

  return 0;
}

int BSP_disableVME_int_lvl(unsigned int level)
{
  (void) level;

  return 0;
}

int
BSP_vme2local_adrs(
  unsigned am, unsigned long vmeaddr, unsigned long *plocaladdr)
{
  unsigned long offset;

  switch (am) {
    default:    return -1;
    case VME_AM_SUP_SHORT_IO: offset = 0x31FF0000; break; /* A16/D16 */
    case VME_AM_STD_SUP_DATA: offset = 0x30000000; break; /* A24/D16 */
    case VME_AM_EXT_SUP_DATA: offset = 0x31000000; break; /* A32/D32 */
  }
  *plocaladdr = vmeaddr + offset;
  return 0;
}

void
bsp_reset_cause(char *buf, size_t capacity)
{
  int bit, rsr;
  size_t i;
  const char *cp;

  if (buf == NULL)
    return;
  if (capacity)
    buf[0] = '\0';
  rsr = MCF5282_RESET_RSR;
  for (i = 0, bit = 0x80 ; bit != 0 ; bit >>= 1) {
    if (rsr & bit) {
      switch (bit) {
        case MCF5282_RESET_RSR_LVD:  cp = "Low voltage";        break;
        case MCF5282_RESET_RSR_SOFT: cp = "Software reset";     break;
        case MCF5282_RESET_RSR_WDR:  cp = "Watchdog reset";     break;
        case MCF5282_RESET_RSR_POR:  cp = "Power-on reset";     break;
        case MCF5282_RESET_RSR_EXT:  cp = "External reset";     break;
        case MCF5282_RESET_RSR_LOC:  cp = "Loss of clock";      break;
        case MCF5282_RESET_RSR_LOL:  cp = "Loss of lock";       break;
        default:                     cp = "??";                 break;
      }
      i += snprintf(buf+i, capacity-i, cp);
      if (i >= capacity)
        break;
      rsr &= ~bit;
      if (rsr == 0)
        break;
      i += snprintf(buf+i, capacity-i, ", ");
      if (i >= capacity)
        break;
    }
  }
}
