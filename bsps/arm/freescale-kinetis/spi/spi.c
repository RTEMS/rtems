#include <bsp/spi.h>
#include <bsp/sim.h>
#include <bsp/port.h>
#include <bsp/pinout.h>


typedef struct
{
    const char *path;
	unsigned int speed;
}kinetis_spi_bus_config_t;

extern unsigned int system_core_clock;

// static void spi0_irq_handler(void);
// static void spi1_irq_handler(void);
// static void spi2_irq_handler(void);

int spi_clock_enable ( kinetis_spi_t *SPIx )
{
	switch ((unsigned int)SPIx) {
		case (unsigned int)SPI0:
			KINETIS_SIM->scgc6 |= SIM_SCGC6_SPI0_MASK;
			break;
		case (unsigned int)SPI1:
			KINETIS_SIM->scgc6 |= SIM_SCGC6_SPI1_MASK;
			break;
//		case (unsigned int)SPI2:
//			KINETIS_SIM->SCGC3 |= SIM_SCGC3_SPI2_MASK;
//			break;
		default:
			;
	}
	
	return (1);
}

int spi_clock_disable ( kinetis_spi_t *SPIx )
{
	switch ((unsigned int)SPIx) {
		case (unsigned int)SPI0:
			KINETIS_SIM->scgc6 &= ~SIM_SCGC6_SPI0_MASK;
			break;
		case (unsigned int)SPI1:
			KINETIS_SIM->scgc6 &= ~SIM_SCGC6_SPI1_MASK;
			break;
//		case (unsigned int)SPI2:
//			KINETIS_SIM->scgc3 &= ~SIM_SCGC3_SPI2_MASK;
//			break;
		default:
			;
	}
	
	return (1);
}

int spi_baudrate_evaluation(unsigned int baud, unsigned int *preal_baud, unsigned char *pctar_br)
{
	unsigned int scaler;
	unsigned int sck;
	
	if (!pctar_br)
	{
		return -1;
	}

	*pctar_br = 0;

	do
	{
		// BR <= 3  ---- Scaler = 2 x (BR+1)
	    // BR >= 4  ---- Scaler = 2^BR , max is 32768
		if ((*pctar_br) <= 3)
		{
			scaler = 2 * ((*pctar_br) + 1);
		}
		else
		{
            scaler = 1 << (*pctar_br);
		}
	    // PBR = 0  ---- Prescaler = 2
	    // DBR = 0  ---- Ratio = 50/50 = 1
		// SCK_baud_rate = (f_SYS / Prescaler) x [(1+Ratio)/Scaler]
		sck = system_core_clock / scaler;
		if (sck <= baud)
			break;
        else
	        *pctar_br = (*pctar_br) + 1;
	} while (scaler <= 32768);
	
    if (preal_baud)
	    *preal_baud = sck;

	return 0;
}

int spi_baudrate_config(kinetis_spi_t *SPIx, unsigned int baud)
{
    SPIx->ctar[0] &= ~(SPI_CTAR_BR_MASK);
	unsigned char ctar_br = 7;
	spi_baudrate_evaluation(baud, (unsigned int *)0, &ctar_br);
	SPIx->ctar[0] |= SPI_CTAR_BR(ctar_br);

	return 0;
}

int spi_init ( kinetis_spi_t *SPIx, unsigned int baud )
{
	SPIx->mcr	 = (SPI_MCR_CLR_RXF_MASK|
					SPI_MCR_CLR_TXF_MASK|
					SPI_MCR_PCSIS_MASK	|
					SPI_MCR_HALT_MASK	);	
	SPIx->mcr	|= 	SPI_MCR_MSTR_MASK;

    unsigned int sck;
	unsigned char ctar_br = 7;
	spi_baudrate_evaluation(baud, &sck, &ctar_br);
	
	SPIx->ctar[0] = (SPI_CTAR_BR(ctar_br)	|
					 SPI_CTAR_FMSZ(7)		|
					 SPI_CTAR_CSSCK(4)		|
					 SPI_CTAR_ASC(4)		|
					 SPI_CTAR_DT(6)			|
					 SPI_CTAR_PDT_MASK		
// 					SPI_CTAR_CPOL_MASK	|
// 					SPI_CTAR_CPHA_MASK	
					);
						
	SPIx->sr	|= (SPI_SR_EOQF_MASK	|
					SPI_SR_TFFF_MASK	|
					SPI_SR_TFUF_MASK	|
					SPI_SR_RFDF_MASK	|
					SPI_SR_RFOF_MASK	);

	SPIx->mcr	&=  ~SPI_MCR_HALT_MASK;
	return (1);
}

