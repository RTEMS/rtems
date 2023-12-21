/* SPDX-License-Identifier: BSD-3-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This source file contains errata SSWF021#45 workaround
 *   implementation.
 */

/*
 * Copyright (C) 2009-2018 Texas Instruments Incorporated - www.ti.com
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <bsp/ti_herc/errata_SSWF021_45.h>
#include <bsp/tms570.h>

#define SYS_CLKSRC_PLL1 0x00000002U
#define SYS_CLKSRC_PLL2 0x00000040U
#define SYS_CLKCNTRL_PENA 0x00000100U
#define ESM_SR1_PLL1SLIP 0x400U
#define ESM_SR4_PLL2SLIP 0x400U
#define PLL1 0x08
#define PLL2 0x80
#define dcc1CNT1_CLKSRC_PLL1 0x0000A000U
#define dcc1CNT1_CLKSRC_PLL2 0x0000A001U

static uint32_t check_frequency(uint32_t cnt1_clksrc);
static uint32_t disable_plls(uint32_t plls);

/** @fn uint32_t _errata_SSWF021_45_both_plls(uint32_t count)
 *   @brief This handles the errata for PLL1 and PLL2. This function is called
 *      in device startup
 *
 *   @param[in] count : Number of retries until both PLLs are locked
 *      successfully Minimum value recommended is 5
 *
 *   @return 0 = Success (the PLL or both PLLs have successfully locked and then
 *               been disabled)
 *           1 = PLL1 failed to successfully lock in "count" tries
 *           2 = PLL2 failed to successfully lock in "count" tries
 *           3 = Neither PLL1 nor PLL2 successfully locked in "count" tries
 *           4 = The workaround function was not able to disable at least one of
 *               the PLLs. The most likely reason is that a PLL is already being
 *               used as a clock source. This can be caused by the workaround
 *               function being called from the wrong place in the code.
 */
uint32_t _errata_SSWF021_45_both_plls(uint32_t count) {
    uint32_t failCode, retries, clkCntlSav;

    /* save CLKCNTL */
    clkCntlSav = TMS570_SYS1.CLKCNTL;
    /* First set VCLK2 = HCLK */
    TMS570_SYS1.CLKCNTL = clkCntlSav & 0x000F0100U;
    /* Now set VCLK = HCLK and enable peripherals */
    TMS570_SYS1.CLKCNTL = SYS_CLKCNTRL_PENA;
    failCode = 0U;
    for (retries = 0U; (retries < count); retries++) {
        failCode = 0U;
        /* Disable PLL1 and PLL2 */
        failCode = disable_plls(SYS_CLKSRC_PLL1 | SYS_CLKSRC_PLL2);
        if (failCode != 0U) {
            break;
        }

        /* Clear Global Status Register */
        TMS570_SYS1.GLBSTAT = 0x00000301U;
        /* Clear the ESM PLL slip flags */
        TMS570_ESM.SR[0U] = ESM_SR1_PLL1SLIP;
        TMS570_ESM.SR4 = ESM_SR4_PLL2SLIP;
        /* set both PLLs to OSCIN/1*27/(2*1) */
        TMS570_SYS1.PLLCTL1 = 0x20001A00U;
        TMS570_SYS1.PLLCTL2 = 0x3FC0723DU;
        TMS570_SYS2.PLLCTL3 = 0x20001A00U;
        TMS570_SYS1.CSDISCLR = SYS_CLKSRC_PLL1 | SYS_CLKSRC_PLL2;
        /* Check for (PLL1 valid or PLL1 slip) and (PLL2 valid or PLL2 slip) */
        while ((((TMS570_SYS1.CSVSTAT & SYS_CLKSRC_PLL1) == 0U) &&
                ((TMS570_ESM.SR[0U] & ESM_SR1_PLL1SLIP) == 0U)) ||
               (((TMS570_SYS1.CSVSTAT & SYS_CLKSRC_PLL2) == 0U) &&
                ((TMS570_ESM.SR4 & ESM_SR4_PLL2SLIP) == 0U))) {
            /* Wait */
        }
        /* If PLL1 valid, check the frequency */
        if (((TMS570_ESM.SR[0U] & ESM_SR1_PLL1SLIP) != 0U) ||
            ((TMS570_SYS1.GLBSTAT & 0x00000300U) != 0U)) {
            failCode |= 1U;
        } else {
            failCode |= check_frequency(dcc1CNT1_CLKSRC_PLL1);
        }
        /* If PLL2 valid, check the frequency */
        if (((TMS570_ESM.SR4 & ESM_SR4_PLL2SLIP) != 0U) ||
            ((TMS570_SYS1.GLBSTAT & 0x00000300U) != 0U)) {
            failCode |= 2U;
        } else {
            failCode |= (check_frequency(dcc1CNT1_CLKSRC_PLL2) << 1U);
        }
        if (failCode == 0U) {
            break;
        }
    }
    /* To avoid MISRA violation 382S
       (void)missing for discarded return value */
    failCode = disable_plls(SYS_CLKSRC_PLL1 | SYS_CLKSRC_PLL2);
    /* restore CLKCNTL, VCLKR and PENA first */
    TMS570_SYS1.CLKCNTL = (clkCntlSav & 0x000F0100U);
    /* restore CLKCNTL, VCLK2R */
    TMS570_SYS1.CLKCNTL = clkCntlSav;
    return failCode;
}

