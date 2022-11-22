/**
 * @file flexbus.c
 *
 * @ingroup kinetis_flexbus
 *
 * @brief FreeScale Kinetis Flexbus interface definitions.
 */

#include <bsp/flexbus.h>
#include <bsp/sim.h>
#include <bsp/port.h>

typedef struct {
	unsigned int address;
	unsigned int mask;
	unsigned int control;
	unsigned int signal;
}flexbus_configure_t;

static void flexbus_chip_select_configure(unsigned int index, flexbus_configure_t *config);
static void flexbus_port_multiplexing_control(flexbus_configure_t *config);
static void flexbus_clock_divider(unsigned int scale);

#define TFT_LCD_ADDRESS   CS0_BASE
#define TFT_LCD_CONTROL ( /* 16-bit port	*/   													\
						  FB_CSCR_PS(3)		|    													\
						  					     													\
						  /* auto-acknowledge */ 													\
					      FB_CSCR_AA_MASK	|														\
																									\
						  /* right aligned */ 														\
					      FB_CSCR_BLS_MASK	|														\
						  																			\
						  /* be signal is asserted for rw acess */ 									\
					      FB_CSCR_BEM_MASK	|														\
						  																			\
						  /* assert chip select on second clock edge after address is asserted */	\
					      FB_CSCR_ASET(1)	|														\
						  																			\
					      FB_CSCR_EXTS_MASK	|														\
					      FB_CSCR_WRAH(0)	|														\
					      FB_CSCR_RDAH(0)	|														\
						  																			\
						  /* 1 wait state - may need a wait state depending on the bus speed */		\
					      FB_CSCR_WS(1)																)
#define TFT_LCD_MASK      FB_CSMR_V_MASK //Enable cs signal


#define PSRAM_ADDRESS	CS2_BASE
#define PSRAM_CONTROL 	( /* 16-bit port	*/   													\
						  FB_CSCR_PS(3)		|    													\
						  					     													\
						  /* auto-acknowledge */ 													\
					      FB_CSCR_AA_MASK	|														\
																									\
						  /* right aligned */ 														\
					      FB_CSCR_BLS_MASK	|														\
						  																			\
						  /* assert chip select on second clock edge after address is asserted */	\
					      FB_CSCR_ASET(1)	|														\
						  																			\
					      FB_CSCR_WRAH(5)	|														\
					      FB_CSCR_RDAH(5)	|														\
						  																			\
						  /* 1 wait state - may need a wait state depending on the bus speed */		\
					      FB_CSCR_WS(1)																)

#define PSRAM_MASK	(	/* Enable cs signal */		\
						FB_CSMR_V_MASK		|		\
													\
						FB_CSMR_BAM(0x007F)			)

#define FLEXBUS_PORT_MULTIPLEXING ( /* FB_BE_7_0 signal */		\
									FB_CSPMCR_GROUP5(2)	|		\
																\
				        			/* FB_CS2 signal /*/		\
									FB_CSPMCR_GROUP4(1)	|		\
																\
				        			/* FB_BE_23_16 signal */	\
									FB_CSPMCR_GROUP3(2)	|		\
																\
				        			/* FB_BE_31_24 */			\
									FB_CSPMCR_GROUP2(2)	|		\
																\
				        			/* FB_ALE signal */			\
									FB_CSPMCR_GROUP1(0)			)



