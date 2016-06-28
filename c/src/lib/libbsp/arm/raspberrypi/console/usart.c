/**
 * @file
 *
 * @ingroup raspberrypi_usart
 *
 * @brief USART support.
 */

/*
 * Copyright (c) 2013 Alan Cudmore
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.org/license/LICENSE
 *
 */

#include <libchip/sersupp.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/usart.h>
#include <bsp/raspberrypi.h>
#include <rtems/bspIo.h>

static void usart_delay(uint32_t n)
{
 volatile uint32_t i = 0;

 for(i = 0; i < n; i++)
   ;
}

#if 0
/*
 *  These will be useful when the driver supports interrupt driven IO.
 */
static rtems_vector_number usart_get_irq_number(const console_tbl *ct)
{
  return ct->ulIntVector;
}

static uint32_t usart_get_baud(const console_tbl *ct)
{
  return ct->ulClock;
}
#endif

static void usart_set_baud(int minor, int baud)
{
 /*
  * Nothing for now
  */
 return;
}

static void usart_initialize(int minor)
{
  unsigned int gpio_reg;

  /*
  ** Program GPIO pins for UART 0
  */
  gpio_reg = BCM2835_REG(BCM2835_GPIO_GPFSEL1);
  gpio_reg &= ~(7<<12);    /* gpio14 */
  gpio_reg |=  (4<<12);    /* alt0   */
  gpio_reg &= ~(7<<15);    /* gpio15 */
  gpio_reg |=  (4<<15);    /* alt0   */
  BCM2835_REG(BCM2835_GPIO_GPFSEL1) = gpio_reg;

  BCM2835_REG(BCM2835_GPIO_GPPUD) = 0;
  usart_delay(150);
  BCM2835_REG(BCM2835_GPIO_GPPUDCLK0) = (1<<14)|(1<<15);
  usart_delay(150);
  BCM2835_REG(BCM2835_GPIO_GPPUDCLK0) = 0;

  /*
  ** Init the PL011 UART
  */
  BCM2835_REG(BCM2835_UART0_CR)   = 0;
  BCM2835_REG(BCM2835_UART0_ICR)  = 0x7FF;
  BCM2835_REG(BCM2835_UART0_IMSC) = 0;
  BCM2835_REG(BCM2835_UART0_IBRD) = 1;
  BCM2835_REG(BCM2835_UART0_FBRD) = 40;
  BCM2835_REG(BCM2835_UART0_LCRH) = 0x70;
  BCM2835_REG(BCM2835_UART0_RSRECR) =  0;

  BCM2835_REG(BCM2835_UART0_CR)   = 0x301;

  BCM2835_REG(BCM2835_UART0_IMSC) = BCM2835_UART0_IMSC_RX;

  usart_set_baud(minor, 115000);
}

static int usart_first_open(int major, int minor, void *arg)
{
  rtems_libio_open_close_args_t *oc = (rtems_libio_open_close_args_t *) arg;
  struct rtems_termios_tty *tty = (struct rtems_termios_tty *) oc->iop->data1;
  const console_tbl *ct = Console_Port_Tbl [minor];
  console_data *cd = &Console_Port_Data [minor];

  cd->termios_data = tty;
  rtems_termios_set_initial_baud(tty, ct->ulClock);

  return 0;
}

static int usart_last_close(int major, int minor, void *arg)
{
  return 0;
}

static int usart_read_polled(int minor)
{
  if (minor == 0) {
    if (((BCM2835_REG(BCM2835_UART0_FR)) & BCM2835_UART0_FR_RXFE) == 0) {
       return((BCM2835_REG(BCM2835_UART0_DR)) & 0xFF );
    } else {
      return -1;
    }
  } else {
    printk("Unknown console minor number: %d\n", minor);
    return -1;
  }
}

static void usart_write_polled(int minor, char c)
{
   while (1) {
     if ((BCM2835_REG(BCM2835_UART0_FR) & BCM2835_UART0_FR_TXFF) == 0)
       break;
   }
   BCM2835_REG(BCM2835_UART0_DR) = c;
}

static ssize_t usart_write_support_polled(
  int minor,
  const char *s,
  size_t n
)
{
  ssize_t i = 0;

  for (i = 0; i < n; ++i) {
    usart_write_polled(minor, s [i]);
  }

  return n;
}

static int usart_set_attributes(int minor, const struct termios *term)
{
  return -1;
}

const console_fns bcm2835_usart_fns = {
  .deviceProbe = libchip_serial_default_probe,
  .deviceFirstOpen = usart_first_open,
  .deviceLastClose = usart_last_close,
  .deviceRead = usart_read_polled,
  .deviceWrite = usart_write_support_polled,
  .deviceInitialize = usart_initialize,
  .deviceWritePolled = usart_write_polled,
  .deviceSetAttributes = usart_set_attributes,
  .deviceOutputUsesInterrupts = false
};
