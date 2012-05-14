/**
 * @file
 *
 * @brief Driver for serial ports on the ERC32.
 */

/*
 * Copyright (c) 2010 Tiemen Schut <T.Schut@sron.nl>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/console.h>
#include <rtems/termiostypes.h>

#include <libchip/serial.h>
#include <libchip/sersupp.h>

#include <bsp.h>
#include <bspopts.h>

#define CONSOLE_BUF_SIZE (16)

#define CONSOLE_UART_A_TRAP  ERC32_TRAP_TYPE(ERC32_INTERRUPT_UART_A_RX_TX)
#define CONSOLE_UART_B_TRAP  ERC32_TRAP_TYPE(ERC32_INTERRUPT_UART_B_RX_TX)

static uint8_t erc32_console_get_register(uint32_t addr, uint8_t i)
{
  volatile uint32_t *reg = (volatile uint32_t *)addr;
  return (uint8_t) reg [i];
}

static void erc32_console_set_register(uint32_t addr, uint8_t i, uint8_t val)
{
  volatile uint32_t *reg = (volatile uint32_t *)addr;
  reg [i] = val;
}

static int erc32_console_first_open(int major, int minor, void *arg);

#if (CONSOLE_USE_INTERRUPTS)
  static ssize_t erc32_console_write_support_int(
    int minor, const char *buf, size_t len);
#else
  int console_inbyte_nonblocking( int port );
  static ssize_t erc32_console_write_support_polled(
      int minor, const char *buf, size_t len);
#endif
static void erc32_console_initialize(int minor);

#if (CONSOLE_USE_INTERRUPTS)
  console_fns erc32_fns = {
    libchip_serial_default_probe,           /* deviceProbe */
    erc32_console_first_open,               /* deviceFirstOpen */
    NULL,                                   /* deviceLastClose */
    NULL,                                   /* deviceRead */
    erc32_console_write_support_int,        /* deviceWrite */
    erc32_console_initialize,               /* deviceInitialize */
    NULL,                                   /* deviceWritePolled */
    NULL,                                   /* deviceSetAttributes */
    TERMIOS_IRQ_DRIVEN                      /* deviceOutputUsesInterrupts */
  };
#else
  console_fns erc32_fns = {
    libchip_serial_default_probe,           /* deviceProbe */
    erc32_console_first_open,               /* deviceFirstOpen */
    NULL,                                   /* deviceLastClose */
    console_inbyte_nonblocking,             /* deviceRead */
    erc32_console_write_support_polled,     /* deviceWrite */
    erc32_console_initialize,               /* deviceInitialize */
    NULL,                                   /* deviceWritePolled */
    NULL,                                   /* deviceSetAttributes */
    TERMIOS_POLLED                          /* deviceOutputUsesInterrupts */
  };
#endif

console_tbl Console_Configuration_Ports [] = {
  {
    .sDeviceName = "/dev/console_a",
    .deviceType = SERIAL_CUSTOM,
    .pDeviceFns = &erc32_fns,
    .deviceProbe = NULL,
    .pDeviceFlow = NULL,
    .ulMargin = 16,
    .ulHysteresis = 8,
    .pDeviceParams = (void *) -1,  /* could be baud rate */
    .ulCtrlPort1 = 0,
    .ulCtrlPort2 = 0,
    .ulDataPort = 0,
    .getRegister = erc32_console_get_register,
    .setRegister = erc32_console_set_register,
    .getData = NULL,
    .setData = NULL,
    .ulClock = 16,
    .ulIntVector = ERC32_INTERRUPT_UART_A_RX_TX
  },
  {
    .sDeviceName = "/dev/console_b",
    .deviceType = SERIAL_CUSTOM,
    .pDeviceFns = &erc32_fns,
    .deviceProbe = NULL,
    .pDeviceFlow = NULL,
    .ulMargin = 16,
    .ulHysteresis = 8,
    .pDeviceParams = (void *) -1,  /* could be baud rate */
    .ulCtrlPort1 = 0,
    .ulCtrlPort2 = 0,
    .ulDataPort = 0,
    .getRegister = erc32_console_get_register,
    .setRegister = erc32_console_set_register,
    .getData = NULL,
    .setData = NULL,
    .ulClock = 16,
    .ulIntVector = ERC32_INTERRUPT_UART_B_RX_TX
  },
};

/* always exactly two uarts for erc32 */
#define ERC32_UART_COUNT (2)

unsigned long Console_Configuration_Count = ERC32_UART_COUNT;

static int erc32_console_first_open(int major, int minor, void *arg)
{
  /* Check minor number */
  if (minor < 0 || minor > 1) {
    return -1;
  }
  
  rtems_libio_open_close_args_t *oca = arg;
  struct rtems_termios_tty *tty = oca->iop->data1;
  console_tbl *ct = Console_Port_Tbl [minor];
  console_data *cd = &Console_Port_Data [minor];
  
  cd->termios_data = tty;
  rtems_termios_set_initial_baud(tty, (int32_t)ct->pDeviceParams);
  
  return 0;
}

