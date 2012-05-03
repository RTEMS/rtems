/*#########################################################
#
# This code is a modified version of what you will find at the
# end of the IDP User's manual.  The original code is copyrighted
# by Motorola and Motorola Semiconductor Products as well as
# Motorola Software products group.
#
# Modifications to the original IDP code by Doug McBride, Colorado
# Space Grant College.  Modifications include a means of accessing
# port B of the duart as well as port A as well as modifications for
# buffering and RTEMS support.  Modifications are provided
# as is and may not be correct.
#
# Rob Savoye provided the format for the mc68681 header file
#
# Joel Sherrill provided inspiration for recoding my original assembly
# for this file into C (a good idea)
#
##########################################################*/

#include <bsp.h>
#include <rtems/ringbuf.h>

rtems_isr C_Receive_ISR(rtems_vector_number vector);
extern Ring_buffer_t  Console_Buffer[];

extern unsigned char inbuf[];
extern unsigned char inbuf_portb[];
extern unsigned tail;
extern unsigned tail_portb;
unsigned char Pit_initialized = 0;

/*#####################################################################
# The volatile routine to initialize the duart -- port a and port b
######################################################################*/
void init_pit()
{
  /*
   *  ports A & B while configuring PIT by:
   *
   *    + disable Interrupt Mask Register
   *    + disable port A transmitter
   *    + disable port A receiver
   *    + disable port B transmitter
   *    + disable port B receiver
   */

  MC68681_WRITE(DUART_ADDR, MC68681_INTERRUPT_MASK_REG, 0x00);
  MC68681_WRITE(DUART_ADDR, MC68681_COMMAND_REG_A ,MC68681_MODE_REG_DISABLE_TX);
  MC68681_WRITE(DUART_ADDR, MC68681_COMMAND_REG_A, MC68681_MODE_REG_DISABLE_RX);
  MC68681_WRITE(DUART_ADDR, MC68681_COMMAND_REG_B, MC68681_MODE_REG_DISABLE_TX);
  MC68681_WRITE(DUART_ADDR, MC68681_COMMAND_REG_B, MC68681_MODE_REG_DISABLE_RX);

  /*
   *  install ISR for ports A and B
   */
  set_vector(C_Receive_ISR, (MC68230_VECT+MC68230_H3VECT), 1);

  /*
   *  initialize pit
   *
   *  set mode to 0 -- disable all ports
   *  set up pirq and piack
   *  all pins on port b are input
   *  submode 1x, h3 interrupt enabled
   *  setup pivr
   *  turn on all ports
   */
  MC68230_WRITE(MC68230_PGCR, 0x00);
  MC68230_WRITE(MC68230_PSRR, 0x18);
  MC68230_WRITE(MC68230_PBDDR, 0x00);
  MC68230_WRITE(MC68230_PBCR, 0x82);
  MC68230_WRITE(MC68230_PIVR, MC68230_VECT);
  MC68230_WRITE(MC68230_PGCR, 0x20);

  /*
   *  For some reason, the reset of receiver/transmitter only works for
   *  the first time around -- it garbles the output otherwise
   *  (e.g., sp21)
   */
  if (!Pit_initialized)
  {
    /*
     * initialize the duart registers on port b
     * WARNING:OPTIMIZER MAY ONLY EXECUTE THIRD STATEMENT IF NOT VOLATILE
     *
     *  reset tx, channel b
     *  reset rx, channel b
     *  reset mr pointer, ch
     */
     MC68681_WRITE(DUART_ADDR, MC68681_COMMAND_REG_B, MC68681_MODE_REG_RESET_TX);
     MC68681_WRITE(DUART_ADDR, MC68681_COMMAND_REG_B, MC68681_MODE_REG_RESET_RX);
     MC68681_WRITE(DUART_ADDR, MC68681_COMMAND_REG_B, MC68681_MODE_REG_RESET_MR_PTR);

    /*
     * initialize the duart registers on port a
     * WARNING:OPTIMIZER MAY ONLY EXECUTE THIRD STATEMENT IF NOT VOLATILE
     *
     *  reset tx, channel a
     *  reset rx, channel a
     *  reset mr pointer, ch
     */
     MC68681_WRITE(DUART_ADDR, MC68681_COMMAND_REG_A, MC68681_MODE_REG_RESET_TX);
     MC68681_WRITE(DUART_ADDR, MC68681_COMMAND_REG_A, MC68681_MODE_REG_RESET_RX);
     MC68681_WRITE(DUART_ADDR, MC68681_COMMAND_REG_A, MC68681_MODE_REG_RESET_MR_PTR);

     Pit_initialized = 1;
  }

  /*
   * Init the general registers of the duart
   *
   * init ivr
   * init imr
   * init acr
   * init ctur
   * init ctlr
   * init opcr
   * init cts
   */
  MC68681_WRITE(DUART_ADDR, MC68681_INTERRUPT_VECTOR_REG,
                MC68681_INTERRUPT_VECTOR_INIT);
  MC68681_WRITE(DUART_ADDR, MC68681_INTERRUPT_MASK_REG,
                MC68681_IR_RX_READY_A | MC68681_IR_RX_READY_B);
  MC68681_WRITE(DUART_ADDR, MC68681_AUX_CTRL_REG, MC68681_CLEAR);
  MC68681_WRITE(DUART_ADDR, MC68681_COUNTER_TIMER_UPPER_REG, 0x00);
  MC68681_WRITE(DUART_ADDR, MC68681_COUNTER_TIMER_LOWER_REG, 0x02);
  MC68681_WRITE(DUART_ADDR, MC68681_OUTPUT_PORT_CONFIG_REG, MC68681_CLEAR);
  MC68681_WRITE(DUART_ADDR, MC68681_OUTPUT_PORT_SET_REG, 0x01);

  /*
   * init the actual serial port for port a
   *
   * Set Baud Rate to 9600
   * Set Stop bit length of 1
   * enable Transmit and receive
   */
  MC68681_WRITE(DUART_ADDR, MC68681_CLOCK_SELECT_REG_A, MC68681_BAUD_RATE_MASK_9600);
  MC68681_WRITE(DUART_ADDR, MC68681_MODE_REG_1A,
                (MC68681_8BIT_CHARS | MC68681_NO_PARITY));
  MC68681_WRITE(DUART_ADDR, MC68681_MODE_REG_2A,MC68681_STOP_BIT_LENGTH_1);
  MC68681_WRITE(DUART_ADDR, MC68681_COMMAND_REG_A,
                (MC68681_MODE_REG_ENABLE_TX | MC68681_MODE_REG_ENABLE_RX));

  /*
   * init the actual serial port for port b
   * init csrb -- 9600 baud
   */
  MC68681_WRITE(DUART_ADDR, MC68681_CLOCK_SELECT_REG_B, MC68681_BAUD_RATE_MASK_9600);

#define EIGHT_BITS_NO_PARITY
#ifdef EIGHT_BITS_NO_PARITY
  /*
   * Set 8 Bit characters with no parity
   */
  MC68681_WRITE(DUART_ADDR, MC68681_MODE_REG_1B,
                (MC68681_NO_PARITY | MC68681_8BIT_CHARS) );
#else
  /*
   * Set 7 Bit Characters with parity
   */
  MC68681_WRITE(DUART_ADDR, MC68681_MODE_REG_1B,
                (MC68681_WITH_PARITY |  MC68681_7BIT_CHARS) );
#endif

  /*
   * Set Stop Bit length to 1
   * Disable Recieve and transmit on B
   */
  MC68681_WRITE(DUART_ADDR, MC68681_MODE_REG_2B,MC68681_STOP_BIT_LENGTH_1);
  MC68681_WRITE(DUART_ADDR, MC68681_COMMAND_REG_B,
                (MC68681_MODE_REG_ENABLE_TX | MC68681_MODE_REG_ENABLE_RX) );
}