void flexbus_gpio_init(void)
{
	port_clock_enable ( PORTA );
	port_clock_enable ( PORTB );
	port_clock_enable ( PORTC );
	port_clock_enable ( PORTD );

	/* Configure the pins needed to FlexBus Function (Alt 5) */
	/* this example uses low drive strength settings         */

	//address/data
	port_bit_function ( PORTB, PIN_9, FN_5);				// fb_ad[20]
	port_bit_function ( PORTB,PIN_10, FN_5);				// fb_ad[19]
	port_bit_function ( PORTB,PIN_11, FN_5);				// fb_ad[18]
	port_bit_function ( PORTB,PIN_16, FN_5);				// fb_ad[17]
	port_bit_function ( PORTB,PIN_17, FN_5);				// fb_ad[16]
	port_bit_function ( PORTB,PIN_18, FN_5);				// fb_ad[15]
	port_bit_function ( PORTC, PIN_0, FN_5);				// fb_ad[14]
	port_bit_function ( PORTC, PIN_1, FN_5);				// fb_ad[13]
	port_bit_function ( PORTC, PIN_2, FN_5);				// fb_ad[12]
	port_bit_function ( PORTC, PIN_4, FN_5);				// fb_ad[11]
	port_bit_function ( PORTC, PIN_5, FN_5);				// fb_ad[10]
	port_bit_function ( PORTC, PIN_6, FN_5);				// fb_ad[9]
	port_bit_function ( PORTC, PIN_7, FN_5);				// fb_ad[8]
	port_bit_function ( PORTC, PIN_8, FN_5);				// fb_ad[7]
	port_bit_function ( PORTC, PIN_9, FN_5);				// fb_ad[6]
	port_bit_function ( PORTC,PIN_10, FN_5);				// fb_ad[5]
	port_bit_function ( PORTD, PIN_2, FN_5);				// fb_ad[4]
	port_bit_function ( PORTD, PIN_3, FN_5);				// fb_ad[3]
	port_bit_function ( PORTD, PIN_4, FN_5);				// fb_ad[2]
	port_bit_function ( PORTD, PIN_5, FN_5);				// fb_ad[1]
	port_bit_function ( PORTD, PIN_6, FN_5);				// fb_ad[0]
	//control signals
// 	port_bit_function ( PORTC,PIN_16, FN_5);				// fb_be[15:8]
//	port_bit_function ( PORTC,PIN_17, FN_5);				// fb_be[ 7:0]
	
	port_bit_function ( PORTB,PIN_19, FN_5);				// fb_oe_b
	port_bit_function ( PORTC,PIN_11, FN_5);				// fb_rw_b
	port_bit_function ( PORTD, PIN_0, FN_5);				// fb_ale
//	port_bit_function ( PORTC, PIN_3, FN_5);				// fb_clk_out
	
	port_bit_function ( PORTD, PIN_1, FN_5);				// fb_cs0_b
 	port_bit_function ( PORTC,PIN_18, FN_5);				// fb_cs2_b
}

bool flexbus_clock_enable (void)
{
	KINETIS_SIM->scgc7 |= SIM_SCGC7_FLEXBUS_MASK;
	return true;
}

bool flexbus_clock_disable (void)
{
	KINETIS_SIM->scgc7 &= ~SIM_SCGC7_FLEXBUS_MASK;
	return true;
}

static void flexbus_clock_divider(unsigned int scale)
{
	KINETIS_SIM->clk_div1 |= SIM_CLKDIV1_OUTDIV3(scale);
}

static void flexbus_port_multiplexing_control(flexbus_configure_t *cfg)
{
    KINETIS_FB->cspmcr = cfg->signal;
}

static void flexbus_chip_select_configure(unsigned int index, flexbus_configure_t *cfg)
{
    KINETIS_FB->cs[index].csar = cfg->address;
	KINETIS_FB->cs[index].cscr = cfg->control;
	KINETIS_FB->cs[index].csmr = cfg->mask;
}

bool flexbus_init ( unsigned int div_n )
{
	//fb clock divider 3
	flexbus_clock_divider(div_n);
	
	flexbus_configure_t config;
	config.signal = FLEXBUS_PORT_MULTIPLEXING;
	flexbus_port_multiplexing_control(&config);
	
	//Chip select for TFT LCD
	config.address = TFT_LCD_ADDRESS;          
	config.control = TFT_LCD_CONTROL;
	config.mask = TFT_LCD_MASK;    
	flexbus_chip_select_configure(0, &config);

    //Chip select for for PSRAM
	config.address = PSRAM_ADDRESS;
	config.control = PSRAM_CONTROL;
	config.mask = PSRAM_MASK;
	flexbus_chip_select_configure(2, &config);

	return true;
}