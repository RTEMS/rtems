/*
 * Atmel AT91RM9200 PMC functions
 *
 * Copyright (c) 2004 by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <at91rm9200.h>
#include <at91rm9200_pmc.h>

int at91rm9200_get_mainclk(void)
{
    return BSP_MAIN_FREQ;
}

int at91rm9200_get_slck(void)
{
    return BSP_SLCK_FREQ;
}

int at91rm9200_get_mck(void)
{
    uint32_t mck_reg;
    uint32_t mck_freq = 0;  /* to avoid warnings */
    uint32_t pll_reg;
    int prescaler = 0;  /* to avoid warnings */

    mck_reg = PMC_REG(PMC_MCKR);

    switch(mck_reg & PMC_MCKR_PRES_MASK) {
    case PMC_MCKR_PRES_1:
        prescaler = 1;
        break;
    case PMC_MCKR_PRES_2:
        prescaler = 2;
        break;
    case PMC_MCKR_PRES_4:
        prescaler = 4;
        break;
    case PMC_MCKR_PRES_8:
        prescaler = 8;
        break;
    case PMC_MCKR_PRES_16:
        prescaler = 16;
        break;
    case PMC_MCKR_PRES_32:
        prescaler = 32;
        break;
    case PMC_MCKR_PRES_64:
        prescaler = 64;
        break;
    }

    /* Let's find out what MCK's source is */
    switch (mck_reg & PMC_MCKR_CSS_MASK) {
    case PMC_MCKR_CSS_SLOW:
        /* I'm assuming the slow clock is 32.768 MHz */
        mck_freq = at91rm9200_get_slck() / prescaler;
        break;

    case PMC_MCKR_CSS_MAIN:
        mck_freq = at91rm9200_get_mainclk() / prescaler;
        break;

    case PMC_MCKR_CSS_PLLA:
        pll_reg = PMC_REG(PMC_PLLAR);
        mck_freq = at91rm9200_get_mainclk() / prescaler;
        mck_freq = mck_freq / (pll_reg & PMC_PLLAR_DIV_MASK);
        mck_freq = mck_freq * (((pll_reg & PMC_PLLAR_MUL_MASK) >> 16) + 1);
        break;

    case PMC_MCKR_CSS_PLLB:
        pll_reg = PMC_REG(PMC_PLLBR);
        mck_freq = at91rm9200_get_mainclk() / prescaler;
        mck_freq = mck_freq / (pll_reg & PMC_PLLBR_DIV_MASK);
        mck_freq = mck_freq * (((pll_reg & PMC_PLLBR_MUL_MASK) >> 16) + 1);
        break;
    }

    if ((mck_reg & PMC_MCKR_MDIV_MASK) == PMC_MCKR_MDIV_2) {
        mck_freq = mck_freq / 2;
    } else if ((mck_reg & PMC_MCKR_MDIV_MASK) == PMC_MCKR_MDIV_3) {
        mck_freq = mck_freq / 3;
    } else if ((mck_reg & PMC_MCKR_MDIV_MASK) == PMC_MCKR_MDIV_4) {
        mck_freq = mck_freq / 4;
    }


    return mck_freq;
}

