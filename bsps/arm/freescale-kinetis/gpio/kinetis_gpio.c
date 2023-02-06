/**************************************************************************************************
*			file: port.c
*			
*			date: 2013.2.28 Manley
**************************************************************************************************/
#include <bsp/kinetis.h>

unsigned char port_clock_enable(kinetis_port_t *port)
{
	switch ((unsigned int)port) {
		case (unsigned int)PORTA:
			KINETIS_SIM->scgc5 |= SIM_SCGC5_PORTA_MASK;
			break;
		case (unsigned int)PORTB:
			KINETIS_SIM->scgc5 |= SIM_SCGC5_PORTB_MASK;
			break;
		case (unsigned int)PORTC:
			KINETIS_SIM->scgc5 |= SIM_SCGC5_PORTC_MASK;
			break;
		case (unsigned int)PORTD:
			KINETIS_SIM->scgc5 |= SIM_SCGC5_PORTD_MASK;
			break;
		case (unsigned int)PORTE:
			KINETIS_SIM->scgc5 |= SIM_SCGC5_PORTE_MASK;
			break;
		default:
			break;
	}
	return 1;
}

unsigned char port_clock_disable(kinetis_port_t *port)
{
	switch ((unsigned int)port) {
		case (unsigned int)PORTA:
			KINETIS_SIM->scgc5 &= ~SIM_SCGC5_PORTA_MASK;
			break;
		case (unsigned int)PORTB:
			KINETIS_SIM->scgc5 &= ~SIM_SCGC5_PORTB_MASK;
			break;
		case (unsigned int)PORTC:
			KINETIS_SIM->scgc5 &= ~SIM_SCGC5_PORTC_MASK;
			break;
		case (unsigned int)PORTD:
			KINETIS_SIM->scgc5 &= ~SIM_SCGC5_PORTD_MASK;
			break;
		case (unsigned int)PORTE:
			KINETIS_SIM->scgc5 &= ~SIM_SCGC5_PORTE_MASK;
			break;
		default:
			break;
	}
	return 1;
}

//mode	0000 Interrupt/DMA Request disabled
//		0001 DMA Request on rising edge
//		0010 DMA Request on falling edge
//		0011 DMA Request on either edge
//		0100 Reserved
//		1000 Interrupt when logic zero
//		1001 Interrupt on rising edge
//		1010 Interrupt on falling edge
//		1011 Interrupt on either edge
//		1100 Interrupt when logic one
unsigned char port_init_config(kinetis_port_t *port, unsigned int pin_n, unsigned int mode_n)
{
	port->pcr[pin_n] &= ~PORT_PCR_IRQC_MASK;
	port->pcr[pin_n] |= PORT_PCR_IRQC(mode_n);
	return 1;
}

unsigned char port_bit_lock(kinetis_port_t *port, unsigned int pin_n)
{
	port->pcr[pin_n] |= PORT_PCR_LK_MASK;
	return 1;
}

unsigned char port_bit_function(kinetis_port_t *port, unsigned int pin_n, unsigned int fn_n)
{
	port->pcr[pin_n] &= ~PORT_PCR_MUX_MASK;
	port->pcr[pin_n] |= PORT_PCR_MUX(fn_n);
	return 1;
}

//dse: drive strength enable			ode: open drain enable
//pfe: passive filter enable			sre: slew rate enable
//pe : pull enable						ps : pull select(0:pull-down 1:pull-up)
unsigned char port_bit_drive(kinetis_port_t *port, unsigned int pin_n, unsigned int dse, unsigned int ode,
					unsigned int pfe, unsigned int sre, unsigned int pe, unsigned int ps)
{
	unsigned int tmp = 0;
	
	port->pcr[pin_n] &= ~0xFF;
	
	if (dse != 0) {tmp |= PORT_PCR_DSE_MASK;}
	if (ode != 0) {tmp |= PORT_PCR_ODE_MASK;}
	if (pfe != 0) {tmp |= PORT_PCR_PFE_MASK;}
	if (sre != 0) {tmp |= PORT_PCR_SRE_MASK;}
	if (pe  != 0) {tmp |= PORT_PCR_PE_MASK;}
	if (ps  != 0) {tmp |= PORT_PCR_PS_MASK;}
	
	port->pcr[pin_n] |= tmp;
	return 1;
}

unsigned char port_global_lock(kinetis_port_t *port)
{
	port->gpclr = PORT_PCR_LK_MASK|
				PORT_PCR_LK_MASK<<16;
	port->gpchr = PORT_PCR_LK_MASK|
				PORT_PCR_LK_MASK<<16;
	return 1;
}

unsigned char port_global_function(kinetis_port_t *port, unsigned int fn_n)
{
	port->gpclr = PORT_PCR_MUX(fn_n)|
				PORT_PCR_MUX_MASK<<16;
	port->gpchr = PORT_PCR_MUX(fn_n)|
				PORT_PCR_MUX_MASK<<16;
	return 1;
}

unsigned char port_global_drive(kinetis_port_t *port, unsigned int dse, unsigned int ode,
					unsigned int pfe, unsigned int sre, unsigned int pe, unsigned int ps)
{
	unsigned int tmp = 0;	
	if (dse != 0) {tmp |= PORT_PCR_DSE_MASK;}
	if (ode != 0) {tmp |= PORT_PCR_ODE_MASK;}
	if (pfe != 0) {tmp |= PORT_PCR_PFE_MASK;}
	if (sre != 0) {tmp |= PORT_PCR_SRE_MASK;}
	if (pe  != 0) {tmp |= PORT_PCR_PE_MASK;}
	if (ps  != 0) {tmp |= PORT_PCR_PS_MASK;}

	port->gpclr = tmp | 0xFF<<16;
	port->gpchr = tmp | 0xFF<<16;
	return 1;
}

unsigned char port_bit_filter_enable(kinetis_port_t *port, unsigned int bit_n)
{
	port->dfer |= 1<<bit_n;
	return 1;
}

unsigned char port_bit_filter_disable(kinetis_port_t *port, unsigned int bit_n)
{
	port->dfer &= ~(1<<bit_n);
	return 1;
}

unsigned char port_global_filter_enable(kinetis_port_t *port)
{
	port->dfer = (unsigned int)0xFFFFFFFF;
	return 1;
}

unsigned char port_global_filter_disable(kinetis_port_t *port)
{
	port->dfer = (unsigned int)0x0;
	return 1;
}

//port : port register base address
//cs : clock source						flen: filter length
unsigned char port_filter_config(kinetis_port_t *port, unsigned int cs, unsigned int flen)
{
	(cs == 0) ? (port->dfcr = (unsigned int)0x0) : (port->dfcr = (unsigned int)0x1);
	port->dfwr = PORT_DFWR_FILT(flen);
	return 1;
}
