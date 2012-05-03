/*
 *  Helper functions for SMSC LAN91C11x
 *
 *  Copyright (c) 2004 by Cogent Computer Systems
 *  Written by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
#include <rtems.h>
#include "lan91c11x.h"

uint16_t lan91c11x_read_reg(int reg)
{
    volatile uint16_t *ptr = (uint16_t *)LAN91C11X_BASE_ADDR;
    uint16_t old_bank;
    uint16_t val;
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);

    /* save the bank register */
    old_bank = ptr[7] & 0x7;

    /* set the bank register */
    ptr[7] = (reg >> 4) & 0x7;

    val = ptr[((reg & 0xf) >> 1)];

    /* restore the bank register */
    ptr[7] = old_bank;

    rtems_interrupt_enable(level);
    return val;
}

void lan91c11x_write_reg(int reg, uint16_t value)
{
    volatile uint16_t *ptr = (uint16_t *)LAN91C11X_BASE_ADDR;
    uint16_t old_bank;
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);

    /* save the bank register */
    old_bank = ptr[7] & 0x7;

    /* set the bank register */
    ptr[7] = (reg >> 4) & 0x7;

    ptr[((reg & 0xf) >> 1)] = value;

    /* restore the bank register */
    ptr[7] = old_bank;

    rtems_interrupt_enable(level);
}

uint16_t lan91c11x_read_reg_fast(int reg)
{
    volatile uint16_t *ptr = (uint16_t *)LAN91C11X_BASE_ADDR;
    uint16_t val;

    val = ptr[((reg & 0xf) >> 1)];

    return val;
}

void lan91c11x_write_reg_fast(int reg, uint16_t value)
{
    volatile uint16_t *ptr = (uint16_t *)LAN91C11X_BASE_ADDR;

    ptr[((reg & 0xf) >> 1)] = value;
}


uint16_t lan91c11x_read_phy_reg(int reg)
{
    int i;
    uint16_t mask;
    uint16_t bits[64];
    int clk_idx = 0;
    int input_idx = 0;
    uint16_t phydata;

    /* 32 consecutive ones on MDO to establish sync */
    for (i = 0; i < 32; ++i) {
        bits[clk_idx++] = LAN91C11X_MGMT_MDOE | LAN91C11X_MGMT_MDO;
    }

    /* Start code <01> */
    bits[clk_idx++] = LAN91C11X_MGMT_MDOE;
    bits[clk_idx++] = LAN91C11X_MGMT_MDOE | LAN91C11X_MGMT_MDO;

    /* Read command <10> */
    bits[clk_idx++] = LAN91C11X_MGMT_MDOE | LAN91C11X_MGMT_MDO;
    bits[clk_idx++] = LAN91C11X_MGMT_MDOE;

    /* Output the PHY address, msb first - Internal PHY is address 0 */
    for (i = 0; i < 5; ++i) {
        bits[clk_idx++] = LAN91C11X_MGMT_MDOE;
    }

    /* Output the phy register number, msb first */
    mask = 0x10;
    for (i = 0; i < 5; ++i) {
        if (reg & mask) {
            bits[clk_idx++] = LAN91C11X_MGMT_MDOE | LAN91C11X_MGMT_MDO;
        } else {
            bits[clk_idx++] = LAN91C11X_MGMT_MDOE;
        }


        /* Shift to next lowest bit */
        mask >>= 1;
    }

    /* 1 bit time for turnaround */
    bits[clk_idx++] = 0;

    /* Input starts at this bit time */
    input_idx = clk_idx;

    /* Will input 16 bits */
    for (i = 0; i < 16; ++i) {
        bits[clk_idx++] = 0;
    }

    /* Final clock bit */
    bits[clk_idx++] = 0;

    /* Turn off all MII Interface bits */
    lan91c11x_write_reg(LAN91C11X_MGMT,
                        lan91c11x_read_reg(LAN91C11X_MGMT) & 0xfff0);

    /* Clock all 64 cycles */
    for (i = 0; i < sizeof bits; ++i) {
        /* Clock Low - output data */
        lan91c11x_write_reg(LAN91C11X_MGMT, bits[i]);
        rtems_task_wake_after(1);

        /* Clock Hi - input data */
        lan91c11x_write_reg(LAN91C11X_MGMT, bits[i] | LAN91C11X_MGMT_MCLK);
        rtems_task_wake_after(1);
        bits[i] |= lan91c11x_read_reg(LAN91C11X_MGMT) & LAN91C11X_MGMT_MDI;
    }

    /* Return to idle state */
    /* Set clock to low, data to low, and output tristated */
    lan91c11x_write_reg(LAN91C11X_MGMT, lan91c11x_read_reg(LAN91C11X_MGMT) & 0xfff0);
    rtems_task_wake_after(1);

    /* Recover input data */
    phydata = 0;
    for (i = 0; i < 16; ++i) {
        phydata <<= 1;

        if (bits[input_idx++] & LAN91C11X_MGMT_MDI) {
            phydata |= 0x0001;
        }
    }

    return phydata;
}



