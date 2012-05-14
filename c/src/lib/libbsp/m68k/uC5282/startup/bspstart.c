/*
 *  BSP startup
 *
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  Author: W. Eric Norum <norume@aps.anl.gov>
 *
 *  COPYRIGHT (c) 2005.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <rtems/error.h>
#include <errno.h>
#include <stdio.h>
#include <mcf5282/mcf5282.h>

/*
 * Location of 'VME' access
 */
#define VME_ONE_BASE    0x30000000
#define VME_TWO_BASE    0x31000000

/*
 * Linker Script Defined Variables
 */
extern char RamSize[];
extern char RamBase[];
extern char _CPUClockSpeed[];
extern char _PLLRefClockSpeed[];

uint32_t BSP_sys_clk_speed = (uint32_t)_CPUClockSpeed;
uint32_t BSP_pll_ref_clock = (uint32_t)_PLLRefClockSpeed;
/*
 * CPU-space access
 * The NOP after writing the CACR is there to address the following issue as
 * described in "Device Errata MCF5282DE", Rev. 1.7, 09/2004:
 *
 * 6 Possible Cache Corruption after Setting  CACR[CINV]
 * 6.1 Description
 * The cache on the MCF5282 was enhanced to function as a unified data and
 * instruction cache, an instruction cache, or an operand cache.  The cache
 * function and organization is controlled by the cache control register (CACR).
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
#define m68k_set_cacr_nop(_cacr) __asm__ volatile ("movec %0,%%cacr\n\tnop" : : "d" (_cacr))
#define m68k_set_cacr(_cacr) __asm__ volatile ("movec %0,%%cacr" : : "d" (_cacr))
#define m68k_set_acr0(_acr0) __asm__ volatile ("movec %0,%%acr0" : : "d" (_acr0))
#define m68k_set_acr1(_acr1) __asm__ volatile ("movec %0,%%acr1" : : "d" (_acr1))

/*
 * Read/write copy of cache registers
 *   Split instruction/data or instruction-only
 *   Allow CPUSHL to invalidate a cache line
 *   Disable buffered writes
 *   No burst transfers on non-cacheable accesses
 *   Default cache mode is *disabled* (cache only ACRx areas)
 */
uint32_t mcf5282_cacr_mode = MCF5XXX_CACR_CENB |
#ifndef RTEMS_MCF5282_BSP_ENABLE_DATA_CACHE
                             MCF5XXX_CACR_DISD |
#endif
                             MCF5XXX_CACR_DCM;
uint32_t mcf5282_acr0_mode = 0;
uint32_t mcf5282_acr1_mode = 0;
/*
 * Cannot be frozen
 */
void _CPU_cache_freeze_data(void) {}
void _CPU_cache_unfreeze_data(void) {}
void _CPU_cache_freeze_instruction(void) {}
void _CPU_cache_unfreeze_instruction(void) {}

/*
 * Write-through data cache -- flushes are unnecessary
 */
void _CPU_cache_flush_1_data_line(const void *d_addr) {}
void _CPU_cache_flush_entire_data(void) {}

void _CPU_cache_enable_instruction(void)
{
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);
    mcf5282_cacr_mode &= ~MCF5XXX_CACR_DIDI;
    m68k_set_cacr_nop(mcf5282_cacr_mode | MCF5XXX_CACR_CINV | MCF5XXX_CACR_INVI);
    rtems_interrupt_enable(level);
}

void _CPU_cache_disable_instruction(void)
{
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);
    mcf5282_cacr_mode |= MCF5XXX_CACR_DIDI;
    m68k_set_cacr(mcf5282_cacr_mode);
    rtems_interrupt_enable(level);
}

void _CPU_cache_invalidate_entire_instruction(void)
{
    m68k_set_cacr_nop(mcf5282_cacr_mode | MCF5XXX_CACR_CINV | MCF5XXX_CACR_INVI);
}

void _CPU_cache_invalidate_1_instruction_line(const void *addr)
{
    /*
     * Top half of cache is I-space
     */
    addr = (void *)((int)addr | 0x400);
    __asm__ volatile ("cpushl %%bc,(%0)" :: "a" (addr));
}