/** @fn uint32_t _errata_SSWF021_45_pll1(uint32_t count)
 *   @brief This handles the errata for PLL1. This function is called in device
 *       startup
 *
 *   @param[in] count : Number of retries until both PLL1 is locked successfully
 *                      Minimum value recommended is 5
 *
 *   @return 0 = Success (the PLL or both PLLs have successfully locked and then
 *               been disabled)
 *           1 = PLL1 failed to successfully lock in "count" tries
 *           2 = PLL2 failed to successfully lock in "count" tries
 *           3 = Neither PLL1 nor PLL2 successfully locked in "count" tries
 *           4 = The workaround function was not able to disable at least one of
 *               the PLLs. The most likely reason is that a PLL is already being
 *               used as a clock source. This can be caused by the workaround
 *               function being called from the wrong place in the code.
 */
uint32_t _errata_SSWF021_45_pll1(uint32_t count) {
    uint32_t failCode, retries, clkCntlSav;

    /* save CLKCNTL */
    clkCntlSav = TMS570_SYS1.CLKCNTL;
    /* First set VCLK2 = HCLK */
    TMS570_SYS1.CLKCNTL = clkCntlSav & 0x000F0100U;
    /* Now set VCLK = HCLK and enable peripherals */
    TMS570_SYS1.CLKCNTL = SYS_CLKCNTRL_PENA;
    failCode = 0U;
    for (retries = 0U; (retries < count); retries++) {
        failCode = 0U;
        /* Disable PLL1 */
        failCode = disable_plls(SYS_CLKSRC_PLL1);
        if (failCode != 0U) {
            break;
        }

        /* Clear Global Status Register */
        TMS570_SYS1.GLBSTAT = 0x00000301U;
        /* Clear the ESM PLL slip flags */
        TMS570_ESM.SR[0U] = ESM_SR1_PLL1SLIP;
        /* set  PLL1 to OSCIN/1*27/(2*1) */
        TMS570_SYS1.PLLCTL1 = 0x20001A00U;
        TMS570_SYS1.PLLCTL2 = 0x3FC0723DU;
        TMS570_SYS1.CSDISCLR = SYS_CLKSRC_PLL1;
        /* Check for PLL1 valid or PLL1 slip*/
        while (((TMS570_SYS1.CSVSTAT & SYS_CLKSRC_PLL1) == 0U) &&
               ((TMS570_ESM.SR[0U] & ESM_SR1_PLL1SLIP) == 0U)) {
            /* Wait */
        }
        /* If PLL1 valid, check the frequency */
        if (((TMS570_ESM.SR[0U] & ESM_SR1_PLL1SLIP) != 0U) ||
            ((TMS570_SYS1.GLBSTAT & 0x00000300U) != 0U)) {
            failCode |= 1U;
        } else {
            failCode |= check_frequency(dcc1CNT1_CLKSRC_PLL1);
        }
        if (failCode == 0U) {
            break;
        }
    }
    /* To avoid MISRA violation 382S
       (void)missing for discarded return value */
    failCode = disable_plls(SYS_CLKSRC_PLL1);
    /* restore CLKCNTL, VCLKR and PENA first */
    TMS570_SYS1.CLKCNTL = (clkCntlSav & 0x000F0100U);
    /* restore CLKCNTL, VCLK2R */
    TMS570_SYS1.CLKCNTL = clkCntlSav;
    return failCode;
}

