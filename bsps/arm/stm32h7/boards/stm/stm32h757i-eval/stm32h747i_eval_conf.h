/* SPDX-License-Identifier: BSD-3-Clause */
/** 
  ******************************************************************************
  * @file    stm32h747i_eval_config.h
  * @author  MCD Application Team
  * @brief   STM32H747I_EVAL board configuration file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/*
 * RTEMS committer clarification comment on license above:
 *
 * This file comes from STM32CubeH7 project and is located here:
 * https://github.com/STMicroelectronics/STM32CubeH7/blob/master/Drivers/BSP/STM32H747I-EVAL/stm32h747i_eval_conf_template.h
 *
 * The file root directory is:
 * https://github.com/STMicroelectronics/STM32CubeH7/tree/master/Drivers/BSP/STM32H747I-EVAL
 *
 * This directory contains LICENSE.md file with a following license text:
 *
 * Copyright 2019 STMicroelectronics.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32H747I_EVAL_CONFIG_H
#define STM32H747I_EVAL_CONFIG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

   /* COM define */
#define USE_COM_LOG                         0U
   
   /* IO class usage define */  
#define USE_BSP_IO_CLASS                    1U
   
   /* JOY usage define */
#define USE_BSP_JOY_FEATURE                 1U
   
   /* POT usage define */   
#define USE_BSP_POT_FEATURE                 1U
   
   /* LCD controllers defines */
#define USE_LCD_CTRL_OTM8009A               1U
#define USE_LCD_CTRL_ADV7533                1U
#define LCD_LAYER_0_ADDRESS                 0xD0000000U
#define LCD_LAYER_1_ADDRESS                 0xD0200000U
   
   /* SD high performance usage define */
#define USE_SD_HIGH_PERFORMANCE             0U
   
   /*DMA2D to fill RGB rectangle usage define*/
#define USE_DMA2D_TO_FILL_RGB_RECT          0U
   
   /* Audio codecs defines */
#define USE_AUDIO_CODEC_WM8994              1U
#define USE_AUDIO_CODEC_ADV7533             1U
   
   /* Default Audio IN internal buffer size */   
#define DEFAULT_AUDIO_IN_BUFFER_SIZE        256U
   
/* TS supported features defines */
#define USE_TS_GESTURE                      1U
#define USE_TS_MULTI_TOUCH                  1U

/* Default TS touch number */   
#define TS_TOUCH_NBR                        2U

/* Default EEPROM max trials */
#define EEPROM_MAX_TRIALS                   3000U

/* IRQ priorities */
#define BSP_SRAM_IT_PRIORITY                15U
#define BSP_SDRAM_IT_PRIORITY               15U
#define BSP_IOEXPANDER_IT_PRIORITY          15U
#define BSP_BUTTON_USER_IT_PRIORITY         15U
#define BSP_BUTTON_WAKEUP_IT_PRIORITY       15U
#define BSP_BUTTON_TAMPER_IT_PRIORITY       15U
#define BSP_AUDIO_OUT_IT_PRIORITY           14U  
#define BSP_AUDIO_IN_IT_PRIORITY            15U  
#define BSP_SD_IT_PRIORITY                  14U 
#define BSP_SD_RX_IT_PRIORITY               14U    
#define BSP_SD_TX_IT_PRIORITY               15U 
#define BSP_TS_IT_PRIORITY                  15U
   
#ifdef __cplusplus
}
#endif
  
#endif /* STM32H747I_EVAL_CONFIG_H */

