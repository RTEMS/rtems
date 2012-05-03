/*
 * linux/drivers/char/pc_keyb.c
 * Separation of the PC low-level part by Geert Uytterhoeven, May 1997
 * See keyboard.c for the whole history.
 * Major cleanup by Martin Mares, May 1997
 * Combined the keyboard and PS/2 mouse handling into one file,
 * because they share the same hardware.
 * Johan Myreen <jem@iki.fi> 1998-10-08.
 * Code fixes to handle mouse ACKs properly.
 * C. Scott Ananian <cananian@alumni.princeton.edu> 1999-01-29.
 *
 *  RTEMS port: by Rosimildo da Silva.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/libio.h>
#include <termios.h>
#include <i386_io.h>
#include <rtems/mw_uid.h>
#include <rtems/mouse_parser.h>

#define  INITIALIZE_MOUSE
/* Some configuration switches are present in the include file... */
#include "ps2_mouse.h"

static void kbd_write_command_w(int data);
#if 0
static void kbd_write_output_w(int data);
#endif

static unsigned char handle_kbd_event(void);

/* used only by send_data - set by keyboard_interrupt */
static volatile unsigned char reply_expected = 0;
static volatile unsigned char acknowledge = 0;
static volatile unsigned char resend = 0;

/*
 *	PS/2 Auxiliary Device
 */
static int psaux_init(void);

static struct aux_queue *queue;	/* Mouse data buffer. */
static int aux_count = 0;
/* used when we send commands to the mouse that expect an ACK. */
static unsigned char mouse_reply_expected = 0;

#define AUX_INTS_OFF (KBD_MODE_KCC | KBD_MODE_DISABLE_MOUSE | KBD_MODE_SYS | KBD_MODE_KBD_INT)
#define AUX_INTS_ON  (KBD_MODE_KCC | KBD_MODE_SYS | KBD_MODE_MOUSE_INT | KBD_MODE_KBD_INT)
#define MAX_RETRIES	60		/* some aux operations take long time*/

static void ps2_mouse_interrupt(rtems_irq_hdl_param);
static mouse_parser_enqueue_handler driver_input_handler_ps2 = NULL;

/*
 * This routine sets the handler to handle the characters received
 * from the serial port.
 */
void ps2_set_driver_handler(
  int                          port,
  mouse_parser_enqueue_handler handler
)
{
  driver_input_handler_ps2 = handler;
}

static void mdelay( unsigned long t )
{
  Wait_X_ms( t );
}

static void*    termios_ttyp_paux = NULL;

static void
isr_on(const rtems_irq_connect_data *unused)
{
  return;
}

static void
isr_off(const rtems_irq_connect_data *unused)
{
  return;
}

static int isr_is_on(const rtems_irq_connect_data *irq)
{
  return BSP_irq_enabled_at_i8259s( irq->name );
}

static rtems_irq_connect_data ps2_isr_data = { AUX_IRQ,
                                               ps2_mouse_interrupt,
                                               0,
                                               isr_on,
                                               isr_off,
                                               isr_is_on };

/*
 * Wait for keyboard controller input buffer to drain.
 *
 * Don't use 'jiffies' so that we don't depend on
 * interrupts..
 *
 * Quote from PS/2 System Reference Manual:
 *
 * "Address hex 0060 and address hex 0064 should be written only when
 * the input-buffer-full bit and output-buffer-full bit in the
 * Controller Status register are set 0."
 */

static void kb_wait(void)
{
  unsigned long timeout = KBC_TIMEOUT;

  do {
    /*
     * "handle_kbd_event()" will handle any incoming events
     * while we wait - keypresses or mouse movement.
     */
    unsigned char status = handle_kbd_event();

    if (! (status & KBD_STAT_IBF))
      return;

    mdelay(1);
    timeout--;
  } while (timeout);

  #ifdef KBD_REPORT_TIMEOUTS
    printk( "Keyboard timed out[1]\n");
  #endif
}

static int do_acknowledge(unsigned char scancode)
{
  if (reply_expected) {

    /* Unfortunately, we must recognise these codes only if we know they
     * are known to be valid (i.e., after sending a command), because there
     * are some brain-damaged keyboards (yes, FOCUS 9000 again) which have
     * keys with such codes :(
     */
    if (scancode == KBD_REPLY_ACK) {
      acknowledge = 1;
      reply_expected = 0;
      return 0;
    } else if (scancode == KBD_REPLY_RESEND) {
      resend = 1;
      reply_expected = 0;
      return 0;
    }

    /* Should not happen... */
    #if 0
      printk( "keyboard reply expected - got %02x\n", scancode);
    #endif
  }
  return 1;
}

