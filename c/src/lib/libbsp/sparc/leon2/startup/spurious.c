/*
 *  LEON Spurious Trap Handler
 *
 *  This is just enough of a trap handler to let us know what
 *  the likely source of the trap was.
 *
 *  Developed as part of the port of RTEMS to the LEON implementation
 *  of the SPARC by On-Line Applications Research Corporation (OAR)
 *  under contract to the European Space Agency (ESA).
 *
 *  COPYRIGHT (c) 1995. European Space Agency.
 *
 *  This terms of the RTEMS license apply to this file.
 */

#include <bsp.h>
#include <rtems/bspIo.h>

/*
 *  bsp_spurious_handler
 *
 *  Print a message on the debug console and then die
 */

rtems_isr bsp_spurious_handler(
   rtems_vector_number trap,
   CPU_Interrupt_frame *isf
)
{
  uint32_t real_trap;

  real_trap = SPARC_REAL_TRAP_NUMBER(trap);

  printk( "Unexpected trap (%2d) at address 0x%08x\n", real_trap, isf->tpc);

  switch (real_trap) {

    /*
     *  First the ones defined by the basic architecture
     */

    case 0x00:
      printk( "reset\n" );
      break;
    case 0x01:
      printk( "instruction access exception\n" );
      break;
    case 0x02:
      printk( "illegal instruction\n" );
      break;
    case 0x03:
      printk( "privileged instruction\n" );
      break;
    case 0x04:
      printk( "fp disabled\n" );
      break;
    case 0x07:
      printk( "memory address not aligned\n" );
      break;
    case 0x08:
      printk( "fp exception\n" );
      break;
    case 0x09:
      printk("data access exception at 0x%08x\n", LEON_REG.Failed_Address );
      break;
    case 0x0A:
      printk( "tag overflow\n" );
      break;

    /*
     *  Then the ones defined by the LEON in particular
     */

    case LEON_TRAP_TYPE( LEON_INTERRUPT_CORRECTABLE_MEMORY_ERROR ):
      printk( "LEON_INTERRUPT_CORRECTABLE_MEMORY_ERROR\n" );
      break;
    case LEON_TRAP_TYPE( LEON_INTERRUPT_UART_2_RX_TX ):
      printk( "LEON_INTERRUPT_UART_2_RX_TX\n" );
      break;
    case LEON_TRAP_TYPE( LEON_INTERRUPT_UART_1_RX_TX ):
      printk( "LEON_INTERRUPT_UART_1_RX_TX\n" );
      break;
    case LEON_TRAP_TYPE( LEON_INTERRUPT_EXTERNAL_0 ):
      printk( "LEON_INTERRUPT_EXTERNAL_0\n" );
      break;
    case LEON_TRAP_TYPE( LEON_INTERRUPT_EXTERNAL_1 ):
      printk( "LEON_INTERRUPT_EXTERNAL_1\n" );
      break;
    case LEON_TRAP_TYPE( LEON_INTERRUPT_EXTERNAL_2 ):
      printk( "LEON_INTERRUPT_EXTERNAL_2\n" );
      break;
    case LEON_TRAP_TYPE( LEON_INTERRUPT_EXTERNAL_3 ):
      printk( "LEON_INTERRUPT_EXTERNAL_3\n" );
      break;
    case LEON_TRAP_TYPE( LEON_INTERRUPT_TIMER1 ):
      printk( "LEON_INTERRUPT_TIMER1\n" );
      break;
    case LEON_TRAP_TYPE( LEON_INTERRUPT_TIMER2 ):
      printk( "LEON_INTERRUPT_TIMER2\n" );
      break;

    default:
      break;
  }

  /*
   *  What else can we do but stop ...
   */

  __asm__ volatile( "mov 1, %g1; ta 0x0" );
}

/*
 *  bsp_spurious_initialize
 *
 *  Install the spurious handler for most traps. Note that set_vector()
 *  will unmask the corresponding asynchronous interrupt, so the initial
 *  interrupt mask is restored after the handlers are installed.
 */

void bsp_spurious_initialize()
{
  uint32_t trap;
  uint32_t level;
  uint32_t mask;

  level = sparc_disable_interrupts();
  mask = LEON_REG.Interrupt_Mask;

  for ( trap=0 ; trap<256 ; trap++ ) {

    /*
     *  Skip window overflow, underflow, and flush as well as software
     *  trap 0 which we will use as a shutdown. Also avoid trap 0x70 - 0x7f
     *  which cannot happen and where some of the space is used to pass
     *  paramaters to the program.
     */

    if (( trap == 5 || trap == 6 ) ||
    	(( trap >= 0x11 ) && ( trap <= 0x1f )) ||
    	(( trap >= 0x70 ) && ( trap <= 0x83 )))
      continue;

    set_vector(
        (rtems_isr_entry) bsp_spurious_handler,
        SPARC_SYNCHRONOUS_TRAP( trap ),
        1
    );
  }

  LEON_REG.Interrupt_Mask = mask;
  sparc_enable_interrupts(level);

}