void lan91c11x_write_phy_reg(int reg, uint16_t phydata)
{
    int i;
    ushort mask;
    ushort bits[64];
    int clk_idx = 0;

    /* 32 consecutive ones on MDO to establish sync */
    for (i = 0; i < 32; ++i) {
        bits[clk_idx++] = LAN91C11X_MGMT_MDOE | LAN91C11X_MGMT_MDO;
    }

    /* Start code <01> */
    bits[clk_idx++] = LAN91C11X_MGMT_MDOE;
    bits[clk_idx++] = LAN91C11X_MGMT_MDOE | LAN91C11X_MGMT_MDO;

    /* Write command <01> */
    bits[clk_idx++] = LAN91C11X_MGMT_MDOE;
    bits[clk_idx++] = LAN91C11X_MGMT_MDOE | LAN91C11X_MGMT_MDO;

    /* Output the PHY address, msb first - Internal PHY is address 0 */
    for (i = 0; i < 5; ++i) {
        bits[clk_idx++] = LAN91C11X_MGMT_MDOE;
    }

    /* Output the phy register number, msb first */
    mask = 0x10;
    for (i = 0; i < 5; ++i) {
        if (reg & mask) {
            bits[clk_idx++] = LAN91C11X_MGMT_MDOE | LAN91C11X_MGMT_MDO;
        } else {
            bits[clk_idx++] = LAN91C11X_MGMT_MDOE;
        }

        /* Shift to next lowest bit */
        mask >>= 1;
    }

    /* 2 extra bit times for turnaround */
    bits[clk_idx++] = 0;
    bits[clk_idx++] = 0;

    /* Write out 16 bits of data, msb first */
    mask = 0x8000;
    for (i = 0; i < 16; ++i) {
        if (phydata & mask) {
            bits[clk_idx++] = LAN91C11X_MGMT_MDOE | LAN91C11X_MGMT_MDO;
        } else {
            bits[clk_idx++] = LAN91C11X_MGMT_MDOE;
        }

        /* Shift to next lowest bit */
        mask >>= 1;
    }

    /* Turn off all MII Interface bits */
    lan91c11x_write_reg(LAN91C11X_MGMT,
                        lan91c11x_read_reg(LAN91C11X_MGMT) & 0xfff0);

    /* Clock all 64 cycles */
    for (i = 0; i < sizeof bits; ++i) {
        /* Clock Low - output data */
        lan91c11x_write_reg(LAN91C11X_MGMT, bits[i]);
        rtems_task_wake_after(1);

        /* Clock Hi - input data */
        lan91c11x_write_reg(LAN91C11X_MGMT, bits[i] | LAN91C11X_MGMT_MCLK);
        rtems_task_wake_after(1);
        bits[i] |= lan91c11x_read_reg(LAN91C11X_MGMT) & LAN91C11X_MGMT_MDI;
    }

    /* Return to idle state */
    /* Set clock to low, data to low, and output tristated */
    lan91c11x_write_reg(LAN91C11X_MGMT,
                        lan91c11x_read_reg(LAN91C11X_MGMT) & 0xfff0);
    rtems_task_wake_after(1);

}



