#include <bsp.h>
#include <rtems/bspIo.h>
#include <rtems/sysinit.h>
#include <rtems/rtems/status.h>
#include <bsp/uart.h>
#include <bsp/port.h>

static void kinetis_output_char(char c)
{
    while(kinetis_uart_is_busy());
    kinetis_uart_write_char_in_polling(CONSOLE_UART_MINOR, c);
}

static int kinetis_poll_char(void)
{
    return kinetis_uart_read_char_in_polling(CONSOLE_UART_MINOR);
}

static void kinetis_uart_probe(void)
{
    int i;

    //kinetis_gpio_select_uart();
    kinetis_uart_pin_init();
    
    for (i = KINETIS_UART_4; i <= KINETIS_UART_4; i++) {
        kinetis_uart_clock_enable(i);
        kinetis_uart_initialize(i, 115200);
    }

    BSP_output_char = kinetis_output_char;
    BSP_poll_char = kinetis_poll_char;
}

static void kinetis_console_early_init(char c)
{
    kinetis_uart_probe();
    kinetis_output_char(c);
}

BSP_output_char_function_type BSP_output_char =
  kinetis_console_early_init;

BSP_polling_getchar_function_type BSP_poll_char = NULL;

RTEMS_SYSINIT_ITEM(
  kinetis_uart_probe,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_LAST_BUT_5
);