/*****************************************************************************/
/*
  Boot the CPU.

  Occurs in 3 phases for a 68302.

  Phase 1.

  Called as soon as able after reset. The BAR has been programed, and
  a small stack exists in the DPRAM. All interrupts are masked, and
  the processor is running in supervisor mode. No other hardware or
  chip selects are active.

  This phase programs the chip select registers, the parallel ports
  are set into default configurations, and basic registers cleared or
  reset. The leds are programmed to show the end of phase 1.

  Phase 2.

  This is a piece of code which is copied to DPRAM and executed. It
  should not do any more thann is currently present. The return to ROM
  is managed by modifing the return address. Again leds show the status.

  Phase 3.

  This code executes with a valid C environment. That is the data
  section has been intialised and the bss section set to 0. This phase
  performs any special card initialisation and then calls boot card.

*/
/*****************************************************************************/

#include <bsp.h>
#include <rtems/m68k/m68302.h>
#include <debugport.h>
#include <crc.h>

/*
  Open the address, reset all registers
  */

extern int ROM_SIZE, ROM_BASE;
extern int RAM_SIZE, RAM_BASE;

#define _ROM_SIZE ((unsigned int)&ROM_SIZE)
#define _ROM_BASE ((unsigned int)&ROM_BASE)
#define _RAM_SIZE ((unsigned int)&RAM_SIZE)
#define _RAM_BASE ((unsigned int)&RAM_BASE)

void boot_phase_1(void)
{
  M302_SCR = SCR_DEFAULT;

  WRITE_OR(CSEL_ROM, _ROM_SIZE, ROM_WAIT_STATES, OR_MASK_RW, OR_MASK_FC);
  WRITE_BR(CSEL_ROM, _RAM_BASE, BR_READ_ONLY, BR_FC_NULL, BR_ENABLED);
  WRITE_OR(CSEL_RAM, _RAM_SIZE, RAM_WAIT_STATES, OR_MASK_RW, OR_MASK_FC);
  WRITE_BR(CSEL_RAM, _ROM_BASE, BR_READ_WRITE, BR_FC_NULL, BR_ENABLED);

#if defined(CSEL_1)
  WRITE_OR(CSEL_1, CSEL_1_SIZE, CSEL_1_WAIT_STATES, OR_MASK_RW, OR_MASK_FC);
  WRITE_BR(CSEL_1, CSEL_1_BASE, BR_READ_WRITE, BR_FC_NULL, BR_ENABLED);
#endif

#if defined(CSEL_2)
  WRITE_OR(CSEL_2, CSEL_2_SIZE, CSEL_2_WAIT_STATES, OR_MASK_RW, OR_MASK_FC);
  WRITE_BR(CSEL_2, CSEL_2_BASE, BR_READ_WRITE, BR_FC_NULL, BR_ENABLED);
#endif

  m302.reg.ipr = m302.reg.imr = m302.reg.isr = 0;
  m302.reg.gimr = 0x0080;

  m302.reg.simode = 0;

  m302.reg.pacnt = CARD_PA_CONFIGURATION;
  m302.reg.paddr = CARD_PA_DEFAULT_DIRECTIONS;
  m302.reg.padat = CARD_PA_DEFAULT_DATA;

  m302.reg.pbcnt = CARD_PB_CONFIGURATION;
  m302.reg.pbddr = CARD_PB_DEFAULT_DIRECTIONS;
  m302.reg.pbdat = CARD_PB_DEFAULT_DATA;

  m302.reg.wrr = WATCHDOG_TIMEOUT_PERIOD | WATCHDOG_ENABLE;

#if defined(LED_CONTROL)
  LED_CONTROL(LED_1_RED, LED_2_OFF, LED_3_OFF, LED_4_OFF,
              LED_5_OFF, LED_6_OFF, LED_7_OFF, LED_8_OFF);
#endif
}

/*
  Swap the chip select mapping for ROM and RAM
  */

void boot_phase_2(void)
{
  uint32_t         stack;

#if defined(LED_CONTROL)
  LED_CONTROL(LED_1_RED, LED_2_RED, LED_3_OFF, LED_4_OFF,
              LED_5_OFF, LED_6_OFF, LED_7_OFF, LED_8_OFF);
#endif

  WRITE_BR(CSEL_ROM, _ROM_BASE, BR_READ_ONLY, BR_FC_NULL, BR_ENABLED);
  WRITE_BR(CSEL_RAM, _RAM_BASE, BR_READ_WRITE, BR_FC_NULL, BR_ENABLED);

#if defined(LED_CONTROL)
  LED_CONTROL(LED_1_GREEN, LED_2_RED, LED_3_OFF, LED_4_OFF,
              LED_5_OFF, LED_6_OFF, LED_7_OFF, LED_8_OFF);
#endif

  /* seems to want 2, looked at assember code output */
  *((volatile uint32_t*) (&stack + 2)) |= ROM_BASE;
}

/*
  Any pre-main initialisation, the C environment is setup, how-ever C++
  static constructors have not been called, and RTEMS is not initialised.
  */

void boot_card(const char* cmdline);
void set_debug_traps(void);
void breakpoint(void);

void boot_phase_3(void)
{
  if (GDB_RUN_MONITOR())
  {
    set_debug_traps();
    breakpoint();
  }

  debug_port_banner();

  /* FIXME : add RAM and ROM checks */

  /* boot the bsp, what ever this means */
  boot_card((void*)0);

  WATCHDOG_TRIGGER();
}
