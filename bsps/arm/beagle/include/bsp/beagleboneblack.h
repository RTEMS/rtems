/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup arm_beagle
 *
 * @brief BeagleBone Black BSP definitions.
 */

/**
 * Copyright (c) 2015 Ketul Shah <ketulshah1993 at gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LIBBSP_ARM_BEAGLE_BEAGLEBONEBLACK_H
#define LIBBSP_ARM_BEAGLE_BEAGLEBONEBLACK_H

/* In general GPIOs of BeagleBone Black/White can be addressed
 * using two 46-pin dual-row expansion connectors P9 and P8,
 * which are also known as Expansion A and Expansion B Connectors,
 * respectively.
 *
 * Each Expansion Connector consists of 23 pins. So 2x23 pins would
 * be available. It has 4 GPIO Banks each consists of 32 pins each.
 * Toatal number of pins are 128 (32x4).
 *
 * So for mapping between generalized pin name and the unique pin
 * numbers in this header file Macros are declared.
 */

/* USER LEDs of BeagleBone Black */
#define BBB_LED_USR0  53 /* USR LED0 */
#define BBB_LED_USR1  54 /* USR LED1 */
#define BBB_LED_USR2  55 /* USR LED2 */
#define BBB_LED_USR3  56 /* USR LED3 */

/* Header P8 of BeagleBone Black */
#define BBB_P8_7      66 /* GPIO2_2 */
#define BBB_P8_8      67 /* GPIO2_3 */
#define BBB_P8_9      69 /* GPIO2_5 */
#define BBB_P8_10     68 /* GPIO2_4 */
#define BBB_P8_11     45 /* GPIO1_13 */
#define BBB_P8_12     44 /* GPIO1_12 */
#define BBB_P8_13     23 /* GPIO0_23 */
#define BBB_P8_14     26 /* GPIO0_26 */
#define BBB_P8_15     47 /* GPIO1_15 */
#define BBB_P8_16     46 /* GPIO1_14 */
#define BBB_P8_17     27 /* GPIO0_27 */
#define BBB_P8_18     65 /* GPIO2_1 */
#define BBB_P8_19     22 /* GPIO0_22 */
#define BBB_P8_26     61 /* GPIO1_29 */
	
/* Header P9 of BeagleBone Black */
#define BBB_P9_11     30 /* GPIO0_30 */
#define BBB_P9_12     60 /* GPIO1_28 */
#define BBB_P9_13     31 /* GPIO0_31 */
#define BBB_P9_14     50 /* GPIO1_18 */
#define BBB_P9_15     48 /* GPIO1_16 */
#define BBB_P9_16     51 /* GPIO1_19 */
#define BBB_P9_17      5 /* GPIO0_5 */
#define BBB_P9_18      4 /* GPIO0_4 */
#define BBB_P9_23     49 /* GPIO1_17 */
#define BBB_P9_24     15 /* GPIO0_15 */
#define BBB_P9_26     14 /* GPIO1_14 */
#define BBB_P9_27     115/* GPIO3_19 */
#define BBB_P9_30     112/* GPIO3_16 */
#define BBB_P9_41     20 /* GPIO0_20 */
#define BBB_P9_42      7 /* GPIO0_7 */

#endif /* LIBBSP_ARM_BEAGLE_GPIO_H */
