/*
 *  Score603e Spurious Trap Handler
 *
 *  This is just enough of a trap handler to let us know what
 *  the likely source of the trap was.
 *
 *  Developed as part of the port of RTEMS to the ERC32 implementation
 *  of the SPARC by On-Line Applications Research Corporation (OAR)
 *  under contract to the European Space Agency (ESA).
 *
 *  COPYRIGHT (c) 1995. European Space Agency.
 *
 *  This terms of the RTEMS license apply to this file.
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/bspIo.h>

#include <string.h>

rtems_isr bsp_stub_handler(
   rtems_vector_number trap
)
{
}

/*
 *  bsp_spurious_handler
 *
 *  Print a message on the debug console and then die
 */
rtems_isr bsp_spurious_handler(
   rtems_vector_number trap
)
{

  switch ( trap ) {
    case PPC_IRQ_SYSTEM_RESET:
      printk( "\nTrap: System reset\n" );
      break;
    case PPC_IRQ_MCHECK:
      printk( "\nTrap: Machine check\n" );
      break;
    case PPC_IRQ_PROTECT:
      printk( "\nTrap: DSI\n" );
      break;
    case PPC_IRQ_ISI:
      printk( "ISI\n" );
      break;
    case PPC_IRQ_EXTERNAL:
      printk( "\nTrap: External interupt\n" );
      break;
    case PPC_IRQ_ALIGNMENT:
      printk( "\nTrap: Alignment Exception\n" );
      break;
    case PPC_IRQ_PROGRAM:
      printk( "\nTrap: Program\n" );
      break;
    case PPC_IRQ_NOFP:
      printk( "\nTrap: Floating point unavailable\n" );
      break;
    case PPC_IRQ_DECREMENTER:
      printk( "\nTrap: Decrementer\n" );
      break;
    case PPC_IRQ_RESERVED_A:
      printk( "\nTrap: Reserved 0x00a00\n" );
      break;
    case PPC_IRQ_RESERVED_B:
      printk( "\nTrap: Reserved 0x00b00\n" );
      break;
    case PPC_IRQ_SCALL:
      printk( "\nTrap: System call\n" );
      break;
    case PPC_IRQ_TRACE:
      printk( "\nTrap: Trace\n" );
      break;
    case PPC_IRQ_FP_ASST:
      printk( "\nTrap: Floating point Assist\n" );
      break;

#if defined(ppc403)
#error "Please fill in names. "
    case PPC_IRQ_CRIT :
      printk( "\nTrap: Critical Error\n ");
      break;
    case PPC_IRQ_PIT:
      printk( "\nTrap: 0x01000\n" );
      break;
    case PPC_IRQ_FIT:
      printk( "\nTrap: 0x01010\n" );
      break;
    case PPC_IRQ_WATCHDOG :
      printk( "\nTrap: 0x01020\n" );
      break;
    case PPC_IRQ_DEBUG	 :
      printk( "\nTrap: 0x02000\n" );
      break;

#elif defined(ppc601)
#error "Please fill in names. "
    case PPC_IRQ_TRACE    :
      printk( "\nTrap: 0x02000\n" );
      break;

#elif defined(ppc603) || defined(ppc603e)
    case PPC_IRQ_TRANS_MISS:
      printk( "\nTrap: Instruction Translation Miss\n" );
      break;
    case PPC_IRQ_DATA_LOAD:
      printk( "\nTrap: Data Load Translation Miss\n" );
      break;
    case PPC_IRQ_DATA_STORE:
      printk( "\nTrap: Data store Translation Mis\ns");
      break;
    case PPC_IRQ_ADDR_BRK:
      printk( "\nTrap: Instruction address break point\n" );
      break;
    case PPC_IRQ_SYS_MGT:
      printk( "\nTrap: System management interrupt\n" );
      break;

#elif defined(mpc604)
#error "Please fill in names. "
    case PPC_IRQ_ADDR_BRK:
      printk( "0x1300\n" );
      break;
    case PPC_IRQ_SYS_MGT:
      printk( "0x1400\n" );
      break;
#endif

  default:
     printk( "\nTrap: Undefined exception\n" );
     break;
  }

  /*
   *  What else can we do but stop ...
   */
  /*
   asm volatile( "" );
   */
   while (1);
}

/*
 *  bsp_spurious_initialize
 *
 *  Install the spurious handler for most traps.
 */

void bsp_spurious_initialize()
{
  uint32_t         trap;

  for ( trap=0 ; trap < PPC_IRQ_LAST ; trap++ ) {
    if (trap == PPC_IRQ_DECREMENTER)
      ;
      /* set_vector( bsp_stub_handler, trap, 1 ); */
    else
      set_vector( bsp_spurious_handler, trap,  1 );
  }
}

void bsp_set_trap_vectors( void )
{
  volatile uint32_t         *ptr;

  /* reset_vector  */
  ptr = (uint32_t*)0x00100  ;
  *ptr = 0x48000000;

  /* org    mach_vector  */
  ptr = (uint32_t*)0x00200;
  *ptr = 0x48000000;

  /* org    prot_vector  */
  ptr = (uint32_t*)0x00300;
  *ptr = 0x48000000;

  /* org    isi_vector  */
  ptr = (uint32_t*)0x00400;
  *ptr = 0x48000000;

  /* org    ext_vector  */
  ptr = (uint32_t*)0x0500  ;
  *ptr = 0x48000000;

  /* org    align_vector  */
  ptr = (uint32_t*)0x00600  ;
  *ptr = 0x48000000;

  /* org    prog_vector  */
  ptr = (uint32_t*)0x00700  ;
  *ptr = 0x48000000;

  /* org    float_vector  */
  ptr = (uint32_t*)0x00800;
  *ptr = 0x48000000;

  /* org    dec_vector  - rfi */
  ptr = (uint32_t*)0x900;
  *ptr = 0x4c000064;

  /* org    sys_vector  */
  ptr = (uint32_t*)0x0c00  ;
  *ptr = 0x48000000;

  /* org    trace_vector  */
  ptr = (uint32_t*)0x0d00  ;
  *ptr = 0x48000000;

  /* org    itm_vector  */
  ptr = (uint32_t*)0x01000  ;
  *ptr = 0x48000000;

  /* org    dltm_vector  */
  ptr = (uint32_t*)0x01100  ;
  *ptr = 0x48000000;

  /* org    dstm_vector  */
  ptr = (uint32_t*)0x1200  ;
  *ptr = 0x48000000;

  /* org    addr_vector  */
  ptr = (uint32_t*)0x1300  ;
  *ptr = 0x48000000;

  /* org    sysmgmt_vector  */
  ptr = (uint32_t*)0x1400  ;
  *ptr = 0x48000000;

}
