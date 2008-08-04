/*
 *  ERC32 Spurious Trap Handler
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

/* Simple integer-to-string conversion */

void itos(uint32_t   u, char *s)
{
  int i;

  for (i=0; i<8; i++) {
    s[i] =  digits[(u >> (28 - (i*4))) & 0x0f];
  }
}

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
  char line[ 80 ];
  uint32_t         real_trap;

  real_trap = SPARC_REAL_TRAP_NUMBER(trap);

  strcpy(line, "Unexpected trap (0x  ) at address 0x        ");
  line[ 19 ] = digits[ real_trap >> 4 ];
  line[ 20 ] = digits[ real_trap & 0xf ];
  itos(isf->tpc, &line[36]);
  DEBUG_puts( line );

  switch (real_trap) {

    /*
     *  First the ones defined by the basic architecture
     */

    case 0x00:
      DEBUG_puts( "reset" );
      break;
    case 0x01:
      DEBUG_puts( "instruction access exception" );
      break;
    case 0x02:
      DEBUG_puts( "illegal instruction" );
      break;
    case 0x03:
      DEBUG_puts( "privileged instruction" );
      break;
    case 0x04:
      DEBUG_puts( "fp disabled" );
      break;
    case 0x07:
      DEBUG_puts( "memory address not aligned" );
      break;
    case 0x08:
      DEBUG_puts( "fp exception" );
      break;
    case 0x09:
      strcpy(line, "data access exception at 0x        " );
      itos(ERC32_MEC.First_Failing_Address, &line[27]);
      DEBUG_puts( line );
      break;
    case 0x0A:
      DEBUG_puts( "tag overflow" );
      break;

    /*
     *  Then the ones defined by the ERC32 in particular
     */

    case ERC32_TRAP_TYPE( ERC32_INTERRUPT_MASKED_ERRORS ):
      DEBUG_puts( "ERC32_INTERRUPT_MASKED_ERRORS" );
      break;
    case ERC32_TRAP_TYPE( ERC32_INTERRUPT_EXTERNAL_1 ):
      DEBUG_puts( "ERC32_INTERRUPT_EXTERNAL_1" );
      break;
    case ERC32_TRAP_TYPE( ERC32_INTERRUPT_EXTERNAL_2 ):
      DEBUG_puts( "ERC32_INTERRUPT_EXTERNAL_2" );
      break;
    case ERC32_TRAP_TYPE( ERC32_INTERRUPT_UART_A_RX_TX ):
      DEBUG_puts( "ERC32_INTERRUPT_UART_A_RX_TX" );
      break;
    case ERC32_TRAP_TYPE( ERC32_INTERRUPT_UART_B_RX_TX ):
      DEBUG_puts( "ERC32_INTERRUPT_UART_A_RX_TX" );
      break;
    case ERC32_TRAP_TYPE( ERC32_INTERRUPT_CORRECTABLE_MEMORY_ERROR ):
      DEBUG_puts( "ERC32_INTERRUPT_CORRECTABLE_MEMORY_ERROR" );
      break;
    case ERC32_TRAP_TYPE( ERC32_INTERRUPT_UART_ERROR ):
      DEBUG_puts( "ERC32_INTERRUPT_UART_ERROR" );
      break;
    case ERC32_TRAP_TYPE( ERC32_INTERRUPT_DMA_ACCESS_ERROR ):
      DEBUG_puts( "ERC32_INTERRUPT_DMA_ACCESS_ERROR" );
      break;
    case ERC32_TRAP_TYPE( ERC32_INTERRUPT_DMA_TIMEOUT ):
      DEBUG_puts( "ERC32_INTERRUPT_DMA_TIMEOUT" );
      break;
    case ERC32_TRAP_TYPE( ERC32_INTERRUPT_EXTERNAL_3 ):
      DEBUG_puts( "ERC32_INTERRUPT_EXTERNAL_3" );
      break;
    case ERC32_TRAP_TYPE( ERC32_INTERRUPT_EXTERNAL_4 ):
      DEBUG_puts( "ERC32_INTERRUPT_EXTERNAL_4" );
      break;
    case ERC32_TRAP_TYPE( ERC32_INTERRUPT_GENERAL_PURPOSE_TIMER ):
      DEBUG_puts( "ERC32_INTERRUPT_GENERAL_PURPOSE_TIMER" );
      break;
    case ERC32_TRAP_TYPE( ERC32_INTERRUPT_REAL_TIME_CLOCK ):
      DEBUG_puts( "ERC32_INTERRUPT_REAL_TIME_CLOCK" );
      break;
    case ERC32_TRAP_TYPE( ERC32_INTERRUPT_EXTERNAL_5 ):
      DEBUG_puts( "ERC32_INTERRUPT_EXTERNAL_5" );
      break;
    case ERC32_TRAP_TYPE( ERC32_INTERRUPT_WATCHDOG_TIMEOUT ):
      DEBUG_puts( "ERC32_INTERRUPT_WATCHDOG_TIMEOUT" );
      break;

    default:
      break;
  }

  /*
   *  What else can we do but stop ...
   */

  asm volatile( "mov 1, %g1; ta 0x0" );
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
  uint32_t   trap;
  uint32_t   level = 15;
  uint32_t   mask;

  level = sparc_disable_interrupts();
  mask = ERC32_MEC.Interrupt_Mask;

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

    set_vector( (rtems_isr_entry) bsp_spurious_handler,
         SPARC_SYNCHRONOUS_TRAP( trap ), 1 );
  }

  ERC32_MEC.Interrupt_Mask = mask;
  sparc_enable_interrupts(level);

}
