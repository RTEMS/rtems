/*
 *
 */
#include <bsp/consoleIo.h>
#include <libcpu/spr.h>
#include <bsp/residual.h>
#include <bsp/pci.h>
#include <bsp/openpic.h>
#include <bsp/irq.h>
#include <bsp.h>
#include <libcpu/bat.h>

extern void _return_to_ppcbug();
extern unsigned long __rtems_end;

RESIDUAL residualCopy;
#define INIT_STACK_SIZE 0x1000
#define INTR_STACK_SIZE 0x4000

void BSP_panic(char *s)
{
  printk("RTEMS 4.x PANIC %s\n", s);
  _return_to_ppcbug();
}

void _BSP_Fatal_error(unsigned int v)
{
  printk("RTEMS 4.x PANIC ERROR %x\n", v);
  _return_to_ppcbug();
}

void
boot_card(RESIDUAL* r3, void *r4, void* r5, char *additional_boot_options)
{
  int err;
  unsigned char *stack;
  unsigned l2cr;
  register unsigned char* intrStack;
  register unsigned int intrNestingLevel = 0;
  
  L1_caches_enables();
  stack = ((unsigned char*) &__rtems_end) + INIT_STACK_SIZE;
  intrStack = ((unsigned char*) &__rtems_end) + INIT_STACK_SIZE + INTR_STACK_SIZE;
  asm volatile ("mtspr	273, %0" : "=r" (intrStack) : "0" (intrStack));
  asm volatile ("mtspr	272, %0" : "=r" (intrNestingLevel) : "0" (intrNestingLevel));
  residualCopy = *r3;
#define NO_DYNAMIC_EXCEPTION_VECTOR_INSTALL
#undef  NO_DYNAMIC_EXCEPTION_VECTOR_INSTALL
#ifndef NO_DYNAMIC_EXCEPTION_VECTOR_INSTALL  
  initialize_exceptions();
#endif
  select_console(CONSOLE_LOG);

  /* We check that the keyboard is present and immediately 
   * select the serial console if not.
   */
  err = kbdreset();
  if (err) select_console(CONSOLE_SERIAL);

  
  printk("Welcome to RTEMS 4.0.2 on Motorola MCP750\n\n\n");
  printk("-----------------------------------------\n");
  printk("Residuals are located at %x\n", (unsigned) r3);
  printk("Additionnal boot options are %s\n", additional_boot_options);
  printk("-----------------------------------------\n");

  printk("Initial system stack at %x\n",stack);
  l2cr = get_L2CR();
  printk("Initial L2CR value = %x\n", l2cr);
  if (! (l2cr & 0x80000000))
    set_L2CR(0xb9A14000);
#ifdef TEST_RETURN_TO_PPCBUG  
  printk("Hit <Enter> to return to PPCBUG monitor\n");
  printk("When Finished hit GO. It should print <Back from monitor>\n");
  debug_getc();
  _return_to_ppcbug();
  printk("Back from monitor\n");
  _return_to_ppcbug();
#endif /* TEST_RETURN_TO_PPCBUG  */

  /*
   * Init MMU block address translation to enable hardware
   * access
   */
  /*
   * PC legacy IO space used for inb/outb and all PC
   * compatible hardware
   */
  setdbat(1, 0x80000000, 0x80000000, 0x10000000, IO_PAGE);
  /*
   * PCI devices memory area. Needed to access OPENPIC features
   * provided by the RAVEN
   */
  setdbat(2, 0xc0000000, 0xc0000000, 0x08000000, IO_PAGE);
  /*
   * Must have acces to open pic PCI ACK registers 
   * provided by the RAVEN
   */
  setdbat(3, 0xfeff0000, 0xfeff0000, 0x10000, IO_PAGE);

  printk("Going to start PCI buses scanning and initialization\n");
  InitializePCI();
  printk("Number of PCI buses is found : %d\n", BusCountPCI());
#ifdef TEST_RAW_EXCEPTION_CODE  
  printk("Testing exception handling Part 1\n");
  /*
   * Cause a software exception
   */
  __asm__ __volatile ("sc");
  /*
   * Check we can still catch exceptions
   */
  printk("Testing exception handling Part 2\n");
  __asm__ __volatile ("sc");
#endif  
  BSP_rtems_irq_mng_init(0);
  printk("Init done\n");
  debug_getc();
  while(1);
}