static inline void handle_mouse_event(unsigned char scancode)
{
  if (mouse_reply_expected) {
    if (scancode == AUX_ACK) {
      mouse_reply_expected--;
      return;
    }
    mouse_reply_expected = 0;
  }

  if (aux_count) {
    int head = queue->head;
    queue->buf[head] = scancode;
    head = (head + 1) & (AUX_BUF_SIZE-1);
    if (head != queue->tail) {
      queue->head = head;
    }

    /* if the input queue is active, add to it */
    if( driver_input_handler_ps2 ) {
      driver_input_handler_ps2( &scancode, 1 );
    } else {
      /* post this byte to termios */
      rtems_termios_enqueue_raw_characters( termios_ttyp_paux, (char *)&scancode, 1 );
    }
  }
}

/*
 * This reads the keyboard status port, and does the
 * appropriate action.
 *
 * It requires that we hold the keyboard controller
 * spinlock.
 */
static unsigned char handle_kbd_event(void)
{
  unsigned char status = kbd_read_status();
  unsigned int work = 10000;

  while (status & KBD_STAT_OBF) {
    unsigned char scancode;
    scancode = kbd_read_input();
    if (status & KBD_STAT_MOUSE_OBF) {
      handle_mouse_event(scancode);
    } else {
      do_acknowledge(scancode);
      printk("pc_keyb: %X ", scancode );
    }
    status = kbd_read_status();
    if(!work--) {
      printk("pc_keyb: controller jammed (0x%02X).\n", status);
      break;
    }
  }
  return status;
}

static void ps2_mouse_interrupt(rtems_irq_hdl_param ignored)
{
  handle_kbd_event();
}

static void kbd_write_command_w(int data)
{
  kb_wait();
  kbd_write_command(data);
}

static void kbd_write_cmd(int cmd)
{
  kb_wait();
  kbd_write_command(KBD_CCMD_WRITE_MODE);
  kb_wait();
  kbd_write_output(cmd);
}

/*
 * Check if this is a dual port controller.
 */
static int detect_auxiliary_port(void)
{
  int loops = 10;
  int retval = 0;

  /* Put the value 0x5A in the output buffer using the "Write
   * Auxiliary Device Output Buffer" command (0xD3). Poll the
   * Status Register for a while to see if the value really
   * turns up in the Data Register. If the KBD_STAT_MOUSE_OBF
   * bit is also set to 1 in the Status Register, we assume this
   * controller has an Auxiliary Port (a.k.a. Mouse Port).
   */
  kb_wait();
  kbd_write_command(KBD_CCMD_WRITE_AUX_OBUF);

  kb_wait();
  kbd_write_output(0x5a); /* 0x5a is a random dummy value. */

  do {
    unsigned char status = kbd_read_status();
    if (status & KBD_STAT_OBF) {
      kbd_read_input();
      if (status & KBD_STAT_MOUSE_OBF) {
        printk( "Detected PS/2 Mouse Port.\n");
        retval = 1;
      }
      break;
    }
    mdelay(1);
  } while (--loops);
  return retval;
}

/*
 * Send a byte to the mouse.
 */
static void aux_write_dev(int val)
{
  kb_wait();
  kbd_write_command(KBD_CCMD_WRITE_MOUSE);
  kb_wait();
  kbd_write_output(val);
}

/*
 * Send a byte to the mouse & handle returned ack
 */
static void aux_write_ack(int val)
{
  kb_wait();
  kbd_write_command(KBD_CCMD_WRITE_MOUSE);
  kb_wait();
  kbd_write_output(val);
  /* we expect an ACK in response. */
  mouse_reply_expected++;
  kb_wait();
}

static unsigned char get_from_queue(void)
{
  unsigned char result;
  result = queue->buf[queue->tail];
  queue->tail = (queue->tail + 1) & (AUX_BUF_SIZE-1);
  return result;
}

static int queue_empty(void)
{
  return queue->head == queue->tail;
}

/*
 * Random magic cookie for the aux device
 */
#define AUX_DEV ((void *)queue)

static int release_aux(void)
{
  if (--aux_count)
    return 0;
  kbd_write_cmd(AUX_INTS_OFF);			    /* Disable controller ints */
  kbd_write_command_w(KBD_CCMD_MOUSE_DISABLE);
  BSP_remove_rtems_irq_handler( &ps2_isr_data );
  return 0;
}

/*
 * Install interrupt handler.
 * Enable auxiliary device.
 */

static int open_aux(void)
{
  int status;

  if (aux_count++) {
    return 0;
  }
  queue->head = queue->tail = 0;		/* Flush input queue */

  status = BSP_install_rtems_irq_handler( &ps2_isr_data );
  if( !status ) {
    printk("Error installing ps2-mouse interrupt handler!\n" );
    rtems_fatal_error_occurred( status );
  }

  kbd_write_command_w(KBD_CCMD_MOUSE_ENABLE); /* Enable the auxiliary port on
                                                 controller. */
  aux_write_ack(AUX_ENABLE_DEV); /* Enable aux device */
  kbd_write_cmd(AUX_INTS_ON); /* Enable controller ints */
  return 0;
}

/*
 * Put bytes from input queue to buffer.
 */
size_t read_aux(char * buffer, size_t count )
{
  size_t i = count;
  unsigned char c;

  if (queue_empty()) {
    return 0;
  }
  while (i > 0 && !queue_empty()) {
    c = get_from_queue();
    *buffer++ = c;
    i--;
  }
  return count-i;
}