/*#####################################################################
# interrupt handler for receive of character from duart on ports A & B
#####################################################################*/
rtems_isr C_Receive_ISR(rtems_vector_number vector)
{
  volatile unsigned char *_addr;

  /*
   *  Clear pit interrupt.
   */
  _addr = (unsigned char *) (MC68230_PIT_ADDR + MC68230_PITSR);
  *_addr = 0x04;

  /*
   *  check port A first for input
   *     extract rcvrdy on port B
   *     set ptr to recieve buffer and read character into ring buffer
   */
  _addr = (unsigned char *) (DUART_ADDR + MC68681_STATUS_REG_A);
  if (*_addr & MC68681_RX_READY)  /* extract rcvrdy on port A */
  {
    _addr = (unsigned char *) (DUART_ADDR + MC68681_RECEIVE_BUFFER_A);
    Ring_buffer_Add_character( &Console_Buffer[ 0 ], *_addr );
  }

  /*
   *  If not on port A, let's check port B
   *     extract rcvrdy on port B
   *     set ptr to recieve buffer and read character into ring buffer
   */
  else
  {
    _addr = (unsigned char *) (DUART_ADDR + MC68681_STATUS_REG_B);
    if (*_addr & MC68681_RX_READY)  /* extract rcvrdy on port B */
    {
      _addr = (unsigned char *) (DUART_ADDR + MC68681_RECEIVE_BUFFER_B);
      Ring_buffer_Add_character( &Console_Buffer[ 1 ], *_addr );
    }

    /*
     * if not ready on port A or port B, must be an error
     * if error, get out so that fifo is undisturbed
     */
  }
}

/*#####################################################################
# This is the routine that actually transmits a character one at a time
# This routine transmits on port A of the IDP board
#####################################################################*/
void transmit_char(char ch)
{
  volatile unsigned char *_addr;

  /*
   * Get SRA (extract txrdy)
   */
  _addr = (unsigned char *) (DUART_ADDR + MC68681_STATUS_REG_A);
  while (!(*_addr & MC68681_TX_READY))
  {
  }

  /*
   * transmit character over port A
   */
  MC68681_WRITE(DUART_ADDR, MC68681_TRANSMIT_BUFFER_A, ch);
}

/*#####################################################################
# This is the routine that actually transmits a character one at a time
# This routine transmits on port B of the IDP board
#####################################################################*/
void transmit_char_portb(char ch)
{
  volatile unsigned char *_addr;

  /*
   * Get SRB (extract txrdy)
   */
  _addr = (unsigned char *) (DUART_ADDR + MC68681_STATUS_REG_B);
  while (!(*_addr &  MC68681_TX_READY))
  {
  }

  /*
   * transmit character over port B
   */
  MC68681_WRITE(DUART_ADDR, MC68681_TRANSMIT_BUFFER_B, ch);
}
