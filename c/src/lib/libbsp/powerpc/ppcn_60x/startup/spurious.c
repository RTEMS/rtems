/*
 *  PPCn_60x Spurious Trap Handler
 *
 *  This is just enough of a trap handler to let us know what 
 *  the likely source of the trap was.
 *
 *  Based upon the SPARC ERC32 version which was developed as
 *  part of the port of RTEMS to the ERC32 implementation 
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



  DEBUG_puts( "Spurious Trap" );
  
  switch ( trap ) {
    case PPC_IRQ_SYSTEM_RESET:
      DEBUG_puts( "System reset" );
      break;
    case PPC_IRQ_MCHECK:
      DEBUG_puts( "Machine check" );
      break;
    case PPC_IRQ_PROTECT:
      DEBUG_puts( "DSI" );
      break;
    case PPC_IRQ_ISI:
      DEBUG_puts( "ISI" );
      break;
    case PPC_IRQ_EXTERNAL:
      DEBUG_puts( "External interupt" );
      break;
    case PPC_IRQ_ALIGNMENT:
      DEBUG_puts( "Alignment Exception" );
      break;
    case PPC_IRQ_PROGRAM:
      DEBUG_puts( "Program" );
      break;
    case PPC_IRQ_NOFP:
      DEBUG_puts( "Floating point unavailable" );
      break;
    case PPC_IRQ_DECREMENTER:
      DEBUG_puts( "Decrementer" );
      break;
    case PPC_IRQ_RESERVED_A:
      DEBUG_puts( "Reserved 0x00a00" );
      break;
    case PPC_IRQ_RESERVED_B:
      DEBUG_puts( "Reserved 0x00b00" );
      break;
    case PPC_IRQ_SCALL:
      DEBUG_puts( "System call" );
      break;
    case PPC_IRQ_TRACE:
      DEBUG_puts( "Trace" );
      break;
    case PPC_IRQ_FP_ASST:
      DEBUG_puts( "Floating point Assist" );
      break;

#if defined(ppc403) || defined(ppc405)
    case PPC_IRQ_CRIT :
      DEBUG_puts( "Critical Error ");     
      break;
    case PPC_IRQ_PIT:
      DEBUG_puts( "Prog. Interval Timer " );
      break;
    case PPC_IRQ_FIT:
      DEBUG_puts( "Fixed Interval Timer " );
      break;
    case PPC_IRQ_WATCHDOG :
      DEBUG_puts( "Watchdog Timer " );
      break;
    case PPC_IRQ_DEBUG	 :
      DEBUG_puts( "Debug " );
      break;

#elif defined(ppc601)
#error "Please fill in names. "                                  
    case PPC_IRQ_TRACE    :
      DEBUG_puts( "0x02000" );
      break;

#elif defined(ppc603)
#error "Please fill in names. "                                  
    case PPC_IRQ_TRANS_MISS :
      DEBUG_puts( "0x1000" );
      break;
    case PPC_IRQ_DATA_LOAD:
      DEBUG_puts( "0x1100" );
      break;
    case PPC_IRQ_DATA_STORE:
      DEBUG_puts( "0x1200" );
      break;
    case PPC_IRQ_ADDR_BRK:
      DEBUG_puts( "0x1300" );
      break;
    case PPC_IRQ_SYS_MGT:
      DEBUG_puts( "0x1400" );
      break;

#elif defined(ppc603e)
    case PPC_TLB_INST_MISS:
      DEBUG_puts( "Instruction Translation Miss" );
      break;
    case PPC_TLB_LOAD_MISS:
      DEBUG_puts( "Data Load Translation Miss" );
      break;
    case PPC_TLB_STORE_MISS :
      DEBUG_puts( "Data store Translation Miss");
      break;
    case PPC_IRQ_ADDRBRK:
      DEBUG_puts( "Instruction address break point" );
      break;
    case PPC_IRQ_SYS_MGT:
      DEBUG_puts( "System management interrupt" );
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
     DEBUG_puts( "Undefined exception " );
     break;
  }

  /*
   *  What else can we do but stop ...
   */
  /*
   asm volatile( "" );
   */
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

    /*
     *  Skip window overflow, underflow, and flush as well as software
     *  trap 0 which we will use as a shutdown.
     */

    set_vector( bsp_spurious_handler, trap,  1 ); 
  }

  set_vector( bsp_stub_handler, PPC_IRQ_DECREMENTER, 1 );
  set_vector( bsp_stub_handler, PPC_IRQ_TRACE, 1 );
  set_vector( bsp_stub_handler, PPC_IRQ_SYS_MGT, 1 );
}








