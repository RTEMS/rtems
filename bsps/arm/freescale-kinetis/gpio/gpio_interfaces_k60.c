/**
 * @file gpio_interfaces_k60.c
 *
 * @ingroup kinetis_gpio
 *
 * @brief FreeScale Kinetis GPIO interface definitions.
 */

#include <bsp.h>
#include <bsp/gpio.h>

static kinetis_port_t * port_base_address[GPIO_BANK_COUNT] = 
{
    PORTA, PORTB, PORTC, PORTD, PORTE
};

static kinetis_gpio_t * gpio_base_address[GPIO_BANK_COUNT] = 
{
    PTA, PTB, PTC, PTD, PTE
};

rtems_status_code rtems_gpio_bsp_multi_select(
  rtems_gpio_multiple_pin_select *pins,
  uint32_t pin_count,
  uint32_t select_bank)
{
    kinetis_port_t *bank_address = port_base_address[select_bank];
    kinetis_gpio_t *gpio_address = gpio_base_address[select_bank];
    uint8_t i;

    port_clock_enable(bank_address);

    for ( i = 0; i < pin_count; ++i ) {
        if ( pins[i].function == DIGITAL_INPUT ) {
            gpio_bit_direction(gpio_address, pins[i].pin_number, IN);
        }
        else if ( pins[i].function == DIGITAL_OUTPUT ) {
            gpio_bit_direction(gpio_address, pins[i].pin_number, OUT);
        }
        else { /* BSP_SPECIFIC function. */
            port_bit_function(bank_address, pins[i].pin_number, pins[i].io_function);
        }
    }

    return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_gpio_bsp_clear(uint32_t bank, uint32_t pin)
{
    kinetis_gpio_t *gpio_address = gpio_base_address[bank];
    gpio_clear_bit(gpio_address, (0x1 << pin));
    return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_gpio_bsp_set(uint32_t bank, uint32_t pin)
{
    kinetis_gpio_t *gpio_address = gpio_base_address[bank];
    gpio_set_bit(gpio_address, (0x1 << pin));
    return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_gpio_bsp_enable_interrupt(
  uint32_t bank,
  uint32_t pin,
  rtems_gpio_interrupt interrupt)
{
    return RTEMS_NOT_DEFINED;
}

rtems_vector_number rtems_gpio_bsp_get_vector(uint32_t bank)
{
    return RTEMS_NOT_DEFINED;
}

rtems_status_code rtems_gpio_bsp_set_resistor_mode(
  uint32_t bank,
  uint32_t pin,
  rtems_gpio_pull_mode mode
)
{
    return RTEMS_NOT_DEFINED;
}

uint32_t rtems_gpio_bsp_interrupt_line(rtems_vector_number vector)
{
    return RTEMS_NOT_DEFINED;
}