/** @fn uint32_t _errata_SSWF021_45_pll2(uint32_t count)
 *   @brief This handles the errata for PLL2. This function is called in device
 *           startup
 *
 *   @param[in] count : Number of retries until PLL2 is locked successfully
 *                      Minimum value recommended is 5
 *
 *   @return 0 = Success (the PLL or both PLLs have successfully locked and then
 *       been disabled)
 *           1 = PLL1 failed to successfully lock in "count" tries
 *           2 = PLL2 failed to successfully lock in "count" tries
 *           3 = Neither PLL1 nor PLL2 successfully locked in "count" tries
 *           4 = The workaround function was not able to disable at least one of
 *               the PLLs. The most likely reason is that a PLL is already being
 *               used as a clock source. This can be caused by the workaround
 *               function being called from the wrong place in the code.
 */
uint32_t _errata_SSWF021_45_pll2(uint32_t count) {
    uint32_t failCode, retries, clkCntlSav;

    /* save CLKCNTL */
    clkCntlSav = TMS570_SYS1.CLKCNTL;
    /* First set VCLK2 = HCLK */
    TMS570_SYS1.CLKCNTL = clkCntlSav & 0x000F0100U;
    /* Now set VCLK = HCLK and enable peripherals */
    TMS570_SYS1.CLKCNTL = SYS_CLKCNTRL_PENA;
    failCode = 0U;
    for (retries = 0U; (retries < count); retries++) {
        failCode = 0U;
        /* Disable PLL2 */
        failCode = disable_plls(SYS_CLKSRC_PLL2);
        if (failCode != 0U) {
            break;
        }

        /* Clear Global Status Register */
        TMS570_SYS1.GLBSTAT = 0x00000301U;
        /* Clear the ESM PLL slip flags */
        TMS570_ESM.SR4 = ESM_SR4_PLL2SLIP;
        /* set PLL2 to OSCIN/1*27/(2*1) */
        TMS570_SYS2.PLLCTL3 = 0x20001A00U;
        TMS570_SYS1.CSDISCLR = SYS_CLKSRC_PLL2;
        /* Check for PLL2 valid or PLL2 slip */
        while (((TMS570_SYS1.CSVSTAT & SYS_CLKSRC_PLL2) == 0U) &&
               ((TMS570_ESM.SR4 & ESM_SR4_PLL2SLIP) == 0U)) {
            /* Wait */
        }
        /* If PLL2 valid, check the frequency */
        if (((TMS570_ESM.SR4 & ESM_SR4_PLL2SLIP) != 0U) ||
            ((TMS570_SYS1.GLBSTAT & 0x00000300U) != 0U)) {
            failCode |= 2U;
        } else {
            failCode |= (check_frequency(dcc1CNT1_CLKSRC_PLL2) << 1U);
        }
        if (failCode == 0U) {
            break;
        }
    }
    /* To avoid MISRA violation 382S
       (void)missing for discarded return value */
    failCode = disable_plls(SYS_CLKSRC_PLL2);
    /* restore CLKCNTL, VCLKR and PENA first */
    TMS570_SYS1.CLKCNTL = (clkCntlSav & 0x000F0100U);
    /* restore CLKCNTL, VCLK2R */
    TMS570_SYS1.CLKCNTL = clkCntlSav;
    return failCode;
}