void _CPU_cache_enable_data(void)
{
#ifdef RTEMS_MCF5282_BSP_ENABLE_DATA_CACHE
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);
    mcf5282_cacr_mode &= ~MCF5XXX_CACR_DISD;
    m68k_set_cacr_nop(mcf5282_cacr_mode | MCF5XXX_CACR_CINV | MCF5XXX_CACR_INVD);
    rtems_interrupt_enable(level);
#endif
}

void _CPU_cache_disable_data(void)
{
#ifdef RTEMS_MCF5282_BSP_ENABLE_DATA_CACHE
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);
    mcf5282_cacr_mode |= MCF5XXX_CACR_DISD;
    m68k_set_cacr(mcf5282_cacr_mode);
    rtems_interrupt_enable(level);
#endif
}

void _CPU_cache_invalidate_entire_data(void)
{
#ifdef RTEMS_MCF5282_BSP_ENABLE_DATA_CACHE
    m68k_set_cacr_nop(mcf5282_cacr_mode | MCF5XXX_CACR_CINV | MCF5XXX_CACR_INVD);
#endif
}

void _CPU_cache_invalidate_1_data_line(const void *addr)
{
#ifdef RTEMS_MCF5282_BSP_ENABLE_DATA_CACHE
    /*
     * Bottom half of cache is D-space
     */
    addr = (void *)((int)addr & ~0x400);
    __asm__ volatile ("cpushl %%bc,(%0)" :: "a" (addr));
#endif
}

extern void bsp_fake_syscall(int);

/*
 * The Arcturus boot ROM prints exception information improperly
 * so use this default exception handler instead.  This one also
 * prints a call backtrace
 */
static void handler(int pc)
{
    int level;
    static volatile int reent;

    rtems_interrupt_disable(level);
    if (reent++) bsp_sysReset(0);
    {
    int *p = &pc;
    int info = p[-1];
    int pc = p[0];
    int format = (info >> 28) & 0xF;
    int faultStatus = ((info >> 24) & 0xC) | ((info >> 16) & 0x3);
    int vector = (info >> 18) & 0xFF;
    int statusRegister = info & 0xFFFF;
    int *fp;

    printk("\n\nPC:%x  SR:%x  VEC:%x  FORMAT:%x  STATUS:%x\n", pc,
                                                               statusRegister,
                                                               vector,
                                                               format,
                                                               faultStatus);
    fp = &p[-2];
    for(;;) {
        int *nfp = (int *)*fp;
        if ((nfp <= fp)
         || ((char *)nfp >= RamSize)
         || ((char *)(nfp[1]) >= RamSize))
            break;
        printk("FP:%x -> %x    PC:%x\n", fp, nfp, nfp[1]);
        fp = nfp;
    }
    }
    rtems_task_suspend(0);
    rtems_panic("done");
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialisation.
 */
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
      if (i != (32+2)) /* Catch all but bootrom system calls */
          *((void (**)(int))(i * 4)) = handler;

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
  if ( 0 == *((void (**)(int))((32+2) * 4)) )
    *((void (**)(int))((32+2) * 4)) = bsp_fake_syscall;

  /*
   * Enable the cache
   */
  m68k_set_cacr(mcf5282_cacr_mode);

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

  if ( 0 == clk_speed )
	clk_speed = BSP_sys_clk_speed;

  if ( 0 == clk_speed ) {
	printk("Using some heuristics to determine clock speed...\n");
	byte = MCF5282_CLOCK_SYNSR;
	if ( 0 == byte ) {
		printk("SYNSR == 0; assuming QEMU at 66MHz\n");
		BSP_pll_ref_clock = 8250000;
		mfd = ( 0 << 8 ) | ( 2 << 12 );
	} else {
		if ( 0xf8 != byte ) {
			printk("FATAL ERROR: Unexpected SYNSR contents (0x%02x), can't proceed\n", byte);
			bsp_sysReset(0);
		}
		mfd = MCF5282_CLOCK_SYNCR;
	}
	printk("Assuming %uHz PLL ref. clock\n", BSP_pll_ref_clock);
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
	printk("PLL multiplier: %u, output divisor: %u\n", mfd, rfd);
	clk_speed = (BSP_pll_ref_clock * mfd) >> rfd;
  }

  if ( 0 == clk_speed ) {
	printk("FATAL ERROR: Unable to determine system clock speed\n");
	bsp_sysReset(0);
  } else {
  	BSP_sys_clk_speed = clk_speed;
	printk("System clock speed: %uHz\n", bsp_get_CPU_clock_speed());
  }
}

