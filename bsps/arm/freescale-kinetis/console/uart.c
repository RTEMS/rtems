/*
 * Implementation of Kinetis UARTs driver.
 *
 * Written yonoko <shiroyonoko@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *    http://www.rtems.org/license
 */
#pragma GCC push_options
#pragma GCC optimize("O0")

#include <bsp/kinetis.h>

extern uint32_t system_core_clock;

static kinetis_uart_t * uart_device_table[KINETIS_UART_MAX_NUM] = {
    KINETIS_UART0,
    KINETIS_UART1,
    KINETIS_UART2,
    KINETIS_UART3,
    KINETIS_UART4,
	KINETIS_UART5
};

static int kinetis_uart_busy_flag = 0;

int kinetis_uart_is_busy(void)
{
	return kinetis_uart_busy_flag;
}

void kinetis_uart_obtain(void)
{
	kinetis_uart_busy_flag = 1;
}

void kinetis_uart_release(void)
{
	kinetis_uart_busy_flag = 0;
}

/**
 * @brief Serial drivers init function
 *
 * Initialize all serial drivers specified in driver_context_table
 *
 * @param[in] index   : which uart device we want to use
 * @retval UART_READY : Initialization completed
 */
void kinetis_uart_clock_enable(uint8_t index)
{
	switch (index) {
		case KINETIS_UART_0:
			KINETIS_SIM->scgc4 |= SIM_SCGC4_UART0_MASK;
			break;
		case KINETIS_UART_1:
			KINETIS_SIM->scgc4 |= SIM_SCGC4_UART1_MASK;
			break;
		case KINETIS_UART_2:
			KINETIS_SIM->scgc4 |= SIM_SCGC4_UART2_MASK;
			break;
		case KINETIS_UART_3:
			KINETIS_SIM->scgc4 |= SIM_SCGC4_UART3_MASK;
			break;
		case KINETIS_UART_4:
			KINETIS_SIM->scgc1 |= SIM_SCGC1_UART4_MASK;
			break;
		case KINETIS_UART_5:
			KINETIS_SIM->scgc1 |= SIM_SCGC1_UART5_MASK;
			break;
		default:
			break;
	}
}

void kinetis_uart_clock_disable(uint8_t index)
{
	switch (index) {
		case KINETIS_UART_0:
			KINETIS_SIM->scgc4 &= ~SIM_SCGC4_UART0_MASK;
			break;
		case KINETIS_UART_1:
			KINETIS_SIM->scgc4 &= ~SIM_SCGC4_UART1_MASK;
			break;
		case KINETIS_UART_2:
			KINETIS_SIM->scgc4 &= ~SIM_SCGC4_UART2_MASK;
			break;
		case KINETIS_UART_3:
			KINETIS_SIM->scgc4 &= ~SIM_SCGC4_UART3_MASK;
			break;
		case KINETIS_UART_4:
			KINETIS_SIM->scgc1 &= ~SIM_SCGC1_UART4_MASK;
			break;
		case KINETIS_UART_5:
			KINETIS_SIM->scgc1 &= ~SIM_SCGC1_UART5_MASK;
			break;
		default:
			break;
	}
}

static uint32_t kinetis_uart_clock_get(void)
{
	return (system_core_clock / (((KINETIS_SIM->clk_div1 & SIM_CLKDIV1_OUTDIV2_MASK) \
			>> SIM_CLKDIV1_OUTDIV2_SHIFT) + 1));
}

void kinetis_uart_set_bauderate (uint8_t index, uint32_t baud )
{
    uint16_t sbr,tmp,brfa;
    kinetis_uart_t *uart = uart_device_table[index];

    sbr = kinetis_uart_clock_get() / (baud * 16);
    tmp = uart->bdh & ~UART_BDH_SBR_MASK;
    uart->bdh = tmp |  UART_BDH_SBR(((sbr & 0x1F00) >> 8));

    uart->bdl = (uint8_t)(sbr & UART_BDL_SBR_MASK);

    brfa = (((kinetis_uart_clock_get() * 32)/(baud * 16)) - (sbr * 32));
    tmp = uart->ctrl4 & ~UART_C4_BRFA_MASK;
    uart->ctrl4 = tmp |  UART_C4_BRFA(brfa); 
}


int kinetis_uart_read_char(int index, char *pData)
{
    kinetis_uart_t *uart = uart_device_table[index];

    if (uart->sr1 & UART_S1_RDRF_MASK)
    {
        *pData = uart->data;
        return UART_GET_DATA;
    }
    else
    {
        return UART_NO_DATA;
    }
}

