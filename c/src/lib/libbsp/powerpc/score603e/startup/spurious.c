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

#include <string.h>

static const char digits[16] = "0123456789abcdef";

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
      DEBUG_puts( "\nTrap: System reset" );
      break;
    case PPC_IRQ_MCHECK:
      DEBUG_puts( "\nTrap: Machine check" );
      break;
    case PPC_IRQ_PROTECT:
      DEBUG_puts( "\nTrap: DSI" );
      break;
    case PPC_IRQ_ISI:
      DEBUG_puts( "ISI" );
      break;
    case PPC_IRQ_EXTERNAL:
      DEBUG_puts( "\nTrap: External interupt" );
      break;
    case PPC_IRQ_ALIGNMENT:
      DEBUG_puts( "\nTrap: Alignment Exception" );
      break;
    case PPC_IRQ_PROGRAM:
      DEBUG_puts( "\nTrap: Program" );
      break;
    case PPC_IRQ_NOFP:
      DEBUG_puts( "\nTrap: Floating point unavailable" );
      break;
    case PPC_IRQ_DECREMENTER:
      DEBUG_puts( "\nTrap: Decrementer" );
      break;
    case PPC_IRQ_RESERVED_A:
      DEBUG_puts( "\nTrap: Reserved 0x00a00" );
      break;
    case PPC_IRQ_RESERVED_B:
      DEBUG_puts( "\nTrap: Reserved 0x00b00" );
      break;
    case PPC_IRQ_SCALL:
      DEBUG_puts( "\nTrap: System call" );
      break;
    case PPC_IRQ_TRACE:
      DEBUG_puts( "\nTrap: Trace" );
      break;
    case PPC_IRQ_FP_ASST:
      DEBUG_puts( "\nTrap: Floating point Assist" );
      break;

#if defined(ppc403)
#error "Please fill in names. "                                  
    case PPC_IRQ_CRIT :
      DEBUG_puts( "\nTrap: Critical Error ");     
      break;
    case PPC_IRQ_PIT:
      DEBUG_puts( "\nTrap: 0x01000" );
      break;
    case PPC_IRQ_FIT:
      DEBUG_puts( "\nTrap: 0x01010" );
      break;
    case PPC_IRQ_WATCHDOG :
      DEBUG_puts( "\nTrap: 0x01020" );
      break;
    case PPC_IRQ_DEBUG	 :
      DEBUG_puts( "\nTrap: 0x02000" );
      break;

#elif defined(ppc601)
#error "Please fill in names. "                                  
    case PPC_IRQ_TRACE    :
      DEBUG_puts( "\nTrap: 0x02000" );
      break;

#elif defined(ppc603)
#error "Please fill in names. "                                  
    case PPC_IRQ_TRANS_MISS :
      DEBUG_puts( "\nTrap: 0x1000" );
      break;
    case PPC_IRQ_DATA_LOAD:
      DEBUG_puts( "\nTrap: 0x1100" );
      break;
    case PPC_IRQ_DATA_STORE:
      DEBUG_puts( "\nTrap: 0x1200" );
      break;
    case PPC_IRQ_ADDR_BRK:
      DEBUG_puts( "\nTrap: 0x1300" );
      break;
    case PPC_IRQ_SYS_MGT:
      DEBUG_puts( "\nTrap: 0x1400" );
      break;

#elif defined(ppc603e)
    case PPC_TLB_INST_MISS:
      DEBUG_puts( "\nTrap: Instruction Translation Miss" );
      break;
    case PPC_TLB_LOAD_MISS:
      DEBUG_puts( "\nTrap: Data Load Translation Miss" );
      break;
    case PPC_TLB_STORE_MISS :
      DEBUG_puts( "\nTrap: Data store Translation Miss");
      break;
    case PPC_IRQ_ADDRBRK:
      DEBUG_puts( "\nTrap: Instruction address break point" );
      break;
    case PPC_IRQ_SYS_MGT:
      DEBUG_puts( "\nTrap: System management interrupt" );
      break;

#elif defined(mpc604)
#error "Please fill in names. "                                  
    case PPC_IRQ_ADDR_BRK:
      DEBUG_puts( "0x1300" );
      break;
    case PPC_IRQ_SYS_MGT:
      DEBUG_puts( "0x1400" );
      break;
#endif

  default:
     DEBUG_puts( "\nTrap: Undefined exception " );
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
  rtems_unsigned32 trap;

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
  volatile rtems_unsigned32 *ptr;

  /* reset_vector  */
  ptr = (rtems_unsigned32 *)0x00100  ;
  *ptr = 0x48000000;

  /* org    mach_vector  */
  ptr = (rtems_unsigned32 *)0x00200;
  *ptr = 0x48000000;

  /* org    prot_vector  */
  ptr = (rtems_unsigned32 *)0x00300;
  *ptr = 0x48000000;

  /* org    isi_vector  */
  ptr = (rtems_unsigned32 *)0x00400;
  *ptr = 0x48000000;

  /* org    ext_vector  */
  ptr = (rtems_unsigned32 *)0x0500  ;
  *ptr = 0x48000000;

  /* org    align_vector  */
  ptr = (rtems_unsigned32 *)0x00600  ;
  *ptr = 0x48000000;

  /* org    prog_vector  */
  ptr = (rtems_unsigned32 *)0x00700  ;
  *ptr = 0x48000000;

  /* org    float_vector  */
  ptr = (rtems_unsigned32 *)0x00800;
  *ptr = 0x48000000;

  /* org    dec_vector  - rfi */
  ptr = (rtems_unsigned32 *)0x900;
  *ptr = 0x4c000064;

  /* org    sys_vector  */
  ptr = (rtems_unsigned32 *)0x0c00  ;
  *ptr = 0x48000000;

  /* org    trace_vector  */
  ptr = (rtems_unsigned32 *)0x0d00  ;
  *ptr = 0x48000000;

  /* org    itm_vector  */
  ptr = (rtems_unsigned32 *)0x01000  ;
  *ptr = 0x48000000;

  /* org    dltm_vector  */
  ptr = (rtems_unsigned32 *)0x01100  ;
  *ptr = 0x48000000;

  /* org    dstm_vector  */
  ptr = (rtems_unsigned32 *)0x1200  ;
  *ptr = 0x48000000;

  /* org    addr_vector  */
  ptr = (rtems_unsigned32 *)0x1300  ;
  *ptr = 0x48000000;

  /* org    sysmgmt_vector  */
  ptr = (rtems_unsigned32 *)0x1400  ;
  *ptr = 0x48000000;

}