unsigned char spi_send_receive ( kinetis_spi_t *SPIx, unsigned char tx_dat )
{
	SPIx->sr	 = (SPI_SR_EOQF_MASK	|
				SPI_SR_TFFF_MASK	|
				SPI_SR_TFUF_MASK	|
				SPI_SR_RFDF_MASK	|
				SPI_SR_RFOF_MASK	);
	
	SPIx->mcr |= (SPI_MCR_CLR_RXF_MASK|
				SPI_MCR_CLR_TXF_MASK);	
		
	SPIx->pushr = (SPI_PUSHR_CTAS(0)	|
				SPI_PUSHR_EOQ_MASK	|
				SPI_PUSHR_PCS(1)	|	/*SPI Flash cs Conflict with lcd_en(pcs1) spiflash need comment this statement*/
				SPI_PUSHR_TXDATA(tx_dat));
		
 	while((SPIx->sr & SPI_SR_TCF_MASK) == 0);
	SPIx->sr |= SPI_SR_TCF_MASK;
	
	return (SPIx->popr);
}

unsigned int spi_send ( kinetis_spi_t *SPIx, unsigned char *buff, unsigned int len )
{
	unsigned int cnt = len;
	while(cnt-- != 0) 
		spi_send_receive ( SPIx, *buff++);
	return (len);
}

unsigned int spi_receive ( kinetis_spi_t *SPIx, unsigned char *buff, unsigned int len )
{
	unsigned int cnt = len;
	while(cnt-- != 0)
		*buff++ = spi_send_receive ( SPIx, 0xFF);
	return (len);
}

// void spi0_irq_handler(void)
// {
// 	;
// }

// void spi1_irq_handler(void)
// {
// 	;
// }

// void spi2_irq_handler(void)
// {
// 	;
// }

void spi_gpio_init(void)
{
	port_clock_enable ( PORTA );
	port_clock_enable ( PORTB );
	port_clock_enable ( PORTE );
	
	/* ---------------SPI0--------------- */
	port_bit_function ( PORTB, PIN_23, FN_1 );		// 25Q80_CS(SPI0_PCS5 FN3)
	port_bit_function ( PORTA, PIN_14, FN_1 );		// LCD_CS(SPI0_PCS0 FN2)
	port_bit_function ( PORTB, PIN_20, FN_1 );		// LCD_CD
	port_bit_function ( PORTA, PIN_15, FN_2 );		// SPI0_SCK
	port_bit_function ( PORTA, PIN_16, FN_2 );		// SPI0_SIN
	port_bit_function ( PORTA, PIN_17, FN_2 );		// SPI0_SOUT
	
	port_bit_drive ( PORTB, PIN_23, DS_EN, OD_DIS, PF_DIS, SR_EN, P_EN, P_UP );
	gpio_bit_direction ( PTB, IO_23, OUT );
	gpio_set_bit ( PTB, IO_23 );
	
	port_bit_drive ( PORTA, PIN_14, DS_EN, OD_DIS, PF_DIS, SR_EN, P_EN, P_UP );
	gpio_bit_direction ( PTA, IO_14, OUT );
	gpio_set_bit ( PTA, IO_14 );
	
	port_bit_drive ( PORTB, PIN_20, DS_EN, OD_DIS, PF_DIS, SR_EN, P_EN, P_UP );
	gpio_bit_direction ( PTB, IO_20, OUT );
	gpio_set_bit ( PTB, IO_20 );
	/* ----------------END---------------*/

    /* ---------------SPI1--------------- */
	port_bit_function ( PORTE, PIN_4, FN_1 );		// PTE4 SPI1_PCS0 FN2 = JP2-2 = JP2-3 = SDCARD_CN8-1(nCS/DAT3)
	port_bit_function ( PORTE, PIN_2, FN_2 );		// PTE2 SPI1_SCK = SD_CLK = SDCARD_CN8-5(SCLK/CLK)
	port_bit_function ( PORTE, PIN_3, FN_2 );		// PTE3 SPI1_SIN = JP6-2 = JP6-1 = SDCARD_CN8-3(MOSI/CMD)
	port_bit_function ( PORTE, PIN_1, FN_2 );		// PTE1 SPI1_SOUT = JP7-2 = JP7-1 = JP6-3 = SD_DAT0 = SDCARD_CN8-7(MISO/DAT0)

    port_bit_drive ( PORTE, PIN_4, DS_EN, OD_DIS, PF_DIS, SR_EN, P_EN, P_UP );
	gpio_bit_direction ( PTE, IO_4, OUT );
	gpio_set_bit ( PTE, IO_4 );
	/* ----------------END---------------*/
}