int kinetis_uart_read_char_in_polling(int index)
{
    char data;

    /* Wait recieving data from uart */
    while (UART_NO_DATA == kinetis_uart_read_char(index, &data));
    return data;
}

void kinetis_uart_write_char_in_polling(int index, char data)
{
    kinetis_uart_t *uart = uart_device_table[index];

	/* Wait for being ready to transmit*/
	while (!(uart->sr1 & UART_S1_TDRE_MASK));

	uart->data = data;

	/* Wait for completing last transmition */
	while (!(uart->sr1 & UART_S1_TC_MASK));	
}

int kinetis_uart_write_char_in_isr(int index, char data)
{
    kinetis_uart_t *uart = uart_device_table[index];

    if (uart->ctrl2 & UART_C2_TCIE_MASK) 
    {
        if (uart->sr1 & UART_S1_TDRE_MASK)
        {
            uart->data = data;
            return UART_HAVE_SENT;
        }
        else
        {
            return UART_CANT_SEND;
        }
    }
	else
	{
		return UART_NO_TX_ISR;
	}
}


/**
 * @brief Enables TX interrupt
 *
 * Disables TX interrupt source of UART peripheral specified in the driver
 * context.
 *
 * @param[in] uart controller of the driver
 * @retval Void
 */
void kinetis_uart_tx_interrupt_enable(int index)
{
	kinetis_uart_t *uart = uart_device_table[index];
	uart->ctrl2 |= UART_C2_TCIE_MASK;
}

/**
 * @brief Disables TX interrupt
 *
 * Disables TX interrupt source of UART peripheral specified in the driver
 * context.
 *
 * @param[in] uart controller of the driver
 * @retval Void
 */
void kinetis_uart_tx_interrupt_disable(int index)
{
	kinetis_uart_t *uart = uart_device_table[index];
	uart->ctrl2 &= ~UART_C2_TCIE_MASK;
}

/**
 * @brief Enables RX interrupt
 *
 * Enables RX interrupt source of uart peripheral
 * specified in the driver context.
 *
 * @param[in] uart controller of the driver
 * @retval Void
 */
void kinetis_uart_rx_interrupt_enable(int index)
{
	kinetis_uart_t *uart = uart_device_table[index];
  	uart->ctrl2 |= UART_C2_RIE_MASK;
}

/**
 * @brief Disables RX interrupt
 *
 * Disables RX interrupt source of UART peripheral specified in the driver
 * context.
 *
 * @param[in] uart controller of the driver
 * @retval Void
 */
void kinetis_uart_rx_interrupt_disable(int index)
{
	kinetis_uart_t *uart = uart_device_table[index];
  	uart->ctrl2 &= ~UART_C2_RIE_MASK;
}


void kinetis_uart_initialize(uint8_t index, uint32_t baud)
{
    kinetis_uart_t *uart = uart_device_table[index];
    
    uart->ctrl1 = 0x00;
	uart->ctrl2 = UART_C2_TE_MASK | UART_C2_RE_MASK;
    uart->ctrl3 = 0x00;
    
    kinetis_uart_set_bauderate(index, baud);
}

void kinetis_uart_pin_init(void)
{
	// port_clock_enable(PORTA);
	// port_bit_drive(PORTA, PIN_1, DS_EN, OD_DIS, PF_EN, SR_DIS, P_EN, P_UP );
	// port_bit_drive(PORTA, PIN_2, DS_EN, OD_DIS, PF_EN, SR_DIS, P_EN, P_UP );
	// port_bit_function(PORTA, PIN_1, FN_2);
	// port_bit_function(PORTA, PIN_2, FN_2);
	
	port_clock_enable(PORTC);
	port_bit_drive(PORTC, PIN_14, DS_EN, OD_DIS, PF_EN, SR_DIS, P_EN, P_UP );
	port_bit_drive(PORTC, PIN_15, DS_EN, OD_DIS, PF_EN, SR_DIS, P_EN, P_UP );
	port_bit_function(PORTC, PIN_14, FN_3);
	port_bit_function(PORTC, PIN_15, FN_3);
	
	// port_clock_enable(PORTE);
	// port_bit_drive(PORTE, PIN_4, DS_EN, OD_DIS, PF_EN, SR_DIS, P_EN, P_UP );
	// port_bit_drive(PORTE, PIN_5, DS_EN, OD_DIS, PF_EN, SR_DIS, P_EN, P_UP );
	// port_bit_function(PORTE, PIN_4, FN_3);
	// port_bit_function(PORTE, PIN_5, FN_3);
}

#pragma GCC pop_options