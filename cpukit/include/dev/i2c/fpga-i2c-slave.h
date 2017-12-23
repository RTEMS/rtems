/*
 * Copyright (c) 2016-2017 Chris Johns <chrisj@rtems.org>
 * All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

/*
 * I2C slave for testing:
 *       https://github.com/oetr/FPGA-I2C-Slave
 */

#ifndef FPGA_I2C_SLAVE_H
#define FPGA_I2C_SLAVE_H

#include <dev/i2c/i2c.h>

int i2c_dev_register_fpga_i2c_slave(const char* bus_path,
                                    const char* dev_path,
                                    uint16_t   address,
                                    size_t     size);

#endif