#if (CONSOLE_USE_INTERRUPTS)
static ssize_t erc32_console_write_support_int(int minor, const char *buf, size_t len)
{
  console_data *cd = &Console_Port_Data[minor];
  int k = 0;

  if (minor == 0) { /* uart a */
    for (k = 0;
         k < len && (ERC32_MEC.UART_Status & ERC32_MEC_UART_STATUS_THEA); k ++) {
      ERC32_MEC.UART_Channel_A = (unsigned char)buf[k];
    }
    ERC32_Force_interrupt(ERC32_INTERRUPT_UART_A_RX_TX);
  } else { /* uart b */
    for (k = 0;
         k < len && (ERC32_MEC.UART_Status & ERC32_MEC_UART_STATUS_THEB); k ++) {
      ERC32_MEC.UART_Channel_B = (unsigned char)buf[k];
    }
    ERC32_Force_interrupt(ERC32_INTERRUPT_UART_B_RX_TX);
  }
  
  if (len > 0) {
    cd->pDeviceContext = (void *)k;
    cd->bActive = true;
  }
  
  return 0;
}

static void erc32_console_isr_a(
  rtems_vector_number vector
)
{
  console_data *cd = &Console_Port_Data[0];

  /* check for error */
  if (ERC32_MEC.UART_Status & ERC32_MEC_UART_STATUS_ERRA) {
    ERC32_MEC.UART_Status = ERC32_MEC_UART_STATUS_CLRA;
    ERC32_MEC.Control = ERC32_MEC.Control;
  }

  do {
    int chars_to_dequeue = (int)cd->pDeviceContext;
    int rv = 0;
    int i = 0;
    char buf[CONSOLE_BUF_SIZE];
        
    /* enqueue received chars */
    while (i < CONSOLE_BUF_SIZE) {
      if (!(ERC32_MEC.UART_Status & ERC32_MEC_UART_STATUS_DRA))
        break;
      buf[i] = ERC32_MEC.UART_Channel_A;
      ++i;
    }
    if ( i ) 
      rtems_termios_enqueue_raw_characters(cd->termios_data, buf, i);

    /* dequeue transmitted chars */
    if (ERC32_MEC.UART_Status & ERC32_MEC_UART_STATUS_THEA) {
      rv = rtems_termios_dequeue_characters(
         cd->termios_data, chars_to_dequeue);
      if ( !rv ) {
        cd->pDeviceContext = 0;
        cd->bActive = false;
      }
      ERC32_Clear_interrupt (ERC32_INTERRUPT_UART_A_RX_TX);
    }
  } while (ERC32_Is_interrupt_pending (ERC32_INTERRUPT_UART_A_RX_TX));
}

static void erc32_console_isr_b(
  rtems_vector_number vector
)
{
  console_data *cd = &Console_Port_Data[1];

  /* check for error */
  if (ERC32_MEC.UART_Status & ERC32_MEC_UART_STATUS_ERRB) {
      ERC32_MEC.UART_Status = ERC32_MEC_UART_STATUS_CLRB;
      ERC32_MEC.Control = ERC32_MEC.Control;
  }

  do {
    int chars_to_dequeue = (int)cd->pDeviceContext;
    int rv = 0;
    int i = 0;
    char buf[CONSOLE_BUF_SIZE];
        
    /* enqueue received chars */
    while (i < CONSOLE_BUF_SIZE) {
      if (!(ERC32_MEC.UART_Status & ERC32_MEC_UART_STATUS_DRB))
        break;
      buf[i] = ERC32_MEC.UART_Channel_B;
      ++i;
    }
    if ( i ) 
      rtems_termios_enqueue_raw_characters(cd->termios_data, buf, i);

    /* dequeue transmitted chars */
    if (ERC32_MEC.UART_Status & ERC32_MEC_UART_STATUS_THEB) {
      rv = rtems_termios_dequeue_characters(
         cd->termios_data, chars_to_dequeue);
      if ( !rv ) {
        cd->pDeviceContext = 0;
        cd->bActive = false;
      }
      ERC32_Clear_interrupt (ERC32_INTERRUPT_UART_B_RX_TX);
    }
  } while (ERC32_Is_interrupt_pending (ERC32_INTERRUPT_UART_B_RX_TX));
}
#else

extern void console_outbyte_polled( int  port, unsigned char ch );

static ssize_t erc32_console_write_support_polled(
  int         minor,
  const char *buf,
  size_t      len
)
{
  int nwrite = 0;

  while (nwrite < len) {
    console_outbyte_polled( minor, *buf++ );
    nwrite++;
  }
  return nwrite;
}

#endif


/*
 *  Console Device Driver Entry Points
 *
 */

static void erc32_console_initialize(
    int minor
)
{
  console_data *cd = &Console_Port_Data [minor];

  cd->bActive = false;
  cd->pDeviceContext = 0;

 /*
  * Initialize the Termios infrastructure.  If Termios has already
  * been initialized by another device driver, then this call will
  * have no effect.
  */
  rtems_termios_initialize();

 /*
  *  Initialize Hardware
  */
  #if (CONSOLE_USE_INTERRUPTS)
    set_vector(erc32_console_isr_a, CONSOLE_UART_A_TRAP, 1);
    set_vector(erc32_console_isr_b, CONSOLE_UART_B_TRAP, 1);
  #endif
}