uint32_t bsp_get_CPU_clock_speed(void)
{
  return( BSP_sys_clk_speed );
}

/*
 * Interrupt controller allocation
 */
rtems_status_code
bsp_allocate_interrupt(int level, int priority)
{
  static char used[7];
  rtems_interrupt_level l;
  rtems_status_code ret = RTEMS_RESOURCE_IN_USE;

  if ((level < 1) || (level > 7) || (priority < 0) || (priority > 7))
    return RTEMS_INVALID_NUMBER;
  rtems_interrupt_disable(l);
  if ((used[level-1] & (1 << priority)) == 0) {
    used[level-1] |= (1 << priority);
    ret = RTEMS_SUCCESSFUL;
  }
  rtems_interrupt_enable(l);
  return ret;
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
syscall_3(int, flash_erase_range, volatile unsigned short *, flashptr, int, start, int, end);
syscall_3(int, flash_write_range, volatile unsigned short *, flashptr, bsp_mnode_t *, chain, int, offset);

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
int BSP_enableVME_int_lvl(unsigned int level) { return 0; }
int BSP_disableVME_int_lvl(unsigned int level) { return 0; }

/*
 * 'VME' interrupt support
 * Interrupt vectors 192-255 are set aside for use by external logic which
 * drives IRQ1*.  The actual interrupt source is read from the external
 * logic at FPGA_IRQ_INFO.  The most-significant bit of the least-significant
 * byte read from this location is set as long as the external logic has
 * interrupts to be serviced.  The least-significant six bits indicate the
 * interrupt source within the external logic and are used to select the
 * specified interupt handler.
 */
#define NVECTOR 256
#define FPGA_VECTOR (64+1)  /* IRQ1* pin connected to external FPGA */
#define FPGA_IRQ_INFO    *((vuint16 *)(0x31000000 + 0xfffffe))

static struct handlerTab {
  BSP_VME_ISR_t func;
  void         *arg;
} handlerTab[NVECTOR];

BSP_VME_ISR_t
BSP_getVME_isr(unsigned long vector, void **pusrArg)
{
  if (vector >= NVECTOR)
    return (BSP_VME_ISR_t)NULL;
  if (pusrArg)
    *pusrArg = handlerTab[vector].arg;
  return handlerTab[vector].func;
}

static rtems_isr
fpga_trampoline (rtems_vector_number v)
{
  /*
   * Handle FPGA interrupts until all have been consumed
   */
  int loopcount = 0;
  while (((v = FPGA_IRQ_INFO) & 0x80) != 0) {
    v = 192 + (v & 0x3f);
    if (++loopcount >= 50) {
      rtems_interrupt_level level;
      rtems_interrupt_disable(level);
      printk("\nTOO MANY FPGA INTERRUPTS (LAST WAS 0x%x) -- DISABLING ALL FPGA INTERRUPTS.\n", v & 0x3f);
      MCF5282_INTC0_IMRL |= MCF5282_INTC_IMRL_INT1;
      rtems_interrupt_enable(level);
      return;
    }
    if (handlerTab[v].func)  {
      (*handlerTab[v].func)(handlerTab[v].arg, (unsigned long)v);
    }
    else {
      rtems_interrupt_level level;
      rtems_vector_number nv;
      rtems_interrupt_disable(level);
      printk("\nSPURIOUS FPGA INTERRUPT (0x%x).\n", v & 0x3f);
      if ((((nv = FPGA_IRQ_INFO) & 0x80) != 0)
          && ((nv & 0x3f) == (v & 0x3f))) {
        printk("DISABLING ALL FPGA INTERRUPTS.\n");
        MCF5282_INTC0_IMRL |= MCF5282_INTC_IMRL_INT1;
      }
      rtems_interrupt_enable(level);
      return;
    }
  }
}

static rtems_isr
trampoline (rtems_vector_number v)
{
    if (handlerTab[v].func)
        (*handlerTab[v].func)(handlerTab[v].arg, (unsigned long)v);
}

static void
enable_irq(unsigned source)
{
rtems_interrupt_level level;
  rtems_interrupt_disable(level);
  if (source >= 32)
    MCF5282_INTC0_IMRH &= ~(1 << (source - 32));
  else
    MCF5282_INTC0_IMRL &= ~((1 << source) |
        MCF5282_INTC_IMRL_MASKALL);
  rtems_interrupt_enable(level);
}

static void
disable_irq(unsigned source)
{
rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  if (source >= 32)
    MCF5282_INTC0_IMRH |= (1 << (source - 32));
  else
    MCF5282_INTC0_IMRL |= (1 << source);
  rtems_interrupt_enable(level);
}

void
BSP_enable_irq_at_pic(rtems_vector_number v)
{
int                   source = v - 64;

  if ( source > 0 && source < 64 ) {
    enable_irq(source);
  }
}

void
BSP_disable_irq_at_pic(rtems_vector_number v)
{
int                   source = v - 64;

  if ( source > 0 && source < 64 ) {
    disable_irq(source);
  }
}

int
BSP_irq_is_enabled_at_pic(rtems_vector_number v)
{
int                   source = v - 64;

  if ( source > 0 && source < 64 ) {
    return ! ((source >= 32) ?
      MCF5282_INTC0_IMRH & (1 << (source - 32)) :
      MCF5282_INTC0_IMRL & (1 << source));
  }
  return -1;
}


static int
init_intc0_bit(unsigned long vector)
{
rtems_interrupt_level level;

    /*
     * Find an unused level/priority if this is an on-chip (INTC0)
     * source and this is the first time the source is being used.
     * Interrupt sources 1 through 7 are fixed level/priority
     */

    if ((vector >= 65) && (vector <= 127)) {
        int l, p;
        int source = vector - 64;
        static unsigned char installed[8];

        rtems_interrupt_disable(level);
        if (installed[source/8] & (1 << (source % 8))) {
            rtems_interrupt_enable(level);
            return 0;
        }
        installed[source/8] |= (1 << (source % 8));
        rtems_interrupt_enable(level);
        for (l = 1 ; l < 7 ; l++) {
            for (p = 0 ; p < 8 ; p++) {
                if ((source < 8)
                 || (bsp_allocate_interrupt(l,p) == RTEMS_SUCCESSFUL)) {
                    if (source < 8)
                        MCF5282_EPORT_EPIER |= 1 << source;
                    else
                        *(&MCF5282_INTC0_ICR1 + (source - 1)) =
                                                       MCF5282_INTC_ICR_IL(l) |
                                                       MCF5282_INTC_ICR_IP(p);
          enable_irq(source);
                    return 0;
                }
            }
        }
        return -1;
    }
  return 0;
}

int
BSP_installVME_isr(unsigned long vector, BSP_VME_ISR_t handler, void *usrArg)
{
  rtems_isr_entry old_handler;
  rtems_interrupt_level level;

  /*
   * Register the handler information
   */
  if (vector >= NVECTOR)
    return -1;
  handlerTab[vector].func = handler;
  handlerTab[vector].arg = usrArg;

  /*
   * If this is an external FPGA ('VME') vector set up the real IRQ.
   */
  if ((vector >= 192) && (vector <= 255)) {
    int i;
    static volatile int setupDone;
    rtems_interrupt_disable(level);
    if (setupDone) {
      rtems_interrupt_enable(level);
      return 0;
    }
    setupDone = 1;
    rtems_interrupt_catch(fpga_trampoline, FPGA_VECTOR, &old_handler);
    i = init_intc0_bit(FPGA_VECTOR);
    rtems_interrupt_enable(level);
    return i;
  }

  /*
   * Make the connection between the interrupt and the local handler
   */
  rtems_interrupt_catch(trampoline, vector, &old_handler);

  return init_intc0_bit(vector);
}

int
BSP_removeVME_isr(unsigned long vector, BSP_VME_ISR_t handler, void *usrArg)
{
  if (vector >= NVECTOR)
    return -1;
  if ((handlerTab[vector].func != handler)
     || (handlerTab[vector].arg != usrArg))
    return -1;
  handlerTab[vector].func = (BSP_VME_ISR_t)NULL;
  return 0;
}

int
BSP_vme2local_adrs(unsigned am, unsigned long vmeaddr, unsigned long *plocaladdr)
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
rtems_bsp_reset_cause(char *buf, size_t capacity)
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