/*
 * Write to the aux device.
 */
static int write_aux( int minor, const char * buffer, int count )
{
  int retval = 0;

  if (count) {
    int written = 0;
    if (count > 32)
      count = 32; /* Limit to 32 bytes. */
    do {
      char c;
      c = *buffer++;
      aux_write_dev(c);
      written++;
    } while (--count);
    retval = -EIO;
    if (written) {
      retval = written;
    }
  }
  return retval;
}

static int psaux_init( void )
{
  if( !detect_auxiliary_port() ) {
    printk( "PS/2 - mouse not found.\n" );
    return -EIO;
  }
  queue = (struct aux_queue *)malloc( sizeof(*queue) );
  memset(queue, 0, sizeof(*queue));
  queue->head = queue->tail = 0;
  queue->proc_list = NULL;

  #ifdef INITIALIZE_MOUSE
    kbd_write_command_w(KBD_CCMD_MOUSE_ENABLE); /* Enable Aux. */
    aux_write_ack(AUX_SET_SAMPLE);
    aux_write_ack(100);			      /* 100 samples/sec */
    aux_write_ack(AUX_SET_RES);
    aux_write_ack(3);			         /* 8 counts per mm */
    aux_write_ack(AUX_SET_SCALE21);	/* 2:1 scaling */
  #endif /* INITIALIZE_MOUSE */
  kbd_write_command(KBD_CCMD_MOUSE_DISABLE); /* Disable aux device. */
  kbd_write_cmd(AUX_INTS_OFF); /* Disable controller ints. */
  return 0;
}

/*
 * paux device driver INITIALIZE entry point.
 */
rtems_device_driver paux_initialize(  
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                      *arg)
{
  rtems_status_code status;

  /*
   * Set up TERMIOS
   */
  rtems_termios_initialize();

  printk( "PS/2 mouse probe.\n" );
  if( psaux_init() < 0 ) {
    printk("Error detecting PS/2 mouse --\n");
    /* we might want to finish the application here !!! */
  }
  open_aux();

  /*
   * Register the device
   */
  status = rtems_io_register_name ("/dev/mouse", major, 0);
  if (status != RTEMS_SUCCESSFUL) {
    printk("Error registering paux device!\n");
    rtems_fatal_error_occurred (status);
  }
  return RTEMS_SUCCESSFUL;
} /* tty_initialize */

static int paux_last_close(int major, int minor, void *arg)
{
  release_aux();
  return 0;
}

/*
 * Write to the aux device. This routine is invoked by the
 * termios framework whenever the "ECHO" feature is on.
 * It does nothing write now.
 */
static ssize_t write_aux_echo( int minor, const char * buffer, size_t count )
{
  return 0;
}

/*
 * paux device driver OPEN entry point
 */
rtems_device_driver paux_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                      *arg)
{
  rtems_status_code              status;
  static rtems_termios_callbacks cb =
  {
    NULL,	          /* firstOpen */
    paux_last_close,      /* lastClose */
    NULL,                 /* poll read  */
    write_aux_echo,       /* write */
    NULL,	          /* setAttributes */
    NULL,	          /* stopRemoteTx */
    NULL,	          /* startRemoteTx */
    0		          /* outputUsesInterrupts */
  };

  status = rtems_termios_open (major, minor, arg, &cb );
  termios_ttyp_paux = ( (rtems_libio_open_close_args_t *)arg)->iop->data1;
  return status;
}

/*
 * paux device driver CLOSE entry point
 */
rtems_device_driver paux_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                      *arg)
{
  return (rtems_termios_close (arg));
}

/*
 * paux device driver READ entry point.
 * Read characters from the PS/2 mouse.
 */
rtems_device_driver paux_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                      *arg)
{
  return rtems_termios_read (arg);
} /* tty_read */

/*
 * paux device driver WRITE entry point.
 * Write characters to the PS/2 mouse.
 */
rtems_device_driver  paux_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                      *arg)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  char                  *buffer  = rw_args->buffer;
  int                    maximum  = rw_args->count;
  rw_args->bytes_moved = write_aux( minor, buffer, maximum );
  return RTEMS_SUCCESSFUL;
} /* tty_write */

/*
 * Handle ioctl request.
 */
rtems_device_driver paux_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                      *arg
)
{
  rtems_libio_ioctl_args_t *args = arg;

  switch( args->command ) {
    default:
      return rtems_termios_ioctl (arg);
      break;

    case MW_UID_REGISTER_DEVICE:
      printk( "PS2 Mouse: registering\n" );
      mouse_parser_initialize( "ps2" );
      ps2_set_driver_handler( minor, mouse_parser_enqueue );
      break;

    case MW_UID_UNREGISTER_DEVICE:
/*
      unregister_mou_msg_queue( -1 );
*/
      ps2_set_driver_handler( minor, NULL );
      break;
  }
  args->ioctl_return = 0;
  return RTEMS_SUCCESSFUL;
}