/** @fn uint32_t check_frequency(uint32_t cnt1_clksrc)
 *   @brief This function checks for the PLL frequency.
 *
 *   @param[in] cnt1_clksrc : Clock source for Counter1
 *                            0U - PLL1      (clock source 0)
 *                            1U - PLL2      (clock source 1)
 *
 *   @return   DCC Error status
 *             0 - DCC error has not occurred
 *             1 - DCC error has occurred
 */
static uint32_t check_frequency(uint32_t cnt1_clksrc) {
    /* Setup DCC1 */
    /** DCC1 Global Control register configuration */
    TMS570_DCC1.GCTRL =
        (uint32_t)0x5U |                   /** Disable  DCC1 */
        (uint32_t)((uint32_t)0x5U << 4U) | /** No Error Interrupt */
        (uint32_t)((uint32_t)0xAU << 8U) | /** Single Shot mode */
        (uint32_t)((uint32_t)0x5U << 12U); /** No Done Interrupt */
    /* Clear ERR and DONE bits */
    TMS570_DCC1.STAT = 3U;
    /** DCC1 Clock0 Counter Seed value configuration */
    TMS570_DCC1.CNT0SEED = 68U;
    /** DCC1 Clock0 Valid Counter Seed value configuration */
    TMS570_DCC1.VALID0SEED = 4U;
    /** DCC1 Clock1 Counter Seed value configuration */
    TMS570_DCC1.CNT1SEED = 972U;
    /** DCC1 Clock1 Source 1 Select */
    TMS570_DCC1.CNT1CLKSRC =
        (uint32_t)((uint32_t)10U << 12U) | /** DCC Enable / Disable Key */
        (uint32_t)cnt1_clksrc;             /** DCC1 Clock Source 1 */

    TMS570_DCC1.CNT0CLKSRC =
        (uint32_t)DCC1_CNT0_OSCIN; /** DCC1 Clock Source 0 */

    /** DCC1 Global Control register configuration */
    TMS570_DCC1.GCTRL =
        (uint32_t)0xAU |                   /** Enable  DCC1 */
        (uint32_t)((uint32_t)0x5U << 4U) | /** No Error Interrupt */
        (uint32_t)((uint32_t)0xAU << 8U) | /** Single Shot mode */
        (uint32_t)((uint32_t)0x5U << 12U); /** No Done Interrupt */
    while (TMS570_DCC1.STAT == 0U) {
        /* Wait */
    }
    return (TMS570_DCC1.STAT & 0x01U);
}

/** @fn uint32_t disable_plls(uint32_t plls)
 *   @brief This function disables plls and clears the respective ESM flags.
 *
 *   @param[in] plls : Clock source for Counter1
 *                            2U - PLL1
 *                            40U - PLL2
 *
 *   @return   failCode
 *      0 = Success (the PLL or both PLLs have successfully locked and
 *          then been disabled)
 *      4 = The workaround function was not able to disable at least one
 *          of the PLLs. The most likely reason is that a PLL is already being
 *          used as a clock source. This can be caused by the workaround
 *          function being called from the wrong place in the code.
 */
static uint32_t disable_plls(uint32_t plls) {
    uint32_t timeout, failCode;

    TMS570_SYS1.CSDISSET = plls;
    failCode = 0U;
    timeout = 0x10U;
    timeout--;
    while (((TMS570_SYS1.CSVSTAT & (plls)) != 0U) && (timeout != 0U)) {
        /* Clear ESM and GLBSTAT PLL slip flags */
        TMS570_SYS1.GLBSTAT = 0x00000300U;

        if ((plls & SYS_CLKSRC_PLL1) == SYS_CLKSRC_PLL1) {
            TMS570_ESM.SR[0U] = ESM_SR1_PLL1SLIP;
        }
        if ((plls & SYS_CLKSRC_PLL2) == SYS_CLKSRC_PLL2) {
            TMS570_ESM.SR4 = ESM_SR4_PLL2SLIP;
        }
        timeout--;
        /* Wait */
    }
    if (timeout == 0U) {
        failCode = 4U;
    } else {
        failCode = 0U;
    }
    return failCode;
}
