/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp/atsam-i2c.h>
#include <bsp/i2c.h>

/** TWI0 data pin */
#define PIN_TWI_TWD0 {PIO_PA3A_TWD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** TWI0 clock pin */
#define PIN_TWI_TWCK0 {PIO_PA4A_TWCK0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

/** TWI1 data pin */
#define PIN_TWI_TWD1 {PIO_PB4A_TWD1, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/** TWI1 clock pin */
#define PIN_TWI_TWCK1 {PIO_PB5A_TWCK1, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}

/** TWI2 data pin */
#define PIN_TWI_TWD2 {PIO_PD27C_TWD2, PIOD, ID_PIOD, PIO_PERIPH_C, PIO_DEFAULT}
/** TWI2 clock pin */
#define PIN_TWI_TWCK2 {PIO_PD28C_TWCK2, PIOD, ID_PIOD, PIO_PERIPH_C, PIO_DEFAULT}

int atsam_register_i2c_0(void)
{
	static const Pin pins[] = {PIN_TWI_TWD0, PIN_TWI_TWCK0};

	return i2c_bus_register_atsam(
	    ATSAM_I2C_0_BUS_PATH,
	    TWIHS0,
	    ID_TWIHS0,
	    pins);
}

int atsam_register_i2c_1(void)
{
	static const Pin pins[] = {PIN_TWI_TWD1, PIN_TWI_TWCK1};

	return i2c_bus_register_atsam(
	    ATSAM_I2C_1_BUS_PATH,
	    TWIHS1,
	    ID_TWIHS1,
	    pins);
}

int atsam_register_i2c_2(void)
{
	static const Pin pins[] = {PIN_TWI_TWD2, PIN_TWI_TWCK2};

	return i2c_bus_register_atsam(
	    ATSAM_I2C_2_BUS_PATH,
	    TWIHS2,
	    ID_TWIHS2,
	    pins);
}
